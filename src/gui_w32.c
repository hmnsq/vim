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

#شامل "vim.h"

#اگر defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#اگر_ختم

// values for "dead_key"
#تعریف_کرو DEAD_کلید_OFF			0	// no dead key
#تعریف_کرو DEAD_کلید_سیٹ_DEFAULT		1	// dead key pressed
#تعریف_کرو DEAD_کلید_TRANSIENT_اندر_ON_CHAR	2	// wait for next key press
#تعریف_کرو DEAD_کلید_SKIP_ON_CHAR		3	// skip next _OnChar()

#اگر defined(FEAT_DIRECTX)
ساکن DWriteContext *s_dwc = تہی;
ساکن صحیح s_directx_enabled = 0;
ساکن صحیح s_directx_بھرو_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != تہی && enc_utf8)
ساکن صحیح directx_enabled(خالی);
ساکن خالی directx_binddc(خالی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
ساکن صحیح gui_mswin_get_menu_اونچائی(صحیح fix_کھڑکی);
#ورنہ
# define gui_mswin_get_menu_اونچائی(fix_کھڑکی)	0
#اگر_ختم

قسمتعریف ساخت keycode_trans_strategy {
    خالی (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
    خالی (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
    خالی (*ptr_process_پیغام_usual_کلید) (UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
    صحیح  (*ptr_get_فعال_modifiers)(خالی);
    صحیح  (*is_experimental)(خالی);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ساکن خالی _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ساکن خالی _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ساکن خالی process_پیغام_usual_کلید_experimental(UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
ساکن صحیح  get_فعال_modifiers_experimental(خالی);
ساکن صحیح  is_experimental_true(خالی);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_پیغام_usual_کلید_experimental // ptr_process_message_usual_key
    , get_فعال_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ساکن خالی _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ساکن خالی _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ساکن خالی process_پیغام_usual_کلید_classic(UINT /*vk*/, ثابت پیغام_3* /*pmsg*/);
ساکن صحیح  get_فعال_modifiers_classic(خالی);
ساکن صحیح  is_experimental_false(خالی);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_پیغام_usual_کلید_classic // ptr_process_message_usual_key
    , get_فعال_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_مستعمل = تہی;

ساکن صحیح is_experimental_true(خالی)
{
    واپس 1;
}

ساکن صحیح is_experimental_false(خالی)
{
    واپس 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ساکن خالی keycode_trans_strategy_init(خالی)
{
    ثابت حرف *strategy = تہی;

    // set default value as fallback
    keycode_trans_strategy_مستعمل = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    اگر (strategy == تہی)
    {
	واپس;
    }

    اگر (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_مستعمل = &keycode_trans_strategy_classic;
	واپس;
    }

    اگر (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_مستعمل = &keycode_trans_strategy_experimental;
	واپس;
    }

}

#اگر defined(FEAT_RENDER_OPTIONS)
    صحیح
gui_mch_سیٹ_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    صحیح	    dx_enable = 0;
    صحیح	    dx_flags = 0;
    اعشاری   dx_gamma = 0.0f;
    اعشاری   dx_contrast = 0.0f;
    اعشاری   dx_level = 0.0f;
    صحیح	    dx_geom = 0;
    صحیح	    dx_renmode = 0;
    صحیح	    dx_taamode = 0;
    // parse string as rendering options.
    برائے (p = s; p != تہی && *p != NUL; )
    {
	char_u  شے[256];
	char_u  name[128];
	char_u  قدر[128];

	copy_option_part(&p, شے, حجم(شے), ",");
	اگر (p == تہی)
	    توڑو;
	q = &شے[0];
	copy_option_part(&q, name, حجم(name), ":");
	اگر (q == تہی)
	    واپس FAIL;
	copy_option_part(&q, قدر, حجم(قدر), ":");

	اگر (STRCMP(name, "type") == 0)
	{
	    اگر (STRCMP(قدر, "directx") == 0)
		dx_enable = 1;
	    ورنہ
		واپس FAIL;
	}
	ورنہ اگر (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (اعشاری)atof((حرف *)قدر);
	}
	ورنہ اگر (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (اعشاری)atof((حرف *)قدر);
	}
	ورنہ اگر (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (اعشاری)atof((حرف *)قدر);
	}
	ورنہ اگر (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((حرف *)قدر);
	    اگر (dx_geom < 0 || dx_geom > 2)
		واپس FAIL;
	}
	ورنہ اگر (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((حرف *)قدر);
	    اگر (dx_renmode < 0 || dx_renmode > 6)
		واپس FAIL;
	}
	ورنہ اگر (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((حرف *)قدر);
	    اگر (dx_taamode < 0 || dx_taamode > 3)
		واپس FAIL;
	}
	ورنہ اگر (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	ورنہ
	    واپس FAIL;
    }

    اگر (!gui.اندر_use)
	واپس OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    اگر (dx_enable)
    {
	اگر (!directx_enabled())
	    واپس FAIL;
	DWrite_Context_سیٹ_Rendering_Params(s_dwc, تہی);
	اگر (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    اگر (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    اگر (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    اگر (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    اگر (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    اگر (dx_flags & (1 << 4))
		param.rendering_منوال = dx_renmode;
	    اگر (dx_flags & (1 << 5))
		param.متن_Antialias_منوال = dx_taamode;
	    DWrite_Context_سیٹ_Rendering_Params(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    واپس OK;
# ورنہ
    واپس FAIL;
# endif
}
#اگر_ختم

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#اگر_غیر_تعریف_شدہ HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (صحیح)(مختصر)LOWORD(lParam), (صحیح)(مختصر)HIWORD(lParam), (UINT)(wParam)), 0L)
#اگر_ختم
#اگر_غیر_تعریف_شدہ HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (صحیح)(مختصر)LOWORD(lParam), (صحیح)(مختصر)HIWORD(lParam), (UINT)(wParam)), 0L)
#اگر_ختم
#اگر_غیر_تعریف_شدہ HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (صحیح)(مختصر)LOWORD(lParam), (صحیح)(مختصر)HIWORD(lParam), (UINT)(wParam)), 0L)
#اگر_ختم


#شامل "version.h"	// used by dialog box routine for default title
#اگر_تعریف_شدہ DEBUG
# include <tchar.h>
#اگر_ختم

#شامل <shellapi.h>
#شامل <commctrl.h>
#شامل <windowsx.h>

#اگر_تعریف_شدہ FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#اگر_ختم

// Some parameters for dialog boxes.  All in pixels.
#تعریف_کرو DLG_PADDING_X		10
#تعریف_کرو DLG_PADDING_Y		10
#تعریف_کرو DLG_VERT_PADDING_X	4	// For vertical buttons
#تعریف_کرو DLG_VERT_PADDING_Y	4
#تعریف_کرو DLG_ICON_چوڑائی		34
#تعریف_کرو DLG_ICON_اونچائی		34
#تعریف_کرو DLG_MIN_چوڑائی		150
#تعریف_کرو DLG_FONT_NAME		"MS Shell Dlg"
#تعریف_کرو DLG_FONT_نقطہ_سائز	8
#تعریف_کرو DLG_MIN_MAX_چوڑائی	400
#تعریف_کرو DLG_MIN_MAX_اونچائی	400

#تعریف_کرو DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#اگر_غیر_تعریف_شدہ WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#اگر_ختم

#اگر_غیر_تعریف_شدہ WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#اگر_ختم

#اگر_غیر_تعریف_شدہ WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#اگر_ختم

#اگر_غیر_تعریف_شدہ SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#اگر_ختم

#اگر_غیر_تعریف_شدہ SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#اگر_ختم

#اگر_غیر_تعریف_شدہ DWMWA_USE_IMMERSIVE_DARK_منوال
# define DWMWA_USE_IMMERSIVE_DARK_منوال	20
#اگر_ختم

#اگر_غیر_تعریف_شدہ DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#اگر_ختم

#اگر_غیر_تعریف_شدہ DWMWA_متن_COLOR
# define DWMWA_متن_COLOR		36
#اگر_ختم

ساکن خالی _OnPaint(HWND hwnd);
ساکن خالی fill_rect(ثابت RECT *rcp, HBRUSH hbr, COLORREF color);
ساکن خالی clear_rect(RECT *rcp);

ساکن لفظ_2		s_dlgfntheight;		// height of the dialog font
ساکن لفظ_2		s_dlgfntwidth;		// width of the dialog font

#اگر_تعریف_شدہ FEAT_MENU
ساکن HMENU		s_menuBar = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TEAROFF
ساکن خالی rebuild_tearoff(vimmenu_T *menu);
ساکن HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#اگر_ختم

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ساکن صحیح		s_busy_processing = FALSE;

ساکن صحیح		destroying = FALSE;	// call DestroyWindow() ourselves

#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
ساکن UINT		s_findrep_پیغام = 0;
ساکن FINDREPLACEW	s_findrep_struct;
ساکن HWND		s_findrep_hwnd = تہی;
ساکن صحیح		s_findrep_is_ڈھونڈو;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#اگر_ختم

HWND			s_hwnd = تہی;
ساکن HDC		s_hdc = تہی;
ساکن HBRUSH		s_brush = تہی;

#اگر_تعریف_شدہ FEAT_TOOLBAR
ساکن HWND		s_toolbarhwnd = تہی;
ساکن WNDPROC		s_toolbar_wndproc = تہی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
ساکن HWND		s_tabhwnd = تہی;
ساکن WNDPROC		s_tabline_wndproc = تہی;
ساکن صحیح		showing_tabline = 0;
#اگر_ختم

ساکن WPARAM		s_wParam = 0;

ساکن HWND		s_متن_Area = تہی;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ساکن bool		s_textarea_bg_only = false;

ساکن UINT		s_u_پیغام = 0;

ساکن char_u		*s_textfield; // Used by dialogs to pass back strings

ساکن صحیح		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ساکن صحیح allow_scrollbar = FALSE;

#اگر_غیر_تعریف_شدہ _DPI_AWARENESS_CONTEXTS_
قسمتعریف HANDLE DPI_AWARENESS_CONTEXT;

قسمتعریف شمار DPI_AWARENESS {
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
#اگر_ختم

#تعریف_کرو DEFAULT_DPI	96
ساکن صحیح		s_dpi = DEFAULT_DPI;
ساکن BOOL		s_اندر_dpichanged = FALSE;
#اگر_تعریف_شدہ FEAT_MBYTE_IME
ساکن DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#اگر_ختم
ساکن RECT		s_suggested_rect;

ساکن UINT (WINAPI *pGetDpiForSystem)(خالی) = تہی;
ساکن UINT (WINAPI *p_Get_Dpi_For_کھڑکی)(HWND hwnd) = تہی;
ساکن صحیح (WINAPI *pGetSystemMetricsForDpi)(صحیح, UINT) = تہی;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ساکن DPI_AWARENESS_CONTEXT (WINAPI *p_سیٹ_Thread_Dpi_Awareness_Context)(DPI_AWARENESS_CONTEXT dpiContext) = تہی;
#اگر_تعریف_شدہ FEAT_MBYTE_IME
ساکن DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = تہی;
#اگر_ختم

ساکن HINSTANCE hLibDwm = تہی;
ساکن HRESULT (WINAPI *p_Dwm_سیٹ_کھڑکی_Attribute)(HWND, DWORD, LPCVOID, DWORD);
ساکن خالی dyn_dwm_بھرو(خالی);

ساکن صحیح fullscreen_on = FALSE;

#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME

ساکن HINSTANCE hUxThemeLib = تہی;
ساکن DWORD (WINAPI *p_سیٹ_Preferred_App_منوال)(DWORD) = تہی;
ساکن خالی (WINAPI *pFlushMenuThemes)(خالی) = تہی;
ساکن خالی dyn_uxtheme_بھرو(خالی);
#اگر_ختم

    ساکن صحیح WINAPI
stubGetSystemMetricsForDpi(صحیح n_اشاریہ, UINT dpi غیر_مستعمل)
{
    واپس GetSystemMetrics(n_اشاریہ);
}

    ساکن صحیح
adjust_fontsize_by_dpi(صحیح سائز)
{
    واپس سائز * s_dpi / (صحیح)pGetDpiForSystem();
}

    ساکن صحیح
adjust_by_system_dpi(صحیح سائز)
{
    واپس سائز * (صحیح)pGetDpiForSystem() / DEFAULT_DPI;
}

#اگر defined(FEAT_DIRECTX)
    ساکن صحیح
directx_enabled(خالی)
{
    اگر (s_dwc != تہی)
	واپس 1;
    ورنہ اگر (s_directx_بھرو_attempted)
	واپس 0;
    // load DirectX
    DWrite_Init();
    s_directx_بھرو_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    واپس s_dwc != تہی ? 1 : 0;
}

    ساکن خالی
directx_binddc(خالی)
{
    اگر (s_متن_Area == تہی)
	واپس;

    RECT	rect;
    GetClientRect(s_متن_Area, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#اگر_ختم

خارجی صحیح current_font_اونچائی;	    // this is in os_mswin.c

ساکن ساخت
{
    UINT    کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدیں[] =
{
    {VK_اوپر,		'k', 'u'},
    {VK_نیچے_کو,		'k', 'd'},
    {VK_بایاں,		'k', 'l'},
    {VK_دایاں,		'k', 'r'},

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
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#اگر_ختم
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_عقب,		'k', 'b'},
    {VK_داخل_کرو,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_انتہا,		'@', '7'},
    {VK_پیشگی,		'k', 'P'},
    {VK_اگلا,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_جوڑو,		'K', '6'},
    {VK_تفریق,	'K', '7'},
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
ساکن صحیح	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ساکن صحیح	s_getting_focus = FALSE;

ساکن صحیح	s_x_pending;
ساکن صحیح	s_y_pending;
ساکن UINT	s_kFlags_pending;
ساکن UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ساکن صحیح	s_timed_برآمد = FALSE;
ساکن صحیح	dead_کلید = DEAD_کلید_OFF;
ساکن UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ساکن خالی Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ساکن خالی track_user_activity(UINT u_پیغام);
#اگر_ختم

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#اگر_تعریف_شدہ FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ساکن LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ساکن LOGFONTW sub_logfont;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MBYTE_IME
ساکن LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ڈیٹا);
#اگر_ختم

#اگر defined(FEAT_BROWSE)
ساکن char_u *convert_filter(char_u *s);
#اگر_ختم

#اگر_تعریف_شدہ DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ساکن خالی
print_windows_error(خالی)
{
    LPVOID  lp_پیغام_Buf;

    Format_پیغام(FORMAT_پیغام_ALLOCATE_بفر | FORMAT_پیغام_FROM_SYSTEM,
		  تہی, Get_آخری_Error(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_پیغام_Buf, 0, تہی);
    TRACE1("Error: %s\n", lp_پیغام_Buf);
    LocalFree(lp_پیغام_Buf);
}
#اگر_ختم

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کرو BLINK_NONE  0
#تعریف_کرو BLINK_OFF   1
#تعریف_کرو BLINK_ON    2

ساکن صحیح		blink_state = BLINK_NONE;
ساکن long_u		blink_waittime = 700;
ساکن long_u		blink_ontime = 400;
ساکن long_u		blink_offtime = 250;
ساکن UINT_PTR		blink_timer = 0;

    صحیح
gui_mch_is_blinking(خالی)
{
    واپس blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(خالی)
{
    واپس blink_state == BLINK_OFF;
}

    خالی
gui_mch_سیٹ_blinking(طویل wait, طویل on, طویل off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ساکن VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_پیغام غیر_مستعمل,
    UINT_PTR idEvent,
    DWORD dw_وقت غیر_مستعمل)
{
    پیغام_3 پیغام;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(تہی, idEvent);

    // Eat spurious WM_TIMER messages
    جب (Peek_پیغام_W(&پیغام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = سیٹ_Timer(تہی, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    ورنہ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = سیٹ_Timer(تہی, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ساکن خالی
gui_mswin_rm_blink_timer(خالی)
{
    پیغام_3 پیغام;

    اگر (blink_timer == 0)
	واپس;

    KillTimer(تہی, blink_timer);
    // Eat spurious WM_TIMER messages
    جب (Peek_پیغام_W(&پیغام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    خالی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    خالی
gui_mch_آغاز_blink(خالی)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
    {
	blink_timer = سیٹ_Timer(تہی, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ساکن VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_پیغام غیر_مستعمل,
    UINT_PTR idEvent,
    DWORD dw_وقت غیر_مستعمل)
{
    پیغام_3 پیغام;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(تہی, idEvent);
    s_timed_برآمد = TRUE;

    // Eat spurious WM_TIMER messages
    جب (Peek_پیغام_W(&پیغام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    اگر (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ساکن خالی
_OnDeadChar(
    HWND hwnd غیر_مستعمل,
    UINT ch غیر_مستعمل,
    صحیح cRepeat غیر_مستعمل)
{
    dead_کلید = DEAD_کلید_سیٹ_DEFAULT;
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
    ساکن صحیح
char_مقصد_string(صحیح ch, char_u *string, صحیح slen, صحیح had_alt)
{
    صحیح		len;
    صحیح		i;
    WCHAR	wstring[2];
    char_u	*ws = تہی;

    اگر (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    ورنہ اگر (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	واپس 0;
    }
    ورنہ
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    اگر (enc_codepage > 0)
    {
	len = Wide_Char_مقصد_Multi_Byte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, تہی);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	اگر (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = Wide_Char_مقصد_Multi_Byte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, تہی);
	    اگر (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    ورنہ
    {
	ws = utf16_مقصد_enc(wstring, &len);
	اگر (ws == تہی)
	    len = 0;
	ورنہ
	{
	    اگر (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    اگر (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    برائے (i = 0; i < len; ++i)
	اگر (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (صحیح)KE_CSI;
	    len += 2;
	}

    واپس len;
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
    ساکن صحیح
get_فعال_modifiers_experimental(خالی)
{
    صحیح modifiers = 0;

    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_نقاب_CTRL;
    اگر (Get_کلید_State(VK_سرکاؤ) & 0x8000)
	modifiers |= MOD_نقاب_سرکاؤ;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    اگر (Get_کلید_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_نقاب_ALT;
    اگر ((modifiers & MOD_نقاب_CTRL) && (Get_کلید_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_نقاب_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    اگر (!(Get_کلید_State(VK_CONTROL) & 0x8000)
	    &&  (Get_کلید_State(VK_RMENU) & 0x8000)
	    && !(Get_کلید_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_نقاب_ALT;

    واپس modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساکن صحیح
get_فعال_modifiers_classic(خالی)
{
    صحیح modifiers = 0;

    اگر (Get_کلید_State(VK_سرکاؤ) & 0x8000)
	modifiers |= MOD_نقاب_سرکاؤ;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_نقاب_CTRL;
    اگر (Get_کلید_State(VK_MENU) & 0x8000)
	modifiers |= MOD_نقاب_ALT;

    واپس modifiers;
}

    ساکن صحیح
get_فعال_modifiers(خالی)
{
    واپس get_فعال_modifiers_experimental();
}

    ساکن صحیح
get_فعال_modifiers_بذریعہ_ptr(خالی)
{
    // marshal to corresponding implementation
    واپس keycode_trans_strategy_مستعمل->ptr_get_فعال_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ساکن خالی
_OnChar(
    HWND hwnd غیر_مستعمل,
    UINT cch,
    صحیح cRepeat غیر_مستعمل)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_مستعمل->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساکن خالی
_OnChar_experimental(
    HWND hwnd غیر_مستعمل,
    UINT cch,
    صحیح cRepeat غیر_مستعمل)
{
    char_u	string[40];
    صحیح		len = 0;
    صحیح		modifiers;
    صحیح		ch = cch;   // special keys are negative

    اگر (dead_کلید == DEAD_کلید_SKIP_ON_CHAR)
	واپس;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    اگر (dead_کلید != DEAD_کلید_TRANSIENT_اندر_ON_CHAR)
	dead_کلید = DEAD_کلید_OFF;

    modifiers = get_فعال_modifiers_experimental();

    ch = simplify_کلید(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_کلیدیں;
    ch = may_adjust_کلید_for_ctrl(modifiers, ch);
    --no_reduce_کلیدیں;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_سرکاؤ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = نکالو_modifiers(ch, &modifiers, FALSE, تہی);
    اگر (ch == CSI)
	ch = K_CSI;

    اگر (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	جوڑو_مقصد_داخلہ_buf(string, 3);
    }

    len = char_مقصد_string(ch, string, 40, FALSE);
    اگر (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_داخلہ_buf();
	got_int = TRUE;
    }

    جوڑو_مقصد_داخلہ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساکن خالی
_OnChar_classic(
    HWND hwnd غیر_مستعمل,
    UINT ch,
    صحیح cRepeat غیر_مستعمل)
{
    char_u	string[40];
    صحیح		len = 0;

    dead_کلید = 0;

    len = char_مقصد_string(ch, string, 40, FALSE);
    اگر (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_داخلہ_buf();
	got_int = TRUE;
    }

    جوڑو_مقصد_داخلہ_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ساکن خالی
_OnSysChar(
    HWND hwnd غیر_مستعمل,
    UINT cch,
    صحیح cRepeat غیر_مستعمل)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_مستعمل->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساکن خالی
_OnSysChar_experimental(
    HWND hwnd غیر_مستعمل,
    UINT cch,
    صحیح cRepeat غیر_مستعمل)
{
    char_u	string[40]; // Enough for multibyte character
    صحیح		len;
    صحیح		modifiers;
    صحیح		ch = cch;   // special keys are negative

    dead_کلید = DEAD_کلید_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_فعال_modifiers_experimental();
    ch = simplify_کلید(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_سرکاؤ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = نکالو_modifiers(ch, &modifiers, FALSE, تہی);
    اگر (ch == CSI)
	ch = K_CSI;

    len = 0;
    اگر (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    اگر (IS_SPECIAL((صحیح)ch))
    {
	string[len++] = CSI;
	string[len++] = K_دوسرا((صحیح)ch);
	string[len++] = K_تیسرا((صحیح)ch);
    }
    ورنہ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_مقصد_string(ch, string + len, 40 - len, TRUE);
    }

    جوڑو_مقصد_داخلہ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساکن خالی
_OnSysChar_classic(
    HWND hwnd غیر_مستعمل,
    UINT cch,
    صحیح cRepeat غیر_مستعمل)
{
    char_u	string[40]; // Enough for multibyte character
    صحیح		len;
    صحیح		modifiers;
    صحیح		ch = cch;   // special keys are negative

    dead_کلید = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_نقاب_ALT;
    اگر (Get_کلید_State(VK_سرکاؤ) & 0x8000)
	modifiers |= MOD_نقاب_سرکاؤ;
    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_نقاب_CTRL;

    ch = simplify_کلید(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_سرکاؤ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = نکالو_modifiers(ch, &modifiers, FALSE, تہی);
    اگر (ch == CSI)
	ch = K_CSI;

    len = 0;
    اگر (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    اگر (IS_SPECIAL((صحیح)ch))
    {
	string[len++] = CSI;
	string[len++] = K_دوسرا((صحیح)ch);
	string[len++] = K_تیسرا((صحیح)ch);
    }
    ورنہ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_مقصد_string(ch, string + len, 40 - len, TRUE);
    }

    جوڑو_مقصد_داخلہ_buf(string, len);
}

    ساکن خالی
_OnMouseEvent(
    صحیح button,
    صحیح x,
    صحیح y,
    صحیح repeated_click,
    UINT کلید_Flags)
{
    صحیح vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    اگر (کلید_Flags & MK_سرکاؤ)
	vim_modifiers |= MOUSE_سرکاؤ;
    اگر (کلید_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    اگر (Get_کلید_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ساکن خالی
On_Mouse_Button_نیچے_کو(
    HWND hwnd غیر_مستعمل,
    BOOL fDoubleClick غیر_مستعمل,
    صحیح x,
    صحیح y,
    UINT کلید_Flags)
{
    ساکن LONG	s_پچھلا_وقت = 0;

    LONG    current_وقت = Get_پیغام_وقت();
    صحیح	    button = -1;
    صحیح	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (خالی)سیٹ_Focus(s_hwnd);

    اگر (s_u_پیغام == WM_LBUTTONDOWN || s_u_پیغام == WM_LBUTTONDBLCLK)
	button = MOUSE_بایاں;
    ورنہ اگر (s_u_پیغام == WM_MBUTTONDOWN || s_u_پیغام == WM_MBUTTONDBLCLK)
	button = MOUSE_درمیان;
    ورنہ اگر (s_u_پیغام == WM_RBUTTONDOWN || s_u_پیغام == WM_RBUTTONDBLCLK)
	button = MOUSE_دایاں;
    ورنہ اگر (s_u_پیغام == WM_XBUTTONDOWN || s_u_پیغام == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    ورنہ اگر (s_u_پیغام == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	اگر (s_button_pending == MOUSE_بایاں)
	    button = MOUSE_دایاں;
	ورنہ
	    button = MOUSE_بایاں;
    }

    اگر (button < 0)
	واپس;

    repeated_click = ((صحیح)(current_وقت - s_پچھلا_وقت) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    اگر (repeated_click
	    && ((button == MOUSE_بایاں && s_button_pending == MOUSE_دایاں)
		|| (button == MOUSE_دایاں
		    && s_button_pending == MOUSE_بایاں)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_درمیان;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, کلید_Flags);
    }
    ورنہ اگر ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_دایاں)))
    {
	اگر (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, کلید_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, کلید_Flags);
    }
    ورنہ
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
	s_kFlags_pending = کلید_Flags;
    }

    s_پچھلا_وقت = current_وقت;
}

    ساکن خالی
_OnMouseMoveOrRelease(
    HWND hwnd غیر_مستعمل,
    صحیح x,
    صحیح y,
    UINT کلید_Flags)
{
    صحیح button;

    s_getting_focus = FALSE;
    اگر (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    اگر (s_u_پیغام == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	اگر (!(کلید_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    واپس;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	سیٹ_Capture(s_متن_Area);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    ورنہ
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, کلید_Flags);
}

    ساکن خالی
On_سائز_متن_Area(
    HWND hwnd غیر_مستعمل,
    UINT state غیر_مستعمل,
    صحیح cx غیر_مستعمل,
    صحیح cy غیر_مستعمل)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	directx_binddc();
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ساکن vimmenu_T *
gui_mswin_ڈھونڈو_menu(
    vimmenu_T	*pMenu,
    صحیح		id)
{
    vimmenu_T	*p_بچہ_Menu;

    جب (pMenu)
    {
	اگر (pMenu->id == (UINT)id)
	    توڑو;
	اگر (pMenu->children != تہی)
	{
	    p_بچہ_Menu = gui_mswin_ڈھونڈو_menu(pMenu->children, id);
	    اگر (p_بچہ_Menu)
	    {
		pMenu = p_بچہ_Menu;
		توڑو;
	    }
	}
	pMenu = pMenu->اگلا;
    }
    واپس pMenu;
}

    ساکن خالی
_OnMenu(
    HWND	hwnd غیر_مستعمل,
    صحیح		id,
    HWND	hwndCtl غیر_مستعمل,
    UINT	رمز_Notify غیر_مستعمل)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_ڈھونڈو_menu(جڑ_menu, id);
    اگر (pMenu)
	gui_menu_cb(pMenu);
}
#اگر_ختم

#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ساکن خالی
On_ڈھونڈو_Repl(خالی)
{
    صحیح	    flags = 0;
    صحیح	    نیچے_کو;

    اگر (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(خالی)سیٹ_Focus(s_hwnd);

    اگر (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(خالی)سیٹ_Focus(s_hwnd);
    }
    ورنہ اگر (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(خالی)سیٹ_Focus(s_hwnd);
    }
    ورنہ اگر (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    اگر (flags == 0)
	واپس;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    اگر (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_پورا_لفظ;
    اگر (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_مطابقت_CASE;
    نیچے_کو = (s_findrep_struct.Flags & FR_نیچے_کو) != 0;
    p = utf16_مقصد_enc(s_findrep_struct.lpstr_ڈھونڈو_What, تہی);
    q = utf16_مقصد_enc(s_findrep_struct.lpstrReplaceWith, تہی);
    اگر (p != تہی && q != تہی)
	gui_do_findrepl(flags, p, q, نیچے_کو);
    vim_free(p);
    vim_free(q);
}
#اگر_ختم

    ساکن خالی
HandleMouseHide(UINT u_پیغام, LPARAM lParam)
{
    ساکن LPARAM آخری_l_Param = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    اگر (u_پیغام == WM_MOUSEMOVE || u_پیغام == WM_NCMOUSEMOVE)
    {
	اگر (lParam == آخری_l_Param)
	    واپس;
	آخری_l_Param = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    انتخاب (u_پیغام)
    {
    حالت WM_KEYUP:
    حالت WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	اگر (p_mh)
	    gui_mch_mousehide(TRUE);
	توڑو;

    حالت WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    حالت WM_SYSCHAR:
    حالت WM_MOUSEMOVE:	 // show the pointer on any mouse action
    حالت WM_LBUTTONDOWN:
    حالت WM_LBUTTONUP:
    حالت WM_MBUTTONDOWN:
    حالت WM_MBUTTONUP:
    حالت WM_RBUTTONDOWN:
    حالت WM_RBUTTONUP:
    حالت WM_XBUTTONDOWN:
    حالت WM_XBUTTONUP:
    حالت WM_NCMOUSEMOVE:
    حالت WM_NCLBUTTONDOWN:
    حالت WM_NCLBUTTONUP:
    حالت WM_NCMBUTTONDOWN:
    حالت WM_NCMBUTTONUP:
    حالت WM_NCRBUTTONDOWN:
    حالت WM_NCRBUTTONUP:
    حالت WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	توڑو;
    }
}

    ساکن LRESULT CALLBACK
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

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    track_user_activity(u_پیغام);
#اگر_ختم

    انتخاب (u_پیغام)
    {
	HANDLE_پیغام(hwnd, WM_LBUTTONDBLCLK,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_LBUTTONDOWN,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_MBUTTONDBLCLK,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_MBUTTONDOWN,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_پیغام(hwnd, WM_RBUTTONDBLCLK,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_RBUTTONDOWN,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_XBUTTONDBLCLK,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_XBUTTONDOWN,On_Mouse_Button_نیچے_کو);
	HANDLE_پیغام(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیغام(hwnd, WM_سائز,	On_سائز_متن_Area);

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
	حالت WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    واپس TRUE;
#اگر_ختم
	طےشدہ:
	    واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
    }
}

    ساکن خالی
dyn_dwm_بھرو(خالی)
{
    hLibDwm = vim_بھرو_Lib("dwmapi.dll");
    اگر (hLibDwm == تہی)
	واپس;

    p_Dwm_سیٹ_کھڑکی_Attribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	Get_Proc_پتہ(hLibDwm, "DwmSetWindowAttribute");

    اگر (p_Dwm_سیٹ_کھڑکی_Attribute == تہی)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = تہی;
	واپس;
    }
}

خارجی DWORD win_نسخہ; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    خالی
gui_mch_سیٹ_titlebar_colors(خالی)
{
#تعریف_کرو DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    اگر (p_Dwm_سیٹ_کھڑکی_Attribute == تہی || win_نسخہ < MAKE_VER(10, 0, 22000))
	واپس;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T متن_Color = DWMWA_COLOR_DEFAULT;

    اگر (vim_strchr(p_go, GO_TITLEBAR) != تہی)
    {
	اگر (gui.اندر_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    متن_Color = gui.title_fg_pixel;
	}
	ورنہ
	{
	    captionColor = gui.titlenc_bg_pixel;
	    متن_Color = gui.titlenc_fg_pixel;
	}

	اگر (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	اگر (متن_Color == INVALCOLOR)
	    متن_Color = DWMWA_COLOR_DEFAULT;
    }

    p_Dwm_سیٹ_کھڑکی_Attribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, حجم(captionColor));
    p_Dwm_سیٹ_کھڑکی_Attribute(s_hwnd, DWMWA_متن_COLOR,
	    &متن_Color, حجم(متن_Color));
}

/*
 * Called when the foreground or background color has been changed.
 */
    خالی
gui_mch_new_colors(خالی)
{
    HBRUSH پچھلا_Brush;

    s_brush = CreateSolidBrush(gui.عقب_pixel);
    پچھلا_Brush = (HBRUSH)سیٹ_Class_Long_Ptr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, تہی, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    اگر (s_textarea_bg_only && s_متن_Area != تہی)
	InvalidateRect(s_متن_Area, تہی, TRUE);
    DeleteObject(پچھلا_Brush);
}

/*
 * Set the colors to their default values.
 */
    خالی
gui_mch_def_colors(خالی)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.عقب_pixel = GetSysColor(COLOR_کھڑکی);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_عقب_pixel = gui.عقب_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(خالی)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    اگر (!Is_کھڑکی_Visible(s_hwnd))
    {
	Show_کھڑکی(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	اگر (s_متن_Area != تہی)
	{
	    Update_کھڑکی(s_متن_Area);
	    gui_mch_flush();
	}
    }

#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#اگر_ختم

    واپس OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    RECT    rect;

    Get_کھڑکی_Rect(s_hwnd, &rect);
    *x = rect.بایاں;
    *y = rect.چوٹی;
    واپس OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    خالی
gui_mch_سیٹ_winpos(صحیح x, صحیح y)
{
    سیٹ_کھڑکی_Pos(s_hwnd, تہی, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    خالی
gui_mch_سیٹ_متن_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    ساکن صحیح oldx = 0;
    ساکن صحیح oldy = 0;

    سیٹ_کھڑکی_Pos(s_متن_Area, تہی, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#اگر_تعریف_شدہ FEAT_TOOLBAR
    اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی)
	Send_پیغام(s_toolbarhwnd, WM_سائز,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_اونچائی));
#اگر_ختم
#اگر defined(FEAT_GUI_TABLINE)
    اگر (showing_tabline)
    {
	صحیح	چوٹی = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی)
	    چوٹی = gui.toolbar_اونچائی;
# endif
	GetClientRect(s_hwnd, &rect);
	Move_کھڑکی(s_tabhwnd, 0, چوٹی, rect.دایاں, gui.tabline_اونچائی, TRUE);
    }
#اگر_ختم

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    اگر (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, تہی, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    خالی
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    صحیح		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    خالی
gui_mch_سیٹ_scrollbar_pos(
    scrollbar_T *sb,
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    سیٹ_کھڑکی_Pos(sb->id, تہی, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    RECT    rcTxt, rcWnd;
    صحیح	    xpad;

    Get_کھڑکی_Rect(s_متن_Area, &rcTxt);
    Get_کھڑکی_Rect(s_hwnd, &rcWnd);
    xpad = rcWnd.دایاں - rcTxt.دایاں - gui.scrollbar_چوڑائی
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    واپس (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    RECT    rcTxt, rcWnd;
    صحیح	    ypad;

    Get_کھڑکی_Rect(s_متن_Area, &rcTxt);
    Get_کھڑکی_Rect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_اونچائی
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    واپس (ypad < 0) ? 0 : ypad;
}

    خالی
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحیح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = Create_کھڑکی(
	"SCROLLBAR", "Scrollbar",
	WS_بچہ | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, تہی,
	g_hinst, تہی);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ساکن scrollbar_T *
gui_mswin_ڈھونڈو_scrollbar(HWND hwnd)
{
    win_T	*wp;

    اگر (gui.bottom_sbar.id == hwnd)
	واپس &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_scrollbars[SBAR_بایاں].id == hwnd)
	    واپس &wp->w_scrollbars[SBAR_بایاں];
	اگر (wp->w_scrollbars[SBAR_دایاں].id == hwnd)
	    واپس &wp->w_scrollbars[SBAR_دایاں];
    }
    واپس تہی;
}

    ساکن خالی
update_scrollbar_سائز(خالی)
{
    gui.scrollbar_چوڑائی = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_اونچائی = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ساکن خالی
Get_اوسط_قدر_Font_سائز(HDC hdc, سائز_3 *سائز)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    Get_متن_Extent_نقطہ(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, سائز);
    سائز->cx = (سائز->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    ساکن خالی
Get_Font_سائز(GuiFont font, صحیح *char_چوڑائی, صحیح *char_اونچائی)
{
    HWND    hwnd = Get_Desktop_کھڑکی();
    HDC	    hdc = Get_کھڑکی_DC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    سائز_3    سائز;
    TEXTMETRIC tm;

    Get_متن_Metrics(hdc, &tm);
    Get_اوسط_قدر_Font_سائز(hdc, &سائز);

    اگر (char_چوڑائی)
	*char_چوڑائی = سائز.cx + tm.tmOverhang;
    اگر (char_اونچائی)
	*char_اونچائی = tm.tm_اونچائی + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ساکن خالی
Update_Font_سائز(GuiFont font)
{
    Get_Font_سائز(font, &gui.char_چوڑائی, &gui.char_اونچائی);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(خالی)
{
    Update_Font_سائز(gui.norm_font);
    واپس OK;
}

    ساکن GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = تہی;

    // Load the font
    font = Create_Font_بالواسطہ_W(lf);

    اگر (font == تہی)
	واپس NOFONT;

    واپس (GuiFont)font;
}

    ساکن صحیح
pixels_مقصد_points(صحیح pixels, صحیح vertical)
{
    صحیح		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = Get_Desktop_کھڑکی();
    hdc = Get_کھڑکی_DC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    واپس points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    صحیح		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    اگر (get_logfont(&lf, name, تہی, giveErrorIfMissing) == OK)
    {
	lf.lf_اونچائی = adjust_fontsize_by_dpi(lf.lf_اونچائی);
	font = get_font_handle(&lf);
    }
    اگر (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    واپس font;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font غیر_مستعمل, char_u *name)
{
    اگر (name == تہی)
	واپس تہی;
    واپس vim_strsave(name);
}
#اگر_ختم

    خالی
gui_mch_free_font(GuiFont font)
{
    اگر (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    صحیح i;

    قسمتعریف ساخت Sys_Color_جدول
    {
	حرف	    *name;
	صحیح	    color;
    } Sys_Color_جدول;

    ساکن Sys_Color_جدول sys_جدول[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#اگر_تعریف_شدہ COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#اگر_ختم
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
	{"SYS_WINDOW", COLOR_کھڑکی},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    برائے (i = 0; i < (صحیح)ARRAY_لمبائی(sys_جدول); i++)
	اگر (STRICMP(name, sys_جدول[i].name) == 0)
	    واپس GetSysColor(sys_جدول[i].color);

    واپس gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    واپس gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برائے (i = 0; special_کلیدیں[i].vim_code1 != NUL; i++)
	اگر (name[0] == special_کلیدیں[i].vim_code0
				       && name[1] == special_کلیدیں[i].vim_code1)
	    واپس OK;
    واپس FAIL;
}

    خالی
gui_mch_beep(خالی)
{
    پیغام_Beep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    خالی
gui_mch_invert_rectangle(
    صحیح	    r,
    صحیح	    c,
    صحیح	    nr,
    صحیح	    nc)
{
    RECT    rc;

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#اگر_ختم

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.بایاں = FILL_X(c);
    rc.چوٹی = FILL_Y(r);
    rc.دایاں = rc.بایاں + nc * gui.char_چوڑائی;
    rc.bottom = rc.چوٹی + nr * gui.char_اونچائی;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    خالی
gui_mch_iconify(خالی)
{
    Show_کھڑکی(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    خالی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#اگر_ختم

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.بایاں = FILL_X(gui.ستون);
    rc.چوٹی = FILL_Y(gui.صف);
    rc.دایاں = rc.بایاں + gui.char_چوڑائی;
    اگر (mb_lefthalve(gui.صف, gui.ستون))
	rc.دایاں += gui.char_چوڑائی;
    rc.bottom = rc.چوٹی + gui.char_اونچائی;
    hbr = CreateSolidBrush(color);
    چوکھٹا_Rect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    خالی
gui_mch_draw_part_cursor(
    صحیح		w,
    صحیح		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.بایاں =
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_دایاں ? FILL_X(gui.ستون + 1) - w :
#اگر_ختم
		    FILL_X(gui.ستون);
    rc.چوٹی = FILL_Y(gui.صف) + gui.char_اونچائی - h;
    rc.دایاں = rc.بایاں + w;
    rc.bottom = rc.چوٹی + h;

    fill_rect(&rc, تہی, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ساکن خالی
output_Dead_کلید_re_Post_Ex(پیغام_3 original_پیغام, صحیح dead_key2set)
{
    ساکن پیغام_3 deadCharExpel;

    اگر (dead_کلید == DEAD_کلید_OFF)
	واپس;

    dead_کلید = dead_key2set;

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
    ساکن خالی
output_Dead_کلید_re_Post(پیغام_3 original_پیغام)
{
    output_Dead_کلید_re_Post_Ex(original_پیغام, DEAD_کلید_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ساکن خالی process_پیغام_usual_کلید(UINT vk, ثابت پیغام_3 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_مستعمل->ptr_process_پیغام_usual_کلید(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساکن خالی
process_پیغام_usual_کلید_experimental(UINT vk, ثابت پیغام_3 *pmsg)
{
    WCHAR	ch[8];
    صحیح		len;
    صحیح		i;
    UINT	جائزہ_رمز;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    اگر (Get_کلید_State(VK_سرکاؤ) & 0x8000)
	keyboard_state[VK_سرکاؤ] = 0x80;
    اگر (Get_کلید_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    اگر ((Get_کلید_State(VK_RMENU) & 0x8000)
				 && (Get_کلید_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    جائزہ_رمز = Map_Virtual_کلید(vk, MAPVK_VK_مقصد_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = مقصد_Unicode(vk, جائزہ_رمز, keyboard_state, ch, ARRAY_لمبائی(ch),
	    0);
    اگر (len < 0)
	dead_کلید = DEAD_کلید_سیٹ_DEFAULT;

    اگر (len <= 0)
    {
	صحیح wm_char = NUL;

	اگر (dead_کلید == DEAD_کلید_سیٹ_DEFAULT
		&& (Get_کلید_State(VK_CONTROL) & 0x8000))
	{
	    اگر (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && جائزہ_رمز == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && جائزہ_رمز == 41))
		wm_char = '[';
	    اگر (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && جائزہ_رمز == 27))
		wm_char = ']';
	}
	اگر (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    Post_پیغام_W(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_کلید = DEAD_کلید_TRANSIENT_اندر_ON_CHAR;
	}
	واپس;
    }

    // Post the message as TranslateMessage would do.
    اگر (pmsg->پیغام_2 == WM_KEYDOWN)
    {
	برائے (i = 0; i < len; i++)
	    Post_پیغام_W(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    ورنہ
    {
	برائے (i = 0; i < len; i++)
	    Post_پیغام_W(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساکن خالی
process_پیغام_usual_کلید_classic(UINT vk, ثابت پیغام_3 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    اگر (vk != 0xff
	    && (Get_کلید_State(VK_CONTROL) & 0x8000)
	    && !(Get_کلید_State(VK_سرکاؤ) & 0x8000)
	    && !(Get_کلید_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	اگر (vk == '6' || Map_Virtual_کلید(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    جوڑو_مقصد_داخلہ_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	ورنہ اگر (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    جوڑو_مقصد_داخلہ_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	ورنہ اگر (vk == '2' || Map_Virtual_کلید(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    جوڑو_مقصد_داخلہ_buf(string, 1);
	}
	ورنہ
	    Translate_پیغام(pmsg);
    }
    ورنہ
	Translate_پیغام(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ساکن خالی
process_پیغام(خالی)
{
    پیغام_3		پیغام;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    صحیح		i;
    صحیح		modifiers = 0;
    صحیح		کلید;
#اگر_تعریف_شدہ FEAT_MENU
    ساکن char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#اگر_ختم
    ساکن صحیح  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    اگر (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    Get_پیغام_W(&پیغام, تہی, 0, 0);

#اگر_تعریف_شدہ FEAT_OLE
    // Look after OLE Automation commands
    اگر (پیغام.پیغام_2 == WM_OLE)
    {
	char_u *str = (char_u *)پیغام.lParam;
	اگر (str == تہی || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    Dispatch_پیغام_W(&پیغام);
	}
	ورنہ
	{
	    جوڑو_مقصد_داخلہ_buf(str, (صحیح)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	واپس;
    }
#اگر_ختم

#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    // Don't process messages used by the dialog
    اگر (s_findrep_hwnd != تہی && Is_Dialog_پیغام_W(s_findrep_hwnd, &پیغام))
    {
	HandleMouseHide(پیغام.پیغام_2, پیغام.lParam);
	واپس;
    }
#اگر_ختم

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    اگر (پیغام.پیغام_2 == WM_KEYDOWN || پیغام.پیغام_2 == WM_SYSKEYDOWN)
    {
	vk = (صحیح) پیغام.wParam;

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
	اگر (keycode_trans_strategy_مستعمل->is_experimental()
		&& dead_کلید == DEAD_کلید_SKIP_ON_CHAR)
	{
	    dead_کلید = DEAD_کلید_OFF;
	}

	اگر (dead_کلید != DEAD_کلید_OFF)
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
	    اگر (keycode_trans_strategy_مستعمل->is_experimental() &&
		    dead_کلید == DEAD_کلید_TRANSIENT_اندر_ON_CHAR)
	    {
		output_Dead_کلید_re_Post_Ex(پیغام,
				       /*dead_key2set=*/DEAD_کلید_SKIP_ON_CHAR);
		واپس;
	    }

	    اگر (dead_کلید != DEAD_کلید_سیٹ_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		واپس;
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
	    اگر ((vk == VK_SPACE || vk == VK_عقب || vk == VK_ESCAPE))
	    {
		dead_کلید = DEAD_کلید_OFF;
		Translate_پیغام(&پیغام);
		واپس;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    ورنہ اگر ((get_real_state()
			    & (منوال_داخل_کرو | منوال_CMDLINE | منوال_SELECT)) == 0)
	    {
		output_Dead_کلید_re_Post(پیغام);
		واپس;
	    }
	}

	// Check for CTRL-BREAK
	اگر (vk == VK_CANCEL)
	{
	    trash_داخلہ_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    جوڑو_مقصد_داخلہ_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	اگر (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    Translate_پیغام(&پیغام);
	    واپس;
	}

	برائے (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    اگر (special_کلیدیں[i].کلید_sym == vk
		    && (vk != VK_SPACE || !(Get_کلید_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		اگر (dead_کلید == DEAD_کلید_سیٹ_DEFAULT
			&& (special_کلیدیں[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_کلید_re_Post(پیغام);
		    واپس;
		}

#اگر_تعریف_شدہ FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		اگر (vk == VK_F10
			&& gui.menu_is_فعال
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  تہی, تہی) == تہی)
		    توڑو;
#اگر_ختم
		modifiers = get_فعال_modifiers_بذریعہ_ptr();

		اگر (special_کلیدیں[i].vim_code1 == NUL)
		    کلید = special_کلیدیں[i].vim_code0;
		ورنہ
		    کلید = مقصد_SPECIAL(special_کلیدیں[i].vim_code0,
						   special_کلیدیں[i].vim_code1);
		کلید = simplify_کلید(کلید, &modifiers);
		اگر (کلید == CSI)
		    کلید = K_CSI;

		اگر (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    جوڑو_مقصد_داخلہ_buf(string, 3);
		}

		اگر (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_دوسرا(کلید);
		    string[2] = K_تیسرا(کلید);
		    جوڑو_مقصد_داخلہ_buf(string, 3);
		}
		ورنہ
		{
		    صحیح	len;

		    // Handle "key" as a Unicode character.
		    len = char_مقصد_string(کلید, string, 40, FALSE);
		    جوڑو_مقصد_داخلہ_buf(string, len);
		}
		توڑو;
	    }
	}

	// Not a special key.
	اگر (special_کلیدیں[i].کلید_sym == 0)
	{
	    process_پیغام_usual_کلید(vk, &پیغام);
	}
    }
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    ورنہ اگر (پیغام.پیغام_2 == WM_IME_NOTIFY)
	_OnImeNotify(پیغام.hwnd, (DWORD)پیغام.wParam, (DWORD)پیغام.lParam);
    ورنہ اگر (پیغام.پیغام_2 == WM_KEYUP && im_get_حالت())
	// added for non-MS IME (Yasuhiro Matsumoto)
	Translate_پیغام(&پیغام);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    اگر (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  تہی, تہی) == تہی)
#اگر_ختم
	Dispatch_پیغام_W(&پیغام);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    خالی
gui_mch_update(خالی)
{
    پیغام_3	    پیغام;

    اگر (!s_busy_processing)
	جب (Peek_پیغام_W(&پیغام, تہی, 0, 0, PM_NOREMOVE)
						  && !vim_is_داخلہ_buf_بھرا())
	    process_پیغام();
}

    ساکن خالی
remove_any_timer(خالی)
{
    پیغام_3		پیغام;

    اگر (s_wait_timer != 0 && !s_timed_برآمد)
    {
	KillTimer(تہی, s_wait_timer);

	// Eat spurious WM_TIMER messages
	جب (Peek_پیغام_W(&پیغام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ساکن garray_T	server_pending_ga = {0, 0, 0, 0, تہی};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    خالی
server_جوڑو_داخلہ(char_u *str)
{
    اگر (server_pending_ga.ga_ڈیٹا == تہی)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ساکن خالی
server_flush_داخلہ(خالی)
{
    char_u  *str;

    اگر (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	واپس;
    str = vim_strnsave(server_pending_ga.ga_ڈیٹا, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    اگر (str != تہی)
    {
	server_مقصد_داخلہ_buf(str);
	vim_free(str);
    }
}
#اگر_ختم

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    صحیح
gui_mch_wait_for_chars(طویل wtime)
{
    صحیح		focus;

    s_timed_برآمد = FALSE;

    اگر (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	اگر (s_busy_processing)
	    واپس FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = سیٹ_Timer(تہی, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.اندر_focus;
    جب (!s_timed_برآمد)
    {
	// Stop or start blinking when focus changes
	اگر (gui.اندر_focus != focus)
	{
	    اگر (gui.اندر_focus)
		gui_mch_آغاز_blink();
	    ورنہ
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندر_focus;
	}

	اگر (s_need_activate)
	{
	    (خالی)سیٹ_Foreground_کھڑکی(s_hwnd);
	    s_need_activate = FALSE;
	}

#اگر_تعریف_شدہ FEAT_TIMERS
	did_جوڑو_timer = FALSE;
#اگر_ختم
#اگر_تعریف_شدہ پیغام_قطار
	// Check channel I/O while waiting for a message.
	برائے (;;)
	{
	    پیغام_3 پیغام;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_داخلہ();
# endif
# ifdef FEAT_TIMERS
	    اگر (did_جوڑو_timer)
		توڑو;
# endif
	    اگر (Peek_پیغام_W(&پیغام, تہی, 0, 0, PM_NOREMOVE))
	    {
		process_پیغام();
		توڑو;
	    }
	    ورنہ اگر (داخلہ_دستیاب()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || پیغام_Wait_For_مضرب_Objects(0, تہی, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		توڑو;
	}
#ورنہ
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_پیغام();
#اگر_ختم

	اگر (داخلہ_دستیاب())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    اگر (!s_getting_focus)
		s_button_pending = -1;

	    واپس OK;
	}

#اگر_تعریف_شدہ FEAT_TIMERS
	اگر (did_جوڑو_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    توڑو;
	}
#اگر_ختم
    }
    allow_scrollbar = FALSE;
    واپس FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    خالی
gui_mch_clear_بلاک(
    صحیح		row1,
    صحیح		col1,
    صحیح		row2,
    صحیح		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.بایاں = FILL_X(col1);
    rc.چوٹی = FILL_Y(row1);
    rc.دایاں = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    خالی
gui_mch_clear_all(خالی)
{
    RECT    rc;

    rc.بایاں = 0;
    rc.چوٹی = 0;
    rc.دایاں = Columns * gui.char_چوڑائی + 2 * gui.border_چوڑائی;
    rc.bottom = صفیں * gui.char_اونچائی + 2 * gui.border_چوڑائی;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    خالی
gui_mch_enable_menu(صحیح flag)
{
#اگر_تعریف_شدہ FEAT_MENU
    سیٹ_Menu(s_hwnd, flag ? s_menuBar : تہی);
#اگر_ختم
}

    خالی
gui_mch_سیٹ_menu_pos(
    صحیح	    x غیر_مستعمل,
    صحیح	    y غیر_مستعمل,
    صحیح	    w غیر_مستعمل,
    صحیح	    h غیر_مستعمل)
{
    // It will be in the right place anyway
}

#اگر defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    خالی
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    صحیح		hidden)
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
    خالی
gui_mch_draw_menubar(خالی)
{
    DrawMenuBar(s_hwnd);
}
#اگر_ختم // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    واپس (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#اگر defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ساکن لفظ_2
Pixel_مقصد_Dialog_X(صحیح عدد_Pixels)
{
    واپس (لفظ_2)((عدد_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ساکن لفظ_2
Pixel_مقصد_Dialog_Y(صحیح عدد_Pixels)
{
    واپس (لفظ_2)((عدد_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ساکن صحیح
Get_متن_چوڑائی(HDC hdc, char_u *str, صحیح len)
{
    سائز_3    سائز;

    Get_متن_Extent_نقطہ(hdc, (LPCSTR)str, len, &سائز);
    واپس سائز.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ساکن صحیح
Get_متن_چوڑائی_Enc(HDC hdc, char_u *str, صحیح len)
{
    سائز_3	سائز;
    WCHAR	*wstr;
    صحیح		n;
    صحیح		wlen = len;

    wstr = enc_مقصد_utf16(str, &wlen);
    اگر (wstr == تہی)
	واپس 0;

    n = Get_متن_Extent_نقطہ_W(hdc, wstr, wlen, &سائز);
    vim_free(wstr);
    اگر (n)
	واپس سائز.cx;
    واپس 0;
}

ساکن خالی get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ساکن BOOL
Center_کھڑکی(
    HWND hwnd_بچہ,
    HWND hwnd_والد)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    r_بچہ, r_والد, rScreen;
    صحیح		    w_بچہ, h_بچہ, w_والد, h_والد;
    صحیح		    xNew, yNew;
    HDC		    hdc;

    Get_کھڑکی_Rect(hwnd_بچہ, &r_بچہ);
    w_بچہ = r_بچہ.دایاں - r_بچہ.بایاں;
    h_بچہ = r_بچہ.bottom - r_بچہ.چوٹی;

    // If Vim is minimized put the window in the middle of the screen.
    اگر (hwnd_والد == تہی || IsMinimized(hwnd_والد))
	get_work_area(&r_والد);
    ورنہ
	Get_کھڑکی_Rect(hwnd_والد, &r_والد);
    w_والد = r_والد.دایاں - r_والد.بایاں;
    h_والد = r_والد.bottom - r_والد.چوٹی;

    moninfo.cb_سائز = حجم(MONITORINFO);
    mon = Monitor_From_کھڑکی(hwnd_بچہ, MONITOR_DEFAULTTOPRIMARY);
    اگر (mon != تہی && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    ورنہ
    {
	hdc = GetDC(hwnd_بچہ);
	rScreen.بایاں = 0;
	rScreen.چوٹی = 0;
	rScreen.دایاں = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwnd_بچہ, hdc);
    }

    xNew = r_والد.بایاں + ((w_والد - w_بچہ) / 2);
    اگر (xNew < rScreen.بایاں)
	xNew = rScreen.بایاں;
    ورنہ اگر ((xNew + w_بچہ) > rScreen.دایاں)
	xNew = rScreen.دایاں - w_بچہ;

    yNew = r_والد.چوٹی + ((h_والد - h_بچہ) / 2);
    اگر (yNew < rScreen.چوٹی)
	yNew = rScreen.چوٹی;
    ورنہ اگر ((yNew + h_بچہ) > rScreen.bottom)
	yNew = rScreen.bottom - h_بچہ;

    واپس سیٹ_کھڑکی_Pos(hwnd_بچہ, تہی, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#اگر_ختم // FEAT_GUI_DIALOG

#اگر defined(FEAT_TOOLBAR)
    خالی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (s_toolbarhwnd == تہی)
	واپس;

    اگر (showit)
    {
	// Enable unicode support
	Send_پیغام(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	Show_کھڑکی(s_toolbarhwnd, SW_SHOW);
    }
    ورنہ
	Show_کھڑکی(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_گنتی 31

#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE)
    ساکن خالی
جوڑو_tabline_popup_menu_اندراج(HMENU pmenu, UINT شے_id, char_u *شے_متن)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_مقصد_utf16(شے_متن, تہی);
    اگر (wn == تہی)
	واپس;

    infow.cb_سائز = حجم(infow);
    infow.f_نقاب = MIIM_TYPE | MIIM_ID;
    infow.wID = شے_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_ڈیٹا = wn;
    infow.cch = (UINT)wcslen(wn);
    داخل_کرو_Menu_شے_W(pmenu, شے_id, FALSE, &infow);
    vim_free(wn);
}

    ساکن خالی
show_tabline_popup_menu(خالی)
{
    HMENU	    tab_pmenu;
    طویل	    rval;
    نقطہ_2	    pt;

    // When ignoring events don't show the menu.
    اگر (hold_gui_events || cmdwin_type != 0)
	واپس;

    tab_pmenu = CreatePopupMenu();
    اگر (tab_pmenu == تہی)
	واپس;

    اگر (پہلا_tabpage->tp_اگلا != تہی)
	جوڑو_tabline_popup_menu_اندراج(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    جوڑو_tabline_popup_menu_اندراج(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    جوڑو_tabline_popup_menu_اندراج(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									تہی);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    اگر (rval > 0)
    {
	TCHITTESTINFO htinfo;
	صحیح idx;

	اگر (Screen_مقصد_Client(s_tabhwnd, &pt) == 0)
	    واپس;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = Tab_Ctrl_کامیابی_Test(s_tabhwnd, &htinfo);
	اگر (idx == -1)
	    idx = 0;
	ورنہ
	    idx += 1;

	send_tabline_menu_event(idx, (صحیح)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    خالی
gui_mch_show_tabline(صحیح showit)
{
    اگر (s_tabhwnd == تہی)
	واپس;

    اگر (!showit != !showing_tabline)
    {
	اگر (showit)
	    Show_کھڑکی(s_tabhwnd, SW_SHOW);
	ورنہ
	    Show_کھڑکی(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(خالی)
{
    واپس s_tabhwnd != تہی && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    خالی
gui_mch_update_tabline(خالی)
{
    tabpage_T	*tp;
    TCITEM	tie;
    صحیح		nr = 0;
    صحیح		curtabidx = 0;
    صحیح		tabadded = 0;
    WCHAR	*wstr = تہی;

    اگر (s_tabhwnd == تہی)
	واپس;

    // Enable unicode support
    Send_پیغام(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.نقاب = TCIF_متن;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    Send_پیغام(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	اگر (nr >= Tab_Ctrl_Get_شے_گنتی(s_tabhwnd))
	{
	    // Add the tab
	    tie.psz_متن = "-Empty-";
	    Tab_Ctrl_داخل_کرو_شے(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.psz_متن = (LPSTR)NameBuff;

	wstr = enc_مقصد_utf16(NameBuff, تہی);
	اگر (wstr != تہی)
	{
	    TCITEMW		tiw;

	    tiw.نقاب = TCIF_متن;
	    tiw.iImage = -1;
	    tiw.psz_متن = wstr;
	    Send_پیغام(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    جب (nr < Tab_Ctrl_Get_شے_گنتی(s_tabhwnd))
	Tab_Ctrl_Delete_شے(s_tabhwnd, nr);

    اگر (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_سیٹ_Cur_Sel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    Send_پیغام(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    Redraw_کھڑکی(s_tabhwnd, تہی, تہی,
		    RDW_ERASE | RDW_چوکھٹا | RDW_INVALIDATE | RDW_ALLCHILDREN);

    اگر (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_سیٹ_Cur_Sel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    خالی
gui_mch_سیٹ_curtab(صحیح nr)
{
    اگر (s_tabhwnd == تہی)
	واپس;

    اگر (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	Tab_Ctrl_سیٹ_Cur_Sel(s_tabhwnd, nr - 1);
}

#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME
    خالی
gui_mch_سیٹ_dark_theme(صحیح dark)
{
    اگر (p_Dwm_سیٹ_کھڑکی_Attribute != تہی && win_نسخہ >= MAKE_VER(10, 0, 18985))
	p_Dwm_سیٹ_کھڑکی_Attribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_منوال, &dark,
		حجم(dark));

    اگر (p_سیٹ_Preferred_App_منوال != تہی && win_نسخہ >= MAKE_VER(10, 0, 18362))
	p_سیٹ_Preferred_App_منوال(dark);

    اگر (pFlushMenuThemes != تہی && win_نسخہ >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ساکن خالی
dyn_uxtheme_بھرو(خالی)
{
    hUxThemeLib = vim_بھرو_Lib("uxtheme.dll");
    اگر (hUxThemeLib == تہی)
	واپس;

    p_سیٹ_Preferred_App_منوال = (DWORD (WINAPI *)(DWORD))
	Get_Proc_پتہ(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (خالی (WINAPI *)(خالی))
	Get_Proc_پتہ(hUxThemeLib, MAKEINTRESOURCE(136));

    اگر (p_سیٹ_Preferred_App_منوال == تہی || pFlushMenuThemes == تہی)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = تہی;
	واپس;
    }
}

#اگر_ختم // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    خالی
gui_mch_سیٹ_fullscreen(صحیح flag)
{
    ساکن RECT معمول_rect;
    ساکن LONG_PTR معمول_style, معمول_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    اگر (!بھرا_screen) // Windows not set yet.
	واپس;

    اگر (flag)
    {
	اگر (fullscreen_on)
	    واپس;

	// Enter fullscreen mode
	Get_کھڑکی_Rect(s_hwnd, &rc);

	moninfo.cb_سائز = حجم(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	اگر (mon == تہی || !GetMonitorInfo(mon, &moninfo))
	    واپس;

	// Save current window state
	Get_کھڑکی_Rect(s_hwnd, &معمول_rect);
	معمول_style = Get_کھڑکی_Long_Ptr(s_hwnd, GWL_STYLE);
	معمول_exstyle = Get_کھڑکی_Long_Ptr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	سیٹ_کھڑکی_Long_Ptr(s_hwnd, GWL_STYLE,
		معمول_style & ~(WS_CAPTION | WS_THICKFRAME));
	سیٹ_کھڑکی_Long_Ptr(s_hwnd, GWL_EXSTYLE,
		معمول_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	سیٹ_کھڑکی_Pos(s_hwnd, تہی,
		moninfo.rcMonitor.بایاں,
		moninfo.rcMonitor.چوٹی,
		moninfo.rcMonitor.دایاں - moninfo.rcMonitor.بایاں,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.چوٹی,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    ورنہ
    {
	اگر (!fullscreen_on)
	    واپس;

	// Exit fullscreen mode
	سیٹ_کھڑکی_Long_Ptr(s_hwnd, GWL_STYLE, معمول_style);
	سیٹ_کھڑکی_Long_Ptr(s_hwnd, GWL_EXSTYLE, معمول_exstyle);

	// Restore original window position and size
	سیٹ_کھڑکی_Pos(s_hwnd, تہی,
		معمول_rect.بایاں,
		معمول_rect.چوٹی,
		معمول_rect.دایاں - معمول_rect.بایاں,
		معمول_rect.bottom - معمول_rect.چوٹی,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    خالی
ex_simalt(exarg_T *eap)
{
    char_u	*کلیدیں = eap->arg;
    صحیح		fill_typebuf = FALSE;
    char_u	کلید_name[4];

    Post_پیغام(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    جب (*کلیدیں)
    {
	اگر (*کلیدیں == '~')
	    *کلیدیں = ' ';	    // for showing system menu
	Post_پیغام(s_hwnd, WM_CHAR, (WPARAM)*کلیدیں, (LPARAM)0);
	کلیدیں++;
	fill_typebuf = TRUE;
    }
    اگر (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	کلید_name[0] = K_SPECIAL;
	کلید_name[1] = KS_EXTRA;
	کلید_name[2] = KE_NOP;
	کلید_name[3] = NUL;
#اگر defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#اگر_ختم
	(خالی)ins_typebuf(کلید_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    ساکن خالی
initialise_findrep(char_u *initial_string)
{
    صحیح		wword = FALSE;
    صحیح		mcase = !p_ic;
    char_u	*اندراج_متن;

    // Get the search string to use.
    اندراج_متن = get_ڈھونڈو_dialog_متن(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_نیچے_کو;
    اگر (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    اگر (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    اگر (اندراج_متن != تہی && *اندراج_متن != NUL)
    {
	WCHAR *p = enc_مقصد_utf16(اندراج_متن, تہی);
	اگر (p != تہی)
	{
	    صحیح len = s_findrep_struct.w_ڈھونڈو_What_Len - 1;

	    wcsncpy(s_findrep_struct.lpstr_ڈھونڈو_What, p, len);
	    s_findrep_struct.lpstr_ڈھونڈو_What[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(اندراج_متن);
}
#اگر_ختم

    ساکن خالی
سیٹ_کھڑکی_title(HWND hwnd, حرف *title)
{
    اگر (title != تہی)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_مقصد_utf16((char_u *)title, تہی);
	اگر (wbuf != تہی)
	{
	    سیٹ_کھڑکی_متن_W(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    ورنہ
	(خالی)سیٹ_کھڑکی_متن_W(hwnd, تہی);
}

    خالی
gui_mch_ڈھونڈو_dialog(exarg_T *eap)
{
#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    اگر (s_findrep_پیغام != 0)
    {
	اگر (Is_کھڑکی(s_findrep_hwnd) && !s_findrep_is_ڈھونڈو)
	    Destroy_کھڑکی(s_findrep_hwnd);

	اگر (!Is_کھڑکی(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ڈھونڈو_متن_W(&s_findrep_struct);
	}

	سیٹ_کھڑکی_title(s_findrep_hwnd, _("Find string"));
	(خالی)سیٹ_Focus(s_findrep_hwnd);

	s_findrep_is_ڈھونڈو = TRUE;
    }
#اگر_ختم
}


    خالی
gui_mch_replace_dialog(exarg_T *eap)
{
#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    اگر (s_findrep_پیغام != 0)
    {
	اگر (Is_کھڑکی(s_findrep_hwnd) && s_findrep_is_ڈھونڈو)
	    Destroy_کھڑکی(s_findrep_hwnd);

	اگر (!Is_کھڑکی(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = Replace_متن_W(&s_findrep_struct);
	}

	سیٹ_کھڑکی_title(s_findrep_hwnd, _("Find & Replace"));
	(خالی)سیٹ_Focus(s_findrep_hwnd);

	s_findrep_is_ڈھونڈو = FALSE;
    }
#اگر_ختم
}


/*
 * Set visibility of the pointer.
 */
    خالی
gui_mch_mousehide(صحیح hide)
{
    اگر (hide == gui.اشارہ_گر_hidden)
	واپس;

    ShowCursor(!hide);
    gui.اشارہ_گر_hidden = hide;
}

#اگر_تعریف_شدہ FEAT_MENU
    ساکن خالی
gui_mch_show_popupmenu_at(vimmenu_T *menu, صحیح x, صحیح y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (خالی)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(صحیح)0,	    //reserved param
	s_hwnd,
	تہی);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#اگر_ختم

/*
 * Got a message when the system will go down.
 */
    ساکن خالی
On_انتہا_Session(خالی)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ساکن خالی
_OnClose(HWND hwnd غیر_مستعمل)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ساکن خالی
_OnDestroy(HWND hwnd)
{
    اگر (!destroying)
	_OnClose(hwnd);
}

    ساکن خالی
_OnPaint(
    HWND hwnd)
{
    اگر (IsMinimized(hwnd))
	واپس;

    PAINTSTRUCT ps;

    برآمد_flush();	    // make sure all output has been processed
    (خالی)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    اگر (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.بایاں = rect.بایاں;
	ps.rcPaint.دایاں = rect.دایاں;
    }

    اگر (!Is_Rect_خالی_خانے(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	اگر (!screen_cleared || Screen_سطریں == تہی)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	ورنہ
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.بایاں, ps.rcPaint.چوٹی,
		ps.rcPaint.دایاں - ps.rcPaint.بایاں + 1,
		ps.rcPaint.bottom - ps.rcPaint.چوٹی + 1);
    }

    انتہا_Paint(hwnd, &ps);
}

    ساکن خالی
On_سائز(
    HWND hwnd,
    UINT state غیر_مستعمل,
    صحیح cx,
    صحیح cy)
{
    اگر (!IsMinimized(hwnd) && !s_اندر_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_اونچائی(TRUE);
    }
}

    ساکن خالی
On_سیٹ_Focus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (خالی)Def_کھڑکی_Proc_W(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ساکن خالی
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    اگر (destroying)
	واپس;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (خالی)Def_کھڑکی_Proc_W(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ساکن LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    واپس Def_کھڑکی_Proc_W(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    خالی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    Destroy_کھڑکی(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    RECT rct;
    نقطہ_2 mp;

    (خالی)Get_کھڑکی_Rect(s_متن_Area, &rct);
    (خالی)GetCursorPos(&mp);
    *x = (صحیح)(mp.x - rct.بایاں);
    *y = (صحیح)(mp.y - rct.چوٹی);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    خالی
gui_mch_setmouse(صحیح x, صحیح y)
{
    RECT rct;

    (خالی)Get_کھڑکی_Rect(s_متن_Area, &rct);
    (خالی)سیٹ_Cursor_Pos(x + gui.border_انحراف + rct.بایاں,
		       y + gui.border_انحراف + rct.چوٹی);
}

    ساکن خالی
gui_mswin_get_valid_dimensions(
    صحیح w,
    صحیح h,
    صحیح *valid_w,
    صحیح *valid_h,
    صحیح *ستون_تعداد,
    صحیح *صفیں_2)
{
    صحیح	    بنیاد_چوڑائی, بنیاد_اونچائی;

    بنیاد_چوڑائی = gui_get_بنیاد_چوڑائی()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    بنیاد_اونچائی = gui_get_بنیاد_اونچائی()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_اونچائی(FALSE);
    *ستون_تعداد = (w - بنیاد_چوڑائی) / gui.char_چوڑائی;
    *صفیں_2 = (h - بنیاد_اونچائی) / gui.char_اونچائی;
    *valid_w = بنیاد_چوڑائی + *ستون_تعداد * gui.char_چوڑائی;
    *valid_h = بنیاد_اونچائی + *صفیں_2 * gui.char_اونچائی;
}

    خالی
gui_mch_flash(صحیح msec)
{
    RECT    rc;

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#اگر_ختم

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.بایاں = 0;
    rc.چوٹی = 0;
    rc.دایاں = gui.عدد_ستون_تعداد * gui.char_چوڑائی;
    rc.bottom = gui.عدد_صفیں * gui.char_اونچائی;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((طویل)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ساکن BOOL
is_نقطہ_onscreen(صحیح x, صحیح y)
{
    نقطہ_2   pt = {x, y};

    واپس Monitor_From_نقطہ(pt, MONITOR_DEFAULTTONULL) != تہی;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ساکن BOOL
is_کھڑکی_onscreen(HWND hwnd)
{
    RECT    rc;
    نقطہ_2   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.بایاں;
    p1.y = rc.چوٹی;
    p2.x = rc.دایاں - 1;
    p2.y = rc.bottom - 1;
    Client_مقصد_Screen(hwnd, &p1);
    Client_مقصد_Screen(hwnd, &p2);

    اگر (!is_نقطہ_onscreen(p1.x, p1.y))
	واپس FALSE;
    اگر (!is_نقطہ_onscreen(p1.x, p2.y))
	واپس FALSE;
    اگر (!is_نقطہ_onscreen(p2.x, p1.y))
	واپس FALSE;
    اگر (!is_نقطہ_onscreen(p2.x, p2.y))
	واپس FALSE;
    واپس TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ساکن صحیح
get_scroll_flags(خالی)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    اگر (!is_کھڑکی_onscreen(s_hwnd))
	واپس SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    Get_کھڑکی_Rect(s_hwnd, &rcVim);
    برائے (hwnd = s_hwnd; (hwnd = Get_کھڑکی(hwnd, GW_HWNDPREV)) != (HWND)0; )
	اگر (Is_کھڑکی_Visible(hwnd))
	{
	    Get_کھڑکی_Rect(hwnd, &rcOther);
	    اگر (IntersectRect(&rcDest, &rcVim, &rcOther))
		واپس SW_INVALIDATE;
	}
    واپس 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ساکن خالی
intel_gpu_workaround(خالی)
{
    GetPixel(s_hdc, FILL_X(gui.ستون), FILL_Y(gui.صف));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    خالی
gui_mch_delete_سطریں(
    صحیح	    صف,
    صحیح	    عدد_سطریں)
{
    RECT	rc;

    rc.بایاں = FILL_X(gui.scroll_region_بایاں);
    rc.دایاں = FILL_X(gui.scroll_region_دایاں + 1);
    rc.چوٹی = FILL_Y(صف);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX() && is_کھڑکی_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -عدد_سطریں * gui.char_اونچائی, &rc);
    }
    ورنہ
#اگر_ختم
    {
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#اگر_ختم
	intel_gpu_workaround();
	Scroll_کھڑکی_Ex(s_متن_Area, 0, -عدد_سطریں * gui.char_اونچائی,
				    &rc, &rc, تہی, تہی, get_scroll_flags());
	Update_کھڑکی(s_متن_Area);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
						       gui.scroll_region_بایاں,
	gui.scroll_region_bot, gui.scroll_region_دایاں);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    خالی
gui_mch_داخل_کرو_سطریں(
    صحیح		صف,
    صحیح		عدد_سطریں)
{
    RECT	rc;

    rc.بایاں = FILL_X(gui.scroll_region_بایاں);
    rc.دایاں = FILL_X(gui.scroll_region_دایاں + 1);
    rc.چوٹی = FILL_Y(صف);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX() && is_کھڑکی_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, عدد_سطریں * gui.char_اونچائی, &rc);
    }
    ورنہ
#اگر_ختم
    {
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#اگر_ختم
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	Scroll_کھڑکی_Ex(s_متن_Area, 0, عدد_سطریں * gui.char_اونچائی,
				    &rc, &rc, تہی, تہی, get_scroll_flags());
	Update_کھڑکی(s_متن_Area);
    }

    gui_clear_بلاک(صف, gui.scroll_region_بایاں,
				صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
}


    خالی
gui_mch_exit(صحیح rc غیر_مستعمل)
{
#اگر defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = تہی;
#اگر_ختم

    ReleaseDC(s_متن_Area, s_hdc);
    DeleteObject(s_brush);

#اگر_تعریف_شدہ FEAT_TEAROFF
    // Unload the tearoff bitmap
    (خالی)DeleteObject((HGDIOBJ)s_htearbitmap);
#اگر_ختم

    // Destroy our window (if we have one).
    اگر (s_hwnd != تہی)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	Destroy_کھڑکی(s_hwnd);
    }
}

    ساکن char_u *
logfont2name(LOGFONTW lf)
{
    حرف	*charset_name;
    حرف	*quality_name;
    حرف	*font_name;
    size_t	res_سائز;
    حرف	*res;

    font_name = (حرف *)utf16_مقصد_enc(lf.lfFaceName, تہی);
    اگر (font_name == تہی)
	واپس تہی;
    charset_name = charset_id2name((صحیح)lf.lf_Char_سیٹ);
    quality_name = quality_id2name((صحیح)lf.lfQuality);

    res_سائز = STRLEN(font_name) + 30
		    + (charset_name == تہی ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == تہی ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_سائز);
    اگر (res != تہی)
    {
	حرف	*p;
	صحیح	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	برائے (p = font_name; *p != NUL; ++p)
	{
	    اگر (خلا_ہے(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_مقصد_points(
			 lf.lf_اونچائی < 0 ? -lf.lf_اونچائی : lf.lf_اونچائی, TRUE);
	اگر (lf.lf_وزن == FW_معمول || lf.lf_وزن == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_سائز, "%s:h%d", font_name, points);
	ورنہ
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_سائز, "%s:h%d:W%ld", font_name, points, lf.lf_وزن);

	res_len += vim_snprintf_safelen(
	    (حرف *)res + res_len,
	    res_سائز - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lf_وزن == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lf_Strike_برآمد ? ":s" : "");

	اگر (charset_name != تہی)
	    res_len += vim_snprintf_safelen((حرف *)res + res_len,
		res_سائز - res_len, ":c%s", charset_name);
	اگر (quality_name != تہی)
	    vim_snprintf((حرف *)res + res_len,
		res_سائز - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    واپس (char_u *)res;
}


#اگر_تعریف_شدہ FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ساکن خالی
update_im_font(خالی)
{
    LOGFONTW	lf_wide, lf;

    اگر (p_guifontwide != تہی && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, حجم(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    ورنہ
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lf_اونچائی = lf.lf_اونچائی * DEFAULT_DPI / s_dpi;
    im_سیٹ_font(&lf);
}
#اگر_ختم

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    خالی
gui_mch_wide_font_بدلا(خالی)
{
    LOGFONTW lf;

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    update_im_font();
#اگر_ختم

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    اگر (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, حجم(lf), &lf))
    {
	اگر (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	اگر (lf.lf_وزن < FW_BOLD)
	{
	    lf.lf_وزن = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    اگر (!lf.lfItalic)
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
    صحیح
gui_mch_init_font(char_u *font_name, صحیح fontset غیر_مستعمل)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    اگر (get_logfont(&lf, font_name, تہی, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lf_اونچائی = adjust_fontsize_by_dpi(lf.lf_اونچائی);
	font = get_font_handle(&lf);
    }
    اگر (font == NOFONT)
	واپس FAIL;

#اگر defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	صحیح		    feat_گنتی = 0;
	char_u		    *fp;

	اگر (font_name != تہی)
	{
	    // Find each ":f" option in font_name.
	    برائے (fp = font_name; *fp != NUL; fp++)
	    {
		اگر (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u لیبل[5];
		    صحیح    ti = 0;
		    غیرعلامتی صحیح param = 1;

		    fp += 2;  // skip ":f"
		    جب (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			لیبل[ti++] = *fp++;
		    لیبل[ti] = NUL;

		    اگر (ti != 4)
			جاری;  // invalid tag length

		    اگر (*fp == '=')
		    {
			fp++;
			param = (غیرعلامتی صحیح)atoi((حرف *)fp);
			جب (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    اگر (feat_گنتی < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_گنتی].لیبل =
			    ((غیرعلامتی صحیح)لیبل[0])
			    | ((غیرعلامتی صحیح)لیبل[1] << 8)
			    | ((غیرعلامتی صحیح)لیبل[2] << 16)
			    | ((غیرعلامتی صحیح)لیبل[3] << 24);
			features[feat_گنتی].parameter = param;
			feat_گنتی++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWrite_Context_سیٹ_Font_Features(s_dwc, features, feat_گنتی);
    }
#اگر_ختم

    اگر (font_name == تہی)
	font_name = (char_u *)"";
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    اگر (!s_اندر_dpichanged)
	update_im_font();
#اگر_ختم
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_اونچائی = lfOrig.lf_اونچائی;
    Update_Font_سائز(font);

    p = logfont2name(lfOrig);
    اگر (p != تہی)
    {
	hl_سیٹ_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	اگر (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	ورنہ
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    اگر (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    اگر (lf.lf_وزن < FW_BOLD)
    {
	lf.lf_وزن = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	اگر (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    واپس OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    صحیح
gui_mch_maximized(خالی)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.لمبائی = حجم(WINDOWPLACEMENT);
    اگر (Get_کھڑکی_Placement(s_hwnd, &wp))
    {
	اگر (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    واپس TRUE;
	اگر (wp.showCmd == SW_SHOWMINIMIZED)
	    واپس FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	Get_کھڑکی_Rect(s_hwnd, &rc);
	اگر ((rc.دایاں - rc.بایاں !=
		    wp.rc_معمول_Position.دایاں - wp.rc_معمول_Position.بایاں)
		|| (rc.bottom - rc.چوٹی !=
		    wp.rc_معمول_Position.bottom - wp.rc_معمول_Position.چوٹی))
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    خالی
gui_mch_newfont(خالی)
{
    RECT	rect;

    Get_کھڑکی_Rect(s_hwnd, &rect);
    اگر (win_socket_id == 0)
    {
	gui_resize_shell(rect.دایاں - rect.بایاں
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.چوٹی
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_اونچائی(FALSE));
    }
    ورنہ
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.دایاں - rect.بایاں,
	    rect.bottom - rect.چوٹی - gui_mswin_get_menu_اونچائی(FALSE));
    }
}

/*
 * Set the window title
 */
    خالی
gui_mch_settitle(
    char_u  *title,
    char_u  *icon غیر_مستعمل)
{
    سیٹ_کھڑکی_title(s_hwnd, (title == تہی ? "VIM" : (حرف *)title));
}

#اگر defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساکن LPCSTR mshape_idcs[] =
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

    خالی
mch_سیٹ_mouse_shape(صحیح shape)
{
    LPCSTR idc;

    اگر (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    ورنہ
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	ورنہ
	    idc = mshape_idcs[shape];
	سیٹ_Class_Long_Ptr(s_متن_Area, GCLP_HCURSOR, (LONG_PTR)بھرو_Cursor(تہی, idc));
	اگر (!p_mh)
	{
	    نقطہ_2 mp;

	    // Set the position to make it redrawn with the new shape.
	    (خالی)GetCursorPos(&mp);
	    (خالی)سیٹ_Cursor_Pos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#اگر_ختم

#اگر defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ساکن WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    صحیح len;
    WCHAR *res;

    tmp = convert_filter(s);
    اگر (tmp == تہی)
	واپس تہی;
    len = (صحیح)STRLEN(s) + 3;
    res = enc_مقصد_utf16(tmp, &len);
    vim_free(tmp);
    واپس res;
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
	صحیح saving,
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
    صحیح			i;
    WCHAR		*titlep = تہی;
    WCHAR		*extp = تہی;
    WCHAR		*initdirp = تہی;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    اگر (dflt == تہی)
	fileBuf[0] = NUL;
    ورنہ
    {
	wp = enc_مقصد_utf16(dflt, تہی);
	اگر (wp == تہی)
	    fileBuf[0] = NUL;
	ورنہ
	{
	    برائے (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_میدان(fileStruct);
# ifdef OPENFILENAME_سائز_نسخہ_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_سائز = OPENFILENAME_سائز_نسخہ_400_W;
# ورنہ
    fileStruct.l_Struct_سائز = حجم(fileStruct);
# endif

    اگر (title != تہی)
	titlep = enc_مقصد_utf16(title, تہی);
    fileStruct.lpstrTitle = titlep;

    اگر (ext != تہی)
	extp = enc_مقصد_utf16(ext, تہی);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    اگر (initdir != تہی && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_مقصد_utf16(initdir, تہی);
	اگر (initdirp != تہی)
	{
	    برائے (wp = initdirp; *wp != NUL; ++wp)
		اگر (*wp == '/')
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
    اگر (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    اگر (saving)
	ret = GetSaveFileNameW(&fileStruct);
    ورنہ
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    اگر (!ret)
	واپس تہی;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_مقصد_enc(fileBuf, تہی);
    اگر (p == تہی)
	واپس تہی;

    // Give focus back to main window (when using MDI).
    سیٹ_Focus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    واپس q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ساکن char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    غیرعلامتی	s_len = (غیرعلامتی)STRLEN(s);
    غیرعلامتی	i;

    res = alloc(s_len + 3);
    اگر (res != تہی)
    {
	برائے (i = 0; i < s_len; ++i)
	    اگر (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    ورنہ
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    واپس res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    واپس gui_mch_browse(0, title, (char_u *)_("Not Used"), تہی,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#اگر_ختم // FEAT_BROWSE

    ساکن خالی
_OnDropFiles(
    HWND hwnd غیر_مستعمل,
    HDROP hDrop)
{
#تعریف_کرو BUFPATHLEN MAX_راستہ_2
#تعریف_کرو DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    حرف    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, تہی, 0);
    UINT    i;
    char_u  **fnames;
    نقطہ_2   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    Drag_Query_نقطہ(hDrop, &pt);
    Map_کھڑکی_Points(s_hwnd, s_متن_Area, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    اگر (fnames != تہی)
	برائے (i = 0; i < cFiles; ++i)
	{
	    اگر (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_مقصد_enc(wszFile, تہی);
	    ورنہ
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    Drag_اختتام(hDrop);

    اگر (fnames == تہی)
	واپس;

    صحیح kbd_modifiers = get_فعال_modifiers();

    اگر ((kbd_modifiers & MOD_نقاب_سرکاؤ) != 0)
	modifiers |= MOUSE_سرکاؤ;
    اگر ((kbd_modifiers & MOD_نقاب_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    اگر ((kbd_modifiers & MOD_نقاب_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ساکن صحیح
_OnScroll(
    HWND hwnd غیر_مستعمل,
    HWND hwndCtl,
    UINT رمز,
    صحیح pos)
{
    ساکن UINT	پچھلا_رمز = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    طویل	val;
    صحیح		dragging = FALSE;
    صحیح		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_سائز = حجم(si);
    si.f_نقاب = SIF_POS;

    sb = gui_mswin_ڈھونڈو_scrollbar(hwndCtl);
    اگر (sb == تہی)
	واپس 0;

    اگر (sb->wp != تہی)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    ورنہ	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->قدر;

    انتخاب (رمز)
    {
	حالت SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    اگر (sb->scroll_سرکاؤ > 0)
		val <<= sb->scroll_سرکاؤ;
	    توڑو;
	حالت SB_LINEDOWN:
	    val++;
	    توڑو;
	حالت SB_LINEUP:
	    val--;
	    توڑو;
	حالت SB_PAGEDOWN:
	    val += (sb_info->سائز > 2 ? sb_info->سائز - 2 : 1);
	    توڑو;
	حالت SB_PAGEUP:
	    val -= (sb_info->سائز > 2 ? sb_info->سائز - 2 : 1);
	    توڑو;
	حالت SB_چوٹی:
	    val = 0;
	    توڑو;
	حالت SB_BOTTOM:
	    val = sb_info->max;
	    توڑو;
	حالت SB_ENDSCROLL:
	    اگر (پچھلا_رمز == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		اگر (sb->scroll_سرکاؤ > 0)
		    val <<= sb->scroll_سرکاؤ;
	    }
	    توڑو;

	طےشدہ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    واپس 0;
    }
    پچھلا_رمز = رمز;

    si.nPos = (sb->scroll_سرکاؤ > 0) ? val >> sb->scroll_سرکاؤ : val;
    سیٹ_Scroll_Info(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    اگر (sb->wp != تہی)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_بایاں) ? SBAR_دایاں : SBAR_بایاں].id;

	سیٹ_Scroll_Info(id, SB_CTL, &si, TRUE);
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

    واپس 0;
}


#اگر_تعریف_شدہ FEAT_XPM_W32
# include "xpm_w32.h"
#اگر_ختم


// Some parameters for tearoff menus.  All in pixels.
#تعریف_کرو TEAROFF_PADDING_X	2
#تعریف_کرو TEAROFF_BUTTON_PAD_X	8
#تعریف_کرو TEAROFF_MIN_چوڑائی	200
#تعریف_کرو TEAROFF_SUBMENU_LABEL	">>"
#تعریف_کرو TEAROFF_عمود_PADDING	3	// # spaces to pad column with.


#اگر_تعریف_شدہ FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_متن_LEN	    MAXPATHL

ساکن BalloonEval  *cur_beval = تہی;
ساکن UINT_PTR	    beval_timer_id = 0;
ساکن DWORD	    آخری_user_activity = 0;
#اگر_ختم // defined(FEAT_BEVAL_GUI)


// Local variables:

#اگر_تعریف_شدہ FEAT_MENU
ساکن UINT	s_menu_id = 100;
#اگر_ختم

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#تعریف_کرو USE_SYSMENU_FONT

#تعریف_کرو VIM_NAME	"vim"
#تعریف_کرو VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#تعریف_کرو DLG_ALLOC_سائز 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
ساکن PWORD
جوڑو_dialog_element(
	PWORD p,
	DWORD lStyle,
	لفظ_2 x,
	لفظ_2 y,
	لفظ_2 w,
	لفظ_2 h,
	لفظ_2 Id,
	لفظ_2 clss,
	ثابت حرف *caption);
ساکن LPWORD lpwAlign(LPWORD);
ساکن صحیح n_Copy_Ansi_مقصد_Wide_Char(LPWORD, LPSTR, BOOL);
#اگر defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ساکن خالی gui_mch_tearoff(char_u *title, vimmenu_T *menu, صحیح initX, صحیح initY);
#اگر_ختم
ساکن خالی get_dialog_font_metrics(خالی);

ساکن صحیح dialog_default_button = -1;

#اگر_تعریف_شدہ FEAT_TOOLBAR
ساکن خالی initialise_toolbar(خالی);
ساکن خالی update_toolbar_سائز(خالی);
ساکن LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_پیغام, WPARAM wParam, LPARAM lParam);
ساکن صحیح get_toolbar_bitmap(vimmenu_T *menu);
#ورنہ
# define update_toolbar_سائز()
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
ساکن خالی initialise_tabline(خالی);
ساکن LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_پیغام, WPARAM wParam, LPARAM lParam);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MBYTE_IME
ساکن LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ساکن char_u *Get_نتیجہ_Str(HWND hwnd, صحیح GCS, صحیح *lenp);
#اگر_ختم
#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
قسمتعریف ساخت لیبل_COMPOSITIONFORM {
    DWORD dwStyle;
    نقطہ_2 ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
قسمتعریف HANDLE HIMC;
# endif

ساکن HINSTANCE hLibImm = تہی;
ساکن LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ساکن HIMC (WINAPI *pImmGetContext)(HWND);
ساکن HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ساکن BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ساکن BOOL (WINAPI *p_Imm_Get_Open_حالت)(HIMC);
ساکن BOOL (WINAPI *p_Imm_سیٹ_Open_حالت)(HIMC, BOOL);
ساکن BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ساکن BOOL (WINAPI *p_Imm_سیٹ_Composition_Font_W)(HIMC, LPLOGFONTW);
ساکن BOOL (WINAPI *p_Imm_سیٹ_Composition_کھڑکی)(HIMC, LPCOMPOSITIONFORM);
ساکن BOOL (WINAPI *p_Imm_Get_Conversion_حالت)(HIMC, LPDWORD, LPDWORD);
ساکن BOOL (WINAPI *p_Imm_سیٹ_Conversion_حالت)(HIMC, DWORD, DWORD);
ساکن خالی dyn_imm_بھرو(خالی);
#ورنہ
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define p_Imm_Get_Open_حالت	  Imm_Get_Open_حالت
# define p_Imm_سیٹ_Open_حالت	  Imm_سیٹ_Open_حالت
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define p_Imm_سیٹ_Composition_Font_W  Imm_سیٹ_Composition_Font_W
# define p_Imm_سیٹ_Composition_کھڑکی Imm_سیٹ_Composition_کھڑکی
# define p_Imm_Get_Conversion_حالت  Imm_Get_Conversion_حالت
# define p_Imm_سیٹ_Conversion_حالت  Imm_سیٹ_Conversion_حالت
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    ساکن صحیح
gui_mswin_get_menu_اونچائی(
    صحیح	    fix_کھڑکی)	    // If TRUE, resize window if menu height changed
{
    ساکن صحیح	old_menu_اونچائی = -1;

    RECT    rc1, rc2;
    صحیح	    عدد;
    صحیح	    menu_اونچائی;

    اگر (gui.menu_is_فعال)
	عدد = Get_Menu_شے_گنتی(s_menuBar);
    ورنہ
	عدد = 0;

    اگر (عدد == 0)
	menu_اونچائی = 0;
    ورنہ اگر (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_اونچائی = old_menu_اونچائی == -1 ? 0 : old_menu_اونچائی;
    }
    ورنہ
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	Get_Menu_شے_Rect(s_hwnd, s_menuBar, 0, &rc1);
	اگر (gui.starting)
	    menu_اونچائی = rc1.bottom - rc1.چوٹی + 1;
	ورنہ
	{
	    Get_Menu_شے_Rect(s_hwnd, s_menuBar, عدد - 1, &rc2);
	    menu_اونچائی = rc2.bottom - rc1.چوٹی + 1;
	}
    }

    اگر (fix_کھڑکی && menu_اونچائی != old_menu_اونچائی)
	gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_اونچائی = menu_اونچائی;

    واپس menu_اونچائی;
}
#اگر_ختم // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ساکن طویل
mouse_vertical_scroll_قدم(خالی)
{
    UINT val;
    اگر (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	واپس (val != WHEEL_PAGESCROLL) ? (طویل)val : -1;
    واپس 3; // Safe default;
}

    ساکن طویل
mouse_horizontal_scroll_قدم(خالی)
{
    UINT val;
    اگر (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	واپس (طویل)val;
    واپس 3; // Safe default;
}

    ساکن خالی
init_mouse_wheel(خالی)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_سیٹ_vert_scroll_قدم(mouse_vertical_scroll_قدم());
    mouse_سیٹ_hor_scroll_قدم(mouse_horizontal_scroll_قدم());
}

/*
 * Mouse scroll event handler.
 */
    ساکن خالی
_OnMouseWheel(HWND hwnd غیر_مستعمل, WPARAM wParam, LPARAM lParam, صحیح horizontal)
{
    صحیح		button;
    win_T	*wp;
    صحیح		modifiers = 0;
    صحیح		kbd_modifiers;
    صحیح		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    نقطہ_2	pt;

    wp = gui_mouse_کھڑکی(ڈھونڈو_POPUP);

#اگر_تعریف_شدہ FEAT_PROP_POPUP
    اگر (wp != تہی && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_صف = wp->w_winrow;
	mouse_ستون = wp->w_wincol;
	CLEAR_میدان(cap);
	اگر (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_بایاں : MSCR_دایاں;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ورنہ
	{
	    cap.arg = zDelta < 0 ? MSCR_اوپر : MSCR_نیچے_کو;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	برآمد_flush();
    }
#اگر_ختم

    اگر (wp == تہی || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    اگر (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    ورنہ
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_فعال_modifiers();

    اگر ((kbd_modifiers & MOD_نقاب_سرکاؤ) != 0)
	modifiers |= MOUSE_سرکاؤ;
    اگر ((kbd_modifiers & MOD_نقاب_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    اگر ((kbd_modifiers & MOD_نقاب_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    Screen_مقصد_Client(s_متن_Area, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#اگر_تعریف_شدہ USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    ساکن صحیح
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_سائز = حجم(NONCLIENTMETRICSW);
    اگر (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    حجم(NONCLIENTMETRICSW),
	    &nm,
	    0))
	واپس FAIL;
    *lf = nm.lfMenuFont;
    واپس OK;
}
#اگر_ختم


#اگر defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ساکن خالی
سیٹ_tabline_font(خالی)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    اگر (gui_w32_get_menu_font(&lfSysmenu) != OK)
	واپس;

    lfSysmenu.lf_اونچائی = adjust_fontsize_by_dpi(lfSysmenu.lf_اونچائی);
    font = Create_Font_بالواسطہ_W(&lfSysmenu);

    Send_پیغام(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = Get_Desktop_کھڑکی();
    hdc = Get_کھڑکی_DC(hwnd);
    hfntOld = SelectFont(hdc, font);

    Get_متن_Metrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_اونچائی = tm.tm_اونچائی + tm.tm_اندرونی_Leading + 7;
}
#ورنہ
# define سیٹ_tabline_font()
#اگر_ختم

/*
 * Invoked when a setting was changed.
 */
    ساکن LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    انتخاب (param)
    {
	حالت SPI_SETWHEELSCROLLLINES:
	    mouse_سیٹ_vert_scroll_قدم(mouse_vertical_scroll_قدم());
	    توڑو;
	حالت SPI_SETWHEELSCROLLCHARS:
	    mouse_سیٹ_hor_scroll_قدم(mouse_horizontal_scroll_قدم());
	    توڑو;
	حالت SPI_SETNONCLIENTMETRICS:
	    سیٹ_tabline_font();
	    توڑو;
	طےشدہ:
	    توڑو;
    }
    واپس 0;
}

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    ساکن خالی
On_کھڑکی_Pos_بدلا(
    HWND hwnd,
    ثابت LPWINDOWPOS lpwpos)
{
    ساکن صحیح x = 0, y = 0, cx = 0, cy = 0;
    خارجی صحیح WSInitialized;

    اگر (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_چوکھٹا_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, Def_کھڑکی_Proc_W);
}
#اگر_ختم


ساکن HWND hwndTip = تہی;

    ساکن خالی
show_sizing_tip(صحیح ستون_تعداد, صحیح صفیں_2)
{
    TOOLINFOA ti;
    حرف buf[32];

    ti.cb_سائز = حجم(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpsz_متن = buf;
    sprintf(buf, "%dx%d", ستون_تعداد, صفیں_2);
    اگر (hwndTip == تہی)
    {
	hwndTip = Create_کھڑکی_Ex_A(0, TOOLTIPS_CLASSA, تہی,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, تہی, GetModuleHandle(تہی), تہی);
	Send_پیغام(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	Send_پیغام(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    ورنہ
    {
	Send_پیغام(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    Send_پیغام(hwndTip, TTM_POPUP, 0, 0);
}

    ساکن خالی
destroy_sizing_tip(خالی)
{
    اگر (hwndTip == تہی)
	واپس;

    Destroy_کھڑکی(hwndTip);
    hwndTip = تہی;
}

    ساکن صحیح
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    صحیح	    w, h;
    صحیح	    valid_w, valid_h;
    صحیح	    w_انحراف, h_انحراف;
    صحیح	    ستون_تعداد, صفیں_2;

    w = lprc->دایاں - lprc->بایاں;
    h = lprc->bottom - lprc->چوٹی;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &ستون_تعداد, &صفیں_2);
    w_انحراف = w - valid_w;
    h_انحراف = h - valid_h;

    اگر (fwSide == WMSZ_بایاں || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->بایاں += w_انحراف;
    ورنہ اگر (fwSide == WMSZ_دایاں || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->دایاں -= w_انحراف;

    اگر (fwSide == WMSZ_چوٹی || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->چوٹی += h_انحراف;
    ورنہ اگر (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_انحراف;

    show_sizing_tip(ستون_تعداد, صفیں_2);
    واپس TRUE;
}

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    ساکن خالی
On_RButton_اوپر(HWND hwnd, صحیح x, صحیح y, UINT کلید_Flags)
{
    اگر (gui_mch_showing_tabline())
    {
	نقطہ_2 pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	Get_کھڑکی_Rect(s_متن_Area, &rect);
	اگر (pt.y < rect.چوٹی)
	{
	    show_tabline_popup_menu();
	    واپس;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, کلید_Flags, Def_کھڑکی_Proc_W);
}

    ساکن خالی
On_LButton_نیچے_کو(HWND hwnd, BOOL fDoubleClick, صحیح x, صحیح y, UINT کلید_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    اگر (gui_mch_showing_tabline())
    {
	نقطہ_2 pt;
	RECT rect;

	GetCursorPos(&pt);
	Get_کھڑکی_Rect(s_متن_Area, &rect);
	اگر (pt.y < rect.چوٹی)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, کلید_Flags, Def_کھڑکی_Proc_W);
}
#اگر_ختم

    ساکن UINT
_OnNCHitTest(HWND hwnd, صحیح xPos, صحیح yPos)
{
    UINT	نتیجہ;
    صحیح		x, y;

    نتیجہ = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, Def_کھڑکی_Proc_W);
    اگر (نتیجہ != HTCLIENT)
	واپس نتیجہ;

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    اگر (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	Get_کھڑکی_Rect(s_متن_Area, &rct);
	اگر (yPos < rct.چوٹی)
	    واپس نتیجہ;
    }
#اگر_ختم
    (خالی)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    اگر (xPos < 48) // <VN> TODO should use system metric?
	واپس HTBOTTOMLEFT;
    ورنہ
	واپس HTBOTTOMRIGHT;
}

#اگر defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساکن LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    انتخاب (hdr->رمز)
    {
	حالت TTN_GETDISPINFOW:
	حالت TTN_GETDISPINFO:
	    {
		char_u		*str = تہی;
		ساکن خالی	*tt_متن = تہی;

		VIM_CLEAR(tt_متن);

# ifdef FEAT_GUI_TABLINE
		اگر (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    نقطہ_2	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    اگر (Screen_مقصد_Client(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			صحیح idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = Tab_Ctrl_کامیابی_Test(s_tabhwnd, &htinfo);
			اگر (idx != -1)
			{
			    tabpage_T *tp;

			    tp = ڈھونڈو_tabpage(idx + 1);
			    اگر (tp != تہی)
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
		ورنہ
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_ڈھونڈو_menu(جڑ_menu, idButton);
		    اگر (pMenu)
			str = pMenu->لڑیاں[MENU_اشاریہ_TIP];
		}
# endif
		اگر (str == تہی)
		    توڑو;

		// Set the maximum width, this also enables using \n for
		// line break.
		Send_پیغام(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		اگر (hdr->رمز == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_متن = enc_مقصد_utf16(str, تہی);
		    lpdi->lpsz_متن = tt_متن;
		    // can't show tooltip if failed
		}
		ورنہ
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    اگر (len < حجم(lpdi->sz_متن)
			    || ((tt_متن = vim_strnsave(str, len)) == تہی))
			vim_strncpy((char_u *)lpdi->sz_متن, str,
				حجم(lpdi->sz_متن) - 1);
		    ورنہ
			lpdi->lpsz_متن = tt_متن;
		}
	    }
	    توڑو;

# ifdef FEAT_GUI_TABLINE
	حالت TCN_SELCHANGE:
	    اگر (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		واپس 0L;
	    }
	    توڑو;

	حالت NM_RCLICK:
	    اگر (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		واپس 0L;
	    }
	    توڑو;
# endif

	طےشدہ:
	    توڑو;
    }
    واپس Def_کھڑکی_Proc_W(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#اگر_ختم

#اگر defined(MENUHINTS) && defined(FEAT_MENU)
    ساکن LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    اگر (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & منوال_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ساکن صحیح  did_menu_tip = FALSE;

	اگر (did_menu_tip)
	{
	    پیغام_clr_cmdline();
	    setcursor();
	    برآمد_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_ڈھونڈو_menu(جڑ_menu, (UINT)LOWORD(wParam));
	اگر (pMenu != تہی && pMenu->لڑیاں[MENU_اشاریہ_TIP] != تہی)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_سائز = حجم(MENUITEMINFO);
	    menuinfo.f_نقاب = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    اگر (Get_Menu_شے_Info(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++پیغام_hist_off;
		پیغام((حرف *)pMenu->لڑیاں[MENU_اشاریہ_TIP]);
		--پیغام_hist_off;
		setcursor();
		برآمد_flush();
		did_menu_tip = TRUE;
	    }
	}
	واپس 0L;
    }
    واپس Def_کھڑکی_Proc_W(hwnd, WM_MENUSELECT, wParam, lParam);
}
#اگر_ختم

    ساکن BOOL
On_Get_Dpi_Scaled_سائز(HWND hwnd غیر_مستعمل, UINT dpi, سائز_3 *سائز)
{
    صحیح		old_چوڑائی, old_اونچائی;
    صحیح		new_چوڑائی, new_اونچائی;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_سائز(gui.norm_font, &old_چوڑائی, &old_اونچائی);    // Current size
    GetObjectW((HFONT)gui.norm_font, حجم(lf), &lf);
    lf.lf_اونچائی = lf.lf_اونچائی * (صحیح)dpi / s_dpi;
    font = Create_Font_بالواسطہ_W(&lf);
    اگر (font)
    {
	Get_Font_سائز((GuiFont)font, &new_چوڑائی, &new_اونچائی);	// New size
	DeleteFont(font);
    }
    ورنہ
    {
	new_چوڑائی = old_چوڑائی;
	new_اونچائی = old_اونچائی;
    }
    سائز->cx = سائز->cx * new_چوڑائی / old_چوڑائی;
    سائز->cy = سائز->cy * new_اونچائی / old_اونچائی;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    واپس TRUE;
}

    ساکن LRESULT
On_Dpi_بدلا(HWND hwnd, UINT xdpi غیر_مستعمل, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_اندر_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_سائز();
    update_toolbar_سائز();
    سیٹ_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_اونچائی(FALSE);
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    im_سیٹ_position(gui.صف, gui.ستون);
#اگر_ختم
    InvalidateRect(hwnd, تہی, TRUE);

    s_اندر_dpichanged = FALSE;
    واپس 0L;
}


    ساکن LRESULT CALLBACK
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

    انتخاب (u_پیغام)
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
#اگر_تعریف_شدہ FEAT_MENU
	HANDLE_پیغام(hwnd, WM_COMMAND,	_OnMenu);
#اگر_ختم
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_پیغام(hwnd, WM_SETFOCUS,	On_سیٹ_Focus);
	HANDLE_پیغام(hwnd, WM_سائز,	On_سائز);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_پیغام(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_پیغام(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	HANDLE_پیغام(hwnd, WM_WINDOWPOSCHANGED, On_کھڑکی_Pos_بدلا);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	HANDLE_پیغام(hwnd, WM_RBUTTONUP,	On_RButton_اوپر);
	HANDLE_پیغام(hwnd, WM_LBUTTONDBLCLK,  On_LButton_نیچے_کو);
#اگر_ختم
	HANDLE_پیغام(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    حالت WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	واپس FALSE;		// Do NOT allow system to go down.

    حالت WM_ENDSESSION:
	اگر (wParam)	// system only really goes down when wParam is TRUE
	{
	    On_انتہا_Session();
	    واپس 0L;
	}
	توڑو;

    حالت WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (صحیح)(مختصر)LOWORD(lParam));
	واپس 0L;

    حالت WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#اگر_تعریف_شدہ FEAT_MENU
	اگر (	!gui.menu_is_فعال
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((صحیح)wParam))
		)
#اگر_ختم
	{
	    _OnSysChar(hwnd, (UINT)wParam, (صحیح)(مختصر)LOWORD(lParam));
	    واپس 0L;
	}
#اگر_تعریف_شدہ FEAT_MENU
	ورنہ
	    واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
#اگر_ختم

    حالت WM_SYSKEYUP:
#اگر_تعریف_شدہ FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
#ورنہ
	واپس 0L;
#اگر_ختم

    حالت WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	توڑو;

    حالت WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	واپس _DuringSizing((UINT)wParam, (LPRECT)lParam);

    حالت WM_MOUSEWHEEL:
    حالت WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_پیغام == WM_MOUSEHWHEEL);
	واپس 0L;

	// Notification for change in SystemParametersInfo()
    حالت WM_SETTINGCHANGE:
	واپس _OnSettingChange((UINT)wParam);

#اگر defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    حالت WM_NOTIFY:
	واپس _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#اگر_ختم

#اگر defined(MENUHINTS) && defined(FEAT_MENU)
    حالت WM_MENUSELECT:
	واپس _OnMenuSelect(hwnd, wParam, lParam);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MBYTE_IME
    حالت WM_IME_NOTIFY:
	اگر (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
	واپس 1L;

    حالت WM_IME_COMPOSITION:
	اگر (!_OnImeComposition(hwnd, wParam, lParam))
	    واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
	واپس 1L;
#اگر_ختم
    حالت WM_GETDPISCALEDSIZE:
	واپس On_Get_Dpi_Scaled_سائز(hwnd, (UINT)wParam, (سائز_3 *)lParam);
    حالت WM_DPICHANGED:
	واپس On_Dpi_بدلا(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    طےشدہ:
#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
	اگر (u_پیغام == s_findrep_پیغام && s_findrep_پیغام != 0)
	    On_ڈھونڈو_Repl();
#اگر_ختم
	توڑو;
    }

    واپس Def_کھڑکی_Proc_W(hwnd, u_پیغام, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_والد_hwnd = تہی;

    ساکن BOOL CALLBACK
ڈھونڈو_کھڑکی_Title(HWND hwnd, LPARAM lParam)
{
    حرف	buf[2048];
    حرف	*title = (حرف *)lParam;

    اگر (Get_کھڑکی_متن(hwnd, buf, حجم(buf)))
    {
	اگر (لڑی_تلاش(buf, title) != تہی)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_والد_hwnd = ڈھونڈو_کھڑکی_Ex(hwnd, تہی, "MDIClient", تہی);
	    اگر (vim_والد_hwnd != تہی)
		واپس FALSE;
	}
    }
    واپس TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    خالی
gui_mch_سیٹ_والد(حرف *title)
{
    EnumWindows(ڈھونڈو_کھڑکی_Title, (LPARAM)title);
    اگر (vim_والد_hwnd == تہی)
    {
	semsg(_(e_cannot_ڈھونڈو_کھڑکی_title_str), title);
	mch_exit(2);
    }
}

#اگر_غیر_تعریف_شدہ FEAT_OLE
    ساکن خالی
ole_error(حرف *arg)
{
    حرف buf[IOSIZE];

# ifdef VIMDLL
    gui.اندر_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_دلیل_not_supported_str_use_ole_نسخہ), arg);
    mch_errmsg(buf);
}
#اگر_ختم

#اگر defined(GUI_MAY_SPAWN)
    ساکن حرف *
gvim_error(خالی)
{
    حرف *پیغام = _(e_gui_cannot_be_مستعمل_cannot_execute_gvim_exe);

    اگر (starting)
    {
	mch_errmsg(پیغام);
	mch_errmsg("\n");
	mch_exit(2);
    }
    واپس پیغام;
}

    حرف *
gui_mch_do_spawn(char_u *arg)
{
    صحیح			len;
# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = تہی;
    LPWSTR		tofree1 = تہی;
# endif
    WCHAR		name[MAX_راستہ];
    LPWSTR		cmd, newcmd = تہی, p, warg, tofree2 = تہی;
    STARTUPINFOW	si = {حجم(si)};
    PROCESS_INFORMATION pi;

    اگر (!GetModuleFileNameW(g_hinst, name, MAX_راستہ))
	جاؤ error;
    p = wcsrchr(name, L'\\');
    اگر (p == تہی)
	جاؤ error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# ورنہ
    wcscpy(p + 1, L"gvim.exe");
# endif

# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    اگر (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_سطر_W();
	// Skip 1st argument.
	جب (*p && *p != L' ' && *p != L'\t')
	{
	    اگر (*p == L'"')
	    {
		// Skip quoted strings
		جب (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ورنہ
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	صحیح	ret;

	session = vim_tempname('s', FALSE);
	اگر (session == تہی)
	    جاؤ error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	اگر (p_bg == تہی)
	{
	    p_bg = savebg;
	    جاؤ error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	اگر (!ret)
	    جاؤ error;
	wsession = enc_مقصد_utf16(session, تہی);
	اگر (wsession == تہی)
	    جاؤ error;
	len = (صحیح)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	اگر (cmd == تہی)
	{
	    vim_free(wsession);
	    جاؤ error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    اگر (arg != تہی)
    {
	warg = enc_مقصد_utf16(arg, تہی);
	اگر (warg == تہی)
	    جاؤ error;
	tofree2 = warg;
    }
    ورنہ
	warg = L"";

    // Set up the new command line.
    len = (صحیح)wcslen(name) + (صحیح)wcslen(cmd) + (صحیح)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    اگر (newcmd == تہی)
	جاؤ error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    اگر (!CreateProcessW(تہی, newcmd, تہی, تہی, TRUE, 0,
		تہی, تہی, &si, &pi))
	جاؤ error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    اگر (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    واپس gvim_error();
}
#اگر_ختم

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    خالی
gui_mch_prepare(صحیح *argc, حرف **argv)
{
    صحیح		silent = FALSE;
    صحیح		idx;

    // Check for special OLE command line parameters
    اگر ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	اگر (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	ورنہ
	    idx = 1;

	// Register Vim as an OLE Automation server
	اگر (STRICMP(argv[idx] + 1, "register") == 0)
	{
#اگر_تعریف_شدہ FEAT_OLE
	    رجسٹر_Me(silent);
	    mch_exit(0);
#ورنہ
	    اگر (!silent)
		ole_error("register");
	    mch_exit(2);
#اگر_ختم
	}

	// Unregister Vim as an OLE Automation server
	اگر (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#اگر_تعریف_شدہ FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#ورنہ
	    اگر (!silent)
		ole_error("unregister");
	    mch_exit(2);
#اگر_ختم
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	اگر (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#اگر_تعریف_شدہ FEAT_OLE
	    *argc = 1;
#ورنہ
	    ole_error("embedding");
	    mch_exit(2);
#اگر_ختم
	}
    }

#اگر_تعریف_شدہ FEAT_OLE
# ifdef VIMDLL
    اگر (mch_is_gui_executable())
# endif
    {
	صحیح	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	اگر (bDoRestart)
	    mch_exit(0);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	صحیح arg;

	برائے (arg = 1; arg < *argc; arg++)
	    اگر (لڑی_موازنہ("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * حجم(حرف *));
		argv[*argc] = تہی;
		توڑو;	// enough?
	    }
    }
#اگر_ختم
}

    ساکن خالی
بھرو_dpi_func(خالی)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    اگر (hUser32 == تہی)
	جاؤ fail;

    pGetDpiForSystem = (UINT (WINAPI *)(خالی))Get_Proc_پتہ(hUser32, "GetDpiForSystem");
    p_Get_Dpi_For_کھڑکی = (UINT (WINAPI *)(HWND))Get_Proc_پتہ(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (صحیح (WINAPI *)(صحیح, UINT))Get_Proc_پتہ(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    p_سیٹ_Thread_Dpi_Awareness_Context = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_پتہ(hUser32, "SetThreadDpiAwarenessContext");
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_پتہ(hUser32, "GetAwarenessFromDpiAwarenessContext");
#اگر_ختم

    اگر (p_سیٹ_Thread_Dpi_Awareness_Context != تہی)
    {
	DPI_AWARENESS_CONTEXT oldctx = p_سیٹ_Thread_Dpi_Awareness_Context(
		DPI_AWARENESS_CONTEXT_فی_MONITOR_AWARE_V2);
	اگر (oldctx != تہی)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#اگر_تعریف_شدہ FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#اگر_ختم
	    واپس;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    p_Get_Dpi_For_کھڑکی = تہی;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    p_سیٹ_Thread_Dpi_Awareness_Context = تہی;
#اگر_تعریف_شدہ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = تہی;
#اگر_ختم
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحیح
gui_mch_init(خالی)
{
    ثابت WCHAR szVimWndClassW[] = VIM_CLASSW;
    ثابت WCHAR sz_متن_Area_Class_W[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    اگر (s_hwnd != تہی)
	جاؤ theend;

    /*
     * Load the tearoff bitmap
     */
#اگر_تعریف_شدہ FEAT_TEAROFF
    s_htearbitmap = بھرو_Bitmap(g_hinst, "IDB_TEAROFF");
#اگر_ختم

    بھرو_dpi_func();

#اگر_تعریف_شدہ FEAT_GUI_DARKTHEME
    dyn_uxtheme_بھرو();
#اگر_ختم

    dyn_dwm_بھرو();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_سائز();

#اگر_تعریف_شدہ FEAT_MENU
    gui.menu_اونچائی = 0;	// Windows takes care of this
#اگر_ختم
    gui.border_چوڑائی = 0;
#اگر_تعریف_شدہ FEAT_TOOLBAR
    gui.toolbar_اونچائی = TOOLBAR_BUTTON_اونچائی + TOOLBAR_BORDER_اونچائی;
#اگر_ختم

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    اگر (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = بھرو_Icon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = بھرو_Cursor(تہی, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = تہی;
	wndclassw.lpszClassName = szVimWndClassW;

	اگر (رجسٹر_Class_W(&wndclassw) == 0)
	    واپس FAIL;
    }

    اگر (vim_والد_hwnd != تہی)
    {
#اگر_تعریف_شدہ HAVE_TRY_EXCEPT
	__try
	{
#اگر_ختم
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = Create_کھڑکی_Ex_W(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_بچہ
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_والد_hwnd, تہی,
		g_hinst, تہی);
#اگر_تعریف_شدہ HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#اگر_ختم
	اگر (s_hwnd == تہی)
	{
	    emsg(_(e_unable_مقصد_open_کھڑکی_inside_mdi_application));
	    mch_exit(2);
	}
    }
    ورنہ
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	اگر (Is_کھڑکی((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = Create_کھڑکی_W(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		تہی, تہی,
		g_hinst, تہی);
	اگر (s_hwnd != تہی && win_socket_id != 0)
	{
	    سیٹ_والد(s_hwnd, (HWND)win_socket_id);
	    Show_کھڑکی(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    اگر (s_hwnd == تہی)
	واپس FAIL;

    اگر (p_Get_Dpi_For_کھڑکی != تہی)
    {
	s_dpi = p_Get_Dpi_For_کھڑکی(s_hwnd);
	update_scrollbar_سائز();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_بھرو();
#اگر_ختم

    // Create the text area window
    اگر (GetClassInfoW(g_hinst, sz_متن_Area_Class_W, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = متن_Area_Wnd_Proc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = تہی;
	wndclassw.hCursor = بھرو_Cursor(تہی, IDC_ARROW);
	wndclassw.hbrBackground = تہی;
	wndclassw.lpszMenuName = تہی;
	wndclassw.lpszClassName = sz_متن_Area_Class_W;

	اگر (رجسٹر_Class_W(&wndclassw) == 0)
	    واپس FAIL;
    }

    s_متن_Area = Create_کھڑکی_Ex_W(
	0,
	sz_متن_Area_Class_W, L"Vim text area",
	WS_بچہ | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, تہی,
	g_hinst, تہی);

    اگر (s_متن_Area == تہی)
	واپس FAIL;

#اگر_تعریف_شدہ FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = تہی;

	اگر (mch_icon_بھرو(&hIcon) == OK && hIcon != تہی)
	    Send_پیغام(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MENU
    s_menuBar = CreateMenu();
#اگر_ختم
    s_hdc = GetDC(s_متن_Area);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    سیٹ_معمول_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_عقب_pixel = gui.عقب_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_انحراف = gui.border_چوڑائی;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#اگر_تعریف_شدہ FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#اگر_ختم
#اگر_تعریف_شدہ MSWIN_ڈھونڈو_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_پیغام = رجسٹر_کھڑکی_پیغام(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_سائز = حجم(s_findrep_struct);
    s_findrep_struct.lpstr_ڈھونڈو_What = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstr_ڈھونڈو_What[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.w_ڈھونڈو_What_Len = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_EVAL
    // set the v:windowid variable
    سیٹ_vim_var_nr(VV_WINDOWID, Handle_مقصد_Long(s_hwnd));
#اگر_ختم

#اگر_تعریف_شدہ FEAT_RENDER_OPTIONS
    اگر (p_rop)
	(خالی)gui_mch_سیٹ_rendering_options(p_rop);
#اگر_ختم

theend:
    // Display any pending error messages
    display_errors();

    واپس OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ساکن خالی
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = Monitor_From_کھڑکی(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    اگر (mon != تہی)
    {
	moninfo.cb_سائز = حجم(MONITORINFO);
	اگر (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    واپس;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    خالی
gui_mch_سیٹ_shellsize(
	صحیح چوڑائی,
	صحیح اونچائی,
	صحیح min_چوڑائی غیر_مستعمل,
	صحیح min_اونچائی غیر_مستعمل,
	صحیح بنیاد_چوڑائی غیر_مستعمل,
	صحیح بنیاد_اونچائی غیر_مستعمل,
	صحیح direction)
{
    RECT	workarea_rect;
    RECT	کھڑکی_rect;
    صحیح		win_چوڑائی, win_اونچائی;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    اگر (IsZoomed(s_hwnd) && starting == 0)
	Show_کھڑکی(s_hwnd, SW_SHOWNORMAL);

    اگر (s_اندر_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	کھڑکی_rect = s_suggested_rect;
    ورنہ
	// Use current position.
	Get_کھڑکی_Rect(s_hwnd, &کھڑکی_rect);

    // compute the size of the outside of the window
    win_چوڑائی = چوڑائی + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_اونچائی = اونچائی + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_اونچائی(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    کھڑکی_rect.دایاں = کھڑکی_rect.بایاں + win_چوڑائی;
    کھڑکی_rect.bottom = کھڑکی_rect.چوٹی + win_اونچائی;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    اگر (!s_اندر_dpichanged)
    {
	اگر ((direction & RESIZE_HOR)
		&& کھڑکی_rect.دایاں > workarea_rect.دایاں)
	    انحراف_Rect(&کھڑکی_rect,
		    workarea_rect.دایاں - کھڑکی_rect.دایاں, 0);

	اگر ((direction & RESIZE_HOR)
		&& کھڑکی_rect.بایاں < workarea_rect.بایاں)
	    انحراف_Rect(&کھڑکی_rect,
		    workarea_rect.بایاں - کھڑکی_rect.بایاں, 0);

	اگر ((direction & RESIZE_VERT)
		&& کھڑکی_rect.bottom > workarea_rect.bottom)
	    انحراف_Rect(&کھڑکی_rect,
		    0, workarea_rect.bottom - کھڑکی_rect.bottom);

	اگر ((direction & RESIZE_VERT)
		&& کھڑکی_rect.چوٹی < workarea_rect.چوٹی)
	    انحراف_Rect(&کھڑکی_rect,
		    0, workarea_rect.چوٹی - کھڑکی_rect.چوٹی);
    }

    Move_کھڑکی(s_hwnd, کھڑکی_rect.بایاں, کھڑکی_rect.چوٹی,
						win_چوڑائی, win_اونچائی, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    سیٹ_فعال_کھڑکی(s_hwnd);
    سیٹ_Focus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_اونچائی(!gui.starting);
}


    خالی
gui_mch_سیٹ_scrollbar_thumb(
    scrollbar_T *sb,
    طویل	val,
    طویل	سائز,
    طویل	max)
{
    SCROLLINFO	info;

    sb->scroll_سرکاؤ = 0;
    جب (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	سائز >>= 1;
	++sb->scroll_سرکاؤ;
    }

    اگر (sb->scroll_سرکاؤ > 0)
	++سائز;

    info.cb_سائز = حجم(info);
    info.f_نقاب = SIF_POS | SIF_RANGE | SIF_صفحہ;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.n_صفحہ = سائز;
    سیٹ_Scroll_Info(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    خالی
gui_mch_سیٹ_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    خالی
gui_mch_سیٹ_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    خالی
gui_mch_سیٹ_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    خالی
gui_mch_سیٹ_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#اگر_تعریف_شدہ FEAT_MBYTE_IME
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
    ساکن LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ڈیٹا غیر_مستعمل)
{
    LRESULT l_نتیجہ = 0;
    HIMC hImc;

    اگر (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	واپس l_نتیجہ;
    انتخاب (dwCommand)
    {
	حالت IMN_SETOPENSTATUS:
	    اگر (p_Imm_Get_Open_حالت(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lf_اونچائی = lf.lf_اونچائی * DEFAULT_DPI / s_dpi;
		p_Imm_سیٹ_Composition_Font_W(hImc, &lf);
		im_سیٹ_position(gui.صف, gui.ستون);

		// Disable langmap
		State &= ~منوال_LANGMAP;
		اگر (State & منوال_داخل_کرو)
		{
# اگر defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			صحیح old_صف = gui.صف;
			صحیح old_ستون = gui.ستون;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			حالت_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.صف = old_صف;
			gui.ستون = old_ستون;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_نتیجہ = 0;
	    توڑو;
    }
    pImmReleaseContext(hWnd, hImc);
    واپس l_نتیجہ;
}

    ساکن LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs غیر_مستعمل, LPARAM param)
{
    char_u	*ret;
    صحیح		len;

    اگر ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	واپس 0;

    ret = Get_نتیجہ_Str(hwnd, GCS_RESULTSTR, &len);
    اگر (ret != تہی)
    {
	جوڑو_مقصد_داخلہ_buf_csi(ret, len);
	vim_free(ret);
	واپس 1;
    }
    واپس 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ساکن char_u *
Get_نتیجہ_Str(HWND hwnd, صحیح GCS, صحیح *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = تہی;
    char_u	*convbuf = تہی;

    اگر (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	واپس تہی;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, تہی, 0);
    اگر (ret <= 0)
	واپس تہی;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + حجم(WCHAR));
    اگر (buf == تہی)
	واپس تہی;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / حجم(WCHAR);

    convbuf = utf16_مقصد_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    واپس convbuf;
}
#اگر_ختم

// For global functions we need prototypes.
#اگر defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    خالی
im_سیٹ_font(LOGFONTW *lf)
{
    HIMC hImc;

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	p_Imm_سیٹ_Composition_Font_W(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    خالی
im_سیٹ_position(صحیح صف, صحیح ستون)
{
    HIMC hImc;

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_نقطہ;
	cfs.ptCurrentPos.x = FILL_X(ستون);
	cfs.ptCurrentPos.y = FILL_Y(صف);
	Map_کھڑکی_Points(s_متن_Area, s_hwnd, &cfs.ptCurrentPos, 1);
	اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	p_Imm_سیٹ_Composition_کھڑکی(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    خالی
im_سیٹ_فعال(صحیح فعال)
{
    HIMC	hImc;
    ساکن HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    اگر (!gui.اندر_use && !gui.starting)
    {
	mbyte_im_سیٹ_فعال(فعال);
	واپس;
    }
# endif

    اگر (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	واپس;

    اگر (p_imdisable)
    {
	اگر (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    اگر (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	فعال = FALSE;
    }
    ورنہ اگر (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    اگر (!hImc)
	واپس;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    اگر (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ساکن DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ساکن BOOL bSaved = FALSE;

	اگر (فعال)
	{
	    // if we have a saved conversion status, restore it
	    اگر (bSaved)
		p_Imm_سیٹ_Conversion_حالت(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	ورنہ
	{
	    // save conversion status and disable korean
	    اگر (p_Imm_Get_Conversion_حالت(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		p_Imm_سیٹ_Conversion_حالت(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    p_Imm_سیٹ_Open_حالت(hImc, فعال);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    صحیح
im_get_حالت(خالی)
{
    صحیح		حالت_2 = 0;
    HIMC	hImc;

# ifdef VIMDLL
    اگر (!gui.اندر_use && !gui.starting)
	واپس mbyte_im_get_حالت();
# endif

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	حالت_2 = p_Imm_Get_Open_حالت(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    واپس حالت_2;
}

#اگر_ختم // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ساکن خالی
latin9_مقصد_ucs(char_u *متن, صحیح len, WCHAR *unicodebuf)
{
    صحیح		c;

    جب (--len >= 0)
    {
	c = *متن++;
	انتخاب (c)
	{
	    حالت 0xa4: c = 0x20ac; توڑو;   // euro
	    حالت 0xa6: c = 0x0160; توڑو;   // S hat
	    حالت 0xa8: c = 0x0161; توڑو;   // S -hat
	    حالت 0xb4: c = 0x017d; توڑو;   // Z hat
	    حالت 0xb8: c = 0x017e; توڑو;   // Z -hat
	    حالت 0xbc: c = 0x0152; توڑو;   // OE
	    حالت 0xbd: c = 0x0153; توڑو;   // oe
	    حالت 0xbe: c = 0x0178; توڑو;   // Y
	}
	*unicodebuf++ = c;
    }
}

#اگر_تعریف_شدہ FEAT_RIGHTLEFT
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
    ساکن خالی
Rev_برآمد( HDC hdc,
	صحیح ستون,
	صحیح صف,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR متن,
	UINT len,
	CONST INT *padding)
{
    صحیح		ix;

    برائے (ix = 0; ix < (صحیح)len; ++ix)
	Ext_متن_برآمد(hdc, ستون + متن_X(ix), صف, foptions,
					pcliprect, متن + ix, 1, padding);
}
#اگر_ختم

    ساکن خالی
draw_سطر(
    صحیح		x1,
    صحیح		y1,
    صحیح		x2,
    صحیح		y2,
    COLORREF	color)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_سطر(s_dwc, x1, y1, x2, y2, color);
    ورنہ
#اگر_ختم
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	Move_مقصد_Ex(s_hdc, x1, y1, تہی);
	// Note: LineTo() excludes the last pixel in the line.
	سطر_مقصد(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ساکن خالی
سیٹ_pixel(
    صحیح		x,
    صحیح		y,
    COLORREF	color)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWrite_Context_سیٹ_Pixel(s_dwc, x, y, color);
    ورنہ
#اگر_ختم
	سیٹ_Pixel(s_hdc, x, y, color);
}

    ساکن خالی
fill_rect(
    ثابت RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    ورنہ
#اگر_ختم
    {
	HBRUSH	hbr2;

	اگر (hbr == تہی)
	    hbr2 = CreateSolidBrush(color);
	ورنہ
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	اگر (hbr == تہی)
	    DeleteBrush(hbr2);
    }
}

    خالی
gui_mch_draw_string(
    صحیح		صف,
    صحیح		ستون,
    char_u	*متن,
    صحیح		len,
    صحیح		flags)
{
    ساکن صحیح	*padding = تہی;
    ساکن صحیح	pad_سائز = 0;
    ثابت RECT	*pcliprect = تہی;
    UINT	foptions = 0;
    ساکن WCHAR *unicodebuf = تہی;
    ساکن صحیح   *unicodepdy = تہی;
    ساکن صحیح	unibuflen = 0;
    صحیح		n = 0;
    صحیح		y;

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
    ساکن HBRUSH	hbr_cache[2] = {تہی, تہی};
    ساکن guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ساکن صحیح		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    اگر (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.بایاں = FILL_X(ستون);
	rc.چوٹی = FILL_Y(صف);
	اگر (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.دایاں = FILL_X(ستون + mb_string2cells(متن, len));
	}
	ورنہ
	    rc.دایاں = FILL_X(ستون + len);
	rc.bottom = FILL_Y(صف + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	اگر (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	ورنہ اگر (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	ورنہ
	{
	    اگر (hbr_cache[brush_lru] != تہی)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	سیٹ_Bk_منوال(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	اگر (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#اگر_تعریف_شدہ FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	ورنہ اگر (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#اگر_ختم
    }
    سیٹ_متن_Color(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#اگر_تعریف_شدہ FEAT_DIRECTX
    اگر (IS_ENABLE_DIRECTX())
	DWrite_Context_سیٹ_Font(s_dwc, (HFONT)gui.currFont);
#اگر_ختم

    اگر (pad_سائز != Columns || padding == تہی || padding[0] != gui.char_چوڑائی)
    {
	صحیح	i;

	vim_free(padding);
	pad_سائز = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(صحیح, pad_سائز);
	اگر (padding != تہی)
	    برائے (i = 0; i < pad_سائز; i++)
		padding[i] = gui.char_چوڑائی;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    اگر (enc_utf8)
	برائے (n = 0; n < len; ++n)
	    اگر (متن[n] >= 0x80)
		توڑو;

#اگر defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    اگر (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#اگر_ختم

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    اگر ((enc_utf8
		|| (enc_codepage > 0 && (صحیح)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == تہی || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(صحیح, len);

	unibuflen = len;
    }

    اگر (enc_utf8 && n < len && unicodebuf != تہی)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	صحیح		i;
	صحیح		wlen;	// string length in words
	صحیح		cells;	// cell width of string up to composing char
	صحیح		cw;	// width of current cell
	صحیح		c;

	wlen = 0;
	cells = 0;
	برائے (i = 0; i < len; )
	{
	    c = utf_ptr2char(متن + i);
	    اگر (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    ورنہ
	    {
		unicodebuf[wlen++] = c;
	    }

	    اگر (utf_iscomposing(c))
		cw = 0;
	    ورنہ
	    {
		cw = utf_char2cells(c);
		اگر (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    اگر (unicodepdy != تہی)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		اگر (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_چوڑائی;
		    unicodepdy[wlen - 1] = 0;
		}
		ورنہ
		    unicodepdy[wlen - 1] = cw * gui.char_چوڑائی;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(متن + i, len - i);
	}
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWrite_Context_Draw_متن(s_dwc, unicodebuf, wlen,
		    متن_X(ستون), متن_Y(صف),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_چوڑائی, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	ورنہ
#اگر_ختم
	    Ext_متن_برآمد_W(s_hdc, متن_X(ستون), متن_Y(صف),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    ورنہ اگر ((enc_codepage > 0 && (صحیح)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	اگر (unicodebuf != تہی)
	{
	    اگر (enc_latin9)
		latin9_مقصد_ucs(متن, len, unicodebuf);
	    ورنہ
		len = Multi_Byte_مقصد_Wide_Char(enc_codepage,
			MB_PRECOMPOSED,
			(حرف *)متن, len,
			(LPWSTR)unicodebuf, unibuflen);
	    اگر (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		اگر (unicodepdy != تہی)
		{
		    صحیح i;
		    صحیح cw;

		    برائے (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			اگر (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_چوڑائی;
		    }
		}
		Ext_متن_برآمد_W(s_hdc, متن_X(ستون), متن_Y(صف),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    ورنہ
    {
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	اگر (curwin->w_p_rl)
	    Rev_برآمد(s_hdc, متن_X(ستون), متن_Y(صف),
			 foptions, pcliprect, (حرف *)متن, len, padding);
	ورنہ
#اگر_ختم
	    Ext_متن_برآمد(s_hdc, متن_X(ستون), متن_Y(صف),
			 foptions, pcliprect, (حرف *)متن, len, padding);
    }

    // Underline
    اگر (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(صف + 1) - 1;
	اگر (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_سطر(FILL_X(ستون), y, FILL_X(ستون + len), y, gui.currFgColor);
    }

    // Strikethrough
    اگر (flags & DRAW_STRIKE)
    {
	y = FILL_Y(صف + 1) - gui.char_اونچائی/2;
	draw_سطر(FILL_X(ستون), y, FILL_X(ستون + len), y, gui.currSpColor);
    }

    // Undercurl
    اگر (flags & DRAW_UNDERC)
    {
	صحیح			x;
	صحیح			انحراف;
	ساکن ثابت صحیح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(صف + 1) - 1;
	برائے (x = FILL_X(ستون); x < FILL_X(ستون + len); ++x)
	{
	    انحراف = val[x % 8];
	    سیٹ_pixel(x, y - انحراف, gui.currSpColor);
	}
    }
}


/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    خالی
gui_mch_flush(خالی)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#اگر_ختم

    GdiFlush();
}

    ساکن خالی
clear_rect(RECT *rcp)
{
    fill_rect(rcp, تہی, gui.عقب_pixel);
}


    خالی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.دایاں - workarea_rect.بایاں
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.چوٹی
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_اونچائی(FALSE);
}


#اگر defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    خالی
gui_mch_جوڑو_menu(
    vimmenu_T	*menu,
    صحیح		pos)
{
    vimmenu_T	*والد = menu->والد;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    اگر (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_مقصد_utf16(menu->name, تہی);
	اگر (wn == تہی)
	    واپس;

	infow.cb_سائز = حجم(infow);
	infow.f_نقاب = MIIM_ڈیٹا | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_شے_ڈیٹا = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_ڈیٹا = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	داخل_کرو_Menu_شے_W((والد == تہی)
		? s_menuBar : والد->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    اگر (والد == تہی)
	gui_mswin_get_menu_اونچائی(!gui.starting);
# ifdef FEAT_TEAROFF
    ورنہ اگر (Is_کھڑکی(والد->tearoff_handle))
	rebuild_tearoff(والد);
# endif
}

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    نقطہ_2 mp;

    (خالی)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (صحیح)mp.x, (صحیح)mp.y);
}

    خالی
gui_make_popup(char_u *راستہ_name, صحیح mouse_pos)
{
    vimmenu_T	*menu = gui_ڈھونڈو_menu(راستہ_name);

    اگر (menu == تہی)
	واپس;

    نقطہ_2	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    اگر (mouse_pos)
    {
	صحیح	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    ورنہ اگر (curwin != تہی)
    {
	p.x += متن_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += متن_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    پیغام_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (صحیح)p.x, (صحیح)p.y);
}

# اگر defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    خالی
gui_make_tearoff(char_u *راستہ_name)
{
    vimmenu_T	*menu = gui_ڈھونڈو_menu(راستہ_name);

    // Found the menu, so tear it off.
    اگر (menu != تہی)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    خالی
gui_mch_جوڑو_menu_شے(
    vimmenu_T	*menu,
    صحیح		idx)
{
    vimmenu_T	*والد = menu->والد;

    menu->id = s_menu_id++;
    menu->submenu_id = تہی;

# ifdef FEAT_TEAROFF
    اگر (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	داخل_کرو_Menu(والد->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    ورنہ
# endif
# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(والد->name))
    {
	TBBUTTON newtb;

	CLEAR_میدان(newtb);
	اگر (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	ورنہ
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
    ورنہ
# endif
    {
	WCHAR	*wn;

	wn = enc_مقصد_utf16(menu->name, تہی);
	اگر (wn != تہی)
	{
	    داخل_کرو_Menu_W(والد->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	اگر (Is_کھڑکی(والد->tearoff_handle))
	    rebuild_tearoff(والد);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    خالی
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    اگر (menu->submenu_id == (HMENU)-1)
    {
	صحیح iButton;

	iButton = (صحیح)Send_پیغام(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	Send_پیغام(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    ورنہ
# endif
    {
	اگر (menu->والد != تہی
		&& menu_is_popup(menu->والد->dname)
		&& menu->والد->submenu_id != تہی)
	    RemoveMenu(menu->والد->submenu_id, menu->id, MF_BYCOMMAND);
	ورنہ
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	اگر (menu->submenu_id != تہی)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	اگر (Is_کھڑکی(menu->tearoff_handle))
	    Destroy_کھڑکی(menu->tearoff_handle);
	اگر (menu->والد != تہی
		&& menu->والد->children != تہی
		&& Is_کھڑکی(menu->والد->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->والد);
	}
# endif
    }
}

# ifdef FEAT_TEAROFF
    ساکن خالی
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    صحیح		x, y;

    HWND thwnd = menu->tearoff_handle;

    Get_کھڑکی_متن(thwnd, (LPSTR)tbuf, 127);
    اگر (Get_کھڑکی_Rect(thwnd, &trect)
	    && Get_کھڑکی_Rect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.بایاں - rct.بایاں;
	y = (trect.چوٹی -  rct.bottom  + roct.bottom);
    }
    ورنہ
    {
	x = y = 0xffffL;
    }
    Destroy_کھڑکی(thwnd);
    اگر (menu->children != تہی)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	اگر (Is_کھڑکی(menu->tearoff_handle))
	    (خالی) سیٹ_کھڑکی_Pos(menu->tearoff_handle,
				تہی,
				(صحیح)trect.بایاں,
				(صحیح)trect.چوٹی,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    خالی
gui_mch_menu_grey(
    vimmenu_T	*menu,
    صحیح	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    اگر (menu->submenu_id == (HMENU)-1)
	Send_پیغام(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    ورنہ
# endif
	(خالی)Enable_Menu_شے(menu->والد ? menu->والد->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    اگر ((menu->والد != تہی) && (Is_کھڑکی(menu->والد->tearoff_handle)))
    {
	لفظ_2 menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	اگر (menu->children == تہی)
	    menuID = (لفظ_2)(menu->id);
	ورنہ
	    menuID = (لفظ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = Get_Dlg_شے(menu->والد->tearoff_handle, menuID);
	اگر (menuHandle)
	    Enable_کھڑکی(menuHandle, !grey);

    }
# endif
}

#اگر_ختم // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#تعریف_کرو جوڑو_لفظ(x)		*p++ = (x)
#تعریف_کرو جوڑو_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (لفظ_2 *)dwp

#اگر defined(FEAT_GUI_DIALOG)
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
    ساکن LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT پیغام_2,
    WPARAM wParam,
    LPARAM lParam غیر_مستعمل)
{
    اگر (پیغام_2 == WM_INITDIALOG)
    {
	Center_کھڑکی(hwnd, Get_کھڑکی(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(خالی)سیٹ_Focus(hwnd);
	اگر (dialog_default_button > IDCANCEL)
	    (خالی)سیٹ_Focus(Get_Dlg_شے(hwnd, dialog_default_button));
	ورنہ
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (خالی)سیٹ_Focus(Get_Dlg_شے(hwnd, DLG_NONBUTTON_CONTROL));
	واپس FALSE;
    }

    اگر (پیغام_2 == WM_COMMAND)
    {
	صحیح	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	اگر (button >= DLG_NONBUTTON_CONTROL)
	    واپس TRUE;

	// If the edit box exists, copy the string.
	اگر (s_textfield != تہی)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    Get_Dlg_شے_متن_W(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_مقصد_enc(wp, تہی);
	    اگر (p != تہی)
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
	اگر (button == IDOK)
	{
	    اگر (dialog_default_button > IDCANCEL)
		انتہا_Dialog(hwnd, dialog_default_button);
	}
	ورنہ
	    انتہا_Dialog(hwnd, button - IDCANCEL);
	واپس TRUE;
    }

    اگر ((پیغام_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	انتہا_Dialog(hwnd, 0);
	واپس TRUE;
    }
    واپس FALSE;
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

ساکن ثابت حرف *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    صحیح
gui_mch_dialog(
    صحیح		 type,
    char_u	*title,
    char_u	*پیغام_2,
    char_u	*buttons,
    صحیح		 dfltbutton,
    char_u	*textfield,
    صحیح		ex_cmd غیر_مستعمل)
{
    لفظ_2	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    صحیح		عدد_Buttons;
    صحیح		*buttonWidths, *buttonPositions;
    صحیح		buttonYpos;
    صحیح		nchar, i;
    DWORD	lStyle;
    صحیح		dlgwidth = 0;
    صحیح		dlgheight;
    صحیح		editboxheight;
    صحیح		horiz_چوڑائی = 0;
    صحیح		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*آخری_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    صحیح		font_اونچائی_2;
    صحیح		متن_چوڑائی_2, min_Button_چوڑائی, پیغام_چوڑائی;
    صحیح		max_Dialog_چوڑائی;
    صحیح		max_Dialog_اونچائی;
    صحیح		scroll_flag = 0;
    صحیح		vertical;
    صحیح		dlgPaddingX;
    صحیح		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    صحیح		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    صحیح		l;
    صحیح		dlg_icon_چوڑائی;
    صحیح		dlg_icon_اونچائی;
    صحیح		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    اگر (!(gui.اندر_use || gui.starting))
#  endif
	اگر (silent_منوال)
	    واپس dfltbutton;   // return default option
# endif

    اگر (s_hwnd == تہی)
    {
	بھرو_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    ورنہ
	dpi = pGetDpiForSystem();

    اگر ((type < 0) || (type > VIM_آخری_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_سائز + STRLEN(پیغام_2) * 2);

    اگر (p == تہی)
	واپس -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    اگر (tbuffer == تہی)
	واپس -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    عدد_Buttons = 1;
    برائے (i = 0; tbuffer[i] != '\0'; i++)
    {
	اگر (tbuffer[i] == DLG_BUTTON_SEP)
	    عدد_Buttons++;
    }
    اگر (dfltbutton >= عدد_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(صحیح, عدد_Buttons);
    اگر (buttonWidths == تہی)
	واپس -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(صحیح, عدد_Buttons);
    اگر (buttonPositions == تہی)
	واپس -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = Get_Desktop_کھڑکی();
    hdc = Get_کھڑکی_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_بالواسطہ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ورنہ
# endif
	font = CreateFont(-DLG_FONT_نقطہ_سائز, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    Get_متن_Metrics(hdc, &fontInfo);
    font_اونچائی_2 = fontInfo.tm_اونچائی;

    // Minimum width for horizontal button
    min_Button_چوڑائی = Get_متن_چوڑائی(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    اگر (s_hwnd == تہی)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	max_Dialog_چوڑائی = workarea_rect.دایاں - workarea_rect.بایاں - 100;
	اگر (max_Dialog_چوڑائی > adjust_by_system_dpi(600))
	    max_Dialog_چوڑائی = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	max_Dialog_اونچائی = workarea_rect.bottom - workarea_rect.چوٹی - 150;
    }
    ورنہ
    {
	// Use our own window for the size, unless it's very small.
	Get_کھڑکی_Rect(s_hwnd, &rect);
	max_Dialog_چوڑائی = rect.دایاں - rect.بایاں
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	اگر (max_Dialog_چوڑائی < adjust_by_system_dpi(DLG_MIN_MAX_چوڑائی))
	    max_Dialog_چوڑائی = adjust_by_system_dpi(DLG_MIN_MAX_چوڑائی);

	max_Dialog_اونچائی = rect.bottom - rect.چوٹی
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	اگر (max_Dialog_اونچائی < adjust_by_system_dpi(DLG_MIN_MAX_اونچائی))
	    max_Dialog_اونچائی = adjust_by_system_dpi(DLG_MIN_MAX_اونچائی);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = پیغام_2;
    پیغام_چوڑائی = 0;
    msgheight = 0;
    ga_init2(&ga, حجم(حرف), 500);
    کرو
    {
	msgheight += font_اونچائی_2;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	متن_چوڑائی_2 = 0;
	آخری_white = تہی;
	برائے (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    اگر (l == 1 && VIM_ISWHITE(*pend)
					&& متن_چوڑائی_2 > max_Dialog_چوڑائی * 3 / 4)
		آخری_white = pend;
	    متن_چوڑائی_2 += Get_متن_چوڑائی_Enc(hdc, pend, l);
	    اگر (متن_چوڑائی_2 >= max_Dialog_چوڑائی)
	    {
		// Line will wrap.
		پیغام_چوڑائی = max_Dialog_چوڑائی;
		msgheight += font_اونچائی_2;
		متن_چوڑائی_2 = 0;

		اگر (آخری_white != تہی)
		{
		    // break the line just after a space
		    اگر (pend > آخری_white)
			ga.ga_len -= (صحیح)(pend - (آخری_white + 1));
		    pend = آخری_white + 1;
		    آخری_white = تہی;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		جاری;
	    }

	    جب (--l >= 0)
		ga_append(&ga, *pend++);
	}
	اگر (متن_چوڑائی_2 > پیغام_چوڑائی)
	    پیغام_چوڑائی = متن_چوڑائی_2;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } جب (*pend != NUL);

    اگر (ga.ga_ڈیٹا != تہی)
	پیغام_2 = ga.ga_ڈیٹا;

    پیغام_چوڑائی += 10;		// roundoff space

    dlg_icon_چوڑائی = adjust_by_system_dpi(DLG_ICON_چوڑائی);
    dlg_icon_اونچائی = adjust_by_system_dpi(DLG_ICON_اونچائی);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = پیغام_چوڑائی + dlg_icon_چوڑائی + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    اگر (msgheight < dlg_icon_اونچائی)
	msgheight = dlg_icon_اونچائی;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != تہی && vim_strchr(p_go, GO_VERTICAL) != تہی);
    اگر (!vertical)
    {
	// Place buttons horizontally if they fit.
	horiz_چوڑائی = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	کرو
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    اگر (pend == تہی)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    متن_چوڑائی_2 = Get_متن_چوڑائی_Enc(hdc, pstart, (صحیح)(pend - pstart));
	    اگر (متن_چوڑائی_2 < min_Button_چوڑائی)
		متن_چوڑائی_2 = min_Button_چوڑائی;
	    متن_چوڑائی_2 += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = متن_چوڑائی_2;
	    buttonPositions[i++] = horiz_چوڑائی;
	    horiz_چوڑائی += متن_چوڑائی_2 + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} جب (*pend != NUL);

	اگر (horiz_چوڑائی > max_Dialog_چوڑائی)
	    vertical = TRUE;	// Too wide to fit on the screen.
	ورنہ اگر (horiz_چوڑائی > dlgwidth)
	    dlgwidth = horiz_چوڑائی;
    }

    اگر (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	کرو
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    اگر (pend == تہی)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    متن_چوڑائی_2 = Get_متن_چوڑائی_Enc(hdc, pstart, (صحیح)(pend - pstart));
	    متن_چوڑائی_2 += dlgPaddingX;		// Padding within button
	    متن_چوڑائی_2 += DLG_VERT_PADDING_X * 2; // Padding around button
	    اگر (متن_چوڑائی_2 > dlgwidth)
		dlgwidth = متن_چوڑائی_2;
	    pstart = pend + 1;
	} جب (*pend != NUL);
    }

    اگر (dlgwidth < DLG_MIN_چوڑائی)
	dlgwidth = DLG_MIN_چوڑائی;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    جوڑو_long(lStyle);
    جوڑو_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    جوڑو_لفظ(0);	// NumberOfItems(will change later)
    جوڑو_لفظ(10);	// x
    جوڑو_لفظ(10);	// y
    جوڑو_لفظ(Pixel_مقصد_Dialog_X(dlgwidth));	// cx

    // Dialog height.
    اگر (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * font_اونچائی_2 * عدد_Buttons;
    ورنہ
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * font_اونچائی_2;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = font_اونچائی_2 + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    اگر (textfield != تہی)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    اگر (dlgheight > max_Dialog_اونچائی)
    {
	msgheight = msgheight - (dlgheight - max_Dialog_اونچائی);
	dlgheight = max_Dialog_اونچائی;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	پیغام_چوڑائی = dlgwidth - dlg_icon_چوڑائی - 3 * dlgPaddingX;
    }

    جوڑو_لفظ(Pixel_مقصد_Dialog_Y(dlgheight));

    جوڑو_لفظ(0);	// Menu
    جوڑو_لفظ(0);	// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_مقصد_Wide_Char(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_نسخہ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    اگر (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_اونچائی, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحیح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ورنہ
# endif
    {
	*p++ = DLG_FONT_نقطہ_سائز;		// point size
	nchar = n_Copy_Ansi_مقصد_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    اگر (textfield != تہی)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    اگر (!vertical)
	horiz_چوڑائی = (dlgwidth - horiz_چوڑائی) / 2;	// Now it's X offset
    برائے (i = 0; i < عدد_Buttons; i++)
    {
	// get end of this button.
	برائے (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	اگر (*pend)
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
	اگر (vertical)
	{
	    p = جوڑو_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_مقصد_Dialog_X(DLG_VERT_PADDING_X),
		    Pixel_مقصد_Dialog_Y(buttonYpos // TBK
				   + 2 * font_اونچائی_2 * i),
		    Pixel_مقصد_Dialog_X(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (لفظ_2)(Pixel_مقصد_Dialog_Y(2 * font_اونچائی_2) - 1),
		    (لفظ_2)(IDCANCEL + 1 + i), (لفظ_2)0x0080, (حرف *)pstart);
	}
	ورنہ
	{
	    p = جوڑو_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_مقصد_Dialog_X(horiz_چوڑائی + buttonPositions[i]),
		    Pixel_مقصد_Dialog_Y(buttonYpos), // TBK
		    Pixel_مقصد_Dialog_X(buttonWidths[i]),
		    (لفظ_2)(Pixel_مقصد_Dialog_Y(2 * font_اونچائی_2) - 1),
		    (لفظ_2)(IDCANCEL + 1 + i), (لفظ_2)0x0080, (حرف *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += عدد_Buttons;

    // Vim icon
    p = جوڑو_dialog_element(p, SS_ICON,
	    Pixel_مقصد_Dialog_X(dlgPaddingX),
	    Pixel_مقصد_Dialog_Y(dlgPaddingY),
	    Pixel_مقصد_Dialog_X(dlg_icon_چوڑائی),
	    Pixel_مقصد_Dialog_Y(dlg_icon_اونچائی),
	    DLG_NONBUTTON_CONTROL + 0, (لفظ_2)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = جوڑو_dialog_element(p, ES_بایاں|scroll_flag|ES_MULTILINE|ES_READONLY,
	    Pixel_مقصد_Dialog_X(2 * dlgPaddingX + dlg_icon_چوڑائی),
	    Pixel_مقصد_Dialog_Y(dlgPaddingY),
	    (لفظ_2)(Pixel_مقصد_Dialog_X(پیغام_چوڑائی) + 1),
	    Pixel_مقصد_Dialog_Y(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (لفظ_2)0x0081, (حرف *)پیغام_2);

    // Edit box
    اگر (textfield != تہی)
    {
	p = جوڑو_dialog_element(p, ES_بایاں|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		Pixel_مقصد_Dialog_X(2 * dlgPaddingX),
		Pixel_مقصد_Dialog_Y(2 * dlgPaddingY + msgheight),
		Pixel_مقصد_Dialog_X(dlgwidth - 4 * dlgPaddingX),
		Pixel_مقصد_Dialog_Y(font_اونچائی_2 + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (لفظ_2)0x0081, (حرف *)textfield);
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
    اگر (textfield != تہی)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    ورنہ
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = تہی;
    }

    // show the dialog box modally and get a return value
    nchar = (صحیح)Dialog_Box_بالواسطہ(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_ڈیٹا);

    // Focus back to our window (for when MDI is used).
    (خالی)سیٹ_Focus(s_hwnd);

    واپس nchar;
}

#اگر_ختم // FEAT_GUI_DIALOG

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
    ساکن PWORD
جوڑو_dialog_element(
    PWORD p,
    DWORD lStyle,
    لفظ_2 x,
    لفظ_2 y,
    لفظ_2 w,
    لفظ_2 h,
    لفظ_2 Id,
    لفظ_2 clss,
    ثابت حرف *caption)
{
    صحیح nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_بچہ;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (لفظ_2)0xffff;
    *p++ = clss;			//2 more here

    nchar = n_Copy_Ansi_مقصد_Wide_Char(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    واپس p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ساکن LPWORD
lpwAlign(
    LPWORD lp_اندر)
{
    long_u ul;

    ul = (long_u)lp_اندر;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    واپس (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ساکن صحیح
n_Copy_Ansi_مقصد_Wide_Char(
    LPWORD lpWCStr,
    LPSTR lp_Ansi_اندر,
    BOOL use_enc)
{
    صحیح		nChar = 0;
    صحیح		len = lstrlen(lp_Ansi_اندر) + 1;	// include NUL character
    صحیح		i;
    WCHAR	*wn;

    اگر (use_enc && enc_codepage >= 0 && (صحیح)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_مقصد_utf16((char_u *)lp_Ansi_اندر, تہی);
	اگر (wn != تہی)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (صحیح)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    اگر (nChar == 0)
	// Use Win32 conversion function.
	nChar = Multi_Byte_مقصد_Wide_Char(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lp_Ansi_اندر, len,
		lpWCStr, len);
    برائے (i = 0; i < nChar; ++i)
	اگر (lpWCStr[i] == (لفظ_2)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (لفظ_2)' ';

    واپس nChar;
}


#اگر_تعریف_شدہ FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ساکن HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    لفظ_2 menu_id)
{
    برائے ( ; menu != تہی; menu = menu->اگلا)
    {
	اگر (menu->modes == 0)	// this menu has just been deleted
	    جاری;
	اگر (menu_is_separator(menu->dname))
	    جاری;
	اگر ((لفظ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    واپس menu->submenu_id;
    }
    واپس تہی;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ساکن LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT پیغام_2,
    WPARAM wParam,
    LPARAM lParam)
{
    اگر (پیغام_2 == WM_INITDIALOG)
    {
	سیٹ_کھڑکی_Long_Ptr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	واپس (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(پیغام_2, lParam);

    اگر (پیغام_2 == WM_COMMAND)
    {
	اگر ((لفظ_2)(LOWORD(wParam)) & 0x8000)
	{
	    نقطہ_2   mp;
	    RECT    rect;

	    اگر (GetCursorPos(&mp) && Get_کھڑکی_Rect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)Get_کھڑکی_Long_Ptr(hwnd, DWLP_USER);
		(خالی)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (صحیح)rect.دایاں - 8,
			 (صحیح)mp.y,
			 (صحیح)0,	    // reserved param
			 s_hwnd,
			 تہی);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	ورنہ
	    // Pass on messages to the main Vim window
	    Post_پیغام(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(خالی)سیٹ_Focus(s_hwnd);
	واپس TRUE;
    }
    اگر ((پیغام_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	Destroy_کھڑکی(hwnd);
	واپس TRUE;
    }

    // When moved around, give main window the focus back.
    اگر (پیغام_2 == WM_EXITSIZEMOVE)
	(خالی)سیٹ_فعال_کھڑکی(s_hwnd);

    واپس FALSE;
}
#اگر_ختم


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ساکن خالی
get_dialog_font_metrics(خالی)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    سائز_3	    سائز;
#اگر_تعریف_شدہ USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#اگر_ختم

#اگر_تعریف_شدہ USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = Create_Font_بالواسطہ_W(&lfSysmenu);
    ورنہ
#اگر_ختم
	hfontTools = CreateFont(-DLG_FONT_نقطہ_سائز, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_اوسط_قدر_Font_سائز(hdc, &سائز);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (لفظ_2)سائز.cx;
    s_dlgfntheight = (لفظ_2)سائز.cy;
}

#اگر defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ساکن خالی
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    صحیح		initX,
    صحیح		initY)
{
    لفظ_2	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    صحیح		template_len;
    صحیح		nchar, متن_چوڑائی_2, submenu_چوڑائی;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    لفظ_2	dlgwidth;
    لفظ_2	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*چوٹی_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    صحیح		ستون, space_چوڑائی, len;
    صحیح		عمود_Widths[2];
    char_u	*label, *متن;
    صحیح		acLen = 0;
    صحیح		nameLen;
    صحیح		padding0, padding1, padding2 = 0;
    صحیح		sepPadding=0;
    صحیح		x;
    صحیح		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    صحیح		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    اگر (Is_کھڑکی(menu->tearoff_handle))
    {
	نقطہ_2 mp;
	اگر (GetCursorPos(&mp))
	{
	    سیٹ_کھڑکی_Pos(menu->tearoff_handle, تہی, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	واپس;
    }

    /*
     * Create a new tearoff.
     */
    اگر (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_سائز;
    pdlgtemplate = p = (لفظ_2 *)LocalAlloc(LPTR, template_len);
    اگر (p == تہی)
	واپس;

    hwnd = Get_Desktop_کھڑکی();
    hdc = Get_کھڑکی_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_بالواسطہ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ورنہ
# endif
	font = CreateFont(-DLG_FONT_نقطہ_سائز, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    space_چوڑائی = Get_متن_چوڑائی(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenu_چوڑائی = 0;
    برائے (ستون = 0; ستون < 2; ستون++)
    {
	عمود_Widths[ستون] = 0;
	FOR_ALL_بچہ_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    متن = (ستون == 0) ? pmenu->dname : pmenu->actext;
	    اگر (متن != تہی && *متن != NUL)
	    {
		متن_چوڑائی_2 = Get_متن_چوڑائی_Enc(hdc, متن, (صحیح)STRLEN(متن));
		اگر (متن_چوڑائی_2 > عمود_Widths[ستون])
		    عمود_Widths[ستون] = متن_چوڑائی_2;
	    }
	    اگر (pmenu->children != تہی)
		submenu_چوڑائی = TEAROFF_عمود_PADDING * space_چوڑائی;
	}
    }
    اگر (عمود_Widths[1] == 0)
    {
	// no accelerators
	اگر (submenu_چوڑائی != 0)
	    عمود_Widths[0] += submenu_چوڑائی;
	ورنہ
	    عمود_Widths[0] += space_چوڑائی;
    }
    ورنہ
    {
	// there is an accelerator column
	عمود_Widths[0] += TEAROFF_عمود_PADDING * space_چوڑائی;
	عمود_Widths[1] += submenu_چوڑائی;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    متن_چوڑائی_2 = عمود_Widths[0] + عمود_Widths[1];
    اگر (submenu_چوڑائی != 0)
    {
	submenu_چوڑائی = Get_متن_چوڑائی(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (صحیح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	متن_چوڑائی_2 += submenu_چوڑائی;
    }
    dlgwidth = Get_متن_چوڑائی_Enc(hdc, title, (صحیح)STRLEN(title));
    اگر (متن_چوڑائی_2 > dlgwidth)
	dlgwidth = متن_چوڑائی_2;
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
    اگر (initX == 0xffffL)
	*p++ = Pixel_مقصد_Dialog_X(x); // x
    ورنہ
	*p++ = Pixel_مقصد_Dialog_X(initX); // x
    اگر (initY == 0xffffL)
	*p++ = Pixel_مقصد_Dialog_Y(y); // y
    ورنہ
	*p++ = Pixel_مقصد_Dialog_Y(initY); // y
    *p++ = Pixel_مقصد_Dialog_X(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_مقصد_Wide_Char(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_نسخہ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    اگر (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_اونچائی, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحیح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ورنہ
# endif
    {
	*p++ = DLG_FONT_نقطہ_سائز;		// point size
	nchar = n_Copy_Ansi_مقصد_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    اگر (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->اگلا;
    ورنہ
	menu = menu->children;
    چوٹی_menu = menu;
    برائے ( ; menu != تہی; menu = menu->اگلا)
    {
	اگر (menu->modes == 0)	// this menu has just been deleted
	    جاری;
	اگر (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    جاری;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	اگر (((حرف *)p - (حرف *)pdlgtemplate) + 1000 > template_len)
	{
	    لفظ_2    *newp;

	    newp = (لفظ_2 *)LocalAlloc(LPTR, template_len + 4096);
	    اگر (newp != تہی)
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
	len = nameLen = (صحیح)STRLEN(menu->name);
	padding0 = (عمود_Widths[0] - Get_متن_چوڑائی_Enc(hdc, menu->dname,
				      (صحیح)STRLEN(menu->dname))) / space_چوڑائی;
	len += padding0;

	اگر (menu->actext != تہی)
	{
	    acLen = (صحیح)STRLEN(menu->actext);
	    len += acLen;
	    متن_چوڑائی_2 = Get_متن_چوڑائی_Enc(hdc, menu->actext, acLen);
	}
	ورنہ
	    متن_چوڑائی_2 = 0;
	padding1 = (عمود_Widths[1] - متن_چوڑائی_2) / space_چوڑائی;
	len += padding1;

	اگر (menu->children == تہی)
	{
	    padding2 = submenu_چوڑائی / space_چوڑائی;
	    len += padding2;
	    menuID = (لفظ_2)(menu->id);
	}
	ورنہ
	{
	    len += (صحیح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (لفظ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	متن = label = alloc(len + 1);
	اگر (label == تہی)
	    توڑو;

	vim_strncpy(متن, menu->name, nameLen);
	متن = vim_strchr(متن, TAB);	    // stop at TAB before actext
	اگر (متن == تہی)
	    متن = label + nameLen;	    // no actext, use whole name
	جب (padding0-- > 0)
	    *متن++ = ' ';
	اگر (menu->actext != تہی)
	{
	    STRNCPY(متن, menu->actext, acLen);
	    متن += acLen;
	}
	جب (padding1-- > 0)
	    *متن++ = ' ';
	اگر (menu->children != تہی)
	{
	    STRCPY(متن, TEAROFF_SUBMENU_LABEL);
	    متن += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	ورنہ
	{
	    جب (padding2-- > 0)
		*متن++ = ' ';
	}
	*متن = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = جوڑو_dialog_element(p,
		BS_PUSHBUTTON|BS_بایاں,
		(لفظ_2)Pixel_مقصد_Dialog_X(TEAROFF_PADDING_X),
		(لفظ_2)(sepPadding + 1 + 13 * (*pnumitems)),
		(لفظ_2)Pixel_مقصد_Dialog_X(dlgwidth - 2 * TEAROFF_PADDING_X),
		(لفظ_2)12,
		menuID, (لفظ_2)0x0080, (حرف *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (لفظ_2)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = Create_Dialog_بالواسطہ_Param(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback,
	    (LPARAM)چوٹی_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (خالی)سیٹ_فعال_کھڑکی(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#اگر_ختم

#اگر defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ساکن خالی
initialise_toolbar(خالی)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_بچہ | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    تہی,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_چوڑائی, //api guide is wrong!
		    TOOLBAR_BUTTON_اونچائی,
		    TOOLBAR_BUTTON_چوڑائی,
		    TOOLBAR_BUTTON_اونچائی,
		    حجم(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    Send_پیغام(s_toolbarhwnd, TB_SETSTYLE, 0,
	Send_پیغام(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = Subclass_کھڑکی(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != تہی);

    update_toolbar_سائز();
}

    ساکن خالی
update_toolbar_سائز(خالی)
{
    صحیح		w, h;
    TBMETRICS	tbm;

    tbm.cb_سائز = حجم(TBMETRICS);
    tbm.dw_نقاب = TBMF_PAD | TBMF_BUTTONSPACING;
    Send_پیغام(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_چوڑائی + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_اونچائی + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    Send_پیغام(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_اونچائی = h + 6;

    //DWORD s = SendMessage(s_toolbarhwnd, TB_GETBUTTONSIZE, 0, 0);
    //TRACE("actual button size: %d, %d", LOWORD(s), HIWORD(s));

    // TODO:
    // Currently, this function only updates the size of toolbar buttons.
    // It would be nice if the toolbar images are resized based on DPI.
}

    ساکن LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_پیغام, lParam);
    واپس Call_کھڑکی_Proc(s_toolbar_wndproc, hwnd, u_پیغام, wParam, lParam);
}

    ساکن صحیح
get_toolbar_bitmap(vimmenu_T *menu)
{
    صحیح i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    اگر (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = تہی;

	اگر (menu->iconfile != تہی)
	{
	    gui_ڈھونڈو_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = بھرو_Image(
			تہی,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_چوڑائی,
			TOOLBAR_BUTTON_اونچائی,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	اگر (hbitmap == تہی
		&& (gui_ڈھونڈو_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != تہی ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = بھرو_Image(
		    تہی,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_چوڑائی,
		    TOOLBAR_BUTTON_اونچائی,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	اگر (hbitmap != تہی)
	{
	    TBADDBITMAP tb_جوڑو_Bitmap;

	    tb_جوڑو_Bitmap.hInst = تہی;
	    tb_جوڑو_Bitmap.nID = (long_u)hbitmap;

	    i = (صحیح)Send_پیغام(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tb_جوڑو_Bitmap);
	    // i will be set to -1 if it fails
	}
    }
    اگر (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_گنتی)
	i = menu->iconidx;

    واپس i;
}
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE)
    ساکن خالی
initialise_tabline(خالی)
{
    InitCommonControls();

    s_tabhwnd = Create_کھڑکی(WC_TABCONTROL, "Vim tabline",
	    WS_بچہ|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, تہی, g_hinst, تہی);
    s_tabline_wndproc = Subclass_کھڑکی(s_tabhwnd, tabline_wndproc);

    gui.tabline_اونچائی = TABLINE_اونچائی;

    سیٹ_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ساکن tabpage_T *
Get_Tab_From_نقطہ(
    HWND    hWnd,
    نقطہ_2   pt)
{
    tabpage_T	*ptp = تہی;

    اگر (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	اگر (s_tabhwnd == hWnd)
	{
	    صحیح idx = Tab_Ctrl_کامیابی_Test(s_tabhwnd, &htinfo);
	    اگر (idx != -1)
		ptp = ڈھونڈو_tabpage(idx + 1);
	}
    }
    واپس ptp;
}

ساکن نقطہ_2	    s_pt = {0, 0};
ساکن HCURSOR      s_hCursor = تہی;

    ساکن LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_پیغام,
    WPARAM wParam,
    LPARAM lParam)
{
    نقطہ_2	pt;
    tabpage_T	*tp;
    RECT	rect;
    صحیح		nCenter;
    صحیح		idx0;
    صحیح		idx1;

    HandleMouseHide(u_پیغام, lParam);

    انتخاب (u_پیغام)
    {
	حالت WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		سیٹ_Capture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		توڑو;
	    }
	حالت WM_MOUSEMOVE:
	    اگر (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		اگر (مطلق_قدر(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    سیٹ_Cursor(بھرو_Cursor(تہی, IDC_SIZEWE));

		    tp = Get_Tab_From_نقطہ(hwnd, pt);
		    اگر (tp != تہی)
		    {
			idx0 = tabpage_اشاریہ(curtab) - 1;
			idx1 = tabpage_اشاریہ(tp) - 1;

			Tab_Ctrl_Get_شے_Rect(hwnd, idx1, &rect);
			nCenter = rect.بایاں + (rect.دایاں - rect.بایاں) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			اگر ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			ورنہ اگر ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    توڑو;
	حالت WM_LBUTTONUP:
	    {
		اگر (GetCapture() == hwnd)
		{
		    سیٹ_Cursor(s_hCursor);
		    ReleaseCapture();
		}
		توڑو;
	    }
	حالت WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = Tab_Ctrl_کامیابی_Test(hwnd, &htinfo);
		اگر (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		توڑو;
	    }
	طےشدہ:
	    توڑو;
    }

    واپس Call_کھڑکی_Proc(s_tabline_wndproc, hwnd, u_پیغام, wParam, lParam);
}
#اگر_ختم

#اگر defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    خالی
gui_mch_سیٹ_foreground(خالی)
{
    اگر (IsIconic(s_hwnd))
	 Send_پیغام(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    سیٹ_Foreground_کھڑکی(s_hwnd);
}
#اگر_ختم

#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ساکن خالی
dyn_imm_بھرو(خالی)
{
    hLibImm = vim_بھرو_Lib("imm32.dll");
    اگر (hLibImm == تہی)
	واپس;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))Get_Proc_پتہ(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))Get_Proc_پتہ(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))Get_Proc_پتہ(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))Get_Proc_پتہ(hLibImm, "ImmReleaseContext");
    p_Imm_Get_Open_حالت
	    = (BOOL (WINAPI *)(HIMC))Get_Proc_پتہ(hLibImm, "ImmGetOpenStatus");
    p_Imm_سیٹ_Open_حالت
	    = (BOOL (WINAPI *)(HIMC, BOOL))Get_Proc_پتہ(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_پتہ(hLibImm, "ImmGetCompositionFontW");
    p_Imm_سیٹ_Composition_Font_W
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_پتہ(hLibImm, "ImmSetCompositionFontW");
    p_Imm_سیٹ_Composition_کھڑکی
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))Get_Proc_پتہ(hLibImm, "ImmSetCompositionWindow");
    p_Imm_Get_Conversion_حالت
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))Get_Proc_پتہ(hLibImm, "ImmGetConversionStatus");
    p_Imm_سیٹ_Conversion_حالت
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))Get_Proc_پتہ(hLibImm, "ImmSetConversionStatus");

    اگر (       pImmGetCompositionStringW == تہی
	    || pImmGetContext == تہی
	    || pImmAssociateContext == تہی
	    || pImmReleaseContext == تہی
	    || p_Imm_Get_Open_حالت == تہی
	    || p_Imm_سیٹ_Open_حالت == تہی
	    || pImmGetCompositionFontW == تہی
	    || p_Imm_سیٹ_Composition_Font_W == تہی
	    || p_Imm_سیٹ_Composition_کھڑکی == تہی
	    || p_Imm_Get_Conversion_حالت == تہی
	    || p_Imm_سیٹ_Conversion_حالت == تہی)
    {
	FreeLibrary(hLibImm);
	hLibImm = تہی;
	pImmGetContext = تہی;
	واپس;
    }

    واپس;
}

#اگر_ختم

#اگر defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

قسمتعریف ساخت _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    خالی
gui_mch_drawsign(صحیح صف, صحیح ستون, صحیح typenr)
{
    signicon_t *sign;
    صحیح x, y, w, h;

    اگر (!gui.اندر_use || (sign = (signicon_t *)sign_get_image(typenr)) == تہی)
	واپس;

# اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = متن_X(ستون);
    y = متن_Y(صف);
    w = gui.char_چوڑائی * 2;
    h = gui.char_اونچائی;
    انتخاب (sign->uType)
    {
	حالت IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		ثنائی_ہندسہ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    توڑو;
	حالت IMAGE_ICON:
	حالت IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, تہی, DI_معمول);
	    توڑو;
# ifdef FEAT_XPM_W32
	حالت IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		ثنائی_ہندسہ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		ثنائی_ہندسہ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    توڑو;
# endif
    }
}

    ساکن خالی
close_signicon_image(signicon_t *sign)
{
    اگر (sign == تہی)
	واپس;

    انتخاب (sign->uType)
    {
	حالت IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    توڑو;
	حالت IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    توڑو;
	حالت IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    توڑو;
# ifdef FEAT_XPM_W32
	حالت IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    توڑو;
# endif
    }
}

    خالی *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = تہی;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    اگر (ext > signfile)
    {
	صحیح do_بھرو = 1;

	اگر (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	ورنہ اگر (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	ورنہ اگر (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	ورنہ
	    do_بھرو = 0;

	اگر (do_بھرو)
	    sign.hImage = (HANDLE)بھرو_Image(تہی, (LPCSTR)signfile, sign.uType,
		    gui.char_چوڑائی * 2, gui.char_اونچائی,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	اگر (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    بھرو_Xpm_Image((حرف *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = تہی;
    اگر (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != تہی)
	*psign = sign;

    اگر (!psign)
    {
	اگر (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_اندر_sign_ڈیٹا));
    }
    واپس (خالی *)psign;

}

    خالی
gui_mch_destroy_sign(خالی *sign)
{
    اگر (sign == تہی)
	واپس;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#اگر_ختم

#اگر defined(FEAT_BEVAL_GUI)

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

    ساکن خالی
make_tooltip(BalloonEval *beval, حرف *متن, نقطہ_2 pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    اگر (pti == تہی)
	واپس;

    beval->balloon = Create_کھڑکی_Ex_W(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    تہی, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->ہدف, تہی, g_hinst, تہی);

    سیٹ_کھڑکی_Pos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_سائز = حجم(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->ہدف;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpsz_متن = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_مقصد_utf16((char_u*)متن, تہی);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    اگر (GetClientRect(s_متن_Area, &rect))
	Send_پیغام_W(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.دایاں);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.بایاں = pt.x - 3;
    pti->rect.چوٹی = pt.y - 3;
    pti->rect.دایاں = pt.x + 3;
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

    ساکن خالی
delete_tooltip(BalloonEval *beval)
{
    Post_پیغام(beval->balloon, WM_CLOSE, 0, 0);
}

    ساکن VOID CALLBACK
beval_timer_proc(
    HWND	hwnd غیر_مستعمل,
    UINT	u_پیغام غیر_مستعمل,
    UINT_PTR	idEvent غیر_مستعمل,
    DWORD	dw_وقت)
{
    نقطہ_2	pt;
    RECT	rect;

    اگر (cur_beval == تہی || cur_beval->showState == ShS_SHOWING || !p_beval)
	واپس;

    GetCursorPos(&pt);
    اگر (کھڑکی_From_نقطہ(pt) != s_متن_Area)
	واپس;

    Screen_مقصد_Client(s_متن_Area, &pt);
    GetClientRect(s_متن_Area, &rect);
    اگر (!Pt_اندر_Rect(&rect, pt))
	واپس;

    اگر (آخری_user_activity > 0
	    && (dw_وقت - آخری_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| مطلق_قدر(cur_beval->x - pt.x) > 3
		|| مطلق_قدر(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	اگر (cur_beval->پیغام_CB != تہی)
	    (*cur_beval->پیغام_CB)(cur_beval, 0);
    }
}

    خالی
gui_mch_disable_beval_area(BalloonEval *beval غیر_مستعمل)
{
    KillTimer(s_متن_Area, beval_timer_id);
}

    خالی
gui_mch_enable_beval_area(BalloonEval *beval)
{
    اگر (beval == تہی)
	واپس;
    beval_timer_id = سیٹ_Timer(s_متن_Area, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    خالی
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    نقطہ_2   pt;

    vim_free(beval->پیغام);
    beval->پیغام = mesg == تہی ? تہی : vim_strsave(mesg);
    اگر (beval->پیغام == تہی)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	واپس;
    }

    اگر (beval->showState == ShS_SHOWING)
	واپس;
    GetCursorPos(&pt);
    Screen_مقصد_Client(s_متن_Area, &pt);

    اگر (مطلق_قدر(beval->x - pt.x) < 3 && مطلق_قدر(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (حرف *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    خالی	*ہدف غیر_مستعمل,	// ignored, always use s_textArea
    char_u	*mesg,
    خالی	(*mesgCB)(BalloonEval *, صحیح),
    خالی	*client_ڈیٹا)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    اگر (mesg != تہی && mesgCB != تہی)
    {
	iemsg(e_cannot_create_ballooneval_with_both_پیغام_and_callback);
	واپس تہی;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    اگر (beval != تہی)
    {
	beval->ہدف = s_متن_Area;

	beval->showState = ShS_NEUTRAL;
	beval->پیغام = mesg;
	beval->پیغام_CB = mesgCB;
	beval->client_ڈیٹا = client_ڈیٹا;

	InitCommonControls();
	cur_beval = beval;

	اگر (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    واپس beval;
}

    ساکن خالی
Handle_WM_Notify(HWND hwnd غیر_مستعمل, LPNMHDR pnmh)
{
    اگر (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	واپس;

    اگر (cur_beval == تہی)
	واپس;

    انتخاب (pnmh->رمز)
    {
	حالت TTN_SHOW:
	    توڑو;
	حالت TTN_نکالو: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    توڑو;
	حالت TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpsz_متن = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    توڑو;
	حالت TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpsz_متن = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    توڑو;
    }
}

    ساکن خالی
track_user_activity(UINT u_پیغام)
{
    اگر ((u_پیغام >= WM_MOUSEFIRST && u_پیغام <= WM_MOUSELAST)
	    || (u_پیغام >= WM_KEYFIRST && u_پیغام <= WM_KEYLAST))
	آخری_user_activity = Get_Tick_گنتی();
}

    خالی
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#اگر_ختم // FEAT_BEVAL_GUI

#اگر defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    خالی
netbeans_draw_multisign_indicator(صحیح صف)
{
    صحیح i;
    صحیح y;
    صحیح x;

    اگر (!netbeans_فعال())
	واپس;

    x = 0;
    y = متن_Y(صف);

# اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    برائے (i = 0; i < gui.char_اونچائی - 3; i++)
	سیٹ_Pixel(s_hdc, x+2, y++, gui.currFgColor);

    سیٹ_Pixel(s_hdc, x+0, y, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+4, y++, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+1, y, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+3, y++, gui.currFgColor);
    سیٹ_Pixel(s_hdc, x+2, y, gui.currFgColor);
}
#اگر_ختم

#اگر defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ساکن صحیح
test_gui_w32_sendevent_mouse(dict_T *دلائل)
{
    اگر (!dict_has_کلید(دلائل, "row") || !dict_has_کلید(دلائل, "col"))
	واپس FALSE;

    // Note: "move" is optional, requires fewer arguments
    صحیح move = (صحیح)dict_get_bool(دلائل, "move", FALSE);

    اگر (!move && (!dict_has_کلید(دلائل, "button")
	    || !dict_has_کلید(دلائل, "multiclick")
	    || !dict_has_کلید(دلائل, "modifiers")))
	واپس FALSE;

    صحیح صف = (صحیح)dict_get_نمبر(دلائل, "row");
    صحیح ستون = (صحیح)dict_get_نمبر(دلائل, "col");

    اگر (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	اگر (dict_get_bool(دلائل, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    صحیح pY = (صف - 1) * gui.char_اونچائی + gui.char_اونچائی / 2;
	    صحیح pX = (ستون - 1) * gui.char_چوڑائی + gui.char_چوڑائی / 2;
	    gui_mouse_moved(pX, pY);
	}
	ورنہ
	    gui_mouse_moved(ستون, صف);
    }
    ورنہ
    {
	صحیح button = (صحیح)dict_get_نمبر(دلائل, "button");
	صحیح repeated_click = (صحیح)dict_get_نمبر(دلائل, "multiclick");
	int_u mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_سیٹ_hor_scroll_قدم(6);
	mouse_سیٹ_vert_scroll_قدم(3);

	gui_send_mouse_event(button, متن_X(ستون - 1), متن_Y(صف - 1),
							repeated_click, mods);
    }
    واپس TRUE;
}

    ساکن صحیح
test_gui_w32_sendevent_keyboard(dict_T *دلائل)
{
    داخلہ_2 inputs[1];
    داخلہ_2 modkeys[3];
    Secure_Zero_حافظہ(inputs, حجم(داخلہ_2));
    Secure_Zero_حافظہ(modkeys, 3 * حجم(داخلہ_2));

    char_u *event = dict_get_string(دلائل, "event", TRUE);

    اگر (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	لفظ_2 vk_رمز = dict_get_نمبر_def(دلائل, "keycode", 0);
	اگر (vk_رمز <= 0 || vk_رمز >= 0xFF)
	{
	    semsg(_(e_invalid_دلیل_nr), (طویل)vk_رمز);
	    واپس FALSE;
	}

	BOOL is_Mod_کلید = (vk_رمز == VK_سرکاؤ || vk_رمز == VK_CONTROL
	    || vk_رمز == VK_MENU || vk_رمز == VK_LSHIFT || vk_رمز == VK_RSHIFT
	    || vk_رمز == VK_LCONTROL || vk_رمز == VK_RCONTROL
	    || vk_رمز == VK_LMENU || vk_رمز == VK_RMENU );

	BOOL unwrapMods = FALSE;
	صحیح mods = (صحیح)dict_get_نمبر(دلائل, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	اگر (mods && STRICMP(event, "keydown") == 0 && !is_Mod_کلید)
	{
	    صحیح n = 0;
	    اگر (mods & MOD_نقاب_سرکاؤ)
	    {
		modkeys[n].type = داخلہ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    اگر (mods & MOD_نقاب_CTRL)
	    {
		modkeys[n].type = داخلہ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    اگر (mods & MOD_نقاب_ALT)
	    {
		modkeys[n].type = داخلہ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    اگر (n)
	    {
		(خالی)سیٹ_Foreground_کھڑکی(s_hwnd);
		Send_داخلہ(n, modkeys, حجم(داخلہ_2));
	    }
	}

	inputs[0].type = داخلہ_KEYBOARD;
	inputs[0].ki.wVk = vk_رمز;
	اگر (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    اگر (!is_Mod_کلید)
		unwrapMods = TRUE;
	}

	(خالی)سیٹ_Foreground_کھڑکی(s_hwnd);
	Send_داخلہ(ARRAYSIZE(inputs), inputs, حجم(داخلہ_2));
	vim_free(event);

	اگر (unwrapMods)
	{
	    modkeys[0].type = داخلہ_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = داخلہ_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = داخلہ_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (خالی)سیٹ_Foreground_کھڑکی(s_hwnd);
	    Send_داخلہ(3, modkeys, حجم(داخلہ_2));
	}
    }
    ورنہ
    {
	اگر (event == تہی)
	{
	    semsg(_(e_missing_دلیل_str), "event");
	}
	ورنہ
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", event);
	    vim_free(event);
	}
	واپس FALSE;
    }
    واپس TRUE;
}

    ساکن صحیح
test_gui_w32_sendevent_سیٹ_keycode_trans_strategy(dict_T *دلائل)
{
    صحیح handled = 0;
    char_u *strategy = dict_get_string(دلائل, "strategy", TRUE);

    اگر (strategy)
    {
	اگر (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_مستعمل = &keycode_trans_strategy_classic;
	}
	ورنہ اگر (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_مستعمل = &keycode_trans_strategy_experimental;
	}
    }

    اگر (!handled)
    {
	اگر (strategy == تہی)
	{
	    semsg(_(e_missing_دلیل_str), "strategy");
	}
	ورنہ
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	واپس FALSE;
    }
    واپس TRUE;
}


    صحیح
test_gui_w32_sendevent(char_u *event, dict_T *دلائل)
{
    اگر (STRICMP(event, "key") == 0)
	واپس test_gui_w32_sendevent_keyboard(دلائل);
    ورنہ اگر (STRICMP(event, "mouse") == 0)
	واپس test_gui_w32_sendevent_mouse(دلائل);
    ورنہ اگر (STRICMP(event, "set_keycode_trans_strategy") == 0)
	واپس test_gui_w32_sendevent_سیٹ_keycode_trans_strategy(دلائل);
    ورنہ
    {
	semsg(_(e_invalid_قدر_for_دلیل_str_str), "event", event);
	واپس FALSE;
    }
}
#اگر_ختم

#اگر defined(FEAT_IMAGE_GUI)

قسمتعریف ساخت
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
    ساکن خالی
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, صحیح iw, صحیح ih, image_format_T fmt)
{
    صحیح		y, x;
    صحیح		stride = iw * 4;
    صحیح		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.عقب_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    برائے (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	برائے (x = 0; x < iw; x++)
	{
	    اگر (fmt == IMAGE_FORMAT_RGBA)
	    {
		صحیح a = s[3];

		اگر (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		ورنہ اگر (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		ورنہ
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    ورنہ
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

    صحیح
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    صحیح		iw, ih;
    خالی	*بٹ = تہی;

    اگر (ctx == تہی)
	واپس FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_میدان(bmi);
    bmi.bmi_سرخی.bi_سائز = حجم(BITMAPINFOHEADER);
    bmi.bmi_سرخی.bi_چوڑائی = iw;
    bmi.bmi_سرخی.bi_اونچائی = -ih; // Top-down
    bmi.bmi_سرخی.biPlanes = 1;
    bmi.bmi_سرخی.bi_ثنائی_ہندسہ_گنتی = 32;
    bmi.bmi_سرخی.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &بٹ, تہی, 0);
    اگر (hbm == تہی || بٹ == تہی)
    {
	اگر (hbm != تہی)
	    DeleteObject(hbm);
	واپس false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    اگر (mem_dc == تہی)
    {
	DeleteObject(hbm);
	واپس false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)بٹ, img->ڈیٹا, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_ڈیٹا = ctx;
    واپس OK;
}

    خالی
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_ڈیٹا;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    صحیح
image_placement_gui_init(image_placement_T *رکھو غیر_مستعمل)
{
    واپس OK;
}

    خالی
image_placement_gui_uninit(image_placement_T *رکھو غیر_مستعمل)
{
}

    خالی
image_placement_gui_draw(image_placement_T *رکھو)
{
    image_T	    *img = رکھو->img;
    image_gdi_T	    *ictx = img->backend_ڈیٹا;
    pixman_box32_t  *rects;
    صحیح		    n_rects;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    اگر (rects == تہی || n_rects == 0)
	واپس;

# اگر defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		صف, ستون;
	صحیح		posx, posy;
	صحیح		x, y, w, h;

	image_placement_subrect(رکھو, rect, &صف, &ستون, &x, &y, &w, &h, true);
	posx = FILL_X(ستون);
	posy = FILL_Y(صف);

	ثنائی_ہندسہ_Blt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    خالی
image_placement_gui_clear(image_placement_T *رکھو غیر_مستعمل)
{
}

#اگر_ختم // FEAT_IMAGE_GUI
