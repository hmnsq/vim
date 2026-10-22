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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_ચાવી_OFF			0	// no dead key
#define DEAD_ચાવી_SET_DEFAULT		1	// dead key pressed
#define DEAD_ચાવી_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_ચાવી_SKIP_ON_CHAR		3	// skip next _OnChar()

#જો defined(FEAT_DIRECTX)
સ્થિર DWriteContext *s_dwc = ખાલી;
સ્થિર પૂર્ણાંક s_directx_enabled = 0;
સ્થિર પૂર્ણાંક s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ખાલી && enc_utf8)
સ્થિર પૂર્ણાંક directx_enabled(શૂન્ય);
સ્થિર શૂન્ય directx_binddc(શૂન્ય);
#endif

#ifdef FEAT_MENU
સ્થિર પૂર્ણાંક gui_mswin_get_menu_height(પૂર્ણાંક fix_window);
#નહીંતર
# define gui_mswin_get_menu_height(fix_window)	0
#endif

પ્રકારવ્યાખ્યા સંરચના keycode_trans_strategy {
    શૂન્ય (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
    શૂન્ય (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
    શૂન્ય (*ptr_process_message_usual_ચાવી) (UINT /*vk*/, અચલ સંદેશ_2* /*pmsg*/);
    પૂર્ણાંક  (*ptr_get_active_modifiers)(શૂન્ય);
    પૂર્ણાંક  (*is_experimental)(શૂન્ય);
} keycode_trans_strategy;

// forward declarations for input instance initializer
સ્થિર શૂન્ય _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
સ્થિર શૂન્ય _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
સ્થિર શૂન્ય process_message_usual_ચાવી_experimental(UINT /*vk*/, અચલ સંદેશ_2* /*pmsg*/);
સ્થિર પૂર્ણાંક  get_active_modifiers_experimental(શૂન્ય);
સ્થિર પૂર્ણાંક  is_experimental_true(શૂન્ય);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_ચાવી_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
સ્થિર શૂન્ય _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
સ્થિર શૂન્ય _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, પૂર્ણાંક /*cRepeat UNUSED*/);
સ્થિર શૂન્ય process_message_usual_ચાવી_classic(UINT /*vk*/, અચલ સંદેશ_2* /*pmsg*/);
સ્થિર પૂર્ણાંક  get_active_modifiers_classic(શૂન્ય);
સ્થિર પૂર્ણાંક  is_experimental_false(શૂન્ય);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_ચાવી_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = ખાલી;

સ્થિર પૂર્ણાંક is_experimental_true(શૂન્ય)
{
    પાછા 1;
}

સ્થિર પૂર્ણાંક is_experimental_false(શૂન્ય)
{
    પાછા 0;
}

/*
 * Initialize the keycode translation strategy.
 */
સ્થિર શૂન્ય keycode_trans_strategy_init(શૂન્ય)
{
    અચલ અક્ષર *strategy = ખાલી;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    જો (strategy == ખાલી)
    {
	પાછા;
    }

    જો (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	પાછા;
    }

    જો (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	પાછા;
    }

}

#જો defined(FEAT_RENDER_OPTIONS)
    પૂર્ણાંક
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    પૂર્ણાંક	    dx_enable = 0;
    પૂર્ણાંક	    dx_flags = 0;
    દશાંશ   dx_gamma = 0.0f;
    દશાંશ   dx_contrast = 0.0f;
    દશાંશ   dx_level = 0.0f;
    પૂર્ણાંક	    dx_geom = 0;
    પૂર્ણાંક	    dx_renmode = 0;
    પૂર્ણાંક	    dx_taamode = 0;
    // parse string as rendering options.
    માટે (p = s; p != ખાલી && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  કિંમત[128];

	copy_option_part(&p, item, કદ(item), ",");
	જો (p == ખાલી)
	    તોડો;
	q = &item[0];
	copy_option_part(&q, name, કદ(name), ":");
	જો (q == ખાલી)
	    પાછા FAIL;
	copy_option_part(&q, કિંમત, કદ(કિંમત), ":");

	જો (STRCMP(name, "type") == 0)
	{
	    જો (STRCMP(કિંમત, "directx") == 0)
		dx_enable = 1;
	    નહીંતર
		પાછા FAIL;
	}
	નહીંતર જો (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (દશાંશ)atof((અક્ષર *)કિંમત);
	}
	નહીંતર જો (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (દશાંશ)atof((અક્ષર *)કિંમત);
	}
	નહીંતર જો (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (દશાંશ)atof((અક્ષર *)કિંમત);
	}
	નહીંતર જો (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((અક્ષર *)કિંમત);
	    જો (dx_geom < 0 || dx_geom > 2)
		પાછા FAIL;
	}
	નહીંતર જો (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((અક્ષર *)કિંમત);
	    જો (dx_renmode < 0 || dx_renmode > 6)
		પાછા FAIL;
	}
	નહીંતર જો (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((અક્ષર *)કિંમત);
	    જો (dx_taamode < 0 || dx_taamode > 3)
		પાછા FAIL;
	}
	નહીંતર જો (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	નહીંતર
	    પાછા FAIL;
    }

    જો (!gui.in_use)
	પાછા OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    જો (dx_enable)
    {
	જો (!directx_enabled())
	    પાછા FAIL;
	DWriteContext_SetRenderingParams(s_dwc, ખાલી);
	જો (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    જો (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    જો (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    જો (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    જો (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    જો (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    જો (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    પાછા OK;
# નહીંતર
    પાછા FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (પૂર્ણાંક)(short)LOWORD(lParam), (પૂર્ણાંક)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (પૂર્ણાંક)(short)LOWORD(lParam), (પૂર્ણાંક)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (પૂર્ણાંક)(short)LOWORD(lParam), (પૂર્ણાંક)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#અન્તર્ભાવ "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#અન્તર્ભાવ <shellapi.h>
#અન્તર્ભાવ <commctrl.h>
#અન્તર્ભાવ <windowsx.h>

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

સ્થિર શૂન્ય _OnPaint(HWND hwnd);
સ્થિર શૂન્ય fill_rect(અચલ RECT *rcp, HBRUSH hbr, COLORREF color);
સ્થિર શૂન્ય clear_rect(RECT *rcp);

સ્થિર WORD		s_dlgfntheight;		// height of the dialog font
સ્થિર WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
સ્થિર HMENU		s_menuBar = ખાલી;
#endif
#ifdef FEAT_TEAROFF
સ્થિર શૂન્ય rebuild_tearoff(vimmenu_T *menu);
સ્થિર HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
સ્થિર પૂર્ણાંક		s_busy_processing = FALSE;

સ્થિર પૂર્ણાંક		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
સ્થિર UINT		s_findrep_સંદેશ = 0;
સ્થિર FINDREPLACEW	s_findrep_struct;
સ્થિર HWND		s_findrep_hwnd = ખાલી;
સ્થિર પૂર્ણાંક		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = ખાલી;
સ્થિર HDC		s_hdc = ખાલી;
સ્થિર HBRUSH		s_brush = ખાલી;

#ifdef FEAT_TOOLBAR
સ્થિર HWND		s_toolbarhwnd = ખાલી;
સ્થિર WNDPROC		s_toolbar_wndproc = ખાલી;
#endif

#ifdef FEAT_GUI_TABLINE
સ્થિર HWND		s_tabhwnd = ખાલી;
સ્થિર WNDPROC		s_tabline_wndproc = ખાલી;
સ્થિર પૂર્ણાંક		showing_tabline = 0;
#endif

સ્થિર WPARAM		s_wParam = 0;

સ્થિર HWND		s_textArea = ખાલી;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
સ્થિર bool		s_textarea_bg_only = false;

સ્થિર UINT		s_u_સંદેશ = 0;

સ્થિર char_u		*s_textfield; // Used by dialogs to pass back strings

સ્થિર પૂર્ણાંક		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
સ્થિર પૂર્ણાંક allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
પ્રકારવ્યાખ્યા HANDLE DPI_AWARENESS_CONTEXT;

પ્રકારવ્યાખ્યા ગણના DPI_AWARENESS {
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
સ્થિર પૂર્ણાંક		s_dpi = DEFAULT_DPI;
સ્થિર BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
સ્થિર DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
સ્થિર RECT		s_suggested_rect;

સ્થિર UINT (WINAPI *pGetDpiForSystem)(શૂન્ય) = ખાલી;
સ્થિર UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = ખાલી;
સ્થિર પૂર્ણાંક (WINAPI *pGetSystemMetricsForDpi)(પૂર્ણાંક, UINT) = ખાલી;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
સ્થિર DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = ખાલી;
#ifdef FEAT_MBYTE_IME
સ્થિર DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ખાલી;
#endif

સ્થિર HINSTANCE hLibDwm = ખાલી;
સ્થિર HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
સ્થિર શૂન્ય dyn_dwm_load(શૂન્ય);

સ્થિર પૂર્ણાંક fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

સ્થિર HINSTANCE hUxThemeLib = ખાલી;
સ્થિર DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = ખાલી;
સ્થિર શૂન્ય (WINAPI *pFlushMenuThemes)(શૂન્ય) = ખાલી;
સ્થિર શૂન્ય dyn_uxtheme_load(શૂન્ય);
#endif

    સ્થિર પૂર્ણાંક WINAPI
stubGetSystemMetricsForDpi(પૂર્ણાંક nIndex, UINT dpi UNUSED)
{
    પાછા GetSystemMetrics(nIndex);
}

    સ્થિર પૂર્ણાંક
adjust_fontsize_by_dpi(પૂર્ણાંક size)
{
    પાછા size * s_dpi / (પૂર્ણાંક)pGetDpiForSystem();
}

    સ્થિર પૂર્ણાંક
adjust_by_system_dpi(પૂર્ણાંક size)
{
    પાછા size * (પૂર્ણાંક)pGetDpiForSystem() / DEFAULT_DPI;
}

#જો defined(FEAT_DIRECTX)
    સ્થિર પૂર્ણાંક
directx_enabled(શૂન્ય)
{
    જો (s_dwc != ખાલી)
	પાછા 1;
    નહીંતર જો (s_directx_load_attempted)
	પાછા 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    પાછા s_dwc != ખાલી ? 1 : 0;
}

    સ્થિર શૂન્ય
directx_binddc(શૂન્ય)
{
    જો (s_textArea == ખાલી)
	પાછા;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

બાહ્ય પૂર્ણાંક current_font_height;	    // this is in os_mswin.c

સ્થિર સંરચના
{
    UINT    ચાવી_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ચાવીઓ[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_ડાબું,		'k', 'l'},
    {VK_જમણું,		'k', 'r'},

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
    {VK_ઉમેરો,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_END,		'@', '7'},
    {VK_PRIOR,		'k', 'P'},
    {VK_NEXT,		'k', 'N'},
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
સ્થિર પૂર્ણાંક	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
સ્થિર પૂર્ણાંક	s_getting_focus = FALSE;

સ્થિર પૂર્ણાંક	s_x_pending;
સ્થિર પૂર્ણાંક	s_y_pending;
સ્થિર UINT	s_kFlags_pending;
સ્થિર UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
સ્થિર પૂર્ણાંક	s_timed_out = FALSE;
સ્થિર પૂર્ણાંક	dead_ચાવી = DEAD_ચાવી_OFF;
સ્થિર UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
સ્થિર શૂન્ય Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
સ્થિર શૂન્ય track_user_activity(UINT u_સંદેશ);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
સ્થિર LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
સ્થિર LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
સ્થિર LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#જો defined(FEAT_BROWSE)
સ્થિર char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    સ્થિર શૂન્ય
print_windows_error(શૂન્ય)
{
    LPVOID  lp_સંદેશ_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  ખાલી, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_સંદેશ_Buf, 0, ખાલી);
    TRACE1("Error: %s\n", lp_સંદેશ_Buf);
    LocalFree(lp_સંદેશ_Buf);
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

સ્થિર પૂર્ણાંક		blink_state = BLINK_NONE;
સ્થિર long_u		blink_waittime = 700;
સ્થિર long_u		blink_ontime = 400;
સ્થિર long_u		blink_offtime = 250;
સ્થિર UINT_PTR		blink_timer = 0;

    પૂર્ણાંક
gui_mch_is_blinking(શૂન્ય)
{
    પાછા blink_state != BLINK_NONE;
}

    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા blink_state == BLINK_OFF;
}

    શૂન્ય
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    સ્થિર VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_સંદેશ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    સંદેશ_2 સંદેશ;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ખાલી, idEvent);

    // Eat spurious WM_TIMER messages
    જ્યારે (PeekMessageW(&સંદેશ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    જો (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(ખાલી, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    નહીંતર
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(ખાલી, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    સ્થિર શૂન્ય
gui_mswin_rm_blink_timer(શૂન્ય)
{
    સંદેશ_2 સંદેશ;

    જો (blink_timer == 0)
	પાછા;

    KillTimer(ખાલી, blink_timer);
    // Eat spurious WM_TIMER messages
    જ્યારે (PeekMessageW(&સંદેશ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    જો (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    શૂન્ય
gui_mch_start_blink(શૂન્ય)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(ખાલી, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    સ્થિર VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_સંદેશ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    સંદેશ_2 સંદેશ;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ખાલી, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    જ્યારે (PeekMessageW(&સંદેશ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    જો (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    સ્થિર શૂન્ય
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    પૂર્ણાંક cRepeat UNUSED)
{
    dead_ચાવી = DEAD_ચાવી_SET_DEFAULT;
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
    સ્થિર પૂર્ણાંક
char_to_string(પૂર્ણાંક ch, char_u *string, પૂર્ણાંક slen, પૂર્ણાંક had_alt)
{
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    WCHAR	wstring[2];
    char_u	*ws = ખાલી;

    જો (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    નહીંતર જો (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	પાછા 0;
    }
    નહીંતર
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    જો (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ખાલી);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	જો (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ખાલી);
	    જો (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    નહીંતર
    {
	ws = utf16_to_enc(wstring, &len);
	જો (ws == ખાલી)
	    len = 0;
	નહીંતર
	{
	    જો (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    જો (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    માટે (i = 0; i < len; ++i)
	જો (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (પૂર્ણાંક)KE_CSI;
	    len += 2;
	}

    પાછા len;
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
    સ્થિર પૂર્ણાંક
get_active_modifiers_experimental(શૂન્ય)
{
    પૂર્ણાંક modifiers = 0;

    જો (Get_ચાવી_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    જો (Get_ચાવી_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    જો (Get_ચાવી_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    જો ((modifiers & MOD_MASK_CTRL) && (Get_ચાવી_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    જો (!(Get_ચાવી_State(VK_CONTROL) & 0x8000)
	    &&  (Get_ચાવી_State(VK_RMENU) & 0x8000)
	    && !(Get_ચાવી_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    પાછા modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    સ્થિર પૂર્ણાંક
get_active_modifiers_classic(શૂન્ય)
{
    પૂર્ણાંક modifiers = 0;

    જો (Get_ચાવી_State(VK_SHIFT) & 0x8000)
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
    જો (Get_ચાવી_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    જો (Get_ચાવી_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    પાછા modifiers;
}

    સ્થિર પૂર્ણાંક
get_active_modifiers(શૂન્ય)
{
    પાછા get_active_modifiers_experimental();
}

    સ્થિર પૂર્ણાંક
get_active_modifiers_via_ptr(શૂન્ય)
{
    // marshal to corresponding implementation
    પાછા keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    સ્થિર શૂન્ય
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    પૂર્ણાંક cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    સ્થિર શૂન્ય
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    પૂર્ણાંક cRepeat UNUSED)
{
    char_u	string[40];
    પૂર્ણાંક		len = 0;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		ch = cch;   // special keys are negative

    જો (dead_ચાવી == DEAD_ચાવી_SKIP_ON_CHAR)
	પાછા;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    જો (dead_ચાવી != DEAD_ચાવી_TRANSIENT_IN_ON_CHAR)
	dead_ચાવી = DEAD_ચાવી_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_ચાવી(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_ચાવીઓ;
    ch = may_adjust_ચાવી_for_ctrl(modifiers, ch);
    --no_reduce_ચાવીઓ;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ખાલી);
    જો (ch == CSI)
	ch = K_CSI;

    જો (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_આગત_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    જો (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_આગત_buf();
	got_int = TRUE;
    }

    add_to_આગત_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    સ્થિર શૂન્ય
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    પૂર્ણાંક cRepeat UNUSED)
{
    char_u	string[40];
    પૂર્ણાંક		len = 0;

    dead_ચાવી = 0;

    len = char_to_string(ch, string, 40, FALSE);
    જો (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_આગત_buf();
	got_int = TRUE;
    }

    add_to_આગત_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    સ્થિર શૂન્ય
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    પૂર્ણાંક cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    સ્થિર શૂન્ય
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    પૂર્ણાંક cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    પૂર્ણાંક		len;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		ch = cch;   // special keys are negative

    dead_ચાવી = DEAD_ચાવી_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_ચાવી(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ખાલી);
    જો (ch == CSI)
	ch = K_CSI;

    len = 0;
    જો (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    જો (IS_SPECIAL((પૂર્ણાંક)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((પૂર્ણાંક)ch);
	string[len++] = K_THIRD((પૂર્ણાંક)ch);
    }
    નહીંતર
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_આગત_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    સ્થિર શૂન્ય
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    પૂર્ણાંક cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    પૂર્ણાંક		len;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		ch = cch;   // special keys are negative

    dead_ચાવી = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    જો (Get_ચાવી_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    જો (Get_ચાવી_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_ચાવી(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ખાલી);
    જો (ch == CSI)
	ch = K_CSI;

    len = 0;
    જો (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    જો (IS_SPECIAL((પૂર્ણાંક)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((પૂર્ણાંક)ch);
	string[len++] = K_THIRD((પૂર્ણાંક)ch);
    }
    નહીંતર
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_આગત_buf(string, len);
}

    સ્થિર શૂન્ય
_OnMouseEvent(
    પૂર્ણાંક button,
    પૂર્ણાંક x,
    પૂર્ણાંક y,
    પૂર્ણાંક repeated_click,
    UINT ચાવી_Flags)
{
    પૂર્ણાંક vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    જો (ચાવી_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    જો (ચાવી_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    જો (Get_ચાવી_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    સ્થિર શૂન્ય
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    પૂર્ણાંક x,
    પૂર્ણાંક y,
    UINT ચાવી_Flags)
{
    સ્થિર LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    પૂર્ણાંક	    button = -1;
    પૂર્ણાંક	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (શૂન્ય)SetFocus(s_hwnd);

    જો (s_u_સંદેશ == WM_LBUTTONDOWN || s_u_સંદેશ == WM_LBUTTONDBLCLK)
	button = MOUSE_ડાબું;
    નહીંતર જો (s_u_સંદેશ == WM_MBUTTONDOWN || s_u_સંદેશ == WM_MBUTTONDBLCLK)
	button = MOUSE_મધ્ય;
    નહીંતર જો (s_u_સંદેશ == WM_RBUTTONDOWN || s_u_સંદેશ == WM_RBUTTONDBLCLK)
	button = MOUSE_જમણું;
    નહીંતર જો (s_u_સંદેશ == WM_XBUTTONDOWN || s_u_સંદેશ == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    નહીંતર જો (s_u_સંદેશ == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	જો (s_button_pending == MOUSE_ડાબું)
	    button = MOUSE_જમણું;
	નહીંતર
	    button = MOUSE_ડાબું;
    }

    જો (button < 0)
	પાછા;

    repeated_click = ((પૂર્ણાંક)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    જો (repeated_click
	    && ((button == MOUSE_ડાબું && s_button_pending == MOUSE_જમણું)
		|| (button == MOUSE_જમણું
		    && s_button_pending == MOUSE_ડાબું)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_મધ્ય;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, ચાવી_Flags);
    }
    નહીંતર જો ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_જમણું)))
    {
	જો (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, ચાવી_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, ચાવી_Flags);
    }
    નહીંતર
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
	s_kFlags_pending = ચાવી_Flags;
    }

    s_prevTime = currentTime;
}

    સ્થિર શૂન્ય
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    પૂર્ણાંક x,
    પૂર્ણાંક y,
    UINT ચાવી_Flags)
{
    પૂર્ણાંક button;

    s_getting_focus = FALSE;
    જો (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    જો (s_u_સંદેશ == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	જો (!(ચાવી_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    પાછા;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    નહીંતર
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, ચાવી_Flags);
}

    સ્થિર શૂન્ય
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    પૂર્ણાંક cx UNUSED,
    પૂર્ણાંક cy UNUSED)
{
#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    સ્થિર vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    પૂર્ણાંક		id)
{
    vimmenu_T	*pChildMenu;

    જ્યારે (pMenu)
    {
	જો (pMenu->id == (UINT)id)
	    તોડો;
	જો (pMenu->children != ખાલી)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    જો (pChildMenu)
	    {
		pMenu = pChildMenu;
		તોડો;
	    }
	}
	pMenu = pMenu->next;
    }
    પાછા pMenu;
}

    સ્થિર શૂન્ય
_OnMenu(
    HWND	hwnd UNUSED,
    પૂર્ણાંક		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(મૂળ_menu, id);
    જો (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    સ્થિર શૂન્ય
_OnFindRepl(શૂન્ય)
{
    પૂર્ણાંક	    flags = 0;
    પૂર્ણાંક	    down;

    જો (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(શૂન્ય)SetFocus(s_hwnd);

    જો (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(શૂન્ય)SetFocus(s_hwnd);
    }
    નહીંતર જો (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(શૂન્ય)SetFocus(s_hwnd);
    }
    નહીંતર જો (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    જો (flags == 0)
	પાછા;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    જો (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    જો (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, ખાલી);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, ખાલી);
    જો (p != ખાલી && q != ખાલી)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    સ્થિર શૂન્ય
HandleMouseHide(UINT u_સંદેશ, LPARAM lParam)
{
    સ્થિર LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    જો (u_સંદેશ == WM_MOUSEMOVE || u_સંદેશ == WM_NCMOUSEMOVE)
    {
	જો (lParam == last_lParam)
	    પાછા;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    પસંદગી (u_સંદેશ)
    {
    સ્થિતિ WM_KEYUP:
    સ્થિતિ WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	જો (p_mh)
	    gui_mch_mousehide(TRUE);
	તોડો;

    સ્થિતિ WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    સ્થિતિ WM_SYSCHAR:
    સ્થિતિ WM_MOUSEMOVE:	 // show the pointer on any mouse action
    સ્થિતિ WM_LBUTTONDOWN:
    સ્થિતિ WM_LBUTTONUP:
    સ્થિતિ WM_MBUTTONDOWN:
    સ્થિતિ WM_MBUTTONUP:
    સ્થિતિ WM_RBUTTONDOWN:
    સ્થિતિ WM_RBUTTONUP:
    સ્થિતિ WM_XBUTTONDOWN:
    સ્થિતિ WM_XBUTTONUP:
    સ્થિતિ WM_NCMOUSEMOVE:
    સ્થિતિ WM_NCLBUTTONDOWN:
    સ્થિતિ WM_NCLBUTTONUP:
    સ્થિતિ WM_NCMBUTTONDOWN:
    સ્થિતિ WM_NCMBUTTONUP:
    સ્થિતિ WM_NCRBUTTONDOWN:
    સ્થિતિ WM_NCRBUTTONUP:
    સ્થિતિ WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	તોડો;
    }
}

    સ્થિર LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_સંદેશ,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_સંદેશ, lParam);

    s_u_સંદેશ = u_સંદેશ;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_સંદેશ);
#endif

    પસંદગી (u_સંદેશ)
    {
	HANDLE_સંદેશ(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_સંદેશ(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_સંદેશ(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_સંદેશ(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_સંદેશ(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_સંદેશ(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_સંદેશ(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_સંદેશ(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	સ્થિતિ WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    પાછા TRUE;
#endif
	મૂળભૂત:
	    પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
    }
}

    સ્થિર શૂન્ય
dyn_dwm_load(શૂન્ય)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    જો (hLibDwm == ખાલી)
	પાછા;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    જો (pDwmSetWindowAttribute == ખાલી)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ખાલી;
	પાછા;
    }
}

બાહ્ય DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    શૂન્ય
gui_mch_set_titlebar_colors(શૂન્ય)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    જો (pDwmSetWindowAttribute == ખાલી || win_version < MAKE_VER(10, 0, 22000))
	પાછા;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    જો (vim_strchr(p_go, GO_TITLEBAR) != ખાલી)
    {
	જો (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	નહીંતર
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	જો (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	જો (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, કદ(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, કદ(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    શૂન્ય
gui_mch_new_colors(શૂન્ય)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ખાલી, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    જો (s_textarea_bg_only && s_textArea != ખાલી)
	InvalidateRect(s_textArea, ખાલી, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    શૂન્ય
gui_mch_def_colors(શૂન્ય)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    પૂર્ણાંક
gui_mch_open(શૂન્ય)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    જો (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	જો (s_textArea != ખાલી)
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

    પાછા OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.ડાબું;
    *y = rect.ટોચ;
    પાછા OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    SetWindowPos(s_hwnd, ખાલી, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    શૂન્ય
gui_mch_set_text_area_pos(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક w, પૂર્ણાંક h)
{
    સ્થિર પૂર્ણાંક oldx = 0;
    સ્થિર પૂર્ણાંક oldy = 0;

    SetWindowPos(s_textArea, ખાલી, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#જો defined(FEAT_GUI_TABLINE)
    જો (showing_tabline)
    {
	પૂર્ણાંક	ટોચ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી)
	    ટોચ = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, ટોચ, rect.જમણું, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    જો (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ખાલી, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    શૂન્ય
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    પૂર્ણાંક		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    શૂન્ય
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    પૂર્ણાંક		x,
    પૂર્ણાંક		y,
    પૂર્ણાંક		w,
    પૂર્ણાંક		h)
{
    SetWindowPos(sb->id, ખાલી, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    પૂર્ણાંક
gui_mch_get_scrollbar_xpadding(શૂન્ય)
{
    RECT    rcTxt, rcWnd;
    પૂર્ણાંક	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.જમણું - rcTxt.જમણું - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    પાછા (xpad < 0) ? 0 : xpad;
}

    પૂર્ણાંક
gui_mch_get_scrollbar_ypadding(શૂન્ય)
{
    RECT    rcTxt, rcWnd;
    પૂર્ણાંક	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    પાછા (ypad < 0) ? 0 : ypad;
}

    શૂન્ય
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    પૂર્ણાંક		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ખાલી,
	g_hinst, ખાલી);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    સ્થિર scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    જો (gui.bottom_sbar.id == hwnd)
	પાછા &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	જો (wp->w_scrollbars[SBAR_ડાબું].id == hwnd)
	    પાછા &wp->w_scrollbars[SBAR_ડાબું];
	જો (wp->w_scrollbars[SBAR_જમણું].id == hwnd)
	    પાછા &wp->w_scrollbars[SBAR_જમણું];
    }
    પાછા ખાલી;
}

    સ્થિર શૂન્ય
update_scrollbar_size(શૂન્ય)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
GetFontSize(GuiFont font, પૂર્ણાંક *char_width, પૂર્ણાંક *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    જો (char_width)
	*char_width = size.cx + tm.tmOverhang;
    જો (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    સ્થિર શૂન્ય
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    પૂર્ણાંક
gui_mch_adjust_charheight(શૂન્ય)
{
    UpdateFontSize(gui.norm_font);
    પાછા OK;
}

    સ્થિર GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ખાલી;

    // Load the font
    font = CreateFontIndirectW(lf);

    જો (font == ખાલી)
	પાછા NOFONT;

    પાછા (GuiFont)font;
}

    સ્થિર પૂર્ણાંક
pixels_to_points(પૂર્ણાંક pixels, પૂર્ણાંક vertical)
{
    પૂર્ણાંક		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    પાછા points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    પૂર્ણાંક		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    જો (get_logfont(&lf, name, ખાલી, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    જો (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    પાછા font;
}

#જો defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    જો (name == ખાલી)
	પાછા ખાલી;
    પાછા vim_strsave(name);
}
#endif

    શૂન્ય
gui_mch_free_font(GuiFont font)
{
    જો (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    પૂર્ણાંક i;

    પ્રકારવ્યાખ્યા સંરચના SysColorTable
    {
	અક્ષર	    *name;
	પૂર્ણાંક	    color;
    } SysColorTable;

    સ્થિર SysColorTable sys_table[] =
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
    માટે (i = 0; i < (પૂર્ણાંક)ARRAY_LENGTH(sys_table); i++)
	જો (STRICMP(name, sys_table[i].name) == 0)
	    પાછા GetSysColor(sys_table[i].color);

    પાછા gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    પાછા gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    પૂર્ણાંક
gui_mch_haskey(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].vim_code1 != NUL; i++)
	જો (name[0] == special_ચાવીઓ[i].vim_code0
				       && name[1] == special_ચાવીઓ[i].vim_code1)
	    પાછા OK;
    પાછા FAIL;
}

    શૂન્ય
gui_mch_beep(શૂન્ય)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    શૂન્ય
gui_mch_invert_rectangle(
    પૂર્ણાંક	    r,
    પૂર્ણાંક	    c,
    પૂર્ણાંક	    nr,
    પૂર્ણાંક	    nc)
{
    RECT    rc;

#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ડાબું = FILL_X(c);
    rc.ટોચ = FILL_Y(r);
    rc.જમણું = rc.ડાબું + nc * gui.char_width;
    rc.bottom = rc.ટોચ + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    શૂન્ય
gui_mch_iconify(શૂન્ય)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    શૂન્ય
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ડાબું = FILL_X(gui.col);
    rc.ટોચ = FILL_Y(gui.row);
    rc.જમણું = rc.ડાબું + gui.char_width;
    જો (mb_lefthalve(gui.row, gui.col))
	rc.જમણું += gui.char_width;
    rc.bottom = rc.ટોચ + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    શૂન્ય
gui_mch_draw_part_cursor(
    પૂર્ણાંક		w,
    પૂર્ણાંક		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ડાબું =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_જમણું ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.ટોચ = FILL_Y(gui.row) + gui.char_height - h;
    rc.જમણું = rc.ડાબું + w;
    rc.bottom = rc.ટોચ + h;

    fill_rect(&rc, ખાલી, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    સ્થિર શૂન્ય
output_Dead_ચાવી_re_Post_Ex(સંદેશ_2 original_સંદેશ, પૂર્ણાંક dead_key2set)
{
    સ્થિર સંદેશ_2 deadCharExpel;

    જો (dead_ચાવી == DEAD_ચાવી_OFF)
	પાછા;

    dead_ચાવી = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_સંદેશ.message;
    deadCharExpel.hwnd    = original_સંદેશ.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_સંદેશ.hwnd, original_સંદેશ.message, original_સંદેશ.wParam,
							  original_સંદેશ.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    સ્થિર શૂન્ય
output_Dead_ચાવી_re_Post(સંદેશ_2 original_સંદેશ)
{
    output_Dead_ચાવી_re_Post_Ex(original_સંદેશ, DEAD_ચાવી_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
સ્થિર શૂન્ય process_message_usual_ચાવી(UINT vk, અચલ સંદેશ_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_ચાવી(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    સ્થિર શૂન્ય
process_message_usual_ચાવી_experimental(UINT vk, અચલ સંદેશ_2 *pmsg)
{
    WCHAR	ch[8];
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    જો (Get_ચાવી_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    જો (Get_ચાવી_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    જો ((Get_ચાવી_State(VK_RMENU) & 0x8000)
				 && (Get_ચાવી_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_ચાવી(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    જો (len < 0)
	dead_ચાવી = DEAD_ચાવી_SET_DEFAULT;

    જો (len <= 0)
    {
	પૂર્ણાંક wm_char = NUL;

	જો (dead_ચાવી == DEAD_ચાવી_SET_DEFAULT
		&& (Get_ચાવી_State(VK_CONTROL) & 0x8000))
	{
	    જો (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    જો (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	જો (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_ચાવી = DEAD_ચાવી_TRANSIENT_IN_ON_CHAR;
	}
	પાછા;
    }

    // Post the message as TranslateMessage would do.
    જો (pmsg->message == WM_KEYDOWN)
    {
	માટે (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    નહીંતર
    {
	માટે (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    સ્થિર શૂન્ય
process_message_usual_ચાવી_classic(UINT vk, અચલ સંદેશ_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    જો (vk != 0xff
	    && (Get_ચાવી_State(VK_CONTROL) & 0x8000)
	    && !(Get_ચાવી_State(VK_SHIFT) & 0x8000)
	    && !(Get_ચાવી_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	જો (vk == '6' || Map_Virtual_ચાવી(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_આગત_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	નહીંતર જો (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_આગત_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	નહીંતર જો (vk == '2' || Map_Virtual_ચાવી(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_આગત_buf(string, 1);
	}
	નહીંતર
	    TranslateMessage(pmsg);
    }
    નહીંતર
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    સ્થિર શૂન્ય
process_message(શૂન્ય)
{
    સંદેશ_2		સંદેશ;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    પૂર્ણાંક		i;
    પૂર્ણાંક		modifiers = 0;
    પૂર્ણાંક		ચાવી;
#ifdef FEAT_MENU
    સ્થિર char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    સ્થિર પૂર્ણાંક  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    જો (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&સંદેશ, ખાલી, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    જો (સંદેશ.message == WM_OLE)
    {
	char_u *str = (char_u *)સંદેશ.lParam;
	જો (str == ખાલી || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&સંદેશ);
	}
	નહીંતર
	{
	    add_to_આગત_buf(str, (પૂર્ણાંક)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	પાછા;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    જો (s_findrep_hwnd != ખાલી && IsDialogMessageW(s_findrep_hwnd, &સંદેશ))
    {
	HandleMouseHide(સંદેશ.message, સંદેશ.lParam);
	પાછા;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    જો (સંદેશ.message == WM_KEYDOWN || સંદેશ.message == WM_SYSKEYDOWN)
    {
	vk = (પૂર્ણાંક) સંદેશ.wParam;

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
	જો (keycode_trans_strategy_used->is_experimental()
		&& dead_ચાવી == DEAD_ચાવી_SKIP_ON_CHAR)
	{
	    dead_ચાવી = DEAD_ચાવી_OFF;
	}

	જો (dead_ચાવી != DEAD_ચાવી_OFF)
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
	    જો (keycode_trans_strategy_used->is_experimental() &&
		    dead_ચાવી == DEAD_ચાવી_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_ચાવી_re_Post_Ex(સંદેશ,
				       /*dead_key2set=*/DEAD_ચાવી_SKIP_ON_CHAR);
		પાછા;
	    }

	    જો (dead_ચાવી != DEAD_ચાવી_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		પાછા;
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
	    જો ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_ચાવી = DEAD_ચાવી_OFF;
		TranslateMessage(&સંદેશ);
		પાછા;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    નહીંતર જો ((get_real_state()
			    & (MODE_ઉમેરો | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_ચાવી_re_Post(સંદેશ);
		પાછા;
	    }
	}

	// Check for CTRL-BREAK
	જો (vk == VK_CANCEL)
	{
	    trash_આગત_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_આગત_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	જો (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&સંદેશ);
	    પાછા;
	}

	માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    જો (special_ચાવીઓ[i].ચાવી_sym == vk
		    && (vk != VK_SPACE || !(Get_ચાવી_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		જો (dead_ચાવી == DEAD_ચાવી_SET_DEFAULT
			&& (special_ચાવીઓ[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_ચાવી_re_Post(સંદેશ);
		    પાછા;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		જો (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ખાલી, ખાલી) == ખાલી)
		    તોડો;
#endif
		modifiers = get_active_modifiers_via_ptr();

		જો (special_ચાવીઓ[i].vim_code1 == NUL)
		    ચાવી = special_ચાવીઓ[i].vim_code0;
		નહીંતર
		    ચાવી = TO_SPECIAL(special_ચાવીઓ[i].vim_code0,
						   special_ચાવીઓ[i].vim_code1);
		ચાવી = simplify_ચાવી(ચાવી, &modifiers);
		જો (ચાવી == CSI)
		    ચાવી = K_CSI;

		જો (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_આગત_buf(string, 3);
		}

		જો (IS_SPECIAL(ચાવી))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ચાવી);
		    string[2] = K_THIRD(ચાવી);
		    add_to_આગત_buf(string, 3);
		}
		નહીંતર
		{
		    પૂર્ણાંક	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(ચાવી, string, 40, FALSE);
		    add_to_આગત_buf(string, len);
		}
		તોડો;
	    }
	}

	// Not a special key.
	જો (special_ચાવીઓ[i].ચાવી_sym == 0)
	{
	    process_message_usual_ચાવી(vk, &સંદેશ);
	}
    }
#ifdef FEAT_MBYTE_IME
    નહીંતર જો (સંદેશ.message == WM_IME_NOTIFY)
	_OnImeNotify(સંદેશ.hwnd, (DWORD)સંદેશ.wParam, (DWORD)સંદેશ.lParam);
    નહીંતર જો (સંદેશ.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&સંદેશ);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    જો (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ખાલી, ખાલી) == ખાલી)
#endif
	DispatchMessageW(&સંદેશ);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    શૂન્ય
gui_mch_update(શૂન્ય)
{
    સંદેશ_2	    સંદેશ;

    જો (!s_busy_processing)
	જ્યારે (PeekMessageW(&સંદેશ, ખાલી, 0, 0, PM_NOREMOVE)
						  && !vim_is_આગત_buf_full())
	    process_message();
}

    સ્થિર શૂન્ય
remove_any_timer(શૂન્ય)
{
    સંદેશ_2		સંદેશ;

    જો (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(ખાલી, s_wait_timer);

	// Eat spurious WM_TIMER messages
	જ્યારે (PeekMessageW(&સંદેશ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
સ્થિર garray_T	server_pending_ga = {0, 0, 0, 0, ખાલી};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    શૂન્ય
server_add_આગત(char_u *str)
{
    જો (server_pending_ga.ga_data == ખાલી)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    સ્થિર શૂન્ય
server_flush_આગત(શૂન્ય)
{
    char_u  *str;

    જો (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	પાછા;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    જો (str != ખાલી)
    {
	server_to_આગત_buf(str);
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
    પૂર્ણાંક
gui_mch_wait_for_chars(long wtime)
{
    પૂર્ણાંક		focus;

    s_timed_out = FALSE;

    જો (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	જો (s_busy_processing)
	    પાછા FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(ખાલી, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    જ્યારે (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	જો (gui.in_focus != focus)
	{
	    જો (gui.in_focus)
		gui_mch_start_blink();
	    નહીંતર
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	જો (s_need_activate)
	{
	    (શૂન્ય)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	માટે (;;)
	{
	    સંદેશ_2 સંદેશ;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_આગત();
# endif
# ifdef FEAT_TIMERS
	    જો (did_add_timer)
		તોડો;
# endif
	    જો (PeekMessageW(&સંદેશ, ખાલી, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		તોડો;
	    }
	    નહીંતર જો (આગત_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || સંદેશ_Wait_For_Multiple_Objects(0, ખાલી, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		તોડો;
	}
#નહીંતર
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	જો (આગત_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    જો (!s_getting_focus)
		s_button_pending = -1;

	    પાછા OK;
	}

#ifdef FEAT_TIMERS
	જો (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    તોડો;
	}
#endif
    }
    allow_scrollbar = FALSE;
    પાછા FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    શૂન્ય
gui_mch_clear_block(
    પૂર્ણાંક		row1,
    પૂર્ણાંક		col1,
    પૂર્ણાંક		row2,
    પૂર્ણાંક		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ડાબું = FILL_X(col1);
    rc.ટોચ = FILL_Y(row1);
    rc.જમણું = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    શૂન્ય
gui_mch_clear_all(શૂન્ય)
{
    RECT    rc;

    rc.ડાબું = 0;
    rc.ટોચ = 0;
    rc.જમણું = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    શૂન્ય
gui_mch_enable_menu(પૂર્ણાંક flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : ખાલી);
#endif
}

    શૂન્ય
gui_mch_set_menu_pos(
    પૂર્ણાંક	    x UNUSED,
    પૂર્ણાંક	    y UNUSED,
    પૂર્ણાંક	    w UNUSED,
    પૂર્ણાંક	    h UNUSED)
{
    // It will be in the right place anyway
}

#જો defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    શૂન્ય
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    પૂર્ણાંક		hidden)
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
    શૂન્ય
gui_mch_draw_menubar(શૂન્ય)
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
    પાછા (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#જો defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    સ્થિર WORD
PixelToDialogX(પૂર્ણાંક અંક_Pixels)
{
    પાછા (WORD)((અંક_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    સ્થિર WORD
PixelToDialogY(પૂર્ણાંક અંક_Pixels)
{
    પાછા (WORD)((અંક_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    સ્થિર પૂર્ણાંક
GetTextWidth(HDC hdc, char_u *str, પૂર્ણાંક len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    પાછા size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    સ્થિર પૂર્ણાંક
GetTextWidthEnc(HDC hdc, char_u *str, પૂર્ણાંક len)
{
    SIZE	size;
    WCHAR	*wstr;
    પૂર્ણાંક		n;
    પૂર્ણાંક		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    જો (wstr == ખાલી)
	પાછા 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    જો (n)
	પાછા size.cx;
    પાછા 0;
}

સ્થિર શૂન્ય get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    સ્થિર BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    પૂર્ણાંક		    wChild, hChild, wParent, hParent;
    પૂર્ણાંક		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.જમણું - rChild.ડાબું;
    hChild = rChild.bottom - rChild.ટોચ;

    // If Vim is minimized put the window in the middle of the screen.
    જો (hwndParent == ખાલી || IsMinimized(hwndParent))
	get_work_area(&rParent);
    નહીંતર
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.જમણું - rParent.ડાબું;
    hParent = rParent.bottom - rParent.ટોચ;

    moninfo.cbSize = કદ(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    જો (mon != ખાલી && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    નહીંતર
    {
	hdc = GetDC(hwndChild);
	rScreen.ડાબું = 0;
	rScreen.ટોચ = 0;
	rScreen.જમણું = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.ડાબું + ((wParent - wChild) / 2);
    જો (xNew < rScreen.ડાબું)
	xNew = rScreen.ડાબું;
    નહીંતર જો ((xNew + wChild) > rScreen.જમણું)
	xNew = rScreen.જમણું - wChild;

    yNew = rParent.ટોચ + ((hParent - hChild) / 2);
    જો (yNew < rScreen.ટોચ)
	yNew = rScreen.ટોચ;
    નહીંતર જો ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    પાછા SetWindowPos(hwndChild, ખાલી, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#જો defined(FEAT_TOOLBAR)
    શૂન્ય
gui_mch_show_toolbar(પૂર્ણાંક showit)
{
    જો (s_toolbarhwnd == ખાલી)
	પાછા;

    જો (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    નહીંતર
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_ગણતરી 31

#endif

#જો defined(FEAT_GUI_TABLINE)
    સ્થિર શૂન્ય
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, ખાલી);
    જો (wn == ખાલી)
	પાછા;

    infow.cbSize = કદ(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    ઉમેરો_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    સ્થિર શૂન્ય
show_tabline_popup_menu(શૂન્ય)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    જો (hold_gui_events || cmdwin_type != 0)
	પાછા;

    tab_pmenu = CreatePopupMenu();
    જો (tab_pmenu == ખાલી)
	પાછા;

    જો (first_tabpage->tp_next != ખાલી)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ખાલી);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    જો (rval > 0)
    {
	TCHITTESTINFO htinfo;
	પૂર્ણાંક idx;

	જો (ScreenToClient(s_tabhwnd, &pt) == 0)
	    પાછા;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	જો (idx == -1)
	    idx = 0;
	નહીંતર
	    idx += 1;

	send_tabline_menu_event(idx, (પૂર્ણાંક)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    શૂન્ય
gui_mch_show_tabline(પૂર્ણાંક showit)
{
    જો (s_tabhwnd == ખાલી)
	પાછા;

    જો (!showit != !showing_tabline)
    {
	જો (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	નહીંતર
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    પૂર્ણાંક
gui_mch_showing_tabline(શૂન્ય)
{
    પાછા s_tabhwnd != ખાલી && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    શૂન્ય
gui_mch_update_tabline(શૂન્ય)
{
    tabpage_T	*tp;
    TCITEM	tie;
    પૂર્ણાંક		nr = 0;
    પૂર્ણાંક		curtabidx = 0;
    પૂર્ણાંક		tabadded = 0;
    WCHAR	*wstr = ખાલી;

    જો (s_tabhwnd == ખાલી)
	પાછા;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    માટે (tp = first_tabpage; tp != ખાલી; tp = tp->tp_next, ++nr)
    {
	જો (tp == curtab)
	    curtabidx = nr;

	જો (nr >= Tab_Ctrl_Get_Item_ગણતરી(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_ઉમેરો_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, ખાલી);
	જો (wstr != ખાલી)
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
    જ્યારે (nr < Tab_Ctrl_Get_Item_ગણતરી(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    જો (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, ખાલી, ખાલી,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    જો (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    શૂન્ય
gui_mch_set_curtab(પૂર્ણાંક nr)
{
    જો (s_tabhwnd == ખાલી)
	પાછા;

    જો (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    શૂન્ય
gui_mch_set_dark_theme(પૂર્ણાંક dark)
{
    જો (pDwmSetWindowAttribute != ખાલી && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		કદ(dark));

    જો (pSetPreferredAppMode != ખાલી && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    જો (pFlushMenuThemes != ખાલી && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    સ્થિર શૂન્ય
dyn_uxtheme_load(શૂન્ય)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    જો (hUxThemeLib == ખાલી)
	પાછા;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (શૂન્ય (WINAPI *)(શૂન્ય))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    જો (pSetPreferredAppMode == ખાલી || pFlushMenuThemes == ખાલી)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ખાલી;
	પાછા;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    શૂન્ય
gui_mch_set_fullscreen(પૂર્ણાંક flag)
{
    સ્થિર RECT normal_rect;
    સ્થિર LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    જો (!full_screen) // Windows not set yet.
	પાછા;

    જો (flag)
    {
	જો (fullscreen_on)
	    પાછા;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = કદ(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	જો (mon == ખાલી || !GetMonitorInfo(mon, &moninfo))
	    પાછા;

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
	SetWindowPos(s_hwnd, ખાલી,
		moninfo.rcMonitor.ડાબું,
		moninfo.rcMonitor.ટોચ,
		moninfo.rcMonitor.જમણું - moninfo.rcMonitor.ડાબું,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.ટોચ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    નહીંતર
    {
	જો (!fullscreen_on)
	    પાછા;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, ખાલી,
		normal_rect.ડાબું,
		normal_rect.ટોચ,
		normal_rect.જમણું - normal_rect.ડાબું,
		normal_rect.bottom - normal_rect.ટોચ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    શૂન્ય
ex_simalt(exarg_T *eap)
{
    char_u	*ચાવીઓ = eap->arg;
    પૂર્ણાંક		fill_typebuf = FALSE;
    char_u	ચાવી_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    જ્યારે (*ચાવીઓ)
    {
	જો (*ચાવીઓ == '~')
	    *ચાવીઓ = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*ચાવીઓ, (LPARAM)0);
	ચાવીઓ++;
	fill_typebuf = TRUE;
    }
    જો (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	ચાવી_name[0] = K_SPECIAL;
	ચાવી_name[1] = KS_EXTRA;
	ચાવી_name[2] = KE_NOP;
	ચાવી_name[3] = NUL;
#જો defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(શૂન્ય)ins_typebuf(ચાવી_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    સ્થિર શૂન્ય
initialise_findrep(char_u *initial_string)
{
    પૂર્ણાંક		wword = FALSE;
    પૂર્ણાંક		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    જો (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    જો (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    જો (entry_text != ખાલી && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, ખાલી);
	જો (p != ખાલી)
	{
	    પૂર્ણાંક len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    સ્થિર શૂન્ય
set_window_title(HWND hwnd, અક્ષર *title)
{
    જો (title != ખાલી)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, ખાલી);
	જો (wbuf != ખાલી)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    નહીંતર
	(શૂન્ય)SetWindowTextW(hwnd, ખાલી);
}

    શૂન્ય
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    જો (s_findrep_સંદેશ != 0)
    {
	જો (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	જો (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(શૂન્ય)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    શૂન્ય
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    જો (s_findrep_સંદેશ != 0)
    {
	જો (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	જો (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(શૂન્ય)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    શૂન્ય
gui_mch_mousehide(પૂર્ણાંક hide)
{
    જો (hide == gui.pointer_hidden)
	પાછા;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    સ્થિર શૂન્ય
gui_mch_show_popupmenu_at(vimmenu_T *menu, પૂર્ણાંક x, પૂર્ણાંક y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (શૂન્ય)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(પૂર્ણાંક)0,	    //reserved param
	s_hwnd,
	ખાલી);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    સ્થિર શૂન્ય
_OnEndSession(શૂન્ય)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    સ્થિર શૂન્ય
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    સ્થિર શૂન્ય
_OnDestroy(HWND hwnd)
{
    જો (!destroying)
	_OnClose(hwnd);
}

    સ્થિર શૂન્ય
_OnPaint(
    HWND hwnd)
{
    જો (IsMinimized(hwnd))
	પાછા;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (શૂન્ય)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    જો (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ડાબું = rect.ડાબું;
	ps.rcPaint.જમણું = rect.જમણું;
    }

    જો (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	જો (!screen_cleared || Screen_લીટીઓ == ખાલી)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	નહીંતર
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ડાબું, ps.rcPaint.ટોચ,
		ps.rcPaint.જમણું - ps.rcPaint.ડાબું + 1,
		ps.rcPaint.bottom - ps.rcPaint.ટોચ + 1);
    }

    EndPaint(hwnd, &ps);
}

    સ્થિર શૂન્ય
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    પૂર્ણાંક cx,
    પૂર્ણાંક cy)
{
    જો (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    સ્થિર શૂન્ય
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (શૂન્ય)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    સ્થિર શૂન્ય
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    જો (destroying)
	પાછા;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (શૂન્ય)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    સ્થિર LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    પાછા DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    શૂન્ય
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    RECT rct;
    POINT mp;

    (શૂન્ય)GetWindowRect(s_textArea, &rct);
    (શૂન્ય)GetCursorPos(&mp);
    *x = (પૂર્ણાંક)(mp.x - rct.ડાબું);
    *y = (પૂર્ણાંક)(mp.y - rct.ટોચ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x, પૂર્ણાંક y)
{
    RECT rct;

    (શૂન્ય)GetWindowRect(s_textArea, &rct);
    (શૂન્ય)SetCursorPos(x + gui.border_offset + rct.ડાબું,
		       y + gui.border_offset + rct.ટોચ);
}

    સ્થિર શૂન્ય
gui_mswin_get_valid_dimensions(
    પૂર્ણાંક w,
    પૂર્ણાંક h,
    પૂર્ણાંક *valid_w,
    પૂર્ણાંક *valid_h,
    પૂર્ણાંક *cols,
    પૂર્ણાંક *rows)
{
    પૂર્ણાંક	    base_width, base_height;

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

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
{
    RECT    rc;

#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ડાબું = 0;
    rc.ટોચ = 0;
    rc.જમણું = gui.અંક_cols * gui.char_width;
    rc.bottom = gui.અંક_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    સ્થિર BOOL
is_point_onscreen(પૂર્ણાંક x, પૂર્ણાંક y)
{
    POINT   pt = {x, y};

    પાછા MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != ખાલી;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    સ્થિર BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ડાબું;
    p1.y = rc.ટોચ;
    p2.x = rc.જમણું - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    જો (!is_point_onscreen(p1.x, p1.y))
	પાછા FALSE;
    જો (!is_point_onscreen(p1.x, p2.y))
	પાછા FALSE;
    જો (!is_point_onscreen(p2.x, p1.y))
	પાછા FALSE;
    જો (!is_point_onscreen(p2.x, p2.y))
	પાછા FALSE;
    પાછા TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    સ્થિર પૂર્ણાંક
get_scroll_flags(શૂન્ય)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    જો (!is_window_onscreen(s_hwnd))
	પાછા SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    માટે (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	જો (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    જો (IntersectRect(&rcDest, &rcVim, &rcOther))
		પાછા SW_INVALIDATE;
	}
    પાછા 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    સ્થિર શૂન્ય
intel_gpu_workaround(શૂન્ય)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    શૂન્ય
gui_mch_delete_લીટીઓ(
    પૂર્ણાંક	    row,
    પૂર્ણાંક	    અંક_લીટીઓ)
{
    RECT	rc;

    rc.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    rc.જમણું = FILL_X(gui.scroll_region_જમણું + 1);
    rc.ટોચ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -અંક_લીટીઓ * gui.char_height, &rc);
    }
    નહીંતર
#endif
    {
#જો defined(FEAT_DIRECTX)
	જો (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -અંક_લીટીઓ * gui.char_height,
				    &rc, &rc, ખાલી, ખાલી, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - અંક_લીટીઓ + 1,
						       gui.scroll_region_ડાબું,
	gui.scroll_region_bot, gui.scroll_region_જમણું);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(
    પૂર્ણાંક		row,
    પૂર્ણાંક		અંક_લીટીઓ)
{
    RECT	rc;

    rc.ડાબું = FILL_X(gui.scroll_region_ડાબું);
    rc.જમણું = FILL_X(gui.scroll_region_જમણું + 1);
    rc.ટોચ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, અંક_લીટીઓ * gui.char_height, &rc);
    }
    નહીંતર
#endif
    {
#જો defined(FEAT_DIRECTX)
	જો (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, અંક_લીટીઓ * gui.char_height,
				    &rc, &rc, ખાલી, ખાલી, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_ડાબું,
				row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);
}


    શૂન્ય
gui_mch_exit(પૂર્ણાંક rc UNUSED)
{
#જો defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ખાલી;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (શૂન્ય)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    જો (s_hwnd != ખાલી)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    સ્થિર char_u *
logfont2name(LOGFONTW lf)
{
    અક્ષર	*charset_name;
    અક્ષર	*quality_name;
    અક્ષર	*font_name;
    size_t	res_size;
    અક્ષર	*res;

    font_name = (અક્ષર *)utf16_to_enc(lf.lfFaceName, ખાલી);
    જો (font_name == ખાલી)
	પાછા ખાલી;
    charset_name = charset_id2name((પૂર્ણાંક)lf.lfCharSet);
    quality_name = quality_id2name((પૂર્ણાંક)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == ખાલી ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ખાલી ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    જો (res != ખાલી)
    {
	અક્ષર	*p;
	પૂર્ણાંક	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	માટે (p = font_name; *p != NUL; ++p)
	{
	    જો (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	જો (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(અક્ષર *)res, res_size, "%s:h%d", font_name, points);
	નહીંતર
	    res_len = vim_snprintf_safelen(
		(અક્ષર *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (અક્ષર *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	જો (charset_name != ખાલી)
	    res_len += vim_snprintf_safelen((અક્ષર *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	જો (quality_name != ખાલી)
	    vim_snprintf((અક્ષર *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    પાછા (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    સ્થિર શૂન્ય
update_im_font(શૂન્ય)
{
    LOGFONTW	lf_wide, lf;

    જો (p_guifontwide != ખાલી && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, કદ(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    નહીંતર
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    જો (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    શૂન્ય
gui_mch_wide_font_changed(શૂન્ય)
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

    જો (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, કદ(lf), &lf))
    {
	જો (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	જો (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    જો (!lf.lfItalic)
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
    પૂર્ણાંક
gui_mch_init_font(char_u *font_name, પૂર્ણાંક fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    જો (get_logfont(&lf, font_name, ખાલી, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    જો (font == NOFONT)
	પાછા FAIL;

#જો defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	પૂર્ણાંક		    feat_ગણતરી = 0;
	char_u		    *fp;

	જો (font_name != ખાલી)
	{
	    // Find each ":f" option in font_name.
	    માટે (fp = font_name; *fp != NUL; fp++)
	    {
		જો (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    પૂર્ણાંક    ti = 0;
		    unsigned પૂર્ણાંક param = 1;

		    fp += 2;  // skip ":f"
		    જ્યારે (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    જો (ti != 4)
			ચાલુ;  // invalid tag length

		    જો (*fp == '=')
		    {
			fp++;
			param = (unsigned પૂર્ણાંક)atoi((અક્ષર *)fp);
			જ્યારે (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    જો (feat_ગણતરી < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_ગણતરી].tag =
			    ((unsigned પૂર્ણાંક)tag[0])
			    | ((unsigned પૂર્ણાંક)tag[1] << 8)
			    | ((unsigned પૂર્ણાંક)tag[2] << 16)
			    | ((unsigned પૂર્ણાંક)tag[3] << 24);
			features[feat_ગણતરી].parameter = param;
			feat_ગણતરી++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_ગણતરી);
    }
#endif

    જો (font_name == ખાલી)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    જો (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    જો (p != ખાલી)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	જો (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	નહીંતર
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    જો (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    જો (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	જો (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    પાછા OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    પૂર્ણાંક
gui_mch_maximized(શૂન્ય)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = કદ(WINDOWPLACEMENT);
    જો (GetWindowPlacement(s_hwnd, &wp))
    {
	જો (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    પાછા TRUE;
	જો (wp.showCmd == SW_SHOWMINIMIZED)
	    પાછા FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	જો ((rc.જમણું - rc.ડાબું !=
		    wp.rcNormalPosition.જમણું - wp.rcNormalPosition.ડાબું)
		|| (rc.bottom - rc.ટોચ !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.ટોચ))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    શૂન્ય
gui_mch_newfont(શૂન્ય)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    જો (win_socket_id == 0)
    {
	gui_resize_shell(rect.જમણું - rect.ડાબું
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.ટોચ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    નહીંતર
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.જમણું - rect.ડાબું,
	    rect.bottom - rect.ટોચ - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    શૂન્ય
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == ખાલી ? "VIM" : (અક્ષર *)title));
}

#જો defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
સ્થિર LPCSTR mshape_idcs[] =
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

    શૂન્ય
mch_set_mouse_shape(પૂર્ણાંક shape)
{
    LPCSTR idc;

    જો (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    નહીંતર
    {
	જો (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	નહીંતર
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(ખાલી, idc));
	જો (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (શૂન્ય)GetCursorPos(&mp);
	    (શૂન્ય)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#જો defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    સ્થિર WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    પૂર્ણાંક len;
    WCHAR *res;

    tmp = convert_filter(s);
    જો (tmp == ખાલી)
	પાછા ખાલી;
    len = (પૂર્ણાંક)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    પાછા res;
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
	પૂર્ણાંક saving,
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
    પૂર્ણાંક			i;
    WCHAR		*titlep = ખાલી;
    WCHAR		*extp = ખાલી;
    WCHAR		*initdirp = ખાલી;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    જો (dflt == ખાલી)
	fileBuf[0] = NUL;
    નહીંતર
    {
	wp = enc_to_utf16(dflt, ખાલી);
	જો (wp == ખાલી)
	    fileBuf[0] = NUL;
	નહીંતર
	{
	    માટે (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# નહીંતર
    fileStruct.lStructSize = કદ(fileStruct);
# endif

    જો (title != ખાલી)
	titlep = enc_to_utf16(title, ખાલી);
    fileStruct.lpstrTitle = titlep;

    જો (ext != ખાલી)
	extp = enc_to_utf16(ext, ખાલી);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    જો (initdir != ખાલી && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, ખાલી);
	જો (initdirp != ખાલી)
	{
	    માટે (wp = initdirp; *wp != NUL; ++wp)
		જો (*wp == '/')
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
    જો (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    જો (saving)
	ret = GetSaveFileNameW(&fileStruct);
    નહીંતર
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    જો (!ret)
	પાછા ખાલી;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, ખાલી);
    જો (p == ખાલી)
	પાછા ખાલી;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    પાછા q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    સ્થિર char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    જો (res != ખાલી)
    {
	માટે (i = 0; i < s_len; ++i)
	    જો (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    નહીંતર
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    પાછા res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    પાછા gui_mch_browse(0, title, (char_u *)_("Not Used"), ખાલી,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    સ્થિર શૂન્ય
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    અક્ષર    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ખાલી, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    જો (fnames != ખાલી)
	માટે (i = 0; i < cFiles; ++i)
	{
	    જો (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, ખાલી);
	    નહીંતર
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    જો (fnames == ખાલી)
	પાછા;

    પૂર્ણાંક kbd_modifiers = get_active_modifiers();

    જો ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    જો ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    જો ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    સ્થિર પૂર્ણાંક
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    પૂર્ણાંક pos)
{
    સ્થિર UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    પૂર્ણાંક		dragging = FALSE;
    પૂર્ણાંક		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = કદ(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    જો (sb == ખાલી)
	પાછા 0;

    જો (sb->wp != ખાલી)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    નહીંતર	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->કિંમત;

    પસંદગી (code)
    {
	સ્થિતિ SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    જો (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    તોડો;
	સ્થિતિ SB_LINEDOWN:
	    val++;
	    તોડો;
	સ્થિતિ SB_LINEUP:
	    val--;
	    તોડો;
	સ્થિતિ SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    તોડો;
	સ્થિતિ SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    તોડો;
	સ્થિતિ SB_ટોચ:
	    val = 0;
	    તોડો;
	સ્થિતિ SB_BOTTOM:
	    val = sb_info->max;
	    તોડો;
	સ્થિતિ SB_ENDSCROLL:
	    જો (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		જો (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    તોડો;

	મૂળભૂત:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    પાછા 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    જો (sb->wp != ખાલી)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ડાબું) ? SBAR_જમણું : SBAR_ડાબું].id;

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

    પાછા 0;
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

સ્થિર BalloonEval  *cur_beval = ખાલી;
સ્થિર UINT_PTR	    beval_timer_id = 0;
સ્થિર DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
સ્થિર UINT	s_menu_id = 100;
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
સ્થિર PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	અચલ અક્ષર *caption);
સ્થિર LPWORD lpwAlign(LPWORD);
સ્થિર પૂર્ણાંક nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#જો defined(FEAT_MENU) && defined(FEAT_TEAROFF)
સ્થિર શૂન્ય gui_mch_tearoff(char_u *title, vimmenu_T *menu, પૂર્ણાંક initX, પૂર્ણાંક initY);
#endif
સ્થિર શૂન્ય get_dialog_font_metrics(શૂન્ય);

સ્થિર પૂર્ણાંક dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
સ્થિર શૂન્ય initialise_toolbar(શૂન્ય);
સ્થિર શૂન્ય update_toolbar_size(શૂન્ય);
સ્થિર LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_સંદેશ, WPARAM wParam, LPARAM lParam);
સ્થિર પૂર્ણાંક get_toolbar_bitmap(vimmenu_T *menu);
#નહીંતર
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
સ્થિર શૂન્ય initialise_tabline(શૂન્ય);
સ્થિર LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_સંદેશ, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
સ્થિર LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
સ્થિર char_u *GetResultStr(HWND hwnd, પૂર્ણાંક GCS, પૂર્ણાંક *lenp);
#endif
#જો defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
પ્રકારવ્યાખ્યા સંરચના tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
પ્રકારવ્યાખ્યા HANDLE HIMC;
# endif

સ્થિર HINSTANCE hLibImm = ખાલી;
સ્થિર LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
સ્થિર HIMC (WINAPI *pImmGetContext)(HWND);
સ્થિર HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
સ્થિર BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
સ્થિર BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
સ્થિર BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
સ્થિર BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
સ્થિર BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
સ્થિર BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
સ્થિર BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
સ્થિર BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
સ્થિર શૂન્ય dyn_imm_load(શૂન્ય);
#નહીંતર
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
    સ્થિર પૂર્ણાંક
gui_mswin_get_menu_height(
    પૂર્ણાંક	    fix_window)	    // If TRUE, resize window if menu height changed
{
    સ્થિર પૂર્ણાંક	old_menu_height = -1;

    RECT    rc1, rc2;
    પૂર્ણાંક	    અંક;
    પૂર્ણાંક	    menu_height;

    જો (gui.menu_is_active)
	અંક = Get_Menu_Item_ગણતરી(s_menuBar);
    નહીંતર
	અંક = 0;

    જો (અંક == 0)
	menu_height = 0;
    નહીંતર જો (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    નહીંતર
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	જો (gui.starting)
	    menu_height = rc1.bottom - rc1.ટોચ + 1;
	નહીંતર
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, અંક - 1, &rc2);
	    menu_height = rc2.bottom - rc1.ટોચ + 1;
	}
    }

    જો (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    પાછા menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    સ્થિર long
mouse_vertical_scroll_step(શૂન્ય)
{
    UINT val;
    જો (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	પાછા (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    પાછા 3; // Safe default;
}

    સ્થિર long
mouse_horizontal_scroll_step(શૂન્ય)
{
    UINT val;
    જો (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	પાછા (long)val;
    પાછા 3; // Safe default;
}

    સ્થિર શૂન્ય
init_mouse_wheel(શૂન્ય)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    સ્થિર શૂન્ય
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, પૂર્ણાંક horizontal)
{
    પૂર્ણાંક		button;
    win_T	*wp;
    પૂર્ણાંક		modifiers = 0;
    પૂર્ણાંક		kbd_modifiers;
    પૂર્ણાંક		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    જો (wp != ખાલી && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	જો (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ડાબું : MSCR_જમણું;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	નહીંતર
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

    જો (wp == ખાલી || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    જો (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    નહીંતર
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    જો ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    જો ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    જો ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    સ્થિર પૂર્ણાંક
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = કદ(NONCLIENTMETRICSW);
    જો (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    કદ(NONCLIENTMETRICSW),
	    &nm,
	    0))
	પાછા FAIL;
    *lf = nm.lfMenuFont;
    પાછા OK;
}
#endif


#જો defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    સ્થિર શૂન્ય
set_tabline_font(શૂન્ય)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    જો (gui_w32_get_menu_font(&lfSysmenu) != OK)
	પાછા;

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
#નહીંતર
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    સ્થિર LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    પસંદગી (param)
    {
	સ્થિતિ SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    તોડો;
	સ્થિતિ SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    તોડો;
	સ્થિતિ SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    તોડો;
	મૂળભૂત:
	    તોડો;
    }
    પાછા 0;
}

#ifdef FEAT_NETBEANS_INTG
    સ્થિર શૂન્ય
_OnWindowPosChanged(
    HWND hwnd,
    અચલ LPWINDOWPOS lpwpos)
{
    સ્થિર પૂર્ણાંક x = 0, y = 0, cx = 0, cy = 0;
    બાહ્ય પૂર્ણાંક WSInitialized;

    જો (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


સ્થિર HWND hwndTip = ખાલી;

    સ્થિર શૂન્ય
show_sizing_tip(પૂર્ણાંક cols, પૂર્ણાંક rows)
{
    TOOLINFOA ti;
    અક્ષર buf[32];

    ti.cbSize = કદ(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    જો (hwndTip == ખાલી)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, ખાલી,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ખાલી, GetModuleHandle(ખાલી), ખાલી);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    નહીંતર
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    સ્થિર શૂન્ય
destroy_sizing_tip(શૂન્ય)
{
    જો (hwndTip == ખાલી)
	પાછા;

    DestroyWindow(hwndTip);
    hwndTip = ખાલી;
}

    સ્થિર પૂર્ણાંક
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    પૂર્ણાંક	    w, h;
    પૂર્ણાંક	    valid_w, valid_h;
    પૂર્ણાંક	    w_offset, h_offset;
    પૂર્ણાંક	    cols, rows;

    w = lprc->જમણું - lprc->ડાબું;
    h = lprc->bottom - lprc->ટોચ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    જો (fwSide == WMSZ_ડાબું || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ડાબું += w_offset;
    નહીંતર જો (fwSide == WMSZ_જમણું || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->જમણું -= w_offset;

    જો (fwSide == WMSZ_ટોચ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->ટોચ += h_offset;
    નહીંતર જો (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    પાછા TRUE;
}

#ifdef FEAT_GUI_TABLINE
    સ્થિર શૂન્ય
_OnRButtonUp(HWND hwnd, પૂર્ણાંક x, પૂર્ણાંક y, UINT ચાવી_Flags)
{
    જો (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	જો (pt.y < rect.ટોચ)
	{
	    show_tabline_popup_menu();
	    પાછા;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, ચાવી_Flags, DefWindowProcW);
}

    સ્થિર શૂન્ય
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, પૂર્ણાંક x, પૂર્ણાંક y, UINT ચાવી_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    જો (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	જો (pt.y < rect.ટોચ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, ચાવી_Flags, DefWindowProcW);
}
#endif

    સ્થિર UINT
_OnNCHitTest(HWND hwnd, પૂર્ણાંક xPos, પૂર્ણાંક yPos)
{
    UINT	result;
    પૂર્ણાંક		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    જો (result != HTCLIENT)
	પાછા result;

#ifdef FEAT_GUI_TABLINE
    જો (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	જો (yPos < rct.ટોચ)
	    પાછા result;
    }
#endif
    (શૂન્ય)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    જો (xPos < 48) // <VN> TODO should use system metric?
	પાછા HTBOTTOMLEFT;
    નહીંતર
	પાછા HTBOTTOMRIGHT;
}

#જો defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    સ્થિર LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    પસંદગી (hdr->code)
    {
	સ્થિતિ TTN_GETDISPINFOW:
	સ્થિતિ TTN_GETDISPINFO:
	    {
		char_u		*str = ખાલી;
		સ્થિર શૂન્ય	*tt_text = ખાલી;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		જો (gui_mch_showing_tabline()
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
		    જો (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			પૂર્ણાંક idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			જો (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    જો (tp != ખાલી)
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
		નહીંતર
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(મૂળ_menu, idButton);
		    જો (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		જો (str == ખાલી)
		    તોડો;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		જો (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, ખાલી);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		નહીંતર
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    જો (len < કદ(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == ખાલી))
			vim_strncpy((char_u *)lpdi->szText, str,
				કદ(lpdi->szText) - 1);
		    નહીંતર
			lpdi->lpszText = tt_text;
		}
	    }
	    તોડો;

# ifdef FEAT_GUI_TABLINE
	સ્થિતિ TCN_SELCHANGE:
	    જો (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		પાછા 0L;
	    }
	    તોડો;

	સ્થિતિ NM_RCLICK:
	    જો (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		પાછા 0L;
	    }
	    તોડો;
# endif

	મૂળભૂત:
	    તોડો;
    }
    પાછા DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#જો defined(MENUHINTS) && defined(FEAT_MENU)
    સ્થિર LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    જો (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	સ્થિર પૂર્ણાંક  did_menu_tip = FALSE;

	જો (did_menu_tip)
	{
	    સંદેશ_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(મૂળ_menu, (UINT)LOWORD(wParam));
	જો (pMenu != ખાલી && pMenu->strings[MENU_INDEX_TIP] != ખાલી)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = કદ(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    જો (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++સંદેશ_hist_off;
		સંદેશ((અક્ષર *)pMenu->strings[MENU_INDEX_TIP]);
		--સંદેશ_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	પાછા 0L;
    }
    પાછા DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    સ્થિર BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    પૂર્ણાંક		old_width, old_height;
    પૂર્ણાંક		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, કદ(lf), &lf);
    lf.lfHeight = lf.lfHeight * (પૂર્ણાંક)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    જો (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    નહીંતર
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    પાછા TRUE;
}

    સ્થિર LRESULT
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
    InvalidateRect(hwnd, ખાલી, TRUE);

    s_in_dpichanged = FALSE;
    પાછા 0L;
}


    સ્થિર LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_સંદેશ,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_સંદેશ, lParam);

    s_u_સંદેશ = u_સંદેશ;
    s_wParam = wParam;

    પસંદગી (u_સંદેશ)
    {
	HANDLE_સંદેશ(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_સંદેશ(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_સંદેશ(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_સંદેશ(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_સંદેશ(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_સંદેશ(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_સંદેશ(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_સંદેશ(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_સંદેશ(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_સંદેશ(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_સંદેશ(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_સંદેશ(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_સંદેશ(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_સંદેશ(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_સંદેશ(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_સંદેશ(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    સ્થિતિ WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	પાછા FALSE;		// Do NOT allow system to go down.

    સ્થિતિ WM_ENDSESSION:
	જો (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    પાછા 0L;
	}
	તોડો;

    સ્થિતિ WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (પૂર્ણાંક)(short)LOWORD(lParam));
	પાછા 0L;

    સ્થિતિ WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	જો (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((પૂર્ણાંક)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (પૂર્ણાંક)(short)LOWORD(lParam));
	    પાછા 0L;
	}
#ifdef FEAT_MENU
	નહીંતર
	    પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
#endif

    સ્થિતિ WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
#નહીંતર
	પાછા 0L;
#endif

    સ્થિતિ WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	તોડો;

    સ્થિતિ WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	પાછા _DuringSizing((UINT)wParam, (LPRECT)lParam);

    સ્થિતિ WM_MOUSEWHEEL:
    સ્થિતિ WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_સંદેશ == WM_MOUSEHWHEEL);
	પાછા 0L;

	// Notification for change in SystemParametersInfo()
    સ્થિતિ WM_SETTINGCHANGE:
	પાછા _OnSettingChange((UINT)wParam);

#જો defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    સ્થિતિ WM_NOTIFY:
	પાછા _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#જો defined(MENUHINTS) && defined(FEAT_MENU)
    સ્થિતિ WM_MENUSELECT:
	પાછા _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    સ્થિતિ WM_IME_NOTIFY:
	જો (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
	પાછા 1L;

    સ્થિતિ WM_IME_COMPOSITION:
	જો (!_OnImeComposition(hwnd, wParam, lParam))
	    પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
	પાછા 1L;
#endif
    સ્થિતિ WM_GETDPISCALEDSIZE:
	પાછા _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    સ્થિતિ WM_DPICHANGED:
	પાછા _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    મૂળભૂત:
#ifdef MSWIN_FIND_REPLACE
	જો (u_સંદેશ == s_findrep_સંદેશ && s_findrep_સંદેશ != 0)
	    _OnFindRepl();
#endif
	તોડો;
    }

    પાછા DefWindowProcW(hwnd, u_સંદેશ, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = ખાલી;

    સ્થિર BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    અક્ષર	buf[2048];
    અક્ષર	*title = (અક્ષર *)lParam;

    જો (GetWindowText(hwnd, buf, કદ(buf)))
    {
	જો (strstr(buf, title) != ખાલી)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, ખાલી, "MDIClient", ખાલી);
	    જો (vim_parent_hwnd != ખાલી)
		પાછા FALSE;
	}
    }
    પાછા TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    શૂન્ય
gui_mch_set_parent(અક્ષર *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    જો (vim_parent_hwnd == ખાલી)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    સ્થિર શૂન્ય
ole_error(અક્ષર *arg)
{
    અક્ષર buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#જો defined(GUI_MAY_SPAWN)
    સ્થિર અક્ષર *
gvim_error(શૂન્ય)
{
    અક્ષર *સંદેશ = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    જો (starting)
    {
	mch_errmsg(સંદેશ);
	mch_errmsg("\n");
	mch_exit(2);
    }
    પાછા સંદેશ;
}

    અક્ષર *
gui_mch_do_spawn(char_u *arg)
{
    પૂર્ણાંક			len;
# જો defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ખાલી;
    LPWSTR		tofree1 = ખાલી;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = ખાલી, p, warg, tofree2 = ખાલી;
    STARTUPINFOW	si = {કદ(si)};
    PROCESS_INFORMATION pi;

    જો (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	જાઓ error;
    p = wcsrchr(name, L'\\');
    જો (p == ખાલી)
	જાઓ error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# નહીંતર
    wcscpy(p + 1, L"gvim.exe");
# endif

# જો defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    જો (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_લીટી_W();
	// Skip 1st argument.
	જ્યારે (*p && *p != L' ' && *p != L'\t')
	{
	    જો (*p == L'"')
	    {
		// Skip quoted strings
		જ્યારે (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# જો defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    નહીંતર
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	પૂર્ણાંક	ret;

	session = vim_tempname('s', FALSE);
	જો (session == ખાલી)
	    જાઓ error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	જો (p_bg == ખાલી)
	{
	    p_bg = savebg;
	    જાઓ error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	જો (!ret)
	    જાઓ error;
	wsession = enc_to_utf16(session, ખાલી);
	જો (wsession == ખાલી)
	    જાઓ error;
	len = (પૂર્ણાંક)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	જો (cmd == ખાલી)
	{
	    vim_free(wsession);
	    જાઓ error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    જો (arg != ખાલી)
    {
	warg = enc_to_utf16(arg, ખાલી);
	જો (warg == ખાલી)
	    જાઓ error;
	tofree2 = warg;
    }
    નહીંતર
	warg = L"";

    // Set up the new command line.
    len = (પૂર્ણાંક)wcslen(name) + (પૂર્ણાંક)wcslen(cmd) + (પૂર્ણાંક)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    જો (newcmd == ખાલી)
	જાઓ error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    જો (!CreateProcessW(ખાલી, newcmd, ખાલી, ખાલી, TRUE, 0,
		ખાલી, ખાલી, &si, &pi))
	જાઓ error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# જો defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    જો (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    પાછા gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    શૂન્ય
gui_mch_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
{
    પૂર્ણાંક		silent = FALSE;
    પૂર્ણાંક		idx;

    // Check for special OLE command line parameters
    જો ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	જો (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	નહીંતર
	    idx = 1;

	// Register Vim as an OLE Automation server
	જો (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#નહીંતર
	    જો (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	જો (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#નહીંતર
	    જો (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	જો (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#નહીંતર
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    જો (mch_is_gui_executable())
# endif
    {
	પૂર્ણાંક	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	જો (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	પૂર્ણાંક arg;

	માટે (arg = 1; arg < *argc; arg++)
	    જો (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * કદ(અક્ષર *));
		argv[*argc] = ખાલી;
		તોડો;	// enough?
	    }
    }
#endif
}

    સ્થિર શૂન્ય
load_dpi_func(શૂન્ય)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    જો (hUser32 == ખાલી)
	જાઓ fail;

    pGetDpiForSystem = (UINT (WINAPI *)(શૂન્ય))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (પૂર્ણાંક (WINAPI *)(પૂર્ણાંક, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    જો (pSetThreadDpiAwarenessContext != ખાલી)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	જો (oldctx != ખાલી)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    જો (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    પાછા;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = ખાલી;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = ખાલી;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ખાલી;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    પૂર્ણાંક
gui_mch_init(શૂન્ય)
{
    અચલ WCHAR szVimWndClassW[] = VIM_CLASSW;
    અચલ WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    જો (s_hwnd != ખાલી)
	જાઓ theend;

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
    જો (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(ખાલી, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ખાલી;
	wndclassw.lpszClassName = szVimWndClassW;

	જો (RegisterClassW(&wndclassw) == 0)
	    પાછા FAIL;
    }

    જો (vim_parent_hwnd != ખાલી)
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
		vim_parent_hwnd, ખાલી,
		g_hinst, ખાલી);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	જો (s_hwnd == ખાલી)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    નહીંતર
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	જો (IsWindow((HWND)win_socket_id) <= 0)
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
		ખાલી, ખાલી,
		g_hinst, ખાલી);
	જો (s_hwnd != ખાલી && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    જો (s_hwnd == ખાલી)
	પાછા FAIL;

    જો (pGetDpiForWindow != ખાલી)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#જો defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    જો (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ખાલી;
	wndclassw.hCursor = LoadCursor(ખાલી, IDC_ARROW);
	wndclassw.hbrBackground = ખાલી;
	wndclassw.lpszMenuName = ખાલી;
	wndclassw.lpszClassName = szTextAreaClassW;

	જો (RegisterClassW(&wndclassw) == 0)
	    પાછા FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ખાલી,
	g_hinst, ખાલી);

    જો (s_textArea == ખાલી)
	પાછા FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ખાલી;

	જો (mch_icon_load(&hIcon) == OK && hIcon != ખાલી)
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
    s_findrep_સંદેશ = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = કદ(s_findrep_struct);
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
    જો (p_rop)
	(શૂન્ય)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    પાછા OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    સ્થિર શૂન્ય
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    જો (mon != ખાલી)
    {
	moninfo.cbSize = કદ(MONITORINFO);
	જો (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    પાછા;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    શૂન્ય
gui_mch_set_shellsize(
	પૂર્ણાંક width,
	પૂર્ણાંક height,
	પૂર્ણાંક min_width UNUSED,
	પૂર્ણાંક min_height UNUSED,
	પૂર્ણાંક base_width UNUSED,
	પૂર્ણાંક base_height UNUSED,
	પૂર્ણાંક direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    પૂર્ણાંક		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    જો (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    જો (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    નહીંતર
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
    window_rect.જમણું = window_rect.ડાબું + win_width;
    window_rect.bottom = window_rect.ટોચ + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    જો (!s_in_dpichanged)
    {
	જો ((direction & RESIZE_HOR)
		&& window_rect.જમણું > workarea_rect.જમણું)
	    OffsetRect(&window_rect,
		    workarea_rect.જમણું - window_rect.જમણું, 0);

	જો ((direction & RESIZE_HOR)
		&& window_rect.ડાબું < workarea_rect.ડાબું)
	    OffsetRect(&window_rect,
		    workarea_rect.ડાબું - window_rect.ડાબું, 0);

	જો ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	જો ((direction & RESIZE_VERT)
		&& window_rect.ટોચ < workarea_rect.ટોચ)
	    OffsetRect(&window_rect,
		    0, workarea_rect.ટોચ - window_rect.ટોચ);
    }

    MoveWindow(s_hwnd, window_rect.ડાબું, window_rect.ટોચ,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    શૂન્ય
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    જ્યારે (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    જો (sb->scroll_shift > 0)
	++size;

    info.cbSize = કદ(info);
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
    શૂન્ય
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    શૂન્ય
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    શૂન્ય
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    શૂન્ય
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
    સ્થિર LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    જો (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	પાછા lResult;
    પસંદગી (dwCommand)
    {
	સ્થિતિ IMN_SETOPENSTATUS:
	    જો (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		જો (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		જો (State & MODE_ઉમેરો)
		{
# જો defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    જો (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			પૂર્ણાંક old_row = gui.row;
			પૂર્ણાંક old_col = gui.col;

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
	    તોડો;
    }
    pImmReleaseContext(hWnd, hImc);
    પાછા lResult;
}

    સ્થિર LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    પૂર્ણાંક		len;

    જો ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	પાછા 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    જો (ret != ખાલી)
    {
	add_to_આગત_buf_csi(ret, len);
	vim_free(ret);
	પાછા 1;
    }
    પાછા 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    સ્થિર char_u *
GetResultStr(HWND hwnd, પૂર્ણાંક GCS, પૂર્ણાંક *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ખાલી;
    char_u	*convbuf = ખાલી;

    જો (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	પાછા ખાલી;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ખાલી, 0);
    જો (ret <= 0)
	પાછા ખાલી;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + કદ(WCHAR));
    જો (buf == ખાલી)
	પાછા ખાલી;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / કદ(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    પાછા convbuf;
}
#endif

// For global functions we need prototypes.
#જો defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    શૂન્ય
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    જો (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    શૂન્ય
im_set_position(પૂર્ણાંક row, પૂર્ણાંક col)
{
    HIMC hImc;

    જો (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	જો (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    શૂન્ય
im_set_active(પૂર્ણાંક active)
{
    HIMC	hImc;
    સ્થિર HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    જો (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	પાછા;
    }
# endif

    જો (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	પાછા;

    જો (p_imdisable)
    {
	જો (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    જો (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    નહીંતર જો (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    જો (!hImc)
	પાછા;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    જો (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	સ્થિર DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	સ્થિર BOOL bSaved = FALSE;

	જો (active)
	{
	    // if we have a saved conversion status, restore it
	    જો (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	નહીંતર
	{
	    // save conversion status and disable korean
	    જો (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    પૂર્ણાંક
im_get_status(શૂન્ય)
{
    પૂર્ણાંક		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    જો (!gui.in_use && !gui.starting)
	પાછા mbyte_im_get_status();
# endif

    જો (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    પાછા status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    સ્થિર શૂન્ય
latin9_to_ucs(char_u *text, પૂર્ણાંક len, WCHAR *unicodebuf)
{
    પૂર્ણાંક		c;

    જ્યારે (--len >= 0)
    {
	c = *text++;
	પસંદગી (c)
	{
	    સ્થિતિ 0xa4: c = 0x20ac; તોડો;   // euro
	    સ્થિતિ 0xa6: c = 0x0160; તોડો;   // S hat
	    સ્થિતિ 0xa8: c = 0x0161; તોડો;   // S -hat
	    સ્થિતિ 0xb4: c = 0x017d; તોડો;   // Z hat
	    સ્થિતિ 0xb8: c = 0x017e; તોડો;   // Z -hat
	    સ્થિતિ 0xbc: c = 0x0152; તોડો;   // OE
	    સ્થિતિ 0xbd: c = 0x0153; તોડો;   // oe
	    સ્થિતિ 0xbe: c = 0x0178; તોડો;   // Y
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
    સ્થિર શૂન્ય
RevOut( HDC hdc,
	પૂર્ણાંક col,
	પૂર્ણાંક row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    પૂર્ણાંક		ix;

    માટે (ix = 0; ix < (પૂર્ણાંક)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    સ્થિર શૂન્ય
draw_લીટી(
    પૂર્ણાંક		x1,
    પૂર્ણાંક		y1,
    પૂર્ણાંક		x2,
    પૂર્ણાંક		y2,
    COLORREF	color)
{
#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_લીટી(s_dwc, x1, y1, x2, y2, color);
    નહીંતર
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, ખાલી);
	// Note: LineTo() excludes the last pixel in the line.
	લીટી_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    સ્થિર શૂન્ય
set_pixel(
    પૂર્ણાંક		x,
    પૂર્ણાંક		y,
    COLORREF	color)
{
#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    નહીંતર
#endif
	SetPixel(s_hdc, x, y, color);
}

    સ્થિર શૂન્ય
fill_rect(
    અચલ RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    નહીંતર
#endif
    {
	HBRUSH	hbr2;

	જો (hbr == ખાલી)
	    hbr2 = CreateSolidBrush(color);
	નહીંતર
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	જો (hbr == ખાલી)
	    DeleteBrush(hbr2);
    }
}

    શૂન્ય
gui_mch_draw_string(
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    char_u	*text,
    પૂર્ણાંક		len,
    પૂર્ણાંક		flags)
{
    સ્થિર પૂર્ણાંક	*padding = ખાલી;
    સ્થિર પૂર્ણાંક	pad_size = 0;
    અચલ RECT	*pcliprect = ખાલી;
    UINT	foptions = 0;
    સ્થિર WCHAR *unicodebuf = ખાલી;
    સ્થિર પૂર્ણાંક   *unicodepdy = ખાલી;
    સ્થિર પૂર્ણાંક	unibuflen = 0;
    પૂર્ણાંક		n = 0;
    પૂર્ણાંક		y;

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
    સ્થિર HBRUSH	hbr_cache[2] = {ખાલી, ખાલી};
    સ્થિર guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    સ્થિર પૂર્ણાંક		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    જો (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ડાબું = FILL_X(col);
	rc.ટોચ = FILL_Y(row);
	જો (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.જમણું = FILL_X(col + mb_string2cells(text, len));
	}
	નહીંતર
	    rc.જમણું = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	જો (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	નહીંતર જો (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	નહીંતર
	{
	    જો (hbr_cache[brush_lru] != ખાલી)
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
	જો (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	નહીંતર જો (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    જો (pad_size != Columns || padding == ખાલી || padding[0] != gui.char_width)
    {
	પૂર્ણાંક	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(પૂર્ણાંક, pad_size);
	જો (padding != ખાલી)
	    માટે (i = 0; i < pad_size; i++)
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
    જો (enc_utf8)
	માટે (n = 0; n < len; ++n)
	    જો (text[n] >= 0x80)
		તોડો;

#જો defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    જો (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    જો ((enc_utf8
		|| (enc_codepage > 0 && (પૂર્ણાંક)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ખાલી || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(પૂર્ણાંક, len);

	unibuflen = len;
    }

    જો (enc_utf8 && n < len && unicodebuf != ખાલી)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	પૂર્ણાંક		i;
	પૂર્ણાંક		wlen;	// string length in words
	પૂર્ણાંક		cells;	// cell width of string up to composing char
	પૂર્ણાંક		cw;	// width of current cell
	પૂર્ણાંક		c;

	wlen = 0;
	cells = 0;
	માટે (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    જો (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    નહીંતર
	    {
		unicodebuf[wlen++] = c;
	    }

	    જો (utf_iscomposing(c))
		cw = 0;
	    નહીંતર
	    {
		cw = utf_char2cells(c);
		જો (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    જો (unicodepdy != ખાલી)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		જો (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		નહીંતર
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#જો defined(FEAT_DIRECTX)
	જો (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	નહીંતર
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    નહીંતર જો ((enc_codepage > 0 && (પૂર્ણાંક)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	જો (unicodebuf != ખાલી)
	{
	    જો (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    નહીંતર
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(અક્ષર *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    જો (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		જો (unicodepdy != ખાલી)
		{
		    પૂર્ણાંક i;
		    પૂર્ણાંક cw;

		    માટે (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			જો (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    નહીંતર
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	જો (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (અક્ષર *)text, len, padding);
	નહીંતર
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (અક્ષર *)text, len, padding);
    }

    // Underline
    જો (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	જો (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_લીટી(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    જો (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_લીટી(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    જો (flags & DRAW_UNDERC)
    {
	પૂર્ણાંક			x;
	પૂર્ણાંક			offset;
	સ્થિર અચલ પૂર્ણાંક	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	માટે (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    શૂન્ય
gui_mch_flush(શૂન્ય)
{
#જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    સ્થિર શૂન્ય
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ખાલી, gui.back_pixel);
}


    શૂન્ય
gui_mch_get_screen_dimensions(પૂર્ણાંક *screen_w, પૂર્ણાંક *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.જમણું - workarea_rect.ડાબું
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.ટોચ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#જો defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    શૂન્ય
gui_mch_add_menu(
    vimmenu_T	*menu,
    પૂર્ણાંક		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    જો (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, ખાલી);
	જો (wn == ખાલી)
	    પાછા;

	infow.cbSize = કદ(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ઉમેરો_Menu_Item_W((parent == ખાલી)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    જો (parent == ખાલી)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    નહીંતર જો (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    શૂન્ય
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (શૂન્ય)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (પૂર્ણાંક)mp.x, (પૂર્ણાંક)mp.y);
}

    શૂન્ય
gui_make_popup(char_u *path_name, પૂર્ણાંક mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    જો (menu == ખાલી)
	પાછા;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    જો (mouse_pos)
    {
	પૂર્ણાંક	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    નહીંતર જો (curwin != ખાલી)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    સંદેશ_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (પૂર્ણાંક)p.x, (પૂર્ણાંક)p.y);
}

# જો defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    શૂન્ય
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    જો (menu != ખાલી)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    શૂન્ય
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    પૂર્ણાંક		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = ખાલી;

# ifdef FEAT_TEAROFF
    જો (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ઉમેરો_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    નહીંતર
# endif
# ifdef FEAT_TOOLBAR
    જો (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	જો (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	નહીંતર
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
    નહીંતર
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, ખાલી);
	જો (wn != ખાલી)
	{
	    ઉમેરો_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	જો (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    શૂન્ય
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    જો (menu->submenu_id == (HMENU)-1)
    {
	પૂર્ણાંક iButton;

	iButton = (પૂર્ણાંક)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    નહીંતર
# endif
    {
	જો (menu->parent != ખાલી
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != ખાલી)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	નહીંતર
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	જો (menu->submenu_id != ખાલી)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	જો (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	જો (menu->parent != ખાલી
		&& menu->parent->children != ખાલી
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
    સ્થિર શૂન્ય
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    પૂર્ણાંક		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    જો (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ડાબું - rct.ડાબું;
	y = (trect.ટોચ -  rct.bottom  + roct.bottom);
    }
    નહીંતર
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    જો (menu->children != ખાલી)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	જો (IsWindow(menu->tearoff_handle))
	    (શૂન્ય) SetWindowPos(menu->tearoff_handle,
				ખાલી,
				(પૂર્ણાંક)trect.ડાબું,
				(પૂર્ણાંક)trect.ટોચ,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    શૂન્ય
gui_mch_menu_grey(
    vimmenu_T	*menu,
    પૂર્ણાંક	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    જો (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    નહીંતર
# endif
	(શૂન્ય)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    જો ((menu->parent != ખાલી) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	જો (menu->children == ખાલી)
	    menuID = (WORD)(menu->id);
	નહીંતર
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	જો (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#જો defined(FEAT_GUI_DIALOG)
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
    સ્થિર LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    જો (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(શૂન્ય)SetFocus(hwnd);
	જો (dialog_default_button > IDCANCEL)
	    (શૂન્ય)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	નહીંતર
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (શૂન્ય)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	પાછા FALSE;
    }

    જો (message == WM_COMMAND)
    {
	પૂર્ણાંક	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	જો (button >= DLG_NONBUTTON_CONTROL)
	    પાછા TRUE;

	// If the edit box exists, copy the string.
	જો (s_textfield != ખાલી)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, ખાલી);
	    જો (p != ખાલી)
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
	જો (button == IDOK)
	{
	    જો (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	નહીંતર
	    EndDialog(hwnd, button - IDCANCEL);
	પાછા TRUE;
    }

    જો ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	પાછા TRUE;
    }
    પાછા FALSE;
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

સ્થિર અચલ અક્ષર *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    પૂર્ણાંક
gui_mch_dialog(
    પૂર્ણાંક		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    પૂર્ણાંક		 dfltbutton,
    char_u	*textfield,
    પૂર્ણાંક		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    પૂર્ણાંક		અંક_Buttons;
    પૂર્ણાંક		*buttonWidths, *buttonPositions;
    પૂર્ણાંક		buttonYpos;
    પૂર્ણાંક		nchar, i;
    DWORD	lStyle;
    પૂર્ણાંક		dlgwidth = 0;
    પૂર્ણાંક		dlgheight;
    પૂર્ણાંક		editboxheight;
    પૂર્ણાંક		horizWidth = 0;
    પૂર્ણાંક		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    પૂર્ણાંક		fontHeight;
    પૂર્ણાંક		textWidth, minButtonWidth, messageWidth;
    પૂર્ણાંક		maxDialogWidth;
    પૂર્ણાંક		maxDialogHeight;
    પૂર્ણાંક		scroll_flag = 0;
    પૂર્ણાંક		vertical;
    પૂર્ણાંક		dlgPaddingX;
    પૂર્ણાંક		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    પૂર્ણાંક		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    પૂર્ણાંક		l;
    પૂર્ણાંક		dlg_icon_width;
    પૂર્ણાંક		dlg_icon_height;
    પૂર્ણાંક		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    જો (!(gui.in_use || gui.starting))
#  endif
	જો (silent_mode)
	    પાછા dfltbutton;   // return default option
# endif

    જો (s_hwnd == ખાલી)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    નહીંતર
	dpi = pGetDpiForSystem();

    જો ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    જો (p == ખાલી)
	પાછા -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    જો (tbuffer == ખાલી)
	પાછા -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    અંક_Buttons = 1;
    માટે (i = 0; tbuffer[i] != '\0'; i++)
    {
	જો (tbuffer[i] == DLG_BUTTON_SEP)
	    અંક_Buttons++;
    }
    જો (dfltbutton >= અંક_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(પૂર્ણાંક, અંક_Buttons);
    જો (buttonWidths == ખાલી)
	પાછા -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(પૂર્ણાંક, અંક_Buttons);
    જો (buttonPositions == ખાલી)
	પાછા -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    જો (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    નહીંતર
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
    જો (s_hwnd == ખાલી)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.જમણું - workarea_rect.ડાબું - 100;
	જો (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.ટોચ - 150;
    }
    નહીંતર
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.જમણું - rect.ડાબું
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	જો (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.ટોચ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	જો (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, કદ(અક્ષર), 500);
    કરો
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = ખાલી;
	માટે (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    જો (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    જો (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		જો (last_white != ખાલી)
		{
		    // break the line just after a space
		    જો (pend > last_white)
			ga.ga_len -= (પૂર્ણાંક)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = ખાલી;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ચાલુ;
	    }

	    જ્યારે (--l >= 0)
		ga_append(&ga, *pend++);
	}
	જો (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } જ્યારે (*pend != NUL);

    જો (ga.ga_data != ખાલી)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    જો (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ખાલી && vim_strchr(p_go, GO_VERTICAL) != ખાલી);
    જો (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	કરો
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    જો (pend == ખાલી)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (પૂર્ણાંક)(pend - pstart));
	    જો (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} જ્યારે (*pend != NUL);

	જો (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	નહીંતર જો (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    જો (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	કરો
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    જો (pend == ખાલી)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (પૂર્ણાંક)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    જો (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} જ્યારે (*pend != NUL);
    }

    જો (dlgwidth < DLG_MIN_WIDTH)
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
    જો (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * અંક_Buttons;
    નહીંતર
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    જો (textfield != ખાલી)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    જો (dlgheight > maxDialogHeight)
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
    જો (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (પૂર્ણાંક)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    નહીંતર
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    જો (textfield != ખાલી)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    જો (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    માટે (i = 0; i < અંક_Buttons; i++)
    {
	// get end of this button.
	માટે (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	જો (*pend)
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
	જો (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (અક્ષર *)pstart);
	}
	નહીંતર
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (અક્ષર *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += અંક_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_ડાબું|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (અક્ષર *)message);

    // Edit box
    જો (textfield != ખાલી)
    {
	p = add_dialog_element(p, ES_ડાબું|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (અક્ષર *)textfield);
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
    જો (textfield != ખાલી)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    નહીંતર
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ખાલી;
    }

    // show the dialog box modally and get a return value
    nchar = (પૂર્ણાંક)DialogBoxIndirect(
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
    (શૂન્ય)SetFocus(s_hwnd);

    પાછા nchar;
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
    સ્થિર PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    અચલ અક્ષર *caption)
{
    પૂર્ણાંક nchar;

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

    પાછા p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    સ્થિર LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    પાછા (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    સ્થિર પૂર્ણાંક
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    પૂર્ણાંક		nChar = 0;
    પૂર્ણાંક		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    પૂર્ણાંક		i;
    WCHAR	*wn;

    જો (use_enc && enc_codepage >= 0 && (પૂર્ણાંક)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, ખાલી);
	જો (wn != ખાલી)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (પૂર્ણાંક)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    જો (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    માટે (i = 0; i < nChar; ++i)
	જો (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    પાછા nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    સ્થિર HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    માટે ( ; menu != ખાલી; menu = menu->next)
    {
	જો (menu->modes == 0)	// this menu has just been deleted
	    ચાલુ;
	જો (menu_is_separator(menu->dname))
	    ચાલુ;
	જો ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    પાછા menu->submenu_id;
    }
    પાછા ખાલી;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    સ્થિર LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    જો (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	પાછા (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    જો (message == WM_COMMAND)
    {
	જો ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    જો (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(શૂન્ય)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (પૂર્ણાંક)rect.જમણું - 8,
			 (પૂર્ણાંક)mp.y,
			 (પૂર્ણાંક)0,	    // reserved param
			 s_hwnd,
			 ખાલી);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	નહીંતર
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(શૂન્ય)SetFocus(s_hwnd);
	પાછા TRUE;
    }
    જો ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	પાછા TRUE;
    }

    // When moved around, give main window the focus back.
    જો (message == WM_EXITSIZEMOVE)
	(શૂન્ય)SetActiveWindow(s_hwnd);

    પાછા FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    સ્થિર શૂન્ય
get_dialog_font_metrics(શૂન્ય)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    જો (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    નહીંતર
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

#જો defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    સ્થિર શૂન્ય
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    પૂર્ણાંક		initX,
    પૂર્ણાંક		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    પૂર્ણાંક		template_len;
    પૂર્ણાંક		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*ટોચ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    પૂર્ણાંક		col, spaceWidth, len;
    પૂર્ણાંક		columnWidths[2];
    char_u	*label, *text;
    પૂર્ણાંક		acLen = 0;
    પૂર્ણાંક		nameLen;
    પૂર્ણાંક		padding0, padding1, padding2 = 0;
    પૂર્ણાંક		sepPadding=0;
    પૂર્ણાંક		x;
    પૂર્ણાંક		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    પૂર્ણાંક		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    જો (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	જો (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, ખાલી, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	પાછા;
    }

    /*
     * Create a new tearoff.
     */
    જો (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    જો (p == ખાલી)
	પાછા;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    જો (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    નહીંતર
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
    માટે (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    જો (text != ખાલી && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (પૂર્ણાંક)STRLEN(text));
		જો (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    જો (pmenu->children != ખાલી)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    જો (columnWidths[1] == 0)
    {
	// no accelerators
	જો (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	નહીંતર
	    columnWidths[0] += spaceWidth;
    }
    નહીંતર
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    જો (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (પૂર્ણાંક)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (પૂર્ણાંક)STRLEN(title));
    જો (textWidth > dlgwidth)
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
    જો (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    નહીંતર
	*p++ = PixelToDialogX(initX); // x
    જો (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    નહીંતર
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
    જો (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (પૂર્ણાંક)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    નહીંતર
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
    જો (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    નહીંતર
	menu = menu->children;
    ટોચ_menu = menu;
    માટે ( ; menu != ખાલી; menu = menu->next)
    {
	જો (menu->modes == 0)	// this menu has just been deleted
	    ચાલુ;
	જો (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ચાલુ;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	જો (((અક્ષર *)p - (અક્ષર *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    જો (newp != ખાલી)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (અક્ષર *)p - (અક્ષર *)pdlgtemplate);
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
	len = nameLen = (પૂર્ણાંક)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (પૂર્ણાંક)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	જો (menu->actext != ખાલી)
	{
	    acLen = (પૂર્ણાંક)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	નહીંતર
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	જો (menu->children == ખાલી)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	નહીંતર
	{
	    len += (પૂર્ણાંક)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	જો (label == ખાલી)
	    તોડો;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	જો (text == ખાલી)
	    text = label + nameLen;	    // no actext, use whole name
	જ્યારે (padding0-- > 0)
	    *text++ = ' ';
	જો (menu->actext != ખાલી)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	જ્યારે (padding1-- > 0)
	    *text++ = ' ';
	જો (menu->children != ખાલી)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	નહીંતર
	{
	    જ્યારે (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_ડાબું,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (અક્ષર *)label);
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
	    (LPARAM)ટોચ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (શૂન્ય)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#જો defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    સ્થિર શૂન્ય
initialise_toolbar(શૂન્ય)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ખાલી,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    કદ(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ખાલી);

    update_toolbar_size();
}

    સ્થિર શૂન્ય
update_toolbar_size(શૂન્ય)
{
    પૂર્ણાંક		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = કદ(TBMETRICS);
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

    સ્થિર LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_સંદેશ,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_સંદેશ, lParam);
    પાછા CallWindowProc(s_toolbar_wndproc, hwnd, u_સંદેશ, wParam, lParam);
}

    સ્થિર પૂર્ણાંક
get_toolbar_bitmap(vimmenu_T *menu)
{
    પૂર્ણાંક i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    જો (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ખાલી;

	જો (menu->iconfile != ખાલી)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			ખાલી,
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
	જો (hbitmap == ખાલી
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ખાલી ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    ખાલી,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	જો (hbitmap != ખાલી)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = ખાલી;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (પૂર્ણાંક)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    જો (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_ગણતરી)
	i = menu->iconidx;

    પાછા i;
}
#endif

#જો defined(FEAT_GUI_TABLINE)
    સ્થિર શૂન્ય
initialise_tabline(શૂન્ય)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ખાલી, g_hinst, ખાલી);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    સ્થિર tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = ખાલી;

    જો (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	જો (s_tabhwnd == hWnd)
	{
	    પૂર્ણાંક idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    જો (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    પાછા ptp;
}

સ્થિર POINT	    s_pt = {0, 0};
સ્થિર HCURSOR      s_hCursor = ખાલી;

    સ્થિર LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_સંદેશ,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    પૂર્ણાંક		nCenter;
    પૂર્ણાંક		idx0;
    પૂર્ણાંક		idx1;

    HandleMouseHide(u_સંદેશ, lParam);

    પસંદગી (u_સંદેશ)
    {
	સ્થિતિ WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		તોડો;
	    }
	સ્થિતિ WM_MOUSEMOVE:
	    જો (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		જો (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(ખાલી, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    જો (tp != ખાલી)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.ડાબું + (rect.જમણું - rect.ડાબું) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			જો ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			નહીંતર જો ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    તોડો;
	સ્થિતિ WM_LBUTTONUP:
	    {
		જો (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		તોડો;
	    }
	સ્થિતિ WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		જો (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		તોડો;
	    }
	મૂળભૂત:
	    તોડો;
    }

    પાછા CallWindowProc(s_tabline_wndproc, hwnd, u_સંદેશ, wParam, lParam);
}
#endif

#જો defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
{
    જો (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#જો defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    સ્થિર શૂન્ય
dyn_imm_load(શૂન્ય)
{
    hLibImm = vimLoadLib("imm32.dll");
    જો (hLibImm == ખાલી)
	પાછા;

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

    જો (       pImmGetCompositionStringW == ખાલી
	    || pImmGetContext == ખાલી
	    || pImmAssociateContext == ખાલી
	    || pImmReleaseContext == ખાલી
	    || pImmGetOpenStatus == ખાલી
	    || pImmSetOpenStatus == ખાલી
	    || pImmGetCompositionFontW == ખાલી
	    || pImmSetCompositionFontW == ખાલી
	    || pImmSetCompositionWindow == ખાલી
	    || pImmGetConversionStatus == ખાલી
	    || pImmSetConversionStatus == ખાલી)
    {
	FreeLibrary(hLibImm);
	hLibImm = ખાલી;
	pImmGetContext = ખાલી;
	પાછા;
    }

    પાછા;
}

#endif

#જો defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

પ્રકારવ્યાખ્યા સંરચના _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    શૂન્ય
gui_mch_drawsign(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક typenr)
{
    signicon_t *sign;
    પૂર્ણાંક x, y, w, h;

    જો (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == ખાલી)
	પાછા;

# જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    પસંદગી (sign->uType)
    {
	સ્થિતિ IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    તોડો;
	સ્થિતિ IMAGE_ICON:
	સ્થિતિ IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ખાલી, DI_NORMAL);
	    તોડો;
# ifdef FEAT_XPM_W32
	સ્થિતિ IMAGE_XPM:
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
	    તોડો;
# endif
    }
}

    સ્થિર શૂન્ય
close_signicon_image(signicon_t *sign)
{
    જો (sign == ખાલી)
	પાછા;

    પસંદગી (sign->uType)
    {
	સ્થિતિ IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    તોડો;
	સ્થિતિ IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    તોડો;
	સ્થિતિ IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    તોડો;
# ifdef FEAT_XPM_W32
	સ્થિતિ IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    તોડો;
# endif
    }
}

    શૂન્ય *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ખાલી;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    જો (ext > signfile)
    {
	પૂર્ણાંક do_load = 1;

	જો (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	નહીંતર જો (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	નહીંતર જો (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	નહીંતર
	    do_load = 0;

	જો (do_load)
	    sign.hImage = (HANDLE)LoadImage(ખાલી, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	જો (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((અક્ષર *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ખાલી;
    જો (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ખાલી)
	*psign = sign;

    જો (!psign)
    {
	જો (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    પાછા (શૂન્ય *)psign;

}

    શૂન્ય
gui_mch_destroy_sign(શૂન્ય *sign)
{
    જો (sign == ખાલી)
	પાછા;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#જો defined(FEAT_BEVAL_GUI)

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

    સ્થિર શૂન્ય
make_tooltip(BalloonEval *beval, અક્ષર *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    જો (pti == ખાલી)
	પાછા;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ખાલી, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, ખાલી, g_hinst, ખાલી);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = કદ(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, ખાલી);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    જો (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.જમણું);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ડાબું = pt.x - 3;
    pti->rect.ટોચ = pt.y - 3;
    pti->rect.જમણું = pt.x + 3;
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

    સ્થિર શૂન્ય
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    સ્થિર VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_સંદેશ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    જો (cur_beval == ખાલી || cur_beval->showState == ShS_SHOWING || !p_beval)
	પાછા;

    GetCursorPos(&pt);
    જો (WindowFromPoint(pt) != s_textArea)
	પાછા;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    જો (!PtInRect(&rect, pt))
	પાછા;

    જો (last_user_activity > 0
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

	જો (cur_beval->સંદેશ_CB != ખાલી)
	    (*cur_beval->સંદેશ_CB)(cur_beval, 0);
    }
}

    શૂન્ય
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    શૂન્ય
gui_mch_enable_beval_area(BalloonEval *beval)
{
    જો (beval == ખાલી)
	પાછા;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    શૂન્ય
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->સંદેશ);
    beval->સંદેશ = mesg == ખાલી ? ખાલી : vim_strsave(mesg);
    જો (beval->સંદેશ == ખાલી)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	પાછા;
    }

    જો (beval->showState == ShS_SHOWING)
	પાછા;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    જો (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (અક્ષર *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    શૂન્ય	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    શૂન્ય	(*mesgCB)(BalloonEval *, પૂર્ણાંક),
    શૂન્ય	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    જો (mesg != ખાલી && mesgCB != ખાલી)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	પાછા ખાલી;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    જો (beval != ખાલી)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->સંદેશ = mesg;
	beval->સંદેશ_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	જો (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    પાછા beval;
}

    સ્થિર શૂન્ય
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    જો (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	પાછા;

    જો (cur_beval == ખાલી)
	પાછા;

    પસંદગી (pnmh->code)
    {
	સ્થિતિ TTN_SHOW:
	    તોડો;
	સ્થિતિ TTN_કાઢો: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    તોડો;
	સ્થિતિ TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    તોડો;
	સ્થિતિ TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    તોડો;
    }
}

    સ્થિર શૂન્ય
track_user_activity(UINT u_સંદેશ)
{
    જો ((u_સંદેશ >= WM_MOUSEFIRST && u_સંદેશ <= WM_MOUSELAST)
	    || (u_સંદેશ >= WM_KEYFIRST && u_સંદેશ <= WM_KEYLAST))
	last_user_activity = Get_Tick_ગણતરી();
}

    શૂન્ય
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#જો defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    શૂન્ય
netbeans_draw_multisign_indicator(પૂર્ણાંક row)
{
    પૂર્ણાંક i;
    પૂર્ણાંક y;
    પૂર્ણાંક x;

    જો (!netbeans_active())
	પાછા;

    x = 0;
    y = TEXT_Y(row);

# જો defined(FEAT_DIRECTX)
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    માટે (i = 0; i < gui.char_height - 3; i++)
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

#જો defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    સ્થિર પૂર્ણાંક
test_gui_w32_sendevent_mouse(dict_T *args)
{
    જો (!dict_has_ચાવી(args, "row") || !dict_has_ચાવી(args, "col"))
	પાછા FALSE;

    // Note: "move" is optional, requires fewer arguments
    પૂર્ણાંક move = (પૂર્ણાંક)dict_get_bool(args, "move", FALSE);

    જો (!move && (!dict_has_ચાવી(args, "button")
	    || !dict_has_ચાવી(args, "multiclick")
	    || !dict_has_ચાવી(args, "modifiers")))
	પાછા FALSE;

    પૂર્ણાંક row = (પૂર્ણાંક)dict_get_સંખ્યા(args, "row");
    પૂર્ણાંક col = (પૂર્ણાંક)dict_get_સંખ્યા(args, "col");

    જો (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	જો (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    પૂર્ણાંક pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    પૂર્ણાંક pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	નહીંતર
	    gui_mouse_moved(col, row);
    }
    નહીંતર
    {
	પૂર્ણાંક button = (પૂર્ણાંક)dict_get_સંખ્યા(args, "button");
	પૂર્ણાંક repeated_click = (પૂર્ણાંક)dict_get_સંખ્યા(args, "multiclick");
	int_u mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    પાછા TRUE;
}

    સ્થિર પૂર્ણાંક
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    આગત_2 inputs[1];
    આગત_2 modkeys[3];
    SecureZeroMemory(inputs, કદ(આગત_2));
    SecureZeroMemory(modkeys, 3 * કદ(આગત_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    જો (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_સંખ્યા_def(args, "keycode", 0);
	જો (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    પાછા FALSE;
	}

	BOOL is_Mod_ચાવી = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	પૂર્ણાંક mods = (પૂર્ણાંક)dict_get_સંખ્યા(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	જો (mods && STRICMP(event, "keydown") == 0 && !is_Mod_ચાવી)
	{
	    પૂર્ણાંક n = 0;
	    જો (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = આગત_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    જો (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = આગત_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    જો (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = આગત_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    જો (n)
	    {
		(શૂન્ય)SetForegroundWindow(s_hwnd);
		Send_આગત(n, modkeys, કદ(આગત_2));
	    }
	}

	inputs[0].type = આગત_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	જો (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    જો (!is_Mod_ચાવી)
		unwrapMods = TRUE;
	}

	(શૂન્ય)SetForegroundWindow(s_hwnd);
	Send_આગત(ARRAYSIZE(inputs), inputs, કદ(આગત_2));
	vim_free(event);

	જો (unwrapMods)
	{
	    modkeys[0].type = આગત_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = આગત_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = આગત_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (શૂન્ય)SetForegroundWindow(s_hwnd);
	    Send_આગત(3, modkeys, કદ(આગત_2));
	}
    }
    નહીંતર
    {
	જો (event == ખાલી)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	નહીંતર
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	પાછા FALSE;
    }
    પાછા TRUE;
}

    સ્થિર પૂર્ણાંક
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    પૂર્ણાંક handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    જો (strategy)
    {
	જો (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	નહીંતર જો (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    જો (!handled)
    {
	જો (strategy == ખાલી)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	નહીંતર
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	પાછા FALSE;
    }
    પાછા TRUE;
}


    પૂર્ણાંક
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    જો (STRICMP(event, "key") == 0)
	પાછા test_gui_w32_sendevent_keyboard(args);
    નહીંતર જો (STRICMP(event, "mouse") == 0)
	પાછા test_gui_w32_sendevent_mouse(args);
    નહીંતર જો (STRICMP(event, "set_keycode_trans_strategy") == 0)
	પાછા test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    નહીંતર
    {
	semsg(_(e_invalid_કિંમત_for_argument_str_str), "event", event);
	પાછા FALSE;
    }
}
#endif

#જો defined(FEAT_IMAGE_GUI)

પ્રકારવ્યાખ્યા સંરચના
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
    સ્થિર શૂન્ય
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, પૂર્ણાંક iw, પૂર્ણાંક ih, image_format_T fmt)
{
    પૂર્ણાંક		y, x;
    પૂર્ણાંક		stride = iw * 4;
    પૂર્ણાંક		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    માટે (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	માટે (x = 0; x < iw; x++)
	{
	    જો (fmt == IMAGE_FORMAT_RGBA)
	    {
		પૂર્ણાંક a = s[3];

		જો (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		નહીંતર જો (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		નહીંતર
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    નહીંતર
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

    પૂર્ણાંક
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    પૂર્ણાંક		iw, ih;
    શૂન્ય	*bits = ખાલી;

    જો (ctx == ખાલી)
	પાછા FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = કદ(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_ગણતરી = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, ખાલી, 0);
    જો (hbm == ખાલી || bits == ખાલી)
    {
	જો (hbm != ખાલી)
	    DeleteObject(hbm);
	પાછા false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    જો (mem_dc == ખાલી)
    {
	DeleteObject(hbm);
	પાછા false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    પાછા OK;
}

    શૂન્ય
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    પૂર્ણાંક
image_placement_gui_init(image_placement_T *place UNUSED)
{
    પાછા OK;
}

    શૂન્ય
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    શૂન્ય
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    પૂર્ણાંક		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    જો (rects == ખાલી || n_rects == 0)
	પાછા;

# જો defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    જો (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    માટે (પૂર્ણાંક i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	પૂર્ણાંક		row, col;
	પૂર્ણાંક		posx, posy;
	પૂર્ણાંક		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    શૂન્ય
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
