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
#endif

// values for "dead_key"
#define DEAD_کلید_OFF			0	// no dead key
#define DEAD_کلید_SET_DEFAULT		1	// dead key pressed
#define DEAD_کلید_TRANSIENT_IN_ON_CHAR	2	// wait for next key press
#define DEAD_کلید_SKIP_ON_CHAR		3	// skip next _OnChar()

#اگر defined(FEAT_DIRECTX)
ایستا DWriteContext *s_dwc = پوچ;
ایستا صحیح s_directx_enabled = 0;
ایستا صحیح s_directx_load_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != پوچ && enc_utf8)
ایستا صحیح directx_enabled(تهی);
ایستا تهی directx_binddc(تهی);
#endif

#ifdef FEAT_MENU
ایستا صحیح gui_mswin_get_menu_height(صحیح fix_window);
#وگرنه
# define gui_mswin_get_menu_height(fix_window)	0
#endif

تعریفنوع ساختار keycode_trans_strategy {
    تهی (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
    تهی (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
    تهی (*ptr_process_message_usual_کلید) (UINT /*vk*/, ثابت پیام_2* /*pmsg*/);
    صحیح  (*ptr_get_active_modifiers)(تهی);
    صحیح  (*is_experimental)(تهی);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ایستا تهی _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ایستا تهی _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ایستا تهی process_message_usual_کلید_experimental(UINT /*vk*/, ثابت پیام_2* /*pmsg*/);
ایستا صحیح  get_active_modifiers_experimental(تهی);
ایستا صحیح  is_experimental_true(تهی);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_message_usual_کلید_experimental // ptr_process_message_usual_key
    , get_active_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ایستا تهی _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ایستا تهی _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, صحیح /*cRepeat UNUSED*/);
ایستا تهی process_message_usual_کلید_classic(UINT /*vk*/, ثابت پیام_2* /*pmsg*/);
ایستا صحیح  get_active_modifiers_classic(تهی);
ایستا صحیح  is_experimental_false(تهی);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_message_usual_کلید_classic // ptr_process_message_usual_key
    , get_active_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_used = پوچ;

ایستا صحیح is_experimental_true(تهی)
{
    بازگشت 1;
}

ایستا صحیح is_experimental_false(تهی)
{
    بازگشت 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ایستا تهی keycode_trans_strategy_init(تهی)
{
    ثابت نویسه *strategy = پوچ;

    // set default value as fallback
    keycode_trans_strategy_used = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    اگر (strategy == پوچ)
    {
	بازگشت;
    }

    اگر (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	بازگشت;
    }

    اگر (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	بازگشت;
    }

}

#اگر defined(FEAT_RENDER_OPTIONS)
    صحیح
gui_mch_set_rendering_options(char_u *s)
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
    برای (p = s; p != پوچ && *p != NUL; )
    {
	char_u  item[256];
	char_u  name[128];
	char_u  مقدار[128];

	copy_option_part(&p, item, اندازه(item), ",");
	اگر (p == پوچ)
	    بشکن;
	q = &item[0];
	copy_option_part(&q, name, اندازه(name), ":");
	اگر (q == پوچ)
	    بازگشت FAIL;
	copy_option_part(&q, مقدار, اندازه(مقدار), ":");

	اگر (STRCMP(name, "type") == 0)
	{
	    اگر (STRCMP(مقدار, "directx") == 0)
		dx_enable = 1;
	    وگرنه
		بازگشت FAIL;
	}
	وگرنه اگر (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (اعشاری)atof((نویسه *)مقدار);
	}
	وگرنه اگر (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (اعشاری)atof((نویسه *)مقدار);
	}
	وگرنه اگر (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (اعشاری)atof((نویسه *)مقدار);
	}
	وگرنه اگر (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((نویسه *)مقدار);
	    اگر (dx_geom < 0 || dx_geom > 2)
		بازگشت FAIL;
	}
	وگرنه اگر (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((نویسه *)مقدار);
	    اگر (dx_renmode < 0 || dx_renmode > 6)
		بازگشت FAIL;
	}
	وگرنه اگر (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((نویسه *)مقدار);
	    اگر (dx_taamode < 0 || dx_taamode > 3)
		بازگشت FAIL;
	}
	وگرنه اگر (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	وگرنه
	    بازگشت FAIL;
    }

    اگر (!gui.in_use)
	بازگشت OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    اگر (dx_enable)
    {
	اگر (!directx_enabled())
	    بازگشت FAIL;
	DWriteContext_SetRenderingParams(s_dwc, پوچ);
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
		param.renderingMode = dx_renmode;
	    اگر (dx_flags & (1 << 5))
		param.textAntialiasMode = dx_taamode;
	    DWriteContext_SetRenderingParams(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    بازگشت OK;
# وگرنه
    بازگشت FAIL;
# endif
}
#endif

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ifndef HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (صحیح)(short)LOWORD(lParam), (صحیح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (صحیح)(short)LOWORD(lParam), (صحیح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif
#ifndef HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (صحیح)(short)LOWORD(lParam), (صحیح)(short)HIWORD(lParam), (UINT)(wParam)), 0L)
#endif


#شامل "version.h"	// used by dialog box routine for default title
#ifdef DEBUG
# include <tchar.h>
#endif

#شامل <shellapi.h>
#شامل <commctrl.h>
#شامل <windowsx.h>

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

ایستا تهی _OnPaint(HWND hwnd);
ایستا تهی fill_rect(ثابت RECT *rcp, HBRUSH hbr, COLORREF color);
ایستا تهی clear_rect(RECT *rcp);

ایستا WORD		s_dlgfntheight;		// height of the dialog font
ایستا WORD		s_dlgfntwidth;		// width of the dialog font

#ifdef FEAT_MENU
ایستا HMENU		s_menuBar = پوچ;
#endif
#ifdef FEAT_TEAROFF
ایستا تهی rebuild_tearoff(vimmenu_T *menu);
ایستا HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#endif

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ایستا صحیح		s_busy_processing = FALSE;

ایستا صحیح		destroying = FALSE;	// call DestroyWindow() ourselves

#ifdef MSWIN_FIND_REPLACE
ایستا UINT		s_findrep_پیام = 0;
ایستا FINDREPLACEW	s_findrep_struct;
ایستا HWND		s_findrep_hwnd = پوچ;
ایستا صحیح		s_findrep_is_find;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#endif

HWND			s_hwnd = پوچ;
ایستا HDC		s_hdc = پوچ;
ایستا HBRUSH		s_brush = پوچ;

#ifdef FEAT_TOOLBAR
ایستا HWND		s_toolbarhwnd = پوچ;
ایستا WNDPROC		s_toolbar_wndproc = پوچ;
#endif

#ifdef FEAT_GUI_TABLINE
ایستا HWND		s_tabhwnd = پوچ;
ایستا WNDPROC		s_tabline_wndproc = پوچ;
ایستا صحیح		showing_tabline = 0;
#endif

ایستا WPARAM		s_wParam = 0;

ایستا HWND		s_textArea = پوچ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ایستا bool		s_textarea_bg_only = false;

ایستا UINT		s_u_پیام = 0;

ایستا char_u		*s_textfield; // Used by dialogs to pass back strings

ایستا صحیح		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ایستا صحیح allow_scrollbar = FALSE;

#ifndef _DPI_AWARENESS_CONTEXTS_
تعریفنوع HANDLE DPI_AWARENESS_CONTEXT;

تعریفنوع شمارش DPI_AWARENESS {
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
ایستا صحیح		s_dpi = DEFAULT_DPI;
ایستا BOOL		s_in_dpichanged = FALSE;
#ifdef FEAT_MBYTE_IME
ایستا DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#endif
ایستا RECT		s_suggested_rect;

ایستا UINT (WINAPI *pGetDpiForSystem)(تهی) = پوچ;
ایستا UINT (WINAPI *pGetDpiForWindow)(HWND hwnd) = پوچ;
ایستا صحیح (WINAPI *pGetSystemMetricsForDpi)(صحیح, UINT) = پوچ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ایستا DPI_AWARENESS_CONTEXT (WINAPI *pSetThreadDpiAwarenessContext)(DPI_AWARENESS_CONTEXT dpiContext) = پوچ;
#ifdef FEAT_MBYTE_IME
ایستا DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = پوچ;
#endif

ایستا HINSTANCE hLibDwm = پوچ;
ایستا HRESULT (WINAPI *pDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
ایستا تهی dyn_dwm_load(تهی);

ایستا صحیح fullscreen_on = FALSE;

#ifdef FEAT_GUI_DARKTHEME

ایستا HINSTANCE hUxThemeLib = پوچ;
ایستا DWORD (WINAPI *pSetPreferredAppMode)(DWORD) = پوچ;
ایستا تهی (WINAPI *pFlushMenuThemes)(تهی) = پوچ;
ایستا تهی dyn_uxtheme_load(تهی);
#endif

    ایستا صحیح WINAPI
stubGetSystemMetricsForDpi(صحیح nIndex, UINT dpi UNUSED)
{
    بازگشت GetSystemMetrics(nIndex);
}

    ایستا صحیح
adjust_fontsize_by_dpi(صحیح size)
{
    بازگشت size * s_dpi / (صحیح)pGetDpiForSystem();
}

    ایستا صحیح
adjust_by_system_dpi(صحیح size)
{
    بازگشت size * (صحیح)pGetDpiForSystem() / DEFAULT_DPI;
}

#اگر defined(FEAT_DIRECTX)
    ایستا صحیح
directx_enabled(تهی)
{
    اگر (s_dwc != پوچ)
	بازگشت 1;
    وگرنه اگر (s_directx_load_attempted)
	بازگشت 0;
    // load DirectX
    DWrite_Init();
    s_directx_load_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    بازگشت s_dwc != پوچ ? 1 : 0;
}

    ایستا تهی
directx_binddc(تهی)
{
    اگر (s_textArea == پوچ)
	بازگشت;

    RECT	rect;
    GetClientRect(s_textArea, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#endif

بیرونی صحیح current_font_height;	    // this is in os_mswin.c

ایستا ساختار
{
    UINT    کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدها[] =
{
    {VK_UP,		'k', 'u'},
    {VK_DOWN,		'k', 'd'},
    {VK_چپ,		'k', 'l'},
    {VK_راست,		'k', 'r'},

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
    {VK_درج,		'k', 'I'},
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
ایستا صحیح	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ایستا صحیح	s_getting_focus = FALSE;

ایستا صحیح	s_x_pending;
ایستا صحیح	s_y_pending;
ایستا UINT	s_kFlags_pending;
ایستا UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ایستا صحیح	s_timed_out = FALSE;
ایستا صحیح	dead_کلید = DEAD_کلید_OFF;
ایستا UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ifdef FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ایستا تهی Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ایستا تهی track_user_activity(UINT u_پیام);
#endif

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ifdef FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ایستا LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ایستا LOGFONTW sub_logfont;
#endif

#ifdef FEAT_MBYTE_IME
ایستا LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData);
#endif

#اگر defined(FEAT_BROWSE)
ایستا char_u *convert_filter(char_u *s);
#endif

#ifdef DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ایستا تهی
print_windows_error(تهی)
{
    LPVOID  lp_پیام_Buf;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		  پوچ, GetLastError(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_پیام_Buf, 0, پوچ);
    TRACE1("Error: %s\n", lp_پیام_Buf);
    LocalFree(lp_پیام_Buf);
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

ایستا صحیح		blink_state = BLINK_NONE;
ایستا long_u		blink_waittime = 700;
ایستا long_u		blink_ontime = 400;
ایستا long_u		blink_offtime = 250;
ایستا UINT_PTR		blink_timer = 0;

    صحیح
gui_mch_is_blinking(تهی)
{
    بازگشت blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(تهی)
{
    بازگشت blink_state == BLINK_OFF;
}

    تهی
gui_mch_set_blinking(long wait, long on, long off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ایستا VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_پیام UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    پیام_2 پیام;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(پوچ, idEvent);

    // Eat spurious WM_TIMER messages
    وقتی (PeekMessageW(&پیام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = SetTimer(پوچ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    وگرنه
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = SetTimer(پوچ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ایستا تهی
gui_mswin_rm_blink_timer(تهی)
{
    پیام_2 پیام;

    اگر (blink_timer == 0)
	بازگشت;

    KillTimer(پوچ, blink_timer);
    // Eat spurious WM_TIMER messages
    وقتی (PeekMessageW(&پیام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    تهی
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
    تهی
gui_mch_start_blink(تهی)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = SetTimer(پوچ, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ایستا VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_پیام UNUSED,
    UINT_PTR idEvent,
    DWORD dwTime UNUSED)
{
    پیام_2 پیام;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(پوچ, idEvent);
    s_timed_out = TRUE;

    // Eat spurious WM_TIMER messages
    وقتی (PeekMessageW(&پیام, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    اگر (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ایستا تهی
_OnDeadChar(
    HWND hwnd UNUSED,
    UINT ch UNUSED,
    صحیح cRepeat UNUSED)
{
    dead_کلید = DEAD_کلید_SET_DEFAULT;
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
    ایستا صحیح
char_to_string(صحیح ch, char_u *string, صحیح slen, صحیح had_alt)
{
    صحیح		len;
    صحیح		i;
    WCHAR	wstring[2];
    char_u	*ws = پوچ;

    اگر (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    وگرنه اگر (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	بازگشت 0;
    }
    وگرنه
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    اگر (enc_codepage > 0)
    {
	len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, پوچ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	اگر (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = WideCharToMultiByte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, پوچ);
	    اگر (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    وگرنه
    {
	ws = utf16_to_enc(wstring, &len);
	اگر (ws == پوچ)
	    len = 0;
	وگرنه
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

    برای (i = 0; i < len; ++i)
	اگر (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (صحیح)KE_CSI;
	    len += 2;
	}

    بازگشت len;
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
    ایستا صحیح
get_active_modifiers_experimental(تهی)
{
    صحیح modifiers = 0;

    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    اگر (Get_کلید_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    اگر (Get_کلید_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;
    اگر ((modifiers & MOD_MASK_CTRL) && (Get_کلید_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_MASK_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    اگر (!(Get_کلید_State(VK_CONTROL) & 0x8000)
	    &&  (Get_کلید_State(VK_RMENU) & 0x8000)
	    && !(Get_کلید_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_MASK_ALT;

    بازگشت modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ایستا صحیح
get_active_modifiers_classic(تهی)
{
    صحیح modifiers = 0;

    اگر (Get_کلید_State(VK_SHIFT) & 0x8000)
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
    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;
    اگر (Get_کلید_State(VK_MENU) & 0x8000)
	modifiers |= MOD_MASK_ALT;

    بازگشت modifiers;
}

    ایستا صحیح
get_active_modifiers(تهی)
{
    بازگشت get_active_modifiers_experimental();
}

    ایستا صحیح
get_active_modifiers_via_ptr(تهی)
{
    // marshal to corresponding implementation
    بازگشت keycode_trans_strategy_used->ptr_get_active_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ایستا تهی
_OnChar(
    HWND hwnd UNUSED,
    UINT cch,
    صحیح cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ایستا تهی
_OnChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    صحیح cRepeat UNUSED)
{
    char_u	string[40];
    صحیح		len = 0;
    صحیح		modifiers;
    صحیح		ch = cch;   // special keys are negative

    اگر (dead_کلید == DEAD_کلید_SKIP_ON_CHAR)
	بازگشت;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    اگر (dead_کلید != DEAD_کلید_TRANSIENT_IN_ON_CHAR)
	dead_کلید = DEAD_کلید_OFF;

    modifiers = get_active_modifiers_experimental();

    ch = simplify_کلید(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_کلیدها;
    ch = may_adjust_کلید_for_ctrl(modifiers, ch);
    --no_reduce_کلیدها;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, پوچ);
    اگر (ch == CSI)
	ch = K_CSI;

    اگر (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	add_to_درون_داد_buf(string, 3);
    }

    len = char_to_string(ch, string, 40, FALSE);
    اگر (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_درون_داد_buf();
	got_int = TRUE;
    }

    add_to_درون_داد_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ایستا تهی
_OnChar_classic(
    HWND hwnd UNUSED,
    UINT ch,
    صحیح cRepeat UNUSED)
{
    char_u	string[40];
    صحیح		len = 0;

    dead_کلید = 0;

    len = char_to_string(ch, string, 40, FALSE);
    اگر (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_درون_داد_buf();
	got_int = TRUE;
    }

    add_to_درون_داد_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ایستا تهی
_OnSysChar(
    HWND hwnd UNUSED,
    UINT cch,
    صحیح cRepeat UNUSED)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ایستا تهی
_OnSysChar_experimental(
    HWND hwnd UNUSED,
    UINT cch,
    صحیح cRepeat UNUSED)
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
    modifiers = get_active_modifiers_experimental();
    ch = simplify_کلید(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, پوچ);
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
	string[len++] = K_SECOND((صحیح)ch);
	string[len++] = K_THIRD((صحیح)ch);
    }
    وگرنه
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_درون_داد_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ایستا تهی
_OnSysChar_classic(
    HWND hwnd UNUSED,
    UINT cch,
    صحیح cRepeat UNUSED)
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
    modifiers = MOD_MASK_ALT;
    اگر (Get_کلید_State(VK_SHIFT) & 0x8000)
	modifiers |= MOD_MASK_SHIFT;
    اگر (Get_کلید_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_MASK_CTRL;

    ch = simplify_کلید(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_shift_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = extract_modifiers(ch, &modifiers, FALSE, پوچ);
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
	string[len++] = K_SECOND((صحیح)ch);
	string[len++] = K_THIRD((صحیح)ch);
    }
    وگرنه
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_to_string(ch, string + len, 40 - len, TRUE);
    }

    add_to_درون_داد_buf(string, len);
}

    ایستا تهی
_OnMouseEvent(
    صحیح button,
    صحیح x,
    صحیح y,
    صحیح repeated_click,
    UINT کلید_Flags)
{
    صحیح vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    اگر (کلید_Flags & MK_SHIFT)
	vim_modifiers |= MOUSE_SHIFT;
    اگر (کلید_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    اگر (Get_کلید_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ایستا تهی
_OnMouseButtonDown(
    HWND hwnd UNUSED,
    BOOL fDoubleClick UNUSED,
    صحیح x,
    صحیح y,
    UINT کلید_Flags)
{
    ایستا LONG	s_prevTime = 0;

    LONG    currentTime = GetMessageTime();
    صحیح	    button = -1;
    صحیح	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (تهی)SetFocus(s_hwnd);

    اگر (s_u_پیام == WM_LBUTTONDOWN || s_u_پیام == WM_LBUTTONDBLCLK)
	button = MOUSE_چپ;
    وگرنه اگر (s_u_پیام == WM_MBUTTONDOWN || s_u_پیام == WM_MBUTTONDBLCLK)
	button = MOUSE_میانه;
    وگرنه اگر (s_u_پیام == WM_RBUTTONDOWN || s_u_پیام == WM_RBUTTONDBLCLK)
	button = MOUSE_راست;
    وگرنه اگر (s_u_پیام == WM_XBUTTONDOWN || s_u_پیام == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    وگرنه اگر (s_u_پیام == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	اگر (s_button_pending == MOUSE_چپ)
	    button = MOUSE_راست;
	وگرنه
	    button = MOUSE_چپ;
    }

    اگر (button < 0)
	بازگشت;

    repeated_click = ((صحیح)(currentTime - s_prevTime) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    اگر (repeated_click
	    && ((button == MOUSE_چپ && s_button_pending == MOUSE_راست)
		|| (button == MOUSE_راست
		    && s_button_pending == MOUSE_چپ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_میانه;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, کلید_Flags);
    }
    وگرنه اگر ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_راست)))
    {
	اگر (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, کلید_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, کلید_Flags);
    }
    وگرنه
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

    s_prevTime = currentTime;
}

    ایستا تهی
_OnMouseMoveOrRelease(
    HWND hwnd UNUSED,
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
    اگر (s_u_پیام == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	اگر (!(کلید_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    بازگشت;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	SetCapture(s_textArea);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    وگرنه
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, کلید_Flags);
}

    ایستا تهی
_OnSizeTextArea(
    HWND hwnd UNUSED,
    UINT state UNUSED,
    صحیح cx UNUSED,
    صحیح cy UNUSED)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	directx_binddc();
#endif
}

#ifdef FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ایستا vimmenu_T *
gui_mswin_find_menu(
    vimmenu_T	*pMenu,
    صحیح		id)
{
    vimmenu_T	*pChildMenu;

    وقتی (pMenu)
    {
	اگر (pMenu->id == (UINT)id)
	    بشکن;
	اگر (pMenu->children != پوچ)
	{
	    pChildMenu = gui_mswin_find_menu(pMenu->children, id);
	    اگر (pChildMenu)
	    {
		pMenu = pChildMenu;
		بشکن;
	    }
	}
	pMenu = pMenu->next;
    }
    بازگشت pMenu;
}

    ایستا تهی
_OnMenu(
    HWND	hwnd UNUSED,
    صحیح		id,
    HWND	hwndCtl UNUSED,
    UINT	codeNotify UNUSED)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_find_menu(ریشه_menu, id);
    اگر (pMenu)
	gui_menu_cb(pMenu);
}
#endif

#ifdef MSWIN_FIND_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ایستا تهی
_OnFindRepl(تهی)
{
    صحیح	    flags = 0;
    صحیح	    down;

    اگر (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(تهی)SetFocus(s_hwnd);

    اگر (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(تهی)SetFocus(s_hwnd);
    }
    وگرنه اگر (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(تهی)SetFocus(s_hwnd);
    }
    وگرنه اگر (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    اگر (flags == 0)
	بازگشت;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    اگر (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_WHOLE_WORD;
    اگر (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_MATCH_CASE;
    down = (s_findrep_struct.Flags & FR_DOWN) != 0;
    p = utf16_to_enc(s_findrep_struct.lpstrFindWhat, پوچ);
    q = utf16_to_enc(s_findrep_struct.lpstrReplaceWith, پوچ);
    اگر (p != پوچ && q != پوچ)
	gui_do_findrepl(flags, p, q, down);
    vim_free(p);
    vim_free(q);
}
#endif

    ایستا تهی
HandleMouseHide(UINT u_پیام, LPARAM lParam)
{
    ایستا LPARAM last_lParam = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    اگر (u_پیام == WM_MOUSEMOVE || u_پیام == WM_NCMOUSEMOVE)
    {
	اگر (lParam == last_lParam)
	    بازگشت;
	last_lParam = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    انتخاب (u_پیام)
    {
    حالت WM_KEYUP:
    حالت WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	اگر (p_mh)
	    gui_mch_mousehide(TRUE);
	بشکن;

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
	بشکن;
    }
}

    ایستا LRESULT CALLBACK
_TextAreaWndProc(
    HWND hwnd,
    UINT u_پیام,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_پیام, lParam);

    s_u_پیام = u_پیام;
    s_wParam = wParam;

#ifdef FEAT_BEVAL_GUI
    track_user_activity(u_پیام);
#endif

    انتخاب (u_پیام)
    {
	HANDLE_پیام(hwnd, WM_LBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_LBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیام(hwnd, WM_MBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_MBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیام(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_پیام(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_پیام(hwnd, WM_RBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_RBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیام(hwnd, WM_XBUTTONDBLCLK,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_XBUTTONDOWN,_OnMouseButtonDown);
	HANDLE_پیام(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_پیام(hwnd, WM_SIZE,	_OnSizeTextArea);

#ifdef FEAT_BEVAL_GUI
	حالت WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    بازگشت TRUE;
#endif
	پیشفرض:
	    بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
    }
}

    ایستا تهی
dyn_dwm_load(تهی)
{
    hLibDwm = vimLoadLib("dwmapi.dll");
    اگر (hLibDwm == پوچ)
	بازگشت;

    pDwmSetWindowAttribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	GetProcAddress(hLibDwm, "DwmSetWindowAttribute");

    اگر (pDwmSetWindowAttribute == پوچ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = پوچ;
	بازگشت;
    }
}

بیرونی DWORD win_version; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    تهی
gui_mch_set_titlebar_colors(تهی)
{
#define DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    اگر (pDwmSetWindowAttribute == پوچ || win_version < MAKE_VER(10, 0, 22000))
	بازگشت;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T textColor = DWMWA_COLOR_DEFAULT;

    اگر (vim_strchr(p_go, GO_TITLEBAR) != پوچ)
    {
	اگر (gui.in_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    textColor = gui.title_fg_pixel;
	}
	وگرنه
	{
	    captionColor = gui.titlenc_bg_pixel;
	    textColor = gui.titlenc_fg_pixel;
	}

	اگر (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	اگر (textColor == INVALCOLOR)
	    textColor = DWMWA_COLOR_DEFAULT;
    }

    pDwmSetWindowAttribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, اندازه(captionColor));
    pDwmSetWindowAttribute(s_hwnd, DWMWA_TEXT_COLOR,
	    &textColor, اندازه(textColor));
}

/*
 * Called when the foreground or background color has been changed.
 */
    تهی
gui_mch_new_colors(تهی)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.back_pixel);
    prevBrush = (HBRUSH)SetClassLongPtr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, پوچ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    اگر (s_textarea_bg_only && s_textArea != پوچ)
	InvalidateRect(s_textArea, پوچ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    تهی
gui_mch_def_colors(تهی)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.back_pixel = GetSysColor(COLOR_WINDOW);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_back_pixel = gui.back_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(تهی)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    اگر (!IsWindowVisible(s_hwnd))
    {
	ShowWindow(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	اگر (s_textArea != پوچ)
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

    بازگشت OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    RECT    rect;

    GetWindowRect(s_hwnd, &rect);
    *x = rect.چپ;
    *y = rect.بالاترین;
    بازگشت OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    تهی
gui_mch_set_winpos(صحیح x, صحیح y)
{
    SetWindowPos(s_hwnd, پوچ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    تهی
gui_mch_set_text_area_pos(صحیح x, صحیح y, صحیح w, صحیح h)
{
    ایستا صحیح oldx = 0;
    ایستا صحیح oldy = 0;

    SetWindowPos(s_textArea, پوچ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ifdef FEAT_TOOLBAR
    اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ)
	SendMessage(s_toolbarhwnd, WM_SIZE,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_height));
#endif
#اگر defined(FEAT_GUI_TABLINE)
    اگر (showing_tabline)
    {
	صحیح	بالاترین = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ)
	    بالاترین = gui.toolbar_height;
# endif
	GetClientRect(s_hwnd, &rect);
	MoveWindow(s_tabhwnd, 0, بالاترین, rect.راست, gui.tabline_height, TRUE);
    }
#endif

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    اگر (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, پوچ, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    تهی
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    صحیح		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    تهی
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    SetWindowPos(sb->id, پوچ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    صحیح
gui_mch_get_scrollbar_xpadding(تهی)
{
    RECT    rcTxt, rcWnd;
    صحیح	    xpad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    xpad = rcWnd.راست - rcTxt.راست - gui.scrollbar_width
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    بازگشت (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(تهی)
{
    RECT    rcTxt, rcWnd;
    صحیح	    ypad;

    GetWindowRect(s_textArea, &rcTxt);
    GetWindowRect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_height
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    بازگشت (ypad < 0) ? 0 : ypad;
}

    تهی
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحیح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = CreateWindow(
	"SCROLLBAR", "Scrollbar",
	WS_CHILD | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, پوچ,
	g_hinst, پوچ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ایستا scrollbar_T *
gui_mswin_find_scrollbar(HWND hwnd)
{
    win_T	*wp;

    اگر (gui.bottom_sbar.id == hwnd)
	بازگشت &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	اگر (wp->w_scrollbars[SBAR_چپ].id == hwnd)
	    بازگشت &wp->w_scrollbars[SBAR_چپ];
	اگر (wp->w_scrollbars[SBAR_راست].id == hwnd)
	    بازگشت &wp->w_scrollbars[SBAR_راست];
    }
    بازگشت پوچ;
}

    ایستا تهی
update_scrollbar_size(تهی)
{
    gui.scrollbar_width = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_height = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ایستا تهی
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
    ایستا تهی
GetFontSize(GuiFont font, صحیح *char_width, صحیح *char_height)
{
    HWND    hwnd = GetDesktopWindow();
    HDC	    hdc = GetWindowDC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    SIZE    size;
    TEXTMETRIC tm;

    GetTextMetrics(hdc, &tm);
    GetAverageFontSize(hdc, &size);

    اگر (char_width)
	*char_width = size.cx + tm.tmOverhang;
    اگر (char_height)
	*char_height = tm.tmHeight + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ایستا تهی
UpdateFontSize(GuiFont font)
{
    GetFontSize(font, &gui.char_width, &gui.char_height);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(تهی)
{
    UpdateFontSize(gui.norm_font);
    بازگشت OK;
}

    ایستا GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = پوچ;

    // Load the font
    font = CreateFontIndirectW(lf);

    اگر (font == پوچ)
	بازگشت NOFONT;

    بازگشت (GuiFont)font;
}

    ایستا صحیح
pixels_to_points(صحیح pixels, صحیح vertical)
{
    صحیح		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    بازگشت points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    صحیح		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    اگر (get_logfont(&lf, name, پوچ, giveErrorIfMissing) == OK)
    {
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    اگر (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    بازگشت font;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font UNUSED, char_u *name)
{
    اگر (name == پوچ)
	بازگشت پوچ;
    بازگشت vim_strsave(name);
}
#endif

    تهی
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

    تعریفنوع ساختار SysColorTable
    {
	نویسه	    *name;
	صحیح	    color;
    } SysColorTable;

    ایستا SysColorTable sys_table[] =
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
    برای (i = 0; i < (صحیح)ARRAY_LENGTH(sys_table); i++)
	اگر (STRICMP(name, sys_table[i].name) == 0)
	    بازگشت GetSysColor(sys_table[i].color);

    بازگشت gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    بازگشت gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برای (i = 0; special_کلیدها[i].vim_code1 != NUL; i++)
	اگر (name[0] == special_کلیدها[i].vim_code0
				       && name[1] == special_کلیدها[i].vim_code1)
	    بازگشت OK;
    بازگشت FAIL;
}

    تهی
gui_mch_beep(تهی)
{
    MessageBeep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    تهی
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
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.چپ = FILL_X(c);
    rc.بالاترین = FILL_Y(r);
    rc.راست = rc.چپ + nc * gui.char_width;
    rc.bottom = rc.بالاترین + nr * gui.char_height;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    تهی
gui_mch_iconify(تهی)
{
    ShowWindow(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    تهی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.چپ = FILL_X(gui.col);
    rc.بالاترین = FILL_Y(gui.row);
    rc.راست = rc.چپ + gui.char_width;
    اگر (mb_lefthalve(gui.row, gui.col))
	rc.راست += gui.char_width;
    rc.bottom = rc.بالاترین + gui.char_height;
    hbr = CreateSolidBrush(color);
    FrameRect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    تهی
gui_mch_draw_part_cursor(
    صحیح		w,
    صحیح		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.چپ =
#ifdef FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_راست ? FILL_X(gui.col + 1) - w :
#endif
		    FILL_X(gui.col);
    rc.بالاترین = FILL_Y(gui.row) + gui.char_height - h;
    rc.راست = rc.چپ + w;
    rc.bottom = rc.بالاترین + h;

    fill_rect(&rc, پوچ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ایستا تهی
output_Dead_کلید_re_Post_Ex(پیام_2 original_پیام, صحیح dead_key2set)
{
    ایستا پیام_2 deadCharExpel;

    اگر (dead_کلید == DEAD_کلید_OFF)
	بازگشت;

    dead_کلید = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.message = original_پیام.message;
    deadCharExpel.hwnd    = original_پیام.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    TranslateMessage(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    PostMessage(original_پیام.hwnd, original_پیام.message, original_پیام.wParam,
							  original_پیام.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ایستا تهی
output_Dead_کلید_re_Post(پیام_2 original_پیام)
{
    output_Dead_کلید_re_Post_Ex(original_پیام, DEAD_کلید_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ایستا تهی process_message_usual_کلید(UINT vk, ثابت پیام_2 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_used->ptr_process_message_usual_کلید(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ایستا تهی
process_message_usual_کلید_experimental(UINT vk, ثابت پیام_2 *pmsg)
{
    WCHAR	ch[8];
    صحیح		len;
    صحیح		i;
    UINT	scan_code;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    اگر (Get_کلید_State(VK_SHIFT) & 0x8000)
	keyboard_state[VK_SHIFT] = 0x80;
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
    scan_code = Map_Virtual_کلید(vk, MAPVK_VK_TO_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ToUnicode(vk, scan_code, keyboard_state, ch, ARRAY_LENGTH(ch),
	    0);
    اگر (len < 0)
	dead_کلید = DEAD_کلید_SET_DEFAULT;

    اگر (len <= 0)
    {
	صحیح wm_char = NUL;

	اگر (dead_کلید == DEAD_کلید_SET_DEFAULT
		&& (Get_کلید_State(VK_CONTROL) & 0x8000))
	{
	    اگر (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && scan_code == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && scan_code == 41))
		wm_char = '[';
	    اگر (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && scan_code == 27))
		wm_char = ']';
	}
	اگر (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    PostMessageW(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_کلید = DEAD_کلید_TRANSIENT_IN_ON_CHAR;
	}
	بازگشت;
    }

    // Post the message as TranslateMessage would do.
    اگر (pmsg->message == WM_KEYDOWN)
    {
	برای (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    وگرنه
    {
	برای (i = 0; i < len; i++)
	    PostMessageW(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ایستا تهی
process_message_usual_کلید_classic(UINT vk, ثابت پیام_2 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    اگر (vk != 0xff
	    && (Get_کلید_State(VK_CONTROL) & 0x8000)
	    && !(Get_کلید_State(VK_SHIFT) & 0x8000)
	    && !(Get_کلید_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	اگر (vk == '6' || Map_Virtual_کلید(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    add_to_درون_داد_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	وگرنه اگر (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    add_to_درون_داد_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	وگرنه اگر (vk == '2' || Map_Virtual_کلید(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    add_to_درون_داد_buf(string, 1);
	}
	وگرنه
	    TranslateMessage(pmsg);
    }
    وگرنه
	TranslateMessage(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ایستا تهی
process_message(تهی)
{
    پیام_2		پیام;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    صحیح		i;
    صحیح		modifiers = 0;
    صحیح		کلید;
#ifdef FEAT_MENU
    ایستا char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#endif
    ایستا صحیح  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    اگر (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    GetMessageW(&پیام, پوچ, 0, 0);

#ifdef FEAT_OLE
    // Look after OLE Automation commands
    اگر (پیام.message == WM_OLE)
    {
	char_u *str = (char_u *)پیام.lParam;
	اگر (str == پوچ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    DispatchMessageW(&پیام);
	}
	وگرنه
	{
	    add_to_درون_داد_buf(str, (صحیح)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	بازگشت;
    }
#endif

#ifdef MSWIN_FIND_REPLACE
    // Don't process messages used by the dialog
    اگر (s_findrep_hwnd != پوچ && IsDialogMessageW(s_findrep_hwnd, &پیام))
    {
	HandleMouseHide(پیام.message, پیام.lParam);
	بازگشت;
    }
#endif

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    اگر (پیام.message == WM_KEYDOWN || پیام.message == WM_SYSKEYDOWN)
    {
	vk = (صحیح) پیام.wParam;

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
	اگر (keycode_trans_strategy_used->is_experimental()
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
	    اگر (keycode_trans_strategy_used->is_experimental() &&
		    dead_کلید == DEAD_کلید_TRANSIENT_IN_ON_CHAR)
	    {
		output_Dead_کلید_re_Post_Ex(پیام,
				       /*dead_key2set=*/DEAD_کلید_SKIP_ON_CHAR);
		بازگشت;
	    }

	    اگر (dead_کلید != DEAD_کلید_SET_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		بازگشت;
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
	    اگر ((vk == VK_SPACE || vk == VK_BACK || vk == VK_ESCAPE))
	    {
		dead_کلید = DEAD_کلید_OFF;
		TranslateMessage(&پیام);
		بازگشت;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    وگرنه اگر ((get_real_state()
			    & (MODE_درج | MODE_CMDLINE | MODE_SELECT)) == 0)
	    {
		output_Dead_کلید_re_Post(پیام);
		بازگشت;
	    }
	}

	// Check for CTRL-BREAK
	اگر (vk == VK_CANCEL)
	{
	    trash_درون_داد_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    add_to_درون_داد_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	اگر (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    TranslateMessage(&پیام);
	    بازگشت;
	}

	برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    اگر (special_کلیدها[i].کلید_sym == vk
		    && (vk != VK_SPACE || !(Get_کلید_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		اگر (dead_کلید == DEAD_کلید_SET_DEFAULT
			&& (special_کلیدها[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_کلید_re_Post(پیام);
		    بازگشت;
		}

#ifdef FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		اگر (vk == VK_F10
			&& gui.menu_is_active
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  پوچ, پوچ) == پوچ)
		    بشکن;
#endif
		modifiers = get_active_modifiers_via_ptr();

		اگر (special_کلیدها[i].vim_code1 == NUL)
		    کلید = special_کلیدها[i].vim_code0;
		وگرنه
		    کلید = TO_SPECIAL(special_کلیدها[i].vim_code0,
						   special_کلیدها[i].vim_code1);
		کلید = simplify_کلید(کلید, &modifiers);
		اگر (کلید == CSI)
		    کلید = K_CSI;

		اگر (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    add_to_درون_داد_buf(string, 3);
		}

		اگر (IS_SPECIAL(کلید))
		{
		    string[0] = CSI;
		    string[1] = K_SECOND(کلید);
		    string[2] = K_THIRD(کلید);
		    add_to_درون_داد_buf(string, 3);
		}
		وگرنه
		{
		    صحیح	len;

		    // Handle "key" as a Unicode character.
		    len = char_to_string(کلید, string, 40, FALSE);
		    add_to_درون_داد_buf(string, len);
		}
		بشکن;
	    }
	}

	// Not a special key.
	اگر (special_کلیدها[i].کلید_sym == 0)
	{
	    process_message_usual_کلید(vk, &پیام);
	}
    }
#ifdef FEAT_MBYTE_IME
    وگرنه اگر (پیام.message == WM_IME_NOTIFY)
	_OnImeNotify(پیام.hwnd, (DWORD)پیام.wParam, (DWORD)پیام.lParam);
    وگرنه اگر (پیام.message == WM_KEYUP && im_get_status())
	// added for non-MS IME (Yasuhiro Matsumoto)
	TranslateMessage(&پیام);
#endif

#ifdef FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    اگر (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  پوچ, پوچ) == پوچ)
#endif
	DispatchMessageW(&پیام);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    تهی
gui_mch_update(تهی)
{
    پیام_2	    پیام;

    اگر (!s_busy_processing)
	وقتی (PeekMessageW(&پیام, پوچ, 0, 0, PM_NOREMOVE)
						  && !vim_is_درون_داد_buf_full())
	    process_message();
}

    ایستا تهی
remove_any_timer(تهی)
{
    پیام_2		پیام;

    اگر (s_wait_timer != 0 && !s_timed_out)
    {
	KillTimer(پوچ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	وقتی (PeekMessageW(&پیام, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ifdef FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ایستا garray_T	server_pending_ga = {0, 0, 0, 0, پوچ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    تهی
server_add_درون_داد(char_u *str)
{
    اگر (server_pending_ga.ga_data == پوچ)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ایستا تهی
server_flush_درون_داد(تهی)
{
    char_u  *str;

    اگر (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	بازگشت;
    str = vim_strnsave(server_pending_ga.ga_data, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    اگر (str != پوچ)
    {
	server_to_درون_داد_buf(str);
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
    صحیح
gui_mch_wait_for_chars(long wtime)
{
    صحیح		focus;

    s_timed_out = FALSE;

    اگر (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	اگر (s_busy_processing)
	    بازگشت FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = SetTimer(پوچ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.in_focus;
    وقتی (!s_timed_out)
    {
	// Stop or start blinking when focus changes
	اگر (gui.in_focus != focus)
	{
	    اگر (gui.in_focus)
		gui_mch_start_blink();
	    وگرنه
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

	اگر (s_need_activate)
	{
	    (تهی)SetForegroundWindow(s_hwnd);
	    s_need_activate = FALSE;
	}

#ifdef FEAT_TIMERS
	did_add_timer = FALSE;
#endif
#ifdef MESSAGE_QUEUE
	// Check channel I/O while waiting for a message.
	برای (;;)
	{
	    پیام_2 پیام;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_درون_داد();
# endif
# ifdef FEAT_TIMERS
	    اگر (did_add_timer)
		بشکن;
# endif
	    اگر (PeekMessageW(&پیام, پوچ, 0, 0, PM_NOREMOVE))
	    {
		process_message();
		بشکن;
	    }
	    وگرنه اگر (درون_داد_available()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || پیام_Wait_For_Multiple_Objects(0, پوچ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		بشکن;
	}
#وگرنه
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_message();
#endif

	اگر (درون_داد_available())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    اگر (!s_getting_focus)
		s_button_pending = -1;

	    بازگشت OK;
	}

#ifdef FEAT_TIMERS
	اگر (did_add_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    بشکن;
	}
#endif
    }
    allow_scrollbar = FALSE;
    بازگشت FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    تهی
gui_mch_clear_block(
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
    rc.چپ = FILL_X(col1);
    rc.بالاترین = FILL_Y(row1);
    rc.راست = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    تهی
gui_mch_clear_all(تهی)
{
    RECT    rc;

    rc.چپ = 0;
    rc.بالاترین = 0;
    rc.راست = Columns * gui.char_width + 2 * gui.border_width;
    rc.bottom = Rows * gui.char_height + 2 * gui.border_width;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    تهی
gui_mch_enable_menu(صحیح flag)
{
#ifdef FEAT_MENU
    SetMenu(s_hwnd, flag ? s_menuBar : پوچ);
#endif
}

    تهی
gui_mch_set_menu_pos(
    صحیح	    x UNUSED,
    صحیح	    y UNUSED,
    صحیح	    w UNUSED,
    صحیح	    h UNUSED)
{
    // It will be in the right place anyway
}

#اگر defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    تهی
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
    تهی
gui_mch_draw_menubar(تهی)
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
    بازگشت (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#اگر defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ایستا WORD
PixelToDialogX(صحیح رقم_Pixels)
{
    بازگشت (WORD)((رقم_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ایستا WORD
PixelToDialogY(صحیح رقم_Pixels)
{
    بازگشت (WORD)((رقم_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ایستا صحیح
GetTextWidth(HDC hdc, char_u *str, صحیح len)
{
    SIZE    size;

    GetTextExtentPoint(hdc, (LPCSTR)str, len, &size);
    بازگشت size.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ایستا صحیح
GetTextWidthEnc(HDC hdc, char_u *str, صحیح len)
{
    SIZE	size;
    WCHAR	*wstr;
    صحیح		n;
    صحیح		wlen = len;

    wstr = enc_to_utf16(str, &wlen);
    اگر (wstr == پوچ)
	بازگشت 0;

    n = GetTextExtentPointW(hdc, wstr, wlen, &size);
    vim_free(wstr);
    اگر (n)
	بازگشت size.cx;
    بازگشت 0;
}

ایستا تهی get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ایستا BOOL
CenterWindow(
    HWND hwndChild,
    HWND hwndParent)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    rChild, rParent, rScreen;
    صحیح		    wChild, hChild, wParent, hParent;
    صحیح		    xNew, yNew;
    HDC		    hdc;

    GetWindowRect(hwndChild, &rChild);
    wChild = rChild.راست - rChild.چپ;
    hChild = rChild.bottom - rChild.بالاترین;

    // If Vim is minimized put the window in the middle of the screen.
    اگر (hwndParent == پوچ || IsMinimized(hwndParent))
	get_work_area(&rParent);
    وگرنه
	GetWindowRect(hwndParent, &rParent);
    wParent = rParent.راست - rParent.چپ;
    hParent = rParent.bottom - rParent.بالاترین;

    moninfo.cbSize = اندازه(MONITORINFO);
    mon = MonitorFromWindow(hwndChild, MONITOR_DEFAULTTOPRIMARY);
    اگر (mon != پوچ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    وگرنه
    {
	hdc = GetDC(hwndChild);
	rScreen.چپ = 0;
	rScreen.بالاترین = 0;
	rScreen.راست = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwndChild, hdc);
    }

    xNew = rParent.چپ + ((wParent - wChild) / 2);
    اگر (xNew < rScreen.چپ)
	xNew = rScreen.چپ;
    وگرنه اگر ((xNew + wChild) > rScreen.راست)
	xNew = rScreen.راست - wChild;

    yNew = rParent.بالاترین + ((hParent - hChild) / 2);
    اگر (yNew < rScreen.بالاترین)
	yNew = rScreen.بالاترین;
    وگرنه اگر ((yNew + hChild) > rScreen.bottom)
	yNew = rScreen.bottom - hChild;

    بازگشت SetWindowPos(hwndChild, پوچ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#endif // FEAT_GUI_DIALOG

#اگر defined(FEAT_TOOLBAR)
    تهی
gui_mch_show_toolbar(صحیح showit)
{
    اگر (s_toolbarhwnd == پوچ)
	بازگشت;

    اگر (showit)
    {
	// Enable unicode support
	SendMessage(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	ShowWindow(s_toolbarhwnd, SW_SHOW);
    }
    وگرنه
	ShowWindow(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_شمار 31

#endif

#اگر defined(FEAT_GUI_TABLINE)
    ایستا تهی
add_tabline_popup_menu_entry(HMENU pmenu, UINT item_id, char_u *item_text)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_to_utf16(item_text, پوچ);
    اگر (wn == پوچ)
	بازگشت;

    infow.cbSize = اندازه(infow);
    infow.fMask = MIIM_TYPE | MIIM_ID;
    infow.wID = item_id;
    infow.fType = MFT_STRING;
    infow.dwTypeData = wn;
    infow.cch = (UINT)wcslen(wn);
    درج_Menu_Item_W(pmenu, item_id, FALSE, &infow);
    vim_free(wn);
}

    ایستا تهی
show_tabline_popup_menu(تهی)
{
    HMENU	    tab_pmenu;
    long	    rval;
    POINT	    pt;

    // When ignoring events don't show the menu.
    اگر (hold_gui_events || cmdwin_type != 0)
	بازگشت;

    tab_pmenu = CreatePopupMenu();
    اگر (tab_pmenu == پوچ)
	بازگشت;

    اگر (first_tabpage->tp_next != پوچ)
	add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    add_tabline_popup_menu_entry(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									پوچ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    اگر (rval > 0)
    {
	TCHITTESTINFO htinfo;
	صحیح idx;

	اگر (ScreenToClient(s_tabhwnd, &pt) == 0)
	    بازگشت;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	اگر (idx == -1)
	    idx = 0;
	وگرنه
	    idx += 1;

	send_tabline_menu_event(idx, (صحیح)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    تهی
gui_mch_show_tabline(صحیح showit)
{
    اگر (s_tabhwnd == پوچ)
	بازگشت;

    اگر (!showit != !showing_tabline)
    {
	اگر (showit)
	    ShowWindow(s_tabhwnd, SW_SHOW);
	وگرنه
	    ShowWindow(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(تهی)
{
    بازگشت s_tabhwnd != پوچ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    تهی
gui_mch_update_tabline(تهی)
{
    tabpage_T	*tp;
    TCITEM	tie;
    صحیح		nr = 0;
    صحیح		curtabidx = 0;
    صحیح		tabadded = 0;
    WCHAR	*wstr = پوچ;

    اگر (s_tabhwnd == پوچ)
	بازگشت;

    // Enable unicode support
    SendMessage(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.mask = TCIF_TEXT;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	اگر (nr >= Tab_Ctrl_Get_Item_شمار(s_tabhwnd))
	{
	    // Add the tab
	    tie.pszText = "-Empty-";
	    Tab_Ctrl_درج_Item(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.pszText = (LPSTR)NameBuff;

	wstr = enc_to_utf16(NameBuff, پوچ);
	اگر (wstr != پوچ)
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
    وقتی (nr < Tab_Ctrl_Get_Item_شمار(s_tabhwnd))
	TabCtrl_DeleteItem(s_tabhwnd, nr);

    اگر (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    SendMessage(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    RedrawWindow(s_tabhwnd, پوچ, پوچ,
		    RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);

    اگر (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	TabCtrl_SetCurSel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    تهی
gui_mch_set_curtab(صحیح nr)
{
    اگر (s_tabhwnd == پوچ)
	بازگشت;

    اگر (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	TabCtrl_SetCurSel(s_tabhwnd, nr - 1);
}

#endif

#ifdef FEAT_GUI_DARKTHEME
    تهی
gui_mch_set_dark_theme(صحیح dark)
{
    اگر (pDwmSetWindowAttribute != پوچ && win_version >= MAKE_VER(10, 0, 18985))
	pDwmSetWindowAttribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
		اندازه(dark));

    اگر (pSetPreferredAppMode != پوچ && win_version >= MAKE_VER(10, 0, 18362))
	pSetPreferredAppMode(dark);

    اگر (pFlushMenuThemes != پوچ && win_version >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ایستا تهی
dyn_uxtheme_load(تهی)
{
    hUxThemeLib = vimLoadLib("uxtheme.dll");
    اگر (hUxThemeLib == پوچ)
	بازگشت;

    pSetPreferredAppMode = (DWORD (WINAPI *)(DWORD))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (تهی (WINAPI *)(تهی))
	GetProcAddress(hUxThemeLib, MAKEINTRESOURCE(136));

    اگر (pSetPreferredAppMode == پوچ || pFlushMenuThemes == پوچ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = پوچ;
	بازگشت;
    }
}

#endif // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    تهی
gui_mch_set_fullscreen(صحیح flag)
{
    ایستا RECT normal_rect;
    ایستا LONG_PTR normal_style, normal_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    اگر (!full_screen) // Windows not set yet.
	بازگشت;

    اگر (flag)
    {
	اگر (fullscreen_on)
	    بازگشت;

	// Enter fullscreen mode
	GetWindowRect(s_hwnd, &rc);

	moninfo.cbSize = اندازه(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	اگر (mon == پوچ || !GetMonitorInfo(mon, &moninfo))
	    بازگشت;

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
	SetWindowPos(s_hwnd, پوچ,
		moninfo.rcMonitor.چپ,
		moninfo.rcMonitor.بالاترین,
		moninfo.rcMonitor.راست - moninfo.rcMonitor.چپ,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.بالاترین,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    وگرنه
    {
	اگر (!fullscreen_on)
	    بازگشت;

	// Exit fullscreen mode
	SetWindowLongPtr(s_hwnd, GWL_STYLE, normal_style);
	SetWindowLongPtr(s_hwnd, GWL_EXSTYLE, normal_exstyle);

	// Restore original window position and size
	SetWindowPos(s_hwnd, پوچ,
		normal_rect.چپ,
		normal_rect.بالاترین,
		normal_rect.راست - normal_rect.چپ,
		normal_rect.bottom - normal_rect.بالاترین,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    تهی
ex_simalt(exarg_T *eap)
{
    char_u	*کلیدها = eap->arg;
    صحیح		fill_typebuf = FALSE;
    char_u	کلید_name[4];

    PostMessage(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    وقتی (*کلیدها)
    {
	اگر (*کلیدها == '~')
	    *کلیدها = ' ';	    // for showing system menu
	PostMessage(s_hwnd, WM_CHAR, (WPARAM)*کلیدها, (LPARAM)0);
	کلیدها++;
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
#endif
	(تهی)ins_typebuf(کلید_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ifdef MSWIN_FIND_REPLACE
    ایستا تهی
initialise_findrep(char_u *initial_string)
{
    صحیح		wword = FALSE;
    صحیح		mcase = !p_ic;
    char_u	*entry_text;

    // Get the search string to use.
    entry_text = get_find_dialog_text(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_DOWN;
    اگر (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    اگر (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    اگر (entry_text != پوچ && *entry_text != NUL)
    {
	WCHAR *p = enc_to_utf16(entry_text, پوچ);
	اگر (p != پوچ)
	{
	    صحیح len = s_findrep_struct.wFindWhatLen - 1;

	    wcsncpy(s_findrep_struct.lpstrFindWhat, p, len);
	    s_findrep_struct.lpstrFindWhat[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(entry_text);
}
#endif

    ایستا تهی
set_window_title(HWND hwnd, نویسه *title)
{
    اگر (title != پوچ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_to_utf16((char_u *)title, پوچ);
	اگر (wbuf != پوچ)
	{
	    SetWindowTextW(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    وگرنه
	(تهی)SetWindowTextW(hwnd, پوچ);
}

    تهی
gui_mch_find_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    اگر (s_findrep_پیام != 0)
    {
	اگر (IsWindow(s_findrep_hwnd) && !s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	اگر (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = FindTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find string"));
	(تهی)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = TRUE;
    }
#endif
}


    تهی
gui_mch_replace_dialog(exarg_T *eap)
{
#ifdef MSWIN_FIND_REPLACE
    اگر (s_findrep_پیام != 0)
    {
	اگر (IsWindow(s_findrep_hwnd) && s_findrep_is_find)
	    DestroyWindow(s_findrep_hwnd);

	اگر (!IsWindow(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ReplaceTextW(&s_findrep_struct);
	}

	set_window_title(s_findrep_hwnd, _("Find & Replace"));
	(تهی)SetFocus(s_findrep_hwnd);

	s_findrep_is_find = FALSE;
    }
#endif
}


/*
 * Set visibility of the pointer.
 */
    تهی
gui_mch_mousehide(صحیح hide)
{
    اگر (hide == gui.pointer_hidden)
	بازگشت;

    ShowCursor(!hide);
    gui.pointer_hidden = hide;
}

#ifdef FEAT_MENU
    ایستا تهی
gui_mch_show_popupmenu_at(vimmenu_T *menu, صحیح x, صحیح y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (تهی)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(صحیح)0,	    //reserved param
	s_hwnd,
	پوچ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#endif

/*
 * Got a message when the system will go down.
 */
    ایستا تهی
_OnEndSession(تهی)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ایستا تهی
_OnClose(HWND hwnd UNUSED)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ایستا تهی
_OnDestroy(HWND hwnd)
{
    اگر (!destroying)
	_OnClose(hwnd);
}

    ایستا تهی
_OnPaint(
    HWND hwnd)
{
    اگر (IsMinimized(hwnd))
	بازگشت;

    PAINTSTRUCT ps;

    out_flush();	    // make sure all output has been processed
    (تهی)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    اگر (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.چپ = rect.چپ;
	ps.rcPaint.راست = rect.راست;
    }

    اگر (!IsRectEmpty(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	اگر (!screen_cleared || Screen_سطرها == پوچ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	وگرنه
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.چپ, ps.rcPaint.بالاترین,
		ps.rcPaint.راست - ps.rcPaint.چپ + 1,
		ps.rcPaint.bottom - ps.rcPaint.بالاترین + 1);
    }

    EndPaint(hwnd, &ps);
}

    ایستا تهی
_OnSize(
    HWND hwnd,
    UINT state UNUSED,
    صحیح cx,
    صحیح cy)
{
    اگر (!IsMinimized(hwnd) && !s_in_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_height(TRUE);
    }
}

    ایستا تهی
_OnSetFocus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (تهی)DefWindowProcW(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ایستا تهی
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    اگر (destroying)
	بازگشت;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (تهی)DefWindowProcW(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ایستا LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    بازگشت DefWindowProcW(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    تهی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    DestroyWindow(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    تهی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    RECT rct;
    POINT mp;

    (تهی)GetWindowRect(s_textArea, &rct);
    (تهی)GetCursorPos(&mp);
    *x = (صحیح)(mp.x - rct.چپ);
    *y = (صحیح)(mp.y - rct.بالاترین);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    تهی
gui_mch_setmouse(صحیح x, صحیح y)
{
    RECT rct;

    (تهی)GetWindowRect(s_textArea, &rct);
    (تهی)SetCursorPos(x + gui.border_offset + rct.چپ,
		       y + gui.border_offset + rct.بالاترین);
}

    ایستا تهی
gui_mswin_get_valid_dimensions(
    صحیح w,
    صحیح h,
    صحیح *valid_w,
    صحیح *valid_h,
    صحیح *cols,
    صحیح *rows)
{
    صحیح	    base_width, base_height;

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

    تهی
gui_mch_flash(صحیح msec)
{
    RECT    rc;

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.چپ = 0;
    rc.بالاترین = 0;
    rc.راست = gui.رقم_cols * gui.char_width;
    rc.bottom = gui.رقم_rows * gui.char_height;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((long)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ایستا BOOL
is_point_onscreen(صحیح x, صحیح y)
{
    POINT   pt = {x, y};

    بازگشت MonitorFromPoint(pt, MONITOR_DEFAULTTONULL) != پوچ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ایستا BOOL
is_window_onscreen(HWND hwnd)
{
    RECT    rc;
    POINT   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.چپ;
    p1.y = rc.بالاترین;
    p2.x = rc.راست - 1;
    p2.y = rc.bottom - 1;
    ClientToScreen(hwnd, &p1);
    ClientToScreen(hwnd, &p2);

    اگر (!is_point_onscreen(p1.x, p1.y))
	بازگشت FALSE;
    اگر (!is_point_onscreen(p1.x, p2.y))
	بازگشت FALSE;
    اگر (!is_point_onscreen(p2.x, p1.y))
	بازگشت FALSE;
    اگر (!is_point_onscreen(p2.x, p2.y))
	بازگشت FALSE;
    بازگشت TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ایستا صحیح
get_scroll_flags(تهی)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    اگر (!is_window_onscreen(s_hwnd))
	بازگشت SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    GetWindowRect(s_hwnd, &rcVim);
    برای (hwnd = s_hwnd; (hwnd = GetWindow(hwnd, GW_HWNDPREV)) != (HWND)0; )
	اگر (IsWindowVisible(hwnd))
	{
	    GetWindowRect(hwnd, &rcOther);
	    اگر (IntersectRect(&rcDest, &rcVim, &rcOther))
		بازگشت SW_INVALIDATE;
	}
    بازگشت 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ایستا تهی
intel_gpu_workaround(تهی)
{
    GetPixel(s_hdc, FILL_X(gui.col), FILL_Y(gui.row));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    تهی
gui_mch_delete_سطرها(
    صحیح	    row,
    صحیح	    رقم_سطرها)
{
    RECT	rc;

    rc.چپ = FILL_X(gui.scroll_region_چپ);
    rc.راست = FILL_X(gui.scroll_region_راست + 1);
    rc.بالاترین = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -رقم_سطرها * gui.char_height, &rc);
    }
    وگرنه
#endif
    {
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	ScrollWindowEx(s_textArea, 0, -رقم_سطرها * gui.char_height,
				    &rc, &rc, پوچ, پوچ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_block(gui.scroll_region_bot - رقم_سطرها + 1,
						       gui.scroll_region_چپ,
	gui.scroll_region_bot, gui.scroll_region_راست);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    تهی
gui_mch_درج_سطرها(
    صحیح		row,
    صحیح		رقم_سطرها)
{
    RECT	rc;

    rc.چپ = FILL_X(gui.scroll_region_چپ);
    rc.راست = FILL_X(gui.scroll_region_راست + 1);
    rc.بالاترین = FILL_Y(row);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX() && is_window_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, رقم_سطرها * gui.char_height, &rc);
    }
    وگرنه
#endif
    {
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#endif
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	ScrollWindowEx(s_textArea, 0, رقم_سطرها * gui.char_height,
				    &rc, &rc, پوچ, پوچ, get_scroll_flags());
	UpdateWindow(s_textArea);
    }

    gui_clear_block(row, gui.scroll_region_چپ,
				row + رقم_سطرها - 1, gui.scroll_region_راست);
}


    تهی
gui_mch_exit(صحیح rc UNUSED)
{
#اگر defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = پوچ;
#endif

    ReleaseDC(s_textArea, s_hdc);
    DeleteObject(s_brush);

#ifdef FEAT_TEAROFF
    // Unload the tearoff bitmap
    (تهی)DeleteObject((HGDIOBJ)s_htearbitmap);
#endif

    // Destroy our window (if we have one).
    اگر (s_hwnd != پوچ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	DestroyWindow(s_hwnd);
    }
}

    ایستا char_u *
logfont2name(LOGFONTW lf)
{
    نویسه	*charset_name;
    نویسه	*quality_name;
    نویسه	*font_name;
    size_t	res_size;
    نویسه	*res;

    font_name = (نویسه *)utf16_to_enc(lf.lfFaceName, پوچ);
    اگر (font_name == پوچ)
	بازگشت پوچ;
    charset_name = charset_id2name((صحیح)lf.lfCharSet);
    quality_name = quality_id2name((صحیح)lf.lfQuality);

    res_size = STRLEN(font_name) + 30
		    + (charset_name == پوچ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == پوچ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_size);
    اگر (res != پوچ)
    {
	نویسه	*p;
	صحیح	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	برای (p = font_name; *p != NUL; ++p)
	{
	    اگر (isspace(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_to_points(
			 lf.lfHeight < 0 ? -lf.lfHeight : lf.lfHeight, TRUE);
	اگر (lf.lfWeight == FW_NORMAL || lf.lfWeight == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(نویسه *)res, res_size, "%s:h%d", font_name, points);
	وگرنه
	    res_len = vim_snprintf_safelen(
		(نویسه *)res, res_size, "%s:h%d:W%ld", font_name, points, lf.lfWeight);

	res_len += vim_snprintf_safelen(
	    (نویسه *)res + res_len,
	    res_size - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lfWeight == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lfStrikeOut ? ":s" : "");

	اگر (charset_name != پوچ)
	    res_len += vim_snprintf_safelen((نویسه *)res + res_len,
		res_size - res_len, ":c%s", charset_name);
	اگر (quality_name != پوچ)
	    vim_snprintf((نویسه *)res + res_len,
		res_size - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    بازگشت (char_u *)res;
}


#ifdef FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ایستا تهی
update_im_font(تهی)
{
    LOGFONTW	lf_wide, lf;

    اگر (p_guifontwide != پوچ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, اندازه(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    وگرنه
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
    im_set_font(&lf);
}
#endif

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    تهی
gui_mch_wide_font_changed(تهی)
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

    اگر (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, اندازه(lf), &lf))
    {
	اگر (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	اگر (lf.lfWeight < FW_BOLD)
	{
	    lf.lfWeight = FW_BOLD;
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
gui_mch_init_font(char_u *font_name, صحیح fontset UNUSED)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    اگر (get_logfont(&lf, font_name, پوچ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lfHeight = adjust_fontsize_by_dpi(lf.lfHeight);
	font = get_font_handle(&lf);
    }
    اگر (font == NOFONT)
	بازگشت FAIL;

#اگر defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	صحیح		    feat_شمار = 0;
	char_u		    *fp;

	اگر (font_name != پوچ)
	{
	    // Find each ":f" option in font_name.
	    برای (fp = font_name; *fp != NUL; fp++)
	    {
		اگر (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u tag[5];
		    صحیح    ti = 0;
		    unsigned صحیح param = 1;

		    fp += 2;  // skip ":f"
		    وقتی (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			tag[ti++] = *fp++;
		    tag[ti] = NUL;

		    اگر (ti != 4)
			ادامه;  // invalid tag length

		    اگر (*fp == '=')
		    {
			fp++;
			param = (unsigned صحیح)atoi((نویسه *)fp);
			وقتی (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    اگر (feat_شمار < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_شمار].tag =
			    ((unsigned صحیح)tag[0])
			    | ((unsigned صحیح)tag[1] << 8)
			    | ((unsigned صحیح)tag[2] << 16)
			    | ((unsigned صحیح)tag[3] << 24);
			features[feat_شمار].parameter = param;
			feat_شمار++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWriteContext_SetFontFeatures(s_dwc, features, feat_شمار);
    }
#endif

    اگر (font_name == پوچ)
	font_name = (char_u *)"";
#ifdef FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    اگر (!s_in_dpichanged)
	update_im_font();
#endif
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_height = lfOrig.lfHeight;
    UpdateFontSize(font);

    p = logfont2name(lfOrig);
    اگر (p != پوچ)
    {
	hl_set_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	اگر (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	وگرنه
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
    اگر (lf.lfWeight < FW_BOLD)
    {
	lf.lfWeight = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	اگر (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    بازگشت OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    صحیح
gui_mch_maximized(تهی)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.length = اندازه(WINDOWPLACEMENT);
    اگر (GetWindowPlacement(s_hwnd, &wp))
    {
	اگر (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    بازگشت TRUE;
	اگر (wp.showCmd == SW_SHOWMINIMIZED)
	    بازگشت FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	GetWindowRect(s_hwnd, &rc);
	اگر ((rc.راست - rc.چپ !=
		    wp.rcNormalPosition.راست - wp.rcNormalPosition.چپ)
		|| (rc.bottom - rc.بالاترین !=
		    wp.rcNormalPosition.bottom - wp.rcNormalPosition.بالاترین))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    تهی
gui_mch_newfont(تهی)
{
    RECT	rect;

    GetWindowRect(s_hwnd, &rect);
    اگر (win_socket_id == 0)
    {
	gui_resize_shell(rect.راست - rect.چپ
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.بالاترین
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_height(FALSE));
    }
    وگرنه
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.راست - rect.چپ,
	    rect.bottom - rect.بالاترین - gui_mswin_get_menu_height(FALSE));
    }
}

/*
 * Set the window title
 */
    تهی
gui_mch_settitle(
    char_u  *title,
    char_u  *icon UNUSED)
{
    set_window_title(s_hwnd, (title == پوچ ? "VIM" : (نویسه *)title));
}

#اگر defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ایستا LPCSTR mshape_idcs[] =
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

    تهی
mch_set_mouse_shape(صحیح shape)
{
    LPCSTR idc;

    اگر (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    وگرنه
    {
	اگر (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	وگرنه
	    idc = mshape_idcs[shape];
	SetClassLongPtr(s_textArea, GCLP_HCURSOR, (LONG_PTR)LoadCursor(پوچ, idc));
	اگر (!p_mh)
	{
	    POINT mp;

	    // Set the position to make it redrawn with the new shape.
	    (تهی)GetCursorPos(&mp);
	    (تهی)SetCursorPos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#endif

#اگر defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ایستا WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    صحیح len;
    WCHAR *res;

    tmp = convert_filter(s);
    اگر (tmp == پوچ)
	بازگشت پوچ;
    len = (صحیح)STRLEN(s) + 3;
    res = enc_to_utf16(tmp, &len);
    vim_free(tmp);
    بازگشت res;
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
    WCHAR		*titlep = پوچ;
    WCHAR		*extp = پوچ;
    WCHAR		*initdirp = پوچ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    اگر (dflt == پوچ)
	fileBuf[0] = NUL;
    وگرنه
    {
	wp = enc_to_utf16(dflt, پوچ);
	اگر (wp == پوچ)
	    fileBuf[0] = NUL;
	وگرنه
	{
	    برای (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
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
# وگرنه
    fileStruct.lStructSize = اندازه(fileStruct);
# endif

    اگر (title != پوچ)
	titlep = enc_to_utf16(title, پوچ);
    fileStruct.lpstrTitle = titlep;

    اگر (ext != پوچ)
	extp = enc_to_utf16(ext, پوچ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    اگر (initdir != پوچ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_to_utf16(initdir, پوچ);
	اگر (initdirp != پوچ)
	{
	    برای (wp = initdirp; *wp != NUL; ++wp)
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
    وگرنه
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    اگر (!ret)
	بازگشت پوچ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_to_enc(fileBuf, پوچ);
    اگر (p == پوچ)
	بازگشت پوچ;

    // Give focus back to main window (when using MDI).
    SetFocus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    بازگشت q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ایستا char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    unsigned	s_len = (unsigned)STRLEN(s);
    unsigned	i;

    res = alloc(s_len + 3);
    اگر (res != پوچ)
    {
	برای (i = 0; i < s_len; ++i)
	    اگر (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    وگرنه
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    بازگشت res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    بازگشت gui_mch_browse(0, title, (char_u *)_("Not Used"), پوچ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#endif // FEAT_BROWSE

    ایستا تهی
_OnDropFiles(
    HWND hwnd UNUSED,
    HDROP hDrop)
{
#define BUFPATHLEN _MAX_PATH
#define DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    نویسه    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, پوچ, 0);
    UINT    i;
    char_u  **fnames;
    POINT   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    DragQueryPoint(hDrop, &pt);
    MapWindowPoints(s_hwnd, s_textArea, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    اگر (fnames != پوچ)
	برای (i = 0; i < cFiles; ++i)
	{
	    اگر (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_to_enc(wszFile, پوچ);
	    وگرنه
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    DragFinish(hDrop);

    اگر (fnames == پوچ)
	بازگشت;

    صحیح kbd_modifiers = get_active_modifiers();

    اگر ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    اگر ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    اگر ((kbd_modifiers & MOD_MASK_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ایستا صحیح
_OnScroll(
    HWND hwnd UNUSED,
    HWND hwndCtl,
    UINT code,
    صحیح pos)
{
    ایستا UINT	prev_code = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    long	val;
    صحیح		dragging = FALSE;
    صحیح		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cbSize = اندازه(si);
    si.fMask = SIF_POS;

    sb = gui_mswin_find_scrollbar(hwndCtl);
    اگر (sb == پوچ)
	بازگشت 0;

    اگر (sb->wp != پوچ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    وگرنه	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->مقدار;

    انتخاب (code)
    {
	حالت SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    اگر (sb->scroll_shift > 0)
		val <<= sb->scroll_shift;
	    بشکن;
	حالت SB_LINEDOWN:
	    val++;
	    بشکن;
	حالت SB_LINEUP:
	    val--;
	    بشکن;
	حالت SB_PAGEDOWN:
	    val += (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    بشکن;
	حالت SB_PAGEUP:
	    val -= (sb_info->size > 2 ? sb_info->size - 2 : 1);
	    بشکن;
	حالت SB_بالاترین:
	    val = 0;
	    بشکن;
	حالت SB_BOTTOM:
	    val = sb_info->max;
	    بشکن;
	حالت SB_ENDSCROLL:
	    اگر (prev_code == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		اگر (sb->scroll_shift > 0)
		    val <<= sb->scroll_shift;
	    }
	    بشکن;

	پیشفرض:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    بازگشت 0;
    }
    prev_code = code;

    si.nPos = (sb->scroll_shift > 0) ? val >> sb->scroll_shift : val;
    SetScrollInfo(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    اگر (sb->wp != پوچ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_چپ) ? SBAR_راست : SBAR_چپ].id;

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

    بازگشت 0;
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

ایستا BalloonEval  *cur_beval = پوچ;
ایستا UINT_PTR	    beval_timer_id = 0;
ایستا DWORD	    last_user_activity = 0;
#endif // defined(FEAT_BEVAL_GUI)


// Local variables:

#ifdef FEAT_MENU
ایستا UINT	s_menu_id = 100;
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
ایستا PWORD
add_dialog_element(
	PWORD p,
	DWORD lStyle,
	WORD x,
	WORD y,
	WORD w,
	WORD h,
	WORD Id,
	WORD clss,
	ثابت نویسه *caption);
ایستا LPWORD lpwAlign(LPWORD);
ایستا صحیح nCopyAnsiToWideChar(LPWORD, LPSTR, BOOL);
#اگر defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ایستا تهی gui_mch_tearoff(char_u *title, vimmenu_T *menu, صحیح initX, صحیح initY);
#endif
ایستا تهی get_dialog_font_metrics(تهی);

ایستا صحیح dialog_default_button = -1;

#ifdef FEAT_TOOLBAR
ایستا تهی initialise_toolbar(تهی);
ایستا تهی update_toolbar_size(تهی);
ایستا LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_پیام, WPARAM wParam, LPARAM lParam);
ایستا صحیح get_toolbar_bitmap(vimmenu_T *menu);
#وگرنه
# define update_toolbar_size()
#endif

#ifdef FEAT_GUI_TABLINE
ایستا تهی initialise_tabline(تهی);
ایستا LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_پیام, WPARAM wParam, LPARAM lParam);
#endif

#ifdef FEAT_MBYTE_IME
ایستا LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ایستا char_u *GetResultStr(HWND hwnd, صحیح GCS, صحیح *lenp);
#endif
#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
تعریفنوع ساختار tagCOMPOSITIONFORM {
    DWORD dwStyle;
    POINT ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
تعریفنوع HANDLE HIMC;
# endif

ایستا HINSTANCE hLibImm = پوچ;
ایستا LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ایستا HIMC (WINAPI *pImmGetContext)(HWND);
ایستا HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ایستا BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ایستا BOOL (WINAPI *pImmGetOpenStatus)(HIMC);
ایستا BOOL (WINAPI *pImmSetOpenStatus)(HIMC, BOOL);
ایستا BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ایستا BOOL (WINAPI *pImmSetCompositionFontW)(HIMC, LPLOGFONTW);
ایستا BOOL (WINAPI *pImmSetCompositionWindow)(HIMC, LPCOMPOSITIONFORM);
ایستا BOOL (WINAPI *pImmGetConversionStatus)(HIMC, LPDWORD, LPDWORD);
ایستا BOOL (WINAPI *pImmSetConversionStatus)(HIMC, DWORD, DWORD);
ایستا تهی dyn_imm_load(تهی);
#وگرنه
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
    ایستا صحیح
gui_mswin_get_menu_height(
    صحیح	    fix_window)	    // If TRUE, resize window if menu height changed
{
    ایستا صحیح	old_menu_height = -1;

    RECT    rc1, rc2;
    صحیح	    رقم;
    صحیح	    menu_height;

    اگر (gui.menu_is_active)
	رقم = Get_Menu_Item_شمار(s_menuBar);
    وگرنه
	رقم = 0;

    اگر (رقم == 0)
	menu_height = 0;
    وگرنه اگر (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_height = old_menu_height == -1 ? 0 : old_menu_height;
    }
    وگرنه
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	GetMenuItemRect(s_hwnd, s_menuBar, 0, &rc1);
	اگر (gui.starting)
	    menu_height = rc1.bottom - rc1.بالاترین + 1;
	وگرنه
	{
	    GetMenuItemRect(s_hwnd, s_menuBar, رقم - 1, &rc2);
	    menu_height = rc2.bottom - rc1.بالاترین + 1;
	}
    }

    اگر (fix_window && menu_height != old_menu_height)
	gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_height = menu_height;

    بازگشت menu_height;
}
#endif // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ایستا long
mouse_vertical_scroll_step(تهی)
{
    UINT val;
    اگر (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	بازگشت (val != WHEEL_PAGESCROLL) ? (long)val : -1;
    بازگشت 3; // Safe default;
}

    ایستا long
mouse_horizontal_scroll_step(تهی)
{
    UINT val;
    اگر (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	بازگشت (long)val;
    بازگشت 3; // Safe default;
}

    ایستا تهی
init_mouse_wheel(تهی)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
}

/*
 * Mouse scroll event handler.
 */
    ایستا تهی
_OnMouseWheel(HWND hwnd UNUSED, WPARAM wParam, LPARAM lParam, صحیح horizontal)
{
    صحیح		button;
    win_T	*wp;
    صحیح		modifiers = 0;
    صحیح		kbd_modifiers;
    صحیح		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    POINT	pt;

    wp = gui_mouse_window(FIND_POPUP);

#ifdef FEAT_PROP_POPUP
    اگر (wp != پوچ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_row = wp->w_winrow;
	mouse_col = wp->w_wincol;
	CLEAR_FIELD(cap);
	اگر (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_چپ : MSCR_راست;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	وگرنه
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

    اگر (wp == پوچ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    اگر (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    وگرنه
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_active_modifiers();

    اگر ((kbd_modifiers & MOD_MASK_SHIFT) != 0)
	modifiers |= MOUSE_SHIFT;
    اگر ((kbd_modifiers & MOD_MASK_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    اگر ((kbd_modifiers & MOD_MASK_ALT) != 0)
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
    ایستا صحیح
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cbSize = اندازه(NONCLIENTMETRICSW);
    اگر (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    اندازه(NONCLIENTMETRICSW),
	    &nm,
	    0))
	بازگشت FAIL;
    *lf = nm.lfMenuFont;
    بازگشت OK;
}
#endif


#اگر defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ایستا تهی
set_tabline_font(تهی)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    اگر (gui_w32_get_menu_font(&lfSysmenu) != OK)
	بازگشت;

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
#وگرنه
# define set_tabline_font()
#endif

/*
 * Invoked when a setting was changed.
 */
    ایستا LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    انتخاب (param)
    {
	حالت SPI_SETWHEELSCROLLLINES:
	    mouse_set_vert_scroll_step(mouse_vertical_scroll_step());
	    بشکن;
	حالت SPI_SETWHEELSCROLLCHARS:
	    mouse_set_hor_scroll_step(mouse_horizontal_scroll_step());
	    بشکن;
	حالت SPI_SETNONCLIENTMETRICS:
	    set_tabline_font();
	    بشکن;
	پیشفرض:
	    بشکن;
    }
    بازگشت 0;
}

#ifdef FEAT_NETBEANS_INTG
    ایستا تهی
_OnWindowPosChanged(
    HWND hwnd,
    ثابت LPWINDOWPOS lpwpos)
{
    ایستا صحیح x = 0, y = 0, cx = 0, cy = 0;
    بیرونی صحیح WSInitialized;

    اگر (WSInitialized && (lpwpos->x != x || lpwpos->y != y
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


ایستا HWND hwndTip = پوچ;

    ایستا تهی
show_sizing_tip(صحیح cols, صحیح rows)
{
    TOOLINFOA ti;
    نویسه buf[32];

    ti.cbSize = اندازه(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpszText = buf;
    sprintf(buf, "%dx%d", cols, rows);
    اگر (hwndTip == پوچ)
    {
	hwndTip = CreateWindowExA(0, TOOLTIPS_CLASSA, پوچ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, پوچ, GetModuleHandle(پوچ), پوچ);
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	SendMessage(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    وگرنه
    {
	SendMessage(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    SendMessage(hwndTip, TTM_POPUP, 0, 0);
}

    ایستا تهی
destroy_sizing_tip(تهی)
{
    اگر (hwndTip == پوچ)
	بازگشت;

    DestroyWindow(hwndTip);
    hwndTip = پوچ;
}

    ایستا صحیح
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    صحیح	    w, h;
    صحیح	    valid_w, valid_h;
    صحیح	    w_offset, h_offset;
    صحیح	    cols, rows;

    w = lprc->راست - lprc->چپ;
    h = lprc->bottom - lprc->بالاترین;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &cols, &rows);
    w_offset = w - valid_w;
    h_offset = h - valid_h;

    اگر (fwSide == WMSZ_چپ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->چپ += w_offset;
    وگرنه اگر (fwSide == WMSZ_راست || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->راست -= w_offset;

    اگر (fwSide == WMSZ_بالاترین || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->بالاترین += h_offset;
    وگرنه اگر (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_offset;

    show_sizing_tip(cols, rows);
    بازگشت TRUE;
}

#ifdef FEAT_GUI_TABLINE
    ایستا تهی
_OnRButtonUp(HWND hwnd, صحیح x, صحیح y, UINT کلید_Flags)
{
    اگر (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	اگر (pt.y < rect.بالاترین)
	{
	    show_tabline_popup_menu();
	    بازگشت;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, کلید_Flags, DefWindowProcW);
}

    ایستا تهی
_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, صحیح x, صحیح y, UINT کلید_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    اگر (gui_mch_showing_tabline())
    {
	POINT pt;
	RECT rect;

	GetCursorPos(&pt);
	GetWindowRect(s_textArea, &rect);
	اگر (pt.y < rect.بالاترین)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, کلید_Flags, DefWindowProcW);
}
#endif

    ایستا UINT
_OnNCHitTest(HWND hwnd, صحیح xPos, صحیح yPos)
{
    UINT	result;
    صحیح		x, y;

    result = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, DefWindowProcW);
    اگر (result != HTCLIENT)
	بازگشت result;

#ifdef FEAT_GUI_TABLINE
    اگر (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	GetWindowRect(s_textArea, &rct);
	اگر (yPos < rct.بالاترین)
	    بازگشت result;
    }
#endif
    (تهی)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    اگر (xPos < 48) // <VN> TODO should use system metric?
	بازگشت HTBOTTOMLEFT;
    وگرنه
	بازگشت HTBOTTOMRIGHT;
}

#اگر defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ایستا LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    انتخاب (hdr->code)
    {
	حالت TTN_GETDISPINFOW:
	حالت TTN_GETDISPINFO:
	    {
		char_u		*str = پوچ;
		ایستا تهی	*tt_text = پوچ;

		VIM_CLEAR(tt_text);

# ifdef FEAT_GUI_TABLINE
		اگر (gui_mch_showing_tabline()
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
		    اگر (ScreenToClient(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			صحیح idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
			اگر (idx != -1)
			{
			    tabpage_T *tp;

			    tp = find_tabpage(idx + 1);
			    اگر (tp != پوچ)
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
		وگرنه
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_find_menu(ریشه_menu, idButton);
		    اگر (pMenu)
			str = pMenu->strings[MENU_INDEX_TIP];
		}
# endif
		اگر (str == پوچ)
		    بشکن;

		// Set the maximum width, this also enables using \n for
		// line break.
		SendMessage(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		اگر (hdr->code == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_text = enc_to_utf16(str, پوچ);
		    lpdi->lpszText = tt_text;
		    // can't show tooltip if failed
		}
		وگرنه
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    اگر (len < اندازه(lpdi->szText)
			    || ((tt_text = vim_strnsave(str, len)) == پوچ))
			vim_strncpy((char_u *)lpdi->szText, str,
				اندازه(lpdi->szText) - 1);
		    وگرنه
			lpdi->lpszText = tt_text;
		}
	    }
	    بشکن;

# ifdef FEAT_GUI_TABLINE
	حالت TCN_SELCHANGE:
	    اگر (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		بازگشت 0L;
	    }
	    بشکن;

	حالت NM_RCLICK:
	    اگر (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		بازگشت 0L;
	    }
	    بشکن;
# endif

	پیشفرض:
	    بشکن;
    }
    بازگشت DefWindowProcW(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#endif

#اگر defined(MENUHINTS) && defined(FEAT_MENU)
    ایستا LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    اگر (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & MODE_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ایستا صحیح  did_menu_tip = FALSE;

	اگر (did_menu_tip)
	{
	    پیام_clr_cmdline();
	    setcursor();
	    out_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_find_menu(ریشه_menu, (UINT)LOWORD(wParam));
	اگر (pMenu != پوچ && pMenu->strings[MENU_INDEX_TIP] != پوچ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cbSize = اندازه(MENUITEMINFO);
	    menuinfo.fMask = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    اگر (GetMenuItemInfo(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++پیام_hist_off;
		پیام((نویسه *)pMenu->strings[MENU_INDEX_TIP]);
		--پیام_hist_off;
		setcursor();
		out_flush();
		did_menu_tip = TRUE;
	    }
	}
	بازگشت 0L;
    }
    بازگشت DefWindowProcW(hwnd, WM_MENUSELECT, wParam, lParam);
}
#endif

    ایستا BOOL
_OnGetDpiScaledSize(HWND hwnd UNUSED, UINT dpi, SIZE *size)
{
    صحیح		old_width, old_height;
    صحیح		new_width, new_height;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    GetFontSize(gui.norm_font, &old_width, &old_height);    // Current size
    GetObjectW((HFONT)gui.norm_font, اندازه(lf), &lf);
    lf.lfHeight = lf.lfHeight * (صحیح)dpi / s_dpi;
    font = CreateFontIndirectW(&lf);
    اگر (font)
    {
	GetFontSize((GuiFont)font, &new_width, &new_height);	// New size
	DeleteFont(font);
    }
    وگرنه
    {
	new_width = old_width;
	new_height = old_height;
    }
    size->cx = size->cx * new_width / old_width;
    size->cy = size->cy * new_height / old_height;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    بازگشت TRUE;
}

    ایستا LRESULT
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
    InvalidateRect(hwnd, پوچ, TRUE);

    s_in_dpichanged = FALSE;
    بازگشت 0L;
}


    ایستا LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_پیام,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_پیام, lParam);

    s_u_پیام = u_پیام;
    s_wParam = wParam;

    انتخاب (u_پیام)
    {
	HANDLE_پیام(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_پیام(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_پیام(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_پیام(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_پیام(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_پیام(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_پیام(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ifdef FEAT_MENU
	HANDLE_پیام(hwnd, WM_COMMAND,	_OnMenu);
#endif
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_پیام(hwnd, WM_SETFOCUS,	_OnSetFocus);
	HANDLE_پیام(hwnd, WM_SIZE,	_OnSize);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_پیام(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_پیام(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ifdef FEAT_NETBEANS_INTG
	HANDLE_پیام(hwnd, WM_WINDOWPOSCHANGED, _OnWindowPosChanged);
#endif
#ifdef FEAT_GUI_TABLINE
	HANDLE_پیام(hwnd, WM_RBUTTONUP,	_OnRButtonUp);
	HANDLE_پیام(hwnd, WM_LBUTTONDBLCLK,  _OnLButtonDown);
#endif
	HANDLE_پیام(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    حالت WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	بازگشت FALSE;		// Do NOT allow system to go down.

    حالت WM_ENDSESSION:
	اگر (wParam)	// system only really goes down when wParam is TRUE
	{
	    _OnEndSession();
	    بازگشت 0L;
	}
	بشکن;

    حالت WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (صحیح)(short)LOWORD(lParam));
	بازگشت 0L;

    حالت WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ifdef FEAT_MENU
	اگر (	!gui.menu_is_active
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((صحیح)wParam))
		)
#endif
	{
	    _OnSysChar(hwnd, (UINT)wParam, (صحیح)(short)LOWORD(lParam));
	    بازگشت 0L;
	}
#ifdef FEAT_MENU
	وگرنه
	    بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
#endif

    حالت WM_SYSKEYUP:
#ifdef FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
#وگرنه
	بازگشت 0L;
#endif

    حالت WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	بشکن;

    حالت WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	بازگشت _DuringSizing((UINT)wParam, (LPRECT)lParam);

    حالت WM_MOUSEWHEEL:
    حالت WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_پیام == WM_MOUSEHWHEEL);
	بازگشت 0L;

	// Notification for change in SystemParametersInfo()
    حالت WM_SETTINGCHANGE:
	بازگشت _OnSettingChange((UINT)wParam);

#اگر defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    حالت WM_NOTIFY:
	بازگشت _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#endif

#اگر defined(MENUHINTS) && defined(FEAT_MENU)
    حالت WM_MENUSELECT:
	بازگشت _OnMenuSelect(hwnd, wParam, lParam);
#endif

#ifdef FEAT_MBYTE_IME
    حالت WM_IME_NOTIFY:
	اگر (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
	بازگشت 1L;

    حالت WM_IME_COMPOSITION:
	اگر (!_OnImeComposition(hwnd, wParam, lParam))
	    بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
	بازگشت 1L;
#endif
    حالت WM_GETDPISCALEDSIZE:
	بازگشت _OnGetDpiScaledSize(hwnd, (UINT)wParam, (SIZE *)lParam);
    حالت WM_DPICHANGED:
	بازگشت _OnDpiChanged(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    پیشفرض:
#ifdef MSWIN_FIND_REPLACE
	اگر (u_پیام == s_findrep_پیام && s_findrep_پیام != 0)
	    _OnFindRepl();
#endif
	بشکن;
    }

    بازگشت DefWindowProcW(hwnd, u_پیام, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_parent_hwnd = پوچ;

    ایستا BOOL CALLBACK
FindWindowTitle(HWND hwnd, LPARAM lParam)
{
    نویسه	buf[2048];
    نویسه	*title = (نویسه *)lParam;

    اگر (GetWindowText(hwnd, buf, اندازه(buf)))
    {
	اگر (strstr(buf, title) != پوچ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_parent_hwnd = FindWindowEx(hwnd, پوچ, "MDIClient", پوچ);
	    اگر (vim_parent_hwnd != پوچ)
		بازگشت FALSE;
	}
    }
    بازگشت TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    تهی
gui_mch_set_parent(نویسه *title)
{
    EnumWindows(FindWindowTitle, (LPARAM)title);
    اگر (vim_parent_hwnd == پوچ)
    {
	semsg(_(e_cannot_find_window_title_str), title);
	mch_exit(2);
    }
}

#ifndef FEAT_OLE
    ایستا تهی
ole_error(نویسه *arg)
{
    نویسه buf[IOSIZE];

# ifdef VIMDLL
    gui.in_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_argument_not_supported_str_use_ole_version), arg);
    mch_errmsg(buf);
}
#endif

#اگر defined(GUI_MAY_SPAWN)
    ایستا نویسه *
gvim_error(تهی)
{
    نویسه *پیام = _(e_gui_cannot_be_used_cannot_execute_gvim_exe);

    اگر (starting)
    {
	mch_errmsg(پیام);
	mch_errmsg("\n");
	mch_exit(2);
    }
    بازگشت پیام;
}

    نویسه *
gui_mch_do_spawn(char_u *arg)
{
    صحیح			len;
# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = پوچ;
    LPWSTR		tofree1 = پوچ;
# endif
    WCHAR		name[MAX_PATH];
    LPWSTR		cmd, newcmd = پوچ, p, warg, tofree2 = پوچ;
    STARTUPINFOW	si = {اندازه(si)};
    PROCESS_INFORMATION pi;

    اگر (!GetModuleFileNameW(g_hinst, name, MAX_PATH))
	برو error;
    p = wcsrchr(name, L'\\');
    اگر (p == پوچ)
	برو error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# وگرنه
    wcscpy(p + 1, L"gvim.exe");
# endif

# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    اگر (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_سطر_W();
	// Skip 1st argument.
	وقتی (*p && *p != L' ' && *p != L'\t')
	{
	    اگر (*p == L'"')
	    {
		// Skip quoted strings
		وقتی (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# اگر defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    وگرنه
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	صحیح	ret;

	session = vim_tempname('s', FALSE);
	اگر (session == پوچ)
	    برو error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	اگر (p_bg == پوچ)
	{
	    p_bg = savebg;
	    برو error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	اگر (!ret)
	    برو error;
	wsession = enc_to_utf16(session, پوچ);
	اگر (wsession == پوچ)
	    برو error;
	len = (صحیح)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	اگر (cmd == پوچ)
	{
	    vim_free(wsession);
	    برو error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    اگر (arg != پوچ)
    {
	warg = enc_to_utf16(arg, پوچ);
	اگر (warg == پوچ)
	    برو error;
	tofree2 = warg;
    }
    وگرنه
	warg = L"";

    // Set up the new command line.
    len = (صحیح)wcslen(name) + (صحیح)wcslen(cmd) + (صحیح)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    اگر (newcmd == پوچ)
	برو error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    اگر (!CreateProcessW(پوچ, newcmd, پوچ, پوچ, TRUE, 0,
		پوچ, پوچ, &si, &pi))
	برو error;
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
    بازگشت gvim_error();
}
#endif

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    تهی
gui_mch_prepare(صحیح *argc, نویسه **argv)
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
	وگرنه
	    idx = 1;

	// Register Vim as an OLE Automation server
	اگر (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ifdef FEAT_OLE
	    RegisterMe(silent);
	    mch_exit(0);
#وگرنه
	    اگر (!silent)
		ole_error("register");
	    mch_exit(2);
#endif
	}

	// Unregister Vim as an OLE Automation server
	اگر (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ifdef FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#وگرنه
	    اگر (!silent)
		ole_error("unregister");
	    mch_exit(2);
#endif
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	اگر (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ifdef FEAT_OLE
	    *argc = 1;
#وگرنه
	    ole_error("embedding");
	    mch_exit(2);
#endif
	}
    }

#ifdef FEAT_OLE
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
#endif

#ifdef FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	صحیح arg;

	برای (arg = 1; arg < *argc; arg++)
	    اگر (strncmp("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * اندازه(نویسه *));
		argv[*argc] = پوچ;
		بشکن;	// enough?
	    }
    }
#endif
}

    ایستا تهی
load_dpi_func(تهی)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    اگر (hUser32 == پوچ)
	برو fail;

    pGetDpiForSystem = (UINT (WINAPI *)(تهی))GetProcAddress(hUser32, "GetDpiForSystem");
    pGetDpiForWindow = (UINT (WINAPI *)(HWND))GetProcAddress(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (صحیح (WINAPI *)(صحیح, UINT))GetProcAddress(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    pSetThreadDpiAwarenessContext = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "SetThreadDpiAwarenessContext");
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))GetProcAddress(hUser32, "GetAwarenessFromDpiAwarenessContext");
#endif

    اگر (pSetThreadDpiAwarenessContext != پوچ)
    {
	DPI_AWARENESS_CONTEXT oldctx = pSetThreadDpiAwarenessContext(
		DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
	اگر (oldctx != پوچ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ifdef FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#endif
	    بازگشت;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    pGetDpiForWindow = پوچ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    pSetThreadDpiAwarenessContext = پوچ;
#ifdef FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = پوچ;
#endif
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    صحیح
gui_mch_init(تهی)
{
    ثابت WCHAR szVimWndClassW[] = VIM_CLASSW;
    ثابت WCHAR szTextAreaClassW[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    اگر (s_hwnd != پوچ)
	برو theend;

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
    اگر (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = LoadIcon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = LoadCursor(پوچ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = پوچ;
	wndclassw.lpszClassName = szVimWndClassW;

	اگر (RegisterClassW(&wndclassw) == 0)
	    بازگشت FAIL;
    }

    اگر (vim_parent_hwnd != پوچ)
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
		vim_parent_hwnd, پوچ,
		g_hinst, پوچ);
#ifdef HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#endif
	اگر (s_hwnd == پوچ)
	{
	    emsg(_(e_unable_to_open_window_inside_mdi_application));
	    mch_exit(2);
	}
    }
    وگرنه
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	اگر (IsWindow((HWND)win_socket_id) <= 0)
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
		پوچ, پوچ,
		g_hinst, پوچ);
	اگر (s_hwnd != پوچ && win_socket_id != 0)
	{
	    SetParent(s_hwnd, (HWND)win_socket_id);
	    ShowWindow(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    اگر (s_hwnd == پوچ)
	بازگشت FAIL;

    اگر (pGetDpiForWindow != پوچ)
    {
	s_dpi = pGetDpiForWindow(s_hwnd);
	update_scrollbar_size();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_load();
#endif

    // Create the text area window
    اگر (GetClassInfoW(g_hinst, szTextAreaClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = _TextAreaWndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = پوچ;
	wndclassw.hCursor = LoadCursor(پوچ, IDC_ARROW);
	wndclassw.hbrBackground = پوچ;
	wndclassw.lpszMenuName = پوچ;
	wndclassw.lpszClassName = szTextAreaClassW;

	اگر (RegisterClassW(&wndclassw) == 0)
	    بازگشت FAIL;
    }

    s_textArea = CreateWindowExW(
	0,
	szTextAreaClassW, L"Vim text area",
	WS_CHILD | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, پوچ,
	g_hinst, پوچ);

    اگر (s_textArea == پوچ)
	بازگشت FAIL;

#ifdef FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = پوچ;

	اگر (mch_icon_load(&hIcon) == OK && hIcon != پوچ)
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
    s_findrep_پیام = RegisterWindowMessage(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.lStructSize = اندازه(s_findrep_struct);
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
    اگر (p_rop)
	(تهی)gui_mch_set_rendering_options(p_rop);
#endif

theend:
    // Display any pending error messages
    display_errors();

    بازگشت OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ایستا تهی
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = MonitorFromWindow(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    اگر (mon != پوچ)
    {
	moninfo.cbSize = اندازه(MONITORINFO);
	اگر (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    بازگشت;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    تهی
gui_mch_set_shellsize(
	صحیح width,
	صحیح height,
	صحیح min_width UNUSED,
	صحیح min_height UNUSED,
	صحیح base_width UNUSED,
	صحیح base_height UNUSED,
	صحیح direction)
{
    RECT	workarea_rect;
    RECT	window_rect;
    صحیح		win_width, win_height;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    اگر (IsZoomed(s_hwnd) && starting == 0)
	ShowWindow(s_hwnd, SW_SHOWNORMAL);

    اگر (s_in_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	window_rect = s_suggested_rect;
    وگرنه
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
    window_rect.راست = window_rect.چپ + win_width;
    window_rect.bottom = window_rect.بالاترین + win_height;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    اگر (!s_in_dpichanged)
    {
	اگر ((direction & RESIZE_HOR)
		&& window_rect.راست > workarea_rect.راست)
	    OffsetRect(&window_rect,
		    workarea_rect.راست - window_rect.راست, 0);

	اگر ((direction & RESIZE_HOR)
		&& window_rect.چپ < workarea_rect.چپ)
	    OffsetRect(&window_rect,
		    workarea_rect.چپ - window_rect.چپ, 0);

	اگر ((direction & RESIZE_VERT)
		&& window_rect.bottom > workarea_rect.bottom)
	    OffsetRect(&window_rect,
		    0, workarea_rect.bottom - window_rect.bottom);

	اگر ((direction & RESIZE_VERT)
		&& window_rect.بالاترین < workarea_rect.بالاترین)
	    OffsetRect(&window_rect,
		    0, workarea_rect.بالاترین - window_rect.بالاترین);
    }

    MoveWindow(s_hwnd, window_rect.چپ, window_rect.بالاترین,
						win_width, win_height, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    SetActiveWindow(s_hwnd);
    SetFocus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_height(!gui.starting);
}


    تهی
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    SCROLLINFO	info;

    sb->scroll_shift = 0;
    وقتی (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	size >>= 1;
	++sb->scroll_shift;
    }

    اگر (sb->scroll_shift > 0)
	++size;

    info.cbSize = اندازه(info);
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
    تهی
gui_mch_set_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    تهی
gui_mch_set_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    تهی
gui_mch_set_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    تهی
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
    ایستا LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dwData UNUSED)
{
    LRESULT lResult = 0;
    HIMC hImc;

    اگر (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	بازگشت lResult;
    انتخاب (dwCommand)
    {
	حالت IMN_SETOPENSTATUS:
	    اگر (pImmGetOpenStatus(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lfHeight = lf.lfHeight * DEFAULT_DPI / s_dpi;
		pImmSetCompositionFontW(hImc, &lf);
		im_set_position(gui.row, gui.col);

		// Disable langmap
		State &= ~MODE_LANGMAP;
		اگر (State & MODE_درج)
		{
# اگر defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    اگر (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			صحیح old_row = gui.row;
			صحیح old_col = gui.col;

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
	    بشکن;
    }
    pImmReleaseContext(hWnd, hImc);
    بازگشت lResult;
}

    ایستا LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs UNUSED, LPARAM param)
{
    char_u	*ret;
    صحیح		len;

    اگر ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	بازگشت 0;

    ret = GetResultStr(hwnd, GCS_RESULTSTR, &len);
    اگر (ret != پوچ)
    {
	add_to_درون_داد_buf_csi(ret, len);
	vim_free(ret);
	بازگشت 1;
    }
    بازگشت 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ایستا char_u *
GetResultStr(HWND hwnd, صحیح GCS, صحیح *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = پوچ;
    char_u	*convbuf = پوچ;

    اگر (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	بازگشت پوچ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, پوچ, 0);
    اگر (ret <= 0)
	بازگشت پوچ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + اندازه(WCHAR));
    اگر (buf == پوچ)
	بازگشت پوچ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / اندازه(WCHAR);

    convbuf = utf16_to_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    بازگشت convbuf;
}
#endif

// For global functions we need prototypes.
#اگر defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    تهی
im_set_font(LOGFONTW *lf)
{
    HIMC hImc;

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	pImmSetCompositionFontW(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    تهی
im_set_position(صحیح row, صحیح col)
{
    HIMC hImc;

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_POINT;
	cfs.ptCurrentPos.x = FILL_X(col);
	cfs.ptCurrentPos.y = FILL_Y(row);
	MapWindowPoints(s_textArea, s_hwnd, &cfs.ptCurrentPos, 1);
	اگر (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
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
    تهی
im_set_active(صحیح active)
{
    HIMC	hImc;
    ایستا HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    اگر (!gui.in_use && !gui.starting)
    {
	mbyte_im_set_active(active);
	بازگشت;
    }
# endif

    اگر (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	بازگشت;

    اگر (p_imdisable)
    {
	اگر (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    اگر (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	active = FALSE;
    }
    وگرنه اگر (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    اگر (!hImc)
	بازگشت;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    اگر (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ایستا DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ایستا BOOL bSaved = FALSE;

	اگر (active)
	{
	    // if we have a saved conversion status, restore it
	    اگر (bSaved)
		pImmSetConversionStatus(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	وگرنه
	{
	    // save conversion status and disable korean
	    اگر (pImmGetConversionStatus(hImc, &dwConversionSaved,
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
    صحیح
im_get_status(تهی)
{
    صحیح		status = 0;
    HIMC	hImc;

# ifdef VIMDLL
    اگر (!gui.in_use && !gui.starting)
	بازگشت mbyte_im_get_status();
# endif

    اگر (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	status = pImmGetOpenStatus(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    بازگشت status;
}

#endif // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ایستا تهی
latin9_to_ucs(char_u *text, صحیح len, WCHAR *unicodebuf)
{
    صحیح		c;

    وقتی (--len >= 0)
    {
	c = *text++;
	انتخاب (c)
	{
	    حالت 0xa4: c = 0x20ac; بشکن;   // euro
	    حالت 0xa6: c = 0x0160; بشکن;   // S hat
	    حالت 0xa8: c = 0x0161; بشکن;   // S -hat
	    حالت 0xb4: c = 0x017d; بشکن;   // Z hat
	    حالت 0xb8: c = 0x017e; بشکن;   // Z -hat
	    حالت 0xbc: c = 0x0152; بشکن;   // OE
	    حالت 0xbd: c = 0x0153; بشکن;   // oe
	    حالت 0xbe: c = 0x0178; بشکن;   // Y
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
    ایستا تهی
RevOut( HDC hdc,
	صحیح col,
	صحیح row,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR text,
	UINT len,
	CONST INT *padding)
{
    صحیح		ix;

    برای (ix = 0; ix < (صحیح)len; ++ix)
	ExtTextOut(hdc, col + TEXT_X(ix), row, foptions,
					pcliprect, text + ix, 1, padding);
}
#endif

    ایستا تهی
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
    وگرنه
#endif
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	MoveToEx(s_hdc, x1, y1, پوچ);
	// Note: LineTo() excludes the last pixel in the line.
	سطر_To(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ایستا تهی
set_pixel(
    صحیح		x,
    صحیح		y,
    COLORREF	color)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_SetPixel(s_dwc, x, y, color);
    وگرنه
#endif
	SetPixel(s_hdc, x, y, color);
}

    ایستا تهی
fill_rect(
    ثابت RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    وگرنه
#endif
    {
	HBRUSH	hbr2;

	اگر (hbr == پوچ)
	    hbr2 = CreateSolidBrush(color);
	وگرنه
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	اگر (hbr == پوچ)
	    DeleteBrush(hbr2);
    }
}

    تهی
gui_mch_draw_string(
    صحیح		row,
    صحیح		col,
    char_u	*text,
    صحیح		len,
    صحیح		flags)
{
    ایستا صحیح	*padding = پوچ;
    ایستا صحیح	pad_size = 0;
    ثابت RECT	*pcliprect = پوچ;
    UINT	foptions = 0;
    ایستا WCHAR *unicodebuf = پوچ;
    ایستا صحیح   *unicodepdy = پوچ;
    ایستا صحیح	unibuflen = 0;
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
    ایستا HBRUSH	hbr_cache[2] = {پوچ, پوچ};
    ایستا guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ایستا صحیح		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    اگر (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.چپ = FILL_X(col);
	rc.بالاترین = FILL_Y(row);
	اگر (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.راست = FILL_X(col + mb_string2cells(text, len));
	}
	وگرنه
	    rc.راست = FILL_X(col + len);
	rc.bottom = FILL_Y(row + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	اگر (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	وگرنه اگر (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	وگرنه
	{
	    اگر (hbr_cache[brush_lru] != پوچ)
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
	اگر (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ifdef FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	وگرنه اگر (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#endif
    }
    SetTextColor(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ifdef FEAT_DIRECTX
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_SetFont(s_dwc, (HFONT)gui.currFont);
#endif

    اگر (pad_size != Columns || padding == پوچ || padding[0] != gui.char_width)
    {
	صحیح	i;

	vim_free(padding);
	pad_size = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(صحیح, pad_size);
	اگر (padding != پوچ)
	    برای (i = 0; i < pad_size; i++)
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
    اگر (enc_utf8)
	برای (n = 0; n < len; ++n)
	    اگر (text[n] >= 0x80)
		بشکن;

#اگر defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    اگر (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#endif

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    اگر ((enc_utf8
		|| (enc_codepage > 0 && (صحیح)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == پوچ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(صحیح, len);

	unibuflen = len;
    }

    اگر (enc_utf8 && n < len && unicodebuf != پوچ)
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
	برای (i = 0; i < len; )
	{
	    c = utf_ptr2char(text + i);
	    اگر (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    وگرنه
	    {
		unicodebuf[wlen++] = c;
	    }

	    اگر (utf_iscomposing(c))
		cw = 0;
	    وگرنه
	    {
		cw = utf_char2cells(c);
		اگر (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    اگر (unicodepdy != پوچ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		اگر (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_width;
		    unicodepdy[wlen - 1] = 0;
		}
		وگرنه
		    unicodepdy[wlen - 1] = cw * gui.char_width;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(text + i, len - i);
	}
#اگر defined(FEAT_DIRECTX)
	اگر (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWriteContext_DrawText(s_dwc, unicodebuf, wlen,
		    TEXT_X(col), TEXT_Y(row),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_width, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	وگرنه
#endif
	    ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    وگرنه اگر ((enc_codepage > 0 && (صحیح)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	اگر (unicodebuf != پوچ)
	{
	    اگر (enc_latin9)
		latin9_to_ucs(text, len, unicodebuf);
	    وگرنه
		len = MultiByteToWideChar(enc_codepage,
			MB_PRECOMPOSED,
			(نویسه *)text, len,
			(LPWSTR)unicodebuf, unibuflen);
	    اگر (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		اگر (unicodepdy != پوچ)
		{
		    صحیح i;
		    صحیح cw;

		    برای (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			اگر (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_width;
		    }
		}
		ExtTextOutW(s_hdc, TEXT_X(col), TEXT_Y(row),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    وگرنه
    {
#ifdef FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	اگر (curwin->w_p_rl)
	    RevOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (نویسه *)text, len, padding);
	وگرنه
#endif
	    ExtTextOut(s_hdc, TEXT_X(col), TEXT_Y(row),
			 foptions, pcliprect, (نویسه *)text, len, padding);
    }

    // Underline
    اگر (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(row + 1) - 1;
	اگر (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_سطر(FILL_X(col), y, FILL_X(col + len), y, gui.currFgColor);
    }

    // Strikethrough
    اگر (flags & DRAW_STRIKE)
    {
	y = FILL_Y(row + 1) - gui.char_height/2;
	draw_سطر(FILL_X(col), y, FILL_X(col + len), y, gui.currSpColor);
    }

    // Undercurl
    اگر (flags & DRAW_UNDERC)
    {
	صحیح			x;
	صحیح			offset;
	ایستا ثابت صحیح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(row + 1) - 1;
	برای (x = FILL_X(col); x < FILL_X(col + len); ++x)
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
    تهی
gui_mch_flush(تهی)
{
#اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#endif

    GdiFlush();
}

    ایستا تهی
clear_rect(RECT *rcp)
{
    fill_rect(rcp, پوچ, gui.back_pixel);
}


    تهی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.راست - workarea_rect.چپ
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.بالاترین
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_height(FALSE);
}


#اگر defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    تهی
gui_mch_add_menu(
    vimmenu_T	*menu,
    صحیح		pos)
{
    vimmenu_T	*parent = menu->parent;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    اگر (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_to_utf16(menu->name, پوچ);
	اگر (wn == پوچ)
	    بازگشت;

	infow.cbSize = اندازه(infow);
	infow.fMask = MIIM_DATA | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dwItemData = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dwTypeData = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	درج_Menu_Item_W((parent == پوچ)
		? s_menuBar : parent->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    اگر (parent == پوچ)
	gui_mswin_get_menu_height(!gui.starting);
# ifdef FEAT_TEAROFF
    وگرنه اگر (IsWindow(parent->tearoff_handle))
	rebuild_tearoff(parent);
# endif
}

    تهی
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    POINT mp;

    (تهی)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (صحیح)mp.x, (صحیح)mp.y);
}

    تهی
gui_make_popup(char_u *path_name, صحیح mouse_pos)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    اگر (menu == پوچ)
	بازگشت;

    POINT	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    اگر (mouse_pos)
    {
	صحیح	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    وگرنه اگر (curwin != پوچ)
    {
	p.x += TEXT_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += TEXT_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    پیام_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (صحیح)p.x, (صحیح)p.y);
}

# اگر defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    تهی
gui_make_tearoff(char_u *path_name)
{
    vimmenu_T	*menu = gui_find_menu(path_name);

    // Found the menu, so tear it off.
    اگر (menu != پوچ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    تهی
gui_mch_add_menu_item(
    vimmenu_T	*menu,
    صحیح		idx)
{
    vimmenu_T	*parent = menu->parent;

    menu->id = s_menu_id++;
    menu->submenu_id = پوچ;

# ifdef FEAT_TEAROFF
    اگر (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	درج_Menu(parent->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    وگرنه
# endif
# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(parent->name))
    {
	TBBUTTON newtb;

	CLEAR_FIELD(newtb);
	اگر (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	وگرنه
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
    وگرنه
# endif
    {
	WCHAR	*wn;

	wn = enc_to_utf16(menu->name, پوچ);
	اگر (wn != پوچ)
	{
	    درج_Menu_W(parent->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	اگر (IsWindow(parent->tearoff_handle))
	    rebuild_tearoff(parent);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    تهی
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    اگر (menu->submenu_id == (HMENU)-1)
    {
	صحیح iButton;

	iButton = (صحیح)SendMessage(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	SendMessage(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    وگرنه
# endif
    {
	اگر (menu->parent != پوچ
		&& menu_is_popup(menu->parent->dname)
		&& menu->parent->submenu_id != پوچ)
	    RemoveMenu(menu->parent->submenu_id, menu->id, MF_BYCOMMAND);
	وگرنه
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	اگر (menu->submenu_id != پوچ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	اگر (IsWindow(menu->tearoff_handle))
	    DestroyWindow(menu->tearoff_handle);
	اگر (menu->parent != پوچ
		&& menu->parent->children != پوچ
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
    ایستا تهی
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    صحیح		x, y;

    HWND thwnd = menu->tearoff_handle;

    GetWindowText(thwnd, (LPSTR)tbuf, 127);
    اگر (GetWindowRect(thwnd, &trect)
	    && GetWindowRect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.چپ - rct.چپ;
	y = (trect.بالاترین -  rct.bottom  + roct.bottom);
    }
    وگرنه
    {
	x = y = 0xffffL;
    }
    DestroyWindow(thwnd);
    اگر (menu->children != پوچ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	اگر (IsWindow(menu->tearoff_handle))
	    (تهی) SetWindowPos(menu->tearoff_handle,
				پوچ,
				(صحیح)trect.چپ,
				(صحیح)trect.بالاترین,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    تهی
gui_mch_menu_grey(
    vimmenu_T	*menu,
    صحیح	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    اگر (menu->submenu_id == (HMENU)-1)
	SendMessage(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    وگرنه
# endif
	(تهی)EnableMenuItem(menu->parent ? menu->parent->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    اگر ((menu->parent != پوچ) && (IsWindow(menu->parent->tearoff_handle)))
    {
	WORD menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	اگر (menu->children == پوچ)
	    menuID = (WORD)(menu->id);
	وگرنه
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = GetDlgItem(menu->parent->tearoff_handle, menuID);
	اگر (menuHandle)
	    EnableWindow(menuHandle, !grey);

    }
# endif
}

#endif // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#define add_word(x)		*p++ = (x)
#define add_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (WORD *)dwp

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
    ایستا LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam UNUSED)
{
    اگر (message == WM_INITDIALOG)
    {
	CenterWindow(hwnd, GetWindow(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(تهی)SetFocus(hwnd);
	اگر (dialog_default_button > IDCANCEL)
	    (تهی)SetFocus(GetDlgItem(hwnd, dialog_default_button));
	وگرنه
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (تهی)SetFocus(GetDlgItem(hwnd, DLG_NONBUTTON_CONTROL));
	بازگشت FALSE;
    }

    اگر (message == WM_COMMAND)
    {
	صحیح	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	اگر (button >= DLG_NONBUTTON_CONTROL)
	    بازگشت TRUE;

	// If the edit box exists, copy the string.
	اگر (s_textfield != پوچ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    GetDlgItemTextW(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_to_enc(wp, پوچ);
	    اگر (p != پوچ)
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
		EndDialog(hwnd, dialog_default_button);
	}
	وگرنه
	    EndDialog(hwnd, button - IDCANCEL);
	بازگشت TRUE;
    }

    اگر ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	EndDialog(hwnd, 0);
	بازگشت TRUE;
    }
    بازگشت FALSE;
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

ایستا ثابت نویسه *dlg_icons[] = // must match names in resource file
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
    char_u	*message,
    char_u	*buttons,
    صحیح		 dfltbutton,
    char_u	*textfield,
    صحیح		ex_cmd UNUSED)
{
    WORD	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    صحیح		رقم_Buttons;
    صحیح		*buttonWidths, *buttonPositions;
    صحیح		buttonYpos;
    صحیح		nchar, i;
    DWORD	lStyle;
    صحیح		dlgwidth = 0;
    صحیح		dlgheight;
    صحیح		editboxheight;
    صحیح		horizWidth = 0;
    صحیح		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*last_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    صحیح		fontHeight;
    صحیح		textWidth, minButtonWidth, messageWidth;
    صحیح		maxDialogWidth;
    صحیح		maxDialogHeight;
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
    صحیح		dlg_icon_width;
    صحیح		dlg_icon_height;
    صحیح		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    اگر (!(gui.in_use || gui.starting))
#  endif
	اگر (silent_mode)
	    بازگشت dfltbutton;   // return default option
# endif

    اگر (s_hwnd == پوچ)
    {
	load_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    وگرنه
	dpi = pGetDpiForSystem();

    اگر ((type < 0) || (type > VIM_LAST_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_SIZE + STRLEN(message) * 2);

    اگر (p == پوچ)
	بازگشت -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    اگر (tbuffer == پوچ)
	بازگشت -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    رقم_Buttons = 1;
    برای (i = 0; tbuffer[i] != '\0'; i++)
    {
	اگر (tbuffer[i] == DLG_BUTTON_SEP)
	    رقم_Buttons++;
    }
    اگر (dfltbutton >= رقم_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(صحیح, رقم_Buttons);
    اگر (buttonWidths == پوچ)
	بازگشت -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(صحیح, رقم_Buttons);
    اگر (buttonPositions == پوچ)
	بازگشت -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وگرنه
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
    اگر (s_hwnd == پوچ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	maxDialogWidth = workarea_rect.راست - workarea_rect.چپ - 100;
	اگر (maxDialogWidth > adjust_by_system_dpi(600))
	    maxDialogWidth = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	maxDialogHeight = workarea_rect.bottom - workarea_rect.بالاترین - 150;
    }
    وگرنه
    {
	// Use our own window for the size, unless it's very small.
	GetWindowRect(s_hwnd, &rect);
	maxDialogWidth = rect.راست - rect.چپ
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	اگر (maxDialogWidth < adjust_by_system_dpi(DLG_MIN_MAX_WIDTH))
	    maxDialogWidth = adjust_by_system_dpi(DLG_MIN_MAX_WIDTH);

	maxDialogHeight = rect.bottom - rect.بالاترین
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	اگر (maxDialogHeight < adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT))
	    maxDialogHeight = adjust_by_system_dpi(DLG_MIN_MAX_HEIGHT);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = message;
    messageWidth = 0;
    msgheight = 0;
    ga_init2(&ga, اندازه(نویسه), 500);
    انجامبده
    {
	msgheight += fontHeight;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	textWidth = 0;
	last_white = پوچ;
	برای (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    اگر (l == 1 && VIM_ISWHITE(*pend)
					&& textWidth > maxDialogWidth * 3 / 4)
		last_white = pend;
	    textWidth += GetTextWidthEnc(hdc, pend, l);
	    اگر (textWidth >= maxDialogWidth)
	    {
		// Line will wrap.
		messageWidth = maxDialogWidth;
		msgheight += fontHeight;
		textWidth = 0;

		اگر (last_white != پوچ)
		{
		    // break the line just after a space
		    اگر (pend > last_white)
			ga.ga_len -= (صحیح)(pend - (last_white + 1));
		    pend = last_white + 1;
		    last_white = پوچ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ادامه;
	    }

	    وقتی (--l >= 0)
		ga_append(&ga, *pend++);
	}
	اگر (textWidth > messageWidth)
	    messageWidth = textWidth;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } وقتی (*pend != NUL);

    اگر (ga.ga_data != پوچ)
	message = ga.ga_data;

    messageWidth += 10;		// roundoff space

    dlg_icon_width = adjust_by_system_dpi(DLG_ICON_WIDTH);
    dlg_icon_height = adjust_by_system_dpi(DLG_ICON_HEIGHT);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = messageWidth + dlg_icon_width + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    اگر (msgheight < dlg_icon_height)
	msgheight = dlg_icon_height;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != پوچ && vim_strchr(p_go, GO_VERTICAL) != پوچ);
    اگر (!vertical)
    {
	// Place buttons horizontally if they fit.
	horizWidth = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	انجامبده
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    اگر (pend == پوچ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (صحیح)(pend - pstart));
	    اگر (textWidth < minButtonWidth)
		textWidth = minButtonWidth;
	    textWidth += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = textWidth;
	    buttonPositions[i++] = horizWidth;
	    horizWidth += textWidth + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} وقتی (*pend != NUL);

	اگر (horizWidth > maxDialogWidth)
	    vertical = TRUE;	// Too wide to fit on the screen.
	وگرنه اگر (horizWidth > dlgwidth)
	    dlgwidth = horizWidth;
    }

    اگر (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	انجامبده
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    اگر (pend == پوچ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    textWidth = GetTextWidthEnc(hdc, pstart, (صحیح)(pend - pstart));
	    textWidth += dlgPaddingX;		// Padding within button
	    textWidth += DLG_VERT_PADDING_X * 2; // Padding around button
	    اگر (textWidth > dlgwidth)
		dlgwidth = textWidth;
	    pstart = pend + 1;
	} وقتی (*pend != NUL);
    }

    اگر (dlgwidth < DLG_MIN_WIDTH)
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
    اگر (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * fontHeight * رقم_Buttons;
    وگرنه
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * fontHeight;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = fontHeight + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    اگر (textfield != پوچ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    اگر (dlgheight > maxDialogHeight)
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
    اگر (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحیح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وگرنه
# endif
    {
	*p++ = DLG_FONT_POINT_SIZE;		// point size
	nchar = nCopyAnsiToWideChar(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    اگر (textfield != پوچ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    اگر (!vertical)
	horizWidth = (dlgwidth - horizWidth) / 2;	// Now it's X offset
    برای (i = 0; i < رقم_Buttons; i++)
    {
	// get end of this button.
	برای (	pend = pstart;
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
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(DLG_VERT_PADDING_X),
		    PixelToDialogY(buttonYpos // TBK
				   + 2 * fontHeight * i),
		    PixelToDialogX(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (نویسه *)pstart);
	}
	وگرنه
	{
	    p = add_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    PixelToDialogX(horizWidth + buttonPositions[i]),
		    PixelToDialogY(buttonYpos), // TBK
		    PixelToDialogX(buttonWidths[i]),
		    (WORD)(PixelToDialogY(2 * fontHeight) - 1),
		    (WORD)(IDCANCEL + 1 + i), (WORD)0x0080, (نویسه *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += رقم_Buttons;

    // Vim icon
    p = add_dialog_element(p, SS_ICON,
	    PixelToDialogX(dlgPaddingX),
	    PixelToDialogY(dlgPaddingY),
	    PixelToDialogX(dlg_icon_width),
	    PixelToDialogY(dlg_icon_height),
	    DLG_NONBUTTON_CONTROL + 0, (WORD)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = add_dialog_element(p, ES_چپ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    PixelToDialogX(2 * dlgPaddingX + dlg_icon_width),
	    PixelToDialogY(dlgPaddingY),
	    (WORD)(PixelToDialogX(messageWidth) + 1),
	    PixelToDialogY(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (WORD)0x0081, (نویسه *)message);

    // Edit box
    اگر (textfield != پوچ)
    {
	p = add_dialog_element(p, ES_چپ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		PixelToDialogX(2 * dlgPaddingX),
		PixelToDialogY(2 * dlgPaddingY + msgheight),
		PixelToDialogX(dlgwidth - 4 * dlgPaddingX),
		PixelToDialogY(fontHeight + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (WORD)0x0081, (نویسه *)textfield);
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
    اگر (textfield != پوچ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    وگرنه
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = پوچ;
    }

    // show the dialog box modally and get a return value
    nchar = (صحیح)DialogBoxIndirect(
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
    (تهی)SetFocus(s_hwnd);

    بازگشت nchar;
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
    ایستا PWORD
add_dialog_element(
    PWORD p,
    DWORD lStyle,
    WORD x,
    WORD y,
    WORD w,
    WORD h,
    WORD Id,
    WORD clss,
    ثابت نویسه *caption)
{
    صحیح nchar;

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

    بازگشت p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ایستا LPWORD
lpwAlign(
    LPWORD lpIn)
{
    long_u ul;

    ul = (long_u)lpIn;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    بازگشت (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ایستا صحیح
nCopyAnsiToWideChar(
    LPWORD lpWCStr,
    LPSTR lpAnsiIn,
    BOOL use_enc)
{
    صحیح		nChar = 0;
    صحیح		len = lstrlen(lpAnsiIn) + 1;	// include NUL character
    صحیح		i;
    WCHAR	*wn;

    اگر (use_enc && enc_codepage >= 0 && (صحیح)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_to_utf16((char_u *)lpAnsiIn, پوچ);
	اگر (wn != پوچ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (صحیح)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    اگر (nChar == 0)
	// Use Win32 conversion function.
	nChar = MultiByteToWideChar(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lpAnsiIn, len,
		lpWCStr, len);
    برای (i = 0; i < nChar; ++i)
	اگر (lpWCStr[i] == (WORD)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (WORD)' ';

    بازگشت nChar;
}


#ifdef FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ایستا HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    WORD menu_id)
{
    برای ( ; menu != پوچ; menu = menu->next)
    {
	اگر (menu->modes == 0)	// this menu has just been deleted
	    ادامه;
	اگر (menu_is_separator(menu->dname))
	    ادامه;
	اگر ((WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    بازگشت menu->submenu_id;
    }
    بازگشت پوچ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ایستا LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    اگر (message == WM_INITDIALOG)
    {
	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	بازگشت (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(message, lParam);

    اگر (message == WM_COMMAND)
    {
	اگر ((WORD)(LOWORD(wParam)) & 0x8000)
	{
	    POINT   mp;
	    RECT    rect;

	    اگر (GetCursorPos(&mp) && GetWindowRect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)GetWindowLongPtr(hwnd, DWLP_USER);
		(تهی)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (صحیح)rect.راست - 8,
			 (صحیح)mp.y,
			 (صحیح)0,	    // reserved param
			 s_hwnd,
			 پوچ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	وگرنه
	    // Pass on messages to the main Vim window
	    PostMessage(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(تهی)SetFocus(s_hwnd);
	بازگشت TRUE;
    }
    اگر ((message == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	DestroyWindow(hwnd);
	بازگشت TRUE;
    }

    // When moved around, give main window the focus back.
    اگر (message == WM_EXITSIZEMOVE)
	(تهی)SetActiveWindow(s_hwnd);

    بازگشت FALSE;
}
#endif


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ایستا تهی
get_dialog_font_metrics(تهی)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    SIZE	    size;
#ifdef USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#endif

#ifdef USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = CreateFontIndirectW(&lfSysmenu);
    وگرنه
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

#اگر defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ایستا تهی
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    صحیح		initX,
    صحیح		initY)
{
    WORD	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    صحیح		template_len;
    صحیح		nchar, textWidth, submenuWidth;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    WORD	dlgwidth;
    WORD	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*بالاترین_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    صحیح		col, spaceWidth, len;
    صحیح		columnWidths[2];
    char_u	*label, *text;
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
    اگر (IsWindow(menu->tearoff_handle))
    {
	POINT mp;
	اگر (GetCursorPos(&mp))
	{
	    SetWindowPos(menu->tearoff_handle, پوچ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	بازگشت;
    }

    /*
     * Create a new tearoff.
     */
    اگر (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_SIZE;
    pdlgtemplate = p = (WORD *)LocalAlloc(LPTR, template_len);
    اگر (p == پوچ)
	بازگشت;

    hwnd = GetDesktopWindow();
    hdc = GetWindowDC(hwnd);
# ifdef USE_SYSMENU_FONT
    اگر (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = CreateFontIndirectW(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    وگرنه
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
    برای (col = 0; col < 2; col++)
    {
	columnWidths[col] = 0;
	FOR_ALL_CHILD_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    text = (col == 0) ? pmenu->dname : pmenu->actext;
	    اگر (text != پوچ && *text != NUL)
	    {
		textWidth = GetTextWidthEnc(hdc, text, (صحیح)STRLEN(text));
		اگر (textWidth > columnWidths[col])
		    columnWidths[col] = textWidth;
	    }
	    اگر (pmenu->children != پوچ)
		submenuWidth = TEAROFF_COLUMN_PADDING * spaceWidth;
	}
    }
    اگر (columnWidths[1] == 0)
    {
	// no accelerators
	اگر (submenuWidth != 0)
	    columnWidths[0] += submenuWidth;
	وگرنه
	    columnWidths[0] += spaceWidth;
    }
    وگرنه
    {
	// there is an accelerator column
	columnWidths[0] += TEAROFF_COLUMN_PADDING * spaceWidth;
	columnWidths[1] += submenuWidth;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    textWidth = columnWidths[0] + columnWidths[1];
    اگر (submenuWidth != 0)
    {
	submenuWidth = GetTextWidth(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (صحیح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	textWidth += submenuWidth;
    }
    dlgwidth = GetTextWidthEnc(hdc, title, (صحیح)STRLEN(title));
    اگر (textWidth > dlgwidth)
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
    اگر (initX == 0xffffL)
	*p++ = PixelToDialogX(x); // x
    وگرنه
	*p++ = PixelToDialogX(initX); // x
    اگر (initY == 0xffffL)
	*p++ = PixelToDialogY(y); // y
    وگرنه
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
    اگر (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lfHeight, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (صحیح)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    وگرنه
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
    اگر (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->next;
    وگرنه
	menu = menu->children;
    بالاترین_menu = menu;
    برای ( ; menu != پوچ; menu = menu->next)
    {
	اگر (menu->modes == 0)	// this menu has just been deleted
	    ادامه;
	اگر (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ادامه;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	اگر (((نویسه *)p - (نویسه *)pdlgtemplate) + 1000 > template_len)
	{
	    WORD    *newp;

	    newp = (WORD *)LocalAlloc(LPTR, template_len + 4096);
	    اگر (newp != پوچ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (نویسه *)p - (نویسه *)pdlgtemplate);
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
	padding0 = (columnWidths[0] - GetTextWidthEnc(hdc, menu->dname,
				      (صحیح)STRLEN(menu->dname))) / spaceWidth;
	len += padding0;

	اگر (menu->actext != پوچ)
	{
	    acLen = (صحیح)STRLEN(menu->actext);
	    len += acLen;
	    textWidth = GetTextWidthEnc(hdc, menu->actext, acLen);
	}
	وگرنه
	    textWidth = 0;
	padding1 = (columnWidths[1] - textWidth) / spaceWidth;
	len += padding1;

	اگر (menu->children == پوچ)
	{
	    padding2 = submenuWidth / spaceWidth;
	    len += padding2;
	    menuID = (WORD)(menu->id);
	}
	وگرنه
	{
	    len += (صحیح)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (WORD)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	text = label = alloc(len + 1);
	اگر (label == پوچ)
	    بشکن;

	vim_strncpy(text, menu->name, nameLen);
	text = vim_strchr(text, TAB);	    // stop at TAB before actext
	اگر (text == پوچ)
	    text = label + nameLen;	    // no actext, use whole name
	وقتی (padding0-- > 0)
	    *text++ = ' ';
	اگر (menu->actext != پوچ)
	{
	    STRNCPY(text, menu->actext, acLen);
	    text += acLen;
	}
	وقتی (padding1-- > 0)
	    *text++ = ' ';
	اگر (menu->children != پوچ)
	{
	    STRCPY(text, TEAROFF_SUBMENU_LABEL);
	    text += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	وگرنه
	{
	    وقتی (padding2-- > 0)
		*text++ = ' ';
	}
	*text = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = add_dialog_element(p,
		BS_PUSHBUTTON|BS_چپ,
		(WORD)PixelToDialogX(TEAROFF_PADDING_X),
		(WORD)(sepPadding + 1 + 13 * (*pnumitems)),
		(WORD)PixelToDialogX(dlgwidth - 2 * TEAROFF_PADDING_X),
		(WORD)12,
		menuID, (WORD)0x0080, (نویسه *)label);
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
	    (LPARAM)بالاترین_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (تهی)SetActiveWindow(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#endif

#اگر defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ایستا تهی
initialise_toolbar(تهی)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_CHILD | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    پوچ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_WIDTH, //api guide is wrong!
		    TOOLBAR_BUTTON_HEIGHT,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    اندازه(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    SendMessage(s_toolbarhwnd, TB_SETSTYLE, 0,
	SendMessage(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = SubclassWindow(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != پوچ);

    update_toolbar_size();
}

    ایستا تهی
update_toolbar_size(تهی)
{
    صحیح		w, h;
    TBMETRICS	tbm;

    tbm.cbSize = اندازه(TBMETRICS);
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

    ایستا LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_پیام,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_پیام, lParam);
    بازگشت CallWindowProc(s_toolbar_wndproc, hwnd, u_پیام, wParam, lParam);
}

    ایستا صحیح
get_toolbar_bitmap(vimmenu_T *menu)
{
    صحیح i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    اگر (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = پوچ;

	اگر (menu->iconfile != پوچ)
	{
	    gui_find_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = LoadImage(
			پوچ,
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
	اگر (hbitmap == پوچ
		&& (gui_find_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != پوچ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = LoadImage(
		    پوچ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_WIDTH,
		    TOOLBAR_BUTTON_HEIGHT,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	اگر (hbitmap != پوچ)
	{
	    TBADDBITMAP tbAddBitmap;

	    tbAddBitmap.hInst = پوچ;
	    tbAddBitmap.nID = (long_u)hbitmap;

	    i = (صحیح)SendMessage(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tbAddBitmap);
	    // i will be set to -1 if it fails
	}
    }
    اگر (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_شمار)
	i = menu->iconidx;

    بازگشت i;
}
#endif

#اگر defined(FEAT_GUI_TABLINE)
    ایستا تهی
initialise_tabline(تهی)
{
    InitCommonControls();

    s_tabhwnd = CreateWindow(WC_TABCONTROL, "Vim tabline",
	    WS_CHILD|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, پوچ, g_hinst, پوچ);
    s_tabline_wndproc = SubclassWindow(s_tabhwnd, tabline_wndproc);

    gui.tabline_height = TABLINE_HEIGHT;

    set_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ایستا tabpage_T *
GetTabFromPoint(
    HWND    hWnd,
    POINT   pt)
{
    tabpage_T	*ptp = پوچ;

    اگر (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	اگر (s_tabhwnd == hWnd)
	{
	    صحیح idx = TabCtrl_HitTest(s_tabhwnd, &htinfo);
	    اگر (idx != -1)
		ptp = find_tabpage(idx + 1);
	}
    }
    بازگشت ptp;
}

ایستا POINT	    s_pt = {0, 0};
ایستا HCURSOR      s_hCursor = پوچ;

    ایستا LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_پیام,
    WPARAM wParam,
    LPARAM lParam)
{
    POINT	pt;
    tabpage_T	*tp;
    RECT	rect;
    صحیح		nCenter;
    صحیح		idx0;
    صحیح		idx1;

    HandleMouseHide(u_پیام, lParam);

    انتخاب (u_پیام)
    {
	حالت WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		SetCapture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		بشکن;
	    }
	حالت WM_MOUSEMOVE:
	    اگر (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		اگر (abs(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    SetCursor(LoadCursor(پوچ, IDC_SIZEWE));

		    tp = GetTabFromPoint(hwnd, pt);
		    اگر (tp != پوچ)
		    {
			idx0 = tabpage_index(curtab) - 1;
			idx1 = tabpage_index(tp) - 1;

			TabCtrl_GetItemRect(hwnd, idx1, &rect);
			nCenter = rect.چپ + (rect.راست - rect.چپ) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			اگر ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			وگرنه اگر ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    بشکن;
	حالت WM_LBUTTONUP:
	    {
		اگر (GetCapture() == hwnd)
		{
		    SetCursor(s_hCursor);
		    ReleaseCapture();
		}
		بشکن;
	    }
	حالت WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = TabCtrl_HitTest(hwnd, &htinfo);
		اگر (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		بشکن;
	    }
	پیشفرض:
	    بشکن;
    }

    بازگشت CallWindowProc(s_tabline_wndproc, hwnd, u_پیام, wParam, lParam);
}
#endif

#اگر defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    تهی
gui_mch_set_foreground(تهی)
{
    اگر (IsIconic(s_hwnd))
	 SendMessage(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    SetForegroundWindow(s_hwnd);
}
#endif

#اگر defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ایستا تهی
dyn_imm_load(تهی)
{
    hLibImm = vimLoadLib("imm32.dll");
    اگر (hLibImm == پوچ)
	بازگشت;

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

    اگر (       pImmGetCompositionStringW == پوچ
	    || pImmGetContext == پوچ
	    || pImmAssociateContext == پوچ
	    || pImmReleaseContext == پوچ
	    || pImmGetOpenStatus == پوچ
	    || pImmSetOpenStatus == پوچ
	    || pImmGetCompositionFontW == پوچ
	    || pImmSetCompositionFontW == پوچ
	    || pImmSetCompositionWindow == پوچ
	    || pImmGetConversionStatus == پوچ
	    || pImmSetConversionStatus == پوچ)
    {
	FreeLibrary(hLibImm);
	hLibImm = پوچ;
	pImmGetContext = پوچ;
	بازگشت;
    }

    بازگشت;
}

#endif

#اگر defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

تعریفنوع ساختار _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    تهی
gui_mch_drawsign(صحیح row, صحیح col, صحیح typenr)
{
    signicon_t *sign;
    صحیح x, y, w, h;

    اگر (!gui.in_use || (sign = (signicon_t *)sign_get_image(typenr)) == پوچ)
	بازگشت;

# اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = TEXT_X(col);
    y = TEXT_Y(row);
    w = gui.char_width * 2;
    h = gui.char_height;
    انتخاب (sign->uType)
    {
	حالت IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		BitBlt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    بشکن;
	حالت IMAGE_ICON:
	حالت IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, پوچ, DI_NORMAL);
	    بشکن;
# ifdef FEAT_XPM_W32
	حالت IMAGE_XPM:
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
	    بشکن;
# endif
    }
}

    ایستا تهی
close_signicon_image(signicon_t *sign)
{
    اگر (sign == پوچ)
	بازگشت;

    انتخاب (sign->uType)
    {
	حالت IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    بشکن;
	حالت IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    بشکن;
	حالت IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    بشکن;
# ifdef FEAT_XPM_W32
	حالت IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    بشکن;
# endif
    }
}

    تهی *
gui_mch_register_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = پوچ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    اگر (ext > signfile)
    {
	صحیح do_load = 1;

	اگر (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	وگرنه اگر (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	وگرنه اگر (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	وگرنه
	    do_load = 0;

	اگر (do_load)
	    sign.hImage = (HANDLE)LoadImage(پوچ, (LPCSTR)signfile, sign.uType,
		    gui.char_width * 2, gui.char_height,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	اگر (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    LoadXpmImage((نویسه *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = پوچ;
    اگر (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != پوچ)
	*psign = sign;

    اگر (!psign)
    {
	اگر (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_in_sign_data));
    }
    بازگشت (تهی *)psign;

}

    تهی
gui_mch_destroy_sign(تهی *sign)
{
    اگر (sign == پوچ)
	بازگشت;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#endif

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

    ایستا تهی
make_tooltip(BalloonEval *beval, نویسه *text, POINT pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    اگر (pti == پوچ)
	بازگشت;

    beval->balloon = CreateWindowExW(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    پوچ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->target, پوچ, g_hinst, پوچ);

    SetWindowPos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cbSize = اندازه(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->target;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpszText = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_to_utf16((char_u*)text, پوچ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    اگر (GetClientRect(s_textArea, &rect))
	SendMessageW(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.راست);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.چپ = pt.x - 3;
    pti->rect.بالاترین = pt.y - 3;
    pti->rect.راست = pt.x + 3;
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

    ایستا تهی
delete_tooltip(BalloonEval *beval)
{
    PostMessage(beval->balloon, WM_CLOSE, 0, 0);
}

    ایستا VOID CALLBACK
beval_timer_proc(
    HWND	hwnd UNUSED,
    UINT	u_پیام UNUSED,
    UINT_PTR	idEvent UNUSED,
    DWORD	dwTime)
{
    POINT	pt;
    RECT	rect;

    اگر (cur_beval == پوچ || cur_beval->showState == ShS_SHOWING || !p_beval)
	بازگشت;

    GetCursorPos(&pt);
    اگر (WindowFromPoint(pt) != s_textArea)
	بازگشت;

    ScreenToClient(s_textArea, &pt);
    GetClientRect(s_textArea, &rect);
    اگر (!PtInRect(&rect, pt))
	بازگشت;

    اگر (last_user_activity > 0
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

	اگر (cur_beval->پیام_CB != پوچ)
	    (*cur_beval->پیام_CB)(cur_beval, 0);
    }
}

    تهی
gui_mch_disable_beval_area(BalloonEval *beval UNUSED)
{
    KillTimer(s_textArea, beval_timer_id);
}

    تهی
gui_mch_enable_beval_area(BalloonEval *beval)
{
    اگر (beval == پوچ)
	بازگشت;
    beval_timer_id = SetTimer(s_textArea, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    تهی
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    POINT   pt;

    vim_free(beval->پیام);
    beval->پیام = mesg == پوچ ? پوچ : vim_strsave(mesg);
    اگر (beval->پیام == پوچ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	بازگشت;
    }

    اگر (beval->showState == ShS_SHOWING)
	بازگشت;
    GetCursorPos(&pt);
    ScreenToClient(s_textArea, &pt);

    اگر (abs(beval->x - pt.x) < 3 && abs(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (نویسه *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    تهی	*target UNUSED,	// ignored, always use s_textArea
    char_u	*mesg,
    تهی	(*mesgCB)(BalloonEval *, صحیح),
    تهی	*clientData)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    اگر (mesg != پوچ && mesgCB != پوچ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_message_and_callback);
	بازگشت پوچ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    اگر (beval != پوچ)
    {
	beval->target = s_textArea;

	beval->showState = ShS_NEUTRAL;
	beval->پیام = mesg;
	beval->پیام_CB = mesgCB;
	beval->clientData = clientData;

	InitCommonControls();
	cur_beval = beval;

	اگر (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    بازگشت beval;
}

    ایستا تهی
Handle_WM_Notify(HWND hwnd UNUSED, LPNMHDR pnmh)
{
    اگر (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	بازگشت;

    اگر (cur_beval == پوچ)
	بازگشت;

    انتخاب (pnmh->code)
    {
	حالت TTN_SHOW:
	    بشکن;
	حالت TTN_برداشت: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    بشکن;
	حالت TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpszText = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    بشکن;
	حالت TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpszText = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    بشکن;
    }
}

    ایستا تهی
track_user_activity(UINT u_پیام)
{
    اگر ((u_پیام >= WM_MOUSEFIRST && u_پیام <= WM_MOUSELAST)
	    || (u_پیام >= WM_KEYFIRST && u_پیام <= WM_KEYLAST))
	last_user_activity = Get_Tick_شمار();
}

    تهی
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#endif // FEAT_BEVAL_GUI

#اگر defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    تهی
netbeans_draw_multisign_indicator(صحیح row)
{
    صحیح i;
    صحیح y;
    صحیح x;

    اگر (!netbeans_active())
	بازگشت;

    x = 0;
    y = TEXT_Y(row);

# اگر defined(FEAT_DIRECTX)
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    برای (i = 0; i < gui.char_height - 3; i++)
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

#اگر defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ایستا صحیح
test_gui_w32_sendevent_mouse(dict_T *args)
{
    اگر (!dict_has_کلید(args, "row") || !dict_has_کلید(args, "col"))
	بازگشت FALSE;

    // Note: "move" is optional, requires fewer arguments
    صحیح move = (صحیح)dict_get_bool(args, "move", FALSE);

    اگر (!move && (!dict_has_کلید(args, "button")
	    || !dict_has_کلید(args, "multiclick")
	    || !dict_has_کلید(args, "modifiers")))
	بازگشت FALSE;

    صحیح row = (صحیح)dict_get_عدد(args, "row");
    صحیح col = (صحیح)dict_get_عدد(args, "col");

    اگر (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	اگر (dict_get_bool(args, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    صحیح pY = (row - 1) * gui.char_height + gui.char_height / 2;
	    صحیح pX = (col - 1) * gui.char_width + gui.char_width / 2;
	    gui_mouse_moved(pX, pY);
	}
	وگرنه
	    gui_mouse_moved(col, row);
    }
    وگرنه
    {
	صحیح button = (صحیح)dict_get_عدد(args, "button");
	صحیح repeated_click = (صحیح)dict_get_عدد(args, "multiclick");
	int_u mods = (صحیح)dict_get_عدد(args, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_set_hor_scroll_step(6);
	mouse_set_vert_scroll_step(3);

	gui_send_mouse_event(button, TEXT_X(col - 1), TEXT_Y(row - 1),
							repeated_click, mods);
    }
    بازگشت TRUE;
}

    ایستا صحیح
test_gui_w32_sendevent_keyboard(dict_T *args)
{
    درون_داد_2 inputs[1];
    درون_داد_2 modkeys[3];
    SecureZeroMemory(inputs, اندازه(درون_داد_2));
    SecureZeroMemory(modkeys, 3 * اندازه(درون_داد_2));

    char_u *event = dict_get_string(args, "event", TRUE);

    اگر (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	WORD vkCode = dict_get_عدد_def(args, "keycode", 0);
	اگر (vkCode <= 0 || vkCode >= 0xFF)
	{
	    semsg(_(e_invalid_argument_nr), (long)vkCode);
	    بازگشت FALSE;
	}

	BOOL is_Mod_کلید = (vkCode == VK_SHIFT || vkCode == VK_CONTROL
	    || vkCode == VK_MENU || vkCode == VK_LSHIFT || vkCode == VK_RSHIFT
	    || vkCode == VK_LCONTROL || vkCode == VK_RCONTROL
	    || vkCode == VK_LMENU || vkCode == VK_RMENU );

	BOOL unwrapMods = FALSE;
	صحیح mods = (صحیح)dict_get_عدد(args, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	اگر (mods && STRICMP(event, "keydown") == 0 && !is_Mod_کلید)
	{
	    صحیح n = 0;
	    اگر (mods & MOD_MASK_SHIFT)
	    {
		modkeys[n].type = درون_داد_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    اگر (mods & MOD_MASK_CTRL)
	    {
		modkeys[n].type = درون_داد_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    اگر (mods & MOD_MASK_ALT)
	    {
		modkeys[n].type = درون_داد_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    اگر (n)
	    {
		(تهی)SetForegroundWindow(s_hwnd);
		Send_درون_داد(n, modkeys, اندازه(درون_داد_2));
	    }
	}

	inputs[0].type = درون_داد_KEYBOARD;
	inputs[0].ki.wVk = vkCode;
	اگر (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    اگر (!is_Mod_کلید)
		unwrapMods = TRUE;
	}

	(تهی)SetForegroundWindow(s_hwnd);
	Send_درون_داد(ARRAYSIZE(inputs), inputs, اندازه(درون_داد_2));
	vim_free(event);

	اگر (unwrapMods)
	{
	    modkeys[0].type = درون_داد_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = درون_داد_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = درون_داد_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (تهی)SetForegroundWindow(s_hwnd);
	    Send_درون_داد(3, modkeys, اندازه(درون_داد_2));
	}
    }
    وگرنه
    {
	اگر (event == پوچ)
	{
	    semsg(_(e_missing_argument_str), "event");
	}
	وگرنه
	{
	    semsg(_(e_invalid_مقدار_for_argument_str_str), "event", event);
	    vim_free(event);
	}
	بازگشت FALSE;
    }
    بازگشت TRUE;
}

    ایستا صحیح
test_gui_w32_sendevent_set_keycode_trans_strategy(dict_T *args)
{
    صحیح handled = 0;
    char_u *strategy = dict_get_string(args, "strategy", TRUE);

    اگر (strategy)
    {
	اگر (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_classic;
	}
	وگرنه اگر (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_used = &keycode_trans_strategy_experimental;
	}
    }

    اگر (!handled)
    {
	اگر (strategy == پوچ)
	{
	    semsg(_(e_missing_argument_str), "strategy");
	}
	وگرنه
	{
	    semsg(_(e_invalid_مقدار_for_argument_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	بازگشت FALSE;
    }
    بازگشت TRUE;
}


    صحیح
test_gui_w32_sendevent(char_u *event, dict_T *args)
{
    اگر (STRICMP(event, "key") == 0)
	بازگشت test_gui_w32_sendevent_keyboard(args);
    وگرنه اگر (STRICMP(event, "mouse") == 0)
	بازگشت test_gui_w32_sendevent_mouse(args);
    وگرنه اگر (STRICMP(event, "set_keycode_trans_strategy") == 0)
	بازگشت test_gui_w32_sendevent_set_keycode_trans_strategy(args);
    وگرنه
    {
	semsg(_(e_invalid_مقدار_for_argument_str_str), "event", event);
	بازگشت FALSE;
    }
}
#endif

#اگر defined(FEAT_IMAGE_GUI)

تعریفنوع ساختار
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
    ایستا تهی
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, صحیح iw, صحیح ih, image_format_T fmt)
{
    صحیح		y, x;
    صحیح		stride = iw * 4;
    صحیح		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.back_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    برای (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	برای (x = 0; x < iw; x++)
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
		وگرنه اگر (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		وگرنه
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    وگرنه
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
    تهی	*bits = پوچ;

    اگر (ctx == پوچ)
	بازگشت FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_FIELD(bmi);
    bmi.bmiHeader.biSize = اندازه(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = iw;
    bmi.bmiHeader.biHeight = -ih; // Top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.bi_Bit_شمار = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &bits, پوچ, 0);
    اگر (hbm == پوچ || bits == پوچ)
    {
	اگر (hbm != پوچ)
	    DeleteObject(hbm);
	بازگشت false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    اگر (mem_dc == پوچ)
    {
	DeleteObject(hbm);
	بازگشت false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)bits, img->data, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_data = ctx;
    بازگشت OK;
}

    تهی
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_data;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    صحیح
image_placement_gui_init(image_placement_T *place UNUSED)
{
    بازگشت OK;
}

    تهی
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    تهی
image_placement_gui_draw(image_placement_T *place)
{
    image_T	    *img = place->img;
    image_gdi_T	    *ictx = img->backend_data;
    pixman_box32_t  *rects;
    صحیح		    n_rects;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    اگر (rects == پوچ || n_rects == 0)
	بازگشت;

# اگر defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    اگر (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    برای (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		row, col;
	صحیح		posx, posy;
	صحیح		x, y, w, h;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, true);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	BitBlt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    تهی
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE_GUI
