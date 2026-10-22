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

#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_চাবি_OFF			0	// no dead key
#define DEAD_চাবি_SET_DEFAULT		1	// dead key pressed
#define DEAD_চাবি_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_চাবি_SKIP_ON_CHAR		3	// skip next _OnChar()

#যদি defined(FEAT_DIRECTX)
স্থির DWriteContext *s_dwc = ফাঁকা;
স্থির পূর্ণসংখ্যা s_directx_enabled = 0;
স্থির পূর্ণসংখ্যা s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ফাঁকা && enc_utf8)
স্থির পূর্ণসংখ্যা directx_enabled(শূন্য);
স্থির শূন্য directx_binddc(শূন্য);
#endif

#ifdef FEAT_MENU
স্থির পূর্ণসংখ্যা gui_mswin_get_menu_height(পূর্ণসংখ্যা fix_window);
#অন্যথা
# define gui_mswin_get_menu_height(fix_window)	0
#endif

প্রকারলেখ কাঠামো keycode_trans_strategy {
    শূন্য (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
    শূন্য (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
    শূন্য (*ptr_process_message_usual_চাবি) (UINT /*vk*/, ধ্রুবক বার্তা_2* /*pmsg*/);
    পূর্ণসংখ্যা  (*ptr_get_active_modifiers)(শূন্য);
    পূর্ণসংখ্যা  (*is_experimental)(শূন্য);
} keycode_trans_strategy;

// forward declarations for input instance initializer
স্থির শূন্য _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
স্থির শূন্য _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
স্থির শূন্য process_message_usual_চাবি_experimental(UINT /*vk*/, ধ্রুবক বার্তা_2* /*pmsg*/);
স্থির পূর্ণসংখ্যা  get_active_modifiers_experimental(শূন্য);
স্থির পূর্ণসংখ্যা  is_experimental_true(শূন্য);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_চাবি_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
স্থির শূন্য _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
স্থির শূন্য _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, পূর্ণসংখ্যা /*cRepeat UNUSED*/);
স্থির শূন্য process_message_usual_চাবি_classic(UINT /*vk*/, ধ্রুবক বার্তা_2* /*pmsg*/);
স্থির পূর্ণসংখ্যা  get_active_modifiers_classic(শূন্য);
স্থির পূর্ণসংখ্যা  is_experimental_false(শূন্য);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_চাবি_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = ফাঁকা;

স্থির পূর্ণসংখ্যা is_experimental_true(শূন্য)
{
    ফেরত 1;
}

স্থির পূর্ণসংখ্যা is_experimental_false(শূন্য)
{
    ফেরত 0;
}

/*
 * Initialize the keycode translation strategy.
 */
স্থির শূন্য keycode_trans_strategy_init(শূন্য)
{
    ধ্রুবক অক্ষর *strategy = ফাঁকা;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    যদি (strategy == ফাঁকা)
    {
	ফেরত;
    }

    যদি (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	ফেরত;
    }

    যদি (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	ফেরত;
    }

}

#যদি defined(FEAT_RENDER_OPTIONS)
    পূর্ণসংখ্যা
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    পূর্ণসংখ্যা	    dx_enable = 0;
    পূর্ণসংখ্যা	    dx_flags = 0;
    দশমিক   dx_gamma = 0.0f;
    দশমিক   dx_contrast = 0.0f;
    দশমিক   dx_level = 0.0f;
    পূর্ণসংখ্যা	    dx_geom = 0;
    পূর্ণসংখ্যা	    dx_renmode = 0;
    পূর্ণসংখ্যা	    dx_taamode = 0;
    // parse string as rendering options.
    জন্য (p = s; p != ফাঁকা && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  মান[128];

	copy_option_part(&p, item, আকার(item), ",");
	যদি (p == ফাঁকা)
	    বিরতি;
	q = &item[0];
	copy_option_part(&q, name, আকার(name), ":");
	যদি (q == ফাঁকা)
	    ফেরত FAIL;
	copy_option_part(&q, মান, আকার(মান), ":");

	যদি (STRCMP(name, "type") == 0)
	{
	    যদি (STRCMP(মান, "directx") == 0)
		dx_enable = 1;
	    অন্যথা
		ফেরত FAIL;
	}
	অন্যথা যদি (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (দশমিক)atof((অক্ষর *)মান);
	}
	অন্যথা যদি (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (দশমিক)atof((অক্ষর *)মান);
	}
	অন্যথা যদি (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (দশমিক)atof((অক্ষর *)মান);
	}
	অন্যথা যদি (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((অক্ষর *)মান);
	    যদি (dx_geom < 0 || dx_geom > 2)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((অক্ষর *)মান);
	    যদি (dx_renmode < 0 || dx_renmode > 6)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((অক্ষর *)মান);
	    যদি (dx_taamode < 0 || dx_taamode > 3)
		ফেরত FAIL;
	}
	অন্যথা যদি (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	অন্যথা
	    ফেরত FAIL;
    }

    যদি (!gui.in_use)
	ফেরত OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    যদি (dx_enable)
    {
	যদি (!directx_enabled())
	    ফেরত FAIL;
	DWriteContext_SetRenderingParams(s_dwc, ফাঁকা);
	যদি (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    যদি (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    যদি (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    যদি (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    যদি (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    যদি (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    যদি (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ফেরত OK;
# অন্যথা
    ফেরত FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (পূর্ণসংখ্যা)(short)LOWORD(lParam), (পূর্ণসংখ্যা)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (পূর্ণসংখ্যা)(short)LOWORD(lParam), (পূর্ণসংখ্যা)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (পূর্ণসংখ্যা)(short)LOWORD(lParam), (পূর্ণসংখ্যা)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#অন্তর্ভাব "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#অন্তর্ভাব <shellapi.h>
#অন্তর্ভাব <commctrl.h>
#অন্তর্ভাব <windowsx.h>

#ifdef FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#endif

// Some parameters for dialog boxes.  All in pixels.
#define DLG_PADDING_X		10
#define DLG_PADDING_Y		10
#define DLG_VERT_PADDING_X	4	// For vertical buttons
#define DLG_VERT_PADDING_Y	4
#define DLG_ICON_WIDTH		34
#define DLG_ICON_HEIGHT		34
#define DLG_MIN_WIDTH		150
#define DLG_FONT_NAME		"MS Shell Dlg"
#define DLG_FONT_POINT_আকার	8
#define DLG_MIN_MAX_WIDTH	400
#define DLG_MIN_MAX_HEIGHT	400

#define DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#ifndef WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#endif

#ifndef WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#endif

#ifndef WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#endif

#ifndef SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#endif

#ifndef SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
# define DWMWA_USE_IMMERSIVE_DARK_MODE	20
#endif

#ifndef DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#endif

#ifndef DWMWA_TEXT_COLOR
# define DWMWA_TEXT_COLOR		36
#endif

স্থির শূন্য _OnPaint(HWND hwnd);
স্থির শূন্য fill_rect(ধ্রুবক RECT *rcp, HBRUSH hbr, COLORREF color);
স্থির শূন্য clear_rect(RECT *rcp);

স্থির WORD		s_dlgfntheight;		// height of the dialog font
স্থির WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
স্থির HMENU		s_menuBar = ফাঁকা;
#endif
#ifdef FEAT_TEAROFF
স্থির শূন্য rebuild_tearoff(vimmenu_T *menu);
স্থির HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
স্থির পূর্ণসংখ্যা		s_busy_processing = FALSE;

স্থির পূর্ণসংখ্যা		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
স্থির UINT		s_findrep_বার্তা = 0;
স্থির FINDREPLACEW	s_findrep_struct;
স্থির HWND		s_findrep_hwnd = ফাঁকা;
স্থির পূর্ণসংখ্যা		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = ফাঁকা;
স্থির HDC		s_hdc = ফাঁকা;
স্থির HBRUSH		s_brush = ফাঁকা;

#ifdef FEAT_TOOLBAR
স্থির HWND		s_toolbarhwnd = ফাঁকা;
স্থির WNDPROC		s_toolbar_wndproc = ফাঁকা;
#endif

#ifdef FEAT_GUI_TABLINE
স্থির HWND		s_tabhwnd = ফাঁকা;
স্থির WNDPROC		s_tabline_wndproc = ফাঁকা;
স্থির পূর্ণসংখ্যা		showing_tabline = 0;
#endif

স্থির WPARAM		s_wParam = 0;

স্থির HWND		s_textArea = ফাঁকা;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
স্থির bool		s_textarea_bg_only = false;

স্থির UINT		s_u_বার্তা = 0;

স্থির char_u		*s_textfield; // Used by dialogs to pass back strings

স্থির পূর্ণসংখ্যা		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
স্থির পূর্ণসংখ্যা allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
প্রকারলেখ HANDLE DPI_AWARENESS_CONTEXT;

প্রকারলেখ গণনা DPI_AWARENESS {
    DPI_AWARENESS_INVALID	    = -1,
    DPI_AWARENESS_UNAWARE	    = 0,
    DPI_AWARENESS_SYSTEM_AWARE	    = 1,
    DPI_AWARENESS_PER_MONITOR_AWARE = 2
} DPI_AWARENESS;

# define DPI_AWARENESS_CONTEXT_UNAWARE		    ((DPI_AWARENESS_CONTEXT)-1)
# define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE	    ((DPI_AWARENESS_CONTEXT)-2)
# define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
# define DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
# define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
#endif

#define DEFAULT_DPI	96
স্থির পূর্ণসংখ্যা		s_dpi = DEFAULT_DPI;
স্থির BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
স্থির DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
স্থির RECT		s_suggested_rect;

স্থির UINT (WINAPI *pGetDpiForSystem)(শূন্য) = ফাঁকা;
স্থির UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = ফাঁকা;
স্থির পূর্ণসংখ্যা (WINAPI *pGetSystemMetricsForDpi)(পূর্ণসংখ্যা, UINT) = ফাঁকা;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
স্থির DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = ফাঁকা;
#ifdef FEAT_MBYTE_IME
স্থির DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ফাঁকা;
#endif

স্থির HINSTANCE hLibDwm = ফাঁকা;
স্থির HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
স্থির শূন্য dyn_dwm_load(শূন্য);

স্থির পূর্ণসংখ্যা fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

স্থির HINSTANCE hUxThemeLib = ফাঁকা;
স্থির DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = ফাঁকা;
স্থির শূন্য (WINAPI *pFlushMenuThemes)(শূন্য) = ফাঁকা;
স্থির শূন্য dyn_uxtheme_load(শূন্য);
#endif

    স্থির পূর্ণসংখ্যা WINAPI
stubGetSystemMetricsForDpi(পূর্ণসংখ্যা n_সূচক, UINT dpi UNUSED)
{
    ফেরত GetSystemMetrics(n_সূচক);
}

    স্থির পূর্ণসংখ্যা
adjust_fontsize_by_dpi(পূর্ণসংখ্যা আকার_2)
{
    ফেরত আকার_2 * s_dpi / (পূর্ণসংখ্যা)pGetDpiForSystem();
}

    স্থির পূর্ণসংখ্যা
adjust_by_system_dpi(পূর্ণসংখ্যা আকার_2)
{
    ফেরত আকার_2 * (পূর্ণসংখ্যা)pGetDpiForSystem() / DEFAULT_DPI;
}

#যদি defined(FEAT_DIRECTX)
    স্থির পূর্ণসংখ্যা
directx_enabled(শূন্য)
{
    যদি (s_dwc != ফাঁকা)
	ফেরত 1;
    অন্যথা যদি (s_directx_load_attempted)
	ফেরত 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ফেরত s_dwc != ফাঁকা ? 1 : 0;
}

    স্থির শূন্য
directx_binddc(শূন্য)
{
    যদি (s_textArea == ফাঁকা)
	ফেরত;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

বাহ্যিক পূর্ণসংখ্যা current_font_height;	    // this is in os_mswin.c

স্থির কাঠামো
{
    UINT    চাবি_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_চাবিগুলি[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_বাম,		'k', 'l'},
    {VK_ডান,		'k', 'r'},

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
#ifdef FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#endif
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_BACK,		'k', 'b'},
    {VK_ঢোকাও,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_END,		'@', '7'},
    {VK_PRIOR,		'k', 'P'},
    {VK_পরবর্তী,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_ADD,		'K', '6'},
    {VK_SUBTRACT,	'K', '7'},
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
স্থির পূর্ণসংখ্যা	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
স্থির পূর্ণসংখ্যা	s_getting_focus = FALSE;

স্থির পূর্ণসংখ্যা	s_x_pending;
স্থির পূর্ণসংখ্যা	s_y_pending;
স্থির UINT	s_kFlags_pending;
স্থির UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
স্থির পূর্ণসংখ্যা	s_timed_out = FALSE;
স্থির পূর্ণসংখ্যা	dead_চাবি = DEAD_চাবি_OFF;
স্থির UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
স্থির শূন্য Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
স্থির শূন্য track_user_activity(UINT u_বার্তা);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
স্থির LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
স্থির LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
স্থির LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_তথ্য);
#endif

#যদি defined(FEAT_BROWSE)
স্থির char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    স্থির শূন্য
print_windows_error(শূন্য)
{
    LPVOID  lp_বার্তা_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_বাফার | FORMAT_MESSAGE_FROM_SYSTEM,
		  ফাঁকা, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_বার্তা_Buf, 0, ফাঁকা);
    TRACE1("Error: %s\n", lp_বার্তা_Buf);
    LocalFree(lp_বার্তা_Buf);
}
#endif

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

স্থির পূর্ণসংখ্যা		blink_state = BLINK_NONE;
স্থির long_u		blink_waittime = 700;
স্থির long_u		blink_ontime = 400;
স্থির long_u		blink_offtime = 250;
স্থির UINT_PTR		blink_timer = 0;

    পূর্ণসংখ্যা
gui_mch_is_blinking(শূন্য)
{
    ফেরত blink_state != BLINK_NONE;
}

    পূর্ণসংখ্যা
gui_mch_is_blink_off(শূন্য)
{
    ফেরত blink_state == BLINK_OFF;
}

    শূন্য
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    স্থির VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_বার্তা UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    বার্তা_2 বার্তা;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ফাঁকা, idEvent);

    // Eat spurious WM_TIMER messages
    যখন (PeekMessageW(&বার্তা, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    যদি (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(ফাঁকা, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    অন্যথা
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(ফাঁকা, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    স্থির শূন্য
gui_mswin_rm_blink_timer(শূন্য)
{
    বার্তা_2 বার্তা;

    যদি (blink_timer == 0)
	ফেরত;

    KillTimer(ফাঁকা, blink_timer);
    // Eat spurious WM_TIMER messages
    যখন (PeekMessageW(&বার্তা, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    শূন্য
gui_mch_stop_blink(পূর্ণসংখ্যা may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    যদি (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    শূন্য
gui_mch_start_blink(শূন্য)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    যদি (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(ফাঁকা, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    স্থির VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_বার্তা UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    বার্তা_2 বার্তা;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ফাঁকা, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    যখন (PeekMessageW(&বার্তা, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    যদি (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    স্থির শূন্য
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    dead_চাবি = DEAD_চাবি_SET_DEFAULT;
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
    স্থির পূর্ণসংখ্যা
char_to_string(পূর্ণসংখ্যা ch, char_u *string, পূর্ণসংখ্যা slen, পূর্ণসংখ্যা had_alt)
{
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i;
    WCHAR	wstring[2];
    char_u	*ws = ফাঁকা;

    যদি (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    অন্যথা যদি (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ফেরত 0;
    }
    অন্যথা
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    যদি (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ফাঁকা);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	যদি (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ফাঁকা);
	    যদি (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    অন্যথা
    {
	ws = utf16_to_enc(wstring, &len);
	যদি (ws == ফাঁকা)
	    len = 0;
	অন্যথা
	{
	    যদি (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    যদি (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    জন্য (i = 0; i < len; ++i)
	যদি (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (পূর্ণসংখ্যা)KE_CSI;
	    len += 2;
	}

    ফেরত len;
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
    স্থির পূর্ণসংখ্যা
get_active_modifiers_experimental(শূন্য)
{
    পূর্ণসংখ্যা modifiers = 0;

    যদি (Get_চাবি_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    যদি (Get_চাবি_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    যদি (Get_চাবি_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    যদি ((modifiers & MOD_MASK_CTRL) && (Get_চাবি_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    যদি (!(Get_চাবি_State(VK_CONTROL) & 0x8000)
	    &&  (Get_চাবি_State(VK_RMENU) & 0x8000)
	    && !(Get_চাবি_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    ফেরত modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    স্থির পূর্ণসংখ্যা
get_active_modifiers_classic(শূন্য)
{
    পূর্ণসংখ্যা modifiers = 0;

    যদি (Get_চাবি_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    যদি (Get_চাবি_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    যদি (Get_চাবি_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    ফেরত modifiers;
}

    স্থির পূর্ণসংখ্যা
get_active_modifiers(শূন্য)
{
    ফেরত get_active_modifiers_experimental();
}

    স্থির পূর্ণসংখ্যা
get_active_modifiers_via_ptr(শূন্য)
{
    // marshal to corresponding implementation
    ফেরত keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    স্থির শূন্য
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    স্থির শূন্য
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    char_u	string[40];
    পূর্ণসংখ্যা		len = 0;
    পূর্ণসংখ্যা		modifiers;
    পূর্ণসংখ্যা		ch = cch;   // special keys are negative

    যদি (dead_চাবি == DEAD_চাবি_SKIP_ON_CHAR)
	ফেরত;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    যদি (dead_চাবি != DEAD_চাবি_TRANSIENT_IN_ON_CHAR)
	dead_চাবি = DEAD_চাবি_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_চাবি(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_চাবিগুলি;
    ch = may_adjust_চাবি_for_ctrl(modifiers, ch);
    --no_reduce_চাবিগুলি;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ফাঁকা);
    যদি (ch == CSI)
	ch = K_CSI;

    যদি (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_প্রবেশ_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    যদি (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_প্রবেশ_buf();
	got_int = TRUE;
    }

    add_to_প্রবেশ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    স্থির শূন্য
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    char_u	string[40];
    পূর্ণসংখ্যা		len = 0;

    dead_চাবি = 0;

    len = char_to_string(ch, string, 40, FALSE);
    যদি (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_প্রবেশ_buf();
	got_int = TRUE;
    }

    add_to_প্রবেশ_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    স্থির শূন্য
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    স্থির শূন্য
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		modifiers;
    পূর্ণসংখ্যা		ch = cch;   // special keys are negative

    dead_চাবি = DEAD_চাবি_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_চাবি(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ফাঁকা);
    যদি (ch == CSI)
	ch = K_CSI;

    len = 0;
    যদি (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    যদি (IS_SPECIAL((পূর্ণসংখ্যা)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((পূর্ণসংখ্যা)ch);
	string[len++] = K_THIRD((পূর্ণসংখ্যা)ch);
    }
    অন্যথা
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_প্রবেশ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    স্থির শূন্য
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    পূর্ণসংখ্যা cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		modifiers;
    পূর্ণসংখ্যা		ch = cch;   // special keys are negative

    dead_চাবি = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    যদি (Get_চাবি_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    যদি (Get_চাবি_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_চাবি(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ফাঁকা);
    যদি (ch == CSI)
	ch = K_CSI;

    len = 0;
    যদি (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    যদি (IS_SPECIAL((পূর্ণসংখ্যা)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((পূর্ণসংখ্যা)ch);
	string[len++] = K_THIRD((পূর্ণসংখ্যা)ch);
    }
    অন্যথা
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_প্রবেশ_buf(string, len);
}

    স্থির শূন্য
_OnMouseEvent(
    পূর্ণসংখ্যা button,
    পূর্ণসংখ্যা x,
    পূর্ণসংখ্যা y,
    পূর্ণসংখ্যা repeated_click,
    UINT চাবি_Flags)
{
    পূর্ণসংখ্যা vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    যদি (চাবি_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    যদি (চাবি_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    যদি (Get_চাবি_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    স্থির শূন্য
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    পূর্ণসংখ্যা x,
    পূর্ণসংখ্যা y,
    UINT চাবি_Flags)
{
    স্থির LONG	s_পূর্ব_Time = 0;

    LONG    currentTime = GetMessageTime();
    পূর্ণসংখ্যা	    button = -1;
    পূর্ণসংখ্যা	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (শূন্য)SetFocus(s_hwnd);

    যদি (s_u_বার্তা == WM_LBUTTONDOWN || s_u_বার্তা == WM_LBUTTONDBLCLK)
	button = MOUSE_বাম;
    অন্যথা যদি (s_u_বার্তা == WM_MBUTTONDOWN || s_u_বার্তা == WM_MBUTTONDBLCLK)
	button = MOUSE_মধ্য;
    অন্যথা যদি (s_u_বার্তা == WM_RBUTTONDOWN || s_u_বার্তা == WM_RBUTTONDBLCLK)
	button = MOUSE_ডান;
    অন্যথা যদি (s_u_বার্তা == WM_XBUTTONDOWN || s_u_বার্তা == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    অন্যথা যদি (s_u_বার্তা == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	যদি (s_button_pending == MOUSE_বাম)
	    button = MOUSE_ডান;
	অন্যথা
	    button = MOUSE_বাম;
    }

    যদি (button < 0)
	ফেরত;

    repeated_click = ((পূর্ণসংখ্যা)(currentTime - s_পূর্ব_Time) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    যদি (repeated_click
	    && ((button == MOUSE_বাম && s_button_pending == MOUSE_ডান)
		|| (button == MOUSE_ডান
		    && s_button_pending == MOUSE_বাম)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_মধ্য;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, চাবি_Flags);
    }
    অন্যথা যদি ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ডান)))
    {
	যদি (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, চাবি_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, চাবি_Flags);
    }
    অন্যথা
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
	s_kFlags_pending = চাবি_Flags;
    }

    s_পূর্ব_Time = currentTime;
}

    স্থির শূন্য
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    পূর্ণসংখ্যা x,
    পূর্ণসংখ্যা y,
    UINT চাবি_Flags)
{
    পূর্ণসংখ্যা button;

    s_getting_focus = FALSE;
    যদি (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    যদি (s_u_বার্তা == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	যদি (!(চাবি_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ফেরত;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    অন্যথা
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, চাবি_Flags);
}

    স্থির শূন্য
On_আকার_Text_Area(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    পূর্ণসংখ্যা cx UNUSED,
    পূর্ণসংখ্যা cy UNUSED)
{
#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    স্থির vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    পূর্ণসংখ্যা		id)
{
    vimmenu_T	*pChildMenu;

    যখন (pMenu)
    {
	যদি (pMenu->id == (UINT)id)
	    বিরতি;
	যদি (pMenu->children != ফাঁকা)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    যদি (pChildMenu)
	    {
		pMenu = pChildMenu;
		বিরতি;
	    }
	}
	pMenu = pMenu->পরবর্তী;
    }
    ফেরত pMenu;
}

    স্থির শূন্য
_OnMenu(
    HWND	hwnd UNUSED,
    পূর্ণসংখ্যা		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(মূল_menu, id);
    যদি (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    স্থির শূন্য
_OnFindRepl(শূন্য)
{
    পূর্ণসংখ্যা	    flags = 0;
    পূর্ণসংখ্যা	    down;

    যদি (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(শূন্য)SetFocus(s_hwnd);

    যদি (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(শূন্য)SetFocus(s_hwnd);
    }
    অন্যথা যদি (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(শূন্য)SetFocus(s_hwnd);
    }
    অন্যথা যদি (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    যদি (flags == 0)
	ফেরত;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    যদি (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    যদি (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, ফাঁকা);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, ফাঁকা);
    যদি (p != ফাঁকা && q != ফাঁকা)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    স্থির শূন্য
HandleMouseHide(UINT u_বার্তা, LPARAM lParam)
{
    স্থির LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    যদি (u_বার্তা == WM_MOUSEMOVE || u_বার্তা == WM_NCMOUSEMOVE)
    {
	যদি (lParam == last_lParam)
	    ফেরত;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    নির্বাচন (u_বার্তা)
    {
    ক্ষেত্র WM_KEYUP:
    ক্ষেত্র WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	যদি (p_mh)
	    gui_mch_mousehide(TRUE);
	বিরতি;

    ক্ষেত্র WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    ক্ষেত্র WM_SYSCHAR:
    ক্ষেত্র WM_MOUSEMOVE:	 // show the pointer on any mouse action
    ক্ষেত্র WM_LBUTTONDOWN:
    ক্ষেত্র WM_LBUTTONUP:
    ক্ষেত্র WM_MBUTTONDOWN:
    ক্ষেত্র WM_MBUTTONUP:
    ক্ষেত্র WM_RBUTTONDOWN:
    ক্ষেত্র WM_RBUTTONUP:
    ক্ষেত্র WM_XBUTTONDOWN:
    ক্ষেত্র WM_XBUTTONUP:
    ক্ষেত্র WM_NCMOUSEMOVE:
    ক্ষেত্র WM_NCLBUTTONDOWN:
    ক্ষেত্র WM_NCLBUTTONUP:
    ক্ষেত্র WM_NCMBUTTONDOWN:
    ক্ষেত্র WM_NCMBUTTONUP:
    ক্ষেত্র WM_NCRBUTTONDOWN:
    ক্ষেত্র WM_NCRBUTTONUP:
    ক্ষেত্র WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	বিরতি;
    }
}

    স্থির LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_বার্তা,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_বার্তা, lParam);

    s_u_বার্তা = u_বার্তা;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_বার্তা);
#endif

    নির্বাচন (u_বার্তা)
    {
	HANDLE_বার্তা(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_বার্তা(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_বার্তা(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_বার্তা(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_বার্তা(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_বার্তা(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_বার্তা(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_বার্তা(hwnd, WM_আকার,	On_আকার_Text_Area);

#ifdef FEAT_BEVAL_GUI
	ক্ষেত্র WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ফেরত TRUE;
#endif
	পূর্বনির্ধারিত:
	    ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
    }
}

    স্থির শূন্য
dyn_dwm_load(শূন্য)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    যদি (hLibDwm == ফাঁকা)
	ফেরত;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    যদি (pDwmSetWindowAttribute == ফাঁকা)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ফাঁকা;
	ফেরত;
    }
}

বাহ্যিক DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    শূন্য
gui_mch_set_titlebar_colors(শূন্য)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    যদি (pDwmSetWindowAttribute == ফাঁকা || win_version < MAKE_VER(10, 0, 22000))
	ফেরত;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    যদি (vim_strchr(p_go, GO_TITLEBAR) != ফাঁকা)
    {
	যদি (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	অন্যথা
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	যদি (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	যদি (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, আকার(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, আকার(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    শূন্য
gui_mch_new_colors(শূন্য)
{
    HBRUSH পূর্ব_Brush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    পূর্ব_Brush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ফাঁকা, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    যদি (s_textarea_bg_only && s_textArea != ফাঁকা)
	InvalidateRect(s_textArea, ফাঁকা, TRUE);
    DeleteObject(পূর্ব_Brush);
}

/*
 * Set the colors to their default values.
 */
    শূন্য
gui_mch_def_colors(শূন্য)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    পূর্ণসংখ্যা
gui_mch_open(শূন্য)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    যদি (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	যদি (s_textArea != ফাঁকা)
	{
	    UpdateWindow(s_textArea);
	    gui_mch_flush();
	}
    }

#ifdef MSWIN_FIND_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#endif

    ফেরত OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    পূর্ণসংখ্যা
gui_mch_get_winpos(পূর্ণসংখ্যা *x, পূর্ণসংখ্যা *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.বাম;
    *y = rect.শীর্ষ_2;
    ফেরত OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    শূন্য
gui_mch_set_winpos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    SetWindowPos(s_hwnd, ফাঁকা, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    শূন্য
gui_mch_set_text_area_pos(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, পূর্ণসংখ্যা w, পূর্ণসংখ্যা h)
{
    স্থির পূর্ণসংখ্যা oldx = 0;
    স্থির পূর্ণসংখ্যা oldy = 0;

    SetWindowPos(s_textArea, ফাঁকা, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    যদি (vim_strchr(p_go, GO_TOOLBAR) != ফাঁকা)
	SendMessage(s_toolbarhwnd, WM_আকার,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#যদি defined(FEAT_GUI_TABLINE)
    যদি (showing_tabline)
    {
	পূর্ণসংখ্যা	শীর্ষ_2 = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	যদি (vim_strchr(p_go, GO_TOOLBAR) != ফাঁকা)
	    শীর্ষ_2 = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, শীর্ষ_2, rect.ডান, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    যদি (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ফাঁকা, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    শূন্য
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    পূর্ণসংখ্যা		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    শূন্য
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    পূর্ণসংখ্যা		x,
    পূর্ণসংখ্যা		y,
    পূর্ণসংখ্যা		w,
    পূর্ণসংখ্যা		h)
{
    SetWindowPos(sb->id, ফাঁকা, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    RECT    rcTxt, rcWnd;
    পূর্ণসংখ্যা	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.ডান - rcTxt.ডান - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ফেরত (xpad < 0) ? 0 : xpad;
}

    পূর্ণসংখ্যা
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    RECT    rcTxt, rcWnd;
    পূর্ণসংখ্যা	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ফেরত (ypad < 0) ? 0 : ypad;
}

    শূন্য
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    পূর্ণসংখ্যা		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ফাঁকা,
	g_hinst, ফাঁকা);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    স্থির scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    যদি (gui.bottom_sbar.id == hwnd)
	ফেরত &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	যদি (wp->w_scrollbars[SBAR_বাম].id == hwnd)
	    ফেরত &wp->w_scrollbars[SBAR_বাম];
	যদি (wp->w_scrollbars[SBAR_ডান].id == hwnd)
	    ফেরত &wp->w_scrollbars[SBAR_ডান];
    }
    ফেরত ফাঁকা;
}

    স্থির শূন্য
update_scrollbar_আকার(শূন্য)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    স্থির শূন্য
Get_Average_Font_আকার(HDC hdc, আকার_4 *আকার_2)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    GetTextExtentPoint(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, আকার_2);
    আকার_2->cx = (আকার_2->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    স্থির শূন্য
Get_Font_আকার(GuiFont font, পূর্ণসংখ্যা *char_width, পূর্ণসংখ্যা *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    আকার_4    আকার_2;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    Get_Average_Font_আকার(hdc, &আকার_2);

    যদি (char_width)
	*char_width = আকার_2.cx + tm.tmOverhang;
    যদি (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    স্থির শূন্য
Update_Font_আকার(GuiFont font)
{
    Get_Font_আকার(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    পূর্ণসংখ্যা
gui_mch_adjust_charheight(শূন্য)
{
    Update_Font_আকার(gui.norm_font);
    ফেরত OK;
}

    স্থির GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ফাঁকা;

    // Load the font
    font = CreateFontIndirectW(lf);

    যদি (font == ফাঁকা)
	ফেরত NOFONT;

    ফেরত (GuiFont)font;
}

    স্থির পূর্ণসংখ্যা
pixels_to_points(পূর্ণসংখ্যা pixels, পূর্ণসংখ্যা vertical)
{
    পূর্ণসংখ্যা		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ফেরত points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    পূর্ণসংখ্যা		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    যদি (get_logfont(&lf, name, ফাঁকা, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    যদি (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ফেরত font;
}

#যদি defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    যদি (name == ফাঁকা)
	ফেরত ফাঁকা;
    ফেরত vim_strsave(name);
}
#endif

    শূন্য
gui_mch_free_font(GuiFont font)
{
    যদি (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    পূর্ণসংখ্যা i;

    প্রকারলেখ কাঠামো SysColorTable
    {
	অক্ষর	    *name;
	পূর্ণসংখ্যা	    color;
    } SysColorTable;

    স্থির SysColorTable sys_table[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#ifdef COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#endif
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
	{"SYS_WINDOW", COLOR_WINDOW},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    জন্য (i = 0; i < (পূর্ণসংখ্যা)ARRAY_দৈর্ঘ্য(sys_table); i++)
	যদি (STRICMP(name, sys_table[i].name) == 0)
	    ফেরত GetSysColor(sys_table[i].color);

    ফেরত gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(পূর্ণসংখ্যা r, পূর্ণসংখ্যা g, পূর্ণসংখ্যা b)
{
    ফেরত gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    পূর্ণসংখ্যা
gui_mch_haskey(char_u *name)
{
    পূর্ণসংখ্যা i;

    জন্য (i = 0; special_চাবিগুলি[i].vim_code1 != NUL; i++)
	যদি (name[0] == special_চাবিগুলি[i].vim_code0
				       && name[1] == special_চাবিগুলি[i].vim_code1)
	    ফেরত OK;
    ফেরত FAIL;
}

    শূন্য
gui_mch_beep(শূন্য)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    শূন্য
gui_mch_invert_rectangle(
    পূর্ণসংখ্যা	    r,
    পূর্ণসংখ্যা	    c,
    পূর্ণসংখ্যা	    nr,
    পূর্ণসংখ্যা	    nc)
{
    RECT    rc;

#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.বাম = FILL_X(c);
    rc.শীর্ষ_2 = FILL_Y(r);
    rc.ডান = rc.বাম + nc * gui.char_width;
    rc.bottom = rc.শীর্ষ_2 + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    শূন্য
gui_mch_iconify(শূন্য)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    শূন্য
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.বাম = FILL_X(gui.col);
    rc.শীর্ষ_2 = FILL_Y(gui.row);
    rc.ডান = rc.বাম + gui.char_width;
    যদি (mb_lefthalve(gui.row, gui.col))
	rc.ডান += gui.char_width;
    rc.bottom = rc.শীর্ষ_2 + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    শূন্য
gui_mch_draw_part_cursor(
    পূর্ণসংখ্যা		w,
    পূর্ণসংখ্যা		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.বাম =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ডান ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.শীর্ষ_2 = FILL_Y(gui.row) + gui.char_height - h;
    rc.ডান = rc.বাম + w;
    rc.bottom = rc.শীর্ষ_2 + h;

    fill_rect(&rc, ফাঁকা, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    স্থির শূন্য
output_Dead_চাবি_re_Post_Ex(বার্তা_2 original_বার্তা, পূর্ণসংখ্যা dead_key2set)
{
    স্থির বার্তা_2 deadCharExpel;

    যদি (dead_চাবি == DEAD_চাবি_OFF)
	ফেরত;

    dead_চাবি = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_বার্তা.message;
    deadCharExpel.hwnd    = original_বার্তা.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_বার্তা.hwnd, original_বার্তা.message, original_বার্তা.wParam,
							  original_বার্তা.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    স্থির শূন্য
output_Dead_চাবি_re_Post(বার্তা_2 original_বার্তা)
{
    output_Dead_চাবি_re_Post_Ex(original_বার্তা, DEAD_চাবি_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
স্থির শূন্য process_message_usual_চাবি(UINT vk, ধ্রুবক বার্তা_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_চাবি(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    স্থির শূন্য
process_message_usual_চাবি_experimental(UINT vk, ধ্রুবক বার্তা_2 *pmsg)
{
    WCHAR	ch[8];
    পূর্ণসংখ্যা		len;
    পূর্ণসংখ্যা		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    যদি (Get_চাবি_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    যদি (Get_চাবি_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    যদি ((Get_চাবি_State(VK_RMENU) & 0x8000)
				 && (Get_চাবি_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_চাবি(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_দৈর্ঘ্য(ch),
	    0);
    যদি (len < 0)
	dead_চাবি = DEAD_চাবি_SET_DEFAULT;

    যদি (len <= 0)
    {
	পূর্ণসংখ্যা wm_char = NUL;

	যদি (dead_চাবি == DEAD_চাবি_SET_DEFAULT
		&& (Get_চাবি_State(VK_CONTROL) & 0x8000))
	{
	    যদি (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    যদি (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	যদি (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_চাবি = DEAD_চাবি_TRANSIENT_IN_ON_CHAR;
	}
	ফেরত;
    }

    // Post the message as TranslateMessage would do.
    যদি (pmsg->message == WM_KEYDOWN)
    {
	জন্য (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    অন্যথা
    {
	জন্য (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    স্থির শূন্য
process_message_usual_চাবি_classic(UINT vk, ধ্রুবক বার্তা_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    যদি (vk != 0xff
	    && (Get_চাবি_State(VK_CONTROL) & 0x8000)
	    && !(Get_চাবি_State(VK_SHIFT) & 0x8000)
	    && !(Get_চাবি_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	যদি (vk == '6' || Map_Virtual_চাবি(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_প্রবেশ_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	অন্যথা যদি (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_প্রবেশ_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	অন্যথা যদি (vk == '2' || Map_Virtual_চাবি(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_প্রবেশ_buf(string, 1);
	}
	অন্যথা
	    TranslateMessage(pmsg);
    }
    অন্যথা
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    স্থির শূন্য
process_message(শূন্য)
{
    বার্তা_2		বার্তা;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    পূর্ণসংখ্যা		i;
    পূর্ণসংখ্যা		modifiers = 0;
    পূর্ণসংখ্যা		চাবি;
#ifdef FEAT_MENU
    স্থির char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    স্থির পূর্ণসংখ্যা  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    যদি (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&বার্তা, ফাঁকা, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    যদি (বার্তা.message == WM_OLE)
    {
	char_u *str = (char_u *)বার্তা.lParam;
	যদি (str == ফাঁকা || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&বার্তা);
	}
	অন্যথা
	{
	    add_to_প্রবেশ_buf(str, (পূর্ণসংখ্যা)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ফেরত;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    যদি (s_findrep_hwnd != ফাঁকা && IsDialogMessageW(s_findrep_hwnd, &বার্তা))
    {
	HandleMouseHide(বার্তা.message, বার্তা.lParam);
	ফেরত;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    যদি (বার্তা.message == WM_KEYDOWN || বার্তা.message == WM_SYSKEYDOWN)
    {
	vk = (পূর্ণসংখ্যা) বার্তা.wParam;

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
	যদি (keycode_trans_strategy_used->is_experimental()
		&& dead_চাবি == DEAD_চাবি_SKIP_ON_CHAR)
	{
	    dead_চাবি = DEAD_চাবি_OFF;
	}

	যদি (dead_চাবি != DEAD_চাবি_OFF)
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
	    যদি (keycode_trans_strategy_used->is_experimental() &&
		    dead_চাবি == DEAD_চাবি_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_চাবি_re_Post_Ex(বার্তা,
				       /*dead_key2set=*/DEAD_চাবি_SKIP_ON_CHAR);
		ফেরত;
	    }

	    যদি (dead_চাবি != DEAD_চাবি_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ফেরত;
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
	    যদি ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_চাবি = DEAD_চাবি_OFF;
		TranslateMessage(&বার্তা);
		ফেরত;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    অন্যথা যদি ((get_real_state()
			    & (MODE_ঢোকাও | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_চাবি_re_Post(বার্তা);
		ফেরত;
	    }
	}

	// Check for CTRL-BREAK
	যদি (vk == VK_CANCEL)
	{
	    trash_প্রবেশ_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_প্রবেশ_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	যদি (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&বার্তা);
	    ফেরত;
	}

	জন্য (i = 0; special_চাবিগুলি[i].চাবি_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    যদি (special_চাবিগুলি[i].চাবি_sym == vk
		    && (vk != VK_SPACE || !(Get_চাবি_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		যদি (dead_চাবি == DEAD_চাবি_SET_DEFAULT
			&& (special_চাবিগুলি[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_চাবি_re_Post(বার্তা);
		    ফেরত;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		যদি (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ফাঁকা, ফাঁকা) == ফাঁকা)
		    বিরতি;
#endif
		modifiers = get_active_modifiers_via_ptr();

		যদি (special_চাবিগুলি[i].vim_code1 == NUL)
		    চাবি = special_চাবিগুলি[i].vim_code0;
		অন্যথা
		    চাবি = TO_SPECIAL(special_চাবিগুলি[i].vim_code0,
						   special_চাবিগুলি[i].vim_code1);
		চাবি = simplify_চাবি(চাবি, &modifiers);
		যদি (চাবি == CSI)
		    চাবি = K_CSI;

		যদি (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_প্রবেশ_buf(string, 3);
		}

		যদি (IS_SPECIAL(চাবি))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(চাবি);
		    string[2] = K_THIRD(চাবি);
		    add_to_প্রবেশ_buf(string, 3);
		}
		অন্যথা
		{
		    পূর্ণসংখ্যা	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(চাবি, string, 40, FALSE);
		    add_to_প্রবেশ_buf(string, len);
		}
		বিরতি;
	    }
	}

	// Not a special key.
	যদি (special_চাবিগুলি[i].চাবি_sym == 0)
	{
	    process_message_usual_চাবি(vk, &বার্তা);
	}
    }
#ifdef FEAT_MBYTE_IME
    অন্যথা যদি (বার্তা.message == WM_IME_NOTIFY)
	_OnImeNotify(বার্তা.hwnd, (DWORD)বার্তা.wParam, (DWORD)বার্তা.lParam);
    অন্যথা যদি (বার্তা.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&বার্তা);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    যদি (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ফাঁকা, ফাঁকা) == ফাঁকা)
#endif
	DispatchMessageW(&বার্তা);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    শূন্য
gui_mch_update(শূন্য)
{
    বার্তা_2	    বার্তা;

    যদি (!s_busy_processing)
	যখন (PeekMessageW(&বার্তা, ফাঁকা, 0, 0, PM_NOREMOVE)
						  && !vim_is_প্রবেশ_buf_full())
	    process_message();
}

    স্থির শূন্য
remove_any_timer(শূন্য)
{
    বার্তা_2		বার্তা;

    যদি (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(ফাঁকা, s_wait_timer);

	// Eat spurious WM_TIMER messages
	যখন (PeekMessageW(&বার্তা, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
স্থির garray_T	server_pending_ga = {0, 0, 0, 0, ফাঁকা};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    শূন্য
server_add_প্রবেশ(char_u *str)
{
    যদি (server_pending_ga.ga_তথ্য == ফাঁকা)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    স্থির শূন্য
server_flush_প্রবেশ(শূন্য)
{
    char_u  *str;

    যদি (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ফেরত;
    str = vim_strnsave(server_pending_ga.ga_তথ্য, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    যদি (str != ফাঁকা)
    {
	server_to_প্রবেশ_buf(str);
	vim_free(str);
    }
}
#endif

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    পূর্ণসংখ্যা
gui_mch_wait_for_chars(long wtime)
{
    পূর্ণসংখ্যা		focus;

    s_timed_out = FALSE;

    যদি (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	যদি (s_busy_processing)
	    ফেরত FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(ফাঁকা, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    যখন (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	যদি (gui.in_focus != focus)
	{
	    যদি (gui.in_focus)
		gui_mch_start_blink();
	    অন্যথা
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	যদি (s_need_activate)
	{
	    (শূন্য)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	জন্য (;;)
	{
	    বার্তা_2 বার্তা;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_প্রবেশ();
# endif
# ifdef FEAT_TIMERS
	    যদি (did_add_timer)
		বিরতি;
# endif
	    যদি (PeekMessageW(&বার্তা, ফাঁকা, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		বিরতি;
	    }
	    অন্যথা যদি (প্রবেশ_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || বার্তা_Wait_For_Multiple_Objects(0, ফাঁকা, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		বিরতি;
	}
#অন্যথা
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	যদি (প্রবেশ_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    যদি (!s_getting_focus)
		s_button_pending = -1;

	    ফেরত OK;
	}

#ifdef FEAT_TIMERS
	যদি (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    বিরতি;
	}
#endif
    }
    allow_scrollbar = FALSE;
    ফেরত FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    শূন্য
gui_mch_clear_block(
    পূর্ণসংখ্যা		row1,
    পূর্ণসংখ্যা		col1,
    পূর্ণসংখ্যা		row2,
    পূর্ণসংখ্যা		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.বাম = FILL_X(col1);
    rc.শীর্ষ_2 = FILL_Y(row1);
    rc.ডান = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    শূন্য
gui_mch_clear_all(শূন্য)
{
    RECT    rc;

    rc.বাম = 0;
    rc.শীর্ষ_2 = 0;
    rc.ডান = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    শূন্য
gui_mch_enable_menu(পূর্ণসংখ্যা flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : ফাঁকা);
#endif
}

    শূন্য
gui_mch_set_menu_pos(
    পূর্ণসংখ্যা	    x UNUSED,
    পূর্ণসংখ্যা	    y UNUSED,
    পূর্ণসংখ্যা	    w UNUSED,
    পূর্ণসংখ্যা	    h UNUSED)
{
    // It will be in the right place anyway
}

#যদি defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    শূন্য
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    পূর্ণসংখ্যা		hidden)
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
    শূন্য
gui_mch_draw_menubar(শূন্য)
{
    DrawMenuBar(s_hwnd);
}
#endif // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ফেরত (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#যদি defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    স্থির WORD
PixelToDialogX(পূর্ণসংখ্যা অঙ্ক_Pixels)
{
    ফেরত (WORD)((অঙ্ক_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    স্থির WORD
PixelToDialogY(পূর্ণসংখ্যা অঙ্ক_Pixels)
{
    ফেরত (WORD)((অঙ্ক_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    স্থির পূর্ণসংখ্যা
GetTextWidth(HDC hdc, char_u *str, পূর্ণসংখ্যা len)
{
    আকার_4    আকার_2;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &আকার_2);
    ফেরত আকার_2.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    স্থির পূর্ণসংখ্যা
GetTextWidthEnc(HDC hdc, char_u *str, পূর্ণসংখ্যা len)
{
    আকার_4	আকার_2;
    WCHAR	*wstr;
    পূর্ণসংখ্যা		n;
    পূর্ণসংখ্যা		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    যদি (wstr == ফাঁকা)
	ফেরত 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &আকার_2);
    vim_free(wstr);
    যদি (n)
	ফেরত আকার_2.cx;
    ফেরত 0;
}

স্থির শূন্য get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    স্থির BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    পূর্ণসংখ্যা		    wChild, hChild, wParent, hParent;
    পূর্ণসংখ্যা		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.ডান - rChild.বাম;
    hChild = rChild.bottom - rChild.শীর্ষ_2;

    // If Vim is minimized put the window in the middle of the screen.
    যদি (hwndParent == ফাঁকা || IsMinimized(hwndParent))
	get_work_area(&rParent);
    অন্যথা
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.ডান - rParent.বাম;
    hParent = rParent.bottom - rParent.শীর্ষ_2;

    moninfo.cb_আকার = আকার(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    যদি (mon != ফাঁকা && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    অন্যথা
    {
	hdc = GetDC(hwndChild);
	rScreen.বাম = 0;
	rScreen.শীর্ষ_2 = 0;
	rScreen.ডান = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.বাম + ((wParent - wChild) / 2);
    যদি (xNew < rScreen.বাম)
	xNew = rScreen.বাম;
    অন্যথা যদি ((xNew + wChild) > rScreen.ডান)
	xNew = rScreen.ডান - wChild;

    yNew = rParent.শীর্ষ_2 + ((hParent - hChild) / 2);
    যদি (yNew < rScreen.শীর্ষ_2)
	yNew = rScreen.শীর্ষ_2;
    অন্যথা যদি ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    ফেরত SetWindowPos(hwndChild, ফাঁকা, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#যদি defined(FEAT_TOOLBAR)
    শূন্য
gui_mch_show_toolbar(পূর্ণসংখ্যা showit)
{
    যদি (s_toolbarhwnd == ফাঁকা)
	ফেরত;

    যদি (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    অন্যথা
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_গুনতি 31

#endif

#যদি defined(FEAT_GUI_TABLINE)
    স্থির শূন্য
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, ফাঁকা);
    যদি (wn == ফাঁকা)
	ফেরত;

    infow.cb_আকার = আকার(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_তথ্য = wn;
    infow.cch = (UINT)wcslen(wn);
    ঢোকাও_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    স্থির শূন্য
show_tabline_popup_menu(শূন্য)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    যদি (hold_gui_events || cmdwin_type != 0)
	ফেরত;

    tab_pmenu = CreatePopupMenu();
    যদি (tab_pmenu == ফাঁকা)
	ফেরত;

    যদি (first_tabpage->tp_পরবর্তী != ফাঁকা)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ফাঁকা);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    যদি (rval > 0)
    {
	TCHITTESTINFO htinfo;
	পূর্ণসংখ্যা idx;

	যদি (ScreenToClient(s_tabhwnd, &pt) == 0)
	    ফেরত;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	যদি (idx == -1)
	    idx = 0;
	অন্যথা
	    idx += 1;

	send_tabline_menu_event(idx, (পূর্ণসংখ্যা)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    শূন্য
gui_mch_show_tabline(পূর্ণসংখ্যা showit)
{
    যদি (s_tabhwnd == ফাঁকা)
	ফেরত;

    যদি (!showit != !showing_tabline)
    {
	যদি (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	অন্যথা
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    পূর্ণসংখ্যা
gui_mch_showing_tabline(শূন্য)
{
    ফেরত s_tabhwnd != ফাঁকা && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    শূন্য
gui_mch_update_tabline(শূন্য)
{
    tabpage_T	*tp;
    TCITEM	tie;
    পূর্ণসংখ্যা		nr = 0;
    পূর্ণসংখ্যা		curtabidx = 0;
    পূর্ণসংখ্যা		tabadded = 0;
    WCHAR	*wstr = ফাঁকা;

    যদি (s_tabhwnd == ফাঁকা)
	ফেরত;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    জন্য (tp = first_tabpage; tp != ফাঁকা; tp = tp->tp_পরবর্তী, ++nr)
    {
	যদি (tp == curtab)
	    curtabidx = nr;

	যদি (nr >= Tab_Ctrl_Get_Item_গুনতি(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_ঢোকাও_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, ফাঁকা);
	যদি (wstr != ফাঁকা)
	{
	    TCITEMW		tiw;

	    tiw.mask = TCIF_TEXT;
	    tiw.iImage = -1;
	    tiw.pszText = wstr;
	    SendMessage(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    যখন (nr < Tab_Ctrl_Get_Item_গুনতি(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    যদি (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, ফাঁকা, ফাঁকা,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    যদি (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    শূন্য
gui_mch_set_curtab(পূর্ণসংখ্যা nr)
{
    যদি (s_tabhwnd == ফাঁকা)
	ফেরত;

    যদি (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    শূন্য
gui_mch_set_dark_theme(পূর্ণসংখ্যা dark)
{
    যদি (pDwmSetWindowAttribute != ফাঁকা && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		আকার(dark));

    যদি (pSetPreferredAppMode != ফাঁকা && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    যদি (pFlushMenuThemes != ফাঁকা && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    স্থির শূন্য
dyn_uxtheme_load(শূন্য)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    যদি (hUxThemeLib == ফাঁকা)
	ফেরত;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (শূন্য (WINAPI *)(শূন্য))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    যদি (pSetPreferredAppMode == ফাঁকা || pFlushMenuThemes == ফাঁকা)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ফাঁকা;
	ফেরত;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    শূন্য
gui_mch_set_fullscreen(পূর্ণসংখ্যা flag)
{
    স্থির RECT normal_rect;
    স্থির LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    যদি (!full_screen) // Windows not set yet.
	ফেরত;

    যদি (flag)
    {
	যদি (fullscreen_on)
	    ফেরত;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cb_আকার = আকার(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	যদি (mon == ফাঁকা || !GetMonitorInfo(mon, &moninfo))
	    ফেরত;

	// Save current window state
	GetWindowRect(s_hwnd, &normal_rect);
	normal_style = GetWindowLongPtr(s_hwnd, GWL_STYLE);
	normal_exstyle = GetWindowLongPtr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	SetWindowLongPtr(s_hwnd, GWL_STYLE,
		normal_style & ~(WS_CAPTION | WS_THICKFRAME));
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE,
		normal_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	SetWindowPos(s_hwnd, ফাঁকা,
		moninfo.rcMonitor.বাম,
		moninfo.rcMonitor.শীর্ষ_2,
		moninfo.rcMonitor.ডান - moninfo.rcMonitor.বাম,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.শীর্ষ_2,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    অন্যথা
    {
	যদি (!fullscreen_on)
	    ফেরত;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, ফাঁকা,
		normal_rect.বাম,
		normal_rect.শীর্ষ_2,
		normal_rect.ডান - normal_rect.বাম,
		normal_rect.bottom - normal_rect.শীর্ষ_2,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    শূন্য
ex_simalt(exarg_T *eap)
{
    char_u	*চাবিগুলি = eap->arg;
    পূর্ণসংখ্যা		fill_typebuf = FALSE;
    char_u	চাবি_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    যখন (*চাবিগুলি)
    {
	যদি (*চাবিগুলি == '~')
	    *চাবিগুলি = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*চাবিগুলি, (LPARAM)0);
	চাবিগুলি++;
	fill_typebuf = TRUE;
    }
    যদি (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	চাবি_name[0] = K_SPECIAL;
	চাবি_name[1] = KS_EXTRA;
	চাবি_name[2] = KE_NOP;
	চাবি_name[3] = NUL;
#যদি defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(শূন্য)ins_typebuf(চাবি_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    স্থির শূন্য
initialise_findrep(char_u *initial_string)
{
    পূর্ণসংখ্যা		wword = FALSE;
    পূর্ণসংখ্যা		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    যদি (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    যদি (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    যদি (entry_text != ফাঁকা && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, ফাঁকা);
	যদি (p != ফাঁকা)
	{
	    পূর্ণসংখ্যা len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    স্থির শূন্য
set_window_title(HWND hwnd, অক্ষর *title)
{
    যদি (title != ফাঁকা)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, ফাঁকা);
	যদি (wbuf != ফাঁকা)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    অন্যথা
	(শূন্য)SetWindowTextW(hwnd, ফাঁকা);
}

    শূন্য
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    যদি (s_findrep_বার্তা != 0)
    {
	যদি (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	যদি (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(শূন্য)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    শূন্য
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    যদি (s_findrep_বার্তা != 0)
    {
	যদি (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	যদি (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(শূন্য)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    শূন্য
gui_mch_mousehide(পূর্ণসংখ্যা hide)
{
    যদি (hide == gui.pointer_hidden)
	ফেরত;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    স্থির শূন্য
gui_mch_show_popupmenu_at(vimmenu_T *menu, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (শূন্য)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(পূর্ণসংখ্যা)0,	    //reserved param
	s_hwnd,
	ফাঁকা);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    স্থির শূন্য
_OnEndSession(শূন্য)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    স্থির শূন্য
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    স্থির শূন্য
_OnDestroy(HWND hwnd)
{
    যদি (!destroying)
	_OnClose(hwnd);
}

    স্থির শূন্য
_OnPaint(
    HWND hwnd)
{
    যদি (IsMinimized(hwnd))
	ফেরত;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (শূন্য)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    যদি (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.বাম = rect.বাম;
	ps.rcPaint.ডান = rect.ডান;
    }

    যদি (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	যদি (!screen_cleared || Screen_সারিগুলি == ফাঁকা)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	অন্যথা
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.বাম, ps.rcPaint.শীর্ষ_2,
		ps.rcPaint.ডান - ps.rcPaint.বাম + 1,
		ps.rcPaint.bottom - ps.rcPaint.শীর্ষ_2 + 1);
    }

    EndPaint(hwnd, &ps);
}

    স্থির শূন্য
On_আকার(
    HWND hwnd,
    UINT state UNUSED,
    পূর্ণসংখ্যা cx,
    পূর্ণসংখ্যা cy)
{
    যদি (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    স্থির শূন্য
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (শূন্য)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    স্থির শূন্য
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    যদি (destroying)
	ফেরত;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (শূন্য)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    স্থির LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    ফেরত DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    শূন্য
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    শূন্য
gui_mch_getmouse(পূর্ণসংখ্যা *x, পূর্ণসংখ্যা *y)
{
    RECT rct;
    POINT mp;

    (শূন্য)GetWindowRect(s_textArea, &rct);
    (শূন্য)GetCursorPos(&mp);
    *x = (পূর্ণসংখ্যা)(mp.x - rct.বাম);
    *y = (পূর্ণসংখ্যা)(mp.y - rct.শীর্ষ_2);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    শূন্য
gui_mch_setmouse(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    RECT rct;

    (শূন্য)GetWindowRect(s_textArea, &rct);
    (শূন্য)SetCursorPos(x + gui.border_offset + rct.বাম,
		       y + gui.border_offset + rct.শীর্ষ_2);
}

    স্থির শূন্য
gui_mswin_get_valid_dimensions(
    পূর্ণসংখ্যা w,
    পূর্ণসংখ্যা h,
    পূর্ণসংখ্যা *valid_w,
    পূর্ণসংখ্যা *valid_h,
    পূর্ণসংখ্যা *cols,
    পূর্ণসংখ্যা *rows)
{
    পূর্ণসংখ্যা	    base_width, base_height;

    base_width = gui_get_base_width()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    base_height = gui_get_base_height()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_height(FALSE);
    *cols = (w - base_width) / gui.char_width;
    *rows = (h - base_height) / gui.char_height;
    *valid_w = base_width + *cols * gui.char_width;
    *valid_h = base_height + *rows * gui.char_height;
}

    শূন্য
gui_mch_flash(পূর্ণসংখ্যা msec)
{
    RECT    rc;

#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.বাম = 0;
    rc.শীর্ষ_2 = 0;
    rc.ডান = gui.অঙ্ক_cols * gui.char_width;
    rc.bottom = gui.অঙ্ক_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    স্থির BOOL
is_point_onscreen(পূর্ণসংখ্যা x, পূর্ণসংখ্যা y)
{
    POINT   pt = {x, y};

    ফেরত MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != ফাঁকা;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    স্থির BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.বাম;
    p1.y = rc.শীর্ষ_2;
    p2.x = rc.ডান - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    যদি (!is_point_onscreen(p1.x, p1.y))
	ফেরত FALSE;
    যদি (!is_point_onscreen(p1.x, p2.y))
	ফেরত FALSE;
    যদি (!is_point_onscreen(p2.x, p1.y))
	ফেরত FALSE;
    যদি (!is_point_onscreen(p2.x, p2.y))
	ফেরত FALSE;
    ফেরত TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    স্থির পূর্ণসংখ্যা
get_scroll_flags(শূন্য)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    যদি (!is_window_onscreen(s_hwnd))
	ফেরত SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    জন্য (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	যদি (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    যদি (IntersectRect(&rcDest, &rcVim, &rcOther))
		ফেরত SW_INVALIDATE;
	}
    ফেরত 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    স্থির শূন্য
intel_gpu_workaround(শূন্য)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    শূন্য
gui_mch_delete_সারিগুলি(
    পূর্ণসংখ্যা	    row,
    পূর্ণসংখ্যা	    অঙ্ক_সারিগুলি)
{
    RECT	rc;

    rc.বাম = FILL_X(gui.scroll_region_বাম);
    rc.ডান = FILL_X(gui.scroll_region_ডান + 1);
    rc.শীর্ষ_2 = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -অঙ্ক_সারিগুলি * gui.char_height, &rc);
    }
    অন্যথা
#endif
    {
#যদি defined(FEAT_DIRECTX)
	যদি (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -অঙ্ক_সারিগুলি * gui.char_height,
				    &rc, &rc, ফাঁকা, ফাঁকা, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - অঙ্ক_সারিগুলি + 1,
						       gui.scroll_region_বাম,
	gui.scroll_region_bot, gui.scroll_region_ডান);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    শূন্য
gui_mch_ঢোকাও_সারিগুলি(
    পূর্ণসংখ্যা		row,
    পূর্ণসংখ্যা		অঙ্ক_সারিগুলি)
{
    RECT	rc;

    rc.বাম = FILL_X(gui.scroll_region_বাম);
    rc.ডান = FILL_X(gui.scroll_region_ডান + 1);
    rc.শীর্ষ_2 = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, অঙ্ক_সারিগুলি * gui.char_height, &rc);
    }
    অন্যথা
#endif
    {
#যদি defined(FEAT_DIRECTX)
	যদি (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, অঙ্ক_সারিগুলি * gui.char_height,
				    &rc, &rc, ফাঁকা, ফাঁকা, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_বাম,
				row + অঙ্ক_সারিগুলি - 1, gui.scroll_region_ডান);
}


    শূন্য
gui_mch_exit(পূর্ণসংখ্যা rc UNUSED)
{
#যদি defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ফাঁকা;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (শূন্য)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    যদি (s_hwnd != ফাঁকা)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    স্থির char_u *
logfont2name(LOGFONTW lf)
{
    অক্ষর	*charset_name;
    অক্ষর	*quality_name;
    অক্ষর	*font_name;
    size_t	res_আকার;
    অক্ষর	*res;

    font_name = (অক্ষর *)utf16_to_enc(lf.lfFaceName, ফাঁকা);
    যদি (font_name == ফাঁকা)
	ফেরত ফাঁকা;
    charset_name = charset_id2name((পূর্ণসংখ্যা)lf.lfCharSet);
    quality_name = quality_id2name((পূর্ণসংখ্যা)lf.lfQuality);

    res_আকার = STRLEN(font_name) + 30
		    + (charset_name == ফাঁকা ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ফাঁকা ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_আকার);
    যদি (res != ফাঁকা)
    {
	অক্ষর	*p;
	পূর্ণসংখ্যা	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	জন্য (p = font_name; *p != NUL; ++p)
	{
	    যদি (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	যদি (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(অক্ষর *)res, res_আকার, "%s:h%d", font_name, points);
	অন্যথা
	    res_len = vim_snprintf_safelen(
		(অক্ষর *)res, res_আকার, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (অক্ষর *)res + res_len,
	    res_আকার - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	যদি (charset_name != ফাঁকা)
	    res_len += vim_snprintf_safelen((অক্ষর *)res + res_len,
		res_আকার - res_len, ":c%s", charset_name);
	যদি (quality_name != ফাঁকা)
	    vim_snprintf((অক্ষর *)res + res_len,
		res_আকার - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ফেরত (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    স্থির শূন্য
update_im_font(শূন্য)
{
    LOGFONTW	lf_wide, lf;

    যদি (p_guifontwide != ফাঁকা && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, আকার(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    অন্যথা
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    যদি (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    শূন্য
gui_mch_wide_font_changed(শূন্য)
{
    LOGFONTW lf;

#ifdef FEAT_MBYTE_IME
    update_im_font();
#endif

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    যদি (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, আকার(lf), &lf))
    {
	যদি (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	যদি (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    যদি (!lf.lfItalic)
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
    পূর্ণসংখ্যা
gui_mch_init_font(char_u *font_name, পূর্ণসংখ্যা fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    যদি (get_logfont(&lf, font_name, ফাঁকা, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    যদি (font == NOFONT)
	ফেরত FAIL;

#যদি defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	পূর্ণসংখ্যা		    feat_গুনতি = 0;
	char_u		    *fp;

	যদি (font_name != ফাঁকা)
	{
	    // Find each ":f" option in font_name.
	    জন্য (fp = font_name; *fp != NUL; fp++)
	    {
		যদি (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    পূর্ণসংখ্যা    ti = 0;
		    unsigned পূর্ণসংখ্যা param = 1;

		    fp += 2;  // skip ":f"
		    যখন (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    যদি (ti != 4)
			চালু;  // invalid tag length

		    যদি (*fp == '=')
		    {
			fp++;
			param = (unsigned পূর্ণসংখ্যা)atoi((অক্ষর *)fp);
			যখন (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    যদি (feat_গুনতি < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_গুনতি].tag =
			    ((unsigned পূর্ণসংখ্যা)tag[0])
			    | ((unsigned পূর্ণসংখ্যা)tag[1] << 8)
			    | ((unsigned পূর্ণসংখ্যা)tag[2] << 16)
			    | ((unsigned পূর্ণসংখ্যা)tag[3] << 24);
			features[feat_গুনতি].parameter = param;
			feat_গুনতি++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_গুনতি);
    }
#endif

    যদি (font_name == ফাঁকা)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    যদি (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    Update_Font_আকার(font);

    p = logfont2name(lfOrig);
    যদি (p != ফাঁকা)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	যদি (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	অন্যথা
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    যদি (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    যদি (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	যদি (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ফেরত OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    পূর্ণসংখ্যা
gui_mch_maximized(শূন্য)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.দৈর্ঘ্য = আকার(WINDOWPLACEMENT);
    যদি (GetWindowPlacement(s_hwnd, &wp))
    {
	যদি (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ফেরত TRUE;
	যদি (wp.showCmd == SW_SHOWMINIMIZED)
	    ফেরত FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	যদি ((rc.ডান - rc.বাম !=
		    wp.rcNormalPosition.ডান - wp.rcNormalPosition.বাম)
		|| (rc.bottom - rc.শীর্ষ_2 !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.শীর্ষ_2))
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    শূন্য
gui_mch_newfont(শূন্য)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    যদি (win_socket_id == 0)
    {
	gui_resize_shell(rect.ডান - rect.বাম
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.শীর্ষ_2
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    অন্যথা
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ডান - rect.বাম,
	    rect.bottom - rect.শীর্ষ_2 - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    শূন্য
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == ফাঁকা ? "VIM" : (অক্ষর *)title));
}

#যদি defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
স্থির LPCSTR mshape_idcs[] =
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

    শূন্য
mch_set_mouse_shape(পূর্ণসংখ্যা shape)
{
    LPCSTR idc;

    যদি (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    অন্যথা
    {
	যদি (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	অন্যথা
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(ফাঁকা, idc));
	যদি (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (শূন্য)GetCursorPos(&mp);
	    (শূন্য)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#যদি defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    স্থির WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    পূর্ণসংখ্যা len;
    WCHAR *res;

    tmp = convert_filter(s);
    যদি (tmp == ফাঁকা)
	ফেরত ফাঁকা;
    len = (পূর্ণসংখ্যা)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    ফেরত res;
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
	পূর্ণসংখ্যা saving,
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
    পূর্ণসংখ্যা			i;
    WCHAR		*titlep = ফাঁকা;
    WCHAR		*extp = ফাঁকা;
    WCHAR		*initdirp = ফাঁকা;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    যদি (dflt == ফাঁকা)
	fileBuf[0] = NUL;
    অন্যথা
    {
	wp = enc_to_utf16(dflt, ফাঁকা);
	যদি (wp == ফাঁকা)
	    fileBuf[0] = NUL;
	অন্যথা
	{
	    জন্য (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_FIELD(fileStruct);
# ifdef OPENFILENAME_আকার_VERSION_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_আকার = OPENFILENAME_আকার_VERSION_400_W;
# অন্যথা
    fileStruct.l_Struct_আকার = আকার(fileStruct);
# endif

    যদি (title != ফাঁকা)
	titlep = enc_to_utf16(title, ফাঁকা);
    fileStruct.lpstrTitle = titlep;

    যদি (ext != ফাঁকা)
	extp = enc_to_utf16(ext, ফাঁকা);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    যদি (initdir != ফাঁকা && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, ফাঁকা);
	যদি (initdirp != ফাঁকা)
	{
	    জন্য (wp = initdirp; *wp != NUL; ++wp)
		যদি (*wp == '/')
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
    যদি (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    যদি (saving)
	ret = GetSaveFileNameW(&fileStruct);
    অন্যথা
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    যদি (!ret)
	ফেরত ফাঁকা;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, ফাঁকা);
    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ফেরত q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    স্থির char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    যদি (res != ফাঁকা)
    {
	জন্য (i = 0; i < s_len; ++i)
	    যদি (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    অন্যথা
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ফেরত res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ফেরত gui_mch_browse(0, title, (char_u *)_("Not Used"), ফাঁকা,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    স্থির শূন্য
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    অক্ষর    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ফাঁকা, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    যদি (fnames != ফাঁকা)
	জন্য (i = 0; i < cFiles; ++i)
	{
	    যদি (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, ফাঁকা);
	    অন্যথা
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    যদি (fnames == ফাঁকা)
	ফেরত;

    পূর্ণসংখ্যা kbd_modifiers = get_active_modifiers();

    যদি ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    যদি ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    যদি ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    স্থির পূর্ণসংখ্যা
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    পূর্ণসংখ্যা pos)
{
    স্থির UINT	পূর্ব_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    পূর্ণসংখ্যা		dragging = FALSE;
    পূর্ণসংখ্যা		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_আকার = আকার(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    যদি (sb == ফাঁকা)
	ফেরত 0;

    যদি (sb->wp != ফাঁকা)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    অন্যথা	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->মান;

    নির্বাচন (code)
    {
	ক্ষেত্র SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    যদি (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    বিরতি;
	ক্ষেত্র SB_LINEDOWN:
	    val++;
	    বিরতি;
	ক্ষেত্র SB_LINEUP:
	    val--;
	    বিরতি;
	ক্ষেত্র SB_PAGEDOWN:
	    val += (sb_info->আকার_2 > 2 ? sb_info->আকার_2 - 2 : 1);
	    বিরতি;
	ক্ষেত্র SB_PAGEUP:
	    val -= (sb_info->আকার_2 > 2 ? sb_info->আকার_2 - 2 : 1);
	    বিরতি;
	ক্ষেত্র SB_শীর্ষ:
	    val = 0;
	    বিরতি;
	ক্ষেত্র SB_BOTTOM:
	    val = sb_info->max;
	    বিরতি;
	ক্ষেত্র SB_ENDSCROLL:
	    যদি (পূর্ব_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		যদি (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    বিরতি;

	পূর্বনির্ধারিত:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ফেরত 0;
    }
    পূর্ব_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    যদি (sb->wp != ফাঁকা)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_বাম) ? SBAR_ডান : SBAR_বাম].id;

	SetScrollInfo(id, SB_CTL, &si, TRUE);
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

    ফেরত 0;
}


#ifdef FEAT_XPM_W32
# include "xpm_w32.h"
#endif


// Some parameters for tearoff menus.  All in pixels.
#define TEAROFF_PADDING_X	2
#define TEAROFF_BUTTON_PAD_X	8
#define TEAROFF_MIN_WIDTH	200
#define TEAROFF_SUBMENU_LABEL	">>"
#define TEAROFF_COLUMN_PADDING	3	// # spaces to pad column with.


#ifdef FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_TEXT_LEN	    MAXPATHL

স্থির BalloonEval  *cur_beval = ফাঁকা;
স্থির UINT_PTR	    beval_timer_id = 0;
স্থির DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
স্থির UINT	s_menu_id = 100;
#endif

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#define USE_SYSMENU_FONT

#define VIM_NAME	"vim"
#define VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#define DLG_ALLOC_আকার 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
স্থির PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	ধ্রুবক অক্ষর *caption);
স্থির LPWORD lpwAlign(LPWORD);
স্থির পূর্ণসংখ্যা nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#যদি defined(FEAT_MENU) && defined(FEAT_TEAROFF)
স্থির শূন্য gui_mch_tearoff(char_u *title, vimmenu_T *menu, পূর্ণসংখ্যা initX, পূর্ণসংখ্যা initY);
#endif
স্থির শূন্য get_dialog_font_metrics(শূন্য);

স্থির পূর্ণসংখ্যা dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
স্থির শূন্য initialise_toolbar(শূন্য);
স্থির শূন্য update_toolbar_আকার(শূন্য);
স্থির LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_বার্তা, WPARAM wParam, LPARAM lParam);
স্থির পূর্ণসংখ্যা get_toolbar_bitmap(vimmenu_T *menu);
#অন্যথা
# define update_toolbar_আকার()
#endif

#ifdef FEAT_GUI_TABLINE
স্থির শূন্য initialise_tabline(শূন্য);
স্থির LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_বার্তা, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
স্থির LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
স্থির char_u *Get_ফলাফল_Str(HWND hwnd, পূর্ণসংখ্যা GCS, পূর্ণসংখ্যা *lenp);
#endif
#যদি defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
প্রকারলেখ কাঠামো tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
প্রকারলেখ HANDLE HIMC;
# endif

স্থির HINSTANCE hLibImm = ফাঁকা;
স্থির LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
স্থির HIMC (WINAPI *pImmGetContext)(HWND);
স্থির HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
স্থির BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
স্থির BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
স্থির BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
স্থির BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
স্থির BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
স্থির BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
স্থির BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
স্থির BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
স্থির শূন্য dyn_imm_load(শূন্য);
#অন্যথা
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define pImmGetOpenStatus	  ImmGetOpenStatus
# define pImmSetOpenStatus	  ImmSetOpenStatus
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define pImmSetCompositionFontW  ImmSetCompositionFontW
# define pImmSetCompositionWindow ImmSetCompositionWindow
# define pImmGetConversionStatus  ImmGetConversionStatus
# define pImmSetConversionStatus  ImmSetConversionStatus
#endif

#ifdef FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    স্থির পূর্ণসংখ্যা
gui_mswin_get_menu_height(
    পূর্ণসংখ্যা	    fix_window)	    // If TRUE, resize window if menu height changed
{
    স্থির পূর্ণসংখ্যা	old_menu_height = -1;

    RECT    rc1, rc2;
    পূর্ণসংখ্যা	    অঙ্ক;
    পূর্ণসংখ্যা	    menu_height;

    যদি (gui.menu_is_active)
	অঙ্ক = Get_Menu_Item_গুনতি(s_menuBar);
    অন্যথা
	অঙ্ক = 0;

    যদি (অঙ্ক == 0)
	menu_height = 0;
    অন্যথা যদি (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    অন্যথা
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	যদি (gui.starting)
	    menu_height = rc1.bottom - rc1.শীর্ষ_2 + 1;
	অন্যথা
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, অঙ্ক - 1, &rc2);
	    menu_height = rc2.bottom - rc1.শীর্ষ_2 + 1;
	}
    }

    যদি (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    ফেরত menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    স্থির long
mouse_vertical_scroll_step(শূন্য)
{
    UINT val;
    যদি (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ফেরত (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    ফেরত 3; // Safe default;
}

    স্থির long
mouse_horizontal_scroll_step(শূন্য)
{
    UINT val;
    যদি (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ফেরত (long)val;
    ফেরত 3; // Safe default;
}

    স্থির শূন্য
init_mouse_wheel(শূন্য)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    স্থির শূন্য
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, পূর্ণসংখ্যা horizontal)
{
    পূর্ণসংখ্যা		button;
    win_T	*wp;
    পূর্ণসংখ্যা		modifiers = 0;
    পূর্ণসংখ্যা		kbd_modifiers;
    পূর্ণসংখ্যা		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    যদি (wp != ফাঁকা && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	যদি (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_বাম : MSCR_ডান;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	অন্যথা
	{
	    cap.arg = zDelta < 0 ? MSCR_UP : MSCR_DOWN;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	out_flush();
    }
#endif

    যদি (wp == ফাঁকা || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    যদি (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    অন্যথা
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    যদি ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    যদি ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    যদি ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    ScreenToClient(s_textArea, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#ifdef USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_আকার = আকার(NONCLIENTMETRICSW);
    যদি (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    আকার(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ফেরত FAIL;
    *lf = nm.lfMenuFont;
    ফেরত OK;
}
#endif


#যদি defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    স্থির শূন্য
set_tabline_font(শূন্য)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    যদি (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ফেরত;

    lfSysmenu.lfHeight = adjust_fontsize_by_dpi(lfSysmenu.lfHeight);
    font = CreateFontIndirectW(&lfSysmenu);

    SendMessage(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
    hfntOld = SelectFont(hdc, font);

    GetTextMetrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_height = tm.tmHeight + tm.tmInternalLeading + 7;
}
#অন্যথা
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    স্থির LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    নির্বাচন (param)
    {
	ক্ষেত্র SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    বিরতি;
	ক্ষেত্র SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    বিরতি;
	ক্ষেত্র SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    বিরতি;
	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত 0;
}

#ifdef FEAT_NETBEANS_INTG
    স্থির শূন্য
_OnWindowPosChanged(
    HWND hwnd,
    ধ্রুবক LPWINDOWPOS lpwpos)
{
    স্থির পূর্ণসংখ্যা x = 0, y = 0, cx = 0, cy = 0;
    বাহ্যিক পূর্ণসংখ্যা WSInitialized;

    যদি (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_frame_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, DefWindowProcW);
}
#endif


স্থির HWND hwndTip = ফাঁকা;

    স্থির শূন্য
show_sizing_tip(পূর্ণসংখ্যা cols, পূর্ণসংখ্যা rows)
{
    TOOLINFOA ti;
    অক্ষর buf[32];

    ti.cb_আকার = আকার(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    যদি (hwndTip == ফাঁকা)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, ফাঁকা,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ফাঁকা, GetModuleHandle(ফাঁকা), ফাঁকা);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    অন্যথা
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    স্থির শূন্য
destroy_sizing_tip(শূন্য)
{
    যদি (hwndTip == ফাঁকা)
	ফেরত;

    DestroyWindow(hwndTip);
    hwndTip = ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    পূর্ণসংখ্যা	    w, h;
    পূর্ণসংখ্যা	    valid_w, valid_h;
    পূর্ণসংখ্যা	    w_offset, h_offset;
    পূর্ণসংখ্যা	    cols, rows;

    w = lprc->ডান - lprc->বাম;
    h = lprc->bottom - lprc->শীর্ষ_2;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    যদি (fwSide == WMSZ_বাম || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->বাম += w_offset;
    অন্যথা যদি (fwSide == WMSZ_ডান || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ডান -= w_offset;

    যদি (fwSide == WMSZ_শীর্ষ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->শীর্ষ_2 += h_offset;
    অন্যথা যদি (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    ফেরত TRUE;
}

#ifdef FEAT_GUI_TABLINE
    স্থির শূন্য
_OnRButtonUp(HWND hwnd, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, UINT চাবি_Flags)
{
    যদি (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	যদি (pt.y < rect.শীর্ষ_2)
	{
	    show_tabline_popup_menu();
	    ফেরত;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, চাবি_Flags, DefWindowProcW);
}

    স্থির শূন্য
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, পূর্ণসংখ্যা x, পূর্ণসংখ্যা y, UINT চাবি_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    যদি (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	যদি (pt.y < rect.শীর্ষ_2)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, চাবি_Flags, DefWindowProcW);
}
#endif

    স্থির UINT
_OnNCHitTest(HWND hwnd, পূর্ণসংখ্যা xPos, পূর্ণসংখ্যা yPos)
{
    UINT	ফলাফল;
    পূর্ণসংখ্যা		x, y;

    ফলাফল = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    যদি (ফলাফল != HTCLIENT)
	ফেরত ফলাফল;

#ifdef FEAT_GUI_TABLINE
    যদি (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	যদি (yPos < rct.শীর্ষ_2)
	    ফেরত ফলাফল;
    }
#endif
    (শূন্য)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    যদি (xPos < 48) // <VN> TODO should use system metric?
	ফেরত HTBOTTOMLEFT;
    অন্যথা
	ফেরত HTBOTTOMRIGHT;
}

#যদি defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    স্থির LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    নির্বাচন (hdr->code)
    {
	ক্ষেত্র TTN_GETDISPINFOW:
	ক্ষেত্র TTN_GETDISPINFO:
	    {
		char_u		*str = ফাঁকা;
		স্থির শূন্য	*tt_text = ফাঁকা;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		যদি (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    POINT	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    যদি (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			পূর্ণসংখ্যা idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			যদি (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    যদি (tp != ফাঁকা)
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
		অন্যথা
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(মূল_menu, idButton);
		    যদি (pMenu)
			str = pMenu->strings[MENU_সূচক_TIP];
		}
# endif
		যদি (str == ফাঁকা)
		    বিরতি;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		যদি (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, ফাঁকা);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		অন্যথা
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    যদি (len < আকার(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == ফাঁকা))
			vim_strncpy((char_u *)lpdi->szText, str,
				আকার(lpdi->szText) - 1);
		    অন্যথা
			lpdi->lpszText = tt_text;
		}
	    }
	    বিরতি;

# ifdef FEAT_GUI_TABLINE
	ক্ষেত্র TCN_SELCHANGE:
	    যদি (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ফেরত 0L;
	    }
	    বিরতি;

	ক্ষেত্র NM_RCLICK:
	    যদি (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ফেরত 0L;
	    }
	    বিরতি;
# endif

	পূর্বনির্ধারিত:
	    বিরতি;
    }
    ফেরত DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#যদি defined(MENUHINTS) && defined(FEAT_MENU)
    স্থির LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    যদি (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	স্থির পূর্ণসংখ্যা  did_menu_tip = FALSE;

	যদি (did_menu_tip)
	{
	    বার্তা_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(মূল_menu, (UINT)LOWORD(wParam));
	যদি (pMenu != ফাঁকা && pMenu->strings[MENU_সূচক_TIP] != ফাঁকা)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_আকার = আকার(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    যদি (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++বার্তা_hist_off;
		বার্তা((অক্ষর *)pMenu->strings[MENU_সূচক_TIP]);
		--বার্তা_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	ফেরত 0L;
    }
    ফেরত DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    স্থির BOOL
On_Get_Dpi_Scaled_আকার(HWND hwnd UNUSED, UINT dpi, আকার_4 *আকার_2)
{
    পূর্ণসংখ্যা		old_width, old_height;
    পূর্ণসংখ্যা		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_আকার(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, আকার(lf), &lf);
    lf.lfHeight = lf.lfHeight * (পূর্ণসংখ্যা)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    যদি (font)
    {
	Get_Font_আকার((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    অন্যথা
    {
	new_width = old_width;
	new_height = old_height;
    }
    আকার_2->cx = আকার_2->cx * new_width / old_width;
    আকার_2->cy = আকার_2->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ফেরত TRUE;
}

    স্থির LRESULT
_OnDpiChanged(HWND hwnd, UINT xdpi UNUSED, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_in_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_আকার();
    update_toolbar_আকার();
    set_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_height(FALSE);
#ifdef FEAT_MBYTE_IME
    im_set_position(gui.row, gui.col);
#endif
    InvalidateRect(hwnd, ফাঁকা, TRUE);

    s_in_dpichanged = FALSE;
    ফেরত 0L;
}


    স্থির LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_বার্তা,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_বার্তা, lParam);

    s_u_বার্তা = u_বার্তা;
    s_wParam = wParam;

    নির্বাচন (u_বার্তা)
    {
	HANDLE_বার্তা(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_বার্তা(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_বার্তা(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_বার্তা(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_বার্তা(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_বার্তা(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_বার্তা(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_বার্তা(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_বার্তা(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_বার্তা(hwnd, WM_আকার,	On_আকার);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_বার্তা(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_বার্তা(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_বার্তা(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_বার্তা(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_বার্তা(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_বার্তা(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    ক্ষেত্র WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ফেরত FALSE;		// Do NOT allow system to go down.

    ক্ষেত্র WM_ENDSESSION:
	যদি (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    ফেরত 0L;
	}
	বিরতি;

    ক্ষেত্র WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (পূর্ণসংখ্যা)(short)LOWORD(lParam));
	ফেরত 0L;

    ক্ষেত্র WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	যদি (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((পূর্ণসংখ্যা)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (পূর্ণসংখ্যা)(short)LOWORD(lParam));
	    ফেরত 0L;
	}
#ifdef FEAT_MENU
	অন্যথা
	    ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
#endif

    ক্ষেত্র WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
#অন্যথা
	ফেরত 0L;
#endif

    ক্ষেত্র WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	বিরতি;

    ক্ষেত্র WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ফেরত _DuringSizing((UINT)wParam, (LPRECT)lParam);

    ক্ষেত্র WM_MOUSEWHEEL:
    ক্ষেত্র WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_বার্তা == WM_MOUSEHWHEEL);
	ফেরত 0L;

	// Notification for change in SystemParametersInfo()
    ক্ষেত্র WM_SETTINGCHANGE:
	ফেরত _OnSettingChange((UINT)wParam);

#যদি defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ক্ষেত্র WM_NOTIFY:
	ফেরত _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#যদি defined(MENUHINTS) && defined(FEAT_MENU)
    ক্ষেত্র WM_MENUSELECT:
	ফেরত _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    ক্ষেত্র WM_IME_NOTIFY:
	যদি (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
	ফেরত 1L;

    ক্ষেত্র WM_IME_COMPOSITION:
	যদি (!_OnImeComposition(hwnd, wParam, lParam))
	    ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
	ফেরত 1L;
#endif
    ক্ষেত্র WM_GETDPISCALEDSIZE:
	ফেরত On_Get_Dpi_Scaled_আকার(hwnd, (UINT)wParam, (আকার_4 *)lParam);
    ক্ষেত্র WM_DPICHANGED:
	ফেরত _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    পূর্বনির্ধারিত:
#ifdef MSWIN_FIND_REPLACE
	যদি (u_বার্তা == s_findrep_বার্তা && s_findrep_বার্তা != 0)
	    _OnFindRepl();
#endif
	বিরতি;
    }

    ফেরত DefWindowProcW(hwnd, u_বার্তা, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = ফাঁকা;

    স্থির BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    অক্ষর	buf[2048];
    অক্ষর	*title = (অক্ষর *)lParam;

    যদি (GetWindowText(hwnd, buf, আকার(buf)))
    {
	যদি (strstr(buf, title) != ফাঁকা)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, ফাঁকা, "MDIClient", ফাঁকা);
	    যদি (vim_parent_hwnd != ফাঁকা)
		ফেরত FALSE;
	}
    }
    ফেরত TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    শূন্য
gui_mch_set_parent(অক্ষর *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    যদি (vim_parent_hwnd == ফাঁকা)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    স্থির শূন্য
ole_error(অক্ষর *arg)
{
    অক্ষর buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#যদি defined(GUI_MAY_SPAWN)
    স্থির অক্ষর *
gvim_error(শূন্য)
{
    অক্ষর *বার্তা = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    যদি (starting)
    {
	mch_errmsg(বার্তা);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ফেরত বার্তা;
}

    অক্ষর *
gui_mch_do_spawn(char_u *arg)
{
    পূর্ণসংখ্যা			len;
# যদি defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ফাঁকা;
    LPWSTR		tofree1 = ফাঁকা;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = ফাঁকা, p, warg, tofree2 = ফাঁকা;
    STARTUPINFOW	si = {আকার(si)};
    PROCESS_INFORMATION pi;

    যদি (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	যাও error;
    p = wcsrchr(name, L'\\');
    যদি (p == ফাঁকা)
	যাও error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# অন্যথা
    wcscpy(p + 1, L"gvim.exe");
# endif

# যদি defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    যদি (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_সারি_W();
	// Skip 1st argument.
	যখন (*p && *p != L' ' && *p != L'\t')
	{
	    যদি (*p == L'"')
	    {
		// Skip quoted strings
		যখন (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# যদি defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    অন্যথা
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	পূর্ণসংখ্যা	ret;

	session = vim_tempname('s', FALSE);
	যদি (session == ফাঁকা)
	    যাও error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	যদি (p_bg == ফাঁকা)
	{
	    p_bg = savebg;
	    যাও error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	যদি (!ret)
	    যাও error;
	wsession = enc_to_utf16(session, ফাঁকা);
	যদি (wsession == ফাঁকা)
	    যাও error;
	len = (পূর্ণসংখ্যা)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	যদি (cmd == ফাঁকা)
	{
	    vim_free(wsession);
	    যাও error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    যদি (arg != ফাঁকা)
    {
	warg = enc_to_utf16(arg, ফাঁকা);
	যদি (warg == ফাঁকা)
	    যাও error;
	tofree2 = warg;
    }
    অন্যথা
	warg = L"";

    // Set up the new command line.
    len = (পূর্ণসংখ্যা)wcslen(name) + (পূর্ণসংখ্যা)wcslen(cmd) + (পূর্ণসংখ্যা)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    যদি (newcmd == ফাঁকা)
	যাও error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    যদি (!CreateProcessW(ফাঁকা, newcmd, ফাঁকা, ফাঁকা, TRUE, 0,
		ফাঁকা, ফাঁকা, &si, &pi))
	যাও error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# যদি defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    যদি (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ফেরত gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    শূন্য
gui_mch_prepare(পূর্ণসংখ্যা *argc, অক্ষর **argv)
{
    পূর্ণসংখ্যা		silent = FALSE;
    পূর্ণসংখ্যা		idx;

    // Check for special OLE command line parameters
    যদি ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	যদি (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	অন্যথা
	    idx = 1;

	// Register Vim as an OLE Automation server
	যদি (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#অন্যথা
	    যদি (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	যদি (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#অন্যথা
	    যদি (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	যদি (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#অন্যথা
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    যদি (mch_is_gui_executable())
# endif
    {
	পূর্ণসংখ্যা	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	যদি (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	পূর্ণসংখ্যা arg;

	জন্য (arg = 1; arg < *argc; arg++)
	    যদি (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * আকার(অক্ষর *));
		argv[*argc] = ফাঁকা;
		বিরতি;	// enough?
	    }
    }
#endif
}

    স্থির শূন্য
load_dpi_func(শূন্য)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    যদি (hUser32 == ফাঁকা)
	যাও fail;

    pGetDpiForSystem = (UINT (WINAPI *)(শূন্য))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (পূর্ণসংখ্যা (WINAPI *)(পূর্ণসংখ্যা, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    যদি (pSetThreadDpiAwarenessContext != ফাঁকা)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	যদি (oldctx != ফাঁকা)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    যদি (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    ফেরত;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = ফাঁকা;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = ফাঁকা;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ফাঁকা;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    পূর্ণসংখ্যা
gui_mch_init(শূন্য)
{
    ধ্রুবক WCHAR szVimWndClassW[] = VIM_CLASSW;
    ধ্রুবক WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    যদি (s_hwnd != ফাঁকা)
	যাও theend;

    /*
     * Load the tearoff bitmap
     */
#ifdef FEAT_TEAROFF
    s_htearbitmap = LoadBitmap(g_hinst, "IDB_TEAROFF");
#endif

    load_dpi_func();

#ifdef FEAT_GUI_DARKTHEME
    dyn_uxtheme_load();
#endif

    dyn_dwm_load();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_আকার();

#ifdef FEAT_MENU
    gui.menu_height = 0;	// Windows takes care of this
#endif
    gui.border_width = 0;
#ifdef FEAT_TOOLBAR
    gui.toolbar_height = TOOLBAR_BUTTON_HEIGHT + TOOLBAR_BORDER_HEIGHT;
#endif

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    যদি (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(ফাঁকা, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ফাঁকা;
	wndclassw.lpszClassName = szVimWndClassW;

	যদি (RegisterClassW(&wndclassw) == 0)
	    ফেরত FAIL;
    }

    যদি (vim_parent_hwnd != ফাঁকা)
    {
#ifdef HAVE_TRY_EXCEPT
	__try
	{
#endif
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = CreateWindowExW(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_CHILD
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_parent_hwnd, ফাঁকা,
		g_hinst, ফাঁকা);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	যদি (s_hwnd == ফাঁকা)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    অন্যথা
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	যদি (IsWindow((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = CreateWindowW(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		ফাঁকা, ফাঁকা,
		g_hinst, ফাঁকা);
	যদি (s_hwnd != ফাঁকা && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    যদি (s_hwnd == ফাঁকা)
	ফেরত FAIL;

    যদি (pGetDpiForWindow != ফাঁকা)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_আকার();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#যদি defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    যদি (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ফাঁকা;
	wndclassw.hCursor = LoadCursor(ফাঁকা, IDC_ARROW);
	wndclassw.hbrBackground = ফাঁকা;
	wndclassw.lpszMenuName = ফাঁকা;
	wndclassw.lpszClassName = szTextAreaClassW;

	যদি (RegisterClassW(&wndclassw) == 0)
	    ফেরত FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ফাঁকা,
	g_hinst, ফাঁকা);

    যদি (s_textArea == ফাঁকা)
	ফেরত FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ফাঁকা;

	যদি (mch_icon_load(&hIcon) == OK && hIcon != ফাঁকা)
	    SendMessage(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#endif

#ifdef FEAT_MENU
    s_menuBar = CreateMenu();
#endif
    s_hdc = GetDC(s_textArea);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_offset = gui.border_width;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#ifdef FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#endif
#ifdef FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#endif
#ifdef MSWIN_FIND_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_বার্তা = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_আকার = আকার(s_findrep_struct);
    s_findrep_struct.lpstrFindWhat = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrFindWhat[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.wFindWhatLen = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#endif

#ifdef FEAT_EVAL
    // set the v:windowid variable
    set_vim_var_nr(VV_WINDOWID, HandleToLong(s_hwnd));
#endif

#ifdef FEAT_RENDER_OPTIONS
    যদি (p_rop)
	(শূন্য)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    ফেরত OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    স্থির শূন্য
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    যদি (mon != ফাঁকা)
    {
	moninfo.cb_আকার = আকার(MONITORINFO);
	যদি (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ফেরত;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    শূন্য
gui_mch_set_shellsize(
	পূর্ণসংখ্যা width,
	পূর্ণসংখ্যা height,
	পূর্ণসংখ্যা min_width UNUSED,
	পূর্ণসংখ্যা min_height UNUSED,
	পূর্ণসংখ্যা base_width UNUSED,
	পূর্ণসংখ্যা base_height UNUSED,
	পূর্ণসংখ্যা direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    পূর্ণসংখ্যা		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    যদি (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    যদি (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    অন্যথা
	// Use current position.
	GetWindowRect(s_hwnd, &window_rect);

    // compute the size of the outside of the window
    win_width = width + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_height = height + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_height(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    window_rect.ডান = window_rect.বাম + win_width;
    window_rect.bottom = window_rect.শীর্ষ_2 + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    যদি (!s_in_dpichanged)
    {
	যদি ((direction & RESIZE_HOR)
		&& window_rect.ডান > workarea_rect.ডান)
	    OffsetRect(&window_rect,
		    workarea_rect.ডান - window_rect.ডান, 0);

	যদি ((direction & RESIZE_HOR)
		&& window_rect.বাম < workarea_rect.বাম)
	    OffsetRect(&window_rect,
		    workarea_rect.বাম - window_rect.বাম, 0);

	যদি ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	যদি ((direction & RESIZE_VERT)
		&& window_rect.শীর্ষ_2 < workarea_rect.শীর্ষ_2)
	    OffsetRect(&window_rect,
		    0, workarea_rect.শীর্ষ_2 - window_rect.শীর্ষ_2);
    }

    MoveWindow(s_hwnd, window_rect.বাম, window_rect.শীর্ষ_2,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    শূন্য
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	আকার_2,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    যখন (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	আকার_2 >>= 1;
	++sb->scroll_shift;
    }

    যদি (sb->scroll_shift > 0)
	++আকার_2;

    info.cb_আকার = আকার(info);
    info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.nPage = আকার_2;
    SetScrollInfo(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    শূন্য
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    শূন্য
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    শূন্য
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    শূন্য
gui_mch_set_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#ifdef FEAT_MBYTE_IME
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
    স্থির LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_তথ্য UNUSED)
{
    LRESULT l_ফলাফল = 0;
    HIMC hImc;

    যদি (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ফেরত l_ফলাফল;
    নির্বাচন (dwCommand)
    {
	ক্ষেত্র IMN_SETOPENSTATUS:
	    যদি (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		যদি (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		যদি (State & MODE_ঢোকাও)
		{
# যদি defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    যদি (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			পূর্ণসংখ্যা old_row = gui.row;
			পূর্ণসংখ্যা old_col = gui.col;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			status_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.row = old_row;
			gui.col = old_col;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_ফলাফল = 0;
	    বিরতি;
    }
    pImmReleaseContext(hWnd, hImc);
    ফেরত l_ফলাফল;
}

    স্থির LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    পূর্ণসংখ্যা		len;

    যদি ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ফেরত 0;

    ret = Get_ফলাফল_Str(hwnd, GCS_RESULTSTR, &len);
    যদি (ret != ফাঁকা)
    {
	add_to_প্রবেশ_buf_csi(ret, len);
	vim_free(ret);
	ফেরত 1;
    }
    ফেরত 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    স্থির char_u *
Get_ফলাফল_Str(HWND hwnd, পূর্ণসংখ্যা GCS, পূর্ণসংখ্যা *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ফাঁকা;
    char_u	*convbuf = ফাঁকা;

    যদি (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ফেরত ফাঁকা;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ফাঁকা, 0);
    যদি (ret <= 0)
	ফেরত ফাঁকা;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + আকার(WCHAR));
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / আকার(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ফেরত convbuf;
}
#endif

// For global functions we need prototypes.
#যদি defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    শূন্য
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    যদি (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    শূন্য
im_set_position(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col)
{
    HIMC hImc;

    যদি (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	যদি (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	pImmSetCompositionWindow(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    শূন্য
im_set_active(পূর্ণসংখ্যা active)
{
    HIMC	hImc;
    স্থির HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    যদি (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	ফেরত;
    }
# endif

    যদি (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ফেরত;

    যদি (p_imdisable)
    {
	যদি (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    যদি (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    অন্যথা যদি (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    যদি (!hImc)
	ফেরত;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    যদি (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	স্থির DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	স্থির BOOL bSaved = FALSE;

	যদি (active)
	{
	    // if we have a saved conversion status, restore it
	    যদি (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	অন্যথা
	{
	    // save conversion status and disable korean
	    যদি (pImmGetConversionStatus(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		pImmSetConversionStatus(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    pImmSetOpenStatus(hImc, active);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    পূর্ণসংখ্যা
im_get_status(শূন্য)
{
    পূর্ণসংখ্যা		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    যদি (!gui.in_use && !gui.starting)
	ফেরত mbyte_im_get_status();
# endif

    যদি (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ফেরত status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    স্থির শূন্য
latin9_to_ucs(char_u *text, পূর্ণসংখ্যা len, WCHAR *unicodebuf)
{
    পূর্ণসংখ্যা		c;

    যখন (--len >= 0)
    {
	c = *text++;
	নির্বাচন (c)
	{
	    ক্ষেত্র 0xa4: c = 0x20ac; বিরতি;   // euro
	    ক্ষেত্র 0xa6: c = 0x0160; বিরতি;   // S hat
	    ক্ষেত্র 0xa8: c = 0x0161; বিরতি;   // S -hat
	    ক্ষেত্র 0xb4: c = 0x017d; বিরতি;   // Z hat
	    ক্ষেত্র 0xb8: c = 0x017e; বিরতি;   // Z -hat
	    ক্ষেত্র 0xbc: c = 0x0152; বিরতি;   // OE
	    ক্ষেত্র 0xbd: c = 0x0153; বিরতি;   // oe
	    ক্ষেত্র 0xbe: c = 0x0178; বিরতি;   // Y
	}
	*unicodebuf++ = c;
    }
}

#ifdef FEAT_RIGHTLEFT
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
    স্থির শূন্য
RevOut( HDC hdc,
	পূর্ণসংখ্যা col,
	পূর্ণসংখ্যা row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    পূর্ণসংখ্যা		ix;

    জন্য (ix = 0; ix < (পূর্ণসংখ্যা)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    স্থির শূন্য
draw_সারি(
    পূর্ণসংখ্যা		x1,
    পূর্ণসংখ্যা		y1,
    পূর্ণসংখ্যা		x2,
    পূর্ণসংখ্যা		y2,
    COLORREF	color)
{
#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_সারি(s_dwc, x1, y1, x2, y2, color);
    অন্যথা
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, ফাঁকা);
	// Note: LineTo() excludes the last pixel in the line.
	সারি_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    স্থির শূন্য
set_pixel(
    পূর্ণসংখ্যা		x,
    পূর্ণসংখ্যা		y,
    COLORREF	color)
{
#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    অন্যথা
#endif
	SetPixel(s_hdc, x, y, color);
}

    স্থির শূন্য
fill_rect(
    ধ্রুবক RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    অন্যথা
#endif
    {
	HBRUSH	hbr2;

	যদি (hbr == ফাঁকা)
	    hbr2 = CreateSolidBrush(color);
	অন্যথা
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	যদি (hbr == ফাঁকা)
	    DeleteBrush(hbr2);
    }
}

    শূন্য
gui_mch_draw_string(
    পূর্ণসংখ্যা		row,
    পূর্ণসংখ্যা		col,
    char_u	*text,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		flags)
{
    স্থির পূর্ণসংখ্যা	*padding = ফাঁকা;
    স্থির পূর্ণসংখ্যা	pad_আকার = 0;
    ধ্রুবক RECT	*pcliprect = ফাঁকা;
    UINT	foptions = 0;
    স্থির WCHAR *unicodebuf = ফাঁকা;
    স্থির পূর্ণসংখ্যা   *unicodepdy = ফাঁকা;
    স্থির পূর্ণসংখ্যা	unibuflen = 0;
    পূর্ণসংখ্যা		n = 0;
    পূর্ণসংখ্যা		y;

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
    স্থির HBRUSH	hbr_cache[2] = {ফাঁকা, ফাঁকা};
    স্থির guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    স্থির পূর্ণসংখ্যা		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    যদি (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.বাম = FILL_X(col);
	rc.শীর্ষ_2 = FILL_Y(row);
	যদি (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ডান = FILL_X(col + mb_string2cells(text, len));
	}
	অন্যথা
	    rc.ডান = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	যদি (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	অন্যথা যদি (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	অন্যথা
	{
	    যদি (hbr_cache[brush_lru] != ফাঁকা)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	SetBkMode(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	যদি (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	অন্যথা যদি (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    যদি (pad_আকার != Columns || padding == ফাঁকা || padding[0] != gui.char_width)
    {
	পূর্ণসংখ্যা	i;

	vim_free(padding);
	pad_আকার = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(পূর্ণসংখ্যা, pad_আকার);
	যদি (padding != ফাঁকা)
	    জন্য (i = 0; i < pad_আকার; i++)
		padding[i] = gui.char_width;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    যদি (enc_utf8)
	জন্য (n = 0; n < len; ++n)
	    যদি (text[n] >= 0x80)
		বিরতি;

#যদি defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    যদি (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    যদি ((enc_utf8
		|| (enc_codepage > 0 && (পূর্ণসংখ্যা)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ফাঁকা || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(পূর্ণসংখ্যা, len);

	unibuflen = len;
    }

    যদি (enc_utf8 && n < len && unicodebuf != ফাঁকা)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	পূর্ণসংখ্যা		i;
	পূর্ণসংখ্যা		wlen;	// string length in words
	পূর্ণসংখ্যা		cells;	// cell width of string up to composing char
	পূর্ণসংখ্যা		cw;	// width of current cell
	পূর্ণসংখ্যা		c;

	wlen = 0;
	cells = 0;
	জন্য (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    যদি (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    অন্যথা
	    {
		unicodebuf[wlen++] = c;
	    }

	    যদি (utf_iscomposing(c))
		cw = 0;
	    অন্যথা
	    {
		cw = utf_char2cells(c);
		যদি (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    যদি (unicodepdy != ফাঁকা)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		যদি (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		অন্যথা
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#যদি defined(FEAT_DIRECTX)
	যদি (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	অন্যথা
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    অন্যথা যদি ((enc_codepage > 0 && (পূর্ণসংখ্যা)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	যদি (unicodebuf != ফাঁকা)
	{
	    যদি (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    অন্যথা
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(অক্ষর *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    যদি (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		যদি (unicodepdy != ফাঁকা)
		{
		    পূর্ণসংখ্যা i;
		    পূর্ণসংখ্যা cw;

		    জন্য (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			যদি (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    অন্যথা
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	যদি (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (অক্ষর *)text, len, padding);
	অন্যথা
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (অক্ষর *)text, len, padding);
    }

    // Underline
    যদি (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	যদি (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_সারি(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    যদি (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_সারি(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    যদি (flags & DRAW_UNDERC)
    {
	পূর্ণসংখ্যা			x;
	পূর্ণসংখ্যা			offset;
	স্থির ধ্রুবক পূর্ণসংখ্যা	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	জন্য (x = FILL_X(col); x < FILL_X(col + len); ++x)
	{
	    offset = val[x % 8];
	    set_pixel(x, y - offset, gui.currSpColor);
	}
    }
}


/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    শূন্য
gui_mch_flush(শূন্য)
{
#যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    স্থির শূন্য
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ফাঁকা, gui.back_pixel);
}


    শূন্য
gui_mch_get_screen_dimensions(পূর্ণসংখ্যা *screen_w, পূর্ণসংখ্যা *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ডান - workarea_rect.বাম
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.শীর্ষ_2
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#যদি defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    শূন্য
gui_mch_add_menu(
    vimmenu_T	*menu,
    পূর্ণসংখ্যা		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    যদি (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, ফাঁকা);
	যদি (wn == ফাঁকা)
	    ফেরত;

	infow.cb_আকার = আকার(infow);
	infow.fMask = MIIM_তথ্য | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_Item_তথ্য = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_তথ্য = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ঢোকাও_Menu_Item_W((parent == ফাঁকা)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    যদি (parent == ফাঁকা)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    অন্যথা যদি (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    শূন্য
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (শূন্য)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (পূর্ণসংখ্যা)mp.x, (পূর্ণসংখ্যা)mp.y);
}

    শূন্য
gui_make_popup(char_u *path_name, পূর্ণসংখ্যা mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    যদি (menu == ফাঁকা)
	ফেরত;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    যদি (mouse_pos)
    {
	পূর্ণসংখ্যা	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    অন্যথা যদি (curwin != ফাঁকা)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    বার্তা_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (পূর্ণসংখ্যা)p.x, (পূর্ণসংখ্যা)p.y);
}

# যদি defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    শূন্য
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    যদি (menu != ফাঁকা)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    শূন্য
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    পূর্ণসংখ্যা		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = ফাঁকা;

# ifdef FEAT_TEAROFF
    যদি (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ঢোকাও_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    অন্যথা
# endif
# ifdef FEAT_TOOLBAR
    যদি (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	যদি (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	অন্যথা
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	SendMessage(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    অন্যথা
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, ফাঁকা);
	যদি (wn != ফাঁকা)
	{
	    ঢোকাও_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	যদি (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    শূন্য
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    যদি (menu->submenu_id == (HMENU)-1)
    {
	পূর্ণসংখ্যা iButton;

	iButton = (পূর্ণসংখ্যা)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    অন্যথা
# endif
    {
	যদি (menu->parent != ফাঁকা
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != ফাঁকা)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	অন্যথা
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	যদি (menu->submenu_id != ফাঁকা)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	যদি (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	যদি (menu->parent != ফাঁকা
		&& menu->parent->children != ফাঁকা
		&& IsWindow(menu->parent->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->parent);
	}
# endif
    }
}

# ifdef FEAT_TEAROFF
    স্থির শূন্য
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    পূর্ণসংখ্যা		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    যদি (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.বাম - rct.বাম;
	y = (trect.শীর্ষ_2 -  rct.bottom  + roct.bottom);
    }
    অন্যথা
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    যদি (menu->children != ফাঁকা)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	যদি (IsWindow(menu->tearoff_handle))
	    (শূন্য) SetWindowPos(menu->tearoff_handle,
				ফাঁকা,
				(পূর্ণসংখ্যা)trect.বাম,
				(পূর্ণসংখ্যা)trect.শীর্ষ_2,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    শূন্য
gui_mch_menu_grey(
    vimmenu_T	*menu,
    পূর্ণসংখ্যা	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    যদি (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    অন্যথা
# endif
	(শূন্য)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    যদি ((menu->parent != ফাঁকা) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	যদি (menu->children == ফাঁকা)
	    menuID = (WORD)(menu->id);
	অন্যথা
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	যদি (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#যদি defined(FEAT_GUI_DIALOG)
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
    স্থির LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    যদি (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(শূন্য)SetFocus(hwnd);
	যদি (dialog_default_button > IDCANCEL)
	    (শূন্য)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	অন্যথা
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (শূন্য)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	ফেরত FALSE;
    }

    যদি (message == WM_COMMAND)
    {
	পূর্ণসংখ্যা	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	যদি (button >= DLG_NONBUTTON_CONTROL)
	    ফেরত TRUE;

	// If the edit box exists, copy the string.
	যদি (s_textfield != ফাঁকা)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, ফাঁকা);
	    যদি (p != ফাঁকা)
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
	যদি (button == IDOK)
	{
	    যদি (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	অন্যথা
	    EndDialog(hwnd, button - IDCANCEL);
	ফেরত TRUE;
    }

    যদি ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	ফেরত TRUE;
    }
    ফেরত FALSE;
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

স্থির ধ্রুবক অক্ষর *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    পূর্ণসংখ্যা
gui_mch_dialog(
    পূর্ণসংখ্যা		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    পূর্ণসংখ্যা		 dfltbutton,
    char_u	*textfield,
    পূর্ণসংখ্যা		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    পূর্ণসংখ্যা		অঙ্ক_Buttons;
    পূর্ণসংখ্যা		*buttonWidths, *buttonPositions;
    পূর্ণসংখ্যা		buttonYpos;
    পূর্ণসংখ্যা		nchar, i;
    DWORD	lStyle;
    পূর্ণসংখ্যা		dlgwidth = 0;
    পূর্ণসংখ্যা		dlgheight;
    পূর্ণসংখ্যা		editboxheight;
    পূর্ণসংখ্যা		horizWidth = 0;
    পূর্ণসংখ্যা		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    পূর্ণসংখ্যা		fontHeight;
    পূর্ণসংখ্যা		textWidth, minButtonWidth, messageWidth;
    পূর্ণসংখ্যা		maxDialogWidth;
    পূর্ণসংখ্যা		maxDialogHeight;
    পূর্ণসংখ্যা		scroll_flag = 0;
    পূর্ণসংখ্যা		vertical;
    পূর্ণসংখ্যা		dlgPaddingX;
    পূর্ণসংখ্যা		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    পূর্ণসংখ্যা		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    পূর্ণসংখ্যা		l;
    পূর্ণসংখ্যা		dlg_icon_width;
    পূর্ণসংখ্যা		dlg_icon_height;
    পূর্ণসংখ্যা		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    যদি (!(gui.in_use || gui.starting))
#  endif
	যদি (silent_mode)
	    ফেরত dfltbutton;   // return default option
# endif

    যদি (s_hwnd == ফাঁকা)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    অন্যথা
	dpi = pGetDpiForSystem();

    যদি ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_আকার + STRLEN(message) * 2);

    যদি (p == ফাঁকা)
	ফেরত -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    যদি (tbuffer == ফাঁকা)
	ফেরত -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    অঙ্ক_Buttons = 1;
    জন্য (i = 0; tbuffer[i] != '\0'; i++)
    {
	যদি (tbuffer[i] == DLG_BUTTON_SEP)
	    অঙ্ক_Buttons++;
    }
    যদি (dfltbutton >= অঙ্ক_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(পূর্ণসংখ্যা, অঙ্ক_Buttons);
    যদি (buttonWidths == ফাঁকা)
	ফেরত -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(পূর্ণসংখ্যা, অঙ্ক_Buttons);
    যদি (buttonPositions == ফাঁকা)
	ফেরত -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    যদি (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    অন্যথা
# endif
	font = CreateFont(-DLG_FONT_POINT_আকার, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    GetTextMetrics(hdc, &fontInfo);
    fontHeight = fontInfo.tmHeight;

    // Minimum width for horizontal button
    minButtonWidth = GetTextWidth(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    যদি (s_hwnd == ফাঁকা)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.ডান - workarea_rect.বাম - 100;
	যদি (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.শীর্ষ_2 - 150;
    }
    অন্যথা
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.ডান - rect.বাম
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	যদি (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.শীর্ষ_2
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	যদি (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, আকার(অক্ষর), 500);
    করো
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = ফাঁকা;
	জন্য (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    যদি (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    যদি (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		যদি (last_white != ফাঁকা)
		{
		    // break the line just after a space
		    যদি (pend > last_white)
			ga.ga_len -= (পূর্ণসংখ্যা)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = ফাঁকা;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		চালু;
	    }

	    যখন (--l >= 0)
		ga_append(&ga, *pend++);
	}
	যদি (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } যখন (*pend != NUL);

    যদি (ga.ga_তথ্য != ফাঁকা)
	message = ga.ga_তথ্য;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    যদি (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ফাঁকা && vim_strchr(p_go, GO_VERTICAL) != ফাঁকা);
    যদি (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	করো
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    যদি (pend == ফাঁকা)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (পূর্ণসংখ্যা)(pend - pstart));
	    যদি (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} যখন (*pend != NUL);

	যদি (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	অন্যথা যদি (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    যদি (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	করো
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    যদি (pend == ফাঁকা)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (পূর্ণসংখ্যা)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    যদি (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} যখন (*pend != NUL);
    }

    যদি (dlgwidth < DLG_MIN_WIDTH)
	dlgwidth = DLG_MIN_WIDTH;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    add_long(lStyle);
    add_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    add_word(0);	// NumberOfItems(will change later)
    add_word(10);	// x
    add_word(10);	// y
    add_word(PixelToDialogX(dlgwidth));	// cx

    // Dialog height.
    যদি (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * অঙ্ক_Buttons;
    অন্যথা
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    যদি (textfield != ফাঁকা)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    যদি (dlgheight > maxDialogHeight)
    {
	msgheight = msgheight - (dlgheight - maxDialogHeight);
	dlgheight = maxDialogHeight;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	messageWidth = dlgwidth - dlg_icon_width - 3 * dlgPaddingX;
    }

    add_word(PixelToDialogY(dlgheight));

    add_word(0);	// Menu
    add_word(0);	// Class

    // copy the title of the dialog
    nchar = nCopyAnsiToWideChar(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_VERSION_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    যদি (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (পূর্ণসংখ্যা)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    অন্যথা
# endif
    {
	*p++ = DLG_FONT_POINT_আকার;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    যদি (textfield != ফাঁকা)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    যদি (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    জন্য (i = 0; i < অঙ্ক_Buttons; i++)
    {
	// get end of this button.
	জন্য (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	যদি (*pend)
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
	যদি (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (অক্ষর *)pstart);
	}
	অন্যথা
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (অক্ষর *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += অঙ্ক_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_বাম|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (অক্ষর *)message);

    // Edit box
    যদি (textfield != ফাঁকা)
    {
	p = add_dialog_element(p, ES_বাম|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (অক্ষর *)textfield);
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
    যদি (textfield != ফাঁকা)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    অন্যথা
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ফাঁকা;
    }

    // show the dialog box modally and get a return value
    nchar = (পূর্ণসংখ্যা)DialogBoxIndirect(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_তথ্য);

    // Focus back to our window (for when MDI is used).
    (শূন্য)SetFocus(s_hwnd);

    ফেরত nchar;
}

#endif // FEAT_GUI_DIALOG

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
    স্থির PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    ধ্রুবক অক্ষর *caption)
{
    পূর্ণসংখ্যা nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_CHILD;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (WORD)0xffff;
    *p++ = clss;			//2 more here

    nchar = nCopyAnsiToWideChar(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    ফেরত p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    স্থির LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ফেরত (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    স্থির পূর্ণসংখ্যা
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    পূর্ণসংখ্যা		nChar = 0;
    পূর্ণসংখ্যা		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    পূর্ণসংখ্যা		i;
    WCHAR	*wn;

    যদি (use_enc && enc_codepage >= 0 && (পূর্ণসংখ্যা)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, ফাঁকা);
	যদি (wn != ফাঁকা)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (পূর্ণসংখ্যা)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    যদি (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    জন্য (i = 0; i < nChar; ++i)
	যদি (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    ফেরত nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    স্থির HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    জন্য ( ; menu != ফাঁকা; menu = menu->পরবর্তী)
    {
	যদি (menu->modes == 0)	// this menu has just been deleted
	    চালু;
	যদি (menu_is_separator(menu->dname))
	    চালু;
	যদি ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ফেরত menu->submenu_id;
    }
    ফেরত ফাঁকা;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    স্থির LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    যদি (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ফেরত (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    যদি (message == WM_COMMAND)
    {
	যদি ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    যদি (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(শূন্য)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (পূর্ণসংখ্যা)rect.ডান - 8,
			 (পূর্ণসংখ্যা)mp.y,
			 (পূর্ণসংখ্যা)0,	    // reserved param
			 s_hwnd,
			 ফাঁকা);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	অন্যথা
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(শূন্য)SetFocus(s_hwnd);
	ফেরত TRUE;
    }
    যদি ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	ফেরত TRUE;
    }

    // When moved around, give main window the focus back.
    যদি (message == WM_EXITSIZEMOVE)
	(শূন্য)SetActiveWindow(s_hwnd);

    ফেরত FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    স্থির শূন্য
get_dialog_font_metrics(শূন্য)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    আকার_4	    আকার_2;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    যদি (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    অন্যথা
#endif
	hfontTools = CreateFont(-DLG_FONT_POINT_আকার, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_Average_Font_আকার(hdc, &আকার_2);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (WORD)আকার_2.cx;
    s_dlgfntheight = (WORD)আকার_2.cy;
}

#যদি defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    স্থির শূন্য
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    পূর্ণসংখ্যা		initX,
    পূর্ণসংখ্যা		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    পূর্ণসংখ্যা		template_len;
    পূর্ণসংখ্যা		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*শীর্ষ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    পূর্ণসংখ্যা		col, spaceWidth, len;
    পূর্ণসংখ্যা		columnWidths[2];
    char_u	*label, *text;
    পূর্ণসংখ্যা		acLen = 0;
    পূর্ণসংখ্যা		nameLen;
    পূর্ণসংখ্যা		padding0, padding1, padding2 = 0;
    পূর্ণসংখ্যা		sepPadding=0;
    পূর্ণসংখ্যা		x;
    পূর্ণসংখ্যা		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    পূর্ণসংখ্যা		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    যদি (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	যদি (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, ফাঁকা, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ফেরত;
    }

    /*
     * Create a new tearoff.
     */
    যদি (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_আকার;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    যদি (p == ফাঁকা)
	ফেরত;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    যদি (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    অন্যথা
# endif
	font = CreateFont(-DLG_FONT_POINT_আকার, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    spaceWidth = GetTextWidth(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenuWidth = 0;
    জন্য (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    যদি (text != ফাঁকা && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (পূর্ণসংখ্যা)STRLEN(text));
		যদি (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    যদি (pmenu->children != ফাঁকা)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    যদি (columnWidths[1] == 0)
    {
	// no accelerators
	যদি (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	অন্যথা
	    columnWidths[0] += spaceWidth;
    }
    অন্যথা
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    যদি (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (পূর্ণসংখ্যা)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (পূর্ণসংখ্যা)STRLEN(title));
    যদি (textWidth > dlgwidth)
	dlgwidth = textWidth;
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
    যদি (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    অন্যথা
	*p++ = PixelToDialogX(initX); // x
    যদি (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    অন্যথা
	*p++ = PixelToDialogY(initY); // y
    *p++ = PixelToDialogX(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = nCopyAnsiToWideChar(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_VERSION_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    যদি (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (পূর্ণসংখ্যা)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    অন্যথা
# endif
    {
	*p++ = DLG_FONT_POINT_আকার;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    যদি (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->পরবর্তী;
    অন্যথা
	menu = menu->children;
    শীর্ষ_menu = menu;
    জন্য ( ; menu != ফাঁকা; menu = menu->পরবর্তী)
    {
	যদি (menu->modes == 0)	// this menu has just been deleted
	    চালু;
	যদি (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    চালু;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	যদি (((অক্ষর *)p - (অক্ষর *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    যদি (newp != ফাঁকা)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (অক্ষর *)p - (অক্ষর *)pdlgtemplate);
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
	len = nameLen = (পূর্ণসংখ্যা)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (পূর্ণসংখ্যা)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	যদি (menu->actext != ফাঁকা)
	{
	    acLen = (পূর্ণসংখ্যা)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	অন্যথা
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	যদি (menu->children == ফাঁকা)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	অন্যথা
	{
	    len += (পূর্ণসংখ্যা)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	যদি (label == ফাঁকা)
	    বিরতি;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	যদি (text == ফাঁকা)
	    text = label + nameLen;	    // no actext, use whole name
	যখন (padding0-- > 0)
	    *text++ = ' ';
	যদি (menu->actext != ফাঁকা)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	যখন (padding1-- > 0)
	    *text++ = ' ';
	যদি (menu->children != ফাঁকা)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	অন্যথা
	{
	    যখন (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_বাম,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (অক্ষর *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (WORD)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = CreateDialogIndirectParam(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback,
	    (LPARAM)শীর্ষ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (শূন্য)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#যদি defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    স্থির শূন্য
initialise_toolbar(শূন্য)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ফাঁকা,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    আকার(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ফাঁকা);

    update_toolbar_আকার();
}

    স্থির শূন্য
update_toolbar_আকার(শূন্য)
{
    পূর্ণসংখ্যা		w, h;
    TBMETRICS	tbm;

    tbm.cb_আকার = আকার(TBMETRICS);
    tbm.dwMask = TBMF_PAD | TBMF_BUTTONSPACING;
    SendMessage(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_WIDTH + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_HEIGHT + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    SendMessage(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_height = h + 6;

    //DWORD s = SendMessage(s_toolbarhwnd, TB_GETBUTTONSIZE, 0, 0);
    //TRACE("actual button size: %d, %d", LOWORD(s), HIWORD(s));

    // TODO:
    // Currently, this function only updates the size of toolbar buttons.
    // It would be nice if the toolbar images are resized based on DPI.
}

    স্থির LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_বার্তা,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_বার্তা, lParam);
    ফেরত CallWindowProc(s_toolbar_wndproc, hwnd, u_বার্তা, wParam, lParam);
}

    স্থির পূর্ণসংখ্যা
get_toolbar_bitmap(vimmenu_T *menu)
{
    পূর্ণসংখ্যা i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    যদি (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ফাঁকা;

	যদি (menu->iconfile != ফাঁকা)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			ফাঁকা,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_WIDTH,
			TOOLBAR_BUTTON_HEIGHT,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	যদি (hbitmap == ফাঁকা
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ফাঁকা ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    ফাঁকা,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	যদি (hbitmap != ফাঁকা)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = ফাঁকা;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (পূর্ণসংখ্যা)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    যদি (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_গুনতি)
	i = menu->iconidx;

    ফেরত i;
}
#endif

#যদি defined(FEAT_GUI_TABLINE)
    স্থির শূন্য
initialise_tabline(শূন্য)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ফাঁকা, g_hinst, ফাঁকা);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    স্থির tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = ফাঁকা;

    যদি (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	যদি (s_tabhwnd == hWnd)
	{
	    পূর্ণসংখ্যা idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    যদি (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    ফেরত ptp;
}

স্থির POINT	    s_pt = {0, 0};
স্থির HCURSOR      s_hCursor = ফাঁকা;

    স্থির LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_বার্তা,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    পূর্ণসংখ্যা		nCenter;
    পূর্ণসংখ্যা		idx0;
    পূর্ণসংখ্যা		idx1;

    HandleMouseHide(u_বার্তা, lParam);

    নির্বাচন (u_বার্তা)
    {
	ক্ষেত্র WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		বিরতি;
	    }
	ক্ষেত্র WM_MOUSEMOVE:
	    যদি (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		যদি (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(ফাঁকা, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    যদি (tp != ফাঁকা)
		    {
			idx0 = tabpage_সূচক(curtab) - 1;
			idx1 = tabpage_সূচক(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.বাম + (rect.ডান - rect.বাম) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			যদি ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			অন্যথা যদি ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    বিরতি;
	ক্ষেত্র WM_LBUTTONUP:
	    {
		যদি (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		বিরতি;
	    }
	ক্ষেত্র WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		যদি (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		বিরতি;
	    }
	পূর্বনির্ধারিত:
	    বিরতি;
    }

    ফেরত CallWindowProc(s_tabline_wndproc, hwnd, u_বার্তা, wParam, lParam);
}
#endif

#যদি defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    শূন্য
gui_mch_set_foreground(শূন্য)
{
    যদি (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#যদি defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    স্থির শূন্য
dyn_imm_load(শূন্য)
{
    hLibImm = vimLoadLib("imm32.dll");
    যদি (hLibImm == ফাঁকা)
	ফেরত;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))GetProcAddress(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))GetProcAddress(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))GetProcAddress(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))GetProcAddress(hLibImm, "ImmReleaseContext");
    pImmGetOpenStatus
	    = (BOOL (WINAPI *)(HIMC))GetProcAddress(hLibImm, "ImmGetOpenStatus");
    pImmSetOpenStatus
	    = (BOOL (WINAPI *)(HIMC, BOOL))GetProcAddress(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))GetProcAddress(hLibImm, "ImmGetCompositionFontW");
    pImmSetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))GetProcAddress(hLibImm, "ImmSetCompositionFontW");
    pImmSetCompositionWindow
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))GetProcAddress(hLibImm, "ImmSetCompositionWindow");
    pImmGetConversionStatus
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))GetProcAddress(hLibImm, "ImmGetConversionStatus");
    pImmSetConversionStatus
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))GetProcAddress(hLibImm, "ImmSetConversionStatus");

    যদি (       pImmGetCompositionStringW == ফাঁকা
	    || pImmGetContext == ফাঁকা
	    || pImmAssociateContext == ফাঁকা
	    || pImmReleaseContext == ফাঁকা
	    || pImmGetOpenStatus == ফাঁকা
	    || pImmSetOpenStatus == ফাঁকা
	    || pImmGetCompositionFontW == ফাঁকা
	    || pImmSetCompositionFontW == ফাঁকা
	    || pImmSetCompositionWindow == ফাঁকা
	    || pImmGetConversionStatus == ফাঁকা
	    || pImmSetConversionStatus == ফাঁকা)
    {
	FreeLibrary(hLibImm);
	hLibImm = ফাঁকা;
	pImmGetContext = ফাঁকা;
	ফেরত;
    }

    ফেরত;
}

#endif

#যদি defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

প্রকারলেখ কাঠামো _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    শূন্য
gui_mch_drawsign(পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, পূর্ণসংখ্যা typenr)
{
    signicon_t *sign;
    পূর্ণসংখ্যা x, y, w, h;

    যদি (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == ফাঁকা)
	ফেরত;

# যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    নির্বাচন (sign->uType)
    {
	ক্ষেত্র IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    বিরতি;
	ক্ষেত্র IMAGE_ICON:
	ক্ষেত্র IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ফাঁকা, DI_NORMAL);
	    বিরতি;
# ifdef FEAT_XPM_W32
	ক্ষেত্র IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    বিরতি;
# endif
    }
}

    স্থির শূন্য
close_signicon_image(signicon_t *sign)
{
    যদি (sign == ফাঁকা)
	ফেরত;

    নির্বাচন (sign->uType)
    {
	ক্ষেত্র IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    বিরতি;
	ক্ষেত্র IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    বিরতি;
	ক্ষেত্র IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    বিরতি;
# ifdef FEAT_XPM_W32
	ক্ষেত্র IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    বিরতি;
# endif
    }
}

    শূন্য *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ফাঁকা;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    যদি (ext > signfile)
    {
	পূর্ণসংখ্যা do_load = 1;

	যদি (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	অন্যথা যদি (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	অন্যথা যদি (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	অন্যথা
	    do_load = 0;

	যদি (do_load)
	    sign.hImage = (HANDLE)LoadImage(ফাঁকা, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	যদি (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((অক্ষর *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ফাঁকা;
    যদি (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ফাঁকা)
	*psign = sign;

    যদি (!psign)
    {
	যদি (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_তথ্য));
    }
    ফেরত (শূন্য *)psign;

}

    শূন্য
gui_mch_destroy_sign(শূন্য *sign)
{
    যদি (sign == ফাঁকা)
	ফেরত;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#যদি defined(FEAT_BEVAL_GUI)

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

    স্থির শূন্য
make_tooltip(BalloonEval *beval, অক্ষর *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    যদি (pti == ফাঁকা)
	ফেরত;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ফাঁকা, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, ফাঁকা, g_hinst, ফাঁকা);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_আকার = আকার(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, ফাঁকা);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    যদি (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ডান);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.বাম = pt.x - 3;
    pti->rect.শীর্ষ_2 = pt.y - 3;
    pti->rect.ডান = pt.x + 3;
    pti->rect.bottom = pt.y + 3;

    SendMessageW(beval->balloon, TTM_ADDTOOLW, 0, (LPARAM)pti);
    // Make tooltip appear sooner.
    SendMessageW(beval->balloon, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
    // I've performed some tests and it seems the longest possible life time
    // of tooltip is 30 seconds.
    SendMessageW(beval->balloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
    /*
     * HACK: force tooltip to appear, because it'll not appear until
     * first mouse move. D*mn M$
     * Amazingly moving (2, 2) and then (-1, -1) the mouse doesn't move.
     */
    mouse_event(MOUSEEVENTF_MOVE, 2, 2, 0, 0);
    mouse_event(MOUSEEVENTF_MOVE, (DWORD)-1, (DWORD)-1, 0, 0);
    vim_free(pti);
}

    স্থির শূন্য
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    স্থির VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_বার্তা UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    যদি (cur_beval == ফাঁকা || cur_beval->showState == ShS_SHOWING || !p_beval)
	ফেরত;

    GetCursorPos(&pt);
    যদি (WindowFromPoint(pt) != s_textArea)
	ফেরত;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    যদি (!PtInRect(&rect, pt))
	ফেরত;

    যদি (last_user_activity > 0
	    && (dwTime - last_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| abs(cur_beval->x - pt.x) > 3
		|| abs(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	যদি (cur_beval->বার্তা_CB != ফাঁকা)
	    (*cur_beval->বার্তা_CB)(cur_beval, 0);
    }
}

    শূন্য
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    শূন্য
gui_mch_enable_beval_area(BalloonEval *beval)
{
    যদি (beval == ফাঁকা)
	ফেরত;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    শূন্য
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->বার্তা);
    beval->বার্তা = mesg == ফাঁকা ? ফাঁকা : vim_strsave(mesg);
    যদি (beval->বার্তা == ফাঁকা)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ফেরত;
    }

    যদি (beval->showState == ShS_SHOWING)
	ফেরত;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    যদি (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (অক্ষর *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    শূন্য	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    শূন্য	(*mesgCB)(BalloonEval *, পূর্ণসংখ্যা),
    শূন্য	*client_তথ্য)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    যদি (mesg != ফাঁকা && mesgCB != ফাঁকা)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ফেরত ফাঁকা;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    যদি (beval != ফাঁকা)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->বার্তা = mesg;
	beval->বার্তা_CB = mesgCB;
	beval->client_তথ্য = client_তথ্য;

	InitCommonControls();
	cur_beval = beval;

	যদি (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ফেরত beval;
}

    স্থির শূন্য
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    যদি (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ফেরত;

    যদি (cur_beval == ফাঁকা)
	ফেরত;

    নির্বাচন (pnmh->code)
    {
	ক্ষেত্র TTN_SHOW:
	    বিরতি;
	ক্ষেত্র TTN_বের_করো: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    বিরতি;
	ক্ষেত্র TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    বিরতি;
	ক্ষেত্র TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    বিরতি;
    }
}

    স্থির শূন্য
track_user_activity(UINT u_বার্তা)
{
    যদি ((u_বার্তা >= WM_MOUSEFIRST && u_বার্তা <= WM_MOUSELAST)
	    || (u_বার্তা >= WM_KEYFIRST && u_বার্তা <= WM_KEYLAST))
	last_user_activity = Get_Tick_গুনতি();
}

    শূন্য
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#যদি defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    শূন্য
netbeans_draw_multisign_indicator(পূর্ণসংখ্যা row)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা y;
    পূর্ণসংখ্যা x;

    যদি (!netbeans_active())
	ফেরত;

    x = 0;
    y = TEXT_Y(row);

# যদি defined(FEAT_DIRECTX)
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    জন্য (i = 0; i < gui.char_height - 3; i++)
	SetPixel(s_hdc, x+2, y++, gui.currFgColor);

    SetPixel(s_hdc, x+0, y, gui.currFgColor);
    SetPixel(s_hdc, x+2, y, gui.currFgColor);
    SetPixel(s_hdc, x+4, y++, gui.currFgColor);
    SetPixel(s_hdc, x+1, y, gui.currFgColor);
    SetPixel(s_hdc, x+2, y, gui.currFgColor);
    SetPixel(s_hdc, x+3, y++, gui.currFgColor);
    SetPixel(s_hdc, x+2, y, gui.currFgColor);
}
#endif

#যদি defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    স্থির পূর্ণসংখ্যা
test_gui_w32_sendevent_mouse(dict_T *args)
{
    যদি (!dict_has_চাবি(args, "row") || !dict_has_চাবি(args, "col"))
	ফেরত FALSE;

    // Note: "move" is optional, requires fewer arguments
    পূর্ণসংখ্যা move = (পূর্ণসংখ্যা)dict_get_bool(args, "move", FALSE);

    যদি (!move && (!dict_has_চাবি(args, "button")
	    || !dict_has_চাবি(args, "multiclick")
	    || !dict_has_চাবি(args, "modifiers")))
	ফেরত FALSE;

    পূর্ণসংখ্যা row = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "row");
    পূর্ণসংখ্যা col = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "col");

    যদি (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	যদি (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    পূর্ণসংখ্যা pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    পূর্ণসংখ্যা pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	অন্যথা
	    gui_mouse_moved(col, row);
    }
    অন্যথা
    {
	পূর্ণসংখ্যা button = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "button");
	পূর্ণসংখ্যা repeated_click = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "multiclick");
	int_u mods = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    ফেরত TRUE;
}

    স্থির পূর্ণসংখ্যা
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    প্রবেশ_2 inputs[1];
    প্রবেশ_2 modkeys[3];
    SecureZeroMemory(inputs, আকার(প্রবেশ_2));
    SecureZeroMemory(modkeys, 3 * আকার(প্রবেশ_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    যদি (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_সংখ্যা_def(args, "keycode", 0);
	যদি (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ফেরত FALSE;
	}

	BOOL is_Mod_চাবি = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	পূর্ণসংখ্যা mods = (পূর্ণসংখ্যা)dict_get_সংখ্যা(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	যদি (mods && STRICMP(event, "keydown") == 0 && !is_Mod_চাবি)
	{
	    পূর্ণসংখ্যা n = 0;
	    যদি (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = প্রবেশ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    যদি (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = প্রবেশ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    যদি (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = প্রবেশ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    যদি (n)
	    {
		(শূন্য)SetForegroundWindow(s_hwnd);
		Send_প্রবেশ(n, modkeys, আকার(প্রবেশ_2));
	    }
	}

	inputs[0].type = প্রবেশ_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	যদি (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    যদি (!is_Mod_চাবি)
		unwrapMods = TRUE;
	}

	(শূন্য)SetForegroundWindow(s_hwnd);
	Send_প্রবেশ(ARRAYSIZE(inputs), inputs, আকার(প্রবেশ_2));
	vim_free(event);

	যদি (unwrapMods)
	{
	    modkeys[0].type = প্রবেশ_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = প্রবেশ_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = প্রবেশ_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (শূন্য)SetForegroundWindow(s_hwnd);
	    Send_প্রবেশ(3, modkeys, আকার(প্রবেশ_2));
	}
    }
    অন্যথা
    {
	যদি (event == ফাঁকা)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	অন্যথা
	{
	    semsg(_(e_invalid_মান_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	ফেরত FALSE;
    }
    ফেরত TRUE;
}

    স্থির পূর্ণসংখ্যা
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    পূর্ণসংখ্যা handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    যদি (strategy)
    {
	যদি (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	অন্যথা যদি (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    যদি (!handled)
    {
	যদি (strategy == ফাঁকা)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	অন্যথা
	{
	    semsg(_(e_invalid_মান_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ফেরত FALSE;
    }
    ফেরত TRUE;
}


    পূর্ণসংখ্যা
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    যদি (STRICMP(event, "key") == 0)
	ফেরত test_gui_w32_sendevent_keyboard(args);
    অন্যথা যদি (STRICMP(event, "mouse") == 0)
	ফেরত test_gui_w32_sendevent_mouse(args);
    অন্যথা যদি (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ফেরত test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    অন্যথা
    {
	semsg(_(e_invalid_মান_for_argument_str_str), "event", event);
	ফেরত FALSE;
    }
}
#endif

#যদি defined(FEAT_IMAGE_GUI)

প্রকারলেখ কাঠামো
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
    স্থির শূন্য
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, পূর্ণসংখ্যা iw, পূর্ণসংখ্যা ih, image_format_T fmt)
{
    পূর্ণসংখ্যা		y, x;
    পূর্ণসংখ্যা		stride = iw * 4;
    পূর্ণসংখ্যা		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    জন্য (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	জন্য (x = 0; x < iw; x++)
	{
	    যদি (fmt == IMAGE_FORMAT_RGBA)
	    {
		পূর্ণসংখ্যা a = s[3];

		যদি (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		অন্যথা যদি (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		অন্যথা
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    অন্যথা
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

    পূর্ণসংখ্যা
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    পূর্ণসংখ্যা		iw, ih;
    শূন্য	*bits = ফাঁকা;

    যদি (ctx == ফাঁকা)
	ফেরত FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.bi_আকার = আকার(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_গুনতি = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, ফাঁকা, 0);
    যদি (hbm == ফাঁকা || bits == ফাঁকা)
    {
	যদি (hbm != ফাঁকা)
	    DeleteObject(hbm);
	ফেরত false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    যদি (mem_dc == ফাঁকা)
    {
	DeleteObject(hbm);
	ফেরত false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->তথ্য, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_তথ্য = ctx;
    ফেরত OK;
}

    শূন্য
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_তথ্য;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    পূর্ণসংখ্যা
image_placement_gui_init(image_placement_T *place UNUSED)
{
    ফেরত OK;
}

    শূন্য
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    শূন্য
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_তথ্য;
    pixman_box32_t  *rects;
    পূর্ণসংখ্যা		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    যদি (rects == ফাঁকা || n_rects == 0)
	ফেরত;

# যদি defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    যদি (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    জন্য (পূর্ণসংখ্যা i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	পূর্ণসংখ্যা		row, col;
	পূর্ণসংখ্যা		posx, posy;
	পূর্ণসংখ্যা		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    শূন্য
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
