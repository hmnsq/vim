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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_சாவி_OFF			0	// no dead key
#define DEAD_சாவி_SET_DEFAULT		1	// dead key pressed
#define DEAD_சாவி_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_சாவி_SKIP_ON_CHAR		3	// skip next _OnChar()

#எனில் defined(FEAT_DIRECTX)
நிலையான DWriteContext *s_dwc = வெற்று;
நிலையான முழு s_directx_enabled = 0;
நிலையான முழு s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != வெற்று && enc_utf8)
நிலையான முழு directx_enabled(வெறுமை);
நிலையான வெறுமை directx_binddc(வெறுமை);
#endif

#ifdef FEAT_MENU
நிலையான முழு gui_mswin_get_menu_height(முழு fix_window);
#இல்லையெனில்
# define gui_mswin_get_menu_height(fix_window)	0
#endif

வகைவரை கட்டமைப்பு keycode_trans_strategy {
    வெறுமை (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
    வெறுமை (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
    வெறுமை (*ptr_process_message_usual_சாவி) (UINT /*vk*/, மாறிலி செய்தி_2* /*pmsg*/);
    முழு  (*ptr_get_active_modifiers)(வெறுமை);
    முழு  (*is_experimental)(வெறுமை);
} keycode_trans_strategy;

// forward declarations for input instance initializer
நிலையான வெறுமை _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
நிலையான வெறுமை _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
நிலையான வெறுமை process_message_usual_சாவி_experimental(UINT /*vk*/, மாறிலி செய்தி_2* /*pmsg*/);
நிலையான முழு  get_active_modifiers_experimental(வெறுமை);
நிலையான முழு  is_experimental_true(வெறுமை);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_சாவி_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
நிலையான வெறுமை _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
நிலையான வெறுமை _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, முழு /*cRepeat UNUSED*/);
நிலையான வெறுமை process_message_usual_சாவி_classic(UINT /*vk*/, மாறிலி செய்தி_2* /*pmsg*/);
நிலையான முழு  get_active_modifiers_classic(வெறுமை);
நிலையான முழு  is_experimental_false(வெறுமை);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_சாவி_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = வெற்று;

நிலையான முழு is_experimental_true(வெறுமை)
{
    திரும்பு 1;
}

நிலையான முழு is_experimental_false(வெறுமை)
{
    திரும்பு 0;
}

/*
 * Initialize the keycode translation strategy.
 */
நிலையான வெறுமை keycode_trans_strategy_init(வெறுமை)
{
    மாறிலி எழுத்து *strategy = வெற்று;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    எனில் (strategy == வெற்று)
    {
	திரும்பு;
    }

    எனில் (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	திரும்பு;
    }

    எனில் (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	திரும்பு;
    }

}

#எனில் defined(FEAT_RENDER_OPTIONS)
    முழு
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    முழு	    dx_enable = 0;
    முழு	    dx_flags = 0;
    தசம   dx_gamma = 0.0f;
    தசம   dx_contrast = 0.0f;
    தசம   dx_level = 0.0f;
    முழு	    dx_geom = 0;
    முழு	    dx_renmode = 0;
    முழு	    dx_taamode = 0;
    // parse string as rendering options.
    க்கு (p = s; p != வெற்று && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  மதிப்பு[128];

	copy_option_part(&p, item, அளவு(item), ",");
	எனில் (p == வெற்று)
	    நிறுத்து;
	q = &item[0];
	copy_option_part(&q, name, அளவு(name), ":");
	எனில் (q == வெற்று)
	    திரும்பு FAIL;
	copy_option_part(&q, மதிப்பு, அளவு(மதிப்பு), ":");

	எனில் (STRCMP(name, "type") == 0)
	{
	    எனில் (STRCMP(மதிப்பு, "directx") == 0)
		dx_enable = 1;
	    இல்லையெனில்
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (தசம)atof((எழுத்து *)மதிப்பு);
	}
	இல்லையெனில் எனில் (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (தசம)atof((எழுத்து *)மதிப்பு);
	}
	இல்லையெனில் எனில் (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (தசம)atof((எழுத்து *)மதிப்பு);
	}
	இல்லையெனில் எனில் (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((எழுத்து *)மதிப்பு);
	    எனில் (dx_geom < 0 || dx_geom > 2)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((எழுத்து *)மதிப்பு);
	    எனில் (dx_renmode < 0 || dx_renmode > 6)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((எழுத்து *)மதிப்பு);
	    எனில் (dx_taamode < 0 || dx_taamode > 3)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	இல்லையெனில்
	    திரும்பு FAIL;
    }

    எனில் (!gui.in_use)
	திரும்பு OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    எனில் (dx_enable)
    {
	எனில் (!directx_enabled())
	    திரும்பு FAIL;
	DWriteContext_SetRenderingParams(s_dwc, வெற்று);
	எனில் (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    எனில் (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    எனில் (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    எனில் (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    எனில் (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    எனில் (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    எனில் (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    திரும்பு OK;
# இல்லையெனில்
    திரும்பு FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (முழு)(short)LOWORD(lParam), (முழு)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (முழு)(short)LOWORD(lParam), (முழு)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (முழு)(short)LOWORD(lParam), (முழு)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#அந்தர்பாவ "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#அந்தர்பாவ <shellapi.h>
#அந்தர்பாவ <commctrl.h>
#அந்தர்பாவ <windowsx.h>

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

நிலையான வெறுமை _OnPaint(HWND hwnd);
நிலையான வெறுமை fill_rect(மாறிலி RECT *rcp, HBRUSH hbr, COLORREF color);
நிலையான வெறுமை clear_rect(RECT *rcp);

நிலையான WORD		s_dlgfntheight;		// height of the dialog font
நிலையான WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
நிலையான HMENU		s_menuBar = வெற்று;
#endif
#ifdef FEAT_TEAROFF
நிலையான வெறுமை rebuild_tearoff(vimmenu_T *menu);
நிலையான HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
நிலையான முழு		s_busy_processing = FALSE;

நிலையான முழு		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
நிலையான UINT		s_findrep_செய்தி = 0;
நிலையான FINDREPLACEW	s_findrep_struct;
நிலையான HWND		s_findrep_hwnd = வெற்று;
நிலையான முழு		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = வெற்று;
நிலையான HDC		s_hdc = வெற்று;
நிலையான HBRUSH		s_brush = வெற்று;

#ifdef FEAT_TOOLBAR
நிலையான HWND		s_toolbarhwnd = வெற்று;
நிலையான WNDPROC		s_toolbar_wndproc = வெற்று;
#endif

#ifdef FEAT_GUI_TABLINE
நிலையான HWND		s_tabhwnd = வெற்று;
நிலையான WNDPROC		s_tabline_wndproc = வெற்று;
நிலையான முழு		showing_tabline = 0;
#endif

நிலையான WPARAM		s_wParam = 0;

நிலையான HWND		s_textArea = வெற்று;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
நிலையான bool		s_textarea_bg_only = false;

நிலையான UINT		s_u_செய்தி = 0;

நிலையான char_u		*s_textfield; // Used by dialogs to pass back strings

நிலையான முழு		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
நிலையான முழு allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
வகைவரை HANDLE DPI_AWARENESS_CONTEXT;

வகைவரை எண்ணிக்கை DPI_AWARENESS {
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
நிலையான முழு		s_dpi = DEFAULT_DPI;
நிலையான BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
நிலையான DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
நிலையான RECT		s_suggested_rect;

நிலையான UINT (WINAPI *pGetDpiForSystem)(வெறுமை) = வெற்று;
நிலையான UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = வெற்று;
நிலையான முழு (WINAPI *pGetSystemMetricsForDpi)(முழு, UINT) = வெற்று;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
நிலையான DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = வெற்று;
#ifdef FEAT_MBYTE_IME
நிலையான DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = வெற்று;
#endif

நிலையான HINSTANCE hLibDwm = வெற்று;
நிலையான HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
நிலையான வெறுமை dyn_dwm_load(வெறுமை);

நிலையான முழு fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

நிலையான HINSTANCE hUxThemeLib = வெற்று;
நிலையான DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = வெற்று;
நிலையான வெறுமை (WINAPI *pFlushMenuThemes)(வெறுமை) = வெற்று;
நிலையான வெறுமை dyn_uxtheme_load(வெறுமை);
#endif

    நிலையான முழு WINAPI
stubGetSystemMetricsForDpi(முழு nIndex, UINT dpi UNUSED)
{
    திரும்பு GetSystemMetrics(nIndex);
}

    நிலையான முழு
adjust_fontsize_by_dpi(முழு size)
{
    திரும்பு size * s_dpi / (முழு)pGetDpiForSystem();
}

    நிலையான முழு
adjust_by_system_dpi(முழு size)
{
    திரும்பு size * (முழு)pGetDpiForSystem() / DEFAULT_DPI;
}

#எனில் defined(FEAT_DIRECTX)
    நிலையான முழு
directx_enabled(வெறுமை)
{
    எனில் (s_dwc != வெற்று)
	திரும்பு 1;
    இல்லையெனில் எனில் (s_directx_load_attempted)
	திரும்பு 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    திரும்பு s_dwc != வெற்று ? 1 : 0;
}

    நிலையான வெறுமை
directx_binddc(வெறுமை)
{
    எனில் (s_textArea == வெற்று)
	திரும்பு;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

வெளி முழு current_font_height;	    // this is in os_mswin.c

நிலையான கட்டமைப்பு
{
    UINT    சாவி_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_சாவிகள்[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_இடது,		'k', 'l'},
    {VK_வலது,		'k', 'r'},

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
    {VK_செருகு,		'k', 'I'},
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
நிலையான முழு	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
நிலையான முழு	s_getting_focus = FALSE;

நிலையான முழு	s_x_pending;
நிலையான முழு	s_y_pending;
நிலையான UINT	s_kFlags_pending;
நிலையான UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
நிலையான முழு	s_timed_out = FALSE;
நிலையான முழு	dead_சாவி = DEAD_சாவி_OFF;
நிலையான UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
நிலையான வெறுமை Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
நிலையான வெறுமை track_user_activity(UINT u_செய்தி);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
நிலையான LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
நிலையான LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
நிலையான LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#எனில் defined(FEAT_BROWSE)
நிலையான char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    நிலையான வெறுமை
print_windows_error(வெறுமை)
{
    LPVOID  lp_செய்தி_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  வெற்று, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_செய்தி_Buf, 0, வெற்று);
    TRACE1("Error: %s\n", lp_செய்தி_Buf);
    LocalFree(lp_செய்தி_Buf);
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

நிலையான முழு		blink_state = BLINK_NONE;
நிலையான long_u		blink_waittime = 700;
நிலையான long_u		blink_ontime = 400;
நிலையான long_u		blink_offtime = 250;
நிலையான UINT_PTR		blink_timer = 0;

    முழு
gui_mch_is_blinking(வெறுமை)
{
    திரும்பு blink_state != BLINK_NONE;
}

    முழு
gui_mch_is_blink_off(வெறுமை)
{
    திரும்பு blink_state == BLINK_OFF;
}

    வெறுமை
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    நிலையான VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_செய்தி UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    செய்தி_2 செய்தி;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(வெற்று, idEvent);

    // Eat spurious WM_TIMER messages
    போது (PeekMessageW(&செய்தி, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    எனில் (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(வெற்று, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    இல்லையெனில்
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(வெற்று, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    நிலையான வெறுமை
gui_mswin_rm_blink_timer(வெறுமை)
{
    செய்தி_2 செய்தி;

    எனில் (blink_timer == 0)
	திரும்பு;

    KillTimer(வெற்று, blink_timer);
    // Eat spurious WM_TIMER messages
    போது (PeekMessageW(&செய்தி, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    வெறுமை
gui_mch_stop_blink(முழு may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    எனில் (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    வெறுமை
gui_mch_start_blink(வெறுமை)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    எனில் (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(வெற்று, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    நிலையான VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_செய்தி UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    செய்தி_2 செய்தி;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(வெற்று, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    போது (PeekMessageW(&செய்தி, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    எனில் (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    நிலையான வெறுமை
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    முழு cRepeat UNUSED)
{
    dead_சாவி = DEAD_சாவி_SET_DEFAULT;
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
    நிலையான முழு
char_to_string(முழு ch, char_u *string, முழு slen, முழு had_alt)
{
    முழு		len;
    முழு		i;
    WCHAR	wstring[2];
    char_u	*ws = வெற்று;

    எனில் (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    இல்லையெனில் எனில் (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	திரும்பு 0;
    }
    இல்லையெனில்
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    எனில் (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, வெற்று);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	எனில் (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, வெற்று);
	    எனில் (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    இல்லையெனில்
    {
	ws = utf16_to_enc(wstring, &len);
	எனில் (ws == வெற்று)
	    len = 0;
	இல்லையெனில்
	{
	    எனில் (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    எனில் (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    க்கு (i = 0; i < len; ++i)
	எனில் (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (முழு)KE_CSI;
	    len += 2;
	}

    திரும்பு len;
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
    நிலையான முழு
get_active_modifiers_experimental(வெறுமை)
{
    முழு modifiers = 0;

    எனில் (Get_சாவி_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    எனில் (Get_சாவி_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    எனில் (Get_சாவி_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    எனில் ((modifiers & MOD_MASK_CTRL) && (Get_சாவி_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    எனில் (!(Get_சாவி_State(VK_CONTROL) & 0x8000)
	    &&  (Get_சாவி_State(VK_RMENU) & 0x8000)
	    && !(Get_சாவி_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    திரும்பு modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    நிலையான முழு
get_active_modifiers_classic(வெறுமை)
{
    முழு modifiers = 0;

    எனில் (Get_சாவி_State(VK_SHIFT) & 0x8000)
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
    எனில் (Get_சாவி_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    எனில் (Get_சாவி_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    திரும்பு modifiers;
}

    நிலையான முழு
get_active_modifiers(வெறுமை)
{
    திரும்பு get_active_modifiers_experimental();
}

    நிலையான முழு
get_active_modifiers_via_ptr(வெறுமை)
{
    // marshal to corresponding implementation
    திரும்பு keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    நிலையான வெறுமை
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    முழு cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    நிலையான வெறுமை
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    முழு cRepeat UNUSED)
{
    char_u	string[40];
    முழு		len = 0;
    முழு		modifiers;
    முழு		ch = cch;   // special keys are negative

    எனில் (dead_சாவி == DEAD_சாவி_SKIP_ON_CHAR)
	திரும்பு;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    எனில் (dead_சாவி != DEAD_சாவி_TRANSIENT_IN_ON_CHAR)
	dead_சாவி = DEAD_சாவி_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_சாவி(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_சாவிகள்;
    ch = may_adjust_சாவி_for_ctrl(modifiers, ch);
    --no_reduce_சாவிகள்;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, வெற்று);
    எனில் (ch == CSI)
	ch = K_CSI;

    எனில் (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_உள்ளீடு_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    எனில் (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_உள்ளீடு_buf();
	got_int = TRUE;
    }

    add_to_உள்ளீடு_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    நிலையான வெறுமை
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    முழு cRepeat UNUSED)
{
    char_u	string[40];
    முழு		len = 0;

    dead_சாவி = 0;

    len = char_to_string(ch, string, 40, FALSE);
    எனில் (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_உள்ளீடு_buf();
	got_int = TRUE;
    }

    add_to_உள்ளீடு_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    நிலையான வெறுமை
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    முழு cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    நிலையான வெறுமை
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    முழு cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    முழு		len;
    முழு		modifiers;
    முழு		ch = cch;   // special keys are negative

    dead_சாவி = DEAD_சாவி_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_சாவி(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, வெற்று);
    எனில் (ch == CSI)
	ch = K_CSI;

    len = 0;
    எனில் (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    எனில் (IS_SPECIAL((முழு)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((முழு)ch);
	string[len++] = K_THIRD((முழு)ch);
    }
    இல்லையெனில்
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_உள்ளீடு_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    நிலையான வெறுமை
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    முழு cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    முழு		len;
    முழு		modifiers;
    முழு		ch = cch;   // special keys are negative

    dead_சாவி = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    எனில் (Get_சாவி_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    எனில் (Get_சாவி_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_சாவி(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, வெற்று);
    எனில் (ch == CSI)
	ch = K_CSI;

    len = 0;
    எனில் (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    எனில் (IS_SPECIAL((முழு)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((முழு)ch);
	string[len++] = K_THIRD((முழு)ch);
    }
    இல்லையெனில்
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_உள்ளீடு_buf(string, len);
}

    நிலையான வெறுமை
_OnMouseEvent(
    முழு button,
    முழு x,
    முழு y,
    முழு repeated_click,
    UINT சாவி_Flags)
{
    முழு vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    எனில் (சாவி_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    எனில் (சாவி_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    எனில் (Get_சாவி_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    நிலையான வெறுமை
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    முழு x,
    முழு y,
    UINT சாவி_Flags)
{
    நிலையான LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    முழு	    button = -1;
    முழு	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (வெறுமை)SetFocus(s_hwnd);

    எனில் (s_u_செய்தி == WM_LBUTTONDOWN || s_u_செய்தி == WM_LBUTTONDBLCLK)
	button = MOUSE_இடது;
    இல்லையெனில் எனில் (s_u_செய்தி == WM_MBUTTONDOWN || s_u_செய்தி == WM_MBUTTONDBLCLK)
	button = MOUSE_நடு;
    இல்லையெனில் எனில் (s_u_செய்தி == WM_RBUTTONDOWN || s_u_செய்தி == WM_RBUTTONDBLCLK)
	button = MOUSE_வலது;
    இல்லையெனில் எனில் (s_u_செய்தி == WM_XBUTTONDOWN || s_u_செய்தி == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    இல்லையெனில் எனில் (s_u_செய்தி == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	எனில் (s_button_pending == MOUSE_இடது)
	    button = MOUSE_வலது;
	இல்லையெனில்
	    button = MOUSE_இடது;
    }

    எனில் (button < 0)
	திரும்பு;

    repeated_click = ((முழு)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    எனில் (repeated_click
	    && ((button == MOUSE_இடது && s_button_pending == MOUSE_வலது)
		|| (button == MOUSE_வலது
		    && s_button_pending == MOUSE_இடது)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_நடு;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, சாவி_Flags);
    }
    இல்லையெனில் எனில் ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_வலது)))
    {
	எனில் (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, சாவி_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, சாவி_Flags);
    }
    இல்லையெனில்
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
	s_kFlags_pending = சாவி_Flags;
    }

    s_prevTime = currentTime;
}

    நிலையான வெறுமை
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    முழு x,
    முழு y,
    UINT சாவி_Flags)
{
    முழு button;

    s_getting_focus = FALSE;
    எனில் (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    எனில் (s_u_செய்தி == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	எனில் (!(சாவி_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    திரும்பு;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    இல்லையெனில்
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, சாவி_Flags);
}

    நிலையான வெறுமை
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    முழு cx UNUSED,
    முழு cy UNUSED)
{
#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    நிலையான vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    முழு		id)
{
    vimmenu_T	*pChildMenu;

    போது (pMenu)
    {
	எனில் (pMenu->id == (UINT)id)
	    நிறுத்து;
	எனில் (pMenu->children != வெற்று)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    எனில் (pChildMenu)
	    {
		pMenu = pChildMenu;
		நிறுத்து;
	    }
	}
	pMenu = pMenu->next;
    }
    திரும்பு pMenu;
}

    நிலையான வெறுமை
_OnMenu(
    HWND	hwnd UNUSED,
    முழு		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(வேர்_menu, id);
    எனில் (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    நிலையான வெறுமை
_OnFindRepl(வெறுமை)
{
    முழு	    flags = 0;
    முழு	    down;

    எனில் (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(வெறுமை)SetFocus(s_hwnd);

    எனில் (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(வெறுமை)SetFocus(s_hwnd);
    }
    இல்லையெனில் எனில் (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(வெறுமை)SetFocus(s_hwnd);
    }
    இல்லையெனில் எனில் (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    எனில் (flags == 0)
	திரும்பு;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    எனில் (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    எனில் (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, வெற்று);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, வெற்று);
    எனில் (p != வெற்று && q != வெற்று)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    நிலையான வெறுமை
HandleMouseHide(UINT u_செய்தி, LPARAM lParam)
{
    நிலையான LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    எனில் (u_செய்தி == WM_MOUSEMOVE || u_செய்தி == WM_NCMOUSEMOVE)
    {
	எனில் (lParam == last_lParam)
	    திரும்பு;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    தேர்வு (u_செய்தி)
    {
    நிலை WM_KEYUP:
    நிலை WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	எனில் (p_mh)
	    gui_mch_mousehide(TRUE);
	நிறுத்து;

    நிலை WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    நிலை WM_SYSCHAR:
    நிலை WM_MOUSEMOVE:	 // show the pointer on any mouse action
    நிலை WM_LBUTTONDOWN:
    நிலை WM_LBUTTONUP:
    நிலை WM_MBUTTONDOWN:
    நிலை WM_MBUTTONUP:
    நிலை WM_RBUTTONDOWN:
    நிலை WM_RBUTTONUP:
    நிலை WM_XBUTTONDOWN:
    நிலை WM_XBUTTONUP:
    நிலை WM_NCMOUSEMOVE:
    நிலை WM_NCLBUTTONDOWN:
    நிலை WM_NCLBUTTONUP:
    நிலை WM_NCMBUTTONDOWN:
    நிலை WM_NCMBUTTONUP:
    நிலை WM_NCRBUTTONDOWN:
    நிலை WM_NCRBUTTONUP:
    நிலை WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	நிறுத்து;
    }
}

    நிலையான LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_செய்தி,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_செய்தி, lParam);

    s_u_செய்தி = u_செய்தி;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_செய்தி);
#endif

    தேர்வு (u_செய்தி)
    {
	HANDLE_செய்தி(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_செய்தி(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_செய்தி(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_செய்தி(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_செய்தி(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_செய்தி(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_செய்தி(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_செய்தி(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	நிலை WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    திரும்பு TRUE;
#endif
	இயல்பு:
	    திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
    }
}

    நிலையான வெறுமை
dyn_dwm_load(வெறுமை)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    எனில் (hLibDwm == வெற்று)
	திரும்பு;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    எனில் (pDwmSetWindowAttribute == வெற்று)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = வெற்று;
	திரும்பு;
    }
}

வெளி DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    வெறுமை
gui_mch_set_titlebar_colors(வெறுமை)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    எனில் (pDwmSetWindowAttribute == வெற்று || win_version < MAKE_VER(10, 0, 22000))
	திரும்பு;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    எனில் (vim_strchr(p_go, GO_TITLEBAR) != வெற்று)
    {
	எனில் (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	இல்லையெனில்
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	எனில் (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	எனில் (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, அளவு(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, அளவு(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    வெறுமை
gui_mch_new_colors(வெறுமை)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, வெற்று, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    எனில் (s_textarea_bg_only && s_textArea != வெற்று)
	InvalidateRect(s_textArea, வெற்று, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    வெறுமை
gui_mch_def_colors(வெறுமை)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    முழு
gui_mch_open(வெறுமை)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    எனில் (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	எனில் (s_textArea != வெற்று)
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

    திரும்பு OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    முழு
gui_mch_get_winpos(முழு *x, முழு *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.இடது;
    *y = rect.உச்சி;
    திரும்பு OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    வெறுமை
gui_mch_set_winpos(முழு x, முழு y)
{
    SetWindowPos(s_hwnd, வெற்று, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    வெறுமை
gui_mch_set_text_area_pos(முழு x, முழு y, முழு w, முழு h)
{
    நிலையான முழு oldx = 0;
    நிலையான முழு oldy = 0;

    SetWindowPos(s_textArea, வெற்று, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    எனில் (vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#எனில் defined(FEAT_GUI_TABLINE)
    எனில் (showing_tabline)
    {
	முழு	உச்சி = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	எனில் (vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
	    உச்சி = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, உச்சி, rect.வலது, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    எனில் (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, வெற்று, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    வெறுமை
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    முழு		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    வெறுமை
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    முழு		x,
    முழு		y,
    முழு		w,
    முழு		h)
{
    SetWindowPos(sb->id, வெற்று, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    முழு
gui_mch_get_scrollbar_xpadding(வெறுமை)
{
    RECT    rcTxt, rcWnd;
    முழு	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.வலது - rcTxt.வலது - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    திரும்பு (xpad < 0) ? 0 : xpad;
}

    முழு
gui_mch_get_scrollbar_ypadding(வெறுமை)
{
    RECT    rcTxt, rcWnd;
    முழு	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    திரும்பு (ypad < 0) ? 0 : ypad;
}

    வெறுமை
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    முழு		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, வெற்று,
	g_hinst, வெற்று);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    நிலையான scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    எனில் (gui.bottom_sbar.id == hwnd)
	திரும்பு &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	எனில் (wp->w_scrollbars[SBAR_இடது].id == hwnd)
	    திரும்பு &wp->w_scrollbars[SBAR_இடது];
	எனில் (wp->w_scrollbars[SBAR_வலது].id == hwnd)
	    திரும்பு &wp->w_scrollbars[SBAR_வலது];
    }
    திரும்பு வெற்று;
}

    நிலையான வெறுமை
update_scrollbar_size(வெறுமை)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
GetFontSize(GuiFont font, முழு *char_width, முழு *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    எனில் (char_width)
	*char_width = size.cx + tm.tmOverhang;
    எனில் (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    நிலையான வெறுமை
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    முழு
gui_mch_adjust_charheight(வெறுமை)
{
    UpdateFontSize(gui.norm_font);
    திரும்பு OK;
}

    நிலையான GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = வெற்று;

    // Load the font
    font = CreateFontIndirectW(lf);

    எனில் (font == வெற்று)
	திரும்பு NOFONT;

    திரும்பு (GuiFont)font;
}

    நிலையான முழு
pixels_to_points(முழு pixels, முழு vertical)
{
    முழு		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    திரும்பு points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    முழு		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    எனில் (get_logfont(&lf, name, வெற்று, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    எனில் (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    திரும்பு font;
}

#எனில் defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    எனில் (name == வெற்று)
	திரும்பு வெற்று;
    திரும்பு vim_strsave(name);
}
#endif

    வெறுமை
gui_mch_free_font(GuiFont font)
{
    எனில் (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    முழு i;

    வகைவரை கட்டமைப்பு SysColorTable
    {
	எழுத்து	    *name;
	முழு	    color;
    } SysColorTable;

    நிலையான SysColorTable sys_table[] =
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
    க்கு (i = 0; i < (முழு)ARRAY_LENGTH(sys_table); i++)
	எனில் (STRICMP(name, sys_table[i].name) == 0)
	    திரும்பு GetSysColor(sys_table[i].color);

    திரும்பு gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(முழு r, முழு g, முழு b)
{
    திரும்பு gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    முழு
gui_mch_haskey(char_u *name)
{
    முழு i;

    க்கு (i = 0; special_சாவிகள்[i].vim_code1 != NUL; i++)
	எனில் (name[0] == special_சாவிகள்[i].vim_code0
				       && name[1] == special_சாவிகள்[i].vim_code1)
	    திரும்பு OK;
    திரும்பு FAIL;
}

    வெறுமை
gui_mch_beep(வெறுமை)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    வெறுமை
gui_mch_invert_rectangle(
    முழு	    r,
    முழு	    c,
    முழு	    nr,
    முழு	    nc)
{
    RECT    rc;

#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.இடது = FILL_X(c);
    rc.உச்சி = FILL_Y(r);
    rc.வலது = rc.இடது + nc * gui.char_width;
    rc.bottom = rc.உச்சி + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    வெறுமை
gui_mch_iconify(வெறுமை)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    வெறுமை
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.இடது = FILL_X(gui.col);
    rc.உச்சி = FILL_Y(gui.row);
    rc.வலது = rc.இடது + gui.char_width;
    எனில் (mb_lefthalve(gui.row, gui.col))
	rc.வலது += gui.char_width;
    rc.bottom = rc.உச்சி + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    வெறுமை
gui_mch_draw_part_cursor(
    முழு		w,
    முழு		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.இடது =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_வலது ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.உச்சி = FILL_Y(gui.row) + gui.char_height - h;
    rc.வலது = rc.இடது + w;
    rc.bottom = rc.உச்சி + h;

    fill_rect(&rc, வெற்று, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    நிலையான வெறுமை
output_Dead_சாவி_re_Post_Ex(செய்தி_2 original_செய்தி, முழு dead_key2set)
{
    நிலையான செய்தி_2 deadCharExpel;

    எனில் (dead_சாவி == DEAD_சாவி_OFF)
	திரும்பு;

    dead_சாவி = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_செய்தி.message;
    deadCharExpel.hwnd    = original_செய்தி.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_செய்தி.hwnd, original_செய்தி.message, original_செய்தி.wParam,
							  original_செய்தி.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    நிலையான வெறுமை
output_Dead_சாவி_re_Post(செய்தி_2 original_செய்தி)
{
    output_Dead_சாவி_re_Post_Ex(original_செய்தி, DEAD_சாவி_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
நிலையான வெறுமை process_message_usual_சாவி(UINT vk, மாறிலி செய்தி_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_சாவி(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    நிலையான வெறுமை
process_message_usual_சாவி_experimental(UINT vk, மாறிலி செய்தி_2 *pmsg)
{
    WCHAR	ch[8];
    முழு		len;
    முழு		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    எனில் (Get_சாவி_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    எனில் (Get_சாவி_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    எனில் ((Get_சாவி_State(VK_RMENU) & 0x8000)
				 && (Get_சாவி_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_சாவி(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    எனில் (len < 0)
	dead_சாவி = DEAD_சாவி_SET_DEFAULT;

    எனில் (len <= 0)
    {
	முழு wm_char = NUL;

	எனில் (dead_சாவி == DEAD_சாவி_SET_DEFAULT
		&& (Get_சாவி_State(VK_CONTROL) & 0x8000))
	{
	    எனில் (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    எனில் (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	எனில் (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_சாவி = DEAD_சாவி_TRANSIENT_IN_ON_CHAR;
	}
	திரும்பு;
    }

    // Post the message as TranslateMessage would do.
    எனில் (pmsg->message == WM_KEYDOWN)
    {
	க்கு (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    இல்லையெனில்
    {
	க்கு (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    நிலையான வெறுமை
process_message_usual_சாவி_classic(UINT vk, மாறிலி செய்தி_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    எனில் (vk != 0xff
	    && (Get_சாவி_State(VK_CONTROL) & 0x8000)
	    && !(Get_சாவி_State(VK_SHIFT) & 0x8000)
	    && !(Get_சாவி_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	எனில் (vk == '6' || Map_Virtual_சாவி(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_உள்ளீடு_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	இல்லையெனில் எனில் (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_உள்ளீடு_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	இல்லையெனில் எனில் (vk == '2' || Map_Virtual_சாவி(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_உள்ளீடு_buf(string, 1);
	}
	இல்லையெனில்
	    TranslateMessage(pmsg);
    }
    இல்லையெனில்
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    நிலையான வெறுமை
process_message(வெறுமை)
{
    செய்தி_2		செய்தி;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    முழு		i;
    முழு		modifiers = 0;
    முழு		சாவி;
#ifdef FEAT_MENU
    நிலையான char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    நிலையான முழு  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    எனில் (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&செய்தி, வெற்று, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    எனில் (செய்தி.message == WM_OLE)
    {
	char_u *str = (char_u *)செய்தி.lParam;
	எனில் (str == வெற்று || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&செய்தி);
	}
	இல்லையெனில்
	{
	    add_to_உள்ளீடு_buf(str, (முழு)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	திரும்பு;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    எனில் (s_findrep_hwnd != வெற்று && IsDialogMessageW(s_findrep_hwnd, &செய்தி))
    {
	HandleMouseHide(செய்தி.message, செய்தி.lParam);
	திரும்பு;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    எனில் (செய்தி.message == WM_KEYDOWN || செய்தி.message == WM_SYSKEYDOWN)
    {
	vk = (முழு) செய்தி.wParam;

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
	எனில் (keycode_trans_strategy_used->is_experimental()
		&& dead_சாவி == DEAD_சாவி_SKIP_ON_CHAR)
	{
	    dead_சாவி = DEAD_சாவி_OFF;
	}

	எனில் (dead_சாவி != DEAD_சாவி_OFF)
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
	    எனில் (keycode_trans_strategy_used->is_experimental() &&
		    dead_சாவி == DEAD_சாவி_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_சாவி_re_Post_Ex(செய்தி,
				       /*dead_key2set=*/DEAD_சாவி_SKIP_ON_CHAR);
		திரும்பு;
	    }

	    எனில் (dead_சாவி != DEAD_சாவி_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		திரும்பு;
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
	    எனில் ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_சாவி = DEAD_சாவி_OFF;
		TranslateMessage(&செய்தி);
		திரும்பு;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    இல்லையெனில் எனில் ((get_real_state()
			    & (MODE_செருகு | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_சாவி_re_Post(செய்தி);
		திரும்பு;
	    }
	}

	// Check for CTRL-BREAK
	எனில் (vk == VK_CANCEL)
	{
	    trash_உள்ளீடு_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_உள்ளீடு_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	எனில் (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&செய்தி);
	    திரும்பு;
	}

	க்கு (i = 0; special_சாவிகள்[i].சாவி_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    எனில் (special_சாவிகள்[i].சாவி_sym == vk
		    && (vk != VK_SPACE || !(Get_சாவி_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		எனில் (dead_சாவி == DEAD_சாவி_SET_DEFAULT
			&& (special_சாவிகள்[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_சாவி_re_Post(செய்தி);
		    திரும்பு;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		எனில் (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  வெற்று, வெற்று) == வெற்று)
		    நிறுத்து;
#endif
		modifiers = get_active_modifiers_via_ptr();

		எனில் (special_சாவிகள்[i].vim_code1 == NUL)
		    சாவி = special_சாவிகள்[i].vim_code0;
		இல்லையெனில்
		    சாவி = TO_SPECIAL(special_சாவிகள்[i].vim_code0,
						   special_சாவிகள்[i].vim_code1);
		சாவி = simplify_சாவி(சாவி, &modifiers);
		எனில் (சாவி == CSI)
		    சாவி = K_CSI;

		எனில் (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_உள்ளீடு_buf(string, 3);
		}

		எனில் (IS_SPECIAL(சாவி))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(சாவி);
		    string[2] = K_THIRD(சாவி);
		    add_to_உள்ளீடு_buf(string, 3);
		}
		இல்லையெனில்
		{
		    முழு	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(சாவி, string, 40, FALSE);
		    add_to_உள்ளீடு_buf(string, len);
		}
		நிறுத்து;
	    }
	}

	// Not a special key.
	எனில் (special_சாவிகள்[i].சாவி_sym == 0)
	{
	    process_message_usual_சாவி(vk, &செய்தி);
	}
    }
#ifdef FEAT_MBYTE_IME
    இல்லையெனில் எனில் (செய்தி.message == WM_IME_NOTIFY)
	_OnImeNotify(செய்தி.hwnd, (DWORD)செய்தி.wParam, (DWORD)செய்தி.lParam);
    இல்லையெனில் எனில் (செய்தி.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&செய்தி);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    எனில் (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  வெற்று, வெற்று) == வெற்று)
#endif
	DispatchMessageW(&செய்தி);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    வெறுமை
gui_mch_update(வெறுமை)
{
    செய்தி_2	    செய்தி;

    எனில் (!s_busy_processing)
	போது (PeekMessageW(&செய்தி, வெற்று, 0, 0, PM_NOREMOVE)
						  && !vim_is_உள்ளீடு_buf_full())
	    process_message();
}

    நிலையான வெறுமை
remove_any_timer(வெறுமை)
{
    செய்தி_2		செய்தி;

    எனில் (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(வெற்று, s_wait_timer);

	// Eat spurious WM_TIMER messages
	போது (PeekMessageW(&செய்தி, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
நிலையான garray_T	server_pending_ga = {0, 0, 0, 0, வெற்று};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    வெறுமை
server_add_உள்ளீடு(char_u *str)
{
    எனில் (server_pending_ga.ga_data == வெற்று)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    நிலையான வெறுமை
server_flush_உள்ளீடு(வெறுமை)
{
    char_u  *str;

    எனில் (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	திரும்பு;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    எனில் (str != வெற்று)
    {
	server_to_உள்ளீடு_buf(str);
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
    முழு
gui_mch_wait_for_chars(long wtime)
{
    முழு		focus;

    s_timed_out = FALSE;

    எனில் (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	எனில் (s_busy_processing)
	    திரும்பு FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(வெற்று, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    போது (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	எனில் (gui.in_focus != focus)
	{
	    எனில் (gui.in_focus)
		gui_mch_start_blink();
	    இல்லையெனில்
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	எனில் (s_need_activate)
	{
	    (வெறுமை)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	க்கு (;;)
	{
	    செய்தி_2 செய்தி;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_உள்ளீடு();
# endif
# ifdef FEAT_TIMERS
	    எனில் (did_add_timer)
		நிறுத்து;
# endif
	    எனில் (PeekMessageW(&செய்தி, வெற்று, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		நிறுத்து;
	    }
	    இல்லையெனில் எனில் (உள்ளீடு_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || செய்தி_Wait_For_Multiple_Objects(0, வெற்று, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		நிறுத்து;
	}
#இல்லையெனில்
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	எனில் (உள்ளீடு_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    எனில் (!s_getting_focus)
		s_button_pending = -1;

	    திரும்பு OK;
	}

#ifdef FEAT_TIMERS
	எனில் (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    நிறுத்து;
	}
#endif
    }
    allow_scrollbar = FALSE;
    திரும்பு FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    வெறுமை
gui_mch_clear_block(
    முழு		row1,
    முழு		col1,
    முழு		row2,
    முழு		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.இடது = FILL_X(col1);
    rc.உச்சி = FILL_Y(row1);
    rc.வலது = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    வெறுமை
gui_mch_clear_all(வெறுமை)
{
    RECT    rc;

    rc.இடது = 0;
    rc.உச்சி = 0;
    rc.வலது = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    வெறுமை
gui_mch_enable_menu(முழு flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : வெற்று);
#endif
}

    வெறுமை
gui_mch_set_menu_pos(
    முழு	    x UNUSED,
    முழு	    y UNUSED,
    முழு	    w UNUSED,
    முழு	    h UNUSED)
{
    // It will be in the right place anyway
}

#எனில் defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    வெறுமை
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    முழு		hidden)
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
    வெறுமை
gui_mch_draw_menubar(வெறுமை)
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
    திரும்பு (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#எனில் defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    நிலையான WORD
PixelToDialogX(முழு இலக்கம்_Pixels)
{
    திரும்பு (WORD)((இலக்கம்_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    நிலையான WORD
PixelToDialogY(முழு இலக்கம்_Pixels)
{
    திரும்பு (WORD)((இலக்கம்_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    நிலையான முழு
GetTextWidth(HDC hdc, char_u *str, முழு len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    திரும்பு size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    நிலையான முழு
GetTextWidthEnc(HDC hdc, char_u *str, முழு len)
{
    SIZE	size;
    WCHAR	*wstr;
    முழு		n;
    முழு		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    எனில் (wstr == வெற்று)
	திரும்பு 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    எனில் (n)
	திரும்பு size.cx;
    திரும்பு 0;
}

நிலையான வெறுமை get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    நிலையான BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    முழு		    wChild, hChild, wParent, hParent;
    முழு		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.வலது - rChild.இடது;
    hChild = rChild.bottom - rChild.உச்சி;

    // If Vim is minimized put the window in the middle of the screen.
    எனில் (hwndParent == வெற்று || IsMinimized(hwndParent))
	get_work_area(&rParent);
    இல்லையெனில்
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.வலது - rParent.இடது;
    hParent = rParent.bottom - rParent.உச்சி;

    moninfo.cbSize = அளவு(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    எனில் (mon != வெற்று && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    இல்லையெனில்
    {
	hdc = GetDC(hwndChild);
	rScreen.இடது = 0;
	rScreen.உச்சி = 0;
	rScreen.வலது = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.இடது + ((wParent - wChild) / 2);
    எனில் (xNew < rScreen.இடது)
	xNew = rScreen.இடது;
    இல்லையெனில் எனில் ((xNew + wChild) > rScreen.வலது)
	xNew = rScreen.வலது - wChild;

    yNew = rParent.உச்சி + ((hParent - hChild) / 2);
    எனில் (yNew < rScreen.உச்சி)
	yNew = rScreen.உச்சி;
    இல்லையெனில் எனில் ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    திரும்பு SetWindowPos(hwndChild, வெற்று, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#எனில் defined(FEAT_TOOLBAR)
    வெறுமை
gui_mch_show_toolbar(முழு showit)
{
    எனில் (s_toolbarhwnd == வெற்று)
	திரும்பு;

    எனில் (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    இல்லையெனில்
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_மொத்தம் 31

#endif

#எனில் defined(FEAT_GUI_TABLINE)
    நிலையான வெறுமை
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, வெற்று);
    எனில் (wn == வெற்று)
	திரும்பு;

    infow.cbSize = அளவு(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    செருகு_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    நிலையான வெறுமை
show_tabline_popup_menu(வெறுமை)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    எனில் (hold_gui_events || cmdwin_type != 0)
	திரும்பு;

    tab_pmenu = CreatePopupMenu();
    எனில் (tab_pmenu == வெற்று)
	திரும்பு;

    எனில் (first_tabpage->tp_next != வெற்று)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									வெற்று);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    எனில் (rval > 0)
    {
	TCHITTESTINFO htinfo;
	முழு idx;

	எனில் (ScreenToClient(s_tabhwnd, &pt) == 0)
	    திரும்பு;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	எனில் (idx == -1)
	    idx = 0;
	இல்லையெனில்
	    idx += 1;

	send_tabline_menu_event(idx, (முழு)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    வெறுமை
gui_mch_show_tabline(முழு showit)
{
    எனில் (s_tabhwnd == வெற்று)
	திரும்பு;

    எனில் (!showit != !showing_tabline)
    {
	எனில் (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	இல்லையெனில்
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    முழு
gui_mch_showing_tabline(வெறுமை)
{
    திரும்பு s_tabhwnd != வெற்று && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    வெறுமை
gui_mch_update_tabline(வெறுமை)
{
    tabpage_T	*tp;
    TCITEM	tie;
    முழு		nr = 0;
    முழு		curtabidx = 0;
    முழு		tabadded = 0;
    WCHAR	*wstr = வெற்று;

    எனில் (s_tabhwnd == வெற்று)
	திரும்பு;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    க்கு (tp = first_tabpage; tp != வெற்று; tp = tp->tp_next, ++nr)
    {
	எனில் (tp == curtab)
	    curtabidx = nr;

	எனில் (nr >= Tab_Ctrl_Get_Item_மொத்தம்(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_செருகு_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, வெற்று);
	எனில் (wstr != வெற்று)
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
    போது (nr < Tab_Ctrl_Get_Item_மொத்தம்(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    எனில் (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, வெற்று, வெற்று,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    எனில் (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    வெறுமை
gui_mch_set_curtab(முழு nr)
{
    எனில் (s_tabhwnd == வெற்று)
	திரும்பு;

    எனில் (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    வெறுமை
gui_mch_set_dark_theme(முழு dark)
{
    எனில் (pDwmSetWindowAttribute != வெற்று && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		அளவு(dark));

    எனில் (pSetPreferredAppMode != வெற்று && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    எனில் (pFlushMenuThemes != வெற்று && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    நிலையான வெறுமை
dyn_uxtheme_load(வெறுமை)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    எனில் (hUxThemeLib == வெற்று)
	திரும்பு;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (வெறுமை (WINAPI *)(வெறுமை))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    எனில் (pSetPreferredAppMode == வெற்று || pFlushMenuThemes == வெற்று)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = வெற்று;
	திரும்பு;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    வெறுமை
gui_mch_set_fullscreen(முழு flag)
{
    நிலையான RECT normal_rect;
    நிலையான LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    எனில் (!full_screen) // Windows not set yet.
	திரும்பு;

    எனில் (flag)
    {
	எனில் (fullscreen_on)
	    திரும்பு;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = அளவு(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	எனில் (mon == வெற்று || !GetMonitorInfo(mon, &moninfo))
	    திரும்பு;

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
	SetWindowPos(s_hwnd, வெற்று,
		moninfo.rcMonitor.இடது,
		moninfo.rcMonitor.உச்சி,
		moninfo.rcMonitor.வலது - moninfo.rcMonitor.இடது,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.உச்சி,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    இல்லையெனில்
    {
	எனில் (!fullscreen_on)
	    திரும்பு;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, வெற்று,
		normal_rect.இடது,
		normal_rect.உச்சி,
		normal_rect.வலது - normal_rect.இடது,
		normal_rect.bottom - normal_rect.உச்சி,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    வெறுமை
ex_simalt(exarg_T *eap)
{
    char_u	*சாவிகள் = eap->arg;
    முழு		fill_typebuf = FALSE;
    char_u	சாவி_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    போது (*சாவிகள்)
    {
	எனில் (*சாவிகள் == '~')
	    *சாவிகள் = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*சாவிகள், (LPARAM)0);
	சாவிகள்++;
	fill_typebuf = TRUE;
    }
    எனில் (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	சாவி_name[0] = K_SPECIAL;
	சாவி_name[1] = KS_EXTRA;
	சாவி_name[2] = KE_NOP;
	சாவி_name[3] = NUL;
#எனில் defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(வெறுமை)ins_typebuf(சாவி_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    நிலையான வெறுமை
initialise_findrep(char_u *initial_string)
{
    முழு		wword = FALSE;
    முழு		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    எனில் (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    எனில் (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    எனில் (entry_text != வெற்று && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, வெற்று);
	எனில் (p != வெற்று)
	{
	    முழு len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    நிலையான வெறுமை
set_window_title(HWND hwnd, எழுத்து *title)
{
    எனில் (title != வெற்று)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, வெற்று);
	எனில் (wbuf != வெற்று)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    இல்லையெனில்
	(வெறுமை)SetWindowTextW(hwnd, வெற்று);
}

    வெறுமை
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    எனில் (s_findrep_செய்தி != 0)
    {
	எனில் (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	எனில் (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(வெறுமை)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    வெறுமை
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    எனில் (s_findrep_செய்தி != 0)
    {
	எனில் (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	எனில் (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(வெறுமை)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    வெறுமை
gui_mch_mousehide(முழு hide)
{
    எனில் (hide == gui.pointer_hidden)
	திரும்பு;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    நிலையான வெறுமை
gui_mch_show_popupmenu_at(vimmenu_T *menu, முழு x, முழு y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (வெறுமை)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(முழு)0,	    //reserved param
	s_hwnd,
	வெற்று);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    நிலையான வெறுமை
_OnEndSession(வெறுமை)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    நிலையான வெறுமை
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    நிலையான வெறுமை
_OnDestroy(HWND hwnd)
{
    எனில் (!destroying)
	_OnClose(hwnd);
}

    நிலையான வெறுமை
_OnPaint(
    HWND hwnd)
{
    எனில் (IsMinimized(hwnd))
	திரும்பு;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (வெறுமை)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    எனில் (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.இடது = rect.இடது;
	ps.rcPaint.வலது = rect.வலது;
    }

    எனில் (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	எனில் (!screen_cleared || Screen_வரிகள் == வெற்று)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	இல்லையெனில்
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.இடது, ps.rcPaint.உச்சி,
		ps.rcPaint.வலது - ps.rcPaint.இடது + 1,
		ps.rcPaint.bottom - ps.rcPaint.உச்சி + 1);
    }

    EndPaint(hwnd, &ps);
}

    நிலையான வெறுமை
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    முழு cx,
    முழு cy)
{
    எனில் (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    நிலையான வெறுமை
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (வெறுமை)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    நிலையான வெறுமை
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    எனில் (destroying)
	திரும்பு;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (வெறுமை)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    நிலையான LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    திரும்பு DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    வெறுமை
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    வெறுமை
gui_mch_getmouse(முழு *x, முழு *y)
{
    RECT rct;
    POINT mp;

    (வெறுமை)GetWindowRect(s_textArea, &rct);
    (வெறுமை)GetCursorPos(&mp);
    *x = (முழு)(mp.x - rct.இடது);
    *y = (முழு)(mp.y - rct.உச்சி);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    வெறுமை
gui_mch_setmouse(முழு x, முழு y)
{
    RECT rct;

    (வெறுமை)GetWindowRect(s_textArea, &rct);
    (வெறுமை)SetCursorPos(x + gui.border_offset + rct.இடது,
		       y + gui.border_offset + rct.உச்சி);
}

    நிலையான வெறுமை
gui_mswin_get_valid_dimensions(
    முழு w,
    முழு h,
    முழு *valid_w,
    முழு *valid_h,
    முழு *cols,
    முழு *rows)
{
    முழு	    base_width, base_height;

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

    வெறுமை
gui_mch_flash(முழு msec)
{
    RECT    rc;

#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.இடது = 0;
    rc.உச்சி = 0;
    rc.வலது = gui.இலக்கம்_cols * gui.char_width;
    rc.bottom = gui.இலக்கம்_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    நிலையான BOOL
is_point_onscreen(முழு x, முழு y)
{
    POINT   pt = {x, y};

    திரும்பு MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != வெற்று;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    நிலையான BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.இடது;
    p1.y = rc.உச்சி;
    p2.x = rc.வலது - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    எனில் (!is_point_onscreen(p1.x, p1.y))
	திரும்பு FALSE;
    எனில் (!is_point_onscreen(p1.x, p2.y))
	திரும்பு FALSE;
    எனில் (!is_point_onscreen(p2.x, p1.y))
	திரும்பு FALSE;
    எனில் (!is_point_onscreen(p2.x, p2.y))
	திரும்பு FALSE;
    திரும்பு TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    நிலையான முழு
get_scroll_flags(வெறுமை)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    எனில் (!is_window_onscreen(s_hwnd))
	திரும்பு SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    க்கு (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	எனில் (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    எனில் (IntersectRect(&rcDest, &rcVim, &rcOther))
		திரும்பு SW_INVALIDATE;
	}
    திரும்பு 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    நிலையான வெறுமை
intel_gpu_workaround(வெறுமை)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    வெறுமை
gui_mch_delete_வரிகள்(
    முழு	    row,
    முழு	    இலக்கம்_வரிகள்)
{
    RECT	rc;

    rc.இடது = FILL_X(gui.scroll_region_இடது);
    rc.வலது = FILL_X(gui.scroll_region_வலது + 1);
    rc.உச்சி = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -இலக்கம்_வரிகள் * gui.char_height, &rc);
    }
    இல்லையெனில்
#endif
    {
#எனில் defined(FEAT_DIRECTX)
	எனில் (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -இலக்கம்_வரிகள் * gui.char_height,
				    &rc, &rc, வெற்று, வெற்று, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - இலக்கம்_வரிகள் + 1,
						       gui.scroll_region_இடது,
	gui.scroll_region_bot, gui.scroll_region_வலது);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    வெறுமை
gui_mch_செருகு_வரிகள்(
    முழு		row,
    முழு		இலக்கம்_வரிகள்)
{
    RECT	rc;

    rc.இடது = FILL_X(gui.scroll_region_இடது);
    rc.வலது = FILL_X(gui.scroll_region_வலது + 1);
    rc.உச்சி = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, இலக்கம்_வரிகள் * gui.char_height, &rc);
    }
    இல்லையெனில்
#endif
    {
#எனில் defined(FEAT_DIRECTX)
	எனில் (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, இலக்கம்_வரிகள் * gui.char_height,
				    &rc, &rc, வெற்று, வெற்று, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_இடது,
				row + இலக்கம்_வரிகள் - 1, gui.scroll_region_வலது);
}


    வெறுமை
gui_mch_exit(முழு rc UNUSED)
{
#எனில் defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = வெற்று;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (வெறுமை)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    எனில் (s_hwnd != வெற்று)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    நிலையான char_u *
logfont2name(LOGFONTW lf)
{
    எழுத்து	*charset_name;
    எழுத்து	*quality_name;
    எழுத்து	*font_name;
    size_t	res_size;
    எழுத்து	*res;

    font_name = (எழுத்து *)utf16_to_enc(lf.lfFaceName, வெற்று);
    எனில் (font_name == வெற்று)
	திரும்பு வெற்று;
    charset_name = charset_id2name((முழு)lf.lfCharSet);
    quality_name = quality_id2name((முழு)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == வெற்று ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == வெற்று ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    எனில் (res != வெற்று)
    {
	எழுத்து	*p;
	முழு	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	க்கு (p = font_name; *p != NUL; ++p)
	{
	    எனில் (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	எனில் (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(எழுத்து *)res, res_size, "%s:h%d", font_name, points);
	இல்லையெனில்
	    res_len = vim_snprintf_safelen(
		(எழுத்து *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (எழுத்து *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	எனில் (charset_name != வெற்று)
	    res_len += vim_snprintf_safelen((எழுத்து *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	எனில் (quality_name != வெற்று)
	    vim_snprintf((எழுத்து *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    திரும்பு (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    நிலையான வெறுமை
update_im_font(வெறுமை)
{
    LOGFONTW	lf_wide, lf;

    எனில் (p_guifontwide != வெற்று && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, அளவு(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    இல்லையெனில்
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    எனில் (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    வெறுமை
gui_mch_wide_font_changed(வெறுமை)
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

    எனில் (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, அளவு(lf), &lf))
    {
	எனில் (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	எனில் (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    எனில் (!lf.lfItalic)
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
    முழு
gui_mch_init_font(char_u *font_name, முழு fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    எனில் (get_logfont(&lf, font_name, வெற்று, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    எனில் (font == NOFONT)
	திரும்பு FAIL;

#எனில் defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	முழு		    feat_மொத்தம் = 0;
	char_u		    *fp;

	எனில் (font_name != வெற்று)
	{
	    // Find each ":f" option in font_name.
	    க்கு (fp = font_name; *fp != NUL; fp++)
	    {
		எனில் (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    முழு    ti = 0;
		    unsigned முழு param = 1;

		    fp += 2;  // skip ":f"
		    போது (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    எனில் (ti != 4)
			தொடர்;  // invalid tag length

		    எனில் (*fp == '=')
		    {
			fp++;
			param = (unsigned முழு)atoi((எழுத்து *)fp);
			போது (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    எனில் (feat_மொத்தம் < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_மொத்தம்].tag =
			    ((unsigned முழு)tag[0])
			    | ((unsigned முழு)tag[1] << 8)
			    | ((unsigned முழு)tag[2] << 16)
			    | ((unsigned முழு)tag[3] << 24);
			features[feat_மொத்தம்].parameter = param;
			feat_மொத்தம்++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_மொத்தம்);
    }
#endif

    எனில் (font_name == வெற்று)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    எனில் (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    எனில் (p != வெற்று)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	எனில் (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	இல்லையெனில்
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    எனில் (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    எனில் (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	எனில் (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    திரும்பு OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    முழு
gui_mch_maximized(வெறுமை)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = அளவு(WINDOWPLACEMENT);
    எனில் (GetWindowPlacement(s_hwnd, &wp))
    {
	எனில் (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    திரும்பு TRUE;
	எனில் (wp.showCmd == SW_SHOWMINIMIZED)
	    திரும்பு FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	எனில் ((rc.வலது - rc.இடது !=
		    wp.rcNormalPosition.வலது - wp.rcNormalPosition.இடது)
		|| (rc.bottom - rc.உச்சி !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.உச்சி))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    வெறுமை
gui_mch_newfont(வெறுமை)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    எனில் (win_socket_id == 0)
    {
	gui_resize_shell(rect.வலது - rect.இடது
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.உச்சி
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    இல்லையெனில்
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.வலது - rect.இடது,
	    rect.bottom - rect.உச்சி - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    வெறுமை
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == வெற்று ? "VIM" : (எழுத்து *)title));
}

#எனில் defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
நிலையான LPCSTR mshape_idcs[] =
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

    வெறுமை
mch_set_mouse_shape(முழு shape)
{
    LPCSTR idc;

    எனில் (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    இல்லையெனில்
    {
	எனில் (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	இல்லையெனில்
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(வெற்று, idc));
	எனில் (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (வெறுமை)GetCursorPos(&mp);
	    (வெறுமை)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#எனில் defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    நிலையான WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    முழு len;
    WCHAR *res;

    tmp = convert_filter(s);
    எனில் (tmp == வெற்று)
	திரும்பு வெற்று;
    len = (முழு)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    திரும்பு res;
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
	முழு saving,
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
    முழு			i;
    WCHAR		*titlep = வெற்று;
    WCHAR		*extp = வெற்று;
    WCHAR		*initdirp = வெற்று;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    எனில் (dflt == வெற்று)
	fileBuf[0] = NUL;
    இல்லையெனில்
    {
	wp = enc_to_utf16(dflt, வெற்று);
	எனில் (wp == வெற்று)
	    fileBuf[0] = NUL;
	இல்லையெனில்
	{
	    க்கு (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# இல்லையெனில்
    fileStruct.lStructSize = அளவு(fileStruct);
# endif

    எனில் (title != வெற்று)
	titlep = enc_to_utf16(title, வெற்று);
    fileStruct.lpstrTitle = titlep;

    எனில் (ext != வெற்று)
	extp = enc_to_utf16(ext, வெற்று);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    எனில் (initdir != வெற்று && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, வெற்று);
	எனில் (initdirp != வெற்று)
	{
	    க்கு (wp = initdirp; *wp != NUL; ++wp)
		எனில் (*wp == '/')
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
    எனில் (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    எனில் (saving)
	ret = GetSaveFileNameW(&fileStruct);
    இல்லையெனில்
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    எனில் (!ret)
	திரும்பு வெற்று;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, வெற்று);
    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    திரும்பு q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    நிலையான char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    எனில் (res != வெற்று)
    {
	க்கு (i = 0; i < s_len; ++i)
	    எனில் (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    இல்லையெனில்
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    திரும்பு res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    திரும்பு gui_mch_browse(0, title, (char_u *)_("Not Used"), வெற்று,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    நிலையான வெறுமை
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    எழுத்து    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, வெற்று, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    எனில் (fnames != வெற்று)
	க்கு (i = 0; i < cFiles; ++i)
	{
	    எனில் (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, வெற்று);
	    இல்லையெனில்
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    எனில் (fnames == வெற்று)
	திரும்பு;

    முழு kbd_modifiers = get_active_modifiers();

    எனில் ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    எனில் ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    எனில் ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    நிலையான முழு
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    முழு pos)
{
    நிலையான UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    முழு		dragging = FALSE;
    முழு		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = அளவு(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    எனில் (sb == வெற்று)
	திரும்பு 0;

    எனில் (sb->wp != வெற்று)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    இல்லையெனில்	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->மதிப்பு;

    தேர்வு (code)
    {
	நிலை SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    எனில் (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    நிறுத்து;
	நிலை SB_LINEDOWN:
	    val++;
	    நிறுத்து;
	நிலை SB_LINEUP:
	    val--;
	    நிறுத்து;
	நிலை SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    நிறுத்து;
	நிலை SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    நிறுத்து;
	நிலை SB_உச்சி:
	    val = 0;
	    நிறுத்து;
	நிலை SB_BOTTOM:
	    val = sb_info->max;
	    நிறுத்து;
	நிலை SB_ENDSCROLL:
	    எனில் (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		எனில் (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    நிறுத்து;

	இயல்பு:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    திரும்பு 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    எனில் (sb->wp != வெற்று)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_இடது) ? SBAR_வலது : SBAR_இடது].id;

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

    திரும்பு 0;
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

நிலையான BalloonEval  *cur_beval = வெற்று;
நிலையான UINT_PTR	    beval_timer_id = 0;
நிலையான DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
நிலையான UINT	s_menu_id = 100;
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
நிலையான PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	மாறிலி எழுத்து *caption);
நிலையான LPWORD lpwAlign(LPWORD);
நிலையான முழு nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#எனில் defined(FEAT_MENU) && defined(FEAT_TEAROFF)
நிலையான வெறுமை gui_mch_tearoff(char_u *title, vimmenu_T *menu, முழு initX, முழு initY);
#endif
நிலையான வெறுமை get_dialog_font_metrics(வெறுமை);

நிலையான முழு dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
நிலையான வெறுமை initialise_toolbar(வெறுமை);
நிலையான வெறுமை update_toolbar_size(வெறுமை);
நிலையான LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_செய்தி, WPARAM wParam, LPARAM lParam);
நிலையான முழு get_toolbar_bitmap(vimmenu_T *menu);
#இல்லையெனில்
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
நிலையான வெறுமை initialise_tabline(வெறுமை);
நிலையான LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_செய்தி, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
நிலையான LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
நிலையான char_u *GetResultStr(HWND hwnd, முழு GCS, முழு *lenp);
#endif
#எனில் defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
வகைவரை கட்டமைப்பு tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
வகைவரை HANDLE HIMC;
# endif

நிலையான HINSTANCE hLibImm = வெற்று;
நிலையான LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
நிலையான HIMC (WINAPI *pImmGetContext)(HWND);
நிலையான HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
நிலையான BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
நிலையான BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
நிலையான BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
நிலையான BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
நிலையான BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
நிலையான BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
நிலையான BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
நிலையான BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
நிலையான வெறுமை dyn_imm_load(வெறுமை);
#இல்லையெனில்
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
    நிலையான முழு
gui_mswin_get_menu_height(
    முழு	    fix_window)	    // If TRUE, resize window if menu height changed
{
    நிலையான முழு	old_menu_height = -1;

    RECT    rc1, rc2;
    முழு	    இலக்கம்;
    முழு	    menu_height;

    எனில் (gui.menu_is_active)
	இலக்கம் = Get_Menu_Item_மொத்தம்(s_menuBar);
    இல்லையெனில்
	இலக்கம் = 0;

    எனில் (இலக்கம் == 0)
	menu_height = 0;
    இல்லையெனில் எனில் (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    இல்லையெனில்
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	எனில் (gui.starting)
	    menu_height = rc1.bottom - rc1.உச்சி + 1;
	இல்லையெனில்
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, இலக்கம் - 1, &rc2);
	    menu_height = rc2.bottom - rc1.உச்சி + 1;
	}
    }

    எனில் (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    திரும்பு menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    நிலையான long
mouse_vertical_scroll_step(வெறுமை)
{
    UINT val;
    எனில் (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	திரும்பு (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    திரும்பு 3; // Safe default;
}

    நிலையான long
mouse_horizontal_scroll_step(வெறுமை)
{
    UINT val;
    எனில் (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	திரும்பு (long)val;
    திரும்பு 3; // Safe default;
}

    நிலையான வெறுமை
init_mouse_wheel(வெறுமை)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    நிலையான வெறுமை
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, முழு horizontal)
{
    முழு		button;
    win_T	*wp;
    முழு		modifiers = 0;
    முழு		kbd_modifiers;
    முழு		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    எனில் (wp != வெற்று && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	எனில் (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_இடது : MSCR_வலது;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	இல்லையெனில்
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

    எனில் (wp == வெற்று || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    எனில் (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    இல்லையெனில்
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    எனில் ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    எனில் ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    எனில் ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    நிலையான முழு
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = அளவு(NONCLIENTMETRICSW);
    எனில் (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    அளவு(NONCLIENTMETRICSW),
	    &nm,
	    0))
	திரும்பு FAIL;
    *lf = nm.lfMenuFont;
    திரும்பு OK;
}
#endif


#எனில் defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    நிலையான வெறுமை
set_tabline_font(வெறுமை)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    எனில் (gui_w32_get_menu_font(&lfSysmenu) != OK)
	திரும்பு;

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
#இல்லையெனில்
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    நிலையான LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    தேர்வு (param)
    {
	நிலை SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    நிறுத்து;
	நிலை SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    நிறுத்து;
	நிலை SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு 0;
}

#ifdef FEAT_NETBEANS_INTG
    நிலையான வெறுமை
_OnWindowPosChanged(
    HWND hwnd,
    மாறிலி LPWINDOWPOS lpwpos)
{
    நிலையான முழு x = 0, y = 0, cx = 0, cy = 0;
    வெளி முழு WSInitialized;

    எனில் (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


நிலையான HWND hwndTip = வெற்று;

    நிலையான வெறுமை
show_sizing_tip(முழு cols, முழு rows)
{
    TOOLINFOA ti;
    எழுத்து buf[32];

    ti.cbSize = அளவு(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    எனில் (hwndTip == வெற்று)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, வெற்று,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, வெற்று, GetModuleHandle(வெற்று), வெற்று);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    இல்லையெனில்
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    நிலையான வெறுமை
destroy_sizing_tip(வெறுமை)
{
    எனில் (hwndTip == வெற்று)
	திரும்பு;

    DestroyWindow(hwndTip);
    hwndTip = வெற்று;
}

    நிலையான முழு
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    முழு	    w, h;
    முழு	    valid_w, valid_h;
    முழு	    w_offset, h_offset;
    முழு	    cols, rows;

    w = lprc->வலது - lprc->இடது;
    h = lprc->bottom - lprc->உச்சி;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    எனில் (fwSide == WMSZ_இடது || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->இடது += w_offset;
    இல்லையெனில் எனில் (fwSide == WMSZ_வலது || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->வலது -= w_offset;

    எனில் (fwSide == WMSZ_உச்சி || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->உச்சி += h_offset;
    இல்லையெனில் எனில் (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    திரும்பு TRUE;
}

#ifdef FEAT_GUI_TABLINE
    நிலையான வெறுமை
_OnRButtonUp(HWND hwnd, முழு x, முழு y, UINT சாவி_Flags)
{
    எனில் (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	எனில் (pt.y < rect.உச்சி)
	{
	    show_tabline_popup_menu();
	    திரும்பு;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, சாவி_Flags, DefWindowProcW);
}

    நிலையான வெறுமை
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, முழு x, முழு y, UINT சாவி_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    எனில் (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	எனில் (pt.y < rect.உச்சி)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, சாவி_Flags, DefWindowProcW);
}
#endif

    நிலையான UINT
_OnNCHitTest(HWND hwnd, முழு xPos, முழு yPos)
{
    UINT	result;
    முழு		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    எனில் (result != HTCLIENT)
	திரும்பு result;

#ifdef FEAT_GUI_TABLINE
    எனில் (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	எனில் (yPos < rct.உச்சி)
	    திரும்பு result;
    }
#endif
    (வெறுமை)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    எனில் (xPos < 48) // <VN> TODO should use system metric?
	திரும்பு HTBOTTOMLEFT;
    இல்லையெனில்
	திரும்பு HTBOTTOMRIGHT;
}

#எனில் defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    நிலையான LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    தேர்வு (hdr->code)
    {
	நிலை TTN_GETDISPINFOW:
	நிலை TTN_GETDISPINFO:
	    {
		char_u		*str = வெற்று;
		நிலையான வெறுமை	*tt_text = வெற்று;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		எனில் (gui_mch_showing_tabline()
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
		    எனில் (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			முழு idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			எனில் (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    எனில் (tp != வெற்று)
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
		இல்லையெனில்
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(வேர்_menu, idButton);
		    எனில் (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		எனில் (str == வெற்று)
		    நிறுத்து;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		எனில் (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, வெற்று);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		இல்லையெனில்
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    எனில் (len < அளவு(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == வெற்று))
			vim_strncpy((char_u *)lpdi->szText, str,
				அளவு(lpdi->szText) - 1);
		    இல்லையெனில்
			lpdi->lpszText = tt_text;
		}
	    }
	    நிறுத்து;

# ifdef FEAT_GUI_TABLINE
	நிலை TCN_SELCHANGE:
	    எனில் (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		திரும்பு 0L;
	    }
	    நிறுத்து;

	நிலை NM_RCLICK:
	    எனில் (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		திரும்பு 0L;
	    }
	    நிறுத்து;
# endif

	இயல்பு:
	    நிறுத்து;
    }
    திரும்பு DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#எனில் defined(MENUHINTS) && defined(FEAT_MENU)
    நிலையான LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    எனில் (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	நிலையான முழு  did_menu_tip = FALSE;

	எனில் (did_menu_tip)
	{
	    செய்தி_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(வேர்_menu, (UINT)LOWORD(wParam));
	எனில் (pMenu != வெற்று && pMenu->strings[MENU_INDEX_TIP] != வெற்று)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = அளவு(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    எனில் (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++செய்தி_hist_off;
		செய்தி((எழுத்து *)pMenu->strings[MENU_INDEX_TIP]);
		--செய்தி_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	திரும்பு 0L;
    }
    திரும்பு DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    நிலையான BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    முழு		old_width, old_height;
    முழு		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, அளவு(lf), &lf);
    lf.lfHeight = lf.lfHeight * (முழு)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    எனில் (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    இல்லையெனில்
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    திரும்பு TRUE;
}

    நிலையான LRESULT
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
    InvalidateRect(hwnd, வெற்று, TRUE);

    s_in_dpichanged = FALSE;
    திரும்பு 0L;
}


    நிலையான LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_செய்தி,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_செய்தி, lParam);

    s_u_செய்தி = u_செய்தி;
    s_wParam = wParam;

    தேர்வு (u_செய்தி)
    {
	HANDLE_செய்தி(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_செய்தி(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_செய்தி(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_செய்தி(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_செய்தி(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_செய்தி(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_செய்தி(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_செய்தி(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_செய்தி(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_செய்தி(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_செய்தி(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_செய்தி(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_செய்தி(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_செய்தி(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_செய்தி(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_செய்தி(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    நிலை WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	திரும்பு FALSE;		// Do NOT allow system to go down.

    நிலை WM_ENDSESSION:
	எனில் (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    திரும்பு 0L;
	}
	நிறுத்து;

    நிலை WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (முழு)(short)LOWORD(lParam));
	திரும்பு 0L;

    நிலை WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	எனில் (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((முழு)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (முழு)(short)LOWORD(lParam));
	    திரும்பு 0L;
	}
#ifdef FEAT_MENU
	இல்லையெனில்
	    திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
#endif

    நிலை WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
#இல்லையெனில்
	திரும்பு 0L;
#endif

    நிலை WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	நிறுத்து;

    நிலை WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	திரும்பு _DuringSizing((UINT)wParam, (LPRECT)lParam);

    நிலை WM_MOUSEWHEEL:
    நிலை WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_செய்தி == WM_MOUSEHWHEEL);
	திரும்பு 0L;

	// Notification for change in SystemParametersInfo()
    நிலை WM_SETTINGCHANGE:
	திரும்பு _OnSettingChange((UINT)wParam);

#எனில் defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    நிலை WM_NOTIFY:
	திரும்பு _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#எனில் defined(MENUHINTS) && defined(FEAT_MENU)
    நிலை WM_MENUSELECT:
	திரும்பு _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    நிலை WM_IME_NOTIFY:
	எனில் (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
	திரும்பு 1L;

    நிலை WM_IME_COMPOSITION:
	எனில் (!_OnImeComposition(hwnd, wParam, lParam))
	    திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
	திரும்பு 1L;
#endif
    நிலை WM_GETDPISCALEDSIZE:
	திரும்பு _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    நிலை WM_DPICHANGED:
	திரும்பு _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    இயல்பு:
#ifdef MSWIN_FIND_REPLACE
	எனில் (u_செய்தி == s_findrep_செய்தி && s_findrep_செய்தி != 0)
	    _OnFindRepl();
#endif
	நிறுத்து;
    }

    திரும்பு DefWindowProcW(hwnd, u_செய்தி, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = வெற்று;

    நிலையான BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    எழுத்து	buf[2048];
    எழுத்து	*title = (எழுத்து *)lParam;

    எனில் (GetWindowText(hwnd, buf, அளவு(buf)))
    {
	எனில் (strstr(buf, title) != வெற்று)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, வெற்று, "MDIClient", வெற்று);
	    எனில் (vim_parent_hwnd != வெற்று)
		திரும்பு FALSE;
	}
    }
    திரும்பு TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    வெறுமை
gui_mch_set_parent(எழுத்து *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    எனில் (vim_parent_hwnd == வெற்று)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    நிலையான வெறுமை
ole_error(எழுத்து *arg)
{
    எழுத்து buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#எனில் defined(GUI_MAY_SPAWN)
    நிலையான எழுத்து *
gvim_error(வெறுமை)
{
    எழுத்து *செய்தி = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    எனில் (starting)
    {
	mch_errmsg(செய்தி);
	mch_errmsg("\n");
	mch_exit(2);
    }
    திரும்பு செய்தி;
}

    எழுத்து *
gui_mch_do_spawn(char_u *arg)
{
    முழு			len;
# எனில் defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = வெற்று;
    LPWSTR		tofree1 = வெற்று;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = வெற்று, p, warg, tofree2 = வெற்று;
    STARTUPINFOW	si = {அளவு(si)};
    PROCESS_INFORMATION pi;

    எனில் (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	செல் error;
    p = wcsrchr(name, L'\\');
    எனில் (p == வெற்று)
	செல் error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# இல்லையெனில்
    wcscpy(p + 1, L"gvim.exe");
# endif

# எனில் defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    எனில் (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_வரி_W();
	// Skip 1st argument.
	போது (*p && *p != L' ' && *p != L'\t')
	{
	    எனில் (*p == L'"')
	    {
		// Skip quoted strings
		போது (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# எனில் defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    இல்லையெனில்
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	முழு	ret;

	session = vim_tempname('s', FALSE);
	எனில் (session == வெற்று)
	    செல் error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	எனில் (p_bg == வெற்று)
	{
	    p_bg = savebg;
	    செல் error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	எனில் (!ret)
	    செல் error;
	wsession = enc_to_utf16(session, வெற்று);
	எனில் (wsession == வெற்று)
	    செல் error;
	len = (முழு)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	எனில் (cmd == வெற்று)
	{
	    vim_free(wsession);
	    செல் error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    எனில் (arg != வெற்று)
    {
	warg = enc_to_utf16(arg, வெற்று);
	எனில் (warg == வெற்று)
	    செல் error;
	tofree2 = warg;
    }
    இல்லையெனில்
	warg = L"";

    // Set up the new command line.
    len = (முழு)wcslen(name) + (முழு)wcslen(cmd) + (முழு)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    எனில் (newcmd == வெற்று)
	செல் error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    எனில் (!CreateProcessW(வெற்று, newcmd, வெற்று, வெற்று, TRUE, 0,
		வெற்று, வெற்று, &si, &pi))
	செல் error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# எனில் defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    எனில் (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    திரும்பு gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    வெறுமை
gui_mch_prepare(முழு *argc, எழுத்து **argv)
{
    முழு		silent = FALSE;
    முழு		idx;

    // Check for special OLE command line parameters
    எனில் ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	எனில் (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	இல்லையெனில்
	    idx = 1;

	// Register Vim as an OLE Automation server
	எனில் (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#இல்லையெனில்
	    எனில் (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	எனில் (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#இல்லையெனில்
	    எனில் (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	எனில் (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#இல்லையெனில்
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    எனில் (mch_is_gui_executable())
# endif
    {
	முழு	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	எனில் (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	முழு arg;

	க்கு (arg = 1; arg < *argc; arg++)
	    எனில் (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * அளவு(எழுத்து *));
		argv[*argc] = வெற்று;
		நிறுத்து;	// enough?
	    }
    }
#endif
}

    நிலையான வெறுமை
load_dpi_func(வெறுமை)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    எனில் (hUser32 == வெற்று)
	செல் fail;

    pGetDpiForSystem = (UINT (WINAPI *)(வெறுமை))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (முழு (WINAPI *)(முழு, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    எனில் (pSetThreadDpiAwarenessContext != வெற்று)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	எனில் (oldctx != வெற்று)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    எனில் (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    திரும்பு;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = வெற்று;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = வெற்று;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = வெற்று;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    முழு
gui_mch_init(வெறுமை)
{
    மாறிலி WCHAR szVimWndClassW[] = VIM_CLASSW;
    மாறிலி WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    எனில் (s_hwnd != வெற்று)
	செல் theend;

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
    எனில் (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(வெற்று, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = வெற்று;
	wndclassw.lpszClassName = szVimWndClassW;

	எனில் (RegisterClassW(&wndclassw) == 0)
	    திரும்பு FAIL;
    }

    எனில் (vim_parent_hwnd != வெற்று)
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
		vim_parent_hwnd, வெற்று,
		g_hinst, வெற்று);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	எனில் (s_hwnd == வெற்று)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    இல்லையெனில்
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	எனில் (IsWindow((HWND)win_socket_id) <= 0)
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
		வெற்று, வெற்று,
		g_hinst, வெற்று);
	எனில் (s_hwnd != வெற்று && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    எனில் (s_hwnd == வெற்று)
	திரும்பு FAIL;

    எனில் (pGetDpiForWindow != வெற்று)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#எனில் defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    எனில் (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = வெற்று;
	wndclassw.hCursor = LoadCursor(வெற்று, IDC_ARROW);
	wndclassw.hbrBackground = வெற்று;
	wndclassw.lpszMenuName = வெற்று;
	wndclassw.lpszClassName = szTextAreaClassW;

	எனில் (RegisterClassW(&wndclassw) == 0)
	    திரும்பு FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, வெற்று,
	g_hinst, வெற்று);

    எனில் (s_textArea == வெற்று)
	திரும்பு FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = வெற்று;

	எனில் (mch_icon_load(&hIcon) == OK && hIcon != வெற்று)
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
    s_findrep_செய்தி = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = அளவு(s_findrep_struct);
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
    எனில் (p_rop)
	(வெறுமை)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    திரும்பு OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    நிலையான வெறுமை
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    எனில் (mon != வெற்று)
    {
	moninfo.cbSize = அளவு(MONITORINFO);
	எனில் (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    திரும்பு;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    வெறுமை
gui_mch_set_shellsize(
	முழு width,
	முழு height,
	முழு min_width UNUSED,
	முழு min_height UNUSED,
	முழு base_width UNUSED,
	முழு base_height UNUSED,
	முழு direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    முழு		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    எனில் (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    எனில் (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    இல்லையெனில்
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
    window_rect.வலது = window_rect.இடது + win_width;
    window_rect.bottom = window_rect.உச்சி + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    எனில் (!s_in_dpichanged)
    {
	எனில் ((direction & RESIZE_HOR)
		&& window_rect.வலது > workarea_rect.வலது)
	    OffsetRect(&window_rect,
		    workarea_rect.வலது - window_rect.வலது, 0);

	எனில் ((direction & RESIZE_HOR)
		&& window_rect.இடது < workarea_rect.இடது)
	    OffsetRect(&window_rect,
		    workarea_rect.இடது - window_rect.இடது, 0);

	எனில் ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	எனில் ((direction & RESIZE_VERT)
		&& window_rect.உச்சி < workarea_rect.உச்சி)
	    OffsetRect(&window_rect,
		    0, workarea_rect.உச்சி - window_rect.உச்சி);
    }

    MoveWindow(s_hwnd, window_rect.இடது, window_rect.உச்சி,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    வெறுமை
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    போது (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    எனில் (sb->scroll_shift > 0)
	++size;

    info.cbSize = அளவு(info);
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
    வெறுமை
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    வெறுமை
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    வெறுமை
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    வெறுமை
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
    நிலையான LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    எனில் (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	திரும்பு lResult;
    தேர்வு (dwCommand)
    {
	நிலை IMN_SETOPENSTATUS:
	    எனில் (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		எனில் (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		எனில் (State & MODE_செருகு)
		{
# எனில் defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    எனில் (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			முழு old_row = gui.row;
			முழு old_col = gui.col;

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
	    நிறுத்து;
    }
    pImmReleaseContext(hWnd, hImc);
    திரும்பு lResult;
}

    நிலையான LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    முழு		len;

    எனில் ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	திரும்பு 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    எனில் (ret != வெற்று)
    {
	add_to_உள்ளீடு_buf_csi(ret, len);
	vim_free(ret);
	திரும்பு 1;
    }
    திரும்பு 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    நிலையான char_u *
GetResultStr(HWND hwnd, முழு GCS, முழு *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = வெற்று;
    char_u	*convbuf = வெற்று;

    எனில் (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	திரும்பு வெற்று;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, வெற்று, 0);
    எனில் (ret <= 0)
	திரும்பு வெற்று;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + அளவு(WCHAR));
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / அளவு(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    திரும்பு convbuf;
}
#endif

// For global functions we need prototypes.
#எனில் defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    வெறுமை
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    எனில் (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    வெறுமை
im_set_position(முழு row, முழு col)
{
    HIMC hImc;

    எனில் (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	எனில் (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    வெறுமை
im_set_active(முழு active)
{
    HIMC	hImc;
    நிலையான HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    எனில் (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	திரும்பு;
    }
# endif

    எனில் (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	திரும்பு;

    எனில் (p_imdisable)
    {
	எனில் (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    எனில் (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    இல்லையெனில் எனில் (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    எனில் (!hImc)
	திரும்பு;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    எனில் (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	நிலையான DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	நிலையான BOOL bSaved = FALSE;

	எனில் (active)
	{
	    // if we have a saved conversion status, restore it
	    எனில் (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	இல்லையெனில்
	{
	    // save conversion status and disable korean
	    எனில் (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    முழு
im_get_status(வெறுமை)
{
    முழு		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    எனில் (!gui.in_use && !gui.starting)
	திரும்பு mbyte_im_get_status();
# endif

    எனில் (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    திரும்பு status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    நிலையான வெறுமை
latin9_to_ucs(char_u *text, முழு len, WCHAR *unicodebuf)
{
    முழு		c;

    போது (--len >= 0)
    {
	c = *text++;
	தேர்வு (c)
	{
	    நிலை 0xa4: c = 0x20ac; நிறுத்து;   // euro
	    நிலை 0xa6: c = 0x0160; நிறுத்து;   // S hat
	    நிலை 0xa8: c = 0x0161; நிறுத்து;   // S -hat
	    நிலை 0xb4: c = 0x017d; நிறுத்து;   // Z hat
	    நிலை 0xb8: c = 0x017e; நிறுத்து;   // Z -hat
	    நிலை 0xbc: c = 0x0152; நிறுத்து;   // OE
	    நிலை 0xbd: c = 0x0153; நிறுத்து;   // oe
	    நிலை 0xbe: c = 0x0178; நிறுத்து;   // Y
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
    நிலையான வெறுமை
RevOut( HDC hdc,
	முழு col,
	முழு row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    முழு		ix;

    க்கு (ix = 0; ix < (முழு)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    நிலையான வெறுமை
draw_வரி(
    முழு		x1,
    முழு		y1,
    முழு		x2,
    முழு		y2,
    COLORREF	color)
{
#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_வரி(s_dwc, x1, y1, x2, y2, color);
    இல்லையெனில்
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, வெற்று);
	// Note: LineTo() excludes the last pixel in the line.
	வரி_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    நிலையான வெறுமை
set_pixel(
    முழு		x,
    முழு		y,
    COLORREF	color)
{
#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    இல்லையெனில்
#endif
	SetPixel(s_hdc, x, y, color);
}

    நிலையான வெறுமை
fill_rect(
    மாறிலி RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    இல்லையெனில்
#endif
    {
	HBRUSH	hbr2;

	எனில் (hbr == வெற்று)
	    hbr2 = CreateSolidBrush(color);
	இல்லையெனில்
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	எனில் (hbr == வெற்று)
	    DeleteBrush(hbr2);
    }
}

    வெறுமை
gui_mch_draw_string(
    முழு		row,
    முழு		col,
    char_u	*text,
    முழு		len,
    முழு		flags)
{
    நிலையான முழு	*padding = வெற்று;
    நிலையான முழு	pad_size = 0;
    மாறிலி RECT	*pcliprect = வெற்று;
    UINT	foptions = 0;
    நிலையான WCHAR *unicodebuf = வெற்று;
    நிலையான முழு   *unicodepdy = வெற்று;
    நிலையான முழு	unibuflen = 0;
    முழு		n = 0;
    முழு		y;

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
    நிலையான HBRUSH	hbr_cache[2] = {வெற்று, வெற்று};
    நிலையான guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    நிலையான முழு		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    எனில் (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.இடது = FILL_X(col);
	rc.உச்சி = FILL_Y(row);
	எனில் (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.வலது = FILL_X(col + mb_string2cells(text, len));
	}
	இல்லையெனில்
	    rc.வலது = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	எனில் (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	இல்லையெனில் எனில் (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	இல்லையெனில்
	{
	    எனில் (hbr_cache[brush_lru] != வெற்று)
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
	எனில் (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	இல்லையெனில் எனில் (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    எனில் (pad_size != Columns || padding == வெற்று || padding[0] != gui.char_width)
    {
	முழு	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(முழு, pad_size);
	எனில் (padding != வெற்று)
	    க்கு (i = 0; i < pad_size; i++)
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
    எனில் (enc_utf8)
	க்கு (n = 0; n < len; ++n)
	    எனில் (text[n] >= 0x80)
		நிறுத்து;

#எனில் defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    எனில் (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    எனில் ((enc_utf8
		|| (enc_codepage > 0 && (முழு)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == வெற்று || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(முழு, len);

	unibuflen = len;
    }

    எனில் (enc_utf8 && n < len && unicodebuf != வெற்று)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	முழு		i;
	முழு		wlen;	// string length in words
	முழு		cells;	// cell width of string up to composing char
	முழு		cw;	// width of current cell
	முழு		c;

	wlen = 0;
	cells = 0;
	க்கு (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    எனில் (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    இல்லையெனில்
	    {
		unicodebuf[wlen++] = c;
	    }

	    எனில் (utf_iscomposing(c))
		cw = 0;
	    இல்லையெனில்
	    {
		cw = utf_char2cells(c);
		எனில் (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    எனில் (unicodepdy != வெற்று)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		எனில் (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		இல்லையெனில்
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#எனில் defined(FEAT_DIRECTX)
	எனில் (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	இல்லையெனில்
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    இல்லையெனில் எனில் ((enc_codepage > 0 && (முழு)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	எனில் (unicodebuf != வெற்று)
	{
	    எனில் (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    இல்லையெனில்
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(எழுத்து *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    எனில் (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		எனில் (unicodepdy != வெற்று)
		{
		    முழு i;
		    முழு cw;

		    க்கு (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			எனில் (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    இல்லையெனில்
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	எனில் (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (எழுத்து *)text, len, padding);
	இல்லையெனில்
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (எழுத்து *)text, len, padding);
    }

    // Underline
    எனில் (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	எனில் (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_வரி(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    எனில் (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_வரி(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    எனில் (flags & DRAW_UNDERC)
    {
	முழு			x;
	முழு			offset;
	நிலையான மாறிலி முழு	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	க்கு (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    வெறுமை
gui_mch_flush(வெறுமை)
{
#எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    நிலையான வெறுமை
clear_rect(RECT *rcp)
{
    fill_rect(rcp, வெற்று, gui.back_pixel);
}


    வெறுமை
gui_mch_get_screen_dimensions(முழு *screen_w, முழு *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.வலது - workarea_rect.இடது
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.உச்சி
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#எனில் defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    வெறுமை
gui_mch_add_menu(
    vimmenu_T	*menu,
    முழு		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    எனில் (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, வெற்று);
	எனில் (wn == வெற்று)
	    திரும்பு;

	infow.cbSize = அளவு(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	செருகு_Menu_Item_W((parent == வெற்று)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    எனில் (parent == வெற்று)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    இல்லையெனில் எனில் (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    வெறுமை
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (வெறுமை)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (முழு)mp.x, (முழு)mp.y);
}

    வெறுமை
gui_make_popup(char_u *path_name, முழு mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    எனில் (menu == வெற்று)
	திரும்பு;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    எனில் (mouse_pos)
    {
	முழு	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    இல்லையெனில் எனில் (curwin != வெற்று)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    செய்தி_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (முழு)p.x, (முழு)p.y);
}

# எனில் defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    வெறுமை
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    எனில் (menu != வெற்று)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    வெறுமை
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    முழு		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = வெற்று;

# ifdef FEAT_TEAROFF
    எனில் (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	செருகு_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    இல்லையெனில்
# endif
# ifdef FEAT_TOOLBAR
    எனில் (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	எனில் (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	இல்லையெனில்
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
    இல்லையெனில்
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, வெற்று);
	எனில் (wn != வெற்று)
	{
	    செருகு_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	எனில் (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    வெறுமை
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    எனில் (menu->submenu_id == (HMENU)-1)
    {
	முழு iButton;

	iButton = (முழு)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    இல்லையெனில்
# endif
    {
	எனில் (menu->parent != வெற்று
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != வெற்று)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	இல்லையெனில்
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	எனில் (menu->submenu_id != வெற்று)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	எனில் (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	எனில் (menu->parent != வெற்று
		&& menu->parent->children != வெற்று
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
    நிலையான வெறுமை
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    முழு		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    எனில் (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.இடது - rct.இடது;
	y = (trect.உச்சி -  rct.bottom  + roct.bottom);
    }
    இல்லையெனில்
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    எனில் (menu->children != வெற்று)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	எனில் (IsWindow(menu->tearoff_handle))
	    (வெறுமை) SetWindowPos(menu->tearoff_handle,
				வெற்று,
				(முழு)trect.இடது,
				(முழு)trect.உச்சி,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    வெறுமை
gui_mch_menu_grey(
    vimmenu_T	*menu,
    முழு	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    எனில் (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    இல்லையெனில்
# endif
	(வெறுமை)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    எனில் ((menu->parent != வெற்று) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	எனில் (menu->children == வெற்று)
	    menuID = (WORD)(menu->id);
	இல்லையெனில்
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	எனில் (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#எனில் defined(FEAT_GUI_DIALOG)
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
    நிலையான LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    எனில் (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(வெறுமை)SetFocus(hwnd);
	எனில் (dialog_default_button > IDCANCEL)
	    (வெறுமை)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	இல்லையெனில்
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (வெறுமை)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	திரும்பு FALSE;
    }

    எனில் (message == WM_COMMAND)
    {
	முழு	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	எனில் (button >= DLG_NONBUTTON_CONTROL)
	    திரும்பு TRUE;

	// If the edit box exists, copy the string.
	எனில் (s_textfield != வெற்று)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, வெற்று);
	    எனில் (p != வெற்று)
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
	எனில் (button == IDOK)
	{
	    எனில் (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	இல்லையெனில்
	    EndDialog(hwnd, button - IDCANCEL);
	திரும்பு TRUE;
    }

    எனில் ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
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

நிலையான மாறிலி எழுத்து *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    முழு
gui_mch_dialog(
    முழு		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    முழு		 dfltbutton,
    char_u	*textfield,
    முழு		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    முழு		இலக்கம்_Buttons;
    முழு		*buttonWidths, *buttonPositions;
    முழு		buttonYpos;
    முழு		nchar, i;
    DWORD	lStyle;
    முழு		dlgwidth = 0;
    முழு		dlgheight;
    முழு		editboxheight;
    முழு		horizWidth = 0;
    முழு		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    முழு		fontHeight;
    முழு		textWidth, minButtonWidth, messageWidth;
    முழு		maxDialogWidth;
    முழு		maxDialogHeight;
    முழு		scroll_flag = 0;
    முழு		vertical;
    முழு		dlgPaddingX;
    முழு		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    முழு		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    முழு		l;
    முழு		dlg_icon_width;
    முழு		dlg_icon_height;
    முழு		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    எனில் (!(gui.in_use || gui.starting))
#  endif
	எனில் (silent_mode)
	    திரும்பு dfltbutton;   // return default option
# endif

    எனில் (s_hwnd == வெற்று)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    இல்லையெனில்
	dpi = pGetDpiForSystem();

    எனில் ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    எனில் (p == வெற்று)
	திரும்பு -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    எனில் (tbuffer == வெற்று)
	திரும்பு -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    இலக்கம்_Buttons = 1;
    க்கு (i = 0; tbuffer[i] != '\0'; i++)
    {
	எனில் (tbuffer[i] == DLG_BUTTON_SEP)
	    இலக்கம்_Buttons++;
    }
    எனில் (dfltbutton >= இலக்கம்_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(முழு, இலக்கம்_Buttons);
    எனில் (buttonWidths == வெற்று)
	திரும்பு -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(முழு, இலக்கம்_Buttons);
    எனில் (buttonPositions == வெற்று)
	திரும்பு -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    எனில் (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    இல்லையெனில்
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
    எனில் (s_hwnd == வெற்று)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.வலது - workarea_rect.இடது - 100;
	எனில் (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.உச்சி - 150;
    }
    இல்லையெனில்
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.வலது - rect.இடது
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	எனில் (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.உச்சி
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	எனில் (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, அளவு(எழுத்து), 500);
    செய்
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = வெற்று;
	க்கு (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    எனில் (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    எனில் (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		எனில் (last_white != வெற்று)
		{
		    // break the line just after a space
		    எனில் (pend > last_white)
			ga.ga_len -= (முழு)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = வெற்று;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		தொடர்;
	    }

	    போது (--l >= 0)
		ga_append(&ga, *pend++);
	}
	எனில் (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } போது (*pend != NUL);

    எனில் (ga.ga_data != வெற்று)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    எனில் (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != வெற்று && vim_strchr(p_go, GO_VERTICAL) != வெற்று);
    எனில் (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	செய்
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    எனில் (pend == வெற்று)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (முழு)(pend - pstart));
	    எனில் (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} போது (*pend != NUL);

	எனில் (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	இல்லையெனில் எனில் (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    எனில் (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	செய்
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    எனில் (pend == வெற்று)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (முழு)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    எனில் (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} போது (*pend != NUL);
    }

    எனில் (dlgwidth < DLG_MIN_WIDTH)
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
    எனில் (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * இலக்கம்_Buttons;
    இல்லையெனில்
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    எனில் (textfield != வெற்று)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    எனில் (dlgheight > maxDialogHeight)
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
    எனில் (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (முழு)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    இல்லையெனில்
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    எனில் (textfield != வெற்று)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    எனில் (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    க்கு (i = 0; i < இலக்கம்_Buttons; i++)
    {
	// get end of this button.
	க்கு (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	எனில் (*pend)
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
	எனில் (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (எழுத்து *)pstart);
	}
	இல்லையெனில்
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (எழுத்து *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += இலக்கம்_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_இடது|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (எழுத்து *)message);

    // Edit box
    எனில் (textfield != வெற்று)
    {
	p = add_dialog_element(p, ES_இடது|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (எழுத்து *)textfield);
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
    எனில் (textfield != வெற்று)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    இல்லையெனில்
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = வெற்று;
    }

    // show the dialog box modally and get a return value
    nchar = (முழு)DialogBoxIndirect(
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
    (வெறுமை)SetFocus(s_hwnd);

    திரும்பு nchar;
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
    நிலையான PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    மாறிலி எழுத்து *caption)
{
    முழு nchar;

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

    திரும்பு p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    நிலையான LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    திரும்பு (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    நிலையான முழு
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    முழு		nChar = 0;
    முழு		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    முழு		i;
    WCHAR	*wn;

    எனில் (use_enc && enc_codepage >= 0 && (முழு)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, வெற்று);
	எனில் (wn != வெற்று)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (முழு)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    எனில் (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    க்கு (i = 0; i < nChar; ++i)
	எனில் (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    திரும்பு nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    நிலையான HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    க்கு ( ; menu != வெற்று; menu = menu->next)
    {
	எனில் (menu->modes == 0)	// this menu has just been deleted
	    தொடர்;
	எனில் (menu_is_separator(menu->dname))
	    தொடர்;
	எனில் ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    திரும்பு menu->submenu_id;
    }
    திரும்பு வெற்று;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    நிலையான LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    எனில் (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	திரும்பு (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    எனில் (message == WM_COMMAND)
    {
	எனில் ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    எனில் (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(வெறுமை)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (முழு)rect.வலது - 8,
			 (முழு)mp.y,
			 (முழு)0,	    // reserved param
			 s_hwnd,
			 வெற்று);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	இல்லையெனில்
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(வெறுமை)SetFocus(s_hwnd);
	திரும்பு TRUE;
    }
    எனில் ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	திரும்பு TRUE;
    }

    // When moved around, give main window the focus back.
    எனில் (message == WM_EXITSIZEMOVE)
	(வெறுமை)SetActiveWindow(s_hwnd);

    திரும்பு FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    நிலையான வெறுமை
get_dialog_font_metrics(வெறுமை)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    எனில் (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    இல்லையெனில்
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

#எனில் defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    நிலையான வெறுமை
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    முழு		initX,
    முழு		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    முழு		template_len;
    முழு		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*உச்சி_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    முழு		col, spaceWidth, len;
    முழு		columnWidths[2];
    char_u	*label, *text;
    முழு		acLen = 0;
    முழு		nameLen;
    முழு		padding0, padding1, padding2 = 0;
    முழு		sepPadding=0;
    முழு		x;
    முழு		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    முழு		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    எனில் (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	எனில் (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, வெற்று, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	திரும்பு;
    }

    /*
     * Create a new tearoff.
     */
    எனில் (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    எனில் (p == வெற்று)
	திரும்பு;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    எனில் (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    இல்லையெனில்
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
    க்கு (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    எனில் (text != வெற்று && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (முழு)STRLEN(text));
		எனில் (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    எனில் (pmenu->children != வெற்று)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    எனில் (columnWidths[1] == 0)
    {
	// no accelerators
	எனில் (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	இல்லையெனில்
	    columnWidths[0] += spaceWidth;
    }
    இல்லையெனில்
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    எனில் (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (முழு)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (முழு)STRLEN(title));
    எனில் (textWidth > dlgwidth)
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
    எனில் (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    இல்லையெனில்
	*p++ = PixelToDialogX(initX); // x
    எனில் (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    இல்லையெனில்
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
    எனில் (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (முழு)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    இல்லையெனில்
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
    எனில் (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    இல்லையெனில்
	menu = menu->children;
    உச்சி_menu = menu;
    க்கு ( ; menu != வெற்று; menu = menu->next)
    {
	எனில் (menu->modes == 0)	// this menu has just been deleted
	    தொடர்;
	எனில் (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    தொடர்;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	எனில் (((எழுத்து *)p - (எழுத்து *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    எனில் (newp != வெற்று)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (எழுத்து *)p - (எழுத்து *)pdlgtemplate);
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
	len = nameLen = (முழு)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (முழு)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	எனில் (menu->actext != வெற்று)
	{
	    acLen = (முழு)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	இல்லையெனில்
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	எனில் (menu->children == வெற்று)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	இல்லையெனில்
	{
	    len += (முழு)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	எனில் (label == வெற்று)
	    நிறுத்து;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	எனில் (text == வெற்று)
	    text = label + nameLen;	    // no actext, use whole name
	போது (padding0-- > 0)
	    *text++ = ' ';
	எனில் (menu->actext != வெற்று)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	போது (padding1-- > 0)
	    *text++ = ' ';
	எனில் (menu->children != வெற்று)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	இல்லையெனில்
	{
	    போது (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_இடது,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (எழுத்து *)label);
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
	    (LPARAM)உச்சி_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (வெறுமை)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#எனில் defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    நிலையான வெறுமை
initialise_toolbar(வெறுமை)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    வெற்று,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    அளவு(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != வெற்று);

    update_toolbar_size();
}

    நிலையான வெறுமை
update_toolbar_size(வெறுமை)
{
    முழு		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = அளவு(TBMETRICS);
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

    நிலையான LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_செய்தி,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_செய்தி, lParam);
    திரும்பு CallWindowProc(s_toolbar_wndproc, hwnd, u_செய்தி, wParam, lParam);
}

    நிலையான முழு
get_toolbar_bitmap(vimmenu_T *menu)
{
    முழு i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    எனில் (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = வெற்று;

	எனில் (menu->iconfile != வெற்று)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			வெற்று,
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
	எனில் (hbitmap == வெற்று
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != வெற்று ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    வெற்று,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	எனில் (hbitmap != வெற்று)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = வெற்று;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (முழு)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    எனில் (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_மொத்தம்)
	i = menu->iconidx;

    திரும்பு i;
}
#endif

#எனில் defined(FEAT_GUI_TABLINE)
    நிலையான வெறுமை
initialise_tabline(வெறுமை)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, வெற்று, g_hinst, வெற்று);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    நிலையான tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = வெற்று;

    எனில் (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	எனில் (s_tabhwnd == hWnd)
	{
	    முழு idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    எனில் (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    திரும்பு ptp;
}

நிலையான POINT	    s_pt = {0, 0};
நிலையான HCURSOR      s_hCursor = வெற்று;

    நிலையான LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_செய்தி,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    முழு		nCenter;
    முழு		idx0;
    முழு		idx1;

    HandleMouseHide(u_செய்தி, lParam);

    தேர்வு (u_செய்தி)
    {
	நிலை WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		நிறுத்து;
	    }
	நிலை WM_MOUSEMOVE:
	    எனில் (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		எனில் (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(வெற்று, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    எனில் (tp != வெற்று)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.இடது + (rect.வலது - rect.இடது) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			எனில் ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			இல்லையெனில் எனில் ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    நிறுத்து;
	நிலை WM_LBUTTONUP:
	    {
		எனில் (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		நிறுத்து;
	    }
	நிலை WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		எனில் (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		நிறுத்து;
	    }
	இயல்பு:
	    நிறுத்து;
    }

    திரும்பு CallWindowProc(s_tabline_wndproc, hwnd, u_செய்தி, wParam, lParam);
}
#endif

#எனில் defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    வெறுமை
gui_mch_set_foreground(வெறுமை)
{
    எனில் (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#எனில் defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    நிலையான வெறுமை
dyn_imm_load(வெறுமை)
{
    hLibImm = vimLoadLib("imm32.dll");
    எனில் (hLibImm == வெற்று)
	திரும்பு;

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

    எனில் (       pImmGetCompositionStringW == வெற்று
	    || pImmGetContext == வெற்று
	    || pImmAssociateContext == வெற்று
	    || pImmReleaseContext == வெற்று
	    || pImmGetOpenStatus == வெற்று
	    || pImmSetOpenStatus == வெற்று
	    || pImmGetCompositionFontW == வெற்று
	    || pImmSetCompositionFontW == வெற்று
	    || pImmSetCompositionWindow == வெற்று
	    || pImmGetConversionStatus == வெற்று
	    || pImmSetConversionStatus == வெற்று)
    {
	FreeLibrary(hLibImm);
	hLibImm = வெற்று;
	pImmGetContext = வெற்று;
	திரும்பு;
    }

    திரும்பு;
}

#endif

#எனில் defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

வகைவரை கட்டமைப்பு _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    வெறுமை
gui_mch_drawsign(முழு row, முழு col, முழு typenr)
{
    signicon_t *sign;
    முழு x, y, w, h;

    எனில் (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == வெற்று)
	திரும்பு;

# எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    தேர்வு (sign->uType)
    {
	நிலை IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    நிறுத்து;
	நிலை IMAGE_ICON:
	நிலை IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, வெற்று, DI_NORMAL);
	    நிறுத்து;
# ifdef FEAT_XPM_W32
	நிலை IMAGE_XPM:
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
	    நிறுத்து;
# endif
    }
}

    நிலையான வெறுமை
close_signicon_image(signicon_t *sign)
{
    எனில் (sign == வெற்று)
	திரும்பு;

    தேர்வு (sign->uType)
    {
	நிலை IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    நிறுத்து;
	நிலை IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    நிறுத்து;
	நிலை IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    நிறுத்து;
# ifdef FEAT_XPM_W32
	நிலை IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    நிறுத்து;
# endif
    }
}

    வெறுமை *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = வெற்று;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    எனில் (ext > signfile)
    {
	முழு do_load = 1;

	எனில் (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	இல்லையெனில் எனில் (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	இல்லையெனில் எனில் (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	இல்லையெனில்
	    do_load = 0;

	எனில் (do_load)
	    sign.hImage = (HANDLE)LoadImage(வெற்று, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	எனில் (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((எழுத்து *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = வெற்று;
    எனில் (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != வெற்று)
	*psign = sign;

    எனில் (!psign)
    {
	எனில் (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    திரும்பு (வெறுமை *)psign;

}

    வெறுமை
gui_mch_destroy_sign(வெறுமை *sign)
{
    எனில் (sign == வெற்று)
	திரும்பு;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#எனில் defined(FEAT_BEVAL_GUI)

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

    நிலையான வெறுமை
make_tooltip(BalloonEval *beval, எழுத்து *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    எனில் (pti == வெற்று)
	திரும்பு;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    வெற்று, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, வெற்று, g_hinst, வெற்று);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = அளவு(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, வெற்று);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    எனில் (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.வலது);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.இடது = pt.x - 3;
    pti->rect.உச்சி = pt.y - 3;
    pti->rect.வலது = pt.x + 3;
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

    நிலையான வெறுமை
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    நிலையான VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_செய்தி UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    எனில் (cur_beval == வெற்று || cur_beval->showState == ShS_SHOWING || !p_beval)
	திரும்பு;

    GetCursorPos(&pt);
    எனில் (WindowFromPoint(pt) != s_textArea)
	திரும்பு;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    எனில் (!PtInRect(&rect, pt))
	திரும்பு;

    எனில் (last_user_activity > 0
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

	எனில் (cur_beval->செய்தி_CB != வெற்று)
	    (*cur_beval->செய்தி_CB)(cur_beval, 0);
    }
}

    வெறுமை
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    வெறுமை
gui_mch_enable_beval_area(BalloonEval *beval)
{
    எனில் (beval == வெற்று)
	திரும்பு;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    வெறுமை
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->செய்தி);
    beval->செய்தி = mesg == வெற்று ? வெற்று : vim_strsave(mesg);
    எனில் (beval->செய்தி == வெற்று)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	திரும்பு;
    }

    எனில் (beval->showState == ShS_SHOWING)
	திரும்பு;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    எனில் (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (எழுத்து *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    வெறுமை	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    வெறுமை	(*mesgCB)(BalloonEval *, முழு),
    வெறுமை	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    எனில் (mesg != வெற்று && mesgCB != வெற்று)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	திரும்பு வெற்று;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    எனில் (beval != வெற்று)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->செய்தி = mesg;
	beval->செய்தி_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	எனில் (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    திரும்பு beval;
}

    நிலையான வெறுமை
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    எனில் (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	திரும்பு;

    எனில் (cur_beval == வெற்று)
	திரும்பு;

    தேர்வு (pnmh->code)
    {
	நிலை TTN_SHOW:
	    நிறுத்து;
	நிலை TTN_எடு: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    நிறுத்து;
	நிலை TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    நிறுத்து;
	நிலை TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    நிறுத்து;
    }
}

    நிலையான வெறுமை
track_user_activity(UINT u_செய்தி)
{
    எனில் ((u_செய்தி >= WM_MOUSEFIRST && u_செய்தி <= WM_MOUSELAST)
	    || (u_செய்தி >= WM_KEYFIRST && u_செய்தி <= WM_KEYLAST))
	last_user_activity = Get_Tick_மொத்தம்();
}

    வெறுமை
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#எனில் defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    வெறுமை
netbeans_draw_multisign_indicator(முழு row)
{
    முழு i;
    முழு y;
    முழு x;

    எனில் (!netbeans_active())
	திரும்பு;

    x = 0;
    y = TEXT_Y(row);

# எனில் defined(FEAT_DIRECTX)
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    க்கு (i = 0; i < gui.char_height - 3; i++)
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

#எனில் defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    நிலையான முழு
test_gui_w32_sendevent_mouse(dict_T *args)
{
    எனில் (!dict_has_சாவி(args, "row") || !dict_has_சாவி(args, "col"))
	திரும்பு FALSE;

    // Note: "move" is optional, requires fewer arguments
    முழு move = (முழு)dict_get_bool(args, "move", FALSE);

    எனில் (!move && (!dict_has_சாவி(args, "button")
	    || !dict_has_சாவி(args, "multiclick")
	    || !dict_has_சாவி(args, "modifiers")))
	திரும்பு FALSE;

    முழு row = (முழு)dict_get_எண்(args, "row");
    முழு col = (முழு)dict_get_எண்(args, "col");

    எனில் (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	எனில் (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    முழு pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    முழு pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	இல்லையெனில்
	    gui_mouse_moved(col, row);
    }
    இல்லையெனில்
    {
	முழு button = (முழு)dict_get_எண்(args, "button");
	முழு repeated_click = (முழு)dict_get_எண்(args, "multiclick");
	int_u mods = (முழு)dict_get_எண்(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    திரும்பு TRUE;
}

    நிலையான முழு
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    உள்ளீடு_2 inputs[1];
    உள்ளீடு_2 modkeys[3];
    SecureZeroMemory(inputs, அளவு(உள்ளீடு_2));
    SecureZeroMemory(modkeys, 3 * அளவு(உள்ளீடு_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    எனில் (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_எண்_def(args, "keycode", 0);
	எனில் (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    திரும்பு FALSE;
	}

	BOOL is_Mod_சாவி = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	முழு mods = (முழு)dict_get_எண்(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	எனில் (mods && STRICMP(event, "keydown") == 0 && !is_Mod_சாவி)
	{
	    முழு n = 0;
	    எனில் (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = உள்ளீடு_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    எனில் (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = உள்ளீடு_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    எனில் (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = உள்ளீடு_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    எனில் (n)
	    {
		(வெறுமை)SetForegroundWindow(s_hwnd);
		Send_உள்ளீடு(n, modkeys, அளவு(உள்ளீடு_2));
	    }
	}

	inputs[0].type = உள்ளீடு_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	எனில் (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    எனில் (!is_Mod_சாவி)
		unwrapMods = TRUE;
	}

	(வெறுமை)SetForegroundWindow(s_hwnd);
	Send_உள்ளீடு(ARRAYSIZE(inputs), inputs, அளவு(உள்ளீடு_2));
	vim_free(event);

	எனில் (unwrapMods)
	{
	    modkeys[0].type = உள்ளீடு_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = உள்ளீடு_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = உள்ளீடு_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (வெறுமை)SetForegroundWindow(s_hwnd);
	    Send_உள்ளீடு(3, modkeys, அளவு(உள்ளீடு_2));
	}
    }
    இல்லையெனில்
    {
	எனில் (event == வெற்று)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

    நிலையான முழு
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    முழு handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    எனில் (strategy)
    {
	எனில் (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	இல்லையெனில் எனில் (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    எனில் (!handled)
    {
	எனில் (strategy == வெற்று)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	இல்லையெனில்
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	திரும்பு FALSE;
    }
    திரும்பு TRUE;
}


    முழு
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    எனில் (STRICMP(event, "key") == 0)
	திரும்பு test_gui_w32_sendevent_keyboard(args);
    இல்லையெனில் எனில் (STRICMP(event, "mouse") == 0)
	திரும்பு test_gui_w32_sendevent_mouse(args);
    இல்லையெனில் எனில் (STRICMP(event, "set_keycode_trans_strategy") == 0)
	திரும்பு test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    இல்லையெனில்
    {
	semsg(_(e_invalid_மதிப்பு_for_argument_str_str), "event", event);
	திரும்பு FALSE;
    }
}
#endif

#எனில் defined(FEAT_IMAGE_GUI)

வகைவரை கட்டமைப்பு
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
    நிலையான வெறுமை
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, முழு iw, முழு ih, image_format_T fmt)
{
    முழு		y, x;
    முழு		stride = iw * 4;
    முழு		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    க்கு (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	க்கு (x = 0; x < iw; x++)
	{
	    எனில் (fmt == IMAGE_FORMAT_RGBA)
	    {
		முழு a = s[3];

		எனில் (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		இல்லையெனில் எனில் (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		இல்லையெனில்
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    இல்லையெனில்
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

    முழு
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    முழு		iw, ih;
    வெறுமை	*bits = வெற்று;

    எனில் (ctx == வெற்று)
	திரும்பு FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = அளவு(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_மொத்தம் = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, வெற்று, 0);
    எனில் (hbm == வெற்று || bits == வெற்று)
    {
	எனில் (hbm != வெற்று)
	    DeleteObject(hbm);
	திரும்பு false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    எனில் (mem_dc == வெற்று)
    {
	DeleteObject(hbm);
	திரும்பு false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    திரும்பு OK;
}

    வெறுமை
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    முழு
image_placement_gui_init(image_placement_T *place UNUSED)
{
    திரும்பு OK;
}

    வெறுமை
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    வெறுமை
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    முழு		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    எனில் (rects == வெற்று || n_rects == 0)
	திரும்பு;

# எனில் defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    எனில் (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    க்கு (முழு i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	முழு		row, col;
	முழு		posx, posy;
	முழு		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    வெறுமை
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
