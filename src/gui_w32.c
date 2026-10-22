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

#include "vim.h"

#නම් defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#endif

// values for "dead_key"
#define DEAD_KEY_OFF			0	// no dead key
#define DEAD_KEY_SET_DEFAULT		1	// dead key pressed
#define DEAD_KEY_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_KEY_SKIP_ON_CHAR		3	// skip next _OnChar()

#නම් defined(FEAT_DIRECTX)
static DWriteContext *s_dwc = NULL;
static පූර්ණ_සංඛ්‍යා s_directx_enabled = 0;
static පූර්ණ_සංඛ්‍යා s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != NULL && enc_utf8)
static පූර්ණ_සංඛ්‍යා directx_enabled(void);
static void directx_binddc(void);
#endif

#ifdef FEAT_MENU
static පූර්ණ_සංඛ්‍යා gui_mswin_get_menu_height(පූර්ණ_සංඛ්‍යා fix_window);
#නැතහොත්
# define gui_mswin_get_menu_height(fix_window)	0
#endif

typedef sa_nracanaa keycode_trans_strategy {
    void (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
    void (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
    void (*ptr_process_message_usual_key) (UINT /*vk*/, const MSG* /*pmsg*/);
    පූර්ණ_සංඛ්‍යා  (*ptr_get_active_modifiers)(void);
    පූර්ණ_සංඛ්‍යා  (*is_experimental)(void);
} keycode_trans_strategy;

// forward declarations for input instance initializer
static void _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
static void _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
static void process_message_usual_key_experimental(UINT /*vk*/, const MSG* /*pmsg*/);
static පූර්ණ_සංඛ්‍යා  get_active_modifiers_experimental(void);
static පූර්ණ_සංඛ්‍යා  is_experimental_true(void);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_key_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
static void _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
static void _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, පූර්ණ_සංඛ්‍යා /*cRepeat UNUSED*/);
static void process_message_usual_key_classic(UINT /*vk*/, const MSG* /*pmsg*/);
static පූර්ණ_සංඛ්‍යා  get_active_modifiers_classic(void);
static පූර්ණ_සංඛ්‍යා  is_experimental_false(void);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_key_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = NULL;

static පූර්ණ_සංඛ්‍යා is_experimental_true(void)
{
    ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා is_experimental_false(void)
{
    ආපසු 0;
}

/*
 * Initialize the keycode translation strategy.
 */
static void keycode_trans_strategy_init(void)
{
    const var_na *strategy = NULL;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    නම් (strategy == NULL)
    {
	ආපසු;
    }

    නම් (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	ආපසු;
    }

    නම් (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	ආපසු;
    }

}

#නම් defined(FEAT_RENDER_OPTIONS)
    පූර්ණ_සංඛ්‍යා
gui_mch_set_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    පූර්ණ_සංඛ්‍යා	    dx_enable = 0;
    පූර්ණ_සංඛ්‍යා	    dx_flags = 0;
    float   dx_gamma = 0.0f;
    float   dx_contrast = 0.0f;
    float   dx_level = 0.0f;
    පූර්ණ_සංඛ්‍යා	    dx_geom = 0;
    පූර්ණ_සංඛ්‍යා	    dx_renmode = 0;
    පූර්ණ_සංඛ්‍යා	    dx_taamode = 0;
    // parse string as rendering options.
    සඳහා (p = s; p != NULL && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  value[128];

	copy_option_part(&p, item, parimaa_na(item), ",");
	නම් (p == NULL)
	    bha_nga;
	q = &item[0];
	copy_option_part(&q, name, parimaa_na(name), ":");
	නම් (q == NULL)
	    ආපසු FAIL;
	copy_option_part(&q, value, parimaa_na(value), ":");

	නම් (STRCMP(name, "type") == 0)
	{
	    නම් (STRCMP(value, "directx") == 0)
		dx_enable = 1;
	    නැතහොත්
		ආපසු FAIL;
	}
	නැතහොත් නම් (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (float)atof((var_na *)value);
	}
	නැතහොත් නම් (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (float)atof((var_na *)value);
	}
	නැතහොත් නම් (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (float)atof((var_na *)value);
	}
	නැතහොත් නම් (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((var_na *)value);
	    නම් (dx_geom < 0 || dx_geom > 2)
		ආපසු FAIL;
	}
	නැතහොත් නම් (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((var_na *)value);
	    නම් (dx_renmode < 0 || dx_renmode > 6)
		ආපසු FAIL;
	}
	නැතහොත් නම් (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((var_na *)value);
	    නම් (dx_taamode < 0 || dx_taamode > 3)
		ආපසු FAIL;
	}
	නැතහොත් නම් (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	නැතහොත්
	    ආපසු FAIL;
    }

    නම් (!gui.in_use)
	ආපසු OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    නම් (dx_enable)
    {
	නම් (!directx_enabled())
	    ආපසු FAIL;
	DWriteContext_SetRenderingParams(s_dwc, NULL);
	නම් (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    නම් (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    නම් (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    නම් (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    නම් (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    නම් (dx_flags & (1 << 4))
		param.renderingMode = dx_renmode;
	    නම් (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ආපසු OK;
# නැතහොත්
    ආපසු FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (පූර්ණ_සංඛ්‍යා)(short)LOWORD(lParam), (පූර්ණ_සංඛ්‍යා)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (පූර්ණ_සංඛ්‍යා)(short)LOWORD(lParam), (පූර්ණ_සංඛ්‍යා)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (පූර්ණ_සංඛ්‍යා)(short)LOWORD(lParam), (පූර්ණ_සංඛ්‍යා)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#include "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#include <shellapi.h>
#include <commctrl.h>
#include <windowsx.h>

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

static void _OnPaint(HWND hwnd);
static void fill_rect(const RECT *rcp, HBRUSH hbr, COLORREF color);
static void clear_rect(RECT *rcp);

static WORD		s_dlgfntheight;		// height of the dialog font
static WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
static HMENU		s_menuBar = NULL;
#endif
#ifdef FEAT_TEAROFF
static void rebuild_tearoff(vimmenu_T *menu);
static HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
static පූර්ණ_සංඛ්‍යා		s_busy_processing = FALSE;

static පූර්ණ_සංඛ්‍යා		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
static UINT		s_findrep_msg = 0;
static FINDREPLACEW	s_findrep_struct;
static HWND		s_findrep_hwnd = NULL;
static පූර්ණ_සංඛ්‍යා		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = NULL;
static HDC		s_hdc = NULL;
static HBRUSH		s_brush = NULL;

#ifdef FEAT_TOOLBAR
static HWND		s_toolbarhwnd = NULL;
static WNDPROC		s_toolbar_wndproc = NULL;
#endif

#ifdef FEAT_GUI_TABLINE
static HWND		s_tabhwnd = NULL;
static WNDPROC		s_tabline_wndproc = NULL;
static පූර්ණ_සංඛ්‍යා		showing_tabline = 0;
#endif

static WPARAM		s_wParam = 0;

static HWND		s_textArea = NULL;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
static bool		s_textarea_bg_only = false;

static UINT		s_uMsg = 0;

static char_u		*s_textfield; // Used by dialogs to pass back strings

static පූර්ණ_සංඛ්‍යා		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
static පූර්ණ_සංඛ්‍යා allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
typedef HANDLE DPI_AWARENESS_CONTEXT;

typedef ga_nanaa DPI_AWARENESS {
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
static පූර්ණ_සංඛ්‍යා		s_dpi = DEFAULT_DPI;
static BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
static DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
static RECT		s_suggested_rect;

static UINT (WINAPI *pGetDpiForSystem)(void) = NULL;
static UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = NULL;
static පූර්ණ_සංඛ්‍යා (WINAPI *pGetSystemMetricsForDpi)(පූර්ණ_සංඛ්‍යා, UINT) = NULL;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
static DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = NULL;
#ifdef FEAT_MBYTE_IME
static DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = NULL;
#endif

static HINSTANCE hLibDwm = NULL;
static HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
static void dyn_dwm_load(void);

static පූර්ණ_සංඛ්‍යා fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

static HINSTANCE hUxThemeLib = NULL;
static DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = NULL;
static void (WINAPI *pFlushMenuThemes)(void) = NULL;
static void dyn_uxtheme_load(void);
#endif

    static පූර්ණ_සංඛ්‍යා WINAPI
stubGetSystemMetricsForDpi(පූර්ණ_සංඛ්‍යා nIndex, UINT dpi UNUSED)
{
    ආපසු GetSystemMetrics(nIndex);
}

    static පූර්ණ_සංඛ්‍යා
adjust_fontsize_by_dpi(පූර්ණ_සංඛ්‍යා size)
{
    ආපසු size * s_dpi / (පූර්ණ_සංඛ්‍යා)pGetDpiForSystem();
}

    static පූර්ණ_සංඛ්‍යා
adjust_by_system_dpi(පූර්ණ_සංඛ්‍යා size)
{
    ආපසු size * (පූර්ණ_සංඛ්‍යා)pGetDpiForSystem() / DEFAULT_DPI;
}

#නම් defined(FEAT_DIRECTX)
    static පූර්ණ_සංඛ්‍යා
directx_enabled(void)
{
    නම් (s_dwc != NULL)
	ආපසු 1;
    නැතහොත් නම් (s_directx_load_attempted)
	ආපසු 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ආපසු s_dwc != NULL ? 1 : 0;
}

    static void
directx_binddc(void)
{
    නම් (s_textArea == NULL)
	ආපසු;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

extern පූර්ණ_සංඛ්‍යා current_font_height;	    // this is in os_mswin.c

static sa_nracanaa
{
    UINT    key_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_keys[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_LEFT,		'k', 'l'},
    {VK_RIGHT,		'k', 'r'},

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
    {VK_INSERT,		'k', 'I'},
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
static පූර්ණ_සංඛ්‍යා	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
static පූර්ණ_සංඛ්‍යා	s_getting_focus = FALSE;

static පූර්ණ_සංඛ්‍යා	s_x_pending;
static පූර්ණ_සංඛ්‍යා	s_y_pending;
static UINT	s_kFlags_pending;
static UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
static පූර්ණ_සංඛ්‍යා	s_timed_out = FALSE;
static පූර්ණ_සංඛ්‍යා	dead_key = DEAD_KEY_OFF;
static UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
static void Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
static void track_user_activity(UINT uMsg);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
static LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
static LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
static LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#නම් defined(FEAT_BROWSE)
static char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    static void
print_windows_error(void)
{
    LPVOID  lpMsgBuf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  NULL, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lpMsgBuf, 0, NULL);
    TRACE1("Error: %s\n", lpMsgBuf);
    LocalFree(lpMsgBuf);
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

static පූර්ණ_සංඛ්‍යා		blink_state = BLINK_NONE;
static long_u		blink_waittime = 700;
static long_u		blink_ontime = 400;
static long_u		blink_offtime = 250;
static UINT_PTR		blink_timer = 0;

    පූර්ණ_සංඛ්‍යා
gui_mch_is_blinking(void)
{
    ආපසු blink_state != BLINK_NONE;
}

    පූර්ණ_සංඛ්‍යා
gui_mch_is_blink_off(void)
{
    ආපසු blink_state == BLINK_OFF;
}

    void
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    static VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT uMsg UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    MSG msg;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(NULL, idEvent);

    // Eat spurious WM_TIMER messages
    අතරතුර (PeekMessageW(&msg, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    නම් (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(NULL, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    නැතහොත්
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(NULL, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    static void
gui_mswin_rm_blink_timer(void)
{
    MSG msg;

    නම් (blink_timer == 0)
	ආපසු;

    KillTimer(NULL, blink_timer);
    // Eat spurious WM_TIMER messages
    අතරතුර (PeekMessageW(&msg, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    void
gui_mch_stop_blink(පූර්ණ_සංඛ්‍යා may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    නම් (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    void
gui_mch_start_blink(void)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    නම් (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(NULL, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    static VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT uMsg UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    MSG msg;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(NULL, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    අතරතුර (PeekMessageW(&msg, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    නම් (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    static void
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    dead_key = DEAD_KEY_SET_DEFAULT;
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
    static පූර්ණ_සංඛ්‍යා
char_to_string(පූර්ණ_සංඛ්‍යා ch, char_u *string, පූර්ණ_සංඛ්‍යා slen, පූර්ණ_සංඛ්‍යා had_alt)
{
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		i;
    WCHAR	wstring[2];
    char_u	*ws = NULL;

    නම් (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    නැතහොත් නම් (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ආපසු 0;
    }
    නැතහොත්
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    නම් (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, NULL);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	නම් (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, NULL);
	    නම් (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    නැතහොත්
    {
	ws = utf16_to_enc(wstring, &len);
	නම් (ws == NULL)
	    len = 0;
	නැතහොත්
	{
	    නම් (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    නම් (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    සඳහා (i = 0; i < len; ++i)
	නම් (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (පූර්ණ_සංඛ්‍යා)KE_CSI;
	    len += 2;
	}

    ආපසු len;
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
    static පූර්ණ_සංඛ්‍යා
get_active_modifiers_experimental(void)
{
    පූර්ණ_සංඛ්‍යා modifiers = 0;

    නම් (GetKeyState(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    නම් (GetKeyState(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    නම් (GetKeyState(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    නම් ((modifiers & MOD_MASK_CTRL) && (GetKeyState(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    නම් (!(GetKeyState(VK_CONTROL) & 0x8000)
	    &&  (GetKeyState(VK_RMENU) & 0x8000)
	    && !(GetKeyState(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    ආපසු modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    static පූර්ණ_සංඛ්‍යා
get_active_modifiers_classic(void)
{
    පූර්ණ_සංඛ්‍යා modifiers = 0;

    නම් (GetKeyState(VK_SHIFT) & 0x8000)
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
    නම් (GetKeyState(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    නම් (GetKeyState(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    ආපසු modifiers;
}

    static පූර්ණ_සංඛ්‍යා
get_active_modifiers(void)
{
    ආපසු get_active_modifiers_experimental();
}

    static පූර්ණ_සංඛ්‍යා
get_active_modifiers_via_ptr(void)
{
    // marshal to corresponding implementation
    ආපසු keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    static void
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    static void
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    char_u	string[40];
    පූර්ණ_සංඛ්‍යා		len = 0;
    පූර්ණ_සංඛ්‍යා		modifiers;
    පූර්ණ_සංඛ්‍යා		ch = cch;   // special keys are negative

    නම් (dead_key == DEAD_KEY_SKIP_ON_CHAR)
	ආපසු;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    නම් (dead_key != DEAD_KEY_TRANSIENT_IN_ON_CHAR)
	dead_key = DEAD_KEY_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_key(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_keys;
    ch = may_adjust_key_for_ctrl(modifiers, ch);
    --no_reduce_keys;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, NULL);
    නම් (ch == CSI)
	ch = K_CSI;

    නම් (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_input_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    නම් (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_input_buf();
	got_int = TRUE;
    }

    add_to_input_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    static void
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    char_u	string[40];
    පූර්ණ_සංඛ්‍යා		len = 0;

    dead_key = 0;

    len = char_to_string(ch, string, 40, FALSE);
    නම් (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_input_buf();
	got_int = TRUE;
    }

    add_to_input_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    static void
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    static void
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		modifiers;
    පූර්ණ_සංඛ්‍යා		ch = cch;   // special keys are negative

    dead_key = DEAD_KEY_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_active_modifiers_experimental();
    ch = simplify_key(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, NULL);
    නම් (ch == CSI)
	ch = K_CSI;

    len = 0;
    නම් (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    නම් (IS_SPECIAL((පූර්ණ_සංඛ්‍යා)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((පූර්ණ_සංඛ්‍යා)ch);
	string[len++] = K_THIRD((පූර්ණ_සංඛ්‍යා)ch);
    }
    නැතහොත්
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_input_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    static void
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    පූර්ණ_සංඛ්‍යා cRepeat UNUSED)
{
    char_u	string[40]; // Enough for multibyte character
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		modifiers;
    පූර්ණ_සංඛ්‍යා		ch = cch;   // special keys are negative

    dead_key = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_MASK_ALT;
    නම් (GetKeyState(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    නම් (GetKeyState(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_key(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, NULL);
    නම් (ch == CSI)
	ch = K_CSI;

    len = 0;
    නම් (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    නම් (IS_SPECIAL((පූර්ණ_සංඛ්‍යා)ch))
    {
	string[len++] = CSI;
	string[len++] = K_SECOND((පූර්ණ_සංඛ්‍යා)ch);
	string[len++] = K_THIRD((පූර්ණ_සංඛ්‍යා)ch);
    }
    නැතහොත්
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_input_buf(string, len);
}

    static void
_OnMouseEvent(
    පූර්ණ_සංඛ්‍යා button,
    පූර්ණ_සංඛ්‍යා x,
    පූර්ණ_සංඛ්‍යා y,
    පූර්ණ_සංඛ්‍යා repeated_click,
    UINT keyFlags)
{
    පූර්ණ_සංඛ්‍යා vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    නම් (keyFlags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    නම් (keyFlags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    නම් (GetKeyState(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    static void
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    පූර්ණ_සංඛ්‍යා x,
    පූර්ණ_සංඛ්‍යා y,
    UINT keyFlags)
{
    static LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    පූර්ණ_සංඛ්‍යා	    button = -1;
    පූර්ණ_සංඛ්‍යා	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (void)SetFocus(s_hwnd);

    නම් (s_uMsg == WM_LBUTTONDOWN || s_uMsg == WM_LBUTTONDBLCLK)
	button = MOUSE_LEFT;
    නැතහොත් නම් (s_uMsg == WM_MBUTTONDOWN || s_uMsg == WM_MBUTTONDBLCLK)
	button = MOUSE_MIDDLE;
    නැතහොත් නම් (s_uMsg == WM_RBUTTONDOWN || s_uMsg == WM_RBUTTONDBLCLK)
	button = MOUSE_RIGHT;
    නැතහොත් නම් (s_uMsg == WM_XBUTTONDOWN || s_uMsg == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    නැතහොත් නම් (s_uMsg == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	නම් (s_button_pending == MOUSE_LEFT)
	    button = MOUSE_RIGHT;
	නැතහොත්
	    button = MOUSE_LEFT;
    }

    නම් (button < 0)
	ආපසු;

    repeated_click = ((පූර්ණ_සංඛ්‍යා)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    නම් (repeated_click
	    && ((button == MOUSE_LEFT && s_button_pending == MOUSE_RIGHT)
		|| (button == MOUSE_RIGHT
		    && s_button_pending == MOUSE_LEFT)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_MIDDLE;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, keyFlags);
    }
    නැතහොත් නම් ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_RIGHT)))
    {
	නම් (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, keyFlags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, keyFlags);
    }
    නැතහොත්
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
	s_kFlags_pending = keyFlags;
    }

    s_prevTime = currentTime;
}

    static void
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
    පූර්ණ_සංඛ්‍යා x,
    පූර්ණ_සංඛ්‍යා y,
    UINT keyFlags)
{
    පූර්ණ_සංඛ්‍යා button;

    s_getting_focus = FALSE;
    නම් (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    නම් (s_uMsg == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	නම් (!(keyFlags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ආපසු;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    නැතහොත්
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, keyFlags);
}

    static void
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    පූර්ණ_සංඛ්‍යා cx UNUSED,
    පූර්ණ_සංඛ්‍යා cy UNUSED)
{
#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    static vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    පූර්ණ_සංඛ්‍යා		id)
{
    vimmenu_T	*pChildMenu;

    අතරතුර (pMenu)
    {
	නම් (pMenu->id == (UINT)id)
	    bha_nga;
	නම් (pMenu->children != NULL)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    නම් (pChildMenu)
	    {
		pMenu = pChildMenu;
		bha_nga;
	    }
	}
	pMenu = pMenu->next;
    }
    ආපසු pMenu;
}

    static void
_OnMenu(
    HWND	hwnd UNUSED,
    පූර්ණ_සංඛ්‍යා		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(root_menu, id);
    නම් (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    static void
_OnFindRepl(void)
{
    පූර්ණ_සංඛ්‍යා	    flags = 0;
    පූර්ණ_සංඛ්‍යා	    down;

    නම් (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(void)SetFocus(s_hwnd);

    නම් (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(void)SetFocus(s_hwnd);
    }
    නැතහොත් නම් (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(void)SetFocus(s_hwnd);
    }
    නැතහොත් නම් (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    නම් (flags == 0)
	ආපසු;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    නම් (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    නම් (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, NULL);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, NULL);
    නම් (p != NULL && q != NULL)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    static void
HandleMouseHide(UINT uMsg, LPARAM lParam)
{
    static LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    නම් (uMsg == WM_MOUSEMOVE || uMsg == WM_NCMOUSEMOVE)
    {
	නම් (lParam == last_lParam)
	    ආපසු;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    switch (uMsg)
    {
    pak_sa WM_KEYUP:
    pak_sa WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	නම් (p_mh)
	    gui_mch_mousehide(TRUE);
	bha_nga;

    pak_sa WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    pak_sa WM_SYSCHAR:
    pak_sa WM_MOUSEMOVE:	 // show the pointer on any mouse action
    pak_sa WM_LBUTTONDOWN:
    pak_sa WM_LBUTTONUP:
    pak_sa WM_MBUTTONDOWN:
    pak_sa WM_MBUTTONUP:
    pak_sa WM_RBUTTONDOWN:
    pak_sa WM_RBUTTONUP:
    pak_sa WM_XBUTTONDOWN:
    pak_sa WM_XBUTTONUP:
    pak_sa WM_NCMOUSEMOVE:
    pak_sa WM_NCLBUTTONDOWN:
    pak_sa WM_NCLBUTTONUP:
    pak_sa WM_NCMBUTTONDOWN:
    pak_sa WM_NCMBUTTONUP:
    pak_sa WM_NCRBUTTONDOWN:
    pak_sa WM_NCRBUTTONUP:
    pak_sa WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	bha_nga;
    }
}

    static LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(uMsg, lParam);

    s_uMsg = uMsg;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(uMsg);
#endif

    switch (uMsg)
    {
	HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_MSG(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_MSG(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_MSG(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	pak_sa WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ආපසු TRUE;
#endif
	default:
	    ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
}

    static void
dyn_dwm_load(void)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    නම් (hLibDwm == NULL)
	ආපසු;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    නම් (pDwmSetWindowAttribute == NULL)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = NULL;
	ආපසු;
    }
}

extern DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    void
gui_mch_set_titlebar_colors(void)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    නම් (pDwmSetWindowAttribute == NULL || win_version < MAKE_VER(10, 0, 22000))
	ආපසු;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    නම් (vim_strchr(p_go, GO_TITLEBAR) != NULL)
    {
	නම් (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	නැතහොත්
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	නම් (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	නම් (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, parimaa_na(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, parimaa_na(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    void
gui_mch_new_colors(void)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, NULL, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    නම් (s_textarea_bg_only && s_textArea != NULL)
	InvalidateRect(s_textArea, NULL, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    void
gui_mch_def_colors(void)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_open(void)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    නම් (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	නම් (s_textArea != NULL)
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

    ආපසු OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_get_winpos(පූර්ණ_සංඛ්‍යා *x, පූර්ණ_සංඛ්‍යා *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.left;
    *y = rect.top;
    ආපසු OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    void
gui_mch_set_winpos(පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y)
{
    SetWindowPos(s_hwnd, NULL, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    void
gui_mch_set_text_area_pos(පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y, පූර්ණ_සංඛ්‍යා w, පූර්ණ_සංඛ්‍යා h)
{
    static පූර්ණ_සංඛ්‍යා oldx = 0;
    static පූර්ණ_සංඛ්‍යා oldy = 0;

    SetWindowPos(s_textArea, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    නම් (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#නම් defined(FEAT_GUI_TABLINE)
    නම් (showing_tabline)
    {
	පූර්ණ_සංඛ්‍යා	top = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	නම් (vim_strchr(p_go, GO_TOOLBAR) != NULL)
	    top = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, top, rect.right, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    නම් (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, NULL, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    void
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    පූර්ණ_සංඛ්‍යා		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    void
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    පූර්ණ_සංඛ්‍යා		x,
    පූර්ණ_සංඛ්‍යා		y,
    පූර්ණ_සංඛ්‍යා		w,
    පූර්ණ_සංඛ්‍යා		h)
{
    SetWindowPos(sb->id, NULL, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    පූර්ණ_සංඛ්‍යා
gui_mch_get_scrollbar_xpadding(void)
{
    RECT    rcTxt, rcWnd;
    පූර්ණ_සංඛ්‍යා	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.right - rcTxt.right - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ආපසු (xpad < 0) ? 0 : xpad;
}

    පූර්ණ_සංඛ්‍යා
gui_mch_get_scrollbar_ypadding(void)
{
    RECT    rcTxt, rcWnd;
    පූර්ණ_සංඛ්‍යා	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ආපසු (ypad < 0) ? 0 : ypad;
}

    void
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    පූර්ණ_සංඛ්‍යා		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, NULL,
	g_hinst, NULL);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    static scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    නම් (gui.bottom_sbar.id == hwnd)
	ආපසු &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	නම් (wp->w_scrollbars[SBAR_LEFT].id == hwnd)
	    ආපසු &wp->w_scrollbars[SBAR_LEFT];
	නම් (wp->w_scrollbars[SBAR_RIGHT].id == hwnd)
	    ආපසු &wp->w_scrollbars[SBAR_RIGHT];
    }
    ආපසු NULL;
}

    static void
update_scrollbar_size(void)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    static void
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
    static void
GetFontSize(GuiFont font, පූර්ණ_සංඛ්‍යා *char_width, පූර්ණ_සංඛ්‍යා *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    නම් (char_width)
	*char_width = size.cx + tm.tmOverhang;
    නම් (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    static void
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_adjust_charheight(void)
{
    UpdateFontSize(gui.norm_font);
    ආපසු OK;
}

    static GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = NULL;

    // Load the font
    font = CreateFontIndirectW(lf);

    නම් (font == NULL)
	ආපසු NOFONT;

    ආපසු (GuiFont)font;
}

    static පූර්ණ_සංඛ්‍යා
pixels_to_points(පූර්ණ_සංඛ්‍යා pixels, පූර්ණ_සංඛ්‍යා vertical)
{
    පූර්ණ_සංඛ්‍යා		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ආපසු points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    පූර්ණ_සංඛ්‍යා		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    නම් (get_logfont(&lf, name, NULL, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    නම් (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ආපසු font;
}

#නම් defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    නම් (name == NULL)
	ආපසු NULL;
    ආපසු vim_strsave(name);
}
#endif

    void
gui_mch_free_font(GuiFont font)
{
    නම් (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    පූර්ණ_සංඛ්‍යා i;

    typedef sa_nracanaa SysColorTable
    {
	var_na	    *name;
	පූර්ණ_සංඛ්‍යා	    color;
    } SysColorTable;

    static SysColorTable sys_table[] =
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
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(sys_table); i++)
	නම් (STRICMP(name, sys_table[i].name) == 0)
	    ආපසු GetSysColor(sys_table[i].color);

    ආපසු gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(පූර්ණ_සංඛ්‍යා r, පූර්ණ_සංඛ්‍යා g, පූර්ණ_සංඛ්‍යා b)
{
    ආපසු gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_haskey(char_u *name)
{
    පූර්ණ_සංඛ්‍යා i;

    සඳහා (i = 0; special_keys[i].vim_code1 != NUL; i++)
	නම් (name[0] == special_keys[i].vim_code0
				       && name[1] == special_keys[i].vim_code1)
	    ආපසු OK;
    ආපසු FAIL;
}

    void
gui_mch_beep(void)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    void
gui_mch_invert_rectangle(
    පූර්ණ_සංඛ්‍යා	    r,
    පූර්ණ_සංඛ්‍යා	    c,
    පූර්ණ_සංඛ්‍යා	    nr,
    පූර්ණ_සංඛ්‍යා	    nc)
{
    RECT    rc;

#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(c);
    rc.top = FILL_Y(r);
    rc.right = rc.left + nc * gui.char_width;
    rc.bottom = rc.top + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    void
gui_mch_iconify(void)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    void
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(gui.col);
    rc.top = FILL_Y(gui.row);
    rc.right = rc.left + gui.char_width;
    නම් (mb_lefthalve(gui.row, gui.col))
	rc.right += gui.char_width;
    rc.bottom = rc.top + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    void
gui_mch_draw_part_cursor(
    පූර්ණ_සංඛ්‍යා		w,
    පූර්ණ_සංඛ්‍යා		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.left =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_RIGHT ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.top = FILL_Y(gui.row) + gui.char_height - h;
    rc.right = rc.left + w;
    rc.bottom = rc.top + h;

    fill_rect(&rc, NULL, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    static void
outputDeadKey_rePost_Ex(MSG originalMsg, පූර්ණ_සංඛ්‍යා dead_key2set)
{
    static MSG deadCharExpel;

    නම් (dead_key == DEAD_KEY_OFF)
	ආපසු;

    dead_key = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = originalMsg.message;
    deadCharExpel.hwnd    = originalMsg.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(originalMsg.hwnd, originalMsg.message, originalMsg.wParam,
							  originalMsg.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    static void
outputDeadKey_rePost(MSG originalMsg)
{
    outputDeadKey_rePost_Ex(originalMsg, DEAD_KEY_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
static void process_message_usual_key(UINT vk, const MSG *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_key(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    static void
process_message_usual_key_experimental(UINT vk, const MSG *pmsg)
{
    WCHAR	ch[8];
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    නම් (GetKeyState(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
    නම් (GetKeyState(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    නම් ((GetKeyState(VK_RMENU) & 0x8000)
				 && (GetKeyState(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    scan_code = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    නම් (len < 0)
	dead_key = DEAD_KEY_SET_DEFAULT;

    නම් (len <= 0)
    {
	පූර්ණ_සංඛ්‍යා wm_char = NUL;

	නම් (dead_key == DEAD_KEY_SET_DEFAULT
		&& (GetKeyState(VK_CONTROL) & 0x8000))
	{
	    නම් (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    නම් (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	නම් (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_key = DEAD_KEY_TRANSIENT_IN_ON_CHAR;
	}
	ආපසු;
    }

    // Post the message as TranslateMessage would do.
    නම් (pmsg->message == WM_KEYDOWN)
    {
	සඳහා (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    නැතහොත්
    {
	සඳහා (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    static void
process_message_usual_key_classic(UINT vk, const MSG *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    නම් (vk != 0xff
	    && (GetKeyState(VK_CONTROL) & 0x8000)
	    && !(GetKeyState(VK_SHIFT) & 0x8000)
	    && !(GetKeyState(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	නම් (vk == '6' || MapVirtualKey(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_input_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	නැතහොත් නම් (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_input_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	නැතහොත් නම් (vk == '2' || MapVirtualKey(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_input_buf(string, 1);
	}
	නැතහොත්
	    TranslateMessage(pmsg);
    }
    නැතහොත්
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    static void
process_message(void)
{
    MSG		msg;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		modifiers = 0;
    පූර්ණ_සංඛ්‍යා		key;
#ifdef FEAT_MENU
    static char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    static පූර්ණ_සංඛ්‍යා  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    නම් (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&msg, NULL, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    නම් (msg.message == WM_OLE)
    {
	char_u *str = (char_u *)msg.lParam;
	නම් (str == NULL || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&msg);
	}
	නැතහොත්
	{
	    add_to_input_buf(str, (පූර්ණ_සංඛ්‍යා)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ආපසු;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    නම් (s_findrep_hwnd != NULL && IsDialogMessageW(s_findrep_hwnd, &msg))
    {
	HandleMouseHide(msg.message, msg.lParam);
	ආපසු;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    නම් (msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN)
    {
	vk = (පූර්ණ_සංඛ්‍යා) msg.wParam;

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
	නම් (keycode_trans_strategy_used->is_experimental()
		&& dead_key == DEAD_KEY_SKIP_ON_CHAR)
	{
	    dead_key = DEAD_KEY_OFF;
	}

	නම් (dead_key != DEAD_KEY_OFF)
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
	    නම් (keycode_trans_strategy_used->is_experimental() &&
		    dead_key == DEAD_KEY_TRANSIENT_IN_ON_CHAR)
	    {
		outputDeadKey_rePost_Ex(msg,
				       /*dead_key2set=*/DEAD_KEY_SKIP_ON_CHAR);
		ආපසු;
	    }

	    නම් (dead_key != DEAD_KEY_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ආපසු;
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
	    නම් ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_key = DEAD_KEY_OFF;
		TranslateMessage(&msg);
		ආපසු;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    නැතහොත් නම් ((get_real_state()
			    & (MODE_INSERT | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		outputDeadKey_rePost(msg);
		ආපසු;
	    }
	}

	// Check for CTRL-BREAK
	නම් (vk == VK_CANCEL)
	{
	    trash_input_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_input_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	නම් (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&msg);
	    ආපසු;
	}

	සඳහා (i = 0; special_keys[i].key_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    නම් (special_keys[i].key_sym == vk
		    && (vk != VK_SPACE || !(GetKeyState(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		නම් (dead_key == DEAD_KEY_SET_DEFAULT
			&& (special_keys[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    outputDeadKey_rePost(msg);
		    ආපසු;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		නම් (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  NULL, NULL) == NULL)
		    bha_nga;
#endif
		modifiers = get_active_modifiers_via_ptr();

		නම් (special_keys[i].vim_code1 == NUL)
		    key = special_keys[i].vim_code0;
		නැතහොත්
		    key = TO_SPECIAL(special_keys[i].vim_code0,
						   special_keys[i].vim_code1);
		key = simplify_key(key, &modifiers);
		නම් (key == CSI)
		    key = K_CSI;

		නම් (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_input_buf(string, 3);
		}

		නම් (IS_SPECIAL(key))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(key);
		    string[2] = K_THIRD(key);
		    add_to_input_buf(string, 3);
		}
		නැතහොත්
		{
		    පූර්ණ_සංඛ්‍යා	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(key, string, 40, FALSE);
		    add_to_input_buf(string, len);
		}
		bha_nga;
	    }
	}

	// Not a special key.
	නම් (special_keys[i].key_sym == 0)
	{
	    process_message_usual_key(vk, &msg);
	}
    }
#ifdef FEAT_MBYTE_IME
    නැතහොත් නම් (msg.message == WM_IME_NOTIFY)
	_OnImeNotify(msg.hwnd, (DWORD)msg.wParam, (DWORD)msg.lParam);
    නැතහොත් නම් (msg.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&msg);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    නම් (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  NULL, NULL) == NULL)
#endif
	DispatchMessageW(&msg);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    void
gui_mch_update(void)
{
    MSG	    msg;

    නම් (!s_busy_processing)
	අතරතුර (PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE)
						  && !vim_is_input_buf_full())
	    process_message();
}

    static void
remove_any_timer(void)
{
    MSG		msg;

    නම් (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(NULL, s_wait_timer);

	// Eat spurious WM_TIMER messages
	අතරතුර (PeekMessageW(&msg, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
static garray_T	server_pending_ga = {0, 0, 0, 0, NULL};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    void
server_add_input(char_u *str)
{
    නම් (server_pending_ga.ga_data == NULL)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    static void
server_flush_input(void)
{
    char_u  *str;

    නම් (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ආපසු;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    නම් (str != NULL)
    {
	server_to_input_buf(str);
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
    පූර්ණ_සංඛ්‍යා
gui_mch_wait_for_chars(long wtime)
{
    පූර්ණ_සංඛ්‍යා		focus;

    s_timed_out = FALSE;

    නම් (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	නම් (s_busy_processing)
	    ආපසු FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(NULL, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    අතරතුර (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	නම් (gui.in_focus != focus)
	{
	    නම් (gui.in_focus)
		gui_mch_start_blink();
	    නැතහොත්
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	නම් (s_need_activate)
	{
	    (void)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	සඳහා (;;)
	{
	    MSG msg;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_input();
# endif
# ifdef FEAT_TIMERS
	    නම් (did_add_timer)
		bha_nga;
# endif
	    නම් (PeekMessageW(&msg, NULL, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		bha_nga;
	    }
	    නැතහොත් නම් (input_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || MsgWaitForMultipleObjects(0, NULL, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		bha_nga;
	}
#නැතහොත්
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	නම් (input_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    නම් (!s_getting_focus)
		s_button_pending = -1;

	    ආපසු OK;
	}

#ifdef FEAT_TIMERS
	නම් (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    bha_nga;
	}
#endif
    }
    allow_scrollbar = FALSE;
    ආපසු FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    void
gui_mch_clear_block(
    පූර්ණ_සංඛ්‍යා		row1,
    පූර්ණ_සංඛ්‍යා		col1,
    පූර්ණ_සංඛ්‍යා		row2,
    පූර්ණ_සංඛ්‍යා		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.left = FILL_X(col1);
    rc.top = FILL_Y(row1);
    rc.right = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    void
gui_mch_clear_all(void)
{
    RECT    rc;

    rc.left = 0;
    rc.top = 0;
    rc.right = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    void
gui_mch_enable_menu(පූර්ණ_සංඛ්‍යා flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : NULL);
#endif
}

    void
gui_mch_set_menu_pos(
    පූර්ණ_සංඛ්‍යා	    x UNUSED,
    පූර්ණ_සංඛ්‍යා	    y UNUSED,
    පූර්ණ_සංඛ්‍යා	    w UNUSED,
    පූර්ණ_සංඛ්‍යා	    h UNUSED)
{
    // It will be in the right place anyway
}

#නම් defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    void
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    පූර්ණ_සංඛ්‍යා		hidden)
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
    void
gui_mch_draw_menubar(void)
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
    ආපසු (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#නම් defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    static WORD
PixelToDialogX(පූර්ණ_සංඛ්‍යා numPixels)
{
    ආපසු (WORD)((numPixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    static WORD
PixelToDialogY(පූර්ණ_සංඛ්‍යා numPixels)
{
    ආපසු (WORD)((numPixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    static පූර්ණ_සංඛ්‍යා
GetTextWidth(HDC hdc, char_u *str, පූර්ණ_සංඛ්‍යා len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    ආපසු size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    static පූර්ණ_සංඛ්‍යා
GetTextWidthEnc(HDC hdc, char_u *str, පූර්ණ_සංඛ්‍යා len)
{
    SIZE	size;
    WCHAR	*wstr;
    පූර්ණ_සංඛ්‍යා		n;
    පූර්ණ_සංඛ්‍යා		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    නම් (wstr == NULL)
	ආපසු 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    නම් (n)
	ආපසු size.cx;
    ආපසු 0;
}

static void get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    static BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    පූර්ණ_සංඛ්‍යා		    wChild, hChild, wParent, hParent;
    පූර්ණ_සංඛ්‍යා		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.right - rChild.left;
    hChild = rChild.bottom - rChild.top;

    // If Vim is minimized put the window in the middle of the screen.
    නම් (hwndParent == NULL || IsMinimized(hwndParent))
	get_work_area(&rParent);
    නැතහොත්
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.right - rParent.left;
    hParent = rParent.bottom - rParent.top;

    moninfo.cbSize = parimaa_na(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    නම් (mon != NULL && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    නැතහොත්
    {
	hdc = GetDC(hwndChild);
	rScreen.left = 0;
	rScreen.top = 0;
	rScreen.right = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.left + ((wParent - wChild) / 2);
    නම් (xNew < rScreen.left)
	xNew = rScreen.left;
    නැතහොත් නම් ((xNew + wChild) > rScreen.right)
	xNew = rScreen.right - wChild;

    yNew = rParent.top + ((hParent - hChild) / 2);
    නම් (yNew < rScreen.top)
	yNew = rScreen.top;
    නැතහොත් නම් ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    ආපසු SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#නම් defined(FEAT_TOOLBAR)
    void
gui_mch_show_toolbar(පූර්ණ_සංඛ්‍යා showit)
{
    නම් (s_toolbarhwnd == NULL)
	ආපසු;

    නම් (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    නැතහොත්
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_COUNT 31

#endif

#නම් defined(FEAT_GUI_TABLINE)
    static void
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, NULL);
    නම් (wn == NULL)
	ආපසු;

    infow.cbSize = parimaa_na(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    InsertMenuItemW(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    static void
show_tabline_popup_menu(void)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    නම් (hold_gui_events || cmdwin_type != 0)
	ආපසු;

    tab_pmenu = CreatePopupMenu();
    නම් (tab_pmenu == NULL)
	ආපසු;

    නම් (first_tabpage->tp_next != NULL)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									NULL);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    නම් (rval > 0)
    {
	TCHITTESTINFO htinfo;
	පූර්ණ_සංඛ්‍යා idx;

	නම් (ScreenToClient(s_tabhwnd, &pt) == 0)
	    ආපසු;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	නම් (idx == -1)
	    idx = 0;
	නැතහොත්
	    idx += 1;

	send_tabline_menu_event(idx, (පූර්ණ_සංඛ්‍යා)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    void
gui_mch_show_tabline(පූර්ණ_සංඛ්‍යා showit)
{
    නම් (s_tabhwnd == NULL)
	ආපසු;

    නම් (!showit != !showing_tabline)
    {
	නම් (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	නැතහොත්
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_showing_tabline(void)
{
    ආපසු s_tabhwnd != NULL && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    void
gui_mch_update_tabline(void)
{
    tabpage_T	*tp;
    TCITEM	tie;
    පූර්ණ_සංඛ්‍යා		nr = 0;
    පූර්ණ_සංඛ්‍යා		curtabidx = 0;
    පූර්ණ_සංඛ්‍යා		tabadded = 0;
    WCHAR	*wstr = NULL;

    නම් (s_tabhwnd == NULL)
	ආපසු;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    සඳහා (tp = first_tabpage; tp != NULL; tp = tp->tp_next, ++nr)
    {
	නම් (tp == curtab)
	    curtabidx = nr;

	නම් (nr >= TabCtrl_GetItemCount(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    TabCtrl_InsertItem(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, NULL);
	නම් (wstr != NULL)
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
    අතරතුර (nr < TabCtrl_GetItemCount(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    නම් (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, NULL, NULL,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    නම් (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    void
gui_mch_set_curtab(පූර්ණ_සංඛ්‍යා nr)
{
    නම් (s_tabhwnd == NULL)
	ආපසු;

    නම් (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    void
gui_mch_set_dark_theme(පූර්ණ_සංඛ්‍යා dark)
{
    නම් (pDwmSetWindowAttribute != NULL && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		parimaa_na(dark));

    නම් (pSetPreferredAppMode != NULL && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    නම් (pFlushMenuThemes != NULL && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    static void
dyn_uxtheme_load(void)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    නම් (hUxThemeLib == NULL)
	ආපසු;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (void (WINAPI *)(void))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    නම් (pSetPreferredAppMode == NULL || pFlushMenuThemes == NULL)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = NULL;
	ආපසු;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    void
gui_mch_set_fullscreen(පූර්ණ_සංඛ්‍යා flag)
{
    static RECT normal_rect;
    static LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    නම් (!full_screen) // Windows not set yet.
	ආපසු;

    නම් (flag)
    {
	නම් (fullscreen_on)
	    ආපසු;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = parimaa_na(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	නම් (mon == NULL || !GetMonitorInfo(mon, &moninfo))
	    ආපසු;

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
	SetWindowPos(s_hwnd, NULL,
		moninfo.rcMonitor.left,
		moninfo.rcMonitor.top,
		moninfo.rcMonitor.right - moninfo.rcMonitor.left,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.top,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    නැතහොත්
    {
	නම් (!fullscreen_on)
	    ආපසු;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, NULL,
		normal_rect.left,
		normal_rect.top,
		normal_rect.right - normal_rect.left,
		normal_rect.bottom - normal_rect.top,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    void
ex_simalt(exarg_T *eap)
{
    char_u	*keys = eap->arg;
    පූර්ණ_සංඛ්‍යා		fill_typebuf = FALSE;
    char_u	key_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    අතරතුර (*keys)
    {
	නම් (*keys == '~')
	    *keys = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*keys, (LPARAM)0);
	keys++;
	fill_typebuf = TRUE;
    }
    නම් (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	key_name[0] = K_SPECIAL;
	key_name[1] = KS_EXTRA;
	key_name[2] = KE_NOP;
	key_name[3] = NUL;
#නම් defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#endif
	(void)ins_typebuf(key_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    static void
initialise_findrep(char_u *initial_string)
{
    පූර්ණ_සංඛ්‍යා		wword = FALSE;
    පූර්ණ_සංඛ්‍යා		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    නම් (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    නම් (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    නම් (entry_text != NULL && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, NULL);
	නම් (p != NULL)
	{
	    පූර්ණ_සංඛ්‍යා len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    static void
set_window_title(HWND hwnd, var_na *title)
{
    නම් (title != NULL)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, NULL);
	නම් (wbuf != NULL)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    නැතහොත්
	(void)SetWindowTextW(hwnd, NULL);
}

    void
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    නම් (s_findrep_msg != 0)
    {
	නම් (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	නම් (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(void)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    void
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    නම් (s_findrep_msg != 0)
    {
	නම් (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	නම් (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(void)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    void
gui_mch_mousehide(පූර්ණ_සංඛ්‍යා hide)
{
    නම් (hide == gui.pointer_hidden)
	ආපසු;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    static void
gui_mch_show_popupmenu_at(vimmenu_T *menu, පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (void)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(පූර්ණ_සංඛ්‍යා)0,	    //reserved param
	s_hwnd,
	NULL);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    static void
_OnEndSession(void)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    static void
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    static void
_OnDestroy(HWND hwnd)
{
    නම් (!destroying)
	_OnClose(hwnd);
}

    static void
_OnPaint(
    HWND hwnd)
{
    නම් (IsMinimized(hwnd))
	ආපසු;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (void)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    නම් (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.left = rect.left;
	ps.rcPaint.right = rect.right;
    }

    නම් (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	නම් (!screen_cleared || ScreenLines == NULL)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	නැතහොත්
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.left, ps.rcPaint.top,
		ps.rcPaint.right - ps.rcPaint.left + 1,
		ps.rcPaint.bottom - ps.rcPaint.top + 1);
    }

    EndPaint(hwnd, &ps);
}

    static void
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    පූර්ණ_සංඛ්‍යා cx,
    පූර්ණ_සංඛ්‍යා cy)
{
    නම් (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    static void
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (void)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    static void
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    නම් (destroying)
	ආපසු;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (void)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    static LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    ආපසු DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    void
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    void
gui_mch_getmouse(පූර්ණ_සංඛ්‍යා *x, පූර්ණ_සංඛ්‍යා *y)
{
    RECT rct;
    POINT mp;

    (void)GetWindowRect(s_textArea, &rct);
    (void)GetCursorPos(&mp);
    *x = (පූර්ණ_සංඛ්‍යා)(mp.x - rct.left);
    *y = (පූර්ණ_සංඛ්‍යා)(mp.y - rct.top);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    void
gui_mch_setmouse(පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y)
{
    RECT rct;

    (void)GetWindowRect(s_textArea, &rct);
    (void)SetCursorPos(x + gui.border_offset + rct.left,
		       y + gui.border_offset + rct.top);
}

    static void
gui_mswin_get_valid_dimensions(
    පූර්ණ_සංඛ්‍යා w,
    පූර්ණ_සංඛ්‍යා h,
    පූර්ණ_සංඛ්‍යා *valid_w,
    පූර්ණ_සංඛ්‍යා *valid_h,
    පූර්ණ_සංඛ්‍යා *cols,
    පූර්ණ_සංඛ්‍යා *rows)
{
    පූර්ණ_සංඛ්‍යා	    base_width, base_height;

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

    void
gui_mch_flash(පූර්ණ_සංඛ්‍යා msec)
{
    RECT    rc;

#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.left = 0;
    rc.top = 0;
    rc.right = gui.num_cols * gui.char_width;
    rc.bottom = gui.num_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    static BOOL
is_point_onscreen(පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y)
{
    POINT   pt = {x, y};

    ආපසු MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != NULL;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    static BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.left;
    p1.y = rc.top;
    p2.x = rc.right - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    නම් (!is_point_onscreen(p1.x, p1.y))
	ආපසු FALSE;
    නම් (!is_point_onscreen(p1.x, p2.y))
	ආපසු FALSE;
    නම් (!is_point_onscreen(p2.x, p1.y))
	ආපසු FALSE;
    නම් (!is_point_onscreen(p2.x, p2.y))
	ආපසු FALSE;
    ආපසු TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    static පූර්ණ_සංඛ්‍යා
get_scroll_flags(void)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    නම් (!is_window_onscreen(s_hwnd))
	ආපසු SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    සඳහා (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	නම් (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    නම් (IntersectRect(&rcDest, &rcVim, &rcOther))
		ආපසු SW_INVALIDATE;
	}
    ආපසු 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    static void
intel_gpu_workaround(void)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    void
gui_mch_delete_lines(
    පූර්ණ_සංඛ්‍යා	    row,
    පූර්ණ_සංඛ්‍යා	    num_lines)
{
    RECT	rc;

    rc.left = FILL_X(gui.scroll_region_left);
    rc.right = FILL_X(gui.scroll_region_right + 1);
    rc.top = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -num_lines * gui.char_height, &rc);
    }
    නැතහොත්
#endif
    {
#නම් defined(FEAT_DIRECTX)
	නම් (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -num_lines * gui.char_height,
				    &rc, &rc, NULL, NULL, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - num_lines + 1,
						       gui.scroll_region_left,
	gui.scroll_region_bot, gui.scroll_region_right);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    void
gui_mch_insert_lines(
    පූර්ණ_සංඛ්‍යා		row,
    පූර්ණ_සංඛ්‍යා		num_lines)
{
    RECT	rc;

    rc.left = FILL_X(gui.scroll_region_left);
    rc.right = FILL_X(gui.scroll_region_right + 1);
    rc.top = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, num_lines * gui.char_height, &rc);
    }
    නැතහොත්
#endif
    {
#නම් defined(FEAT_DIRECTX)
	නම් (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, num_lines * gui.char_height,
				    &rc, &rc, NULL, NULL, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_left,
				row + num_lines - 1, gui.scroll_region_right);
}


    void
gui_mch_exit(පූර්ණ_සංඛ්‍යා rc UNUSED)
{
#නම් defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = NULL;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (void)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    නම් (s_hwnd != NULL)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    static char_u *
logfont2name(LOGFONTW lf)
{
    var_na	*charset_name;
    var_na	*quality_name;
    var_na	*font_name;
    size_t	res_size;
    var_na	*res;

    font_name = (var_na *)utf16_to_enc(lf.lfFaceName, NULL);
    නම් (font_name == NULL)
	ආපසු NULL;
    charset_name = charset_id2name((පූර්ණ_සංඛ්‍යා)lf.lfCharSet);
    quality_name = quality_id2name((පූර්ණ_සංඛ්‍යා)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == NULL ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == NULL ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    නම් (res != NULL)
    {
	var_na	*p;
	පූර්ණ_සංඛ්‍යා	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	සඳහා (p = font_name; *p != NUL; ++p)
	{
	    නම් (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	නම් (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(var_na *)res, res_size, "%s:h%d", font_name, points);
	නැතහොත්
	    res_len = vim_snprintf_safelen(
		(var_na *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (var_na *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	නම් (charset_name != NULL)
	    res_len += vim_snprintf_safelen((var_na *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	නම් (quality_name != NULL)
	    vim_snprintf((var_na *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ආපසු (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    static void
update_im_font(void)
{
    LOGFONTW	lf_wide, lf;

    නම් (p_guifontwide != NULL && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, parimaa_na(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    නැතහොත්
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    නම් (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    void
gui_mch_wide_font_changed(void)
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

    නම් (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, parimaa_na(lf), &lf))
    {
	නම් (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	නම් (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    නම් (!lf.lfItalic)
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
    පූර්ණ_සංඛ්‍යා
gui_mch_init_font(char_u *font_name, පූර්ණ_සංඛ්‍යා fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    නම් (get_logfont(&lf, font_name, NULL, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    නම් (font == NOFONT)
	ආපසු FAIL;

#නම් defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	පූර්ණ_සංඛ්‍යා		    feat_count = 0;
	char_u		    *fp;

	නම් (font_name != NULL)
	{
	    // Find each ":f" option in font_name.
	    සඳහා (fp = font_name; *fp != NUL; fp++)
	    {
		නම් (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    පූර්ණ_සංඛ්‍යා    ti = 0;
		    unsigned පූර්ණ_සංඛ්‍යා param = 1;

		    fp += 2;  // skip ":f"
		    අතරතුර (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    නම් (ti != 4)
			continue;  // invalid tag length

		    නම් (*fp == '=')
		    {
			fp++;
			param = (unsigned පූර්ණ_සංඛ්‍යා)atoi((var_na *)fp);
			අතරතුර (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    නම් (feat_count < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_count].tag =
			    ((unsigned පූර්ණ_සංඛ්‍යා)tag[0])
			    | ((unsigned පූර්ණ_සංඛ්‍යා)tag[1] << 8)
			    | ((unsigned පූර්ණ_සංඛ්‍යා)tag[2] << 16)
			    | ((unsigned පූර්ණ_සංඛ්‍යා)tag[3] << 24);
			features[feat_count].parameter = param;
			feat_count++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_count);
    }
#endif

    නම් (font_name == NULL)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    නම් (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    නම් (p != NULL)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	නම් (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	නැතහොත්
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    නම් (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    නම් (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	නම් (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ආපසු OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_maximized(void)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = parimaa_na(WINDOWPLACEMENT);
    නම් (GetWindowPlacement(s_hwnd, &wp))
    {
	නම් (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ආපසු TRUE;
	නම් (wp.showCmd == SW_SHOWMINIMIZED)
	    ආපසු FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	නම් ((rc.right - rc.left !=
		    wp.rcNormalPosition.right - wp.rcNormalPosition.left)
		|| (rc.bottom - rc.top !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.top))
	    ආපසු TRUE;
    }
    ආපසු FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    void
gui_mch_newfont(void)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    නම් (win_socket_id == 0)
    {
	gui_resize_shell(rect.right - rect.left
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.top
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    නැතහොත්
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.right - rect.left,
	    rect.bottom - rect.top - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    void
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == NULL ? "VIM" : (var_na *)title));
}

#නම් defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
static LPCSTR mshape_idcs[] =
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

    void
mch_set_mouse_shape(පූර්ණ_සංඛ්‍යා shape)
{
    LPCSTR idc;

    නම් (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    නැතහොත්
    {
	නම් (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	නැතහොත්
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(NULL, idc));
	නම් (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (void)GetCursorPos(&mp);
	    (void)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#නම් defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    static WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    පූර්ණ_සංඛ්‍යා len;
    WCHAR *res;

    tmp = convert_filter(s);
    නම් (tmp == NULL)
	ආපසු NULL;
    len = (පූර්ණ_සංඛ්‍යා)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    ආපසු res;
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
	පූර්ණ_සංඛ්‍යා saving,
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
    පූර්ණ_සංඛ්‍යා			i;
    WCHAR		*titlep = NULL;
    WCHAR		*extp = NULL;
    WCHAR		*initdirp = NULL;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    නම් (dflt == NULL)
	fileBuf[0] = NUL;
    නැතහොත්
    {
	wp = enc_to_utf16(dflt, NULL);
	නම් (wp == NULL)
	    fileBuf[0] = NUL;
	නැතහොත්
	{
	    සඳහා (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# නැතහොත්
    fileStruct.lStructSize = parimaa_na(fileStruct);
# endif

    නම් (title != NULL)
	titlep = enc_to_utf16(title, NULL);
    fileStruct.lpstrTitle = titlep;

    නම් (ext != NULL)
	extp = enc_to_utf16(ext, NULL);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    නම් (initdir != NULL && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, NULL);
	නම් (initdirp != NULL)
	{
	    සඳහා (wp = initdirp; *wp != NUL; ++wp)
		නම් (*wp == '/')
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
    නම් (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    නම් (saving)
	ret = GetSaveFileNameW(&fileStruct);
    නැතහොත්
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    නම් (!ret)
	ආපසු NULL;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, NULL);
    නම් (p == NULL)
	ආපසු NULL;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ආපසු q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    static char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    නම් (res != NULL)
    {
	සඳහා (i = 0; i < s_len; ++i)
	    නම් (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    නැතහොත්
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ආපසු res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ආපසු gui_mch_browse(0, title, (char_u *)_("Not Used"), NULL,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    static void
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    var_na    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, NULL, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    නම් (fnames != NULL)
	සඳහා (i = 0; i < cFiles; ++i)
	{
	    නම් (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, NULL);
	    නැතහොත්
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    නම් (fnames == NULL)
	ආපසු;

    පූර්ණ_සංඛ්‍යා kbd_modifiers = get_active_modifiers();

    නම් ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    නම් ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    නම් ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    static පූර්ණ_සංඛ්‍යා
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    පූර්ණ_සංඛ්‍යා pos)
{
    static UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    පූර්ණ_සංඛ්‍යා		dragging = FALSE;
    පූර්ණ_සංඛ්‍යා		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = parimaa_na(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    නම් (sb == NULL)
	ආපසු 0;

    නම් (sb->wp != NULL)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    නැතහොත්	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->value;

    switch (code)
    {
	pak_sa SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    නම් (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    bha_nga;
	pak_sa SB_LINEDOWN:
	    val++;
	    bha_nga;
	pak_sa SB_LINEUP:
	    val--;
	    bha_nga;
	pak_sa SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    bha_nga;
	pak_sa SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    bha_nga;
	pak_sa SB_TOP:
	    val = 0;
	    bha_nga;
	pak_sa SB_BOTTOM:
	    val = sb_info->max;
	    bha_nga;
	pak_sa SB_ENDSCROLL:
	    නම් (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		නම් (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    bha_nga;

	default:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ආපසු 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    නම් (sb->wp != NULL)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_LEFT) ? SBAR_RIGHT : SBAR_LEFT].id;

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

    ආපසු 0;
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

static BalloonEval  *cur_beval = NULL;
static UINT_PTR	    beval_timer_id = 0;
static DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
static UINT	s_menu_id = 100;
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
static PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	const var_na *caption);
static LPWORD lpwAlign(LPWORD);
static පූර්ණ_සංඛ්‍යා nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#නම් defined(FEAT_MENU) && defined(FEAT_TEAROFF)
static void gui_mch_tearoff(char_u *title, vimmenu_T *menu, පූර්ණ_සංඛ්‍යා initX, පූර්ණ_සංඛ්‍යා initY);
#endif
static void get_dialog_font_metrics(void);

static පූර්ණ_සංඛ්‍යා dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
static void initialise_toolbar(void);
static void update_toolbar_size(void);
static LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static පූර්ණ_සංඛ්‍යා get_toolbar_bitmap(vimmenu_T *menu);
#නැතහොත්
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
static void initialise_tabline(void);
static LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
static LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
static char_u *GetResultStr(HWND hwnd, පූර්ණ_සංඛ්‍යා GCS, පූර්ණ_සංඛ්‍යා *lenp);
#endif
#නම් defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
typedef sa_nracanaa tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
typedef HANDLE HIMC;
# endif

static HINSTANCE hLibImm = NULL;
static LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
static HIMC (WINAPI *pImmGetContext)(HWND);
static HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
static BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
static BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
static BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
static BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
static BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
static BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
static BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
static BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
static void dyn_imm_load(void);
#නැතහොත්
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
    static පූර්ණ_සංඛ්‍යා
gui_mswin_get_menu_height(
    පූර්ණ_සංඛ්‍යා	    fix_window)	    // If TRUE, resize window if menu height changed
{
    static පූර්ණ_සංඛ්‍යා	old_menu_height = -1;

    RECT    rc1, rc2;
    පූර්ණ_සංඛ්‍යා	    num;
    පූර්ණ_සංඛ්‍යා	    menu_height;

    නම් (gui.menu_is_active)
	num = GetMenuItemCount(s_menuBar);
    නැතහොත්
	num = 0;

    නම් (num == 0)
	menu_height = 0;
    නැතහොත් නම් (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    නැතහොත්
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	නම් (gui.starting)
	    menu_height = rc1.bottom - rc1.top + 1;
	නැතහොත්
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, num - 1, &rc2);
	    menu_height = rc2.bottom - rc1.top + 1;
	}
    }

    නම් (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    ආපසු menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    static long
mouse_vertical_scroll_step(void)
{
    UINT val;
    නම් (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ආපසු (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    ආපසු 3; // Safe default;
}

    static long
mouse_horizontal_scroll_step(void)
{
    UINT val;
    නම් (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ආපසු (long)val;
    ආපසු 3; // Safe default;
}

    static void
init_mouse_wheel(void)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    static void
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, පූර්ණ_සංඛ්‍යා horizontal)
{
    පූර්ණ_සංඛ්‍යා		button;
    win_T	*wp;
    පූර්ණ_සංඛ්‍යා		modifiers = 0;
    පූර්ණ_සංඛ්‍යා		kbd_modifiers;
    පූර්ණ_සංඛ්‍යා		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    නම් (wp != NULL && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	නම් (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_LEFT : MSCR_RIGHT;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	නැතහොත්
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

    නම් (wp == NULL || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    නම් (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    නැතහොත්
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    නම් ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    නම් ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    නම් ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    static පූර්ණ_සංඛ්‍යා
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = parimaa_na(NONCLIENTMETRICSW);
    නම් (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    parimaa_na(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ආපසු FAIL;
    *lf = nm.lfMenuFont;
    ආපසු OK;
}
#endif


#නම් defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    static void
set_tabline_font(void)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    නම් (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ආපසු;

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
#නැතහොත්
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    static LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    switch (param)
    {
	pak_sa SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    bha_nga;
	pak_sa SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    bha_nga;
	pak_sa SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    bha_nga;
	default:
	    bha_nga;
    }
    ආපසු 0;
}

#ifdef FEAT_NETBEANS_INTG
    static void
_OnWindowPosChanged(
    HWND hwnd,
    const LPWINDOWPOS lpwpos)
{
    static පූර්ණ_සංඛ්‍යා x = 0, y = 0, cx = 0, cy = 0;
    extern පූර්ණ_සංඛ්‍යා WSInitialized;

    නම් (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


static HWND hwndTip = NULL;

    static void
show_sizing_tip(පූර්ණ_සංඛ්‍යා cols, පූර්ණ_සංඛ්‍යා rows)
{
    TOOLINFOA ti;
    var_na buf[32];

    ti.cbSize = parimaa_na(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    නම් (hwndTip == NULL)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, NULL,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, NULL, GetModuleHandle(NULL), NULL);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    නැතහොත්
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    static void
destroy_sizing_tip(void)
{
    නම් (hwndTip == NULL)
	ආපසු;

    DestroyWindow(hwndTip);
    hwndTip = NULL;
}

    static පූර්ණ_සංඛ්‍යා
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    පූර්ණ_සංඛ්‍යා	    w, h;
    පූර්ණ_සංඛ්‍යා	    valid_w, valid_h;
    පූර්ණ_සංඛ්‍යා	    w_offset, h_offset;
    පූර්ණ_සංඛ්‍යා	    cols, rows;

    w = lprc->right - lprc->left;
    h = lprc->bottom - lprc->top;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    නම් (fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->left += w_offset;
    නැතහොත් නම් (fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->right -= w_offset;

    නම් (fwSide == WMSZ_TOP || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->top += h_offset;
    නැතහොත් නම් (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    ආපසු TRUE;
}

#ifdef FEAT_GUI_TABLINE
    static void
_OnRButtonUp(HWND hwnd, පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y, UINT keyFlags)
{
    නම් (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	නම් (pt.y < rect.top)
	{
	    show_tabline_popup_menu();
	    ආපසු;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, keyFlags, DefWindowProcW);
}

    static void
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, පූර්ණ_සංඛ්‍යා x, පූර්ණ_සංඛ්‍යා y, UINT keyFlags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    නම් (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	නම් (pt.y < rect.top)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, keyFlags, DefWindowProcW);
}
#endif

    static UINT
_OnNCHitTest(HWND hwnd, පූර්ණ_සංඛ්‍යා xPos, පූර්ණ_සංඛ්‍යා yPos)
{
    UINT	result;
    පූර්ණ_සංඛ්‍යා		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    නම් (result != HTCLIENT)
	ආපසු result;

#ifdef FEAT_GUI_TABLINE
    නම් (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	නම් (yPos < rct.top)
	    ආපසු result;
    }
#endif
    (void)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    නම් (xPos < 48) // <VN> TODO should use system metric?
	ආපසු HTBOTTOMLEFT;
    නැතහොත්
	ආපසු HTBOTTOMRIGHT;
}

#නම් defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    static LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    switch (hdr->code)
    {
	pak_sa TTN_GETDISPINFOW:
	pak_sa TTN_GETDISPINFO:
	    {
		char_u		*str = NULL;
		static void	*tt_text = NULL;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		නම් (gui_mch_showing_tabline()
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
		    නම් (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			පූර්ණ_සංඛ්‍යා idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			නම් (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    නම් (tp != NULL)
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
		නැතහොත්
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(root_menu, idButton);
		    නම් (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		නම් (str == NULL)
		    bha_nga;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		නම් (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, NULL);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		නැතහොත්
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    නම් (len < parimaa_na(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == NULL))
			vim_strncpy((char_u *)lpdi->szText, str,
				parimaa_na(lpdi->szText) - 1);
		    නැතහොත්
			lpdi->lpszText = tt_text;
		}
	    }
	    bha_nga;

# ifdef FEAT_GUI_TABLINE
	pak_sa TCN_SELCHANGE:
	    නම් (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ආපසු 0L;
	    }
	    bha_nga;

	pak_sa NM_RCLICK:
	    නම් (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ආපසු 0L;
	    }
	    bha_nga;
# endif

	default:
	    bha_nga;
    }
    ආපසු DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#නම් defined(MENUHINTS) && defined(FEAT_MENU)
    static LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    නම් (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	static පූර්ණ_සංඛ්‍යා  did_menu_tip = FALSE;

	නම් (did_menu_tip)
	{
	    msg_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(root_menu, (UINT)LOWORD(wParam));
	නම් (pMenu != NULL && pMenu->strings[MENU_INDEX_TIP] != NULL)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = parimaa_na(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    නම් (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++msg_hist_off;
		msg((var_na *)pMenu->strings[MENU_INDEX_TIP]);
		--msg_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	ආපසු 0L;
    }
    ආපසු DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    static BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    පූර්ණ_සංඛ්‍යා		old_width, old_height;
    පූර්ණ_සංඛ්‍යා		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, parimaa_na(lf), &lf);
    lf.lfHeight = lf.lfHeight * (පූර්ණ_සංඛ්‍යා)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    නම් (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    නැතහොත්
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ආපසු TRUE;
}

    static LRESULT
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
    InvalidateRect(hwnd, NULL, TRUE);

    s_in_dpichanged = FALSE;
    ආපසු 0L;
}


    static LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(uMsg, lParam);

    s_uMsg = uMsg;
    s_wParam = wParam;

    switch (uMsg)
    {
	HANDLE_MSG(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_MSG(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_MSG(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_MSG(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_MSG(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_MSG(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_MSG(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_MSG(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_MSG(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_MSG(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_MSG(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_MSG(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_MSG(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_MSG(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_MSG(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    pak_sa WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ආපසු FALSE;		// Do NOT allow system to go down.

    pak_sa WM_ENDSESSION:
	නම් (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    ආපසු 0L;
	}
	bha_nga;

    pak_sa WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (පූර්ණ_සංඛ්‍යා)(short)LOWORD(lParam));
	ආපසු 0L;

    pak_sa WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	නම් (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((පූර්ණ_සංඛ්‍යා)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (පූර්ණ_සංඛ්‍යා)(short)LOWORD(lParam));
	    ආපසු 0L;
	}
#ifdef FEAT_MENU
	නැතහොත්
	    ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
#endif

    pak_sa WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
#නැතහොත්
	ආපසු 0L;
#endif

    pak_sa WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	bha_nga;

    pak_sa WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ආපසු _DuringSizing((UINT)wParam, (LPRECT)lParam);

    pak_sa WM_MOUSEWHEEL:
    pak_sa WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, uMsg == WM_MOUSEHWHEEL);
	ආපසු 0L;

	// Notification for change in SystemParametersInfo()
    pak_sa WM_SETTINGCHANGE:
	ආපසු _OnSettingChange((UINT)wParam);

#නම් defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    pak_sa WM_NOTIFY:
	ආපසු _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#නම් defined(MENUHINTS) && defined(FEAT_MENU)
    pak_sa WM_MENUSELECT:
	ආපසු _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    pak_sa WM_IME_NOTIFY:
	නම් (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
	ආපසු 1L;

    pak_sa WM_IME_COMPOSITION:
	නම් (!_OnImeComposition(hwnd, wParam, lParam))
	    ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
	ආපසු 1L;
#endif
    pak_sa WM_GETDPISCALEDSIZE:
	ආපසු _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    pak_sa WM_DPICHANGED:
	ආපසු _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    default:
#ifdef MSWIN_FIND_REPLACE
	නම් (uMsg == s_findrep_msg && s_findrep_msg != 0)
	    _OnFindRepl();
#endif
	bha_nga;
    }

    ආපසු DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = NULL;

    static BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    var_na	buf[2048];
    var_na	*title = (var_na *)lParam;

    නම් (GetWindowText(hwnd, buf, parimaa_na(buf)))
    {
	නම් (strstr(buf, title) != NULL)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, NULL, "MDIClient", NULL);
	    නම් (vim_parent_hwnd != NULL)
		ආපසු FALSE;
	}
    }
    ආපසු TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    void
gui_mch_set_parent(var_na *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    නම් (vim_parent_hwnd == NULL)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    static void
ole_error(var_na *arg)
{
    var_na buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#නම් defined(GUI_MAY_SPAWN)
    static var_na *
gvim_error(void)
{
    var_na *msg = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    නම් (starting)
    {
	mch_errmsg(msg);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ආපසු msg;
}

    var_na *
gui_mch_do_spawn(char_u *arg)
{
    පූර්ණ_සංඛ්‍යා			len;
# නම් defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = NULL;
    LPWSTR		tofree1 = NULL;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = NULL, p, warg, tofree2 = NULL;
    STARTUPINFOW	si = {parimaa_na(si)};
    PROCESS_INFORMATION pi;

    නම් (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	goto error;
    p = wcsrchr(name, L'\\');
    නම් (p == NULL)
	goto error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# නැතහොත්
    wcscpy(p + 1, L"gvim.exe");
# endif

# නම් defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    නම් (starting)
# endif
    {
	// Pass the command line to the new process.
	p = GetCommandLineW();
	// Skip 1st argument.
	අතරතුර (*p && *p != L' ' && *p != L'\t')
	{
	    නම් (*p == L'"')
	    {
		// Skip quoted strings
		අතරතුර (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# නම් defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    නැතහොත්
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	පූර්ණ_සංඛ්‍යා	ret;

	session = vim_tempname('s', FALSE);
	නම් (session == NULL)
	    goto error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	නම් (p_bg == NULL)
	{
	    p_bg = savebg;
	    goto error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	නම් (!ret)
	    goto error;
	wsession = enc_to_utf16(session, NULL);
	නම් (wsession == NULL)
	    goto error;
	len = (පූර්ණ_සංඛ්‍යා)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	නම් (cmd == NULL)
	{
	    vim_free(wsession);
	    goto error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    නම් (arg != NULL)
    {
	warg = enc_to_utf16(arg, NULL);
	නම් (warg == NULL)
	    goto error;
	tofree2 = warg;
    }
    නැතහොත්
	warg = L"";

    // Set up the new command line.
    len = (පූර්ණ_සංඛ්‍යා)wcslen(name) + (පූර්ණ_සංඛ්‍යා)wcslen(cmd) + (පූර්ණ_සංඛ්‍යා)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    නම් (newcmd == NULL)
	goto error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    නම් (!CreateProcessW(NULL, newcmd, NULL, NULL, TRUE, 0,
		NULL, NULL, &si, &pi))
	goto error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# නම් defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    නම් (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ආපසු gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    void
gui_mch_prepare(පූර්ණ_සංඛ්‍යා *argc, var_na **argv)
{
    පූර්ණ_සංඛ්‍යා		silent = FALSE;
    පූර්ණ_සංඛ්‍යා		idx;

    // Check for special OLE command line parameters
    නම් ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	නම් (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	නැතහොත්
	    idx = 1;

	// Register Vim as an OLE Automation server
	නම් (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#නැතහොත්
	    නම් (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	නම් (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#නැතහොත්
	    නම් (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	නම් (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#නැතහොත්
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
# ifdef VIMDLL
    නම් (mch_is_gui_executable())
# endif
    {
	පූර්ණ_සංඛ්‍යා	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	නම් (bDoRestart)
	    mch_exit(0);
    }
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	පූර්ණ_සංඛ්‍යා arg;

	සඳහා (arg = 1; arg < *argc; arg++)
	    නම් (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * parimaa_na(var_na *));
		argv[*argc] = NULL;
		bha_nga;	// enough?
	    }
    }
#endif
}

    static void
load_dpi_func(void)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    නම් (hUser32 == NULL)
	goto fail;

    pGetDpiForSystem = (UINT (WINAPI *)(void))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (පූර්ණ_සංඛ්‍යා (WINAPI *)(පූර්ණ_සංඛ්‍යා, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    නම් (pSetThreadDpiAwarenessContext != NULL)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	නම් (oldctx != NULL)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    නම් (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    ආපසු;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = NULL;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = NULL;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = NULL;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    පූර්ණ_සංඛ්‍යා
gui_mch_init(void)
{
    const WCHAR szVimWndClassW[] = VIM_CLASSW;
    const WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    නම් (s_hwnd != NULL)
	goto theend;

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
    නම් (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = NULL;
	wndclassw.lpszClassName = szVimWndClassW;

	නම් (RegisterClassW(&wndclassw) == 0)
	    ආපසු FAIL;
    }

    නම් (vim_parent_hwnd != NULL)
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
		vim_parent_hwnd, NULL,
		g_hinst, NULL);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	නම් (s_hwnd == NULL)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    නැතහොත්
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	නම් (IsWindow((HWND)win_socket_id) <= 0)
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
		NULL, NULL,
		g_hinst, NULL);
	නම් (s_hwnd != NULL && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    නම් (s_hwnd == NULL)
	ආපසු FAIL;

    නම් (pGetDpiForWindow != NULL)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#නම් defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    නම් (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = NULL;
	wndclassw.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclassw.hbrBackground = NULL;
	wndclassw.lpszMenuName = NULL;
	wndclassw.lpszClassName = szTextAreaClassW;

	නම් (RegisterClassW(&wndclassw) == 0)
	    ආපසු FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, NULL,
	g_hinst, NULL);

    නම් (s_textArea == NULL)
	ආපසු FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = NULL;

	නම් (mch_icon_load(&hIcon) == OK && hIcon != NULL)
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
    s_findrep_msg = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = parimaa_na(s_findrep_struct);
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
    නම් (p_rop)
	(void)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    ආපසු OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    static void
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    නම් (mon != NULL)
    {
	moninfo.cbSize = parimaa_na(MONITORINFO);
	නම් (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ආපසු;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    void
gui_mch_set_shellsize(
	පූර්ණ_සංඛ්‍යා width,
	පූර්ණ_සංඛ්‍යා height,
	පූර්ණ_සංඛ්‍යා min_width UNUSED,
	පූර්ණ_සංඛ්‍යා min_height UNUSED,
	පූර්ණ_සංඛ්‍යා base_width UNUSED,
	පූර්ණ_සංඛ්‍යා base_height UNUSED,
	පූර්ණ_සංඛ්‍යා direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    පූර්ණ_සංඛ්‍යා		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    නම් (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    නම් (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    නැතහොත්
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
    window_rect.right = window_rect.left + win_width;
    window_rect.bottom = window_rect.top + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    නම් (!s_in_dpichanged)
    {
	නම් ((direction & RESIZE_HOR)
		&& window_rect.right > workarea_rect.right)
	    OffsetRect(&window_rect,
		    workarea_rect.right - window_rect.right, 0);

	නම් ((direction & RESIZE_HOR)
		&& window_rect.left < workarea_rect.left)
	    OffsetRect(&window_rect,
		    workarea_rect.left - window_rect.left, 0);

	නම් ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	නම් ((direction & RESIZE_VERT)
		&& window_rect.top < workarea_rect.top)
	    OffsetRect(&window_rect,
		    0, workarea_rect.top - window_rect.top);
    }

    MoveWindow(s_hwnd, window_rect.left, window_rect.top,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    void
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    අතරතුර (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    නම් (sb->scroll_shift > 0)
	++size;

    info.cbSize = parimaa_na(info);
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
    void
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    void
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    void
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    void
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
    static LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    නම් (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ආපසු lResult;
    switch (dwCommand)
    {
	pak_sa IMN_SETOPENSTATUS:
	    නම් (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		නම් (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		නම් (State & MODE_INSERT)
		{
# නම් defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    නම් (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			පූර්ණ_සංඛ්‍යා old_row = gui.row;
			පූර්ණ_සංඛ්‍යා old_col = gui.col;

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
	    bha_nga;
    }
    pImmReleaseContext(hWnd, hImc);
    ආපසු lResult;
}

    static LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    පූර්ණ_සංඛ්‍යා		len;

    නම් ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ආපසු 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    නම් (ret != NULL)
    {
	add_to_input_buf_csi(ret, len);
	vim_free(ret);
	ආපසු 1;
    }
    ආපසු 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    static char_u *
GetResultStr(HWND hwnd, පූර්ණ_සංඛ්‍යා GCS, පූර්ණ_සංඛ්‍යා *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = NULL;
    char_u	*convbuf = NULL;

    නම් (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ආපසු NULL;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, NULL, 0);
    නම් (ret <= 0)
	ආපසු NULL;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + parimaa_na(WCHAR));
    නම් (buf == NULL)
	ආපසු NULL;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / parimaa_na(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ආපසු convbuf;
}
#endif

// For global functions we need prototypes.
#නම් defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    void
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    නම් (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    void
im_set_position(පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col)
{
    HIMC hImc;

    නම් (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	නම් (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    void
im_set_active(පූර්ණ_සංඛ්‍යා active)
{
    HIMC	hImc;
    static HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    නම් (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	ආපසු;
    }
# endif

    නම් (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ආපසු;

    නම් (p_imdisable)
    {
	නම් (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    නම් (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    නැතහොත් නම් (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    නම් (!hImc)
	ආපසු;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    නම් (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	static DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	static BOOL bSaved = FALSE;

	නම් (active)
	{
	    // if we have a saved conversion status, restore it
	    නම් (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	නැතහොත්
	{
	    // save conversion status and disable korean
	    නම් (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    පූර්ණ_සංඛ්‍යා
im_get_status(void)
{
    පූර්ණ_සංඛ්‍යා		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    නම් (!gui.in_use && !gui.starting)
	ආපසු mbyte_im_get_status();
# endif

    නම් (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ආපසු status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    static void
latin9_to_ucs(char_u *text, පූර්ණ_සංඛ්‍යා len, WCHAR *unicodebuf)
{
    පූර්ණ_සංඛ්‍යා		c;

    අතරතුර (--len >= 0)
    {
	c = *text++;
	switch (c)
	{
	    pak_sa 0xa4: c = 0x20ac; bha_nga;   // euro
	    pak_sa 0xa6: c = 0x0160; bha_nga;   // S hat
	    pak_sa 0xa8: c = 0x0161; bha_nga;   // S -hat
	    pak_sa 0xb4: c = 0x017d; bha_nga;   // Z hat
	    pak_sa 0xb8: c = 0x017e; bha_nga;   // Z -hat
	    pak_sa 0xbc: c = 0x0152; bha_nga;   // OE
	    pak_sa 0xbd: c = 0x0153; bha_nga;   // oe
	    pak_sa 0xbe: c = 0x0178; bha_nga;   // Y
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
    static void
RevOut( HDC hdc,
	පූර්ණ_සංඛ්‍යා col,
	පූර්ණ_සංඛ්‍යා row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    පූර්ණ_සංඛ්‍යා		ix;

    සඳහා (ix = 0; ix < (පූර්ණ_සංඛ්‍යා)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    static void
draw_line(
    පූර්ණ_සංඛ්‍යා		x1,
    පූර්ණ_සංඛ්‍යා		y1,
    පූර්ණ_සංඛ්‍යා		x2,
    පූර්ණ_සංඛ්‍යා		y2,
    COLORREF	color)
{
#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_DrawLine(s_dwc, x1, y1, x2, y2, color);
    නැතහොත්
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, NULL);
	// Note: LineTo() excludes the last pixel in the line.
	LineTo(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    static void
set_pixel(
    පූර්ණ_සංඛ්‍යා		x,
    පූර්ණ_සංඛ්‍යා		y,
    COLORREF	color)
{
#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    නැතහොත්
#endif
	SetPixel(s_hdc, x, y, color);
}

    static void
fill_rect(
    const RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    නැතහොත්
#endif
    {
	HBRUSH	hbr2;

	නම් (hbr == NULL)
	    hbr2 = CreateSolidBrush(color);
	නැතහොත්
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	නම් (hbr == NULL)
	    DeleteBrush(hbr2);
    }
}

    void
gui_mch_draw_string(
    පූර්ණ_සංඛ්‍යා		row,
    පූර්ණ_සංඛ්‍යා		col,
    char_u	*text,
    පූර්ණ_සංඛ්‍යා		len,
    පූර්ණ_සංඛ්‍යා		flags)
{
    static පූර්ණ_සංඛ්‍යා	*padding = NULL;
    static පූර්ණ_සංඛ්‍යා	pad_size = 0;
    const RECT	*pcliprect = NULL;
    UINT	foptions = 0;
    static WCHAR *unicodebuf = NULL;
    static පූර්ණ_සංඛ්‍යා   *unicodepdy = NULL;
    static පූර්ණ_සංඛ්‍යා	unibuflen = 0;
    පූර්ණ_සංඛ්‍යා		n = 0;
    පූර්ණ_සංඛ්‍යා		y;

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
    static HBRUSH	hbr_cache[2] = {NULL, NULL};
    static guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    static පූර්ණ_සංඛ්‍යා		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    නම් (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.left = FILL_X(col);
	rc.top = FILL_Y(row);
	නම් (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.right = FILL_X(col + mb_string2cells(text, len));
	}
	නැතහොත්
	    rc.right = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	නම් (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	නැතහොත් නම් (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	නැතහොත්
	{
	    නම් (hbr_cache[brush_lru] != NULL)
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
	නම් (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	නැතහොත් නම් (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    නම් (pad_size != Columns || padding == NULL || padding[0] != gui.char_width)
    {
	පූර්ණ_සංඛ්‍යා	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(පූර්ණ_සංඛ්‍යා, pad_size);
	නම් (padding != NULL)
	    සඳහා (i = 0; i < pad_size; i++)
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
    නම් (enc_utf8)
	සඳහා (n = 0; n < len; ++n)
	    නම් (text[n] >= 0x80)
		bha_nga;

#නම් defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    නම් (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    නම් ((enc_utf8
		|| (enc_codepage > 0 && (පූර්ණ_සංඛ්‍යා)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == NULL || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(පූර්ණ_සංඛ්‍යා, len);

	unibuflen = len;
    }

    නම් (enc_utf8 && n < len && unicodebuf != NULL)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	පූර්ණ_සංඛ්‍යා		i;
	පූර්ණ_සංඛ්‍යා		wlen;	// string length in words
	පූර්ණ_සංඛ්‍යා		cells;	// cell width of string up to composing char
	පූර්ණ_සංඛ්‍යා		cw;	// width of current cell
	පූර්ණ_සංඛ්‍යා		c;

	wlen = 0;
	cells = 0;
	සඳහා (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    නම් (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    නැතහොත්
	    {
		unicodebuf[wlen++] = c;
	    }

	    නම් (utf_iscomposing(c))
		cw = 0;
	    නැතහොත්
	    {
		cw = utf_char2cells(c);
		නම් (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    නම් (unicodepdy != NULL)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		නම් (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		නැතහොත්
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#නම් defined(FEAT_DIRECTX)
	නම් (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	නැතහොත්
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    නැතහොත් නම් ((enc_codepage > 0 && (පූර්ණ_සංඛ්‍යා)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	නම් (unicodebuf != NULL)
	{
	    නම් (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    නැතහොත්
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(var_na *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    නම් (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		නම් (unicodepdy != NULL)
		{
		    පූර්ණ_සංඛ්‍යා i;
		    පූර්ණ_සංඛ්‍යා cw;

		    සඳහා (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			නම් (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    නැතහොත්
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	නම් (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (var_na *)text, len, padding);
	නැතහොත්
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (var_na *)text, len, padding);
    }

    // Underline
    නම් (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	නම් (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_line(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    නම් (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_line(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    නම් (flags & DRAW_UNDERC)
    {
	පූර්ණ_සංඛ්‍යා			x;
	පූර්ණ_සංඛ්‍යා			offset;
	static const පූර්ණ_සංඛ්‍යා	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	සඳහා (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    void
gui_mch_flush(void)
{
#නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    static void
clear_rect(RECT *rcp)
{
    fill_rect(rcp, NULL, gui.back_pixel);
}


    void
gui_mch_get_screen_dimensions(පූර්ණ_සංඛ්‍යා *screen_w, පූර්ණ_සංඛ්‍යා *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.right - workarea_rect.left
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.top
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#නම් defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    void
gui_mch_add_menu(
    vimmenu_T	*menu,
    පූර්ණ_සංඛ්‍යා		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    නම් (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, NULL);
	නම් (wn == NULL)
	    ආපසු;

	infow.cbSize = parimaa_na(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	InsertMenuItemW((parent == NULL)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    නම් (parent == NULL)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    නැතහොත් නම් (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    void
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (void)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (පූර්ණ_සංඛ්‍යා)mp.x, (පූර්ණ_සංඛ්‍යා)mp.y);
}

    void
gui_make_popup(char_u *path_name, පූර්ණ_සංඛ්‍යා mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    නම් (menu == NULL)
	ආපසු;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    නම් (mouse_pos)
    {
	පූර්ණ_සංඛ්‍යා	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    නැතහොත් නම් (curwin != NULL)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    msg_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (පූර්ණ_සංඛ්‍යා)p.x, (පූර්ණ_සංඛ්‍යා)p.y);
}

# නම් defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    void
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    නම් (menu != NULL)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    void
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    පූර්ණ_සංඛ්‍යා		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = NULL;

# ifdef FEAT_TEAROFF
    නම් (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	InsertMenu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    නැතහොත්
# endif
# ifdef FEAT_TOOLBAR
    නම් (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	නම් (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	නැතහොත්
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
    නැතහොත්
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, NULL);
	නම් (wn != NULL)
	{
	    InsertMenuW(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	නම් (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    void
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    නම් (menu->submenu_id == (HMENU)-1)
    {
	පූර්ණ_සංඛ්‍යා iButton;

	iButton = (පූර්ණ_සංඛ්‍යා)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    නැතහොත්
# endif
    {
	නම් (menu->parent != NULL
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != NULL)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	නැතහොත්
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	නම් (menu->submenu_id != NULL)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	නම් (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	නම් (menu->parent != NULL
		&& menu->parent->children != NULL
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
    static void
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    පූර්ණ_සංඛ්‍යා		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    නම් (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.left - rct.left;
	y = (trect.top -  rct.bottom  + roct.bottom);
    }
    නැතහොත්
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    නම් (menu->children != NULL)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	නම් (IsWindow(menu->tearoff_handle))
	    (void) SetWindowPos(menu->tearoff_handle,
				NULL,
				(පූර්ණ_සංඛ්‍යා)trect.left,
				(පූර්ණ_සංඛ්‍යා)trect.top,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    void
gui_mch_menu_grey(
    vimmenu_T	*menu,
    පූර්ණ_සංඛ්‍යා	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    නම් (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    නැතහොත්
# endif
	(void)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    නම් ((menu->parent != NULL) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	නම් (menu->children == NULL)
	    menuID = (WORD)(menu->id);
	නැතහොත්
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	නම් (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

#නම් defined(FEAT_GUI_DIALOG)
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
    static LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    නම් (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(void)SetFocus(hwnd);
	නම් (dialog_default_button > IDCANCEL)
	    (void)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	නැතහොත්
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (void)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	ආපසු FALSE;
    }

    නම් (message == WM_COMMAND)
    {
	පූර්ණ_සංඛ්‍යා	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	නම් (button >= DLG_NONBUTTON_CONTROL)
	    ආපසු TRUE;

	// If the edit box exists, copy the string.
	නම් (s_textfield != NULL)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, NULL);
	    නම් (p != NULL)
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
	නම් (button == IDOK)
	{
	    නම් (dialog_default_button > IDCANCEL)
		EndDialog(hwnd, dialog_default_button);
	}
	නැතහොත්
	    EndDialog(hwnd, button - IDCANCEL);
	ආපසු TRUE;
    }

    නම් ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	ආපසු TRUE;
    }
    ආපසු FALSE;
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

static const var_na *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    පූර්ණ_සංඛ්‍යා
gui_mch_dialog(
    පූර්ණ_සංඛ්‍යා		 type,
    char_u	*title,
    char_u	*message,
    char_u	*buttons,
    පූර්ණ_සංඛ්‍යා		 dfltbutton,
    char_u	*textfield,
    පූර්ණ_සංඛ්‍යා		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    පූර්ණ_සංඛ්‍යා		numButtons;
    පූර්ණ_සංඛ්‍යා		*buttonWidths, *buttonPositions;
    පූර්ණ_සංඛ්‍යා		buttonYpos;
    පූර්ණ_සංඛ්‍යා		nchar, i;
    DWORD	lStyle;
    පූර්ණ_සංඛ්‍යා		dlgwidth = 0;
    පූර්ණ_සංඛ්‍යා		dlgheight;
    පූර්ණ_සංඛ්‍යා		editboxheight;
    පූර්ණ_සංඛ්‍යා		horizWidth = 0;
    පූර්ණ_සංඛ්‍යා		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    පූර්ණ_සංඛ්‍යා		fontHeight;
    පූර්ණ_සංඛ්‍යා		textWidth, minButtonWidth, messageWidth;
    පූර්ණ_සංඛ්‍යා		maxDialogWidth;
    පූර්ණ_සංඛ්‍යා		maxDialogHeight;
    පූර්ණ_සංඛ්‍යා		scroll_flag = 0;
    පූර්ණ_සංඛ්‍යා		vertical;
    පූර්ණ_සංඛ්‍යා		dlgPaddingX;
    පූර්ණ_සංඛ්‍යා		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    පූර්ණ_සංඛ්‍යා		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    පූර්ණ_සංඛ්‍යා		l;
    පූර්ණ_සංඛ්‍යා		dlg_icon_width;
    පූර්ණ_සංඛ්‍යා		dlg_icon_height;
    පූර්ණ_සංඛ්‍යා		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    නම් (!(gui.in_use || gui.starting))
#  endif
	නම් (silent_mode)
	    ආපසු dfltbutton;   // return default option
# endif

    නම් (s_hwnd == NULL)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    නැතහොත්
	dpi = pGetDpiForSystem();

    නම් ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    නම් (p == NULL)
	ආපසු -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    නම් (tbuffer == NULL)
	ආපසු -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    numButtons = 1;
    සඳහා (i = 0; tbuffer[i] != '\0'; i++)
    {
	නම් (tbuffer[i] == DLG_BUTTON_SEP)
	    numButtons++;
    }
    නම් (dfltbutton >= numButtons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(පූර්ණ_සංඛ්‍යා, numButtons);
    නම් (buttonWidths == NULL)
	ආපසු -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(පූර්ණ_සංඛ්‍යා, numButtons);
    නම් (buttonPositions == NULL)
	ආපසු -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    නම් (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    නැතහොත්
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
    නම් (s_hwnd == NULL)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.right - workarea_rect.left - 100;
	නම් (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.top - 150;
    }
    නැතහොත්
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.right - rect.left
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	නම් (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.top
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	නම් (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, parimaa_na(var_na), 500);
    do
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = NULL;
	සඳහා (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    නම් (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    නම් (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		නම් (last_white != NULL)
		{
		    // break the line just after a space
		    නම් (pend > last_white)
			ga.ga_len -= (පූර්ණ_සංඛ්‍යා)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = NULL;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		continue;
	    }

	    අතරතුර (--l >= 0)
		ga_append(&ga, *pend++);
	}
	නම් (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } අතරතුර (*pend != NUL);

    නම් (ga.ga_data != NULL)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    නම් (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != NULL && vim_strchr(p_go, GO_VERTICAL) != NULL);
    නම් (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	do
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    නම් (pend == NULL)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (පූර්ණ_සංඛ්‍යා)(pend - pstart));
	    නම් (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} අතරතුර (*pend != NUL);

	නම් (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	නැතහොත් නම් (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    නම් (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	do
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    නම් (pend == NULL)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (පූර්ණ_සංඛ්‍යා)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    නම් (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} අතරතුර (*pend != NUL);
    }

    නම් (dlgwidth < DLG_MIN_WIDTH)
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
    නම් (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * numButtons;
    නැතහොත්
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    නම් (textfield != NULL)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    නම් (dlgheight > maxDialogHeight)
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
    නම් (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (පූර්ණ_සංඛ්‍යා)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    නැතහොත්
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    නම් (textfield != NULL)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    නම් (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    සඳහා (i = 0; i < numButtons; i++)
    {
	// get end of this button.
	සඳහා (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	නම් (*pend)
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
	නම් (vertical)
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (var_na *)pstart);
	}
	නැතහොත්
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (var_na *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += numButtons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_LEFT|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (var_na *)message);

    // Edit box
    නම් (textfield != NULL)
    {
	p = add_dialog_element(p, ES_LEFT|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (var_na *)textfield);
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
    නම් (textfield != NULL)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    නැතහොත්
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = NULL;
    }

    // show the dialog box modally and get a return value
    nchar = (පූර්ණ_සංඛ්‍යා)DialogBoxIndirect(
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
    (void)SetFocus(s_hwnd);

    ආපසු nchar;
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
    static PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    const var_na *caption)
{
    පූර්ණ_සංඛ්‍යා nchar;

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

    ආපසු p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    static LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ආපසු (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    static පූර්ණ_සංඛ්‍යා
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    පූර්ණ_සංඛ්‍යා		nChar = 0;
    පූර්ණ_සංඛ්‍යා		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    පූර්ණ_සංඛ්‍යා		i;
    WCHAR	*wn;

    නම් (use_enc && enc_codepage >= 0 && (පූර්ණ_සංඛ්‍යා)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, NULL);
	නම් (wn != NULL)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (පූර්ණ_සංඛ්‍යා)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    නම් (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    සඳහා (i = 0; i < nChar; ++i)
	නම් (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    ආපසු nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    static HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    සඳහා ( ; menu != NULL; menu = menu->next)
    {
	නම් (menu->modes == 0)	// this menu has just been deleted
	    continue;
	නම් (menu_is_separator(menu->dname))
	    continue;
	නම් ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ආපසු menu->submenu_id;
    }
    ආපසු NULL;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    static LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    නම් (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ආපසු (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    නම් (message == WM_COMMAND)
    {
	නම් ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    නම් (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(void)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (පූර්ණ_සංඛ්‍යා)rect.right - 8,
			 (පූර්ණ_සංඛ්‍යා)mp.y,
			 (පූර්ණ_සංඛ්‍යා)0,	    // reserved param
			 s_hwnd,
			 NULL);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	නැතහොත්
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(void)SetFocus(s_hwnd);
	ආපසු TRUE;
    }
    නම් ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	ආපසු TRUE;
    }

    // When moved around, give main window the focus back.
    නම් (message == WM_EXITSIZEMOVE)
	(void)SetActiveWindow(s_hwnd);

    ආපසු FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    static void
get_dialog_font_metrics(void)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    නම් (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    නැතහොත්
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

#නම් defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    static void
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    පූර්ණ_සංඛ්‍යා		initX,
    පූර්ණ_සංඛ්‍යා		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    පූර්ණ_සංඛ්‍යා		template_len;
    පූර්ණ_සංඛ්‍යා		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*top_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    පූර්ණ_සංඛ්‍යා		col, spaceWidth, len;
    පූර්ණ_සංඛ්‍යා		columnWidths[2];
    char_u	*label, *text;
    පූර්ණ_සංඛ්‍යා		acLen = 0;
    පූර්ණ_සංඛ්‍යා		nameLen;
    පූර්ණ_සංඛ්‍යා		padding0, padding1, padding2 = 0;
    පූර්ණ_සංඛ්‍යා		sepPadding=0;
    පූර්ණ_සංඛ්‍යා		x;
    පූර්ණ_සංඛ්‍යා		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    පූර්ණ_සංඛ්‍යා		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    නම් (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	නම් (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, NULL, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ආපසු;
    }

    /*
     * Create a new tearoff.
     */
    නම් (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    නම් (p == NULL)
	ආපසු;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    නම් (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    නැතහොත්
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
    සඳහා (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    නම් (text != NULL && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (පූර්ණ_සංඛ්‍යා)STRLEN(text));
		නම් (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    නම් (pmenu->children != NULL)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    නම් (columnWidths[1] == 0)
    {
	// no accelerators
	නම් (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	නැතහොත්
	    columnWidths[0] += spaceWidth;
    }
    නැතහොත්
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    නම් (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (පූර්ණ_සංඛ්‍යා)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (පූර්ණ_සංඛ්‍යා)STRLEN(title));
    නම් (textWidth > dlgwidth)
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
    නම් (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    නැතහොත්
	*p++ = PixelToDialogX(initX); // x
    නම් (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    නැතහොත්
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
    නම් (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (පූර්ණ_සංඛ්‍යා)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    නැතහොත්
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
    නම් (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    නැතහොත්
	menu = menu->children;
    top_menu = menu;
    සඳහා ( ; menu != NULL; menu = menu->next)
    {
	නම් (menu->modes == 0)	// this menu has just been deleted
	    continue;
	නම් (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    continue;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	නම් (((var_na *)p - (var_na *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    නම් (newp != NULL)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (var_na *)p - (var_na *)pdlgtemplate);
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
	len = nameLen = (පූර්ණ_සංඛ්‍යා)STRLEN(menu->name);
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (පූර්ණ_සංඛ්‍යා)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	නම් (menu->actext != NULL)
	{
	    acLen = (පූර්ණ_සංඛ්‍යා)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	නැතහොත්
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	නම් (menu->children == NULL)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	නැතහොත්
	{
	    len += (පූර්ණ_සංඛ්‍යා)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	නම් (label == NULL)
	    bha_nga;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	නම් (text == NULL)
	    text = label + nameLen;	    // no actext, use whole name
	අතරතුර (padding0-- > 0)
	    *text++ = ' ';
	නම් (menu->actext != NULL)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	අතරතුර (padding1-- > 0)
	    *text++ = ' ';
	නම් (menu->children != NULL)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	නැතහොත්
	{
	    අතරතුර (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_LEFT,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (var_na *)label);
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
	    (LPARAM)top_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (void)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#නම් defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    static void
initialise_toolbar(void)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    NULL,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    parimaa_na(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != NULL);

    update_toolbar_size();
}

    static void
update_toolbar_size(void)
{
    පූර්ණ_සංඛ්‍යා		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = parimaa_na(TBMETRICS);
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

    static LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(uMsg, lParam);
    ආපසු CallWindowProc(s_toolbar_wndproc, hwnd, uMsg, wParam, lParam);
}

    static පූර්ණ_සංඛ්‍යා
get_toolbar_bitmap(vimmenu_T *menu)
{
    පූර්ණ_සංඛ්‍යා i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    නම් (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = NULL;

	නම් (menu->iconfile != NULL)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			NULL,
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
	නම් (hbitmap == NULL
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != NULL ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    NULL,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	නම් (hbitmap != NULL)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = NULL;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (පූර්ණ_සංඛ්‍යා)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    නම් (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_COUNT)
	i = menu->iconidx;

    ආපසු i;
}
#endif

#නම් defined(FEAT_GUI_TABLINE)
    static void
initialise_tabline(void)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, NULL, g_hinst, NULL);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    static tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = NULL;

    නම් (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	නම් (s_tabhwnd == hWnd)
	{
	    පූර්ණ_සංඛ්‍යා idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    නම් (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    ආපසු ptp;
}

static POINT	    s_pt = {0, 0};
static HCURSOR      s_hCursor = NULL;

    static LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    පූර්ණ_සංඛ්‍යා		nCenter;
    පූර්ණ_සංඛ්‍යා		idx0;
    පූර්ණ_සංඛ්‍යා		idx1;

    HandleMouseHide(uMsg, lParam);

    switch (uMsg)
    {
	pak_sa WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		bha_nga;
	    }
	pak_sa WM_MOUSEMOVE:
	    නම් (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		නම් (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(NULL, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    නම් (tp != NULL)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.left + (rect.right - rect.left) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			නම් ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			නැතහොත් නම් ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    bha_nga;
	pak_sa WM_LBUTTONUP:
	    {
		නම් (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		bha_nga;
	    }
	pak_sa WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		නම් (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		bha_nga;
	    }
	default:
	    bha_nga;
    }

    ආපසු CallWindowProc(s_tabline_wndproc, hwnd, uMsg, wParam, lParam);
}
#endif

#නම් defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    void
gui_mch_set_foreground(void)
{
    නම් (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#නම් defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    static void
dyn_imm_load(void)
{
    hLibImm = vimLoadLib("imm32.dll");
    නම් (hLibImm == NULL)
	ආපසු;

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

    නම් (       pImmGetCompositionStringW == NULL
	    || pImmGetContext == NULL
	    || pImmAssociateContext == NULL
	    || pImmReleaseContext == NULL
	    || pImmGetOpenStatus == NULL
	    || pImmSetOpenStatus == NULL
	    || pImmGetCompositionFontW == NULL
	    || pImmSetCompositionFontW == NULL
	    || pImmSetCompositionWindow == NULL
	    || pImmGetConversionStatus == NULL
	    || pImmSetConversionStatus == NULL)
    {
	FreeLibrary(hLibImm);
	hLibImm = NULL;
	pImmGetContext = NULL;
	ආපසු;
    }

    ආපසු;
}

#endif

#නම් defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

typedef sa_nracanaa _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    void
gui_mch_drawsign(පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, පූර්ණ_සංඛ්‍යා typenr)
{
    signicon_t *sign;
    පූර්ණ_සංඛ්‍යා x, y, w, h;

    නම් (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == NULL)
	ආපසු;

# නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    switch (sign->uType)
    {
	pak_sa IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    bha_nga;
	pak_sa IMAGE_ICON:
	pak_sa IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, NULL, DI_NORMAL);
	    bha_nga;
# ifdef FEAT_XPM_W32
	pak_sa IMAGE_XPM:
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
	    bha_nga;
# endif
    }
}

    static void
close_signicon_image(signicon_t *sign)
{
    නම් (sign == NULL)
	ආපසු;

    switch (sign->uType)
    {
	pak_sa IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    bha_nga;
	pak_sa IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    bha_nga;
	pak_sa IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    bha_nga;
# ifdef FEAT_XPM_W32
	pak_sa IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    bha_nga;
# endif
    }
}

    void *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = NULL;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    නම් (ext > signfile)
    {
	පූර්ණ_සංඛ්‍යා do_load = 1;

	නම් (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	නැතහොත් නම් (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	නැතහොත් නම් (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	නැතහොත්
	    do_load = 0;

	නම් (do_load)
	    sign.hImage = (HANDLE)LoadImage(NULL, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	නම් (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((var_na *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = NULL;
    නම් (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != NULL)
	*psign = sign;

    නම් (!psign)
    {
	නම් (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    ආපසු (void *)psign;

}

    void
gui_mch_destroy_sign(void *sign)
{
    නම් (sign == NULL)
	ආපසු;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

#නම් defined(FEAT_BEVAL_GUI)

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

    static void
make_tooltip(BalloonEval *beval, var_na *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    නම් (pti == NULL)
	ආපසු;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    NULL, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, NULL, g_hinst, NULL);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = parimaa_na(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, NULL);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    නම් (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.right);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.left = pt.x - 3;
    pti->rect.top = pt.y - 3;
    pti->rect.right = pt.x + 3;
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

    static void
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    static VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	uMsg UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    නම් (cur_beval == NULL || cur_beval->showState == ShS_SHOWING || !p_beval)
	ආපසු;

    GetCursorPos(&pt);
    නම් (WindowFromPoint(pt) != s_textArea)
	ආපසු;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    නම් (!PtInRect(&rect, pt))
	ආපසු;

    නම් (last_user_activity > 0
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

	නම් (cur_beval->msgCB != NULL)
	    (*cur_beval->msgCB)(cur_beval, 0);
    }
}

    void
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    void
gui_mch_enable_beval_area(BalloonEval *beval)
{
    නම් (beval == NULL)
	ආපසු;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    void
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->msg);
    beval->msg = mesg == NULL ? NULL : vim_strsave(mesg);
    නම් (beval->msg == NULL)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ආපසු;
    }

    නම් (beval->showState == ShS_SHOWING)
	ආපසු;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    නම් (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (var_na *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    void	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    void	(*mesgCB)(BalloonEval *, පූර්ණ_සංඛ්‍යා),
    void	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    නම් (mesg != NULL && mesgCB != NULL)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	ආපසු NULL;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    නම් (beval != NULL)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->msg = mesg;
	beval->msgCB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	නම් (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ආපසු beval;
}

    static void
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    නම් (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ආපසු;

    නම් (cur_beval == NULL)
	ආපසු;

    switch (pnmh->code)
    {
	pak_sa TTN_SHOW:
	    bha_nga;
	pak_sa TTN_POP: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    bha_nga;
	pak_sa TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    bha_nga;
	pak_sa TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    bha_nga;
    }
}

    static void
track_user_activity(UINT uMsg)
{
    නම් ((uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
	    || (uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST))
	last_user_activity = GetTickCount();
}

    void
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#නම් defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    void
netbeans_draw_multisign_indicator(පූර්ණ_සංඛ්‍යා row)
{
    පූර්ණ_සංඛ්‍යා i;
    පූර්ණ_සංඛ්‍යා y;
    පූර්ණ_සංඛ්‍යා x;

    නම් (!netbeans_active())
	ආපසු;

    x = 0;
    y = TEXT_Y(row);

# නම් defined(FEAT_DIRECTX)
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    සඳහා (i = 0; i < gui.char_height - 3; i++)
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

#නම් defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    static පූර්ණ_සංඛ්‍යා
test_gui_w32_sendevent_mouse(dict_T *args)
{
    නම් (!dict_has_key(args, "row") || !dict_has_key(args, "col"))
	ආපසු FALSE;

    // Note: "move" is optional, requires fewer arguments
    පූර්ණ_සංඛ්‍යා move = (පූර්ණ_සංඛ්‍යා)dict_get_bool(args, "move", FALSE);

    නම් (!move && (!dict_has_key(args, "button")
	    || !dict_has_key(args, "multiclick")
	    || !dict_has_key(args, "modifiers")))
	ආපසු FALSE;

    පූර්ණ_සංඛ්‍යා row = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "row");
    පූර්ණ_සංඛ්‍යා col = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "col");

    නම් (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	නම් (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    පූර්ණ_සංඛ්‍යා pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    පූර්ණ_සංඛ්‍යා pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	නැතහොත්
	    gui_mouse_moved(col, row);
    }
    නැතහොත්
    {
	පූර්ණ_සංඛ්‍යා button = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "button");
	පූර්ණ_සංඛ්‍යා repeated_click = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "multiclick");
	int_u mods = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    ආපසු TRUE;
}

    static පූර්ණ_සංඛ්‍යා
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    INPUT inputs[1];
    INPUT modkeys[3];
    SecureZeroMemory(inputs, parimaa_na(INPUT));
    SecureZeroMemory(modkeys, 3 * parimaa_na(INPUT));

    char_u *event = dict_get_string(args, "event", TRUE);

    නම් (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_number_def(args, "keycode", 0);
	නම් (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    ආපසු FALSE;
	}

	BOOL isModKey = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	පූර්ණ_සංඛ්‍යා mods = (පූර්ණ_සංඛ්‍යා)dict_get_number(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	නම් (mods && STRICMP(event, "keydown") == 0 && !isModKey)
	{
	    පූර්ණ_සංඛ්‍යා n = 0;
	    නම් (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = INPUT_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    නම් (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = INPUT_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    නම් (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = INPUT_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    නම් (n)
	    {
		(void)SetForegroundWindow(s_hwnd);
		SendInput(n, modkeys, parimaa_na(INPUT));
	    }
	}

	inputs[0].type = INPUT_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	නම් (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    නම් (!isModKey)
		unwrapMods = TRUE;
	}

	(void)SetForegroundWindow(s_hwnd);
	SendInput(ARRAYSIZE(inputs), inputs, parimaa_na(INPUT));
	vim_free(event);

	නම් (unwrapMods)
	{
	    modkeys[0].type = INPUT_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = INPUT_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = INPUT_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (void)SetForegroundWindow(s_hwnd);
	    SendInput(3, modkeys, parimaa_na(INPUT));
	}
    }
    නැතහොත්
    {
	නම් (event == NULL)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	නැතහොත්
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	ආපසු FALSE;
    }
    ආපසු TRUE;
}

    static පූර්ණ_සංඛ්‍යා
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    පූර්ණ_සංඛ්‍යා handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    නම් (strategy)
    {
	නම් (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	නැතහොත් නම් (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    නම් (!handled)
    {
	නම් (strategy == NULL)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	නැතහොත්
	{
	    semsg(_(e_invalid_value_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ආපසු FALSE;
    }
    ආපසු TRUE;
}


    පූර්ණ_සංඛ්‍යා
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    නම් (STRICMP(event, "key") == 0)
	ආපසු test_gui_w32_sendevent_keyboard(args);
    නැතහොත් නම් (STRICMP(event, "mouse") == 0)
	ආපසු test_gui_w32_sendevent_mouse(args);
    නැතහොත් නම් (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ආපසු test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    නැතහොත්
    {
	semsg(_(e_invalid_value_for_argument_str_str), "event", event);
	ආපසු FALSE;
    }
}
#endif

#නම් defined(FEAT_IMAGE_GUI)

typedef sa_nracanaa
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
    static void
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, පූර්ණ_සංඛ්‍යා iw, පූර්ණ_සංඛ්‍යා ih, image_format_T fmt)
{
    පූර්ණ_සංඛ්‍යා		y, x;
    පූර්ණ_සංඛ්‍යා		stride = iw * 4;
    පූර්ණ_සංඛ්‍යා		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    සඳහා (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	සඳහා (x = 0; x < iw; x++)
	{
	    නම් (fmt == IMAGE_FORMAT_RGBA)
	    {
		පූර්ණ_සංඛ්‍යා a = s[3];

		නම් (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		නැතහොත් නම් (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		නැතහොත්
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    නැතහොත්
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

    පූර්ණ_සංඛ්‍යා
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    පූර්ණ_සංඛ්‍යා		iw, ih;
    void	*bits = NULL;

    නම් (ctx == NULL)
	ආපසු FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = parimaa_na(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, NULL, 0);
    නම් (hbm == NULL || bits == NULL)
    {
	නම් (hbm != NULL)
	    DeleteObject(hbm);
	ආපසු false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    නම් (mem_dc == NULL)
    {
	DeleteObject(hbm);
	ආපසු false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    ආපසු OK;
}

    void
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    පූර්ණ_සංඛ්‍යා
image_placement_gui_init(image_placement_T *place UNUSED)
{
    ආපසු OK;
}

    void
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    void
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    පූර්ණ_සංඛ්‍යා		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    නම් (rects == NULL || n_rects == 0)
	ආපසු;

# නම් defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    නම් (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	පූර්ණ_සංඛ්‍යා		row, col;
	පූර්ණ_සංඛ්‍යා		posx, posy;
	පූර්ණ_සංඛ්‍යා		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    void
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
