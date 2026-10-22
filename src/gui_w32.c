/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Windows GUI.
 *
 * GUI support for Microsoft Windows, aka Win32.  Also for Win64.
 *
 * George V. Reilly <george@reilly.org> wrote the original Win32 GUI.
 * Robert Webb reworked it to use the existing GUI stuff and added menu,
 * scrollbars, etc.
 *
 * Note: Clipboard stuff, for cutting and pasting text to other windows, is in
 * winclip.c.	(It can also be done from the terminal version).
 *
 * TODO: Some of the function signatures ought to be updated for Win64;
 * e.g., replace LONG with LONG_PTR, etc.
 */

#ضمن "vim.h"

#إذا defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#د_که_پای

// values for "dead_key"
#تعریف_کړه DEAD_کلی_OFF			0	// no dead key
#تعریف_کړه DEAD_کلی_سېټ_DEFAULT		1	// dead key pressed
#تعریف_کړه DEAD_کلی_TRANSIENT_دننه_ON_CHAR	2	// wait for next key press
#تعریف_کړه DEAD_کلی_SKIP_ON_CHAR		3	// skip next _OnChar()

#إذا defined(FEAT_DIRECTX)
ساكن DWriteContext *s_dwc = عدم;
ساكن صحيح s_directx_enabled = 0;
ساكن صحيح s_directx_بارول_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != عدم && enc_utf8)
ساكن صحيح directx_enabled(فراغ);
ساكن فراغ directx_binddc(فراغ);
#د_که_پای

#که_تعریف_شوی FEAT_MENU
ساكن صحيح gui_mswin_get_menu_لوړوالی(صحيح fix_کړکۍ);
#وإلا
# define gui_mswin_get_menu_لوړوالی(fix_کړکۍ)	0
#د_که_پای

