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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_താക്കോൽ_OFF			0	// no dead key
#define DEAD_താക്കോൽ_SET_DEFAULT		1	// dead key pressed
#define DEAD_താക്കോൽ_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_താക്കോൽ_SKIP_ON_CHAR		3	// skip next _OnChar()

#എങ്കിൽ defined(FEAT_DIRECTX)
സ്ഥിതം DWriteContext *s_dwc = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ s_directx_enabled = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ശൂന്യ && enc_utf8)
സ്ഥിതം പൂർണ്ണസംഖ്യ directx_enabled(ശൂന്യം);
സ്ഥിതം ശൂന്യം directx_binddc(ശൂന്യം);
#endif

#ifdef FEAT_MENU
സ്ഥിതം പൂർണ്ണസംഖ്യ gui_mswin_get_menu_height(പൂർണ്ണസംഖ്യ fix_window);
#അല്ലെങ്കിൽ
# define gui_mswin_get_menu_height(fix_window)	0
#endif

തരനിർവചനം ഘടന keycode_trans_strategy {
    ശൂന്യം (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
    ശൂന്യം (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
    ശൂന്യം (*ptr_process_message_usual_താക്കോൽ) (UINT /*vk*/, സ്ഥിരം സന്ദേശം_2* /*pmsg*/);
    പൂർണ്ണസംഖ്യ  (*ptr_get_active_modifiers)(ശൂന്യം);
    പൂർണ്ണസംഖ്യ  (*is_experimental)(ശൂന്യം);
} keycode_trans_strategy;

// forward declarations for input instance initializer
സ്ഥിതം ശൂന്യം _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
സ്ഥിതം ശൂന്യം _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
സ്ഥിതം ശൂന്യം process_message_usual_താക്കോൽ_experimental(UINT /*vk*/, സ്ഥിരം സന്ദേശം_2* /*pmsg*/);
സ്ഥിതം പൂർണ്ണസംഖ്യ  get_active_modifiers_experimental(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  is_experimental_true(ശൂന്യം);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_താക്കോൽ_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
സ്ഥിതം ശൂന്യം _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
സ്ഥിതം ശൂന്യം _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, പൂർണ്ണസംഖ്യ /*cRepeat UNUSED*/);
സ്ഥിതം ശൂന്യം process_message_usual_താക്കോൽ_classic(UINT /*vk*/, സ്ഥിരം സന്ദേശം_2* /*pmsg*/);
സ്ഥിതം പൂർണ്ണസംഖ്യ  get_active_modifiers_classic(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  is_experimental_false(ശൂന്യം);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_താക്കോൽ_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = ശൂന്യ;

സ്ഥിതം പൂർണ്ണസംഖ്യ is_experimental_true(ശൂന്യം)
{
    തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ is_experimental_false(ശൂന്യം)
{
    തിരികെ 0;
}

/*
 * Initialize the keycode translation strategy.
 */
സ്ഥിതം ശൂന്യം keycode_trans_strategy_init(ശൂന്യം)
{
    സ്ഥിരം അക്ഷരം *strategy = ശൂന്യ;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    എങ്കിൽ (strategy == ശൂന്യ)
    {
	തിരികെ;
    }

    എങ്കിൽ (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	തിരികെ;
    }

    എങ്കിൽ (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	തിരികെ;
    }

}

#എങ്കിൽ defined(FEAT_RENDER_OPTIONS)
    പൂർണ്ണസംഖ്യ
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    പൂർണ്ണസംഖ്യ	    dx_enable = 0;
    പൂർണ്ണസംഖ്യ	    dx_flags = 0;
    ദശാംശം   dx_gamma = 0.0f;
    ദശാംശം   dx_contrast = 0.0f;
    ദശാംശം   dx_level = 0.0f;
    പൂർണ്ണസംഖ്യ	    dx_geom = 0;
    പൂർണ്ണസംഖ്യ	    dx_renmode = 0;
    പൂർണ്ണസംഖ്യ	    dx_taamode = 0;
    // parse string as rendering options.
    വേണ്ടി (p = s; p != ശൂന്യ && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  മൂല്യം[128];

	copy_option_part(&p, item, വലുപ്പം(item), ",");
	എങ്കിൽ (p == ശൂന്യ)
	    നിർത്തുക;
	q = &item[0];
	copy_option_part(&q, name, വലുപ്പം(name), ":");
	എങ്കിൽ (q == ശൂന്യ)
	    തിരികെ FAIL;
	copy_option_part(&q, മൂല്യം, വലുപ്പം(മൂല്യം), ":");

	എങ്കിൽ (STRCMP(name, "type") == 0)
	{
	    എങ്കിൽ (STRCMP(മൂല്യം, "directx") == 0)
		dx_enable = 1;
	    അല്ലെങ്കിൽ
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (ദശാംശം)atof((അക്ഷരം *)മൂല്യം);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (ദശാംശം)atof((അക്ഷരം *)മൂല്യം);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (ദശാംശം)atof((അക്ഷരം *)മൂല്യം);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((അക്ഷരം *)മൂല്യം);
	    എങ്കിൽ (dx_geom < 0 || dx_geom > 2)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((അക്ഷരം *)മൂല്യം);
	    എങ്കിൽ (dx_renmode < 0 || dx_renmode > 6)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((അക്ഷരം *)മൂല്യം);
	    എങ്കിൽ (dx_taamode < 0 || dx_taamode > 3)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	അല്ലെങ്കിൽ
	    തിരികെ FAIL;
    }

    എങ്കിൽ (!gui.in_use)
	തിരികെ OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    എങ്കിൽ (dx_enable)
    {
	എങ്കിൽ (!directx_enabled())
	    തിരികെ FAIL;
	DWriteContext_SetRenderingParams(s_dwc, ശൂന്യ);
	എങ്കിൽ (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    എങ്കിൽ (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    എങ്കിൽ (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    എങ്കിൽ (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    എങ്കിൽ (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    എങ്കിൽ (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    എങ്കിൽ (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    തിരികെ OK;
# അല്ലെങ്കിൽ
    തിരികെ FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (പൂർണ്ണസംഖ്യ)(short)LOWORD(lParam), (പൂർണ്ണസംഖ്യ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (പൂർണ്ണസംഖ്യ)(short)LOWORD(lParam), (പൂർണ്ണസംഖ്യ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (പൂർണ്ണസംഖ്യ)(short)LOWORD(lParam), (പൂർണ്ണസംഖ്യ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#അന്തര്ഭാവ "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#അന്തര്ഭാവ <shellapi.h>
#അന്തര്ഭാവ <commctrl.h>
#അന്തര്ഭാവ <windowsx.h>

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

സ്ഥിതം ശൂന്യം _OnPaint(HWND hwnd);
സ്ഥിതം ശൂന്യം fill_rect(സ്ഥിരം RECT *rcp, HBRUSH hbr, COLORREF color);
സ്ഥിതം ശൂന്യം clear_rect(RECT *rcp);

സ്ഥിതം WORD		s_dlgfntheight;		// height of the dialog font
സ്ഥിതം WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
സ്ഥിതം HMENU		s_menuBar = ശൂന്യ;
#endif
#ifdef FEAT_TEAROFF
സ്ഥിതം ശൂന്യം rebuild_tearoff(vimmenu_T *menu);
സ്ഥിതം HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
സ്ഥിതം പൂർണ്ണസംഖ്യ		s_busy_processing = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
സ്ഥിതം UINT		s_findrep_സന്ദേശം = 0;
സ്ഥിതം FINDREPLACEW	s_findrep_struct;
സ്ഥിതം HWND		s_findrep_hwnd = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = ശൂന്യ;
സ്ഥിതം HDC		s_hdc = ശൂന്യ;
സ്ഥിതം HBRUSH		s_brush = ശൂന്യ;

#ifdef FEAT_TOOLBAR
സ്ഥിതം HWND		s_toolbarhwnd = ശൂന്യ;
സ്ഥിതം WNDPROC		s_toolbar_wndproc = ശൂന്യ;
#endif

#ifdef FEAT_GUI_TABLINE
സ്ഥിതം HWND		s_tabhwnd = ശൂന്യ;
സ്ഥിതം WNDPROC		s_tabline_wndproc = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ		showing_tabline = 0;
#endif

സ്ഥിതം WPARAM		s_wParam = 0;

സ്ഥിതം HWND		s_textArea = ശൂന്യ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
സ്ഥിതം bool		s_textarea_bg_only = false;

സ്ഥിതം UINT		s_u_സന്ദേശം = 0;

സ്ഥിതം char_u		*s_textfield; // Used by dialogs to pass back strings

സ്ഥിതം പൂർണ്ണസംഖ്യ		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
സ്ഥിതം പൂർണ്ണസംഖ്യ allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
തരനിർവചനം HANDLE DPI_AWARENESS_CONTEXT;

തരനിർവചനം എണ്ണൽ DPI_AWARENESS {
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
സ്ഥിതം പൂർണ്ണസംഖ്യ		s_dpi = DEFAULT_DPI;
സ്ഥിതം BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
സ്ഥിതം DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
സ്ഥിതം RECT		s_suggested_rect;

സ്ഥിതം UINT (WINAPI *pGetDpiForSystem)(ശൂന്യം) = ശൂന്യ;
സ്ഥിതം UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ (WINAPI *pGetSystemMetricsForDpi)(പൂർണ്ണസംഖ്യ, UINT) = ശൂന്യ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
സ്ഥിതം DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = ശൂന്യ;
#ifdef FEAT_MBYTE_IME
സ്ഥിതം DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ശൂന്യ;
#endif

സ്ഥിതം HINSTANCE hLibDwm = ശൂന്യ;
സ്ഥിതം HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
സ്ഥിതം ശൂന്യം dyn_dwm_load(ശൂന്യം);

സ്ഥിതം പൂർണ്ണസംഖ്യ fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

സ്ഥിതം HINSTANCE hUxThemeLib = ശൂന്യ;
സ്ഥിതം DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = ശൂന്യ;
സ്ഥിതം ശൂന്യം (WINAPI *pFlushMenuThemes)(ശൂന്യം) = ശൂന്യ;
സ്ഥിതം ശൂന്യം dyn_uxtheme_load(ശൂന്യം);
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ WINAPI
stubGetSystemMetricsForDpi(പൂർണ്ണസംഖ്യ nIndex, UINT dpi UNUSED)
{
    തിരികെ GetSystemMetrics(nIndex);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
adjust_fontsize_by_dpi(പൂർണ്ണസംഖ്യ size)
{
    തിരികെ size * s_dpi / (പൂർണ്ണസംഖ്യ)pGetDpiForSystem();
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
adjust_by_system_dpi(പൂർണ്ണസംഖ്യ size)
{
    തിരികെ size * (പൂർണ്ണസംഖ്യ)pGetDpiForSystem() / DEFAULT_DPI;
}

#എങ്കിൽ defined(FEAT_DIRECTX)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
directx_enabled(ശൂന്യം)
{
    എങ്കിൽ (s_dwc != ശൂന്യ)
	തിരികെ 1;
    അല്ലെങ്കിൽ എങ്കിൽ (s_directx_load_attempted)
	തിരികെ 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    തിരികെ s_dwc != ശൂന്യ ? 1 : 0;
}

    സ്ഥിതം ശൂന്യം
directx_binddc(ശൂന്യം)
{
    എങ്കിൽ (s_textArea == ശൂന്യ)
	തിരികെ;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

ബാഹ്യ പൂർണ്ണസംഖ്യ current_font_height;	    // this is in os_mswin.c

സ്ഥിതം ഘടന
{
    UINT    താക്കോൽ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_താക്കോലുകൾ[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_ഇടത്,		'k', 'l'},
    {VK_വലത്,		'k', 'r'},

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
    {VK_തിരുകുക,		'k', 'I'},
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
സ്ഥിതം പൂർണ്ണസംഖ്യ	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
സ്ഥിതം പൂർണ്ണസംഖ്യ	s_getting_focus = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ	s_x_pending;
സ്ഥിതം പൂർണ്ണസംഖ്യ	s_y_pending;
സ്ഥിതം UINT	s_kFlags_pending;
സ്ഥിതം UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
സ്ഥിതം പൂർണ്ണസംഖ്യ	s_timed_out = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ	dead_താക്കോൽ = DEAD_താക്കോൽ_OFF;
സ്ഥിതം UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
സ്ഥിതം ശൂന്യം Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
സ്ഥിതം ശൂന്യം track_user_activity(UINT u_സന്ദേശം);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
സ്ഥിതം LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
സ്ഥിതം LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
സ്ഥിതം LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#എങ്കിൽ defined(FEAT_BROWSE)
സ്ഥിതം char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    സ്ഥിതം ശൂന്യം
print_windows_error(ശൂന്യം)
{
    LPVOID  lp_സന്ദേശം_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  ശൂന്യ, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_സന്ദേശം_Buf, 0, ശൂന്യ);
    TRACE1("Error: %s\n", lp_സന്ദേശം_Buf);
    LocalFree(lp_സന്ദേശം_Buf);
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

സ്ഥിതം പൂർണ്ണസംഖ്യ		blink_state = BLINK_NONE;
സ്ഥിതം long_u		blink_waittime = 700;
സ്ഥിതം long_u		blink_ontime = 400;
സ്ഥിതം long_u		blink_offtime = 250;
സ്ഥിതം UINT_PTR		blink_timer = 0;

    പൂർണ്ണസംഖ്യ
gui_mch_is_blinking(ശൂന്യം)
{
    തിരികെ blink_state != BLINK_NONE;
}

    പൂർണ്ണസംഖ്യ
gui_mch_is_blink_off(ശൂന്യം)
{
    തിരികെ blink_state == BLINK_OFF;
}

    ശൂന്യം
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    സ്ഥിതം VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_സന്ദേശം UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    സന്ദേശം_2 സന്ദേശം;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ശൂന്യ, idEvent);

    // Eat spurious WM_TIMER messages
    ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    എങ്കിൽ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(ശൂന്യ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    അല്ലെങ്കിൽ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(ശൂന്യ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    സ്ഥിതം ശൂന്യം
gui_mswin_rm_blink_timer(ശൂന്യം)
{
    സന്ദേശം_2 സന്ദേശം;

    എങ്കിൽ (blink_timer == 0)
	തിരികെ;

    KillTimer(ശൂന്യ, blink_timer);
    // Eat spurious WM_TIMER messages
    ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ശൂന്യം
gui_mch_stop_blink(പൂർണ്ണസംഖ്യ may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    എങ്കിൽ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    ശൂന്യം
gui_mch_start_blink(ശൂന്യം)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    എങ്കിൽ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(ശൂന്യ, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    സ്ഥിതം VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_സന്ദേശം UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    സന്ദേശം_2 സന്ദേശം;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ശൂന്യ, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    എങ്കിൽ (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    സ്ഥിതം ശൂന്യം
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    dead_താക്കോൽ = DEAD_താക്കോൽ_SET_DEFAULT;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
char_to_string(പൂർണ്ണസംഖ്യ ch, char_u *string, പൂർണ്ണസംഖ്യ slen, പൂർണ്ണസംഖ്യ had_alt)
{
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    WCHAR	wstring[2];
    char_u	*ws = ശൂന്യ;

    എങ്കിൽ (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	തിരികെ 0;
    }
    അല്ലെങ്കിൽ
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    എങ്കിൽ (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ശൂന്യ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	എങ്കിൽ (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ശൂന്യ);
	    എങ്കിൽ (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    അല്ലെങ്കിൽ
    {
	ws = utf16_to_enc(wstring, &len);
	എങ്കിൽ (ws == ശൂന്യ)
	    len = 0;
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    എങ്കിൽ (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    വേണ്ടി (i = 0; i < len; ++i)
	എങ്കിൽ (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (പൂർണ്ണസംഖ്യ)KE_CSI;
	    len += 2;
	}

    തിരികെ len;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_active_modifiers_experimental(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ modifiers = 0;

    എങ്കിൽ (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    എങ്കിൽ (Get_താക്കോൽ_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    എങ്കിൽ ((modifiers & MOD_MASK_CTRL) && (Get_താക്കോൽ_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    എങ്കിൽ (!(Get_താക്കോൽ_State(VK_CONTROL) & 0x8000)
	    &&  (Get_താക്കോൽ_State(VK_RMENU) & 0x8000)
	    && !(Get_താക്കോൽ_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    തിരികെ modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_active_modifiers_classic(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ modifiers = 0;

    എങ്കിൽ (Get_താക്കോൽ_State(VK_SHIFT) & 0x8000)
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
    എങ്കിൽ (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    തിരികെ modifiers;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_active_modifiers(ശൂന്യം)
{
    തിരികെ get_active_modifiers_experimental();
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_active_modifiers_via_ptr(ശൂന്യം)
{
    // marshal to corresponding implementation
    തിരികെ keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    സ്ഥിതം ശൂന്യം
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    സ്ഥിതം ശൂന്യം
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    char_u	string[40];
    പൂർണ്ണസംഖ്യ		len = 0;
    പൂർണ്ണസംഖ്യ		modifiers;
    പൂർണ്ണസംഖ്യ		ch = cch;   // special keys are negative

    എങ്കിൽ (dead_താക്കോൽ == DEAD_താക്കോൽ_SKIP_ON_CHAR)
	തിരികെ;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    എങ്കിൽ (dead_താക്കോൽ != DEAD_താക്കോൽ_TRANSIENT_IN_ON_CHAR)
	dead_താക്കോൽ = DEAD_താക്കോൽ_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_താക്കോൽ(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_താക്കോലുകൾ;
    ch = may_adjust_താക്കോൽ_for_ctrl(modifiers, ch);
    --no_reduce_താക്കോലുകൾ;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ശൂന്യ);
    എങ്കിൽ (ch == CSI)
	ch = K_CSI;

    എങ്കിൽ (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_നിവേശം_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    എങ്കിൽ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_നിവേശം_buf();
	got_int = TRUE;
    }

    add_to_നിവേശം_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    സ്ഥിതം ശൂന്യം
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    char_u	string[40];
    പൂർണ്ണസംഖ്യ		len = 0;

    dead_താക്കോൽ = 0;

    len = char_to_string(ch, string, 40, FALSE);
    എങ്കിൽ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_നിവേശം_buf();
	got_int = TRUE;
    }

    add_to_നിവേശം_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    സ്ഥിതം ശൂന്യം
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    സ്ഥിതം ശൂന്യം
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		modifiers;
    പൂർണ്ണസംഖ്യ		ch = cch;   // special keys are negative

    dead_താക്കോൽ = DEAD_താക്കോൽ_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_താക്കോൽ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ശൂന്യ);
    എങ്കിൽ (ch == CSI)
	ch = K_CSI;

    len = 0;
    എങ്കിൽ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    എങ്കിൽ (IS_SPECIAL((പൂർണ്ണസംഖ്യ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((പൂർണ്ണസംഖ്യ)ch);
	string[len++] = K_THIRD((പൂർണ്ണസംഖ്യ)ch);
    }
    അല്ലെങ്കിൽ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_നിവേശം_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    സ്ഥിതം ശൂന്യം
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    പൂർണ്ണസംഖ്യ cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		modifiers;
    പൂർണ്ണസംഖ്യ		ch = cch;   // special keys are negative

    dead_താക്കോൽ = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_താക്കോൽ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ശൂന്യ);
    എങ്കിൽ (ch == CSI)
	ch = K_CSI;

    len = 0;
    എങ്കിൽ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    എങ്കിൽ (IS_SPECIAL((പൂർണ്ണസംഖ്യ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((പൂർണ്ണസംഖ്യ)ch);
	string[len++] = K_THIRD((പൂർണ്ണസംഖ്യ)ch);
    }
    അല്ലെങ്കിൽ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_നിവേശം_buf(string, len);
}

    സ്ഥിതം ശൂന്യം
_OnMouseEvent(
    പൂർണ്ണസംഖ്യ button,
    പൂർണ്ണസംഖ്യ x,
    പൂർണ്ണസംഖ്യ y,
    പൂർണ്ണസംഖ്യ repeated_click,
    UINT താക്കോൽ_Flags)
{
    പൂർണ്ണസംഖ്യ vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    എങ്കിൽ (താക്കോൽ_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    എങ്കിൽ (താക്കോൽ_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    സ്ഥിതം ശൂന്യം
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    പൂർണ്ണസംഖ്യ x,
    പൂർണ്ണസംഖ്യ y,
    UINT താക്കോൽ_Flags)
{
    സ്ഥിതം LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    പൂർണ്ണസംഖ്യ	    button = -1;
    പൂർണ്ണസംഖ്യ	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (ശൂന്യം)SetFocus(s_hwnd);

    എങ്കിൽ (s_u_സന്ദേശം == WM_LBUTTONDOWN || s_u_സന്ദേശം == WM_LBUTTONDBLCLK)
	button = MOUSE_ഇടത്;
    അല്ലെങ്കിൽ എങ്കിൽ (s_u_സന്ദേശം == WM_MBUTTONDOWN || s_u_സന്ദേശം == WM_MBUTTONDBLCLK)
	button = MOUSE_നടുവ്;
    അല്ലെങ്കിൽ എങ്കിൽ (s_u_സന്ദേശം == WM_RBUTTONDOWN || s_u_സന്ദേശം == WM_RBUTTONDBLCLK)
	button = MOUSE_വലത്;
    അല്ലെങ്കിൽ എങ്കിൽ (s_u_സന്ദേശം == WM_XBUTTONDOWN || s_u_സന്ദേശം == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (s_u_സന്ദേശം == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	എങ്കിൽ (s_button_pending == MOUSE_ഇടത്)
	    button = MOUSE_വലത്;
	അല്ലെങ്കിൽ
	    button = MOUSE_ഇടത്;
    }

    എങ്കിൽ (button < 0)
	തിരികെ;

    repeated_click = ((പൂർണ്ണസംഖ്യ)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    എങ്കിൽ (repeated_click
	    && ((button == MOUSE_ഇടത് && s_button_pending == MOUSE_വലത്)
		|| (button == MOUSE_വലത്
		    && s_button_pending == MOUSE_ഇടത്)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_നടുവ്;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, താക്കോൽ_Flags);
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_വലത്)))
    {
	എങ്കിൽ (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, താക്കോൽ_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, താക്കോൽ_Flags);
    }
    അല്ലെങ്കിൽ
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
	s_kFlags_pending = താക്കോൽ_Flags;
    }

    s_prevTime = currentTime;
}

    സ്ഥിതം ശൂന്യം
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    പൂർണ്ണസംഖ്യ x,
    പൂർണ്ണസംഖ്യ y,
    UINT താക്കോൽ_Flags)
{
    പൂർണ്ണസംഖ്യ button;

    s_getting_focus = FALSE;
    എങ്കിൽ (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    എങ്കിൽ (s_u_സന്ദേശം == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	എങ്കിൽ (!(താക്കോൽ_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    തിരികെ;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    അല്ലെങ്കിൽ
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, താക്കോൽ_Flags);
}

    സ്ഥിതം ശൂന്യം
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    പൂർണ്ണസംഖ്യ cx UNUSED,
    പൂർണ്ണസംഖ്യ cy UNUSED)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    സ്ഥിതം vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    പൂർണ്ണസംഖ്യ		id)
{
    vimmenu_T	*pChildMenu;

    ആകുമ്പോൾ (pMenu)
    {
	എങ്കിൽ (pMenu->id == (UINT)id)
	    നിർത്തുക;
	എങ്കിൽ (pMenu->children != ശൂന്യ)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    എങ്കിൽ (pChildMenu)
	    {
		pMenu = pChildMenu;
		നിർത്തുക;
	    }
	}
	pMenu = pMenu->next;
    }
    തിരികെ pMenu;
}

    സ്ഥിതം ശൂന്യം
_OnMenu(
    HWND	hwnd UNUSED,
    പൂർണ്ണസംഖ്യ		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(വേര്_menu, id);
    എങ്കിൽ (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    സ്ഥിതം ശൂന്യം
_OnFindRepl(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ	    flags = 0;
    പൂർണ്ണസംഖ്യ	    down;

    എങ്കിൽ (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(ശൂന്യം)SetFocus(s_hwnd);

    എങ്കിൽ (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ശൂന്യം)SetFocus(s_hwnd);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ശൂന്യം)SetFocus(s_hwnd);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    എങ്കിൽ (flags == 0)
	തിരികെ;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    എങ്കിൽ (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    എങ്കിൽ (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, ശൂന്യ);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, ശൂന്യ);
    എങ്കിൽ (p != ശൂന്യ && q != ശൂന്യ)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    സ്ഥിതം ശൂന്യം
HandleMouseHide(UINT u_സന്ദേശം, LPARAM lParam)
{
    സ്ഥിതം LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    എങ്കിൽ (u_സന്ദേശം == WM_MOUSEMOVE || u_സന്ദേശം == WM_NCMOUSEMOVE)
    {
	എങ്കിൽ (lParam == last_lParam)
	    തിരികെ;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    തിരഞ്ഞെടുപ്പ് (u_സന്ദേശം)
    {
    അവസ്ഥ WM_KEYUP:
    അവസ്ഥ WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	എങ്കിൽ (p_mh)
	    gui_mch_mousehide(TRUE);
	നിർത്തുക;

    അവസ്ഥ WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    അവസ്ഥ WM_SYSCHAR:
    അവസ്ഥ WM_MOUSEMOVE:	 // show the pointer on any mouse action
    അവസ്ഥ WM_LBUTTONDOWN:
    അവസ്ഥ WM_LBUTTONUP:
    അവസ്ഥ WM_MBUTTONDOWN:
    അവസ്ഥ WM_MBUTTONUP:
    അവസ്ഥ WM_RBUTTONDOWN:
    അവസ്ഥ WM_RBUTTONUP:
    അവസ്ഥ WM_XBUTTONDOWN:
    അവസ്ഥ WM_XBUTTONUP:
    അവസ്ഥ WM_NCMOUSEMOVE:
    അവസ്ഥ WM_NCLBUTTONDOWN:
    അവസ്ഥ WM_NCLBUTTONUP:
    അവസ്ഥ WM_NCMBUTTONDOWN:
    അവസ്ഥ WM_NCMBUTTONUP:
    അവസ്ഥ WM_NCRBUTTONDOWN:
    അവസ്ഥ WM_NCRBUTTONUP:
    അവസ്ഥ WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	നിർത്തുക;
    }
}

    സ്ഥിതം LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_സന്ദേശം,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_സന്ദേശം, lParam);

    s_u_സന്ദേശം = u_സന്ദേശം;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_സന്ദേശം);
#endif

    തിരഞ്ഞെടുപ്പ് (u_സന്ദേശം)
    {
	HANDLE_സന്ദേശം(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_സന്ദേശം(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_സന്ദേശം(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_സന്ദേശം(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_സന്ദേശം(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_സന്ദേശം(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_സന്ദേശം(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_സന്ദേശം(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	അവസ്ഥ WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    തിരികെ TRUE;
#endif
	സ്ഥിരസ്ഥിതി:
	    തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
    }
}

    സ്ഥിതം ശൂന്യം
dyn_dwm_load(ശൂന്യം)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    എങ്കിൽ (hLibDwm == ശൂന്യ)
	തിരികെ;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    എങ്കിൽ (pDwmSetWindowAttribute == ശൂന്യ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ശൂന്യ;
	തിരികെ;
    }
}

ബാഹ്യ DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    ശൂന്യം
gui_mch_set_titlebar_colors(ശൂന്യം)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    എങ്കിൽ (pDwmSetWindowAttribute == ശൂന്യ || win_version < MAKE_VER(10, 0, 22000))
	തിരികെ;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    എങ്കിൽ (vim_strchr(p_go, GO_TITLEBAR) != ശൂന്യ)
    {
	എങ്കിൽ (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	അല്ലെങ്കിൽ
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	എങ്കിൽ (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	എങ്കിൽ (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, വലുപ്പം(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, വലുപ്പം(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    ശൂന്യം
gui_mch_new_colors(ശൂന്യം)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ശൂന്യ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    എങ്കിൽ (s_textarea_bg_only && s_textArea != ശൂന്യ)
	InvalidateRect(s_textArea, ശൂന്യ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    ശൂന്യം
gui_mch_def_colors(ശൂന്യം)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    പൂർണ്ണസംഖ്യ
gui_mch_open(ശൂന്യം)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    എങ്കിൽ (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	എങ്കിൽ (s_textArea != ശൂന്യ)
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

    തിരികെ OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.ഇടത്;
    *y = rect.മുകൾ;
    തിരികെ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ശൂന്യം
gui_mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    SetWindowPos(s_hwnd, ശൂന്യ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    ശൂന്യം
gui_mch_set_text_area_pos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ oldx = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ oldy = 0;

    SetWindowPos(s_textArea, ശൂന്യ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#എങ്കിൽ defined(FEAT_GUI_TABLINE)
    എങ്കിൽ (showing_tabline)
    {
	പൂർണ്ണസംഖ്യ	മുകൾ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
	    മുകൾ = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, മുകൾ, rect.വലത്, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    എങ്കിൽ (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ശൂന്യ, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    ശൂന്യം
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    പൂർണ്ണസംഖ്യ		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    ശൂന്യം
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    പൂർണ്ണസംഖ്യ		x,
    പൂർണ്ണസംഖ്യ		y,
    പൂർണ്ണസംഖ്യ		w,
    പൂർണ്ണസംഖ്യ		h)
{
    SetWindowPos(sb->id, ശൂന്യ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_xpadding(ശൂന്യം)
{
    RECT    rcTxt, rcWnd;
    പൂർണ്ണസംഖ്യ	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.വലത് - rcTxt.വലത് - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    തിരികെ (xpad < 0) ? 0 : xpad;
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_ypadding(ശൂന്യം)
{
    RECT    rcTxt, rcWnd;
    പൂർണ്ണസംഖ്യ	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    തിരികെ (ypad < 0) ? 0 : ypad;
}

    ശൂന്യം
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    പൂർണ്ണസംഖ്യ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ശൂന്യ,
	g_hinst, ശൂന്യ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    സ്ഥിതം scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    എങ്കിൽ (gui.bottom_sbar.id == hwnd)
	തിരികെ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	എങ്കിൽ (wp->w_scrollbars[SBAR_ഇടത്].id == hwnd)
	    തിരികെ &wp->w_scrollbars[SBAR_ഇടത്];
	എങ്കിൽ (wp->w_scrollbars[SBAR_വലത്].id == hwnd)
	    തിരികെ &wp->w_scrollbars[SBAR_വലത്];
    }
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
update_scrollbar_size(ശൂന്യം)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
GetFontSize(GuiFont font, പൂർണ്ണസംഖ്യ *char_width, പൂർണ്ണസംഖ്യ *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    എങ്കിൽ (char_width)
	*char_width = size.cx + tm.tmOverhang;
    എങ്കിൽ (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    സ്ഥിതം ശൂന്യം
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charheight(ശൂന്യം)
{
    UpdateFontSize(gui.norm_font);
    തിരികെ OK;
}

    സ്ഥിതം GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ശൂന്യ;

    // Load the font
    font = CreateFontIndirectW(lf);

    എങ്കിൽ (font == ശൂന്യ)
	തിരികെ NOFONT;

    തിരികെ (GuiFont)font;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
pixels_to_points(പൂർണ്ണസംഖ്യ pixels, പൂർണ്ണസംഖ്യ vertical)
{
    പൂർണ്ണസംഖ്യ		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    തിരികെ points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    എങ്കിൽ (get_logfont(&lf, name, ശൂന്യ, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    എങ്കിൽ (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    തിരികെ font;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strsave(name);
}
#endif

    ശൂന്യം
gui_mch_free_font(GuiFont font)
{
    എങ്കിൽ (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    തരനിർവചനം ഘടന SysColorTable
    {
	അക്ഷരം	    *name;
	പൂർണ്ണസംഖ്യ	    color;
    } SysColorTable;

    സ്ഥിതം SysColorTable sys_table[] =
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
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(sys_table); i++)
	എങ്കിൽ (STRICMP(name, sys_table[i].name) == 0)
	    തിരികെ GetSysColor(sys_table[i].color);

    തിരികെ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    തിരികെ gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_haskey(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].vim_code1 != NUL; i++)
	എങ്കിൽ (name[0] == special_താക്കോലുകൾ[i].vim_code0
				       && name[1] == special_താക്കോലുകൾ[i].vim_code1)
	    തിരികെ OK;
    തിരികെ FAIL;
}

    ശൂന്യം
gui_mch_beep(ശൂന്യം)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ശൂന്യം
gui_mch_invert_rectangle(
    പൂർണ്ണസംഖ്യ	    r,
    പൂർണ്ണസംഖ്യ	    c,
    പൂർണ്ണസംഖ്യ	    nr,
    പൂർണ്ണസംഖ്യ	    nc)
{
    RECT    rc;

#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ഇടത് = FILL_X(c);
    rc.മുകൾ = FILL_Y(r);
    rc.വലത് = rc.ഇടത് + nc * gui.char_width;
    rc.bottom = rc.മുകൾ + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    ശൂന്യം
gui_mch_iconify(ശൂന്യം)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    ശൂന്യം
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ഇടത് = FILL_X(gui.col);
    rc.മുകൾ = FILL_Y(gui.row);
    rc.വലത് = rc.ഇടത് + gui.char_width;
    എങ്കിൽ (mb_lefthalve(gui.row, gui.col))
	rc.വലത് += gui.char_width;
    rc.bottom = rc.മുകൾ + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ശൂന്യം
gui_mch_draw_part_cursor(
    പൂർണ്ണസംഖ്യ		w,
    പൂർണ്ണസംഖ്യ		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ഇടത് =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_വലത് ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.മുകൾ = FILL_Y(gui.row) + gui.char_height - h;
    rc.വലത് = rc.ഇടത് + w;
    rc.bottom = rc.മുകൾ + h;

    fill_rect(&rc, ശൂന്യ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    സ്ഥിതം ശൂന്യം
output_Dead_താക്കോൽ_re_Post_Ex(സന്ദേശം_2 original_സന്ദേശം, പൂർണ്ണസംഖ്യ dead_key2set)
{
    സ്ഥിതം സന്ദേശം_2 deadCharExpel;

    എങ്കിൽ (dead_താക്കോൽ == DEAD_താക്കോൽ_OFF)
	തിരികെ;

    dead_താക്കോൽ = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_സന്ദേശം.message;
    deadCharExpel.hwnd    = original_സന്ദേശം.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_സന്ദേശം.hwnd, original_സന്ദേശം.message, original_സന്ദേശം.wParam,
							  original_സന്ദേശം.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    സ്ഥിതം ശൂന്യം
output_Dead_താക്കോൽ_re_Post(സന്ദേശം_2 original_സന്ദേശം)
{
    output_Dead_താക്കോൽ_re_Post_Ex(original_സന്ദേശം, DEAD_താക്കോൽ_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
സ്ഥിതം ശൂന്യം process_message_usual_താക്കോൽ(UINT vk, സ്ഥിരം സന്ദേശം_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_താക്കോൽ(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    സ്ഥിതം ശൂന്യം
process_message_usual_താക്കോൽ_experimental(UINT vk, സ്ഥിരം സന്ദേശം_2 *pmsg)
{
    WCHAR	ch[8];
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    എങ്കിൽ (Get_താക്കോൽ_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    എങ്കിൽ (Get_താക്കോൽ_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    എങ്കിൽ ((Get_താക്കോൽ_State(VK_RMENU) & 0x8000)
				 && (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_താക്കോൽ(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    എങ്കിൽ (len < 0)
	dead_താക്കോൽ = DEAD_താക്കോൽ_SET_DEFAULT;

    എങ്കിൽ (len <= 0)
    {
	പൂർണ്ണസംഖ്യ wm_char = NUL;

	എങ്കിൽ (dead_താക്കോൽ == DEAD_താക്കോൽ_SET_DEFAULT
		&& (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000))
	{
	    എങ്കിൽ (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    എങ്കിൽ (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	എങ്കിൽ (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_താക്കോൽ = DEAD_താക്കോൽ_TRANSIENT_IN_ON_CHAR;
	}
	തിരികെ;
    }

    // Post the message as TranslateMessage would do.
    എങ്കിൽ (pmsg->message == WM_KEYDOWN)
    {
	വേണ്ടി (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    സ്ഥിതം ശൂന്യം
process_message_usual_താക്കോൽ_classic(UINT vk, സ്ഥിരം സന്ദേശം_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    എങ്കിൽ (vk != 0xff
	    && (Get_താക്കോൽ_State(VK_CONTROL) & 0x8000)
	    && !(Get_താക്കോൽ_State(VK_SHIFT) & 0x8000)
	    && !(Get_താക്കോൽ_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	എങ്കിൽ (vk == '6' || Map_Virtual_താക്കോൽ(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_നിവേശം_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	അല്ലെങ്കിൽ എങ്കിൽ (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_നിവേശം_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	അല്ലെങ്കിൽ എങ്കിൽ (vk == '2' || Map_Virtual_താക്കോൽ(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_നിവേശം_buf(string, 1);
	}
	അല്ലെങ്കിൽ
	    TranslateMessage(pmsg);
    }
    അല്ലെങ്കിൽ
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    സ്ഥിതം ശൂന്യം
process_message(ശൂന്യം)
{
    സന്ദേശം_2		സന്ദേശം;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		modifiers = 0;
    പൂർണ്ണസംഖ്യ		താക്കോൽ;
#ifdef FEAT_MENU
    സ്ഥിതം char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    സ്ഥിതം പൂർണ്ണസംഖ്യ  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    എങ്കിൽ (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&സന്ദേശം, ശൂന്യ, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    എങ്കിൽ (സന്ദേശം.message == WM_OLE)
    {
	char_u *str = (char_u *)സന്ദേശം.lParam;
	എങ്കിൽ (str == ശൂന്യ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&സന്ദേശം);
	}
	അല്ലെങ്കിൽ
	{
	    add_to_നിവേശം_buf(str, (പൂർണ്ണസംഖ്യ)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	തിരികെ;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    എങ്കിൽ (s_findrep_hwnd != ശൂന്യ && IsDialogMessageW(s_findrep_hwnd, &സന്ദേശം))
    {
	HandleMouseHide(സന്ദേശം.message, സന്ദേശം.lParam);
	തിരികെ;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    എങ്കിൽ (സന്ദേശം.message == WM_KEYDOWN || സന്ദേശം.message == WM_SYSKEYDOWN)
    {
	vk = (പൂർണ്ണസംഖ്യ) സന്ദേശം.wParam;

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
	എങ്കിൽ (keycode_trans_strategy_used->is_experimental()
		&& dead_താക്കോൽ == DEAD_താക്കോൽ_SKIP_ON_CHAR)
	{
	    dead_താക്കോൽ = DEAD_താക്കോൽ_OFF;
	}

	എങ്കിൽ (dead_താക്കോൽ != DEAD_താക്കോൽ_OFF)
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
	    എങ്കിൽ (keycode_trans_strategy_used->is_experimental() &&
		    dead_താക്കോൽ == DEAD_താക്കോൽ_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_താക്കോൽ_re_Post_Ex(സന്ദേശം,
				       /*dead_key2set=*/DEAD_താക്കോൽ_SKIP_ON_CHAR);
		തിരികെ;
	    }

	    എങ്കിൽ (dead_താക്കോൽ != DEAD_താക്കോൽ_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		തിരികെ;
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
	    എങ്കിൽ ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_താക്കോൽ = DEAD_താക്കോൽ_OFF;
		TranslateMessage(&സന്ദേശം);
		തിരികെ;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    അല്ലെങ്കിൽ എങ്കിൽ ((get_real_state()
			    & (MODE_തിരുകുക | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_താക്കോൽ_re_Post(സന്ദേശം);
		തിരികെ;
	    }
	}

	// Check for CTRL-BREAK
	എങ്കിൽ (vk == VK_CANCEL)
	{
	    trash_നിവേശം_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_നിവേശം_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	എങ്കിൽ (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&സന്ദേശം);
	    തിരികെ;
	}

	വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == vk
		    && (vk != VK_SPACE || !(Get_താക്കോൽ_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		എങ്കിൽ (dead_താക്കോൽ == DEAD_താക്കോൽ_SET_DEFAULT
			&& (special_താക്കോലുകൾ[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_താക്കോൽ_re_Post(സന്ദേശം);
		    തിരികെ;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		എങ്കിൽ (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ശൂന്യ, ശൂന്യ) == ശൂന്യ)
		    നിർത്തുക;
#endif
		modifiers = get_active_modifiers_via_ptr();

		എങ്കിൽ (special_താക്കോലുകൾ[i].vim_code1 == NUL)
		    താക്കോൽ = special_താക്കോലുകൾ[i].vim_code0;
		അല്ലെങ്കിൽ
		    താക്കോൽ = TO_SPECIAL(special_താക്കോലുകൾ[i].vim_code0,
						   special_താക്കോലുകൾ[i].vim_code1);
		താക്കോൽ = simplify_താക്കോൽ(താക്കോൽ, &modifiers);
		എങ്കിൽ (താക്കോൽ == CSI)
		    താക്കോൽ = K_CSI;

		എങ്കിൽ (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_നിവേശം_buf(string, 3);
		}

		എങ്കിൽ (IS_SPECIAL(താക്കോൽ))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(താക്കോൽ);
		    string[2] = K_THIRD(താക്കോൽ);
		    add_to_നിവേശം_buf(string, 3);
		}
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(താക്കോൽ, string, 40, FALSE);
		    add_to_നിവേശം_buf(string, len);
		}
		നിർത്തുക;
	    }
	}

	// Not a special key.
	എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == 0)
	{
	    process_message_usual_താക്കോൽ(vk, &സന്ദേശം);
	}
    }
#ifdef FEAT_MBYTE_IME
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം.message == WM_IME_NOTIFY)
	_OnImeNotify(സന്ദേശം.hwnd, (DWORD)സന്ദേശം.wParam, (DWORD)സന്ദേശം.lParam);
    അല്ലെങ്കിൽ എങ്കിൽ (സന്ദേശം.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&സന്ദേശം);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    എങ്കിൽ (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ശൂന്യ, ശൂന്യ) == ശൂന്യ)
#endif
	DispatchMessageW(&സന്ദേശം);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ശൂന്യം
gui_mch_update(ശൂന്യം)
{
    സന്ദേശം_2	    സന്ദേശം;

    എങ്കിൽ (!s_busy_processing)
	ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, ശൂന്യ, 0, 0, PM_NOREMOVE)
						  && !vim_is_നിവേശം_buf_full())
	    process_message();
}

    സ്ഥിതം ശൂന്യം
remove_any_timer(ശൂന്യം)
{
    സന്ദേശം_2		സന്ദേശം;

    എങ്കിൽ (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(ശൂന്യ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	ആകുമ്പോൾ (PeekMessageW(&സന്ദേശം, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
സ്ഥിതം garray_T	server_pending_ga = {0, 0, 0, 0, ശൂന്യ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    ശൂന്യം
server_add_നിവേശം(char_u *str)
{
    എങ്കിൽ (server_pending_ga.ga_data == ശൂന്യ)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    സ്ഥിതം ശൂന്യം
server_flush_നിവേശം(ശൂന്യം)
{
    char_u  *str;

    എങ്കിൽ (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	തിരികെ;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    എങ്കിൽ (str != ശൂന്യ)
    {
	server_to_നിവേശം_buf(str);
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
    പൂർണ്ണസംഖ്യ
gui_mch_wait_for_chars(long wtime)
{
    പൂർണ്ണസംഖ്യ		focus;

    s_timed_out = FALSE;

    എങ്കിൽ (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	എങ്കിൽ (s_busy_processing)
	    തിരികെ FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(ശൂന്യ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    ആകുമ്പോൾ (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	എങ്കിൽ (gui.in_focus != focus)
	{
	    എങ്കിൽ (gui.in_focus)
		gui_mch_start_blink();
	    അല്ലെങ്കിൽ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	എങ്കിൽ (s_need_activate)
	{
	    (ശൂന്യം)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	വേണ്ടി (;;)
	{
	    സന്ദേശം_2 സന്ദേശം;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_നിവേശം();
# endif
# ifdef FEAT_TIMERS
	    എങ്കിൽ (did_add_timer)
		നിർത്തുക;
# endif
	    എങ്കിൽ (PeekMessageW(&സന്ദേശം, ശൂന്യ, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		നിർത്തുക;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (നിവേശം_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || സന്ദേശം_Wait_For_Multiple_Objects(0, ശൂന്യ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		നിർത്തുക;
	}
#അല്ലെങ്കിൽ
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	എങ്കിൽ (നിവേശം_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    എങ്കിൽ (!s_getting_focus)
		s_button_pending = -1;

	    തിരികെ OK;
	}

#ifdef FEAT_TIMERS
	എങ്കിൽ (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    നിർത്തുക;
	}
#endif
    }
    allow_scrollbar = FALSE;
    തിരികെ FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ശൂന്യം
gui_mch_clear_block(
    പൂർണ്ണസംഖ്യ		row1,
    പൂർണ്ണസംഖ്യ		col1,
    പൂർണ്ണസംഖ്യ		row2,
    പൂർണ്ണസംഖ്യ		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ഇടത് = FILL_X(col1);
    rc.മുകൾ = FILL_Y(row1);
    rc.വലത് = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    ശൂന്യം
gui_mch_clear_all(ശൂന്യം)
{
    RECT    rc;

    rc.ഇടത് = 0;
    rc.മുകൾ = 0;
    rc.വലത് = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    ശൂന്യം
gui_mch_enable_menu(പൂർണ്ണസംഖ്യ flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : ശൂന്യ);
#endif
}

    ശൂന്യം
gui_mch_set_menu_pos(
    പൂർണ്ണസംഖ്യ	    x UNUSED,
    പൂർണ്ണസംഖ്യ	    y UNUSED,
    പൂർണ്ണസംഖ്യ	    w UNUSED,
    പൂർണ്ണസംഖ്യ	    h UNUSED)
{
    // It will be in the right place anyway
}

#എങ്കിൽ defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    ശൂന്യം
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ		hidden)
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
    ശൂന്യം
gui_mch_draw_menubar(ശൂന്യം)
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
    തിരികെ (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#എങ്കിൽ defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    സ്ഥിതം WORD
PixelToDialogX(പൂർണ്ണസംഖ്യ അക്കം_Pixels)
{
    തിരികെ (WORD)((അക്കം_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    സ്ഥിതം WORD
PixelToDialogY(പൂർണ്ണസംഖ്യ അക്കം_Pixels)
{
    തിരികെ (WORD)((അക്കം_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
GetTextWidth(HDC hdc, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    തിരികെ size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
GetTextWidthEnc(HDC hdc, char_u *str, പൂർണ്ണസംഖ്യ len)
{
    SIZE	size;
    WCHAR	*wstr;
    പൂർണ്ണസംഖ്യ		n;
    പൂർണ്ണസംഖ്യ		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    എങ്കിൽ (wstr == ശൂന്യ)
	തിരികെ 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    എങ്കിൽ (n)
	തിരികെ size.cx;
    തിരികെ 0;
}

സ്ഥിതം ശൂന്യം get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    സ്ഥിതം BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    പൂർണ്ണസംഖ്യ		    wChild, hChild, wParent, hParent;
    പൂർണ്ണസംഖ്യ		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.വലത് - rChild.ഇടത്;
    hChild = rChild.bottom - rChild.മുകൾ;

    // If Vim is minimized put the window in the middle of the screen.
    എങ്കിൽ (hwndParent == ശൂന്യ || IsMinimized(hwndParent))
	get_work_area(&rParent);
    അല്ലെങ്കിൽ
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.വലത് - rParent.ഇടത്;
    hParent = rParent.bottom - rParent.മുകൾ;

    moninfo.cbSize = വലുപ്പം(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    എങ്കിൽ (mon != ശൂന്യ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    അല്ലെങ്കിൽ
    {
	hdc = GetDC(hwndChild);
	rScreen.ഇടത് = 0;
	rScreen.മുകൾ = 0;
	rScreen.വലത് = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.ഇടത് + ((wParent - wChild) / 2);
    എങ്കിൽ (xNew < rScreen.ഇടത്)
	xNew = rScreen.ഇടത്;
    അല്ലെങ്കിൽ എങ്കിൽ ((xNew + wChild) > rScreen.വലത്)
	xNew = rScreen.വലത് - wChild;

    yNew = rParent.മുകൾ + ((hParent - hChild) / 2);
    എങ്കിൽ (yNew < rScreen.മുകൾ)
	yNew = rScreen.മുകൾ;
    അല്ലെങ്കിൽ എങ്കിൽ ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    തിരികെ SetWindowPos(hwndChild, ശൂന്യ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#എങ്കിൽ defined(FEAT_TOOLBAR)
    ശൂന്യം
gui_mch_show_toolbar(പൂർണ്ണസംഖ്യ showit)
{
    എങ്കിൽ (s_toolbarhwnd == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    അല്ലെങ്കിൽ
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_എണ്ണം 31

#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
    സ്ഥിതം ശൂന്യം
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, ശൂന്യ);
    എങ്കിൽ (wn == ശൂന്യ)
	തിരികെ;

    infow.cbSize = വലുപ്പം(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    തിരുകുക_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    സ്ഥിതം ശൂന്യം
show_tabline_popup_menu(ശൂന്യം)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    എങ്കിൽ (hold_gui_events || cmdwin_type != 0)
	തിരികെ;

    tab_pmenu = CreatePopupMenu();
    എങ്കിൽ (tab_pmenu == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (first_tabpage->tp_next != ശൂന്യ)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ശൂന്യ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    എങ്കിൽ (rval > 0)
    {
	TCHITTESTINFO htinfo;
	പൂർണ്ണസംഖ്യ idx;

	എങ്കിൽ (ScreenToClient(s_tabhwnd, &pt) == 0)
	    തിരികെ;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	എങ്കിൽ (idx == -1)
	    idx = 0;
	അല്ലെങ്കിൽ
	    idx += 1;

	send_tabline_menu_event(idx, (പൂർണ്ണസംഖ്യ)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    ശൂന്യം
gui_mch_show_tabline(പൂർണ്ണസംഖ്യ showit)
{
    എങ്കിൽ (s_tabhwnd == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (!showit != !showing_tabline)
    {
	എങ്കിൽ (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	അല്ലെങ്കിൽ
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_showing_tabline(ശൂന്യം)
{
    തിരികെ s_tabhwnd != ശൂന്യ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ശൂന്യം
gui_mch_update_tabline(ശൂന്യം)
{
    tabpage_T	*tp;
    TCITEM	tie;
    പൂർണ്ണസംഖ്യ		nr = 0;
    പൂർണ്ണസംഖ്യ		curtabidx = 0;
    പൂർണ്ണസംഖ്യ		tabadded = 0;
    WCHAR	*wstr = ശൂന്യ;

    എങ്കിൽ (s_tabhwnd == ശൂന്യ)
	തിരികെ;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = tp->tp_next, ++nr)
    {
	എങ്കിൽ (tp == curtab)
	    curtabidx = nr;

	എങ്കിൽ (nr >= Tab_Ctrl_Get_Item_എണ്ണം(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_തിരുകുക_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, ശൂന്യ);
	എങ്കിൽ (wstr != ശൂന്യ)
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
    ആകുമ്പോൾ (nr < Tab_Ctrl_Get_Item_എണ്ണം(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    എങ്കിൽ (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, ശൂന്യ, ശൂന്യ,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    എങ്കിൽ (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ശൂന്യം
gui_mch_set_curtab(പൂർണ്ണസംഖ്യ nr)
{
    എങ്കിൽ (s_tabhwnd == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    ശൂന്യം
gui_mch_set_dark_theme(പൂർണ്ണസംഖ്യ dark)
{
    എങ്കിൽ (pDwmSetWindowAttribute != ശൂന്യ && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		വലുപ്പം(dark));

    എങ്കിൽ (pSetPreferredAppMode != ശൂന്യ && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    എങ്കിൽ (pFlushMenuThemes != ശൂന്യ && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    സ്ഥിതം ശൂന്യം
dyn_uxtheme_load(ശൂന്യം)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    എങ്കിൽ (hUxThemeLib == ശൂന്യ)
	തിരികെ;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (ശൂന്യം (WINAPI *)(ശൂന്യം))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    എങ്കിൽ (pSetPreferredAppMode == ശൂന്യ || pFlushMenuThemes == ശൂന്യ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ശൂന്യ;
	തിരികെ;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    ശൂന്യം
gui_mch_set_fullscreen(പൂർണ്ണസംഖ്യ flag)
{
    സ്ഥിതം RECT normal_rect;
    സ്ഥിതം LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    എങ്കിൽ (!full_screen) // Windows not set yet.
	തിരികെ;

    എങ്കിൽ (flag)
    {
	എങ്കിൽ (fullscreen_on)
	    തിരികെ;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = വലുപ്പം(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	എങ്കിൽ (mon == ശൂന്യ || !GetMonitorInfo(mon, &moninfo))
	    തിരികെ;

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
	SetWindowPos(s_hwnd, ശൂന്യ,
		moninfo.rcMonitor.ഇടത്,
		moninfo.rcMonitor.മുകൾ,
		moninfo.rcMonitor.വലത് - moninfo.rcMonitor.ഇടത്,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.മുകൾ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!fullscreen_on)
	    തിരികെ;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, ശൂന്യ,
		normal_rect.ഇടത്,
		normal_rect.മുകൾ,
		normal_rect.വലത് - normal_rect.ഇടത്,
		normal_rect.bottom - normal_rect.മുകൾ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    ശൂന്യം
ex_simalt(exarg_T *eap)
{
    char_u	*താക്കോലുകൾ = eap->arg;
    പൂർണ്ണസംഖ്യ		fill_typebuf = FALSE;
    char_u	താക്കോൽ_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    ആകുമ്പോൾ (*താക്കോലുകൾ)
    {
	എങ്കിൽ (*താക്കോലുകൾ == '~')
	    *താക്കോലുകൾ = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*താക്കോലുകൾ, (LPARAM)0);
	താക്കോലുകൾ++;
	fill_typebuf = TRUE;
    }
    എങ്കിൽ (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	താക്കോൽ_name[0] = K_SPECIAL;
	താക്കോൽ_name[1] = KS_EXTRA;
	താക്കോൽ_name[2] = KE_NOP;
	താക്കോൽ_name[3] = NUL;
#എങ്കിൽ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(ശൂന്യം)ins_typebuf(താക്കോൽ_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    സ്ഥിതം ശൂന്യം
initialise_findrep(char_u *initial_string)
{
    പൂർണ്ണസംഖ്യ		wword = FALSE;
    പൂർണ്ണസംഖ്യ		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    എങ്കിൽ (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    എങ്കിൽ (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    എങ്കിൽ (entry_text != ശൂന്യ && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, ശൂന്യ);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    സ്ഥിതം ശൂന്യം
set_window_title(HWND hwnd, അക്ഷരം *title)
{
    എങ്കിൽ (title != ശൂന്യ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, ശൂന്യ);
	എങ്കിൽ (wbuf != ശൂന്യ)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    അല്ലെങ്കിൽ
	(ശൂന്യം)SetWindowTextW(hwnd, ശൂന്യ);
}

    ശൂന്യം
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    എങ്കിൽ (s_findrep_സന്ദേശം != 0)
    {
	എങ്കിൽ (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	എങ്കിൽ (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(ശൂന്യം)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    ശൂന്യം
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    എങ്കിൽ (s_findrep_സന്ദേശം != 0)
    {
	എങ്കിൽ (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	എങ്കിൽ (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(ശൂന്യം)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    ശൂന്യം
gui_mch_mousehide(പൂർണ്ണസംഖ്യ hide)
{
    എങ്കിൽ (hide == gui.pointer_hidden)
	തിരികെ;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    സ്ഥിതം ശൂന്യം
gui_mch_show_popupmenu_at(vimmenu_T *menu, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (ശൂന്യം)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(പൂർണ്ണസംഖ്യ)0,	    //reserved param
	s_hwnd,
	ശൂന്യ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    സ്ഥിതം ശൂന്യം
_OnEndSession(ശൂന്യം)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    സ്ഥിതം ശൂന്യം
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    സ്ഥിതം ശൂന്യം
_OnDestroy(HWND hwnd)
{
    എങ്കിൽ (!destroying)
	_OnClose(hwnd);
}

    സ്ഥിതം ശൂന്യം
_OnPaint(
    HWND hwnd)
{
    എങ്കിൽ (IsMinimized(hwnd))
	തിരികെ;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (ശൂന്യം)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    എങ്കിൽ (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ഇടത് = rect.ഇടത്;
	ps.rcPaint.വലത് = rect.വലത്;
    }

    എങ്കിൽ (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	എങ്കിൽ (!screen_cleared || Screen_വരികൾ == ശൂന്യ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	അല്ലെങ്കിൽ
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ഇടത്, ps.rcPaint.മുകൾ,
		ps.rcPaint.വലത് - ps.rcPaint.ഇടത് + 1,
		ps.rcPaint.bottom - ps.rcPaint.മുകൾ + 1);
    }

    EndPaint(hwnd, &ps);
}

    സ്ഥിതം ശൂന്യം
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    പൂർണ്ണസംഖ്യ cx,
    പൂർണ്ണസംഖ്യ cy)
{
    എങ്കിൽ (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    സ്ഥിതം ശൂന്യം
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (ശൂന്യം)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    സ്ഥിതം ശൂന്യം
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    എങ്കിൽ (destroying)
	തിരികെ;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (ശൂന്യം)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    സ്ഥിതം LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    തിരികെ DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    ശൂന്യം
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    ശൂന്യം
gui_mch_getmouse(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    RECT rct;
    POINT mp;

    (ശൂന്യം)GetWindowRect(s_textArea, &rct);
    (ശൂന്യം)GetCursorPos(&mp);
    *x = (പൂർണ്ണസംഖ്യ)(mp.x - rct.ഇടത്);
    *y = (പൂർണ്ണസംഖ്യ)(mp.y - rct.മുകൾ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    ശൂന്യം
gui_mch_setmouse(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    RECT rct;

    (ശൂന്യം)GetWindowRect(s_textArea, &rct);
    (ശൂന്യം)SetCursorPos(x + gui.border_offset + rct.ഇടത്,
		       y + gui.border_offset + rct.മുകൾ);
}

    സ്ഥിതം ശൂന്യം
gui_mswin_get_valid_dimensions(
    പൂർണ്ണസംഖ്യ w,
    പൂർണ്ണസംഖ്യ h,
    പൂർണ്ണസംഖ്യ *valid_w,
    പൂർണ്ണസംഖ്യ *valid_h,
    പൂർണ്ണസംഖ്യ *cols,
    പൂർണ്ണസംഖ്യ *rows)
{
    പൂർണ്ണസംഖ്യ	    base_width, base_height;

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

    ശൂന്യം
gui_mch_flash(പൂർണ്ണസംഖ്യ msec)
{
    RECT    rc;

#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ഇടത് = 0;
    rc.മുകൾ = 0;
    rc.വലത് = gui.അക്കം_cols * gui.char_width;
    rc.bottom = gui.അക്കം_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    സ്ഥിതം BOOL
is_point_onscreen(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    POINT   pt = {x, y};

    തിരികെ MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != ശൂന്യ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    സ്ഥിതം BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ഇടത്;
    p1.y = rc.മുകൾ;
    p2.x = rc.വലത് - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    എങ്കിൽ (!is_point_onscreen(p1.x, p1.y))
	തിരികെ FALSE;
    എങ്കിൽ (!is_point_onscreen(p1.x, p2.y))
	തിരികെ FALSE;
    എങ്കിൽ (!is_point_onscreen(p2.x, p1.y))
	തിരികെ FALSE;
    എങ്കിൽ (!is_point_onscreen(p2.x, p2.y))
	തിരികെ FALSE;
    തിരികെ TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_scroll_flags(ശൂന്യം)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    എങ്കിൽ (!is_window_onscreen(s_hwnd))
	തിരികെ SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    വേണ്ടി (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	എങ്കിൽ (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    എങ്കിൽ (IntersectRect(&rcDest, &rcVim, &rcOther))
		തിരികെ SW_INVALIDATE;
	}
    തിരികെ 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    സ്ഥിതം ശൂന്യം
intel_gpu_workaround(ശൂന്യം)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ശൂന്യം
gui_mch_delete_വരികൾ(
    പൂർണ്ണസംഖ്യ	    row,
    പൂർണ്ണസംഖ്യ	    അക്കം_വരികൾ)
{
    RECT	rc;

    rc.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    rc.വലത് = FILL_X(gui.scroll_region_വലത് + 1);
    rc.മുകൾ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -അക്കം_വരികൾ * gui.char_height, &rc);
    }
    അല്ലെങ്കിൽ
#endif
    {
#എങ്കിൽ defined(FEAT_DIRECTX)
	എങ്കിൽ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -അക്കം_വരികൾ * gui.char_height,
				    &rc, &rc, ശൂന്യ, ശൂന്യ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - അക്കം_വരികൾ + 1,
						       gui.scroll_region_ഇടത്,
	gui.scroll_region_bot, gui.scroll_region_വലത്);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ശൂന്യം
gui_mch_തിരുകുക_വരികൾ(
    പൂർണ്ണസംഖ്യ		row,
    പൂർണ്ണസംഖ്യ		അക്കം_വരികൾ)
{
    RECT	rc;

    rc.ഇടത് = FILL_X(gui.scroll_region_ഇടത്);
    rc.വലത് = FILL_X(gui.scroll_region_വലത് + 1);
    rc.മുകൾ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, അക്കം_വരികൾ * gui.char_height, &rc);
    }
    അല്ലെങ്കിൽ
#endif
    {
#എങ്കിൽ defined(FEAT_DIRECTX)
	എങ്കിൽ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, അക്കം_വരികൾ * gui.char_height,
				    &rc, &rc, ശൂന്യ, ശൂന്യ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_ഇടത്,
				row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);
}


    ശൂന്യം
gui_mch_exit(പൂർണ്ണസംഖ്യ rc UNUSED)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ശൂന്യ;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (ശൂന്യം)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    എങ്കിൽ (s_hwnd != ശൂന്യ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    സ്ഥിതം char_u *
logfont2name(LOGFONTW lf)
{
    അക്ഷരം	*charset_name;
    അക്ഷരം	*quality_name;
    അക്ഷരം	*font_name;
    size_t	res_size;
    അക്ഷരം	*res;

    font_name = (അക്ഷരം *)utf16_to_enc(lf.lfFaceName, ശൂന്യ);
    എങ്കിൽ (font_name == ശൂന്യ)
	തിരികെ ശൂന്യ;
    charset_name = charset_id2name((പൂർണ്ണസംഖ്യ)lf.lfCharSet);
    quality_name = quality_id2name((പൂർണ്ണസംഖ്യ)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == ശൂന്യ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ശൂന്യ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    എങ്കിൽ (res != ശൂന്യ)
    {
	അക്ഷരം	*p;
	പൂർണ്ണസംഖ്യ	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	വേണ്ടി (p = font_name; *p != NUL; ++p)
	{
	    എങ്കിൽ (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	എങ്കിൽ (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(അക്ഷരം *)res, res_size, "%s:h%d", font_name, points);
	അല്ലെങ്കിൽ
	    res_len = vim_snprintf_safelen(
		(അക്ഷരം *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (അക്ഷരം *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	എങ്കിൽ (charset_name != ശൂന്യ)
	    res_len += vim_snprintf_safelen((അക്ഷരം *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	എങ്കിൽ (quality_name != ശൂന്യ)
	    vim_snprintf((അക്ഷരം *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    തിരികെ (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    സ്ഥിതം ശൂന്യം
update_im_font(ശൂന്യം)
{
    LOGFONTW	lf_wide, lf;

    എങ്കിൽ (p_guifontwide != ശൂന്യ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, വലുപ്പം(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    അല്ലെങ്കിൽ
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    എങ്കിൽ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    ശൂന്യം
gui_mch_wide_font_changed(ശൂന്യം)
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

    എങ്കിൽ (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, വലുപ്പം(lf), &lf))
    {
	എങ്കിൽ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	എങ്കിൽ (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    എങ്കിൽ (!lf.lfItalic)
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
    പൂർണ്ണസംഖ്യ
gui_mch_init_font(char_u *font_name, പൂർണ്ണസംഖ്യ fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    എങ്കിൽ (get_logfont(&lf, font_name, ശൂന്യ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    എങ്കിൽ (font == NOFONT)
	തിരികെ FAIL;

#എങ്കിൽ defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	പൂർണ്ണസംഖ്യ		    feat_എണ്ണം = 0;
	char_u		    *fp;

	എങ്കിൽ (font_name != ശൂന്യ)
	{
	    // Find each ":f" option in font_name.
	    വേണ്ടി (fp = font_name; *fp != NUL; fp++)
	    {
		എങ്കിൽ (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    പൂർണ്ണസംഖ്യ    ti = 0;
		    unsigned പൂർണ്ണസംഖ്യ param = 1;

		    fp += 2;  // skip ":f"
		    ആകുമ്പോൾ (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    എങ്കിൽ (ti != 4)
			തുടരുക;  // invalid tag length

		    എങ്കിൽ (*fp == '=')
		    {
			fp++;
			param = (unsigned പൂർണ്ണസംഖ്യ)atoi((അക്ഷരം *)fp);
			ആകുമ്പോൾ (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    എങ്കിൽ (feat_എണ്ണം < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_എണ്ണം].tag =
			    ((unsigned പൂർണ്ണസംഖ്യ)tag[0])
			    | ((unsigned പൂർണ്ണസംഖ്യ)tag[1] << 8)
			    | ((unsigned പൂർണ്ണസംഖ്യ)tag[2] << 16)
			    | ((unsigned പൂർണ്ണസംഖ്യ)tag[3] << 24);
			features[feat_എണ്ണം].parameter = param;
			feat_എണ്ണം++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_എണ്ണം);
    }
#endif

    എങ്കിൽ (font_name == ശൂന്യ)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    എങ്കിൽ (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    എങ്കിൽ (p != ശൂന്യ)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	എങ്കിൽ (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	അല്ലെങ്കിൽ
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    എങ്കിൽ (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    എങ്കിൽ (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	എങ്കിൽ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    തിരികെ OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_maximized(ശൂന്യം)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = വലുപ്പം(WINDOWPLACEMENT);
    എങ്കിൽ (GetWindowPlacement(s_hwnd, &wp))
    {
	എങ്കിൽ (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    തിരികെ TRUE;
	എങ്കിൽ (wp.showCmd == SW_SHOWMINIMIZED)
	    തിരികെ FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	എങ്കിൽ ((rc.വലത് - rc.ഇടത് !=
		    wp.rcNormalPosition.വലത് - wp.rcNormalPosition.ഇടത്)
		|| (rc.bottom - rc.മുകൾ !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.മുകൾ))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ശൂന്യം
gui_mch_newfont(ശൂന്യം)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    എങ്കിൽ (win_socket_id == 0)
    {
	gui_resize_shell(rect.വലത് - rect.ഇടത്
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.മുകൾ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    അല്ലെങ്കിൽ
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.വലത് - rect.ഇടത്,
	    rect.bottom - rect.മുകൾ - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    ശൂന്യം
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == ശൂന്യ ? "VIM" : (അക്ഷരം *)title));
}

#എങ്കിൽ defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
സ്ഥിതം LPCSTR mshape_idcs[] =
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

    ശൂന്യം
mch_set_mouse_shape(പൂർണ്ണസംഖ്യ shape)
{
    LPCSTR idc;

    എങ്കിൽ (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	അല്ലെങ്കിൽ
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(ശൂന്യ, idc));
	എങ്കിൽ (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (ശൂന്യം)GetCursorPos(&mp);
	    (ശൂന്യം)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#എങ്കിൽ defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    സ്ഥിതം WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    പൂർണ്ണസംഖ്യ len;
    WCHAR *res;

    tmp = convert_filter(s);
    എങ്കിൽ (tmp == ശൂന്യ)
	തിരികെ ശൂന്യ;
    len = (പൂർണ്ണസംഖ്യ)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    തിരികെ res;
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
	പൂർണ്ണസംഖ്യ saving,
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
    പൂർണ്ണസംഖ്യ			i;
    WCHAR		*titlep = ശൂന്യ;
    WCHAR		*extp = ശൂന്യ;
    WCHAR		*initdirp = ശൂന്യ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    എങ്കിൽ (dflt == ശൂന്യ)
	fileBuf[0] = NUL;
    അല്ലെങ്കിൽ
    {
	wp = enc_to_utf16(dflt, ശൂന്യ);
	എങ്കിൽ (wp == ശൂന്യ)
	    fileBuf[0] = NUL;
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# അല്ലെങ്കിൽ
    fileStruct.lStructSize = വലുപ്പം(fileStruct);
# endif

    എങ്കിൽ (title != ശൂന്യ)
	titlep = enc_to_utf16(title, ശൂന്യ);
    fileStruct.lpstrTitle = titlep;

    എങ്കിൽ (ext != ശൂന്യ)
	extp = enc_to_utf16(ext, ശൂന്യ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    എങ്കിൽ (initdir != ശൂന്യ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, ശൂന്യ);
	എങ്കിൽ (initdirp != ശൂന്യ)
	{
	    വേണ്ടി (wp = initdirp; *wp != NUL; ++wp)
		എങ്കിൽ (*wp == '/')
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
    എങ്കിൽ (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    എങ്കിൽ (saving)
	ret = GetSaveFileNameW(&fileStruct);
    അല്ലെങ്കിൽ
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    എങ്കിൽ (!ret)
	തിരികെ ശൂന്യ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, ശൂന്യ);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    തിരികെ q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    സ്ഥിതം char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    എങ്കിൽ (res != ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < s_len; ++i)
	    എങ്കിൽ (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    അല്ലെങ്കിൽ
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    തിരികെ res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    തിരികെ gui_mch_browse(0, title, (char_u *)_("Not Used"), ശൂന്യ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    സ്ഥിതം ശൂന്യം
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    അക്ഷരം    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ശൂന്യ, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    എങ്കിൽ (fnames != ശൂന്യ)
	വേണ്ടി (i = 0; i < cFiles; ++i)
	{
	    എങ്കിൽ (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, ശൂന്യ);
	    അല്ലെങ്കിൽ
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    എങ്കിൽ (fnames == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ kbd_modifiers = get_active_modifiers();

    എങ്കിൽ ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    എങ്കിൽ ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    എങ്കിൽ ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    പൂർണ്ണസംഖ്യ pos)
{
    സ്ഥിതം UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    പൂർണ്ണസംഖ്യ		dragging = FALSE;
    പൂർണ്ണസംഖ്യ		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = വലുപ്പം(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    എങ്കിൽ (sb == ശൂന്യ)
	തിരികെ 0;

    എങ്കിൽ (sb->wp != ശൂന്യ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    അല്ലെങ്കിൽ	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->മൂല്യം;

    തിരഞ്ഞെടുപ്പ് (code)
    {
	അവസ്ഥ SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    എങ്കിൽ (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    നിർത്തുക;
	അവസ്ഥ SB_LINEDOWN:
	    val++;
	    നിർത്തുക;
	അവസ്ഥ SB_LINEUP:
	    val--;
	    നിർത്തുക;
	അവസ്ഥ SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    നിർത്തുക;
	അവസ്ഥ SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    നിർത്തുക;
	അവസ്ഥ SB_മുകൾ:
	    val = 0;
	    നിർത്തുക;
	അവസ്ഥ SB_BOTTOM:
	    val = sb_info->max;
	    നിർത്തുക;
	അവസ്ഥ SB_ENDSCROLL:
	    എങ്കിൽ (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		എങ്കിൽ (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    തിരികെ 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    എങ്കിൽ (sb->wp != ശൂന്യ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ഇടത്) ? SBAR_വലത് : SBAR_ഇടത്].id;

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

    തിരികെ 0;
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

സ്ഥിതം BalloonEval  *cur_beval = ശൂന്യ;
സ്ഥിതം UINT_PTR	    beval_timer_id = 0;
സ്ഥിതം DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
സ്ഥിതം UINT	s_menu_id = 100;
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
സ്ഥിതം PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	സ്ഥിരം അക്ഷരം *caption);
സ്ഥിതം LPWORD lpwAlign(LPWORD);
സ്ഥിതം പൂർണ്ണസംഖ്യ nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
സ്ഥിതം ശൂന്യം gui_mch_tearoff(char_u *title, vimmenu_T *menu, പൂർണ്ണസംഖ്യ initX, പൂർണ്ണസംഖ്യ initY);
#endif
സ്ഥിതം ശൂന്യം get_dialog_font_metrics(ശൂന്യം);

സ്ഥിതം പൂർണ്ണസംഖ്യ dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
സ്ഥിതം ശൂന്യം initialise_toolbar(ശൂന്യം);
സ്ഥിതം ശൂന്യം update_toolbar_size(ശൂന്യം);
സ്ഥിതം LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_സന്ദേശം, WPARAM wParam, LPARAM lParam);
സ്ഥിതം പൂർണ്ണസംഖ്യ get_toolbar_bitmap(vimmenu_T *menu);
#അല്ലെങ്കിൽ
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
സ്ഥിതം ശൂന്യം initialise_tabline(ശൂന്യം);
സ്ഥിതം LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_സന്ദേശം, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
സ്ഥിതം LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
സ്ഥിതം char_u *GetResultStr(HWND hwnd, പൂർണ്ണസംഖ്യ GCS, പൂർണ്ണസംഖ്യ *lenp);
#endif
#എങ്കിൽ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
തരനിർവചനം ഘടന tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
തരനിർവചനം HANDLE HIMC;
# endif

സ്ഥിതം HINSTANCE hLibImm = ശൂന്യ;
സ്ഥിതം LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
സ്ഥിതം HIMC (WINAPI *pImmGetContext)(HWND);
സ്ഥിതം HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
സ്ഥിതം BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
സ്ഥിതം BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
സ്ഥിതം BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
സ്ഥിതം BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
സ്ഥിതം BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
സ്ഥിതം BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
സ്ഥിതം BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
സ്ഥിതം BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
സ്ഥിതം ശൂന്യം dyn_imm_load(ശൂന്യം);
#അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_mswin_get_menu_height(
    പൂർണ്ണസംഖ്യ	    fix_window)	    // If TRUE, resize window if menu height changed
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	old_menu_height = -1;

    RECT    rc1, rc2;
    പൂർണ്ണസംഖ്യ	    അക്കം;
    പൂർണ്ണസംഖ്യ	    menu_height;

    എങ്കിൽ (gui.menu_is_active)
	അക്കം = Get_Menu_Item_എണ്ണം(s_menuBar);
    അല്ലെങ്കിൽ
	അക്കം = 0;

    എങ്കിൽ (അക്കം == 0)
	menu_height = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	എങ്കിൽ (gui.starting)
	    menu_height = rc1.bottom - rc1.മുകൾ + 1;
	അല്ലെങ്കിൽ
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, അക്കം - 1, &rc2);
	    menu_height = rc2.bottom - rc1.മുകൾ + 1;
	}
    }

    എങ്കിൽ (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    തിരികെ menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    സ്ഥിതം long
mouse_vertical_scroll_step(ശൂന്യം)
{
    UINT val;
    എങ്കിൽ (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	തിരികെ (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    തിരികെ 3; // Safe default;
}

    സ്ഥിതം long
mouse_horizontal_scroll_step(ശൂന്യം)
{
    UINT val;
    എങ്കിൽ (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	തിരികെ (long)val;
    തിരികെ 3; // Safe default;
}

    സ്ഥിതം ശൂന്യം
init_mouse_wheel(ശൂന്യം)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    സ്ഥിതം ശൂന്യം
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, പൂർണ്ണസംഖ്യ horizontal)
{
    പൂർണ്ണസംഖ്യ		button;
    win_T	*wp;
    പൂർണ്ണസംഖ്യ		modifiers = 0;
    പൂർണ്ണസംഖ്യ		kbd_modifiers;
    പൂർണ്ണസംഖ്യ		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    എങ്കിൽ (wp != ശൂന്യ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	എങ്കിൽ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ഇടത് : MSCR_വലത്;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	അല്ലെങ്കിൽ
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

    എങ്കിൽ (wp == ശൂന്യ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    എങ്കിൽ (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    അല്ലെങ്കിൽ
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    എങ്കിൽ ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    എങ്കിൽ ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    എങ്കിൽ ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = വലുപ്പം(NONCLIENTMETRICSW);
    എങ്കിൽ (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    വലുപ്പം(NONCLIENTMETRICSW),
	    &nm,
	    0))
	തിരികെ FAIL;
    *lf = nm.lfMenuFont;
    തിരികെ OK;
}
#endif


#എങ്കിൽ defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    സ്ഥിതം ശൂന്യം
set_tabline_font(ശൂന്യം)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    എങ്കിൽ (gui_w32_get_menu_font(&lfSysmenu) != OK)
	തിരികെ;

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
#അല്ലെങ്കിൽ
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    സ്ഥിതം LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    തിരഞ്ഞെടുപ്പ് (param)
    {
	അവസ്ഥ SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    നിർത്തുക;
	അവസ്ഥ SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    നിർത്തുക;
	അവസ്ഥ SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ 0;
}

#ifdef FEAT_NETBEANS_INTG
    സ്ഥിതം ശൂന്യം
_OnWindowPosChanged(
    HWND hwnd,
    സ്ഥിരം LPWINDOWPOS lpwpos)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ x = 0, y = 0, cx = 0, cy = 0;
    ബാഹ്യ പൂർണ്ണസംഖ്യ WSInitialized;

    എങ്കിൽ (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


സ്ഥിതം HWND hwndTip = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
show_sizing_tip(പൂർണ്ണസംഖ്യ cols, പൂർണ്ണസംഖ്യ rows)
{
    TOOLINFOA ti;
    അക്ഷരം buf[32];

    ti.cbSize = വലുപ്പം(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    എങ്കിൽ (hwndTip == ശൂന്യ)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, ശൂന്യ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ശൂന്യ, GetModuleHandle(ശൂന്യ), ശൂന്യ);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    അല്ലെങ്കിൽ
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    സ്ഥിതം ശൂന്യം
destroy_sizing_tip(ശൂന്യം)
{
    എങ്കിൽ (hwndTip == ശൂന്യ)
	തിരികെ;

    DestroyWindow(hwndTip);
    hwndTip = ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    പൂർണ്ണസംഖ്യ	    w, h;
    പൂർണ്ണസംഖ്യ	    valid_w, valid_h;
    പൂർണ്ണസംഖ്യ	    w_offset, h_offset;
    പൂർണ്ണസംഖ്യ	    cols, rows;

    w = lprc->വലത് - lprc->ഇടത്;
    h = lprc->bottom - lprc->മുകൾ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    എങ്കിൽ (fwSide == WMSZ_ഇടത് || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ഇടത് += w_offset;
    അല്ലെങ്കിൽ എങ്കിൽ (fwSide == WMSZ_വലത് || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->വലത് -= w_offset;

    എങ്കിൽ (fwSide == WMSZ_മുകൾ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->മുകൾ += h_offset;
    അല്ലെങ്കിൽ എങ്കിൽ (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    തിരികെ TRUE;
}

#ifdef FEAT_GUI_TABLINE
    സ്ഥിതം ശൂന്യം
_OnRButtonUp(HWND hwnd, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, UINT താക്കോൽ_Flags)
{
    എങ്കിൽ (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	എങ്കിൽ (pt.y < rect.മുകൾ)
	{
	    show_tabline_popup_menu();
	    തിരികെ;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, താക്കോൽ_Flags, DefWindowProcW);
}

    സ്ഥിതം ശൂന്യം
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, UINT താക്കോൽ_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    എങ്കിൽ (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	എങ്കിൽ (pt.y < rect.മുകൾ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, താക്കോൽ_Flags, DefWindowProcW);
}
#endif

    സ്ഥിതം UINT
_OnNCHitTest(HWND hwnd, പൂർണ്ണസംഖ്യ xPos, പൂർണ്ണസംഖ്യ yPos)
{
    UINT	result;
    പൂർണ്ണസംഖ്യ		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    എങ്കിൽ (result != HTCLIENT)
	തിരികെ result;

#ifdef FEAT_GUI_TABLINE
    എങ്കിൽ (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	എങ്കിൽ (yPos < rct.മുകൾ)
	    തിരികെ result;
    }
#endif
    (ശൂന്യം)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    എങ്കിൽ (xPos < 48) // <VN> TODO should use system metric?
	തിരികെ HTBOTTOMLEFT;
    അല്ലെങ്കിൽ
	തിരികെ HTBOTTOMRIGHT;
}

#എങ്കിൽ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    സ്ഥിതം LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    തിരഞ്ഞെടുപ്പ് (hdr->code)
    {
	അവസ്ഥ TTN_GETDISPINFOW:
	അവസ്ഥ TTN_GETDISPINFO:
	    {
		char_u		*str = ശൂന്യ;
		സ്ഥിതം ശൂന്യം	*tt_text = ശൂന്യ;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		എങ്കിൽ (gui_mch_showing_tabline()
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
		    എങ്കിൽ (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			പൂർണ്ണസംഖ്യ idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			എങ്കിൽ (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    എങ്കിൽ (tp != ശൂന്യ)
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
		അല്ലെങ്കിൽ
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(വേര്_menu, idButton);
		    എങ്കിൽ (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		എങ്കിൽ (str == ശൂന്യ)
		    നിർത്തുക;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		എങ്കിൽ (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, ശൂന്യ);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		അല്ലെങ്കിൽ
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    എങ്കിൽ (len < വലുപ്പം(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == ശൂന്യ))
			vim_strncpy((char_u *)lpdi->szText, str,
				വലുപ്പം(lpdi->szText) - 1);
		    അല്ലെങ്കിൽ
			lpdi->lpszText = tt_text;
		}
	    }
	    നിർത്തുക;

# ifdef FEAT_GUI_TABLINE
	അവസ്ഥ TCN_SELCHANGE:
	    എങ്കിൽ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		തിരികെ 0L;
	    }
	    നിർത്തുക;

	അവസ്ഥ NM_RCLICK:
	    എങ്കിൽ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		തിരികെ 0L;
	    }
	    നിർത്തുക;
# endif

	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    തിരികെ DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#എങ്കിൽ defined(MENUHINTS) && defined(FEAT_MENU)
    സ്ഥിതം LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    എങ്കിൽ (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	സ്ഥിതം പൂർണ്ണസംഖ്യ  did_menu_tip = FALSE;

	എങ്കിൽ (did_menu_tip)
	{
	    സന്ദേശം_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(വേര്_menu, (UINT)LOWORD(wParam));
	എങ്കിൽ (pMenu != ശൂന്യ && pMenu->strings[MENU_INDEX_TIP] != ശൂന്യ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = വലുപ്പം(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    എങ്കിൽ (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++സന്ദേശം_hist_off;
		സന്ദേശം((അക്ഷരം *)pMenu->strings[MENU_INDEX_TIP]);
		--സന്ദേശം_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	തിരികെ 0L;
    }
    തിരികെ DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    സ്ഥിതം BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    പൂർണ്ണസംഖ്യ		old_width, old_height;
    പൂർണ്ണസംഖ്യ		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, വലുപ്പം(lf), &lf);
    lf.lfHeight = lf.lfHeight * (പൂർണ്ണസംഖ്യ)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    എങ്കിൽ (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    അല്ലെങ്കിൽ
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    തിരികെ TRUE;
}

    സ്ഥിതം LRESULT
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
    InvalidateRect(hwnd, ശൂന്യ, TRUE);

    s_in_dpichanged = FALSE;
    തിരികെ 0L;
}


    സ്ഥിതം LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_സന്ദേശം,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_സന്ദേശം, lParam);

    s_u_സന്ദേശം = u_സന്ദേശം;
    s_wParam = wParam;

    തിരഞ്ഞെടുപ്പ് (u_സന്ദേശം)
    {
	HANDLE_സന്ദേശം(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_സന്ദേശം(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_സന്ദേശം(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_സന്ദേശം(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_സന്ദേശം(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_സന്ദേശം(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_സന്ദേശം(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_സന്ദേശം(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_സന്ദേശം(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_സന്ദേശം(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_സന്ദേശം(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_സന്ദേശം(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_സന്ദേശം(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_സന്ദേശം(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_സന്ദേശം(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_സന്ദേശം(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    അവസ്ഥ WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	തിരികെ FALSE;		// Do NOT allow system to go down.

    അവസ്ഥ WM_ENDSESSION:
	എങ്കിൽ (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    തിരികെ 0L;
	}
	നിർത്തുക;

    അവസ്ഥ WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (പൂർണ്ണസംഖ്യ)(short)LOWORD(lParam));
	തിരികെ 0L;

    അവസ്ഥ WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	എങ്കിൽ (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((പൂർണ്ണസംഖ്യ)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (പൂർണ്ണസംഖ്യ)(short)LOWORD(lParam));
	    തിരികെ 0L;
	}
#ifdef FEAT_MENU
	അല്ലെങ്കിൽ
	    തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
#endif

    അവസ്ഥ WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
#അല്ലെങ്കിൽ
	തിരികെ 0L;
#endif

    അവസ്ഥ WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	നിർത്തുക;

    അവസ്ഥ WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	തിരികെ _DuringSizing((UINT)wParam, (LPRECT)lParam);

    അവസ്ഥ WM_MOUSEWHEEL:
    അവസ്ഥ WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_സന്ദേശം == WM_MOUSEHWHEEL);
	തിരികെ 0L;

	// Notification for change in SystemParametersInfo()
    അവസ്ഥ WM_SETTINGCHANGE:
	തിരികെ _OnSettingChange((UINT)wParam);

#എങ്കിൽ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    അവസ്ഥ WM_NOTIFY:
	തിരികെ _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#എങ്കിൽ defined(MENUHINTS) && defined(FEAT_MENU)
    അവസ്ഥ WM_MENUSELECT:
	തിരികെ _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    അവസ്ഥ WM_IME_NOTIFY:
	എങ്കിൽ (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
	തിരികെ 1L;

    അവസ്ഥ WM_IME_COMPOSITION:
	എങ്കിൽ (!_OnImeComposition(hwnd, wParam, lParam))
	    തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
	തിരികെ 1L;
#endif
    അവസ്ഥ WM_GETDPISCALEDSIZE:
	തിരികെ _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    അവസ്ഥ WM_DPICHANGED:
	തിരികെ _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    സ്ഥിരസ്ഥിതി:
#ifdef MSWIN_FIND_REPLACE
	എങ്കിൽ (u_സന്ദേശം == s_findrep_സന്ദേശം && s_findrep_സന്ദേശം != 0)
	    _OnFindRepl();
#endif
	നിർത്തുക;
    }

    തിരികെ DefWindowProcW(hwnd, u_സന്ദേശം, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = ശൂന്യ;

    സ്ഥിതം BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    അക്ഷരം	buf[2048];
    അക്ഷരം	*title = (അക്ഷരം *)lParam;

    എങ്കിൽ (GetWindowText(hwnd, buf, വലുപ്പം(buf)))
    {
	എങ്കിൽ (strstr(buf, title) != ശൂന്യ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, ശൂന്യ, "MDIClient", ശൂന്യ);
	    എങ്കിൽ (vim_parent_hwnd != ശൂന്യ)
		തിരികെ FALSE;
	}
    }
    തിരികെ TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    ശൂന്യം
gui_mch_set_parent(അക്ഷരം *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    എങ്കിൽ (vim_parent_hwnd == ശൂന്യ)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    സ്ഥിതം ശൂന്യം
ole_error(അക്ഷരം *arg)
{
    അക്ഷരം buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#എങ്കിൽ defined(GUI_MAY_SPAWN)
    സ്ഥിതം അക്ഷരം *
gvim_error(ശൂന്യം)
{
    അക്ഷരം *സന്ദേശം = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    എങ്കിൽ (starting)
    {
	mch_errmsg(സന്ദേശം);
	mch_errmsg("\n");
	mch_exit(2);
    }
    തിരികെ സന്ദേശം;
}

    അക്ഷരം *
gui_mch_do_spawn(char_u *arg)
{
    പൂർണ്ണസംഖ്യ			len;
# എങ്കിൽ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ശൂന്യ;
    LPWSTR		tofree1 = ശൂന്യ;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = ശൂന്യ, p, warg, tofree2 = ശൂന്യ;
    STARTUPINFOW	si = {വലുപ്പം(si)};
    PROCESS_INFORMATION pi;

    എങ്കിൽ (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	പോകുക error;
    p = wcsrchr(name, L'\\');
    എങ്കിൽ (p == ശൂന്യ)
	പോകുക error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# അല്ലെങ്കിൽ
    wcscpy(p + 1, L"gvim.exe");
# endif

# എങ്കിൽ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    എങ്കിൽ (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_വരി_W();
	// Skip 1st argument.
	ആകുമ്പോൾ (*p && *p != L' ' && *p != L'\t')
	{
	    എങ്കിൽ (*p == L'"')
	    {
		// Skip quoted strings
		ആകുമ്പോൾ (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# എങ്കിൽ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    അല്ലെങ്കിൽ
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	പൂർണ്ണസംഖ്യ	ret;

	session = vim_tempname('s', FALSE);
	എങ്കിൽ (session == ശൂന്യ)
	    പോകുക error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	എങ്കിൽ (p_bg == ശൂന്യ)
	{
	    p_bg = savebg;
	    പോകുക error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	എങ്കിൽ (!ret)
	    പോകുക error;
	wsession = enc_to_utf16(session, ശൂന്യ);
	എങ്കിൽ (wsession == ശൂന്യ)
	    പോകുക error;
	len = (പൂർണ്ണസംഖ്യ)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	എങ്കിൽ (cmd == ശൂന്യ)
	{
	    vim_free(wsession);
	    പോകുക error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    എങ്കിൽ (arg != ശൂന്യ)
    {
	warg = enc_to_utf16(arg, ശൂന്യ);
	എങ്കിൽ (warg == ശൂന്യ)
	    പോകുക error;
	tofree2 = warg;
    }
    അല്ലെങ്കിൽ
	warg = L"";

    // Set up the new command line.
    len = (പൂർണ്ണസംഖ്യ)wcslen(name) + (പൂർണ്ണസംഖ്യ)wcslen(cmd) + (പൂർണ്ണസംഖ്യ)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    എങ്കിൽ (newcmd == ശൂന്യ)
	പോകുക error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    എങ്കിൽ (!CreateProcessW(ശൂന്യ, newcmd, ശൂന്യ, ശൂന്യ, TRUE, 0,
		ശൂന്യ, ശൂന്യ, &si, &pi))
	പോകുക error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# എങ്കിൽ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    എങ്കിൽ (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    തിരികെ gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    ശൂന്യം
gui_mch_prepare(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv)
{
    പൂർണ്ണസംഖ്യ		silent = FALSE;
    പൂർണ്ണസംഖ്യ		idx;

    // Check for special OLE command line parameters
    എങ്കിൽ ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	എങ്കിൽ (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	അല്ലെങ്കിൽ
	    idx = 1;

	// Register Vim as an OLE Automation server
	എങ്കിൽ (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#അല്ലെങ്കിൽ
	    എങ്കിൽ (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	എങ്കിൽ (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#അല്ലെങ്കിൽ
	    എങ്കിൽ (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	എങ്കിൽ (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#അല്ലെങ്കിൽ
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    എങ്കിൽ (mch_is_gui_executable())
# endif
    {
	പൂർണ്ണസംഖ്യ	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	എങ്കിൽ (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	പൂർണ്ണസംഖ്യ arg;

	വേണ്ടി (arg = 1; arg < *argc; arg++)
	    എങ്കിൽ (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * വലുപ്പം(അക്ഷരം *));
		argv[*argc] = ശൂന്യ;
		നിർത്തുക;	// enough?
	    }
    }
#endif
}

    സ്ഥിതം ശൂന്യം
load_dpi_func(ശൂന്യം)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    എങ്കിൽ (hUser32 == ശൂന്യ)
	പോകുക fail;

    pGetDpiForSystem = (UINT (WINAPI *)(ശൂന്യം))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (പൂർണ്ണസംഖ്യ (WINAPI *)(പൂർണ്ണസംഖ്യ, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    എങ്കിൽ (pSetThreadDpiAwarenessContext != ശൂന്യ)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	എങ്കിൽ (oldctx != ശൂന്യ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    എങ്കിൽ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    തിരികെ;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = ശൂന്യ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = ശൂന്യ;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ശൂന്യ;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init(ശൂന്യം)
{
    സ്ഥിരം WCHAR szVimWndClassW[] = VIM_CLASSW;
    സ്ഥിരം WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    എങ്കിൽ (s_hwnd != ശൂന്യ)
	പോകുക theend;

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
    എങ്കിൽ (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(ശൂന്യ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ശൂന്യ;
	wndclassw.lpszClassName = szVimWndClassW;

	എങ്കിൽ (RegisterClassW(&wndclassw) == 0)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (vim_parent_hwnd != ശൂന്യ)
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
		vim_parent_hwnd, ശൂന്യ,
		g_hinst, ശൂന്യ);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	എങ്കിൽ (s_hwnd == ശൂന്യ)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    അല്ലെങ്കിൽ
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	എങ്കിൽ (IsWindow((HWND)win_socket_id) <= 0)
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
		ശൂന്യ, ശൂന്യ,
		g_hinst, ശൂന്യ);
	എങ്കിൽ (s_hwnd != ശൂന്യ && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    എങ്കിൽ (s_hwnd == ശൂന്യ)
	തിരികെ FAIL;

    എങ്കിൽ (pGetDpiForWindow != ശൂന്യ)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#എങ്കിൽ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    എങ്കിൽ (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ശൂന്യ;
	wndclassw.hCursor = LoadCursor(ശൂന്യ, IDC_ARROW);
	wndclassw.hbrBackground = ശൂന്യ;
	wndclassw.lpszMenuName = ശൂന്യ;
	wndclassw.lpszClassName = szTextAreaClassW;

	എങ്കിൽ (RegisterClassW(&wndclassw) == 0)
	    തിരികെ FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ശൂന്യ,
	g_hinst, ശൂന്യ);

    എങ്കിൽ (s_textArea == ശൂന്യ)
	തിരികെ FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ശൂന്യ;

	എങ്കിൽ (mch_icon_load(&hIcon) == OK && hIcon != ശൂന്യ)
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
    s_findrep_സന്ദേശം = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = വലുപ്പം(s_findrep_struct);
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
    എങ്കിൽ (p_rop)
	(ശൂന്യം)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    തിരികെ OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    സ്ഥിതം ശൂന്യം
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    എങ്കിൽ (mon != ശൂന്യ)
    {
	moninfo.cbSize = വലുപ്പം(MONITORINFO);
	എങ്കിൽ (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    തിരികെ;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    ശൂന്യം
gui_mch_set_shellsize(
	പൂർണ്ണസംഖ്യ width,
	പൂർണ്ണസംഖ്യ height,
	പൂർണ്ണസംഖ്യ min_width UNUSED,
	പൂർണ്ണസംഖ്യ min_height UNUSED,
	പൂർണ്ണസംഖ്യ base_width UNUSED,
	പൂർണ്ണസംഖ്യ base_height UNUSED,
	പൂർണ്ണസംഖ്യ direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    പൂർണ്ണസംഖ്യ		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    എങ്കിൽ (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    എങ്കിൽ (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    അല്ലെങ്കിൽ
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
    window_rect.വലത് = window_rect.ഇടത് + win_width;
    window_rect.bottom = window_rect.മുകൾ + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    എങ്കിൽ (!s_in_dpichanged)
    {
	എങ്കിൽ ((direction & RESIZE_HOR)
		&& window_rect.വലത് > workarea_rect.വലത്)
	    OffsetRect(&window_rect,
		    workarea_rect.വലത് - window_rect.വലത്, 0);

	എങ്കിൽ ((direction & RESIZE_HOR)
		&& window_rect.ഇടത് < workarea_rect.ഇടത്)
	    OffsetRect(&window_rect,
		    workarea_rect.ഇടത് - window_rect.ഇടത്, 0);

	എങ്കിൽ ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	എങ്കിൽ ((direction & RESIZE_VERT)
		&& window_rect.മുകൾ < workarea_rect.മുകൾ)
	    OffsetRect(&window_rect,
		    0, workarea_rect.മുകൾ - window_rect.മുകൾ);
    }

    MoveWindow(s_hwnd, window_rect.ഇടത്, window_rect.മുകൾ,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    ശൂന്യം
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    ആകുമ്പോൾ (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    എങ്കിൽ (sb->scroll_shift > 0)
	++size;

    info.cbSize = വലുപ്പം(info);
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
    ശൂന്യം
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    ശൂന്യം
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    ശൂന്യം
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    ശൂന്യം
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
    സ്ഥിതം LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    എങ്കിൽ (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	തിരികെ lResult;
    തിരഞ്ഞെടുപ്പ് (dwCommand)
    {
	അവസ്ഥ IMN_SETOPENSTATUS:
	    എങ്കിൽ (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		എങ്കിൽ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		എങ്കിൽ (State & MODE_തിരുകുക)
		{
# എങ്കിൽ defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    എങ്കിൽ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			പൂർണ്ണസംഖ്യ old_row = gui.row;
			പൂർണ്ണസംഖ്യ old_col = gui.col;

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
	    നിർത്തുക;
    }
    pImmReleaseContext(hWnd, hImc);
    തിരികെ lResult;
}

    സ്ഥിതം LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    പൂർണ്ണസംഖ്യ		len;

    എങ്കിൽ ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	തിരികെ 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    എങ്കിൽ (ret != ശൂന്യ)
    {
	add_to_നിവേശം_buf_csi(ret, len);
	vim_free(ret);
	തിരികെ 1;
    }
    തിരികെ 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    സ്ഥിതം char_u *
GetResultStr(HWND hwnd, പൂർണ്ണസംഖ്യ GCS, പൂർണ്ണസംഖ്യ *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ശൂന്യ;
    char_u	*convbuf = ശൂന്യ;

    എങ്കിൽ (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	തിരികെ ശൂന്യ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ശൂന്യ, 0);
    എങ്കിൽ (ret <= 0)
	തിരികെ ശൂന്യ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + വലുപ്പം(WCHAR));
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / വലുപ്പം(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    തിരികെ convbuf;
}
#endif

// For global functions we need prototypes.
#എങ്കിൽ defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    ശൂന്യം
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    എങ്കിൽ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    ശൂന്യം
im_set_position(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col)
{
    HIMC hImc;

    എങ്കിൽ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	എങ്കിൽ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    ശൂന്യം
im_set_active(പൂർണ്ണസംഖ്യ active)
{
    HIMC	hImc;
    സ്ഥിതം HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	തിരികെ;
    }
# endif

    എങ്കിൽ (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	തിരികെ;

    എങ്കിൽ (p_imdisable)
    {
	എങ്കിൽ (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    എങ്കിൽ (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    എങ്കിൽ (!hImc)
	തിരികെ;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    എങ്കിൽ (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	സ്ഥിതം DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	സ്ഥിതം BOOL bSaved = FALSE;

	എങ്കിൽ (active)
	{
	    // if we have a saved conversion status, restore it
	    എങ്കിൽ (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // save conversion status and disable korean
	    എങ്കിൽ (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    പൂർണ്ണസംഖ്യ
im_get_status(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    എങ്കിൽ (!gui.in_use && !gui.starting)
	തിരികെ mbyte_im_get_status();
# endif

    എങ്കിൽ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    തിരികെ status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    സ്ഥിതം ശൂന്യം
latin9_to_ucs(char_u *text, പൂർണ്ണസംഖ്യ len, WCHAR *unicodebuf)
{
    പൂർണ്ണസംഖ്യ		c;

    ആകുമ്പോൾ (--len >= 0)
    {
	c = *text++;
	തിരഞ്ഞെടുപ്പ് (c)
	{
	    അവസ്ഥ 0xa4: c = 0x20ac; നിർത്തുക;   // euro
	    അവസ്ഥ 0xa6: c = 0x0160; നിർത്തുക;   // S hat
	    അവസ്ഥ 0xa8: c = 0x0161; നിർത്തുക;   // S -hat
	    അവസ്ഥ 0xb4: c = 0x017d; നിർത്തുക;   // Z hat
	    അവസ്ഥ 0xb8: c = 0x017e; നിർത്തുക;   // Z -hat
	    അവസ്ഥ 0xbc: c = 0x0152; നിർത്തുക;   // OE
	    അവസ്ഥ 0xbd: c = 0x0153; നിർത്തുക;   // oe
	    അവസ്ഥ 0xbe: c = 0x0178; നിർത്തുക;   // Y
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
    സ്ഥിതം ശൂന്യം
RevOut( HDC hdc,
	പൂർണ്ണസംഖ്യ col,
	പൂർണ്ണസംഖ്യ row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    പൂർണ്ണസംഖ്യ		ix;

    വേണ്ടി (ix = 0; ix < (പൂർണ്ണസംഖ്യ)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    സ്ഥിതം ശൂന്യം
draw_വരി(
    പൂർണ്ണസംഖ്യ		x1,
    പൂർണ്ണസംഖ്യ		y1,
    പൂർണ്ണസംഖ്യ		x2,
    പൂർണ്ണസംഖ്യ		y2,
    COLORREF	color)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_വരി(s_dwc, x1, y1, x2, y2, color);
    അല്ലെങ്കിൽ
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, ശൂന്യ);
	// Note: LineTo() excludes the last pixel in the line.
	വരി_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    സ്ഥിതം ശൂന്യം
set_pixel(
    പൂർണ്ണസംഖ്യ		x,
    പൂർണ്ണസംഖ്യ		y,
    COLORREF	color)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    അല്ലെങ്കിൽ
#endif
	SetPixel(s_hdc, x, y, color);
}

    സ്ഥിതം ശൂന്യം
fill_rect(
    സ്ഥിരം RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    അല്ലെങ്കിൽ
#endif
    {
	HBRUSH	hbr2;

	എങ്കിൽ (hbr == ശൂന്യ)
	    hbr2 = CreateSolidBrush(color);
	അല്ലെങ്കിൽ
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	എങ്കിൽ (hbr == ശൂന്യ)
	    DeleteBrush(hbr2);
    }
}

    ശൂന്യം
gui_mch_draw_string(
    പൂർണ്ണസംഖ്യ		row,
    പൂർണ്ണസംഖ്യ		col,
    char_u	*text,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		flags)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	*padding = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	pad_size = 0;
    സ്ഥിരം RECT	*pcliprect = ശൂന്യ;
    UINT	foptions = 0;
    സ്ഥിതം WCHAR *unicodebuf = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ   *unicodepdy = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	unibuflen = 0;
    പൂർണ്ണസംഖ്യ		n = 0;
    പൂർണ്ണസംഖ്യ		y;

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
    സ്ഥിതം HBRUSH	hbr_cache[2] = {ശൂന്യ, ശൂന്യ};
    സ്ഥിതം guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    സ്ഥിതം പൂർണ്ണസംഖ്യ		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    എങ്കിൽ (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ഇടത് = FILL_X(col);
	rc.മുകൾ = FILL_Y(row);
	എങ്കിൽ (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.വലത് = FILL_X(col + mb_string2cells(text, len));
	}
	അല്ലെങ്കിൽ
	    rc.വലത് = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	എങ്കിൽ (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (hbr_cache[brush_lru] != ശൂന്യ)
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
	എങ്കിൽ (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	അല്ലെങ്കിൽ എങ്കിൽ (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    എങ്കിൽ (pad_size != Columns || padding == ശൂന്യ || padding[0] != gui.char_width)
    {
	പൂർണ്ണസംഖ്യ	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(പൂർണ്ണസംഖ്യ, pad_size);
	എങ്കിൽ (padding != ശൂന്യ)
	    വേണ്ടി (i = 0; i < pad_size; i++)
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
    എങ്കിൽ (enc_utf8)
	വേണ്ടി (n = 0; n < len; ++n)
	    എങ്കിൽ (text[n] >= 0x80)
		നിർത്തുക;

#എങ്കിൽ defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    എങ്കിൽ ((enc_utf8
		|| (enc_codepage > 0 && (പൂർണ്ണസംഖ്യ)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ശൂന്യ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(പൂർണ്ണസംഖ്യ, len);

	unibuflen = len;
    }

    എങ്കിൽ (enc_utf8 && n < len && unicodebuf != ശൂന്യ)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	പൂർണ്ണസംഖ്യ		i;
	പൂർണ്ണസംഖ്യ		wlen;	// string length in words
	പൂർണ്ണസംഖ്യ		cells;	// cell width of string up to composing char
	പൂർണ്ണസംഖ്യ		cw;	// width of current cell
	പൂർണ്ണസംഖ്യ		c;

	wlen = 0;
	cells = 0;
	വേണ്ടി (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    എങ്കിൽ (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    അല്ലെങ്കിൽ
	    {
		unicodebuf[wlen++] = c;
	    }

	    എങ്കിൽ (utf_iscomposing(c))
		cw = 0;
	    അല്ലെങ്കിൽ
	    {
		cw = utf_char2cells(c);
		എങ്കിൽ (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    എങ്കിൽ (unicodepdy != ശൂന്യ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		എങ്കിൽ (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		അല്ലെങ്കിൽ
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#എങ്കിൽ defined(FEAT_DIRECTX)
	എങ്കിൽ (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	അല്ലെങ്കിൽ
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    അല്ലെങ്കിൽ എങ്കിൽ ((enc_codepage > 0 && (പൂർണ്ണസംഖ്യ)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	എങ്കിൽ (unicodebuf != ശൂന്യ)
	{
	    എങ്കിൽ (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    അല്ലെങ്കിൽ
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(അക്ഷരം *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    എങ്കിൽ (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		എങ്കിൽ (unicodepdy != ശൂന്യ)
		{
		    പൂർണ്ണസംഖ്യ i;
		    പൂർണ്ണസംഖ്യ cw;

		    വേണ്ടി (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			എങ്കിൽ (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	എങ്കിൽ (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (അക്ഷരം *)text, len, padding);
	അല്ലെങ്കിൽ
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (അക്ഷരം *)text, len, padding);
    }

    // Underline
    എങ്കിൽ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	എങ്കിൽ (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_വരി(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    എങ്കിൽ (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_വരി(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    എങ്കിൽ (flags & DRAW_UNDERC)
    {
	പൂർണ്ണസംഖ്യ			x;
	പൂർണ്ണസംഖ്യ			offset;
	സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	വേണ്ടി (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    ശൂന്യം
gui_mch_flush(ശൂന്യം)
{
#എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    സ്ഥിതം ശൂന്യം
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ശൂന്യ, gui.back_pixel);
}


    ശൂന്യം
gui_mch_get_screen_dimensions(പൂർണ്ണസംഖ്യ *screen_w, പൂർണ്ണസംഖ്യ *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.വലത് - workarea_rect.ഇടത്
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.മുകൾ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#എങ്കിൽ defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    ശൂന്യം
gui_mch_add_menu(
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    എങ്കിൽ (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, ശൂന്യ);
	എങ്കിൽ (wn == ശൂന്യ)
	    തിരികെ;

	infow.cbSize = വലുപ്പം(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	തിരുകുക_Menu_Item_W((parent == ശൂന്യ)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    എങ്കിൽ (parent == ശൂന്യ)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    അല്ലെങ്കിൽ എങ്കിൽ (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    ശൂന്യം
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (ശൂന്യം)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (പൂർണ്ണസംഖ്യ)mp.x, (പൂർണ്ണസംഖ്യ)mp.y);
}

    ശൂന്യം
gui_make_popup(char_u *path_name, പൂർണ്ണസംഖ്യ mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    എങ്കിൽ (menu == ശൂന്യ)
	തിരികെ;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    എങ്കിൽ (mouse_pos)
    {
	പൂർണ്ണസംഖ്യ	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (curwin != ശൂന്യ)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    സന്ദേശം_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (പൂർണ്ണസംഖ്യ)p.x, (പൂർണ്ണസംഖ്യ)p.y);
}

# എങ്കിൽ defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    ശൂന്യം
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    എങ്കിൽ (menu != ശൂന്യ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    ശൂന്യം
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = ശൂന്യ;

# ifdef FEAT_TEAROFF
    എങ്കിൽ (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	തിരുകുക_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    അല്ലെങ്കിൽ
# endif
# ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	എങ്കിൽ (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	അല്ലെങ്കിൽ
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
    അല്ലെങ്കിൽ
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, ശൂന്യ);
	എങ്കിൽ (wn != ശൂന്യ)
	{
	    തിരുകുക_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	എങ്കിൽ (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    ശൂന്യം
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    എങ്കിൽ (menu->submenu_id == (HMENU)-1)
    {
	പൂർണ്ണസംഖ്യ iButton;

	iButton = (പൂർണ്ണസംഖ്യ)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    അല്ലെങ്കിൽ
# endif
    {
	എങ്കിൽ (menu->parent != ശൂന്യ
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != ശൂന്യ)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	അല്ലെങ്കിൽ
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	എങ്കിൽ (menu->submenu_id != ശൂന്യ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	എങ്കിൽ (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	എങ്കിൽ (menu->parent != ശൂന്യ
		&& menu->parent->children != ശൂന്യ
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
    സ്ഥിതം ശൂന്യം
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    പൂർണ്ണസംഖ്യ		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    എങ്കിൽ (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ഇടത് - rct.ഇടത്;
	y = (trect.മുകൾ -  rct.bottom  + roct.bottom);
    }
    അല്ലെങ്കിൽ
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    എങ്കിൽ (menu->children != ശൂന്യ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	എങ്കിൽ (IsWindow(menu->tearoff_handle))
	    (ശൂന്യം) SetWindowPos(menu->tearoff_handle,
				ശൂന്യ,
				(പൂർണ്ണസംഖ്യ)trect.ഇടത്,
				(പൂർണ്ണസംഖ്യ)trect.മുകൾ,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    ശൂന്യം
gui_mch_menu_grey(
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    എങ്കിൽ (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    അല്ലെങ്കിൽ
# endif
	(ശൂന്യം)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    എങ്കിൽ ((menu->parent != ശൂന്യ) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	എങ്കിൽ (menu->children == ശൂന്യ)
	    menuID = (WORD)(menu->id);
	അല്ലെങ്കിൽ
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	എങ്കിൽ (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#എങ്കിൽ defined(FEAT_GUI_DIALOG)
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
    സ്ഥിതം LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    എങ്കിൽ (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(ശൂന്യം)SetFocus(hwnd);
	എങ്കിൽ (dialog_default_button > IDCANCEL)
	    (ശൂന്യം)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	അല്ലെങ്കിൽ
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (ശൂന്യം)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	തിരികെ FALSE;
    }

    എങ്കിൽ (message == WM_COMMAND)
    {
	പൂർണ്ണസംഖ്യ	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	എങ്കിൽ (button >= DLG_NONBUTTON_CONTROL)
	    തിരികെ TRUE;

	// If the edit box exists, copy the string.
	എങ്കിൽ (s_textfield != ശൂന്യ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, ശൂന്യ);
	    എങ്കിൽ (p != ശൂന്യ)
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
	എങ്കിൽ (button == IDOK)
	{
	    എങ്കിൽ (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	അല്ലെങ്കിൽ
	    EndDialog(hwnd, button - IDCANCEL);
	തിരികെ TRUE;
    }

    എങ്കിൽ ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
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

സ്ഥിതം സ്ഥിരം അക്ഷരം *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    പൂർണ്ണസംഖ്യ
gui_mch_dialog(
    പൂർണ്ണസംഖ്യ		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    പൂർണ്ണസംഖ്യ		 dfltbutton,
    char_u	*textfield,
    പൂർണ്ണസംഖ്യ		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    പൂർണ്ണസംഖ്യ		അക്കം_Buttons;
    പൂർണ്ണസംഖ്യ		*buttonWidths, *buttonPositions;
    പൂർണ്ണസംഖ്യ		buttonYpos;
    പൂർണ്ണസംഖ്യ		nchar, i;
    DWORD	lStyle;
    പൂർണ്ണസംഖ്യ		dlgwidth = 0;
    പൂർണ്ണസംഖ്യ		dlgheight;
    പൂർണ്ണസംഖ്യ		editboxheight;
    പൂർണ്ണസംഖ്യ		horizWidth = 0;
    പൂർണ്ണസംഖ്യ		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    പൂർണ്ണസംഖ്യ		fontHeight;
    പൂർണ്ണസംഖ്യ		textWidth, minButtonWidth, messageWidth;
    പൂർണ്ണസംഖ്യ		maxDialogWidth;
    പൂർണ്ണസംഖ്യ		maxDialogHeight;
    പൂർണ്ണസംഖ്യ		scroll_flag = 0;
    പൂർണ്ണസംഖ്യ		vertical;
    പൂർണ്ണസംഖ്യ		dlgPaddingX;
    പൂർണ്ണസംഖ്യ		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    പൂർണ്ണസംഖ്യ		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		l;
    പൂർണ്ണസംഖ്യ		dlg_icon_width;
    പൂർണ്ണസംഖ്യ		dlg_icon_height;
    പൂർണ്ണസംഖ്യ		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    എങ്കിൽ (!(gui.in_use || gui.starting))
#  endif
	എങ്കിൽ (silent_mode)
	    തിരികെ dfltbutton;   // return default option
# endif

    എങ്കിൽ (s_hwnd == ശൂന്യ)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    അല്ലെങ്കിൽ
	dpi = pGetDpiForSystem();

    എങ്കിൽ ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    എങ്കിൽ (tbuffer == ശൂന്യ)
	തിരികെ -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    അക്കം_Buttons = 1;
    വേണ്ടി (i = 0; tbuffer[i] != '\0'; i++)
    {
	എങ്കിൽ (tbuffer[i] == DLG_BUTTON_SEP)
	    അക്കം_Buttons++;
    }
    എങ്കിൽ (dfltbutton >= അക്കം_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(പൂർണ്ണസംഖ്യ, അക്കം_Buttons);
    എങ്കിൽ (buttonWidths == ശൂന്യ)
	തിരികെ -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(പൂർണ്ണസംഖ്യ, അക്കം_Buttons);
    എങ്കിൽ (buttonPositions == ശൂന്യ)
	തിരികെ -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    എങ്കിൽ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (s_hwnd == ശൂന്യ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.വലത് - workarea_rect.ഇടത് - 100;
	എങ്കിൽ (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.മുകൾ - 150;
    }
    അല്ലെങ്കിൽ
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.വലത് - rect.ഇടത്
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	എങ്കിൽ (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.മുകൾ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	എങ്കിൽ (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 500);
    ചെയ്യുക
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = ശൂന്യ;
	വേണ്ടി (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    എങ്കിൽ (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    എങ്കിൽ (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		എങ്കിൽ (last_white != ശൂന്യ)
		{
		    // break the line just after a space
		    എങ്കിൽ (pend > last_white)
			ga.ga_len -= (പൂർണ്ണസംഖ്യ)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = ശൂന്യ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		തുടരുക;
	    }

	    ആകുമ്പോൾ (--l >= 0)
		ga_append(&ga, *pend++);
	}
	എങ്കിൽ (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } ആകുമ്പോൾ (*pend != NUL);

    എങ്കിൽ (ga.ga_data != ശൂന്യ)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    എങ്കിൽ (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ശൂന്യ && vim_strchr(p_go, GO_VERTICAL) != ശൂന്യ);
    എങ്കിൽ (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	ചെയ്യുക
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    എങ്കിൽ (pend == ശൂന്യ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (പൂർണ്ണസംഖ്യ)(pend - pstart));
	    എങ്കിൽ (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} ആകുമ്പോൾ (*pend != NUL);

	എങ്കിൽ (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	അല്ലെങ്കിൽ എങ്കിൽ (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    എങ്കിൽ (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	ചെയ്യുക
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    എങ്കിൽ (pend == ശൂന്യ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (പൂർണ്ണസംഖ്യ)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    എങ്കിൽ (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} ആകുമ്പോൾ (*pend != NUL);
    }

    എങ്കിൽ (dlgwidth < DLG_MIN_WIDTH)
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
    എങ്കിൽ (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * അക്കം_Buttons;
    അല്ലെങ്കിൽ
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    എങ്കിൽ (textfield != ശൂന്യ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    എങ്കിൽ (dlgheight > maxDialogHeight)
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
    എങ്കിൽ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (പൂർണ്ണസംഖ്യ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    അല്ലെങ്കിൽ
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    എങ്കിൽ (textfield != ശൂന്യ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    എങ്കിൽ (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    വേണ്ടി (i = 0; i < അക്കം_Buttons; i++)
    {
	// get end of this button.
	വേണ്ടി (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	എങ്കിൽ (*pend)
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
	എങ്കിൽ (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (അക്ഷരം *)pstart);
	}
	അല്ലെങ്കിൽ
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (അക്ഷരം *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += അക്കം_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_ഇടത്|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (അക്ഷരം *)message);

    // Edit box
    എങ്കിൽ (textfield != ശൂന്യ)
    {
	p = add_dialog_element(p, ES_ഇടത്|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (അക്ഷരം *)textfield);
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
    എങ്കിൽ (textfield != ശൂന്യ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    അല്ലെങ്കിൽ
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ശൂന്യ;
    }

    // show the dialog box modally and get a return value
    nchar = (പൂർണ്ണസംഖ്യ)DialogBoxIndirect(
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
    (ശൂന്യം)SetFocus(s_hwnd);

    തിരികെ nchar;
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
    സ്ഥിതം PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    സ്ഥിരം അക്ഷരം *caption)
{
    പൂർണ്ണസംഖ്യ nchar;

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

    തിരികെ p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    സ്ഥിതം LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    തിരികെ (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    പൂർണ്ണസംഖ്യ		nChar = 0;
    പൂർണ്ണസംഖ്യ		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    പൂർണ്ണസംഖ്യ		i;
    WCHAR	*wn;

    എങ്കിൽ (use_enc && enc_codepage >= 0 && (പൂർണ്ണസംഖ്യ)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, ശൂന്യ);
	എങ്കിൽ (wn != ശൂന്യ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (പൂർണ്ണസംഖ്യ)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    എങ്കിൽ (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    വേണ്ടി (i = 0; i < nChar; ++i)
	എങ്കിൽ (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    തിരികെ nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    സ്ഥിതം HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    വേണ്ടി ( ; menu != ശൂന്യ; menu = menu->next)
    {
	എങ്കിൽ (menu->modes == 0)	// this menu has just been deleted
	    തുടരുക;
	എങ്കിൽ (menu_is_separator(menu->dname))
	    തുടരുക;
	എങ്കിൽ ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    തിരികെ menu->submenu_id;
    }
    തിരികെ ശൂന്യ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    സ്ഥിതം LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    എങ്കിൽ (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	തിരികെ (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    എങ്കിൽ (message == WM_COMMAND)
    {
	എങ്കിൽ ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    എങ്കിൽ (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(ശൂന്യം)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (പൂർണ്ണസംഖ്യ)rect.വലത് - 8,
			 (പൂർണ്ണസംഖ്യ)mp.y,
			 (പൂർണ്ണസംഖ്യ)0,	    // reserved param
			 s_hwnd,
			 ശൂന്യ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	അല്ലെങ്കിൽ
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(ശൂന്യം)SetFocus(s_hwnd);
	തിരികെ TRUE;
    }
    എങ്കിൽ ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	തിരികെ TRUE;
    }

    // When moved around, give main window the focus back.
    എങ്കിൽ (message == WM_EXITSIZEMOVE)
	(ശൂന്യം)SetActiveWindow(s_hwnd);

    തിരികെ FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    സ്ഥിതം ശൂന്യം
get_dialog_font_metrics(ശൂന്യം)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    എങ്കിൽ (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    അല്ലെങ്കിൽ
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

#എങ്കിൽ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    സ്ഥിതം ശൂന്യം
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ		initX,
    പൂർണ്ണസംഖ്യ		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    പൂർണ്ണസംഖ്യ		template_len;
    പൂർണ്ണസംഖ്യ		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*മുകൾ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    പൂർണ്ണസംഖ്യ		col, spaceWidth, len;
    പൂർണ്ണസംഖ്യ		columnWidths[2];
    char_u	*label, *text;
    പൂർണ്ണസംഖ്യ		acLen = 0;
    പൂർണ്ണസംഖ്യ		nameLen;
    പൂർണ്ണസംഖ്യ		padding0, padding1, padding2 = 0;
    പൂർണ്ണസംഖ്യ		sepPadding=0;
    പൂർണ്ണസംഖ്യ		x;
    പൂർണ്ണസംഖ്യ		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    പൂർണ്ണസംഖ്യ		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    എങ്കിൽ (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	എങ്കിൽ (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, ശൂന്യ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	തിരികെ;
    }

    /*
     * Create a new tearoff.
     */
    എങ്കിൽ (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    എങ്കിൽ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    അല്ലെങ്കിൽ
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
    വേണ്ടി (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    എങ്കിൽ (text != ശൂന്യ && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (പൂർണ്ണസംഖ്യ)STRLEN(text));
		എങ്കിൽ (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    എങ്കിൽ (pmenu->children != ശൂന്യ)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    എങ്കിൽ (columnWidths[1] == 0)
    {
	// no accelerators
	എങ്കിൽ (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	അല്ലെങ്കിൽ
	    columnWidths[0] += spaceWidth;
    }
    അല്ലെങ്കിൽ
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    എങ്കിൽ (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (പൂർണ്ണസംഖ്യ)STRLEN(title));
    എങ്കിൽ (textWidth > dlgwidth)
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
    എങ്കിൽ (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    അല്ലെങ്കിൽ
	*p++ = PixelToDialogX(initX); // x
    എങ്കിൽ (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (പൂർണ്ണസംഖ്യ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    അല്ലെങ്കിൽ
	menu = menu->children;
    മുകൾ_menu = menu;
    വേണ്ടി ( ; menu != ശൂന്യ; menu = menu->next)
    {
	എങ്കിൽ (menu->modes == 0)	// this menu has just been deleted
	    തുടരുക;
	എങ്കിൽ (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    തുടരുക;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	എങ്കിൽ (((അക്ഷരം *)p - (അക്ഷരം *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    എങ്കിൽ (newp != ശൂന്യ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (അക്ഷരം *)p - (അക്ഷരം *)pdlgtemplate);
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
	len = nameLen = (പൂർണ്ണസംഖ്യ)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (പൂർണ്ണസംഖ്യ)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	എങ്കിൽ (menu->actext != ശൂന്യ)
	{
	    acLen = (പൂർണ്ണസംഖ്യ)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	അല്ലെങ്കിൽ
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	എങ്കിൽ (menu->children == ശൂന്യ)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	അല്ലെങ്കിൽ
	{
	    len += (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	എങ്കിൽ (label == ശൂന്യ)
	    നിർത്തുക;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	എങ്കിൽ (text == ശൂന്യ)
	    text = label + nameLen;	    // no actext, use whole name
	ആകുമ്പോൾ (padding0-- > 0)
	    *text++ = ' ';
	എങ്കിൽ (menu->actext != ശൂന്യ)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	ആകുമ്പോൾ (padding1-- > 0)
	    *text++ = ' ';
	എങ്കിൽ (menu->children != ശൂന്യ)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	അല്ലെങ്കിൽ
	{
	    ആകുമ്പോൾ (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_ഇടത്,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (അക്ഷരം *)label);
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
	    (LPARAM)മുകൾ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (ശൂന്യം)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    സ്ഥിതം ശൂന്യം
initialise_toolbar(ശൂന്യം)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ശൂന്യ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    വലുപ്പം(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ);

    update_toolbar_size();
}

    സ്ഥിതം ശൂന്യം
update_toolbar_size(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = വലുപ്പം(TBMETRICS);
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

    സ്ഥിതം LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_സന്ദേശം,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_സന്ദേശം, lParam);
    തിരികെ CallWindowProc(s_toolbar_wndproc, hwnd, u_സന്ദേശം, wParam, lParam);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_toolbar_bitmap(vimmenu_T *menu)
{
    പൂർണ്ണസംഖ്യ i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    എങ്കിൽ (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ശൂന്യ;

	എങ്കിൽ (menu->iconfile != ശൂന്യ)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			ശൂന്യ,
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
	എങ്കിൽ (hbitmap == ശൂന്യ
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ശൂന്യ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    ശൂന്യ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	എങ്കിൽ (hbitmap != ശൂന്യ)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = ശൂന്യ;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (പൂർണ്ണസംഖ്യ)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    എങ്കിൽ (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_എണ്ണം)
	i = menu->iconidx;

    തിരികെ i;
}
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
    സ്ഥിതം ശൂന്യം
initialise_tabline(ശൂന്യം)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ശൂന്യ, g_hinst, ശൂന്യ);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    സ്ഥിതം tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = ശൂന്യ;

    എങ്കിൽ (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	എങ്കിൽ (s_tabhwnd == hWnd)
	{
	    പൂർണ്ണസംഖ്യ idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    എങ്കിൽ (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    തിരികെ ptp;
}

സ്ഥിതം POINT	    s_pt = {0, 0};
സ്ഥിതം HCURSOR      s_hCursor = ശൂന്യ;

    സ്ഥിതം LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_സന്ദേശം,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    പൂർണ്ണസംഖ്യ		nCenter;
    പൂർണ്ണസംഖ്യ		idx0;
    പൂർണ്ണസംഖ്യ		idx1;

    HandleMouseHide(u_സന്ദേശം, lParam);

    തിരഞ്ഞെടുപ്പ് (u_സന്ദേശം)
    {
	അവസ്ഥ WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		നിർത്തുക;
	    }
	അവസ്ഥ WM_MOUSEMOVE:
	    എങ്കിൽ (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		എങ്കിൽ (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(ശൂന്യ, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    എങ്കിൽ (tp != ശൂന്യ)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.ഇടത് + (rect.വലത് - rect.ഇടത്) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			എങ്കിൽ ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			അല്ലെങ്കിൽ എങ്കിൽ ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    നിർത്തുക;
	അവസ്ഥ WM_LBUTTONUP:
	    {
		എങ്കിൽ (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		നിർത്തുക;
	    }
	അവസ്ഥ WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		എങ്കിൽ (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		നിർത്തുക;
	    }
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }

    തിരികെ CallWindowProc(s_tabline_wndproc, hwnd, u_സന്ദേശം, wParam, lParam);
}
#endif

#എങ്കിൽ defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    ശൂന്യം
gui_mch_set_foreground(ശൂന്യം)
{
    എങ്കിൽ (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#എങ്കിൽ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    സ്ഥിതം ശൂന്യം
dyn_imm_load(ശൂന്യം)
{
    hLibImm = vimLoadLib("imm32.dll");
    എങ്കിൽ (hLibImm == ശൂന്യ)
	തിരികെ;

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

    എങ്കിൽ (       pImmGetCompositionStringW == ശൂന്യ
	    || pImmGetContext == ശൂന്യ
	    || pImmAssociateContext == ശൂന്യ
	    || pImmReleaseContext == ശൂന്യ
	    || pImmGetOpenStatus == ശൂന്യ
	    || pImmSetOpenStatus == ശൂന്യ
	    || pImmGetCompositionFontW == ശൂന്യ
	    || pImmSetCompositionFontW == ശൂന്യ
	    || pImmSetCompositionWindow == ശൂന്യ
	    || pImmGetConversionStatus == ശൂന്യ
	    || pImmSetConversionStatus == ശൂന്യ)
    {
	FreeLibrary(hLibImm);
	hLibImm = ശൂന്യ;
	pImmGetContext = ശൂന്യ;
	തിരികെ;
    }

    തിരികെ;
}

#endif

#എങ്കിൽ defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

തരനിർവചനം ഘടന _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    ശൂന്യം
gui_mch_drawsign(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ typenr)
{
    signicon_t *sign;
    പൂർണ്ണസംഖ്യ x, y, w, h;

    എങ്കിൽ (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == ശൂന്യ)
	തിരികെ;

# എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    തിരഞ്ഞെടുപ്പ് (sign->uType)
    {
	അവസ്ഥ IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    നിർത്തുക;
	അവസ്ഥ IMAGE_ICON:
	അവസ്ഥ IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ശൂന്യ, DI_NORMAL);
	    നിർത്തുക;
# ifdef FEAT_XPM_W32
	അവസ്ഥ IMAGE_XPM:
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
	    നിർത്തുക;
# endif
    }
}

    സ്ഥിതം ശൂന്യം
close_signicon_image(signicon_t *sign)
{
    എങ്കിൽ (sign == ശൂന്യ)
	തിരികെ;

    തിരഞ്ഞെടുപ്പ് (sign->uType)
    {
	അവസ്ഥ IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    നിർത്തുക;
	അവസ്ഥ IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    നിർത്തുക;
	അവസ്ഥ IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    നിർത്തുക;
# ifdef FEAT_XPM_W32
	അവസ്ഥ IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    നിർത്തുക;
# endif
    }
}

    ശൂന്യം *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ശൂന്യ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    എങ്കിൽ (ext > signfile)
    {
	പൂർണ്ണസംഖ്യ do_load = 1;

	എങ്കിൽ (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	അല്ലെങ്കിൽ
	    do_load = 0;

	എങ്കിൽ (do_load)
	    sign.hImage = (HANDLE)LoadImage(ശൂന്യ, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	എങ്കിൽ (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((അക്ഷരം *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ശൂന്യ;
    എങ്കിൽ (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ശൂന്യ)
	*psign = sign;

    എങ്കിൽ (!psign)
    {
	എങ്കിൽ (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    തിരികെ (ശൂന്യം *)psign;

}

    ശൂന്യം
gui_mch_destroy_sign(ശൂന്യം *sign)
{
    എങ്കിൽ (sign == ശൂന്യ)
	തിരികെ;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#എങ്കിൽ defined(FEAT_BEVAL_GUI)

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

    സ്ഥിതം ശൂന്യം
make_tooltip(BalloonEval *beval, അക്ഷരം *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    എങ്കിൽ (pti == ശൂന്യ)
	തിരികെ;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ശൂന്യ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, ശൂന്യ, g_hinst, ശൂന്യ);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = വലുപ്പം(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, ശൂന്യ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    എങ്കിൽ (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.വലത്);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ഇടത് = pt.x - 3;
    pti->rect.മുകൾ = pt.y - 3;
    pti->rect.വലത് = pt.x + 3;
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

    സ്ഥിതം ശൂന്യം
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    സ്ഥിതം VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_സന്ദേശം UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    എങ്കിൽ (cur_beval == ശൂന്യ || cur_beval->showState == ShS_SHOWING || !p_beval)
	തിരികെ;

    GetCursorPos(&pt);
    എങ്കിൽ (WindowFromPoint(pt) != s_textArea)
	തിരികെ;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    എങ്കിൽ (!PtInRect(&rect, pt))
	തിരികെ;

    എങ്കിൽ (last_user_activity > 0
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

	എങ്കിൽ (cur_beval->സന്ദേശം_CB != ശൂന്യ)
	    (*cur_beval->സന്ദേശം_CB)(cur_beval, 0);
    }
}

    ശൂന്യം
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    ശൂന്യം
gui_mch_enable_beval_area(BalloonEval *beval)
{
    എങ്കിൽ (beval == ശൂന്യ)
	തിരികെ;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    ശൂന്യം
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->സന്ദേശം);
    beval->സന്ദേശം = mesg == ശൂന്യ ? ശൂന്യ : vim_strsave(mesg);
    എങ്കിൽ (beval->സന്ദേശം == ശൂന്യ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	തിരികെ;
    }

    എങ്കിൽ (beval->showState == ShS_SHOWING)
	തിരികെ;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    എങ്കിൽ (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (അക്ഷരം *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    ശൂന്യം	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    ശൂന്യം	(*mesgCB)(BalloonEval *, പൂർണ്ണസംഖ്യ),
    ശൂന്യം	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    എങ്കിൽ (mesg != ശൂന്യ && mesgCB != ശൂന്യ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	തിരികെ ശൂന്യ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    എങ്കിൽ (beval != ശൂന്യ)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->സന്ദേശം = mesg;
	beval->സന്ദേശം_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	എങ്കിൽ (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    തിരികെ beval;
}

    സ്ഥിതം ശൂന്യം
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    എങ്കിൽ (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	തിരികെ;

    എങ്കിൽ (cur_beval == ശൂന്യ)
	തിരികെ;

    തിരഞ്ഞെടുപ്പ് (pnmh->code)
    {
	അവസ്ഥ TTN_SHOW:
	    നിർത്തുക;
	അവസ്ഥ TTN_എടുക്കുക: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    നിർത്തുക;
	അവസ്ഥ TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    നിർത്തുക;
	അവസ്ഥ TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    നിർത്തുക;
    }
}

    സ്ഥിതം ശൂന്യം
track_user_activity(UINT u_സന്ദേശം)
{
    എങ്കിൽ ((u_സന്ദേശം >= WM_MOUSEFIRST && u_സന്ദേശം <= WM_MOUSELAST)
	    || (u_സന്ദേശം >= WM_KEYFIRST && u_സന്ദേശം <= WM_KEYLAST))
	last_user_activity = Get_Tick_എണ്ണം();
}

    ശൂന്യം
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#എങ്കിൽ defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    ശൂന്യം
netbeans_draw_multisign_indicator(പൂർണ്ണസംഖ്യ row)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ y;
    പൂർണ്ണസംഖ്യ x;

    എങ്കിൽ (!netbeans_active())
	തിരികെ;

    x = 0;
    y = TEXT_Y(row);

# എങ്കിൽ defined(FEAT_DIRECTX)
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    വേണ്ടി (i = 0; i < gui.char_height - 3; i++)
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

#എങ്കിൽ defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_w32_sendevent_mouse(dict_T *args)
{
    എങ്കിൽ (!dict_has_താക്കോൽ(args, "row") || !dict_has_താക്കോൽ(args, "col"))
	തിരികെ FALSE;

    // Note: "move" is optional, requires fewer arguments
    പൂർണ്ണസംഖ്യ move = (പൂർണ്ണസംഖ്യ)dict_get_bool(args, "move", FALSE);

    എങ്കിൽ (!move && (!dict_has_താക്കോൽ(args, "button")
	    || !dict_has_താക്കോൽ(args, "multiclick")
	    || !dict_has_താക്കോൽ(args, "modifiers")))
	തിരികെ FALSE;

    പൂർണ്ണസംഖ്യ row = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "row");
    പൂർണ്ണസംഖ്യ col = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "col");

    എങ്കിൽ (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	എങ്കിൽ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    പൂർണ്ണസംഖ്യ pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    പൂർണ്ണസംഖ്യ pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	അല്ലെങ്കിൽ
	    gui_mouse_moved(col, row);
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ button = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "button");
	പൂർണ്ണസംഖ്യ repeated_click = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "multiclick");
	int_u mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    തിരികെ TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    നിവേശം_2 inputs[1];
    നിവേശം_2 modkeys[3];
    SecureZeroMemory(inputs, വലുപ്പം(നിവേശം_2));
    SecureZeroMemory(modkeys, 3 * വലുപ്പം(നിവേശം_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    എങ്കിൽ (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_സംഖ്യ_def(args, "keycode", 0);
	എങ്കിൽ (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    തിരികെ FALSE;
	}

	BOOL is_Mod_താക്കോൽ = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	പൂർണ്ണസംഖ്യ mods = (പൂർണ്ണസംഖ്യ)dict_get_സംഖ്യ(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	എങ്കിൽ (mods && STRICMP(event, "keydown") == 0 && !is_Mod_താക്കോൽ)
	{
	    പൂർണ്ണസംഖ്യ n = 0;
	    എങ്കിൽ (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = നിവേശം_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    എങ്കിൽ (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = നിവേശം_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    എങ്കിൽ (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = നിവേശം_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    എങ്കിൽ (n)
	    {
		(ശൂന്യം)SetForegroundWindow(s_hwnd);
		Send_നിവേശം(n, modkeys, വലുപ്പം(നിവേശം_2));
	    }
	}

	inputs[0].type = നിവേശം_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	എങ്കിൽ (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    എങ്കിൽ (!is_Mod_താക്കോൽ)
		unwrapMods = TRUE;
	}

	(ശൂന്യം)SetForegroundWindow(s_hwnd);
	Send_നിവേശം(ARRAYSIZE(inputs), inputs, വലുപ്പം(നിവേശം_2));
	vim_free(event);

	എങ്കിൽ (unwrapMods)
	{
	    modkeys[0].type = നിവേശം_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = നിവേശം_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = നിവേശം_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (ശൂന്യം)SetForegroundWindow(s_hwnd);
	    Send_നിവേശം(3, modkeys, വലുപ്പം(നിവേശം_2));
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (event == ശൂന്യ)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    പൂർണ്ണസംഖ്യ handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    എങ്കിൽ (strategy)
    {
	എങ്കിൽ (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    എങ്കിൽ (!handled)
    {
	എങ്കിൽ (strategy == ശൂന്യ)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	അല്ലെങ്കിൽ
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	തിരികെ FALSE;
    }
    തിരികെ TRUE;
}


    പൂർണ്ണസംഖ്യ
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    എങ്കിൽ (STRICMP(event, "key") == 0)
	തിരികെ test_gui_w32_sendevent_keyboard(args);
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(event, "mouse") == 0)
	തിരികെ test_gui_w32_sendevent_mouse(args);
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP(event, "set_keycode_trans_strategy") == 0)
	തിരികെ test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_മൂല്യം_for_argument_str_str), "event", event);
	തിരികെ FALSE;
    }
}
#endif

#എങ്കിൽ defined(FEAT_IMAGE_GUI)

തരനിർവചനം ഘടന
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
    സ്ഥിതം ശൂന്യം
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, പൂർണ്ണസംഖ്യ iw, പൂർണ്ണസംഖ്യ ih, image_format_T fmt)
{
    പൂർണ്ണസംഖ്യ		y, x;
    പൂർണ്ണസംഖ്യ		stride = iw * 4;
    പൂർണ്ണസംഖ്യ		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    വേണ്ടി (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	വേണ്ടി (x = 0; x < iw; x++)
	{
	    എങ്കിൽ (fmt == IMAGE_FORMAT_RGBA)
	    {
		പൂർണ്ണസംഖ്യ a = s[3];

		എങ്കിൽ (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		അല്ലെങ്കിൽ എങ്കിൽ (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		അല്ലെങ്കിൽ
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    അല്ലെങ്കിൽ
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

    പൂർണ്ണസംഖ്യ
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    പൂർണ്ണസംഖ്യ		iw, ih;
    ശൂന്യം	*bits = ശൂന്യ;

    എങ്കിൽ (ctx == ശൂന്യ)
	തിരികെ FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = വലുപ്പം(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_എണ്ണം = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, ശൂന്യ, 0);
    എങ്കിൽ (hbm == ശൂന്യ || bits == ശൂന്യ)
    {
	എങ്കിൽ (hbm != ശൂന്യ)
	    DeleteObject(hbm);
	തിരികെ false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    എങ്കിൽ (mem_dc == ശൂന്യ)
    {
	DeleteObject(hbm);
	തിരികെ false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    തിരികെ OK;
}

    ശൂന്യം
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    പൂർണ്ണസംഖ്യ
image_placement_gui_init(image_placement_T *place UNUSED)
{
    തിരികെ OK;
}

    ശൂന്യം
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    ശൂന്യം
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    പൂർണ്ണസംഖ്യ		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    എങ്കിൽ (rects == ശൂന്യ || n_rects == 0)
	തിരികെ;

# എങ്കിൽ defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    എങ്കിൽ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	പൂർണ്ണസംഖ്യ		row, col;
	പൂർണ്ണസംഖ്യ		posx, posy;
	പൂർണ്ണസംഖ്യ		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    ശൂന്യം
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
