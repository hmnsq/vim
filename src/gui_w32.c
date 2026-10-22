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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_ଚାବି_OFF			0	// no dead key
#define DEAD_ଚାବି_SET_DEFAULT		1	// dead key pressed
#define DEAD_ଚାବି_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_ଚାବି_SKIP_ON_CHAR		3	// skip next _OnChar()

#ଯଦି defined(FEAT_DIRECTX)
ସ୍ଥିର DWriteContext *s_dwc = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_directx_enabled = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ଖାଲି && enc_utf8)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା directx_enabled(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ directx_binddc(ଶୂନ୍ୟ);
#endif

#ifdef FEAT_MENU
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gui_mswin_get_menu_height(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fix_window);
#ଅନ୍ୟଥା
# define gui_mswin_get_menu_height(fix_window)	0
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା keycode_trans_strategy {
    ଶୂନ୍ୟ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
    ଶୂନ୍ୟ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
    ଶୂନ୍ୟ (*ptr_process_message_usual_ଚାବି) (UINT /*vk*/, ଅଚଳ ସନ୍ଦେଶ_2* /*pmsg*/);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  (*ptr_get_active_modifiers)(ଶୂନ୍ୟ);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  (*is_experimental)(ଶୂନ୍ୟ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ସ୍ଥିର ଶୂନ୍ୟ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
ସ୍ଥିର ଶୂନ୍ୟ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
ସ୍ଥିର ଶୂନ୍ୟ process_message_usual_ଚାବି_experimental(UINT /*vk*/, ଅଚଳ ସନ୍ଦେଶ_2* /*pmsg*/);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  get_active_modifiers_experimental(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  is_experimental_true(ଶୂନ୍ୟ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_ଚାବି_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ସ୍ଥିର ଶୂନ୍ୟ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
ସ୍ଥିର ଶୂନ୍ୟ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ପୂର୍ଣ୍ଣସଂଖ୍ୟା /*cRepeat UNUSED*/);
ସ୍ଥିର ଶୂନ୍ୟ process_message_usual_ଚାବି_classic(UINT /*vk*/, ଅଚଳ ସନ୍ଦେଶ_2* /*pmsg*/);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  get_active_modifiers_classic(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  is_experimental_false(ଶୂନ୍ୟ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_ଚାବି_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = ଖାଲି;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_experimental_true(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_experimental_false(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ସ୍ଥିର ଶୂନ୍ୟ keycode_trans_strategy_init(ଶୂନ୍ୟ)
{
    ଅଚଳ ଅକ୍ଷର *strategy = ଖାଲି;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    ଯଦି (strategy == ଖାଲି)
    {
	ଫେରନ୍ତୁ;
    }

    ଯଦି (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	ଫେରନ୍ତୁ;
    }

}

#ଯଦି defined(FEAT_RENDER_OPTIONS)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dx_enable = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dx_flags = 0;
    ଦଶମିକ   dx_gamma = 0.0f;
    ଦଶମିକ   dx_contrast = 0.0f;
    ଦଶମିକ   dx_level = 0.0f;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dx_geom = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dx_renmode = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    dx_taamode = 0;
    // parse string as rendering options.
    ପାଇଁ (p = s; p != ଖାଲି && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  ମୂଲ୍ୟ[128];

	copy_option_part(&p, item, ଆକାର(item), ",");
	ଯଦି (p == ଖାଲି)
	    ଭାଙ୍ଗ;
	q = &item[0];
	copy_option_part(&q, name, ଆକାର(name), ":");
	ଯଦି (q == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	copy_option_part(&q, ମୂଲ୍ୟ, ଆକାର(ମୂଲ୍ୟ), ":");

	ଯଦି (STRCMP(name, "type") == 0)
	{
	    ଯଦି (STRCMP(ମୂଲ୍ୟ, "directx") == 0)
		dx_enable = 1;
	    ଅନ୍ୟଥା
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (ଦଶମିକ)atof((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (ଦଶମିକ)atof((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (ଦଶମିକ)atof((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	    ଯଦି (dx_geom < 0 || dx_geom > 2)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	    ଯଦି (dx_renmode < 0 || dx_renmode > 6)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((ଅକ୍ଷର *)ମୂଲ୍ୟ);
	    ଯଦି (dx_taamode < 0 || dx_taamode > 3)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    ଯଦି (dx_enable)
    {
	ଯଦି (!directx_enabled())
	    ଫେରନ୍ତୁ FAIL;
	DWriteContext_SetRenderingParams(s_dwc, ଖାଲି);
	ଯଦି (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    ଯଦି (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    ଯଦି (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    ଯଦି (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    ଯଦି (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    ଯଦି (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    ଯଦି (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ଫେରନ୍ତୁ OK;
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)LOWORD(lParam), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)LOWORD(lParam), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)LOWORD(lParam), (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#ଅନ୍ତର୍ଭାଵ "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#ଅନ୍ତର୍ଭାଵ <shellapi.h>
#ଅନ୍ତର୍ଭାଵ <commctrl.h>
#ଅନ୍ତର୍ଭାଵ <windowsx.h>

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

ସ୍ଥିର ଶୂନ୍ୟ _OnPaint(HWND hwnd);
ସ୍ଥିର ଶୂନ୍ୟ fill_rect(ଅଚଳ RECT *rcp, HBRUSH hbr, COLORREF color);
ସ୍ଥିର ଶୂନ୍ୟ clear_rect(RECT *rcp);

ସ୍ଥିର WORD		s_dlgfntheight;		// height of the dialog font
ସ୍ଥିର WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
ସ୍ଥିର HMENU		s_menuBar = ଖାଲି;
#endif
#ifdef FEAT_TEAROFF
ସ୍ଥିର ଶୂନ୍ୟ rebuild_tearoff(vimmenu_T *menu);
ସ୍ଥିର HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		s_busy_processing = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
ସ୍ଥିର UINT		s_findrep_ସନ୍ଦେଶ = 0;
ସ୍ଥିର FINDREPLACEW	s_findrep_struct;
ସ୍ଥିର HWND		s_findrep_hwnd = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = ଖାଲି;
ସ୍ଥିର HDC		s_hdc = ଖାଲି;
ସ୍ଥିର HBRUSH		s_brush = ଖାଲି;

#ifdef FEAT_TOOLBAR
ସ୍ଥିର HWND		s_toolbarhwnd = ଖାଲି;
ସ୍ଥିର WNDPROC		s_toolbar_wndproc = ଖାଲି;
#endif

#ifdef FEAT_GUI_TABLINE
ସ୍ଥିର HWND		s_tabhwnd = ଖାଲି;
ସ୍ଥିର WNDPROC		s_tabline_wndproc = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		showing_tabline = 0;
#endif

ସ୍ଥିର WPARAM		s_wParam = 0;

ସ୍ଥିର HWND		s_textArea = ଖାଲି;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ସ୍ଥିର bool		s_textarea_bg_only = false;

ସ୍ଥିର UINT		s_u_ସନ୍ଦେଶ = 0;

ସ୍ଥିର char_u		*s_textfield; // Used by dialogs to pass back strings

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
ପ୍ରକାରବ୍ୟାଖ୍ୟା HANDLE DPI_AWARENESS_CONTEXT;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା DPI_AWARENESS {
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		s_dpi = DEFAULT_DPI;
ସ୍ଥିର BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
ସ୍ଥିର DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
ସ୍ଥିର RECT		s_suggested_rect;

ସ୍ଥିର UINT (WINAPI *pGetDpiForSystem)(ଶୂନ୍ୟ) = ଖାଲି;
ସ୍ଥିର UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (WINAPI *pGetSystemMetricsForDpi)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, UINT) = ଖାଲି;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ସ୍ଥିର DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = ଖାଲି;
#ifdef FEAT_MBYTE_IME
ସ୍ଥିର DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ଖାଲି;
#endif

ସ୍ଥିର HINSTANCE hLibDwm = ଖାଲି;
ସ୍ଥିର HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
ସ୍ଥିର ଶୂନ୍ୟ dyn_dwm_load(ଶୂନ୍ୟ);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

ସ୍ଥିର HINSTANCE hUxThemeLib = ଖାଲି;
ସ୍ଥିର DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ (WINAPI *pFlushMenuThemes)(ଶୂନ୍ୟ) = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ dyn_uxtheme_load(ଶୂନ୍ୟ);
#endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା WINAPI
stubGetSystemMetricsForDpi(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nIndex, UINT dpi UNUSED)
{
    ଫେରନ୍ତୁ GetSystemMetrics(nIndex);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
adjust_fontsize_by_dpi(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size)
{
    ଫେରନ୍ତୁ size * s_dpi / (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pGetDpiForSystem();
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
adjust_by_system_dpi(ପୂର୍ଣ୍ଣସଂଖ୍ୟା size)
{
    ଫେରନ୍ତୁ size * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pGetDpiForSystem() / DEFAULT_DPI;
}

#ଯଦି defined(FEAT_DIRECTX)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
directx_enabled(ଶୂନ୍ୟ)
{
    ଯଦି (s_dwc != ଖାଲି)
	ଫେରନ୍ତୁ 1;
    ଅନ୍ୟଥା ଯଦି (s_directx_load_attempted)
	ଫେରନ୍ତୁ 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ଫେରନ୍ତୁ s_dwc != ଖାଲି ? 1 : 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
directx_binddc(ଶୂନ୍ୟ)
{
    ଯଦି (s_textArea == ଖାଲି)
	ଫେରନ୍ତୁ;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା current_font_height;	    // this is in os_mswin.c

ସ୍ଥିର ସଂରଚନା
{
    UINT    ଚାବି_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ଚାବିଗୁଡ଼ିକ[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_ବାମ,		'k', 'l'},
    {VK_ଡାହାଣ,		'k', 'r'},

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
    {VK_ଭର୍ତ୍ତି,		'k', 'I'},
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	s_getting_focus = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	s_x_pending;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	s_y_pending;
ସ୍ଥିର UINT	s_kFlags_pending;
ସ୍ଥିର UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	s_timed_out = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dead_ଚାବି = DEAD_ଚାବି_OFF;
ସ୍ଥିର UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ସ୍ଥିର ଶୂନ୍ୟ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ସ୍ଥିର ଶୂନ୍ୟ track_user_activity(UINT u_ସନ୍ଦେଶ);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ସ୍ଥିର LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ସ୍ଥିର LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
ସ୍ଥିର LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#ଯଦି defined(FEAT_BROWSE)
ସ୍ଥିର char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ସ୍ଥିର ଶୂନ୍ୟ
print_windows_error(ଶୂନ୍ୟ)
{
    LPVOID  lp_ସନ୍ଦେଶ_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  ଖାଲି, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_ସନ୍ଦେଶ_Buf, 0, ଖାଲି);
    TRACE1("Error: %s\n", lp_ସନ୍ଦେଶ_Buf);
    LocalFree(lp_ସନ୍ଦେଶ_Buf);
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blink_state = BLINK_NONE;
ସ୍ଥିର long_u		blink_waittime = 700;
ସ୍ଥିର long_u		blink_ontime = 400;
ସ୍ଥିର long_u		blink_offtime = 250;
ସ୍ଥିର UINT_PTR		blink_timer = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blinking(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state != BLINK_NONE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blink_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state == BLINK_OFF;
}

    ଶୂନ୍ୟ
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ସ୍ଥିର VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ଖାଲି, idEvent);

    // Eat spurious WM_TIMER messages
    ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    ଯଦି (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(ଖାଲି, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    ଅନ୍ୟଥା
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(ଖାଲି, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_mswin_rm_blink_timer(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

    ଯଦି (blink_timer == 0)
	ଫେରନ୍ତୁ;

    KillTimer(ଖାଲି, blink_timer);
    // Eat spurious WM_TIMER messages
    ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ଶୂନ୍ୟ
gui_mch_stop_blink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    ଯଦି (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    ଶୂନ୍ୟ
gui_mch_start_blink(ଶୂନ୍ୟ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    ଯଦି (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(ଖାଲି, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ସ୍ଥିର VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ଖାଲି, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    ଯଦି (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    dead_ଚାବି = DEAD_ଚାବି_SET_DEFAULT;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
char_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch, char_u *string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା slen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା had_alt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    WCHAR	wstring[2];
    char_u	*ws = ଖାଲି;

    ଯଦି (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    ଅନ୍ୟଥା ଯଦି (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    ଯଦି (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ଖାଲି);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	ଯଦି (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ଖାଲି);
	    ଯଦି (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    ଅନ୍ୟଥା
    {
	ws = utf16_to_enc(wstring, &len);
	ଯଦି (ws == ଖାଲି)
	    len = 0;
	ଅନ୍ୟଥା
	{
	    ଯଦି (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    ଯଦି (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    ପାଇଁ (i = 0; i < len; ++i)
	ଯଦି (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	    len += 2;
	}

    ଫେରନ୍ତୁ len;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_active_modifiers_experimental(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers = 0;

    ଯଦି (Get_ଚାବି_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    ଯଦି (Get_ଚାବି_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    ଯଦି (Get_ଚାବି_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    ଯଦି ((modifiers & MOD_MASK_CTRL) && (Get_ଚାବି_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    ଯଦି (!(Get_ଚାବି_State(VK_CONTROL) & 0x8000)
	    &&  (Get_ଚାବି_State(VK_RMENU) & 0x8000)
	    && !(Get_ଚାବି_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    ଫେରନ୍ତୁ modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_active_modifiers_classic(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers = 0;

    ଯଦି (Get_ଚାବି_State(VK_SHIFT) & 0x8000)
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
    ଯଦି (Get_ଚାବି_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    ଯଦି (Get_ଚାବି_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    ଫେରନ୍ତୁ modifiers;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_active_modifiers(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ get_active_modifiers_experimental();
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_active_modifiers_via_ptr(ଶୂନ୍ୟ)
{
    // marshal to corresponding implementation
    ଫେରନ୍ତୁ keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    char_u	string[40];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch = cch;   // special keys are negative

    ଯଦି (dead_ଚାବି == DEAD_ଚାବି_SKIP_ON_CHAR)
	ଫେରନ୍ତୁ;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    ଯଦି (dead_ଚାବି != DEAD_ଚାବି_TRANSIENT_IN_ON_CHAR)
	dead_ଚାବି = DEAD_ଚାବି_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_ଚାବି(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_ଚାବିଗୁଡ଼ିକ;
    ch = may_adjust_ଚାବି_for_ctrl(modifiers, ch);
    --no_reduce_ଚାବିଗୁଡ଼ିକ;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ଖାଲି);
    ଯଦି (ch == CSI)
	ch = K_CSI;

    ଯଦି (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_ନିବେଶ_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    ଯଦି (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ନିବେଶ_buf();
	got_int = TRUE;
    }

    add_to_ନିବେଶ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    char_u	string[40];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 0;

    dead_ଚାବି = 0;

    len = char_to_string(ch, string, 40, FALSE);
    ଯଦି (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ନିବେଶ_buf();
	got_int = TRUE;
    }

    add_to_ନିବେଶ_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch = cch;   // special keys are negative

    dead_ଚାବି = DEAD_ଚାବି_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_ଚାବି(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ଖାଲି);
    ଯଦି (ch == CSI)
	ch = K_CSI;

    len = 0;
    ଯଦି (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ଯଦି (IS_SPECIAL((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch);
	string[len++] = K_THIRD((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch);
    }
    ଅନ୍ୟଥା
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_ନିବେଶ_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ch = cch;   // special keys are negative

    dead_ଚାବି = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    ଯଦି (Get_ଚାବି_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    ଯଦି (Get_ଚାବି_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_ଚାବି(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, ଖାଲି);
    ଯଦି (ch == CSI)
	ch = K_CSI;

    len = 0;
    ଯଦି (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ଯଦି (IS_SPECIAL((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch);
	string[len++] = K_THIRD((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch);
    }
    ଅନ୍ୟଥା
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_ନିବେଶ_buf(string, len);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnMouseEvent(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା button,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeated_click,
    UINT ଚାବି_Flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    ଯଦି (ଚାବି_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    ଯଦି (ଚାବି_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    ଯଦି (Get_ଚାବି_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y,
    UINT ଚାବି_Flags)
{
    ସ୍ଥିର LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    button = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (ଶୂନ୍ୟ)SetFocus(s_hwnd);

    ଯଦି (s_u_ସନ୍ଦେଶ == WM_LBUTTONDOWN || s_u_ସନ୍ଦେଶ == WM_LBUTTONDBLCLK)
	button = MOUSE_ବାମ;
    ଅନ୍ୟଥା ଯଦି (s_u_ସନ୍ଦେଶ == WM_MBUTTONDOWN || s_u_ସନ୍ଦେଶ == WM_MBUTTONDBLCLK)
	button = MOUSE_ମଧ୍ୟ;
    ଅନ୍ୟଥା ଯଦି (s_u_ସନ୍ଦେଶ == WM_RBUTTONDOWN || s_u_ସନ୍ଦେଶ == WM_RBUTTONDBLCLK)
	button = MOUSE_ଡାହାଣ;
    ଅନ୍ୟଥା ଯଦି (s_u_ସନ୍ଦେଶ == WM_XBUTTONDOWN || s_u_ସନ୍ଦେଶ == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    ଅନ୍ୟଥା ଯଦି (s_u_ସନ୍ଦେଶ == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	ଯଦି (s_button_pending == MOUSE_ବାମ)
	    button = MOUSE_ଡାହାଣ;
	ଅନ୍ୟଥା
	    button = MOUSE_ବାମ;
    }

    ଯଦି (button < 0)
	ଫେରନ୍ତୁ;

    repeated_click = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    ଯଦି (repeated_click
	    && ((button == MOUSE_ବାମ && s_button_pending == MOUSE_ଡାହାଣ)
		|| (button == MOUSE_ଡାହାଣ
		    && s_button_pending == MOUSE_ବାମ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_ମଧ୍ୟ;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, ଚାବି_Flags);
    }
    ଅନ୍ୟଥା ଯଦି ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ଡାହାଣ)))
    {
	ଯଦି (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, ଚାବି_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, ଚାବି_Flags);
    }
    ଅନ୍ୟଥା
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
	s_kFlags_pending = ଚାବି_Flags;
    }

    s_prevTime = currentTime;
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y,
    UINT ଚାବି_Flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା button;

    s_getting_focus = FALSE;
    ଯଦି (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    ଯଦି (s_u_ସନ୍ଦେଶ == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	ଯଦି (!(ଚାବି_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ଫେରନ୍ତୁ;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    ଅନ୍ୟଥା
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, ଚାବି_Flags);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cx UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cy UNUSED)
{
#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ସ୍ଥିର vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id)
{
    vimmenu_T	*pChildMenu;

    ଯେତେବେଳେ (pMenu)
    {
	ଯଦି (pMenu->id == (UINT)id)
	    ଭାଙ୍ଗ;
	ଯଦି (pMenu->children != ଖାଲି)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    ଯଦି (pChildMenu)
	    {
		pMenu = pChildMenu;
		ଭାଙ୍ଗ;
	    }
	}
	pMenu = pMenu->next;
    }
    ଫେରନ୍ତୁ pMenu;
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnMenu(
    HWND	hwnd UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(ମୂଳ_menu, id);
    ଯଦି (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnFindRepl(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    flags = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    down;

    ଯଦି (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(ଶୂନ୍ୟ)SetFocus(s_hwnd);

    ଯଦି (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ଶୂନ୍ୟ)SetFocus(s_hwnd);
    }
    ଅନ୍ୟଥା ଯଦି (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ଶୂନ୍ୟ)SetFocus(s_hwnd);
    }
    ଅନ୍ୟଥା ଯଦି (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    ଯଦି (flags == 0)
	ଫେରନ୍ତୁ;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    ଯଦି (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    ଯଦି (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, ଖାଲି);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, ଖାଲି);
    ଯଦି (p != ଖାଲି && q != ଖାଲି)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
HandleMouseHide(UINT u_ସନ୍ଦେଶ, LPARAM lParam)
{
    ସ୍ଥିର LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    ଯଦି (u_ସନ୍ଦେଶ == WM_MOUSEMOVE || u_ସନ୍ଦେଶ == WM_NCMOUSEMOVE)
    {
	ଯଦି (lParam == last_lParam)
	    ଫେରନ୍ତୁ;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    ବାଛ (u_ସନ୍ଦେଶ)
    {
    ସ୍ଥିତି WM_KEYUP:
    ସ୍ଥିତି WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	ଯଦି (p_mh)
	    gui_mch_mousehide(TRUE);
	ଭାଙ୍ଗ;

    ସ୍ଥିତି WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    ସ୍ଥିତି WM_SYSCHAR:
    ସ୍ଥିତି WM_MOUSEMOVE:	 // show the pointer on any mouse action
    ସ୍ଥିତି WM_LBUTTONDOWN:
    ସ୍ଥିତି WM_LBUTTONUP:
    ସ୍ଥିତି WM_MBUTTONDOWN:
    ସ୍ଥିତି WM_MBUTTONUP:
    ସ୍ଥିତି WM_RBUTTONDOWN:
    ସ୍ଥିତି WM_RBUTTONUP:
    ସ୍ଥିତି WM_XBUTTONDOWN:
    ସ୍ଥିତି WM_XBUTTONUP:
    ସ୍ଥିତି WM_NCMOUSEMOVE:
    ସ୍ଥିତି WM_NCLBUTTONDOWN:
    ସ୍ଥିତି WM_NCLBUTTONUP:
    ସ୍ଥିତି WM_NCMBUTTONDOWN:
    ସ୍ଥିତି WM_NCMBUTTONUP:
    ସ୍ଥିତି WM_NCRBUTTONDOWN:
    ସ୍ଥିତି WM_NCRBUTTONUP:
    ସ୍ଥିତି WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_ସନ୍ଦେଶ, lParam);

    s_u_ସନ୍ଦେଶ = u_ସନ୍ଦେଶ;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_ସନ୍ଦେଶ);
#endif

    ବାଛ (u_ସନ୍ଦେଶ)
    {
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	ସ୍ଥିତି WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ଫେରନ୍ତୁ TRUE;
#endif
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
dyn_dwm_load(ଶୂନ୍ୟ)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    ଯଦି (hLibDwm == ଖାଲି)
	ଫେରନ୍ତୁ;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    ଯଦି (pDwmSetWindowAttribute == ଖାଲି)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
}

ବାହ୍ୟ DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    ଶୂନ୍ୟ
gui_mch_set_titlebar_colors(ଶୂନ୍ୟ)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    ଯଦି (pDwmSetWindowAttribute == ଖାଲି || win_version < MAKE_VER(10, 0, 22000))
	ଫେରନ୍ତୁ;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    ଯଦି (vim_strchr(p_go, GO_TITLEBAR) != ଖାଲି)
    {
	ଯଦି (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	ଅନ୍ୟଥା
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	ଯଦି (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	ଯଦି (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, ଆକାର(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, ଆକାର(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    ଶୂନ୍ୟ
gui_mch_new_colors(ଶୂନ୍ୟ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ଖାଲି, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    ଯଦି (s_textarea_bg_only && s_textArea != ଖାଲି)
	InvalidateRect(s_textArea, ଖାଲି, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    ଶୂନ୍ୟ
gui_mch_def_colors(ଶୂନ୍ୟ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_open(ଶୂନ୍ୟ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    ଯଦି (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	ଯଦି (s_textArea != ଖାଲି)
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

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.ବାମ;
    *y = rect.ଶୀର୍ଷ;
    ଫେରନ୍ତୁ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ଶୂନ୍ୟ
gui_mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    SetWindowPos(s_hwnd, ଖାଲି, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    ଶୂନ୍ୟ
gui_mch_set_text_area_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldx = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା oldy = 0;

    SetWindowPos(s_textArea, ଖାଲି, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    ଯଦି (vim_strchr(p_go, GO_TOOLBAR) != ଖାଲି)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#ଯଦି defined(FEAT_GUI_TABLINE)
    ଯଦି (showing_tabline)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଶୀର୍ଷ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	ଯଦି (vim_strchr(p_go, GO_TOOLBAR) != ଖାଲି)
	    ଶୀର୍ଷ = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, ଶୀର୍ଷ, rect.ଡାହାଣ, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    ଯଦି (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ଖାଲି, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    ଶୂନ୍ୟ
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h)
{
    SetWindowPos(sb->id, ଖାଲି, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_xpadding(ଶୂନ୍ୟ)
{
    RECT    rcTxt, rcWnd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.ଡାହାଣ - rcTxt.ଡାହାଣ - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ଫେରନ୍ତୁ (xpad < 0) ? 0 : xpad;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_ypadding(ଶୂନ୍ୟ)
{
    RECT    rcTxt, rcWnd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ଫେରନ୍ତୁ (ypad < 0) ? 0 : ypad;
}

    ଶୂନ୍ୟ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ଖାଲି,
	g_hinst, ଖାଲି);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ସ୍ଥିର scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    ଯଦି (gui.bottom_sbar.id == hwnd)
	ଫେରନ୍ତୁ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ଯଦି (wp->w_scrollbars[SBAR_ବାମ].id == hwnd)
	    ଫେରନ୍ତୁ &wp->w_scrollbars[SBAR_ବାମ];
	ଯଦି (wp->w_scrollbars[SBAR_ଡାହାଣ].id == hwnd)
	    ଫେରନ୍ତୁ &wp->w_scrollbars[SBAR_ଡାହାଣ];
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
update_scrollbar_size(ଶୂନ୍ୟ)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
GetFontSize(GuiFont font, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *char_width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    ଯଦି (char_width)
	*char_width = size.cx + tm.tmOverhang;
    ଯଦି (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_adjust_charheight(ଶୂନ୍ୟ)
{
    UpdateFontSize(gui.norm_font);
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ଖାଲି;

    // Load the font
    font = CreateFontIndirectW(lf);

    ଯଦି (font == ଖାଲି)
	ଫେରନ୍ତୁ NOFONT;

    ଫେରନ୍ତୁ (GuiFont)font;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
pixels_to_points(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pixels, ପୂର୍ଣ୍ଣସଂଖ୍ୟା vertical)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ଫେରନ୍ତୁ points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    ଯଦି (get_logfont(&lf, name, ଖାଲି, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    ଯଦି (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ଫେରନ୍ତୁ font;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strsave(name);
}
#endif

    ଶୂନ୍ୟ
gui_mch_free_font(GuiFont font)
{
    ଯଦି (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା SysColorTable
    {
	ଅକ୍ଷର	    *name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    color;
    } SysColorTable;

    ସ୍ଥିର SysColorTable sys_table[] =
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
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(sys_table); i++)
	ଯଦି (STRICMP(name, sys_table[i].name) == 0)
	    ଫେରନ୍ତୁ GetSysColor(sys_table[i].color);

    ଫେରନ୍ତୁ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଫେରନ୍ତୁ gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_haskey(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].vim_code1 != NUL; i++)
	ଯଦି (name[0] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code0
				       && name[1] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code1)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
gui_mch_beep(ଶୂନ୍ୟ)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ଶୂନ୍ୟ
gui_mch_invert_rectangle(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nc)
{
    RECT    rc;

#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ବାମ = FILL_X(c);
    rc.ଶୀର୍ଷ = FILL_Y(r);
    rc.ଡାହାଣ = rc.ବାମ + nc * gui.char_width;
    rc.bottom = rc.ଶୀର୍ଷ + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    ଶୂନ୍ୟ
gui_mch_iconify(ଶୂନ୍ୟ)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    ଶୂନ୍ୟ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ବାମ = FILL_X(gui.col);
    rc.ଶୀର୍ଷ = FILL_Y(gui.row);
    rc.ଡାହାଣ = rc.ବାମ + gui.char_width;
    ଯଦି (mb_lefthalve(gui.row, gui.col))
	rc.ଡାହାଣ += gui.char_width;
    rc.bottom = rc.ଶୀର୍ଷ + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ଶୂନ୍ୟ
gui_mch_draw_part_cursor(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ବାମ =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ଡାହାଣ ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.ଶୀର୍ଷ = FILL_Y(gui.row) + gui.char_height - h;
    rc.ଡାହାଣ = rc.ବାମ + w;
    rc.bottom = rc.ଶୀର୍ଷ + h;

    fill_rect(&rc, ଖାଲି, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
output_Dead_ଚାବି_re_Post_Ex(ସନ୍ଦେଶ_2 original_ସନ୍ଦେଶ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dead_key2set)
{
    ସ୍ଥିର ସନ୍ଦେଶ_2 deadCharExpel;

    ଯଦି (dead_ଚାବି == DEAD_ଚାବି_OFF)
	ଫେରନ୍ତୁ;

    dead_ଚାବି = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_ସନ୍ଦେଶ.message;
    deadCharExpel.hwnd    = original_ସନ୍ଦେଶ.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_ସନ୍ଦେଶ.hwnd, original_ସନ୍ଦେଶ.message, original_ସନ୍ଦେଶ.wParam,
							  original_ସନ୍ଦେଶ.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
output_Dead_ଚାବି_re_Post(ସନ୍ଦେଶ_2 original_ସନ୍ଦେଶ)
{
    output_Dead_ଚାବି_re_Post_Ex(original_ସନ୍ଦେଶ, DEAD_ଚାବି_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ସ୍ଥିର ଶୂନ୍ୟ process_message_usual_ଚାବି(UINT vk, ଅଚଳ ସନ୍ଦେଶ_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_ଚାବି(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
process_message_usual_ଚାବି_experimental(UINT vk, ଅଚଳ ସନ୍ଦେଶ_2 *pmsg)
{
    WCHAR	ch[8];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    ଯଦି (Get_ଚାବି_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    ଯଦି (Get_ଚାବି_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    ଯଦି ((Get_ଚାବି_State(VK_RMENU) & 0x8000)
				 && (Get_ଚାବି_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_ଚାବି(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    ଯଦି (len < 0)
	dead_ଚାବି = DEAD_ଚାବି_SET_DEFAULT;

    ଯଦି (len <= 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା wm_char = NUL;

	ଯଦି (dead_ଚାବି == DEAD_ଚାବି_SET_DEFAULT
		&& (Get_ଚାବି_State(VK_CONTROL) & 0x8000))
	{
	    ଯଦି (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    ଯଦି (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	ଯଦି (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_ଚାବି = DEAD_ଚାବି_TRANSIENT_IN_ON_CHAR;
	}
	ଫେରନ୍ତୁ;
    }

    // Post the message as TranslateMessage would do.
    ଯଦି (pmsg->message == WM_KEYDOWN)
    {
	ପାଇଁ (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ସ୍ଥିର ଶୂନ୍ୟ
process_message_usual_ଚାବି_classic(UINT vk, ଅଚଳ ସନ୍ଦେଶ_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    ଯଦି (vk != 0xff
	    && (Get_ଚାବି_State(VK_CONTROL) & 0x8000)
	    && !(Get_ଚାବି_State(VK_SHIFT) & 0x8000)
	    && !(Get_ଚାବି_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	ଯଦି (vk == '6' || Map_Virtual_ଚାବି(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_ନିବେଶ_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	ଅନ୍ୟଥା ଯଦି (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_ନିବେଶ_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	ଅନ୍ୟଥା ଯଦି (vk == '2' || Map_Virtual_ଚାବି(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_ନିବେଶ_buf(string, 1);
	}
	ଅନ୍ୟଥା
	    TranslateMessage(pmsg);
    }
    ଅନ୍ୟଥା
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
process_message(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_2		ସନ୍ଦେଶ;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି;
#ifdef FEAT_MENU
    ସ୍ଥିର char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    ଯଦି (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&ସନ୍ଦେଶ, ଖାଲି, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    ଯଦି (ସନ୍ଦେଶ.message == WM_OLE)
    {
	char_u *str = (char_u *)ସନ୍ଦେଶ.lParam;
	ଯଦି (str == ଖାଲି || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&ସନ୍ଦେଶ);
	}
	ଅନ୍ୟଥା
	{
	    add_to_ନିବେଶ_buf(str, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ଫେରନ୍ତୁ;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    ଯଦି (s_findrep_hwnd != ଖାଲି && IsDialogMessageW(s_findrep_hwnd, &ସନ୍ଦେଶ))
    {
	HandleMouseHide(ସନ୍ଦେଶ.message, ସନ୍ଦେଶ.lParam);
	ଫେରନ୍ତୁ;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    ଯଦି (ସନ୍ଦେଶ.message == WM_KEYDOWN || ସନ୍ଦେଶ.message == WM_SYSKEYDOWN)
    {
	vk = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ସନ୍ଦେଶ.wParam;

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
	ଯଦି (keycode_trans_strategy_used->is_experimental()
		&& dead_ଚାବି == DEAD_ଚାବି_SKIP_ON_CHAR)
	{
	    dead_ଚାବି = DEAD_ଚାବି_OFF;
	}

	ଯଦି (dead_ଚାବି != DEAD_ଚାବି_OFF)
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
	    ଯଦି (keycode_trans_strategy_used->is_experimental() &&
		    dead_ଚାବି == DEAD_ଚାବି_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_ଚାବି_re_Post_Ex(ସନ୍ଦେଶ,
				       /*dead_key2set=*/DEAD_ଚାବି_SKIP_ON_CHAR);
		ଫେରନ୍ତୁ;
	    }

	    ଯଦି (dead_ଚାବି != DEAD_ଚାବି_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ଫେରନ୍ତୁ;
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
	    ଯଦି ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_ଚାବି = DEAD_ଚାବି_OFF;
		TranslateMessage(&ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    ଅନ୍ୟଥା ଯଦି ((get_real_state()
			    & (MODE_ଭର୍ତ୍ତି | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_ଚାବି_re_Post(ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ;
	    }
	}

	// Check for CTRL-BREAK
	ଯଦି (vk == VK_CANCEL)
	{
	    trash_ନିବେଶ_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_ନିବେଶ_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	ଯଦି (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&ସନ୍ଦେଶ);
	    ଫେରନ୍ତୁ;
	}

	ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == vk
		    && (vk != VK_SPACE || !(Get_ଚାବି_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		ଯଦି (dead_ଚାବି == DEAD_ଚାବି_SET_DEFAULT
			&& (special_ଚାବିଗୁଡ଼ିକ[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_ଚାବି_re_Post(ସନ୍ଦେଶ);
		    ଫେରନ୍ତୁ;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		ଯଦି (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ଖାଲି, ଖାଲି) == ଖାଲି)
		    ଭାଙ୍ଗ;
#endif
		modifiers = get_active_modifiers_via_ptr();

		ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].vim_code1 == NUL)
		    ଚାବି = special_ଚାବିଗୁଡ଼ିକ[i].vim_code0;
		ଅନ୍ୟଥା
		    ଚାବି = TO_SPECIAL(special_ଚାବିଗୁଡ଼ିକ[i].vim_code0,
						   special_ଚାବିଗୁଡ଼ିକ[i].vim_code1);
		ଚାବି = simplify_ଚାବି(ଚାବି, &modifiers);
		ଯଦି (ଚାବି == CSI)
		    ଚାବି = K_CSI;

		ଯଦି (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_ନିବେଶ_buf(string, 3);
		}

		ଯଦି (IS_SPECIAL(ଚାବି))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(ଚାବି);
		    string[2] = K_THIRD(ଚାବି);
		    add_to_ନିବେଶ_buf(string, 3);
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(ଚାବି, string, 40, FALSE);
		    add_to_ନିବେଶ_buf(string, len);
		}
		ଭାଙ୍ଗ;
	    }
	}

	// Not a special key.
	ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == 0)
	{
	    process_message_usual_ଚାବି(vk, &ସନ୍ଦେଶ);
	}
    }
#ifdef FEAT_MBYTE_IME
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ.message == WM_IME_NOTIFY)
	_OnImeNotify(ସନ୍ଦେଶ.hwnd, (DWORD)ସନ୍ଦେଶ.wParam, (DWORD)ସନ୍ଦେଶ.lParam);
    ଅନ୍ୟଥା ଯଦି (ସନ୍ଦେଶ.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&ସନ୍ଦେଶ);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    ଯଦି (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ଖାଲି, ଖାଲି) == ଖାଲି)
#endif
	DispatchMessageW(&ସନ୍ଦେଶ);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ଶୂନ୍ୟ
gui_mch_update(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_2	    ସନ୍ଦେଶ;

    ଯଦି (!s_busy_processing)
	ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, ଖାଲି, 0, 0, PM_NOREMOVE)
						  && !vim_is_ନିବେଶ_buf_full())
	    process_message();
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_any_timer(ଶୂନ୍ୟ)
{
    ସନ୍ଦେଶ_2		ସନ୍ଦେଶ;

    ଯଦି (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(ଖାଲି, s_wait_timer);

	// Eat spurious WM_TIMER messages
	ଯେତେବେଳେ (PeekMessageW(&ସନ୍ଦେଶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ସ୍ଥିର garray_T	server_pending_ga = {0, 0, 0, 0, ଖାଲି};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    ଶୂନ୍ୟ
server_add_ନିବେଶ(char_u *str)
{
    ଯଦି (server_pending_ga.ga_data == ଖାଲି)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
server_flush_ନିବେଶ(ଶୂନ୍ୟ)
{
    char_u  *str;

    ଯଦି (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ଫେରନ୍ତୁ;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    ଯଦି (str != ଖାଲି)
    {
	server_to_ନିବେଶ_buf(str);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_wait_for_chars(long wtime)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		focus;

    s_timed_out = FALSE;

    ଯଦି (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	ଯଦି (s_busy_processing)
	    ଫେରନ୍ତୁ FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(ଖାଲି, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    ଯେତେବେଳେ (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	ଯଦି (gui.in_focus != focus)
	{
	    ଯଦି (gui.in_focus)
		gui_mch_start_blink();
	    ଅନ୍ୟଥା
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	ଯଦି (s_need_activate)
	{
	    (ଶୂନ୍ୟ)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	ପାଇଁ (;;)
	{
	    ସନ୍ଦେଶ_2 ସନ୍ଦେଶ;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ନିବେଶ();
# endif
# ifdef FEAT_TIMERS
	    ଯଦି (did_add_timer)
		ଭାଙ୍ଗ;
# endif
	    ଯଦି (PeekMessageW(&ସନ୍ଦେଶ, ଖାଲି, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		ଭାଙ୍ଗ;
	    }
	    ଅନ୍ୟଥା ଯଦି (ନିବେଶ_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || ସନ୍ଦେଶ_Wait_For_Multiple_Objects(0, ଖାଲି, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		ଭାଙ୍ଗ;
	}
#ଅନ୍ୟଥା
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	ଯଦି (ନିବେଶ_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    ଯଦି (!s_getting_focus)
		s_button_pending = -1;

	    ଫେରନ୍ତୁ OK;
	}

#ifdef FEAT_TIMERS
	ଯଦି (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    ଭାଙ୍ଗ;
	}
#endif
    }
    allow_scrollbar = FALSE;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ଶୂନ୍ୟ
gui_mch_clear_block(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ବାମ = FILL_X(col1);
    rc.ଶୀର୍ଷ = FILL_Y(row1);
    rc.ଡାହାଣ = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    ଶୂନ୍ୟ
gui_mch_clear_all(ଶୂନ୍ୟ)
{
    RECT    rc;

    rc.ବାମ = 0;
    rc.ଶୀର୍ଷ = 0;
    rc.ଡାହାଣ = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    ଶୂନ୍ୟ
gui_mch_enable_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : ଖାଲି);
#endif
}

    ଶୂନ୍ୟ
gui_mch_set_menu_pos(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    y UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    w UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    h UNUSED)
{
    // It will be in the right place anyway
}

#ଯଦି defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    ଶୂନ୍ୟ
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hidden)
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
    ଶୂନ୍ୟ
gui_mch_draw_menubar(ଶୂନ୍ୟ)
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
    ଫେରନ୍ତୁ (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#ଯଦି defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ସ୍ଥିର WORD
PixelToDialogX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Pixels)
{
    ଫେରନ୍ତୁ (WORD)((ଅଙ୍କ_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ସ୍ଥିର WORD
PixelToDialogY(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Pixels)
{
    ଫେରନ୍ତୁ (WORD)((ଅଙ୍କ_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
GetTextWidth(HDC hdc, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    ଫେରନ୍ତୁ size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
GetTextWidthEnc(HDC hdc, char_u *str, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    SIZE	size;
    WCHAR	*wstr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    ଯଦି (wstr == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    ଯଦି (n)
	ଫେରନ୍ତୁ size.cx;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ସ୍ଥିର BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    wChild, hChild, wParent, hParent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.ଡାହାଣ - rChild.ବାମ;
    hChild = rChild.bottom - rChild.ଶୀର୍ଷ;

    // If Vim is minimized put the window in the middle of the screen.
    ଯଦି (hwndParent == ଖାଲି || IsMinimized(hwndParent))
	get_work_area(&rParent);
    ଅନ୍ୟଥା
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.ଡାହାଣ - rParent.ବାମ;
    hParent = rParent.bottom - rParent.ଶୀର୍ଷ;

    moninfo.cbSize = ଆକାର(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    ଯଦି (mon != ଖାଲି && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    ଅନ୍ୟଥା
    {
	hdc = GetDC(hwndChild);
	rScreen.ବାମ = 0;
	rScreen.ଶୀର୍ଷ = 0;
	rScreen.ଡାହାଣ = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.ବାମ + ((wParent - wChild) / 2);
    ଯଦି (xNew < rScreen.ବାମ)
	xNew = rScreen.ବାମ;
    ଅନ୍ୟଥା ଯଦି ((xNew + wChild) > rScreen.ଡାହାଣ)
	xNew = rScreen.ଡାହାଣ - wChild;

    yNew = rParent.ଶୀର୍ଷ + ((hParent - hChild) / 2);
    ଯଦି (yNew < rScreen.ଶୀର୍ଷ)
	yNew = rScreen.ଶୀର୍ଷ;
    ଅନ୍ୟଥା ଯଦି ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    ଫେରନ୍ତୁ SetWindowPos(hwndChild, ଖାଲି, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#ଯଦି defined(FEAT_TOOLBAR)
    ଶୂନ୍ୟ
gui_mch_show_toolbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (s_toolbarhwnd == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    ଅନ୍ୟଥା
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_ଗଣନା 31

#endif

#ଯଦି defined(FEAT_GUI_TABLINE)
    ସ୍ଥିର ଶୂନ୍ୟ
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, ଖାଲି);
    ଯଦି (wn == ଖାଲି)
	ଫେରନ୍ତୁ;

    infow.cbSize = ଆକାର(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    ଭର୍ତ୍ତି_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    ସ୍ଥିର ଶୂନ୍ୟ
show_tabline_popup_menu(ଶୂନ୍ୟ)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    ଯଦି (hold_gui_events || cmdwin_type != 0)
	ଫେରନ୍ତୁ;

    tab_pmenu = CreatePopupMenu();
    ଯଦି (tab_pmenu == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (first_tabpage->tp_next != ଖାଲି)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ଖାଲି);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    ଯଦି (rval > 0)
    {
	TCHITTESTINFO htinfo;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

	ଯଦି (ScreenToClient(s_tabhwnd, &pt) == 0)
	    ଫେରନ୍ତୁ;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	ଯଦି (idx == -1)
	    idx = 0;
	ଅନ୍ୟଥା
	    idx += 1;

	send_tabline_menu_event(idx, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_show_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (s_tabhwnd == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (!showit != !showing_tabline)
    {
	ଯଦି (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	ଅନ୍ୟଥା
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_showing_tabline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ s_tabhwnd != ଖାଲି && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_update_tabline(ଶୂନ୍ୟ)
{
    tabpage_T	*tp;
    TCITEM	tie;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curtabidx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabadded = 0;
    WCHAR	*wstr = ଖାଲି;

    ଯଦି (s_tabhwnd == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = tp->tp_next, ++nr)
    {
	ଯଦି (tp == curtab)
	    curtabidx = nr;

	ଯଦି (nr >= Tab_Ctrl_Get_Item_ଗଣନା(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_ଭର୍ତ୍ତି_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, ଖାଲି);
	ଯଦି (wstr != ଖାଲି)
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
    ଯେତେବେଳେ (nr < Tab_Ctrl_Get_Item_ଗଣନା(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    ଯଦି (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, ଖାଲି, ଖାଲି,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    ଯଦି (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ଶୂନ୍ୟ
gui_mch_set_curtab(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ଯଦି (s_tabhwnd == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    ଶୂନ୍ୟ
gui_mch_set_dark_theme(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dark)
{
    ଯଦି (pDwmSetWindowAttribute != ଖାଲି && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		ଆକାର(dark));

    ଯଦି (pSetPreferredAppMode != ଖାଲି && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    ଯଦି (pFlushMenuThemes != ଖାଲି && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ସ୍ଥିର ଶୂନ୍ୟ
dyn_uxtheme_load(ଶୂନ୍ୟ)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    ଯଦି (hUxThemeLib == ଖାଲି)
	ଫେରନ୍ତୁ;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (ଶୂନ୍ୟ (WINAPI *)(ଶୂନ୍ୟ))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    ଯଦି (pSetPreferredAppMode == ଖାଲି || pFlushMenuThemes == ଖାଲି)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ଖାଲି;
	ଫେରନ୍ତୁ;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    ଶୂନ୍ୟ
gui_mch_set_fullscreen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ସ୍ଥିର RECT normal_rect;
    ସ୍ଥିର LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    ଯଦି (!full_screen) // Windows not set yet.
	ଫେରନ୍ତୁ;

    ଯଦି (flag)
    {
	ଯଦି (fullscreen_on)
	    ଫେରନ୍ତୁ;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = ଆକାର(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	ଯଦି (mon == ଖାଲି || !GetMonitorInfo(mon, &moninfo))
	    ଫେରନ୍ତୁ;

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
	SetWindowPos(s_hwnd, ଖାଲି,
		moninfo.rcMonitor.ବାମ,
		moninfo.rcMonitor.ଶୀର୍ଷ,
		moninfo.rcMonitor.ଡାହାଣ - moninfo.rcMonitor.ବାମ,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.ଶୀର୍ଷ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!fullscreen_on)
	    ଫେରନ୍ତୁ;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, ଖାଲି,
		normal_rect.ବାମ,
		normal_rect.ଶୀର୍ଷ,
		normal_rect.ଡାହାଣ - normal_rect.ବାମ,
		normal_rect.bottom - normal_rect.ଶୀର୍ଷ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    ଶୂନ୍ୟ
ex_simalt(exarg_T *eap)
{
    char_u	*ଚାବିଗୁଡ଼ିକ = eap->arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fill_typebuf = FALSE;
    char_u	ଚାବି_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    ଯେତେବେଳେ (*ଚାବିଗୁଡ଼ିକ)
    {
	ଯଦି (*ଚାବିଗୁଡ଼ିକ == '~')
	    *ଚାବିଗୁଡ଼ିକ = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*ଚାବିଗୁଡ଼ିକ, (LPARAM)0);
	ଚାବିଗୁଡ଼ିକ++;
	fill_typebuf = TRUE;
    }
    ଯଦି (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	ଚାବି_name[0] = K_SPECIAL;
	ଚାବି_name[1] = KS_EXTRA;
	ଚାବି_name[2] = KE_NOP;
	ଚାବି_name[3] = NUL;
#ଯଦି defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(ଶୂନ୍ୟ)ins_typebuf(ଚାବି_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    ସ୍ଥିର ଶୂନ୍ୟ
initialise_findrep(char_u *initial_string)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wword = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    ଯଦି (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    ଯଦି (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    ଯଦି (entry_text != ଖାଲି && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, ଖାଲି);
	ଯଦି (p != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
set_window_title(HWND hwnd, ଅକ୍ଷର *title)
{
    ଯଦି (title != ଖାଲି)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, ଖାଲି);
	ଯଦି (wbuf != ଖାଲି)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)SetWindowTextW(hwnd, ଖାଲି);
}

    ଶୂନ୍ୟ
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    ଯଦି (s_findrep_ସନ୍ଦେଶ != 0)
    {
	ଯଦି (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	ଯଦି (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(ଶୂନ୍ୟ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    ଶୂନ୍ୟ
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    ଯଦି (s_findrep_ସନ୍ଦେଶ != 0)
    {
	ଯଦି (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	ଯଦି (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(ଶୂନ୍ୟ)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    ଶୂନ୍ୟ
gui_mch_mousehide(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hide)
{
    ଯଦି (hide == gui.pointer_hidden)
	ଫେରନ୍ତୁ;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    ସ୍ଥିର ଶୂନ୍ୟ
gui_mch_show_popupmenu_at(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (ଶୂନ୍ୟ)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)0,	    //reserved param
	s_hwnd,
	ଖାଲି);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnEndSession(ଶୂନ୍ୟ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnDestroy(HWND hwnd)
{
    ଯଦି (!destroying)
	_OnClose(hwnd);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnPaint(
    HWND hwnd)
{
    ଯଦି (IsMinimized(hwnd))
	ଫେରନ୍ତୁ;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (ଶୂନ୍ୟ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    ଯଦି (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ବାମ = rect.ବାମ;
	ps.rcPaint.ଡାହାଣ = rect.ଡାହାଣ;
    }

    ଯଦି (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	ଯଦି (!screen_cleared || Screen_ଧାଡ଼ିଗୁଡ଼ିକ == ଖାଲି)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	ଅନ୍ୟଥା
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ବାମ, ps.rcPaint.ଶୀର୍ଷ,
		ps.rcPaint.ଡାହାଣ - ps.rcPaint.ବାମ + 1,
		ps.rcPaint.bottom - ps.rcPaint.ଶୀର୍ଷ + 1);
    }

    EndPaint(hwnd, &ps);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cx,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cy)
{
    ଯଦି (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (ଶୂନ୍ୟ)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    ଯଦି (destroying)
	ଫେରନ୍ତୁ;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (ଶୂନ୍ୟ)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ସ୍ଥିର LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    ଫେରନ୍ତୁ DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    ଶୂନ୍ୟ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    ଶୂନ୍ୟ
gui_mch_getmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    RECT rct;
    POINT mp;

    (ଶୂନ୍ୟ)GetWindowRect(s_textArea, &rct);
    (ଶୂନ୍ୟ)GetCursorPos(&mp);
    *x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mp.x - rct.ବାମ);
    *y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(mp.y - rct.ଶୀର୍ଷ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    ଶୂନ୍ୟ
gui_mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    RECT rct;

    (ଶୂନ୍ୟ)GetWindowRect(s_textArea, &rct);
    (ଶୂନ୍ୟ)SetCursorPos(x + gui.border_offset + rct.ବାମ,
		       y + gui.border_offset + rct.ଶୀର୍ଷ);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_mswin_get_valid_dimensions(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *valid_w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *valid_h,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *cols,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rows)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    base_width, base_height;

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

    ଶୂନ୍ୟ
gui_mch_flash(ପୂର୍ଣ୍ଣସଂଖ୍ୟା msec)
{
    RECT    rc;

#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ବାମ = 0;
    rc.ଶୀର୍ଷ = 0;
    rc.ଡାହାଣ = gui.ଅଙ୍କ_cols * gui.char_width;
    rc.bottom = gui.ଅଙ୍କ_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ସ୍ଥିର BOOL
is_point_onscreen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    POINT   pt = {x, y};

    ଫେରନ୍ତୁ MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != ଖାଲି;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ସ୍ଥିର BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ବାମ;
    p1.y = rc.ଶୀର୍ଷ;
    p2.x = rc.ଡାହାଣ - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    ଯଦି (!is_point_onscreen(p1.x, p1.y))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!is_point_onscreen(p1.x, p2.y))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!is_point_onscreen(p2.x, p1.y))
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (!is_point_onscreen(p2.x, p2.y))
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_scroll_flags(ଶୂନ୍ୟ)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    ଯଦି (!is_window_onscreen(s_hwnd))
	ଫେରନ୍ତୁ SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    ପାଇଁ (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	ଯଦି (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    ଯଦି (IntersectRect(&rcDest, &rcVim, &rcOther))
		ଫେରନ୍ତୁ SW_INVALIDATE;
	}
    ଫେରନ୍ତୁ 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
intel_gpu_workaround(ଶୂନ୍ୟ)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ଶୂନ୍ୟ
gui_mch_delete_ଧାଡ଼ିଗୁଡ଼ିକ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    row,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    RECT	rc;

    rc.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    rc.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1);
    rc.ଶୀର୍ଷ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height, &rc);
    }
    ଅନ୍ୟଥା
#endif
    {
#ଯଦି defined(FEAT_DIRECTX)
	ଯଦି (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height,
				    &rc, &rc, ଖାଲି, ଖାଲି, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1,
						       gui.scroll_region_ବାମ,
	gui.scroll_region_bot, gui.scroll_region_ଡାହାଣ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ଶୂନ୍ୟ
gui_mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    RECT	rc;

    rc.ବାମ = FILL_X(gui.scroll_region_ବାମ);
    rc.ଡାହାଣ = FILL_X(gui.scroll_region_ଡାହାଣ + 1);
    rc.ଶୀର୍ଷ = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height, &rc);
    }
    ଅନ୍ୟଥା
#endif
    {
#ଯଦି defined(FEAT_DIRECTX)
	ଯଦି (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ * gui.char_height,
				    &rc, &rc, ଖାଲି, ଖାଲି, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_ବାମ,
				row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ - 1, gui.scroll_region_ଡାହାଣ);
}


    ଶୂନ୍ୟ
gui_mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc UNUSED)
{
#ଯଦି defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ଖାଲି;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (ଶୂନ୍ୟ)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    ଯଦି (s_hwnd != ଖାଲି)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    ସ୍ଥିର char_u *
logfont2name(LOGFONTW lf)
{
    ଅକ୍ଷର	*charset_name;
    ଅକ୍ଷର	*quality_name;
    ଅକ୍ଷର	*font_name;
    size_t	res_size;
    ଅକ୍ଷର	*res;

    font_name = (ଅକ୍ଷର *)utf16_to_enc(lf.lfFaceName, ଖାଲି);
    ଯଦି (font_name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    charset_name = charset_id2name((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lf.lfCharSet);
    quality_name = quality_id2name((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == ଖାଲି ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ଖାଲି ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    ଯଦି (res != ଖାଲି)
    {
	ଅକ୍ଷର	*p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	ପାଇଁ (p = font_name; *p != NUL; ++p)
	{
	    ଯଦି (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	ଯଦି (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(ଅକ୍ଷର *)res, res_size, "%s:h%d", font_name, points);
	ଅନ୍ୟଥା
	    res_len = vim_snprintf_safelen(
		(ଅକ୍ଷର *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (ଅକ୍ଷର *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	ଯଦି (charset_name != ଖାଲି)
	    res_len += vim_snprintf_safelen((ଅକ୍ଷର *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	ଯଦି (quality_name != ଖାଲି)
	    vim_snprintf((ଅକ୍ଷର *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ଫେରନ୍ତୁ (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
update_im_font(ଶୂନ୍ୟ)
{
    LOGFONTW	lf_wide, lf;

    ଯଦି (p_guifontwide != ଖାଲି && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, ଆକାର(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    ଅନ୍ୟଥା
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    ଯଦି (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    ଶୂନ୍ୟ
gui_mch_wide_font_changed(ଶୂନ୍ୟ)
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

    ଯଦି (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, ଆକାର(lf), &lf))
    {
	ଯଦି (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	ଯଦି (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    ଯଦି (!lf.lfItalic)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_font(char_u *font_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    ଯଦି (get_logfont(&lf, font_name, ଖାଲି, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    ଯଦି (font == NOFONT)
	ଫେରନ୍ତୁ FAIL;

#ଯଦି defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    feat_ଗଣନା = 0;
	char_u		    *fp;

	ଯଦି (font_name != ଖାଲି)
	{
	    // Find each ":f" option in font_name.
	    ପାଇଁ (fp = font_name; *fp != NUL; fp++)
	    {
		ଯଦି (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା    ti = 0;
		    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା param = 1;

		    fp += 2;  // skip ":f"
		    ଯେତେବେଳେ (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    ଯଦି (ti != 4)
			ଜାରି;  // invalid tag length

		    ଯଦି (*fp == '=')
		    {
			fp++;
			param = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)atoi((ଅକ୍ଷର *)fp);
			ଯେତେବେଳେ (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    ଯଦି (feat_ଗଣନା < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_ଗଣନା].tag =
			    ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tag[0])
			    | ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tag[1] << 8)
			    | ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tag[2] << 16)
			    | ((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)tag[3] << 24);
			features[feat_ଗଣନା].parameter = param;
			feat_ଗଣନା++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_ଗଣନା);
    }
#endif

    ଯଦି (font_name == ଖାଲି)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    ଯଦି (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    ଯଦି (p != ଖାଲି)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	ଯଦି (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	ଅନ୍ୟଥା
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    ଯଦି (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    ଯଦି (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	ଯଦି (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_maximized(ଶୂନ୍ୟ)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = ଆକାର(WINDOWPLACEMENT);
    ଯଦି (GetWindowPlacement(s_hwnd, &wp))
    {
	ଯଦି (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ଫେରନ୍ତୁ TRUE;
	ଯଦି (wp.showCmd == SW_SHOWMINIMIZED)
	    ଫେରନ୍ତୁ FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	ଯଦି ((rc.ଡାହାଣ - rc.ବାମ !=
		    wp.rcNormalPosition.ଡାହାଣ - wp.rcNormalPosition.ବାମ)
		|| (rc.bottom - rc.ଶୀର୍ଷ !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.ଶୀର୍ଷ))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ଶୂନ୍ୟ
gui_mch_newfont(ଶୂନ୍ୟ)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    ଯଦି (win_socket_id == 0)
    {
	gui_resize_shell(rect.ଡାହାଣ - rect.ବାମ
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.ଶୀର୍ଷ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    ଅନ୍ୟଥା
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ଡାହାଣ - rect.ବାମ,
	    rect.bottom - rect.ଶୀର୍ଷ - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    ଶୂନ୍ୟ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == ଖାଲି ? "VIM" : (ଅକ୍ଷର *)title));
}

#ଯଦି defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ସ୍ଥିର LPCSTR mshape_idcs[] =
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

    ଶୂନ୍ୟ
mch_set_mouse_shape(ପୂର୍ଣ୍ଣସଂଖ୍ୟା shape)
{
    LPCSTR idc;

    ଯଦି (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    ଅନ୍ୟଥା
    {
	ଯଦି (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	ଅନ୍ୟଥା
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(ଖାଲି, idc));
	ଯଦି (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (ଶୂନ୍ୟ)GetCursorPos(&mp);
	    (ଶୂନ୍ୟ)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#ଯଦି defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ସ୍ଥିର WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
    WCHAR *res;

    tmp = convert_filter(s);
    ଯଦି (tmp == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    ଫେରନ୍ତୁ res;
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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା saving,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    WCHAR		*titlep = ଖାଲି;
    WCHAR		*extp = ଖାଲି;
    WCHAR		*initdirp = ଖାଲି;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    ଯଦି (dflt == ଖାଲି)
	fileBuf[0] = NUL;
    ଅନ୍ୟଥା
    {
	wp = enc_to_utf16(dflt, ଖାଲି);
	ଯଦି (wp == ଖାଲି)
	    fileBuf[0] = NUL;
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# ଅନ୍ୟଥା
    fileStruct.lStructSize = ଆକାର(fileStruct);
# endif

    ଯଦି (title != ଖାଲି)
	titlep = enc_to_utf16(title, ଖାଲି);
    fileStruct.lpstrTitle = titlep;

    ଯଦି (ext != ଖାଲି)
	extp = enc_to_utf16(ext, ଖାଲି);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    ଯଦି (initdir != ଖାଲି && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, ଖାଲି);
	ଯଦି (initdirp != ଖାଲି)
	{
	    ପାଇଁ (wp = initdirp; *wp != NUL; ++wp)
		ଯଦି (*wp == '/')
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
    ଯଦି (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    ଯଦି (saving)
	ret = GetSaveFileNameW(&fileStruct);
    ଅନ୍ୟଥା
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    ଯଦି (!ret)
	ଫେରନ୍ତୁ ଖାଲି;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, ଖାଲି);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ଫେରନ୍ତୁ q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ସ୍ଥିର char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    ଯଦି (res != ଖାଲି)
    {
	ପାଇଁ (i = 0; i < s_len; ++i)
	    ଯଦି (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    ଅନ୍ୟଥା
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ଫେରନ୍ତୁ res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ଫେରନ୍ତୁ gui_mch_browse(0, title, (char_u *)_("Not Used"), ଖାଲି,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    ସ୍ଥିର ଶୂନ୍ୟ
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    ଅକ୍ଷର    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ଖାଲି, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    ଯଦି (fnames != ଖାଲି)
	ପାଇଁ (i = 0; i < cFiles; ++i)
	{
	    ଯଦି (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, ଖାଲି);
	    ଅନ୍ୟଥା
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    ଯଦି (fnames == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା kbd_modifiers = get_active_modifiers();

    ଯଦି ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    ଯଦି ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ଯଦି ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos)
{
    ସ୍ଥିର UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dragging = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = ଆକାର(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    ଯଦି (sb == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ଯଦି (sb->wp != ଖାଲି)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    ଅନ୍ୟଥା	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->ମୂଲ୍ୟ;

    ବାଛ (code)
    {
	ସ୍ଥିତି SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    ଯଦି (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_LINEDOWN:
	    val++;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_LINEUP:
	    val--;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_ଶୀର୍ଷ:
	    val = 0;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_BOTTOM:
	    val = sb_info->max;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SB_ENDSCROLL:
	    ଯଦି (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		ଯଦି (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ଫେରନ୍ତୁ 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    ଯଦି (sb->wp != ଖାଲି)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ବାମ) ? SBAR_ଡାହାଣ : SBAR_ବାମ].id;

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

    ଫେରନ୍ତୁ 0;
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

ସ୍ଥିର BalloonEval  *cur_beval = ଖାଲି;
ସ୍ଥିର UINT_PTR	    beval_timer_id = 0;
ସ୍ଥିର DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
ସ୍ଥିର UINT	s_menu_id = 100;
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
ସ୍ଥିର PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	ଅଚଳ ଅକ୍ଷର *caption);
ସ୍ଥିର LPWORD lpwAlign(LPWORD);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#ଯଦି defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ସ୍ଥିର ଶୂନ୍ୟ gui_mch_tearoff(char_u *title, vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା initX, ପୂର୍ଣ୍ଣସଂଖ୍ୟା initY);
#endif
ସ୍ଥିର ଶୂନ୍ୟ get_dialog_font_metrics(ଶୂନ୍ୟ);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
ସ୍ଥିର ଶୂନ୍ୟ initialise_toolbar(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ update_toolbar_size(ଶୂନ୍ୟ);
ସ୍ଥିର LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_ସନ୍ଦେଶ, WPARAM wParam, LPARAM lParam);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_toolbar_bitmap(vimmenu_T *menu);
#ଅନ୍ୟଥା
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
ସ୍ଥିର ଶୂନ୍ୟ initialise_tabline(ଶୂନ୍ୟ);
ସ୍ଥିର LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_ସନ୍ଦେଶ, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
ସ୍ଥିର LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ସ୍ଥିର char_u *GetResultStr(HWND hwnd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା GCS, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp);
#endif
#ଯଦି defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
ପ୍ରକାରବ୍ୟାଖ୍ୟା HANDLE HIMC;
# endif

ସ୍ଥିର HINSTANCE hLibImm = ଖାଲି;
ସ୍ଥିର LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ସ୍ଥିର HIMC (WINAPI *pImmGetContext)(HWND);
ସ୍ଥିର HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ସ୍ଥିର BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ସ୍ଥିର BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
ସ୍ଥିର BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
ସ୍ଥିର BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ସ୍ଥିର BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
ସ୍ଥିର BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
ସ୍ଥିର BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
ସ୍ଥିର BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
ସ୍ଥିର ଶୂନ୍ୟ dyn_imm_load(ଶୂନ୍ୟ);
#ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mswin_get_menu_height(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    fix_window)	    // If TRUE, resize window if menu height changed
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	old_menu_height = -1;

    RECT    rc1, rc2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଅଙ୍କ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    menu_height;

    ଯଦି (gui.menu_is_active)
	ଅଙ୍କ = Get_Menu_Item_ଗଣନା(s_menuBar);
    ଅନ୍ୟଥା
	ଅଙ୍କ = 0;

    ଯଦି (ଅଙ୍କ == 0)
	menu_height = 0;
    ଅନ୍ୟଥା ଯଦି (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    ଅନ୍ୟଥା
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	ଯଦି (gui.starting)
	    menu_height = rc1.bottom - rc1.ଶୀର୍ଷ + 1;
	ଅନ୍ୟଥା
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, ଅଙ୍କ - 1, &rc2);
	    menu_height = rc2.bottom - rc1.ଶୀର୍ଷ + 1;
	}
    }

    ଯଦି (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    ଫେରନ୍ତୁ menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ସ୍ଥିର long
mouse_vertical_scroll_step(ଶୂନ୍ୟ)
{
    UINT val;
    ଯଦି (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ଫେରନ୍ତୁ (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    ଫେରନ୍ତୁ 3; // Safe default;
}

    ସ୍ଥିର long
mouse_horizontal_scroll_step(ଶୂନ୍ୟ)
{
    UINT val;
    ଯଦି (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ଫେରନ୍ତୁ (long)val;
    ଫେରନ୍ତୁ 3; // Safe default;
}

    ସ୍ଥିର ଶୂନ୍ୟ
init_mouse_wheel(ଶୂନ୍ୟ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, ପୂର୍ଣ୍ଣସଂଖ୍ୟା horizontal)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button;
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		kbd_modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    ଯଦି (wp != ଖାଲି && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	ଯଦି (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ବାମ : MSCR_ଡାହାଣ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ଅନ୍ୟଥା
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

    ଯଦି (wp == ଖାଲି || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    ଯଦି (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    ଅନ୍ୟଥା
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    ଯଦି ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    ଯଦି ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ଯଦି ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = ଆକାର(NONCLIENTMETRICSW);
    ଯଦି (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    ଆକାର(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ଫେରନ୍ତୁ FAIL;
    *lf = nm.lfMenuFont;
    ଫେରନ୍ତୁ OK;
}
#endif


#ଯଦି defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_tabline_font(ଶୂନ୍ୟ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    ଯଦି (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ଫେରନ୍ତୁ;

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
#ଅନ୍ୟଥା
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    ସ୍ଥିର LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    ବାଛ (param)
    {
	ସ୍ଥିତି SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ 0;
}

#ifdef FEAT_NETBEANS_INTG
    ସ୍ଥିର ଶୂନ୍ୟ
_OnWindowPosChanged(
    HWND hwnd,
    ଅଚଳ LPWINDOWPOS lpwpos)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା x = 0, y = 0, cx = 0, cy = 0;
    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା WSInitialized;

    ଯଦି (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


ସ୍ଥିର HWND hwndTip = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
show_sizing_tip(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows)
{
    TOOLINFOA ti;
    ଅକ୍ଷର buf[32];

    ti.cbSize = ଆକାର(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    ଯଦି (hwndTip == ଖାଲି)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, ଖାଲି,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ଖାଲି, GetModuleHandle(ଖାଲି), ଖାଲି);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    ଅନ୍ୟଥା
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    ସ୍ଥିର ଶୂନ୍ୟ
destroy_sizing_tip(ଶୂନ୍ୟ)
{
    ଯଦି (hwndTip == ଖାଲି)
	ଫେରନ୍ତୁ;

    DestroyWindow(hwndTip);
    hwndTip = ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    w, h;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    valid_w, valid_h;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    w_offset, h_offset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    cols, rows;

    w = lprc->ଡାହାଣ - lprc->ବାମ;
    h = lprc->bottom - lprc->ଶୀର୍ଷ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    ଯଦି (fwSide == WMSZ_ବାମ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ବାମ += w_offset;
    ଅନ୍ୟଥା ଯଦି (fwSide == WMSZ_ଡାହାଣ || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ଡାହାଣ -= w_offset;

    ଯଦି (fwSide == WMSZ_ଶୀର୍ଷ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->ଶୀର୍ଷ += h_offset;
    ଅନ୍ୟଥା ଯଦି (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    ଫେରନ୍ତୁ TRUE;
}

#ifdef FEAT_GUI_TABLINE
    ସ୍ଥିର ଶୂନ୍ୟ
_OnRButtonUp(HWND hwnd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, UINT ଚାବି_Flags)
{
    ଯଦି (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	ଯଦି (pt.y < rect.ଶୀର୍ଷ)
	{
	    show_tabline_popup_menu();
	    ଫେରନ୍ତୁ;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, ଚାବି_Flags, DefWindowProcW);
}

    ସ୍ଥିର ଶୂନ୍ୟ
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, UINT ଚାବି_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    ଯଦି (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	ଯଦି (pt.y < rect.ଶୀର୍ଷ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, ଚାବି_Flags, DefWindowProcW);
}
#endif

    ସ୍ଥିର UINT
_OnNCHitTest(HWND hwnd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା xPos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା yPos)
{
    UINT	result;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    ଯଦି (result != HTCLIENT)
	ଫେରନ୍ତୁ result;

#ifdef FEAT_GUI_TABLINE
    ଯଦି (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	ଯଦି (yPos < rct.ଶୀର୍ଷ)
	    ଫେରନ୍ତୁ result;
    }
#endif
    (ଶୂନ୍ୟ)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    ଯଦି (xPos < 48) // <VN> TODO should use system metric?
	ଫେରନ୍ତୁ HTBOTTOMLEFT;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ HTBOTTOMRIGHT;
}

#ଯଦି defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ସ୍ଥିର LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    ବାଛ (hdr->code)
    {
	ସ୍ଥିତି TTN_GETDISPINFOW:
	ସ୍ଥିତି TTN_GETDISPINFO:
	    {
		char_u		*str = ଖାଲି;
		ସ୍ଥିର ଶୂନ୍ୟ	*tt_text = ଖାଲି;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		ଯଦି (gui_mch_showing_tabline()
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
		    ଯଦି (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			ଯଦି (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    ଯଦି (tp != ଖାଲି)
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
		ଅନ୍ୟଥା
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(ମୂଳ_menu, idButton);
		    ଯଦି (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		ଯଦି (str == ଖାଲି)
		    ଭାଙ୍ଗ;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		ଯଦି (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, ଖାଲି);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		ଅନ୍ୟଥା
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    ଯଦି (len < ଆକାର(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == ଖାଲି))
			vim_strncpy((char_u *)lpdi->szText, str,
				ଆକାର(lpdi->szText) - 1);
		    ଅନ୍ୟଥା
			lpdi->lpszText = tt_text;
		}
	    }
	    ଭାଙ୍ଗ;

# ifdef FEAT_GUI_TABLINE
	ସ୍ଥିତି TCN_SELCHANGE:
	    ଯଦି (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ଫେରନ୍ତୁ 0L;
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି NM_RCLICK:
	    ଯଦି (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ଫେରନ୍ତୁ 0L;
	    }
	    ଭାଙ୍ଗ;
# endif

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#ଯଦି defined(MENUHINTS) && defined(FEAT_MENU)
    ସ୍ଥିର LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ଯଦି (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  did_menu_tip = FALSE;

	ଯଦି (did_menu_tip)
	{
	    ସନ୍ଦେଶ_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(ମୂଳ_menu, (UINT)LOWORD(wParam));
	ଯଦି (pMenu != ଖାଲି && pMenu->strings[MENU_INDEX_TIP] != ଖାଲି)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = ଆକାର(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    ଯଦି (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++ସନ୍ଦେଶ_hist_off;
		ସନ୍ଦେଶ((ଅକ୍ଷର *)pMenu->strings[MENU_INDEX_TIP]);
		--ସନ୍ଦେଶ_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	ଫେରନ୍ତୁ 0L;
    }
    ଫେରନ୍ତୁ DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    ସ୍ଥିର BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		old_width, old_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, ଆକାର(lf), &lf);
    lf.lfHeight = lf.lfHeight * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    ଯଦି (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    ଅନ୍ୟଥା
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର LRESULT
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
    InvalidateRect(hwnd, ଖାଲି, TRUE);

    s_in_dpichanged = FALSE;
    ଫେରନ୍ତୁ 0L;
}


    ସ୍ଥିର LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_ସନ୍ଦେଶ, lParam);

    s_u_ସନ୍ଦେଶ = u_ସନ୍ଦେଶ;
    s_wParam = wParam;

    ବାଛ (u_ସନ୍ଦେଶ)
    {
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_ସନ୍ଦେଶ(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    ସ୍ଥିତି WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ଫେରନ୍ତୁ FALSE;		// Do NOT allow system to go down.

    ସ୍ଥିତି WM_ENDSESSION:
	ଯଦି (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    ଫେରନ୍ତୁ 0L;
	}
	ଭାଙ୍ଗ;

    ସ୍ଥିତି WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)LOWORD(lParam));
	ଫେରନ୍ତୁ 0L;

    ସ୍ଥିତି WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	ଯଦି (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(short)LOWORD(lParam));
	    ଫେରନ୍ତୁ 0L;
	}
#ifdef FEAT_MENU
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
#endif

    ସ୍ଥିତି WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ 0L;
#endif

    ସ୍ଥିତି WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	ଭାଙ୍ଗ;

    ସ୍ଥିତି WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ଫେରନ୍ତୁ _DuringSizing((UINT)wParam, (LPRECT)lParam);

    ସ୍ଥିତି WM_MOUSEWHEEL:
    ସ୍ଥିତି WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_ସନ୍ଦେଶ == WM_MOUSEHWHEEL);
	ଫେରନ୍ତୁ 0L;

	// Notification for change in SystemParametersInfo()
    ସ୍ଥିତି WM_SETTINGCHANGE:
	ଫେରନ୍ତୁ _OnSettingChange((UINT)wParam);

#ଯଦି defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ସ୍ଥିତି WM_NOTIFY:
	ଫେରନ୍ତୁ _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#ଯଦି defined(MENUHINTS) && defined(FEAT_MENU)
    ସ୍ଥିତି WM_MENUSELECT:
	ଫେରନ୍ତୁ _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    ସ୍ଥିତି WM_IME_NOTIFY:
	ଯଦି (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
	ଫେରନ୍ତୁ 1L;

    ସ୍ଥିତି WM_IME_COMPOSITION:
	ଯଦି (!_OnImeComposition(hwnd, wParam, lParam))
	    ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
	ଫେରନ୍ତୁ 1L;
#endif
    ସ୍ଥିତି WM_GETDPISCALEDSIZE:
	ଫେରନ୍ତୁ _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    ସ୍ଥିତି WM_DPICHANGED:
	ଫେରନ୍ତୁ _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ifdef MSWIN_FIND_REPLACE
	ଯଦି (u_ସନ୍ଦେଶ == s_findrep_ସନ୍ଦେଶ && s_findrep_ସନ୍ଦେଶ != 0)
	    _OnFindRepl();
#endif
	ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ DefWindowProcW(hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = ଖାଲି;

    ସ୍ଥିର BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    ଅକ୍ଷର	buf[2048];
    ଅକ୍ଷର	*title = (ଅକ୍ଷର *)lParam;

    ଯଦି (GetWindowText(hwnd, buf, ଆକାର(buf)))
    {
	ଯଦି (strstr(buf, title) != ଖାଲି)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, ଖାଲି, "MDIClient", ଖାଲି);
	    ଯଦି (vim_parent_hwnd != ଖାଲି)
		ଫେରନ୍ତୁ FALSE;
	}
    }
    ଫେରନ୍ତୁ TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    ଶୂନ୍ୟ
gui_mch_set_parent(ଅକ୍ଷର *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    ଯଦି (vim_parent_hwnd == ଖାଲି)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    ସ୍ଥିର ଶୂନ୍ୟ
ole_error(ଅକ୍ଷର *arg)
{
    ଅକ୍ଷର buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#ଯଦି defined(GUI_MAY_SPAWN)
    ସ୍ଥିର ଅକ୍ଷର *
gvim_error(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର *ସନ୍ଦେଶ = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    ଯଦି (starting)
    {
	mch_errmsg(ସନ୍ଦେଶ);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ଫେରନ୍ତୁ ସନ୍ଦେଶ;
}

    ଅକ୍ଷର *
gui_mch_do_spawn(char_u *arg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
# ଯଦି defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ଖାଲି;
    LPWSTR		tofree1 = ଖାଲି;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = ଖାଲି, p, warg, tofree2 = ଖାଲି;
    STARTUPINFOW	si = {ଆକାର(si)};
    PROCESS_INFORMATION pi;

    ଯଦି (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	ଯାଅ error;
    p = wcsrchr(name, L'\\');
    ଯଦି (p == ଖାଲି)
	ଯାଅ error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# ଅନ୍ୟଥା
    wcscpy(p + 1, L"gvim.exe");
# endif

# ଯଦି defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ଯଦି (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_ଧାଡ଼ି_W();
	// Skip 1st argument.
	ଯେତେବେଳେ (*p && *p != L' ' && *p != L'\t')
	{
	    ଯଦି (*p == L'"')
	    {
		// Skip quoted strings
		ଯେତେବେଳେ (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# ଯଦି defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ଅନ୍ୟଥା
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret;

	session = vim_tempname('s', FALSE);
	ଯଦି (session == ଖାଲି)
	    ଯାଅ error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	ଯଦି (p_bg == ଖାଲି)
	{
	    p_bg = savebg;
	    ଯାଅ error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	ଯଦି (!ret)
	    ଯାଅ error;
	wsession = enc_to_utf16(session, ଖାଲି);
	ଯଦି (wsession == ଖାଲି)
	    ଯାଅ error;
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	ଯଦି (cmd == ଖାଲି)
	{
	    vim_free(wsession);
	    ଯାଅ error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    ଯଦି (arg != ଖାଲି)
    {
	warg = enc_to_utf16(arg, ଖାଲି);
	ଯଦି (warg == ଖାଲି)
	    ଯାଅ error;
	tofree2 = warg;
    }
    ଅନ୍ୟଥା
	warg = L"";

    // Set up the new command line.
    len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(name) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(cmd) + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    ଯଦି (newcmd == ଖାଲି)
	ଯାଅ error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    ଯଦି (!CreateProcessW(ଖାଲି, newcmd, ଖାଲି, ଖାଲି, TRUE, 0,
		ଖାଲି, ଖାଲି, &si, &pi))
	ଯାଅ error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# ଯଦି defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ଯଦି (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ଫେରନ୍ତୁ gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    ଶୂନ୍ୟ
gui_mch_prepare(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argc, ଅକ୍ଷର **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    // Check for special OLE command line parameters
    ଯଦି ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	ଯଦି (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	ଅନ୍ୟଥା
	    idx = 1;

	// Register Vim as an OLE Automation server
	ଯଦି (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#ଅନ୍ୟଥା
	    ଯଦି (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	ଯଦି (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#ଅନ୍ୟଥା
	    ଯଦି (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	ଯଦି (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#ଅନ୍ୟଥା
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    ଯଦି (mch_is_gui_executable())
# endif
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	ଯଦି (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg;

	ପାଇଁ (arg = 1; arg < *argc; arg++)
	    ଯଦି (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ଆକାର(ଅକ୍ଷର *));
		argv[*argc] = ଖାଲି;
		ଭାଙ୍ଗ;	// enough?
	    }
    }
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
load_dpi_func(ଶୂନ୍ୟ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    ଯଦି (hUser32 == ଖାଲି)
	ଯାଅ fail;

    pGetDpiForSystem = (UINT (WINAPI *)(ଶୂନ୍ୟ))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା (WINAPI *)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    ଯଦି (pSetThreadDpiAwarenessContext != ଖାଲି)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	ଯଦି (oldctx != ଖାଲି)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    ଯଦି (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    ଫେରନ୍ତୁ;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = ଖାଲି;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = ଖାଲି;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ଖାଲି;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init(ଶୂନ୍ୟ)
{
    ଅଚଳ WCHAR szVimWndClassW[] = VIM_CLASSW;
    ଅଚଳ WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    ଯଦି (s_hwnd != ଖାଲି)
	ଯାଅ theend;

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
    ଯଦି (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(ଖାଲି, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ଖାଲି;
	wndclassw.lpszClassName = szVimWndClassW;

	ଯଦି (RegisterClassW(&wndclassw) == 0)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (vim_parent_hwnd != ଖାଲି)
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
		vim_parent_hwnd, ଖାଲି,
		g_hinst, ଖାଲି);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	ଯଦି (s_hwnd == ଖାଲି)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    ଅନ୍ୟଥା
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	ଯଦି (IsWindow((HWND)win_socket_id) <= 0)
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
		ଖାଲି, ଖାଲି,
		g_hinst, ଖାଲି);
	ଯଦି (s_hwnd != ଖାଲି && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    ଯଦି (s_hwnd == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (pGetDpiForWindow != ଖାଲି)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#ଯଦି defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    ଯଦି (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ଖାଲି;
	wndclassw.hCursor = LoadCursor(ଖାଲି, IDC_ARROW);
	wndclassw.hbrBackground = ଖାଲି;
	wndclassw.lpszMenuName = ଖାଲି;
	wndclassw.lpszClassName = szTextAreaClassW;

	ଯଦି (RegisterClassW(&wndclassw) == 0)
	    ଫେରନ୍ତୁ FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ଖାଲି,
	g_hinst, ଖାଲି);

    ଯଦି (s_textArea == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ଖାଲି;

	ଯଦି (mch_icon_load(&hIcon) == OK && hIcon != ଖାଲି)
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
    s_findrep_ସନ୍ଦେଶ = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = ଆକାର(s_findrep_struct);
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
    ଯଦି (p_rop)
	(ଶୂନ୍ୟ)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    ଫେରନ୍ତୁ OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    ଯଦି (mon != ଖାଲି)
    {
	moninfo.cbSize = ଆକାର(MONITORINFO);
	ଯଦି (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ଫେରନ୍ତୁ;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    ଶୂନ୍ୟ
gui_mch_set_shellsize(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_width UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_height UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_width UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_height UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    ଯଦି (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    ଯଦି (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    ଅନ୍ୟଥା
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
    window_rect.ଡାହାଣ = window_rect.ବାମ + win_width;
    window_rect.bottom = window_rect.ଶୀର୍ଷ + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    ଯଦି (!s_in_dpichanged)
    {
	ଯଦି ((direction & RESIZE_HOR)
		&& window_rect.ଡାହାଣ > workarea_rect.ଡାହାଣ)
	    OffsetRect(&window_rect,
		    workarea_rect.ଡାହାଣ - window_rect.ଡାହାଣ, 0);

	ଯଦି ((direction & RESIZE_HOR)
		&& window_rect.ବାମ < workarea_rect.ବାମ)
	    OffsetRect(&window_rect,
		    workarea_rect.ବାମ - window_rect.ବାମ, 0);

	ଯଦି ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	ଯଦି ((direction & RESIZE_VERT)
		&& window_rect.ଶୀର୍ଷ < workarea_rect.ଶୀର୍ଷ)
	    OffsetRect(&window_rect,
		    0, workarea_rect.ଶୀର୍ଷ - window_rect.ଶୀର୍ଷ);
    }

    MoveWindow(s_hwnd, window_rect.ବାମ, window_rect.ଶୀର୍ଷ,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    ଶୂନ୍ୟ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    ଯେତେବେଳେ (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    ଯଦି (sb->scroll_shift > 0)
	++size;

    info.cbSize = ଆକାର(info);
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
    ଶୂନ୍ୟ
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    ଶୂନ୍ୟ
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    ଶୂନ୍ୟ
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    ଶୂନ୍ୟ
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
    ସ୍ଥିର LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    ଯଦି (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ଫେରନ୍ତୁ lResult;
    ବାଛ (dwCommand)
    {
	ସ୍ଥିତି IMN_SETOPENSTATUS:
	    ଯଦି (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		ଯଦି (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		ଯଦି (State & MODE_ଭର୍ତ୍ତି)
		{
# ଯଦି defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    ଯଦି (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_row = gui.row;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_col = gui.col;

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
	    ଭାଙ୍ଗ;
    }
    pImmReleaseContext(hWnd, hImc);
    ଫେରନ୍ତୁ lResult;
}

    ସ୍ଥିର LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;

    ଯଦି ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ଫେରନ୍ତୁ 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    ଯଦି (ret != ଖାଲି)
    {
	add_to_ନିବେଶ_buf_csi(ret, len);
	vim_free(ret);
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ସ୍ଥିର char_u *
GetResultStr(HWND hwnd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା GCS, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ଖାଲି;
    char_u	*convbuf = ଖାଲି;

    ଯଦି (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ଖାଲି, 0);
    ଯଦି (ret <= 0)
	ଫେରନ୍ତୁ ଖାଲି;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + ଆକାର(WCHAR));
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / ଆକାର(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ଫେରନ୍ତୁ convbuf;
}
#endif

// For global functions we need prototypes.
#ଯଦି defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    ଶୂନ୍ୟ
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    ଯଦି (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    ଶୂନ୍ୟ
im_set_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
    HIMC hImc;

    ଯଦି (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	ଯଦି (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    ଶୂନ୍ୟ
im_set_active(ପୂର୍ଣ୍ଣସଂଖ୍ୟା active)
{
    HIMC	hImc;
    ସ୍ଥିର HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    ଯଦି (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	ଫେରନ୍ତୁ;
    }
# endif

    ଯଦି (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ଫେରନ୍ତୁ;

    ଯଦି (p_imdisable)
    {
	ଯଦି (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    ଯଦି (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    ଅନ୍ୟଥା ଯଦି (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    ଯଦି (!hImc)
	ଫେରନ୍ତୁ;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    ଯଦି (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ସ୍ଥିର DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ସ୍ଥିର BOOL bSaved = FALSE;

	ଯଦି (active)
	{
	    // if we have a saved conversion status, restore it
	    ଯଦି (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	ଅନ୍ୟଥା
	{
	    // save conversion status and disable korean
	    ଯଦି (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
im_get_status(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    ଯଦି (!gui.in_use && !gui.starting)
	ଫେରନ୍ତୁ mbyte_im_get_status();
# endif

    ଯଦି (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ଫେରନ୍ତୁ status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
latin9_to_ucs(char_u *text, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, WCHAR *unicodebuf)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

    ଯେତେବେଳେ (--len >= 0)
    {
	c = *text++;
	ବାଛ (c)
	{
	    ସ୍ଥିତି 0xa4: c = 0x20ac; ଭାଙ୍ଗ;   // euro
	    ସ୍ଥିତି 0xa6: c = 0x0160; ଭାଙ୍ଗ;   // S hat
	    ସ୍ଥିତି 0xa8: c = 0x0161; ଭାଙ୍ଗ;   // S -hat
	    ସ୍ଥିତି 0xb4: c = 0x017d; ଭାଙ୍ଗ;   // Z hat
	    ସ୍ଥିତି 0xb8: c = 0x017e; ଭାଙ୍ଗ;   // Z -hat
	    ସ୍ଥିତି 0xbc: c = 0x0152; ଭାଙ୍ଗ;   // OE
	    ସ୍ଥିତି 0xbd: c = 0x0153; ଭାଙ୍ଗ;   // oe
	    ସ୍ଥିତି 0xbe: c = 0x0178; ଭାଙ୍ଗ;   // Y
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
    ସ୍ଥିର ଶୂନ୍ୟ
RevOut( HDC hdc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ix;

    ପାଇଁ (ix = 0; ix < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
draw_ଧାଡ଼ି(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y2,
    COLORREF	color)
{
#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_ଧାଡ଼ି(s_dwc, x1, y1, x2, y2, color);
    ଅନ୍ୟଥା
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, ଖାଲି);
	// Note: LineTo() excludes the last pixel in the line.
	ଧାଡ଼ି_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_pixel(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y,
    COLORREF	color)
{
#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    ଅନ୍ୟଥା
#endif
	SetPixel(s_hdc, x, y, color);
}

    ସ୍ଥିର ଶୂନ୍ୟ
fill_rect(
    ଅଚଳ RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    ଅନ୍ୟଥା
#endif
    {
	HBRUSH	hbr2;

	ଯଦି (hbr == ଖାଲି)
	    hbr2 = CreateSolidBrush(color);
	ଅନ୍ୟଥା
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	ଯଦି (hbr == ଖାଲି)
	    DeleteBrush(hbr2);
    }
}

    ଶୂନ୍ୟ
gui_mch_draw_string(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    char_u	*text,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*padding = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	pad_size = 0;
    ଅଚଳ RECT	*pcliprect = ଖାଲି;
    UINT	foptions = 0;
    ସ୍ଥିର WCHAR *unicodebuf = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା   *unicodepdy = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	unibuflen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;

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
    ସ୍ଥିର HBRUSH	hbr_cache[2] = {ଖାଲି, ଖାଲି};
    ସ୍ଥିର guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    ଯଦି (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ବାମ = FILL_X(col);
	rc.ଶୀର୍ଷ = FILL_Y(row);
	ଯଦି (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ଡାହାଣ = FILL_X(col + mb_string2cells(text, len));
	}
	ଅନ୍ୟଥା
	    rc.ଡାହାଣ = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	ଯଦି (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	ଅନ୍ୟଥା ଯଦି (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (hbr_cache[brush_lru] != ଖାଲି)
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
	ଯଦି (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	ଅନ୍ୟଥା ଯଦି (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    ଯଦି (pad_size != Columns || padding == ଖାଲି || padding[0] != gui.char_width)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, pad_size);
	ଯଦି (padding != ଖାଲି)
	    ପାଇଁ (i = 0; i < pad_size; i++)
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
    ଯଦି (enc_utf8)
	ପାଇଁ (n = 0; n < len; ++n)
	    ଯଦି (text[n] >= 0x80)
		ଭାଙ୍ଗ;

#ଯଦି defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    ଯଦି (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    ଯଦି ((enc_utf8
		|| (enc_codepage > 0 && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ଖାଲି || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, len);

	unibuflen = len;
    }

    ଯଦି (enc_utf8 && n < len && unicodebuf != ଖାଲି)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wlen;	// string length in words
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cells;	// cell width of string up to composing char
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cw;	// width of current cell
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		c;

	wlen = 0;
	cells = 0;
	ପାଇଁ (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    ଯଦି (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    ଅନ୍ୟଥା
	    {
		unicodebuf[wlen++] = c;
	    }

	    ଯଦି (utf_iscomposing(c))
		cw = 0;
	    ଅନ୍ୟଥା
	    {
		cw = utf_char2cells(c);
		ଯଦି (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    ଯଦି (unicodepdy != ଖାଲି)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ଯଦି (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		ଅନ୍ୟଥା
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#ଯଦି defined(FEAT_DIRECTX)
	ଯଦି (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	ଅନ୍ୟଥା
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    ଅନ୍ୟଥା ଯଦି ((enc_codepage > 0 && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	ଯଦି (unicodebuf != ଖାଲି)
	{
	    ଯଦି (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    ଅନ୍ୟଥା
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(ଅକ୍ଷର *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    ଯଦି (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ଯଦି (unicodepdy != ଖାଲି)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cw;

		    ପାଇଁ (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			ଯଦି (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	ଯଦି (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (ଅକ୍ଷର *)text, len, padding);
	ଅନ୍ୟଥା
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (ଅକ୍ଷର *)text, len, padding);
    }

    // Underline
    ଯଦି (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	ଯଦି (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_ଧାଡ଼ି(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    ଯଦି (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_ଧାଡ଼ି(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    ଯଦି (flags & DRAW_UNDERC)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା			x;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା			offset;
	ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	ପାଇଁ (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    ଶୂନ୍ୟ
gui_mch_flush(ଶୂନ୍ୟ)
{
#ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ଖାଲି, gui.back_pixel);
}


    ଶୂନ୍ୟ
gui_mch_get_screen_dimensions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ଡାହାଣ - workarea_rect.ବାମ
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.ଶୀର୍ଷ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#ଯଦି defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    ଶୂନ୍ୟ
gui_mch_add_menu(
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    ଯଦି (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, ଖାଲି);
	ଯଦି (wn == ଖାଲି)
	    ଫେରନ୍ତୁ;

	infow.cbSize = ଆକାର(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ଭର୍ତ୍ତି_Menu_Item_W((parent == ଖାଲି)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    ଯଦି (parent == ଖାଲି)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    ଅନ୍ୟଥା ଯଦି (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    ଶୂନ୍ୟ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (ଶୂନ୍ୟ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mp.x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mp.y);
}

    ଶୂନ୍ୟ
gui_make_popup(char_u *path_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    ଯଦି (menu == ଖାଲି)
	ଫେରନ୍ତୁ;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    ଯଦି (mouse_pos)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    ଅନ୍ୟଥା ଯଦି (curwin != ଖାଲି)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    ସନ୍ଦେଶ_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p.x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p.y);
}

# ଯଦି defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    ଶୂନ୍ୟ
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    ଯଦି (menu != ଖାଲି)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    ଶୂନ୍ୟ
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = ଖାଲି;

# ifdef FEAT_TEAROFF
    ଯଦି (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ଭର୍ତ୍ତି_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    ଅନ୍ୟଥା
# endif
# ifdef FEAT_TOOLBAR
    ଯଦି (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	ଯଦି (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	ଅନ୍ୟଥା
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
    ଅନ୍ୟଥା
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, ଖାଲି);
	ଯଦି (wn != ଖାଲି)
	{
	    ଭର୍ତ୍ତି_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	ଯଦି (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    ଶୂନ୍ୟ
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ଯଦି (menu->submenu_id == (HMENU)-1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା iButton;

	iButton = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    ଅନ୍ୟଥା
# endif
    {
	ଯଦି (menu->parent != ଖାଲି
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != ଖାଲି)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	ଅନ୍ୟଥା
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	ଯଦି (menu->submenu_id != ଖାଲି)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	ଯଦି (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	ଯଦି (menu->parent != ଖାଲି
		&& menu->parent->children != ଖାଲି
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
    ସ୍ଥିର ଶୂନ୍ୟ
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    ଯଦି (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ବାମ - rct.ବାମ;
	y = (trect.ଶୀର୍ଷ -  rct.bottom  + roct.bottom);
    }
    ଅନ୍ୟଥା
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    ଯଦି (menu->children != ଖାଲି)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	ଯଦି (IsWindow(menu->tearoff_handle))
	    (ଶୂନ୍ୟ) SetWindowPos(menu->tearoff_handle,
				ଖାଲି,
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)trect.ବାମ,
				(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)trect.ଶୀର୍ଷ,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    ଶୂନ୍ୟ
gui_mch_menu_grey(
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ଯଦି (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    ଅନ୍ୟଥା
# endif
	(ଶୂନ୍ୟ)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    ଯଦି ((menu->parent != ଖାଲି) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	ଯଦି (menu->children == ଖାଲି)
	    menuID = (WORD)(menu->id);
	ଅନ୍ୟଥା
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	ଯଦି (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#ଯଦି defined(FEAT_GUI_DIALOG)
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
    ସ୍ଥିର LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    ଯଦି (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(ଶୂନ୍ୟ)SetFocus(hwnd);
	ଯଦି (dialog_default_button > IDCANCEL)
	    (ଶୂନ୍ୟ)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	ଅନ୍ୟଥା
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (ଶୂନ୍ୟ)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	ଫେରନ୍ତୁ FALSE;
    }

    ଯଦି (message == WM_COMMAND)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	ଯଦି (button >= DLG_NONBUTTON_CONTROL)
	    ଫେରନ୍ତୁ TRUE;

	// If the edit box exists, copy the string.
	ଯଦି (s_textfield != ଖାଲି)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, ଖାଲି);
	    ଯଦି (p != ଖାଲି)
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
	ଯଦି (button == IDOK)
	{
	    ଯଦି (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	ଅନ୍ୟଥା
	    EndDialog(hwnd, button - IDCANCEL);
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
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

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_dialog(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		 dfltbutton,
    char_u	*textfield,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_Buttons;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*buttonWidths, *buttonPositions;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buttonYpos;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nchar, i;
    DWORD	lStyle;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlgwidth = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlgheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		editboxheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		horizWidth = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fontHeight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		textWidth, minButtonWidth, messageWidth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxDialogWidth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxDialogHeight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		scroll_flag = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vertical;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlgPaddingX;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlg_icon_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dlg_icon_height;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    ଯଦି (!(gui.in_use || gui.starting))
#  endif
	ଯଦି (silent_mode)
	    ଫେରନ୍ତୁ dfltbutton;   // return default option
# endif

    ଯଦି (s_hwnd == ଖାଲି)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    ଅନ୍ୟଥା
	dpi = pGetDpiForSystem();

    ଯଦି ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    ଯଦି (tbuffer == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    ଅଙ୍କ_Buttons = 1;
    ପାଇଁ (i = 0; tbuffer[i] != '\0'; i++)
    {
	ଯଦି (tbuffer[i] == DLG_BUTTON_SEP)
	    ଅଙ୍କ_Buttons++;
    }
    ଯଦି (dfltbutton >= ଅଙ୍କ_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଙ୍କ_Buttons);
    ଯଦି (buttonWidths == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ଅଙ୍କ_Buttons);
    ଯଦି (buttonPositions == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    ଯଦି (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ଅନ୍ୟଥା
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
    ଯଦି (s_hwnd == ଖାଲି)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.ଡାହାଣ - workarea_rect.ବାମ - 100;
	ଯଦି (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.ଶୀର୍ଷ - 150;
    }
    ଅନ୍ୟଥା
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.ଡାହାଣ - rect.ବାମ
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	ଯଦି (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.ଶୀର୍ଷ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	ଯଦି (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 500);
    କର
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = ଖାଲି;
	ପାଇଁ (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    ଯଦି (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    ଯଦି (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		ଯଦି (last_white != ଖାଲି)
		{
		    // break the line just after a space
		    ଯଦି (pend > last_white)
			ga.ga_len -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = ଖାଲି;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ଜାରି;
	    }

	    ଯେତେବେଳେ (--l >= 0)
		ga_append(&ga, *pend++);
	}
	ଯଦି (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } ଯେତେବେଳେ (*pend != NUL);

    ଯଦି (ga.ga_data != ଖାଲି)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    ଯଦି (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ଖାଲି && vim_strchr(p_go, GO_VERTICAL) != ଖାଲି);
    ଯଦି (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	କର
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ଯଦି (pend == ଖାଲି)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(pend - pstart));
	    ଯଦି (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} ଯେତେବେଳେ (*pend != NUL);

	ଯଦି (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	ଅନ୍ୟଥା ଯଦି (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    ଯଦି (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	କର
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ଯଦି (pend == ଖାଲି)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    ଯଦି (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} ଯେତେବେଳେ (*pend != NUL);
    }

    ଯଦି (dlgwidth < DLG_MIN_WIDTH)
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
    ଯଦି (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * ଅଙ୍କ_Buttons;
    ଅନ୍ୟଥା
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    ଯଦି (textfield != ଖାଲି)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    ଯଦି (dlgheight > maxDialogHeight)
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
    ଯଦି (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ଅନ୍ୟଥା
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    ଯଦି (textfield != ଖାଲି)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    ଯଦି (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    ପାଇଁ (i = 0; i < ଅଙ୍କ_Buttons; i++)
    {
	// get end of this button.
	ପାଇଁ (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	ଯଦି (*pend)
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
	ଯଦି (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (ଅକ୍ଷର *)pstart);
	}
	ଅନ୍ୟଥା
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (ଅକ୍ଷର *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += ଅଙ୍କ_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_ବାମ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (ଅକ୍ଷର *)message);

    // Edit box
    ଯଦି (textfield != ଖାଲି)
    {
	p = add_dialog_element(p, ES_ବାମ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (ଅକ୍ଷର *)textfield);
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
    ଯଦି (textfield != ଖାଲି)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    ଅନ୍ୟଥା
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ଖାଲି;
    }

    // show the dialog box modally and get a return value
    nchar = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)DialogBoxIndirect(
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
    (ଶୂନ୍ୟ)SetFocus(s_hwnd);

    ଫେରନ୍ତୁ nchar;
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
    ସ୍ଥିର PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    ଅଚଳ ଅକ୍ଷର *caption)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nchar;

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

    ଫେରନ୍ତୁ p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ସ୍ଥିର LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ଫେରନ୍ତୁ (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nChar = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    WCHAR	*wn;

    ଯଦି (use_enc && enc_codepage >= 0 && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, ଖାଲି);
	ଯଦି (wn != ଖାଲି)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    ଯଦି (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    ପାଇଁ (i = 0; i < nChar; ++i)
	ଯଦି (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    ଫେରନ୍ତୁ nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ସ୍ଥିର HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    ପାଇଁ ( ; menu != ଖାଲି; menu = menu->next)
    {
	ଯଦି (menu->modes == 0)	// this menu has just been deleted
	    ଜାରି;
	ଯଦି (menu_is_separator(menu->dname))
	    ଜାରି;
	ଯଦି ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ଫେରନ୍ତୁ menu->submenu_id;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ସ୍ଥିର LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    ଯଦି (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ଫେରନ୍ତୁ (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    ଯଦି (message == WM_COMMAND)
    {
	ଯଦି ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    ଯଦି (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(ଶୂନ୍ୟ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)rect.ଡାହାଣ - 8,
			 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)mp.y,
			 (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)0,	    // reserved param
			 s_hwnd,
			 ଖାଲି);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	ଅନ୍ୟଥା
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(ଶୂନ୍ୟ)SetFocus(s_hwnd);
	ଫେରନ୍ତୁ TRUE;
    }
    ଯଦି ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	ଫେରନ୍ତୁ TRUE;
    }

    // When moved around, give main window the focus back.
    ଯଦି (message == WM_EXITSIZEMOVE)
	(ଶୂନ୍ୟ)SetActiveWindow(s_hwnd);

    ଫେରନ୍ତୁ FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_dialog_font_metrics(ଶୂନ୍ୟ)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    ଯଦି (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    ଅନ୍ୟଥା
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

#ଯଦି defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		initX,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		template_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*ଶୀର୍ଷ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col, spaceWidth, len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		columnWidths[2];
    char_u	*label, *text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		acLen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nameLen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		padding0, padding1, padding2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sepPadding=0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    ଯଦି (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	ଯଦି (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, ଖାଲି, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ଫେରନ୍ତୁ;
    }

    /*
     * Create a new tearoff.
     */
    ଯଦି (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    ଯଦି (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ଅନ୍ୟଥା
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
    ପାଇଁ (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    ଯଦି (text != ଖାଲି && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(text));
		ଯଦି (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    ଯଦି (pmenu->children != ଖାଲି)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    ଯଦି (columnWidths[1] == 0)
    {
	// no accelerators
	ଯଦି (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	ଅନ୍ୟଥା
	    columnWidths[0] += spaceWidth;
    }
    ଅନ୍ୟଥା
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    ଯଦି (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(title));
    ଯଦି (textWidth > dlgwidth)
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
    ଯଦି (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    ଅନ୍ୟଥା
	*p++ = PixelToDialogX(initX); // x
    ଯଦି (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    ଅନ୍ୟଥା
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
    ଯଦି (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ଅନ୍ୟଥା
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
    ଯଦି (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    ଅନ୍ୟଥା
	menu = menu->children;
    ଶୀର୍ଷ_menu = menu;
    ପାଇଁ ( ; menu != ଖାଲି; menu = menu->next)
    {
	ଯଦି (menu->modes == 0)	// this menu has just been deleted
	    ଜାରି;
	ଯଦି (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ଜାରି;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	ଯଦି (((ଅକ୍ଷର *)p - (ଅକ୍ଷର *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    ଯଦି (newp != ଖାଲି)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (ଅକ୍ଷର *)p - (ଅକ୍ଷର *)pdlgtemplate);
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
	len = nameLen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	ଯଦି (menu->actext != ଖାଲି)
	{
	    acLen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	ଅନ୍ୟଥା
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	ଯଦି (menu->children == ଖାଲି)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	ଅନ୍ୟଥା
	{
	    len += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	ଯଦି (label == ଖାଲି)
	    ଭାଙ୍ଗ;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	ଯଦି (text == ଖାଲି)
	    text = label + nameLen;	    // no actext, use whole name
	ଯେତେବେଳେ (padding0-- > 0)
	    *text++ = ' ';
	ଯଦି (menu->actext != ଖାଲି)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	ଯେତେବେଳେ (padding1-- > 0)
	    *text++ = ' ';
	ଯଦି (menu->children != ଖାଲି)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	ଅନ୍ୟଥା
	{
	    ଯେତେବେଳେ (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_ବାମ,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (ଅକ୍ଷର *)label);
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
	    (LPARAM)ଶୀର୍ଷ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (ଶୂନ୍ୟ)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#ଯଦି defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ସ୍ଥିର ଶୂନ୍ୟ
initialise_toolbar(ଶୂନ୍ୟ)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ଖାଲି,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    ଆକାର(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ଖାଲି);

    update_toolbar_size();
}

    ସ୍ଥିର ଶୂନ୍ୟ
update_toolbar_size(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = ଆକାର(TBMETRICS);
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

    ସ୍ଥିର LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_ସନ୍ଦେଶ, lParam);
    ଫେରନ୍ତୁ CallWindowProc(s_toolbar_wndproc, hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_toolbar_bitmap(vimmenu_T *menu)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    ଯଦି (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ଖାଲି;

	ଯଦି (menu->iconfile != ଖାଲି)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			ଖାଲି,
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
	ଯଦି (hbitmap == ଖାଲି
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ଖାଲି ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    ଖାଲି,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	ଯଦି (hbitmap != ଖାଲି)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = ଖାଲି;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    ଯଦି (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_ଗଣନା)
	i = menu->iconidx;

    ଫେରନ୍ତୁ i;
}
#endif

#ଯଦି defined(FEAT_GUI_TABLINE)
    ସ୍ଥିର ଶୂନ୍ୟ
initialise_tabline(ଶୂନ୍ୟ)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ଖାଲି, g_hinst, ଖାଲି);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ସ୍ଥିର tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = ଖାଲି;

    ଯଦି (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	ଯଦି (s_tabhwnd == hWnd)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    ଯଦି (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    ଫେରନ୍ତୁ ptp;
}

ସ୍ଥିର POINT	    s_pt = {0, 0};
ସ୍ଥିର HCURSOR      s_hCursor = ଖାଲି;

    ସ୍ଥିର LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_ସନ୍ଦେଶ,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nCenter;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx1;

    HandleMouseHide(u_ସନ୍ଦେଶ, lParam);

    ବାଛ (u_ସନ୍ଦେଶ)
    {
	ସ୍ଥିତି WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		ଭାଙ୍ଗ;
	    }
	ସ୍ଥିତି WM_MOUSEMOVE:
	    ଯଦି (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		ଯଦି (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(ଖାଲି, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    ଯଦି (tp != ଖାଲି)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.ବାମ + (rect.ଡାହାଣ - rect.ବାମ) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			ଯଦି ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			ଅନ୍ୟଥା ଯଦି ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି WM_LBUTTONUP:
	    {
		ଯଦି (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		ଭାଙ୍ଗ;
	    }
	ସ୍ଥିତି WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		ଯଦି (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		ଭାଙ୍ଗ;
	    }
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ CallWindowProc(s_tabline_wndproc, hwnd, u_ସନ୍ଦେଶ, wParam, lParam);
}
#endif

#ଯଦି defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    ଶୂନ୍ୟ
gui_mch_set_foreground(ଶୂନ୍ୟ)
{
    ଯଦି (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#ଯଦି defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ସ୍ଥିର ଶୂନ୍ୟ
dyn_imm_load(ଶୂନ୍ୟ)
{
    hLibImm = vimLoadLib("imm32.dll");
    ଯଦି (hLibImm == ଖାଲି)
	ଫେରନ୍ତୁ;

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

    ଯଦି (       pImmGetCompositionStringW == ଖାଲି
	    || pImmGetContext == ଖାଲି
	    || pImmAssociateContext == ଖାଲି
	    || pImmReleaseContext == ଖାଲି
	    || pImmGetOpenStatus == ଖାଲି
	    || pImmSetOpenStatus == ଖାଲି
	    || pImmGetCompositionFontW == ଖାଲି
	    || pImmSetCompositionFontW == ଖାଲି
	    || pImmSetCompositionWindow == ଖାଲି
	    || pImmGetConversionStatus == ଖାଲି
	    || pImmSetConversionStatus == ଖାଲି)
    {
	FreeLibrary(hLibImm);
	hLibImm = ଖାଲି;
	pImmGetContext = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    ଫେରନ୍ତୁ;
}

#endif

#ଯଦି defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    ଶୂନ୍ୟ
gui_mch_drawsign(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr)
{
    signicon_t *sign;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y, w, h;

    ଯଦି (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == ଖାଲି)
	ଫେରନ୍ତୁ;

# ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    ବାଛ (sign->uType)
    {
	ସ୍ଥିତି IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି IMAGE_ICON:
	ସ୍ଥିତି IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ଖାଲି, DI_NORMAL);
	    ଭାଙ୍ଗ;
# ifdef FEAT_XPM_W32
	ସ୍ଥିତି IMAGE_XPM:
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
	    ଭାଙ୍ଗ;
# endif
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
close_signicon_image(signicon_t *sign)
{
    ଯଦି (sign == ଖାଲି)
	ଫେରନ୍ତୁ;

    ବାଛ (sign->uType)
    {
	ସ୍ଥିତି IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    ଭାଙ୍ଗ;
# ifdef FEAT_XPM_W32
	ସ୍ଥିତି IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    ଭାଙ୍ଗ;
# endif
    }
}

    ଶୂନ୍ୟ *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ଖାଲି;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    ଯଦି (ext > signfile)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_load = 1;

	ଯଦି (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	ଅନ୍ୟଥା ଯଦି (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	ଅନ୍ୟଥା ଯଦି (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	ଅନ୍ୟଥା
	    do_load = 0;

	ଯଦି (do_load)
	    sign.hImage = (HANDLE)LoadImage(ଖାଲି, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	ଯଦି (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((ଅକ୍ଷର *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ଖାଲି;
    ଯଦି (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ଖାଲି)
	*psign = sign;

    ଯଦି (!psign)
    {
	ଯଦି (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)psign;

}

    ଶୂନ୍ୟ
gui_mch_destroy_sign(ଶୂନ୍ୟ *sign)
{
    ଯଦି (sign == ଖାଲି)
	ଫେରନ୍ତୁ;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#ଯଦି defined(FEAT_BEVAL_GUI)

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

    ସ୍ଥିର ଶୂନ୍ୟ
make_tooltip(BalloonEval *beval, ଅକ୍ଷର *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    ଯଦି (pti == ଖାଲି)
	ଫେରନ୍ତୁ;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ଖାଲି, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, ଖାଲି, g_hinst, ଖାଲି);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = ଆକାର(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, ଖାଲି);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    ଯଦି (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ଡାହାଣ);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ବାମ = pt.x - 3;
    pti->rect.ଶୀର୍ଷ = pt.y - 3;
    pti->rect.ଡାହାଣ = pt.x + 3;
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

    ସ୍ଥିର ଶୂନ୍ୟ
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    ସ୍ଥିର VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_ସନ୍ଦେଶ UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    ଯଦି (cur_beval == ଖାଲି || cur_beval->showState == ShS_SHOWING || !p_beval)
	ଫେରନ୍ତୁ;

    GetCursorPos(&pt);
    ଯଦି (WindowFromPoint(pt) != s_textArea)
	ଫେରନ୍ତୁ;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    ଯଦି (!PtInRect(&rect, pt))
	ଫେରନ୍ତୁ;

    ଯଦି (last_user_activity > 0
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

	ଯଦି (cur_beval->ସନ୍ଦେଶ_CB != ଖାଲି)
	    (*cur_beval->ସନ୍ଦେଶ_CB)(cur_beval, 0);
    }
}

    ଶୂନ୍ୟ
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    ଶୂନ୍ୟ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ଯଦି (beval == ଖାଲି)
	ଫେରନ୍ତୁ;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    ଶୂନ୍ୟ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->ସନ୍ଦେଶ);
    beval->ସନ୍ଦେଶ = mesg == ଖାଲି ? ଖାଲି : vim_strsave(mesg);
    ଯଦି (beval->ସନ୍ଦେଶ == ଖାଲି)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ଫେରନ୍ତୁ;
    }

    ଯଦି (beval->showState == ShS_SHOWING)
	ଫେରନ୍ତୁ;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    ଯଦି (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (ଅକ୍ଷର *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    ଶୂନ୍ୟ	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    ଶୂନ୍ୟ	(*mesgCB)(BalloonEval *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
    ଶୂନ୍ୟ	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    ଯଦି (mesg != ଖାଲି && mesgCB != ଖାଲି)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ଯଦି (beval != ଖାଲି)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->ସନ୍ଦେଶ = mesg;
	beval->ସନ୍ଦେଶ_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	ଯଦି (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ଫେରନ୍ତୁ beval;
}

    ସ୍ଥିର ଶୂନ୍ୟ
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    ଯଦି (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ଫେରନ୍ତୁ;

    ଯଦି (cur_beval == ଖାଲି)
	ଫେରନ୍ତୁ;

    ବାଛ (pnmh->code)
    {
	ସ୍ଥିତି TTN_SHOW:
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TTN_କାଢ଼: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ଭାଙ୍ଗ;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
track_user_activity(UINT u_ସନ୍ଦେଶ)
{
    ଯଦି ((u_ସନ୍ଦେଶ >= WM_MOUSEFIRST && u_ସନ୍ଦେଶ <= WM_MOUSELAST)
	    || (u_ସନ୍ଦେଶ >= WM_KEYFIRST && u_ସନ୍ଦେଶ <= WM_KEYLAST))
	last_user_activity = Get_Tick_ଗଣନା();
}

    ଶୂନ୍ୟ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#ଯଦି defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    ଶୂନ୍ୟ
netbeans_draw_multisign_indicator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x;

    ଯଦି (!netbeans_active())
	ଫେରନ୍ତୁ;

    x = 0;
    y = TEXT_Y(row);

# ଯଦି defined(FEAT_DIRECTX)
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ପାଇଁ (i = 0; i < gui.char_height - 3; i++)
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

#ଯଦି defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_w32_sendevent_mouse(dict_T *args)
{
    ଯଦି (!dict_has_ଚାବି(args, "row") || !dict_has_ଚାବି(args, "col"))
	ଫେରନ୍ତୁ FALSE;

    // Note: "move" is optional, requires fewer arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା move = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_bool(args, "move", FALSE);

    ଯଦି (!move && (!dict_has_ଚାବି(args, "button")
	    || !dict_has_ଚାବି(args, "multiclick")
	    || !dict_has_ଚାବି(args, "modifiers")))
	ଫେରନ୍ତୁ FALSE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "row");
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "col");

    ଯଦି (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ଯଦି (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	ଅନ୍ୟଥା
	    gui_mouse_moved(col, row);
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା button = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "button");
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeated_click = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "multiclick");
	int_u mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    ନିବେଶ_2 inputs[1];
    ନିବେଶ_2 modkeys[3];
    SecureZeroMemory(inputs, ଆକାର(ନିବେଶ_2));
    SecureZeroMemory(modkeys, 3 * ଆକାର(ନିବେଶ_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    ଯଦି (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_ସଂଖ୍ୟା_def(args, "keycode", 0);
	ଯଦି (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ଫେରନ୍ତୁ FALSE;
	}

	BOOL is_Mod_ଚାବି = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mods = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict_get_ସଂଖ୍ୟା(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	ଯଦି (mods && STRICMP(event, "keydown") == 0 && !is_Mod_ଚାବି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
	    ଯଦି (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = ନିବେଶ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    ଯଦି (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = ନିବେଶ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    ଯଦି (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = ନିବେଶ_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    ଯଦି (n)
	    {
		(ଶୂନ୍ୟ)SetForegroundWindow(s_hwnd);
		Send_ନିବେଶ(n, modkeys, ଆକାର(ନିବେଶ_2));
	    }
	}

	inputs[0].type = ନିବେଶ_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	ଯଦି (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    ଯଦି (!is_Mod_ଚାବି)
		unwrapMods = TRUE;
	}

	(ଶୂନ୍ୟ)SetForegroundWindow(s_hwnd);
	Send_ନିବେଶ(ARRAYSIZE(inputs), inputs, ଆକାର(ନିବେଶ_2));
	vim_free(event);

	ଯଦି (unwrapMods)
	{
	    modkeys[0].type = ନିବେଶ_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = ନିବେଶ_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = ନିବେଶ_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (ଶୂନ୍ୟ)SetForegroundWindow(s_hwnd);
	    Send_ନିବେଶ(3, modkeys, ଆକାର(ନିବେଶ_2));
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (event == ଖାଲି)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    ଯଦି (strategy)
    {
	ଯଦି (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	ଅନ୍ୟଥା ଯଦି (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    ଯଦି (!handled)
    {
	ଯଦି (strategy == ଖାଲି)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	ଅନ୍ୟଥା
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}


    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    ଯଦି (STRICMP(event, "key") == 0)
	ଫେରନ୍ତୁ test_gui_w32_sendevent_keyboard(args);
    ଅନ୍ୟଥା ଯଦି (STRICMP(event, "mouse") == 0)
	ଫେରନ୍ତୁ test_gui_w32_sendevent_mouse(args);
    ଅନ୍ୟଥା ଯଦି (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ଫେରନ୍ତୁ test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str_str), "event", event);
	ଫେରନ୍ତୁ FALSE;
    }
}
#endif

#ଯଦି defined(FEAT_IMAGE_GUI)

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
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
    ସ୍ଥିର ଶୂନ୍ୟ
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, ପୂର୍ଣ୍ଣସଂଖ୍ୟା iw, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ih, image_format_T fmt)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y, x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		stride = iw * 4;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    ପାଇଁ (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	ପାଇଁ (x = 0; x < iw; x++)
	{
	    ଯଦି (fmt == IMAGE_FORMAT_RGBA)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a = s[3];

		ଯଦି (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		ଅନ୍ୟଥା ଯଦି (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		ଅନ୍ୟଥା
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    ଅନ୍ୟଥା
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		iw, ih;
    ଶୂନ୍ୟ	*bits = ଖାଲି;

    ଯଦି (ctx == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = ଆକାର(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_ଗଣନା = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, ଖାଲି, 0);
    ଯଦି (hbm == ଖାଲି || bits == ଖାଲି)
    {
	ଯଦି (hbm != ଖାଲି)
	    DeleteObject(hbm);
	ଫେରନ୍ତୁ false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    ଯଦି (mem_dc == ଖାଲି)
    {
	DeleteObject(hbm);
	ଫେରନ୍ତୁ false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
image_placement_gui_init(image_placement_T *place UNUSED)
{
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    ଶୂନ୍ୟ
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ଯଦି (rects == ଖାଲି || n_rects == 0)
	ଫେରନ୍ତୁ;

# ଯଦି defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    ଯଦି (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row, col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		posx, posy;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    ଶୂନ୍ୟ
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