تعريفنوع بنية keycode_trans_strategy {
    فراغ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
    فراغ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
    فراغ (*ptr_process_پیغام_usual_کلی) (UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
    صحيح  (*ptr_get_فعال_modifiers)(فراغ);
    صحيح  (*is_experimental)(فراغ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ساكن فراغ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ process_پیغام_usual_کلی_experimental(UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
ساكن صحيح  get_فعال_modifiers_experimental(فراغ);
ساكن صحيح  is_experimental_true(فراغ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_پیغام_usual_کلی_experimental // ptr_process_message_usual_key
    , get_فعال_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ساكن فراغ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ process_پیغام_usual_کلی_classic(UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
ساكن صحيح  get_فعال_modifiers_classic(فراغ);
ساكن صحيح  is_experimental_false(فراغ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_پیغام_usual_کلی_classic // ptr_process_message_usual_key
    , get_فعال_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_کارول_شوی = عدم;

ساكن صحيح is_experimental_true(فراغ)
{
    أرجع 1;
}

ساكن صحيح is_experimental_false(فراغ)
{
    أرجع 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ساكن فراغ keycode_trans_strategy_init(فراغ)
{
    ثابت حرف *strategy = عدم;

    // set default value as fallback
    keycode_trans_strategy_کارول_شوی = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    إذا (strategy == عدم)
    {
	أرجع;
    }

    إذا (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_کارول_شوی = &keycode_trans_strategy_classic;
	أرجع;
    }

    إذا (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_کارول_شوی = &keycode_trans_strategy_experimental;
	أرجع;
    }

}

#إذا defined(FEAT_RENDER_OPTIONS)
    صحيح
gui_mch_سېټ_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    صحيح	    dx_enable = 0;
    صحيح	    dx_flags = 0;
    عائم   dx_gamma = 0.0f;
    عائم   dx_contrast = 0.0f;
    عائم   dx_level = 0.0f;
    صحيح	    dx_geom = 0;
    صحيح	    dx_renmode = 0;
    صحيح	    dx_taamode = 0;
    // parse string as rendering options.
    لكل (p = s; p != عدم && *p != NUL; )
    {
	char_u  توکی[256];
	char_u  name[128];
	char_u  ارزښت[128];

	copy_option_part(&p, توکی, حجم(توکی), ",");
	إذا (p == عدم)
	    اكسر;
	q = &توکی[0];
	copy_option_part(&q, name, حجم(name), ":");
	إذا (q == عدم)
	    أرجع FAIL;
	copy_option_part(&q, ارزښت, حجم(ارزښت), ":");

	إذا (STRCMP(name, "type") == 0)
	{
	    إذا (STRCMP(ارزښت, "directx") == 0)
		dx_enable = 1;
	    وإلا
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (عائم)atof((حرف *)ارزښت);
	}
	وإلا إذا (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (عائم)atof((حرف *)ارزښت);
	}
	وإلا إذا (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (عائم)atof((حرف *)ارزښت);
	}
	وإلا إذا (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((حرف *)ارزښت);
	    إذا (dx_geom < 0 || dx_geom > 2)
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((حرف *)ارزښت);
	    إذا (dx_renmode < 0 || dx_renmode > 6)
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((حرف *)ارزښت);
	    إذا (dx_taamode < 0 || dx_taamode > 3)
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	وإلا
	    أرجع FAIL;
    }

    إذا (!gui.دننه_use)
	أرجع OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    إذا (dx_enable)
    {
	إذا (!directx_enabled())
	    أرجع FAIL;
	DWrite_Context_سېټ_Rendering_Params(s_dwc, عدم);
	إذا (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    إذا (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    إذا (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    إذا (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    إذا (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    إذا (dx_flags & (1 << 4))
		param.rendering_ډېرپېښ = dx_renmode;
	    إذا (dx_flags & (1 << 5))
		param.متن_Antialias_ډېرپېښ = dx_taamode;
	    DWrite_Context_سېټ_Rendering_Params(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    أرجع OK;
# وإلا
    أرجع FAIL;
# endif
}
#د_که_پای

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#که_نه_تعریف_شوی HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (صحيح)(لنډ)LOWORD(lParam), (صحيح)(لنډ)HIWORD(lParam), (UINT)(wParam)), 0L)
#د_که_پای
#که_نه_تعریف_شوی HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (صحيح)(لنډ)LOWORD(lParam), (صحيح)(لنډ)HIWORD(lParam), (UINT)(wParam)), 0L)
#د_که_پای
#که_نه_تعریف_شوی HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (صحيح)(لنډ)LOWORD(lParam), (صحيح)(لنډ)HIWORD(lParam), (UINT)(wParam)), 0L)
#د_که_پای


#ضمن "version.h"	// used by dialog box routine for default title
#که_تعریف_شوی DEBUG
# include <tchar.h>
#د_که_پای

#ضمن <shellapi.h>
#ضمن <commctrl.h>
#ضمن <windowsx.h>

#که_تعریف_شوی FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#د_که_پای

// Some parameters for dialog boxes.  All in pixels.
#تعریف_کړه DLG_PADDING_X		10
#تعریف_کړه DLG_PADDING_Y		10
#تعریف_کړه DLG_VERT_PADDING_X	4	// For vertical buttons
#تعریف_کړه DLG_VERT_PADDING_Y	4
#تعریف_کړه DLG_ICON_پلنوالی		34
#تعریف_کړه DLG_ICON_لوړوالی		34
#تعریف_کړه DLG_MIN_پلنوالی		150
#تعریف_کړه DLG_FONT_NAME		"MS Shell Dlg"
#تعریف_کړه DLG_FONT_ټکی_اندازه	8
#تعریف_کړه DLG_MIN_MAX_پلنوالی	400
#تعریف_کړه DLG_MIN_MAX_لوړوالی	400

#تعریف_کړه DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#که_نه_تعریف_شوی WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#د_که_پای

#که_نه_تعریف_شوی WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#د_که_پای

#که_نه_تعریف_شوی WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#د_که_پای

#که_نه_تعریف_شوی SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#د_که_پای

#که_نه_تعریف_شوی SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#د_که_پای

#که_نه_تعریف_شوی DWMWA_USE_IMMERSIVE_DARK_ډېرپېښ
# define DWMWA_USE_IMMERSIVE_DARK_ډېرپېښ	20
#د_که_پای

#که_نه_تعریف_شوی DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#د_که_پای

#که_نه_تعریف_شوی DWMWA_متن_COLOR
# define DWMWA_متن_COLOR		36
#د_که_پای

ساكن فراغ _OnPaint(HWND hwnd);
ساكن فراغ fill_rect(ثابت RECT *rcp, HBRUSH hbr, COLORREF color);
ساكن فراغ clear_rect(RECT *rcp);

ساكن کلمه_2		s_dlgfntheight;		// height of the dialog font
ساكن کلمه_2		s_dlgfntwidth;		// width of the dialog font

#که_تعریف_شوی FEAT_MENU
ساكن HMENU		s_menuBar = عدم;
#د_که_پای
#که_تعریف_شوی FEAT_TEAROFF
ساكن فراغ rebuild_tearoff(vimmenu_T *menu);
ساكن HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#د_که_پای

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ساكن صحيح		s_busy_processing = FALSE;

ساكن صحيح		destroying = FALSE;	// call DestroyWindow() ourselves

#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
ساكن UINT		s_findrep_پیغام = 0;
ساكن FINDREPLACEW	s_findrep_struct;
ساكن HWND		s_findrep_hwnd = عدم;
ساكن صحيح		s_findrep_is_پیدا_کړه;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#د_که_پای

HWND			s_hwnd = عدم;
ساكن HDC		s_hdc = عدم;
ساكن HBRUSH		s_brush = عدم;

#که_تعریف_شوی FEAT_TOOLBAR
ساكن HWND		s_toolbarhwnd = عدم;
ساكن WNDPROC		s_toolbar_wndproc = عدم;
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
ساكن HWND		s_tabhwnd = عدم;
ساكن WNDPROC		s_tabline_wndproc = عدم;
ساكن صحيح		showing_tabline = 0;
#د_که_پای

ساكن WPARAM		s_wParam = 0;

ساكن HWND		s_متن_Area = عدم;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ساكن bool		s_textarea_bg_only = false;

ساكن UINT		s_u_پیغام = 0;

ساكن char_u		*s_textfield; // Used by dialogs to pass back strings

ساكن صحيح		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ساكن صحيح allow_scrollbar = FALSE;

#که_نه_تعریف_شوی _DPI_AWARENESS_CONTEXTS_
تعريفنوع HANDLE DPI_AWARENESS_CONTEXT;

تعريفنوع تعداد DPI_AWARENESS {
    DPI_AWARENESS_INVALID	    = -1,
    DPI_AWARENESS_UNAWARE	    = 0,
    DPI_AWARENESS_SYSTEM_AWARE	    = 1,
    DPI_AWARENESS_فی_MONITOR_AWARE = 2
} DPI_AWARENESS;

# define DPI_AWARENESS_CONTEXT_UNAWARE		    ((DPI_AWARENESS_CONTEXT)-1)
# define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE	    ((DPI_AWARENESS_CONTEXT)-2)
# define DPI_AWARENESS_CONTEXT_فی_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
# define DPI_AWARENESS_CONTEXT_فی_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
# define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
#د_که_پای

#تعریف_کړه DEFAULT_DPI	96
ساكن صحيح		s_dpi = DEFAULT_DPI;
ساكن BOOL		s_دننه_dpichanged = FALSE;
#که_تعریف_شوی FEAT_MBYTE_IME
ساكن DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#د_که_پای
ساكن RECT		s_suggested_rect;

ساكن UINT (WINAPI *pGetDpiForSystem)(فراغ) = عدم;
ساكن UINT (WINAPI *p_Get_Dpi_For_کړکۍ)(HWND hwnd) = عدم;
ساكن صحيح (WINAPI *pGetSystemMetricsForDpi)(صحيح, UINT) = عدم;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ساكن DPI_AWARENESS_CONTEXT (WINAPI *p_سېټ_Thread_Dpi_Awareness_Context)(DPI_AWARENESS_CONTEXT dpiContext) = عدم;
#که_تعریف_شوی FEAT_MBYTE_IME
ساكن DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = عدم;
#د_که_پای

ساكن HINSTANCE hLibDwm = عدم;
ساكن HRESULT (WINAPI *p_Dwm_سېټ_کړکۍ_Attribute)(HWND, DWORD, LPCVOID, DWORD);
ساكن فراغ dyn_dwm_بارول(فراغ);

ساكن صحيح fullscreen_on = FALSE;

#که_تعریف_شوی FEAT_GUI_DARKTHEME

ساكن HINSTANCE hUxThemeLib = عدم;
ساكن DWORD (WINAPI *p_سېټ_Preferred_App_ډېرپېښ)(DWORD) = عدم;
ساكن فراغ (WINAPI *pFlushMenuThemes)(فراغ) = عدم;
ساكن فراغ dyn_uxtheme_بارول(فراغ);
#د_که_پای

    ساكن صحيح WINAPI
stubGetSystemMetricsForDpi(صحيح n_شاخص, UINT dpi ناکارول_شوی)
{
    أرجع GetSystemMetrics(n_شاخص);
}

    ساكن صحيح
adjust_fontsize_by_dpi(صحيح اندازه)
{
    أرجع اندازه * s_dpi / (صحيح)pGetDpiForSystem();
}

    ساكن صحيح
adjust_by_system_dpi(صحيح اندازه)
{
    أرجع اندازه * (صحيح)pGetDpiForSystem() / DEFAULT_DPI;
}

#إذا defined(FEAT_DIRECTX)
    ساكن صحيح
directx_enabled(فراغ)
{
    إذا (s_dwc != عدم)
	أرجع 1;
    وإلا إذا (s_directx_بارول_attempted)
	أرجع 0;
    // load DirectX
    DWrite_Init();
    s_directx_بارول_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    أرجع s_dwc != عدم ? 1 : 0;
}

    ساكن فراغ
directx_binddc(فراغ)
{
    إذا (s_متن_Area == عدم)
	أرجع;

    RECT	rect;
    GetClientRect(s_متن_Area, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#د_که_پای

خارجي صحيح current_font_لوړوالی;	    // this is in os_mswin.c

ساكن بنية
{
    UINT    کلی_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیګانې[] =
{
    {VK_پورته,		'k', 'u'},
    {VK_ښکته,		'k', 'd'},
    {VK_کیڼ,		'k', 'l'},
    {VK_ښی,		'k', 'r'},

    {VK_F1,		'k', '1'},
    {VK_F2,		'k', '2'},
    {VK_F3,		'k', '3'},
    {VK_F4,		'k', '4'},
    {VK_F5,		'k', '5'},
    {VK_F6,		'k', '6'},
    {VK_F7,		'k', '7'},
    {VK_F8,		'k', '8'},
    {VK_F9,		'k', '9'},
    {VK_F10,		'k', ';'},

    {VK_F11,		'F', '1'},
    {VK_F12,		'F', '2'},
    {VK_F13,		'F', '3'},
    {VK_F14,		'F', '4'},
    {VK_F15,		'F', '5'},
    {VK_F16,		'F', '6'},
    {VK_F17,		'F', '7'},
    {VK_F18,		'F', '8'},
    {VK_F19,		'F', '9'},
    {VK_F20,		'F', 'A'},

    {VK_F21,		'F', 'B'},
#که_تعریف_شوی FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#د_که_پای
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_شا,		'k', 'b'},
    {VK_ورننباسه,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_پای_ټکی,		'@', '7'},
    {VK_مخکینی,		'k', 'P'},
    {VK_بل,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_جمع_کړه,		'K', '6'},
    {VK_کمول,	'K', '7'},
    {VK_DIVIDE,		'K', '8'},
    {VK_MULTIPLY,	'K', '9'},
    {VK_SEPARATOR,	'K', 'A'},	// Keypad Enter
    {VK_DECIMAL,	'K', 'B'},

    {VK_NUMPAD0,	'K', 'C'},
    {VK_NUMPAD1,	'K', 'D'},
    {VK_NUMPAD2,	'K', 'E'},
    {VK_NUMPAD3,	'K', 'F'},
    {VK_NUMPAD4,	'K', 'G'},
    {VK_NUMPAD5,	'K', 'H'},
    {VK_NUMPAD6,	'K', 'I'},
    {VK_NUMPAD7,	'K', 'J'},
    {VK_NUMPAD8,	'K', 'K'},
    {VK_NUMPAD9,	'K', 'L'},

    // Keys that we want to be able to use any modifier with:
    {VK_SPACE,		' ', NUL},
    {VK_TAB,		TAB, NUL},
    {VK_ESCAPE,		ESC, NUL},
    {NL,		NL, NUL},
    {CAR,		CAR, NUL},

    // End of list marker:
    {0,			0, 0}
};

// Local variables
ساكن صحيح	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ساكن صحيح	s_getting_focus = FALSE;

ساكن صحيح	s_x_pending;
ساكن صحيح	s_y_pending;
ساكن UINT	s_kFlags_pending;
ساكن UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ساكن صحيح	s_timed_بهر = FALSE;
ساكن صحيح	dead_کلی = DEAD_کلی_OFF;
ساكن UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#که_تعریف_شوی FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ساكن فراغ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ساكن فراغ track_user_activity(UINT u_پیغام);
#د_که_پای

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#که_تعریف_شوی FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ساكن LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ساكن LOGFONTW sub_logfont;
#د_که_پای

#که_تعریف_شوی FEAT_MBYTE_IME
ساكن LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_معلومات);
#د_که_پای

#إذا defined(FEAT_BROWSE)
ساكن char_u *convert_filter(char_u *s);
#د_که_پای

#که_تعریف_شوی DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ساكن فراغ
print_windows_error(فراغ)
{
    LPVOID  lp_پیغام_Buf;

    Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
		  عدم, Get_وروستی_Error(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_پیغام_Buf, 0, عدم);
    TRACE1("Error: %s\n", lp_پیغام_Buf);
    LocalFree(lp_پیغام_Buf);
}
#د_که_پای

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کړه BLINK_NONE  0
#تعریف_کړه BLINK_OFF   1
#تعریف_کړه BLINK_ON    2

ساكن صحيح		blink_state = BLINK_NONE;
ساكن long_u		blink_waittime = 700;
ساكن long_u		blink_ontime = 400;
ساكن long_u		blink_offtime = 250;
ساكن UINT_PTR		blink_timer = 0;

    صحيح
gui_mch_is_blinking(فراغ)
{
    أرجع blink_state != BLINK_NONE;
}

    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع blink_state == BLINK_OFF;
}

    فراغ
gui_mch_سېټ_blinking(اوږد wait, اوږد on, اوږد off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ساكن VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_پیغام ناکارول_شوی,
    UINT_PTR idEvent,
    DWORD dw_وخت ناکارول_شوی)
{
    پیغام_3 پیغام;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(عدم, idEvent);

    // Eat spurious WM_TIMER messages
    طالما (Peek_پیغام_W(&پیغام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = سېټ_Timer(عدم, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = سېټ_Timer(عدم, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ساكن فراغ
gui_mswin_rm_blink_timer(فراغ)
{
    پیغام_3 پیغام;

    إذا (blink_timer == 0)
	أرجع;

    KillTimer(عدم, blink_timer);
    // Eat spurious WM_TIMER messages
    طالما (Peek_پیغام_W(&پیغام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    إذا (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
    blink_state = BLINK_NONE;
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_پیل_blink(فراغ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.دننه_focus)
    {
	blink_timer = سېټ_Timer(عدم, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ساكن VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_پیغام ناکارول_شوی,
    UINT_PTR idEvent,
    DWORD dw_وخت ناکارول_شوی)
{
    پیغام_3 پیغام;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(عدم, idEvent);
    s_timed_بهر = TRUE;

    // Eat spurious WM_TIMER messages
    طالما (Peek_پیغام_W(&پیغام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    إذا (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ساكن فراغ
_OnDeadChar(
    HWND hwnd ناکارول_شوی,
    UINT ch ناکارول_شوی,
    صحيح cRepeat ناکارول_شوی)
{
    dead_کلی = DEAD_کلی_سېټ_DEFAULT;
}

/*
 * Convert Unicode character "ch" to bytes in "string[slen]".
 * When "had_alt" is TRUE the ALT key was included in "ch".
 * Return the length.
 * Because the Windows API uses UTF-16, we have to deal with surrogate
 * pairs; this is where we choose to deal with them: if "ch" is a high
 * surrogate, it will be stored, and the length returned will be zero; the next
 * char_to_string call will then include the high surrogate, decoding the pair
 * of UTF-16 code units to a single Unicode code point, presuming it is the
 * matching low surrogate.
 */
    ساكن صحيح
char_منزل_string(صحيح ch, char_u *string, صحيح slen, صحيح had_alt)
{
    صحيح		len;
    صحيح		i;
    WCHAR	wstring[2];
    char_u	*ws = عدم;

    إذا (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    وإلا إذا (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	أرجع 0;
    }
    وإلا
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    إذا (enc_codepage > 0)
    {
	len = Wide_Char_منزل_Multi_Byte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, عدم);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	إذا (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = Wide_Char_منزل_Multi_Byte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, عدم);
	    إذا (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    وإلا
    {
	ws = utf16_منزل_enc(wstring, &len);
	إذا (ws == عدم)
	    len = 0;
	وإلا
	{
	    إذا (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    إذا (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    لكل (i = 0; i < len; ++i)
	إذا (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (صحيح)KE_CSI;
	    len += 2;
	}

    أرجع len;
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 *
 * TODO: since introduction, this experimental function started
 * to be used as well outside of original key press/processing
 * area, and usages not via "get_active_modifiers_via_ptr" should
 * be watched.
 */
    ساكن صحيح
get_فعال_modifiers_experimental(فراغ)
{
    صحيح modifiers = 0;

    إذا (Get_کلی_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ماسک_CTRL;
    إذا (Get_کلی_State(VK_ښويدنه) & 0x8000)
	modifiers |= MOD_ماسک_ښويدنه;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    إذا (Get_کلی_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_ماسک_ALT;
    إذا ((modifiers & MOD_ماسک_CTRL) && (Get_کلی_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_ماسک_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    إذا (!(Get_کلی_State(VK_CONTROL) & 0x8000)
	    &&  (Get_کلی_State(VK_RMENU) & 0x8000)
	    && !(Get_کلی_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_ماسک_ALT;

    أرجع modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن صحيح
get_فعال_modifiers_classic(فراغ)
{
    صحيح modifiers = 0;

    إذا (Get_کلی_State(VK_ښويدنه) & 0x8000)
	modifiers |= MOD_ماسک_ښويدنه;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    إذا (Get_کلی_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ماسک_CTRL;
    إذا (Get_کلی_State(VK_MENU) & 0x8000)
	modifiers |= MOD_ماسک_ALT;

    أرجع modifiers;
}

    ساكن صحيح
get_فعال_modifiers(فراغ)
{
    أرجع get_فعال_modifiers_experimental();
}

    ساكن صحيح
get_فعال_modifiers_له_لارې_ptr(فراغ)
{
    // marshal to corresponding implementation
    أرجع keycode_trans_strategy_کارول_شوی->ptr_get_فعال_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ساكن فراغ
_OnChar(
    HWND hwnd ناکارول_شوی,
    UINT cch,
    صحيح cRepeat ناکارول_شوی)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_کارول_شوی->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
_OnChar_experimental(
    HWND hwnd ناکارول_شوی,
    UINT cch,
    صحيح cRepeat ناکارول_شوی)
{
    char_u	string[40];
    صحيح		len = 0;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    إذا (dead_کلی == DEAD_کلی_SKIP_ON_CHAR)
	أرجع;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    إذا (dead_کلی != DEAD_کلی_TRANSIENT_دننه_ON_CHAR)
	dead_کلی = DEAD_کلی_OFF;

    modifiers = get_فعال_modifiers_experimental();

    ch = simplify_کلی(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_کلیګانې;
    ch = may_adjust_کلی_for_ctrl(modifiers, ch);
    --no_reduce_کلیګانې;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ښويدنه_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ایستل_modifiers(ch, &modifiers, FALSE, عدم);
    إذا (ch == CSI)
	ch = K_CSI;

    إذا (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	جمع_کړه_منزل_ننوت_buf(string, 3);
    }

    len = char_منزل_string(ch, string, 40, FALSE);
    إذا (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ننوت_buf();
	got_int = TRUE;
    }

    جمع_کړه_منزل_ننوت_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
_OnChar_classic(
    HWND hwnd ناکارول_شوی,
    UINT ch,
    صحيح cRepeat ناکارول_شوی)
{
    char_u	string[40];
    صحيح		len = 0;

    dead_کلی = 0;

    len = char_منزل_string(ch, string, 40, FALSE);
    إذا (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ننوت_buf();
	got_int = TRUE;
    }

    جمع_کړه_منزل_ننوت_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ساكن فراغ
_OnSysChar(
    HWND hwnd ناکارول_شوی,
    UINT cch,
    صحيح cRepeat ناکارول_شوی)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_کارول_شوی->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
_OnSysChar_experimental(
    HWND hwnd ناکارول_شوی,
    UINT cch,
    صحيح cRepeat ناکارول_شوی)
{
    char_u	string[40]; // Enough for multibyte character
    صحيح		len;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    dead_کلی = DEAD_کلی_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_فعال_modifiers_experimental();
    ch = simplify_کلی(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ښويدنه_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ایستل_modifiers(ch, &modifiers, FALSE, عدم);
    إذا (ch == CSI)
	ch = K_CSI;

    len = 0;
    إذا (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    إذا (IS_SPECIAL((صحيح)ch))
    {
	string[len++] = CSI;
	string[len++] = K_دوهم((صحيح)ch);
	string[len++] = K_دریم((صحيح)ch);
    }
    وإلا
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_منزل_string(ch, string + len, 40 - len, TRUE);
    }

    جمع_کړه_منزل_ننوت_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
_OnSysChar_classic(
    HWND hwnd ناکارول_شوی,
    UINT cch,
    صحيح cRepeat ناکارول_شوی)
{
    char_u	string[40]; // Enough for multibyte character
    صحيح		len;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    dead_کلی = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_ماسک_ALT;
    إذا (Get_کلی_State(VK_ښويدنه) & 0x8000)
	modifiers |= MOD_ماسک_ښويدنه;
    إذا (Get_کلی_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ماسک_CTRL;

    ch = simplify_کلی(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ښويدنه_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ایستل_modifiers(ch, &modifiers, FALSE, عدم);
    إذا (ch == CSI)
	ch = K_CSI;

    len = 0;
    إذا (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    إذا (IS_SPECIAL((صحيح)ch))
    {
	string[len++] = CSI;
	string[len++] = K_دوهم((صحيح)ch);
	string[len++] = K_دریم((صحيح)ch);
    }
    وإلا
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_منزل_string(ch, string + len, 40 - len, TRUE);
    }

    جمع_کړه_منزل_ننوت_buf(string, len);
}

    ساكن فراغ
_OnMouseEvent(
    صحيح button,
    صحيح x,
    صحيح y,
    صحيح repeated_click,
    UINT کلی_Flags)
{
    صحيح vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    إذا (کلی_Flags & MK_ښويدنه)
	vim_modifiers |= MOUSE_ښويدنه;
    إذا (کلی_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    إذا (Get_کلی_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ساكن فراغ
On_Mouse_Button_ښکته(
    HWND hwnd ناکارول_شوی,
    BOOL fDoubleClick ناکارول_شوی,
    صحيح x,
    صحيح y,
    UINT کلی_Flags)
{
    ساكن LONG	s_prev_وخت = 0;

    LONG    current_وخت = Get_پیغام_وخت();
    صحيح	    button = -1;
    صحيح	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (فراغ)سېټ_Focus(s_hwnd);

    إذا (s_u_پیغام == WM_LBUTTONDOWN || s_u_پیغام == WM_LBUTTONDBLCLK)
	button = MOUSE_کیڼ;
    وإلا إذا (s_u_پیغام == WM_MBUTTONDOWN || s_u_پیغام == WM_MBUTTONDBLCLK)
	button = MOUSE_منځ;
    وإلا إذا (s_u_پیغام == WM_RBUTTONDOWN || s_u_پیغام == WM_RBUTTONDBLCLK)
	button = MOUSE_ښی;
    وإلا إذا (s_u_پیغام == WM_XBUTTONDOWN || s_u_پیغام == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    وإلا إذا (s_u_پیغام == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	إذا (s_button_pending == MOUSE_کیڼ)
	    button = MOUSE_ښی;
	وإلا
	    button = MOUSE_کیڼ;
    }

    إذا (button < 0)
	أرجع;

    repeated_click = ((صحيح)(current_وخت - s_prev_وخت) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    إذا (repeated_click
	    && ((button == MOUSE_کیڼ && s_button_pending == MOUSE_ښی)
		|| (button == MOUSE_ښی
		    && s_button_pending == MOUSE_کیڼ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_منځ;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, کلی_Flags);
    }
    وإلا إذا ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ښی)))
    {
	إذا (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, کلی_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, کلی_Flags);
    }
    وإلا
    {
	/*
	 * If this is the first press (i.e. not a multiple click) don't
	 * action immediately, but store and wait for:
	 * i) button-up
	 * ii) mouse move
	 * iii) another button press
	 * before using it.
	 * This enables us to make left+right simulate middle button,
	 * without left or right being actioned first.  The side-effect is
	 * that if you click and hold the mouse without dragging, the
	 * cursor doesn't move until you release the button. In practice
	 * this is hardly a problem.
	 */
	s_button_pending = button;
	s_x_pending = x;
	s_y_pending = y;
	s_kFlags_pending = کلی_Flags;
    }

    s_prev_وخت = current_وخت;
}

    ساكن فراغ
_OnMouseMoveOrRelease(
    HWND hwnd ناکارول_شوی,
    صحيح x,
    صحيح y,
    UINT کلی_Flags)
{
    صحيح button;

    s_getting_focus = FALSE;
    إذا (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    إذا (s_u_پیغام == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	إذا (!(کلی_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    أرجع;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	سېټ_Capture(s_متن_Area);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    وإلا
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, کلی_Flags);
}

    ساكن فراغ
On_اندازه_متن_Area(
    HWND hwnd ناکارول_شوی,
    UINT state ناکارول_شوی,
    صحيح cx ناکارول_شوی,
    صحيح cy ناکارول_شوی)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	directx_binddc();
#د_که_پای
}

#که_تعریف_شوی FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ساكن vimmenu_T *
gui_mswin_پیدا_کړه_menu(
    vimmenu_T	*pMenu,
    صحيح		id)
{
    vimmenu_T	*p_ماشوم_Menu;

    طالما (pMenu)
    {
	إذا (pMenu->id == (UINT)id)
	    اكسر;
	إذا (pMenu->children != عدم)
	{
	    p_ماشوم_Menu = gui_mswin_پیدا_کړه_menu(pMenu->children, id);
	    إذا (p_ماشوم_Menu)
	    {
		pMenu = p_ماشوم_Menu;
		اكسر;
	    }
	}
	pMenu = pMenu->بل;
    }
    أرجع pMenu;
}

    ساكن فراغ
_OnMenu(
    HWND	hwnd ناکارول_شوی,
    صحيح		id,
    HWND	hwndCtl ناکارول_شوی,
    UINT	کوډ_Notify ناکارول_شوی)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_پیدا_کړه_menu(ریښه_menu, id);
    إذا (pMenu)
	gui_menu_cb(pMenu);
}
#د_که_پای

#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ساكن فراغ
On_پیدا_کړه_Repl(فراغ)
{
    صحيح	    flags = 0;
    صحيح	    ښکته;

    إذا (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(فراغ)سېټ_Focus(s_hwnd);

    إذا (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(فراغ)سېټ_Focus(s_hwnd);
    }
    وإلا إذا (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(فراغ)سېټ_Focus(s_hwnd);
    }
    وإلا إذا (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    إذا (flags == 0)
	أرجع;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    إذا (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_ټولیز_کلمه;
    إذا (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_سمون_CASE;
    ښکته = (s_findrep_struct.Flags & FR_ښکته) != 0;
    p = utf16_منزل_enc(s_findrep_struct.lpstr_پیدا_کړه_What, عدم);
    q = utf16_منزل_enc(s_findrep_struct.lpstrReplaceWith, عدم);
    إذا (p != عدم && q != عدم)
	gui_do_findrepl(flags, p, q, ښکته);
    vim_free(p);
    vim_free(q);
}
#د_که_پای

    ساكن فراغ
HandleMouseHide(UINT u_پیغام, LPARAM lParam)
{
    ساكن LPARAM وروستی_l_Param = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    إذا (u_پیغام == WM_MOUSEMOVE || u_پیغام == WM_NCMOUSEMOVE)
    {
	إذا (lParam == وروستی_l_Param)
	    أرجع;
	وروستی_l_Param = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    بدل (u_پیغام)
    {
    حالة WM_KEYUP:
    حالة WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	إذا (p_mh)
	    gui_mch_mousehide(TRUE);
	اكسر;

    حالة WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    حالة WM_SYSCHAR:
    حالة WM_MOUSEMOVE:	 // show the pointer on any mouse action
    حالة WM_LBUTTONDOWN:
    حالة WM_LBUTTONUP:
    حالة WM_MBUTTONDOWN:
    حالة WM_MBUTTONUP:
    حالة WM_RBUTTONDOWN:
    حالة WM_RBUTTONUP:
    حالة WM_XBUTTONDOWN:
    حالة WM_XBUTTONUP:
    حالة WM_NCMOUSEMOVE:
    حالة WM_NCLBUTTONDOWN:
    حالة WM_NCLBUTTONUP:
    حالة WM_NCMBUTTONDOWN:
    حالة WM_NCMBUTTONUP:
    حالة WM_NCRBUTTONDOWN:
    حالة WM_NCRBUTTONUP:
    حالة WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	اكسر;
    }
}

    ساكن LRESULT CALLBACK
متن_Area_Wnd_Proc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_پیغام, lParam);

    s_u_پیغام = u_پیغام;
    s_wParam = wParam;

#که_تعریف_شوی FEAT_BEVAL_GUI
    track_user_activity(u_پیغام);
#د_که_پای

    بدل (u_پیغام)
    {
	HANDLE_پیغام(hwnd, WM_LBUTTONDBLCLK,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_LBUTTONDOWN,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_MBUTTONDBLCLK,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_MBUTTONDOWN,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_پیغام(hwnd, WM_RBUTTONDBLCLK,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_RBUTTONDOWN,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_XBUTTONDBLCLK,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_XBUTTONDOWN,On_Mouse_Button_ښکته);
	HANDLE_پیغام(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_اندازه,	On_اندازه_متن_Area);

#که_تعریف_شوی FEAT_BEVAL_GUI
	حالة WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    أرجع TRUE;
#د_که_پای
	افتراضي:
	    أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
    }
}

    ساكن فراغ
dyn_dwm_بارول(فراغ)
{
    hLibDwm = vim_بارول_Lib("dwmapi.dll");
    إذا (hLibDwm == عدم)
	أرجع;

    p_Dwm_سېټ_کړکۍ_Attribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	Get_Proc_پته(hLibDwm, "DwmSetWindowAttribute");

    إذا (p_Dwm_سېټ_کړکۍ_Attribute == عدم)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = عدم;
	أرجع;
    }
}

خارجي DWORD win_نسخه; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    فراغ
gui_mch_سېټ_titlebar_colors(فراغ)
{
#تعریف_کړه DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    إذا (p_Dwm_سېټ_کړکۍ_Attribute == عدم || win_نسخه < MAKE_VER(10, 0, 22000))
	أرجع;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T متن_Color = DWMWA_COLOR_DEFAULT;

    إذا (vim_strchr(p_go, GO_TITLEBAR) != عدم)
    {
	إذا (gui.دننه_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    متن_Color = gui.title_fg_pixel;
	}
	وإلا
	{
	    captionColor = gui.titlenc_bg_pixel;
	    متن_Color = gui.titlenc_fg_pixel;
	}

	إذا (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	إذا (متن_Color == INVALCOLOR)
	    متن_Color = DWMWA_COLOR_DEFAULT;
    }

    p_Dwm_سېټ_کړکۍ_Attribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, حجم(captionColor));
    p_Dwm_سېټ_کړکۍ_Attribute(s_hwnd, DWMWA_متن_COLOR,
	    &متن_Color, حجم(متن_Color));
}

/*
 * Called when the foreground or background color has been changed.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.شا_pixel);
    prevBrush = (HBRUSH)سېټ_Class_Long_Ptr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, عدم, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    إذا (s_textarea_bg_only && s_متن_Area != عدم)
	InvalidateRect(s_متن_Area, عدم, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.شا_pixel = GetSysColor(COLOR_کړکۍ);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_شا_pixel = gui.شا_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    إذا (!Is_کړکۍ_Visible(s_hwnd))
    {
	Show_کړکۍ(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	إذا (s_متن_Area != عدم)
	{
	    Update_کړکۍ(s_متن_Area);
	    gui_mch_flush();
	}
    }

#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#د_که_پای

    أرجع OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    RECT    rect;

    Get_کړکۍ_Rect(s_hwnd, &rect);
    *x = rect.کیڼ_2;
    *y = rect.سر_2;
    أرجع OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_سېټ_winpos(صحيح x, صحيح y)
{
    سېټ_کړکۍ_Pos(s_hwnd, عدم, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    فراغ
gui_mch_سېټ_متن_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    ساكن صحيح oldx = 0;
    ساكن صحيح oldy = 0;

    سېټ_کړکۍ_Pos(s_متن_Area, عدم, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#که_تعریف_شوی FEAT_TOOLBAR
    إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
	Send_پیغام(s_toolbarhwnd, WM_اندازه,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_لوړوالی));
#د_که_پای
#إذا defined(FEAT_GUI_TABLINE)
    إذا (showing_tabline)
    {
	صحيح	سر_2 = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    سر_2 = gui.toolbar_لوړوالی;
# endif
	GetClientRect(s_hwnd, &rect);
	Move_کړکۍ(s_tabhwnd, 0, سر_2, rect.ښی_2, gui.tabline_لوړوالی, TRUE);
    }
#د_که_پای

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    إذا (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, عدم, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    فراغ
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    صحيح		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    فراغ
gui_mch_سېټ_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    سېټ_کړکۍ_Pos(sb->id, عدم, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    RECT    rcTxt, rcWnd;
    صحيح	    xpad;

    Get_کړکۍ_Rect(s_متن_Area, &rcTxt);
    Get_کړکۍ_Rect(s_hwnd, &rcWnd);
    xpad = rcWnd.ښی_2 - rcTxt.ښی_2 - gui.scrollbar_پلنوالی
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    RECT    rcTxt, rcWnd;
    صحيح	    ypad;

    Get_کړکۍ_Rect(s_متن_Area, &rcTxt);
    Get_کړکۍ_Rect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_لوړوالی
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = Create_کړکۍ(
	"SCROLLBAR", "Scrollbar",
	WS_ماشوم | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, عدم,
	g_hinst, عدم);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ساكن scrollbar_T *
gui_mswin_پیدا_کړه_scrollbar(HWND hwnd)
{
    win_T	*wp;

    إذا (gui.bottom_sbar.id == hwnd)
	أرجع &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_scrollbars[SBAR_کیڼ].id == hwnd)
	    أرجع &wp->w_scrollbars[SBAR_کیڼ];
	إذا (wp->w_scrollbars[SBAR_ښی].id == hwnd)
	    أرجع &wp->w_scrollbars[SBAR_ښی];
    }
    أرجع عدم;
}

    ساكن فراغ
update_scrollbar_اندازه(فراغ)
{
    gui.scrollbar_پلنوالی = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_لوړوالی = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ساكن فراغ
Get_اوسط_Font_اندازه(HDC hdc, اندازه_3 *اندازه)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    Get_متن_Extent_ټکی(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, اندازه);
    اندازه->cx = (اندازه->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    ساكن فراغ
Get_Font_اندازه(GuiFont font, صحيح *char_پلنوالی, صحيح *char_لوړوالی)
{
    HWND    hwnd = Get_Desktop_کړکۍ();
    HDC	    hdc = Get_کړکۍ_DC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    اندازه_3    اندازه;
    TEXTMETRIC tm;

    Get_متن_Metrics(hdc, &tm);
    Get_اوسط_Font_اندازه(hdc, &اندازه);

    إذا (char_پلنوالی)
	*char_پلنوالی = اندازه.cx + tm.tmOverhang;
    إذا (char_لوړوالی)
	*char_لوړوالی = tm.tm_لوړوالی + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ساكن فراغ
Update_Font_اندازه(GuiFont font)
{
    Get_Font_اندازه(font, &gui.char_پلنوالی, &gui.char_لوړوالی);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
gui_mch_adjust_charheight(فراغ)
{
    Update_Font_اندازه(gui.norm_font);
    أرجع OK;
}

    ساكن GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = عدم;

    // Load the font
    font = Create_Font_غیرمستقیم_W(lf);

    إذا (font == عدم)
	أرجع NOFONT;

    أرجع (GuiFont)font;
}

    ساكن صحيح
pixels_منزل_points(صحيح pixels, صحيح vertical)
{
    صحيح		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = Get_Desktop_کړکۍ();
    hdc = Get_کړکۍ_DC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    أرجع points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    صحيح		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    إذا (get_logfont(&lf, name, عدم, giveErrorIfMissing) == OK)
    {
	lf.lf_لوړوالی = adjust_fontsize_by_dpi(lf.lf_لوړوالی);
	font = get_font_handle(&lf);
    }
    إذا (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    أرجع font;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font ناکارول_شوی, char_u *name)
{
    إذا (name == عدم)
	أرجع عدم;
    أرجع vim_strsave(name);
}
#د_که_پای

    فراغ
gui_mch_free_font(GuiFont font)
{
    إذا (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    صحيح i;

    تعريفنوع بنية Sys_Color_جدول
    {
	حرف	    *name;
	صحيح	    color;
    } Sys_Color_جدول;

    ساكن Sys_Color_جدول sys_جدول[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#که_تعریف_شوی COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#د_که_پای
	{"SYS_BTNHILIGHT", COLOR_BTNHILIGHT},
	{"SYS_BTNHIGHLIGHT", COLOR_BTNHIGHLIGHT},
	{"SYS_3DLIGHT", COLOR_3DLIGHT},
	{"SYS_3DSHADOW", COLOR_3DSHADOW},
	{"SYS_DESKTOP", COLOR_DESKTOP},
	{"SYS_INFOBK", COLOR_INFOBK},
	{"SYS_INFOTEXT", COLOR_INFOTEXT},
	{"SYS_3DFACE", COLOR_3DFACE},
	{"SYS_BTNFACE", COLOR_BTNFACE},
	{"SYS_BTNSHADOW", COLOR_BTNSHADOW},
	{"SYS_ACTIVEBORDER", COLOR_ACTIVEBORDER},
	{"SYS_ACTIVECAPTION", COLOR_ACTIVECAPTION},
	{"SYS_APPWORKSPACE", COLOR_APPWORKSPACE},
	{"SYS_BACKGROUND", COLOR_BACKGROUND},
	{"SYS_BTNTEXT", COLOR_BTNTEXT},
	{"SYS_CAPTIONTEXT", COLOR_CAPTIONTEXT},
	{"SYS_GRAYTEXT", COLOR_GRAYTEXT},
	{"SYS_HIGHLIGHT", COLOR_HIGHLIGHT},
	{"SYS_HIGHLIGHTTEXT", COLOR_HIGHLIGHTTEXT},
	{"SYS_INACTIVEBORDER", COLOR_INACTIVEBORDER},
	{"SYS_INACTIVECAPTION", COLOR_INACTIVECAPTION},
	{"SYS_INACTIVECAPTIONTEXT", COLOR_INACTIVECAPTIONTEXT},
	{"SYS_MENU", COLOR_MENU},
	{"SYS_MENUTEXT", COLOR_MENUTEXT},
	{"SYS_SCROLLBAR", COLOR_SCROLLBAR},
	{"SYS_WINDOW", COLOR_کړکۍ},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    لكل (i = 0; i < (صحيح)ARRAY_اوږدتیا(sys_جدول); i++)
	إذا (STRICMP(name, sys_جدول[i].name) == 0)
	    أرجع GetSysColor(sys_جدول[i].color);

    أرجع gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    أرجع gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیګانې[i].vim_code1 != NUL; i++)
	إذا (name[0] == special_کلیګانې[i].vim_code0
				       && name[1] == special_کلیګانې[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

    فراغ
gui_mch_beep(فراغ)
{
    پیغام_Beep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    فراغ
gui_mch_invert_rectangle(
    صحيح	    r,
    صحيح	    c,
    صحيح	    nr,
    صحيح	    nc)
{
    RECT    rc;

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#د_که_پای

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.کیڼ_2 = FILL_X(c);
    rc.سر_2 = FILL_Y(r);
    rc.ښی_2 = rc.کیڼ_2 + nc * gui.char_پلنوالی;
    rc.bottom = rc.سر_2 + nr * gui.char_لوړوالی;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    Show_کړکۍ(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#د_که_پای

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.کیڼ_2 = FILL_X(gui.ستون);
    rc.سر_2 = FILL_Y(gui.لیکه);
    rc.ښی_2 = rc.کیڼ_2 + gui.char_پلنوالی;
    إذا (mb_lefthalve(gui.لیکه, gui.ستون))
	rc.ښی_2 += gui.char_پلنوالی;
    rc.bottom = rc.سر_2 + gui.char_لوړوالی;
    hbr = CreateSolidBrush(color);
    چوکاټ_Rect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    فراغ
gui_mch_draw_part_cursor(
    صحيح		w,
    صحيح		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.کیڼ_2 =
#که_تعریف_شوی FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ښی ? FILL_X(gui.ستون + 1) - w :
#د_که_پای
		    FILL_X(gui.ستون);
    rc.سر_2 = FILL_Y(gui.لیکه) + gui.char_لوړوالی - h;
    rc.ښی_2 = rc.کیڼ_2 + w;
    rc.bottom = rc.سر_2 + h;

    fill_rect(&rc, عدم, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ساكن فراغ
output_Dead_کلی_re_Post_Ex(پیغام_3 original_پیغام, صحيح dead_key2set)
{
    ساكن پیغام_3 deadCharExpel;

    إذا (dead_کلی == DEAD_کلی_OFF)
	أرجع;

    dead_کلی = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.پیغام_2 = original_پیغام.پیغام_2;
    deadCharExpel.hwnd    = original_پیغام.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    Translate_پیغام(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    Post_پیغام(original_پیغام.hwnd, original_پیغام.پیغام_2, original_پیغام.wParam,
							  original_پیغام.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ساكن فراغ
output_Dead_کلی_re_Post(پیغام_3 original_پیغام)
{
    output_Dead_کلی_re_Post_Ex(original_پیغام, DEAD_کلی_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ساكن فراغ process_پیغام_usual_کلی(UINT vk, ثابت پیغام_3 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_کارول_شوی->ptr_process_پیغام_usual_کلی(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
process_پیغام_usual_کلی_experimental(UINT vk, ثابت پیغام_3 *pmsg)
{
    WCHAR	ch[8];
    صحيح		len;
    صحيح		i;
    UINT	لټون_کوډ;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    إذا (Get_کلی_State(VK_ښويدنه) & 0x8000)
	keyboard_state[VK_ښويدنه] = 0x80;
    إذا (Get_کلی_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    إذا ((Get_کلی_State(VK_RMENU) & 0x8000)
				 && (Get_کلی_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    لټون_کوډ = Map_Virtual_کلی(vk, MAPVK_VK_منزل_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = منزل_Unicode(vk, لټون_کوډ, keyboard_state, ch, ARRAY_اوږدتیا(ch),
	    0);
    إذا (len < 0)
	dead_کلی = DEAD_کلی_سېټ_DEFAULT;

    إذا (len <= 0)
    {
	صحيح wm_char = NUL;

	إذا (dead_کلی == DEAD_کلی_سېټ_DEFAULT
		&& (Get_کلی_State(VK_CONTROL) & 0x8000))
	{
	    إذا (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && لټون_کوډ == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && لټون_کوډ == 41))
		wm_char = '[';
	    إذا (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && لټون_کوډ == 27))
		wm_char = ']';
	}
	إذا (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    Post_پیغام_W(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_کلی = DEAD_کلی_TRANSIENT_دننه_ON_CHAR;
	}
	أرجع;
    }

    // Post the message as TranslateMessage would do.
    إذا (pmsg->پیغام_2 == WM_KEYDOWN)
    {
	لكل (i = 0; i < len; i++)
	    Post_پیغام_W(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    وإلا
    {
	لكل (i = 0; i < len; i++)
	    Post_پیغام_W(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
process_پیغام_usual_کلی_classic(UINT vk, ثابت پیغام_3 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    إذا (vk != 0xff
	    && (Get_کلی_State(VK_CONTROL) & 0x8000)
	    && !(Get_کلی_State(VK_ښويدنه) & 0x8000)
	    && !(Get_کلی_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	إذا (vk == '6' || Map_Virtual_کلی(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    جمع_کړه_منزل_ننوت_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	وإلا إذا (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    جمع_کړه_منزل_ننوت_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	وإلا إذا (vk == '2' || Map_Virtual_کلی(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    جمع_کړه_منزل_ننوت_buf(string, 1);
	}
	وإلا
	    Translate_پیغام(pmsg);
    }
    وإلا
	Translate_پیغام(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ساكن فراغ
process_پیغام(فراغ)
{
    پیغام_3		پیغام;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    صحيح		i;
    صحيح		modifiers = 0;
    صحيح		کلی;
#که_تعریف_شوی FEAT_MENU
    ساكن char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#د_که_پای
    ساكن صحيح  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    إذا (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    Get_پیغام_W(&پیغام, عدم, 0, 0);

#که_تعریف_شوی FEAT_OLE
    // Look after OLE Automation commands
    إذا (پیغام.پیغام_2 == WM_OLE)
    {
	char_u *str = (char_u *)پیغام.lParam;
	إذا (str == عدم || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    Dispatch_پیغام_W(&پیغام);
	}
	وإلا
	{
	    جمع_کړه_منزل_ننوت_buf(str, (صحيح)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	أرجع;
    }
#د_که_پای

#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    // Don't process messages used by the dialog
    إذا (s_findrep_hwnd != عدم && Is_Dialog_پیغام_W(s_findrep_hwnd, &پیغام))
    {
	HandleMouseHide(پیغام.پیغام_2, پیغام.lParam);
	أرجع;
    }
#د_که_پای

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    إذا (پیغام.پیغام_2 == WM_KEYDOWN || پیغام.پیغام_2 == WM_SYSKEYDOWN)
    {
	vk = (صحيح) پیغام.wParam;

	/*
	 * Handle dead keys in special conditions in other cases we let Windows
	 * handle them and do not interfere.
	 *
	 * The dead_key flag must be reset on several occasions:
	 * - in _OnChar() (or _OnSysChar()) as any dead key was necessarily
	 *   consumed at that point (This is when we let Windows combine the
	 *   dead character on its own)
	 *
	 * - Before doing something special such as regenerating keypresses to
	 *   expel the dead character as this could trigger an infinite loop if
	 *   for some reason TranslateMessage() do not trigger a call
	 *   immediately to _OnChar() (or _OnSysChar()).
	 */

	/*
	 * We are at the moment after WM_CHAR with DEAD_KEY_SKIP_ON_CHAR event
	 * was handled by _WndProc, this keypress we want to process normally
	 */
	إذا (keycode_trans_strategy_کارول_شوی->is_experimental()
		&& dead_کلی == DEAD_کلی_SKIP_ON_CHAR)
	{
	    dead_کلی = DEAD_کلی_OFF;
	}

	إذا (dead_کلی != DEAD_کلی_OFF)
	{
	    /*
	     * Expel the dead key pressed with Ctrl in a special way.
	     *
	     * After dead key was pressed with Ctrl in some cases, ESC was
	     * artificially injected and handled by _OnChar(), now we are
	     * dealing with completely new key press from the user. If we don't
	     * do anything, ToUnicode() call will interpret this vk+scan_code
	     * under influence of "dead-modifier". To prevent this we translate
	     * this message replacing current char from user with VK_SPACE,
	     * which will cause WM_CHAR with dead_key's character itself. Using
	     * DEAD_KEY_SKIP_ON_CHAR value of dead_char we force _OnChar() to
	     * ignore this one WM_CHAR event completely. Afterwards (due to
	     * usage of PostMessage), this procedure is scheduled to be called
	     * again with user char and on next entry we will clean
	     * DEAD_KEY_SKIP_ON_CHAR. We cannot use original
	     * outputDeadKey_rePost() since we do not wish to reset dead_key
	     * value.
	     */
	    إذا (keycode_trans_strategy_کارول_شوی->is_experimental() &&
		    dead_کلی == DEAD_کلی_TRANSIENT_دننه_ON_CHAR)
	    {
		output_Dead_کلی_re_Post_Ex(پیغام,
				       /*dead_key2set=*/DEAD_کلی_SKIP_ON_CHAR);
		أرجع;
	    }

	    إذا (dead_کلی != DEAD_کلی_سېټ_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		أرجع;
	    }

	    /*
	     * If a dead key was pressed and the user presses VK_SPACE,
	     * VK_BACK, or VK_ESCAPE it means that he actually wants to deal
	     * with the dead char now, so do nothing special and let Windows
	     * handle it.
	     *
	     * Note that VK_SPACE combines with the dead_key's character and
	     * only one WM_CHAR will be generated by TranslateMessage(), in
	     * the two other cases two WM_CHAR will be generated: the dead
	     * char and VK_BACK or VK_ESCAPE. That is most likely what the
	     * user expects.
	     */
	    إذا ((vk == VK_SPACE || vk == VK_شا || vk == VK_ESCAPE))
	    {
		dead_کلی = DEAD_کلی_OFF;
		Translate_پیغام(&پیغام);
		أرجع;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    وإلا إذا ((get_real_state()
			    & (ډېرپېښ_ورننباسه | ډېرپېښ_CMDLINE | ډېرپېښ_SELECT)) == 0)
	    {
		output_Dead_کلی_re_Post(پیغام);
		أرجع;
	    }
	}

	// Check for CTRL-BREAK
	إذا (vk == VK_CANCEL)
	{
	    trash_ننوت_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    جمع_کړه_منزل_ننوت_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	إذا (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    Translate_پیغام(&پیغام);
	    أرجع;
	}

	لكل (i = 0; special_کلیګانې[i].کلی_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    إذا (special_کلیګانې[i].کلی_sym == vk
		    && (vk != VK_SPACE || !(Get_کلی_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		إذا (dead_کلی == DEAD_کلی_سېټ_DEFAULT
			&& (special_کلیګانې[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_کلی_re_Post(پیغام);
		    أرجع;
		}

#که_تعریف_شوی FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		إذا (vk == VK_F10
			&& gui.menu_is_فعال
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  عدم, عدم) == عدم)
		    اكسر;
#د_که_پای
		modifiers = get_فعال_modifiers_له_لارې_ptr();

		إذا (special_کلیګانې[i].vim_code1 == NUL)
		    کلی = special_کلیګانې[i].vim_code0;
		وإلا
		    کلی = منزل_SPECIAL(special_کلیګانې[i].vim_code0,
						   special_کلیګانې[i].vim_code1);
		کلی = simplify_کلی(کلی, &modifiers);
		إذا (کلی == CSI)
		    کلی = K_CSI;

		إذا (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    جمع_کړه_منزل_ننوت_buf(string, 3);
		}

		إذا (IS_SPECIAL(کلی))
		{
		    string[0] = CSI;
		    string[1] = K_دوهم(کلی);
		    string[2] = K_دریم(کلی);
		    جمع_کړه_منزل_ننوت_buf(string, 3);
		}
		وإلا
		{
		    صحيح	len;

		    // Handle "key" as a Unicode character.
		    len = char_منزل_string(کلی, string, 40, FALSE);
		    جمع_کړه_منزل_ننوت_buf(string, len);
		}
		اكسر;
	    }
	}

	// Not a special key.
	إذا (special_کلیګانې[i].کلی_sym == 0)
	{
	    process_پیغام_usual_کلی(vk, &پیغام);
	}
    }
#که_تعریف_شوی FEAT_MBYTE_IME
    وإلا إذا (پیغام.پیغام_2 == WM_IME_NOTIFY)
	_OnImeNotify(پیغام.hwnd, (DWORD)پیغام.wParam, (DWORD)پیغام.lParam);
    وإلا إذا (پیغام.پیغام_2 == WM_KEYUP && im_get_حالت())
	// added for non-MS IME (Yasuhiro Matsumoto)
	Translate_پیغام(&پیغام);
#د_که_پای

#که_تعریف_شوی FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    إذا (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  عدم, عدم) == عدم)
#د_که_پای
	Dispatch_پیغام_W(&پیغام);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    فراغ
gui_mch_update(فراغ)
{
    پیغام_3	    پیغام;

    إذا (!s_busy_processing)
	طالما (Peek_پیغام_W(&پیغام, عدم, 0, 0, PM_NOREMOVE)
						  && !vim_is_ننوت_buf_ډک())
	    process_پیغام();
}

    ساكن فراغ
remove_any_timer(فراغ)
{
    پیغام_3		پیغام;

    إذا (s_wait_timer != 0 && !s_timed_بهر)
    {
	KillTimer(عدم, s_wait_timer);

	// Eat spurious WM_TIMER messages
	طالما (Peek_پیغام_W(&پیغام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#که_تعریف_شوی FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ساكن garray_T	server_pending_ga = {0, 0, 0, 0, عدم};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    فراغ
server_جمع_کړه_ننوت(char_u *str)
{
    إذا (server_pending_ga.ga_معلومات == عدم)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ساكن فراغ
server_flush_ننوت(فراغ)
{
    char_u  *str;

    إذا (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	أرجع;
    str = vim_strnsave(server_pending_ga.ga_معلومات, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    إذا (str != عدم)
    {
	server_منزل_ننوت_buf(str);
	vim_free(str);
    }
}
#د_که_پای

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    صحيح
gui_mch_wait_for_chars(اوږد wtime)
{
    صحيح		focus;

    s_timed_بهر = FALSE;

    إذا (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	إذا (s_busy_processing)
	    أرجع FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = سېټ_Timer(عدم, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.دننه_focus;
    طالما (!s_timed_بهر)
    {
	// Stop or start blinking when focus changes
	إذا (gui.دننه_focus != focus)
	{
	    إذا (gui.دننه_focus)
		gui_mch_پیل_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.دننه_focus;
	}

	إذا (s_need_activate)
	{
	    (فراغ)سېټ_Foreground_کړکۍ(s_hwnd);
	    s_need_activate = FALSE;
	}

#که_تعریف_شوی FEAT_TIMERS
	did_جمع_کړه_timer = FALSE;
#د_که_پای
#که_تعریف_شوی پیغام_کتار
	// Check channel I/O while waiting for a message.
	لكل (;;)
	{
	    پیغام_3 پیغام;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ننوت();
# endif
# ifdef FEAT_TIMERS
	    إذا (did_جمع_کړه_timer)
		اكسر;
# endif
	    إذا (Peek_پیغام_W(&پیغام, عدم, 0, 0, PM_NOREMOVE))
	    {
		process_پیغام();
		اكسر;
	    }
	    وإلا إذا (ننوت_شته()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || پیغام_Wait_For_مضرب_Objects(0, عدم, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		اكسر;
	}
#وإلا
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_پیغام();
#د_که_پای

	إذا (ننوت_شته())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    إذا (!s_getting_focus)
		s_button_pending = -1;

	    أرجع OK;
	}

#که_تعریف_شوی FEAT_TIMERS
	إذا (did_جمع_کړه_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    اكسر;
	}
#د_که_پای
    }
    allow_scrollbar = FALSE;
    أرجع FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_بلاک(
    صحيح		row1,
    صحيح		col1,
    صحيح		row2,
    صحيح		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.کیڼ_2 = FILL_X(col1);
    rc.سر_2 = FILL_Y(row1);
    rc.ښی_2 = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    فراغ
gui_mch_clear_all(فراغ)
{
    RECT    rc;

    rc.کیڼ_2 = 0;
    rc.سر_2 = 0;
    rc.ښی_2 = Columns * gui.char_پلنوالی + 2 * gui.border_پلنوالی;
    rc.bottom = لیکې * gui.char_لوړوالی + 2 * gui.border_پلنوالی;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    فراغ
gui_mch_enable_menu(صحيح flag)
{
#که_تعریف_شوی FEAT_MENU
    سېټ_Menu(s_hwnd, flag ? s_menuBar : عدم);
#د_که_پای
}

    فراغ
gui_mch_سېټ_menu_pos(
    صحيح	    x ناکارول_شوی,
    صحيح	    y ناکارول_شوی,
    صحيح	    w ناکارول_شوی,
    صحيح	    h ناکارول_شوی)
{
    // It will be in the right place anyway
}

#إذا defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    فراغ
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    صحيح		hidden)
{
    /*
     * This doesn't do what we want.  Hmm, just grey the menu items for now.
     */
    /*
    if (hidden)
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_DISABLED);
    else
	EnableMenuItem(s_menuBar, menu->id, MF_BYCOMMAND | MF_ENABLED);
    */
    gui_mch_menu_grey(menu, hidden);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    فراغ
gui_mch_draw_menubar(فراغ)
{
    DrawMenuBar(s_hwnd);
}
#د_که_پای // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    أرجع (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#إذا defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ساكن کلمه_2
Pixel_منزل_Dialog_X(صحيح عدد_Pixels)
{
    أرجع (کلمه_2)((عدد_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ساكن کلمه_2
Pixel_منزل_Dialog_Y(صحيح عدد_Pixels)
{
    أرجع (کلمه_2)((عدد_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ساكن صحيح
Get_متن_پلنوالی(HDC hdc, char_u *str, صحيح len)
{
    اندازه_3    اندازه;

    Get_متن_Extent_ټکی(hdc, (LPCSTR)str, len, &اندازه);
    أرجع اندازه.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ساكن صحيح
Get_متن_پلنوالی_Enc(HDC hdc, char_u *str, صحيح len)
{
    اندازه_3	اندازه;
    WCHAR	*wstr;
    صحيح		n;
    صحيح		wlen = len;

    wstr = enc_منزل_utf16(str, &wlen);
    إذا (wstr == عدم)
	أرجع 0;

    n = Get_متن_Extent_ټکی_W(hdc, wstr, wlen, &اندازه);
    vim_free(wstr);
    إذا (n)
	أرجع اندازه.cx;
    أرجع 0;
}

ساكن فراغ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ساكن BOOL
Center_کړکۍ(
    HWND hwnd_ماشوم,
    HWND hwnd_پلار)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    r_ماشوم, r_پلار, rScreen;
    صحيح		    w_ماشوم, h_ماشوم, w_پلار, h_پلار;
    صحيح		    xNew, yNew;
    HDC		    hdc;

    Get_کړکۍ_Rect(hwnd_ماشوم, &r_ماشوم);
    w_ماشوم = r_ماشوم.ښی_2 - r_ماشوم.کیڼ_2;
    h_ماشوم = r_ماشوم.bottom - r_ماشوم.سر_2;

    // If Vim is minimized put the window in the middle of the screen.
    إذا (hwnd_پلار == عدم || IsMinimized(hwnd_پلار))
	get_work_area(&r_پلار);
    وإلا
	Get_کړکۍ_Rect(hwnd_پلار, &r_پلار);
    w_پلار = r_پلار.ښی_2 - r_پلار.کیڼ_2;
    h_پلار = r_پلار.bottom - r_پلار.سر_2;

    moninfo.cb_اندازه = حجم(MONITORINFO);
    mon = Monitor_From_کړکۍ(hwnd_ماشوم, MONITOR_DEFAULTTOPRIMARY);
    إذا (mon != عدم && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    وإلا
    {
	hdc = GetDC(hwnd_ماشوم);
	rScreen.کیڼ_2 = 0;
	rScreen.سر_2 = 0;
	rScreen.ښی_2 = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwnd_ماشوم, hdc);
    }

    xNew = r_پلار.کیڼ_2 + ((w_پلار - w_ماشوم) / 2);
    إذا (xNew < rScreen.کیڼ_2)
	xNew = rScreen.کیڼ_2;
    وإلا إذا ((xNew + w_ماشوم) > rScreen.ښی_2)
	xNew = rScreen.ښی_2 - w_ماشوم;

    yNew = r_پلار.سر_2 + ((h_پلار - h_ماشوم) / 2);
    إذا (yNew < rScreen.سر_2)
	yNew = rScreen.سر_2;
    وإلا إذا ((yNew + h_ماشوم) > rScreen.bottom)
	yNew = rScreen.bottom - h_ماشوم;

    أرجع سېټ_کړکۍ_Pos(hwnd_ماشوم, عدم, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#د_که_پای // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (s_toolbarhwnd == عدم)
	أرجع;

    إذا (showit)
    {
	// Enable unicode support
	Send_پیغام(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	Show_کړکۍ(s_toolbarhwnd, SW_SHOW);
    }
    وإلا
	Show_کړکۍ(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_شمېر 31

#د_که_پای

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
جمع_کړه_tabline_popup_menu_ثبت(HMENU pmenu, UINT توکی_id, char_u *توکی_متن)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_منزل_utf16(توکی_متن, عدم);
    إذا (wn == عدم)
	أرجع;

    infow.cb_اندازه = حجم(infow);
    infow.f_ماسک = MIIM_TYPE | MIIM_ID;
    infow.wID = توکی_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_معلومات = wn;
    infow.cch = (UINT)wcslen(wn);
    ورننباسه_Menu_توکی_W(pmenu, توکی_id, FALSE, &infow);
    vim_free(wn);
}

    ساكن فراغ
show_tabline_popup_menu(فراغ)
{
    HMENU	    tab_pmenu;
    اوږد	    rval;
    ټکی_2	    pt;

    // When ignoring events don't show the menu.
    إذا (hold_gui_events || cmdwin_type != 0)
	أرجع;

    tab_pmenu = CreatePopupMenu();
    إذا (tab_pmenu == عدم)
	أرجع;

    إذا (لومړی_tabpage->tp_بل != عدم)
	جمع_کړه_tabline_popup_menu_ثبت(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    جمع_کړه_tabline_popup_menu_ثبت(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    جمع_کړه_tabline_popup_menu_ثبت(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									عدم);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    إذا (rval > 0)
    {
	TCHITTESTINFO htinfo;
	صحيح idx;

	إذا (Screen_منزل_Client(s_tabhwnd, &pt) == 0)
	    أرجع;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = Tab_Ctrl_موندنه_Test(s_tabhwnd, &htinfo);
	إذا (idx == -1)
	    idx = 0;
	وإلا
	    idx += 1;

	send_tabline_menu_event(idx, (صحيح)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (s_tabhwnd == عدم)
	أرجع;

    إذا (!showit != !showing_tabline)
    {
	إذا (showit)
	    Show_کړکۍ(s_tabhwnd, SW_SHOW);
	وإلا
	    Show_کړکۍ(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline(فراغ)
{
    أرجع s_tabhwnd != عدم && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline(فراغ)
{
    tabpage_T	*tp;
    TCITEM	tie;
    صحيح		nr = 0;
    صحيح		curtabidx = 0;
    صحيح		tabadded = 0;
    WCHAR	*wstr = عدم;

    إذا (s_tabhwnd == عدم)
	أرجع;

    // Enable unicode support
    Send_پیغام(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.ماسک = TCIF_متن;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    Send_پیغام(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = لومړی_tabpage; tp != عدم; tp = tp->tp_بل, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	إذا (nr >= Tab_Ctrl_Get_توکی_شمېر(s_tabhwnd))
	{
	    // Add the tab
	    tie.psz_متن = "-Empty-";
	    Tab_Ctrl_ورننباسه_توکی(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.psz_متن = (LPSTR)NameBuff;

	wstr = enc_منزل_utf16(NameBuff, عدم);
	إذا (wstr != عدم)
	{
	    TCITEMW		tiw;

	    tiw.ماسک = TCIF_متن;
	    tiw.iImage = -1;
	    tiw.psz_متن = wstr;
	    Send_پیغام(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    طالما (nr < Tab_Ctrl_Get_توکی_شمېر(s_tabhwnd))
	Tab_Ctrl_Delete_توکی(s_tabhwnd, nr);

    إذا (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_سېټ_Cur_Sel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    Send_پیغام(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    Redraw_کړکۍ(s_tabhwnd, عدم, عدم,
		    RDW_ERASE | RDW_چوکاټ | RDW_INVALIDATE | RDW_ALLCHILDREN);

    إذا (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_سېټ_Cur_Sel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سېټ_curtab(صحيح nr)
{
    إذا (s_tabhwnd == عدم)
	أرجع;

    إذا (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	Tab_Ctrl_سېټ_Cur_Sel(s_tabhwnd, nr - 1);
}

#د_که_پای

#که_تعریف_شوی FEAT_GUI_DARKTHEME
    فراغ
gui_mch_سېټ_dark_theme(صحيح dark)
{
    إذا (p_Dwm_سېټ_کړکۍ_Attribute != عدم && win_نسخه >= MAKE_VER(10, 0, 18985))
	p_Dwm_سېټ_کړکۍ_Attribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_ډېرپېښ, &dark,
		حجم(dark));

    إذا (p_سېټ_Preferred_App_ډېرپېښ != عدم && win_نسخه >= MAKE_VER(10, 0, 18362))
	p_سېټ_Preferred_App_ډېرپېښ(dark);

    إذا (pFlushMenuThemes != عدم && win_نسخه >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ساكن فراغ
dyn_uxtheme_بارول(فراغ)
{
    hUxThemeLib = vim_بارول_Lib("uxtheme.dll");
    إذا (hUxThemeLib == عدم)
	أرجع;

    p_سېټ_Preferred_App_ډېرپېښ = (DWORD (WINAPI *)(DWORD))
	Get_Proc_پته(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (فراغ (WINAPI *)(فراغ))
	Get_Proc_پته(hUxThemeLib, MAKEINTRESOURCE(136));

    إذا (p_سېټ_Preferred_App_ډېرپېښ == عدم || pFlushMenuThemes == عدم)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = عدم;
	أرجع;
    }
}

#د_که_پای // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    فراغ
gui_mch_سېټ_fullscreen(صحيح flag)
{
    ساكن RECT نورمال_rect;
    ساكن LONG_PTR نورمال_style, نورمال_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    إذا (!ډک_screen) // Windows not set yet.
	أرجع;

    إذا (flag)
    {
	إذا (fullscreen_on)
	    أرجع;

	// Enter fullscreen mode
	Get_کړکۍ_Rect(s_hwnd, &rc);

	moninfo.cb_اندازه = حجم(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	إذا (mon == عدم || !GetMonitorInfo(mon, &moninfo))
	    أرجع;

	// Save current window state
	Get_کړکۍ_Rect(s_hwnd, &نورمال_rect);
	نورمال_style = Get_کړکۍ_Long_Ptr(s_hwnd, GWL_STYLE);
	نورمال_exstyle = Get_کړکۍ_Long_Ptr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	سېټ_کړکۍ_Long_Ptr(s_hwnd, GWL_STYLE,
		نورمال_style & ~(WS_CAPTION | WS_THICKFRAME));
	سېټ_کړکۍ_Long_Ptr(s_hwnd, GWL_EXSTYLE,
		نورمال_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	سېټ_کړکۍ_Pos(s_hwnd, عدم,
		moninfo.rcMonitor.کیڼ_2,
		moninfo.rcMonitor.سر_2,
		moninfo.rcMonitor.ښی_2 - moninfo.rcMonitor.کیڼ_2,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.سر_2,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    وإلا
    {
	إذا (!fullscreen_on)
	    أرجع;

	// Exit fullscreen mode
	سېټ_کړکۍ_Long_Ptr(s_hwnd, GWL_STYLE, نورمال_style);
	سېټ_کړکۍ_Long_Ptr(s_hwnd, GWL_EXSTYLE, نورمال_exstyle);

	// Restore original window position and size
	سېټ_کړکۍ_Pos(s_hwnd, عدم,
		نورمال_rect.کیڼ_2,
		نورمال_rect.سر_2,
		نورمال_rect.ښی_2 - نورمال_rect.کیڼ_2,
		نورمال_rect.bottom - نورمال_rect.سر_2,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    فراغ
ex_simalt(exarg_T *eap)
{
    char_u	*کلیګانې = eap->arg;
    صحيح		fill_typebuf = FALSE;
    char_u	کلی_name[4];

    Post_پیغام(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    طالما (*کلیګانې)
    {
	إذا (*کلیګانې == '~')
	    *کلیګانې = ' ';	    // for showing system menu
	Post_پیغام(s_hwnd, WM_CHAR, (WPARAM)*کلیګانې, (LPARAM)0);
	کلیګانې++;
	fill_typebuf = TRUE;
    }
    إذا (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	کلی_name[0] = K_SPECIAL;
	کلی_name[1] = KS_EXTRA;
	کلی_name[2] = KE_NOP;
	کلی_name[3] = NUL;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#د_که_پای
	(فراغ)ins_typebuf(کلی_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    ساكن فراغ
initialise_findrep(char_u *initial_string)
{
    صحيح		wword = FALSE;
    صحيح		mcase = !p_ic;
    char_u	*ثبت_متن;

    // Get the search string to use.
    ثبت_متن = get_پیدا_کړه_dialog_متن(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_ښکته;
    إذا (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    إذا (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    إذا (ثبت_متن != عدم && *ثبت_متن != NUL)
    {
	WCHAR *p = enc_منزل_utf16(ثبت_متن, عدم);
	إذا (p != عدم)
	{
	    صحيح len = s_findrep_struct.w_پیدا_کړه_What_Len - 1;

	    wcsncpy(s_findrep_struct.lpstr_پیدا_کړه_What, p, len);
	    s_findrep_struct.lpstr_پیدا_کړه_What[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(ثبت_متن);
}
#د_که_پای

    ساكن فراغ
سېټ_کړکۍ_title(HWND hwnd, حرف *title)
{
    إذا (title != عدم)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_منزل_utf16((char_u *)title, عدم);
	إذا (wbuf != عدم)
	{
	    سېټ_کړکۍ_متن_W(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    وإلا
	(فراغ)سېټ_کړکۍ_متن_W(hwnd, عدم);
}

    فراغ
gui_mch_پیدا_کړه_dialog(exarg_T *eap)
{
#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    إذا (s_findrep_پیغام != 0)
    {
	إذا (Is_کړکۍ(s_findrep_hwnd) && !s_findrep_is_پیدا_کړه)
	    Destroy_کړکۍ(s_findrep_hwnd);

	إذا (!Is_کړکۍ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = پیدا_کړه_متن_W(&s_findrep_struct);
	}

	سېټ_کړکۍ_title(s_findrep_hwnd, _("Find string"));
	(فراغ)سېټ_Focus(s_findrep_hwnd);

	s_findrep_is_پیدا_کړه = TRUE;
    }
#د_که_پای
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    إذا (s_findrep_پیغام != 0)
    {
	إذا (Is_کړکۍ(s_findrep_hwnd) && s_findrep_is_پیدا_کړه)
	    Destroy_کړکۍ(s_findrep_hwnd);

	إذا (!Is_کړکۍ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = Replace_متن_W(&s_findrep_struct);
	}

	سېټ_کړکۍ_title(s_findrep_hwnd, _("Find & Replace"));
	(فراغ)سېټ_Focus(s_findrep_hwnd);

	s_findrep_is_پیدا_کړه = FALSE;
    }
#د_که_پای
}


/*
 * Set visibility of the pointer.
 */
    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (hide == gui.اشاره_ګر_hidden)
	أرجع;

    ShowCursor(!hide);
    gui.اشاره_ګر_hidden = hide;
}

#که_تعریف_شوی FEAT_MENU
    ساكن فراغ
gui_mch_show_popupmenu_at(vimmenu_T *menu, صحيح x, صحيح y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (فراغ)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(صحيح)0,	    //reserved param
	s_hwnd,
	عدم);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#د_که_پای

/*
 * Got a message when the system will go down.
 */
    ساكن فراغ
On_پای_ټکی_Session(فراغ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ساكن فراغ
_OnClose(HWND hwnd ناکارول_شوی)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ساكن فراغ
_OnDestroy(HWND hwnd)
{
    إذا (!destroying)
	_OnClose(hwnd);
}

    ساكن فراغ
_OnPaint(
    HWND hwnd)
{
    إذا (IsMinimized(hwnd))
	أرجع;

    PAINTSTRUCT ps;

    بهر_flush();	    // make sure all output has been processed
    (فراغ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    إذا (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.کیڼ_2 = rect.کیڼ_2;
	ps.rcPaint.ښی_2 = rect.ښی_2;
    }

    إذا (!Is_Rect_تشه(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	إذا (!screen_cleared || Screen_کرښې == عدم)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	وإلا
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.کیڼ_2, ps.rcPaint.سر_2,
		ps.rcPaint.ښی_2 - ps.rcPaint.کیڼ_2 + 1,
		ps.rcPaint.bottom - ps.rcPaint.سر_2 + 1);
    }

    پای_ټکی_Paint(hwnd, &ps);
}

    ساكن فراغ
On_اندازه(
    HWND hwnd,
    UINT state ناکارول_شوی,
    صحيح cx,
    صحيح cy)
{
    إذا (!IsMinimized(hwnd) && !s_دننه_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_لوړوالی(TRUE);
    }
}

    ساكن فراغ
On_سېټ_Focus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (فراغ)Def_کړکۍ_Proc_W(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ساكن فراغ
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    إذا (destroying)
	أرجع;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (فراغ)Def_کړکۍ_Proc_W(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ساكن LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    أرجع Def_کړکۍ_Proc_W(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    Destroy_کړکۍ(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    RECT rct;
    ټکی_2 mp;

    (فراغ)Get_کړکۍ_Rect(s_متن_Area, &rct);
    (فراغ)GetCursorPos(&mp);
    *x = (صحيح)(mp.x - rct.کیڼ_2);
    *y = (صحيح)(mp.y - rct.سر_2);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    RECT rct;

    (فراغ)Get_کړکۍ_Rect(s_متن_Area, &rct);
    (فراغ)سېټ_Cursor_Pos(x + gui.border_دننه_واټن + rct.کیڼ_2,
		       y + gui.border_دننه_واټن + rct.سر_2);
}

    ساكن فراغ
gui_mswin_get_valid_dimensions(
    صحيح w,
    صحيح h,
    صحيح *valid_w,
    صحيح *valid_h,
    صحيح *ستون_شمېر,
    صحيح *لیکې_2)
{
    صحيح	    بنسټ_پلنوالی, بنسټ_لوړوالی;

    بنسټ_پلنوالی = gui_get_بنسټ_پلنوالی()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    بنسټ_لوړوالی = gui_get_بنسټ_لوړوالی()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_لوړوالی(FALSE);
    *ستون_شمېر = (w - بنسټ_پلنوالی) / gui.char_پلنوالی;
    *لیکې_2 = (h - بنسټ_لوړوالی) / gui.char_لوړوالی;
    *valid_w = بنسټ_پلنوالی + *ستون_شمېر * gui.char_پلنوالی;
    *valid_h = بنسټ_لوړوالی + *لیکې_2 * gui.char_لوړوالی;
}

    فراغ
gui_mch_flash(صحيح msec)
{
    RECT    rc;

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#د_که_پای

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.کیڼ_2 = 0;
    rc.سر_2 = 0;
    rc.ښی_2 = gui.عدد_ستون_شمېر * gui.char_پلنوالی;
    rc.bottom = gui.عدد_لیکې * gui.char_لوړوالی;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((اوږد)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ساكن BOOL
is_ټکی_onscreen(صحيح x, صحيح y)
{
    ټکی_2   pt = {x, y};

    أرجع Monitor_From_ټکی(pt, MONITOR_DEFAULTTONULL) != عدم;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ساكن BOOL
is_کړکۍ_onscreen(HWND hwnd)
{
    RECT    rc;
    ټکی_2   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.کیڼ_2;
    p1.y = rc.سر_2;
    p2.x = rc.ښی_2 - 1;
    p2.y = rc.bottom - 1;
    Client_منزل_Screen(hwnd, &p1);
    Client_منزل_Screen(hwnd, &p2);

    إذا (!is_ټکی_onscreen(p1.x, p1.y))
	أرجع FALSE;
    إذا (!is_ټکی_onscreen(p1.x, p2.y))
	أرجع FALSE;
    إذا (!is_ټکی_onscreen(p2.x, p1.y))
	أرجع FALSE;
    إذا (!is_ټکی_onscreen(p2.x, p2.y))
	أرجع FALSE;
    أرجع TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ساكن صحيح
get_scroll_flags(فراغ)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    إذا (!is_کړکۍ_onscreen(s_hwnd))
	أرجع SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    Get_کړکۍ_Rect(s_hwnd, &rcVim);
    لكل (hwnd = s_hwnd; (hwnd = Get_کړکۍ(hwnd, GW_HWNDPREV)) != (HWND)0; )
	إذا (Is_کړکۍ_Visible(hwnd))
	{
	    Get_کړکۍ_Rect(hwnd, &rcOther);
	    إذا (IntersectRect(&rcDest, &rcVim, &rcOther))
		أرجع SW_INVALIDATE;
	}
    أرجع 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ساكن فراغ
intel_gpu_workaround(فراغ)
{
    GetPixel(s_hdc, FILL_X(gui.ستون), FILL_Y(gui.لیکه));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_کرښې(
    صحيح	    لیکه,
    صحيح	    عدد_کرښې)
{
    RECT	rc;

    rc.کیڼ_2 = FILL_X(gui.scroll_region_کیڼ);
    rc.ښی_2 = FILL_X(gui.scroll_region_ښی + 1);
    rc.سر_2 = FILL_Y(لیکه);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX() && is_کړکۍ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -عدد_کرښې * gui.char_لوړوالی, &rc);
    }
    وإلا
#د_که_پای
    {
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#د_که_پای
	intel_gpu_workaround();
	Scroll_کړکۍ_Ex(s_متن_Area, 0, -عدد_کرښې * gui.char_لوړوالی,
				    &rc, &rc, عدم, عدم, get_scroll_flags());
	Update_کړکۍ(s_متن_Area);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_بلاک(gui.scroll_region_bot - عدد_کرښې + 1,
						       gui.scroll_region_کیڼ,
	gui.scroll_region_bot, gui.scroll_region_ښی);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_ورننباسه_کرښې(
    صحيح		لیکه,
    صحيح		عدد_کرښې)
{
    RECT	rc;

    rc.کیڼ_2 = FILL_X(gui.scroll_region_کیڼ);
    rc.ښی_2 = FILL_X(gui.scroll_region_ښی + 1);
    rc.سر_2 = FILL_Y(لیکه);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX() && is_کړکۍ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, عدد_کرښې * gui.char_لوړوالی, &rc);
    }
    وإلا
#د_که_پای
    {
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#د_که_پای
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	Scroll_کړکۍ_Ex(s_متن_Area, 0, عدد_کرښې * gui.char_لوړوالی,
				    &rc, &rc, عدم, عدم, get_scroll_flags());
	Update_کړکۍ(s_متن_Area);
    }

    gui_clear_بلاک(لیکه, gui.scroll_region_کیڼ,
				لیکه + عدد_کرښې - 1, gui.scroll_region_ښی);
}


    فراغ
gui_mch_exit(صحيح rc ناکارول_شوی)
{
#إذا defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = عدم;
#د_که_پای

    ReleaseDC(s_متن_Area, s_hdc);
    DeleteObject(s_brush);

#که_تعریف_شوی FEAT_TEAROFF
    // Unload the tearoff bitmap
    (فراغ)DeleteObject((HGDIOBJ)s_htearbitmap);
#د_که_پای

    // Destroy our window (if we have one).
    إذا (s_hwnd != عدم)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	Destroy_کړکۍ(s_hwnd);
    }
}

    ساكن char_u *
logfont2name(LOGFONTW lf)
{
    حرف	*charset_name;
    حرف	*quality_name;
    حرف	*font_name;
    size_t	res_اندازه;
    حرف	*res;

    font_name = (حرف *)utf16_منزل_enc(lf.lfFaceName, عدم);
    إذا (font_name == عدم)
	أرجع عدم;
    charset_name = charset_id2name((صحيح)lf.lf_Char_سېټ);
    quality_name = quality_id2name((صحيح)lf.lfQuality);

    res_اندازه = STRLEN(font_name) + 30
		    + (charset_name == عدم ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == عدم ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_اندازه);
    إذا (res != عدم)
    {
	حرف	*p;
	صحيح	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	لكل (p = font_name; *p != NUL; ++p)
	{
	    إذا (که_تشه(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_منزل_points(
			 lf.lf_لوړوالی < 0 ? -lf.lf_لوړوالی : lf.lf_لوړوالی, TRUE);
	إذا (lf.lf_وزن == FW_نورمال || lf.lf_وزن == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_اندازه, "%s:h%d", font_name, points);
	وإلا
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_اندازه, "%s:h%d:W%ld", font_name, points, lf.lf_وزن);

	res_len += vim_snprintf_safelen(
	    (حرف *)res + res_len,
	    res_اندازه - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lf_وزن == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lf_Strike_بهر ? ":s" : "");

	إذا (charset_name != عدم)
	    res_len += vim_snprintf_safelen((حرف *)res + res_len,
		res_اندازه - res_len, ":c%s", charset_name);
	إذا (quality_name != عدم)
	    vim_snprintf((حرف *)res + res_len,
		res_اندازه - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    أرجع (char_u *)res;
}


#که_تعریف_شوی FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ساكن فراغ
update_im_font(فراغ)
{
    LOGFONTW	lf_wide, lf;

    إذا (p_guifontwide != عدم && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, حجم(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    وإلا
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lf_لوړوالی = lf.lf_لوړوالی * DEFAULT_DPI / s_dpi;
    im_سېټ_font(&lf);
}
#د_که_پای

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    فراغ
gui_mch_wide_font_بدل_شوی(فراغ)
{
    LOGFONTW lf;

#که_تعریف_شوی FEAT_MBYTE_IME
    update_im_font();
#د_که_پای

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    إذا (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, حجم(lf), &lf))
    {
	إذا (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	إذا (lf.lf_وزن < FW_BOLD)
	{
	    lf.lf_وزن = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    إذا (!lf.lfItalic)
	    {
		lf.lfItalic = TRUE;
		gui.wide_boldital_font = get_font_handle(&lf);
	    }
	}
    }
}

/*
 * Initialise vim to use the font with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset ناکارول_شوی)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    إذا (get_logfont(&lf, font_name, عدم, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lf_لوړوالی = adjust_fontsize_by_dpi(lf.lf_لوړوالی);
	font = get_font_handle(&lf);
    }
    إذا (font == NOFONT)
	أرجع FAIL;

#إذا defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	صحيح		    feat_شمېر = 0;
	char_u		    *fp;

	إذا (font_name != عدم)
	{
	    // Find each ":f" option in font_name.
	    لكل (fp = font_name; *fp != NUL; fp++)
	    {
		إذا (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u نښان[5];
		    صحيح    ti = 0;
		    بینښه صحيح param = 1;

		    fp += 2;  // skip ":f"
		    طالما (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			نښان[ti++] = *fp++;
		    نښان[ti] = NUL;

		    إذا (ti != 4)
			استمر;  // invalid tag length

		    إذا (*fp == '=')
		    {
			fp++;
			param = (بینښه صحيح)atoi((حرف *)fp);
			طالما (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    إذا (feat_شمېر < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_شمېر].نښان =
			    ((بینښه صحيح)نښان[0])
			    | ((بینښه صحيح)نښان[1] << 8)
			    | ((بینښه صحيح)نښان[2] << 16)
			    | ((بینښه صحيح)نښان[3] << 24);
			features[feat_شمېر].parameter = param;
			feat_شمېر++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWrite_Context_سېټ_Font_Features(s_dwc, features, feat_شمېر);
    }
#د_که_پای

    إذا (font_name == عدم)
	font_name = (char_u *)"";
#که_تعریف_شوی FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    إذا (!s_دننه_dpichanged)
	update_im_font();
#د_که_پای
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_لوړوالی = lfOrig.lf_لوړوالی;
    Update_Font_اندازه(font);

    p = logfont2name(lfOrig);
    إذا (p != عدم)
    {
	hl_سېټ_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	إذا (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	وإلا
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    إذا (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    إذا (lf.lf_وزن < FW_BOLD)
    {
	lf.lf_وزن = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	إذا (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    أرجع OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    صحيح
gui_mch_maximized(فراغ)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.اوږدتیا = حجم(WINDOWPLACEMENT);
    إذا (Get_کړکۍ_Placement(s_hwnd, &wp))
    {
	إذا (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    أرجع TRUE;
	إذا (wp.showCmd == SW_SHOWMINIMIZED)
	    أرجع FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	Get_کړکۍ_Rect(s_hwnd, &rc);
	إذا ((rc.ښی_2 - rc.کیڼ_2 !=
		    wp.rc_نورمال_Position.ښی_2 - wp.rc_نورمال_Position.کیڼ_2)
		|| (rc.bottom - rc.سر_2 !=
		    wp.rc_نورمال_Position.bottom - wp.rc_نورمال_Position.سر_2))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    فراغ
gui_mch_newfont(فراغ)
{
    RECT	rect;

    Get_کړکۍ_Rect(s_hwnd, &rect);
    إذا (win_socket_id == 0)
    {
	gui_resize_shell(rect.ښی_2 - rect.کیڼ_2
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.سر_2
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_لوړوالی(FALSE));
    }
    وإلا
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ښی_2 - rect.کیڼ_2,
	    rect.bottom - rect.سر_2 - gui_mswin_get_menu_لوړوالی(FALSE));
    }
}

/*
 * Set the window title
 */
    فراغ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon ناکارول_شوی)
{
    سېټ_کړکۍ_title(s_hwnd, (title == عدم ? "VIM" : (حرف *)title));
}

#إذا defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساكن LPCSTR mshape_idcs[] =
{
    IDC_ARROW,			// arrow
    MAKEINTRESOURCE(0),		// blank
    IDC_IBEAM,			// beam
    IDC_SIZENS,			// updown
    IDC_SIZENS,			// udsizing
    IDC_SIZEWE,			// leftright
    IDC_SIZEWE,			// lrsizing
    IDC_WAIT,			// busy
    IDC_NO,			// no
    IDC_ARROW,			// crosshair
    IDC_ARROW,			// hand1
    IDC_ARROW,			// hand2
    IDC_ARROW,			// pencil
    IDC_ARROW,			// question
    IDC_ARROW,			// right-arrow
    IDC_UPARROW,		// up-arrow
    IDC_ARROW			// last one
};

    فراغ
mch_سېټ_mouse_shape(صحيح shape)
{
    LPCSTR idc;

    إذا (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	وإلا
	    idc = mshape_idcs[shape];
	سېټ_Class_Long_Ptr(s_متن_Area, GCLP_HCURSOR, (LONG_PTR)بارول_Cursor(عدم, idc));
	إذا (!p_mh)
	{
	    ټکی_2 mp;

	    // Set the position to make it redrawn with the new shape.
	    (فراغ)GetCursorPos(&mp);
	    (فراغ)سېټ_Cursor_Pos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#د_که_پای

#إذا defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ساكن WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    صحيح len;
    WCHAR *res;

    tmp = convert_filter(s);
    إذا (tmp == عدم)
	أرجع عدم;
    len = (صحيح)STRLEN(s) + 3;
    res = enc_منزل_utf16(tmp, &len);
    vim_free(tmp);
    أرجع res;
}

/*
 * Pop open a file browser and return the file selected, in allocated memory,
 * or NULL if Cancel is hit.
 *  saving  - TRUE if the file will be saved to, FALSE if it will be opened.
 *  title   - Title message for the file browser dialog.
 *  dflt    - Default name of file.
 *  ext     - Default extension to be added to files without extensions.
 *  initdir - directory in which to open the browser (NULL = current dir)
 *  filter  - Filter for matched files to choose from.
 */
    char_u *
gui_mch_browse(
	صحيح saving,
	char_u *title,
	char_u *dflt,
	char_u *ext,
	char_u *initdir,
	char_u *filter)
{
    // We always use the wide function.  This means enc_to_utf16() must work,
    // otherwise it fails miserably!
    OPENFILENAMEW	fileStruct;
    WCHAR		fileBuf[MAXPATHL];
    WCHAR		*wp;
    صحيح			i;
    WCHAR		*titlep = عدم;
    WCHAR		*extp = عدم;
    WCHAR		*initdirp = عدم;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    إذا (dflt == عدم)
	fileBuf[0] = NUL;
    وإلا
    {
	wp = enc_منزل_utf16(dflt, عدم);
	إذا (wp == عدم)
	    fileBuf[0] = NUL;
	وإلا
	{
	    لكل (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_ډګر(fileStruct);
# ifdef OPENFILENAME_اندازه_نسخه_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_اندازه = OPENFILENAME_اندازه_نسخه_400_W;
# وإلا
    fileStruct.l_Struct_اندازه = حجم(fileStruct);
# endif

    إذا (title != عدم)
	titlep = enc_منزل_utf16(title, عدم);
    fileStruct.lpstrTitle = titlep;

    إذا (ext != عدم)
	extp = enc_منزل_utf16(ext, عدم);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    إذا (initdir != عدم && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_منزل_utf16(initdir, عدم);
	إذا (initdirp != عدم)
	{
	    لكل (wp = initdirp; *wp != NUL; ++wp)
		إذا (*wp == '/')
		    *wp = '\\';
	}
	fileStruct.lpstrInitialDir = initdirp;
    }

    /*
     * TODO: Allow selection of multiple files.  Needs another arg to this
     * function to ask for it, and need to use OFN_ALLOWMULTISELECT below.
     * Also, should we use OFN_FILEMUSTEXIST when opening?  Vim can edit on
     * files that don't exist yet, so I haven't put it in.  What about
     * OFN_PATHMUSTEXIST?
     * Don't use OFN_OVERWRITEPROMPT, Vim has its own ":confirm" dialog.
     */
    fileStruct.Flags = (OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY);
# ifdef FEAT_SHORTCUT
    إذا (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    إذا (saving)
	ret = GetSaveFileNameW(&fileStruct);
    وإلا
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    إذا (!ret)
	أرجع عدم;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_منزل_enc(fileBuf, عدم);
    إذا (p == عدم)
	أرجع عدم;

    // Give focus back to main window (when using MDI).
    سېټ_Focus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    أرجع q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ساكن char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    بینښه	s_len = (بینښه)STRLEN(s);
    بینښه	i;

    res = alloc(s_len + 3);
    إذا (res != عدم)
    {
	لكل (i = 0; i < s_len; ++i)
	    إذا (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    وإلا
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    أرجع res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    أرجع gui_mch_browse(0, title, (char_u *)_("Not Used"), عدم,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#د_که_پای // FEAT_BROWSE

    ساكن فراغ
_OnDropFiles(
    HWND hwnd ناکارول_شوی,
    HDROP hDrop)
{
#تعریف_کړه BUFPATHLEN MAX_لار_2
#تعریف_کړه DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    حرف    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, عدم, 0);
    UINT    i;
    char_u  **fnames;
    ټکی_2   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    Drag_Query_ټکی(hDrop, &pt);
    Map_کړکۍ_Points(s_hwnd, s_متن_Area, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    إذا (fnames != عدم)
	لكل (i = 0; i < cFiles; ++i)
	{
	    إذا (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_منزل_enc(wszFile, عدم);
	    وإلا
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    Drag_پای(hDrop);

    إذا (fnames == عدم)
	أرجع;

    صحيح kbd_modifiers = get_فعال_modifiers();

    إذا ((kbd_modifiers & MOD_ماسک_ښويدنه) != 0)
	modifiers |= MOUSE_ښويدنه;
    إذا ((kbd_modifiers & MOD_ماسک_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    إذا ((kbd_modifiers & MOD_ماسک_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ساكن صحيح
_OnScroll(
    HWND hwnd ناکارول_شوی,
    HWND hwndCtl,
    UINT کوډ,
    صحيح pos)
{
    ساكن UINT	prev_کوډ = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    اوږد	val;
    صحيح		dragging = FALSE;
    صحيح		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_اندازه = حجم(si);
    si.f_ماسک = SIF_POS;

    sb = gui_mswin_پیدا_کړه_scrollbar(hwndCtl);
    إذا (sb == عدم)
	أرجع 0;

    إذا (sb->wp != عدم)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    وإلا	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->ارزښت;

    بدل (کوډ)
    {
	حالة SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    إذا (sb->scroll_ښويدنه > 0)
		val <<= sb->scroll_ښويدنه;
	    اكسر;
	حالة SB_LINEDOWN:
	    val++;
	    اكسر;
	حالة SB_LINEUP:
	    val--;
	    اكسر;
	حالة SB_PAGEDOWN:
	    val += (sb_info->اندازه > 2 ? sb_info->اندازه - 2 : 1);
	    اكسر;
	حالة SB_PAGEUP:
	    val -= (sb_info->اندازه > 2 ? sb_info->اندازه - 2 : 1);
	    اكسر;
	حالة SB_سر:
	    val = 0;
	    اكسر;
	حالة SB_BOTTOM:
	    val = sb_info->max;
	    اكسر;
	حالة SB_ENDSCROLL:
	    إذا (prev_کوډ == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		إذا (sb->scroll_ښويدنه > 0)
		    val <<= sb->scroll_ښويدنه;
	    }
	    اكسر;

	افتراضي:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    أرجع 0;
    }
    prev_کوډ = کوډ;

    si.nPos = (sb->scroll_ښويدنه > 0) ? val >> sb->scroll_ښويدنه : val;
    سېټ_Scroll_Info(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    إذا (sb->wp != عدم)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_کیڼ) ? SBAR_ښی : SBAR_کیڼ].id;

	سېټ_Scroll_Info(id, SB_CTL, &si, TRUE);
    }

    // Don't let us be interrupted here by another message.
    s_busy_processing = TRUE;

    // When "allow_scrollbar" is FALSE still need to remember the new
    // position, but don't actually scroll by setting "dont_scroll".
    dont_scroll = !allow_scrollbar;

    mch_disable_flush();
    gui_drag_scrollbar(sb, val, dragging);
    mch_enable_flush();
    gui_may_flush();

    s_busy_processing = FALSE;
    dont_scroll = dont_scroll_save;

    أرجع 0;
}


#که_تعریف_شوی FEAT_XPM_W32
# include "xpm_w32.h"
#د_که_پای


// Some parameters for tearoff menus.  All in pixels.
#تعریف_کړه TEAROFF_PADDING_X	2
#تعریف_کړه TEAROFF_BUTTON_PAD_X	8
#تعریف_کړه TEAROFF_MIN_پلنوالی	200
#تعریف_کړه TEAROFF_SUBMENU_LABEL	">>"
#تعریف_کړه TEAROFF_ستنه_PADDING	3	// # spaces to pad column with.


#که_تعریف_شوی FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_متن_LEN	    MAXPATHL

ساكن BalloonEval  *cur_beval = عدم;
ساكن UINT_PTR	    beval_timer_id = 0;
ساكن DWORD	    وروستی_user_activity = 0;
#د_که_پای // defined(FEAT_BEVAL_GUI)


// Local variables:

#که_تعریف_شوی FEAT_MENU
ساكن UINT	s_menu_id = 100;
#د_که_پای

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#تعریف_کړه USE_SYSMENU_FONT

#تعریف_کړه VIM_NAME	"vim"
#تعریف_کړه VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#تعریف_کړه DLG_ALLOC_اندازه 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
ساكن PWORD
جمع_کړه_dialog_element(
	PWORD p,
	DWORD lStyle,
	کلمه_2 x,
	کلمه_2 y,
	کلمه_2 w,
	کلمه_2 h,
	کلمه_2 Id,
	کلمه_2 clss,
	ثابت حرف *caption);
ساكن LPWORD lpwAlign(LPWORD);
ساكن صحيح n_Copy_Ansi_منزل_Wide_Char(LPWORD, LPSTR, BOOL);
#إذا defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ساكن فراغ gui_mch_tearoff(char_u *title, vimmenu_T *menu, صحيح initX, صحيح initY);
#د_که_پای
ساكن فراغ get_dialog_font_metrics(فراغ);

ساكن صحيح dialog_default_button = -1;

#که_تعریف_شوی FEAT_TOOLBAR
ساكن فراغ initialise_toolbar(فراغ);
ساكن فراغ update_toolbar_اندازه(فراغ);
ساكن LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_پیغام, WPARAM wParam, LPARAM lParam);
ساكن صحيح get_toolbar_bitmap(vimmenu_T *menu);
#وإلا
# define update_toolbar_اندازه()
#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
ساكن فراغ initialise_tabline(فراغ);
ساكن LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_پیغام, WPARAM wParam, LPARAM lParam);
#د_که_پای

#که_تعریف_شوی FEAT_MBYTE_IME
ساكن LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ساكن char_u *Get_پایله_Str(HWND hwnd, صحيح GCS, صحيح *lenp);
#د_که_پای
#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
تعريفنوع بنية نښان_COMPOSITIONFORM {
    DWORD dwStyle;
    ټکی_2 ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
تعريفنوع HANDLE HIMC;
# endif

ساكن HINSTANCE hLibImm = عدم;
ساكن LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ساكن HIMC (WINAPI *pImmGetContext)(HWND);
ساكن HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ساكن BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ساكن BOOL (WINAPI *p_Imm_Get_Open_حالت)(HIMC);
ساكن BOOL (WINAPI *p_Imm_سېټ_Open_حالت)(HIMC, BOOL);
ساكن BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ساكن BOOL (WINAPI *p_Imm_سېټ_Composition_Font_W)(HIMC, LPLOGFONTW);
ساكن BOOL (WINAPI *p_Imm_سېټ_Composition_کړکۍ)(HIMC, LPCOMPOSITIONFORM);
ساكن BOOL (WINAPI *p_Imm_Get_Conversion_حالت)(HIMC, LPDWORD, LPDWORD);
ساكن BOOL (WINAPI *p_Imm_سېټ_Conversion_حالت)(HIMC, DWORD, DWORD);
ساكن فراغ dyn_imm_بارول(فراغ);
#وإلا
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define p_Imm_Get_Open_حالت	  Imm_Get_Open_حالت
# define p_Imm_سېټ_Open_حالت	  Imm_سېټ_Open_حالت
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define p_Imm_سېټ_Composition_Font_W  Imm_سېټ_Composition_Font_W
# define p_Imm_سېټ_Composition_کړکۍ Imm_سېټ_Composition_کړکۍ
# define p_Imm_Get_Conversion_حالت  Imm_Get_Conversion_حالت
# define p_Imm_سېټ_Conversion_حالت  Imm_سېټ_Conversion_حالت
#د_که_پای

#که_تعریف_شوی FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    ساكن صحيح
gui_mswin_get_menu_لوړوالی(
    صحيح	    fix_کړکۍ)	    // If TRUE, resize window if menu height changed
{
    ساكن صحيح	old_menu_لوړوالی = -1;

    RECT    rc1, rc2;
    صحيح	    عدد;
    صحيح	    menu_لوړوالی;

    إذا (gui.menu_is_فعال)
	عدد = Get_Menu_توکی_شمېر(s_menuBar);
    وإلا
	عدد = 0;

    إذا (عدد == 0)
	menu_لوړوالی = 0;
    وإلا إذا (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_لوړوالی = old_menu_لوړوالی == -1 ? 0 : old_menu_لوړوالی;
    }
    وإلا
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	Get_Menu_توکی_Rect(s_hwnd, s_menuBar, 0, &rc1);
	إذا (gui.starting)
	    menu_لوړوالی = rc1.bottom - rc1.سر_2 + 1;
	وإلا
	{
	    Get_Menu_توکی_Rect(s_hwnd, s_menuBar, عدد - 1, &rc2);
	    menu_لوړوالی = rc2.bottom - rc1.سر_2 + 1;
	}
    }

    إذا (fix_کړکۍ && menu_لوړوالی != old_menu_لوړوالی)
	gui_سېټ_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_لوړوالی = menu_لوړوالی;

    أرجع menu_لوړوالی;
}
#د_که_پای // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ساكن اوږد
mouse_vertical_scroll_ګام(فراغ)
{
    UINT val;
    إذا (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	أرجع (val != WHEEL_PAGESCROLL) ? (اوږد)val : -1;
    أرجع 3; // Safe default;
}

    ساكن اوږد
mouse_horizontal_scroll_ګام(فراغ)
{
    UINT val;
    إذا (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	أرجع (اوږد)val;
    أرجع 3; // Safe default;
}

    ساكن فراغ
init_mouse_wheel(فراغ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_سېټ_vert_scroll_ګام(mouse_vertical_scroll_ګام());
    mouse_سېټ_hor_scroll_ګام(mouse_horizontal_scroll_ګام());
}

/*
 * Mouse scroll event handler.
 */
    ساكن فراغ
_OnMouseWheel(HWND hwnd ناکارول_شوی, WPARAM wParam, LPARAM lParam, صحيح horizontal)
{
    صحيح		button;
    win_T	*wp;
    صحيح		modifiers = 0;
    صحيح		kbd_modifiers;
    صحيح		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    ټکی_2	pt;

    wp = gui_mouse_کړکۍ(پیدا_کړه_POPUP);

#که_تعریف_شوی FEAT_PROP_POPUP
    إذا (wp != عدم && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_لیکه = wp->w_winrow;
	mouse_ستون = wp->w_wincol;
	CLEAR_ډګر(cap);
	إذا (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_کیڼ : MSCR_ښی;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	وإلا
	{
	    cap.arg = zDelta < 0 ? MSCR_پورته : MSCR_ښکته;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	بهر_flush();
    }
#د_که_پای

    إذا (wp == عدم || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    إذا (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    وإلا
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_فعال_modifiers();

    إذا ((kbd_modifiers & MOD_ماسک_ښويدنه) != 0)
	modifiers |= MOUSE_ښويدنه;
    إذا ((kbd_modifiers & MOD_ماسک_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    إذا ((kbd_modifiers & MOD_ماسک_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    Screen_منزل_Client(s_متن_Area, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#که_تعریف_شوی USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    ساكن صحيح
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_اندازه = حجم(NONCLIENTMETRICSW);
    إذا (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    حجم(NONCLIENTMETRICSW),
	    &nm,
	    0))
	أرجع FAIL;
    *lf = nm.lfMenuFont;
    أرجع OK;
}
#د_که_پای


#إذا defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ساكن فراغ
سېټ_tabline_font(فراغ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    إذا (gui_w32_get_menu_font(&lfSysmenu) != OK)
	أرجع;

    lfSysmenu.lf_لوړوالی = adjust_fontsize_by_dpi(lfSysmenu.lf_لوړوالی);
    font = Create_Font_غیرمستقیم_W(&lfSysmenu);

    Send_پیغام(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = Get_Desktop_کړکۍ();
    hdc = Get_کړکۍ_DC(hwnd);
    hfntOld = SelectFont(hdc, font);

    Get_متن_Metrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_لوړوالی = tm.tm_لوړوالی + tm.tm_دننی_Leading + 7;
}
#وإلا
# define سېټ_tabline_font()
#د_که_پای

/*
 * Invoked when a setting was changed.
 */
    ساكن LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    بدل (param)
    {
	حالة SPI_SETWHEELSCROLLLINES:
	    mouse_سېټ_vert_scroll_ګام(mouse_vertical_scroll_ګام());
	    اكسر;
	حالة SPI_SETWHEELSCROLLCHARS:
	    mouse_سېټ_hor_scroll_ګام(mouse_horizontal_scroll_ګام());
	    اكسر;
	حالة SPI_SETNONCLIENTMETRICS:
	    سېټ_tabline_font();
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع 0;
}

#که_تعریف_شوی FEAT_NETBEANS_INTG
    ساكن فراغ
On_کړکۍ_Pos_بدل_شوی(
    HWND hwnd,
    ثابت LPWINDOWPOS lpwpos)
{
    ساكن صحيح x = 0, y = 0, cx = 0, cy = 0;
    خارجي صحيح WSInitialized;

    إذا (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_چوکاټ_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, Def_کړکۍ_Proc_W);
}
#د_که_پای


ساكن HWND hwndTip = عدم;

    ساكن فراغ
show_sizing_tip(صحيح ستون_شمېر, صحيح لیکې_2)
{
    TOOLINFOA ti;
    حرف buf[32];

    ti.cb_اندازه = حجم(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpsz_متن = buf;
    sprintf(buf, "%dx%d", ستون_شمېر, لیکې_2);
    إذا (hwndTip == عدم)
    {
	hwndTip = Create_کړکۍ_Ex_A(0, TOOLTIPS_CLASSA, عدم,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, عدم, GetModuleHandle(عدم), عدم);
	Send_پیغام(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	Send_پیغام(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    وإلا
    {
	Send_پیغام(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    Send_پیغام(hwndTip, TTM_POPUP, 0, 0);
}

    ساكن فراغ
destroy_sizing_tip(فراغ)
{
    إذا (hwndTip == عدم)
	أرجع;

    Destroy_کړکۍ(hwndTip);
    hwndTip = عدم;
}

    ساكن صحيح
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    صحيح	    w, h;
    صحيح	    valid_w, valid_h;
    صحيح	    w_دننه_واټن, h_دننه_واټن;
    صحيح	    ستون_شمېر, لیکې_2;

    w = lprc->ښی_2 - lprc->کیڼ_2;
    h = lprc->bottom - lprc->سر_2;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &ستون_شمېر, &لیکې_2);
    w_دننه_واټن = w - valid_w;
    h_دننه_واټن = h - valid_h;

    إذا (fwSide == WMSZ_کیڼ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->کیڼ_2 += w_دننه_واټن;
    وإلا إذا (fwSide == WMSZ_ښی || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ښی_2 -= w_دننه_واټن;

    إذا (fwSide == WMSZ_سر || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->سر_2 += h_دننه_واټن;
    وإلا إذا (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_دننه_واټن;

    show_sizing_tip(ستون_شمېر, لیکې_2);
    أرجع TRUE;
}

#که_تعریف_شوی FEAT_GUI_TABLINE
    ساكن فراغ
On_RButton_پورته(HWND hwnd, صحيح x, صحيح y, UINT کلی_Flags)
{
    إذا (gui_mch_showing_tabline())
    {
	ټکی_2 pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	Get_کړکۍ_Rect(s_متن_Area, &rect);
	إذا (pt.y < rect.سر_2)
	{
	    show_tabline_popup_menu();
	    أرجع;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, کلی_Flags, Def_کړکۍ_Proc_W);
}

    ساكن فراغ
On_LButton_ښکته(HWND hwnd, BOOL fDoubleClick, صحيح x, صحيح y, UINT کلی_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    إذا (gui_mch_showing_tabline())
    {
	ټکی_2 pt;
	RECT rect;

	GetCursorPos(&pt);
	Get_کړکۍ_Rect(s_متن_Area, &rect);
	إذا (pt.y < rect.سر_2)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, کلی_Flags, Def_کړکۍ_Proc_W);
}
#د_که_پای

    ساكن UINT
_OnNCHitTest(HWND hwnd, صحيح xPos, صحيح yPos)
{
    UINT	پایله;
    صحيح		x, y;

    پایله = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, Def_کړکۍ_Proc_W);
    إذا (پایله != HTCLIENT)
	أرجع پایله;

#که_تعریف_شوی FEAT_GUI_TABLINE
    إذا (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	Get_کړکۍ_Rect(s_متن_Area, &rct);
	إذا (yPos < rct.سر_2)
	    أرجع پایله;
    }
#د_که_پای
    (فراغ)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    إذا (xPos < 48) // <VN> TODO should use system metric?
	أرجع HTBOTTOMLEFT;
    وإلا
	أرجع HTBOTTOMRIGHT;
}

#إذا defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساكن LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    بدل (hdr->کوډ)
    {
	حالة TTN_GETDISPINFOW:
	حالة TTN_GETDISPINFO:
	    {
		char_u		*str = عدم;
		ساكن فراغ	*tt_متن = عدم;

		VIM_CLEAR(tt_متن);

# ifdef FEAT_GUI_TABLINE
		إذا (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    ټکی_2	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    إذا (Screen_منزل_Client(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			صحيح idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = Tab_Ctrl_موندنه_Test(s_tabhwnd, &htinfo);
			إذا (idx != -1)
			{
			    tabpage_T *tp;

			    tp = پیدا_کړه_tabpage(idx + 1);
			    إذا (tp != عدم)
			    {
				get_tabline_label(tp, TRUE);
				str = NameBuff;
			    }
			}
		    }
		}
# endif
# ifdef FEAT_TOOLBAR
#  ifdef FEAT_GUI_TABLINE
		وإلا
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_پیدا_کړه_menu(ریښه_menu, idButton);
		    إذا (pMenu)
			str = pMenu->مزي_ګانې[MENU_شاخص_TIP];
		}
# endif
		إذا (str == عدم)
		    اكسر;

		// Set the maximum width, this also enables using \n for
		// line break.
		Send_پیغام(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		إذا (hdr->کوډ == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_متن = enc_منزل_utf16(str, عدم);
		    lpdi->lpsz_متن = tt_متن;
		    // can't show tooltip if failed
		}
		وإلا
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    إذا (len < حجم(lpdi->sz_متن)
			    || ((tt_متن = vim_strnsave(str, len)) == عدم))
			vim_strncpy((char_u *)lpdi->sz_متن, str,
				حجم(lpdi->sz_متن) - 1);
		    وإلا
			lpdi->lpsz_متن = tt_متن;
		}
	    }
	    اكسر;

# ifdef FEAT_GUI_TABLINE
	حالة TCN_SELCHANGE:
	    إذا (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		أرجع 0L;
	    }
	    اكسر;

	حالة NM_RCLICK:
	    إذا (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		أرجع 0L;
	    }
	    اكسر;
# endif

	افتراضي:
	    اكسر;
    }
    أرجع Def_کړکۍ_Proc_W(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#د_که_پای

#إذا defined(MENUHINTS) && defined(FEAT_MENU)
    ساكن LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    إذا (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & ډېرپېښ_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ساكن صحيح  did_menu_tip = FALSE;

	إذا (did_menu_tip)
	{
	    پیغام_clr_cmdline();
	    setcursor();
	    بهر_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_پیدا_کړه_menu(ریښه_menu, (UINT)LOWORD(wParam));
	إذا (pMenu != عدم && pMenu->مزي_ګانې[MENU_شاخص_TIP] != عدم)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_اندازه = حجم(MENUITEMINFO);
	    menuinfo.f_ماسک = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    إذا (Get_Menu_توکی_Info(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++پیغام_hist_off;
		پیغام((حرف *)pMenu->مزي_ګانې[MENU_شاخص_TIP]);
		--پیغام_hist_off;
		setcursor();
		بهر_flush();
		did_menu_tip = TRUE;
	    }
	}
	أرجع 0L;
    }
    أرجع Def_کړکۍ_Proc_W(hwnd, WM_MENUSELECT, wParam, lParam);
}
#د_که_پای

    ساكن BOOL
On_Get_Dpi_Scaled_اندازه(HWND hwnd ناکارول_شوی, UINT dpi, اندازه_3 *اندازه)
{
    صحيح		old_پلنوالی, old_لوړوالی;
    صحيح		new_پلنوالی, new_لوړوالی;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_اندازه(gui.norm_font, &old_پلنوالی, &old_لوړوالی);    // Current size
    GetObjectW((HFONT)gui.norm_font, حجم(lf), &lf);
    lf.lf_لوړوالی = lf.lf_لوړوالی * (صحيح)dpi / s_dpi;
    font = Create_Font_غیرمستقیم_W(&lf);
    إذا (font)
    {
	Get_Font_اندازه((GuiFont)font, &new_پلنوالی, &new_لوړوالی);	// New size
	DeleteFont(font);
    }
    وإلا
    {
	new_پلنوالی = old_پلنوالی;
	new_لوړوالی = old_لوړوالی;
    }
    اندازه->cx = اندازه->cx * new_پلنوالی / old_پلنوالی;
    اندازه->cy = اندازه->cy * new_لوړوالی / old_لوړوالی;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    أرجع TRUE;
}

    ساكن LRESULT
On_Dpi_بدل_شوی(HWND hwnd, UINT xdpi ناکارول_شوی, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_دننه_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_اندازه();
    update_toolbar_اندازه();
    سېټ_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_لوړوالی(FALSE);
#که_تعریف_شوی FEAT_MBYTE_IME
    im_سېټ_position(gui.لیکه, gui.ستون);
#د_که_پای
    InvalidateRect(hwnd, عدم, TRUE);

    s_دننه_dpichanged = FALSE;
    أرجع 0L;
}


    ساكن LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_پیغام, lParam);

    s_u_پیغام = u_پیغام;
    s_wParam = wParam;

    بدل (u_پیغام)
    {
	HANDLE_پیغام(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_پیغام(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_پیغام(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_پیغام(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_پیغام(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_پیغام(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_پیغام(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#که_تعریف_شوی FEAT_MENU
	HANDLE_پیغام(hwnd, WM_COMMAND,	_OnMenu);
#د_که_پای
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_پیغام(hwnd, WM_SETFOCUS,	On_سېټ_Focus);
	HANDLE_پیغام(hwnd, WM_اندازه,	On_اندازه);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_پیغام(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_پیغام(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#که_تعریف_شوی FEAT_NETBEANS_INTG
	HANDLE_پیغام(hwnd, WM_WINDOWPOSCHANGED, On_کړکۍ_Pos_بدل_شوی);
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
	HANDLE_پیغام(hwnd, WM_RBUTTONUP,	On_RButton_پورته);
	HANDLE_پیغام(hwnd, WM_LBUTTONDBLCLK,  On_LButton_ښکته);
#د_که_پای
	HANDLE_پیغام(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    حالة WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	أرجع FALSE;		// Do NOT allow system to go down.

    حالة WM_ENDSESSION:
	إذا (wParam)	// system only really goes down when wParam is TRUE
	{
	    On_پای_ټکی_Session();
	    أرجع 0L;
	}
	اكسر;

    حالة WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (صحيح)(لنډ)LOWORD(lParam));
	أرجع 0L;

    حالة WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#که_تعریف_شوی FEAT_MENU
	إذا (	!gui.menu_is_فعال
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((صحيح)wParam))
		)
#د_که_پای
	{
	    _OnSysChar(hwnd, (UINT)wParam, (صحيح)(لنډ)LOWORD(lParam));
	    أرجع 0L;
	}
#که_تعریف_شوی FEAT_MENU
	وإلا
	    أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
#د_که_پای

    حالة WM_SYSKEYUP:
#که_تعریف_شوی FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
#وإلا
	أرجع 0L;
#د_که_پای

    حالة WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	اكسر;

    حالة WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	أرجع _DuringSizing((UINT)wParam, (LPRECT)lParam);

    حالة WM_MOUSEWHEEL:
    حالة WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_پیغام == WM_MOUSEHWHEEL);
	أرجع 0L;

	// Notification for change in SystemParametersInfo()
    حالة WM_SETTINGCHANGE:
	أرجع _OnSettingChange((UINT)wParam);

#إذا defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    حالة WM_NOTIFY:
	أرجع _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#د_که_پای

#إذا defined(MENUHINTS) && defined(FEAT_MENU)
    حالة WM_MENUSELECT:
	أرجع _OnMenuSelect(hwnd, wParam, lParam);
#د_که_پای

#که_تعریف_شوی FEAT_MBYTE_IME
    حالة WM_IME_NOTIFY:
	إذا (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
	أرجع 1L;

    حالة WM_IME_COMPOSITION:
	إذا (!_OnImeComposition(hwnd, wParam, lParam))
	    أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
	أرجع 1L;
#د_که_پای
    حالة WM_GETDPISCALEDSIZE:
	أرجع On_Get_Dpi_Scaled_اندازه(hwnd, (UINT)wParam, (اندازه_3 *)lParam);
    حالة WM_DPICHANGED:
	أرجع On_Dpi_بدل_شوی(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    افتراضي:
#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
	إذا (u_پیغام == s_findrep_پیغام && s_findrep_پیغام != 0)
	    On_پیدا_کړه_Repl();
#د_که_پای
	اكسر;
    }

    أرجع Def_کړکۍ_Proc_W(hwnd, u_پیغام, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_پلار_hwnd = عدم;

    ساكن BOOL CALLBACK
پیدا_کړه_کړکۍ_Title(HWND hwnd, LPARAM lParam)
{
    حرف	buf[2048];
    حرف	*title = (حرف *)lParam;

    إذا (Get_کړکۍ_متن(hwnd, buf, حجم(buf)))
    {
	إذا (مزي_پلټنه(buf, title) != عدم)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_پلار_hwnd = پیدا_کړه_کړکۍ_Ex(hwnd, عدم, "MDIClient", عدم);
	    إذا (vim_پلار_hwnd != عدم)
		أرجع FALSE;
	}
    }
    أرجع TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    فراغ
gui_mch_سېټ_پلار(حرف *title)
{
    EnumWindows(پیدا_کړه_کړکۍ_Title, (LPARAM)title);
    إذا (vim_پلار_hwnd == عدم)
    {
	semsg(_(e_cannot_پیدا_کړه_کړکۍ_title_str), title);
	mch_exit(2);
    }
}

#که_نه_تعریف_شوی FEAT_OLE
    ساكن فراغ
ole_error(حرف *arg)
{
    حرف buf[IOSIZE];

# ifdef VIMDLL
    gui.دننه_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_دلیل_not_supported_str_use_ole_نسخه), arg);
    mch_errmsg(buf);
}
#د_که_پای

#إذا defined(GUI_MAY_SPAWN)
    ساكن حرف *
gvim_error(فراغ)
{
    حرف *پیغام = _(e_gui_cannot_be_کارول_شوی_cannot_execute_gvim_exe);

    إذا (starting)
    {
	mch_errmsg(پیغام);
	mch_errmsg("\n");
	mch_exit(2);
    }
    أرجع پیغام;
}

    حرف *
gui_mch_do_spawn(char_u *arg)
{
    صحيح			len;
# إذا defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = عدم;
    LPWSTR		tofree1 = عدم;
# endif
    WCHAR		name[MAX_لار];
    LPWSTR		cmd, newcmd = عدم, p, warg, tofree2 = عدم;
    STARTUPINFOW	si = {حجم(si)};
    PROCESS_INFORMATION pi;

    إذا (!GetModuleFileNameW(g_hinst, name, MAX_لار))
	اذهب error;
    p = wcsrchr(name, L'\\');
    إذا (p == عدم)
	اذهب error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# وإلا
    wcscpy(p + 1, L"gvim.exe");
# endif

# إذا defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    إذا (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_کرښه_W();
	// Skip 1st argument.
	طالما (*p && *p != L' ' && *p != L'\t')
	{
	    إذا (*p == L'"')
	    {
		// Skip quoted strings
		طالما (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# إذا defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    وإلا
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	صحيح	ret;

	session = vim_tempname('s', FALSE);
	إذا (session == عدم)
	    اذهب error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	إذا (p_bg == عدم)
	{
	    p_bg = savebg;
	    اذهب error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	إذا (!ret)
	    اذهب error;
	wsession = enc_منزل_utf16(session, عدم);
	إذا (wsession == عدم)
	    اذهب error;
	len = (صحيح)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	إذا (cmd == عدم)
	{
	    vim_free(wsession);
	    اذهب error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    إذا (arg != عدم)
    {
	warg = enc_منزل_utf16(arg, عدم);
	إذا (warg == عدم)
	    اذهب error;
	tofree2 = warg;
    }
    وإلا
	warg = L"";

    // Set up the new command line.
    len = (صحيح)wcslen(name) + (صحيح)wcslen(cmd) + (صحيح)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    إذا (newcmd == عدم)
	اذهب error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    إذا (!CreateProcessW(عدم, newcmd, عدم, عدم, TRUE, 0,
		عدم, عدم, &si, &pi))
	اذهب error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# إذا defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    إذا (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    أرجع gvim_error();
}
#د_که_پای

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    فراغ
gui_mch_prepare(صحيح *argc, حرف **argv)
{
    صحيح		silent = FALSE;
    صحيح		idx;

    // Check for special OLE command line parameters
    إذا ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	إذا (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	وإلا
	    idx = 1;

	// Register Vim as an OLE Automation server
	إذا (STRICMP(argv[idx] + 1, "register") == 0)
	{
#که_تعریف_شوی FEAT_OLE
	    ثبتوونکی_Me(silent);
	    mch_exit(0);
#وإلا
	    إذا (!silent)
		ole_error("register");
	    mch_exit(2);
#د_که_پای
	}

	// Unregister Vim as an OLE Automation server
	إذا (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#که_تعریف_شوی FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#وإلا
	    إذا (!silent)
		ole_error("unregister");
	    mch_exit(2);
#د_که_پای
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	إذا (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#که_تعریف_شوی FEAT_OLE
	    *argc = 1;
#وإلا
	    ole_error("embedding");
	    mch_exit(2);
#د_که_پای
	}
    }

#که_تعریف_شوی FEAT_OLE
# ifdef VIMDLL
    إذا (mch_is_gui_executable())
# endif
    {
	صحيح	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	إذا (bDoRestart)
	    mch_exit(0);
    }
#د_که_پای

#که_تعریف_شوی FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	صحيح arg;

	لكل (arg = 1; arg < *argc; arg++)
	    إذا (مزي_پرتله("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * حجم(حرف *));
		argv[*argc] = عدم;
		اكسر;	// enough?
	    }
    }
#د_که_پای
}

    ساكن فراغ
بارول_dpi_func(فراغ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    إذا (hUser32 == عدم)
	اذهب fail;

    pGetDpiForSystem = (UINT (WINAPI *)(فراغ))Get_Proc_پته(hUser32, "GetDpiForSystem");
    p_Get_Dpi_For_کړکۍ = (UINT (WINAPI *)(HWND))Get_Proc_پته(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (صحيح (WINAPI *)(صحيح, UINT))Get_Proc_پته(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    p_سېټ_Thread_Dpi_Awareness_Context = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_پته(hUser32, "SetThreadDpiAwarenessContext");
#که_تعریف_شوی FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_پته(hUser32, "GetAwarenessFromDpiAwarenessContext");
#د_که_پای

    إذا (p_سېټ_Thread_Dpi_Awareness_Context != عدم)
    {
	DPI_AWARENESS_CONTEXT oldctx = p_سېټ_Thread_Dpi_Awareness_Context(
		DPI_AWARENESS_CONTEXT_فی_MONITOR_AWARE_V2);
	إذا (oldctx != عدم)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#که_تعریف_شوی FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#د_که_پای
	    أرجع;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    p_Get_Dpi_For_کړکۍ = عدم;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    p_سېټ_Thread_Dpi_Awareness_Context = عدم;
#که_تعریف_شوی FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = عدم;
#د_که_پای
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحيح
gui_mch_init(فراغ)
{
    ثابت WCHAR szVimWndClassW[] = VIM_CLASSW;
    ثابت WCHAR sz_متن_Area_Class_W[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    إذا (s_hwnd != عدم)
	اذهب theend;

    /*
     * Load the tearoff bitmap
     */
#که_تعریف_شوی FEAT_TEAROFF
    s_htearbitmap = بارول_Bitmap(g_hinst, "IDB_TEAROFF");
#د_که_پای

    بارول_dpi_func();

#که_تعریف_شوی FEAT_GUI_DARKTHEME
    dyn_uxtheme_بارول();
#د_که_پای

    dyn_dwm_بارول();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_اندازه();

#که_تعریف_شوی FEAT_MENU
    gui.menu_لوړوالی = 0;	// Windows takes care of this
#د_که_پای
    gui.border_پلنوالی = 0;
#که_تعریف_شوی FEAT_TOOLBAR
    gui.toolbar_لوړوالی = TOOLBAR_BUTTON_لوړوالی + TOOLBAR_BORDER_لوړوالی;
#د_که_پای

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    إذا (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = بارول_Icon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = بارول_Cursor(عدم, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = عدم;
	wndclassw.lpszClassName = szVimWndClassW;

	إذا (ثبتوونکی_Class_W(&wndclassw) == 0)
	    أرجع FAIL;
    }

    إذا (vim_پلار_hwnd != عدم)
    {
#که_تعریف_شوی HAVE_TRY_EXCEPT
	__try
	{
#د_که_پای
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = Create_کړکۍ_Ex_W(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_ماشوم
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_پلار_hwnd, عدم,
		g_hinst, عدم);
#که_تعریف_شوی HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#د_که_پای
	إذا (s_hwnd == عدم)
	{
	    emsg(_(e_unable_منزل_open_کړکۍ_inside_mdi_application));
	    mch_exit(2);
	}
    }
    وإلا
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	إذا (Is_کړکۍ((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = Create_کړکۍ_W(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		عدم, عدم,
		g_hinst, عدم);
	إذا (s_hwnd != عدم && win_socket_id != 0)
	{
	    سېټ_پلار(s_hwnd, (HWND)win_socket_id);
	    Show_کړکۍ(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    إذا (s_hwnd == عدم)
	أرجع FAIL;

    إذا (p_Get_Dpi_For_کړکۍ != عدم)
    {
	s_dpi = p_Get_Dpi_For_کړکۍ(s_hwnd);
	update_scrollbar_اندازه();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_بارول();
#د_که_پای

    // Create the text area window
    إذا (GetClassInfoW(g_hinst, sz_متن_Area_Class_W, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = متن_Area_Wnd_Proc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = عدم;
	wndclassw.hCursor = بارول_Cursor(عدم, IDC_ARROW);
	wndclassw.hbrBackground = عدم;
	wndclassw.lpszMenuName = عدم;
	wndclassw.lpszClassName = sz_متن_Area_Class_W;

	إذا (ثبتوونکی_Class_W(&wndclassw) == 0)
	    أرجع FAIL;
    }

    s_متن_Area = Create_کړکۍ_Ex_W(
	0,
	sz_متن_Area_Class_W, L"Vim text area",
	WS_ماشوم | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, عدم,
	g_hinst, عدم);

    إذا (s_متن_Area == عدم)
	أرجع FAIL;

#که_تعریف_شوی FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = عدم;

	إذا (mch_icon_بارول(&hIcon) == OK && hIcon != عدم)
	    Send_پیغام(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#د_که_پای

#که_تعریف_شوی FEAT_MENU
    s_menuBar = CreateMenu();
#د_که_پای
    s_hdc = GetDC(s_متن_Area);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سېټ_نورمال_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_شا_pixel = gui.شا_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_دننه_واټن = gui.border_پلنوالی;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#که_تعریف_شوی FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#د_که_پای
#که_تعریف_شوی MSWIN_پیدا_کړه_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_پیغام = ثبتوونکی_کړکۍ_پیغام(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_اندازه = حجم(s_findrep_struct);
    s_findrep_struct.lpstr_پیدا_کړه_What = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstr_پیدا_کړه_What[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.w_پیدا_کړه_What_Len = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
    // set the v:windowid variable
    سېټ_vim_var_nr(VV_WINDOWID, Handle_منزل_Long(s_hwnd));
#د_که_پای

#که_تعریف_شوی FEAT_RENDER_OPTIONS
    إذا (p_rop)
	(فراغ)gui_mch_سېټ_rendering_options(p_rop);
#د_که_پای

theend:
    // Display any pending error messages
    display_errors();

    أرجع OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ساكن فراغ
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = Monitor_From_کړکۍ(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    إذا (mon != عدم)
    {
	moninfo.cb_اندازه = حجم(MONITORINFO);
	إذا (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    أرجع;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    فراغ
gui_mch_سېټ_shellsize(
	صحيح پلنوالی,
	صحيح لوړوالی,
	صحيح min_پلنوالی ناکارول_شوی,
	صحيح min_لوړوالی ناکارول_شوی,
	صحيح بنسټ_پلنوالی ناکارول_شوی,
	صحيح بنسټ_لوړوالی ناکارول_شوی,
	صحيح direction)
{
    RECT	workarea_rect;
    RECT	کړکۍ_rect;
    صحيح		win_پلنوالی, win_لوړوالی;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    إذا (IsZoomed(s_hwnd) && starting == 0)
	Show_کړکۍ(s_hwnd, SW_SHOWNORMAL);

    إذا (s_دننه_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	کړکۍ_rect = s_suggested_rect;
    وإلا
	// Use current position.
	Get_کړکۍ_Rect(s_hwnd, &کړکۍ_rect);

    // compute the size of the outside of the window
    win_پلنوالی = پلنوالی + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_لوړوالی = لوړوالی + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_لوړوالی(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    کړکۍ_rect.ښی_2 = کړکۍ_rect.کیڼ_2 + win_پلنوالی;
    کړکۍ_rect.bottom = کړکۍ_rect.سر_2 + win_لوړوالی;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    إذا (!s_دننه_dpichanged)
    {
	إذا ((direction & RESIZE_HOR)
		&& کړکۍ_rect.ښی_2 > workarea_rect.ښی_2)
	    دننه_واټن_Rect(&کړکۍ_rect,
		    workarea_rect.ښی_2 - کړکۍ_rect.ښی_2, 0);

	إذا ((direction & RESIZE_HOR)
		&& کړکۍ_rect.کیڼ_2 < workarea_rect.کیڼ_2)
	    دننه_واټن_Rect(&کړکۍ_rect,
		    workarea_rect.کیڼ_2 - کړکۍ_rect.کیڼ_2, 0);

	إذا ((direction & RESIZE_VERT)
		&& کړکۍ_rect.bottom > workarea_rect.bottom)
	    دننه_واټن_Rect(&کړکۍ_rect,
		    0, workarea_rect.bottom - کړکۍ_rect.bottom);

	إذا ((direction & RESIZE_VERT)
		&& کړکۍ_rect.سر_2 < workarea_rect.سر_2)
	    دننه_واټن_Rect(&کړکۍ_rect,
		    0, workarea_rect.سر_2 - کړکۍ_rect.سر_2);
    }

    Move_کړکۍ(s_hwnd, کړکۍ_rect.کیڼ_2, کړکۍ_rect.سر_2,
						win_پلنوالی, win_لوړوالی, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    سېټ_فعال_کړکۍ(s_hwnd);
    سېټ_Focus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_لوړوالی(!gui.starting);
}


    فراغ
gui_mch_سېټ_scrollbar_thumb(
    scrollbar_T *sb,
    اوږد	val,
    اوږد	اندازه,
    اوږد	max)
{
    SCROLLINFO	info;

    sb->scroll_ښويدنه = 0;
    طالما (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	اندازه >>= 1;
	++sb->scroll_ښويدنه;
    }

    إذا (sb->scroll_ښويدنه > 0)
	++اندازه;

    info.cb_اندازه = حجم(info);
    info.f_ماسک = SIF_POS | SIF_RANGE | SIF_پاڼه;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.n_پاڼه = اندازه;
    سېټ_Scroll_Info(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    فراغ
gui_mch_سېټ_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_سېټ_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_سېټ_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سېټ_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#که_تعریف_شوی FEAT_MBYTE_IME
/*
 * Multi-byte handling, originally by Sung-Hoon Baek.
 * First static functions (no prototypes generated).
 */
# ifdef _MSC_VER
#  include <ime.h>   // Apparently not needed for Cygwin or MinGW.
# endif
# include <imm.h>

/*
 * handle WM_IME_NOTIFY message
 */
    ساكن LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_معلومات ناکارول_شوی)
{
    LRESULT l_پایله = 0;
    HIMC hImc;

    إذا (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	أرجع l_پایله;
    بدل (dwCommand)
    {
	حالة IMN_SETOPENSTATUS:
	    إذا (p_Imm_Get_Open_حالت(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lf_لوړوالی = lf.lf_لوړوالی * DEFAULT_DPI / s_dpi;
		p_Imm_سېټ_Composition_Font_W(hImc, &lf);
		im_سېټ_position(gui.لیکه, gui.ستون);

		// Disable langmap
		State &= ~ډېرپېښ_LANGMAP;
		إذا (State & ډېرپېښ_ورننباسه)
		{
# إذا defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			صحيح old_لیکه = gui.لیکه;
			صحيح old_ستون = gui.ستون;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			حالت_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.لیکه = old_لیکه;
			gui.ستون = old_ستون;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_پایله = 0;
	    اكسر;
    }
    pImmReleaseContext(hWnd, hImc);
    أرجع l_پایله;
}

    ساكن LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs ناکارول_شوی, LPARAM param)
{
    char_u	*ret;
    صحيح		len;

    إذا ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	أرجع 0;

    ret = Get_پایله_Str(hwnd, GCS_RESULTSTR, &len);
    إذا (ret != عدم)
    {
	جمع_کړه_منزل_ننوت_buf_csi(ret, len);
	vim_free(ret);
	أرجع 1;
    }
    أرجع 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ساكن char_u *
Get_پایله_Str(HWND hwnd, صحيح GCS, صحيح *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = عدم;
    char_u	*convbuf = عدم;

    إذا (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	أرجع عدم;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, عدم, 0);
    إذا (ret <= 0)
	أرجع عدم;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + حجم(WCHAR));
    إذا (buf == عدم)
	أرجع عدم;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / حجم(WCHAR);

    convbuf = utf16_منزل_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    أرجع convbuf;
}
#د_که_پای

// For global functions we need prototypes.
#إذا defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    فراغ
im_سېټ_font(LOGFONTW *lf)
{
    HIMC hImc;

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	p_Imm_سېټ_Composition_Font_W(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    فراغ
im_سېټ_position(صحيح لیکه, صحيح ستون)
{
    HIMC hImc;

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_ټکی;
	cfs.ptCurrentPos.x = FILL_X(ستون);
	cfs.ptCurrentPos.y = FILL_Y(لیکه);
	Map_کړکۍ_Points(s_متن_Area, s_hwnd, &cfs.ptCurrentPos, 1);
	إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	p_Imm_سېټ_Composition_کړکۍ(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    فراغ
im_سېټ_فعال(صحيح فعال)
{
    HIMC	hImc;
    ساكن HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    إذا (!gui.دننه_use && !gui.starting)
    {
	mbyte_im_سېټ_فعال(فعال);
	أرجع;
    }
# endif

    إذا (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	أرجع;

    إذا (p_imdisable)
    {
	إذا (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    إذا (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	فعال = FALSE;
    }
    وإلا إذا (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    إذا (!hImc)
	أرجع;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    إذا (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ساكن DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ساكن BOOL bSaved = FALSE;

	إذا (فعال)
	{
	    // if we have a saved conversion status, restore it
	    إذا (bSaved)
		p_Imm_سېټ_Conversion_حالت(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	وإلا
	{
	    // save conversion status and disable korean
	    إذا (p_Imm_Get_Conversion_حالت(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		p_Imm_سېټ_Conversion_حالت(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    p_Imm_سېټ_Open_حالت(hImc, فعال);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    صحيح
im_get_حالت(فراغ)
{
    صحيح		حالت = 0;
    HIMC	hImc;

# ifdef VIMDLL
    إذا (!gui.دننه_use && !gui.starting)
	أرجع mbyte_im_get_حالت();
# endif

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	حالت = p_Imm_Get_Open_حالت(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    أرجع حالت;
}

#د_که_پای // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ساكن فراغ
latin9_منزل_ucs(char_u *متن, صحيح len, WCHAR *unicodebuf)
{
    صحيح		c;

    طالما (--len >= 0)
    {
	c = *متن++;
	بدل (c)
	{
	    حالة 0xa4: c = 0x20ac; اكسر;   // euro
	    حالة 0xa6: c = 0x0160; اكسر;   // S hat
	    حالة 0xa8: c = 0x0161; اكسر;   // S -hat
	    حالة 0xb4: c = 0x017d; اكسر;   // Z hat
	    حالة 0xb8: c = 0x017e; اكسر;   // Z -hat
	    حالة 0xbc: c = 0x0152; اكسر;   // OE
	    حالة 0xbd: c = 0x0153; اكسر;   // oe
	    حالة 0xbe: c = 0x0178; اكسر;   // Y
	}
	*unicodebuf++ = c;
    }
}

#که_تعریف_شوی FEAT_RIGHTLEFT
/*
 * What is this for?  In the case where you are using Win98 or Win2K or later,
 * and you are using a Hebrew font (or Arabic!), Windows does you a favor and
 * reverses the string sent to the TextOut... family.  This sucks, because we
 * go to a lot of effort to do the right thing, and there doesn't seem to be a
 * way to tell Windblows not to do this!
 *
 * The short of it is that this 'RevOut' only gets called if you are running
 * one of the new, "improved" MS OSes, and only if you are running in
 * 'rightleft' mode.  It makes display take *slightly* longer, but not
 * noticeably so.
 */
    ساكن فراغ
Rev_بهر( HDC hdc,
	صحيح ستون,
	صحيح لیکه,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR متن,
	UINT len,
	CONST INT *padding)
{
    صحيح		ix;

    لكل (ix = 0; ix < (صحيح)len; ++ix)
	Ext_متن_بهر(hdc, ستون + متن_X(ix), لیکه, foptions,
					pcliprect, متن + ix, 1, padding);
}
#د_که_پای

    ساكن فراغ
draw_کرښه(
    صحيح		x1,
    صحيح		y1,
    صحيح		x2,
    صحيح		y2,
    COLORREF	color)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_کرښه(s_dwc, x1, y1, x2, y2, color);
    وإلا
#د_که_پای
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	Move_منزل_Ex(s_hdc, x1, y1, عدم);
	// Note: LineTo() excludes the last pixel in the line.
	کرښه_منزل(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ساكن فراغ
سېټ_pixel(
    صحيح		x,
    صحيح		y,
    COLORREF	color)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWrite_Context_سېټ_Pixel(s_dwc, x, y, color);
    وإلا
#د_که_پای
	سېټ_Pixel(s_hdc, x, y, color);
}

    ساكن فراغ
fill_rect(
    ثابت RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    وإلا
#د_که_پای
    {
	HBRUSH	hbr2;

	إذا (hbr == عدم)
	    hbr2 = CreateSolidBrush(color);
	وإلا
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	إذا (hbr == عدم)
	    DeleteBrush(hbr2);
    }
}

    فراغ
gui_mch_draw_string(
    صحيح		لیکه,
    صحيح		ستون,
    char_u	*متن,
    صحيح		len,
    صحيح		flags)
{
    ساكن صحيح	*padding = عدم;
    ساكن صحيح	pad_اندازه = 0;
    ثابت RECT	*pcliprect = عدم;
    UINT	foptions = 0;
    ساكن WCHAR *unicodebuf = عدم;
    ساكن صحيح   *unicodepdy = عدم;
    ساكن صحيح	unibuflen = 0;
    صحيح		n = 0;
    صحيح		y;

    /*
     * Italic and bold text seems to have an extra row of pixels at the bottom
     * (below where the bottom of the character should be).  If we draw the
     * characters with a solid background, the top row of pixels in the
     * character below will be overwritten.  We can fix this by filling in the
     * background ourselves, to the correct character proportions, and then
     * writing the character in transparent mode.  Still have a problem when
     * the character is "_", which gets written on to the character below.
     * New fix: set gui.char_ascent to -1.  This shifts all characters up one
     * pixel in their slots, which fixes the problem with the bottom row of
     * pixels.	We still need this code because otherwise the top row of pixels
     * becomes a problem. - webb.
     */
    ساكن HBRUSH	hbr_cache[2] = {عدم, عدم};
    ساكن guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ساكن صحيح		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    إذا (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.کیڼ_2 = FILL_X(ستون);
	rc.سر_2 = FILL_Y(لیکه);
	إذا (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ښی_2 = FILL_X(ستون + mb_string2cells(متن, len));
	}
	وإلا
	    rc.ښی_2 = FILL_X(ستون + len);
	rc.bottom = FILL_Y(لیکه + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	إذا (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	وإلا إذا (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	وإلا
	{
	    إذا (hbr_cache[brush_lru] != عدم)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	سېټ_Bk_ډېرپېښ(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	إذا (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#که_تعریف_شوی FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	وإلا إذا (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#د_که_پای
    }
    سېټ_متن_Color(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#که_تعریف_شوی FEAT_DIRECTX
    إذا (IS_ENABLE_DIRECTX())
	DWrite_Context_سېټ_Font(s_dwc, (HFONT)gui.currFont);
#د_که_پای

    إذا (pad_اندازه != Columns || padding == عدم || padding[0] != gui.char_پلنوالی)
    {
	صحيح	i;

	vim_free(padding);
	pad_اندازه = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(صحيح, pad_اندازه);
	إذا (padding != عدم)
	    لكل (i = 0; i < pad_اندازه; i++)
		padding[i] = gui.char_پلنوالی;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    إذا (enc_utf8)
	لكل (n = 0; n < len; ++n)
	    إذا (متن[n] >= 0x80)
		اكسر;

#إذا defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    إذا (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#د_که_پای

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    إذا ((enc_utf8
		|| (enc_codepage > 0 && (صحيح)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == عدم || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(صحيح, len);

	unibuflen = len;
    }

    إذا (enc_utf8 && n < len && unicodebuf != عدم)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	صحيح		i;
	صحيح		wlen;	// string length in words
	صحيح		cells;	// cell width of string up to composing char
	صحيح		cw;	// width of current cell
	صحيح		c;

	wlen = 0;
	cells = 0;
	لكل (i = 0; i < len; )
	{
	    c = utf_ptr2char(متن + i);
	    إذا (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    وإلا
	    {
		unicodebuf[wlen++] = c;
	    }

	    إذا (utf_iscomposing(c))
		cw = 0;
	    وإلا
	    {
		cw = utf_char2cells(c);
		إذا (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    إذا (unicodepdy != عدم)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		إذا (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_پلنوالی;
		    unicodepdy[wlen - 1] = 0;
		}
		وإلا
		    unicodepdy[wlen - 1] = cw * gui.char_پلنوالی;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(متن + i, len - i);
	}
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWrite_Context_Draw_متن(s_dwc, unicodebuf, wlen,
		    متن_X(ستون), متن_Y(لیکه),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_پلنوالی, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	وإلا
#د_که_پای
	    Ext_متن_بهر_W(s_hdc, متن_X(ستون), متن_Y(لیکه),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    وإلا إذا ((enc_codepage > 0 && (صحيح)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	إذا (unicodebuf != عدم)
	{
	    إذا (enc_latin9)
		latin9_منزل_ucs(متن, len, unicodebuf);
	    وإلا
		len = Multi_Byte_منزل_Wide_Char(enc_codepage,
			MB_PRECOMPOSED,
			(حرف *)متن, len,
			(LPWSTR)unicodebuf, unibuflen);
	    إذا (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		إذا (unicodepdy != عدم)
		{
		    صحيح i;
		    صحيح cw;

		    لكل (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			إذا (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_پلنوالی;
		    }
		}
		Ext_متن_بهر_W(s_hdc, متن_X(ستون), متن_Y(لیکه),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    وإلا
    {
#که_تعریف_شوی FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	إذا (curwin->w_p_rl)
	    Rev_بهر(s_hdc, متن_X(ستون), متن_Y(لیکه),
			 foptions, pcliprect, (حرف *)متن, len, padding);
	وإلا
#د_که_پای
	    Ext_متن_بهر(s_hdc, متن_X(ستون), متن_Y(لیکه),
			 foptions, pcliprect, (حرف *)متن, len, padding);
    }

    // Underline
    إذا (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(لیکه + 1) - 1;
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_کرښه(FILL_X(ستون), y, FILL_X(ستون + len), y, gui.currFgColor);
    }

    // Strikethrough
    إذا (flags & DRAW_STRIKE)
    {
	y = FILL_Y(لیکه + 1) - gui.char_لوړوالی/2;
	draw_کرښه(FILL_X(ستون), y, FILL_X(ستون + len), y, gui.currSpColor);
    }

    // Undercurl
    إذا (flags & DRAW_UNDERC)
    {
	صحيح			x;
	صحيح			دننه_واټن;
	ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(لیکه + 1) - 1;
	لكل (x = FILL_X(ستون); x < FILL_X(ستون + len); ++x)
	{
	    دننه_واټن = val[x % 8];
	    سېټ_pixel(x, y - دننه_واټن, gui.currSpColor);
	}
    }
}


/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    فراغ
gui_mch_flush(فراغ)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#د_که_پای

    GdiFlush();
}

    ساكن فراغ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, عدم, gui.شا_pixel);
}


    فراغ
gui_mch_get_screen_dimensions(صحيح *screen_w, صحيح *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ښی_2 - workarea_rect.کیڼ_2
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.سر_2
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_لوړوالی(FALSE);
}


#إذا defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    فراغ
gui_mch_جمع_کړه_menu(
    vimmenu_T	*menu,
    صحيح		pos)
{
    vimmenu_T	*پلار = menu->پلار;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    إذا (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_منزل_utf16(menu->name, عدم);
	إذا (wn == عدم)
	    أرجع;

	infow.cb_اندازه = حجم(infow);
	infow.f_ماسک = MIIM_معلومات | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_توکی_معلومات = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_معلومات = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ورننباسه_Menu_توکی_W((پلار == عدم)
		? s_menuBar : پلار->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    إذا (پلار == عدم)
	gui_mswin_get_menu_لوړوالی(!gui.starting);
# ifdef FEAT_TEAROFF
    وإلا إذا (Is_کړکۍ(پلار->tearoff_handle))
	rebuild_tearoff(پلار);
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ټکی_2 mp;

    (فراغ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (صحيح)mp.x, (صحيح)mp.y);
}

    فراغ
gui_make_popup(char_u *لار_name, صحيح mouse_pos)
{
    vimmenu_T	*menu = gui_پیدا_کړه_menu(لار_name);

    إذا (menu == عدم)
	أرجع;

    ټکی_2	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    إذا (mouse_pos)
    {
	صحيح	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    وإلا إذا (curwin != عدم)
    {
	p.x += متن_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += متن_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    پیغام_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (صحيح)p.x, (صحيح)p.y);
}

# إذا defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    فراغ
gui_make_tearoff(char_u *لار_name)
{
    vimmenu_T	*menu = gui_پیدا_کړه_menu(لار_name);

    // Found the menu, so tear it off.
    إذا (menu != عدم)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    فراغ
gui_mch_جمع_کړه_menu_توکی(
    vimmenu_T	*menu,
    صحيح		idx)
{
    vimmenu_T	*پلار = menu->پلار;

    menu->id = s_menu_id++;
    menu->submenu_id = عدم;

# ifdef FEAT_TEAROFF
    إذا (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ورننباسه_Menu(پلار->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    وإلا
# endif
# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(پلار->name))
    {
	TBBUTTON newtb;

	CLEAR_ډګر(newtb);
	إذا (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	وإلا
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	Send_پیغام(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    وإلا
# endif
    {
	WCHAR	*wn;

	wn = enc_منزل_utf16(menu->name, عدم);
	إذا (wn != عدم)
	{
	    ورننباسه_Menu_W(پلار->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	إذا (Is_کړکۍ(پلار->tearoff_handle))
	    rebuild_tearoff(پلار);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    إذا (menu->submenu_id == (HMENU)-1)
    {
	صحيح iButton;

	iButton = (صحيح)Send_پیغام(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	Send_پیغام(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    وإلا
# endif
    {
	إذا (menu->پلار != عدم
		&& menu_is_popup(menu->پلار->dname)
		&& menu->پلار->submenu_id != عدم)
	    RemoveMenu(menu->پلار->submenu_id, menu->id, MF_BYCOMMAND);
	وإلا
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	إذا (menu->submenu_id != عدم)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	إذا (Is_کړکۍ(menu->tearoff_handle))
	    Destroy_کړکۍ(menu->tearoff_handle);
	إذا (menu->پلار != عدم
		&& menu->پلار->children != عدم
		&& Is_کړکۍ(menu->پلار->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->پلار);
	}
# endif
    }
}

# ifdef FEAT_TEAROFF
    ساكن فراغ
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    صحيح		x, y;

    HWND thwnd = menu->tearoff_handle;

    Get_کړکۍ_متن(thwnd, (LPSTR)tbuf, 127);
    إذا (Get_کړکۍ_Rect(thwnd, &trect)
	    && Get_کړکۍ_Rect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.کیڼ_2 - rct.کیڼ_2;
	y = (trect.سر_2 -  rct.bottom  + roct.bottom);
    }
    وإلا
    {
	x = y = 0xffffL;
    }
    Destroy_کړکۍ(thwnd);
    إذا (menu->children != عدم)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	إذا (Is_کړکۍ(menu->tearoff_handle))
	    (فراغ) سېټ_کړکۍ_Pos(menu->tearoff_handle,
				عدم,
				(صحيح)trect.کیڼ_2,
				(صحيح)trect.سر_2,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    فراغ
gui_mch_menu_grey(
    vimmenu_T	*menu,
    صحيح	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    إذا (menu->submenu_id == (HMENU)-1)
	Send_پیغام(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    وإلا
# endif
	(فراغ)Enable_Menu_توکی(menu->پلار ? menu->پلار->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    إذا ((menu->پلار != عدم) && (Is_کړکۍ(menu->پلار->tearoff_handle)))
    {
	کلمه_2 menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	إذا (menu->children == عدم)
	    menuID = (کلمه_2)(menu->id);
	وإلا
	    menuID = (کلمه_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = Get_Dlg_توکی(menu->پلار->tearoff_handle, menuID);
	إذا (menuHandle)
	    Enable_کړکۍ(menuHandle, !grey);

    }
# endif
}

#د_که_پای // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#تعریف_کړه جمع_کړه_کلمه(x)		*p++ = (x)
#تعریف_کړه جمع_کړه_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (کلمه_2 *)dwp

#إذا defined(FEAT_GUI_DIALOG)
/*
 * stuff for dialogs
 */

/*
 * The callback routine used by all the dialogs.  Very simple.  First,
 * acknowledges the INITDIALOG message so that Windows knows to do standard
 * dialog stuff (Return = default, Esc = cancel....) Second, if a button is
 * pressed, return that button's ID - IDCANCEL (2), which is the button's
 * number.
 */
    ساكن LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT پیغام_2,
    WPARAM wParam,
    LPARAM lParam ناکارول_شوی)
{
    إذا (پیغام_2 == WM_INITDIALOG)
    {
	Center_کړکۍ(hwnd, Get_کړکۍ(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(فراغ)سېټ_Focus(hwnd);
	إذا (dialog_default_button > IDCANCEL)
	    (فراغ)سېټ_Focus(Get_Dlg_توکی(hwnd, dialog_default_button));
	وإلا
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (فراغ)سېټ_Focus(Get_Dlg_توکی(hwnd, DLG_NONBUTTON_CONTROL));
	أرجع FALSE;
    }

    إذا (پیغام_2 == WM_COMMAND)
    {
	صحيح	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	إذا (button >= DLG_NONBUTTON_CONTROL)
	    أرجع TRUE;

	// If the edit box exists, copy the string.
	إذا (s_textfield != عدم)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    Get_Dlg_توکی_متن_W(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_منزل_enc(wp, عدم);
	    إذا (p != عدم)
	    {
		vim_strncpy(s_textfield, p, IOSIZE);
		vim_free(p);
	    }
	    vim_free(wp);
	}

	/*
	 * Need to check for IDOK because if the user just hits Return to
	 * accept the default value, some reason this is what we get.
	 */
	إذا (button == IDOK)
	{
	    إذا (dialog_default_button > IDCANCEL)
		پای_ټکی_Dialog(hwnd, dialog_default_button);
	}
	وإلا
	    پای_ټکی_Dialog(hwnd, button - IDCANCEL);
	أرجع TRUE;
    }

    إذا ((پیغام_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	پای_ټکی_Dialog(hwnd, 0);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Create a dialog dynamically from the parameter strings.
 * type		= type of dialog (question, alert, etc.)
 * title	= dialog title. may be NULL for default title.
 * message	= text to display. Dialog sizes to accommodate it.
 * buttons	= '\n' separated list of button captions, default first.
 * dfltbutton	= number of default button.
 *
 * This routine returns 1 if the first button is pressed,
 *			2 for the second, etc.
 *
 *			0 indicates Esc was pressed.
 *			-1 for unexpected error
 *
 * If stubbing out this fn, return 1.
 */

ساكن ثابت حرف *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    صحيح
gui_mch_dialog(
    صحيح		 type,
    char_u	*title,
    char_u	*پیغام_2,
    char_u	*buttons,
    صحيح		 dfltbutton,
    char_u	*textfield,
    صحيح		ex_cmd ناکارول_شوی)
{
    کلمه_2	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    صحيح		عدد_Buttons;
    صحيح		*buttonWidths, *buttonPositions;
    صحيح		buttonYpos;
    صحيح		nchar, i;
    DWORD	lStyle;
    صحيح		dlgwidth = 0;
    صحيح		dlgheight;
    صحيح		editboxheight;
    صحيح		horiz_پلنوالی = 0;
    صحيح		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*وروستی_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    صحيح		font_لوړوالی_2;
    صحيح		متن_پلنوالی_2, min_Button_پلنوالی, پیغام_پلنوالی;
    صحيح		max_Dialog_پلنوالی;
    صحيح		max_Dialog_لوړوالی;
    صحيح		scroll_flag = 0;
    صحيح		vertical;
    صحيح		dlgPaddingX;
    صحيح		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    صحيح		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    صحيح		l;
    صحيح		dlg_icon_پلنوالی;
    صحيح		dlg_icon_لوړوالی;
    صحيح		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    إذا (!(gui.دننه_use || gui.starting))
#  endif
	إذا (silent_ډېرپېښ)
	    أرجع dfltbutton;   // return default option
# endif

    إذا (s_hwnd == عدم)
    {
	بارول_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    وإلا
	dpi = pGetDpiForSystem();

    إذا ((type < 0) || (type > VIM_وروستی_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_اندازه + STRLEN(پیغام_2) * 2);

    إذا (p == عدم)
	أرجع -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    إذا (tbuffer == عدم)
	أرجع -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    عدد_Buttons = 1;
    لكل (i = 0; tbuffer[i] != '\0'; i++)
    {
	إذا (tbuffer[i] == DLG_BUTTON_SEP)
	    عدد_Buttons++;
    }
    إذا (dfltbutton >= عدد_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(صحيح, عدد_Buttons);
    إذا (buttonWidths == عدم)
	أرجع -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(صحيح, عدد_Buttons);
    إذا (buttonPositions == عدم)
	أرجع -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = Get_Desktop_کړکۍ();
    hdc = Get_کړکۍ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_غیرمستقیم_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وإلا
# endif
	font = CreateFont(-DLG_FONT_ټکی_اندازه, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    Get_متن_Metrics(hdc, &fontInfo);
    font_لوړوالی_2 = fontInfo.tm_لوړوالی;

    // Minimum width for horizontal button
    min_Button_پلنوالی = Get_متن_پلنوالی(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    إذا (s_hwnd == عدم)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	max_Dialog_پلنوالی = workarea_rect.ښی_2 - workarea_rect.کیڼ_2 - 100;
	إذا (max_Dialog_پلنوالی > adjust_by_system_dpi(600))
	    max_Dialog_پلنوالی = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	max_Dialog_لوړوالی = workarea_rect.bottom - workarea_rect.سر_2 - 150;
    }
    وإلا
    {
	// Use our own window for the size, unless it's very small.
	Get_کړکۍ_Rect(s_hwnd, &rect);
	max_Dialog_پلنوالی = rect.ښی_2 - rect.کیڼ_2
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	إذا (max_Dialog_پلنوالی < adjust_by_system_dpi(DLG_MIN_MAX_پلنوالی))
	    max_Dialog_پلنوالی = adjust_by_system_dpi(DLG_MIN_MAX_پلنوالی);

	max_Dialog_لوړوالی = rect.bottom - rect.سر_2
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	إذا (max_Dialog_لوړوالی < adjust_by_system_dpi(DLG_MIN_MAX_لوړوالی))
	    max_Dialog_لوړوالی = adjust_by_system_dpi(DLG_MIN_MAX_لوړوالی);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = پیغام_2;
    پیغام_پلنوالی = 0;
    msgheight = 0;
    ga_init2(&ga, حجم(حرف), 500);
    افعل
    {
	msgheight += font_لوړوالی_2;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	متن_پلنوالی_2 = 0;
	وروستی_white = عدم;
	لكل (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    إذا (l == 1 && VIM_ISWHITE(*pend)
					&& متن_پلنوالی_2 > max_Dialog_پلنوالی * 3 / 4)
		وروستی_white = pend;
	    متن_پلنوالی_2 += Get_متن_پلنوالی_Enc(hdc, pend, l);
	    إذا (متن_پلنوالی_2 >= max_Dialog_پلنوالی)
	    {
		// Line will wrap.
		پیغام_پلنوالی = max_Dialog_پلنوالی;
		msgheight += font_لوړوالی_2;
		متن_پلنوالی_2 = 0;

		إذا (وروستی_white != عدم)
		{
		    // break the line just after a space
		    إذا (pend > وروستی_white)
			ga.ga_len -= (صحيح)(pend - (وروستی_white + 1));
		    pend = وروستی_white + 1;
		    وروستی_white = عدم;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		استمر;
	    }

	    طالما (--l >= 0)
		ga_append(&ga, *pend++);
	}
	إذا (متن_پلنوالی_2 > پیغام_پلنوالی)
	    پیغام_پلنوالی = متن_پلنوالی_2;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } طالما (*pend != NUL);

    إذا (ga.ga_معلومات != عدم)
	پیغام_2 = ga.ga_معلومات;

    پیغام_پلنوالی += 10;		// roundoff space

    dlg_icon_پلنوالی = adjust_by_system_dpi(DLG_ICON_پلنوالی);
    dlg_icon_لوړوالی = adjust_by_system_dpi(DLG_ICON_لوړوالی);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = پیغام_پلنوالی + dlg_icon_پلنوالی + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    إذا (msgheight < dlg_icon_لوړوالی)
	msgheight = dlg_icon_لوړوالی;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != عدم && vim_strchr(p_go, GO_VERTICAL) != عدم);
    إذا (!vertical)
    {
	// Place buttons horizontally if they fit.
	horiz_پلنوالی = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	افعل
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    إذا (pend == عدم)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    متن_پلنوالی_2 = Get_متن_پلنوالی_Enc(hdc, pstart, (صحيح)(pend - pstart));
	    إذا (متن_پلنوالی_2 < min_Button_پلنوالی)
		متن_پلنوالی_2 = min_Button_پلنوالی;
	    متن_پلنوالی_2 += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = متن_پلنوالی_2;
	    buttonPositions[i++] = horiz_پلنوالی;
	    horiz_پلنوالی += متن_پلنوالی_2 + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} طالما (*pend != NUL);

	إذا (horiz_پلنوالی > max_Dialog_پلنوالی)
	    vertical = TRUE;	// Too wide to fit on the screen.
	وإلا إذا (horiz_پلنوالی > dlgwidth)
	    dlgwidth = horiz_پلنوالی;
    }

    إذا (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	افعل
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    إذا (pend == عدم)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    متن_پلنوالی_2 = Get_متن_پلنوالی_Enc(hdc, pstart, (صحيح)(pend - pstart));
	    متن_پلنوالی_2 += dlgPaddingX;		// Padding within button
	    متن_پلنوالی_2 += DLG_VERT_PADDING_X * 2; // Padding around button
	    إذا (متن_پلنوالی_2 > dlgwidth)
		dlgwidth = متن_پلنوالی_2;
	    pstart = pend + 1;
	} طالما (*pend != NUL);
    }

    إذا (dlgwidth < DLG_MIN_پلنوالی)
	dlgwidth = DLG_MIN_پلنوالی;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    جمع_کړه_long(lStyle);
    جمع_کړه_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    جمع_کړه_کلمه(0);	// NumberOfItems(will change later)
    جمع_کړه_کلمه(10);	// x
    جمع_کړه_کلمه(10);	// y
    جمع_کړه_کلمه(Pixel_منزل_Dialog_X(dlgwidth));	// cx

    // Dialog height.
    إذا (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * font_لوړوالی_2 * عدد_Buttons;
    وإلا
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * font_لوړوالی_2;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = font_لوړوالی_2 + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    إذا (textfield != عدم)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    إذا (dlgheight > max_Dialog_لوړوالی)
    {
	msgheight = msgheight - (dlgheight - max_Dialog_لوړوالی);
	dlgheight = max_Dialog_لوړوالی;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	پیغام_پلنوالی = dlgwidth - dlg_icon_پلنوالی - 3 * dlgPaddingX;
    }

    جمع_کړه_کلمه(Pixel_منزل_Dialog_Y(dlgheight));

    جمع_کړه_کلمه(0);	// Menu
    جمع_کړه_کلمه(0);	// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_منزل_Wide_Char(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_نسخه_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    إذا (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_لوړوالی, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحيح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وإلا
# endif
    {
	*p++ = DLG_FONT_ټکی_اندازه;		// point size
	nchar = n_Copy_Ansi_منزل_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    إذا (textfield != عدم)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    إذا (!vertical)
	horiz_پلنوالی = (dlgwidth - horiz_پلنوالی) / 2;	// Now it's X offset
    لكل (i = 0; i < عدد_Buttons; i++)
    {
	// get end of this button.
	لكل (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	إذا (*pend)
	    *pend = '\0';

	/*
	 * old NOTE:
	 * setting the BS_DEFPUSHBUTTON style doesn't work because Windows sets
	 * the focus to the first tab-able button and in so doing makes that
	 * the default!! Grrr.  Workaround: Make the default button the only
	 * one with WS_TABSTOP style. Means user can't tab between buttons, but
	 * he/she can use arrow keys.
	 *
	 * new NOTE: BS_DEFPUSHBUTTON is required to be able to select the
	 * right button when hitting <Enter>.  E.g., for the ":confirm quit"
	 * dialog.  Also needed for when the textfield is the default control.
	 * It appears to work now (perhaps not on Win95?).
	 */
	إذا (vertical)
	{
	    p = جمع_کړه_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_منزل_Dialog_X(DLG_VERT_PADDING_X),
		    Pixel_منزل_Dialog_Y(buttonYpos // TBK
				   + 2 * font_لوړوالی_2 * i),
		    Pixel_منزل_Dialog_X(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (کلمه_2)(Pixel_منزل_Dialog_Y(2 * font_لوړوالی_2) - 1),
		    (کلمه_2)(IDCANCEL + 1 + i), (کلمه_2)0x0080, (حرف *)pstart);
	}
	وإلا
	{
	    p = جمع_کړه_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_منزل_Dialog_X(horiz_پلنوالی + buttonPositions[i]),
		    Pixel_منزل_Dialog_Y(buttonYpos), // TBK
		    Pixel_منزل_Dialog_X(buttonWidths[i]),
		    (کلمه_2)(Pixel_منزل_Dialog_Y(2 * font_لوړوالی_2) - 1),
		    (کلمه_2)(IDCANCEL + 1 + i), (کلمه_2)0x0080, (حرف *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += عدد_Buttons;

    // Vim icon
    p = جمع_کړه_dialog_element(p, SS_ICON,
	    Pixel_منزل_Dialog_X(dlgPaddingX),
	    Pixel_منزل_Dialog_Y(dlgPaddingY),
	    Pixel_منزل_Dialog_X(dlg_icon_پلنوالی),
	    Pixel_منزل_Dialog_Y(dlg_icon_لوړوالی),
	    DLG_NONBUTTON_CONTROL + 0, (کلمه_2)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = جمع_کړه_dialog_element(p, ES_کیڼ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    Pixel_منزل_Dialog_X(2 * dlgPaddingX + dlg_icon_پلنوالی),
	    Pixel_منزل_Dialog_Y(dlgPaddingY),
	    (کلمه_2)(Pixel_منزل_Dialog_X(پیغام_پلنوالی) + 1),
	    Pixel_منزل_Dialog_Y(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (کلمه_2)0x0081, (حرف *)پیغام_2);

    // Edit box
    إذا (textfield != عدم)
    {
	p = جمع_کړه_dialog_element(p, ES_کیڼ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		Pixel_منزل_Dialog_X(2 * dlgPaddingX),
		Pixel_منزل_Dialog_Y(2 * dlgPaddingY + msgheight),
		Pixel_منزل_Dialog_X(dlgwidth - 4 * dlgPaddingX),
		Pixel_منزل_Dialog_Y(font_لوړوالی_2 + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (کلمه_2)0x0081, (حرف *)textfield);
	*pnumitems += 1;
    }

    *pnumitems += 2;

    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    // Let the dialog_callback() function know which button to make default
    // If we have an edit box, make that the default. We also need to tell
    // dialog_callback() if this dialog contains an edit box or not. We do
    // this by setting s_textfield if it does.
    إذا (textfield != عدم)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    وإلا
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = عدم;
    }

    // show the dialog box modally and get a return value
    nchar = (صحيح)Dialog_Box_غیرمستقیم(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_معلومات);

    // Focus back to our window (for when MDI is used).
    (فراغ)سېټ_Focus(s_hwnd);

    أرجع nchar;
}

#د_که_پای // FEAT_GUI_DIALOG

/*
 * Put a simple element (basic class) onto a dialog template in memory.
 * return a pointer to where the next item should be added.
 *
 * parameters:
 *  lStyle = additional style flags
 *		(be careful, NT3.51 & Win32s will ignore the new ones)
 *  x,y = x & y positions IN DIALOG UNITS
 *  w,h = width and height IN DIALOG UNITS
 *  Id  = ID used in messages
 *  clss  = class ID, e.g 0x0080 for a button, 0x0082 for a static
 *  caption = usually text or resource name
 *
 *  TODO: use the length information noted here to enable the dialog creation
 *  routines to work out more exactly how much memory they need to alloc.
 */
    ساكن PWORD
جمع_کړه_dialog_element(
    PWORD p,
    DWORD lStyle,
    کلمه_2 x,
    کلمه_2 y,
    کلمه_2 w,
    کلمه_2 h,
    کلمه_2 Id,
    کلمه_2 clss,
    ثابت حرف *caption)
{
    صحيح nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_ماشوم;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (کلمه_2)0xffff;
    *p++ = clss;			//2 more here

    nchar = n_Copy_Ansi_منزل_Wide_Char(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    أرجع p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ساكن LPWORD
lpwAlign(
    LPWORD lp_دننه)
{
    long_u ul;

    ul = (long_u)lp_دننه;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    أرجع (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ساكن صحيح
n_Copy_Ansi_منزل_Wide_Char(
    LPWORD lpWCStr,
    LPSTR lp_Ansi_دننه,
    BOOL use_enc)
{
    صحيح		nChar = 0;
    صحيح		len = lstrlen(lp_Ansi_دننه) + 1;	// include NUL character
    صحيح		i;
    WCHAR	*wn;

    إذا (use_enc && enc_codepage >= 0 && (صحيح)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_منزل_utf16((char_u *)lp_Ansi_دننه, عدم);
	إذا (wn != عدم)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (صحيح)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    إذا (nChar == 0)
	// Use Win32 conversion function.
	nChar = Multi_Byte_منزل_Wide_Char(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lp_Ansi_دننه, len,
		lpWCStr, len);
    لكل (i = 0; i < nChar; ++i)
	إذا (lpWCStr[i] == (کلمه_2)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (کلمه_2)' ';

    أرجع nChar;
}


#که_تعریف_شوی FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ساكن HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    کلمه_2 menu_id)
{
    لكل ( ; menu != عدم; menu = menu->بل)
    {
	إذا (menu->modes == 0)	// this menu has just been deleted
	    استمر;
	إذا (menu_is_separator(menu->dname))
	    استمر;
	إذا ((کلمه_2)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    أرجع menu->submenu_id;
    }
    أرجع عدم;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ساكن LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT پیغام_2,
    WPARAM wParam,
    LPARAM lParam)
{
    إذا (پیغام_2 == WM_INITDIALOG)
    {
	سېټ_کړکۍ_Long_Ptr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	أرجع (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(پیغام_2, lParam);

    إذا (پیغام_2 == WM_COMMAND)
    {
	إذا ((کلمه_2)(LOWORD(wParam)) & 0x8000)
	{
	    ټکی_2   mp;
	    RECT    rect;

	    إذا (GetCursorPos(&mp) && Get_کړکۍ_Rect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)Get_کړکۍ_Long_Ptr(hwnd, DWLP_USER);
		(فراغ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (صحيح)rect.ښی_2 - 8,
			 (صحيح)mp.y,
			 (صحيح)0,	    // reserved param
			 s_hwnd,
			 عدم);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	وإلا
	    // Pass on messages to the main Vim window
	    Post_پیغام(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(فراغ)سېټ_Focus(s_hwnd);
	أرجع TRUE;
    }
    إذا ((پیغام_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	Destroy_کړکۍ(hwnd);
	أرجع TRUE;
    }

    // When moved around, give main window the focus back.
    إذا (پیغام_2 == WM_EXITSIZEMOVE)
	(فراغ)سېټ_فعال_کړکۍ(s_hwnd);

    أرجع FALSE;
}
#د_که_پای


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ساكن فراغ
get_dialog_font_metrics(فراغ)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    اندازه_3	    اندازه;
#که_تعریف_شوی USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#د_که_پای

#که_تعریف_شوی USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = Create_Font_غیرمستقیم_W(&lfSysmenu);
    وإلا
#د_که_پای
	hfontTools = CreateFont(-DLG_FONT_ټکی_اندازه, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_اوسط_Font_اندازه(hdc, &اندازه);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (کلمه_2)اندازه.cx;
    s_dlgfntheight = (کلمه_2)اندازه.cy;
}

#إذا defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ساكن فراغ
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    صحيح		initX,
    صحيح		initY)
{
    کلمه_2	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    صحيح		template_len;
    صحيح		nchar, متن_پلنوالی_2, submenu_پلنوالی;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    کلمه_2	dlgwidth;
    کلمه_2	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*سر_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    صحيح		ستون, space_پلنوالی, len;
    صحيح		ستنه_Widths[2];
    char_u	*label, *متن;
    صحيح		acLen = 0;
    صحيح		nameLen;
    صحيح		padding0, padding1, padding2 = 0;
    صحيح		sepPadding=0;
    صحيح		x;
    صحيح		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    صحيح		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    إذا (Is_کړکۍ(menu->tearoff_handle))
    {
	ټکی_2 mp;
	إذا (GetCursorPos(&mp))
	{
	    سېټ_کړکۍ_Pos(menu->tearoff_handle, عدم, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	أرجع;
    }

    /*
     * Create a new tearoff.
     */
    إذا (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_اندازه;
    pdlgtemplate = p = (کلمه_2 *)LocalAlloc(LPTR, template_len);
    إذا (p == عدم)
	أرجع;

    hwnd = Get_Desktop_کړکۍ();
    hdc = Get_کړکۍ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_غیرمستقیم_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وإلا
# endif
	font = CreateFont(-DLG_FONT_ټکی_اندازه, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    space_پلنوالی = Get_متن_پلنوالی(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenu_پلنوالی = 0;
    لكل (ستون = 0; ستون < 2; ستون++)
    {
	ستنه_Widths[ستون] = 0;
	FOR_ALL_ماشوم_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    متن = (ستون == 0) ? pmenu->dname : pmenu->actext;
	    إذا (متن != عدم && *متن != NUL)
	    {
		متن_پلنوالی_2 = Get_متن_پلنوالی_Enc(hdc, متن, (صحيح)STRLEN(متن));
		إذا (متن_پلنوالی_2 > ستنه_Widths[ستون])
		    ستنه_Widths[ستون] = متن_پلنوالی_2;
	    }
	    إذا (pmenu->children != عدم)
		submenu_پلنوالی = TEAROFF_ستنه_PADDING * space_پلنوالی;
	}
    }
    إذا (ستنه_Widths[1] == 0)
    {
	// no accelerators
	إذا (submenu_پلنوالی != 0)
	    ستنه_Widths[0] += submenu_پلنوالی;
	وإلا
	    ستنه_Widths[0] += space_پلنوالی;
    }
    وإلا
    {
	// there is an accelerator column
	ستنه_Widths[0] += TEAROFF_ستنه_PADDING * space_پلنوالی;
	ستنه_Widths[1] += submenu_پلنوالی;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    متن_پلنوالی_2 = ستنه_Widths[0] + ستنه_Widths[1];
    إذا (submenu_پلنوالی != 0)
    {
	submenu_پلنوالی = Get_متن_پلنوالی(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (صحيح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	متن_پلنوالی_2 += submenu_پلنوالی;
    }
    dlgwidth = Get_متن_پلنوالی_Enc(hdc, title, (صحيح)STRLEN(title));
    إذا (متن_پلنوالی_2 > dlgwidth)
	dlgwidth = متن_پلنوالی_2;
    dlgwidth += 2 * TEAROFF_PADDING_X + TEAROFF_BUTTON_PAD_X;

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | WS_SYSMENU | DS_SETFONT | WS_VISIBLE;

    lExtendedStyle = WS_EX_TOOLWINDOW|WS_EX_STATICEDGE;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = LOWORD(lExtendedStyle);
    *p++ = HIWORD(lExtendedStyle);
    pnumitems = p;	// save where the number of items must be stored
    *p++ = 0;		// NumberOfItems(will change later)
    gui_mch_getmouse(&x, &y);
    إذا (initX == 0xffffL)
	*p++ = Pixel_منزل_Dialog_X(x); // x
    وإلا
	*p++ = Pixel_منزل_Dialog_X(initX); // x
    إذا (initY == 0xffffL)
	*p++ = Pixel_منزل_Dialog_Y(y); // y
    وإلا
	*p++ = Pixel_منزل_Dialog_Y(initY); // y
    *p++ = Pixel_منزل_Dialog_X(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_منزل_Wide_Char(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_نسخه_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    إذا (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_لوړوالی, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحيح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وإلا
# endif
    {
	*p++ = DLG_FONT_ټکی_اندازه;		// point size
	nchar = n_Copy_Ansi_منزل_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    إذا (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->بل;
    وإلا
	menu = menu->children;
    سر_menu = menu;
    لكل ( ; menu != عدم; menu = menu->بل)
    {
	إذا (menu->modes == 0)	// this menu has just been deleted
	    استمر;
	إذا (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    استمر;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	إذا (((حرف *)p - (حرف *)pdlgtemplate) + 1000 > template_len)
	{
	    کلمه_2    *newp;

	    newp = (کلمه_2 *)LocalAlloc(LPTR, template_len + 4096);
	    إذا (newp != عدم)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (حرف *)p - (حرف *)pdlgtemplate);
		p = newp + (p - pdlgtemplate);
		pnumitems = newp + (pnumitems - pdlgtemplate);
		ptrueheight = newp + (ptrueheight - pdlgtemplate);
		LocalFree(LocalHandle(pdlgtemplate));
		pdlgtemplate = newp;
	    }
	}

	// Figure out minimal length of this menu label.  Use "name" for the
	// actual text, "dname" for estimating the displayed size.  "name"
	// has "&a" for mnemonic and includes the accelerator.
	len = nameLen = (صحيح)STRLEN(menu->name);
	padding0 = (ستنه_Widths[0] - Get_متن_پلنوالی_Enc(hdc, menu->dname,
				      (صحيح)STRLEN(menu->dname))) / space_پلنوالی;
	len += padding0;

	إذا (menu->actext != عدم)
	{
	    acLen = (صحيح)STRLEN(menu->actext);
	    len += acLen;
	    متن_پلنوالی_2 = Get_متن_پلنوالی_Enc(hdc, menu->actext, acLen);
	}
	وإلا
	    متن_پلنوالی_2 = 0;
	padding1 = (ستنه_Widths[1] - متن_پلنوالی_2) / space_پلنوالی;
	len += padding1;

	إذا (menu->children == عدم)
	{
	    padding2 = submenu_پلنوالی / space_پلنوالی;
	    len += padding2;
	    menuID = (کلمه_2)(menu->id);
	}
	وإلا
	{
	    len += (صحيح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (کلمه_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	متن = label = alloc(len + 1);
	إذا (label == عدم)
	    اكسر;

	vim_strncpy(متن, menu->name, nameLen);
	متن = vim_strchr(متن, TAB);	    // stop at TAB before actext
	إذا (متن == عدم)
	    متن = label + nameLen;	    // no actext, use whole name
	طالما (padding0-- > 0)
	    *متن++ = ' ';
	إذا (menu->actext != عدم)
	{
	    STRNCPY(متن, menu->actext, acLen);
	    متن += acLen;
	}
	طالما (padding1-- > 0)
	    *متن++ = ' ';
	إذا (menu->children != عدم)
	{
	    STRCPY(متن, TEAROFF_SUBMENU_LABEL);
	    متن += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	وإلا
	{
	    طالما (padding2-- > 0)
		*متن++ = ' ';
	}
	*متن = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = جمع_کړه_dialog_element(p,
		BS_PUSHBUTTON|BS_کیڼ,
		(کلمه_2)Pixel_منزل_Dialog_X(TEAROFF_PADDING_X),
		(کلمه_2)(sepPadding + 1 + 13 * (*pnumitems)),
		(کلمه_2)Pixel_منزل_Dialog_X(dlgwidth - 2 * TEAROFF_PADDING_X),
		(کلمه_2)12,
		menuID, (کلمه_2)0x0080, (حرف *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (کلمه_2)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = Create_Dialog_غیرمستقیم_Param(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback,
	    (LPARAM)سر_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (فراغ)سېټ_فعال_کړکۍ(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#د_که_پای

#إذا defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ساكن فراغ
initialise_toolbar(فراغ)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_ماشوم | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    عدم,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_پلنوالی, //api guide is wrong!
		    TOOLBAR_BUTTON_لوړوالی,
		    TOOLBAR_BUTTON_پلنوالی,
		    TOOLBAR_BUTTON_لوړوالی,
		    حجم(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    Send_پیغام(s_toolbarhwnd, TB_SETSTYLE, 0,
	Send_پیغام(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = Subclass_کړکۍ(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != عدم);

    update_toolbar_اندازه();
}

    ساكن فراغ
update_toolbar_اندازه(فراغ)
{
    صحيح		w, h;
    TBMETRICS	tbm;

    tbm.cb_اندازه = حجم(TBMETRICS);
    tbm.dw_ماسک = TBMF_PAD | TBMF_BUTTONSPACING;
    Send_پیغام(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_پلنوالی + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_لوړوالی + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    Send_پیغام(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_لوړوالی = h + 6;

    //DWORD s = SendMessage(s_toolbarhwnd, TB_GETBUTTONSIZE, 0, 0);
    //TRACE("actual button size: %d, %d", LOWORD(s), HIWORD(s));

    // TODO:
    // Currently, this function only updates the size of toolbar buttons.
    // It would be nice if the toolbar images are resized based on DPI.
}

    ساكن LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_پیغام, lParam);
    أرجع Call_کړکۍ_Proc(s_toolbar_wndproc, hwnd, u_پیغام, wParam, lParam);
}

    ساكن صحيح
get_toolbar_bitmap(vimmenu_T *menu)
{
    صحيح i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    إذا (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = عدم;

	إذا (menu->iconfile != عدم)
	{
	    gui_پیدا_کړه_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = بارول_Image(
			عدم,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_پلنوالی,
			TOOLBAR_BUTTON_لوړوالی,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	إذا (hbitmap == عدم
		&& (gui_پیدا_کړه_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != عدم ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = بارول_Image(
		    عدم,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_پلنوالی,
		    TOOLBAR_BUTTON_لوړوالی,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	إذا (hbitmap != عدم)
	{
	    TBADDBITMAP tb_جمع_کړه_Bitmap;

	    tb_جمع_کړه_Bitmap.hInst = عدم;
	    tb_جمع_کړه_Bitmap.nID = (long_u)hbitmap;

	    i = (صحيح)Send_پیغام(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tb_جمع_کړه_Bitmap);
	    // i will be set to -1 if it fails
	}
    }
    إذا (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_شمېر)
	i = menu->iconidx;

    أرجع i;
}
#د_که_پای

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
initialise_tabline(فراغ)
{
    InitCommonControls();

    s_tabhwnd = Create_کړکۍ(WC_TABCONTROL, "Vim tabline",
	    WS_ماشوم|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, عدم, g_hinst, عدم);
    s_tabline_wndproc = Subclass_کړکۍ(s_tabhwnd, tabline_wndproc);

    gui.tabline_لوړوالی = TABLINE_لوړوالی;

    سېټ_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ساكن tabpage_T *
Get_Tab_From_ټکی(
    HWND    hWnd,
    ټکی_2   pt)
{
    tabpage_T	*ptp = عدم;

    إذا (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	إذا (s_tabhwnd == hWnd)
	{
	    صحيح idx = Tab_Ctrl_موندنه_Test(s_tabhwnd, &htinfo);
	    إذا (idx != -1)
		ptp = پیدا_کړه_tabpage(idx + 1);
	}
    }
    أرجع ptp;
}

ساكن ټکی_2	    s_pt = {0, 0};
ساكن HCURSOR      s_hCursor = عدم;

    ساكن LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    ټکی_2	pt;
    tabpage_T	*tp;
    RECT	rect;
    صحيح		nCenter;
    صحيح		idx0;
    صحيح		idx1;

    HandleMouseHide(u_پیغام, lParam);

    بدل (u_پیغام)
    {
	حالة WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		سېټ_Capture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		اكسر;
	    }
	حالة WM_MOUSEMOVE:
	    إذا (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		إذا (مطلق_قیمت(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    سېټ_Cursor(بارول_Cursor(عدم, IDC_SIZEWE));

		    tp = Get_Tab_From_ټکی(hwnd, pt);
		    إذا (tp != عدم)
		    {
			idx0 = tabpage_شاخص(curtab) - 1;
			idx1 = tabpage_شاخص(tp) - 1;

			Tab_Ctrl_Get_توکی_Rect(hwnd, idx1, &rect);
			nCenter = rect.کیڼ_2 + (rect.ښی_2 - rect.کیڼ_2) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			إذا ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			وإلا إذا ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    اكسر;
	حالة WM_LBUTTONUP:
	    {
		إذا (GetCapture() == hwnd)
		{
		    سېټ_Cursor(s_hCursor);
		    ReleaseCapture();
		}
		اكسر;
	    }
	حالة WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = Tab_Ctrl_موندنه_Test(hwnd, &htinfo);
		إذا (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		اكسر;
	    }
	افتراضي:
	    اكسر;
    }

    أرجع Call_کړکۍ_Proc(s_tabline_wndproc, hwnd, u_پیغام, wParam, lParam);
}
#د_که_پای

#إذا defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    فراغ
gui_mch_سېټ_foreground(فراغ)
{
    إذا (IsIconic(s_hwnd))
	 Send_پیغام(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    سېټ_Foreground_کړکۍ(s_hwnd);
}
#د_که_پای

#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ساكن فراغ
dyn_imm_بارول(فراغ)
{
    hLibImm = vim_بارول_Lib("imm32.dll");
    إذا (hLibImm == عدم)
	أرجع;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))Get_Proc_پته(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))Get_Proc_پته(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))Get_Proc_پته(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))Get_Proc_پته(hLibImm, "ImmReleaseContext");
    p_Imm_Get_Open_حالت
	    = (BOOL (WINAPI *)(HIMC))Get_Proc_پته(hLibImm, "ImmGetOpenStatus");
    p_Imm_سېټ_Open_حالت
	    = (BOOL (WINAPI *)(HIMC, BOOL))Get_Proc_پته(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_پته(hLibImm, "ImmGetCompositionFontW");
    p_Imm_سېټ_Composition_Font_W
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_پته(hLibImm, "ImmSetCompositionFontW");
    p_Imm_سېټ_Composition_کړکۍ
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))Get_Proc_پته(hLibImm, "ImmSetCompositionWindow");
    p_Imm_Get_Conversion_حالت
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))Get_Proc_پته(hLibImm, "ImmGetConversionStatus");
    p_Imm_سېټ_Conversion_حالت
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))Get_Proc_پته(hLibImm, "ImmSetConversionStatus");

    إذا (       pImmGetCompositionStringW == عدم
	    || pImmGetContext == عدم
	    || pImmAssociateContext == عدم
	    || pImmReleaseContext == عدم
	    || p_Imm_Get_Open_حالت == عدم
	    || p_Imm_سېټ_Open_حالت == عدم
	    || pImmGetCompositionFontW == عدم
	    || p_Imm_سېټ_Composition_Font_W == عدم
	    || p_Imm_سېټ_Composition_کړکۍ == عدم
	    || p_Imm_Get_Conversion_حالت == عدم
	    || p_Imm_سېټ_Conversion_حالت == عدم)
    {
	FreeLibrary(hLibImm);
	hLibImm = عدم;
	pImmGetContext = عدم;
	أرجع;
    }

    أرجع;
}

#د_که_پای

#إذا defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

تعريفنوع بنية _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    فراغ
gui_mch_drawsign(صحيح لیکه, صحيح ستون, صحيح typenr)
{
    signicon_t *sign;
    صحيح x, y, w, h;

    إذا (!gui.دننه_use || (sign = (signicon_t *)sign_get_image(typenr)) == عدم)
	أرجع;

# إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = متن_X(ستون);
    y = متن_Y(لیکه);
    w = gui.char_پلنوالی * 2;
    h = gui.char_لوړوالی;
    بدل (sign->uType)
    {
	حالة IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		بټ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    اكسر;
	حالة IMAGE_ICON:
	حالة IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, عدم, DI_نورمال);
	    اكسر;
# ifdef FEAT_XPM_W32
	حالة IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		بټ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		بټ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    اكسر;
# endif
    }
}

    ساكن فراغ
close_signicon_image(signicon_t *sign)
{
    إذا (sign == عدم)
	أرجع;

    بدل (sign->uType)
    {
	حالة IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    اكسر;
	حالة IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    اكسر;
	حالة IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    اكسر;
# ifdef FEAT_XPM_W32
	حالة IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    اكسر;
# endif
    }
}

    فراغ *
gui_mch_ثبتوونکی_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = عدم;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    إذا (ext > signfile)
    {
	صحيح do_بارول = 1;

	إذا (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	وإلا إذا (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	وإلا إذا (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	وإلا
	    do_بارول = 0;

	إذا (do_بارول)
	    sign.hImage = (HANDLE)بارول_Image(عدم, (LPCSTR)signfile, sign.uType,
		    gui.char_پلنوالی * 2, gui.char_لوړوالی,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	إذا (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    بارول_Xpm_Image((حرف *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = عدم;
    إذا (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != عدم)
	*psign = sign;

    إذا (!psign)
    {
	إذا (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_دننه_sign_معلومات));
    }
    أرجع (فراغ *)psign;

}

    فراغ
gui_mch_destroy_sign(فراغ *sign)
{
    إذا (sign == عدم)
	أرجع;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#د_که_پای

#إذا defined(FEAT_BEVAL_GUI)

/*
 * BALLOON-EVAL IMPLEMENTATION FOR WINDOWS.
 *  Added by Sergey Khorev <sergey.khorev@gmail.com>
 *
 * The only reused thing is beval.h and get_beval_info()
 * from gui_beval.c (note it uses x and y of the BalloonEval struct
 * to get current mouse position).
 *
 * Trying to use as more Windows services as possible, and as less
 * IE version as possible :)).
 *
 * 1) Don't create ToolTip in gui_mch_create_beval_area, only initialize
 * BalloonEval struct.
 * 2) Enable/Disable simply create/kill BalloonEval Timer
 * 3) When there was enough inactivity, timer procedure posts
 * async request to debugger
 * 4) gui_mch_post_balloon (invoked from netbeans.c) creates tooltip control
 * and performs some actions to show it ASAP
 * 5) WM_NOTIFY:TTN_POP destroys created tooltip
 */

    ساكن فراغ
make_tooltip(BalloonEval *beval, حرف *متن, ټکی_2 pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    إذا (pti == عدم)
	أرجع;

    beval->balloon = Create_کړکۍ_Ex_W(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    عدم, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->موخه, عدم, g_hinst, عدم);

    سېټ_کړکۍ_Pos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_اندازه = حجم(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->موخه;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpsz_متن = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_منزل_utf16((char_u*)متن, عدم);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    إذا (GetClientRect(s_متن_Area, &rect))
	Send_پیغام_W(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ښی_2);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.کیڼ_2 = pt.x - 3;
    pti->rect.سر_2 = pt.y - 3;
    pti->rect.ښی_2 = pt.x + 3;
    pti->rect.bottom = pt.y + 3;

    Send_پیغام_W(beval->balloon, TTM_ADDTOOLW, 0, (LPARAM)pti);
    // Make tooltip appear sooner.
    Send_پیغام_W(beval->balloon, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
    // I've performed some tests and it seems the longest possible life time
    // of tooltip is 30 seconds.
    Send_پیغام_W(beval->balloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
    /*
     * HACK: force tooltip to appear, because it'll not appear until
     * first mouse move. D*mn M$
     * Amazingly moving (2, 2) and then (-1, -1) the mouse doesn't move.
     */
    mouse_event(MOUSEEVENTF_MOVE, 2, 2, 0, 0);
    mouse_event(MOUSEEVENTF_MOVE, (DWORD)-1, (DWORD)-1, 0, 0);
    vim_free(pti);
}

    ساكن فراغ
delete_tooltip(BalloonEval *beval)
{
    Post_پیغام(beval->balloon, WM_CLOSE, 0, 0);
}

    ساكن VOID CALLBACK
beval_timer_proc(
    HWND	hwnd ناکارول_شوی,
    UINT	u_پیغام ناکارول_شوی,
    UINT_PTR	idEvent ناکارول_شوی,
    DWORD	dw_وخت)
{
    ټکی_2	pt;
    RECT	rect;

    إذا (cur_beval == عدم || cur_beval->showState == ShS_SHOWING || !p_beval)
	أرجع;

    GetCursorPos(&pt);
    إذا (کړکۍ_From_ټکی(pt) != s_متن_Area)
	أرجع;

    Screen_منزل_Client(s_متن_Area, &pt);
    GetClientRect(s_متن_Area, &rect);
    إذا (!Pt_دننه_Rect(&rect, pt))
	أرجع;

    إذا (وروستی_user_activity > 0
	    && (dw_وخت - وروستی_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| مطلق_قیمت(cur_beval->x - pt.x) > 3
		|| مطلق_قیمت(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	إذا (cur_beval->پیغام_CB != عدم)
	    (*cur_beval->پیغام_CB)(cur_beval, 0);
    }
}

    فراغ
gui_mch_disable_beval_area(BalloonEval *beval ناکارول_شوی)
{
    KillTimer(s_متن_Area, beval_timer_id);
}

    فراغ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    إذا (beval == عدم)
	أرجع;
    beval_timer_id = سېټ_Timer(s_متن_Area, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    فراغ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    ټکی_2   pt;

    vim_free(beval->پیغام);
    beval->پیغام = mesg == عدم ? عدم : vim_strsave(mesg);
    إذا (beval->پیغام == عدم)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	أرجع;
    }

    إذا (beval->showState == ShS_SHOWING)
	أرجع;
    GetCursorPos(&pt);
    Screen_منزل_Client(s_متن_Area, &pt);

    إذا (مطلق_قیمت(beval->x - pt.x) < 3 && مطلق_قیمت(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (حرف *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    فراغ	*موخه ناکارول_شوی,	// ignored, always use s_textArea
    char_u	*mesg,
    فراغ	(*mesgCB)(BalloonEval *, صحيح),
    فراغ	*client_معلومات)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    إذا (mesg != عدم && mesgCB != عدم)
    {
	iemsg(e_cannot_create_ballooneval_with_both_پیغام_and_callback);
	أرجع عدم;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    إذا (beval != عدم)
    {
	beval->موخه = s_متن_Area;

	beval->showState = ShS_NEUTRAL;
	beval->پیغام = mesg;
	beval->پیغام_CB = mesgCB;
	beval->client_معلومات = client_معلومات;

	InitCommonControls();
	cur_beval = beval;

	إذا (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    أرجع beval;
}

    ساكن فراغ
Handle_WM_Notify(HWND hwnd ناکارول_شوی, LPNMHDR pnmh)
{
    إذا (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	أرجع;

    إذا (cur_beval == عدم)
	أرجع;

    بدل (pnmh->کوډ)
    {
	حالة TTN_SHOW:
	    اكسر;
	حالة TTN_راوباسه: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    اكسر;
	حالة TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpsz_متن = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    اكسر;
	حالة TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpsz_متن = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    اكسر;
    }
}

    ساكن فراغ
track_user_activity(UINT u_پیغام)
{
    إذا ((u_پیغام >= WM_MOUSEFIRST && u_پیغام <= WM_MOUSELAST)
	    || (u_پیغام >= WM_KEYFIRST && u_پیغام <= WM_KEYLAST))
	وروستی_user_activity = Get_Tick_شمېر();
}

    فراغ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#د_که_پای // FEAT_BEVAL_GUI

#إذا defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    فراغ
netbeans_draw_multisign_indicator(صحيح لیکه)
{
    صحيح i;
    صحيح y;
    صحيح x;

    إذا (!netbeans_فعال())
	أرجع;

    x = 0;
    y = متن_Y(لیکه);

# إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    لكل (i = 0; i < gui.char_لوړوالی - 3; i++)
	سېټ_Pixel(s_hdc, x+2, y++, gui.currFgColor);

    سېټ_Pixel(s_hdc, x+0, y, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+4, y++, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+1, y, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+3, y++, gui.currFgColor);
    سېټ_Pixel(s_hdc, x+2, y, gui.currFgColor);
}
#د_که_پای

#إذا defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ساكن صحيح
test_gui_w32_sendevent_mouse(dict_T *دلیلونه)
{
    إذا (!dict_has_کلی(دلیلونه, "row") || !dict_has_کلی(دلیلونه, "col"))
	أرجع FALSE;

    // Note: "move" is optional, requires fewer arguments
    صحيح move = (صحيح)dict_get_bool(دلیلونه, "move", FALSE);

    إذا (!move && (!dict_has_کلی(دلیلونه, "button")
	    || !dict_has_کلی(دلیلونه, "multiclick")
	    || !dict_has_کلی(دلیلونه, "modifiers")))
	أرجع FALSE;

    صحيح لیکه = (صحيح)dict_get_شمېره(دلیلونه, "row");
    صحيح ستون = (صحيح)dict_get_شمېره(دلیلونه, "col");

    إذا (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	إذا (dict_get_bool(دلیلونه, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    صحيح pY = (لیکه - 1) * gui.char_لوړوالی + gui.char_لوړوالی / 2;
	    صحيح pX = (ستون - 1) * gui.char_پلنوالی + gui.char_پلنوالی / 2;
	    gui_mouse_moved(pX, pY);
	}
	وإلا
	    gui_mouse_moved(ستون, لیکه);
    }
    وإلا
    {
	صحيح button = (صحيح)dict_get_شمېره(دلیلونه, "button");
	صحيح repeated_click = (صحيح)dict_get_شمېره(دلیلونه, "multiclick");
	int_u mods = (صحيح)dict_get_شمېره(دلیلونه, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سېټ_hor_scroll_ګام(6);
	mouse_سېټ_vert_scroll_ګام(3);

	gui_send_mouse_event(button, متن_X(ستون - 1), متن_Y(لیکه - 1),
							repeated_click, mods);
    }
    أرجع TRUE;
}

    ساكن صحيح
test_gui_w32_sendevent_keyboard(dict_T *دلیلونه)
{
    ننوت_2 inputs[1];
    ننوت_2 modkeys[3];
    Secure_Zero_حافظه(inputs, حجم(ننوت_2));
    Secure_Zero_حافظه(modkeys, 3 * حجم(ننوت_2));

    char_u *event = dict_get_string(دلیلونه, "event", TRUE);

    إذا (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	کلمه_2 vk_کوډ = dict_get_شمېره_def(دلیلونه, "keycode", 0);
	إذا (vk_کوډ <= 0 || vk_کوډ >= 0xFF)
	{
	    semsg(_(e_invalid_دلیل_nr), (اوږد)vk_کوډ);
	    أرجع FALSE;
	}

	BOOL is_Mod_کلی = (vk_کوډ == VK_ښويدنه || vk_کوډ == VK_CONTROL
	    || vk_کوډ == VK_MENU || vk_کوډ == VK_LSHIFT || vk_کوډ == VK_RSHIFT
	    || vk_کوډ == VK_LCONTROL || vk_کوډ == VK_RCONTROL
	    || vk_کوډ == VK_LMENU || vk_کوډ == VK_RMENU );

	BOOL unwrapMods = FALSE;
	صحيح mods = (صحيح)dict_get_شمېره(دلیلونه, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	إذا (mods && STRICMP(event, "keydown") == 0 && !is_Mod_کلی)
	{
	    صحيح n = 0;
	    إذا (mods & MOD_ماسک_ښويدنه)
	    {
		modkeys[n].type = ننوت_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    إذا (mods & MOD_ماسک_CTRL)
	    {
		modkeys[n].type = ننوت_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    إذا (mods & MOD_ماسک_ALT)
	    {
		modkeys[n].type = ننوت_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    إذا (n)
	    {
		(فراغ)سېټ_Foreground_کړکۍ(s_hwnd);
		Send_ننوت(n, modkeys, حجم(ننوت_2));
	    }
	}

	inputs[0].type = ننوت_KEYBOARD;
	inputs[0].ki.wVk = vk_کوډ;
	إذا (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    إذا (!is_Mod_کلی)
		unwrapMods = TRUE;
	}

	(فراغ)سېټ_Foreground_کړکۍ(s_hwnd);
	Send_ننوت(ARRAYSIZE(inputs), inputs, حجم(ننوت_2));
	vim_free(event);

	إذا (unwrapMods)
	{
	    modkeys[0].type = ننوت_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = ننوت_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = ننوت_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (فراغ)سېټ_Foreground_کړکۍ(s_hwnd);
	    Send_ننوت(3, modkeys, حجم(ننوت_2));
	}
    }
    وإلا
    {
	إذا (event == عدم)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	}
	وإلا
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "event", event);
	    vim_free(event);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}

    ساكن صحيح
test_gui_w32_sendevent_سېټ_keycode_trans_strategy(dict_T *دلیلونه)
{
    صحيح handled = 0;
    char_u *strategy = dict_get_string(دلیلونه, "strategy", TRUE);

    إذا (strategy)
    {
	إذا (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_کارول_شوی = &keycode_trans_strategy_classic;
	}
	وإلا إذا (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_کارول_شوی = &keycode_trans_strategy_experimental;
	}
    }

    إذا (!handled)
    {
	إذا (strategy == عدم)
	{
	    semsg(_(e_missing_دلیل_str), "strategy");
	}
	وإلا
	{
	    semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}


    صحيح
test_gui_w32_sendevent(char_u *event, dict_T *دلیلونه)
{
    إذا (STRICMP(event, "key") == 0)
	أرجع test_gui_w32_sendevent_keyboard(دلیلونه);
    وإلا إذا (STRICMP(event, "mouse") == 0)
	أرجع test_gui_w32_sendevent_mouse(دلیلونه);
    وإلا إذا (STRICMP(event, "set_keycode_trans_strategy") == 0)
	أرجع test_gui_w32_sendevent_سېټ_keycode_trans_strategy(دلیلونه);
    وإلا
    {
	semsg(_(e_invalid_ارزښت_for_دلیل_str_str), "event", event);
	أرجع FALSE;
    }
}
#د_که_پای

#إذا defined(FEAT_IMAGE_GUI)

تعريفنوع بنية
{
    HBITMAP hbm;
    HDC	    hdc;
} image_gdi_T;

/*
 * Convert the popup's RGB(A) pixel buffer into a 32-bit BGRX buffer. "dst" must
 * be large enough for iw * ih * 4 bytes.  When has_alpha is true, the source
 * has 4 bytes per pixel and the alpha channel is flattened onto the GUI window
 * background colour (pre-multiplied blend).
 */
    ساكن فراغ
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, صحيح iw, صحيح ih, image_format_T fmt)
{
    صحيح		y, x;
    صحيح		stride = iw * 4;
    صحيح		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.شا_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    لكل (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	لكل (x = 0; x < iw; x++)
	{
	    إذا (fmt == IMAGE_FORMAT_RGBA)
	    {
		صحيح a = s[3];

		إذا (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		وإلا إذا (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		وإلا
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    وإلا
	    {
		d[0] = s[2];	// B
		d[1] = s[1];	// G
		d[2] = s[0];	// R
	    }
	    d[3] = 0;
	    s += src_bpp;
	    d += 4;
	}
    }
}

    صحيح
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    صحيح		iw, ih;
    فراغ	*بټونه = عدم;

    إذا (ctx == عدم)
	أرجع FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_ډګر(bmi);
    bmi.bmi_سرلیک.bi_اندازه = حجم(BITMAPINFOHEADER);
    bmi.bmi_سرلیک.bi_پلنوالی = iw;
    bmi.bmi_سرلیک.bi_لوړوالی = -ih; // Top-down
    bmi.bmi_سرلیک.biPlanes = 1;
    bmi.bmi_سرلیک.bi_بټ_شمېر = 32;
    bmi.bmi_سرلیک.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &بټونه, عدم, 0);
    إذا (hbm == عدم || بټونه == عدم)
    {
	إذا (hbm != عدم)
	    DeleteObject(hbm);
	أرجع false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    إذا (mem_dc == عدم)
    {
	DeleteObject(hbm);
	أرجع false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)بټونه, img->معلومات, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_معلومات = ctx;
    أرجع OK;
}

    فراغ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_معلومات;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    صحيح
image_placement_gui_init(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
    أرجع OK;
}

    فراغ
image_placement_gui_uninit(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
}

    فراغ
image_placement_gui_draw(image_placement_T *ځای_پرځای_کړه)
{
    image_T	    *img = ځای_پرځای_کړه->img;
    image_gdi_T	    *ictx = img->backend_معلومات;
    pixman_box32_t  *rects;
    صحيح		    n_rects;

    rects = pixman_region32_rectangles(&ځای_پرځای_کړه->visible, &n_rects);
    إذا (rects == عدم || n_rects == 0)
	أرجع;

# إذا defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		لیکه, ستون;
	صحيح		posx, posy;
	صحيح		x, y, w, h;

	image_placement_subrect(ځای_پرځای_کړه, rect, &لیکه, &ستون, &x, &y, &w, &h, true);
	posx = FILL_X(ستون);
	posy = FILL_Y(لیکه);

	بټ_Blt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    فراغ
image_placement_gui_clear(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
}

#د_که_پای // FEAT_IMAGE_GUI
