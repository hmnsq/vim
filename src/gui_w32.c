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
#endif

// values for "dead_key"
#define DEAD_مبودي_OFF			0	// no dead key
#define DEAD_مبودي_SET_DEFAULT		1	// dead key pressed
#define DEAD_مبودي_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_مبودي_SKIP_ON_CHAR		3	// skip next _OnChar()

#إذا defined(FEAT_DIRECTX)
ساكن DWriteContext *s_dwc = عدم;
ساكن صحيح s_directx_enabled = 0;
ساكن صحيح s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != عدم && enc_utf8)
ساكن صحيح directx_enabled(فراغ);
ساكن فراغ directx_binddc(فراغ);
#endif

#ifdef FEAT_MENU
ساكن صحيح gui_mswin_get_menu_height(صحيح fix_window);
#وإلا
# define gui_mswin_get_menu_height(fix_window)	0
#endif

تعريفنوع بنية keycode_trans_strategy {
    فراغ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
    فراغ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
    فراغ (*ptr_process_message_usual_مبودي) (UINT /*vk*/, ثابت سكو_2* /*pmsg*/);
    صحيح  (*ptr_get_active_modifiers)(فراغ);
    صحيح  (*is_experimental)(فراغ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ساكن فراغ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ process_message_usual_مبودي_experimental(UINT /*vk*/, ثابت سكو_2* /*pmsg*/);
ساكن صحيح  get_active_modifiers_experimental(فراغ);
ساكن صحيح  is_experimental_true(فراغ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_مبودي_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ساكن فراغ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحيح /*cRepeat UNUSED*/);
ساكن فراغ process_message_usual_مبودي_classic(UINT /*vk*/, ثابت سكو_2* /*pmsg*/);
ساكن صحيح  get_active_modifiers_classic(فراغ);
ساكن صحيح  is_experimental_false(فراغ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_مبودي_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = عدم;

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
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    إذا (strategy == عدم)
    {
	أرجع;
    }

    إذا (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	أرجع;
    }

    إذا (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	أرجع;
    }

}

#إذا defined(FEAT_RENDER_OPTIONS)
    صحيح
gui_mch_set_rendering_options(char_u *s)
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
	char_u  item[256];
	char_u  name[128];
	char_u  درج[128];

	copy_option_part(&p, item, حجم(item), ",");
	إذا (p == عدم)
	    اكسر;
	q = &item[0];
	copy_option_part(&q, name, حجم(name), ":");
	إذا (q == عدم)
	    أرجع FAIL;
	copy_option_part(&q, درج, حجم(درج), ":");

	إذا (STRCMP(name, "type") == 0)
	{
	    إذا (STRCMP(درج, "directx") == 0)
		dx_enable = 1;
	    وإلا
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (عائم)atof((حرف *)درج);
	}
	وإلا إذا (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (عائم)atof((حرف *)درج);
	}
	وإلا إذا (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (عائم)atof((حرف *)درج);
	}
	وإلا إذا (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((حرف *)درج);
	    إذا (dx_geom < 0 || dx_geom > 2)
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((حرف *)درج);
	    إذا (dx_renmode < 0 || dx_renmode > 6)
		أرجع FAIL;
	}
	وإلا إذا (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((حرف *)درج);
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

    إذا (!gui.in_use)
	أرجع OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    إذا (dx_enable)
    {
	إذا (!directx_enabled())
	    أرجع FAIL;
	DWriteContext_SetRenderingParams(s_dwc, عدم);
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
		param.renderingMode = dx_renmode;
	    إذا (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    أرجع OK;
# وإلا
    أرجع FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (صحيح)(short)LOWORD(lParam), (صحيح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (صحيح)(short)LOWORD(lParam), (صحيح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (صحيح)(short)LOWORD(lParam), (صحيح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#ضمن "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#ضمن <shellapi.h>
#ضمن <commctrl.h>
#ضمن <windowsx.h>

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
#define DLG_FONT_POINT_SIZE	8
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

ساكن فراغ _OnPaint(HWND hwnd);
ساكن فراغ fill_rect(ثابت RECT *rcp, HBRUSH hbr, COLORREF color);
ساكن فراغ clear_rect(RECT *rcp);

ساكن WORD		s_dlgfntheight;		// height of the dialog font
ساكن WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
ساكن HMENU		s_menuBar = عدم;
#endif
#ifdef FEAT_TEAROFF
ساكن فراغ rebuild_tearoff(vimmenu_T *menu);
ساكن HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ساكن صحيح		s_busy_processing = FALSE;

ساكن صحيح		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
ساكن UINT		s_findrep_سكو = 0;
ساكن FINDREPLACEW	s_findrep_struct;
ساكن HWND		s_findrep_hwnd = عدم;
ساكن صحيح		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = عدم;
ساكن HDC		s_hdc = عدم;
ساكن HBRUSH		s_brush = عدم;

#ifdef FEAT_TOOLBAR
ساكن HWND		s_toolbarhwnd = عدم;
ساكن WNDPROC		s_toolbar_wndproc = عدم;
#endif

#ifdef FEAT_GUI_TABLINE
ساكن HWND		s_tabhwnd = عدم;
ساكن WNDPROC		s_tabline_wndproc = عدم;
ساكن صحيح		showing_tabline = 0;
#endif

ساكن WPARAM		s_wParam = 0;

ساكن HWND		s_textArea = عدم;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ساكن bool		s_textarea_bg_only = false;

ساكن UINT		s_u_سكو = 0;

ساكن char_u		*s_textfield; // Used by dialogs to pass back strings

ساكن صحيح		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ساكن صحيح allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
تعريفنوع HANDLE DPI_AWARENESS_CONTEXT;

تعريفنوع تعداد DPI_AWARENESS {
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
ساكن صحيح		s_dpi = DEFAULT_DPI;
ساكن BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
ساكن DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
ساكن RECT		s_suggested_rect;

ساكن UINT (WINAPI *pGetDpiForSystem)(فراغ) = عدم;
ساكن UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = عدم;
ساكن صحيح (WINAPI *pGetSystemMetricsForDpi)(صحيح, UINT) = عدم;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ساكن DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = عدم;
#ifdef FEAT_MBYTE_IME
ساكن DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = عدم;
#endif

ساكن HINSTANCE hLibDwm = عدم;
ساكن HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
ساكن فراغ dyn_dwm_load(فراغ);

ساكن صحيح fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

ساكن HINSTANCE hUxThemeLib = عدم;
ساكن DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = عدم;
ساكن فراغ (WINAPI *pFlushMenuThemes)(فراغ) = عدم;
ساكن فراغ dyn_uxtheme_load(فراغ);
#endif

    ساكن صحيح WINAPI
stubGetSystemMetricsForDpi(صحيح nIndex, UINT dpi UNUSED)
{
    أرجع GetSystemMetrics(nIndex);
}

    ساكن صحيح
adjust_fontsize_by_dpi(صحيح size)
{
    أرجع size * s_dpi / (صحيح)pGetDpiForSystem();
}

    ساكن صحيح
adjust_by_system_dpi(صحيح size)
{
    أرجع size * (صحيح)pGetDpiForSystem() / DEFAULT_DPI;
}

#إذا defined(FEAT_DIRECTX)
    ساكن صحيح
directx_enabled(فراغ)
{
    إذا (s_dwc != عدم)
	أرجع 1;
    وإلا إذا (s_directx_load_attempted)
	أرجع 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    أرجع s_dwc != عدم ? 1 : 0;
}

    ساكن فراغ
directx_binddc(فراغ)
{
    إذا (s_textArea == عدم)
	أرجع;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

خارجي صحيح current_font_height;	    // this is in os_mswin.c

ساكن بنية
{
    UINT    مبودي_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_مبوداي[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_هغو,		'k', 'l'},
    {VK_دام,		'k', 'r'},

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
    {VK_سك,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_END,		'@', '7'},
    {VK_PRIOR,		'k', 'P'},
    {VK_NEXT,		'k', 'N'},
    {VK_بغ,		'%', '9'},
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
ساكن صحيح	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ساكن صحيح	s_getting_focus = FALSE;

ساكن صحيح	s_x_pending;
ساكن صحيح	s_y_pending;
ساكن UINT	s_kFlags_pending;
ساكن UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ساكن صحيح	s_timed_out = FALSE;
ساكن صحيح	dead_مبودي = DEAD_مبودي_OFF;
ساكن UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ساكن فراغ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ساكن فراغ track_user_activity(UINT u_سكو);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ساكن LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ساكن LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
ساكن LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#إذا defined(FEAT_BROWSE)
ساكن char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_بغ_ERROR
/*
 * Print out the last Windows error message
 */
    ساكن فراغ
بغ_windows_error(فراغ)
{
    LPVOID  lp_سكو_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  عدم, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_سكو_Buf, 0, عدم);
    TRACE1("Error: %s\n", lp_سكو_Buf);
    LocalFree(lp_سكو_Buf);
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
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ساكن VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_سكو UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    سكو_2 سكو;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(عدم, idEvent);

    // Eat spurious WM_TIMER messages
    طالما (PeekMessageW(&سكو, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(عدم, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(عدم, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ساكن فراغ
gui_mswin_rm_blink_timer(فراغ)
{
    سكو_2 سكو;

    إذا (blink_timer == 0)
	أرجع;

    KillTimer(عدم, blink_timer);
    // Eat spurious WM_TIMER messages
    طالما (PeekMessageW(&سكو, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
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
gui_mch_start_blink(فراغ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(عدم, 0, (UINT)blink_waittime, _OnBlinkTimer);
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
    UINT u_سكو UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    سكو_2 سكو;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(عدم, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    طالما (PeekMessageW(&سكو, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    إذا (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ساكن فراغ
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    صحيح cRepeat UNUSED)
{
    dead_مبودي = DEAD_مبودي_SET_DEFAULT;
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
char_to_string(صحيح ch, char_u *string, صحيح slen, صحيح had_alt)
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
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, عدم);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	إذا (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, عدم);
	    إذا (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    وإلا
    {
	ws = utf16_to_enc(wstring, &len);
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
get_active_modifiers_experimental(فراغ)
{
    صحيح modifiers = 0;

    إذا (Get_مبودي_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    إذا (Get_مبودي_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    إذا (Get_مبودي_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    إذا ((modifiers & MOD_MASK_CTRL) && (Get_مبودي_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    إذا (!(Get_مبودي_State(VK_CONTROL) & 0x8000)
	    &&  (Get_مبودي_State(VK_RMENU) & 0x8000)
	    && !(Get_مبودي_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    أرجع modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن صحيح
get_active_modifiers_classic(فراغ)
{
    صحيح modifiers = 0;

    إذا (Get_مبودي_State(VK_SHIFT) & 0x8000)
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
    إذا (Get_مبودي_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    إذا (Get_مبودي_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    أرجع modifiers;
}

    ساكن صحيح
get_active_modifiers(فراغ)
{
    أرجع get_active_modifiers_experimental();
}

    ساكن صحيح
get_active_modifiers_via_ptr(فراغ)
{
    // marshal to corresponding implementation
    أرجع keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ساكن فراغ
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    صحيح cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    صحيح cRepeat UNUSED)
{
    char_u	string[40];
    صحيح		len = 0;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    إذا (dead_مبودي == DEAD_مبودي_SKIP_ON_CHAR)
	أرجع;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    إذا (dead_مبودي != DEAD_مبودي_TRANSIENT_IN_ON_CHAR)
	dead_مبودي = DEAD_مبودي_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_مبودي(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_مبوداي;
    ch = may_adjust_مبودي_for_ctrl(modifiers, ch);
    --no_reduce_مبوداي;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, عدم);
    إذا (ch == CSI)
	ch = K_CSI;

    إذا (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_شغرو_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    إذا (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_شغرو_buf();
	got_int = TRUE;
    }

    add_to_شغرو_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    صحيح cRepeat UNUSED)
{
    char_u	string[40];
    صحيح		len = 0;

    dead_مبودي = 0;

    len = char_to_string(ch, string, 40, FALSE);
    إذا (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_شغرو_buf();
	got_int = TRUE;
    }

    add_to_شغرو_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ساكن فراغ
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    صحيح cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    صحيح cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    صحيح		len;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    dead_مبودي = DEAD_مبودي_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_مبودي(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, عدم);
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
	string[len++] = K_SECOND((صحيح)ch);
	string[len++] = K_THIRD((صحيح)ch);
    }
    وإلا
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_شغرو_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    صحيح cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    صحيح		len;
    صحيح		modifiers;
    صحيح		ch = cch;   // special keys are negative

    dead_مبودي = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    إذا (Get_مبودي_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    إذا (Get_مبودي_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_مبودي(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, عدم);
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
	string[len++] = K_SECOND((صحيح)ch);
	string[len++] = K_THIRD((صحيح)ch);
    }
    وإلا
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_شغرو_buf(string, len);
}

    ساكن فراغ
_OnMouseEvent(
    صحيح button,
    صحيح x,
    صحيح y,
    صحيح repeated_click,
    UINT مبودي_Flags)
{
    صحيح vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    إذا (مبودي_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    إذا (مبودي_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    إذا (Get_مبودي_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ساكن فراغ
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    صحيح x,
    صحيح y,
    UINT مبودي_Flags)
{
    ساكن LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    صحيح	    button = -1;
    صحيح	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (فراغ)SetFocus(s_hwnd);

    إذا (s_u_سكو == WM_LBUTTONDOWN || s_u_سكو == WM_LBUTTONDBLCLK)
	button = MOUSE_هغو;
    وإلا إذا (s_u_سكو == WM_MBUTTONDOWN || s_u_سكو == WM_MBUTTONDBLCLK)
	button = MOUSE_ڟكيا;
    وإلا إذا (s_u_سكو == WM_RBUTTONDOWN || s_u_سكو == WM_RBUTTONDBLCLK)
	button = MOUSE_دام;
    وإلا إذا (s_u_سكو == WM_XBUTTONDOWN || s_u_سكو == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    وإلا إذا (s_u_سكو == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	إذا (s_button_pending == MOUSE_هغو)
	    button = MOUSE_دام;
	وإلا
	    button = MOUSE_هغو;
    }

    إذا (button < 0)
	أرجع;

    repeated_click = ((صحيح)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    إذا (repeated_click
	    && ((button == MOUSE_هغو && s_button_pending == MOUSE_دام)
		|| (button == MOUSE_دام
		    && s_button_pending == MOUSE_هغو)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_ڟكيا;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, مبودي_Flags);
    }
    وإلا إذا ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_دام)))
    {
	إذا (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, مبودي_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, مبودي_Flags);
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
	s_kFlags_pending = مبودي_Flags;
    }

    s_prevTime = currentTime;
}

    ساكن فراغ
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    صحيح x,
    صحيح y,
    UINT مبودي_Flags)
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
    إذا (s_u_سكو == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	إذا (!(مبودي_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    أرجع;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    وإلا
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, مبودي_Flags);
}

    ساكن فراغ
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    صحيح cx UNUSED,
    صحيح cy UNUSED)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ساكن vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    صحيح		id)
{
    vimmenu_T	*pChildMenu;

    طالما (pMenu)
    {
	إذا (pMenu->id == (UINT)id)
	    اكسر;
	إذا (pMenu->children != عدم)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    إذا (pChildMenu)
	    {
		pMenu = pChildMenu;
		اكسر;
	    }
	}
	pMenu = pMenu->next;
    }
    أرجع pMenu;
}

    ساكن فراغ
_OnMenu(
    HWND	hwnd UNUSED,
    صحيح		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(سيو_menu, id);
    إذا (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ساكن فراغ
_OnFindRepl(فراغ)
{
    صحيح	    flags = 0;
    صحيح	    down;

    إذا (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(فراغ)SetFocus(s_hwnd);

    إذا (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(فراغ)SetFocus(s_hwnd);
    }
    وإلا إذا (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(فراغ)SetFocus(s_hwnd);
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
	flags |= FRD_WHOLE_WORD;
    إذا (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, عدم);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, عدم);
    إذا (p != عدم && q != عدم)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    ساكن فراغ
HandleMouseHide(UINT u_سكو, LPARAM lParam)
{
    ساكن LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    إذا (u_سكو == WM_MOUSEMOVE || u_سكو == WM_NCMOUSEMOVE)
    {
	إذا (lParam == last_lParam)
	    أرجع;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    بدل (u_سكو)
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
_TextAreaWndProc(
    HWND hwnd,
    UINT u_سكو,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_سكو, lParam);

    s_u_سكو = u_سكو;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_سكو);
#endif

    بدل (u_سكو)
    {
	HANDLE_سكو(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_سكو(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_سكو(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_سكو(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_سكو(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_سكو(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_سكو(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_سكو(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	حالة WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    أرجع TRUE;
#endif
	افتراضي:
	    أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
    }
}

    ساكن فراغ
dyn_dwm_load(فراغ)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    إذا (hLibDwm == عدم)
	أرجع;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    إذا (pDwmSetWindowAttribute == عدم)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = عدم;
	أرجع;
    }
}

خارجي DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    فراغ
gui_mch_set_titlebar_colors(فراغ)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    إذا (pDwmSetWindowAttribute == عدم || win_version < MAKE_VER(10, 0, 22000))
	أرجع;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    إذا (vim_strchr(p_go, GO_TITLEBAR) != عدم)
    {
	إذا (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	وإلا
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	إذا (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	إذا (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, حجم(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, حجم(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, عدم, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    إذا (s_textarea_bg_only && s_textArea != عدم)
	InvalidateRect(s_textArea, عدم, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    إذا (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	إذا (s_textArea != عدم)
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

    أرجع OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.هغو;
    *y = rect.كولي;
    أرجع OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_set_winpos(صحيح x, صحيح y)
{
    SetWindowPos(s_hwnd, عدم, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    فراغ
gui_mch_set_text_area_pos(صحيح x, صحيح y, صحيح w, صحيح h)
{
    ساكن صحيح oldx = 0;
    ساكن صحيح oldy = 0;

    SetWindowPos(s_textArea, عدم, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#إذا defined(FEAT_GUI_TABLINE)
    إذا (showing_tabline)
    {
	صحيح	كولي = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    كولي = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, كولي, rect.دام, gui.tabline_height, TRUE);
    }
#endif

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
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    SetWindowPos(sb->id, عدم, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    RECT    rcTxt, rcWnd;
    صحيح	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.دام - rcTxt.دام - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    RECT    rcTxt, rcWnd;
    صحيح	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, عدم,
	g_hinst, عدم);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ساكن scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    إذا (gui.bottom_sbar.id == hwnd)
	أرجع &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	إذا (wp->w_scrollbars[SBAR_هغو].id == hwnd)
	    أرجع &wp->w_scrollbars[SBAR_هغو];
	إذا (wp->w_scrollbars[SBAR_دام].id == hwnd)
	    أرجع &wp->w_scrollbars[SBAR_دام];
    }
    أرجع عدم;
}

    ساكن فراغ
update_scrollbar_size(فراغ)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ساكن فراغ
GetAverageFontSize(HDC hdc, SIZE *size)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    GetTextExtentPoint(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, size);
    size->cx = (size->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    ساكن فراغ
GetFontSize(GuiFont font, صحيح *char_width, صحيح *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    إذا (char_width)
	*char_width = size.cx + tm.tmOverhang;
    إذا (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ساكن فراغ
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
gui_mch_adjust_charheight(فراغ)
{
    UpdateFontSize(gui.norm_font);
    أرجع OK;
}

    ساكن GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = عدم;

    // Load the font
    font = CreateFontIndirectW(lf);

    إذا (font == عدم)
	أرجع NOFONT;

    أرجع (GuiFont)font;
}

    ساكن صحيح
pixels_to_points(صحيح pixels, صحيح vertical)
{
    صحيح		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

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
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
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
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    إذا (name == عدم)
	أرجع عدم;
    أرجع vim_strsave(name);
}
#endif

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

    تعريفنوع بنية SysColorTable
    {
	حرف	    *name;
	صحيح	    color;
    } SysColorTable;

    ساكن SysColorTable sys_table[] =
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
    لكل (i = 0; i < (صحيح)ARRAY_LENGTH(sys_table); i++)
	إذا (STRICMP(name, sys_table[i].name) == 0)
	    أرجع GetSysColor(sys_table[i].color);

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

    لكل (i = 0; special_مبوداي[i].vim_code1 != NUL; i++)
	إذا (name[0] == special_مبوداي[i].vim_code0
				       && name[1] == special_مبوداي[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

    فراغ
gui_mch_beep(فراغ)
{
    MessageBeep((UINT)-1);
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
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.هغو = FILL_X(c);
    rc.كولي = FILL_Y(r);
    rc.دام = rc.هغو + nc * gui.char_width;
    rc.bottom = rc.كولي + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
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
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.هغو = FILL_X(gui.col);
    rc.كولي = FILL_Y(gui.row);
    rc.دام = rc.هغو + gui.char_width;
    إذا (mb_lefthalve(gui.row, gui.col))
	rc.دام += gui.char_width;
    rc.bottom = rc.كولي + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
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
    rc.هغو =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_دام ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.كولي = FILL_Y(gui.row) + gui.char_height - h;
    rc.دام = rc.هغو + w;
    rc.bottom = rc.كولي + h;

    fill_rect(&rc, عدم, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ساكن فراغ
output_Dead_مبودي_re_Post_Ex(سكو_2 original_سكو, صحيح dead_key2set)
{
    ساكن سكو_2 deadCharExpel;

    إذا (dead_مبودي == DEAD_مبودي_OFF)
	أرجع;

    dead_مبودي = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_سكو.message;
    deadCharExpel.hwnd    = original_سكو.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_سكو.hwnd, original_سكو.message, original_سكو.wParam,
							  original_سكو.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ساكن فراغ
output_Dead_مبودي_re_Post(سكو_2 original_سكو)
{
    output_Dead_مبودي_re_Post_Ex(original_سكو, DEAD_مبودي_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ساكن فراغ process_message_usual_مبودي(UINT vk, ثابت سكو_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_مبودي(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ساكن فراغ
process_message_usual_مبودي_experimental(UINT vk, ثابت سكو_2 *pmsg)
{
    WCHAR	ch[8];
    صحيح		len;
    صحيح		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    إذا (Get_مبودي_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    إذا (Get_مبودي_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    إذا ((Get_مبودي_State(VK_RMENU) & 0x8000)
				 && (Get_مبودي_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_مبودي(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    إذا (len < 0)
	dead_مبودي = DEAD_مبودي_SET_DEFAULT;

    إذا (len <= 0)
    {
	صحيح wm_char = NUL;

	إذا (dead_مبودي == DEAD_مبودي_SET_DEFAULT
		&& (Get_مبودي_State(VK_CONTROL) & 0x8000))
	{
	    إذا (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    إذا (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	إذا (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_مبودي = DEAD_مبودي_TRANSIENT_IN_ON_CHAR;
	}
	أرجع;
    }

    // Post the message as TranslateMessage would do.
    إذا (pmsg->message == WM_KEYDOWN)
    {
	لكل (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    وإلا
    {
	لكل (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ساكن فراغ
process_message_usual_مبودي_classic(UINT vk, ثابت سكو_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    إذا (vk != 0xff
	    && (Get_مبودي_State(VK_CONTROL) & 0x8000)
	    && !(Get_مبودي_State(VK_SHIFT) & 0x8000)
	    && !(Get_مبودي_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	إذا (vk == '6' || Map_Virtual_مبودي(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_شغرو_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	وإلا إذا (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_شغرو_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	وإلا إذا (vk == '2' || Map_Virtual_مبودي(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_شغرو_buf(string, 1);
	}
	وإلا
	    TranslateMessage(pmsg);
    }
    وإلا
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ساكن فراغ
process_message(فراغ)
{
    سكو_2		سكو;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    صحيح		i;
    صحيح		modifiers = 0;
    صحيح		مبودي;
#ifdef FEAT_MENU
    ساكن char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    ساكن صحيح  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    إذا (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&سكو, عدم, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    إذا (سكو.message == WM_OLE)
    {
	char_u *str = (char_u *)سكو.lParam;
	إذا (str == عدم || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&سكو);
	}
	وإلا
	{
	    add_to_شغرو_buf(str, (صحيح)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	أرجع;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    إذا (s_findrep_hwnd != عدم && IsDialogMessageW(s_findrep_hwnd, &سكو))
    {
	HandleMouseHide(سكو.message, سكو.lParam);
	أرجع;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    إذا (سكو.message == WM_KEYDOWN || سكو.message == WM_SYSKEYDOWN)
    {
	vk = (صحيح) سكو.wParam;

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
	إذا (keycode_trans_strategy_used->is_experimental()
		&& dead_مبودي == DEAD_مبودي_SKIP_ON_CHAR)
	{
	    dead_مبودي = DEAD_مبودي_OFF;
	}

	إذا (dead_مبودي != DEAD_مبودي_OFF)
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
	    إذا (keycode_trans_strategy_used->is_experimental() &&
		    dead_مبودي == DEAD_مبودي_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_مبودي_re_Post_Ex(سكو,
				       /*dead_key2set=*/DEAD_مبودي_SKIP_ON_CHAR);
		أرجع;
	    }

	    إذا (dead_مبودي != DEAD_مبودي_SET_DEFAULT)
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
	    إذا ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_مبودي = DEAD_مبودي_OFF;
		TranslateMessage(&سكو);
		أرجع;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    وإلا إذا ((get_real_state()
			    & (MODE_سك | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_مبودي_re_Post(سكو);
		أرجع;
	    }
	}

	// Check for CTRL-BREAK
	إذا (vk == VK_CANCEL)
	{
	    trash_شغرو_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_شغرو_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	إذا (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&سكو);
	    أرجع;
	}

	لكل (i = 0; special_مبوداي[i].مبودي_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    إذا (special_مبوداي[i].مبودي_sym == vk
		    && (vk != VK_SPACE || !(Get_مبودي_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		إذا (dead_مبودي == DEAD_مبودي_SET_DEFAULT
			&& (special_مبوداي[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_مبودي_re_Post(سكو);
		    أرجع;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		إذا (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  عدم, عدم) == عدم)
		    اكسر;
#endif
		modifiers = get_active_modifiers_via_ptr();

		إذا (special_مبوداي[i].vim_code1 == NUL)
		    مبودي = special_مبوداي[i].vim_code0;
		وإلا
		    مبودي = TO_SPECIAL(special_مبوداي[i].vim_code0,
						   special_مبوداي[i].vim_code1);
		مبودي = simplify_مبودي(مبودي, &modifiers);
		إذا (مبودي == CSI)
		    مبودي = K_CSI;

		إذا (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_شغرو_buf(string, 3);
		}

		إذا (IS_SPECIAL(مبودي))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(مبودي);
		    string[2] = K_THIRD(مبودي);
		    add_to_شغرو_buf(string, 3);
		}
		وإلا
		{
		    صحيح	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(مبودي, string, 40, FALSE);
		    add_to_شغرو_buf(string, len);
		}
		اكسر;
	    }
	}

	// Not a special key.
	إذا (special_مبوداي[i].مبودي_sym == 0)
	{
	    process_message_usual_مبودي(vk, &سكو);
	}
    }
#ifdef FEAT_MBYTE_IME
    وإلا إذا (سكو.message == WM_IME_NOTIFY)
	_OnImeNotify(سكو.hwnd, (DWORD)سكو.wParam, (DWORD)سكو.lParam);
    وإلا إذا (سكو.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&سكو);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    إذا (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  عدم, عدم) == عدم)
#endif
	DispatchMessageW(&سكو);
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
    سكو_2	    سكو;

    إذا (!s_busy_processing)
	طالما (PeekMessageW(&سكو, عدم, 0, 0, PM_NOREMOVE)
						  && !vim_is_شغرو_buf_full())
	    process_message();
}

    ساكن فراغ
remove_any_timer(فراغ)
{
    سكو_2		سكو;

    إذا (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(عدم, s_wait_timer);

	// Eat spurious WM_TIMER messages
	طالما (PeekMessageW(&سكو, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ساكن garray_T	server_pending_ga = {0, 0, 0, 0, عدم};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    فراغ
server_add_شغرو(char_u *str)
{
    إذا (server_pending_ga.ga_data == عدم)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ساكن فراغ
server_flush_شغرو(فراغ)
{
    char_u  *str;

    إذا (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	أرجع;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    إذا (str != عدم)
    {
	server_to_شغرو_buf(str);
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
    صحيح
gui_mch_wait_for_chars(long wtime)
{
    صحيح		focus;

    s_timed_out = FALSE;

    إذا (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	إذا (s_busy_processing)
	    أرجع FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(عدم, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    طالما (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	إذا (gui.in_focus != focus)
	{
	    إذا (gui.in_focus)
		gui_mch_start_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	إذا (s_need_activate)
	{
	    (فراغ)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	لكل (;;)
	{
	    سكو_2 سكو;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_شغرو();
# endif
# ifdef FEAT_TIMERS
	    إذا (did_add_timer)
		اكسر;
# endif
	    إذا (PeekMessageW(&سكو, عدم, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		اكسر;
	    }
	    وإلا إذا (شغرو_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || سكو_Wait_For_Multiple_Objects(0, عدم, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		اكسر;
	}
#وإلا
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	إذا (شغرو_available())
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

#ifdef FEAT_TIMERS
	إذا (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    اكسر;
	}
#endif
    }
    allow_scrollbar = FALSE;
    أرجع FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_block(
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
    rc.هغو = FILL_X(col1);
    rc.كولي = FILL_Y(row1);
    rc.دام = FILL_X(col2 + 1) + (col2 == Columns - 1);
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

    rc.هغو = 0;
    rc.كولي = 0;
    rc.دام = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    فراغ
gui_mch_enable_menu(صحيح flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : عدم);
#endif
}

    فراغ
gui_mch_set_menu_pos(
    صحيح	    x UNUSED,
    صحيح	    y UNUSED,
    صحيح	    w UNUSED,
    صحيح	    h UNUSED)
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
#endif // FEAT_MENU

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
    ساكن WORD
PixelToDialogX(صحيح لمب_Pixels)
{
    أرجع (WORD)((لمب_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ساكن WORD
PixelToDialogY(صحيح لمب_Pixels)
{
    أرجع (WORD)((لمب_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ساكن صحيح
GetTextWidth(HDC hdc, char_u *str, صحيح len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    أرجع size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ساكن صحيح
GetTextWidthEnc(HDC hdc, char_u *str, صحيح len)
{
    SIZE	size;
    WCHAR	*wstr;
    صحيح		n;
    صحيح		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    إذا (wstr == عدم)
	أرجع 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    إذا (n)
	أرجع size.cx;
    أرجع 0;
}

ساكن فراغ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ساكن BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    صحيح		    wChild, hChild, wParent, hParent;
    صحيح		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.دام - rChild.هغو;
    hChild = rChild.bottom - rChild.كولي;

    // If Vim is minimized put the window in the middle of the screen.
    إذا (hwndParent == عدم || IsMinimized(hwndParent))
	get_work_area(&rParent);
    وإلا
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.دام - rParent.هغو;
    hParent = rParent.bottom - rParent.كولي;

    moninfo.cbSize = حجم(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    إذا (mon != عدم && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    وإلا
    {
	hdc = GetDC(hwndChild);
	rScreen.هغو = 0;
	rScreen.كولي = 0;
	rScreen.دام = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.هغو + ((wParent - wChild) / 2);
    إذا (xNew < rScreen.هغو)
	xNew = rScreen.هغو;
    وإلا إذا ((xNew + wChild) > rScreen.دام)
	xNew = rScreen.دام - wChild;

    yNew = rParent.كولي + ((hParent - hChild) / 2);
    إذا (yNew < rScreen.كولي)
	yNew = rScreen.كولي;
    وإلا إذا ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    أرجع SetWindowPos(hwndChild, عدم, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    إذا (s_toolbarhwnd == عدم)
	أرجع;

    إذا (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    وإلا
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_كدي 31

#endif

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, عدم);
    إذا (wn == عدم)
	أرجع;

    infow.cbSize = حجم(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    سك_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    ساكن فراغ
show_tabline_popup_menu(فراغ)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    إذا (hold_gui_events || cmdwin_type != 0)
	أرجع;

    tab_pmenu = CreatePopupMenu();
    إذا (tab_pmenu == عدم)
	أرجع;

    إذا (first_tabpage->tp_next != عدم)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
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

	إذا (ScreenToClient(s_tabhwnd, &pt) == 0)
	    أرجع;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
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
	    ShowWindow(s_tabhwnd, SW_SHOW);
	وإلا
	    ShowWindow(s_tabhwnd, SW_HIDE);
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
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = first_tabpage; tp != عدم; tp = tp->tp_next, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	إذا (nr >= Tab_Ctrl_Get_Item_كدي(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_سك_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, عدم);
	إذا (wstr != عدم)
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
    طالما (nr < Tab_Ctrl_Get_Item_كدي(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    إذا (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, عدم, عدم,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    إذا (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_set_curtab(صحيح nr)
{
    إذا (s_tabhwnd == عدم)
	أرجع;

    إذا (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    فراغ
gui_mch_set_dark_theme(صحيح dark)
{
    إذا (pDwmSetWindowAttribute != عدم && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		حجم(dark));

    إذا (pSetPreferredAppMode != عدم && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    إذا (pFlushMenuThemes != عدم && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ساكن فراغ
dyn_uxtheme_load(فراغ)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    إذا (hUxThemeLib == عدم)
	أرجع;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (فراغ (WINAPI *)(فراغ))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    إذا (pSetPreferredAppMode == عدم || pFlushMenuThemes == عدم)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = عدم;
	أرجع;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    فراغ
gui_mch_set_fullscreen(صحيح flag)
{
    ساكن RECT normal_rect;
    ساكن LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    إذا (!full_screen) // Windows not set yet.
	أرجع;

    إذا (flag)
    {
	إذا (fullscreen_on)
	    أرجع;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = حجم(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	إذا (mon == عدم || !GetMonitorInfo(mon, &moninfo))
	    أرجع;

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
	SetWindowPos(s_hwnd, عدم,
		moninfo.rcMonitor.هغو,
		moninfo.rcMonitor.كولي,
		moninfo.rcMonitor.دام - moninfo.rcMonitor.هغو,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.كولي,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    وإلا
    {
	إذا (!fullscreen_on)
	    أرجع;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, عدم,
		normal_rect.هغو,
		normal_rect.كولي,
		normal_rect.دام - normal_rect.هغو,
		normal_rect.bottom - normal_rect.كولي,
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
    char_u	*مبوداي = eap->arg;
    صحيح		fill_typebuf = FALSE;
    char_u	مبودي_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    طالما (*مبوداي)
    {
	إذا (*مبوداي == '~')
	    *مبوداي = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*مبوداي, (LPARAM)0);
	مبوداي++;
	fill_typebuf = TRUE;
    }
    إذا (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	مبودي_name[0] = K_SPECIAL;
	مبودي_name[1] = KS_EXTRA;
	مبودي_name[2] = KE_NOP;
	مبودي_name[3] = NUL;
#إذا defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(فراغ)ins_typebuf(مبودي_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    ساكن فراغ
initialise_findrep(char_u *initial_string)
{
    صحيح		wword = FALSE;
    صحيح		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    إذا (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    إذا (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    إذا (entry_text != عدم && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, عدم);
	إذا (p != عدم)
	{
	    صحيح len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    ساكن فراغ
set_window_title(HWND hwnd, حرف *title)
{
    إذا (title != عدم)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, عدم);
	إذا (wbuf != عدم)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    وإلا
	(فراغ)SetWindowTextW(hwnd, عدم);
}

    فراغ
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    إذا (s_findrep_سكو != 0)
    {
	إذا (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	إذا (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(فراغ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    إذا (s_findrep_سكو != 0)
    {
	إذا (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	إذا (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(فراغ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (hide == gui.pointer_hidden)
	أرجع;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
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
#endif

/*
 * Got a message when the system will go down.
 */
    ساكن فراغ
_OnEndSession(فراغ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ساكن فراغ
_OnClose(HWND hwnd UNUSED)
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

    out_flush();	    // make sure all output has been processed
    (فراغ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    إذا (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.هغو = rect.هغو;
	ps.rcPaint.دام = rect.دام;
    }

    إذا (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	إذا (!screen_cleared || Screen_ليوك == عدم)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	وإلا
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.هغو, ps.rcPaint.كولي,
		ps.rcPaint.دام - ps.rcPaint.هغو + 1,
		ps.rcPaint.bottom - ps.rcPaint.كولي + 1);
    }

    EndPaint(hwnd, &ps);
}

    ساكن فراغ
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    صحيح cx,
    صحيح cy)
{
    إذا (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    ساكن فراغ
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (فراغ)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
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
    (فراغ)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
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
    أرجع DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    RECT rct;
    POINT mp;

    (فراغ)GetWindowRect(s_textArea, &rct);
    (فراغ)GetCursorPos(&mp);
    *x = (صحيح)(mp.x - rct.هغو);
    *y = (صحيح)(mp.y - rct.كولي);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    RECT rct;

    (فراغ)GetWindowRect(s_textArea, &rct);
    (فراغ)SetCursorPos(x + gui.border_offset + rct.هغو,
		       y + gui.border_offset + rct.كولي);
}

    ساكن فراغ
gui_mswin_get_valid_dimensions(
    صحيح w,
    صحيح h,
    صحيح *valid_w,
    صحيح *valid_h,
    صحيح *cols,
    صحيح *rows)
{
    صحيح	    base_width, base_height;

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

    فراغ
gui_mch_flash(صحيح msec)
{
    RECT    rc;

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.هغو = 0;
    rc.كولي = 0;
    rc.دام = gui.لمب_cols * gui.char_width;
    rc.bottom = gui.لمب_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ساكن BOOL
is_point_onscreen(صحيح x, صحيح y)
{
    POINT   pt = {x, y};

    أرجع MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != عدم;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ساكن BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.هغو;
    p1.y = rc.كولي;
    p2.x = rc.دام - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    إذا (!is_point_onscreen(p1.x, p1.y))
	أرجع FALSE;
    إذا (!is_point_onscreen(p1.x, p2.y))
	أرجع FALSE;
    إذا (!is_point_onscreen(p2.x, p1.y))
	أرجع FALSE;
    إذا (!is_point_onscreen(p2.x, p2.y))
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
    إذا (!is_window_onscreen(s_hwnd))
	أرجع SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    لكل (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	إذا (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
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
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_ليوك(
    صحيح	    row,
    صحيح	    لمب_ليوك)
{
    RECT	rc;

    rc.هغو = FILL_X(gui.scroll_region_هغو);
    rc.دام = FILL_X(gui.scroll_region_دام + 1);
    rc.كولي = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -لمب_ليوك * gui.char_height, &rc);
    }
    وإلا
#endif
    {
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -لمب_ليوك * gui.char_height,
				    &rc, &rc, عدم, عدم, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - لمب_ليوك + 1,
						       gui.scroll_region_هغو,
	gui.scroll_region_bot, gui.scroll_region_دام);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_سك_ليوك(
    صحيح		row,
    صحيح		لمب_ليوك)
{
    RECT	rc;

    rc.هغو = FILL_X(gui.scroll_region_هغو);
    rc.دام = FILL_X(gui.scroll_region_دام + 1);
    rc.كولي = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, لمب_ليوك * gui.char_height, &rc);
    }
    وإلا
#endif
    {
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, لمب_ليوك * gui.char_height,
				    &rc, &rc, عدم, عدم, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_هغو,
				row + لمب_ليوك - 1, gui.scroll_region_دام);
}


    فراغ
gui_mch_exit(صحيح rc UNUSED)
{
#إذا defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = عدم;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (فراغ)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    إذا (s_hwnd != عدم)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    ساكن char_u *
logfont2name(LOGFONTW lf)
{
    حرف	*charset_name;
    حرف	*quality_name;
    حرف	*font_name;
    size_t	res_size;
    حرف	*res;

    font_name = (حرف *)utf16_to_enc(lf.lfFaceName, عدم);
    إذا (font_name == عدم)
	أرجع عدم;
    charset_name = charset_id2name((صحيح)lf.lfCharSet);
    quality_name = quality_id2name((صحيح)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == عدم ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == عدم ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    إذا (res != عدم)
    {
	حرف	*p;
	صحيح	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	لكل (p = font_name; *p != NUL; ++p)
	{
	    إذا (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	إذا (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_size, "%s:h%d", font_name, points);
	وإلا
	    res_len = vim_snprintf_safelen(
		(حرف *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (حرف *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	إذا (charset_name != عدم)
	    res_len += vim_snprintf_safelen((حرف *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	إذا (quality_name != عدم)
	    vim_snprintf((حرف *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    أرجع (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
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
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    فراغ
gui_mch_wide_font_changed(فراغ)
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

    إذا (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, حجم(lf), &lf))
    {
	إذا (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	إذا (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
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
gui_mch_init_font(char_u *font_name, صحيح fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    إذا (get_logfont(&lf, font_name, عدم, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    إذا (font == NOFONT)
	أرجع FAIL;

#إذا defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	صحيح		    feat_كدي = 0;
	char_u		    *fp;

	إذا (font_name != عدم)
	{
	    // Find each ":f" option in font_name.
	    لكل (fp = font_name; *fp != NUL; fp++)
	    {
		إذا (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    صحيح    ti = 0;
		    unsigned صحيح param = 1;

		    fp += 2;  // skip ":f"
		    طالما (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    إذا (ti != 4)
			استمر;  // invalid tag length

		    إذا (*fp == '=')
		    {
			fp++;
			param = (unsigned صحيح)atoi((حرف *)fp);
			طالما (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    إذا (feat_كدي < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_كدي].tag =
			    ((unsigned صحيح)tag[0])
			    | ((unsigned صحيح)tag[1] << 8)
			    | ((unsigned صحيح)tag[2] << 16)
			    | ((unsigned صحيح)tag[3] << 24);
			features[feat_كدي].parameter = param;
			feat_كدي++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_كدي);
    }
#endif

    إذا (font_name == عدم)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    إذا (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    إذا (p != عدم)
    {
	hl_set_font_name(p);

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
    إذا (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
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

    wp.length = حجم(WINDOWPLACEMENT);
    إذا (GetWindowPlacement(s_hwnd, &wp))
    {
	إذا (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    أرجع TRUE;
	إذا (wp.showCmd == SW_SHOWMINIMIZED)
	    أرجع FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	إذا ((rc.دام - rc.هغو !=
		    wp.rcNormalPosition.دام - wp.rcNormalPosition.هغو)
		|| (rc.bottom - rc.كولي !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.كولي))
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

    GetWindowRect(s_hwnd, &rect);
    إذا (win_socket_id == 0)
    {
	gui_resize_shell(rect.دام - rect.هغو
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.كولي
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    وإلا
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.دام - rect.هغو,
	    rect.bottom - rect.كولي - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    فراغ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == عدم ? "VIM" : (حرف *)title));
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
mch_set_mouse_shape(صحيح shape)
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
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(عدم, idc));
	إذا (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (فراغ)GetCursorPos(&mp);
	    (فراغ)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

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
    res = enc_to_utf16(tmp, &len);
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
	wp = enc_to_utf16(dflt, عدم);
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

    CLEAR_FIELD(fileStruct);
# ifdef OPENFILENAME_SIZE_VERSION_400W
    // be compatible with Windows NT 4.0
    fileStruct.lStructSize = OPENFILENAME_SIZE_VERSION_400W;
# وإلا
    fileStruct.lStructSize = حجم(fileStruct);
# endif

    إذا (title != عدم)
	titlep = enc_to_utf16(title, عدم);
    fileStruct.lpstrTitle = titlep;

    إذا (ext != عدم)
	extp = enc_to_utf16(ext, عدم);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    إذا (initdir != عدم && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, عدم);
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
    p = utf16_to_enc(fileBuf, عدم);
    إذا (p == عدم)
	أرجع عدم;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

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
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

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
#endif // FEAT_BROWSE

    ساكن فراغ
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    حرف    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, عدم, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    إذا (fnames != عدم)
	لكل (i = 0; i < cFiles; ++i)
	{
	    إذا (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, عدم);
	    وإلا
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    إذا (fnames == عدم)
	أرجع;

    صحيح kbd_modifiers = get_active_modifiers();

    إذا ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    إذا ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    إذا ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ساكن صحيح
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    صحيح pos)
{
    ساكن UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    صحيح		dragging = FALSE;
    صحيح		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = حجم(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
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
    val = sb_info->درج;

    بدل (code)
    {
	حالة SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    إذا (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    اكسر;
	حالة SB_LINEDOWN:
	    val++;
	    اكسر;
	حالة SB_LINEUP:
	    val--;
	    اكسر;
	حالة SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    اكسر;
	حالة SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    اكسر;
	حالة SB_كولي:
	    val = 0;
	    اكسر;
	حالة SB_BOTTOM:
	    val = sb_info->max;
	    اكسر;
	حالة SB_ENDSCROLL:
	    إذا (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		إذا (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    اكسر;

	افتراضي:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    أرجع 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    إذا (sb->wp != عدم)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_هغو) ? SBAR_دام : SBAR_هغو].id;

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

    أرجع 0;
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

ساكن BalloonEval  *cur_beval = عدم;
ساكن UINT_PTR	    beval_timer_id = 0;
ساكن DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
ساكن UINT	s_menu_id = 100;
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
#define DLG_ALLOC_SIZE 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
ساكن PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	ثابت حرف *caption);
ساكن LPWORD lpwAlign(LPWORD);
ساكن صحيح nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#إذا defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ساكن فراغ gui_mch_tearoff(char_u *title, vimmenu_T *menu, صحيح initX, صحيح initY);
#endif
ساكن فراغ get_dialog_font_metrics(فراغ);

ساكن صحيح dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
ساكن فراغ initialise_toolbar(فراغ);
ساكن فراغ update_toolbar_size(فراغ);
ساكن LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_سكو, WPARAM wParam, LPARAM lParam);
ساكن صحيح get_toolbar_bitmap(vimmenu_T *menu);
#وإلا
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
ساكن فراغ initialise_tabline(فراغ);
ساكن LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_سكو, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
ساكن LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ساكن char_u *GetResultStr(HWND hwnd, صحيح GCS, صحيح *lenp);
#endif
#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
تعريفنوع بنية tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
تعريفنوع HANDLE HIMC;
# endif

ساكن HINSTANCE hLibImm = عدم;
ساكن LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ساكن HIMC (WINAPI *pImmGetContext)(HWND);
ساكن HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ساكن BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ساكن BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
ساكن BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
ساكن BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ساكن BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
ساكن BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
ساكن BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
ساكن BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
ساكن فراغ dyn_imm_load(فراغ);
#وإلا
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
    ساكن صحيح
gui_mswin_get_menu_height(
    صحيح	    fix_window)	    // If TRUE, resize window if menu height changed
{
    ساكن صحيح	old_menu_height = -1;

    RECT    rc1, rc2;
    صحيح	    لمب;
    صحيح	    menu_height;

    إذا (gui.menu_is_active)
	لمب = Get_Menu_Item_كدي(s_menuBar);
    وإلا
	لمب = 0;

    إذا (لمب == 0)
	menu_height = 0;
    وإلا إذا (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
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
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	إذا (gui.starting)
	    menu_height = rc1.bottom - rc1.كولي + 1;
	وإلا
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, لمب - 1, &rc2);
	    menu_height = rc2.bottom - rc1.كولي + 1;
	}
    }

    إذا (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    أرجع menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ساكن long
mouse_vertical_scroll_step(فراغ)
{
    UINT val;
    إذا (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	أرجع (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    أرجع 3; // Safe default;
}

    ساكن long
mouse_horizontal_scroll_step(فراغ)
{
    UINT val;
    إذا (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	أرجع (long)val;
    أرجع 3; // Safe default;
}

    ساكن فراغ
init_mouse_wheel(فراغ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    ساكن فراغ
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, صحيح horizontal)
{
    صحيح		button;
    win_T	*wp;
    صحيح		modifiers = 0;
    صحيح		kbd_modifiers;
    صحيح		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    إذا (wp != عدم && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	إذا (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_هغو : MSCR_دام;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	وإلا
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

    إذا (wp == عدم || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    إذا (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    وإلا
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    إذا ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    إذا ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    إذا ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    ساكن صحيح
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = حجم(NONCLIENTMETRICSW);
    إذا (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    حجم(NONCLIENTMETRICSW),
	    &nm,
	    0))
	أرجع FAIL;
    *lf = nm.lfMenuFont;
    أرجع OK;
}
#endif


#إذا defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ساكن فراغ
set_tabline_font(فراغ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    إذا (gui_w32_get_menu_font(&lfSysmenu) != OK)
	أرجع;

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
#وإلا
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    ساكن LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    بدل (param)
    {
	حالة SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    اكسر;
	حالة SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    اكسر;
	حالة SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    اكسر;
	افتراضي:
	    اكسر;
    }
    أرجع 0;
}

#ifdef FEAT_NETBEANS_INTG
    ساكن فراغ
_OnWindowPosChanged(
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
	netbeans_frame_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, DefWindowProcW);
}
#endif


ساكن HWND hwndTip = عدم;

    ساكن فراغ
show_sizing_tip(صحيح cols, صحيح rows)
{
    TOOLINFOA ti;
    حرف buf[32];

    ti.cbSize = حجم(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    إذا (hwndTip == عدم)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, عدم,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, عدم, GetModuleHandle(عدم), عدم);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    وإلا
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    ساكن فراغ
destroy_sizing_tip(فراغ)
{
    إذا (hwndTip == عدم)
	أرجع;

    DestroyWindow(hwndTip);
    hwndTip = عدم;
}

    ساكن صحيح
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    صحيح	    w, h;
    صحيح	    valid_w, valid_h;
    صحيح	    w_offset, h_offset;
    صحيح	    cols, rows;

    w = lprc->دام - lprc->هغو;
    h = lprc->bottom - lprc->كولي;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    إذا (fwSide == WMSZ_هغو || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->هغو += w_offset;
    وإلا إذا (fwSide == WMSZ_دام || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->دام -= w_offset;

    إذا (fwSide == WMSZ_كولي || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->كولي += h_offset;
    وإلا إذا (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    أرجع TRUE;
}

#ifdef FEAT_GUI_TABLINE
    ساكن فراغ
_OnRButtonUp(HWND hwnd, صحيح x, صحيح y, UINT مبودي_Flags)
{
    إذا (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	إذا (pt.y < rect.كولي)
	{
	    show_tabline_popup_menu();
	    أرجع;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, مبودي_Flags, DefWindowProcW);
}

    ساكن فراغ
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, صحيح x, صحيح y, UINT مبودي_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    إذا (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	إذا (pt.y < rect.كولي)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, مبودي_Flags, DefWindowProcW);
}
#endif

    ساكن UINT
_OnNCHitTest(HWND hwnd, صحيح xPos, صحيح yPos)
{
    UINT	result;
    صحيح		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    إذا (result != HTCLIENT)
	أرجع result;

#ifdef FEAT_GUI_TABLINE
    إذا (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	إذا (yPos < rct.كولي)
	    أرجع result;
    }
#endif
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
    بدل (hdr->code)
    {
	حالة TTN_GETDISPINFOW:
	حالة TTN_GETDISPINFO:
	    {
		char_u		*str = عدم;
		ساكن فراغ	*tt_text = عدم;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		إذا (gui_mch_showing_tabline()
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
		    إذا (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			صحيح idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			إذا (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
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
		    pMenu = gui_mswin_find_menu(سيو_menu, idButton);
		    إذا (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		إذا (str == عدم)
		    اكسر;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		إذا (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, عدم);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		وإلا
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    إذا (len < حجم(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == عدم))
			vim_strncpy((char_u *)lpdi->szText, str,
				حجم(lpdi->szText) - 1);
		    وإلا
			lpdi->lpszText = tt_text;
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
    أرجع DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#إذا defined(MENUHINTS) && defined(FEAT_MENU)
    ساكن LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    إذا (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ساكن صحيح  did_menu_tip = FALSE;

	إذا (did_menu_tip)
	{
	    سكو_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(سيو_menu, (UINT)LOWORD(wParam));
	إذا (pMenu != عدم && pMenu->strings[MENU_INDEX_TIP] != عدم)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = حجم(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    إذا (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++سكو_hist_off;
		سكو((حرف *)pMenu->strings[MENU_INDEX_TIP]);
		--سكو_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	أرجع 0L;
    }
    أرجع DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    ساكن BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    صحيح		old_width, old_height;
    صحيح		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, حجم(lf), &lf);
    lf.lfHeight = lf.lfHeight * (صحيح)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    إذا (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    وإلا
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    أرجع TRUE;
}

    ساكن LRESULT
_OnDpiChanged(HWND hwnd, UINT xdpi UNUSED, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_in_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_size();
    update_toolbar_size();
    set_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_height(FALSE);
#ifdef FEAT_MBYTE_IME
    im_set_position(gui.row, gui.col);
#endif
    InvalidateRect(hwnd, عدم, TRUE);

    s_in_dpichanged = FALSE;
    أرجع 0L;
}


    ساكن LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_سكو,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_سكو, lParam);

    s_u_سكو = u_سكو;
    s_wParam = wParam;

    بدل (u_سكو)
    {
	HANDLE_سكو(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_سكو(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_سكو(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_سكو(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_سكو(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_سكو(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_سكو(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_سكو(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_سكو(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_سكو(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_سكو(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_سكو(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_سكو(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_سكو(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_سكو(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_سكو(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    حالة WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	أرجع FALSE;		// Do NOT allow system to go down.

    حالة WM_ENDSESSION:
	إذا (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    أرجع 0L;
	}
	اكسر;

    حالة WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (صحيح)(short)LOWORD(lParam));
	أرجع 0L;

    حالة WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	إذا (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((صحيح)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (صحيح)(short)LOWORD(lParam));
	    أرجع 0L;
	}
#ifdef FEAT_MENU
	وإلا
	    أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
#endif

    حالة WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
#وإلا
	أرجع 0L;
#endif

    حالة WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	اكسر;

    حالة WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	أرجع _DuringSizing((UINT)wParam, (LPRECT)lParam);

    حالة WM_MOUSEWHEEL:
    حالة WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_سكو == WM_MOUSEHWHEEL);
	أرجع 0L;

	// Notification for change in SystemParametersInfo()
    حالة WM_SETTINGCHANGE:
	أرجع _OnSettingChange((UINT)wParam);

#إذا defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    حالة WM_NOTIFY:
	أرجع _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#إذا defined(MENUHINTS) && defined(FEAT_MENU)
    حالة WM_MENUSELECT:
	أرجع _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    حالة WM_IME_NOTIFY:
	إذا (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
	أرجع 1L;

    حالة WM_IME_COMPOSITION:
	إذا (!_OnImeComposition(hwnd, wParam, lParam))
	    أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
	أرجع 1L;
#endif
    حالة WM_GETDPISCALEDSIZE:
	أرجع _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    حالة WM_DPICHANGED:
	أرجع _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    افتراضي:
#ifdef MSWIN_FIND_REPLACE
	إذا (u_سكو == s_findrep_سكو && s_findrep_سكو != 0)
	    _OnFindRepl();
#endif
	اكسر;
    }

    أرجع DefWindowProcW(hwnd, u_سكو, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = عدم;

    ساكن BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    حرف	buf[2048];
    حرف	*title = (حرف *)lParam;

    إذا (GetWindowText(hwnd, buf, حجم(buf)))
    {
	إذا (strstr(buf, title) != عدم)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, عدم, "MDIClient", عدم);
	    إذا (vim_parent_hwnd != عدم)
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
gui_mch_set_parent(حرف *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    إذا (vim_parent_hwnd == عدم)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    ساكن فراغ
ole_error(حرف *arg)
{
    حرف buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#إذا defined(GUI_MAY_SPAWN)
    ساكن حرف *
gvim_error(فراغ)
{
    حرف *سكو = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    إذا (starting)
    {
	mch_errmsg(سكو);
	mch_errmsg("\n");
	mch_exit(2);
    }
    أرجع سكو;
}

    حرف *
gui_mch_do_spawn(char_u *arg)
{
    صحيح			len;
# إذا defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = عدم;
    LPWSTR		tofree1 = عدم;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = عدم, p, warg, tofree2 = عدم;
    STARTUPINFOW	si = {حجم(si)};
    PROCESS_INFORMATION pi;

    إذا (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
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
	p = Get_Command_لي_W();
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
	wsession = enc_to_utf16(session, عدم);
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
	warg = enc_to_utf16(arg, عدم);
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
#endif

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
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#وإلا
	    إذا (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	إذا (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#وإلا
	    إذا (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	إذا (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#وإلا
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
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
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	صحيح arg;

	لكل (arg = 1; arg < *argc; arg++)
	    إذا (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * حجم(حرف *));
		argv[*argc] = عدم;
		اكسر;	// enough?
	    }
    }
#endif
}

    ساكن فراغ
load_dpi_func(فراغ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    إذا (hUser32 == عدم)
	اذهب fail;

    pGetDpiForSystem = (UINT (WINAPI *)(فراغ))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (صحيح (WINAPI *)(صحيح, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    إذا (pSetThreadDpiAwarenessContext != عدم)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	إذا (oldctx != عدم)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    أرجع;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = عدم;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = عدم;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = عدم;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحيح
gui_mch_init(فراغ)
{
    ثابت WCHAR szVimWndClassW[] = VIM_CLASSW;
    ثابت WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    إذا (s_hwnd != عدم)
	اذهب theend;

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
    update_scrollbar_size();

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
    إذا (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(عدم, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = عدم;
	wndclassw.lpszClassName = szVimWndClassW;

	إذا (RegisterClassW(&wndclassw) == 0)
	    أرجع FAIL;
    }

    إذا (vim_parent_hwnd != عدم)
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
		vim_parent_hwnd, عدم,
		g_hinst, عدم);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	إذا (s_hwnd == عدم)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    وإلا
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	إذا (IsWindow((HWND)win_socket_id) <= 0)
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
		عدم, عدم,
		g_hinst, عدم);
	إذا (s_hwnd != عدم && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    إذا (s_hwnd == عدم)
	أرجع FAIL;

    إذا (pGetDpiForWindow != عدم)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    إذا (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = عدم;
	wndclassw.hCursor = LoadCursor(عدم, IDC_ARROW);
	wndclassw.hbrBackground = عدم;
	wndclassw.lpszMenuName = عدم;
	wndclassw.lpszClassName = szTextAreaClassW;

	إذا (RegisterClassW(&wndclassw) == 0)
	    أرجع FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, عدم,
	g_hinst, عدم);

    إذا (s_textArea == عدم)
	أرجع FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = عدم;

	إذا (mch_icon_load(&hIcon) == OK && hIcon != عدم)
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
    s_findrep_سكو = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = حجم(s_findrep_struct);
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
    إذا (p_rop)
	(فراغ)gui_mch_set_rendering_options(p_rop);
#endif

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
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    إذا (mon != عدم)
    {
	moninfo.cbSize = حجم(MONITORINFO);
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
gui_mch_set_shellsize(
	صحيح width,
	صحيح height,
	صحيح min_width UNUSED,
	صحيح min_height UNUSED,
	صحيح base_width UNUSED,
	صحيح base_height UNUSED,
	صحيح direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    صحيح		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    إذا (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    إذا (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    وإلا
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
    window_rect.دام = window_rect.هغو + win_width;
    window_rect.bottom = window_rect.كولي + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    إذا (!s_in_dpichanged)
    {
	إذا ((direction & RESIZE_HOR)
		&& window_rect.دام > workarea_rect.دام)
	    OffsetRect(&window_rect,
		    workarea_rect.دام - window_rect.دام, 0);

	إذا ((direction & RESIZE_HOR)
		&& window_rect.هغو < workarea_rect.هغو)
	    OffsetRect(&window_rect,
		    workarea_rect.هغو - window_rect.هغو, 0);

	إذا ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	إذا ((direction & RESIZE_VERT)
		&& window_rect.كولي < workarea_rect.كولي)
	    OffsetRect(&window_rect,
		    0, workarea_rect.كولي - window_rect.كولي);
    }

    MoveWindow(s_hwnd, window_rect.هغو, window_rect.كولي,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    فراغ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    طالما (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    إذا (sb->scroll_shift > 0)
	++size;

    info.cbSize = حجم(info);
    info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.nPage = size;
    SetScrollInfo(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    فراغ
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    فراغ
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
    ساكن LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    إذا (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	أرجع lResult;
    بدل (dwCommand)
    {
	حالة IMN_SETOPENSTATUS:
	    إذا (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		إذا (State & MODE_سك)
		{
# إذا defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    إذا (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			صحيح old_row = gui.row;
			صحيح old_col = gui.col;

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
	    lResult = 0;
	    اكسر;
    }
    pImmReleaseContext(hWnd, hImc);
    أرجع lResult;
}

    ساكن LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    صحيح		len;

    إذا ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	أرجع 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    إذا (ret != عدم)
    {
	add_to_شغرو_buf_csi(ret, len);
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
GetResultStr(HWND hwnd, صحيح GCS, صحيح *lenp)
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

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    أرجع convbuf;
}
#endif

// For global functions we need prototypes.
#إذا defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    فراغ
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    فراغ
im_set_position(صحيح row, صحيح col)
{
    HIMC hImc;

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	إذا (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    فراغ
im_set_active(صحيح active)
{
    HIMC	hImc;
    ساكن HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    إذا (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
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
	active = FALSE;
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

	إذا (active)
	{
	    // if we have a saved conversion status, restore it
	    إذا (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	وإلا
	{
	    // save conversion status and disable korean
	    إذا (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    صحيح
im_get_status(فراغ)
{
    صحيح		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    إذا (!gui.in_use && !gui.starting)
	أرجع mbyte_im_get_status();
# endif

    إذا (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    أرجع status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ساكن فراغ
latin9_to_ucs(char_u *text, صحيح len, WCHAR *unicodebuf)
{
    صحيح		c;

    طالما (--len >= 0)
    {
	c = *text++;
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
    ساكن فراغ
RevOut( HDC hdc,
	صحيح col,
	صحيح row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    صحيح		ix;

    لكل (ix = 0; ix < (صحيح)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    ساكن فراغ
draw_لي(
    صحيح		x1,
    صحيح		y1,
    صحيح		x2,
    صحيح		y2,
    COLORREF	color)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_لي(s_dwc, x1, y1, x2, y2, color);
    وإلا
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, عدم);
	// Note: LineTo() excludes the last pixel in the line.
	لي_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ساكن فراغ
set_pixel(
    صحيح		x,
    صحيح		y,
    COLORREF	color)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    وإلا
#endif
	SetPixel(s_hdc, x, y, color);
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
#endif
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
    صحيح		row,
    صحيح		col,
    char_u	*text,
    صحيح		len,
    صحيح		flags)
{
    ساكن صحيح	*padding = عدم;
    ساكن صحيح	pad_size = 0;
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
	rc.هغو = FILL_X(col);
	rc.كولي = FILL_Y(row);
	إذا (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.دام = FILL_X(col + mb_string2cells(text, len));
	}
	وإلا
	    rc.دام = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

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

	SetBkMode(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	إذا (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	وإلا إذا (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    إذا (pad_size != Columns || padding == عدم || padding[0] != gui.char_width)
    {
	صحيح	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(صحيح, pad_size);
	إذا (padding != عدم)
	    لكل (i = 0; i < pad_size; i++)
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
    إذا (enc_utf8)
	لكل (n = 0; n < len; ++n)
	    إذا (text[n] >= 0x80)
		اكسر;

#إذا defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    إذا (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

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
	    c = utf_ptr2char(text + i);
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
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		وإلا
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#إذا defined(FEAT_DIRECTX)
	إذا (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	وإلا
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
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
		latin9_to_ucs(text, len, unicodebuf);
	    وإلا
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(حرف *)text, len,
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
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    وإلا
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	إذا (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (حرف *)text, len, padding);
	وإلا
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (حرف *)text, len, padding);
    }

    // Underline
    إذا (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_لي(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    إذا (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_لي(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    إذا (flags & DRAW_UNDERC)
    {
	صحيح			x;
	صحيح			offset;
	ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	لكل (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    فراغ
gui_mch_flush(فراغ)
{
#إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    ساكن فراغ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, عدم, gui.back_pixel);
}


    فراغ
gui_mch_get_screen_dimensions(صحيح *screen_w, صحيح *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.دام - workarea_rect.هغو
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.كولي
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#إذا defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    فراغ
gui_mch_add_menu(
    vimmenu_T	*menu,
    صحيح		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    إذا (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, عدم);
	إذا (wn == عدم)
	    أرجع;

	infow.cbSize = حجم(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	سك_Menu_Item_W((parent == عدم)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    إذا (parent == عدم)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    وإلا إذا (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (فراغ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (صحيح)mp.x, (صحيح)mp.y);
}

    فراغ
gui_make_popup(char_u *path_name, صحيح mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    إذا (menu == عدم)
	أرجع;

    POINT	p;

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
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    سكو_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (صحيح)p.x, (صحيح)p.y);
}

# إذا defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    فراغ
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    إذا (menu != عدم)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    فراغ
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    صحيح		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = عدم;

# ifdef FEAT_TEAROFF
    إذا (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	سك_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    وإلا
# endif
# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
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
	SendMessage(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    وإلا
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, عدم);
	إذا (wn != عدم)
	{
	    سك_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	إذا (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
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

	iButton = (صحيح)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    وإلا
# endif
    {
	إذا (menu->parent != عدم
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != عدم)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	وإلا
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	إذا (menu->submenu_id != عدم)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	إذا (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	إذا (menu->parent != عدم
		&& menu->parent->children != عدم
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

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    إذا (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.هغو - rct.هغو;
	y = (trect.كولي -  rct.bottom  + roct.bottom);
    }
    وإلا
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    إذا (menu->children != عدم)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	إذا (IsWindow(menu->tearoff_handle))
	    (فراغ) SetWindowPos(menu->tearoff_handle,
				عدم,
				(صحيح)trect.هغو,
				(صحيح)trect.كولي,
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
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    وإلا
# endif
	(فراغ)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    إذا ((menu->parent != عدم) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	إذا (menu->children == عدم)
	    menuID = (WORD)(menu->id);
	وإلا
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	إذا (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

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
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    إذا (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(فراغ)SetFocus(hwnd);
	إذا (dialog_default_button > IDCANCEL)
	    (فراغ)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	وإلا
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (فراغ)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	أرجع FALSE;
    }

    إذا (message == WM_COMMAND)
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

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, عدم);
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
		EndDialog(hwnd, dialog_default_button);
	}
	وإلا
	    EndDialog(hwnd, button - IDCANCEL);
	أرجع TRUE;
    }

    إذا ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
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
    char_u	*message,
    char_u	*buttons,
    صحيح		 dfltbutton,
    char_u	*textfield,
    صحيح		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    صحيح		لمب_Buttons;
    صحيح		*buttonWidths, *buttonPositions;
    صحيح		buttonYpos;
    صحيح		nchar, i;
    DWORD	lStyle;
    صحيح		dlgwidth = 0;
    صحيح		dlgheight;
    صحيح		editboxheight;
    صحيح		horizWidth = 0;
    صحيح		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    صحيح		fontHeight;
    صحيح		textWidth, minButtonWidth, messageWidth;
    صحيح		maxDialogWidth;
    صحيح		maxDialogHeight;
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
    صحيح		dlg_icon_width;
    صحيح		dlg_icon_height;
    صحيح		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    إذا (!(gui.in_use || gui.starting))
#  endif
	إذا (silent_mode)
	    أرجع dfltbutton;   // return default option
# endif

    إذا (s_hwnd == عدم)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    وإلا
	dpi = pGetDpiForSystem();

    إذا ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

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
    لمب_Buttons = 1;
    لكل (i = 0; tbuffer[i] != '\0'; i++)
    {
	إذا (tbuffer[i] == DLG_BUTTON_SEP)
	    لمب_Buttons++;
    }
    إذا (dfltbutton >= لمب_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(صحيح, لمب_Buttons);
    إذا (buttonWidths == عدم)
	أرجع -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(صحيح, لمب_Buttons);
    إذا (buttonPositions == عدم)
	أرجع -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وإلا
# endif
	font = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    GetTextMetrics(hdc, &fontInfo);
    fontHeight = fontInfo.tmHeight;

    // Minimum width for horizontal button
    minButtonWidth = GetTextWidth(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    إذا (s_hwnd == عدم)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.دام - workarea_rect.هغو - 100;
	إذا (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.كولي - 150;
    }
    وإلا
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.دام - rect.هغو
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	إذا (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.كولي
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	إذا (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, حجم(حرف), 500);
    افعل
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = عدم;
	لكل (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    إذا (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    إذا (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		إذا (last_white != عدم)
		{
		    // break the line just after a space
		    إذا (pend > last_white)
			ga.ga_len -= (صحيح)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = عدم;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		استمر;
	    }

	    طالما (--l >= 0)
		ga_append(&ga, *pend++);
	}
	إذا (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } طالما (*pend != NUL);

    إذا (ga.ga_data != عدم)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    إذا (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != عدم && vim_strchr(p_go, GO_VERTICAL) != عدم);
    إذا (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	افعل
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    إذا (pend == عدم)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (صحيح)(pend - pstart));
	    إذا (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} طالما (*pend != NUL);

	إذا (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	وإلا إذا (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
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
	    textWidth = GetTextWidthEnc(hdc, pstart, (صحيح)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    إذا (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} طالما (*pend != NUL);
    }

    إذا (dlgwidth < DLG_MIN_WIDTH)
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
    إذا (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * لمب_Buttons;
    وإلا
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    إذا (textfield != عدم)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    إذا (dlgheight > maxDialogHeight)
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
    إذا (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحيح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وإلا
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    إذا (textfield != عدم)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    إذا (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    لكل (i = 0; i < لمب_Buttons; i++)
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
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (حرف *)pstart);
	}
	وإلا
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (حرف *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += لمب_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_هغو|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (حرف *)message);

    // Edit box
    إذا (textfield != عدم)
    {
	p = add_dialog_element(p, ES_هغو|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (حرف *)textfield);
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
    nchar = (صحيح)DialogBoxIndirect(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_data);

    // Focus back to our window (for when MDI is used).
    (فراغ)SetFocus(s_hwnd);

    أرجع nchar;
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
    ساكن PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    ثابت حرف *caption)
{
    صحيح nchar;

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

    أرجع p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ساكن LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
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
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    صحيح		nChar = 0;
    صحيح		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    صحيح		i;
    WCHAR	*wn;

    إذا (use_enc && enc_codepage >= 0 && (صحيح)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, عدم);
	إذا (wn != عدم)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (صحيح)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    إذا (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    لكل (i = 0; i < nChar; ++i)
	إذا (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    أرجع nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ساكن HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    لكل ( ; menu != عدم; menu = menu->next)
    {
	إذا (menu->modes == 0)	// this menu has just been deleted
	    استمر;
	إذا (menu_is_separator(menu->dname))
	    استمر;
	إذا ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
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
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    إذا (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	أرجع (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    إذا (message == WM_COMMAND)
    {
	إذا ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    إذا (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(فراغ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (صحيح)rect.دام - 8,
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
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(فراغ)SetFocus(s_hwnd);
	أرجع TRUE;
    }
    إذا ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	أرجع TRUE;
    }

    // When moved around, give main window the focus back.
    إذا (message == WM_EXITSIZEMOVE)
	(فراغ)SetActiveWindow(s_hwnd);

    أرجع FALSE;
}
#endif


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
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    وإلا
#endif
	hfontTools = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    GetAverageFontSize(hdc, &size);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (WORD)size.cx;
    s_dlgfntheight = (WORD)size.cy;
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
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    صحيح		template_len;
    صحيح		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*كولي_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    صحيح		col, spaceWidth, len;
    صحيح		columnWidths[2];
    char_u	*label, *text;
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
    إذا (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	إذا (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, عدم, mp.x, mp.y, 0, 0,
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
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    إذا (p == عدم)
	أرجع;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    إذا (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وإلا
# endif
	font = CreateFont(-DLG_FONT_POINT_SIZE, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    spaceWidth = GetTextWidth(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenuWidth = 0;
    لكل (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    إذا (text != عدم && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (صحيح)STRLEN(text));
		إذا (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    إذا (pmenu->children != عدم)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    إذا (columnWidths[1] == 0)
    {
	// no accelerators
	إذا (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	وإلا
	    columnWidths[0] += spaceWidth;
    }
    وإلا
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    إذا (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (صحيح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (صحيح)STRLEN(title));
    إذا (textWidth > dlgwidth)
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
    إذا (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    وإلا
	*p++ = PixelToDialogX(initX); // x
    إذا (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    وإلا
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
    إذا (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحيح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وإلا
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    إذا (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    وإلا
	menu = menu->children;
    كولي_menu = menu;
    لكل ( ; menu != عدم; menu = menu->next)
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
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
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
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (صحيح)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	إذا (menu->actext != عدم)
	{
	    acLen = (صحيح)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	وإلا
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	إذا (menu->children == عدم)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	وإلا
	{
	    len += (صحيح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	إذا (label == عدم)
	    اكسر;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	إذا (text == عدم)
	    text = label + nameLen;	    // no actext, use whole name
	طالما (padding0-- > 0)
	    *text++ = ' ';
	إذا (menu->actext != عدم)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	طالما (padding1-- > 0)
	    *text++ = ' ';
	إذا (menu->children != عدم)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	وإلا
	{
	    طالما (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_هغو,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (حرف *)label);
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
	    (LPARAM)كولي_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (فراغ)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

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
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    عدم,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    حجم(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != عدم);

    update_toolbar_size();
}

    ساكن فراغ
update_toolbar_size(فراغ)
{
    صحيح		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = حجم(TBMETRICS);
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

    ساكن LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_سكو,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_سكو, lParam);
    أرجع CallWindowProc(s_toolbar_wndproc, hwnd, u_سكو, wParam, lParam);
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
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			عدم,
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
	إذا (hbitmap == عدم
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != عدم ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    عدم,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	إذا (hbitmap != عدم)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = عدم;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (صحيح)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    إذا (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_كدي)
	i = menu->iconidx;

    أرجع i;
}
#endif

#إذا defined(FEAT_GUI_TABLINE)
    ساكن فراغ
initialise_tabline(فراغ)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, عدم, g_hinst, عدم);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ساكن tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = عدم;

    إذا (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	إذا (s_tabhwnd == hWnd)
	{
	    صحيح idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    إذا (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    أرجع ptp;
}

ساكن POINT	    s_pt = {0, 0};
ساكن HCURSOR      s_hCursor = عدم;

    ساكن LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_سكو,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    صحيح		nCenter;
    صحيح		idx0;
    صحيح		idx1;

    HandleMouseHide(u_سكو, lParam);

    بدل (u_سكو)
    {
	حالة WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		اكسر;
	    }
	حالة WM_MOUSEMOVE:
	    إذا (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		إذا (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(عدم, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    إذا (tp != عدم)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.هغو + (rect.دام - rect.هغو) / 2;

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
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		اكسر;
	    }
	حالة WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
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

    أرجع CallWindowProc(s_tabline_wndproc, hwnd, u_سكو, wParam, lParam);
}
#endif

#إذا defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    فراغ
gui_mch_set_foreground(فراغ)
{
    إذا (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#إذا defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ساكن فراغ
dyn_imm_load(فراغ)
{
    hLibImm = vimLoadLib("imm32.dll");
    إذا (hLibImm == عدم)
	أرجع;

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

    إذا (       pImmGetCompositionStringW == عدم
	    || pImmGetContext == عدم
	    || pImmAssociateContext == عدم
	    || pImmReleaseContext == عدم
	    || pImmGetOpenStatus == عدم
	    || pImmSetOpenStatus == عدم
	    || pImmGetCompositionFontW == عدم
	    || pImmSetCompositionFontW == عدم
	    || pImmSetCompositionWindow == عدم
	    || pImmGetConversionStatus == عدم
	    || pImmSetConversionStatus == عدم)
    {
	FreeLibrary(hLibImm);
	hLibImm = عدم;
	pImmGetContext = عدم;
	أرجع;
    }

    أرجع;
}

#endif

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
gui_mch_drawsign(صحيح row, صحيح col, صحيح typenr)
{
    signicon_t *sign;
    صحيح x, y, w, h;

    إذا (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == عدم)
	أرجع;

# إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    بدل (sign->uType)
    {
	حالة IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    اكسر;
	حالة IMAGE_ICON:
	حالة IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, عدم, DI_NORMAL);
	    اكسر;
# ifdef FEAT_XPM_W32
	حالة IMAGE_XPM:
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
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = عدم;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    إذا (ext > signfile)
    {
	صحيح do_load = 1;

	إذا (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	وإلا إذا (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	وإلا إذا (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	وإلا
	    do_load = 0;

	إذا (do_load)
	    sign.hImage = (HANDLE)LoadImage(عدم, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	إذا (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((حرف *)signfile, (HBITMAP *)&sign.hImage,
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
	emsg(_(e_couldnt_read_in_sign_data));
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
#endif

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
make_tooltip(BalloonEval *beval, حرف *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    إذا (pti == عدم)
	أرجع;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    عدم, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, عدم, g_hinst, عدم);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = حجم(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, عدم);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    إذا (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.دام);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.هغو = pt.x - 3;
    pti->rect.كولي = pt.y - 3;
    pti->rect.دام = pt.x + 3;
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

    ساكن فراغ
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    ساكن VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_سكو UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    إذا (cur_beval == عدم || cur_beval->showState == ShS_SHOWING || !p_beval)
	أرجع;

    GetCursorPos(&pt);
    إذا (WindowFromPoint(pt) != s_textArea)
	أرجع;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    إذا (!PtInRect(&rect, pt))
	أرجع;

    إذا (last_user_activity > 0
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

	إذا (cur_beval->سكو_CB != عدم)
	    (*cur_beval->سكو_CB)(cur_beval, 0);
    }
}

    فراغ
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    فراغ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    إذا (beval == عدم)
	أرجع;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    فراغ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->سكو);
    beval->سكو = mesg == عدم ? عدم : vim_strsave(mesg);
    إذا (beval->سكو == عدم)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	أرجع;
    }

    إذا (beval->showState == ShS_SHOWING)
	أرجع;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    إذا (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (حرف *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    فراغ	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    فراغ	(*mesgCB)(BalloonEval *, صحيح),
    فراغ	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    إذا (mesg != عدم && mesgCB != عدم)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	أرجع عدم;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    إذا (beval != عدم)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->سكو = mesg;
	beval->سكو_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	إذا (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    أرجع beval;
}

    ساكن فراغ
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    إذا (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	أرجع;

    إذا (cur_beval == عدم)
	أرجع;

    بدل (pnmh->code)
    {
	حالة TTN_SHOW:
	    اكسر;
	حالة TTN_چرو: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    اكسر;
	حالة TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    اكسر;
	حالة TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    اكسر;
    }
}

    ساكن فراغ
track_user_activity(UINT u_سكو)
{
    إذا ((u_سكو >= WM_MOUSEFIRST && u_سكو <= WM_MOUSELAST)
	    || (u_سكو >= WM_KEYFIRST && u_سكو <= WM_KEYLAST))
	last_user_activity = Get_Tick_كدي();
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
#endif // FEAT_BEVAL_GUI

#إذا defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    فراغ
netbeans_draw_multisign_indicator(صحيح row)
{
    صحيح i;
    صحيح y;
    صحيح x;

    إذا (!netbeans_active())
	أرجع;

    x = 0;
    y = TEXT_Y(row);

# إذا defined(FEAT_DIRECTX)
    إذا (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    لكل (i = 0; i < gui.char_height - 3; i++)
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

#إذا defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ساكن صحيح
test_gui_w32_sendevent_mouse(dict_T *حجوجي)
{
    إذا (!dict_has_مبودي(حجوجي, "row") || !dict_has_مبودي(حجوجي, "col"))
	أرجع FALSE;

    // Note: "move" is optional, requires fewer arguments
    صحيح move = (صحيح)dict_get_bool(حجوجي, "move", FALSE);

    إذا (!move && (!dict_has_مبودي(حجوجي, "button")
	    || !dict_has_مبودي(حجوجي, "multiclick")
	    || !dict_has_مبودي(حجوجي, "modifiers")))
	أرجع FALSE;

    صحيح row = (صحيح)dict_get_لمب(حجوجي, "row");
    صحيح col = (صحيح)dict_get_لمب(حجوجي, "col");

    إذا (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	إذا (dict_get_bool(حجوجي, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    صحيح pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    صحيح pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	وإلا
	    gui_mouse_moved(col, row);
    }
    وإلا
    {
	صحيح button = (صحيح)dict_get_لمب(حجوجي, "button");
	صحيح repeated_click = (صحيح)dict_get_لمب(حجوجي, "multiclick");
	int_u mods = (صحيح)dict_get_لمب(حجوجي, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    أرجع TRUE;
}

    ساكن صحيح
test_gui_w32_sendevent_keyboard(dict_T *حجوجي)
{
    شغرو_2 inputs[1];
    شغرو_2 modkeys[3];
    SecureZeroMemory(inputs, حجم(شغرو_2));
    SecureZeroMemory(modkeys, 3 * حجم(شغرو_2));

    char_u *event = dict_get_string(حجوجي, "event", TRUE);

    إذا (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_لمب_def(حجوجي, "keycode", 0);
	إذا (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    أرجع FALSE;
	}

	BOOL is_Mod_مبودي = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	صحيح mods = (صحيح)dict_get_لمب(حجوجي, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	إذا (mods && STRICMP(event, "keydown") == 0 && !is_Mod_مبودي)
	{
	    صحيح n = 0;
	    إذا (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = شغرو_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    إذا (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = شغرو_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    إذا (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = شغرو_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    إذا (n)
	    {
		(فراغ)SetForegroundWindow(s_hwnd);
		Send_شغرو(n, modkeys, حجم(شغرو_2));
	    }
	}

	inputs[0].type = شغرو_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	إذا (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    إذا (!is_Mod_مبودي)
		unwrapMods = TRUE;
	}

	(فراغ)SetForegroundWindow(s_hwnd);
	Send_شغرو(ARRAYSIZE(inputs), inputs, حجم(شغرو_2));
	vim_free(event);

	إذا (unwrapMods)
	{
	    modkeys[0].type = شغرو_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = شغرو_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = شغرو_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (فراغ)SetForegroundWindow(s_hwnd);
	    Send_شغرو(3, modkeys, حجم(شغرو_2));
	}
    }
    وإلا
    {
	إذا (event == عدم)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	وإلا
	{
	    semsg(_(e_invalid_درج_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}

    ساكن صحيح
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *حجوجي)
{
    صحيح handled = 0;
    char_u *strategy = dict_get_string(حجوجي, "strategy", TRUE);

    إذا (strategy)
    {
	إذا (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	وإلا إذا (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    إذا (!handled)
    {
	إذا (strategy == عدم)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	وإلا
	{
	    semsg(_(e_invalid_درج_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	أرجع FALSE;
    }
    أرجع TRUE;
}


    صحيح
test_gui_w32_sendevent(char_u *event, dict_T *حجوجي)
{
    إذا (STRICMP(event, "key") == 0)
	أرجع test_gui_w32_sendevent_keyboard(حجوجي);
    وإلا إذا (STRICMP(event, "mouse") == 0)
	أرجع test_gui_w32_sendevent_mouse(حجوجي);
    وإلا إذا (STRICMP(event, "set_keycode_trans_strategy") == 0)
	أرجع test_gui_w32_sendevent_set_keycode_trans_strategy(حجوجي);
    وإلا
    {
	semsg(_(e_invalid_درج_for_argument_str_str), "event", event);
	أرجع FALSE;
    }
}
#endif

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
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
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
    فراغ	*bits = عدم;

    إذا (ctx == عدم)
	أرجع FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = حجم(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_كدي = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, عدم, 0);
    إذا (hbm == عدم || bits == عدم)
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
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    أرجع OK;
}

    فراغ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    صحيح
image_placement_gui_init(image_placement_T *place UNUSED)
{
    أرجع OK;
}

    فراغ
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    فراغ
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    صحيح		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
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
	صحيح		row, col;
	صحيح		posx, posy;
	صحيح		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    فراغ
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
