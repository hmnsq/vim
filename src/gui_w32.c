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

#अन्तर्भाव "vim.h"

#जर defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_किल्ली_OFF			0	// no dead key
#define DEAD_किल्ली_SET_DEFAULT		1	// dead key pressed
#define DEAD_किल्ली_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_किल्ली_SKIP_ON_CHAR		3	// skip next _OnChar()

#जर defined(FEAT_DIRECTX)
स्थिर DWriteContext *s_dwc = रिक्त;
स्थिर पूर्णांक s_directx_enabled = 0;
स्थिर पूर्णांक s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != रिक्त && enc_utf8)
स्थिर पूर्णांक directx_enabled(शून्य);
स्थिर शून्य directx_binddc(शून्य);
#endif

#ifdef FEAT_MENU
स्थिर पूर्णांक gui_mswin_get_menu_height(पूर्णांक fix_window);
#अन्यथा
# define gui_mswin_get_menu_height(fix_window)	0
#endif

प्रकारव्याख्या संरचना keycode_trans_strategy {
    शून्य (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
    शून्य (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
    शून्य (*ptr_process_message_usual_किल्ली) (UINT /*vk*/, स्थिरांक संदेश_2* /*pmsg*/);
    पूर्णांक  (*ptr_get_active_modifiers)(शून्य);
    पूर्णांक  (*is_experimental)(शून्य);
} keycode_trans_strategy;

// forward declarations for input instance initializer
स्थिर शून्य _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
स्थिर शून्य _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
स्थिर शून्य process_message_usual_किल्ली_experimental(UINT /*vk*/, स्थिरांक संदेश_2* /*pmsg*/);
स्थिर पूर्णांक  get_active_modifiers_experimental(शून्य);
स्थिर पूर्णांक  is_experimental_true(शून्य);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_किल्ली_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
स्थिर शून्य _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
स्थिर शून्य _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, पूर्णांक /*cRepeat UNUSED*/);
स्थिर शून्य process_message_usual_किल्ली_classic(UINT /*vk*/, स्थिरांक संदेश_2* /*pmsg*/);
स्थिर पूर्णांक  get_active_modifiers_classic(शून्य);
स्थिर पूर्णांक  is_experimental_false(शून्य);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_किल्ली_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = रिक्त;

स्थिर पूर्णांक is_experimental_true(शून्य)
{
    परत 1;
}

स्थिर पूर्णांक is_experimental_false(शून्य)
{
    परत 0;
}

/*
 * Initialize the keycode translation strategy.
 */
स्थिर शून्य keycode_trans_strategy_init(शून्य)
{
    स्थिरांक अक्षर *strategy = रिक्त;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = दो_पर्या("VIM_KEYCODE_TRANS_STRATEGY");
    जर (strategy == रिक्त)
    {
	परत;
    }

    जर (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	परत;
    }

    जर (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	परत;
    }

}

#जर defined(FEAT_RENDER_OPTIONS)
    पूर्णांक
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    पूर्णांक	    dx_enable = 0;
    पूर्णांक	    dx_flags = 0;
    दशांश   dx_gamma = 0.0f;
    दशांश   dx_contrast = 0.0f;
    दशांश   dx_level = 0.0f;
    पूर्णांक	    dx_geom = 0;
    पूर्णांक	    dx_renmode = 0;
    पूर्णांक	    dx_taamode = 0;
    // parse string as rendering options.
    साठी (p = s; p != रिक्त && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  मूल्य[128];

	copy_option_part(&p, item, आकार(item), ",");
	जर (p == रिक्त)
	    थांबा;
	q = &item[0];
	copy_option_part(&q, name, आकार(name), ":");
	जर (q == रिक्त)
	    परत FAIL;
	copy_option_part(&q, मूल्य, आकार(मूल्य), ":");

	जर (STRCMP(name, "type") == 0)
	{
	    जर (STRCMP(मूल्य, "directx") == 0)
		dx_enable = 1;
	    अन्यथा
		परत FAIL;
	}
	अन्यथा जर (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (दशांश)म_से_भ((अक्षर *)मूल्य);
	}
	अन्यथा जर (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (दशांश)म_से_भ((अक्षर *)मूल्य);
	}
	अन्यथा जर (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (दशांश)म_से_भ((अक्षर *)मूल्य);
	}
	अन्यथा जर (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = म_से_प((अक्षर *)मूल्य);
	    जर (dx_geom < 0 || dx_geom > 2)
		परत FAIL;
	}
	अन्यथा जर (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = म_से_प((अक्षर *)मूल्य);
	    जर (dx_renmode < 0 || dx_renmode > 6)
		परत FAIL;
	}
	अन्यथा जर (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = म_से_प((अक्षर *)मूल्य);
	    जर (dx_taamode < 0 || dx_taamode > 3)
		परत FAIL;
	}
	अन्यथा जर (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	अन्यथा
	    परत FAIL;
    }

    जर (!gui.in_use)
	परत OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    जर (dx_enable)
    {
	जर (!directx_enabled())
	    परत FAIL;
	DWriteContext_SetRenderingParams(s_dwc, रिक्त);
	जर (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    जर (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    जर (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    जर (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    जर (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    जर (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    जर (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    परत OK;
# अन्यथा
    परत FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (पूर्णांक)(लघु)LOWORD(lParam), (पूर्णांक)(लघु)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (पूर्णांक)(लघु)LOWORD(lParam), (पूर्णांक)(लघु)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (पूर्णांक)(लघु)LOWORD(lParam), (पूर्णांक)(लघु)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#अन्तर्भाव "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#अन्तर्भाव <shellapi.h>
#अन्तर्भाव <commctrl.h>
#अन्तर्भाव <windowsx.h>

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

स्थिर शून्य _OnPaint(HWND hwnd);
स्थिर शून्य fill_rect(स्थिरांक RECT *rcp, HBRUSH hbr, COLORREF color);
स्थिर शून्य clear_rect(RECT *rcp);

स्थिर WORD		s_dlgfntheight;		// height of the dialog font
स्थिर WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
स्थिर HMENU		s_menuBar = रिक्त;
#endif
#ifdef FEAT_TEAROFF
स्थिर शून्य rebuild_tearoff(vimmenu_T *menu);
स्थिर HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
स्थिर पूर्णांक		s_busy_processing = FALSE;

स्थिर पूर्णांक		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
स्थिर UINT		s_findrep_संदेश = 0;
स्थिर FINDREPLACEW	s_findrep_struct;
स्थिर HWND		s_findrep_hwnd = रिक्त;
स्थिर पूर्णांक		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = रिक्त;
स्थिर HDC		s_hdc = रिक्त;
स्थिर HBRUSH		s_brush = रिक्त;

#ifdef FEAT_TOOLBAR
स्थिर HWND		s_toolbarhwnd = रिक्त;
स्थिर WNDPROC		s_toolbar_wndproc = रिक्त;
#endif

#ifdef FEAT_GUI_TABLINE
स्थिर HWND		s_tabhwnd = रिक्त;
स्थिर WNDPROC		s_tabline_wndproc = रिक्त;
स्थिर पूर्णांक		showing_tabline = 0;
#endif

स्थिर WPARAM		s_wParam = 0;

स्थिर HWND		s_textArea = रिक्त;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
स्थिर bool		s_textarea_bg_only = false;

स्थिर UINT		s_u_संदेश = 0;

स्थिर char_u		*s_textfield; // Used by dialogs to pass back strings

स्थिर पूर्णांक		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
स्थिर पूर्णांक allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
प्रकारव्याख्या HANDLE DPI_AWARENESS_CONTEXT;

प्रकारव्याख्या गणना DPI_AWARENESS {
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
स्थिर पूर्णांक		s_dpi = DEFAULT_DPI;
स्थिर BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
स्थिर DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
स्थिर RECT		s_suggested_rect;

स्थिर UINT (WINAPI *pGetDpiForSystem)(शून्य) = रिक्त;
स्थिर UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = रिक्त;
स्थिर पूर्णांक (WINAPI *pGetSystemMetricsForDpi)(पूर्णांक, UINT) = रिक्त;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
स्थिर DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = रिक्त;
#ifdef FEAT_MBYTE_IME
स्थिर DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = रिक्त;
#endif

स्थिर HINSTANCE hLibDwm = रिक्त;
स्थिर HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
स्थिर शून्य dyn_dwm_load(शून्य);

स्थिर पूर्णांक fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

स्थिर HINSTANCE hUxThemeLib = रिक्त;
स्थिर DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = रिक्त;
स्थिर शून्य (WINAPI *pFlushMenuThemes)(शून्य) = रिक्त;
स्थिर शून्य dyn_uxtheme_load(शून्य);
#endif

    स्थिर पूर्णांक WINAPI
stubGetSystemMetricsForDpi(पूर्णांक nIndex, UINT dpi UNUSED)
{
    परत GetSystemMetrics(nIndex);
}

    स्थिर पूर्णांक
adjust_fontsize_by_dpi(पूर्णांक size)
{
    परत size * s_dpi / (पूर्णांक)pGetDpiForSystem();
}

    स्थिर पूर्णांक
adjust_by_system_dpi(पूर्णांक size)
{
    परत size * (पूर्णांक)pGetDpiForSystem() / DEFAULT_DPI;
}

#जर defined(FEAT_DIRECTX)
    स्थिर पूर्णांक
directx_enabled(शून्य)
{
    जर (s_dwc != रिक्त)
	परत 1;
    अन्यथा जर (s_directx_load_attempted)
	परत 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    परत s_dwc != रिक्त ? 1 : 0;
}

    स्थिर शून्य
directx_binddc(शून्य)
{
    जर (s_textArea == रिक्त)
	परत;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

बाह्य पूर्णांक current_font_height;	    // this is in os_mswin.c

स्थिर संरचना
{
    UINT    किल्ली_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_किल्ल्या[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_डावा,		'k', 'l'},
    {VK_उजवा,		'k', 'r'},

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
    {VK_घाला,		'k', 'I'},
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
स्थिर पूर्णांक	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
स्थिर पूर्णांक	s_getting_focus = FALSE;

स्थिर पूर्णांक	s_x_pending;
स्थिर पूर्णांक	s_y_pending;
स्थिर UINT	s_kFlags_pending;
स्थिर UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
स्थिर पूर्णांक	s_timed_out = FALSE;
स्थिर पूर्णांक	dead_किल्ली = DEAD_किल्ली_OFF;
स्थिर UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
स्थिर शून्य Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
स्थिर शून्य track_user_activity(UINT u_संदेश);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
स्थिर LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
स्थिर LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
स्थिर LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#जर defined(FEAT_BROWSE)
स्थिर char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    स्थिर शून्य
print_windows_error(शून्य)
{
    LPVOID  lp_संदेश_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  रिक्त, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_संदेश_Buf, 0, रिक्त);
    TRACE1("Error: %s\n", lp_संदेश_Buf);
    LocalFree(lp_संदेश_Buf);
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

स्थिर पूर्णांक		blink_state = BLINK_NONE;
स्थिर long_u		blink_waittime = 700;
स्थिर long_u		blink_ontime = 400;
स्थिर long_u		blink_offtime = 250;
स्थिर UINT_PTR		blink_timer = 0;

    पूर्णांक
gui_mch_is_blinking(शून्य)
{
    परत blink_state != BLINK_NONE;
}

    पूर्णांक
gui_mch_is_blink_off(शून्य)
{
    परत blink_state == BLINK_OFF;
}

    शून्य
gui_mch_set_blinking(दीर्घ रुको, दीर्घ on, दीर्घ off)
{
    blink_waittime = रुको;
    blink_ontime = on;
    blink_offtime = off;
}

    स्थिर VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_संदेश UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    संदेश_2 संदेश;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(रिक्त, idEvent);

    // Eat spurious WM_TIMER messages
    पर्यंत (PeekMessageW(&संदेश, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    जर (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(रिक्त, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    अन्यथा
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(रिक्त, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    स्थिर शून्य
gui_mswin_rm_blink_timer(शून्य)
{
    संदेश_2 संदेश;

    जर (blink_timer == 0)
	परत;

    KillTimer(रिक्त, blink_timer);
    // Eat spurious WM_TIMER messages
    पर्यंत (PeekMessageW(&संदेश, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    शून्य
gui_mch_stop_blink(पूर्णांक may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    जर (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    शून्य
gui_mch_start_blink(शून्य)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    जर (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(रिक्त, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    स्थिर VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_संदेश UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    संदेश_2 संदेश;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(रिक्त, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    पर्यंत (PeekMessageW(&संदेश, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    जर (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    स्थिर शून्य
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    पूर्णांक cRepeat UNUSED)
{
    dead_किल्ली = DEAD_किल्ली_SET_DEFAULT;
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
    स्थिर पूर्णांक
char_to_string(पूर्णांक ch, char_u *string, पूर्णांक slen, पूर्णांक had_alt)
{
    पूर्णांक		len;
    पूर्णांक		i;
    WCHAR	wstring[2];
    char_u	*ws = रिक्त;

    जर (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    अन्यथा जर (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	परत 0;
    }
    अन्यथा
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    जर (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, रिक्त);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	जर (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, रिक्त);
	    जर (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    अन्यथा
    {
	ws = utf16_to_enc(wstring, &len);
	जर (ws == रिक्त)
	    len = 0;
	अन्यथा
	{
	    जर (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    जर (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    साठी (i = 0; i < len; ++i)
	जर (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (पूर्णांक)KE_CSI;
	    len += 2;
	}

    परत len;
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
    स्थिर पूर्णांक
get_active_modifiers_experimental(शून्य)
{
    पूर्णांक modifiers = 0;

    जर (Get_किल्ली_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    जर (Get_किल्ली_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    जर (Get_किल्ली_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    जर ((modifiers & MOD_MASK_CTRL) && (Get_किल्ली_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    जर (!(Get_किल्ली_State(VK_CONTROL) & 0x8000)
	    &&  (Get_किल्ली_State(VK_RMENU) & 0x8000)
	    && !(Get_किल्ली_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    परत modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    स्थिर पूर्णांक
get_active_modifiers_classic(शून्य)
{
    पूर्णांक modifiers = 0;

    जर (Get_किल्ली_State(VK_SHIFT) & 0x8000)
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
    जर (Get_किल्ली_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    जर (Get_किल्ली_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    परत modifiers;
}

    स्थिर पूर्णांक
get_active_modifiers(शून्य)
{
    परत get_active_modifiers_experimental();
}

    स्थिर पूर्णांक
get_active_modifiers_via_ptr(शून्य)
{
    // marshal to corresponding implementation
    परत keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    स्थिर शून्य
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    पूर्णांक cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    स्थिर शून्य
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    पूर्णांक cRepeat UNUSED)
{
    char_u	string[40];
    पूर्णांक		len = 0;
    पूर्णांक		modifiers;
    पूर्णांक		ch = cch;   // special keys are negative

    जर (dead_किल्ली == DEAD_किल्ली_SKIP_ON_CHAR)
	परत;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    जर (dead_किल्ली != DEAD_किल्ली_TRANSIENT_IN_ON_CHAR)
	dead_किल्ली = DEAD_किल्ली_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_किल्ली(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_किल्ल्या;
    ch = may_adjust_किल्ली_for_ctrl(modifiers, ch);
    --no_reduce_किल्ल्या;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, रिक्त);
    जर (ch == CSI)
	ch = K_CSI;

    जर (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_आदान_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    जर (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_आदान_buf();
	got_int = TRUE;
    }

    add_to_आदान_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    स्थिर शून्य
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    पूर्णांक cRepeat UNUSED)
{
    char_u	string[40];
    पूर्णांक		len = 0;

    dead_किल्ली = 0;

    len = char_to_string(ch, string, 40, FALSE);
    जर (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_आदान_buf();
	got_int = TRUE;
    }

    add_to_आदान_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    स्थिर शून्य
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    पूर्णांक cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    स्थिर शून्य
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    पूर्णांक cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    पूर्णांक		len;
    पूर्णांक		modifiers;
    पूर्णांक		ch = cch;   // special keys are negative

    dead_किल्ली = DEAD_किल्ली_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_किल्ली(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, रिक्त);
    जर (ch == CSI)
	ch = K_CSI;

    len = 0;
    जर (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    जर (IS_SPECIAL((पूर्णांक)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((पूर्णांक)ch);
	string[len++] = K_THIRD((पूर्णांक)ch);
    }
    अन्यथा
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_आदान_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    स्थिर शून्य
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    पूर्णांक cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    पूर्णांक		len;
    पूर्णांक		modifiers;
    पूर्णांक		ch = cch;   // special keys are negative

    dead_किल्ली = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    जर (Get_किल्ली_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    जर (Get_किल्ली_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_किल्ली(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, रिक्त);
    जर (ch == CSI)
	ch = K_CSI;

    len = 0;
    जर (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    जर (IS_SPECIAL((पूर्णांक)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((पूर्णांक)ch);
	string[len++] = K_THIRD((पूर्णांक)ch);
    }
    अन्यथा
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_आदान_buf(string, len);
}

    स्थिर शून्य
_OnMouseEvent(
    पूर्णांक button,
    पूर्णांक x,
    पूर्णांक y,
    पूर्णांक repeated_click,
    UINT किल्ली_Flags)
{
    पूर्णांक vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    जर (किल्ली_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    जर (किल्ली_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    जर (Get_किल्ली_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    स्थिर शून्य
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    पूर्णांक x,
    पूर्णांक y,
    UINT किल्ली_Flags)
{
    स्थिर LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    पूर्णांक	    button = -1;
    पूर्णांक	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (शून्य)SetFocus(s_hwnd);

    जर (s_u_संदेश == WM_LBUTTONDOWN || s_u_संदेश == WM_LBUTTONDBLCLK)
	button = MOUSE_डावा;
    अन्यथा जर (s_u_संदेश == WM_MBUTTONDOWN || s_u_संदेश == WM_MBUTTONDBLCLK)
	button = MOUSE_मधला;
    अन्यथा जर (s_u_संदेश == WM_RBUTTONDOWN || s_u_संदेश == WM_RBUTTONDBLCLK)
	button = MOUSE_उजवा;
    अन्यथा जर (s_u_संदेश == WM_XBUTTONDOWN || s_u_संदेश == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    अन्यथा जर (s_u_संदेश == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	जर (s_button_pending == MOUSE_डावा)
	    button = MOUSE_उजवा;
	अन्यथा
	    button = MOUSE_डावा;
    }

    जर (button < 0)
	परत;

    repeated_click = ((पूर्णांक)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    जर (repeated_click
	    && ((button == MOUSE_डावा && s_button_pending == MOUSE_उजवा)
		|| (button == MOUSE_उजवा
		    && s_button_pending == MOUSE_डावा)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_मधला;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, किल्ली_Flags);
    }
    अन्यथा जर ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_उजवा)))
    {
	जर (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, किल्ली_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, किल्ली_Flags);
    }
    अन्यथा
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
	s_kFlags_pending = किल्ली_Flags;
    }

    s_prevTime = currentTime;
}

    स्थिर शून्य
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    पूर्णांक x,
    पूर्णांक y,
    UINT किल्ली_Flags)
{
    पूर्णांक button;

    s_getting_focus = FALSE;
    जर (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    जर (s_u_संदेश == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	जर (!(किल्ली_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    परत;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    अन्यथा
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, किल्ली_Flags);
}

    स्थिर शून्य
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    पूर्णांक cx UNUSED,
    पूर्णांक cy UNUSED)
{
#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    स्थिर vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    पूर्णांक		id)
{
    vimmenu_T	*pChildMenu;

    पर्यंत (pMenu)
    {
	जर (pMenu->id == (UINT)id)
	    थांबा;
	जर (pMenu->children != रिक्त)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    जर (pChildMenu)
	    {
		pMenu = pChildMenu;
		थांबा;
	    }
	}
	pMenu = pMenu->next;
    }
    परत pMenu;
}

    स्थिर शून्य
_OnMenu(
    HWND	hwnd UNUSED,
    पूर्णांक		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(मूळ_menu, id);
    जर (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    स्थिर शून्य
_OnFindRepl(शून्य)
{
    पूर्णांक	    flags = 0;
    पूर्णांक	    down;

    जर (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(शून्य)SetFocus(s_hwnd);

    जर (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(शून्य)SetFocus(s_hwnd);
    }
    अन्यथा जर (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(शून्य)SetFocus(s_hwnd);
    }
    अन्यथा जर (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    जर (flags == 0)
	परत;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    जर (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    जर (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, रिक्त);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, रिक्त);
    जर (p != रिक्त && q != रिक्त)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    स्थिर शून्य
HandleMouseHide(UINT u_संदेश, LPARAM lParam)
{
    स्थिर LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    जर (u_संदेश == WM_MOUSEMOVE || u_संदेश == WM_NCMOUSEMOVE)
    {
	जर (lParam == last_lParam)
	    परत;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    निवड (u_संदेश)
    {
    स्थिती WM_KEYUP:
    स्थिती WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	जर (p_mh)
	    gui_mch_mousehide(TRUE);
	थांबा;

    स्थिती WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    स्थिती WM_SYSCHAR:
    स्थिती WM_MOUSEMOVE:	 // show the pointer on any mouse action
    स्थिती WM_LBUTTONDOWN:
    स्थिती WM_LBUTTONUP:
    स्थिती WM_MBUTTONDOWN:
    स्थिती WM_MBUTTONUP:
    स्थिती WM_RBUTTONDOWN:
    स्थिती WM_RBUTTONUP:
    स्थिती WM_XBUTTONDOWN:
    स्थिती WM_XBUTTONUP:
    स्थिती WM_NCMOUSEMOVE:
    स्थिती WM_NCLBUTTONDOWN:
    स्थिती WM_NCLBUTTONUP:
    स्थिती WM_NCMBUTTONDOWN:
    स्थिती WM_NCMBUTTONUP:
    स्थिती WM_NCRBUTTONDOWN:
    स्थिती WM_NCRBUTTONUP:
    स्थिती WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	थांबा;
    }
}

    स्थिर LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_संदेश,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_संदेश, lParam);

    s_u_संदेश = u_संदेश;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_संदेश);
#endif

    निवड (u_संदेश)
    {
	HANDLE_संदेश(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_संदेश(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_संदेश(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_संदेश(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_संदेश(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_संदेश(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_संदेश(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_संदेश(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	स्थिती WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    परत TRUE;
#endif
	पूर्वनिर्धारित:
	    परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
    }
}

    स्थिर शून्य
dyn_dwm_load(शून्य)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    जर (hLibDwm == रिक्त)
	परत;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    जर (pDwmSetWindowAttribute == रिक्त)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = रिक्त;
	परत;
    }
}

बाह्य DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    शून्य
gui_mch_set_titlebar_colors(शून्य)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    जर (pDwmSetWindowAttribute == रिक्त || win_version < MAKE_VER(10, 0, 22000))
	परत;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    जर (vim_strchr(p_go, GO_TITLEBAR) != रिक्त)
    {
	जर (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	अन्यथा
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	जर (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	जर (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, आकार(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, आकार(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    शून्य
gui_mch_new_colors(शून्य)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, रिक्त, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    जर (s_textarea_bg_only && s_textArea != रिक्त)
	InvalidateRect(s_textArea, रिक्त, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    शून्य
gui_mch_def_colors(शून्य)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    पूर्णांक
gui_mch_open(शून्य)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    जर (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	जर (s_textArea != रिक्त)
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

    परत OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    पूर्णांक
gui_mch_get_winpos(पूर्णांक *x, पूर्णांक *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.डावा;
    *y = rect.शीर्ष;
    परत OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    शून्य
gui_mch_set_winpos(पूर्णांक x, पूर्णांक y)
{
    SetWindowPos(s_hwnd, रिक्त, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    शून्य
gui_mch_set_text_area_pos(पूर्णांक x, पूर्णांक y, पूर्णांक w, पूर्णांक h)
{
    स्थिर पूर्णांक oldx = 0;
    स्थिर पूर्णांक oldy = 0;

    SetWindowPos(s_textArea, रिक्त, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    जर (vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#जर defined(FEAT_GUI_TABLINE)
    जर (showing_tabline)
    {
	पूर्णांक	शीर्ष = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	जर (vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
	    शीर्ष = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, शीर्ष, rect.उजवा, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    जर (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, रिक्त, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    शून्य
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    पूर्णांक		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    शून्य
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    पूर्णांक		x,
    पूर्णांक		y,
    पूर्णांक		w,
    पूर्णांक		h)
{
    SetWindowPos(sb->id, रिक्त, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    पूर्णांक
gui_mch_get_scrollbar_xpadding(शून्य)
{
    RECT    rcTxt, rcWnd;
    पूर्णांक	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.उजवा - rcTxt.उजवा - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    परत (xpad < 0) ? 0 : xpad;
}

    पूर्णांक
gui_mch_get_scrollbar_ypadding(शून्य)
{
    RECT    rcTxt, rcWnd;
    पूर्णांक	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    परत (ypad < 0) ? 0 : ypad;
}

    शून्य
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    पूर्णांक		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, रिक्त,
	g_hinst, रिक्त);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    स्थिर scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    जर (gui.bottom_sbar.id == hwnd)
	परत &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	जर (wp->w_scrollbars[SBAR_डावा].id == hwnd)
	    परत &wp->w_scrollbars[SBAR_डावा];
	जर (wp->w_scrollbars[SBAR_उजवा].id == hwnd)
	    परत &wp->w_scrollbars[SBAR_उजवा];
    }
    परत रिक्त;
}

    स्थिर शून्य
update_scrollbar_size(शून्य)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    स्थिर शून्य
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
    स्थिर शून्य
GetFontSize(GuiFont font, पूर्णांक *char_width, पूर्णांक *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC पंचांग;

    GetTextMetrics(hdc, &पंचांग);
    GetAverageFontSize(hdc, &size);

    जर (char_width)
	*char_width = size.cx + पंचांग.tmOverhang;
    जर (char_height)
	*char_height = पंचांग.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    स्थिर शून्य
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णांक
gui_mch_adjust_charheight(शून्य)
{
    UpdateFontSize(gui.norm_font);
    परत OK;
}

    स्थिर GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = रिक्त;

    // Load the font
    font = CreateFontIndirectW(lf);

    जर (font == रिक्त)
	परत NOFONT;

    परत (GuiFont)font;
}

    स्थिर पूर्णांक
pixels_to_points(पूर्णांक pixels, पूर्णांक vertical)
{
    पूर्णांक		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    परत points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    पूर्णांक		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    जर (get_logfont(&lf, name, रिक्त, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    जर (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    परत font;
}

#जर defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    जर (name == रिक्त)
	परत रिक्त;
    परत vim_strsave(name);
}
#endif

    शून्य
gui_mch_free_font(GuiFont font)
{
    जर (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    पूर्णांक i;

    प्रकारव्याख्या संरचना SysColorTable
    {
	अक्षर	    *name;
	पूर्णांक	    color;
    } SysColorTable;

    स्थिर SysColorTable sys_table[] =
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
    साठी (i = 0; i < (पूर्णांक)ARRAY_LENGTH(sys_table); i++)
	जर (STRICMP(name, sys_table[i].name) == 0)
	    परत GetSysColor(sys_table[i].color);

    परत gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(पूर्णांक r, पूर्णांक g, पूर्णांक b)
{
    परत gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    पूर्णांक
gui_mch_haskey(char_u *name)
{
    पूर्णांक i;

    साठी (i = 0; special_किल्ल्या[i].vim_code1 != NUL; i++)
	जर (name[0] == special_किल्ल्या[i].vim_code0
				       && name[1] == special_किल्ल्या[i].vim_code1)
	    परत OK;
    परत FAIL;
}

    शून्य
gui_mch_beep(शून्य)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    शून्य
gui_mch_invert_rectangle(
    पूर्णांक	    r,
    पूर्णांक	    c,
    पूर्णांक	    nr,
    पूर्णांक	    nc)
{
    RECT    rc;

#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.डावा = FILL_X(c);
    rc.शीर्ष = FILL_Y(r);
    rc.उजवा = rc.डावा + nc * gui.char_width;
    rc.bottom = rc.शीर्ष + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    शून्य
gui_mch_iconify(शून्य)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    शून्य
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.डावा = FILL_X(gui.col);
    rc.शीर्ष = FILL_Y(gui.row);
    rc.उजवा = rc.डावा + gui.char_width;
    जर (mb_lefthalve(gui.row, gui.col))
	rc.उजवा += gui.char_width;
    rc.bottom = rc.शीर्ष + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    शून्य
gui_mch_draw_part_cursor(
    पूर्णांक		w,
    पूर्णांक		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.डावा =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_उजवा ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.शीर्ष = FILL_Y(gui.row) + gui.char_height - h;
    rc.उजवा = rc.डावा + w;
    rc.bottom = rc.शीर्ष + h;

    fill_rect(&rc, रिक्त, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    स्थिर शून्य
output_Dead_किल्ली_re_Post_Ex(संदेश_2 original_संदेश, पूर्णांक dead_key2set)
{
    स्थिर संदेश_2 deadCharExpel;

    जर (dead_किल्ली == DEAD_किल्ली_OFF)
	परत;

    dead_किल्ली = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_संदेश.message;
    deadCharExpel.hwnd    = original_संदेश.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_संदेश.hwnd, original_संदेश.message, original_संदेश.wParam,
							  original_संदेश.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    स्थिर शून्य
output_Dead_किल्ली_re_Post(संदेश_2 original_संदेश)
{
    output_Dead_किल्ली_re_Post_Ex(original_संदेश, DEAD_किल्ली_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
स्थिर शून्य process_message_usual_किल्ली(UINT vk, स्थिरांक संदेश_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_किल्ली(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    स्थिर शून्य
process_message_usual_किल्ली_experimental(UINT vk, स्थिरांक संदेश_2 *pmsg)
{
    WCHAR	ch[8];
    पूर्णांक		len;
    पूर्णांक		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    स_रखो(keyboard_state, 0, 256);
    जर (Get_किल्ली_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    जर (Get_किल्ली_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    जर ((Get_किल्ली_State(VK_RMENU) & 0x8000)
				 && (Get_किल्ली_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = Map_Virtual_किल्ली(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    जर (len < 0)
	dead_किल्ली = DEAD_किल्ली_SET_DEFAULT;

    जर (len <= 0)
    {
	पूर्णांक wm_char = NUL;

	जर (dead_किल्ली == DEAD_किल्ली_SET_DEFAULT
		&& (Get_किल्ली_State(VK_CONTROL) & 0x8000))
	{
	    जर (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    जर (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	जर (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_किल्ली = DEAD_किल्ली_TRANSIENT_IN_ON_CHAR;
	}
	परत;
    }

    // Post the message as TranslateMessage would do.
    जर (pmsg->message == WM_KEYDOWN)
    {
	साठी (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    अन्यथा
    {
	साठी (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    स्थिर शून्य
process_message_usual_किल्ली_classic(UINT vk, स्थिरांक संदेश_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    जर (vk != 0xff
	    && (Get_किल्ली_State(VK_CONTROL) & 0x8000)
	    && !(Get_किल्ली_State(VK_SHIFT) & 0x8000)
	    && !(Get_किल्ली_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	जर (vk == '6' || Map_Virtual_किल्ली(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_आदान_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	अन्यथा जर (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_आदान_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	अन्यथा जर (vk == '2' || Map_Virtual_किल्ली(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_आदान_buf(string, 1);
	}
	अन्यथा
	    TranslateMessage(pmsg);
    }
    अन्यथा
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    स्थिर शून्य
process_message(शून्य)
{
    संदेश_2		संदेश;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    पूर्णांक		i;
    पूर्णांक		modifiers = 0;
    पूर्णांक		किल्ली;
#ifdef FEAT_MENU
    स्थिर char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    स्थिर पूर्णांक  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    जर (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&संदेश, रिक्त, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    जर (संदेश.message == WM_OLE)
    {
	char_u *str = (char_u *)संदेश.lParam;
	जर (str == रिक्त || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&संदेश);
	}
	अन्यथा
	{
	    add_to_आदान_buf(str, (पूर्णांक)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	परत;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    जर (s_findrep_hwnd != रिक्त && IsDialogMessageW(s_findrep_hwnd, &संदेश))
    {
	HandleMouseHide(संदेश.message, संदेश.lParam);
	परत;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    जर (संदेश.message == WM_KEYDOWN || संदेश.message == WM_SYSKEYDOWN)
    {
	vk = (पूर्णांक) संदेश.wParam;

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
	जर (keycode_trans_strategy_used->is_experimental()
		&& dead_किल्ली == DEAD_किल्ली_SKIP_ON_CHAR)
	{
	    dead_किल्ली = DEAD_किल्ली_OFF;
	}

	जर (dead_किल्ली != DEAD_किल्ली_OFF)
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
	    जर (keycode_trans_strategy_used->is_experimental() &&
		    dead_किल्ली == DEAD_किल्ली_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_किल्ली_re_Post_Ex(संदेश,
				       /*dead_key2set=*/DEAD_किल्ली_SKIP_ON_CHAR);
		परत;
	    }

	    जर (dead_किल्ली != DEAD_किल्ली_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		परत;
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
	    जर ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_किल्ली = DEAD_किल्ली_OFF;
		TranslateMessage(&संदेश);
		परत;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    अन्यथा जर ((get_real_state()
			    & (MODE_घाला | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_किल्ली_re_Post(संदेश);
		परत;
	    }
	}

	// Check for CTRL-BREAK
	जर (vk == VK_CANCEL)
	{
	    trash_आदान_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_आदान_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	जर (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&संदेश);
	    परत;
	}

	साठी (i = 0; special_किल्ल्या[i].किल्ली_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    जर (special_किल्ल्या[i].किल्ली_sym == vk
		    && (vk != VK_SPACE || !(Get_किल्ली_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		जर (dead_किल्ली == DEAD_किल्ली_SET_DEFAULT
			&& (special_किल्ल्या[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_किल्ली_re_Post(संदेश);
		    परत;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		जर (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  रिक्त, रिक्त) == रिक्त)
		    थांबा;
#endif
		modifiers = get_active_modifiers_via_ptr();

		जर (special_किल्ल्या[i].vim_code1 == NUL)
		    किल्ली = special_किल्ल्या[i].vim_code0;
		अन्यथा
		    किल्ली = TO_SPECIAL(special_किल्ल्या[i].vim_code0,
						   special_किल्ल्या[i].vim_code1);
		किल्ली = simplify_किल्ली(किल्ली, &modifiers);
		जर (किल्ली == CSI)
		    किल्ली = K_CSI;

		जर (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_आदान_buf(string, 3);
		}

		जर (IS_SPECIAL(किल्ली))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(किल्ली);
		    string[2] = K_THIRD(किल्ली);
		    add_to_आदान_buf(string, 3);
		}
		अन्यथा
		{
		    पूर्णांक	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(किल्ली, string, 40, FALSE);
		    add_to_आदान_buf(string, len);
		}
		थांबा;
	    }
	}

	// Not a special key.
	जर (special_किल्ल्या[i].किल्ली_sym == 0)
	{
	    process_message_usual_किल्ली(vk, &संदेश);
	}
    }
#ifdef FEAT_MBYTE_IME
    अन्यथा जर (संदेश.message == WM_IME_NOTIFY)
	_OnImeNotify(संदेश.hwnd, (DWORD)संदेश.wParam, (DWORD)संदेश.lParam);
    अन्यथा जर (संदेश.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&संदेश);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    जर (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  रिक्त, रिक्त) == रिक्त)
#endif
	DispatchMessageW(&संदेश);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    शून्य
gui_mch_update(शून्य)
{
    संदेश_2	    संदेश;

    जर (!s_busy_processing)
	पर्यंत (PeekMessageW(&संदेश, रिक्त, 0, 0, PM_NOREMOVE)
						  && !vim_is_आदान_buf_full())
	    process_message();
}

    स्थिर शून्य
remove_any_timer(शून्य)
{
    संदेश_2		संदेश;

    जर (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(रिक्त, s_wait_timer);

	// Eat spurious WM_TIMER messages
	पर्यंत (PeekMessageW(&संदेश, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
स्थिर garray_T	server_pending_ga = {0, 0, 0, 0, रिक्त};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    शून्य
server_add_आदान(char_u *str)
{
    जर (server_pending_ga.ga_data == रिक्त)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    स्थिर शून्य
server_flush_आदान(शून्य)
{
    char_u  *str;

    जर (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	परत;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    जर (str != रिक्त)
    {
	server_to_आदान_buf(str);
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
    पूर्णांक
gui_mch_wait_for_chars(दीर्घ wtime)
{
    पूर्णांक		focus;

    s_timed_out = FALSE;

    जर (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	जर (s_busy_processing)
	    परत FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(रिक्त, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    पर्यंत (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	जर (gui.in_focus != focus)
	{
	    जर (gui.in_focus)
		gui_mch_start_blink();
	    अन्यथा
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	जर (s_need_activate)
	{
	    (शून्य)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	साठी (;;)
	{
	    संदेश_2 संदेश;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_आदान();
# endif
# ifdef FEAT_TIMERS
	    जर (did_add_timer)
		थांबा;
# endif
	    जर (PeekMessageW(&संदेश, रिक्त, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		थांबा;
	    }
	    अन्यथा जर (आदान_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || संदेश_Wait_For_Multiple_Objects(0, रिक्त, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		थांबा;
	}
#अन्यथा
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	जर (आदान_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    जर (!s_getting_focus)
		s_button_pending = -1;

	    परत OK;
	}

#ifdef FEAT_TIMERS
	जर (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    थांबा;
	}
#endif
    }
    allow_scrollbar = FALSE;
    परत FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    शून्य
gui_mch_clear_block(
    पूर्णांक		row1,
    पूर्णांक		col1,
    पूर्णांक		row2,
    पूर्णांक		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.डावा = FILL_X(col1);
    rc.शीर्ष = FILL_Y(row1);
    rc.उजवा = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    शून्य
gui_mch_clear_all(शून्य)
{
    RECT    rc;

    rc.डावा = 0;
    rc.शीर्ष = 0;
    rc.उजवा = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    शून्य
gui_mch_enable_menu(पूर्णांक flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : रिक्त);
#endif
}

    शून्य
gui_mch_set_menu_pos(
    पूर्णांक	    x UNUSED,
    पूर्णांक	    y UNUSED,
    पूर्णांक	    w UNUSED,
    पूर्णांक	    h UNUSED)
{
    // It will be in the right place anyway
}

#जर defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    शून्य
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    पूर्णांक		hidden)
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
    शून्य
gui_mch_draw_menubar(शून्य)
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
    परत (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#जर defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    स्थिर WORD
PixelToDialogX(पूर्णांक अंक_Pixels)
{
    परत (WORD)((अंक_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    स्थिर WORD
PixelToDialogY(पूर्णांक अंक_Pixels)
{
    परत (WORD)((अंक_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    स्थिर पूर्णांक
GetTextWidth(HDC hdc, char_u *str, पूर्णांक len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    परत size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    स्थिर पूर्णांक
GetTextWidthEnc(HDC hdc, char_u *str, पूर्णांक len)
{
    SIZE	size;
    WCHAR	*wstr;
    पूर्णांक		n;
    पूर्णांक		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    जर (wstr == रिक्त)
	परत 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    जर (n)
	परत size.cx;
    परत 0;
}

स्थिर शून्य get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    स्थिर BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    पूर्णांक		    wChild, hChild, wParent, hParent;
    पूर्णांक		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.उजवा - rChild.डावा;
    hChild = rChild.bottom - rChild.शीर्ष;

    // If Vim is minimized put the window in the middle of the screen.
    जर (hwndParent == रिक्त || IsMinimized(hwndParent))
	get_work_area(&rParent);
    अन्यथा
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.उजवा - rParent.डावा;
    hParent = rParent.bottom - rParent.शीर्ष;

    moninfo.cbSize = आकार(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    जर (mon != रिक्त && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    अन्यथा
    {
	hdc = GetDC(hwndChild);
	rScreen.डावा = 0;
	rScreen.शीर्ष = 0;
	rScreen.उजवा = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.डावा + ((wParent - wChild) / 2);
    जर (xNew < rScreen.डावा)
	xNew = rScreen.डावा;
    अन्यथा जर ((xNew + wChild) > rScreen.उजवा)
	xNew = rScreen.उजवा - wChild;

    yNew = rParent.शीर्ष + ((hParent - hChild) / 2);
    जर (yNew < rScreen.शीर्ष)
	yNew = rScreen.शीर्ष;
    अन्यथा जर ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    परत SetWindowPos(hwndChild, रिक्त, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#जर defined(FEAT_TOOLBAR)
    शून्य
gui_mch_show_toolbar(पूर्णांक showit)
{
    जर (s_toolbarhwnd == रिक्त)
	परत;

    जर (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    अन्यथा
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_मोजणी 31

#endif

#जर defined(FEAT_GUI_TABLINE)
    स्थिर शून्य
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, रिक्त);
    जर (wn == रिक्त)
	परत;

    infow.cbSize = आकार(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    घाला_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    स्थिर शून्य
show_tabline_popup_menu(शून्य)
{
    HMENU	    tab_pmenu;
    दीर्घ	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    जर (hold_gui_events || cmdwin_type != 0)
	परत;

    tab_pmenu = CreatePopupMenu();
    जर (tab_pmenu == रिक्त)
	परत;

    जर (first_tabpage->tp_next != रिक्त)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									रिक्त);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    जर (rval > 0)
    {
	TCHITTESTINFO htinfo;
	पूर्णांक idx;

	जर (ScreenToClient(s_tabhwnd, &pt) == 0)
	    परत;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	जर (idx == -1)
	    idx = 0;
	अन्यथा
	    idx += 1;

	send_tabline_menu_event(idx, (पूर्णांक)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    शून्य
gui_mch_show_tabline(पूर्णांक showit)
{
    जर (s_tabhwnd == रिक्त)
	परत;

    जर (!showit != !showing_tabline)
    {
	जर (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	अन्यथा
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    पूर्णांक
gui_mch_showing_tabline(शून्य)
{
    परत s_tabhwnd != रिक्त && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    शून्य
gui_mch_update_tabline(शून्य)
{
    tabpage_T	*tp;
    TCITEM	tie;
    पूर्णांक		nr = 0;
    पूर्णांक		curtabidx = 0;
    पूर्णांक		tabadded = 0;
    WCHAR	*wstr = रिक्त;

    जर (s_tabhwnd == रिक्त)
	परत;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    साठी (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, ++nr)
    {
	जर (tp == curtab)
	    curtabidx = nr;

	जर (nr >= Tab_Ctrl_Get_Item_मोजणी(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_घाला_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, रिक्त);
	जर (wstr != रिक्त)
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
    पर्यंत (nr < Tab_Ctrl_Get_Item_मोजणी(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    जर (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, रिक्त, रिक्त,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    जर (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    शून्य
gui_mch_set_curtab(पूर्णांक nr)
{
    जर (s_tabhwnd == रिक्त)
	परत;

    जर (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    शून्य
gui_mch_set_dark_theme(पूर्णांक dark)
{
    जर (pDwmSetWindowAttribute != रिक्त && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		आकार(dark));

    जर (pSetPreferredAppMode != रिक्त && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    जर (pFlushMenuThemes != रिक्त && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    स्थिर शून्य
dyn_uxtheme_load(शून्य)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    जर (hUxThemeLib == रिक्त)
	परत;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (शून्य (WINAPI *)(शून्य))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    जर (pSetPreferredAppMode == रिक्त || pFlushMenuThemes == रिक्त)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = रिक्त;
	परत;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    शून्य
gui_mch_set_fullscreen(पूर्णांक flag)
{
    स्थिर RECT normal_rect;
    स्थिर LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    जर (!full_screen) // Windows not set yet.
	परत;

    जर (flag)
    {
	जर (fullscreen_on)
	    परत;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = आकार(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	जर (mon == रिक्त || !GetMonitorInfo(mon, &moninfo))
	    परत;

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
	SetWindowPos(s_hwnd, रिक्त,
		moninfo.rcMonitor.डावा,
		moninfo.rcMonitor.शीर्ष,
		moninfo.rcMonitor.उजवा - moninfo.rcMonitor.डावा,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.शीर्ष,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    अन्यथा
    {
	जर (!fullscreen_on)
	    परत;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, रिक्त,
		normal_rect.डावा,
		normal_rect.शीर्ष,
		normal_rect.उजवा - normal_rect.डावा,
		normal_rect.bottom - normal_rect.शीर्ष,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    शून्य
ex_simalt(exarg_T *eap)
{
    char_u	*किल्ल्या = eap->arg;
    पूर्णांक		fill_typebuf = FALSE;
    char_u	किल्ली_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    पर्यंत (*किल्ल्या)
    {
	जर (*किल्ल्या == '~')
	    *किल्ल्या = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*किल्ल्या, (LPARAM)0);
	किल्ल्या++;
	fill_typebuf = TRUE;
    }
    जर (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	किल्ली_name[0] = K_SPECIAL;
	किल्ली_name[1] = KS_EXTRA;
	किल्ली_name[2] = KE_NOP;
	किल्ली_name[3] = NUL;
#जर defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(शून्य)ins_typebuf(किल्ली_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    स्थिर शून्य
initialise_findrep(char_u *initial_string)
{
    पूर्णांक		wword = FALSE;
    पूर्णांक		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    जर (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    जर (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    जर (entry_text != रिक्त && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, रिक्त);
	जर (p != रिक्त)
	{
	    पूर्णांक len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    स्थिर शून्य
set_window_title(HWND hwnd, अक्षर *title)
{
    जर (title != रिक्त)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, रिक्त);
	जर (wbuf != रिक्त)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    अन्यथा
	(शून्य)SetWindowTextW(hwnd, रिक्त);
}

    शून्य
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    जर (s_findrep_संदेश != 0)
    {
	जर (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	जर (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(शून्य)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    शून्य
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    जर (s_findrep_संदेश != 0)
    {
	जर (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	जर (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(शून्य)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    शून्य
gui_mch_mousehide(पूर्णांक hide)
{
    जर (hide == gui.pointer_hidden)
	परत;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    स्थिर शून्य
gui_mch_show_popupmenu_at(vimmenu_T *menu, पूर्णांक x, पूर्णांक y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (शून्य)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(पूर्णांक)0,	    //reserved param
	s_hwnd,
	रिक्त);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    स्थिर शून्य
_OnEndSession(शून्य)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    स्थिर शून्य
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    स्थिर शून्य
_OnDestroy(HWND hwnd)
{
    जर (!destroying)
	_OnClose(hwnd);
}

    स्थिर शून्य
_OnPaint(
    HWND hwnd)
{
    जर (IsMinimized(hwnd))
	परत;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (शून्य)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    जर (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.डावा = rect.डावा;
	ps.rcPaint.उजवा = rect.उजवा;
    }

    जर (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	जर (!screen_cleared || Screen_ओळी == रिक्त)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	अन्यथा
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.डावा, ps.rcPaint.शीर्ष,
		ps.rcPaint.उजवा - ps.rcPaint.डावा + 1,
		ps.rcPaint.bottom - ps.rcPaint.शीर्ष + 1);
    }

    EndPaint(hwnd, &ps);
}

    स्थिर शून्य
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    पूर्णांक cx,
    पूर्णांक cy)
{
    जर (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    स्थिर शून्य
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (शून्य)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    स्थिर शून्य
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    जर (destroying)
	परत;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (शून्य)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    स्थिर LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    परत DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    शून्य
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    शून्य
gui_mch_getmouse(पूर्णांक *x, पूर्णांक *y)
{
    RECT rct;
    POINT mp;

    (शून्य)GetWindowRect(s_textArea, &rct);
    (शून्य)GetCursorPos(&mp);
    *x = (पूर्णांक)(mp.x - rct.डावा);
    *y = (पूर्णांक)(mp.y - rct.शीर्ष);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    शून्य
gui_mch_setmouse(पूर्णांक x, पूर्णांक y)
{
    RECT rct;

    (शून्य)GetWindowRect(s_textArea, &rct);
    (शून्य)SetCursorPos(x + gui.border_offset + rct.डावा,
		       y + gui.border_offset + rct.शीर्ष);
}

    स्थिर शून्य
gui_mswin_get_valid_dimensions(
    पूर्णांक w,
    पूर्णांक h,
    पूर्णांक *valid_w,
    पूर्णांक *valid_h,
    पूर्णांक *cols,
    पूर्णांक *rows)
{
    पूर्णांक	    base_width, base_height;

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

    शून्य
gui_mch_flash(पूर्णांक msec)
{
    RECT    rc;

#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.डावा = 0;
    rc.शीर्ष = 0;
    rc.उजवा = gui.अंक_cols * gui.char_width;
    rc.bottom = gui.अंक_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((दीर्घ)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    स्थिर BOOL
is_point_onscreen(पूर्णांक x, पूर्णांक y)
{
    POINT   pt = {x, y};

    परत MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != रिक्त;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    स्थिर BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.डावा;
    p1.y = rc.शीर्ष;
    p2.x = rc.उजवा - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    जर (!is_point_onscreen(p1.x, p1.y))
	परत FALSE;
    जर (!is_point_onscreen(p1.x, p2.y))
	परत FALSE;
    जर (!is_point_onscreen(p2.x, p1.y))
	परत FALSE;
    जर (!is_point_onscreen(p2.x, p2.y))
	परत FALSE;
    परत TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    स्थिर पूर्णांक
get_scroll_flags(शून्य)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    जर (!is_window_onscreen(s_hwnd))
	परत SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    साठी (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	जर (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    जर (IntersectRect(&rcDest, &rcVim, &rcOther))
		परत SW_INVALIDATE;
	}
    परत 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    स्थिर शून्य
intel_gpu_workaround(शून्य)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    शून्य
gui_mch_delete_ओळी(
    पूर्णांक	    row,
    पूर्णांक	    अंक_ओळी)
{
    RECT	rc;

    rc.डावा = FILL_X(gui.scroll_region_डावा);
    rc.उजवा = FILL_X(gui.scroll_region_उजवा + 1);
    rc.शीर्ष = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -अंक_ओळी * gui.char_height, &rc);
    }
    अन्यथा
#endif
    {
#जर defined(FEAT_DIRECTX)
	जर (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -अंक_ओळी * gui.char_height,
				    &rc, &rc, रिक्त, रिक्त, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - अंक_ओळी + 1,
						       gui.scroll_region_डावा,
	gui.scroll_region_bot, gui.scroll_region_उजवा);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    शून्य
gui_mch_घाला_ओळी(
    पूर्णांक		row,
    पूर्णांक		अंक_ओळी)
{
    RECT	rc;

    rc.डावा = FILL_X(gui.scroll_region_डावा);
    rc.उजवा = FILL_X(gui.scroll_region_उजवा + 1);
    rc.शीर्ष = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, अंक_ओळी * gui.char_height, &rc);
    }
    अन्यथा
#endif
    {
#जर defined(FEAT_DIRECTX)
	जर (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, अंक_ओळी * gui.char_height,
				    &rc, &rc, रिक्त, रिक्त, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_डावा,
				row + अंक_ओळी - 1, gui.scroll_region_उजवा);
}


    शून्य
gui_mch_exit(पूर्णांक rc UNUSED)
{
#जर defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = रिक्त;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (शून्य)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    जर (s_hwnd != रिक्त)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    स्थिर char_u *
logfont2name(LOGFONTW lf)
{
    अक्षर	*charset_name;
    अक्षर	*quality_name;
    अक्षर	*font_name;
    माप_प्रकार	res_size;
    अक्षर	*res;

    font_name = (अक्षर *)utf16_to_enc(lf.lfFaceName, रिक्त);
    जर (font_name == रिक्त)
	परत रिक्त;
    charset_name = charset_id2name((पूर्णांक)lf.lfCharSet);
    quality_name = quality_id2name((पूर्णांक)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == रिक्त ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == रिक्त ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    जर (res != रिक्त)
    {
	अक्षर	*p;
	पूर्णांक	points;
	माप_प्रकार	res_len;

	// replace spaces in font_name with underscores.
	साठी (p = font_name; *p != NUL; ++p)
	{
	    जर (है_खाली(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	जर (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(अक्षर *)res, res_size, "%s:h%d", font_name, points);
	अन्यथा
	    res_len = vim_snprintf_safelen(
		(अक्षर *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (अक्षर *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	जर (charset_name != रिक्त)
	    res_len += vim_snprintf_safelen((अक्षर *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	जर (quality_name != रिक्त)
	    vim_snprintf((अक्षर *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    परत (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    स्थिर शून्य
update_im_font(शून्य)
{
    LOGFONTW	lf_wide, lf;

    जर (p_guifontwide != रिक्त && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, आकार(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    अन्यथा
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    जर (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    शून्य
gui_mch_wide_font_changed(शून्य)
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

    जर (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, आकार(lf), &lf))
    {
	जर (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	जर (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    जर (!lf.lfItalic)
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
    पूर्णांक
gui_mch_init_font(char_u *font_name, पूर्णांक fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    जर (get_logfont(&lf, font_name, रिक्त, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    जर (font == NOFONT)
	परत FAIL;

#जर defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	पूर्णांक		    feat_मोजणी = 0;
	char_u		    *fp;

	जर (font_name != रिक्त)
	{
	    // Find each ":f" option in font_name.
	    साठी (fp = font_name; *fp != NUL; fp++)
	    {
		जर (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    पूर्णांक    ti = 0;
		    अचिन्हित पूर्णांक param = 1;

		    fp += 2;  // skip ":f"
		    पर्यंत (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    जर (ti != 4)
			सुरूठेवा;  // invalid tag length

		    जर (*fp == '=')
		    {
			fp++;
			param = (अचिन्हित पूर्णांक)म_से_प((अक्षर *)fp);
			पर्यंत (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    जर (feat_मोजणी < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_मोजणी].tag =
			    ((अचिन्हित पूर्णांक)tag[0])
			    | ((अचिन्हित पूर्णांक)tag[1] << 8)
			    | ((अचिन्हित पूर्णांक)tag[2] << 16)
			    | ((अचिन्हित पूर्णांक)tag[3] << 24);
			features[feat_मोजणी].parameter = param;
			feat_मोजणी++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_मोजणी);
    }
#endif

    जर (font_name == रिक्त)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    जर (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    जर (p != रिक्त)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	जर (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	अन्यथा
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    जर (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    जर (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	जर (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    परत OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    पूर्णांक
gui_mch_maximized(शून्य)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = आकार(WINDOWPLACEMENT);
    जर (GetWindowPlacement(s_hwnd, &wp))
    {
	जर (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    परत TRUE;
	जर (wp.showCmd == SW_SHOWMINIMIZED)
	    परत FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	जर ((rc.उजवा - rc.डावा !=
		    wp.rcNormalPosition.उजवा - wp.rcNormalPosition.डावा)
		|| (rc.bottom - rc.शीर्ष !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.शीर्ष))
	    परत TRUE;
    }
    परत FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    शून्य
gui_mch_newfont(शून्य)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    जर (win_socket_id == 0)
    {
	gui_resize_shell(rect.उजवा - rect.डावा
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.शीर्ष
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    अन्यथा
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.उजवा - rect.डावा,
	    rect.bottom - rect.शीर्ष - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    शून्य
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == रिक्त ? "VIM" : (अक्षर *)title));
}

#जर defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
स्थिर LPCSTR mshape_idcs[] =
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

    शून्य
mch_set_mouse_shape(पूर्णांक shape)
{
    LPCSTR idc;

    जर (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    अन्यथा
    {
	जर (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	अन्यथा
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(रिक्त, idc));
	जर (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (शून्य)GetCursorPos(&mp);
	    (शून्य)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#जर defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    स्थिर WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    पूर्णांक len;
    WCHAR *res;

    tmp = convert_filter(s);
    जर (tmp == रिक्त)
	परत रिक्त;
    len = (पूर्णांक)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    परत res;
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
	पूर्णांक saving,
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
    पूर्णांक			i;
    WCHAR		*titlep = रिक्त;
    WCHAR		*extp = रिक्त;
    WCHAR		*initdirp = रिक्त;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    जर (dflt == रिक्त)
	fileBuf[0] = NUL;
    अन्यथा
    {
	wp = enc_to_utf16(dflt, रिक्त);
	जर (wp == रिक्त)
	    fileBuf[0] = NUL;
	अन्यथा
	{
	    साठी (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# अन्यथा
    fileStruct.lStructSize = आकार(fileStruct);
# endif

    जर (title != रिक्त)
	titlep = enc_to_utf16(title, रिक्त);
    fileStruct.lpstrTitle = titlep;

    जर (ext != रिक्त)
	extp = enc_to_utf16(ext, रिक्त);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    जर (initdir != रिक्त && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, रिक्त);
	जर (initdirp != रिक्त)
	{
	    साठी (wp = initdirp; *wp != NUL; ++wp)
		जर (*wp == '/')
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
    जर (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    जर (saving)
	ret = GetSaveFileNameW(&fileStruct);
    अन्यथा
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    जर (!ret)
	परत रिक्त;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, रिक्त);
    जर (p == रिक्त)
	परत रिक्त;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    परत q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    स्थिर char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    अचिन्हित	s_len = (अचिन्हित)STRLEN(s);
    अचिन्हित	i;

    res = alloc(s_len + 3);
    जर (res != रिक्त)
    {
	साठी (i = 0; i < s_len; ++i)
	    जर (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    अन्यथा
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    परत res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    परत gui_mch_browse(0, title, (char_u *)_("Not Used"), रिक्त,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    स्थिर शून्य
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    अक्षर    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, रिक्त, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    जर (fnames != रिक्त)
	साठी (i = 0; i < cFiles; ++i)
	{
	    जर (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, रिक्त);
	    अन्यथा
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    जर (fnames == रिक्त)
	परत;

    पूर्णांक kbd_modifiers = get_active_modifiers();

    जर ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    जर ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    जर ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    स्थिर पूर्णांक
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    पूर्णांक pos)
{
    स्थिर UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    दीर्घ	val;
    पूर्णांक		dragging = FALSE;
    पूर्णांक		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = आकार(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    जर (sb == रिक्त)
	परत 0;

    जर (sb->wp != रिक्त)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    अन्यथा	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->मूल्य;

    निवड (code)
    {
	स्थिती SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    जर (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    थांबा;
	स्थिती SB_LINEDOWN:
	    val++;
	    थांबा;
	स्थिती SB_LINEUP:
	    val--;
	    थांबा;
	स्थिती SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    थांबा;
	स्थिती SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    थांबा;
	स्थिती SB_शीर्ष:
	    val = 0;
	    थांबा;
	स्थिती SB_BOTTOM:
	    val = sb_info->max;
	    थांबा;
	स्थिती SB_ENDSCROLL:
	    जर (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		जर (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    थांबा;

	पूर्वनिर्धारित:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    परत 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    जर (sb->wp != रिक्त)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_डावा) ? SBAR_उजवा : SBAR_डावा].id;

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

    परत 0;
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

स्थिर BalloonEval  *cur_beval = रिक्त;
स्थिर UINT_PTR	    beval_timer_id = 0;
स्थिर DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
स्थिर UINT	s_menu_id = 100;
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
स्थिर PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	स्थिरांक अक्षर *caption);
स्थिर LPWORD lpwAlign(LPWORD);
स्थिर पूर्णांक nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#जर defined(FEAT_MENU) && defined(FEAT_TEAROFF)
स्थिर शून्य gui_mch_tearoff(char_u *title, vimmenu_T *menu, पूर्णांक initX, पूर्णांक initY);
#endif
स्थिर शून्य get_dialog_font_metrics(शून्य);

स्थिर पूर्णांक dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
स्थिर शून्य initialise_toolbar(शून्य);
स्थिर शून्य update_toolbar_size(शून्य);
स्थिर LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_संदेश, WPARAM wParam, LPARAM lParam);
स्थिर पूर्णांक get_toolbar_bitmap(vimmenu_T *menu);
#अन्यथा
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
स्थिर शून्य initialise_tabline(शून्य);
स्थिर LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_संदेश, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
स्थिर LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
स्थिर char_u *GetResultStr(HWND hwnd, पूर्णांक GCS, पूर्णांक *lenp);
#endif
#जर defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
प्रकारव्याख्या संरचना tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
प्रकारव्याख्या HANDLE HIMC;
# endif

स्थिर HINSTANCE hLibImm = रिक्त;
स्थिर LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
स्थिर HIMC (WINAPI *pImmGetContext)(HWND);
स्थिर HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
स्थिर BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
स्थिर BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
स्थिर BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
स्थिर BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
स्थिर BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
स्थिर BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
स्थिर BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
स्थिर BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
स्थिर शून्य dyn_imm_load(शून्य);
#अन्यथा
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
    स्थिर पूर्णांक
gui_mswin_get_menu_height(
    पूर्णांक	    fix_window)	    // If TRUE, resize window if menu height changed
{
    स्थिर पूर्णांक	old_menu_height = -1;

    RECT    rc1, rc2;
    पूर्णांक	    अंक;
    पूर्णांक	    menu_height;

    जर (gui.menu_is_active)
	अंक = Get_Menu_Item_मोजणी(s_menuBar);
    अन्यथा
	अंक = 0;

    जर (अंक == 0)
	menu_height = 0;
    अन्यथा जर (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    अन्यथा
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	जर (gui.starting)
	    menu_height = rc1.bottom - rc1.शीर्ष + 1;
	अन्यथा
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, अंक - 1, &rc2);
	    menu_height = rc2.bottom - rc1.शीर्ष + 1;
	}
    }

    जर (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    परत menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    स्थिर दीर्घ
mouse_vertical_scroll_step(शून्य)
{
    UINT val;
    जर (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	परत (val != WHEEL_PAGESCROLL) ? (दीर्घ)val : -1;
    परत 3; // Safe default;
}

    स्थिर दीर्घ
mouse_horizontal_scroll_step(शून्य)
{
    UINT val;
    जर (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	परत (दीर्घ)val;
    परत 3; // Safe default;
}

    स्थिर शून्य
init_mouse_wheel(शून्य)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    स्थिर शून्य
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, पूर्णांक horizontal)
{
    पूर्णांक		button;
    win_T	*wp;
    पूर्णांक		modifiers = 0;
    पूर्णांक		kbd_modifiers;
    पूर्णांक		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    जर (wp != रिक्त && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	जर (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_डावा : MSCR_उजवा;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	अन्यथा
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

    जर (wp == रिक्त || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    जर (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    अन्यथा
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    जर ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    जर ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    जर ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    स्थिर पूर्णांक
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = आकार(NONCLIENTMETRICSW);
    जर (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    आकार(NONCLIENTMETRICSW),
	    &nm,
	    0))
	परत FAIL;
    *lf = nm.lfMenuFont;
    परत OK;
}
#endif


#जर defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    स्थिर शून्य
set_tabline_font(शून्य)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	पंचांग;

    जर (gui_w32_get_menu_font(&lfSysmenu) != OK)
	परत;

    lfSysmenu.lfHeight = adjust_fontsize_by_dpi(lfSysmenu.lfHeight);
    font = CreateFontIndirectW(&lfSysmenu);

    SendMessage(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
    hfntOld = SelectFont(hdc, font);

    GetTextMetrics(hdc, &पंचांग);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_height = पंचांग.tmHeight + पंचांग.tmInternalLeading + 7;
}
#अन्यथा
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    स्थिर LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    निवड (param)
    {
	स्थिती SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    थांबा;
	स्थिती SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    थांबा;
	स्थिती SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    थांबा;
	पूर्वनिर्धारित:
	    थांबा;
    }
    परत 0;
}

#ifdef FEAT_NETBEANS_INTG
    स्थिर शून्य
_OnWindowPosChanged(
    HWND hwnd,
    स्थिरांक LPWINDOWPOS lpwpos)
{
    स्थिर पूर्णांक x = 0, y = 0, cx = 0, cy = 0;
    बाह्य पूर्णांक WSInitialized;

    जर (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


स्थिर HWND hwndTip = रिक्त;

    स्थिर शून्य
show_sizing_tip(पूर्णांक cols, पूर्णांक rows)
{
    TOOLINFOA ti;
    अक्षर buf[32];

    ti.cbSize = आकार(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    प्र_लिखो(buf, "%dx%d", cols, rows);
    जर (hwndTip == रिक्त)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, रिक्त,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, रिक्त, GetModuleHandle(रिक्त), रिक्त);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    अन्यथा
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    स्थिर शून्य
destroy_sizing_tip(शून्य)
{
    जर (hwndTip == रिक्त)
	परत;

    DestroyWindow(hwndTip);
    hwndTip = रिक्त;
}

    स्थिर पूर्णांक
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    पूर्णांक	    w, h;
    पूर्णांक	    valid_w, valid_h;
    पूर्णांक	    w_offset, h_offset;
    पूर्णांक	    cols, rows;

    w = lprc->उजवा - lprc->डावा;
    h = lprc->bottom - lprc->शीर्ष;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    जर (fwSide == WMSZ_डावा || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->डावा += w_offset;
    अन्यथा जर (fwSide == WMSZ_उजवा || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->उजवा -= w_offset;

    जर (fwSide == WMSZ_शीर्ष || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->शीर्ष += h_offset;
    अन्यथा जर (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    परत TRUE;
}

#ifdef FEAT_GUI_TABLINE
    स्थिर शून्य
_OnRButtonUp(HWND hwnd, पूर्णांक x, पूर्णांक y, UINT किल्ली_Flags)
{
    जर (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	जर (pt.y < rect.शीर्ष)
	{
	    show_tabline_popup_menu();
	    परत;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, किल्ली_Flags, DefWindowProcW);
}

    स्थिर शून्य
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, पूर्णांक x, पूर्णांक y, UINT किल्ली_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    जर (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	जर (pt.y < rect.शीर्ष)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, किल्ली_Flags, DefWindowProcW);
}
#endif

    स्थिर UINT
_OnNCHitTest(HWND hwnd, पूर्णांक xPos, पूर्णांक yPos)
{
    UINT	result;
    पूर्णांक		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    जर (result != HTCLIENT)
	परत result;

#ifdef FEAT_GUI_TABLINE
    जर (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	जर (yPos < rct.शीर्ष)
	    परत result;
    }
#endif
    (शून्य)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    जर (xPos < 48) // <VN> TODO should use system metric?
	परत HTBOTTOMLEFT;
    अन्यथा
	परत HTBOTTOMRIGHT;
}

#जर defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    स्थिर LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    निवड (hdr->code)
    {
	स्थिती TTN_GETDISPINFOW:
	स्थिती TTN_GETDISPINFO:
	    {
		char_u		*str = रिक्त;
		स्थिर शून्य	*tt_text = रिक्त;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		जर (gui_mch_showing_tabline()
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
		    जर (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			पूर्णांक idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			जर (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    जर (tp != रिक्त)
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
		अन्यथा
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(मूळ_menu, idButton);
		    जर (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		जर (str == रिक्त)
		    थांबा;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		जर (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, रिक्त);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		अन्यथा
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    माप_प्रकार		len = STRLEN(str);

		    जर (len < आकार(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == रिक्त))
			vim_strncpy((char_u *)lpdi->szText, str,
				आकार(lpdi->szText) - 1);
		    अन्यथा
			lpdi->lpszText = tt_text;
		}
	    }
	    थांबा;

# ifdef FEAT_GUI_TABLINE
	स्थिती TCN_SELCHANGE:
	    जर (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		परत 0L;
	    }
	    थांबा;

	स्थिती NM_RCLICK:
	    जर (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		परत 0L;
	    }
	    थांबा;
# endif

	पूर्वनिर्धारित:
	    थांबा;
    }
    परत DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#जर defined(MENUHINTS) && defined(FEAT_MENU)
    स्थिर LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    जर (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	स्थिर पूर्णांक  did_menu_tip = FALSE;

	जर (did_menu_tip)
	{
	    संदेश_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(मूळ_menu, (UINT)LOWORD(wParam));
	जर (pMenu != रिक्त && pMenu->strings[MENU_INDEX_TIP] != रिक्त)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = आकार(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    जर (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++संदेश_hist_off;
		संदेश((अक्षर *)pMenu->strings[MENU_INDEX_TIP]);
		--संदेश_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	परत 0L;
    }
    परत DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    स्थिर BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    पूर्णांक		old_width, old_height;
    पूर्णांक		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, आकार(lf), &lf);
    lf.lfHeight = lf.lfHeight * (पूर्णांक)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    जर (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    अन्यथा
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    परत TRUE;
}

    स्थिर LRESULT
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
    InvalidateRect(hwnd, रिक्त, TRUE);

    s_in_dpichanged = FALSE;
    परत 0L;
}


    स्थिर LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_संदेश,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_संदेश, lParam);

    s_u_संदेश = u_संदेश;
    s_wParam = wParam;

    निवड (u_संदेश)
    {
	HANDLE_संदेश(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_संदेश(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_संदेश(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_संदेश(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_संदेश(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_संदेश(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_संदेश(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_संदेश(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_संदेश(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_संदेश(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_संदेश(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_संदेश(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_संदेश(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_संदेश(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_संदेश(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_संदेश(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    स्थिती WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	परत FALSE;		// Do NOT allow system to go down.

    स्थिती WM_ENDSESSION:
	जर (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    परत 0L;
	}
	थांबा;

    स्थिती WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (पूर्णांक)(लघु)LOWORD(lParam));
	परत 0L;

    स्थिती WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	जर (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((पूर्णांक)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (पूर्णांक)(लघु)LOWORD(lParam));
	    परत 0L;
	}
#ifdef FEAT_MENU
	अन्यथा
	    परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
#endif

    स्थिती WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
#अन्यथा
	परत 0L;
#endif

    स्थिती WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	थांबा;

    स्थिती WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	परत _DuringSizing((UINT)wParam, (LPRECT)lParam);

    स्थिती WM_MOUSEWHEEL:
    स्थिती WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_संदेश == WM_MOUSEHWHEEL);
	परत 0L;

	// Notification for change in SystemParametersInfo()
    स्थिती WM_SETTINGCHANGE:
	परत _OnSettingChange((UINT)wParam);

#जर defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    स्थिती WM_NOTIFY:
	परत _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#जर defined(MENUHINTS) && defined(FEAT_MENU)
    स्थिती WM_MENUSELECT:
	परत _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    स्थिती WM_IME_NOTIFY:
	जर (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
	परत 1L;

    स्थिती WM_IME_COMPOSITION:
	जर (!_OnImeComposition(hwnd, wParam, lParam))
	    परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
	परत 1L;
#endif
    स्थिती WM_GETDPISCALEDSIZE:
	परत _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    स्थिती WM_DPICHANGED:
	परत _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    पूर्वनिर्धारित:
#ifdef MSWIN_FIND_REPLACE
	जर (u_संदेश == s_findrep_संदेश && s_findrep_संदेश != 0)
	    _OnFindRepl();
#endif
	थांबा;
    }

    परत DefWindowProcW(hwnd, u_संदेश, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = रिक्त;

    स्थिर BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    अक्षर	buf[2048];
    अक्षर	*title = (अक्षर *)lParam;

    जर (GetWindowText(hwnd, buf, आकार(buf)))
    {
	जर (म_माला(buf, title) != रिक्त)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, रिक्त, "MDIClient", रिक्त);
	    जर (vim_parent_hwnd != रिक्त)
		परत FALSE;
	}
    }
    परत TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    शून्य
gui_mch_set_parent(अक्षर *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    जर (vim_parent_hwnd == रिक्त)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    स्थिर शून्य
ole_error(अक्षर *arg)
{
    अक्षर buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#जर defined(GUI_MAY_SPAWN)
    स्थिर अक्षर *
gvim_error(शून्य)
{
    अक्षर *संदेश = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    जर (starting)
    {
	mch_errmsg(संदेश);
	mch_errmsg("\n");
	mch_exit(2);
    }
    परत संदेश;
}

    अक्षर *
gui_mch_do_spawn(char_u *arg)
{
    पूर्णांक			len;
# जर defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = रिक्त;
    LPWSTR		tofree1 = रिक्त;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = रिक्त, p, warg, tofree2 = रिक्त;
    STARTUPINFOW	si = {आकार(si)};
    PROCESS_INFORMATION pi;

    जर (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	जा error;
    p = wcsrchr(name, L'\\');
    जर (p == रिक्त)
	जा error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# अन्यथा
    wcscpy(p + 1, L"gvim.exe");
# endif

# जर defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    जर (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_ओळ_W();
	// Skip 1st argument.
	पर्यंत (*p && *p != L' ' && *p != L'\t')
	{
	    जर (*p == L'"')
	    {
		// Skip quoted strings
		पर्यंत (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# जर defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    अन्यथा
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	पूर्णांक	ret;

	session = vim_tempname('s', FALSE);
	जर (session == रिक्त)
	    जा error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	जर (p_bg == रिक्त)
	{
	    p_bg = savebg;
	    जा error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	जर (!ret)
	    जा error;
	wsession = enc_to_utf16(session, रिक्त);
	जर (wsession == रिक्त)
	    जा error;
	len = (पूर्णांक)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	जर (cmd == रिक्त)
	{
	    vim_free(wsession);
	    जा error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    जर (arg != रिक्त)
    {
	warg = enc_to_utf16(arg, रिक्त);
	जर (warg == रिक्त)
	    जा error;
	tofree2 = warg;
    }
    अन्यथा
	warg = L"";

    // Set up the new command line.
    len = (पूर्णांक)wcslen(name) + (पूर्णांक)wcslen(cmd) + (पूर्णांक)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    जर (newcmd == रिक्त)
	जा error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    जर (!CreateProcessW(रिक्त, newcmd, रिक्त, रिक्त, TRUE, 0,
		रिक्त, रिक्त, &si, &pi))
	जा error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# जर defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    जर (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    परत gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    शून्य
gui_mch_prepare(पूर्णांक *argc, अक्षर **argv)
{
    पूर्णांक		silent = FALSE;
    पूर्णांक		idx;

    // Check for special OLE command line parameters
    जर ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	जर (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	अन्यथा
	    idx = 1;

	// Register Vim as an OLE Automation server
	जर (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#अन्यथा
	    जर (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	जर (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#अन्यथा
	    जर (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	जर (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#अन्यथा
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    जर (mch_is_gui_executable())
# endif
    {
	पूर्णांक	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	जर (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	पूर्णांक arg;

	साठी (arg = 1; arg < *argc; arg++)
	    जर (म_भेदन("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * आकार(अक्षर *));
		argv[*argc] = रिक्त;
		थांबा;	// enough?
	    }
    }
#endif
}

    स्थिर शून्य
load_dpi_func(शून्य)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    जर (hUser32 == रिक्त)
	जा fail;

    pGetDpiForSystem = (UINT (WINAPI *)(शून्य))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (पूर्णांक (WINAPI *)(पूर्णांक, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    जर (pSetThreadDpiAwarenessContext != रिक्त)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	जर (oldctx != रिक्त)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    जर (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    परत;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = रिक्त;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = रिक्त;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = रिक्त;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    पूर्णांक
gui_mch_init(शून्य)
{
    स्थिरांक WCHAR szVimWndClassW[] = VIM_CLASSW;
    स्थिरांक WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    जर (s_hwnd != रिक्त)
	जा theend;

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
    जर (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(रिक्त, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = रिक्त;
	wndclassw.lpszClassName = szVimWndClassW;

	जर (RegisterClassW(&wndclassw) == 0)
	    परत FAIL;
    }

    जर (vim_parent_hwnd != रिक्त)
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
		vim_parent_hwnd, रिक्त,
		g_hinst, रिक्त);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	जर (s_hwnd == रिक्त)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    अन्यथा
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	जर (IsWindow((HWND)win_socket_id) <= 0)
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
		रिक्त, रिक्त,
		g_hinst, रिक्त);
	जर (s_hwnd != रिक्त && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    जर (s_hwnd == रिक्त)
	परत FAIL;

    जर (pGetDpiForWindow != रिक्त)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#जर defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    जर (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = रिक्त;
	wndclassw.hCursor = LoadCursor(रिक्त, IDC_ARROW);
	wndclassw.hbrBackground = रिक्त;
	wndclassw.lpszMenuName = रिक्त;
	wndclassw.lpszClassName = szTextAreaClassW;

	जर (RegisterClassW(&wndclassw) == 0)
	    परत FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, रिक्त,
	g_hinst, रिक्त);

    जर (s_textArea == रिक्त)
	परत FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = रिक्त;

	जर (mch_icon_load(&hIcon) == OK && hIcon != रिक्त)
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
    s_findrep_संदेश = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = आकार(s_findrep_struct);
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
    जर (p_rop)
	(शून्य)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    परत OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    स्थिर शून्य
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    जर (mon != रिक्त)
    {
	moninfo.cbSize = आकार(MONITORINFO);
	जर (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    परत;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    शून्य
gui_mch_set_shellsize(
	पूर्णांक width,
	पूर्णांक height,
	पूर्णांक min_width UNUSED,
	पूर्णांक min_height UNUSED,
	पूर्णांक base_width UNUSED,
	पूर्णांक base_height UNUSED,
	पूर्णांक direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    पूर्णांक		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    जर (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    जर (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    अन्यथा
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
    window_rect.उजवा = window_rect.डावा + win_width;
    window_rect.bottom = window_rect.शीर्ष + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    जर (!s_in_dpichanged)
    {
	जर ((direction & RESIZE_HOR)
		&& window_rect.उजवा > workarea_rect.उजवा)
	    OffsetRect(&window_rect,
		    workarea_rect.उजवा - window_rect.उजवा, 0);

	जर ((direction & RESIZE_HOR)
		&& window_rect.डावा < workarea_rect.डावा)
	    OffsetRect(&window_rect,
		    workarea_rect.डावा - window_rect.डावा, 0);

	जर ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	जर ((direction & RESIZE_VERT)
		&& window_rect.शीर्ष < workarea_rect.शीर्ष)
	    OffsetRect(&window_rect,
		    0, workarea_rect.शीर्ष - window_rect.शीर्ष);
    }

    MoveWindow(s_hwnd, window_rect.डावा, window_rect.शीर्ष,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    शून्य
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    दीर्घ	val,
    दीर्घ	size,
    दीर्घ	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    पर्यंत (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    जर (sb->scroll_shift > 0)
	++size;

    info.cbSize = आकार(info);
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
    शून्य
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    शून्य
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    शून्य
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    शून्य
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
    स्थिर LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    जर (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	परत lResult;
    निवड (dwCommand)
    {
	स्थिती IMN_SETOPENSTATUS:
	    जर (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		जर (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		जर (State & MODE_घाला)
		{
# जर defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    जर (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			पूर्णांक old_row = gui.row;
			पूर्णांक old_col = gui.col;

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
	    थांबा;
    }
    pImmReleaseContext(hWnd, hImc);
    परत lResult;
}

    स्थिर LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    पूर्णांक		len;

    जर ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	परत 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    जर (ret != रिक्त)
    {
	add_to_आदान_buf_csi(ret, len);
	vim_free(ret);
	परत 1;
    }
    परत 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    स्थिर char_u *
GetResultStr(HWND hwnd, पूर्णांक GCS, पूर्णांक *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = रिक्त;
    char_u	*convbuf = रिक्त;

    जर (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	परत रिक्त;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, रिक्त, 0);
    जर (ret <= 0)
	परत रिक्त;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + आकार(WCHAR));
    जर (buf == रिक्त)
	परत रिक्त;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / आकार(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    परत convbuf;
}
#endif

// For global functions we need prototypes.
#जर defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    शून्य
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    जर (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    शून्य
im_set_position(पूर्णांक row, पूर्णांक col)
{
    HIMC hImc;

    जर (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	जर (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    शून्य
im_set_active(पूर्णांक active)
{
    HIMC	hImc;
    स्थिर HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    जर (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	परत;
    }
# endif

    जर (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	परत;

    जर (p_imdisable)
    {
	जर (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    जर (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    अन्यथा जर (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    जर (!hImc)
	परत;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    जर (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	स्थिर DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	स्थिर BOOL bSaved = FALSE;

	जर (active)
	{
	    // if we have a saved conversion status, restore it
	    जर (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	अन्यथा
	{
	    // save conversion status and disable korean
	    जर (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    पूर्णांक
im_get_status(शून्य)
{
    पूर्णांक		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    जर (!gui.in_use && !gui.starting)
	परत mbyte_im_get_status();
# endif

    जर (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    परत status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    स्थिर शून्य
latin9_to_ucs(char_u *text, पूर्णांक len, WCHAR *unicodebuf)
{
    पूर्णांक		c;

    पर्यंत (--len >= 0)
    {
	c = *text++;
	निवड (c)
	{
	    स्थिती 0xa4: c = 0x20ac; थांबा;   // euro
	    स्थिती 0xa6: c = 0x0160; थांबा;   // S hat
	    स्थिती 0xa8: c = 0x0161; थांबा;   // S -hat
	    स्थिती 0xb4: c = 0x017d; थांबा;   // Z hat
	    स्थिती 0xb8: c = 0x017e; थांबा;   // Z -hat
	    स्थिती 0xbc: c = 0x0152; थांबा;   // OE
	    स्थिती 0xbd: c = 0x0153; थांबा;   // oe
	    स्थिती 0xbe: c = 0x0178; थांबा;   // Y
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
    स्थिर शून्य
RevOut( HDC hdc,
	पूर्णांक col,
	पूर्णांक row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    पूर्णांक		ix;

    साठी (ix = 0; ix < (पूर्णांक)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    स्थिर शून्य
draw_ओळ(
    पूर्णांक		x1,
    पूर्णांक		y1,
    पूर्णांक		x2,
    पूर्णांक		y2,
    COLORREF	color)
{
#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_ओळ(s_dwc, x1, y1, x2, y2, color);
    अन्यथा
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, रिक्त);
	// Note: LineTo() excludes the last pixel in the line.
	ओळ_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    स्थिर शून्य
set_pixel(
    पूर्णांक		x,
    पूर्णांक		y,
    COLORREF	color)
{
#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    अन्यथा
#endif
	SetPixel(s_hdc, x, y, color);
}

    स्थिर शून्य
fill_rect(
    स्थिरांक RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    अन्यथा
#endif
    {
	HBRUSH	hbr2;

	जर (hbr == रिक्त)
	    hbr2 = CreateSolidBrush(color);
	अन्यथा
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	जर (hbr == रिक्त)
	    DeleteBrush(hbr2);
    }
}

    शून्य
gui_mch_draw_string(
    पूर्णांक		row,
    पूर्णांक		col,
    char_u	*text,
    पूर्णांक		len,
    पूर्णांक		flags)
{
    स्थिर पूर्णांक	*padding = रिक्त;
    स्थिर पूर्णांक	pad_size = 0;
    स्थिरांक RECT	*pcliprect = रिक्त;
    UINT	foptions = 0;
    स्थिर WCHAR *unicodebuf = रिक्त;
    स्थिर पूर्णांक   *unicodepdy = रिक्त;
    स्थिर पूर्णांक	unibuflen = 0;
    पूर्णांक		n = 0;
    पूर्णांक		y;

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
    स्थिर HBRUSH	hbr_cache[2] = {रिक्त, रिक्त};
    स्थिर guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    स्थिर पूर्णांक		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    जर (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.डावा = FILL_X(col);
	rc.शीर्ष = FILL_Y(row);
	जर (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.उजवा = FILL_X(col + mb_string2cells(text, len));
	}
	अन्यथा
	    rc.उजवा = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	जर (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	अन्यथा जर (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	अन्यथा
	{
	    जर (hbr_cache[brush_lru] != रिक्त)
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
	जर (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	अन्यथा जर (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    जर (pad_size != Columns || padding == रिक्त || padding[0] != gui.char_width)
    {
	पूर्णांक	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(पूर्णांक, pad_size);
	जर (padding != रिक्त)
	    साठी (i = 0; i < pad_size; i++)
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
    जर (enc_utf8)
	साठी (n = 0; n < len; ++n)
	    जर (text[n] >= 0x80)
		थांबा;

#जर defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    जर (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    जर ((enc_utf8
		|| (enc_codepage > 0 && (पूर्णांक)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == रिक्त || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(पूर्णांक, len);

	unibuflen = len;
    }

    जर (enc_utf8 && n < len && unicodebuf != रिक्त)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	पूर्णांक		i;
	पूर्णांक		wlen;	// string length in words
	पूर्णांक		cells;	// cell width of string up to composing char
	पूर्णांक		cw;	// width of current cell
	पूर्णांक		c;

	wlen = 0;
	cells = 0;
	साठी (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    जर (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    अन्यथा
	    {
		unicodebuf[wlen++] = c;
	    }

	    जर (utf_iscomposing(c))
		cw = 0;
	    अन्यथा
	    {
		cw = utf_char2cells(c);
		जर (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    जर (unicodepdy != रिक्त)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		जर (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		अन्यथा
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#जर defined(FEAT_DIRECTX)
	जर (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	अन्यथा
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    अन्यथा जर ((enc_codepage > 0 && (पूर्णांक)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	जर (unicodebuf != रिक्त)
	{
	    जर (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    अन्यथा
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(अक्षर *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    जर (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		जर (unicodepdy != रिक्त)
		{
		    पूर्णांक i;
		    पूर्णांक cw;

		    साठी (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			जर (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    अन्यथा
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	जर (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (अक्षर *)text, len, padding);
	अन्यथा
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (अक्षर *)text, len, padding);
    }

    // Underline
    जर (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	जर (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_ओळ(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    जर (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_ओळ(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    जर (flags & DRAW_UNDERC)
    {
	पूर्णांक			x;
	पूर्णांक			offset;
	स्थिर स्थिरांक पूर्णांक	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	साठी (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    शून्य
gui_mch_flush(शून्य)
{
#जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    स्थिर शून्य
clear_rect(RECT *rcp)
{
    fill_rect(rcp, रिक्त, gui.back_pixel);
}


    शून्य
gui_mch_get_screen_dimensions(पूर्णांक *screen_w, पूर्णांक *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.उजवा - workarea_rect.डावा
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.शीर्ष
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#जर defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    शून्य
gui_mch_add_menu(
    vimmenu_T	*menu,
    पूर्णांक		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    जर (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, रिक्त);
	जर (wn == रिक्त)
	    परत;

	infow.cbSize = आकार(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	घाला_Menu_Item_W((parent == रिक्त)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    जर (parent == रिक्त)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    अन्यथा जर (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    शून्य
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (शून्य)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (पूर्णांक)mp.x, (पूर्णांक)mp.y);
}

    शून्य
gui_make_popup(char_u *path_name, पूर्णांक mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    जर (menu == रिक्त)
	परत;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    जर (mouse_pos)
    {
	पूर्णांक	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    अन्यथा जर (curwin != रिक्त)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    संदेश_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (पूर्णांक)p.x, (पूर्णांक)p.y);
}

# जर defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    शून्य
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    जर (menu != रिक्त)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    शून्य
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    पूर्णांक		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = रिक्त;

# ifdef FEAT_TEAROFF
    जर (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	घाला_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    अन्यथा
# endif
# ifdef FEAT_TOOLBAR
    जर (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	जर (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	अन्यथा
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
    अन्यथा
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, रिक्त);
	जर (wn != रिक्त)
	{
	    घाला_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	जर (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    शून्य
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    जर (menu->submenu_id == (HMENU)-1)
    {
	पूर्णांक iButton;

	iButton = (पूर्णांक)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    अन्यथा
# endif
    {
	जर (menu->parent != रिक्त
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != रिक्त)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	अन्यथा
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	जर (menu->submenu_id != रिक्त)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	जर (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	जर (menu->parent != रिक्त
		&& menu->parent->children != रिक्त
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
    स्थिर शून्य
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    पूर्णांक		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    जर (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.डावा - rct.डावा;
	y = (trect.शीर्ष -  rct.bottom  + roct.bottom);
    }
    अन्यथा
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    जर (menu->children != रिक्त)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	जर (IsWindow(menu->tearoff_handle))
	    (शून्य) SetWindowPos(menu->tearoff_handle,
				रिक्त,
				(पूर्णांक)trect.डावा,
				(पूर्णांक)trect.शीर्ष,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    शून्य
gui_mch_menu_grey(
    vimmenu_T	*menu,
    पूर्णांक	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    जर (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    अन्यथा
# endif
	(शून्य)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    जर ((menu->parent != रिक्त) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	जर (menu->children == रिक्त)
	    menuID = (WORD)(menu->id);
	अन्यथा
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	जर (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#जर defined(FEAT_GUI_DIALOG)
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
    स्थिर LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    जर (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(शून्य)SetFocus(hwnd);
	जर (dialog_default_button > IDCANCEL)
	    (शून्य)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	अन्यथा
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (शून्य)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	परत FALSE;
    }

    जर (message == WM_COMMAND)
    {
	पूर्णांक	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	जर (button >= DLG_NONBUTTON_CONTROL)
	    परत TRUE;

	// If the edit box exists, copy the string.
	जर (s_textfield != रिक्त)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, रिक्त);
	    जर (p != रिक्त)
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
	जर (button == IDOK)
	{
	    जर (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	अन्यथा
	    EndDialog(hwnd, button - IDCANCEL);
	परत TRUE;
    }

    जर ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	परत TRUE;
    }
    परत FALSE;
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

स्थिर स्थिरांक अक्षर *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    पूर्णांक
gui_mch_dialog(
    पूर्णांक		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    पूर्णांक		 dfltbutton,
    char_u	*textfield,
    पूर्णांक		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    पूर्णांक		अंक_Buttons;
    पूर्णांक		*buttonWidths, *buttonPositions;
    पूर्णांक		buttonYpos;
    पूर्णांक		nchar, i;
    DWORD	lStyle;
    पूर्णांक		dlgwidth = 0;
    पूर्णांक		dlgheight;
    पूर्णांक		editboxheight;
    पूर्णांक		horizWidth = 0;
    पूर्णांक		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    पूर्णांक		fontHeight;
    पूर्णांक		textWidth, minButtonWidth, messageWidth;
    पूर्णांक		maxDialogWidth;
    पूर्णांक		maxDialogHeight;
    पूर्णांक		scroll_flag = 0;
    पूर्णांक		vertical;
    पूर्णांक		dlgPaddingX;
    पूर्णांक		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    पूर्णांक		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    पूर्णांक		l;
    पूर्णांक		dlg_icon_width;
    पूर्णांक		dlg_icon_height;
    पूर्णांक		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    जर (!(gui.in_use || gui.starting))
#  endif
	जर (silent_mode)
	    परत dfltbutton;   // return default option
# endif

    जर (s_hwnd == रिक्त)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    अन्यथा
	dpi = pGetDpiForSystem();

    जर ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    जर (p == रिक्त)
	परत -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    जर (tbuffer == रिक्त)
	परत -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    अंक_Buttons = 1;
    साठी (i = 0; tbuffer[i] != '\0'; i++)
    {
	जर (tbuffer[i] == DLG_BUTTON_SEP)
	    अंक_Buttons++;
    }
    जर (dfltbutton >= अंक_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(पूर्णांक, अंक_Buttons);
    जर (buttonWidths == रिक्त)
	परत -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(पूर्णांक, अंक_Buttons);
    जर (buttonPositions == रिक्त)
	परत -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    जर (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    अन्यथा
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
    जर (s_hwnd == रिक्त)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.उजवा - workarea_rect.डावा - 100;
	जर (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.शीर्ष - 150;
    }
    अन्यथा
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.उजवा - rect.डावा
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	जर (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.शीर्ष
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	जर (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, आकार(अक्षर), 500);
    करा
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = रिक्त;
	साठी (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    जर (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    जर (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		जर (last_white != रिक्त)
		{
		    // break the line just after a space
		    जर (pend > last_white)
			ga.ga_len -= (पूर्णांक)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = रिक्त;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		सुरूठेवा;
	    }

	    पर्यंत (--l >= 0)
		ga_append(&ga, *pend++);
	}
	जर (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } पर्यंत (*pend != NUL);

    जर (ga.ga_data != रिक्त)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    जर (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != रिक्त && vim_strchr(p_go, GO_VERTICAL) != रिक्त);
    जर (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	करा
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    जर (pend == रिक्त)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (पूर्णांक)(pend - pstart));
	    जर (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} पर्यंत (*pend != NUL);

	जर (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	अन्यथा जर (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    जर (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	करा
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    जर (pend == रिक्त)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (पूर्णांक)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    जर (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} पर्यंत (*pend != NUL);
    }

    जर (dlgwidth < DLG_MIN_WIDTH)
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
    जर (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * अंक_Buttons;
    अन्यथा
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    जर (textfield != रिक्त)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    जर (dlgheight > maxDialogHeight)
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
    जर (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (पूर्णांक)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    अन्यथा
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    जर (textfield != रिक्त)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    जर (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    साठी (i = 0; i < अंक_Buttons; i++)
    {
	// get end of this button.
	साठी (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	जर (*pend)
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
	जर (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (अक्षर *)pstart);
	}
	अन्यथा
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (अक्षर *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += अंक_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_डावा|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (अक्षर *)message);

    // Edit box
    जर (textfield != रिक्त)
    {
	p = add_dialog_element(p, ES_डावा|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (अक्षर *)textfield);
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
    जर (textfield != रिक्त)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    अन्यथा
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = रिक्त;
    }

    // show the dialog box modally and get a return value
    nchar = (पूर्णांक)DialogBoxIndirect(
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
    (शून्य)SetFocus(s_hwnd);

    परत nchar;
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
    स्थिर PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    स्थिरांक अक्षर *caption)
{
    पूर्णांक nchar;

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

    परत p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    स्थिर LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    परत (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    स्थिर पूर्णांक
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    पूर्णांक		nChar = 0;
    पूर्णांक		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    पूर्णांक		i;
    WCHAR	*wn;

    जर (use_enc && enc_codepage >= 0 && (पूर्णांक)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, रिक्त);
	जर (wn != रिक्त)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (पूर्णांक)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    जर (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    साठी (i = 0; i < nChar; ++i)
	जर (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    परत nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    स्थिर HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    साठी ( ; menu != रिक्त; menu = menu->next)
    {
	जर (menu->modes == 0)	// this menu has just been deleted
	    सुरूठेवा;
	जर (menu_is_separator(menu->dname))
	    सुरूठेवा;
	जर ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    परत menu->submenu_id;
    }
    परत रिक्त;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    स्थिर LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    जर (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	परत (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    जर (message == WM_COMMAND)
    {
	जर ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    जर (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(शून्य)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (पूर्णांक)rect.उजवा - 8,
			 (पूर्णांक)mp.y,
			 (पूर्णांक)0,	    // reserved param
			 s_hwnd,
			 रिक्त);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	अन्यथा
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(शून्य)SetFocus(s_hwnd);
	परत TRUE;
    }
    जर ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	परत TRUE;
    }

    // When moved around, give main window the focus back.
    जर (message == WM_EXITSIZEMOVE)
	(शून्य)SetActiveWindow(s_hwnd);

    परत FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    स्थिर शून्य
get_dialog_font_metrics(शून्य)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    जर (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    अन्यथा
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

#जर defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    स्थिर शून्य
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    पूर्णांक		initX,
    पूर्णांक		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    पूर्णांक		template_len;
    पूर्णांक		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*शीर्ष_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    पूर्णांक		col, spaceWidth, len;
    पूर्णांक		columnWidths[2];
    char_u	*label, *text;
    पूर्णांक		acLen = 0;
    पूर्णांक		nameLen;
    पूर्णांक		padding0, padding1, padding2 = 0;
    पूर्णांक		sepPadding=0;
    पूर्णांक		x;
    पूर्णांक		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    पूर्णांक		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    जर (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	जर (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, रिक्त, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	परत;
    }

    /*
     * Create a new tearoff.
     */
    जर (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    जर (p == रिक्त)
	परत;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    जर (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    अन्यथा
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
    साठी (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    जर (text != रिक्त && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (पूर्णांक)STRLEN(text));
		जर (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    जर (pmenu->children != रिक्त)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    जर (columnWidths[1] == 0)
    {
	// no accelerators
	जर (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	अन्यथा
	    columnWidths[0] += spaceWidth;
    }
    अन्यथा
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    जर (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (पूर्णांक)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (पूर्णांक)STRLEN(title));
    जर (textWidth > dlgwidth)
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
    जर (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    अन्यथा
	*p++ = PixelToDialogX(initX); // x
    जर (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    अन्यथा
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
    जर (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (पूर्णांक)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    अन्यथा
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
    जर (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    अन्यथा
	menu = menu->children;
    शीर्ष_menu = menu;
    साठी ( ; menu != रिक्त; menu = menu->next)
    {
	जर (menu->modes == 0)	// this menu has just been deleted
	    सुरूठेवा;
	जर (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    सुरूठेवा;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	जर (((अक्षर *)p - (अक्षर *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    जर (newp != रिक्त)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (अक्षर *)p - (अक्षर *)pdlgtemplate);
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
	len = nameLen = (पूर्णांक)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (पूर्णांक)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	जर (menu->actext != रिक्त)
	{
	    acLen = (पूर्णांक)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	अन्यथा
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	जर (menu->children == रिक्त)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	अन्यथा
	{
	    len += (पूर्णांक)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	जर (label == रिक्त)
	    थांबा;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	जर (text == रिक्त)
	    text = label + nameLen;	    // no actext, use whole name
	पर्यंत (padding0-- > 0)
	    *text++ = ' ';
	जर (menu->actext != रिक्त)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	पर्यंत (padding1-- > 0)
	    *text++ = ' ';
	जर (menu->children != रिक्त)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	अन्यथा
	{
	    पर्यंत (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_डावा,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (अक्षर *)label);
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
	    (LPARAM)शीर्ष_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (शून्य)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#जर defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    स्थिर शून्य
initialise_toolbar(शून्य)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    रिक्त,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    आकार(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != रिक्त);

    update_toolbar_size();
}

    स्थिर शून्य
update_toolbar_size(शून्य)
{
    पूर्णांक		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = आकार(TBMETRICS);
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

    स्थिर LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_संदेश,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_संदेश, lParam);
    परत CallWindowProc(s_toolbar_wndproc, hwnd, u_संदेश, wParam, lParam);
}

    स्थिर पूर्णांक
get_toolbar_bitmap(vimmenu_T *menu)
{
    पूर्णांक i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    जर (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = रिक्त;

	जर (menu->iconfile != रिक्त)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			रिक्त,
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
	जर (hbitmap == रिक्त
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != रिक्त ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    रिक्त,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	जर (hbitmap != रिक्त)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = रिक्त;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (पूर्णांक)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    जर (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_मोजणी)
	i = menu->iconidx;

    परत i;
}
#endif

#जर defined(FEAT_GUI_TABLINE)
    स्थिर शून्य
initialise_tabline(शून्य)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, रिक्त, g_hinst, रिक्त);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    स्थिर tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = रिक्त;

    जर (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	जर (s_tabhwnd == hWnd)
	{
	    पूर्णांक idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    जर (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    परत ptp;
}

स्थिर POINT	    s_pt = {0, 0};
स्थिर HCURSOR      s_hCursor = रिक्त;

    स्थिर LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_संदेश,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    पूर्णांक		nCenter;
    पूर्णांक		idx0;
    पूर्णांक		idx1;

    HandleMouseHide(u_संदेश, lParam);

    निवड (u_संदेश)
    {
	स्थिती WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		थांबा;
	    }
	स्थिती WM_MOUSEMOVE:
	    जर (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		जर (असल(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(रिक्त, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    जर (tp != रिक्त)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.डावा + (rect.उजवा - rect.डावा) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			जर ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			अन्यथा जर ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    थांबा;
	स्थिती WM_LBUTTONUP:
	    {
		जर (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		थांबा;
	    }
	स्थिती WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		जर (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		थांबा;
	    }
	पूर्वनिर्धारित:
	    थांबा;
    }

    परत CallWindowProc(s_tabline_wndproc, hwnd, u_संदेश, wParam, lParam);
}
#endif

#जर defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    शून्य
gui_mch_set_foreground(शून्य)
{
    जर (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#जर defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    स्थिर शून्य
dyn_imm_load(शून्य)
{
    hLibImm = vimLoadLib("imm32.dll");
    जर (hLibImm == रिक्त)
	परत;

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

    जर (       pImmGetCompositionStringW == रिक्त
	    || pImmGetContext == रिक्त
	    || pImmAssociateContext == रिक्त
	    || pImmReleaseContext == रिक्त
	    || pImmGetOpenStatus == रिक्त
	    || pImmSetOpenStatus == रिक्त
	    || pImmGetCompositionFontW == रिक्त
	    || pImmSetCompositionFontW == रिक्त
	    || pImmSetCompositionWindow == रिक्त
	    || pImmGetConversionStatus == रिक्त
	    || pImmSetConversionStatus == रिक्त)
    {
	FreeLibrary(hLibImm);
	hLibImm = रिक्त;
	pImmGetContext = रिक्त;
	परत;
    }

    परत;
}

#endif

#जर defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

प्रकारव्याख्या संरचना _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    शून्य
gui_mch_drawsign(पूर्णांक row, पूर्णांक col, पूर्णांक typenr)
{
    signicon_t *sign;
    पूर्णांक x, y, w, h;

    जर (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == रिक्त)
	परत;

# जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    निवड (sign->uType)
    {
	स्थिती IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    थांबा;
	स्थिती IMAGE_ICON:
	स्थिती IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, रिक्त, DI_NORMAL);
	    थांबा;
# ifdef FEAT_XPM_W32
	स्थिती IMAGE_XPM:
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
	    थांबा;
# endif
    }
}

    स्थिर शून्य
close_signicon_image(signicon_t *sign)
{
    जर (sign == रिक्त)
	परत;

    निवड (sign->uType)
    {
	स्थिती IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    थांबा;
	स्थिती IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    थांबा;
	स्थिती IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    थांबा;
# ifdef FEAT_XPM_W32
	स्थिती IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    थांबा;
# endif
    }
}

    शून्य *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = रिक्त;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    जर (ext > signfile)
    {
	पूर्णांक do_load = 1;

	जर (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	अन्यथा जर (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	अन्यथा जर (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	अन्यथा
	    do_load = 0;

	जर (do_load)
	    sign.hImage = (HANDLE)LoadImage(रिक्त, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	जर (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((अक्षर *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = रिक्त;
    जर (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != रिक्त)
	*psign = sign;

    जर (!psign)
    {
	जर (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    परत (शून्य *)psign;

}

    शून्य
gui_mch_destroy_sign(शून्य *sign)
{
    जर (sign == रिक्त)
	परत;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#जर defined(FEAT_BEVAL_GUI)

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

    स्थिर शून्य
make_tooltip(BalloonEval *beval, अक्षर *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    जर (pti == रिक्त)
	परत;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    रिक्त, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, रिक्त, g_hinst, रिक्त);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = आकार(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, रिक्त);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    जर (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.उजवा);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.डावा = pt.x - 3;
    pti->rect.शीर्ष = pt.y - 3;
    pti->rect.उजवा = pt.x + 3;
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

    स्थिर शून्य
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    स्थिर VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_संदेश UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    जर (cur_beval == रिक्त || cur_beval->showState == ShS_SHOWING || !p_beval)
	परत;

    GetCursorPos(&pt);
    जर (WindowFromPoint(pt) != s_textArea)
	परत;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    जर (!PtInRect(&rect, pt))
	परत;

    जर (last_user_activity > 0
	    && (dwTime - last_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| असल(cur_beval->x - pt.x) > 3
		|| असल(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	जर (cur_beval->संदेश_CB != रिक्त)
	    (*cur_beval->संदेश_CB)(cur_beval, 0);
    }
}

    शून्य
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    शून्य
gui_mch_enable_beval_area(BalloonEval *beval)
{
    जर (beval == रिक्त)
	परत;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    शून्य
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->संदेश);
    beval->संदेश = mesg == रिक्त ? रिक्त : vim_strsave(mesg);
    जर (beval->संदेश == रिक्त)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	परत;
    }

    जर (beval->showState == ShS_SHOWING)
	परत;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    जर (असल(beval->x - pt.x) < 3 && असल(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (अक्षर *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    शून्य	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    शून्य	(*mesgCB)(BalloonEval *, पूर्णांक),
    शून्य	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    जर (mesg != रिक्त && mesgCB != रिक्त)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	परत रिक्त;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    जर (beval != रिक्त)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->संदेश = mesg;
	beval->संदेश_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	जर (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    परत beval;
}

    स्थिर शून्य
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    जर (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	परत;

    जर (cur_beval == रिक्त)
	परत;

    निवड (pnmh->code)
    {
	स्थिती TTN_SHOW:
	    थांबा;
	स्थिती TTN_काढा: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    थांबा;
	स्थिती TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    थांबा;
	स्थिती TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    थांबा;
    }
}

    स्थिर शून्य
track_user_activity(UINT u_संदेश)
{
    जर ((u_संदेश >= WM_MOUSEFIRST && u_संदेश <= WM_MOUSELAST)
	    || (u_संदेश >= WM_KEYFIRST && u_संदेश <= WM_KEYLAST))
	last_user_activity = Get_Tick_मोजणी();
}

    शून्य
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#जर defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    शून्य
netbeans_draw_multisign_indicator(पूर्णांक row)
{
    पूर्णांक i;
    पूर्णांक y;
    पूर्णांक x;

    जर (!netbeans_active())
	परत;

    x = 0;
    y = TEXT_Y(row);

# जर defined(FEAT_DIRECTX)
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    साठी (i = 0; i < gui.char_height - 3; i++)
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

#जर defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    स्थिर पूर्णांक
test_gui_w32_sendevent_mouse(dict_T *args)
{
    जर (!dict_has_किल्ली(args, "row") || !dict_has_किल्ली(args, "col"))
	परत FALSE;

    // Note: "move" is optional, requires fewer arguments
    पूर्णांक move = (पूर्णांक)dict_get_bool(args, "move", FALSE);

    जर (!move && (!dict_has_किल्ली(args, "button")
	    || !dict_has_किल्ली(args, "multiclick")
	    || !dict_has_किल्ली(args, "modifiers")))
	परत FALSE;

    पूर्णांक row = (पूर्णांक)dict_get_संख्या(args, "row");
    पूर्णांक col = (पूर्णांक)dict_get_संख्या(args, "col");

    जर (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	जर (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    पूर्णांक pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    पूर्णांक pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	अन्यथा
	    gui_mouse_moved(col, row);
    }
    अन्यथा
    {
	पूर्णांक button = (पूर्णांक)dict_get_संख्या(args, "button");
	पूर्णांक repeated_click = (पूर्णांक)dict_get_संख्या(args, "multiclick");
	int_u mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    परत TRUE;
}

    स्थिर पूर्णांक
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    आदान_2 inputs[1];
    आदान_2 modkeys[3];
    SecureZeroMemory(inputs, आकार(आदान_2));
    SecureZeroMemory(modkeys, 3 * आकार(आदान_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    जर (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_संख्या_def(args, "keycode", 0);
	जर (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (दीर्घ)vkCode);
	    परत FALSE;
	}

	BOOL is_Mod_किल्ली = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	पूर्णांक mods = (पूर्णांक)dict_get_संख्या(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	जर (mods && STRICMP(event, "keydown") == 0 && !is_Mod_किल्ली)
	{
	    पूर्णांक n = 0;
	    जर (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = आदान_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    जर (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = आदान_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    जर (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = आदान_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    जर (n)
	    {
		(शून्य)SetForegroundWindow(s_hwnd);
		Send_आदान(n, modkeys, आकार(आदान_2));
	    }
	}

	inputs[0].type = आदान_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	जर (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    जर (!is_Mod_किल्ली)
		unwrapMods = TRUE;
	}

	(शून्य)SetForegroundWindow(s_hwnd);
	Send_आदान(ARRAYSIZE(inputs), inputs, आकार(आदान_2));
	vim_free(event);

	जर (unwrapMods)
	{
	    modkeys[0].type = आदान_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = आदान_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = आदान_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (शून्य)SetForegroundWindow(s_hwnd);
	    Send_आदान(3, modkeys, आकार(आदान_2));
	}
    }
    अन्यथा
    {
	जर (event == रिक्त)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	अन्यथा
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	परत FALSE;
    }
    परत TRUE;
}

    स्थिर पूर्णांक
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    पूर्णांक handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    जर (strategy)
    {
	जर (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	अन्यथा जर (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    जर (!handled)
    {
	जर (strategy == रिक्त)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	अन्यथा
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	परत FALSE;
    }
    परत TRUE;
}


    पूर्णांक
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    जर (STRICMP(event, "key") == 0)
	परत test_gui_w32_sendevent_keyboard(args);
    अन्यथा जर (STRICMP(event, "mouse") == 0)
	परत test_gui_w32_sendevent_mouse(args);
    अन्यथा जर (STRICMP(event, "set_keycode_trans_strategy") == 0)
	परत test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    अन्यथा
    {
	semsg(_(e_invalid_मूल्य_for_argument_str_str), "event", event);
	परत FALSE;
    }
}
#endif

#जर defined(FEAT_IMAGE_GUI)

प्रकारव्याख्या संरचना
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
    स्थिर शून्य
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, पूर्णांक iw, पूर्णांक ih, image_format_T fmt)
{
    पूर्णांक		y, x;
    पूर्णांक		stride = iw * 4;
    पूर्णांक		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    साठी (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (माप_प्रकार)y * iw * src_bpp;
	uint8_t *d = dst + (माप_प्रकार)y * stride;

	साठी (x = 0; x < iw; x++)
	{
	    जर (fmt == IMAGE_FORMAT_RGBA)
	    {
		पूर्णांक a = s[3];

		जर (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		अन्यथा जर (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		अन्यथा
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    अन्यथा
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

    पूर्णांक
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    पूर्णांक		iw, ih;
    शून्य	*bits = रिक्त;

    जर (ctx == रिक्त)
	परत FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = आकार(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_मोजणी = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, रिक्त, 0);
    जर (hbm == रिक्त || bits == रिक्त)
    {
	जर (hbm != रिक्त)
	    DeleteObject(hbm);
	परत false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    जर (mem_dc == रिक्त)
    {
	DeleteObject(hbm);
	परत false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    परत OK;
}

    शून्य
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    पूर्णांक
image_placement_gui_init(image_placement_T *place UNUSED)
{
    परत OK;
}

    शून्य
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    शून्य
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    पूर्णांक		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    जर (rects == रिक्त || n_rects == 0)
	परत;

# जर defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    जर (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    साठी (पूर्णांक i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	पूर्णांक		row, col;
	पूर्णांक		posx, posy;
	पूर्णांक		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    शून्य
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
