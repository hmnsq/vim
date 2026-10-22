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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_ಕೀಲಿ_OFF			0	// no dead key
#define DEAD_ಕೀಲಿ_SET_DEFAULT		1	// dead key pressed
#define DEAD_ಕೀಲಿ_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_ಕೀಲಿ_SKIP_ON_CHAR		3	// skip next _OnChar()

#ಯದಿ defined(FEAT_DIRECTX)
ಸ್ಥಿರ DWriteContext *s_dwc = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_directx_enabled = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ರಿಕ್ತ && enc_utf8)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ directx_enabled(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ directx_binddc(ಶೂನ್ಯ);
#endif

#ifdef FEAT_MENU
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gui_mswin_get_menu_height(ಪೂರ್ಣಾಙ್ಕ fix_window);
#ಅನ್ಯಥಾ
# define gui_mswin_get_menu_height(fix_window)	0
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ keycode_trans_strategy {
    ಶೂನ್ಯ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
    ಶೂನ್ಯ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
    ಶೂನ್ಯ (*ptr_process_message_usual_ಕೀಲಿ) (UINT /*vk*/, ಧ್ರುವ ಸಂದೇಶ_2* /*pmsg*/);
    ಪೂರ್ಣಾಙ್ಕ  (*ptr_get_active_modifiers)(ಶೂನ್ಯ);
    ಪೂರ್ಣಾಙ್ಕ  (*is_experimental)(ಶೂನ್ಯ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ಸ್ಥಿರ ಶೂನ್ಯ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ process_message_usual_ಕೀಲಿ_experimental(UINT /*vk*/, ಧ್ರುವ ಸಂದೇಶ_2* /*pmsg*/);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  get_active_modifiers_experimental(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  is_experimental_true(ಶೂನ್ಯ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_ಕೀಲಿ_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ಸ್ಥಿರ ಶೂನ್ಯ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಙ್ಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ process_message_usual_ಕೀಲಿ_classic(UINT /*vk*/, ಧ್ರುವ ಸಂದೇಶ_2* /*pmsg*/);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  get_active_modifiers_classic(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  is_experimental_false(ಶೂನ್ಯ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_ಕೀಲಿ_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = ರಿಕ್ತ;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_experimental_true(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_experimental_false(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ಸ್ಥಿರ ಶೂನ್ಯ keycode_trans_strategy_init(ಶೂನ್ಯ)
{
    ಧ್ರುವ ವರ್ಣ *strategy = ರಿಕ್ತ;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    ಯದಿ (strategy == ರಿಕ್ತ)
    {
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	ಪ್ರತ್ಯಾಗಮ;
    }

}

#ಯದಿ defined(FEAT_RENDER_OPTIONS)
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    ಪೂರ್ಣಾಙ್ಕ	    dx_enable = 0;
    ಪೂರ್ಣಾಙ್ಕ	    dx_flags = 0;
    ಭಿನ್ನ   dx_gamma = 0.0f;
    ಭಿನ್ನ   dx_contrast = 0.0f;
    ಭಿನ್ನ   dx_level = 0.0f;
    ಪೂರ್ಣಾಙ್ಕ	    dx_geom = 0;
    ಪೂರ್ಣಾಙ್ಕ	    dx_renmode = 0;
    ಪೂರ್ಣಾಙ್ಕ	    dx_taamode = 0;
    // parse string as rendering options.
    ಪ್ರತಿ (p = s; p != ರಿಕ್ತ && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  ಮೌಲ್ಯ[128];

	copy_option_part(&p, item, ಪರಿಮಾಣ(item), ",");
	ಯದಿ (p == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	q = &item[0];
	copy_option_part(&q, name, ಪರಿಮಾಣ(name), ":");
	ಯದಿ (q == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	copy_option_part(&q, ಮೌಲ್ಯ, ಪರಿಮಾಣ(ಮೌಲ್ಯ), ":");

	ಯದಿ (STRCMP(name, "type") == 0)
	{
	    ಯದಿ (STRCMP(ಮೌಲ್ಯ, "directx") == 0)
		dx_enable = 1;
	    ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (ಭಿನ್ನ)atof((ವರ್ಣ *)ಮೌಲ್ಯ);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (ಭಿನ್ನ)atof((ವರ್ಣ *)ಮೌಲ್ಯ);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (ಭಿನ್ನ)atof((ವರ್ಣ *)ಮೌಲ್ಯ);
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((ವರ್ಣ *)ಮೌಲ್ಯ);
	    ಯದಿ (dx_geom < 0 || dx_geom > 2)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((ವರ್ಣ *)ಮೌಲ್ಯ);
	    ಯದಿ (dx_renmode < 0 || dx_renmode > 6)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((ವರ್ಣ *)ಮೌಲ್ಯ);
	    ಯದಿ (dx_taamode < 0 || dx_taamode > 3)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    ಯದಿ (dx_enable)
    {
	ಯದಿ (!directx_enabled())
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	DWriteContext_SetRenderingParams(s_dwc, ರಿಕ್ತ);
	ಯದಿ (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    ಯದಿ (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    ಯದಿ (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    ಯದಿ (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    ಯದಿ (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    ಯದಿ (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    ಯದಿ (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ಪ್ರತ್ಯಾಗಮ OK;
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (ಪೂರ್ಣಾಙ್ಕ)(short)LOWORD(lParam), (ಪೂರ್ಣಾಙ್ಕ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (ಪೂರ್ಣಾಙ್ಕ)(short)LOWORD(lParam), (ಪೂರ್ಣಾಙ್ಕ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (ಪೂರ್ಣಾಙ್ಕ)(short)LOWORD(lParam), (ಪೂರ್ಣಾಙ್ಕ)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#ಅನ್ತರ್ಭಾವ "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#ಅನ್ತರ್ಭಾವ <shellapi.h>
#ಅನ್ತರ್ಭಾವ <commctrl.h>
#ಅನ್ತರ್ಭಾವ <windowsx.h>

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

ಸ್ಥಿರ ಶೂನ್ಯ _OnPaint(HWND hwnd);
ಸ್ಥಿರ ಶೂನ್ಯ fill_rect(ಧ್ರುವ RECT *rcp, HBRUSH hbr, COLORREF color);
ಸ್ಥಿರ ಶೂನ್ಯ clear_rect(RECT *rcp);

ಸ್ಥಿರ WORD		s_dlgfntheight;		// height of the dialog font
ಸ್ಥಿರ WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
ಸ್ಥಿರ HMENU		s_menuBar = ರಿಕ್ತ;
#endif
#ifdef FEAT_TEAROFF
ಸ್ಥಿರ ಶೂನ್ಯ rebuild_tearoff(vimmenu_T *menu);
ಸ್ಥಿರ HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		s_busy_processing = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
ಸ್ಥಿರ UINT		s_findrep_ಸಂದೇಶ = 0;
ಸ್ಥಿರ FINDREPLACEW	s_findrep_struct;
ಸ್ಥಿರ HWND		s_findrep_hwnd = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = ರಿಕ್ತ;
ಸ್ಥಿರ HDC		s_hdc = ರಿಕ್ತ;
ಸ್ಥಿರ HBRUSH		s_brush = ರಿಕ್ತ;

#ifdef FEAT_TOOLBAR
ಸ್ಥಿರ HWND		s_toolbarhwnd = ರಿಕ್ತ;
ಸ್ಥಿರ WNDPROC		s_toolbar_wndproc = ರಿಕ್ತ;
#endif

#ifdef FEAT_GUI_TABLINE
ಸ್ಥಿರ HWND		s_tabhwnd = ರಿಕ್ತ;
ಸ್ಥಿರ WNDPROC		s_tabline_wndproc = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		showing_tabline = 0;
#endif

ಸ್ಥಿರ WPARAM		s_wParam = 0;

ಸ್ಥಿರ HWND		s_textArea = ರಿಕ್ತ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ಸ್ಥಿರ bool		s_textarea_bg_only = false;

ಸ್ಥಿರ UINT		s_u_ಸಂದೇಶ = 0;

ಸ್ಥಿರ char_u		*s_textfield; // Used by dialogs to pass back strings

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
ಪ್ರಕಾರನಿರ್ದೇಶ HANDLE DPI_AWARENESS_CONTEXT;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ DPI_AWARENESS {
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		s_dpi = DEFAULT_DPI;
ಸ್ಥಿರ BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
ಸ್ಥಿರ DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
ಸ್ಥಿರ RECT		s_suggested_rect;

ಸ್ಥಿರ UINT (WINAPI *pGetDpiForSystem)(ಶೂನ್ಯ) = ರಿಕ್ತ;
ಸ್ಥಿರ UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (WINAPI *pGetSystemMetricsForDpi)(ಪೂರ್ಣಾಙ್ಕ, UINT) = ರಿಕ್ತ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ಸ್ಥಿರ DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = ರಿಕ್ತ;
#ifdef FEAT_MBYTE_IME
ಸ್ಥಿರ DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ರಿಕ್ತ;
#endif

ಸ್ಥಿರ HINSTANCE hLibDwm = ರಿಕ್ತ;
ಸ್ಥಿರ HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_dwm_load(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

ಸ್ಥಿರ HINSTANCE hUxThemeLib = ರಿಕ್ತ;
ಸ್ಥಿರ DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ (WINAPI *pFlushMenuThemes)(ಶೂನ್ಯ) = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ dyn_uxtheme_load(ಶೂನ್ಯ);
#endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ WINAPI
stubGetSystemMetricsForDpi(ಪೂರ್ಣಾಙ್ಕ nIndex, UINT dpi UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ GetSystemMetrics(nIndex);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
adjust_fontsize_by_dpi(ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪ್ರತ್ಯಾಗಮ size * s_dpi / (ಪೂರ್ಣಾಙ್ಕ)pGetDpiForSystem();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
adjust_by_system_dpi(ಪೂರ್ಣಾಙ್ಕ size)
{
    ಪ್ರತ್ಯಾಗಮ size * (ಪೂರ್ಣಾಙ್ಕ)pGetDpiForSystem() / DEFAULT_DPI;
}

#ಯದಿ defined(FEAT_DIRECTX)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
directx_enabled(ಶೂನ್ಯ)
{
    ಯದಿ (s_dwc != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಅನ್ಯಥಾ ಯದಿ (s_directx_load_attempted)
	ಪ್ರತ್ಯಾಗಮ 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ಪ್ರತ್ಯಾಗಮ s_dwc != ರಿಕ್ತ ? 1 : 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
directx_binddc(ಶೂನ್ಯ)
{
    ಯದಿ (s_textArea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ current_font_height;	    // this is in os_mswin.c

ಸ್ಥಿರ ಸಂರಚನಾ
{
    UINT    ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_ಎಡ,		'k', 'l'},
    {VK_ಬಲ,		'k', 'r'},

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
    {VK_ಸೇರಿಸು,		'k', 'I'},
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	s_getting_focus = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	s_x_pending;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	s_y_pending;
ಸ್ಥಿರ UINT	s_kFlags_pending;
ಸ್ಥಿರ UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	s_timed_out = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
ಸ್ಥಿರ UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ಸ್ಥಿರ ಶೂನ್ಯ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ಸ್ಥಿರ ಶೂನ್ಯ track_user_activity(UINT u_ಸಂದೇಶ);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ಸ್ಥಿರ LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ಸ್ಥಿರ LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
ಸ್ಥಿರ LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#ಯದಿ defined(FEAT_BROWSE)
ಸ್ಥಿರ char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_windows_error(ಶೂನ್ಯ)
{
    LPVOID  lp_ಸಂದೇಶ_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  ರಿಕ್ತ, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_ಸಂದೇಶ_Buf, 0, ರಿಕ್ತ);
    TRACE1("Error: %s\n", lp_ಸಂದೇಶ_Buf);
    LocalFree(lp_ಸಂದೇಶ_Buf);
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u		blink_waittime = 700;
ಸ್ಥಿರ long_u		blink_ontime = 400;
ಸ್ಥಿರ long_u		blink_offtime = 250;
ಸ್ಥಿರ UINT_PTR		blink_timer = 0;

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ಸ್ಥಿರ VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_ಸಂದೇಶ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    ಸಂದೇಶ_2 ಸಂದೇಶ;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ರಿಕ್ತ, idEvent);

    // Eat spurious WM_TIMER messages
    ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    ಯದಿ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(ರಿಕ್ತ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    ಅನ್ಯಥಾ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(ರಿಕ್ತ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mswin_rm_blink_timer(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_2 ಸಂದೇಶ;

    ಯದಿ (blink_timer == 0)
	ಪ್ರತ್ಯಾಗಮ;

    KillTimer(ರಿಕ್ತ, blink_timer);
    // Eat spurious WM_TIMER messages
    ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಙ್ಕ may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    ಯದಿ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    ಶೂನ್ಯ
gui_mch_start_blink(ಶೂನ್ಯ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    ಯದಿ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(ರಿಕ್ತ, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ಸ್ಥಿರ VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_ಸಂದೇಶ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    ಸಂದೇಶ_2 ಸಂದೇಶ;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ರಿಕ್ತ, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    ಯದಿ (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_SET_DEFAULT;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
char_to_string(ಪೂರ್ಣಾಙ್ಕ ch, char_u *string, ಪೂರ್ಣಾಙ್ಕ slen, ಪೂರ್ಣಾಙ್ಕ had_alt)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    WCHAR	wstring[2];
    char_u	*ws = ರಿಕ್ತ;

    ಯದಿ (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    ಅನ್ಯಥಾ ಯದಿ (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    ಯದಿ (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ರಿಕ್ತ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	ಯದಿ (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ರಿಕ್ತ);
	    ಯದಿ (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    ಅನ್ಯಥಾ
    {
	ws = utf16_to_enc(wstring, &len);
	ಯದಿ (ws == ರಿಕ್ತ)
	    len = 0;
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    ಯದಿ (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    ಪ್ರತಿ (i = 0; i < len; ++i)
	ಯದಿ (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	    len += 2;
	}

    ಪ್ರತ್ಯಾಗಮ len;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_active_modifiers_experimental(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ modifiers = 0;

    ಯದಿ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    ಯದಿ (Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    ಯದಿ ((modifiers & MOD_MASK_CTRL) && (Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    ಯದಿ (!(Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	    &&  (Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_active_modifiers_classic(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ modifiers = 0;

    ಯದಿ (Get_ಕೀಲಿ_State(VK_SHIFT) & 0x8000)
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
    ಯದಿ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_active_modifiers(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ get_active_modifiers_experimental();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_active_modifiers_via_ptr(ಶೂನ್ಯ)
{
    // marshal to corresponding implementation
    ಪ್ರತ್ಯಾಗಮ keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    char_u	string[40];
    ಪೂರ್ಣಾಙ್ಕ		len = 0;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		ch = cch;   // special keys are negative

    ಯದಿ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SKIP_ON_CHAR)
	ಪ್ರತ್ಯಾಗಮ;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    ಯದಿ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_TRANSIENT_IN_ON_CHAR)
	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_ಕೀಲಿ(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_ಕೀಲಿಗಳು;
    ch = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ch);
    --no_reduce_ಕೀಲಿಗಳು;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ರಿಕ್ತ);
    ಯದಿ (ch == CSI)
	ch = K_CSI;

    ಯದಿ (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_ಒಳಹರಿವು_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    ಯದಿ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ಒಳಹರಿವು_buf();
	got_int = TRUE;
    }

    add_to_ಒಳಹರಿವು_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    char_u	string[40];
    ಪೂರ್ಣಾಙ್ಕ		len = 0;

    dead_ಕೀಲಿ = 0;

    len = char_to_string(ch, string, 40, FALSE);
    ಯದಿ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ಒಳಹರಿವು_buf();
	got_int = TRUE;
    }

    add_to_ಒಳಹರಿವು_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		ch = cch;   // special keys are negative

    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_ಕೀಲಿ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ರಿಕ್ತ);
    ಯದಿ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ಯದಿ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ಯದಿ (IS_SPECIAL((ಪೂರ್ಣಾಙ್ಕ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((ಪೂರ್ಣಾಙ್ಕ)ch);
	string[len++] = K_THIRD((ಪೂರ್ಣಾಙ್ಕ)ch);
    }
    ಅನ್ಯಥಾ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_ಒಳಹರಿವು_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    ಪೂರ್ಣಾಙ್ಕ cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		ch = cch;   // special keys are negative

    dead_ಕೀಲಿ = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_ಕೀಲಿ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ರಿಕ್ತ);
    ಯದಿ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ಯದಿ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ಯದಿ (IS_SPECIAL((ಪೂರ್ಣಾಙ್ಕ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((ಪೂರ್ಣಾಙ್ಕ)ch);
	string[len++] = K_THIRD((ಪೂರ್ಣಾಙ್ಕ)ch);
    }
    ಅನ್ಯಥಾ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_ಒಳಹರಿವು_buf(string, len);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseEvent(
    ಪೂರ್ಣಾಙ್ಕ button,
    ಪೂರ್ಣಾಙ್ಕ x,
    ಪೂರ್ಣಾಙ್ಕ y,
    ಪೂರ್ಣಾಙ್ಕ repeated_click,
    UINT ಕೀಲಿ_Flags)
{
    ಪೂರ್ಣಾಙ್ಕ vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    ಯದಿ (ಕೀಲಿ_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    ಯದಿ (ಕೀಲಿ_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    ಪೂರ್ಣಾಙ್ಕ x,
    ಪೂರ್ಣಾಙ್ಕ y,
    UINT ಕೀಲಿ_Flags)
{
    ಸ್ಥಿರ LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    ಪೂರ್ಣಾಙ್ಕ	    button = -1;
    ಪೂರ್ಣಾಙ್ಕ	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (ಶೂನ್ಯ)SetFocus(s_hwnd);

    ಯದಿ (s_u_ಸಂದೇಶ == WM_LBUTTONDOWN || s_u_ಸಂದೇಶ == WM_LBUTTONDBLCLK)
	button = MOUSE_ಎಡ;
    ಅನ್ಯಥಾ ಯದಿ (s_u_ಸಂದೇಶ == WM_MBUTTONDOWN || s_u_ಸಂದೇಶ == WM_MBUTTONDBLCLK)
	button = MOUSE_ಮಧ್ಯ;
    ಅನ್ಯಥಾ ಯದಿ (s_u_ಸಂದೇಶ == WM_RBUTTONDOWN || s_u_ಸಂದೇಶ == WM_RBUTTONDBLCLK)
	button = MOUSE_ಬಲ;
    ಅನ್ಯಥಾ ಯದಿ (s_u_ಸಂದೇಶ == WM_XBUTTONDOWN || s_u_ಸಂದೇಶ == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    ಅನ್ಯಥಾ ಯದಿ (s_u_ಸಂದೇಶ == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	ಯದಿ (s_button_pending == MOUSE_ಎಡ)
	    button = MOUSE_ಬಲ;
	ಅನ್ಯಥಾ
	    button = MOUSE_ಎಡ;
    }

    ಯದಿ (button < 0)
	ಪ್ರತ್ಯಾಗಮ;

    repeated_click = ((ಪೂರ್ಣಾಙ್ಕ)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    ಯದಿ (repeated_click
	    && ((button == MOUSE_ಎಡ && s_button_pending == MOUSE_ಬಲ)
		|| (button == MOUSE_ಬಲ
		    && s_button_pending == MOUSE_ಎಡ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_ಮಧ್ಯ;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, ಕೀಲಿ_Flags);
    }
    ಅನ್ಯಥಾ ಯದಿ ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ಬಲ)))
    {
	ಯದಿ (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, ಕೀಲಿ_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, ಕೀಲಿ_Flags);
    }
    ಅನ್ಯಥಾ
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
	s_kFlags_pending = ಕೀಲಿ_Flags;
    }

    s_prevTime = currentTime;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    ಪೂರ್ಣಾಙ್ಕ x,
    ಪೂರ್ಣಾಙ್ಕ y,
    UINT ಕೀಲಿ_Flags)
{
    ಪೂರ್ಣಾಙ್ಕ button;

    s_getting_focus = FALSE;
    ಯದಿ (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    ಯದಿ (s_u_ಸಂದೇಶ == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	ಯದಿ (!(ಕೀಲಿ_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    ಅನ್ಯಥಾ
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, ಕೀಲಿ_Flags);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    ಪೂರ್ಣಾಙ್ಕ cx UNUSED,
    ಪೂರ್ಣಾಙ್ಕ cy UNUSED)
{
#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ಸ್ಥಿರ vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    ಪೂರ್ಣಾಙ್ಕ		id)
{
    vimmenu_T	*pChildMenu;

    ಯಾವತ್ (pMenu)
    {
	ಯದಿ (pMenu->id == (UINT)id)
	    ಭಙ್ಗ;
	ಯದಿ (pMenu->children != ರಿಕ್ತ)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    ಯದಿ (pChildMenu)
	    {
		pMenu = pChildMenu;
		ಭಙ್ಗ;
	    }
	}
	pMenu = pMenu->next;
    }
    ಪ್ರತ್ಯಾಗಮ pMenu;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMenu(
    HWND	hwnd UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(ಬೇರು_menu, id);
    ಯದಿ (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnFindRepl(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ	    flags = 0;
    ಪೂರ್ಣಾಙ್ಕ	    down;

    ಯದಿ (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(ಶೂನ್ಯ)SetFocus(s_hwnd);

    ಯದಿ (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ಶೂನ್ಯ)SetFocus(s_hwnd);
    }
    ಅನ್ಯಥಾ ಯದಿ (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ಶೂನ್ಯ)SetFocus(s_hwnd);
    }
    ಅನ್ಯಥಾ ಯದಿ (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    ಯದಿ (flags == 0)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    ಯದಿ (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    ಯದಿ (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, ರಿಕ್ತ);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, ರಿಕ್ತ);
    ಯದಿ (p != ರಿಕ್ತ && q != ರಿಕ್ತ)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
HandleMouseHide(UINT u_ಸಂದೇಶ, LPARAM lParam)
{
    ಸ್ಥಿರ LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    ಯದಿ (u_ಸಂದೇಶ == WM_MOUSEMOVE || u_ಸಂದೇಶ == WM_NCMOUSEMOVE)
    {
	ಯದಿ (lParam == last_lParam)
	    ಪ್ರತ್ಯಾಗಮ;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    ವಿಕಲ್ಪ (u_ಸಂದೇಶ)
    {
    ಪಕ್ಷ WM_KEYUP:
    ಪಕ್ಷ WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	ಯದಿ (p_mh)
	    gui_mch_mousehide(TRUE);
	ಭಙ್ಗ;

    ಪಕ್ಷ WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    ಪಕ್ಷ WM_SYSCHAR:
    ಪಕ್ಷ WM_MOUSEMOVE:	 // show the pointer on any mouse action
    ಪಕ್ಷ WM_LBUTTONDOWN:
    ಪಕ್ಷ WM_LBUTTONUP:
    ಪಕ್ಷ WM_MBUTTONDOWN:
    ಪಕ್ಷ WM_MBUTTONUP:
    ಪಕ್ಷ WM_RBUTTONDOWN:
    ಪಕ್ಷ WM_RBUTTONUP:
    ಪಕ್ಷ WM_XBUTTONDOWN:
    ಪಕ್ಷ WM_XBUTTONUP:
    ಪಕ್ಷ WM_NCMOUSEMOVE:
    ಪಕ್ಷ WM_NCLBUTTONDOWN:
    ಪಕ್ಷ WM_NCLBUTTONUP:
    ಪಕ್ಷ WM_NCMBUTTONDOWN:
    ಪಕ್ಷ WM_NCMBUTTONUP:
    ಪಕ್ಷ WM_NCRBUTTONDOWN:
    ಪಕ್ಷ WM_NCRBUTTONUP:
    ಪಕ್ಷ WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_ಸಂದೇಶ,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_ಸಂದೇಶ, lParam);

    s_u_ಸಂದೇಶ = u_ಸಂದೇಶ;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_ಸಂದೇಶ);
#endif

    ವಿಕಲ್ಪ (u_ಸಂದೇಶ)
    {
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	ಪಕ್ಷ WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ಪ್ರತ್ಯಾಗಮ TRUE;
#endif
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_dwm_load(ಶೂನ್ಯ)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    ಯದಿ (hLibDwm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    ಯದಿ (pDwmSetWindowAttribute == ರಿಕ್ತ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
}

ಬಾಹ್ಯ DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    ಶೂನ್ಯ
gui_mch_set_titlebar_colors(ಶೂನ್ಯ)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    ಯದಿ (pDwmSetWindowAttribute == ರಿಕ್ತ || win_version < MAKE_VER(10, 0, 22000))
	ಪ್ರತ್ಯಾಗಮ;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    ಯದಿ (vim_strchr(p_go, GO_TITLEBAR) != ರಿಕ್ತ)
    {
	ಯದಿ (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	ಅನ್ಯಥಾ
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	ಯದಿ (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	ಯದಿ (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, ಪರಿಮಾಣ(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, ಪರಿಮಾಣ(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ರಿಕ್ತ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    ಯದಿ (s_textarea_bg_only && s_textArea != ರಿಕ್ತ)
	InvalidateRect(s_textArea, ರಿಕ್ತ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    ಶೂನ್ಯ
gui_mch_def_colors(ಶೂನ್ಯ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_open(ಶೂನ್ಯ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    ಯದಿ (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	ಯದಿ (s_textArea != ರಿಕ್ತ)
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

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.ಎಡ;
    *y = rect.ತುದಿ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    SetWindowPos(s_hwnd, ರಿಕ್ತ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    ಶೂನ್ಯ
gui_mch_set_text_area_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ oldx = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ oldy = 0;

    SetWindowPos(s_textArea, ರಿಕ್ತ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    ಯದಿ (vim_strchr(p_go, GO_TOOLBAR) != ರಿಕ್ತ)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#ಯದಿ defined(FEAT_GUI_TABLINE)
    ಯದಿ (showing_tabline)
    {
	ಪೂರ್ಣಾಙ್ಕ	ತುದಿ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	ಯದಿ (vim_strchr(p_go, GO_TOOLBAR) != ರಿಕ್ತ)
	    ತುದಿ = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, ತುದಿ, rect.ಬಲ, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    ಯದಿ (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ರಿಕ್ತ, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    ಪೂರ್ಣಾಙ್ಕ		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಙ್ಕ		x,
    ಪೂರ್ಣಾಙ್ಕ		y,
    ಪೂರ್ಣಾಙ್ಕ		w,
    ಪೂರ್ಣಾಙ್ಕ		h)
{
    SetWindowPos(sb->id, ರಿಕ್ತ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    RECT    rcTxt, rcWnd;
    ಪೂರ್ಣಾಙ್ಕ	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.ಬಲ - rcTxt.ಬಲ - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ಪ್ರತ್ಯಾಗಮ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    RECT    rcTxt, rcWnd;
    ಪೂರ್ಣಾಙ್ಕ	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ಪ್ರತ್ಯಾಗಮ (ypad < 0) ? 0 : ypad;
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಙ್ಕ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ರಿಕ್ತ,
	g_hinst, ರಿಕ್ತ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ಸ್ಥಿರ scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    ಯದಿ (gui.bottom_sbar.id == hwnd)
	ಪ್ರತ್ಯಾಗಮ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ಯದಿ (wp->w_scrollbars[SBAR_ಎಡ].id == hwnd)
	    ಪ್ರತ್ಯಾಗಮ &wp->w_scrollbars[SBAR_ಎಡ];
	ಯದಿ (wp->w_scrollbars[SBAR_ಬಲ].id == hwnd)
	    ಪ್ರತ್ಯಾಗಮ &wp->w_scrollbars[SBAR_ಬಲ];
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_scrollbar_size(ಶೂನ್ಯ)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
GetFontSize(GuiFont font, ಪೂರ್ಣಾಙ್ಕ *char_width, ಪೂರ್ಣಾಙ್ಕ *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    ಯದಿ (char_width)
	*char_width = size.cx + tm.tmOverhang;
    ಯದಿ (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    UpdateFontSize(gui.norm_font);
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ರಿಕ್ತ;

    // Load the font
    font = CreateFontIndirectW(lf);

    ಯದಿ (font == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ NOFONT;

    ಪ್ರತ್ಯಾಗಮ (GuiFont)font;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
pixels_to_points(ಪೂರ್ಣಾಙ್ಕ pixels, ಪೂರ್ಣಾಙ್ಕ vertical)
{
    ಪೂರ್ಣಾಙ್ಕ		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ಪ್ರತ್ಯಾಗಮ points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    ಯದಿ (get_logfont(&lf, name, ರಿಕ್ತ, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    ಯದಿ (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ಪ್ರತ್ಯಾಗಮ font;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strsave(name);
}
#endif

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಯದಿ (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ SysColorTable
    {
	ವರ್ಣ	    *name;
	ಪೂರ್ಣಾಙ್ಕ	    color;
    } SysColorTable;

    ಸ್ಥಿರ SysColorTable sys_table[] =
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
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(sys_table); i++)
	ಯದಿ (STRICMP(name, sys_table[i].name) == 0)
	    ಪ್ರತ್ಯಾಗಮ GetSysColor(sys_table[i].color);

    ಪ್ರತ್ಯಾಗಮ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].vim_code1 != NUL; i++)
	ಯದಿ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0
				       && name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ಶೂನ್ಯ
gui_mch_invert_rectangle(
    ಪೂರ್ಣಾಙ್ಕ	    r,
    ಪೂರ್ಣಾಙ್ಕ	    c,
    ಪೂರ್ಣಾಙ್ಕ	    nr,
    ಪೂರ್ಣಾಙ್ಕ	    nc)
{
    RECT    rc;

#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ = FILL_X(c);
    rc.ತುದಿ = FILL_Y(r);
    rc.ಬಲ = rc.ಎಡ + nc * gui.char_width;
    rc.bottom = rc.ತುದಿ + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ = FILL_X(gui.col);
    rc.ತುದಿ = FILL_Y(gui.row);
    rc.ಬಲ = rc.ಎಡ + gui.char_width;
    ಯದಿ (mb_lefthalve(gui.row, gui.col))
	rc.ಬಲ += gui.char_width;
    rc.bottom = rc.ತುದಿ + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ಶೂನ್ಯ
gui_mch_draw_part_cursor(
    ಪೂರ್ಣಾಙ್ಕ		w,
    ಪೂರ್ಣಾಙ್ಕ		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ಬಲ ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.ತುದಿ = FILL_Y(gui.row) + gui.char_height - h;
    rc.ಬಲ = rc.ಎಡ + w;
    rc.bottom = rc.ತುದಿ + h;

    fill_rect(&rc, ರಿಕ್ತ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
output_Dead_ಕೀಲಿ_re_Post_Ex(ಸಂದೇಶ_2 original_ಸಂದೇಶ, ಪೂರ್ಣಾಙ್ಕ dead_key2set)
{
    ಸ್ಥಿರ ಸಂದೇಶ_2 deadCharExpel;

    ಯದಿ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_OFF)
	ಪ್ರತ್ಯಾಗಮ;

    dead_ಕೀಲಿ = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_ಸಂದೇಶ.message;
    deadCharExpel.hwnd    = original_ಸಂದೇಶ.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_ಸಂದೇಶ.hwnd, original_ಸಂದೇಶ.message, original_ಸಂದೇಶ.wParam,
							  original_ಸಂದೇಶ.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ_2 original_ಸಂದೇಶ)
{
    output_Dead_ಕೀಲಿ_re_Post_Ex(original_ಸಂದೇಶ, DEAD_ಕೀಲಿ_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ಸ್ಥಿರ ಶೂನ್ಯ process_message_usual_ಕೀಲಿ(UINT vk, ಧ್ರುವ ಸಂದೇಶ_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_ಕೀಲಿ(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_message_usual_ಕೀಲಿ_experimental(UINT vk, ಧ್ರುವ ಸಂದೇಶ_2 *pmsg)
{
    WCHAR	ch[8];
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    ಯದಿ (Get_ಕೀಲಿ_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    ಯದಿ (Get_ಕೀಲಿ_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    ಯದಿ ((Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000)
				 && (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_ಕೀಲಿ(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    ಯದಿ (len < 0)
	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_SET_DEFAULT;

    ಯದಿ (len <= 0)
    {
	ಪೂರ್ಣಾಙ್ಕ wm_char = NUL;

	ಯದಿ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SET_DEFAULT
		&& (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000))
	{
	    ಯದಿ (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    ಯದಿ (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	ಯದಿ (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_TRANSIENT_IN_ON_CHAR;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Post the message as TranslateMessage would do.
    ಯದಿ (pmsg->message == WM_KEYDOWN)
    {
	ಪ್ರತಿ (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_message_usual_ಕೀಲಿ_classic(UINT vk, ಧ್ರುವ ಸಂದೇಶ_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    ಯದಿ (vk != 0xff
	    && (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_SHIFT) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	ಯದಿ (vk == '6' || Map_Virtual_ಕೀಲಿ(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_ಒಳಹರಿವು_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	ಅನ್ಯಥಾ ಯದಿ (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_ಒಳಹರಿವು_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	ಅನ್ಯಥಾ ಯದಿ (vk == '2' || Map_Virtual_ಕೀಲಿ(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_ಒಳಹರಿವು_buf(string, 1);
	}
	ಅನ್ಯಥಾ
	    TranslateMessage(pmsg);
    }
    ಅನ್ಯಥಾ
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_message(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_2		ಸಂದೇಶ;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		modifiers = 0;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
#ifdef FEAT_MENU
    ಸ್ಥಿರ char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    ಯದಿ (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&ಸಂದೇಶ, ರಿಕ್ತ, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    ಯದಿ (ಸಂದೇಶ.message == WM_OLE)
    {
	char_u *str = (char_u *)ಸಂದೇಶ.lParam;
	ಯದಿ (str == ರಿಕ್ತ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&ಸಂದೇಶ);
	}
	ಅನ್ಯಥಾ
	{
	    add_to_ಒಳಹರಿವು_buf(str, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    ಯದಿ (s_findrep_hwnd != ರಿಕ್ತ && IsDialogMessageW(s_findrep_hwnd, &ಸಂದೇಶ))
    {
	HandleMouseHide(ಸಂದೇಶ.message, ಸಂದೇಶ.lParam);
	ಪ್ರತ್ಯಾಗಮ;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    ಯದಿ (ಸಂದೇಶ.message == WM_KEYDOWN || ಸಂದೇಶ.message == WM_SYSKEYDOWN)
    {
	vk = (ಪೂರ್ಣಾಙ್ಕ) ಸಂದೇಶ.wParam;

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
	ಯದಿ (keycode_trans_strategy_used->is_experimental()
		&& dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SKIP_ON_CHAR)
	{
	    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
	}

	ಯದಿ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_OFF)
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
	    ಯದಿ (keycode_trans_strategy_used->is_experimental() &&
		    dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_ಕೀಲಿ_re_Post_Ex(ಸಂದೇಶ,
				       /*dead_key2set=*/DEAD_ಕೀಲಿ_SKIP_ON_CHAR);
		ಪ್ರತ್ಯಾಗಮ;
	    }

	    ಯದಿ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ಪ್ರತ್ಯಾಗಮ;
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
	    ಯದಿ ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
		TranslateMessage(&ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    ಅನ್ಯಥಾ ಯದಿ ((get_real_state()
			    & (MODE_ಸೇರಿಸು | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	}

	// Check for CTRL-BREAK
	ಯದಿ (vk == VK_CANCEL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_ಒಳಹರಿವು_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	ಯದಿ (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&ಸಂದೇಶ);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == vk
		    && (vk != VK_SPACE || !(Get_ಕೀಲಿ_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		ಯದಿ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SET_DEFAULT
			&& (special_ಕೀಲಿಗಳು[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ);
		    ಪ್ರತ್ಯಾಗಮ;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		ಯದಿ (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ರಿಕ್ತ, ರಿಕ್ತ) == ರಿಕ್ತ)
		    ಭಙ್ಗ;
#endif
		modifiers = get_active_modifiers_via_ptr();

		ಯದಿ (special_ಕೀಲಿಗಳು[i].vim_code1 == NUL)
		    ಕೀಲಿ = special_ಕೀಲಿಗಳು[i].vim_code0;
		ಅನ್ಯಥಾ
		    ಕೀಲಿ = TO_SPECIAL(special_ಕೀಲಿಗಳು[i].vim_code0,
						   special_ಕೀಲಿಗಳು[i].vim_code1);
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
		ಯದಿ (ಕೀಲಿ == CSI)
		    ಕೀಲಿ = K_CSI;

		ಯದಿ (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_ಒಳಹರಿವು_buf(string, 3);
		}

		ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ಕೀಲಿ);
		    string[2] = K_THIRD(ಕೀಲಿ);
		    add_to_ಒಳಹರಿವು_buf(string, 3);
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(ಕೀಲಿ, string, 40, FALSE);
		    add_to_ಒಳಹರಿವು_buf(string, len);
		}
		ಭಙ್ಗ;
	    }
	}

	// Not a special key.
	ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == 0)
	{
	    process_message_usual_ಕೀಲಿ(vk, &ಸಂದೇಶ);
	}
    }
#ifdef FEAT_MBYTE_IME
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ.message == WM_IME_NOTIFY)
	_OnImeNotify(ಸಂದೇಶ.hwnd, (DWORD)ಸಂದೇಶ.wParam, (DWORD)ಸಂದೇಶ.lParam);
    ಅನ್ಯಥಾ ಯದಿ (ಸಂದೇಶ.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&ಸಂದೇಶ);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    ಯದಿ (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ರಿಕ್ತ, ರಿಕ್ತ) == ರಿಕ್ತ)
#endif
	DispatchMessageW(&ಸಂದೇಶ);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_2	    ಸಂದೇಶ;

    ಯದಿ (!s_busy_processing)
	ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, ರಿಕ್ತ, 0, 0, PM_NOREMOVE)
						  && !vim_is_ಒಳಹರಿವು_buf_full())
	    process_message();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_any_timer(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_2		ಸಂದೇಶ;

    ಯದಿ (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(ರಿಕ್ತ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	ಯಾವತ್ (PeekMessageW(&ಸಂದೇಶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ಸ್ಥಿರ garray_T	server_pending_ga = {0, 0, 0, 0, ರಿಕ್ತ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    ಶೂನ್ಯ
server_add_ಒಳಹರಿವು(char_u *str)
{
    ಯದಿ (server_pending_ga.ga_data == ರಿಕ್ತ)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
server_flush_ಒಳಹರಿವು(ಶೂನ್ಯ)
{
    char_u  *str;

    ಯದಿ (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ಪ್ರತ್ಯಾಗಮ;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    ಯದಿ (str != ರಿಕ್ತ)
    {
	server_to_ಒಳಹರಿವು_buf(str);
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
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_wait_for_chars(long wtime)
{
    ಪೂರ್ಣಾಙ್ಕ		focus;

    s_timed_out = FALSE;

    ಯದಿ (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	ಯದಿ (s_busy_processing)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(ರಿಕ್ತ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    ಯಾವತ್ (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	ಯದಿ (gui.in_focus != focus)
	{
	    ಯದಿ (gui.in_focus)
		gui_mch_start_blink();
	    ಅನ್ಯಥಾ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	ಯದಿ (s_need_activate)
	{
	    (ಶೂನ್ಯ)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	ಪ್ರತಿ (;;)
	{
	    ಸಂದೇಶ_2 ಸಂದೇಶ;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ಒಳಹರಿವು();
# endif
# ifdef FEAT_TIMERS
	    ಯದಿ (did_add_timer)
		ಭಙ್ಗ;
# endif
	    ಯದಿ (PeekMessageW(&ಸಂದೇಶ, ರಿಕ್ತ, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		ಭಙ್ಗ;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ಒಳಹರಿವು_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || ಸಂದೇಶ_Wait_For_Multiple_Objects(0, ರಿಕ್ತ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		ಭಙ್ಗ;
	}
#ಅನ್ಯಥಾ
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	ಯದಿ (ಒಳಹರಿವು_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    ಯದಿ (!s_getting_focus)
		s_button_pending = -1;

	    ಪ್ರತ್ಯಾಗಮ OK;
	}

#ifdef FEAT_TIMERS
	ಯದಿ (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    ಭಙ್ಗ;
	}
#endif
    }
    allow_scrollbar = FALSE;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ಶೂನ್ಯ
gui_mch_clear_block(
    ಪೂರ್ಣಾಙ್ಕ		row1,
    ಪೂರ್ಣಾಙ್ಕ		col1,
    ಪೂರ್ಣಾಙ್ಕ		row2,
    ಪೂರ್ಣಾಙ್ಕ		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ = FILL_X(col1);
    rc.ತುದಿ = FILL_Y(row1);
    rc.ಬಲ = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    RECT    rc;

    rc.ಎಡ = 0;
    rc.ತುದಿ = 0;
    rc.ಬಲ = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಙ್ಕ flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : ರಿಕ್ತ);
#endif
}

    ಶೂನ್ಯ
gui_mch_set_menu_pos(
    ಪೂರ್ಣಾಙ್ಕ	    x UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    y UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    w UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    h UNUSED)
{
    // It will be in the right place anyway
}

#ಯದಿ defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ		hidden)
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
    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
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
    ಪ್ರತ್ಯಾಗಮ (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#ಯದಿ defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ಸ್ಥಿರ WORD
PixelToDialogX(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Pixels)
{
    ಪ್ರತ್ಯಾಗಮ (WORD)((ಅಂಕೆ_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ಸ್ಥಿರ WORD
PixelToDialogY(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Pixels)
{
    ಪ್ರತ್ಯಾಗಮ (WORD)((ಅಂಕೆ_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
GetTextWidth(HDC hdc, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    ಪ್ರತ್ಯಾಗಮ size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
GetTextWidthEnc(HDC hdc, char_u *str, ಪೂರ್ಣಾಙ್ಕ len)
{
    SIZE	size;
    WCHAR	*wstr;
    ಪೂರ್ಣಾಙ್ಕ		n;
    ಪೂರ್ಣಾಙ್ಕ		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    ಯದಿ (wstr == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    ಯದಿ (n)
	ಪ್ರತ್ಯಾಗಮ size.cx;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ಸ್ಥಿರ BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    ಪೂರ್ಣಾಙ್ಕ		    wChild, hChild, wParent, hParent;
    ಪೂರ್ಣಾಙ್ಕ		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.ಬಲ - rChild.ಎಡ;
    hChild = rChild.bottom - rChild.ತುದಿ;

    // If Vim is minimized put the window in the middle of the screen.
    ಯದಿ (hwndParent == ರಿಕ್ತ || IsMinimized(hwndParent))
	get_work_area(&rParent);
    ಅನ್ಯಥಾ
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.ಬಲ - rParent.ಎಡ;
    hParent = rParent.bottom - rParent.ತುದಿ;

    moninfo.cbSize = ಪರಿಮಾಣ(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    ಯದಿ (mon != ರಿಕ್ತ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    ಅನ್ಯಥಾ
    {
	hdc = GetDC(hwndChild);
	rScreen.ಎಡ = 0;
	rScreen.ತುದಿ = 0;
	rScreen.ಬಲ = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.ಎಡ + ((wParent - wChild) / 2);
    ಯದಿ (xNew < rScreen.ಎಡ)
	xNew = rScreen.ಎಡ;
    ಅನ್ಯಥಾ ಯದಿ ((xNew + wChild) > rScreen.ಬಲ)
	xNew = rScreen.ಬಲ - wChild;

    yNew = rParent.ತುದಿ + ((hParent - hChild) / 2);
    ಯದಿ (yNew < rScreen.ತುದಿ)
	yNew = rScreen.ತುದಿ;
    ಅನ್ಯಥಾ ಯದಿ ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    ಪ್ರತ್ಯಾಗಮ SetWindowPos(hwndChild, ರಿಕ್ತ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#ಯದಿ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (s_toolbarhwnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    ಅನ್ಯಥಾ
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_ಎಣಿಕೆ 31

#endif

#ಯದಿ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, ರಿಕ್ತ);
    ಯದಿ (wn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    infow.cbSize = ಪರಿಮಾಣ(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    ಸೇರಿಸು_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_tabline_popup_menu(ಶೂನ್ಯ)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    ಯದಿ (hold_gui_events || cmdwin_type != 0)
	ಪ್ರತ್ಯಾಗಮ;

    tab_pmenu = CreatePopupMenu();
    ಯದಿ (tab_pmenu == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (first_tabpage->tp_next != ರಿಕ್ತ)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ರಿಕ್ತ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    ಯದಿ (rval > 0)
    {
	TCHITTESTINFO htinfo;
	ಪೂರ್ಣಾಙ್ಕ idx;

	ಯದಿ (ScreenToClient(s_tabhwnd, &pt) == 0)
	    ಪ್ರತ್ಯಾಗಮ;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	ಯದಿ (idx == -1)
	    idx = 0;
	ಅನ್ಯಥಾ
	    idx += 1;

	send_tabline_menu_event(idx, (ಪೂರ್ಣಾಙ್ಕ)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (s_tabhwnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!showit != !showing_tabline)
    {
	ಯದಿ (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	ಅನ್ಯಥಾ
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ s_tabhwnd != ರಿಕ್ತ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    TCITEM	tie;
    ಪೂರ್ಣಾಙ್ಕ		nr = 0;
    ಪೂರ್ಣಾಙ್ಕ		curtabidx = 0;
    ಪೂರ್ಣಾಙ್ಕ		tabadded = 0;
    WCHAR	*wstr = ರಿಕ್ತ;

    ಯದಿ (s_tabhwnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = tp->tp_next, ++nr)
    {
	ಯದಿ (tp == curtab)
	    curtabidx = nr;

	ಯದಿ (nr >= Tab_Ctrl_Get_Item_ಎಣಿಕೆ(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_ಸೇರಿಸು_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, ರಿಕ್ತ);
	ಯದಿ (wstr != ರಿಕ್ತ)
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
    ಯಾವತ್ (nr < Tab_Ctrl_Get_Item_ಎಣಿಕೆ(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    ಯದಿ (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, ರಿಕ್ತ, ರಿಕ್ತ,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    ಯದಿ (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_set_curtab(ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಯದಿ (s_tabhwnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    ಶೂನ್ಯ
gui_mch_set_dark_theme(ಪೂರ್ಣಾಙ್ಕ dark)
{
    ಯದಿ (pDwmSetWindowAttribute != ರಿಕ್ತ && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		ಪರಿಮಾಣ(dark));

    ಯದಿ (pSetPreferredAppMode != ರಿಕ್ತ && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    ಯದಿ (pFlushMenuThemes != ರಿಕ್ತ && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_uxtheme_load(ಶೂನ್ಯ)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    ಯದಿ (hUxThemeLib == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (ಶೂನ್ಯ (WINAPI *)(ಶೂನ್ಯ))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    ಯದಿ (pSetPreferredAppMode == ರಿಕ್ತ || pFlushMenuThemes == ರಿಕ್ತ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    ಶೂನ್ಯ
gui_mch_set_fullscreen(ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಸ್ಥಿರ RECT normal_rect;
    ಸ್ಥಿರ LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    ಯದಿ (!full_screen) // Windows not set yet.
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (flag)
    {
	ಯದಿ (fullscreen_on)
	    ಪ್ರತ್ಯಾಗಮ;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = ಪರಿಮಾಣ(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	ಯದಿ (mon == ರಿಕ್ತ || !GetMonitorInfo(mon, &moninfo))
	    ಪ್ರತ್ಯಾಗಮ;

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
	SetWindowPos(s_hwnd, ರಿಕ್ತ,
		moninfo.rcMonitor.ಎಡ,
		moninfo.rcMonitor.ತುದಿ,
		moninfo.rcMonitor.ಬಲ - moninfo.rcMonitor.ಎಡ,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.ತುದಿ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!fullscreen_on)
	    ಪ್ರತ್ಯಾಗಮ;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, ರಿಕ್ತ,
		normal_rect.ಎಡ,
		normal_rect.ತುದಿ,
		normal_rect.ಬಲ - normal_rect.ಎಡ,
		normal_rect.bottom - normal_rect.ತುದಿ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    ಶೂನ್ಯ
ex_simalt(exarg_T *eap)
{
    char_u	*ಕೀಲಿಗಳು = eap->arg;
    ಪೂರ್ಣಾಙ್ಕ		fill_typebuf = FALSE;
    char_u	ಕೀಲಿ_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    ಯಾವತ್ (*ಕೀಲಿಗಳು)
    {
	ಯದಿ (*ಕೀಲಿಗಳು == '~')
	    *ಕೀಲಿಗಳು = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*ಕೀಲಿಗಳು, (LPARAM)0);
	ಕೀಲಿಗಳು++;
	fill_typebuf = TRUE;
    }
    ಯದಿ (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	ಕೀಲಿ_name[0] = K_SPECIAL;
	ಕೀಲಿ_name[1] = KS_EXTRA;
	ಕೀಲಿ_name[2] = KE_NOP;
	ಕೀಲಿ_name[3] = NUL;
#ಯದಿ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(ಶೂನ್ಯ)ins_typebuf(ಕೀಲಿ_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    ಸ್ಥಿರ ಶೂನ್ಯ
initialise_findrep(char_u *initial_string)
{
    ಪೂರ್ಣಾಙ್ಕ		wword = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    ಯದಿ (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    ಯದಿ (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    ಯದಿ (entry_text != ರಿಕ್ತ && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, ರಿಕ್ತ);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
set_window_title(HWND hwnd, ವರ್ಣ *title)
{
    ಯದಿ (title != ರಿಕ್ತ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, ರಿಕ್ತ);
	ಯದಿ (wbuf != ರಿಕ್ತ)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)SetWindowTextW(hwnd, ರಿಕ್ತ);
}

    ಶೂನ್ಯ
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    ಯದಿ (s_findrep_ಸಂದೇಶ != 0)
    {
	ಯದಿ (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	ಯದಿ (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(ಶೂನ್ಯ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    ಯದಿ (s_findrep_ಸಂದೇಶ != 0)
    {
	ಯದಿ (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	ಯದಿ (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(ಶೂನ್ಯ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಙ್ಕ hide)
{
    ಯದಿ (hide == gui.pointer_hidden)
	ಪ್ರತ್ಯಾಗಮ;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_show_popupmenu_at(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (ಶೂನ್ಯ)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(ಪೂರ್ಣಾಙ್ಕ)0,	    //reserved param
	s_hwnd,
	ರಿಕ್ತ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnEndSession(ಶೂನ್ಯ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDestroy(HWND hwnd)
{
    ಯದಿ (!destroying)
	_OnClose(hwnd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnPaint(
    HWND hwnd)
{
    ಯದಿ (IsMinimized(hwnd))
	ಪ್ರತ್ಯಾಗಮ;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (ಶೂನ್ಯ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    ಯದಿ (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ಎಡ = rect.ಎಡ;
	ps.rcPaint.ಬಲ = rect.ಬಲ;
    }

    ಯದಿ (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	ಯದಿ (!screen_cleared || Screen_ಸಾಲುಗಳು == ರಿಕ್ತ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	ಅನ್ಯಥಾ
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ಎಡ, ps.rcPaint.ತುದಿ,
		ps.rcPaint.ಬಲ - ps.rcPaint.ಎಡ + 1,
		ps.rcPaint.bottom - ps.rcPaint.ತುದಿ + 1);
    }

    EndPaint(hwnd, &ps);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    ಪೂರ್ಣಾಙ್ಕ cx,
    ಪೂರ್ಣಾಙ್ಕ cy)
{
    ಯದಿ (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (ಶೂನ್ಯ)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    ಯದಿ (destroying)
	ಪ್ರತ್ಯಾಗಮ;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (ಶೂನ್ಯ)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ಸ್ಥಿರ LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    RECT rct;
    POINT mp;

    (ಶೂನ್ಯ)GetWindowRect(s_textArea, &rct);
    (ಶೂನ್ಯ)GetCursorPos(&mp);
    *x = (ಪೂರ್ಣಾಙ್ಕ)(mp.x - rct.ಎಡ);
    *y = (ಪೂರ್ಣಾಙ್ಕ)(mp.y - rct.ತುದಿ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    RECT rct;

    (ಶೂನ್ಯ)GetWindowRect(s_textArea, &rct);
    (ಶೂನ್ಯ)SetCursorPos(x + gui.border_offset + rct.ಎಡ,
		       y + gui.border_offset + rct.ತುದಿ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mswin_get_valid_dimensions(
    ಪೂರ್ಣಾಙ್ಕ w,
    ಪೂರ್ಣಾಙ್ಕ h,
    ಪೂರ್ಣಾಙ್ಕ *valid_w,
    ಪೂರ್ಣಾಙ್ಕ *valid_h,
    ಪೂರ್ಣಾಙ್ಕ *cols,
    ಪೂರ್ಣಾಙ್ಕ *rows)
{
    ಪೂರ್ಣಾಙ್ಕ	    base_width, base_height;

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

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಙ್ಕ msec)
{
    RECT    rc;

#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ = 0;
    rc.ತುದಿ = 0;
    rc.ಬಲ = gui.ಅಂಕೆ_cols * gui.char_width;
    rc.bottom = gui.ಅಂಕೆ_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ಸ್ಥಿರ BOOL
is_point_onscreen(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    POINT   pt = {x, y};

    ಪ್ರತ್ಯಾಗಮ MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != ರಿಕ್ತ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ಸ್ಥಿರ BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ಎಡ;
    p1.y = rc.ತುದಿ;
    p2.x = rc.ಬಲ - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    ಯದಿ (!is_point_onscreen(p1.x, p1.y))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!is_point_onscreen(p1.x, p2.y))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!is_point_onscreen(p2.x, p1.y))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (!is_point_onscreen(p2.x, p2.y))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_scroll_flags(ಶೂನ್ಯ)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    ಯದಿ (!is_window_onscreen(s_hwnd))
	ಪ್ರತ್ಯಾಗಮ SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    ಪ್ರತಿ (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	ಯದಿ (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    ಯದಿ (IntersectRect(&rcDest, &rcVim, &rcOther))
		ಪ್ರತ್ಯಾಗಮ SW_INVALIDATE;
	}
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
intel_gpu_workaround(ಶೂನ್ಯ)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಙ್ಕ	    row,
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ_ಸಾಲುಗಳು)
{
    RECT	rc;

    rc.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    rc.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1);
    rc.ತುದಿ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height, &rc);
    }
    ಅನ್ಯಥಾ
#endif
    {
#ಯದಿ defined(FEAT_DIRECTX)
	ಯದಿ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height,
				    &rc, &rc, ರಿಕ್ತ, ರಿಕ್ತ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
						       gui.scroll_region_ಎಡ,
	gui.scroll_region_bot, gui.scroll_region_ಬಲ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_ಸಾಲುಗಳು)
{
    RECT	rc;

    rc.ಎಡ = FILL_X(gui.scroll_region_ಎಡ);
    rc.ಬಲ = FILL_X(gui.scroll_region_ಬಲ + 1);
    rc.ತುದಿ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height, &rc);
    }
    ಅನ್ಯಥಾ
#endif
    {
#ಯದಿ defined(FEAT_DIRECTX)
	ಯದಿ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_height,
				    &rc, &rc, ರಿಕ್ತ, ರಿಕ್ತ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_ಎಡ,
				row + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
}


    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಙ್ಕ rc UNUSED)
{
#ಯದಿ defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ರಿಕ್ತ;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (ಶೂನ್ಯ)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    ಯದಿ (s_hwnd != ರಿಕ್ತ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    ಸ್ಥಿರ char_u *
logfont2name(LOGFONTW lf)
{
    ವರ್ಣ	*charset_name;
    ವರ್ಣ	*quality_name;
    ವರ್ಣ	*font_name;
    size_t	res_size;
    ವರ್ಣ	*res;

    font_name = (ವರ್ಣ *)utf16_to_enc(lf.lfFaceName, ರಿಕ್ತ);
    ಯದಿ (font_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    charset_name = charset_id2name((ಪೂರ್ಣಾಙ್ಕ)lf.lfCharSet);
    quality_name = quality_id2name((ಪೂರ್ಣಾಙ್ಕ)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == ರಿಕ್ತ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ರಿಕ್ತ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    ಯದಿ (res != ರಿಕ್ತ)
    {
	ವರ್ಣ	*p;
	ಪೂರ್ಣಾಙ್ಕ	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	ಪ್ರತಿ (p = font_name; *p != NUL; ++p)
	{
	    ಯದಿ (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	ಯದಿ (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(ವರ್ಣ *)res, res_size, "%s:h%d", font_name, points);
	ಅನ್ಯಥಾ
	    res_len = vim_snprintf_safelen(
		(ವರ್ಣ *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (ವರ್ಣ *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	ಯದಿ (charset_name != ರಿಕ್ತ)
	    res_len += vim_snprintf_safelen((ವರ್ಣ *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	ಯದಿ (quality_name != ರಿಕ್ತ)
	    vim_snprintf((ವರ್ಣ *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ಪ್ರತ್ಯಾಗಮ (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_im_font(ಶೂನ್ಯ)
{
    LOGFONTW	lf_wide, lf;

    ಯದಿ (p_guifontwide != ರಿಕ್ತ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, ಪರಿಮಾಣ(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    ಅನ್ಯಥಾ
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    ಯದಿ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    ಶೂನ್ಯ
gui_mch_wide_font_changed(ಶೂನ್ಯ)
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

    ಯದಿ (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, ಪರಿಮಾಣ(lf), &lf))
    {
	ಯದಿ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	ಯದಿ (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    ಯದಿ (!lf.lfItalic)
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
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_font(char_u *font_name, ಪೂರ್ಣಾಙ್ಕ fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    ಯದಿ (get_logfont(&lf, font_name, ರಿಕ್ತ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    ಯದಿ (font == NOFONT)
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ಯದಿ defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	ಪೂರ್ಣಾಙ್ಕ		    feat_ಎಣಿಕೆ = 0;
	char_u		    *fp;

	ಯದಿ (font_name != ರಿಕ್ತ)
	{
	    // Find each ":f" option in font_name.
	    ಪ್ರತಿ (fp = font_name; *fp != NUL; fp++)
	    {
		ಯದಿ (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    ಪೂರ್ಣಾಙ್ಕ    ti = 0;
		    unsigned ಪೂರ್ಣಾಙ್ಕ param = 1;

		    fp += 2;  // skip ":f"
		    ಯಾವತ್ (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    ಯದಿ (ti != 4)
			ಅನುವರ್ತ;  // invalid tag length

		    ಯದಿ (*fp == '=')
		    {
			fp++;
			param = (unsigned ಪೂರ್ಣಾಙ್ಕ)atoi((ವರ್ಣ *)fp);
			ಯಾವತ್ (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    ಯದಿ (feat_ಎಣಿಕೆ < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_ಎಣಿಕೆ].tag =
			    ((unsigned ಪೂರ್ಣಾಙ್ಕ)tag[0])
			    | ((unsigned ಪೂರ್ಣಾಙ್ಕ)tag[1] << 8)
			    | ((unsigned ಪೂರ್ಣಾಙ್ಕ)tag[2] << 16)
			    | ((unsigned ಪೂರ್ಣಾಙ್ಕ)tag[3] << 24);
			features[feat_ಎಣಿಕೆ].parameter = param;
			feat_ಎಣಿಕೆ++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_ಎಣಿಕೆ);
    }
#endif

    ಯದಿ (font_name == ರಿಕ್ತ)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    ಯದಿ (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    ಯದಿ (p != ರಿಕ್ತ)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	ಯದಿ (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	ಅನ್ಯಥಾ
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    ಯದಿ (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    ಯದಿ (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	ಯದಿ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_maximized(ಶೂನ್ಯ)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = ಪರಿಮಾಣ(WINDOWPLACEMENT);
    ಯದಿ (GetWindowPlacement(s_hwnd, &wp))
    {
	ಯದಿ (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಯದಿ (wp.showCmd == SW_SHOWMINIMIZED)
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	ಯದಿ ((rc.ಬಲ - rc.ಎಡ !=
		    wp.rcNormalPosition.ಬಲ - wp.rcNormalPosition.ಎಡ)
		|| (rc.bottom - rc.ತುದಿ !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.ತುದಿ))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ಶೂನ್ಯ
gui_mch_newfont(ಶೂನ್ಯ)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    ಯದಿ (win_socket_id == 0)
    {
	gui_resize_shell(rect.ಬಲ - rect.ಎಡ
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.ತುದಿ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    ಅನ್ಯಥಾ
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ಬಲ - rect.ಎಡ,
	    rect.bottom - rect.ತುದಿ - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    ಶೂನ್ಯ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == ರಿಕ್ತ ? "VIM" : (ವರ್ಣ *)title));
}

#ಯದಿ defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ LPCSTR mshape_idcs[] =
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

    ಶೂನ್ಯ
mch_set_mouse_shape(ಪೂರ್ಣಾಙ್ಕ shape)
{
    LPCSTR idc;

    ಯದಿ (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	ಅನ್ಯಥಾ
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(ರಿಕ್ತ, idc));
	ಯದಿ (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (ಶೂನ್ಯ)GetCursorPos(&mp);
	    (ಶೂನ್ಯ)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#ಯದಿ defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ಸ್ಥಿರ WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    ಪೂರ್ಣಾಙ್ಕ len;
    WCHAR *res;

    tmp = convert_filter(s);
    ಯದಿ (tmp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    ಪ್ರತ್ಯಾಗಮ res;
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
	ಪೂರ್ಣಾಙ್ಕ saving,
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
    ಪೂರ್ಣಾಙ್ಕ			i;
    WCHAR		*titlep = ರಿಕ್ತ;
    WCHAR		*extp = ರಿಕ್ತ;
    WCHAR		*initdirp = ರಿಕ್ತ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    ಯದಿ (dflt == ರಿಕ್ತ)
	fileBuf[0] = NUL;
    ಅನ್ಯಥಾ
    {
	wp = enc_to_utf16(dflt, ರಿಕ್ತ);
	ಯದಿ (wp == ರಿಕ್ತ)
	    fileBuf[0] = NUL;
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# ಅನ್ಯಥಾ
    fileStruct.lStructSize = ಪರಿಮಾಣ(fileStruct);
# endif

    ಯದಿ (title != ರಿಕ್ತ)
	titlep = enc_to_utf16(title, ರಿಕ್ತ);
    fileStruct.lpstrTitle = titlep;

    ಯದಿ (ext != ರಿಕ್ತ)
	extp = enc_to_utf16(ext, ರಿಕ್ತ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    ಯದಿ (initdir != ರಿಕ್ತ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, ರಿಕ್ತ);
	ಯದಿ (initdirp != ರಿಕ್ತ)
	{
	    ಪ್ರತಿ (wp = initdirp; *wp != NUL; ++wp)
		ಯದಿ (*wp == '/')
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
    ಯದಿ (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    ಯದಿ (saving)
	ret = GetSaveFileNameW(&fileStruct);
    ಅನ್ಯಥಾ
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    ಯದಿ (!ret)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, ರಿಕ್ತ);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ಪ್ರತ್ಯಾಗಮ q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ಸ್ಥಿರ char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    ಯದಿ (res != ರಿಕ್ತ)
    {
	ಪ್ರತಿ (i = 0; i < s_len; ++i)
	    ಯದಿ (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    ಅನ್ಯಥಾ
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ಪ್ರತ್ಯಾಗಮ gui_mch_browse(0, title, (char_u *)_("Not Used"), ರಿಕ್ತ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    ವರ್ಣ    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ರಿಕ್ತ, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    ಯದಿ (fnames != ರಿಕ್ತ)
	ಪ್ರತಿ (i = 0; i < cFiles; ++i)
	{
	    ಯದಿ (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    ಯದಿ (fnames == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪೂರ್ಣಾಙ್ಕ kbd_modifiers = get_active_modifiers();

    ಯದಿ ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    ಯದಿ ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ಯದಿ ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    ಪೂರ್ಣಾಙ್ಕ pos)
{
    ಸ್ಥಿರ UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    ಪೂರ್ಣಾಙ್ಕ		dragging = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = ಪರಿಮಾಣ(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    ಯದಿ (sb == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (sb->wp != ರಿಕ್ತ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    ಅನ್ಯಥಾ	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->ಮೌಲ್ಯ;

    ವಿಕಲ್ಪ (code)
    {
	ಪಕ್ಷ SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    ಯದಿ (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_LINEDOWN:
	    val++;
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_LINEUP:
	    val--;
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_ತುದಿ:
	    val = 0;
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_BOTTOM:
	    val = sb_info->max;
	    ಭಙ್ಗ;
	ಪಕ್ಷ SB_ENDSCROLL:
	    ಯದಿ (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		ಯದಿ (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    ಭಙ್ಗ;

	ಸ್ವಭಾವ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ಪ್ರತ್ಯಾಗಮ 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    ಯದಿ (sb->wp != ರಿಕ್ತ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ಎಡ) ? SBAR_ಬಲ : SBAR_ಎಡ].id;

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

    ಪ್ರತ್ಯಾಗಮ 0;
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

ಸ್ಥಿರ BalloonEval  *cur_beval = ರಿಕ್ತ;
ಸ್ಥಿರ UINT_PTR	    beval_timer_id = 0;
ಸ್ಥಿರ DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
ಸ್ಥಿರ UINT	s_menu_id = 100;
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
ಸ್ಥಿರ PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	ಧ್ರುವ ವರ್ಣ *caption);
ಸ್ಥಿರ LPWORD lpwAlign(LPWORD);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#ಯದಿ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ಸ್ಥಿರ ಶೂನ್ಯ gui_mch_tearoff(char_u *title, vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ initX, ಪೂರ್ಣಾಙ್ಕ initY);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ get_dialog_font_metrics(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
ಸ್ಥಿರ ಶೂನ್ಯ initialise_toolbar(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_toolbar_size(ಶೂನ್ಯ);
ಸ್ಥಿರ LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_ಸಂದೇಶ, WPARAM wParam, LPARAM lParam);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_toolbar_bitmap(vimmenu_T *menu);
#ಅನ್ಯಥಾ
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
ಸ್ಥಿರ ಶೂನ್ಯ initialise_tabline(ಶೂನ್ಯ);
ಸ್ಥಿರ LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_ಸಂದೇಶ, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
ಸ್ಥಿರ LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ಸ್ಥಿರ char_u *GetResultStr(HWND hwnd, ಪೂರ್ಣಾಙ್ಕ GCS, ಪೂರ್ಣಾಙ್ಕ *lenp);
#endif
#ಯದಿ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
ಪ್ರಕಾರನಿರ್ದೇಶ HANDLE HIMC;
# endif

ಸ್ಥಿರ HINSTANCE hLibImm = ರಿಕ್ತ;
ಸ್ಥಿರ LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ಸ್ಥಿರ HIMC (WINAPI *pImmGetContext)(HWND);
ಸ್ಥಿರ HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ಸ್ಥಿರ BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ಸ್ಥಿರ BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
ಸ್ಥಿರ BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
ಸ್ಥಿರ BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ಸ್ಥಿರ BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
ಸ್ಥಿರ BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
ಸ್ಥಿರ BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
ಸ್ಥಿರ BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_imm_load(ಶೂನ್ಯ);
#ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_mswin_get_menu_height(
    ಪೂರ್ಣಾಙ್ಕ	    fix_window)	    // If TRUE, resize window if menu height changed
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	old_menu_height = -1;

    RECT    rc1, rc2;
    ಪೂರ್ಣಾಙ್ಕ	    ಅಂಕೆ;
    ಪೂರ್ಣಾಙ್ಕ	    menu_height;

    ಯದಿ (gui.menu_is_active)
	ಅಂಕೆ = Get_Menu_Item_ಎಣಿಕೆ(s_menuBar);
    ಅನ್ಯಥಾ
	ಅಂಕೆ = 0;

    ಯದಿ (ಅಂಕೆ == 0)
	menu_height = 0;
    ಅನ್ಯಥಾ ಯದಿ (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	ಯದಿ (gui.starting)
	    menu_height = rc1.bottom - rc1.ತುದಿ + 1;
	ಅನ್ಯಥಾ
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, ಅಂಕೆ - 1, &rc2);
	    menu_height = rc2.bottom - rc1.ತುದಿ + 1;
	}
    }

    ಯದಿ (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    ಪ್ರತ್ಯಾಗಮ menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ಸ್ಥಿರ long
mouse_vertical_scroll_step(ಶೂನ್ಯ)
{
    UINT val;
    ಯದಿ (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ಪ್ರತ್ಯಾಗಮ (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    ಪ್ರತ್ಯಾಗಮ 3; // Safe default;
}

    ಸ್ಥಿರ long
mouse_horizontal_scroll_step(ಶೂನ್ಯ)
{
    UINT val;
    ಯದಿ (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ಪ್ರತ್ಯಾಗಮ (long)val;
    ಪ್ರತ್ಯಾಗಮ 3; // Safe default;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_mouse_wheel(ಶೂನ್ಯ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, ಪೂರ್ಣಾಙ್ಕ horizontal)
{
    ಪೂರ್ಣಾಙ್ಕ		button;
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		modifiers = 0;
    ಪೂರ್ಣಾಙ್ಕ		kbd_modifiers;
    ಪೂರ್ಣಾಙ್ಕ		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    ಯದಿ (wp != ರಿಕ್ತ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	ಯದಿ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ಎಡ : MSCR_ಬಲ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ಅನ್ಯಥಾ
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

    ಯದಿ (wp == ರಿಕ್ತ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    ಯದಿ (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    ಅನ್ಯಥಾ
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    ಯದಿ ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    ಯದಿ ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ಯದಿ ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = ಪರಿಮಾಣ(NONCLIENTMETRICSW);
    ಯದಿ (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    ಪರಿಮಾಣ(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *lf = nm.lfMenuFont;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif


#ಯದಿ defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_tabline_font(ಶೂನ್ಯ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    ಯದಿ (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ಪ್ರತ್ಯಾಗಮ;

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
#ಅನ್ಯಥಾ
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    ಸ್ಥಿರ LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    ವಿಕಲ್ಪ (param)
    {
	ಪಕ್ಷ SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    ಭಙ್ಗ;
	ಪಕ್ಷ SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    ಭಙ್ಗ;
	ಪಕ್ಷ SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ifdef FEAT_NETBEANS_INTG
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnWindowPosChanged(
    HWND hwnd,
    ಧ್ರುವ LPWINDOWPOS lpwpos)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ x = 0, y = 0, cx = 0, cy = 0;
    ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ WSInitialized;

    ಯದಿ (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


ಸ್ಥಿರ HWND hwndTip = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
show_sizing_tip(ಪೂರ್ಣಾಙ್ಕ cols, ಪೂರ್ಣಾಙ್ಕ rows)
{
    TOOLINFOA ti;
    ವರ್ಣ buf[32];

    ti.cbSize = ಪರಿಮಾಣ(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    ಯದಿ (hwndTip == ರಿಕ್ತ)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, ರಿಕ್ತ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ರಿಕ್ತ, GetModuleHandle(ರಿಕ್ತ), ರಿಕ್ತ);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    ಅನ್ಯಥಾ
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
destroy_sizing_tip(ಶೂನ್ಯ)
{
    ಯದಿ (hwndTip == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    DestroyWindow(hwndTip);
    hwndTip = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    ಪೂರ್ಣಾಙ್ಕ	    w, h;
    ಪೂರ್ಣಾಙ್ಕ	    valid_w, valid_h;
    ಪೂರ್ಣಾಙ್ಕ	    w_offset, h_offset;
    ಪೂರ್ಣಾಙ್ಕ	    cols, rows;

    w = lprc->ಬಲ - lprc->ಎಡ;
    h = lprc->bottom - lprc->ತುದಿ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    ಯದಿ (fwSide == WMSZ_ಎಡ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ಎಡ += w_offset;
    ಅನ್ಯಥಾ ಯದಿ (fwSide == WMSZ_ಬಲ || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ಬಲ -= w_offset;

    ಯದಿ (fwSide == WMSZ_ತುದಿ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->ತುದಿ += h_offset;
    ಅನ್ಯಥಾ ಯದಿ (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ifdef FEAT_GUI_TABLINE
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnRButtonUp(HWND hwnd, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, UINT ಕೀಲಿ_Flags)
{
    ಯದಿ (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	ಯದಿ (pt.y < rect.ತುದಿ)
	{
	    show_tabline_popup_menu();
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, ಕೀಲಿ_Flags, DefWindowProcW);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, UINT ಕೀಲಿ_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    ಯದಿ (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	ಯದಿ (pt.y < rect.ತುದಿ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, ಕೀಲಿ_Flags, DefWindowProcW);
}
#endif

    ಸ್ಥಿರ UINT
_OnNCHitTest(HWND hwnd, ಪೂರ್ಣಾಙ್ಕ xPos, ಪೂರ್ಣಾಙ್ಕ yPos)
{
    UINT	result;
    ಪೂರ್ಣಾಙ್ಕ		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    ಯದಿ (result != HTCLIENT)
	ಪ್ರತ್ಯಾಗಮ result;

#ifdef FEAT_GUI_TABLINE
    ಯದಿ (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	ಯದಿ (yPos < rct.ತುದಿ)
	    ಪ್ರತ್ಯಾಗಮ result;
    }
#endif
    (ಶೂನ್ಯ)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    ಯದಿ (xPos < 48) // <VN> TODO should use system metric?
	ಪ್ರತ್ಯಾಗಮ HTBOTTOMLEFT;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ HTBOTTOMRIGHT;
}

#ಯದಿ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    ವಿಕಲ್ಪ (hdr->code)
    {
	ಪಕ್ಷ TTN_GETDISPINFOW:
	ಪಕ್ಷ TTN_GETDISPINFO:
	    {
		char_u		*str = ರಿಕ್ತ;
		ಸ್ಥಿರ ಶೂನ್ಯ	*tt_text = ರಿಕ್ತ;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		ಯದಿ (gui_mch_showing_tabline()
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
		    ಯದಿ (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			ಪೂರ್ಣಾಙ್ಕ idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			ಯದಿ (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    ಯದಿ (tp != ರಿಕ್ತ)
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
		ಅನ್ಯಥಾ
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(ಬೇರು_menu, idButton);
		    ಯದಿ (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		ಯದಿ (str == ರಿಕ್ತ)
		    ಭಙ್ಗ;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		ಯದಿ (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, ರಿಕ್ತ);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		ಅನ್ಯಥಾ
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    ಯದಿ (len < ಪರಿಮಾಣ(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == ರಿಕ್ತ))
			vim_strncpy((char_u *)lpdi->szText, str,
				ಪರಿಮಾಣ(lpdi->szText) - 1);
		    ಅನ್ಯಥಾ
			lpdi->lpszText = tt_text;
		}
	    }
	    ಭಙ್ಗ;

# ifdef FEAT_GUI_TABLINE
	ಪಕ್ಷ TCN_SELCHANGE:
	    ಯದಿ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ಪ್ರತ್ಯಾಗಮ 0L;
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ NM_RCLICK:
	    ಯದಿ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ಪ್ರತ್ಯಾಗಮ 0L;
	    }
	    ಭಙ್ಗ;
# endif

	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#ಯದಿ defined(MENUHINTS) && defined(FEAT_MENU)
    ಸ್ಥಿರ LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ಯದಿ (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  did_menu_tip = FALSE;

	ಯದಿ (did_menu_tip)
	{
	    ಸಂದೇಶ_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(ಬೇರು_menu, (UINT)LOWORD(wParam));
	ಯದಿ (pMenu != ರಿಕ್ತ && pMenu->strings[MENU_INDEX_TIP] != ರಿಕ್ತ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = ಪರಿಮಾಣ(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    ಯದಿ (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++ಸಂದೇಶ_hist_off;
		ಸಂದೇಶ((ವರ್ಣ *)pMenu->strings[MENU_INDEX_TIP]);
		--ಸಂದೇಶ_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	ಪ್ರತ್ಯಾಗಮ 0L;
    }
    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    ಸ್ಥಿರ BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    ಪೂರ್ಣಾಙ್ಕ		old_width, old_height;
    ಪೂರ್ಣಾಙ್ಕ		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, ಪರಿಮಾಣ(lf), &lf);
    lf.lfHeight = lf.lfHeight * (ಪೂರ್ಣಾಙ್ಕ)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    ಯದಿ (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    ಅನ್ಯಥಾ
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ LRESULT
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
    InvalidateRect(hwnd, ರಿಕ್ತ, TRUE);

    s_in_dpichanged = FALSE;
    ಪ್ರತ್ಯಾಗಮ 0L;
}


    ಸ್ಥಿರ LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_ಸಂದೇಶ,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_ಸಂದೇಶ, lParam);

    s_u_ಸಂದೇಶ = u_ಸಂದೇಶ;
    s_wParam = wParam;

    ವಿಕಲ್ಪ (u_ಸಂದೇಶ)
    {
	HANDLE_ಸಂದೇಶ(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_ಸಂದೇಶ(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_ಸಂದೇಶ(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_ಸಂದೇಶ(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_ಸಂದೇಶ(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_ಸಂದೇಶ(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_ಸಂದೇಶ(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_ಸಂದೇಶ(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_ಸಂದೇಶ(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_ಸಂದೇಶ(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_ಸಂದೇಶ(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_ಸಂದೇಶ(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_ಸಂದೇಶ(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_ಸಂದೇಶ(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    ಪಕ್ಷ WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ಪ್ರತ್ಯಾಗಮ FALSE;		// Do NOT allow system to go down.

    ಪಕ್ಷ WM_ENDSESSION:
	ಯದಿ (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    ಪ್ರತ್ಯಾಗಮ 0L;
	}
	ಭಙ್ಗ;

    ಪಕ್ಷ WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (ಪೂರ್ಣಾಙ್ಕ)(short)LOWORD(lParam));
	ಪ್ರತ್ಯಾಗಮ 0L;

    ಪಕ್ಷ WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	ಯದಿ (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((ಪೂರ್ಣಾಙ್ಕ)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (ಪೂರ್ಣಾಙ್ಕ)(short)LOWORD(lParam));
	    ಪ್ರತ್ಯಾಗಮ 0L;
	}
#ifdef FEAT_MENU
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
#endif

    ಪಕ್ಷ WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ 0L;
#endif

    ಪಕ್ಷ WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	ಭಙ್ಗ;

    ಪಕ್ಷ WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ಪ್ರತ್ಯಾಗಮ _DuringSizing((UINT)wParam, (LPRECT)lParam);

    ಪಕ್ಷ WM_MOUSEWHEEL:
    ಪಕ್ಷ WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_ಸಂದೇಶ == WM_MOUSEHWHEEL);
	ಪ್ರತ್ಯಾಗಮ 0L;

	// Notification for change in SystemParametersInfo()
    ಪಕ್ಷ WM_SETTINGCHANGE:
	ಪ್ರತ್ಯಾಗಮ _OnSettingChange((UINT)wParam);

#ಯದಿ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ಪಕ್ಷ WM_NOTIFY:
	ಪ್ರತ್ಯಾಗಮ _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#ಯದಿ defined(MENUHINTS) && defined(FEAT_MENU)
    ಪಕ್ಷ WM_MENUSELECT:
	ಪ್ರತ್ಯಾಗಮ _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    ಪಕ್ಷ WM_IME_NOTIFY:
	ಯದಿ (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
	ಪ್ರತ್ಯಾಗಮ 1L;

    ಪಕ್ಷ WM_IME_COMPOSITION:
	ಯದಿ (!_OnImeComposition(hwnd, wParam, lParam))
	    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
	ಪ್ರತ್ಯಾಗಮ 1L;
#endif
    ಪಕ್ಷ WM_GETDPISCALEDSIZE:
	ಪ್ರತ್ಯಾಗಮ _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    ಪಕ್ಷ WM_DPICHANGED:
	ಪ್ರತ್ಯಾಗಮ _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    ಸ್ವಭಾವ:
#ifdef MSWIN_FIND_REPLACE
	ಯದಿ (u_ಸಂದೇಶ == s_findrep_ಸಂದೇಶ && s_findrep_ಸಂದೇಶ != 0)
	    _OnFindRepl();
#endif
	ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ DefWindowProcW(hwnd, u_ಸಂದೇಶ, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = ರಿಕ್ತ;

    ಸ್ಥಿರ BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    ವರ್ಣ	buf[2048];
    ವರ್ಣ	*title = (ವರ್ಣ *)lParam;

    ಯದಿ (GetWindowText(hwnd, buf, ಪರಿಮಾಣ(buf)))
    {
	ಯದಿ (strstr(buf, title) != ರಿಕ್ತ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, ರಿಕ್ತ, "MDIClient", ರಿಕ್ತ);
	    ಯದಿ (vim_parent_hwnd != ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    ಶೂನ್ಯ
gui_mch_set_parent(ವರ್ಣ *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    ಯದಿ (vim_parent_hwnd == ರಿಕ್ತ)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    ಸ್ಥಿರ ಶೂನ್ಯ
ole_error(ವರ್ಣ *arg)
{
    ವರ್ಣ buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#ಯದಿ defined(GUI_MAY_SPAWN)
    ಸ್ಥಿರ ವರ್ಣ *
gvim_error(ಶೂನ್ಯ)
{
    ವರ್ಣ *ಸಂದೇಶ = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    ಯದಿ (starting)
    {
	mch_errmsg(ಸಂದೇಶ);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ;
}

    ವರ್ಣ *
gui_mch_do_spawn(char_u *arg)
{
    ಪೂರ್ಣಾಙ್ಕ			len;
# ಯದಿ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ರಿಕ್ತ;
    LPWSTR		tofree1 = ರಿಕ್ತ;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = ರಿಕ್ತ, p, warg, tofree2 = ರಿಕ್ತ;
    STARTUPINFOW	si = {ಪರಿಮಾಣ(si)};
    PROCESS_INFORMATION pi;

    ಯದಿ (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	ಗಚ್ಛ error;
    p = wcsrchr(name, L'\\');
    ಯದಿ (p == ರಿಕ್ತ)
	ಗಚ್ಛ error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# ಅನ್ಯಥಾ
    wcscpy(p + 1, L"gvim.exe");
# endif

# ಯದಿ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಯದಿ (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_ಸಾಲು_W();
	// Skip 1st argument.
	ಯಾವತ್ (*p && *p != L' ' && *p != L'\t')
	{
	    ಯದಿ (*p == L'"')
	    {
		// Skip quoted strings
		ಯಾವತ್ (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# ಯದಿ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಅನ್ಯಥಾ
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	ಪೂರ್ಣಾಙ್ಕ	ret;

	session = vim_tempname('s', FALSE);
	ಯದಿ (session == ರಿಕ್ತ)
	    ಗಚ್ಛ error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	ಯದಿ (p_bg == ರಿಕ್ತ)
	{
	    p_bg = savebg;
	    ಗಚ್ಛ error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	ಯದಿ (!ret)
	    ಗಚ್ಛ error;
	wsession = enc_to_utf16(session, ರಿಕ್ತ);
	ಯದಿ (wsession == ರಿಕ್ತ)
	    ಗಚ್ಛ error;
	len = (ಪೂರ್ಣಾಙ್ಕ)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	ಯದಿ (cmd == ರಿಕ್ತ)
	{
	    vim_free(wsession);
	    ಗಚ್ಛ error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    ಯದಿ (arg != ರಿಕ್ತ)
    {
	warg = enc_to_utf16(arg, ರಿಕ್ತ);
	ಯದಿ (warg == ರಿಕ್ತ)
	    ಗಚ್ಛ error;
	tofree2 = warg;
    }
    ಅನ್ಯಥಾ
	warg = L"";

    // Set up the new command line.
    len = (ಪೂರ್ಣಾಙ್ಕ)wcslen(name) + (ಪೂರ್ಣಾಙ್ಕ)wcslen(cmd) + (ಪೂರ್ಣಾಙ್ಕ)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    ಯದಿ (newcmd == ರಿಕ್ತ)
	ಗಚ್ಛ error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    ಯದಿ (!CreateProcessW(ರಿಕ್ತ, newcmd, ರಿಕ್ತ, ರಿಕ್ತ, TRUE, 0,
		ರಿಕ್ತ, ರಿಕ್ತ, &si, &pi))
	ಗಚ್ಛ error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# ಯದಿ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಯದಿ (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ಪ್ರತ್ಯಾಗಮ gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಙ್ಕ *argc, ವರ್ಣ **argv)
{
    ಪೂರ್ಣಾಙ್ಕ		silent = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    // Check for special OLE command line parameters
    ಯದಿ ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	ಯದಿ (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	ಅನ್ಯಥಾ
	    idx = 1;

	// Register Vim as an OLE Automation server
	ಯದಿ (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#ಅನ್ಯಥಾ
	    ಯದಿ (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	ಯದಿ (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#ಅನ್ಯಥಾ
	    ಯದಿ (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	ಯದಿ (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#ಅನ್ಯಥಾ
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    ಯದಿ (mch_is_gui_executable())
# endif
    {
	ಪೂರ್ಣಾಙ್ಕ	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	ಯದಿ (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	ಪೂರ್ಣಾಙ್ಕ arg;

	ಪ್ರತಿ (arg = 1; arg < *argc; arg++)
	    ಯದಿ (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ಪರಿಮಾಣ(ವರ್ಣ *));
		argv[*argc] = ರಿಕ್ತ;
		ಭಙ್ಗ;	// enough?
	    }
    }
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
load_dpi_func(ಶೂನ್ಯ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    ಯದಿ (hUser32 == ರಿಕ್ತ)
	ಗಚ್ಛ fail;

    pGetDpiForSystem = (UINT (WINAPI *)(ಶೂನ್ಯ))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (ಪೂರ್ಣಾಙ್ಕ (WINAPI *)(ಪೂರ್ಣಾಙ್ಕ, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    ಯದಿ (pSetThreadDpiAwarenessContext != ರಿಕ್ತ)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	ಯದಿ (oldctx != ರಿಕ್ತ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    ಯದಿ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = ರಿಕ್ತ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = ರಿಕ್ತ;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ರಿಕ್ತ;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init(ಶೂನ್ಯ)
{
    ಧ್ರುವ WCHAR szVimWndClassW[] = VIM_CLASSW;
    ಧ್ರುವ WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    ಯದಿ (s_hwnd != ರಿಕ್ತ)
	ಗಚ್ಛ theend;

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
    ಯದಿ (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(ರಿಕ್ತ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ರಿಕ್ತ;
	wndclassw.lpszClassName = szVimWndClassW;

	ಯದಿ (RegisterClassW(&wndclassw) == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (vim_parent_hwnd != ರಿಕ್ತ)
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
		vim_parent_hwnd, ರಿಕ್ತ,
		g_hinst, ರಿಕ್ತ);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	ಯದಿ (s_hwnd == ರಿಕ್ತ)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    ಅನ್ಯಥಾ
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	ಯದಿ (IsWindow((HWND)win_socket_id) <= 0)
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
		ರಿಕ್ತ, ರಿಕ್ತ,
		g_hinst, ರಿಕ್ತ);
	ಯದಿ (s_hwnd != ರಿಕ್ತ && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    ಯದಿ (s_hwnd == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (pGetDpiForWindow != ರಿಕ್ತ)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#ಯದಿ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    ಯದಿ (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ರಿಕ್ತ;
	wndclassw.hCursor = LoadCursor(ರಿಕ್ತ, IDC_ARROW);
	wndclassw.hbrBackground = ರಿಕ್ತ;
	wndclassw.lpszMenuName = ರಿಕ್ತ;
	wndclassw.lpszClassName = szTextAreaClassW;

	ಯದಿ (RegisterClassW(&wndclassw) == 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ರಿಕ್ತ,
	g_hinst, ರಿಕ್ತ);

    ಯದಿ (s_textArea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ರಿಕ್ತ;

	ಯದಿ (mch_icon_load(&hIcon) == OK && hIcon != ರಿಕ್ತ)
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
    s_findrep_ಸಂದೇಶ = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = ಪರಿಮಾಣ(s_findrep_struct);
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
    ಯದಿ (p_rop)
	(ಶೂನ್ಯ)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    ಯದಿ (mon != ರಿಕ್ತ)
    {
	moninfo.cbSize = ಪರಿಮಾಣ(MONITORINFO);
	ಯದಿ (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    ಶೂನ್ಯ
gui_mch_set_shellsize(
	ಪೂರ್ಣಾಙ್ಕ width,
	ಪೂರ್ಣಾಙ್ಕ height,
	ಪೂರ್ಣಾಙ್ಕ min_width UNUSED,
	ಪೂರ್ಣಾಙ್ಕ min_height UNUSED,
	ಪೂರ್ಣಾಙ್ಕ base_width UNUSED,
	ಪೂರ್ಣಾಙ್ಕ base_height UNUSED,
	ಪೂರ್ಣಾಙ್ಕ direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    ಪೂರ್ಣಾಙ್ಕ		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    ಯದಿ (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    ಯದಿ (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    ಅನ್ಯಥಾ
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
    window_rect.ಬಲ = window_rect.ಎಡ + win_width;
    window_rect.bottom = window_rect.ತುದಿ + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    ಯದಿ (!s_in_dpichanged)
    {
	ಯದಿ ((direction & RESIZE_HOR)
		&& window_rect.ಬಲ > workarea_rect.ಬಲ)
	    OffsetRect(&window_rect,
		    workarea_rect.ಬಲ - window_rect.ಬಲ, 0);

	ಯದಿ ((direction & RESIZE_HOR)
		&& window_rect.ಎಡ < workarea_rect.ಎಡ)
	    OffsetRect(&window_rect,
		    workarea_rect.ಎಡ - window_rect.ಎಡ, 0);

	ಯದಿ ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	ಯದಿ ((direction & RESIZE_VERT)
		&& window_rect.ತುದಿ < workarea_rect.ತುದಿ)
	    OffsetRect(&window_rect,
		    0, workarea_rect.ತುದಿ - window_rect.ತುದಿ);
    }

    MoveWindow(s_hwnd, window_rect.ಎಡ, window_rect.ತುದಿ,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    ಯಾವತ್ (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    ಯದಿ (sb->scroll_shift > 0)
	++size;

    info.cbSize = ಪರಿಮಾಣ(info);
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
    ಶೂನ್ಯ
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    ಶೂನ್ಯ
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    ಶೂನ್ಯ
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
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
    ಸ್ಥಿರ LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    ಯದಿ (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ಪ್ರತ್ಯಾಗಮ lResult;
    ವಿಕಲ್ಪ (dwCommand)
    {
	ಪಕ್ಷ IMN_SETOPENSTATUS:
	    ಯದಿ (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		ಯದಿ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		ಯದಿ (State & MODE_ಸೇರಿಸು)
		{
# ಯದಿ defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    ಯದಿ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			ಪೂರ್ಣಾಙ್ಕ old_row = gui.row;
			ಪೂರ್ಣಾಙ್ಕ old_col = gui.col;

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
	    ಭಙ್ಗ;
    }
    pImmReleaseContext(hWnd, hImc);
    ಪ್ರತ್ಯಾಗಮ lResult;
}

    ಸ್ಥಿರ LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    ಪೂರ್ಣಾಙ್ಕ		len;

    ಯದಿ ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ಪ್ರತ್ಯಾಗಮ 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    ಯದಿ (ret != ರಿಕ್ತ)
    {
	add_to_ಒಳಹರಿವು_buf_csi(ret, len);
	vim_free(ret);
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ಸ್ಥಿರ char_u *
GetResultStr(HWND hwnd, ಪೂರ್ಣಾಙ್ಕ GCS, ಪೂರ್ಣಾಙ್ಕ *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ರಿಕ್ತ;
    char_u	*convbuf = ರಿಕ್ತ;

    ಯದಿ (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ರಿಕ್ತ, 0);
    ಯದಿ (ret <= 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + ಪರಿಮಾಣ(WCHAR));
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / ಪರಿಮಾಣ(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ convbuf;
}
#endif

// For global functions we need prototypes.
#ಯದಿ defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    ಶೂನ್ಯ
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    ಯದಿ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    ಶೂನ್ಯ
im_set_position(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col)
{
    HIMC hImc;

    ಯದಿ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	ಯದಿ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    ಶೂನ್ಯ
im_set_active(ಪೂರ್ಣಾಙ್ಕ active)
{
    HIMC	hImc;
    ಸ್ಥಿರ HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    ಯದಿ (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

    ಯದಿ (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (p_imdisable)
    {
	ಯದಿ (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    ಯದಿ (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    ಯದಿ (!hImc)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    ಯದಿ (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ಸ್ಥಿರ DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ಸ್ಥಿರ BOOL bSaved = FALSE;

	ಯದಿ (active)
	{
	    // if we have a saved conversion status, restore it
	    ಯದಿ (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	ಅನ್ಯಥಾ
	{
	    // save conversion status and disable korean
	    ಯದಿ (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    ಪೂರ್ಣಾಙ್ಕ
im_get_status(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    ಯದಿ (!gui.in_use && !gui.starting)
	ಪ್ರತ್ಯಾಗಮ mbyte_im_get_status();
# endif

    ಯದಿ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ಪ್ರತ್ಯಾಗಮ status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
latin9_to_ucs(char_u *text, ಪೂರ್ಣಾಙ್ಕ len, WCHAR *unicodebuf)
{
    ಪೂರ್ಣಾಙ್ಕ		c;

    ಯಾವತ್ (--len >= 0)
    {
	c = *text++;
	ವಿಕಲ್ಪ (c)
	{
	    ಪಕ್ಷ 0xa4: c = 0x20ac; ಭಙ್ಗ;   // euro
	    ಪಕ್ಷ 0xa6: c = 0x0160; ಭಙ್ಗ;   // S hat
	    ಪಕ್ಷ 0xa8: c = 0x0161; ಭಙ್ಗ;   // S -hat
	    ಪಕ್ಷ 0xb4: c = 0x017d; ಭಙ್ಗ;   // Z hat
	    ಪಕ್ಷ 0xb8: c = 0x017e; ಭಙ್ಗ;   // Z -hat
	    ಪಕ್ಷ 0xbc: c = 0x0152; ಭಙ್ಗ;   // OE
	    ಪಕ್ಷ 0xbd: c = 0x0153; ಭಙ್ಗ;   // oe
	    ಪಕ್ಷ 0xbe: c = 0x0178; ಭಙ್ಗ;   // Y
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
    ಸ್ಥಿರ ಶೂನ್ಯ
RevOut( HDC hdc,
	ಪೂರ್ಣಾಙ್ಕ col,
	ಪೂರ್ಣಾಙ್ಕ row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    ಪೂರ್ಣಾಙ್ಕ		ix;

    ಪ್ರತಿ (ix = 0; ix < (ಪೂರ್ಣಾಙ್ಕ)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಸಾಲು(
    ಪೂರ್ಣಾಙ್ಕ		x1,
    ಪೂರ್ಣಾಙ್ಕ		y1,
    ಪೂರ್ಣಾಙ್ಕ		x2,
    ಪೂರ್ಣಾಙ್ಕ		y2,
    COLORREF	color)
{
#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_ಸಾಲು(s_dwc, x1, y1, x2, y2, color);
    ಅನ್ಯಥಾ
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, ರಿಕ್ತ);
	// Note: LineTo() excludes the last pixel in the line.
	ಸಾಲು_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_pixel(
    ಪೂರ್ಣಾಙ್ಕ		x,
    ಪೂರ್ಣಾಙ್ಕ		y,
    COLORREF	color)
{
#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    ಅನ್ಯಥಾ
#endif
	SetPixel(s_hdc, x, y, color);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fill_rect(
    ಧ್ರುವ RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    ಅನ್ಯಥಾ
#endif
    {
	HBRUSH	hbr2;

	ಯದಿ (hbr == ರಿಕ್ತ)
	    hbr2 = CreateSolidBrush(color);
	ಅನ್ಯಥಾ
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	ಯದಿ (hbr == ರಿಕ್ತ)
	    DeleteBrush(hbr2);
    }
}

    ಶೂನ್ಯ
gui_mch_draw_string(
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    char_u	*text,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	*padding = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	pad_size = 0;
    ಧ್ರುವ RECT	*pcliprect = ರಿಕ್ತ;
    UINT	foptions = 0;
    ಸ್ಥಿರ WCHAR *unicodebuf = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ   *unicodepdy = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	unibuflen = 0;
    ಪೂರ್ಣಾಙ್ಕ		n = 0;
    ಪೂರ್ಣಾಙ್ಕ		y;

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
    ಸ್ಥಿರ HBRUSH	hbr_cache[2] = {ರಿಕ್ತ, ರಿಕ್ತ};
    ಸ್ಥಿರ guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    ಯದಿ (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ಎಡ = FILL_X(col);
	rc.ತುದಿ = FILL_Y(row);
	ಯದಿ (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ಬಲ = FILL_X(col + mb_string2cells(text, len));
	}
	ಅನ್ಯಥಾ
	    rc.ಬಲ = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	ಯದಿ (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	ಅನ್ಯಥಾ ಯದಿ (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (hbr_cache[brush_lru] != ರಿಕ್ತ)
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
	ಯದಿ (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	ಅನ್ಯಥಾ ಯದಿ (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    ಯದಿ (pad_size != Columns || padding == ರಿಕ್ತ || padding[0] != gui.char_width)
    {
	ಪೂರ್ಣಾಙ್ಕ	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, pad_size);
	ಯದಿ (padding != ರಿಕ್ತ)
	    ಪ್ರತಿ (i = 0; i < pad_size; i++)
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
    ಯದಿ (enc_utf8)
	ಪ್ರತಿ (n = 0; n < len; ++n)
	    ಯದಿ (text[n] >= 0x80)
		ಭಙ್ಗ;

#ಯದಿ defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    ಯದಿ (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    ಯದಿ ((enc_utf8
		|| (enc_codepage > 0 && (ಪೂರ್ಣಾಙ್ಕ)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ರಿಕ್ತ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, len);

	unibuflen = len;
    }

    ಯದಿ (enc_utf8 && n < len && unicodebuf != ರಿಕ್ತ)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	ಪೂರ್ಣಾಙ್ಕ		i;
	ಪೂರ್ಣಾಙ್ಕ		wlen;	// string length in words
	ಪೂರ್ಣಾಙ್ಕ		cells;	// cell width of string up to composing char
	ಪೂರ್ಣಾಙ್ಕ		cw;	// width of current cell
	ಪೂರ್ಣಾಙ್ಕ		c;

	wlen = 0;
	cells = 0;
	ಪ್ರತಿ (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    ಯದಿ (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    ಅನ್ಯಥಾ
	    {
		unicodebuf[wlen++] = c;
	    }

	    ಯದಿ (utf_iscomposing(c))
		cw = 0;
	    ಅನ್ಯಥಾ
	    {
		cw = utf_char2cells(c);
		ಯದಿ (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    ಯದಿ (unicodepdy != ರಿಕ್ತ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ಯದಿ (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		ಅನ್ಯಥಾ
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#ಯದಿ defined(FEAT_DIRECTX)
	ಯದಿ (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	ಅನ್ಯಥಾ
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    ಅನ್ಯಥಾ ಯದಿ ((enc_codepage > 0 && (ಪೂರ್ಣಾಙ್ಕ)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	ಯದಿ (unicodebuf != ರಿಕ್ತ)
	{
	    ಯದಿ (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    ಅನ್ಯಥಾ
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(ವರ್ಣ *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    ಯದಿ (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ಯದಿ (unicodepdy != ರಿಕ್ತ)
		{
		    ಪೂರ್ಣಾಙ್ಕ i;
		    ಪೂರ್ಣಾಙ್ಕ cw;

		    ಪ್ರತಿ (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			ಯದಿ (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	ಯದಿ (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (ವರ್ಣ *)text, len, padding);
	ಅನ್ಯಥಾ
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (ವರ್ಣ *)text, len, padding);
    }

    // Underline
    ಯದಿ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	ಯದಿ (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_ಸಾಲು(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    ಯದಿ (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_ಸಾಲು(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    ಯದಿ (flags & DRAW_UNDERC)
    {
	ಪೂರ್ಣಾಙ್ಕ			x;
	ಪೂರ್ಣಾಙ್ಕ			offset;
	ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	ಪ್ರತಿ (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
#ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ರಿಕ್ತ, gui.back_pixel);
}


    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಙ್ಕ *screen_w, ಪೂರ್ಣಾಙ್ಕ *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ಬಲ - workarea_rect.ಎಡ
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.ತುದಿ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#ಯದಿ defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    ಶೂನ್ಯ
gui_mch_add_menu(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    ಯದಿ (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, ರಿಕ್ತ);
	ಯದಿ (wn == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	infow.cbSize = ಪರಿಮಾಣ(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ಸೇರಿಸು_Menu_Item_W((parent == ರಿಕ್ತ)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    ಯದಿ (parent == ರಿಕ್ತ)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    ಅನ್ಯಥಾ ಯದಿ (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (ಶೂನ್ಯ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (ಪೂರ್ಣಾಙ್ಕ)mp.x, (ಪೂರ್ಣಾಙ್ಕ)mp.y);
}

    ಶೂನ್ಯ
gui_make_popup(char_u *path_name, ಪೂರ್ಣಾಙ್ಕ mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    ಯದಿ (menu == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    ಯದಿ (mouse_pos)
    {
	ಪೂರ್ಣಾಙ್ಕ	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    ಅನ್ಯಥಾ ಯದಿ (curwin != ರಿಕ್ತ)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    ಸಂದೇಶ_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (ಪೂರ್ಣಾಙ್ಕ)p.x, (ಪೂರ್ಣಾಙ್ಕ)p.y);
}

# ಯದಿ defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    ಶೂನ್ಯ
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    ಯದಿ (menu != ರಿಕ್ತ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    ಶೂನ್ಯ
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = ರಿಕ್ತ;

# ifdef FEAT_TEAROFF
    ಯದಿ (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ಸೇರಿಸು_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    ಅನ್ಯಥಾ
# endif
# ifdef FEAT_TOOLBAR
    ಯದಿ (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	ಯದಿ (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	ಅನ್ಯಥಾ
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
    ಅನ್ಯಥಾ
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, ರಿಕ್ತ);
	ಯದಿ (wn != ರಿಕ್ತ)
	{
	    ಸೇರಿಸು_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	ಯದಿ (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ಯದಿ (menu->submenu_id == (HMENU)-1)
    {
	ಪೂರ್ಣಾಙ್ಕ iButton;

	iButton = (ಪೂರ್ಣಾಙ್ಕ)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    ಅನ್ಯಥಾ
# endif
    {
	ಯದಿ (menu->parent != ರಿಕ್ತ
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != ರಿಕ್ತ)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	ಅನ್ಯಥಾ
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	ಯದಿ (menu->submenu_id != ರಿಕ್ತ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	ಯದಿ (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	ಯದಿ (menu->parent != ರಿಕ್ತ
		&& menu->parent->children != ರಿಕ್ತ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    ಪೂರ್ಣಾಙ್ಕ		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    ಯದಿ (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ಎಡ - rct.ಎಡ;
	y = (trect.ತುದಿ -  rct.bottom  + roct.bottom);
    }
    ಅನ್ಯಥಾ
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    ಯದಿ (menu->children != ರಿಕ್ತ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	ಯದಿ (IsWindow(menu->tearoff_handle))
	    (ಶೂನ್ಯ) SetWindowPos(menu->tearoff_handle,
				ರಿಕ್ತ,
				(ಪೂರ್ಣಾಙ್ಕ)trect.ಎಡ,
				(ಪೂರ್ಣಾಙ್ಕ)trect.ತುದಿ,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    ಶೂನ್ಯ
gui_mch_menu_grey(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ಯದಿ (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    ಅನ್ಯಥಾ
# endif
	(ಶೂನ್ಯ)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    ಯದಿ ((menu->parent != ರಿಕ್ತ) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	ಯದಿ (menu->children == ರಿಕ್ತ)
	    menuID = (WORD)(menu->id);
	ಅನ್ಯಥಾ
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	ಯದಿ (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#ಯದಿ defined(FEAT_GUI_DIALOG)
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
    ಸ್ಥಿರ LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    ಯದಿ (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(ಶೂನ್ಯ)SetFocus(hwnd);
	ಯದಿ (dialog_default_button > IDCANCEL)
	    (ಶೂನ್ಯ)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	ಅನ್ಯಥಾ
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (ಶೂನ್ಯ)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    ಯದಿ (message == WM_COMMAND)
    {
	ಪೂರ್ಣಾಙ್ಕ	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	ಯದಿ (button >= DLG_NONBUTTON_CONTROL)
	    ಪ್ರತ್ಯಾಗಮ TRUE;

	// If the edit box exists, copy the string.
	ಯದಿ (s_textfield != ರಿಕ್ತ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, ರಿಕ್ತ);
	    ಯದಿ (p != ರಿಕ್ತ)
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
	ಯದಿ (button == IDOK)
	{
	    ಯದಿ (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	ಅನ್ಯಥಾ
	    EndDialog(hwnd, button - IDCANCEL);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
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

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(
    ಪೂರ್ಣಾಙ್ಕ		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    ಪೂರ್ಣಾಙ್ಕ		 dfltbutton,
    char_u	*textfield,
    ಪೂರ್ಣಾಙ್ಕ		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_Buttons;
    ಪೂರ್ಣಾಙ್ಕ		*buttonWidths, *buttonPositions;
    ಪೂರ್ಣಾಙ್ಕ		buttonYpos;
    ಪೂರ್ಣಾಙ್ಕ		nchar, i;
    DWORD	lStyle;
    ಪೂರ್ಣಾಙ್ಕ		dlgwidth = 0;
    ಪೂರ್ಣಾಙ್ಕ		dlgheight;
    ಪೂರ್ಣಾಙ್ಕ		editboxheight;
    ಪೂರ್ಣಾಙ್ಕ		horizWidth = 0;
    ಪೂರ್ಣಾಙ್ಕ		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    ಪೂರ್ಣಾಙ್ಕ		fontHeight;
    ಪೂರ್ಣಾಙ್ಕ		textWidth, minButtonWidth, messageWidth;
    ಪೂರ್ಣಾಙ್ಕ		maxDialogWidth;
    ಪೂರ್ಣಾಙ್ಕ		maxDialogHeight;
    ಪೂರ್ಣಾಙ್ಕ		scroll_flag = 0;
    ಪೂರ್ಣಾಙ್ಕ		vertical;
    ಪೂರ್ಣಾಙ್ಕ		dlgPaddingX;
    ಪೂರ್ಣಾಙ್ಕ		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ಪೂರ್ಣಾಙ್ಕ		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		l;
    ಪೂರ್ಣಾಙ್ಕ		dlg_icon_width;
    ಪೂರ್ಣಾಙ್ಕ		dlg_icon_height;
    ಪೂರ್ಣಾಙ್ಕ		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    ಯದಿ (!(gui.in_use || gui.starting))
#  endif
	ಯದಿ (silent_mode)
	    ಪ್ರತ್ಯಾಗಮ dfltbutton;   // return default option
# endif

    ಯದಿ (s_hwnd == ರಿಕ್ತ)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    ಅನ್ಯಥಾ
	dpi = pGetDpiForSystem();

    ಯದಿ ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    ಯದಿ (tbuffer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    ಅಂಕೆ_Buttons = 1;
    ಪ್ರತಿ (i = 0; tbuffer[i] != '\0'; i++)
    {
	ಯದಿ (tbuffer[i] == DLG_BUTTON_SEP)
	    ಅಂಕೆ_Buttons++;
    }
    ಯದಿ (dfltbutton >= ಅಂಕೆ_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, ಅಂಕೆ_Buttons);
    ಯದಿ (buttonWidths == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, ಅಂಕೆ_Buttons);
    ಯದಿ (buttonPositions == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    ಯದಿ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (s_hwnd == ರಿಕ್ತ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.ಬಲ - workarea_rect.ಎಡ - 100;
	ಯದಿ (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.ತುದಿ - 150;
    }
    ಅನ್ಯಥಾ
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.ಬಲ - rect.ಎಡ
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	ಯದಿ (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.ತುದಿ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	ಯದಿ (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 500);
    ಕುರು
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = ರಿಕ್ತ;
	ಪ್ರತಿ (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    ಯದಿ (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    ಯದಿ (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		ಯದಿ (last_white != ರಿಕ್ತ)
		{
		    // break the line just after a space
		    ಯದಿ (pend > last_white)
			ga.ga_len -= (ಪೂರ್ಣಾಙ್ಕ)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = ರಿಕ್ತ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ಅನುವರ್ತ;
	    }

	    ಯಾವತ್ (--l >= 0)
		ga_append(&ga, *pend++);
	}
	ಯದಿ (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } ಯಾವತ್ (*pend != NUL);

    ಯದಿ (ga.ga_data != ರಿಕ್ತ)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    ಯದಿ (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ರಿಕ್ತ && vim_strchr(p_go, GO_VERTICAL) != ರಿಕ್ತ);
    ಯದಿ (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	ಕುರು
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ಯದಿ (pend == ರಿಕ್ತ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (ಪೂರ್ಣಾಙ್ಕ)(pend - pstart));
	    ಯದಿ (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} ಯಾವತ್ (*pend != NUL);

	ಯದಿ (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	ಅನ್ಯಥಾ ಯದಿ (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    ಯದಿ (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	ಕುರು
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ಯದಿ (pend == ರಿಕ್ತ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (ಪೂರ್ಣಾಙ್ಕ)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    ಯದಿ (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} ಯಾವತ್ (*pend != NUL);
    }

    ಯದಿ (dlgwidth < DLG_MIN_WIDTH)
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
    ಯದಿ (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * ಅಂಕೆ_Buttons;
    ಅನ್ಯಥಾ
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    ಯದಿ (textfield != ರಿಕ್ತ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    ಯದಿ (dlgheight > maxDialogHeight)
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
    ಯದಿ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ಪೂರ್ಣಾಙ್ಕ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ಅನ್ಯಥಾ
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    ಯದಿ (textfield != ರಿಕ್ತ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    ಯದಿ (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_Buttons; i++)
    {
	// get end of this button.
	ಪ್ರತಿ (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	ಯದಿ (*pend)
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
	ಯದಿ (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (ವರ್ಣ *)pstart);
	}
	ಅನ್ಯಥಾ
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (ವರ್ಣ *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += ಅಂಕೆ_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_ಎಡ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (ವರ್ಣ *)message);

    // Edit box
    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	p = add_dialog_element(p, ES_ಎಡ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (ವರ್ಣ *)textfield);
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
    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    ಅನ್ಯಥಾ
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ರಿಕ್ತ;
    }

    // show the dialog box modally and get a return value
    nchar = (ಪೂರ್ಣಾಙ್ಕ)DialogBoxIndirect(
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
    (ಶೂನ್ಯ)SetFocus(s_hwnd);

    ಪ್ರತ್ಯಾಗಮ nchar;
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
    ಸ್ಥಿರ PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    ಧ್ರುವ ವರ್ಣ *caption)
{
    ಪೂರ್ಣಾಙ್ಕ nchar;

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

    ಪ್ರತ್ಯಾಗಮ p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ಸ್ಥಿರ LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ಪ್ರತ್ಯಾಗಮ (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    ಪೂರ್ಣಾಙ್ಕ		nChar = 0;
    ಪೂರ್ಣಾಙ್ಕ		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    ಪೂರ್ಣಾಙ್ಕ		i;
    WCHAR	*wn;

    ಯದಿ (use_enc && enc_codepage >= 0 && (ಪೂರ್ಣಾಙ್ಕ)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, ರಿಕ್ತ);
	ಯದಿ (wn != ರಿಕ್ತ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (ಪೂರ್ಣಾಙ್ಕ)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    ಯದಿ (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    ಪ್ರತಿ (i = 0; i < nChar; ++i)
	ಯದಿ (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    ಪ್ರತ್ಯಾಗಮ nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ಸ್ಥಿರ HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    ಪ್ರತಿ ( ; menu != ರಿಕ್ತ; menu = menu->next)
    {
	ಯದಿ (menu->modes == 0)	// this menu has just been deleted
	    ಅನುವರ್ತ;
	ಯದಿ (menu_is_separator(menu->dname))
	    ಅನುವರ್ತ;
	ಯದಿ ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ಪ್ರತ್ಯಾಗಮ menu->submenu_id;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ಸ್ಥಿರ LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    ಯದಿ (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ಪ್ರತ್ಯಾಗಮ (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    ಯದಿ (message == WM_COMMAND)
    {
	ಯದಿ ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    ಯದಿ (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(ಶೂನ್ಯ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (ಪೂರ್ಣಾಙ್ಕ)rect.ಬಲ - 8,
			 (ಪೂರ್ಣಾಙ್ಕ)mp.y,
			 (ಪೂರ್ಣಾಙ್ಕ)0,	    // reserved param
			 s_hwnd,
			 ರಿಕ್ತ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	ಅನ್ಯಥಾ
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(ಶೂನ್ಯ)SetFocus(s_hwnd);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಯದಿ ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    // When moved around, give main window the focus back.
    ಯದಿ (message == WM_EXITSIZEMOVE)
	(ಶೂನ್ಯ)SetActiveWindow(s_hwnd);

    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_dialog_font_metrics(ಶೂನ್ಯ)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    ಯದಿ (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    ಅನ್ಯಥಾ
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

#ಯದಿ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ		initX,
    ಪೂರ್ಣಾಙ್ಕ		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    ಪೂರ್ಣಾಙ್ಕ		template_len;
    ಪೂರ್ಣಾಙ್ಕ		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*ತುದಿ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    ಪೂರ್ಣಾಙ್ಕ		col, spaceWidth, len;
    ಪೂರ್ಣಾಙ್ಕ		columnWidths[2];
    char_u	*label, *text;
    ಪೂರ್ಣಾಙ್ಕ		acLen = 0;
    ಪೂರ್ಣಾಙ್ಕ		nameLen;
    ಪೂರ್ಣಾಙ್ಕ		padding0, padding1, padding2 = 0;
    ಪೂರ್ಣಾಙ್ಕ		sepPadding=0;
    ಪೂರ್ಣಾಙ್ಕ		x;
    ಪೂರ್ಣಾಙ್ಕ		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ಪೂರ್ಣಾಙ್ಕ		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    ಯದಿ (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	ಯದಿ (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, ರಿಕ್ತ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    /*
     * Create a new tearoff.
     */
    ಯದಿ (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    ಯದಿ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ಅನ್ಯಥಾ
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
    ಪ್ರತಿ (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    ಯದಿ (text != ರಿಕ್ತ && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text));
		ಯದಿ (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    ಯದಿ (pmenu->children != ರಿಕ್ತ)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    ಯದಿ (columnWidths[1] == 0)
    {
	// no accelerators
	ಯದಿ (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	ಅನ್ಯಥಾ
	    columnWidths[0] += spaceWidth;
    }
    ಅನ್ಯಥಾ
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    ಯದಿ (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(title));
    ಯದಿ (textWidth > dlgwidth)
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
    ಯದಿ (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    ಅನ್ಯಥಾ
	*p++ = PixelToDialogX(initX); // x
    ಯದಿ (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    ಅನ್ಯಥಾ
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
    ಯದಿ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ಪೂರ್ಣಾಙ್ಕ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    ಅನ್ಯಥಾ
	menu = menu->children;
    ತುದಿ_menu = menu;
    ಪ್ರತಿ ( ; menu != ರಿಕ್ತ; menu = menu->next)
    {
	ಯದಿ (menu->modes == 0)	// this menu has just been deleted
	    ಅನುವರ್ತ;
	ಯದಿ (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ಅನುವರ್ತ;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	ಯದಿ (((ವರ್ಣ *)p - (ವರ್ಣ *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    ಯದಿ (newp != ರಿಕ್ತ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (ವರ್ಣ *)p - (ವರ್ಣ *)pdlgtemplate);
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
	len = nameLen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (ಪೂರ್ಣಾಙ್ಕ)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	ಯದಿ (menu->actext != ರಿಕ್ತ)
	{
	    acLen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	ಅನ್ಯಥಾ
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	ಯದಿ (menu->children == ರಿಕ್ತ)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	ಅನ್ಯಥಾ
	{
	    len += (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	ಯದಿ (label == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	ಯದಿ (text == ರಿಕ್ತ)
	    text = label + nameLen;	    // no actext, use whole name
	ಯಾವತ್ (padding0-- > 0)
	    *text++ = ' ';
	ಯದಿ (menu->actext != ರಿಕ್ತ)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	ಯಾವತ್ (padding1-- > 0)
	    *text++ = ' ';
	ಯದಿ (menu->children != ರಿಕ್ತ)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	ಅನ್ಯಥಾ
	{
	    ಯಾವತ್ (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_ಎಡ,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (ವರ್ಣ *)label);
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
	    (LPARAM)ತುದಿ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (ಶೂನ್ಯ)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#ಯದಿ defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
initialise_toolbar(ಶೂನ್ಯ)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ರಿಕ್ತ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    ಪರಿಮಾಣ(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ರಿಕ್ತ);

    update_toolbar_size();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_toolbar_size(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = ಪರಿಮಾಣ(TBMETRICS);
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

    ಸ್ಥಿರ LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_ಸಂದೇಶ,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_ಸಂದೇಶ, lParam);
    ಪ್ರತ್ಯಾಗಮ CallWindowProc(s_toolbar_wndproc, hwnd, u_ಸಂದೇಶ, wParam, lParam);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_toolbar_bitmap(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಙ್ಕ i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    ಯದಿ (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ರಿಕ್ತ;

	ಯದಿ (menu->iconfile != ರಿಕ್ತ)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			ರಿಕ್ತ,
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
	ಯದಿ (hbitmap == ರಿಕ್ತ
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ರಿಕ್ತ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    ರಿಕ್ತ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	ಯದಿ (hbitmap != ರಿಕ್ತ)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = ರಿಕ್ತ;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (ಪೂರ್ಣಾಙ್ಕ)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    ಯದಿ (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_ಎಣಿಕೆ)
	i = menu->iconidx;

    ಪ್ರತ್ಯಾಗಮ i;
}
#endif

#ಯದಿ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
initialise_tabline(ಶೂನ್ಯ)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ರಿಕ್ತ, g_hinst, ರಿಕ್ತ);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ಸ್ಥಿರ tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = ರಿಕ್ತ;

    ಯದಿ (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	ಯದಿ (s_tabhwnd == hWnd)
	{
	    ಪೂರ್ಣಾಙ್ಕ idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    ಯದಿ (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    ಪ್ರತ್ಯಾಗಮ ptp;
}

ಸ್ಥಿರ POINT	    s_pt = {0, 0};
ಸ್ಥಿರ HCURSOR      s_hCursor = ರಿಕ್ತ;

    ಸ್ಥಿರ LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_ಸಂದೇಶ,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    ಪೂರ್ಣಾಙ್ಕ		nCenter;
    ಪೂರ್ಣಾಙ್ಕ		idx0;
    ಪೂರ್ಣಾಙ್ಕ		idx1;

    HandleMouseHide(u_ಸಂದೇಶ, lParam);

    ವಿಕಲ್ಪ (u_ಸಂದೇಶ)
    {
	ಪಕ್ಷ WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		ಭಙ್ಗ;
	    }
	ಪಕ್ಷ WM_MOUSEMOVE:
	    ಯದಿ (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		ಯದಿ (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(ರಿಕ್ತ, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    ಯದಿ (tp != ರಿಕ್ತ)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.ಎಡ + (rect.ಬಲ - rect.ಎಡ) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			ಯದಿ ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			ಅನ್ಯಥಾ ಯದಿ ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ WM_LBUTTONUP:
	    {
		ಯದಿ (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		ಭಙ್ಗ;
	    }
	ಪಕ್ಷ WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		ಯದಿ (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		ಭಙ್ಗ;
	    }
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ CallWindowProc(s_tabline_wndproc, hwnd, u_ಸಂದೇಶ, wParam, lParam);
}
#endif

#ಯದಿ defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_set_foreground(ಶೂನ್ಯ)
{
    ಯದಿ (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#ಯದಿ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_imm_load(ಶೂನ್ಯ)
{
    hLibImm = vimLoadLib("imm32.dll");
    ಯದಿ (hLibImm == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಯದಿ (       pImmGetCompositionStringW == ರಿಕ್ತ
	    || pImmGetContext == ರಿಕ್ತ
	    || pImmAssociateContext == ರಿಕ್ತ
	    || pImmReleaseContext == ರಿಕ್ತ
	    || pImmGetOpenStatus == ರಿಕ್ತ
	    || pImmSetOpenStatus == ರಿಕ್ತ
	    || pImmGetCompositionFontW == ರಿಕ್ತ
	    || pImmSetCompositionFontW == ರಿಕ್ತ
	    || pImmSetCompositionWindow == ರಿಕ್ತ
	    || pImmGetConversionStatus == ರಿಕ್ತ
	    || pImmSetConversionStatus == ರಿಕ್ತ)
    {
	FreeLibrary(hLibImm);
	hLibImm = ರಿಕ್ತ;
	pImmGetContext = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತ್ಯಾಗಮ;
}

#endif

#ಯದಿ defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ typenr)
{
    signicon_t *sign;
    ಪೂರ್ಣಾಙ್ಕ x, y, w, h;

    ಯದಿ (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    ವಿಕಲ್ಪ (sign->uType)
    {
	ಪಕ್ಷ IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ IMAGE_ICON:
	ಪಕ್ಷ IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ರಿಕ್ತ, DI_NORMAL);
	    ಭಙ್ಗ;
# ifdef FEAT_XPM_W32
	ಪಕ್ಷ IMAGE_XPM:
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
	    ಭಙ್ಗ;
# endif
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
close_signicon_image(signicon_t *sign)
{
    ಯದಿ (sign == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ವಿಕಲ್ಪ (sign->uType)
    {
	ಪಕ್ಷ IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    ಭಙ್ಗ;
	ಪಕ್ಷ IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    ಭಙ್ಗ;
	ಪಕ್ಷ IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    ಭಙ್ಗ;
# ifdef FEAT_XPM_W32
	ಪಕ್ಷ IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    ಭಙ್ಗ;
# endif
    }
}

    ಶೂನ್ಯ *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ರಿಕ್ತ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    ಯದಿ (ext > signfile)
    {
	ಪೂರ್ಣಾಙ್ಕ do_load = 1;

	ಯದಿ (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	ಅನ್ಯಥಾ
	    do_load = 0;

	ಯದಿ (do_load)
	    sign.hImage = (HANDLE)LoadImage(ರಿಕ್ತ, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	ಯದಿ (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((ವರ್ಣ *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ರಿಕ್ತ;
    ಯದಿ (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ರಿಕ್ತ)
	*psign = sign;

    ಯದಿ (!psign)
    {
	ಯದಿ (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)psign;

}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    ಯದಿ (sign == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#ಯದಿ defined(FEAT_BEVAL_GUI)

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

    ಸ್ಥಿರ ಶೂನ್ಯ
make_tooltip(BalloonEval *beval, ವರ್ಣ *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    ಯದಿ (pti == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ರಿಕ್ತ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, ರಿಕ್ತ, g_hinst, ರಿಕ್ತ);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = ಪರಿಮಾಣ(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, ರಿಕ್ತ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    ಯದಿ (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ಬಲ);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ಎಡ = pt.x - 3;
    pti->rect.ತುದಿ = pt.y - 3;
    pti->rect.ಬಲ = pt.x + 3;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    ಸ್ಥಿರ VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_ಸಂದೇಶ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    ಯದಿ (cur_beval == ರಿಕ್ತ || cur_beval->showState == ShS_SHOWING || !p_beval)
	ಪ್ರತ್ಯಾಗಮ;

    GetCursorPos(&pt);
    ಯದಿ (WindowFromPoint(pt) != s_textArea)
	ಪ್ರತ್ಯಾಗಮ;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    ಯದಿ (!PtInRect(&rect, pt))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (last_user_activity > 0
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

	ಯದಿ (cur_beval->ಸಂದೇಶ_CB != ರಿಕ್ತ)
	    (*cur_beval->ಸಂದೇಶ_CB)(cur_beval, 0);
    }
}

    ಶೂನ್ಯ
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    ಶೂನ್ಯ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ಯದಿ (beval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    ಶೂನ್ಯ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->ಸಂದೇಶ);
    beval->ಸಂದೇಶ = mesg == ರಿಕ್ತ ? ರಿಕ್ತ : vim_strsave(mesg);
    ಯದಿ (beval->ಸಂದೇಶ == ರಿಕ್ತ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (beval->showState == ShS_SHOWING)
	ಪ್ರತ್ಯಾಗಮ;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    ಯದಿ (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (ವರ್ಣ *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    ಶೂನ್ಯ	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    ಶೂನ್ಯ	(*mesgCB)(BalloonEval *, ಪೂರ್ಣಾಙ್ಕ),
    ಶೂನ್ಯ	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    ಯದಿ (mesg != ರಿಕ್ತ && mesgCB != ರಿಕ್ತ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ಯದಿ (beval != ರಿಕ್ತ)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->ಸಂದೇಶ = mesg;
	beval->ಸಂದೇಶ_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	ಯದಿ (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ಪ್ರತ್ಯಾಗಮ beval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    ಯದಿ (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (cur_beval == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ವಿಕಲ್ಪ (pnmh->code)
    {
	ಪಕ್ಷ TTN_SHOW:
	    ಭಙ್ಗ;
	ಪಕ್ಷ TTN_ತೆಗೆ: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ಭಙ್ಗ;
	ಪಕ್ಷ TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ಭಙ್ಗ;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
track_user_activity(UINT u_ಸಂದೇಶ)
{
    ಯದಿ ((u_ಸಂದೇಶ >= WM_MOUSEFIRST && u_ಸಂದೇಶ <= WM_MOUSELAST)
	    || (u_ಸಂದೇಶ >= WM_KEYFIRST && u_ಸಂದೇಶ <= WM_KEYLAST))
	last_user_activity = Get_Tick_ಎಣಿಕೆ();
}

    ಶೂನ್ಯ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#ಯದಿ defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    ಶೂನ್ಯ
netbeans_draw_multisign_indicator(ಪೂರ್ಣಾಙ್ಕ row)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ y;
    ಪೂರ್ಣಾಙ್ಕ x;

    ಯದಿ (!netbeans_active())
	ಪ್ರತ್ಯಾಗಮ;

    x = 0;
    y = TEXT_Y(row);

# ಯದಿ defined(FEAT_DIRECTX)
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ಪ್ರತಿ (i = 0; i < gui.char_height - 3; i++)
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

#ಯದಿ defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_w32_sendevent_mouse(dict_T *args)
{
    ಯದಿ (!dict_has_ಕೀಲಿ(args, "row") || !dict_has_ಕೀಲಿ(args, "col"))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Note: "move" is optional, requires fewer arguments
    ಪೂರ್ಣಾಙ್ಕ move = (ಪೂರ್ಣಾಙ್ಕ)dict_get_bool(args, "move", FALSE);

    ಯದಿ (!move && (!dict_has_ಕೀಲಿ(args, "button")
	    || !dict_has_ಕೀಲಿ(args, "multiclick")
	    || !dict_has_ಕೀಲಿ(args, "modifiers")))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪೂರ್ಣಾಙ್ಕ row = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "row");
    ಪೂರ್ಣಾಙ್ಕ col = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "col");

    ಯದಿ (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ಯದಿ (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    ಪೂರ್ಣಾಙ್ಕ pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    ಪೂರ್ಣಾಙ್ಕ pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	ಅನ್ಯಥಾ
	    gui_mouse_moved(col, row);
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ button = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "button");
	ಪೂರ್ಣಾಙ್ಕ repeated_click = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "multiclick");
	int_u mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    ಒಳಹರಿವು_2 inputs[1];
    ಒಳಹರಿವು_2 modkeys[3];
    SecureZeroMemory(inputs, ಪರಿಮಾಣ(ಒಳಹರಿವು_2));
    SecureZeroMemory(modkeys, 3 * ಪರಿಮಾಣ(ಒಳಹರಿವು_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    ಯದಿ (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_ಸಂಖ್ಯೆ_def(args, "keycode", 0);
	ಯದಿ (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	BOOL is_Mod_ಕೀಲಿ = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	ಪೂರ್ಣಾಙ್ಕ mods = (ಪೂರ್ಣಾಙ್ಕ)dict_get_ಸಂಖ್ಯೆ(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	ಯದಿ (mods && STRICMP(event, "keydown") == 0 && !is_Mod_ಕೀಲಿ)
	{
	    ಪೂರ್ಣಾಙ್ಕ n = 0;
	    ಯದಿ (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    ಯದಿ (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    ಯದಿ (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    ಯದಿ (n)
	    {
		(ಶೂನ್ಯ)SetForegroundWindow(s_hwnd);
		Send_ಒಳಹರಿವು(n, modkeys, ಪರಿಮಾಣ(ಒಳಹರಿವು_2));
	    }
	}

	inputs[0].type = ಒಳಹರಿವು_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	ಯದಿ (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    ಯದಿ (!is_Mod_ಕೀಲಿ)
		unwrapMods = TRUE;
	}

	(ಶೂನ್ಯ)SetForegroundWindow(s_hwnd);
	Send_ಒಳಹರಿವು(ARRAYSIZE(inputs), inputs, ಪರಿಮಾಣ(ಒಳಹರಿವು_2));
	vim_free(event);

	ಯದಿ (unwrapMods)
	{
	    modkeys[0].type = ಒಳಹರಿವು_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = ಒಳಹರಿವು_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = ಒಳಹರಿವು_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (ಶೂನ್ಯ)SetForegroundWindow(s_hwnd);
	    Send_ಒಳಹರಿವು(3, modkeys, ಪರಿಮಾಣ(ಒಳಹರಿವು_2));
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (event == ರಿಕ್ತ)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    ಪೂರ್ಣಾಙ್ಕ handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    ಯದಿ (strategy)
    {
	ಯದಿ (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	ಅನ್ಯಥಾ ಯದಿ (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    ಯದಿ (!handled)
    {
	ಯದಿ (strategy == ರಿಕ್ತ)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	ಅನ್ಯಥಾ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}


    ಪೂರ್ಣಾಙ್ಕ
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    ಯದಿ (STRICMP(event, "key") == 0)
	ಪ್ರತ್ಯಾಗಮ test_gui_w32_sendevent_keyboard(args);
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(event, "mouse") == 0)
	ಪ್ರತ್ಯಾಗಮ test_gui_w32_sendevent_mouse(args);
    ಅನ್ಯಥಾ ಯದಿ (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ಪ್ರತ್ಯಾಗಮ test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str_str), "event", event);
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
}
#endif

#ಯದಿ defined(FEAT_IMAGE_GUI)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, ಪೂರ್ಣಾಙ್ಕ iw, ಪೂರ್ಣಾಙ್ಕ ih, image_format_T fmt)
{
    ಪೂರ್ಣಾಙ್ಕ		y, x;
    ಪೂರ್ಣಾಙ್ಕ		stride = iw * 4;
    ಪೂರ್ಣಾಙ್ಕ		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    ಪ್ರತಿ (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	ಪ್ರತಿ (x = 0; x < iw; x++)
	{
	    ಯದಿ (fmt == IMAGE_FORMAT_RGBA)
	    {
		ಪೂರ್ಣಾಙ್ಕ a = s[3];

		ಯದಿ (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		ಅನ್ಯಥಾ ಯದಿ (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		ಅನ್ಯಥಾ
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    ಅನ್ಯಥಾ
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

    ಪೂರ್ಣಾಙ್ಕ
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    ಪೂರ್ಣಾಙ್ಕ		iw, ih;
    ಶೂನ್ಯ	*bits = ರಿಕ್ತ;

    ಯದಿ (ctx == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = ಪರಿಮಾಣ(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_ಎಣಿಕೆ = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, ರಿಕ್ತ, 0);
    ಯದಿ (hbm == ರಿಕ್ತ || bits == ರಿಕ್ತ)
    {
	ಯದಿ (hbm != ರಿಕ್ತ)
	    DeleteObject(hbm);
	ಪ್ರತ್ಯಾಗಮ false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    ಯದಿ (mem_dc == ರಿಕ್ತ)
    {
	DeleteObject(hbm);
	ಪ್ರತ್ಯಾಗಮ false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    ಪೂರ್ಣಾಙ್ಕ
image_placement_gui_init(image_placement_T *place UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    ಶೂನ್ಯ
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    ಪೂರ್ಣಾಙ್ಕ		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ಯದಿ (rects == ರಿಕ್ತ || n_rects == 0)
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    ಯದಿ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಙ್ಕ		row, col;
	ಪೂರ್ಣಾಙ್ಕ		posx, posy;
	ಪೂರ್ಣಾಙ್ಕ		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    ಶೂನ್ಯ
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
