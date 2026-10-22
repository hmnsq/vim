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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#అయితే_ముగింపు

// values for "dead_key"
#నిర్వచించు DEAD_కీలకం_OFF			0	// no dead key
#నిర్వచించు DEAD_కీలకం_సమితి_DEFAULT		1	// dead key pressed
#నిర్వచించు DEAD_కీలకం_TRANSIENT_లోపల_ON_CHAR	2	// wait for next key press
#నిర్వచించు DEAD_కీలకం_SKIP_ON_CHAR		3	// skip next _OnChar()

#ఒకవేళ defined(FEAT_DIRECTX)
స్థిర DWriteContext *s_dwc = ఖాళీ;
స్థిర పూర్ణాంకం s_directx_enabled = 0;
స్థిర పూర్ణాంకం s_directx_ఎక్కించు_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ఖాళీ && enc_utf8)
స్థిర పూర్ణాంకం directx_enabled(శూన్య);
స్థిర శూన్య directx_binddc(శూన్య);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
స్థిర పూర్ణాంకం gui_mswin_get_menu_ఎత్తు(పూర్ణాంకం fix_కిటికీ);
#లేకపోతే
# define gui_mswin_get_menu_ఎత్తు(fix_కిటికీ)	0
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం keycode_trans_strategy {
    శూన్య (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
    శూన్య (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
    శూన్య (*ptr_process_సందేశం_usual_కీలకం) (UINT /*vk*/, స్థిరరాశి సందేశం_3* /*pmsg*/);
    పూర్ణాంకం  (*ptr_get_చురుకైన_modifiers)(శూన్య);
    పూర్ణాంకం  (*is_experimental)(శూన్య);
} keycode_trans_strategy;

// forward declarations for input instance initializer
స్థిర శూన్య _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
స్థిర శూన్య _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
స్థిర శూన్య process_సందేశం_usual_కీలకం_experimental(UINT /*vk*/, స్థిరరాశి సందేశం_3* /*pmsg*/);
స్థిర పూర్ణాంకం  get_చురుకైన_modifiers_experimental(శూన్య);
స్థిర పూర్ణాంకం  is_experimental_true(శూన్య);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_సందేశం_usual_కీలకం_experimental // ptr_process_message_usual_key
    , get_చురుకైన_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
స్థిర శూన్య _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
స్థిర శూన్య _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, పూర్ణాంకం /*cRepeat UNUSED*/);
స్థిర శూన్య process_సందేశం_usual_కీలకం_classic(UINT /*vk*/, స్థిరరాశి సందేశం_3* /*pmsg*/);
స్థిర పూర్ణాంకం  get_చురుకైన_modifiers_classic(శూన్య);
స్థిర పూర్ణాంకం  is_experimental_false(శూన్య);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_సందేశం_usual_కీలకం_classic // ptr_process_message_usual_key
    , get_చురుకైన_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_వాడిన = ఖాళీ;

స్థిర పూర్ణాంకం is_experimental_true(శూన్య)
{
    తిరిగి 1;
}

స్థిర పూర్ణాంకం is_experimental_false(శూన్య)
{
    తిరిగి 0;
}

/*
 * Initialize the keycode translation strategy.
 */
స్థిర శూన్య keycode_trans_strategy_init(శూన్య)
{
    స్థిరరాశి అక్షరం *strategy = ఖాళీ;

    // set default value as fallback
    keycode_trans_strategy_వాడిన = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    ఒకవేళ (strategy == ఖాళీ)
    {
	తిరిగి;
    }

    ఒకవేళ (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_వాడిన = &keycode_trans_strategy_classic;
	తిరిగి;
    }

    ఒకవేళ (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_వాడిన = &keycode_trans_strategy_experimental;
	తిరిగి;
    }

}

#ఒకవేళ defined(FEAT_RENDER_OPTIONS)
    పూర్ణాంకం
gui_mch_సమితి_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    పూర్ణాంకం	    dx_enable = 0;
    పూర్ణాంకం	    dx_flags = 0;
    దశాంశం   dx_gamma = 0.0f;
    దశాంశం   dx_contrast = 0.0f;
    దశాంశం   dx_level = 0.0f;
    పూర్ణాంకం	    dx_geom = 0;
    పూర్ణాంకం	    dx_renmode = 0;
    పూర్ణాంకం	    dx_taamode = 0;
    // parse string as rendering options.
    కొరకు (p = s; p != ఖాళీ && *p != NUL; )
    {
	char_u  అంశం[256];
	char_u  name[128];
	char_u  విలువ[128];

	copy_option_part(&p, అంశం, పరిమాణం(అంశం), ",");
	ఒకవేళ (p == ఖాళీ)
	    ఆపు;
	q = &అంశం[0];
	copy_option_part(&q, name, పరిమాణం(name), ":");
	ఒకవేళ (q == ఖాళీ)
	    తిరిగి FAIL;
	copy_option_part(&q, విలువ, పరిమాణం(విలువ), ":");

	ఒకవేళ (STRCMP(name, "type") == 0)
	{
	    ఒకవేళ (STRCMP(విలువ, "directx") == 0)
		dx_enable = 1;
	    లేకపోతే
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (దశాంశం)atof((అక్షరం *)విలువ);
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (దశాంశం)atof((అక్షరం *)విలువ);
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (దశాంశం)atof((అక్షరం *)విలువ);
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((అక్షరం *)విలువ);
	    ఒకవేళ (dx_geom < 0 || dx_geom > 2)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((అక్షరం *)విలువ);
	    ఒకవేళ (dx_renmode < 0 || dx_renmode > 6)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((అక్షరం *)విలువ);
	    ఒకవేళ (dx_taamode < 0 || dx_taamode > 3)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	లేకపోతే
	    తిరిగి FAIL;
    }

    ఒకవేళ (!gui.లోపల_use)
	తిరిగి OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    ఒకవేళ (dx_enable)
    {
	ఒకవేళ (!directx_enabled())
	    తిరిగి FAIL;
	DWrite_Context_సమితి_Rendering_Params(s_dwc, ఖాళీ);
	ఒకవేళ (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    ఒకవేళ (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    ఒకవేళ (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    ఒకవేళ (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    ఒకవేళ (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    ఒకవేళ (dx_flags & (1 << 4))
		param.rendering_బహుళకం = dx_renmode;
	    ఒకవేళ (dx_flags & (1 << 5))
		param.పాఠ్యం_Antialias_బహుళకం = dx_taamode;
	    DWrite_Context_సమితి_Rendering_Params(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    తిరిగి OK;
# లేకపోతే
    తిరిగి FAIL;
# endif
}
#అయితే_ముగింపు

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#నిర్వచితం_కాకపోతే HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (పూర్ణాంకం)(పొట్టి)LOWORD(lParam), (పూర్ణాంకం)(పొట్టి)HIWORD(lParam), (UINT)(wParam)), 0L)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (పూర్ణాంకం)(పొట్టి)LOWORD(lParam), (పూర్ణాంకం)(పొట్టి)HIWORD(lParam), (UINT)(wParam)), 0L)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (పూర్ణాంకం)(పొట్టి)LOWORD(lParam), (పూర్ణాంకం)(పొట్టి)HIWORD(lParam), (UINT)(wParam)), 0L)
#అయితే_ముగింపు


#అన్తర్భావ "version.h"	// used by dialog box routine for default title
#నిర్వచితమైతే DEBUG
# include <tchar.h>
#అయితే_ముగింపు

#అన్తర్భావ <shellapi.h>
#అన్తర్భావ <commctrl.h>
#అన్తర్భావ <windowsx.h>

#నిర్వచితమైతే FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#అయితే_ముగింపు

// Some parameters for dialog boxes.  All in pixels.
#నిర్వచించు DLG_PADDING_X		10
#నిర్వచించు DLG_PADDING_Y		10
#నిర్వచించు DLG_VERT_PADDING_X	4	// For vertical buttons
#నిర్వచించు DLG_VERT_PADDING_Y	4
#నిర్వచించు DLG_ICON_వెడల్పు		34
#నిర్వచించు DLG_ICON_ఎత్తు		34
#నిర్వచించు DLG_MIN_వెడల్పు		150
#నిర్వచించు DLG_FONT_NAME		"MS Shell Dlg"
#నిర్వచించు DLG_FONT_బిందువు_కొలత	8
#నిర్వచించు DLG_MIN_MAX_వెడల్పు	400
#నిర్వచించు DLG_MIN_MAX_ఎత్తు	400

#నిర్వచించు DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#నిర్వచితం_కాకపోతే WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DWMWA_USE_IMMERSIVE_DARK_బహుళకం
# define DWMWA_USE_IMMERSIVE_DARK_బహుళకం	20
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DWMWA_పాఠ్యం_COLOR
# define DWMWA_పాఠ్యం_COLOR		36
#అయితే_ముగింపు

స్థిర శూన్య _OnPaint(HWND hwnd);
స్థిర శూన్య fill_rect(స్థిరరాశి RECT *rcp, HBRUSH hbr, COLORREF color);
స్థిర శూన్య clear_rect(RECT *rcp);

స్థిర మాట_2		s_dlgfntheight;		// height of the dialog font
స్థిర మాట_2		s_dlgfntwidth;		// width of the dialog font

#నిర్వచితమైతే FEAT_MENU
స్థిర HMENU		s_menuBar = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TEAROFF
స్థిర శూన్య rebuild_tearoff(vimmenu_T *menu);
స్థిర HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#అయితే_ముగింపు

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
స్థిర పూర్ణాంకం		s_busy_processing = FALSE;

స్థిర పూర్ణాంకం		destroying = FALSE;	// call DestroyWindow() ourselves

#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
స్థిర UINT		s_findrep_సందేశం = 0;
స్థిర FINDREPLACEW	s_findrep_struct;
స్థిర HWND		s_findrep_hwnd = ఖాళీ;
స్థిర పూర్ణాంకం		s_findrep_is_కనుగొను;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#అయితే_ముగింపు

HWND			s_hwnd = ఖాళీ;
స్థిర HDC		s_hdc = ఖాళీ;
స్థిర HBRUSH		s_brush = ఖాళీ;

#నిర్వచితమైతే FEAT_TOOLBAR
స్థిర HWND		s_toolbarhwnd = ఖాళీ;
స్థిర WNDPROC		s_toolbar_wndproc = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
స్థిర HWND		s_tabhwnd = ఖాళీ;
స్థిర WNDPROC		s_tabline_wndproc = ఖాళీ;
స్థిర పూర్ణాంకం		showing_tabline = 0;
#అయితే_ముగింపు

స్థిర WPARAM		s_wParam = 0;

స్థిర HWND		s_పాఠ్యం_Area = ఖాళీ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
స్థిర bool		s_textarea_bg_only = false;

స్థిర UINT		s_u_సందేశం = 0;

స్థిర char_u		*s_textfield; // Used by dialogs to pass back strings

స్థిర పూర్ణాంకం		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
స్థిర పూర్ణాంకం allow_scrollbar = FALSE;

#నిర్వచితం_కాకపోతే _DPI_AWARENESS_CONTEXTS_
రకనిర్వచనం HANDLE DPI_AWARENESS_CONTEXT;

రకనిర్వచనం గణన DPI_AWARENESS {
    DPI_AWARENESS_INVALID	    = -1,
    DPI_AWARENESS_UNAWARE	    = 0,
    DPI_AWARENESS_SYSTEM_AWARE	    = 1,
    DPI_AWARENESS_చొప్పున_MONITOR_AWARE = 2
} DPI_AWARENESS;

# define DPI_AWARENESS_CONTEXT_UNAWARE		    ((DPI_AWARENESS_CONTEXT)-1)
# define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE	    ((DPI_AWARENESS_CONTEXT)-2)
# define DPI_AWARENESS_CONTEXT_చొప్పున_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
# define DPI_AWARENESS_CONTEXT_చొప్పున_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
# define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
#అయితే_ముగింపు

#నిర్వచించు DEFAULT_DPI	96
స్థిర పూర్ణాంకం		s_dpi = DEFAULT_DPI;
స్థిర BOOL		s_లోపల_dpichanged = FALSE;
#నిర్వచితమైతే FEAT_MBYTE_IME
స్థిర DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#అయితే_ముగింపు
స్థిర RECT		s_suggested_rect;

స్థిర UINT (WINAPI *pGetDpiForSystem)(శూన్య) = ఖాళీ;
స్థిర UINT (WINAPI *p_Get_Dpi_For_కిటికీ)(HWND hwnd) = ఖాళీ;
స్థిర పూర్ణాంకం (WINAPI *pGetSystemMetricsForDpi)(పూర్ణాంకం, UINT) = ఖాళీ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
స్థిర DPI_AWARENESS_CONTEXT (WINAPI *p_సమితి_Thread_Dpi_Awareness_Context)(DPI_AWARENESS_CONTEXT dpiContext) = ఖాళీ;
#నిర్వచితమైతే FEAT_MBYTE_IME
స్థిర DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ఖాళీ;
#అయితే_ముగింపు

స్థిర HINSTANCE hLibDwm = ఖాళీ;
స్థిర HRESULT (WINAPI *p_Dwm_సమితి_కిటికీ_Attribute)(HWND, DWORD, LPCVOID, DWORD);
స్థిర శూన్య dyn_dwm_ఎక్కించు(శూన్య);

స్థిర పూర్ణాంకం fullscreen_on = FALSE;

#నిర్వచితమైతే FEAT_GUI_DARKTHEME

స్థిర HINSTANCE hUxThemeLib = ఖాళీ;
స్థిర DWORD (WINAPI *p_సమితి_Preferred_App_బహుళకం)(DWORD) = ఖాళీ;
స్థిర శూన్య (WINAPI *pFlushMenuThemes)(శూన్య) = ఖాళీ;
స్థిర శూన్య dyn_uxtheme_ఎక్కించు(శూన్య);
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం WINAPI
stubGetSystemMetricsForDpi(పూర్ణాంకం n_సూచీ, UINT dpi వాడని)
{
    తిరిగి GetSystemMetrics(n_సూచీ);
}

    స్థిర పూర్ణాంకం
adjust_fontsize_by_dpi(పూర్ణాంకం కొలత)
{
    తిరిగి కొలత * s_dpi / (పూర్ణాంకం)pGetDpiForSystem();
}

    స్థిర పూర్ణాంకం
adjust_by_system_dpi(పూర్ణాంకం కొలత)
{
    తిరిగి కొలత * (పూర్ణాంకం)pGetDpiForSystem() / DEFAULT_DPI;
}

#ఒకవేళ defined(FEAT_DIRECTX)
    స్థిర పూర్ణాంకం
directx_enabled(శూన్య)
{
    ఒకవేళ (s_dwc != ఖాళీ)
	తిరిగి 1;
    లేకపోతే ఒకవేళ (s_directx_ఎక్కించు_attempted)
	తిరిగి 0;
    // load DirectX
    DWrite_Init();
    s_directx_ఎక్కించు_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    తిరిగి s_dwc != ఖాళీ ? 1 : 0;
}

    స్థిర శూన్య
directx_binddc(శూన్య)
{
    ఒకవేళ (s_పాఠ్యం_Area == ఖాళీ)
	తిరిగి;

    RECT	rect;
    GetClientRect(s_పాఠ్యం_Area, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#అయితే_ముగింపు

బాహ్య పూర్ణాంకం current_font_ఎత్తు;	    // this is in os_mswin.c

స్థిర నిర్మాణం
{
    UINT    కీలకం_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_కీలకాలు[] =
{
    {VK_పైకి,		'k', 'u'},
    {VK_కిందికి,		'k', 'd'},
    {VK_ఎడమ,		'k', 'l'},
    {VK_కుడి,		'k', 'r'},

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
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#అయితే_ముగింపు
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_వెనుక,		'k', 'b'},
    {VK_చొప్పించు,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_చివర,		'@', '7'},
    {VK_పూర్వ,		'k', 'P'},
    {VK_తదుపరి,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_కూడు,		'K', '6'},
    {VK_వ్యవకలనం,	'K', '7'},
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
స్థిర పూర్ణాంకం	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
స్థిర పూర్ణాంకం	s_getting_focus = FALSE;

స్థిర పూర్ణాంకం	s_x_pending;
స్థిర పూర్ణాంకం	s_y_pending;
స్థిర UINT	s_kFlags_pending;
స్థిర UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
స్థిర పూర్ణాంకం	s_timed_బహిర్గతం = FALSE;
స్థిర పూర్ణాంకం	dead_కీలకం = DEAD_కీలకం_OFF;
స్థిర UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#నిర్వచితమైతే FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
స్థిర శూన్య Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
స్థిర శూన్య track_user_activity(UINT u_సందేశం);
#అయితే_ముగింపు

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#నిర్వచితమైతే FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
స్థిర LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
స్థిర LOGFONTW sub_logfont;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MBYTE_IME
స్థిర LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_దత్తాంశం);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BROWSE)
స్థిర char_u *convert_filter(char_u *s);
#అయితే_ముగింపు

#నిర్వచితమైతే DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    స్థిర శూన్య
print_windows_error(శూన్య)
{
    LPVOID  lp_సందేశం_Buf;

    Format_సందేశం(FORMAT_సందేశం_ALLOCATE_బఫరు | FORMAT_సందేశం_FROM_SYSTEM,
		  ఖాళీ, Get_చివరి_Error(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_సందేశం_Buf, 0, ఖాళీ);
    TRACE1("Error: %s\n", lp_సందేశం_Buf);
    LocalFree(lp_సందేశం_Buf);
}
#అయితే_ముగింపు

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#నిర్వచించు BLINK_NONE  0
#నిర్వచించు BLINK_OFF   1
#నిర్వచించు BLINK_ON    2

స్థిర పూర్ణాంకం		blink_state = BLINK_NONE;
స్థిర long_u		blink_waittime = 700;
స్థిర long_u		blink_ontime = 400;
స్థిర long_u		blink_offtime = 250;
స్థిర UINT_PTR		blink_timer = 0;

    పూర్ణాంకం
gui_mch_is_blinking(శూన్య)
{
    తిరిగి blink_state != BLINK_NONE;
}

    పూర్ణాంకం
gui_mch_is_blink_off(శూన్య)
{
    తిరిగి blink_state == BLINK_OFF;
}

    శూన్య
gui_mch_సమితి_blinking(దీర్ఘ wait, దీర్ఘ on, దీర్ఘ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    స్థిర VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_సందేశం వాడని,
    UINT_PTR idEvent,
    DWORD dw_సమయం వాడని)
{
    సందేశం_3 సందేశం;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ఖాళీ, idEvent);

    // Eat spurious WM_TIMER messages
    అయితే (Peek_సందేశం_W(&సందేశం, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    ఒకవేళ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = సమితి_Timer(ఖాళీ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    లేకపోతే
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = సమితి_Timer(ఖాళీ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    స్థిర శూన్య
gui_mswin_rm_blink_timer(శూన్య)
{
    సందేశం_3 సందేశం;

    ఒకవేళ (blink_timer == 0)
	తిరిగి;

    KillTimer(ఖాళీ, blink_timer);
    // Eat spurious WM_TIMER messages
    అయితే (Peek_సందేశం_W(&సందేశం, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    ఒకవేళ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    శూన్య
gui_mch_ప్రారంభం_blink(శూన్య)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
    {
	blink_timer = సమితి_Timer(ఖాళీ, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    స్థిర VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_సందేశం వాడని,
    UINT_PTR idEvent,
    DWORD dw_సమయం వాడని)
{
    సందేశం_3 సందేశం;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ఖాళీ, idEvent);
    s_timed_బహిర్గతం = TRUE;

    // Eat spurious WM_TIMER messages
    అయితే (Peek_సందేశం_W(&సందేశం, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    ఒకవేళ (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    స్థిర శూన్య
_OnDeadChar(
    HWND hwnd వాడని,
    UINT ch వాడని,
    పూర్ణాంకం cRepeat వాడని)
{
    dead_కీలకం = DEAD_కీలకం_సమితి_DEFAULT;
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
    స్థిర పూర్ణాంకం
char_గమ్యం_string(పూర్ణాంకం ch, char_u *string, పూర్ణాంకం slen, పూర్ణాంకం had_alt)
{
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    WCHAR	wstring[2];
    char_u	*ws = ఖాళీ;

    ఒకవేళ (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    లేకపోతే ఒకవేళ (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	తిరిగి 0;
    }
    లేకపోతే
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    ఒకవేళ (enc_codepage > 0)
    {
	len = Wide_Char_గమ్యం_Multi_Byte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ఖాళీ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	ఒకవేళ (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = Wide_Char_గమ్యం_Multi_Byte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ఖాళీ);
	    ఒకవేళ (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    లేకపోతే
    {
	ws = utf16_గమ్యం_enc(wstring, &len);
	ఒకవేళ (ws == ఖాళీ)
	    len = 0;
	లేకపోతే
	{
	    ఒకవేళ (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    ఒకవేళ (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    కొరకు (i = 0; i < len; ++i)
	ఒకవేళ (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (పూర్ణాంకం)KE_CSI;
	    len += 2;
	}

    తిరిగి len;
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
    స్థిర పూర్ణాంకం
get_చురుకైన_modifiers_experimental(శూన్య)
{
    పూర్ణాంకం modifiers = 0;

    ఒకవేళ (Get_కీలకం_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ముసుగు_CTRL;
    ఒకవేళ (Get_కీలకం_State(VK_జరుపు) & 0x8000)
	modifiers |= MOD_ముసుగు_జరుపు;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    ఒకవేళ (Get_కీలకం_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_ముసుగు_ALT;
    ఒకవేళ ((modifiers & MOD_ముసుగు_CTRL) && (Get_కీలకం_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_ముసుగు_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    ఒకవేళ (!(Get_కీలకం_State(VK_CONTROL) & 0x8000)
	    &&  (Get_కీలకం_State(VK_RMENU) & 0x8000)
	    && !(Get_కీలకం_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_ముసుగు_ALT;

    తిరిగి modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    స్థిర పూర్ణాంకం
get_చురుకైన_modifiers_classic(శూన్య)
{
    పూర్ణాంకం modifiers = 0;

    ఒకవేళ (Get_కీలకం_State(VK_జరుపు) & 0x8000)
	modifiers |= MOD_ముసుగు_జరుపు;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    ఒకవేళ (Get_కీలకం_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ముసుగు_CTRL;
    ఒకవేళ (Get_కీలకం_State(VK_MENU) & 0x8000)
	modifiers |= MOD_ముసుగు_ALT;

    తిరిగి modifiers;
}

    స్థిర పూర్ణాంకం
get_చురుకైన_modifiers(శూన్య)
{
    తిరిగి get_చురుకైన_modifiers_experimental();
}

    స్థిర పూర్ణాంకం
get_చురుకైన_modifiers_ద్వారా_ptr(శూన్య)
{
    // marshal to corresponding implementation
    తిరిగి keycode_trans_strategy_వాడిన->ptr_get_చురుకైన_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    స్థిర శూన్య
_OnChar(
    HWND hwnd వాడని,
    UINT cch,
    పూర్ణాంకం cRepeat వాడని)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_వాడిన->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    స్థిర శూన్య
_OnChar_experimental(
    HWND hwnd వాడని,
    UINT cch,
    పూర్ణాంకం cRepeat వాడని)
{
    char_u	string[40];
    పూర్ణాంకం		len = 0;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		ch = cch;   // special keys are negative

    ఒకవేళ (dead_కీలకం == DEAD_కీలకం_SKIP_ON_CHAR)
	తిరిగి;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    ఒకవేళ (dead_కీలకం != DEAD_కీలకం_TRANSIENT_లోపల_ON_CHAR)
	dead_కీలకం = DEAD_కీలకం_OFF;

    modifiers = get_చురుకైన_modifiers_experimental();

    ch = simplify_కీలకం(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_కీలకాలు;
    ch = may_adjust_కీలకం_for_ctrl(modifiers, ch);
    --no_reduce_కీలకాలు;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_జరుపు_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = వెలికితీయి_modifiers(ch, &modifiers, FALSE, ఖాళీ);
    ఒకవేళ (ch == CSI)
	ch = K_CSI;

    ఒకవేళ (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	కూడు_గమ్యం_ఆదానం_buf(string, 3);
    }

    len = char_గమ్యం_string(ch, string, 40, FALSE);
    ఒకవేళ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ఆదానం_buf();
	got_int = TRUE;
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    స్థిర శూన్య
_OnChar_classic(
    HWND hwnd వాడని,
    UINT ch,
    పూర్ణాంకం cRepeat వాడని)
{
    char_u	string[40];
    పూర్ణాంకం		len = 0;

    dead_కీలకం = 0;

    len = char_గమ్యం_string(ch, string, 40, FALSE);
    ఒకవేళ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ఆదానం_buf();
	got_int = TRUE;
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    స్థిర శూన్య
_OnSysChar(
    HWND hwnd వాడని,
    UINT cch,
    పూర్ణాంకం cRepeat వాడని)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_వాడిన->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    స్థిర శూన్య
_OnSysChar_experimental(
    HWND hwnd వాడని,
    UINT cch,
    పూర్ణాంకం cRepeat వాడని)
{
    char_u	string[40]; // Enough for multibyte character
    పూర్ణాంకం		len;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		ch = cch;   // special keys are negative

    dead_కీలకం = DEAD_కీలకం_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_చురుకైన_modifiers_experimental();
    ch = simplify_కీలకం(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_జరుపు_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = వెలికితీయి_modifiers(ch, &modifiers, FALSE, ఖాళీ);
    ఒకవేళ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ఒకవేళ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ఒకవేళ (IS_SPECIAL((పూర్ణాంకం)ch))
    {
	string[len++] = CSI;
	string[len++] = K_రెండవ((పూర్ణాంకం)ch);
	string[len++] = K_మూడవ((పూర్ణాంకం)ch);
    }
    లేకపోతే
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_గమ్యం_string(ch, string + len, 40 - len, TRUE);
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    స్థిర శూన్య
_OnSysChar_classic(
    HWND hwnd వాడని,
    UINT cch,
    పూర్ణాంకం cRepeat వాడని)
{
    char_u	string[40]; // Enough for multibyte character
    పూర్ణాంకం		len;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		ch = cch;   // special keys are negative

    dead_కీలకం = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_ముసుగు_ALT;
    ఒకవేళ (Get_కీలకం_State(VK_జరుపు) & 0x8000)
	modifiers |= MOD_ముసుగు_జరుపు;
    ఒకవేళ (Get_కీలకం_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ముసుగు_CTRL;

    ch = simplify_కీలకం(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_జరుపు_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = వెలికితీయి_modifiers(ch, &modifiers, FALSE, ఖాళీ);
    ఒకవేళ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ఒకవేళ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ఒకవేళ (IS_SPECIAL((పూర్ణాంకం)ch))
    {
	string[len++] = CSI;
	string[len++] = K_రెండవ((పూర్ణాంకం)ch);
	string[len++] = K_మూడవ((పూర్ణాంకం)ch);
    }
    లేకపోతే
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_గమ్యం_string(ch, string + len, 40 - len, TRUE);
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);
}

    స్థిర శూన్య
_OnMouseEvent(
    పూర్ణాంకం button,
    పూర్ణాంకం x,
    పూర్ణాంకం y,
    పూర్ణాంకం repeated_click,
    UINT కీలకం_Flags)
{
    పూర్ణాంకం vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    ఒకవేళ (కీలకం_Flags & MK_జరుపు)
	vim_modifiers |= MOUSE_జరుపు;
    ఒకవేళ (కీలకం_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    ఒకవేళ (Get_కీలకం_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    స్థిర శూన్య
On_Mouse_Button_కిందికి(
    HWND hwnd వాడని,
    BOOL fDoubleClick వాడని,
    పూర్ణాంకం x,
    పూర్ణాంకం y,
    UINT కీలకం_Flags)
{
    స్థిర LONG	s_prev_సమయం = 0;

    LONG    current_సమయం = Get_సందేశం_సమయం();
    పూర్ణాంకం	    button = -1;
    పూర్ణాంకం	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (శూన్య)సమితి_Focus(s_hwnd);

    ఒకవేళ (s_u_సందేశం == WM_LBUTTONDOWN || s_u_సందేశం == WM_LBUTTONDBLCLK)
	button = MOUSE_ఎడమ;
    లేకపోతే ఒకవేళ (s_u_సందేశం == WM_MBUTTONDOWN || s_u_సందేశం == WM_MBUTTONDBLCLK)
	button = MOUSE_మధ్య;
    లేకపోతే ఒకవేళ (s_u_సందేశం == WM_RBUTTONDOWN || s_u_సందేశం == WM_RBUTTONDBLCLK)
	button = MOUSE_కుడి;
    లేకపోతే ఒకవేళ (s_u_సందేశం == WM_XBUTTONDOWN || s_u_సందేశం == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    లేకపోతే ఒకవేళ (s_u_సందేశం == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	ఒకవేళ (s_button_pending == MOUSE_ఎడమ)
	    button = MOUSE_కుడి;
	లేకపోతే
	    button = MOUSE_ఎడమ;
    }

    ఒకవేళ (button < 0)
	తిరిగి;

    repeated_click = ((పూర్ణాంకం)(current_సమయం - s_prev_సమయం) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    ఒకవేళ (repeated_click
	    && ((button == MOUSE_ఎడమ && s_button_pending == MOUSE_కుడి)
		|| (button == MOUSE_కుడి
		    && s_button_pending == MOUSE_ఎడమ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_మధ్య;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, కీలకం_Flags);
    }
    లేకపోతే ఒకవేళ ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_కుడి)))
    {
	ఒకవేళ (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, కీలకం_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, కీలకం_Flags);
    }
    లేకపోతే
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
	s_kFlags_pending = కీలకం_Flags;
    }

    s_prev_సమయం = current_సమయం;
}

    స్థిర శూన్య
_OnMouseMoveOrRelease(
    HWND hwnd వాడని,
    పూర్ణాంకం x,
    పూర్ణాంకం y,
    UINT కీలకం_Flags)
{
    పూర్ణాంకం button;

    s_getting_focus = FALSE;
    ఒకవేళ (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    ఒకవేళ (s_u_సందేశం == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	ఒకవేళ (!(కీలకం_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    తిరిగి;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	సమితి_Capture(s_పాఠ్యం_Area);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    లేకపోతే
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, కీలకం_Flags);
}

    స్థిర శూన్య
On_కొలత_పాఠ్యం_Area(
    HWND hwnd వాడని,
    UINT state వాడని,
    పూర్ణాంకం cx వాడని,
    పూర్ణాంకం cy వాడని)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	directx_binddc();
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    స్థిర vimmenu_T *
gui_mswin_కనుగొను_menu(
    vimmenu_T	*pMenu,
    పూర్ణాంకం		id)
{
    vimmenu_T	*p_బిడ్డ_Menu;

    అయితే (pMenu)
    {
	ఒకవేళ (pMenu->id == (UINT)id)
	    ఆపు;
	ఒకవేళ (pMenu->children != ఖాళీ)
	{
	    p_బిడ్డ_Menu = gui_mswin_కనుగొను_menu(pMenu->children, id);
	    ఒకవేళ (p_బిడ్డ_Menu)
	    {
		pMenu = p_బిడ్డ_Menu;
		ఆపు;
	    }
	}
	pMenu = pMenu->తదుపరి;
    }
    తిరిగి pMenu;
}

    స్థిర శూన్య
_OnMenu(
    HWND	hwnd వాడని,
    పూర్ణాంకం		id,
    HWND	hwndCtl వాడని,
    UINT	సంకేతం_Notify వాడని)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_కనుగొను_menu(మూలం_menu, id);
    ఒకవేళ (pMenu)
	gui_menu_cb(pMenu);
}
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    స్థిర శూన్య
On_కనుగొను_Repl(శూన్య)
{
    పూర్ణాంకం	    flags = 0;
    పూర్ణాంకం	    కిందికి;

    ఒకవేళ (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(శూన్య)సమితి_Focus(s_hwnd);

    ఒకవేళ (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(శూన్య)సమితి_Focus(s_hwnd);
    }
    లేకపోతే ఒకవేళ (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(శూన్య)సమితి_Focus(s_hwnd);
    }
    లేకపోతే ఒకవేళ (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    ఒకవేళ (flags == 0)
	తిరిగి;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    ఒకవేళ (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_సంపూర్ణ_మాట;
    ఒకవేళ (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_సరిపోలిక_CASE;
    కిందికి = (s_findrep_struct.Flags & FR_కిందికి) != 0;
    p = utf16_గమ్యం_enc(s_findrep_struct.lpstr_కనుగొను_What, ఖాళీ);
    q = utf16_గమ్యం_enc(s_findrep_struct.lpstrReplaceWith, ఖాళీ);
    ఒకవేళ (p != ఖాళీ && q != ఖాళీ)
	gui_do_findrepl(flags, p, q, కిందికి);
    vim_free(p);
    vim_free(q);
}
#అయితే_ముగింపు

    స్థిర శూన్య
HandleMouseHide(UINT u_సందేశం, LPARAM lParam)
{
    స్థిర LPARAM చివరి_l_Param = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    ఒకవేళ (u_సందేశం == WM_MOUSEMOVE || u_సందేశం == WM_NCMOUSEMOVE)
    {
	ఒకవేళ (lParam == చివరి_l_Param)
	    తిరిగి;
	చివరి_l_Param = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    ఎంపిక (u_సందేశం)
    {
    స్థితి WM_KEYUP:
    స్థితి WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	ఒకవేళ (p_mh)
	    gui_mch_mousehide(TRUE);
	ఆపు;

    స్థితి WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    స్థితి WM_SYSCHAR:
    స్థితి WM_MOUSEMOVE:	 // show the pointer on any mouse action
    స్థితి WM_LBUTTONDOWN:
    స్థితి WM_LBUTTONUP:
    స్థితి WM_MBUTTONDOWN:
    స్థితి WM_MBUTTONUP:
    స్థితి WM_RBUTTONDOWN:
    స్థితి WM_RBUTTONUP:
    స్థితి WM_XBUTTONDOWN:
    స్థితి WM_XBUTTONUP:
    స్థితి WM_NCMOUSEMOVE:
    స్థితి WM_NCLBUTTONDOWN:
    స్థితి WM_NCLBUTTONUP:
    స్థితి WM_NCMBUTTONDOWN:
    స్థితి WM_NCMBUTTONUP:
    స్థితి WM_NCRBUTTONDOWN:
    స్థితి WM_NCRBUTTONUP:
    స్థితి WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	ఆపు;
    }
}

    స్థిర LRESULT CALLBACK
పాఠ్యం_Area_Wnd_Proc(
    HWND hwnd,
    UINT u_సందేశం,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_సందేశం, lParam);

    s_u_సందేశం = u_సందేశం;
    s_wParam = wParam;

#నిర్వచితమైతే FEAT_BEVAL_GUI
    track_user_activity(u_సందేశం);
#అయితే_ముగింపు

    ఎంపిక (u_సందేశం)
    {
	HANDLE_సందేశం(hwnd, WM_LBUTTONDBLCLK,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_LBUTTONDOWN,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_సందేశం(hwnd, WM_MBUTTONDBLCLK,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_MBUTTONDOWN,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_సందేశం(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_సందేశం(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_సందేశం(hwnd, WM_RBUTTONDBLCLK,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_RBUTTONDOWN,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_సందేశం(hwnd, WM_XBUTTONDBLCLK,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_XBUTTONDOWN,On_Mouse_Button_కిందికి);
	HANDLE_సందేశం(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_సందేశం(hwnd, WM_కొలత,	On_కొలత_పాఠ్యం_Area);

#నిర్వచితమైతే FEAT_BEVAL_GUI
	స్థితి WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    తిరిగి TRUE;
#అయితే_ముగింపు
	అప్రమేయ:
	    తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
    }
}

    స్థిర శూన్య
dyn_dwm_ఎక్కించు(శూన్య)
{
    hLibDwm = vim_ఎక్కించు_Lib("dwmapi.dll");
    ఒకవేళ (hLibDwm == ఖాళీ)
	తిరిగి;

    p_Dwm_సమితి_కిటికీ_Attribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	Get_Proc_చిరునామా(hLibDwm, "DwmSetWindowAttribute");

    ఒకవేళ (p_Dwm_సమితి_కిటికీ_Attribute == ఖాళీ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ఖాళీ;
	తిరిగి;
    }
}

బాహ్య DWORD win_సంస్కరణ; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    శూన్య
gui_mch_సమితి_titlebar_colors(శూన్య)
{
#నిర్వచించు DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    ఒకవేళ (p_Dwm_సమితి_కిటికీ_Attribute == ఖాళీ || win_సంస్కరణ < MAKE_VER(10, 0, 22000))
	తిరిగి;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T పాఠ్యం_Color = DWMWA_COLOR_DEFAULT;

    ఒకవేళ (vim_strchr(p_go, GO_TITLEBAR) != ఖాళీ)
    {
	ఒకవేళ (gui.లోపల_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    పాఠ్యం_Color = gui.title_fg_pixel;
	}
	లేకపోతే
	{
	    captionColor = gui.titlenc_bg_pixel;
	    పాఠ్యం_Color = gui.titlenc_fg_pixel;
	}

	ఒకవేళ (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	ఒకవేళ (పాఠ్యం_Color == INVALCOLOR)
	    పాఠ్యం_Color = DWMWA_COLOR_DEFAULT;
    }

    p_Dwm_సమితి_కిటికీ_Attribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, పరిమాణం(captionColor));
    p_Dwm_సమితి_కిటికీ_Attribute(s_hwnd, DWMWA_పాఠ్యం_COLOR,
	    &పాఠ్యం_Color, పరిమాణం(పాఠ్యం_Color));
}

/*
 * Called when the foreground or background color has been changed.
 */
    శూన్య
gui_mch_new_colors(శూన్య)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.వెనుక_pixel);
    prevBrush = (HBRUSH)సమితి_Class_Long_Ptr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ఖాళీ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    ఒకవేళ (s_textarea_bg_only && s_పాఠ్యం_Area != ఖాళీ)
	InvalidateRect(s_పాఠ్యం_Area, ఖాళీ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    శూన్య
gui_mch_def_colors(శూన్య)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.వెనుక_pixel = GetSysColor(COLOR_కిటికీ);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_వెనుక_pixel = gui.వెనుక_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    పూర్ణాంకం
gui_mch_open(శూన్య)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    ఒకవేళ (!Is_కిటికీ_Visible(s_hwnd))
    {
	Show_కిటికీ(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	ఒకవేళ (s_పాఠ్యం_Area != ఖాళీ)
	{
	    Update_కిటికీ(s_పాఠ్యం_Area);
	    gui_mch_flush();
	}
    }

#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    RECT    rect;

    Get_కిటికీ_Rect(s_hwnd, &rect);
    *x = rect.ఎడమ_2;
    *y = rect.శిఖరం;
    తిరిగి OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    సమితి_కిటికీ_Pos(s_hwnd, ఖాళీ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    స్థిర పూర్ణాంకం oldx = 0;
    స్థిర పూర్ణాంకం oldy = 0;

    సమితి_కిటికీ_Pos(s_పాఠ్యం_Area, ఖాళీ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#నిర్వచితమైతే FEAT_TOOLBAR
    ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
	Send_సందేశం(s_toolbarhwnd, WM_కొలత,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_ఎత్తు));
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_TABLINE)
    ఒకవేళ (showing_tabline)
    {
	పూర్ణాంకం	శిఖరం = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
	    శిఖరం = gui.toolbar_ఎత్తు;
# endif
	GetClientRect(s_hwnd, &rect);
	Move_కిటికీ(s_tabhwnd, 0, శిఖరం, rect.కుడి_2, gui.tabline_ఎత్తు, TRUE);
    }
#అయితే_ముగింపు

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    ఒకవేళ (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ఖాళీ, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    శూన్య
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    పూర్ణాంకం		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    శూన్య
gui_mch_సమితి_scrollbar_pos(
    scrollbar_T *sb,
    పూర్ణాంకం		x,
    పూర్ణాంకం		y,
    పూర్ణాంకం		w,
    పూర్ణాంకం		h)
{
    సమితి_కిటికీ_Pos(sb->id, ఖాళీ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    RECT    rcTxt, rcWnd;
    పూర్ణాంకం	    xpad;

    Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rcTxt);
    Get_కిటికీ_Rect(s_hwnd, &rcWnd);
    xpad = rcWnd.కుడి_2 - rcTxt.కుడి_2 - gui.scrollbar_వెడల్పు
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    తిరిగి (xpad < 0) ? 0 : xpad;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    RECT    rcTxt, rcWnd;
    పూర్ణాంకం	    ypad;

    Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rcTxt);
    Get_కిటికీ_Rect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_ఎత్తు
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    తిరిగి (ypad < 0) ? 0 : ypad;
}

    శూన్య
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    పూర్ణాంకం		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = Create_కిటికీ(
	"SCROLLBAR", "Scrollbar",
	WS_బిడ్డ | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ఖాళీ,
	g_hinst, ఖాళీ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    స్థిర scrollbar_T *
gui_mswin_కనుగొను_scrollbar(HWND hwnd)
{
    win_T	*wp;

    ఒకవేళ (gui.bottom_sbar.id == hwnd)
	తిరిగి &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ఒకవేళ (wp->w_scrollbars[SBAR_ఎడమ].id == hwnd)
	    తిరిగి &wp->w_scrollbars[SBAR_ఎడమ];
	ఒకవేళ (wp->w_scrollbars[SBAR_కుడి].id == hwnd)
	    తిరిగి &wp->w_scrollbars[SBAR_కుడి];
    }
    తిరిగి ఖాళీ;
}

    స్థిర శూన్య
update_scrollbar_కొలత(శూన్య)
{
    gui.scrollbar_వెడల్పు = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_ఎత్తు = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    స్థిర శూన్య
Get_సరాసరి_Font_కొలత(HDC hdc, కొలత_3 *కొలత)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    Get_పాఠ్యం_Extent_బిందువు(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, కొలత);
    కొలత->cx = (కొలత->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    స్థిర శూన్య
Get_Font_కొలత(GuiFont font, పూర్ణాంకం *char_వెడల్పు, పూర్ణాంకం *char_ఎత్తు)
{
    HWND    hwnd = Get_Desktop_కిటికీ();
    HDC	    hdc = Get_కిటికీ_DC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    కొలత_3    కొలత;
    TEXTMETRIC tm;

    Get_పాఠ్యం_Metrics(hdc, &tm);
    Get_సరాసరి_Font_కొలత(hdc, &కొలత);

    ఒకవేళ (char_వెడల్పు)
	*char_వెడల్పు = కొలత.cx + tm.tmOverhang;
    ఒకవేళ (char_ఎత్తు)
	*char_ఎత్తు = tm.tm_ఎత్తు + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    స్థిర శూన్య
Update_Font_కొలత(GuiFont font)
{
    Get_Font_కొలత(font, &gui.char_వెడల్పు, &gui.char_ఎత్తు);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    పూర్ణాంకం
gui_mch_adjust_charheight(శూన్య)
{
    Update_Font_కొలత(gui.norm_font);
    తిరిగి OK;
}

    స్థిర GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ఖాళీ;

    // Load the font
    font = Create_Font_పరోక్ష_W(lf);

    ఒకవేళ (font == ఖాళీ)
	తిరిగి NOFONT;

    తిరిగి (GuiFont)font;
}

    స్థిర పూర్ణాంకం
pixels_గమ్యం_points(పూర్ణాంకం pixels, పూర్ణాంకం vertical)
{
    పూర్ణాంకం		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = Get_Desktop_కిటికీ();
    hdc = Get_కిటికీ_DC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    తిరిగి points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    పూర్ణాంకం		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    ఒకవేళ (get_logfont(&lf, name, ఖాళీ, giveErrorIfMissing) == OK)
    {
	lf.lf_ఎత్తు = adjust_fontsize_by_dpi(lf.lf_ఎత్తు);
	font = get_font_handle(&lf);
    }
    ఒకవేళ (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    తిరిగి font;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font వాడని, char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strsave(name);
}
#అయితే_ముగింపు

    శూన్య
gui_mch_free_font(GuiFont font)
{
    ఒకవేళ (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    పూర్ణాంకం i;

    రకనిర్వచనం నిర్మాణం Sys_Color_పట్టిక
    {
	అక్షరం	    *name;
	పూర్ణాంకం	    color;
    } Sys_Color_పట్టిక;

    స్థిర Sys_Color_పట్టిక sys_పట్టిక[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#నిర్వచితమైతే COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#అయితే_ముగింపు
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
	{"SYS_WINDOW", COLOR_కిటికీ},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    కొరకు (i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(sys_పట్టిక); i++)
	ఒకవేళ (STRICMP(name, sys_పట్టిక[i].name) == 0)
	    తిరిగి GetSysColor(sys_పట్టిక[i].color);

    తిరిగి gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    తిరిగి gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    పూర్ణాంకం
gui_mch_haskey(char_u *name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; special_కీలకాలు[i].vim_code1 != NUL; i++)
	ఒకవేళ (name[0] == special_కీలకాలు[i].vim_code0
				       && name[1] == special_కీలకాలు[i].vim_code1)
	    తిరిగి OK;
    తిరిగి FAIL;
}

    శూన్య
gui_mch_beep(శూన్య)
{
    సందేశం_Beep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    శూన్య
gui_mch_invert_rectangle(
    పూర్ణాంకం	    r,
    పూర్ణాంకం	    c,
    పూర్ణాంకం	    nr,
    పూర్ణాంకం	    nc)
{
    RECT    rc;

#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ఎడమ_2 = FILL_X(c);
    rc.శిఖరం = FILL_Y(r);
    rc.కుడి_2 = rc.ఎడమ_2 + nc * gui.char_వెడల్పు;
    rc.bottom = rc.శిఖరం + nr * gui.char_ఎత్తు;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    శూన్య
gui_mch_iconify(శూన్య)
{
    Show_కిటికీ(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    శూన్య
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ఎడమ_2 = FILL_X(gui.నిలువు);
    rc.శిఖరం = FILL_Y(gui.అడ్డువరుస);
    rc.కుడి_2 = rc.ఎడమ_2 + gui.char_వెడల్పు;
    ఒకవేళ (mb_lefthalve(gui.అడ్డువరుస, gui.నిలువు))
	rc.కుడి_2 += gui.char_వెడల్పు;
    rc.bottom = rc.శిఖరం + gui.char_ఎత్తు;
    hbr = CreateSolidBrush(color);
    చట్రం_Rect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    శూన్య
gui_mch_draw_part_cursor(
    పూర్ణాంకం		w,
    పూర్ణాంకం		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ఎడమ_2 =
#నిర్వచితమైతే FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_కుడి ? FILL_X(gui.నిలువు + 1) - w :
#అయితే_ముగింపు
		    FILL_X(gui.నిలువు);
    rc.శిఖరం = FILL_Y(gui.అడ్డువరుస) + gui.char_ఎత్తు - h;
    rc.కుడి_2 = rc.ఎడమ_2 + w;
    rc.bottom = rc.శిఖరం + h;

    fill_rect(&rc, ఖాళీ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    స్థిర శూన్య
output_Dead_కీలకం_re_Post_Ex(సందేశం_3 original_సందేశం, పూర్ణాంకం dead_key2set)
{
    స్థిర సందేశం_3 deadCharExpel;

    ఒకవేళ (dead_కీలకం == DEAD_కీలకం_OFF)
	తిరిగి;

    dead_కీలకం = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.సందేశం_2 = original_సందేశం.సందేశం_2;
    deadCharExpel.hwnd    = original_సందేశం.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    Translate_సందేశం(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    Post_సందేశం(original_సందేశం.hwnd, original_సందేశం.సందేశం_2, original_సందేశం.wParam,
							  original_సందేశం.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    స్థిర శూన్య
output_Dead_కీలకం_re_Post(సందేశం_3 original_సందేశం)
{
    output_Dead_కీలకం_re_Post_Ex(original_సందేశం, DEAD_కీలకం_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
స్థిర శూన్య process_సందేశం_usual_కీలకం(UINT vk, స్థిరరాశి సందేశం_3 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_వాడిన->ptr_process_సందేశం_usual_కీలకం(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    స్థిర శూన్య
process_సందేశం_usual_కీలకం_experimental(UINT vk, స్థిరరాశి సందేశం_3 *pmsg)
{
    WCHAR	ch[8];
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    UINT	పరిశీలన_సంకేతం;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    ఒకవేళ (Get_కీలకం_State(VK_జరుపు) & 0x8000)
	keyboard_state[VK_జరుపు] = 0x80;
    ఒకవేళ (Get_కీలకం_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    ఒకవేళ ((Get_కీలకం_State(VK_RMENU) & 0x8000)
				 && (Get_కీలకం_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    పరిశీలన_సంకేతం = Map_Virtual_కీలకం(vk, MAPVK_VK_గమ్యం_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = గమ్యం_Unicode(vk, పరిశీలన_సంకేతం, keyboard_state, ch, ARRAY_నిడివి(ch),
	    0);
    ఒకవేళ (len < 0)
	dead_కీలకం = DEAD_కీలకం_సమితి_DEFAULT;

    ఒకవేళ (len <= 0)
    {
	పూర్ణాంకం wm_char = NUL;

	ఒకవేళ (dead_కీలకం == DEAD_కీలకం_సమితి_DEFAULT
		&& (Get_కీలకం_State(VK_CONTROL) & 0x8000))
	{
	    ఒకవేళ (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && పరిశీలన_సంకేతం == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && పరిశీలన_సంకేతం == 41))
		wm_char = '[';
	    ఒకవేళ (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && పరిశీలన_సంకేతం == 27))
		wm_char = ']';
	}
	ఒకవేళ (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    Post_సందేశం_W(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_కీలకం = DEAD_కీలకం_TRANSIENT_లోపల_ON_CHAR;
	}
	తిరిగి;
    }

    // Post the message as TranslateMessage would do.
    ఒకవేళ (pmsg->సందేశం_2 == WM_KEYDOWN)
    {
	కొరకు (i = 0; i < len; i++)
	    Post_సందేశం_W(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    లేకపోతే
    {
	కొరకు (i = 0; i < len; i++)
	    Post_సందేశం_W(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    స్థిర శూన్య
process_సందేశం_usual_కీలకం_classic(UINT vk, స్థిరరాశి సందేశం_3 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    ఒకవేళ (vk != 0xff
	    && (Get_కీలకం_State(VK_CONTROL) & 0x8000)
	    && !(Get_కీలకం_State(VK_జరుపు) & 0x8000)
	    && !(Get_కీలకం_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	ఒకవేళ (vk == '6' || Map_Virtual_కీలకం(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    కూడు_గమ్యం_ఆదానం_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	లేకపోతే ఒకవేళ (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    కూడు_గమ్యం_ఆదానం_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	లేకపోతే ఒకవేళ (vk == '2' || Map_Virtual_కీలకం(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    కూడు_గమ్యం_ఆదానం_buf(string, 1);
	}
	లేకపోతే
	    Translate_సందేశం(pmsg);
    }
    లేకపోతే
	Translate_సందేశం(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    స్థిర శూన్య
process_సందేశం(శూన్య)
{
    సందేశం_3		సందేశం;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    పూర్ణాంకం		i;
    పూర్ణాంకం		modifiers = 0;
    పూర్ణాంకం		కీలకం;
#నిర్వచితమైతే FEAT_MENU
    స్థిర char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#అయితే_ముగింపు
    స్థిర పూర్ణాంకం  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    ఒకవేళ (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    Get_సందేశం_W(&సందేశం, ఖాళీ, 0, 0);

#నిర్వచితమైతే FEAT_OLE
    // Look after OLE Automation commands
    ఒకవేళ (సందేశం.సందేశం_2 == WM_OLE)
    {
	char_u *str = (char_u *)సందేశం.lParam;
	ఒకవేళ (str == ఖాళీ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    Dispatch_సందేశం_W(&సందేశం);
	}
	లేకపోతే
	{
	    కూడు_గమ్యం_ఆదానం_buf(str, (పూర్ణాంకం)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	తిరిగి;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    // Don't process messages used by the dialog
    ఒకవేళ (s_findrep_hwnd != ఖాళీ && Is_Dialog_సందేశం_W(s_findrep_hwnd, &సందేశం))
    {
	HandleMouseHide(సందేశం.సందేశం_2, సందేశం.lParam);
	తిరిగి;
    }
#అయితే_ముగింపు

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    ఒకవేళ (సందేశం.సందేశం_2 == WM_KEYDOWN || సందేశం.సందేశం_2 == WM_SYSKEYDOWN)
    {
	vk = (పూర్ణాంకం) సందేశం.wParam;

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
	ఒకవేళ (keycode_trans_strategy_వాడిన->is_experimental()
		&& dead_కీలకం == DEAD_కీలకం_SKIP_ON_CHAR)
	{
	    dead_కీలకం = DEAD_కీలకం_OFF;
	}

	ఒకవేళ (dead_కీలకం != DEAD_కీలకం_OFF)
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
	    ఒకవేళ (keycode_trans_strategy_వాడిన->is_experimental() &&
		    dead_కీలకం == DEAD_కీలకం_TRANSIENT_లోపల_ON_CHAR)
	    {
		output_Dead_కీలకం_re_Post_Ex(సందేశం,
				       /*dead_key2set=*/DEAD_కీలకం_SKIP_ON_CHAR);
		తిరిగి;
	    }

	    ఒకవేళ (dead_కీలకం != DEAD_కీలకం_సమితి_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		తిరిగి;
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
	    ఒకవేళ ((vk == VK_SPACE || vk == VK_వెనుక || vk == VK_ESCAPE))
	    {
		dead_కీలకం = DEAD_కీలకం_OFF;
		Translate_సందేశం(&సందేశం);
		తిరిగి;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    లేకపోతే ఒకవేళ ((get_real_state()
			    & (బహుళకం_చొప్పించు | బహుళకం_CMDLINE | బహుళకం_SELECT)) == 0)
	    {
		output_Dead_కీలకం_re_Post(సందేశం);
		తిరిగి;
	    }
	}

	// Check for CTRL-BREAK
	ఒకవేళ (vk == VK_CANCEL)
	{
	    trash_ఆదానం_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    కూడు_గమ్యం_ఆదానం_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	ఒకవేళ (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    Translate_సందేశం(&సందేశం);
	    తిరిగి;
	}

	కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == vk
		    && (vk != VK_SPACE || !(Get_కీలకం_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		ఒకవేళ (dead_కీలకం == DEAD_కీలకం_సమితి_DEFAULT
			&& (special_కీలకాలు[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_కీలకం_re_Post(సందేశం);
		    తిరిగి;
		}

#నిర్వచితమైతే FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		ఒకవేళ (vk == VK_F10
			&& gui.menu_is_చురుకైన
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ఖాళీ, ఖాళీ) == ఖాళీ)
		    ఆపు;
#అయితే_ముగింపు
		modifiers = get_చురుకైన_modifiers_ద్వారా_ptr();

		ఒకవేళ (special_కీలకాలు[i].vim_code1 == NUL)
		    కీలకం = special_కీలకాలు[i].vim_code0;
		లేకపోతే
		    కీలకం = గమ్యం_SPECIAL(special_కీలకాలు[i].vim_code0,
						   special_కీలకాలు[i].vim_code1);
		కీలకం = simplify_కీలకం(కీలకం, &modifiers);
		ఒకవేళ (కీలకం == CSI)
		    కీలకం = K_CSI;

		ఒకవేళ (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    కూడు_గమ్యం_ఆదానం_buf(string, 3);
		}

		ఒకవేళ (IS_SPECIAL(కీలకం))
		{
		    string[0] = CSI;
		    string[1] = K_రెండవ(కీలకం);
		    string[2] = K_మూడవ(కీలకం);
		    కూడు_గమ్యం_ఆదానం_buf(string, 3);
		}
		లేకపోతే
		{
		    పూర్ణాంకం	len;

		    // Handle "key" as a Unicode character.
		    len = char_గమ్యం_string(కీలకం, string, 40, FALSE);
		    కూడు_గమ్యం_ఆదానం_buf(string, len);
		}
		ఆపు;
	    }
	}

	// Not a special key.
	ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == 0)
	{
	    process_సందేశం_usual_కీలకం(vk, &సందేశం);
	}
    }
#నిర్వచితమైతే FEAT_MBYTE_IME
    లేకపోతే ఒకవేళ (సందేశం.సందేశం_2 == WM_IME_NOTIFY)
	_OnImeNotify(సందేశం.hwnd, (DWORD)సందేశం.wParam, (DWORD)సందేశం.lParam);
    లేకపోతే ఒకవేళ (సందేశం.సందేశం_2 == WM_KEYUP && im_get_స్థితి())
	// added for non-MS IME (Yasuhiro Matsumoto)
	Translate_సందేశం(&సందేశం);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    ఒకవేళ (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ఖాళీ, ఖాళీ) == ఖాళీ)
#అయితే_ముగింపు
	Dispatch_సందేశం_W(&సందేశం);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    శూన్య
gui_mch_update(శూన్య)
{
    సందేశం_3	    సందేశం;

    ఒకవేళ (!s_busy_processing)
	అయితే (Peek_సందేశం_W(&సందేశం, ఖాళీ, 0, 0, PM_NOREMOVE)
						  && !vim_is_ఆదానం_buf_నిండిన())
	    process_సందేశం();
}

    స్థిర శూన్య
remove_any_timer(శూన్య)
{
    సందేశం_3		సందేశం;

    ఒకవేళ (s_wait_timer != 0 && !s_timed_బహిర్గతం)
    {
	KillTimer(ఖాళీ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	అయితే (Peek_సందేశం_W(&సందేశం, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#నిర్వచితమైతే FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
స్థిర garray_T	server_pending_ga = {0, 0, 0, 0, ఖాళీ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    శూన్య
server_కూడు_ఆదానం(char_u *str)
{
    ఒకవేళ (server_pending_ga.ga_దత్తాంశం == ఖాళీ)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    స్థిర శూన్య
server_flush_ఆదానం(శూన్య)
{
    char_u  *str;

    ఒకవేళ (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	తిరిగి;
    str = vim_strnsave(server_pending_ga.ga_దత్తాంశం, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    ఒకవేళ (str != ఖాళీ)
    {
	server_గమ్యం_ఆదానం_buf(str);
	vim_free(str);
    }
}
#అయితే_ముగింపు

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    పూర్ణాంకం
gui_mch_wait_for_chars(దీర్ఘ wtime)
{
    పూర్ణాంకం		focus;

    s_timed_బహిర్గతం = FALSE;

    ఒకవేళ (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	ఒకవేళ (s_busy_processing)
	    తిరిగి FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = సమితి_Timer(ఖాళీ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.లోపల_focus;
    అయితే (!s_timed_బహిర్గతం)
    {
	// Stop or start blinking when focus changes
	ఒకవేళ (gui.లోపల_focus != focus)
	{
	    ఒకవేళ (gui.లోపల_focus)
		gui_mch_ప్రారంభం_blink();
	    లేకపోతే
		gui_mch_stop_blink(TRUE);
	    focus = gui.లోపల_focus;
	}

	ఒకవేళ (s_need_activate)
	{
	    (శూన్య)సమితి_Foreground_కిటికీ(s_hwnd);
	    s_need_activate = FALSE;
	}

#నిర్వచితమైతే FEAT_TIMERS
	did_కూడు_timer = FALSE;
#అయితే_ముగింపు
#నిర్వచితమైతే సందేశం_వరుస
	// Check channel I/O while waiting for a message.
	కొరకు (;;)
	{
	    సందేశం_3 సందేశం;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ఆదానం();
# endif
# ifdef FEAT_TIMERS
	    ఒకవేళ (did_కూడు_timer)
		ఆపు;
# endif
	    ఒకవేళ (Peek_సందేశం_W(&సందేశం, ఖాళీ, 0, 0, PM_NOREMOVE))
	    {
		process_సందేశం();
		ఆపు;
	    }
	    లేకపోతే ఒకవేళ (ఆదానం_అందుబాటు()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || సందేశం_Wait_For_గుణిజం_Objects(0, ఖాళీ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		ఆపు;
	}
#లేకపోతే
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_సందేశం();
#అయితే_ముగింపు

	ఒకవేళ (ఆదానం_అందుబాటు())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    ఒకవేళ (!s_getting_focus)
		s_button_pending = -1;

	    తిరిగి OK;
	}

#నిర్వచితమైతే FEAT_TIMERS
	ఒకవేళ (did_కూడు_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    ఆపు;
	}
#అయితే_ముగింపు
    }
    allow_scrollbar = FALSE;
    తిరిగి FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    శూన్య
gui_mch_clear_ఖండం(
    పూర్ణాంకం		row1,
    పూర్ణాంకం		col1,
    పూర్ణాంకం		row2,
    పూర్ణాంకం		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ఎడమ_2 = FILL_X(col1);
    rc.శిఖరం = FILL_Y(row1);
    rc.కుడి_2 = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    శూన్య
gui_mch_clear_all(శూన్య)
{
    RECT    rc;

    rc.ఎడమ_2 = 0;
    rc.శిఖరం = 0;
    rc.కుడి_2 = Columns * gui.char_వెడల్పు + 2 * gui.border_వెడల్పు;
    rc.bottom = అడ్డువరుసలు * gui.char_ఎత్తు + 2 * gui.border_వెడల్పు;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    శూన్య
gui_mch_enable_menu(పూర్ణాంకం flag)
{
#నిర్వచితమైతే FEAT_MENU
    సమితి_Menu(s_hwnd, flag ? s_menuBar : ఖాళీ);
#అయితే_ముగింపు
}

    శూన్య
gui_mch_సమితి_menu_pos(
    పూర్ణాంకం	    x వాడని,
    పూర్ణాంకం	    y వాడని,
    పూర్ణాంకం	    w వాడని,
    పూర్ణాంకం	    h వాడని)
{
    // It will be in the right place anyway
}

#ఒకవేళ defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    శూన్య
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    పూర్ణాంకం		hidden)
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
    శూన్య
gui_mch_draw_menubar(శూన్య)
{
    DrawMenuBar(s_hwnd);
}
#అయితే_ముగింపు // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    తిరిగి (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#ఒకవేళ defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    స్థిర మాట_2
Pixel_గమ్యం_Dialog_X(పూర్ణాంకం అంకె_Pixels)
{
    తిరిగి (మాట_2)((అంకె_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    స్థిర మాట_2
Pixel_గమ్యం_Dialog_Y(పూర్ణాంకం అంకె_Pixels)
{
    తిరిగి (మాట_2)((అంకె_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    స్థిర పూర్ణాంకం
Get_పాఠ్యం_వెడల్పు(HDC hdc, char_u *str, పూర్ణాంకం len)
{
    కొలత_3    కొలత;

    Get_పాఠ్యం_Extent_బిందువు(hdc, (LPCSTR)str, len, &కొలత);
    తిరిగి కొలత.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    స్థిర పూర్ణాంకం
Get_పాఠ్యం_వెడల్పు_Enc(HDC hdc, char_u *str, పూర్ణాంకం len)
{
    కొలత_3	కొలత;
    WCHAR	*wstr;
    పూర్ణాంకం		n;
    పూర్ణాంకం		wlen = len;

    wstr = enc_గమ్యం_utf16(str, &wlen);
    ఒకవేళ (wstr == ఖాళీ)
	తిరిగి 0;

    n = Get_పాఠ్యం_Extent_బిందువు_W(hdc, wstr, wlen, &కొలత);
    vim_free(wstr);
    ఒకవేళ (n)
	తిరిగి కొలత.cx;
    తిరిగి 0;
}

స్థిర శూన్య get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    స్థిర BOOL
Center_కిటికీ(
    HWND hwnd_బిడ్డ,
    HWND hwnd_మాతృ)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    r_బిడ్డ, r_మాతృ, rScreen;
    పూర్ణాంకం		    w_బిడ్డ, h_బిడ్డ, w_మాతృ, h_మాతృ;
    పూర్ణాంకం		    xNew, yNew;
    HDC		    hdc;

    Get_కిటికీ_Rect(hwnd_బిడ్డ, &r_బిడ్డ);
    w_బిడ్డ = r_బిడ్డ.కుడి_2 - r_బిడ్డ.ఎడమ_2;
    h_బిడ్డ = r_బిడ్డ.bottom - r_బిడ్డ.శిఖరం;

    // If Vim is minimized put the window in the middle of the screen.
    ఒకవేళ (hwnd_మాతృ == ఖాళీ || IsMinimized(hwnd_మాతృ))
	get_work_area(&r_మాతృ);
    లేకపోతే
	Get_కిటికీ_Rect(hwnd_మాతృ, &r_మాతృ);
    w_మాతృ = r_మాతృ.కుడి_2 - r_మాతృ.ఎడమ_2;
    h_మాతృ = r_మాతృ.bottom - r_మాతృ.శిఖరం;

    moninfo.cb_కొలత = పరిమాణం(MONITORINFO);
    mon = Monitor_From_కిటికీ(hwnd_బిడ్డ, MONITOR_DEFAULTTOPRIMARY);
    ఒకవేళ (mon != ఖాళీ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    లేకపోతే
    {
	hdc = GetDC(hwnd_బిడ్డ);
	rScreen.ఎడమ_2 = 0;
	rScreen.శిఖరం = 0;
	rScreen.కుడి_2 = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwnd_బిడ్డ, hdc);
    }

    xNew = r_మాతృ.ఎడమ_2 + ((w_మాతృ - w_బిడ్డ) / 2);
    ఒకవేళ (xNew < rScreen.ఎడమ_2)
	xNew = rScreen.ఎడమ_2;
    లేకపోతే ఒకవేళ ((xNew + w_బిడ్డ) > rScreen.కుడి_2)
	xNew = rScreen.కుడి_2 - w_బిడ్డ;

    yNew = r_మాతృ.శిఖరం + ((h_మాతృ - h_బిడ్డ) / 2);
    ఒకవేళ (yNew < rScreen.శిఖరం)
	yNew = rScreen.శిఖరం;
    లేకపోతే ఒకవేళ ((yNew + h_బిడ్డ) > rScreen.bottom)
	yNew = rScreen.bottom - h_బిడ్డ;

    తిరిగి సమితి_కిటికీ_Pos(hwnd_బిడ్డ, ఖాళీ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#అయితే_ముగింపు // FEAT_GUI_DIALOG

#ఒకవేళ defined(FEAT_TOOLBAR)
    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    ఒకవేళ (s_toolbarhwnd == ఖాళీ)
	తిరిగి;

    ఒకవేళ (showit)
    {
	// Enable unicode support
	Send_సందేశం(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	Show_కిటికీ(s_toolbarhwnd, SW_SHOW);
    }
    లేకపోతే
	Show_కిటికీ(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_లెక్క 31

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE)
    స్థిర శూన్య
కూడు_tabline_popup_menu_నమోదు(HMENU pmenu, UINT అంశం_id, char_u *అంశం_పాఠ్యం)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_గమ్యం_utf16(అంశం_పాఠ్యం, ఖాళీ);
    ఒకవేళ (wn == ఖాళీ)
	తిరిగి;

    infow.cb_కొలత = పరిమాణం(infow);
    infow.f_ముసుగు = MIIM_TYPE | MIIM_ID;
    infow.wID = అంశం_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_దత్తాంశం = wn;
    infow.cch = (UINT)wcslen(wn);
    చొప్పించు_Menu_అంశం_W(pmenu, అంశం_id, FALSE, &infow);
    vim_free(wn);
}

    స్థిర శూన్య
show_tabline_popup_menu(శూన్య)
{
    HMENU	    tab_pmenu;
    దీర్ఘ	    rval;
    బిందువు_2	    pt;

    // When ignoring events don't show the menu.
    ఒకవేళ (hold_gui_events || cmdwin_type != 0)
	తిరిగి;

    tab_pmenu = CreatePopupMenu();
    ఒకవేళ (tab_pmenu == ఖాళీ)
	తిరిగి;

    ఒకవేళ (మొదటి_tabpage->tp_తదుపరి != ఖాళీ)
	కూడు_tabline_popup_menu_నమోదు(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    కూడు_tabline_popup_menu_నమోదు(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    కూడు_tabline_popup_menu_నమోదు(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ఖాళీ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    ఒకవేళ (rval > 0)
    {
	TCHITTESTINFO htinfo;
	పూర్ణాంకం idx;

	ఒకవేళ (Screen_గమ్యం_Client(s_tabhwnd, &pt) == 0)
	    తిరిగి;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = Tab_Ctrl_తగిలింది_Test(s_tabhwnd, &htinfo);
	ఒకవేళ (idx == -1)
	    idx = 0;
	లేకపోతే
	    idx += 1;

	send_tabline_menu_event(idx, (పూర్ణాంకం)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    శూన్య
gui_mch_show_tabline(పూర్ణాంకం showit)
{
    ఒకవేళ (s_tabhwnd == ఖాళీ)
	తిరిగి;

    ఒకవేళ (!showit != !showing_tabline)
    {
	ఒకవేళ (showit)
	    Show_కిటికీ(s_tabhwnd, SW_SHOW);
	లేకపోతే
	    Show_కిటికీ(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    పూర్ణాంకం
gui_mch_showing_tabline(శూన్య)
{
    తిరిగి s_tabhwnd != ఖాళీ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    శూన్య
gui_mch_update_tabline(శూన్య)
{
    tabpage_T	*tp;
    TCITEM	tie;
    పూర్ణాంకం		nr = 0;
    పూర్ణాంకం		curtabidx = 0;
    పూర్ణాంకం		tabadded = 0;
    WCHAR	*wstr = ఖాళీ;

    ఒకవేళ (s_tabhwnd == ఖాళీ)
	తిరిగి;

    // Enable unicode support
    Send_సందేశం(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.ముసుగు = TCIF_పాఠ్యం;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    Send_సందేశం(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, ++nr)
    {
	ఒకవేళ (tp == curtab)
	    curtabidx = nr;

	ఒకవేళ (nr >= Tab_Ctrl_Get_అంశం_లెక్క(s_tabhwnd))
	{
	    // Add the tab
	    tie.psz_పాఠ్యం = "-Empty-";
	    Tab_Ctrl_చొప్పించు_అంశం(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.psz_పాఠ్యం = (LPSTR)NameBuff;

	wstr = enc_గమ్యం_utf16(NameBuff, ఖాళీ);
	ఒకవేళ (wstr != ఖాళీ)
	{
	    TCITEMW		tiw;

	    tiw.ముసుగు = TCIF_పాఠ్యం;
	    tiw.iImage = -1;
	    tiw.psz_పాఠ్యం = wstr;
	    Send_సందేశం(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    అయితే (nr < Tab_Ctrl_Get_అంశం_లెక్క(s_tabhwnd))
	Tab_Ctrl_Delete_అంశం(s_tabhwnd, nr);

    ఒకవేళ (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_సమితి_Cur_Sel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    Send_సందేశం(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    Redraw_కిటికీ(s_tabhwnd, ఖాళీ, ఖాళీ,
		    RDW_ERASE | RDW_చట్రం | RDW_INVALIDATE | RDW_ALLCHILDREN);

    ఒకవేళ (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_సమితి_Cur_Sel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    శూన్య
gui_mch_సమితి_curtab(పూర్ణాంకం nr)
{
    ఒకవేళ (s_tabhwnd == ఖాళీ)
	తిరిగి;

    ఒకవేళ (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	Tab_Ctrl_సమితి_Cur_Sel(s_tabhwnd, nr - 1);
}

#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_DARKTHEME
    శూన్య
gui_mch_సమితి_dark_theme(పూర్ణాంకం dark)
{
    ఒకవేళ (p_Dwm_సమితి_కిటికీ_Attribute != ఖాళీ && win_సంస్కరణ >= MAKE_VER(10, 0, 18985))
	p_Dwm_సమితి_కిటికీ_Attribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_బహుళకం, &dark,
		పరిమాణం(dark));

    ఒకవేళ (p_సమితి_Preferred_App_బహుళకం != ఖాళీ && win_సంస్కరణ >= MAKE_VER(10, 0, 18362))
	p_సమితి_Preferred_App_బహుళకం(dark);

    ఒకవేళ (pFlushMenuThemes != ఖాళీ && win_సంస్కరణ >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    స్థిర శూన్య
dyn_uxtheme_ఎక్కించు(శూన్య)
{
    hUxThemeLib = vim_ఎక్కించు_Lib("uxtheme.dll");
    ఒకవేళ (hUxThemeLib == ఖాళీ)
	తిరిగి;

    p_సమితి_Preferred_App_బహుళకం = (DWORD (WINAPI *)(DWORD))
	Get_Proc_చిరునామా(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (శూన్య (WINAPI *)(శూన్య))
	Get_Proc_చిరునామా(hUxThemeLib, MAKEINTRESOURCE(136));

    ఒకవేళ (p_సమితి_Preferred_App_బహుళకం == ఖాళీ || pFlushMenuThemes == ఖాళీ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ఖాళీ;
	తిరిగి;
    }
}

#అయితే_ముగింపు // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    శూన్య
gui_mch_సమితి_fullscreen(పూర్ణాంకం flag)
{
    స్థిర RECT సాధారణ_rect;
    స్థిర LONG_PTR సాధారణ_style, సాధారణ_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    ఒకవేళ (!నిండిన_screen) // Windows not set yet.
	తిరిగి;

    ఒకవేళ (flag)
    {
	ఒకవేళ (fullscreen_on)
	    తిరిగి;

	// Enter fullscreen mode
	Get_కిటికీ_Rect(s_hwnd, &rc);

	moninfo.cb_కొలత = పరిమాణం(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	ఒకవేళ (mon == ఖాళీ || !GetMonitorInfo(mon, &moninfo))
	    తిరిగి;

	// Save current window state
	Get_కిటికీ_Rect(s_hwnd, &సాధారణ_rect);
	సాధారణ_style = Get_కిటికీ_Long_Ptr(s_hwnd, GWL_STYLE);
	సాధారణ_exstyle = Get_కిటికీ_Long_Ptr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	సమితి_కిటికీ_Long_Ptr(s_hwnd, GWL_STYLE,
		సాధారణ_style & ~(WS_CAPTION | WS_THICKFRAME));
	సమితి_కిటికీ_Long_Ptr(s_hwnd, GWL_EXSTYLE,
		సాధారణ_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	సమితి_కిటికీ_Pos(s_hwnd, ఖాళీ,
		moninfo.rcMonitor.ఎడమ_2,
		moninfo.rcMonitor.శిఖరం,
		moninfo.rcMonitor.కుడి_2 - moninfo.rcMonitor.ఎడమ_2,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.శిఖరం,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    లేకపోతే
    {
	ఒకవేళ (!fullscreen_on)
	    తిరిగి;

	// Exit fullscreen mode
	సమితి_కిటికీ_Long_Ptr(s_hwnd, GWL_STYLE, సాధారణ_style);
	సమితి_కిటికీ_Long_Ptr(s_hwnd, GWL_EXSTYLE, సాధారణ_exstyle);

	// Restore original window position and size
	సమితి_కిటికీ_Pos(s_hwnd, ఖాళీ,
		సాధారణ_rect.ఎడమ_2,
		సాధారణ_rect.శిఖరం,
		సాధారణ_rect.కుడి_2 - సాధారణ_rect.ఎడమ_2,
		సాధారణ_rect.bottom - సాధారణ_rect.శిఖరం,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    శూన్య
ex_simalt(exarg_T *eap)
{
    char_u	*కీలకాలు = eap->arg;
    పూర్ణాంకం		fill_typebuf = FALSE;
    char_u	కీలకం_name[4];

    Post_సందేశం(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    అయితే (*కీలకాలు)
    {
	ఒకవేళ (*కీలకాలు == '~')
	    *కీలకాలు = ' ';	    // for showing system menu
	Post_సందేశం(s_hwnd, WM_CHAR, (WPARAM)*కీలకాలు, (LPARAM)0);
	కీలకాలు++;
	fill_typebuf = TRUE;
    }
    ఒకవేళ (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	కీలకం_name[0] = K_SPECIAL;
	కీలకం_name[1] = KS_EXTRA;
	కీలకం_name[2] = KE_NOP;
	కీలకం_name[3] = NUL;
#ఒకవేళ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#అయితే_ముగింపు
	(శూన్య)ins_typebuf(కీలకం_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    స్థిర శూన్య
initialise_findrep(char_u *initial_string)
{
    పూర్ణాంకం		wword = FALSE;
    పూర్ణాంకం		mcase = !p_ic;
    char_u	*నమోదు_పాఠ్యం;

    // Get the search string to use.
    నమోదు_పాఠ్యం = get_కనుగొను_dialog_పాఠ్యం(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_కిందికి;
    ఒకవేళ (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    ఒకవేళ (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ && *నమోదు_పాఠ్యం != NUL)
    {
	WCHAR *p = enc_గమ్యం_utf16(నమోదు_పాఠ్యం, ఖాళీ);
	ఒకవేళ (p != ఖాళీ)
	{
	    పూర్ణాంకం len = s_findrep_struct.w_కనుగొను_What_Len - 1;

	    wcsncpy(s_findrep_struct.lpstr_కనుగొను_What, p, len);
	    s_findrep_struct.lpstr_కనుగొను_What[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(నమోదు_పాఠ్యం);
}
#అయితే_ముగింపు

    స్థిర శూన్య
సమితి_కిటికీ_title(HWND hwnd, అక్షరం *title)
{
    ఒకవేళ (title != ఖాళీ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_గమ్యం_utf16((char_u *)title, ఖాళీ);
	ఒకవేళ (wbuf != ఖాళీ)
	{
	    సమితి_కిటికీ_పాఠ్యం_W(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    లేకపోతే
	(శూన్య)సమితి_కిటికీ_పాఠ్యం_W(hwnd, ఖాళీ);
}

    శూన్య
gui_mch_కనుగొను_dialog(exarg_T *eap)
{
#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    ఒకవేళ (s_findrep_సందేశం != 0)
    {
	ఒకవేళ (Is_కిటికీ(s_findrep_hwnd) && !s_findrep_is_కనుగొను)
	    Destroy_కిటికీ(s_findrep_hwnd);

	ఒకవేళ (!Is_కిటికీ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = కనుగొను_పాఠ్యం_W(&s_findrep_struct);
	}

	సమితి_కిటికీ_title(s_findrep_hwnd, _("Find string"));
	(శూన్య)సమితి_Focus(s_findrep_hwnd);

	s_findrep_is_కనుగొను = TRUE;
    }
#అయితే_ముగింపు
}


    శూన్య
gui_mch_replace_dialog(exarg_T *eap)
{
#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    ఒకవేళ (s_findrep_సందేశం != 0)
    {
	ఒకవేళ (Is_కిటికీ(s_findrep_hwnd) && s_findrep_is_కనుగొను)
	    Destroy_కిటికీ(s_findrep_hwnd);

	ఒకవేళ (!Is_కిటికీ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = Replace_పాఠ్యం_W(&s_findrep_struct);
	}

	సమితి_కిటికీ_title(s_findrep_hwnd, _("Find & Replace"));
	(శూన్య)సమితి_Focus(s_findrep_hwnd);

	s_findrep_is_కనుగొను = FALSE;
    }
#అయితే_ముగింపు
}


/*
 * Set visibility of the pointer.
 */
    శూన్య
gui_mch_mousehide(పూర్ణాంకం hide)
{
    ఒకవేళ (hide == gui.సూచిక_hidden)
	తిరిగి;

    ShowCursor(!hide);
    gui.సూచిక_hidden = hide;
}

#నిర్వచితమైతే FEAT_MENU
    స్థిర శూన్య
gui_mch_show_popupmenu_at(vimmenu_T *menu, పూర్ణాంకం x, పూర్ణాంకం y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (శూన్య)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(పూర్ణాంకం)0,	    //reserved param
	s_hwnd,
	ఖాళీ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#అయితే_ముగింపు

/*
 * Got a message when the system will go down.
 */
    స్థిర శూన్య
On_చివర_Session(శూన్య)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    స్థిర శూన్య
_OnClose(HWND hwnd వాడని)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    స్థిర శూన్య
_OnDestroy(HWND hwnd)
{
    ఒకవేళ (!destroying)
	_OnClose(hwnd);
}

    స్థిర శూన్య
_OnPaint(
    HWND hwnd)
{
    ఒకవేళ (IsMinimized(hwnd))
	తిరిగి;

    PAINTSTRUCT ps;

    బహిర్గతం_flush();	    // make sure all output has been processed
    (శూన్య)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    ఒకవేళ (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ఎడమ_2 = rect.ఎడమ_2;
	ps.rcPaint.కుడి_2 = rect.కుడి_2;
    }

    ఒకవేళ (!Is_Rect_ఖాళీలు(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	ఒకవేళ (!screen_cleared || Screen_పంక్తులు == ఖాళీ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	లేకపోతే
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ఎడమ_2, ps.rcPaint.శిఖరం,
		ps.rcPaint.కుడి_2 - ps.rcPaint.ఎడమ_2 + 1,
		ps.rcPaint.bottom - ps.rcPaint.శిఖరం + 1);
    }

    చివర_Paint(hwnd, &ps);
}

    స్థిర శూన్య
On_కొలత(
    HWND hwnd,
    UINT state వాడని,
    పూర్ణాంకం cx,
    పూర్ణాంకం cy)
{
    ఒకవేళ (!IsMinimized(hwnd) && !s_లోపల_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_ఎత్తు(TRUE);
    }
}

    స్థిర శూన్య
On_సమితి_Focus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (శూన్య)Def_కిటికీ_Proc_W(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    స్థిర శూన్య
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    ఒకవేళ (destroying)
	తిరిగి;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (శూన్య)Def_కిటికీ_Proc_W(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    స్థిర LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    తిరిగి Def_కిటికీ_Proc_W(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    శూన్య
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    Destroy_కిటికీ(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    RECT rct;
    బిందువు_2 mp;

    (శూన్య)Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rct);
    (శూన్య)GetCursorPos(&mp);
    *x = (పూర్ణాంకం)(mp.x - rct.ఎడమ_2);
    *y = (పూర్ణాంకం)(mp.y - rct.శిఖరం);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    శూన్య
gui_mch_setmouse(పూర్ణాంకం x, పూర్ణాంకం y)
{
    RECT rct;

    (శూన్య)Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rct);
    (శూన్య)సమితి_Cursor_Pos(x + gui.border_స్థానభ్రంశం + rct.ఎడమ_2,
		       y + gui.border_స్థానభ్రంశం + rct.శిఖరం);
}

    స్థిర శూన్య
gui_mswin_get_valid_dimensions(
    పూర్ణాంకం w,
    పూర్ణాంకం h,
    పూర్ణాంకం *valid_w,
    పూర్ణాంకం *valid_h,
    పూర్ణాంకం *నిలువుల_సంఖ్య,
    పూర్ణాంకం *అడ్డువరుసలు_2)
{
    పూర్ణాంకం	    పునాది_వెడల్పు, పునాది_ఎత్తు;

    పునాది_వెడల్పు = gui_get_పునాది_వెడల్పు()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    పునాది_ఎత్తు = gui_get_పునాది_ఎత్తు()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_ఎత్తు(FALSE);
    *నిలువుల_సంఖ్య = (w - పునాది_వెడల్పు) / gui.char_వెడల్పు;
    *అడ్డువరుసలు_2 = (h - పునాది_ఎత్తు) / gui.char_ఎత్తు;
    *valid_w = పునాది_వెడల్పు + *నిలువుల_సంఖ్య * gui.char_వెడల్పు;
    *valid_h = పునాది_ఎత్తు + *అడ్డువరుసలు_2 * gui.char_ఎత్తు;
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
    RECT    rc;

#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ఎడమ_2 = 0;
    rc.శిఖరం = 0;
    rc.కుడి_2 = gui.అంకె_నిలువుల_సంఖ్య * gui.char_వెడల్పు;
    rc.bottom = gui.అంకె_అడ్డువరుసలు * gui.char_ఎత్తు;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((దీర్ఘ)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    స్థిర BOOL
is_బిందువు_onscreen(పూర్ణాంకం x, పూర్ణాంకం y)
{
    బిందువు_2   pt = {x, y};

    తిరిగి Monitor_From_బిందువు(pt, MONITOR_DEFAULTTONULL) != ఖాళీ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    స్థిర BOOL
is_కిటికీ_onscreen(HWND hwnd)
{
    RECT    rc;
    బిందువు_2   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ఎడమ_2;
    p1.y = rc.శిఖరం;
    p2.x = rc.కుడి_2 - 1;
    p2.y = rc.bottom - 1;
    Client_గమ్యం_Screen(hwnd, &p1);
    Client_గమ్యం_Screen(hwnd, &p2);

    ఒకవేళ (!is_బిందువు_onscreen(p1.x, p1.y))
	తిరిగి FALSE;
    ఒకవేళ (!is_బిందువు_onscreen(p1.x, p2.y))
	తిరిగి FALSE;
    ఒకవేళ (!is_బిందువు_onscreen(p2.x, p1.y))
	తిరిగి FALSE;
    ఒకవేళ (!is_బిందువు_onscreen(p2.x, p2.y))
	తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    స్థిర పూర్ణాంకం
get_scroll_flags(శూన్య)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    ఒకవేళ (!is_కిటికీ_onscreen(s_hwnd))
	తిరిగి SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    Get_కిటికీ_Rect(s_hwnd, &rcVim);
    కొరకు (hwnd = s_hwnd; (hwnd = Get_కిటికీ(hwnd, GW_HWNDPREV)) != (HWND)0; )
	ఒకవేళ (Is_కిటికీ_Visible(hwnd))
	{
	    Get_కిటికీ_Rect(hwnd, &rcOther);
	    ఒకవేళ (IntersectRect(&rcDest, &rcVim, &rcOther))
		తిరిగి SW_INVALIDATE;
	}
    తిరిగి 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    స్థిర శూన్య
intel_gpu_workaround(శూన్య)
{
    GetPixel(s_hdc, FILL_X(gui.నిలువు), FILL_Y(gui.అడ్డువరుస));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    శూన్య
gui_mch_delete_పంక్తులు(
    పూర్ణాంకం	    అడ్డువరుస,
    పూర్ణాంకం	    అంకె_పంక్తులు)
{
    RECT	rc;

    rc.ఎడమ_2 = FILL_X(gui.scroll_region_ఎడమ);
    rc.కుడి_2 = FILL_X(gui.scroll_region_కుడి + 1);
    rc.శిఖరం = FILL_Y(అడ్డువరుస);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX() && is_కిటికీ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -అంకె_పంక్తులు * gui.char_ఎత్తు, &rc);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
#ఒకవేళ defined(FEAT_DIRECTX)
	ఒకవేళ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు
	intel_gpu_workaround();
	Scroll_కిటికీ_Ex(s_పాఠ్యం_Area, 0, -అంకె_పంక్తులు * gui.char_ఎత్తు,
				    &rc, &rc, ఖాళీ, ఖాళీ, get_scroll_flags());
	Update_కిటికీ(s_పాఠ్యం_Area);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_ఖండం(gui.scroll_region_bot - అంకె_పంక్తులు + 1,
						       gui.scroll_region_ఎడమ,
	gui.scroll_region_bot, gui.scroll_region_కుడి);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    శూన్య
gui_mch_చొప్పించు_పంక్తులు(
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		అంకె_పంక్తులు)
{
    RECT	rc;

    rc.ఎడమ_2 = FILL_X(gui.scroll_region_ఎడమ);
    rc.కుడి_2 = FILL_X(gui.scroll_region_కుడి + 1);
    rc.శిఖరం = FILL_Y(అడ్డువరుస);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX() && is_కిటికీ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, అంకె_పంక్తులు * gui.char_ఎత్తు, &rc);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
#ఒకవేళ defined(FEAT_DIRECTX)
	ఒకవేళ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	Scroll_కిటికీ_Ex(s_పాఠ్యం_Area, 0, అంకె_పంక్తులు * gui.char_ఎత్తు,
				    &rc, &rc, ఖాళీ, ఖాళీ, get_scroll_flags());
	Update_కిటికీ(s_పాఠ్యం_Area);
    }

    gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
				అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);
}


    శూన్య
gui_mch_exit(పూర్ణాంకం rc వాడని)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ఖాళీ;
#అయితే_ముగింపు

    ReleaseDC(s_పాఠ్యం_Area, s_hdc);
    DeleteObject(s_brush);

#నిర్వచితమైతే FEAT_TEAROFF
    // Unload the tearoff bitmap
    (శూన్య)DeleteObject((HGDIOBJ)s_htearbitmap);
#అయితే_ముగింపు

    // Destroy our window (if we have one).
    ఒకవేళ (s_hwnd != ఖాళీ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	Destroy_కిటికీ(s_hwnd);
    }
}

    స్థిర char_u *
logfont2name(LOGFONTW lf)
{
    అక్షరం	*charset_name;
    అక్షరం	*quality_name;
    అక్షరం	*font_name;
    size_t	res_కొలత;
    అక్షరం	*res;

    font_name = (అక్షరం *)utf16_గమ్యం_enc(lf.lfFaceName, ఖాళీ);
    ఒకవేళ (font_name == ఖాళీ)
	తిరిగి ఖాళీ;
    charset_name = charset_id2name((పూర్ణాంకం)lf.lf_Char_సమితి);
    quality_name = quality_id2name((పూర్ణాంకం)lf.lfQuality);

    res_కొలత = STRLEN(font_name) + 30
		    + (charset_name == ఖాళీ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ఖాళీ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_కొలత);
    ఒకవేళ (res != ఖాళీ)
    {
	అక్షరం	*p;
	పూర్ణాంకం	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	కొరకు (p = font_name; *p != NUL; ++p)
	{
	    ఒకవేళ (ఖాళీనా(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_గమ్యం_points(
			 lf.lf_ఎత్తు < 0 ? -lf.lf_ఎత్తు : lf.lf_ఎత్తు, TRUE);
	ఒకవేళ (lf.lf_బరువు == FW_సాధారణ || lf.lf_బరువు == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(అక్షరం *)res, res_కొలత, "%s:h%d", font_name, points);
	లేకపోతే
	    res_len = vim_snprintf_safelen(
		(అక్షరం *)res, res_కొలత, "%s:h%d:W%ld", font_name, points, lf.lf_బరువు);

	res_len += vim_snprintf_safelen(
	    (అక్షరం *)res + res_len,
	    res_కొలత - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lf_బరువు == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lf_Strike_బహిర్గతం ? ":s" : "");

	ఒకవేళ (charset_name != ఖాళీ)
	    res_len += vim_snprintf_safelen((అక్షరం *)res + res_len,
		res_కొలత - res_len, ":c%s", charset_name);
	ఒకవేళ (quality_name != ఖాళీ)
	    vim_snprintf((అక్షరం *)res + res_len,
		res_కొలత - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    తిరిగి (char_u *)res;
}


#నిర్వచితమైతే FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    స్థిర శూన్య
update_im_font(శూన్య)
{
    LOGFONTW	lf_wide, lf;

    ఒకవేళ (p_guifontwide != ఖాళీ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, పరిమాణం(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    లేకపోతే
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    ఒకవేళ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lf_ఎత్తు = lf.lf_ఎత్తు * DEFAULT_DPI / s_dpi;
    im_సమితి_font(&lf);
}
#అయితే_ముగింపు

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    శూన్య
gui_mch_wide_font_మారింది(శూన్య)
{
    LOGFONTW lf;

#నిర్వచితమైతే FEAT_MBYTE_IME
    update_im_font();
#అయితే_ముగింపు

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    ఒకవేళ (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, పరిమాణం(lf), &lf))
    {
	ఒకవేళ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	ఒకవేళ (lf.lf_బరువు < FW_BOLD)
	{
	    lf.lf_బరువు = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    ఒకవేళ (!lf.lfItalic)
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
    పూర్ణాంకం
gui_mch_init_font(char_u *font_name, పూర్ణాంకం fontset వాడని)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    ఒకవేళ (get_logfont(&lf, font_name, ఖాళీ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lf_ఎత్తు = adjust_fontsize_by_dpi(lf.lf_ఎత్తు);
	font = get_font_handle(&lf);
    }
    ఒకవేళ (font == NOFONT)
	తిరిగి FAIL;

#ఒకవేళ defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	పూర్ణాంకం		    feat_లెక్క = 0;
	char_u		    *fp;

	ఒకవేళ (font_name != ఖాళీ)
	{
	    // Find each ":f" option in font_name.
	    కొరకు (fp = font_name; *fp != NUL; fp++)
	    {
		ఒకవేళ (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u ట్యాగ్[5];
		    పూర్ణాంకం    ti = 0;
		    గుర్తులేని పూర్ణాంకం param = 1;

		    fp += 2;  // skip ":f"
		    అయితే (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			ట్యాగ్[ti++] = *fp++;
		    ట్యాగ్[ti] = NUL;

		    ఒకవేళ (ti != 4)
			కొనసాగు;  // invalid tag length

		    ఒకవేళ (*fp == '=')
		    {
			fp++;
			param = (గుర్తులేని పూర్ణాంకం)atoi((అక్షరం *)fp);
			అయితే (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    ఒకవేళ (feat_లెక్క < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_లెక్క].ట్యాగ్ =
			    ((గుర్తులేని పూర్ణాంకం)ట్యాగ్[0])
			    | ((గుర్తులేని పూర్ణాంకం)ట్యాగ్[1] << 8)
			    | ((గుర్తులేని పూర్ణాంకం)ట్యాగ్[2] << 16)
			    | ((గుర్తులేని పూర్ణాంకం)ట్యాగ్[3] << 24);
			features[feat_లెక్క].parameter = param;
			feat_లెక్క++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWrite_Context_సమితి_Font_Features(s_dwc, features, feat_లెక్క);
    }
#అయితే_ముగింపు

    ఒకవేళ (font_name == ఖాళీ)
	font_name = (char_u *)"";
#నిర్వచితమైతే FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    ఒకవేళ (!s_లోపల_dpichanged)
	update_im_font();
#అయితే_ముగింపు
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_ఎత్తు = lfOrig.lf_ఎత్తు;
    Update_Font_కొలత(font);

    p = logfont2name(lfOrig);
    ఒకవేళ (p != ఖాళీ)
    {
	hl_సమితి_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	ఒకవేళ (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	లేకపోతే
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    ఒకవేళ (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    ఒకవేళ (lf.lf_బరువు < FW_BOLD)
    {
	lf.lf_బరువు = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	ఒకవేళ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    తిరిగి OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    పూర్ణాంకం
gui_mch_maximized(శూన్య)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.నిడివి = పరిమాణం(WINDOWPLACEMENT);
    ఒకవేళ (Get_కిటికీ_Placement(s_hwnd, &wp))
    {
	ఒకవేళ (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    తిరిగి TRUE;
	ఒకవేళ (wp.showCmd == SW_SHOWMINIMIZED)
	    తిరిగి FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	Get_కిటికీ_Rect(s_hwnd, &rc);
	ఒకవేళ ((rc.కుడి_2 - rc.ఎడమ_2 !=
		    wp.rc_సాధారణ_Position.కుడి_2 - wp.rc_సాధారణ_Position.ఎడమ_2)
		|| (rc.bottom - rc.శిఖరం !=
		    wp.rc_సాధారణ_Position.bottom - wp.rc_సాధారణ_Position.శిఖరం))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    శూన్య
gui_mch_newfont(శూన్య)
{
    RECT	rect;

    Get_కిటికీ_Rect(s_hwnd, &rect);
    ఒకవేళ (win_socket_id == 0)
    {
	gui_resize_shell(rect.కుడి_2 - rect.ఎడమ_2
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.శిఖరం
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_ఎత్తు(FALSE));
    }
    లేకపోతే
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.కుడి_2 - rect.ఎడమ_2,
	    rect.bottom - rect.శిఖరం - gui_mswin_get_menu_ఎత్తు(FALSE));
    }
}

/*
 * Set the window title
 */
    శూన్య
gui_mch_settitle(
    char_u  *title,
    char_u  *icon వాడని)
{
    సమితి_కిటికీ_title(s_hwnd, (title == ఖాళీ ? "VIM" : (అక్షరం *)title));
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
స్థిర LPCSTR mshape_idcs[] =
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

    శూన్య
mch_సమితి_mouse_shape(పూర్ణాంకం shape)
{
    LPCSTR idc;

    ఒకవేళ (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    లేకపోతే
    {
	ఒకవేళ (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	లేకపోతే
	    idc = mshape_idcs[shape];
	సమితి_Class_Long_Ptr(s_పాఠ్యం_Area, GCLP_HCURSOR, (LONG_PTR)ఎక్కించు_Cursor(ఖాళీ, idc));
	ఒకవేళ (!p_mh)
	{
	    బిందువు_2 mp;

	    // Set the position to make it redrawn with the new shape.
	    (శూన్య)GetCursorPos(&mp);
	    (శూన్య)సమితి_Cursor_Pos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    స్థిర WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    పూర్ణాంకం len;
    WCHAR *res;

    tmp = convert_filter(s);
    ఒకవేళ (tmp == ఖాళీ)
	తిరిగి ఖాళీ;
    len = (పూర్ణాంకం)STRLEN(s) + 3;
    res = enc_గమ్యం_utf16(tmp, &len);
    vim_free(tmp);
    తిరిగి res;
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
	పూర్ణాంకం saving,
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
    పూర్ణాంకం			i;
    WCHAR		*titlep = ఖాళీ;
    WCHAR		*extp = ఖాళీ;
    WCHAR		*initdirp = ఖాళీ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    ఒకవేళ (dflt == ఖాళీ)
	fileBuf[0] = NUL;
    లేకపోతే
    {
	wp = enc_గమ్యం_utf16(dflt, ఖాళీ);
	ఒకవేళ (wp == ఖాళీ)
	    fileBuf[0] = NUL;
	లేకపోతే
	{
	    కొరకు (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_క్షేత్రం(fileStruct);
# ifdef OPENFILENAME_కొలత_సంస్కరణ_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_కొలత = OPENFILENAME_కొలత_సంస్కరణ_400_W;
# లేకపోతే
    fileStruct.l_Struct_కొలత = పరిమాణం(fileStruct);
# endif

    ఒకవేళ (title != ఖాళీ)
	titlep = enc_గమ్యం_utf16(title, ఖాళీ);
    fileStruct.lpstrTitle = titlep;

    ఒకవేళ (ext != ఖాళీ)
	extp = enc_గమ్యం_utf16(ext, ఖాళీ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    ఒకవేళ (initdir != ఖాళీ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_గమ్యం_utf16(initdir, ఖాళీ);
	ఒకవేళ (initdirp != ఖాళీ)
	{
	    కొరకు (wp = initdirp; *wp != NUL; ++wp)
		ఒకవేళ (*wp == '/')
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
    ఒకవేళ (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    ఒకవేళ (saving)
	ret = GetSaveFileNameW(&fileStruct);
    లేకపోతే
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    ఒకవేళ (!ret)
	తిరిగి ఖాళీ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_గమ్యం_enc(fileBuf, ఖాళీ);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    // Give focus back to main window (when using MDI).
    సమితి_Focus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    తిరిగి q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    స్థిర char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    గుర్తులేని	s_len = (గుర్తులేని)STRLEN(s);
    గుర్తులేని	i;

    res = alloc(s_len + 3);
    ఒకవేళ (res != ఖాళీ)
    {
	కొరకు (i = 0; i < s_len; ++i)
	    ఒకవేళ (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    లేకపోతే
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    తిరిగి res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    తిరిగి gui_mch_browse(0, title, (char_u *)_("Not Used"), ఖాళీ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#అయితే_ముగింపు // FEAT_BROWSE

    స్థిర శూన్య
_OnDropFiles(
    HWND hwnd వాడని,
    HDROP hDrop)
{
#నిర్వచించు BUFPATHLEN MAX_మార్గం_2
#నిర్వచించు DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    అక్షరం    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ఖాళీ, 0);
    UINT    i;
    char_u  **fnames;
    బిందువు_2   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    Drag_Query_బిందువు(hDrop, &pt);
    Map_కిటికీ_Points(s_hwnd, s_పాఠ్యం_Area, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    ఒకవేళ (fnames != ఖాళీ)
	కొరకు (i = 0; i < cFiles; ++i)
	{
	    ఒకవేళ (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_గమ్యం_enc(wszFile, ఖాళీ);
	    లేకపోతే
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    Drag_ముగింపు(hDrop);

    ఒకవేళ (fnames == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం kbd_modifiers = get_చురుకైన_modifiers();

    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_జరుపు) != 0)
	modifiers |= MOUSE_జరుపు;
    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    స్థిర పూర్ణాంకం
_OnScroll(
    HWND hwnd వాడని,
    HWND hwndCtl,
    UINT సంకేతం_2,
    పూర్ణాంకం pos)
{
    స్థిర UINT	prev_సంకేతం = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    దీర్ఘ	val;
    పూర్ణాంకం		dragging = FALSE;
    పూర్ణాంకం		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_కొలత = పరిమాణం(si);
    si.f_ముసుగు = SIF_POS;

    sb = gui_mswin_కనుగొను_scrollbar(hwndCtl);
    ఒకవేళ (sb == ఖాళీ)
	తిరిగి 0;

    ఒకవేళ (sb->wp != ఖాళీ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    లేకపోతే	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->విలువ;

    ఎంపిక (సంకేతం_2)
    {
	స్థితి SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    ఒకవేళ (sb->scroll_జరుపు > 0)
		val <<= sb->scroll_జరుపు;
	    ఆపు;
	స్థితి SB_LINEDOWN:
	    val++;
	    ఆపు;
	స్థితి SB_LINEUP:
	    val--;
	    ఆపు;
	స్థితి SB_PAGEDOWN:
	    val += (sb_info->కొలత > 2 ? sb_info->కొలత - 2 : 1);
	    ఆపు;
	స్థితి SB_PAGEUP:
	    val -= (sb_info->కొలత > 2 ? sb_info->కొలత - 2 : 1);
	    ఆపు;
	స్థితి SB_శిఖరం:
	    val = 0;
	    ఆపు;
	స్థితి SB_BOTTOM:
	    val = sb_info->max;
	    ఆపు;
	స్థితి SB_ENDSCROLL:
	    ఒకవేళ (prev_సంకేతం == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		ఒకవేళ (sb->scroll_జరుపు > 0)
		    val <<= sb->scroll_జరుపు;
	    }
	    ఆపు;

	అప్రమేయ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    తిరిగి 0;
    }
    prev_సంకేతం = సంకేతం_2;

    si.nPos = (sb->scroll_జరుపు > 0) ? val >> sb->scroll_జరుపు : val;
    సమితి_Scroll_Info(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    ఒకవేళ (sb->wp != ఖాళీ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ఎడమ) ? SBAR_కుడి : SBAR_ఎడమ].id;

	సమితి_Scroll_Info(id, SB_CTL, &si, TRUE);
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

    తిరిగి 0;
}


#నిర్వచితమైతే FEAT_XPM_W32
# include "xpm_w32.h"
#అయితే_ముగింపు


// Some parameters for tearoff menus.  All in pixels.
#నిర్వచించు TEAROFF_PADDING_X	2
#నిర్వచించు TEAROFF_BUTTON_PAD_X	8
#నిర్వచించు TEAROFF_MIN_వెడల్పు	200
#నిర్వచించు TEAROFF_SUBMENU_LABEL	">>"
#నిర్వచించు TEAROFF_స్తంభం_PADDING	3	// # spaces to pad column with.


#నిర్వచితమైతే FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_పాఠ్యం_LEN	    MAXPATHL

స్థిర BalloonEval  *cur_beval = ఖాళీ;
స్థిర UINT_PTR	    beval_timer_id = 0;
స్థిర DWORD	    చివరి_user_activity = 0;
#అయితే_ముగింపు // defined(FEAT_BEVAL_GUI)


// Local variables:

#నిర్వచితమైతే FEAT_MENU
స్థిర UINT	s_menu_id = 100;
#అయితే_ముగింపు

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#నిర్వచించు USE_SYSMENU_FONT

#నిర్వచించు VIM_NAME	"vim"
#నిర్వచించు VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#నిర్వచించు DLG_ALLOC_కొలత 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
స్థిర PWORD
కూడు_dialog_element(
	PWORD p,
	DWORD lStyle,
	మాట_2 x,
	మాట_2 y,
	మాట_2 w,
	మాట_2 h,
	మాట_2 Id,
	మాట_2 clss,
	స్థిరరాశి అక్షరం *caption);
స్థిర LPWORD lpwAlign(LPWORD);
స్థిర పూర్ణాంకం n_Copy_Ansi_గమ్యం_Wide_Char(LPWORD, LPSTR, BOOL);
#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
స్థిర శూన్య gui_mch_tearoff(char_u *title, vimmenu_T *menu, పూర్ణాంకం initX, పూర్ణాంకం initY);
#అయితే_ముగింపు
స్థిర శూన్య get_dialog_font_metrics(శూన్య);

స్థిర పూర్ణాంకం dialog_default_button = -1;

#నిర్వచితమైతే FEAT_TOOLBAR
స్థిర శూన్య initialise_toolbar(శూన్య);
స్థిర శూన్య update_toolbar_కొలత(శూన్య);
స్థిర LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_సందేశం, WPARAM wParam, LPARAM lParam);
స్థిర పూర్ణాంకం get_toolbar_bitmap(vimmenu_T *menu);
#లేకపోతే
# define update_toolbar_కొలత()
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
స్థిర శూన్య initialise_tabline(శూన్య);
స్థిర LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_సందేశం, WPARAM wParam, LPARAM lParam);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MBYTE_IME
స్థిర LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
స్థిర char_u *Get_ఫలితం_Str(HWND hwnd, పూర్ణాంకం GCS, పూర్ణాంకం *lenp);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
రకనిర్వచనం నిర్మాణం ట్యాగ్_COMPOSITIONFORM {
    DWORD dwStyle;
    బిందువు_2 ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
రకనిర్వచనం HANDLE HIMC;
# endif

స్థిర HINSTANCE hLibImm = ఖాళీ;
స్థిర LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
స్థిర HIMC (WINAPI *pImmGetContext)(HWND);
స్థిర HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
స్థిర BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
స్థిర BOOL (WINAPI *p_Imm_Get_Open_స్థితి)(HIMC);
స్థిర BOOL (WINAPI *p_Imm_సమితి_Open_స్థితి)(HIMC, BOOL);
స్థిర BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
స్థిర BOOL (WINAPI *p_Imm_సమితి_Composition_Font_W)(HIMC, LPLOGFONTW);
స్థిర BOOL (WINAPI *p_Imm_సమితి_Composition_కిటికీ)(HIMC, LPCOMPOSITIONFORM);
స్థిర BOOL (WINAPI *p_Imm_Get_Conversion_స్థితి)(HIMC, LPDWORD, LPDWORD);
స్థిర BOOL (WINAPI *p_Imm_సమితి_Conversion_స్థితి)(HIMC, DWORD, DWORD);
స్థిర శూన్య dyn_imm_ఎక్కించు(శూన్య);
#లేకపోతే
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define p_Imm_Get_Open_స్థితి	  Imm_Get_Open_స్థితి
# define p_Imm_సమితి_Open_స్థితి	  Imm_సమితి_Open_స్థితి
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define p_Imm_సమితి_Composition_Font_W  Imm_సమితి_Composition_Font_W
# define p_Imm_సమితి_Composition_కిటికీ Imm_సమితి_Composition_కిటికీ
# define p_Imm_Get_Conversion_స్థితి  Imm_Get_Conversion_స్థితి
# define p_Imm_సమితి_Conversion_స్థితి  Imm_సమితి_Conversion_స్థితి
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    స్థిర పూర్ణాంకం
gui_mswin_get_menu_ఎత్తు(
    పూర్ణాంకం	    fix_కిటికీ)	    // If TRUE, resize window if menu height changed
{
    స్థిర పూర్ణాంకం	old_menu_ఎత్తు = -1;

    RECT    rc1, rc2;
    పూర్ణాంకం	    అంకె;
    పూర్ణాంకం	    menu_ఎత్తు;

    ఒకవేళ (gui.menu_is_చురుకైన)
	అంకె = Get_Menu_అంశం_లెక్క(s_menuBar);
    లేకపోతే
	అంకె = 0;

    ఒకవేళ (అంకె == 0)
	menu_ఎత్తు = 0;
    లేకపోతే ఒకవేళ (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_ఎత్తు = old_menu_ఎత్తు == -1 ? 0 : old_menu_ఎత్తు;
    }
    లేకపోతే
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	Get_Menu_అంశం_Rect(s_hwnd, s_menuBar, 0, &rc1);
	ఒకవేళ (gui.starting)
	    menu_ఎత్తు = rc1.bottom - rc1.శిఖరం + 1;
	లేకపోతే
	{
	    Get_Menu_అంశం_Rect(s_hwnd, s_menuBar, అంకె - 1, &rc2);
	    menu_ఎత్తు = rc2.bottom - rc1.శిఖరం + 1;
	}
    }

    ఒకవేళ (fix_కిటికీ && menu_ఎత్తు != old_menu_ఎత్తు)
	gui_సమితి_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_ఎత్తు = menu_ఎత్తు;

    తిరిగి menu_ఎత్తు;
}
#అయితే_ముగింపు // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    స్థిర దీర్ఘ
mouse_vertical_scroll_అడుగు(శూన్య)
{
    UINT val;
    ఒకవేళ (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	తిరిగి (val != WHEEL_PAGESCROLL) ? (దీర్ఘ)val : -1;
    తిరిగి 3; // Safe default;
}

    స్థిర దీర్ఘ
mouse_horizontal_scroll_అడుగు(శూన్య)
{
    UINT val;
    ఒకవేళ (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	తిరిగి (దీర్ఘ)val;
    తిరిగి 3; // Safe default;
}

    స్థిర శూన్య
init_mouse_wheel(శూన్య)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_సమితి_vert_scroll_అడుగు(mouse_vertical_scroll_అడుగు());
    mouse_సమితి_hor_scroll_అడుగు(mouse_horizontal_scroll_అడుగు());
}

/*
 * Mouse scroll event handler.
 */
    స్థిర శూన్య
_OnMouseWheel(HWND hwnd వాడని, WPARAM wParam, LPARAM lParam, పూర్ణాంకం horizontal)
{
    పూర్ణాంకం		button;
    win_T	*wp;
    పూర్ణాంకం		modifiers = 0;
    పూర్ణాంకం		kbd_modifiers;
    పూర్ణాంకం		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    బిందువు_2	pt;

    wp = gui_mouse_కిటికీ(కనుగొను_POPUP);

#నిర్వచితమైతే FEAT_PROP_POPUP
    ఒకవేళ (wp != ఖాళీ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_అడ్డువరుస = wp->w_winrow;
	mouse_నిలువు = wp->w_wincol;
	CLEAR_క్షేత్రం(cap);
	ఒకవేళ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ఎడమ : MSCR_కుడి;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	లేకపోతే
	{
	    cap.arg = zDelta < 0 ? MSCR_పైకి : MSCR_కిందికి;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	బహిర్గతం_flush();
    }
#అయితే_ముగింపు

    ఒకవేళ (wp == ఖాళీ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    ఒకవేళ (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    లేకపోతే
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_చురుకైన_modifiers();

    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_జరుపు) != 0)
	modifiers |= MOUSE_జరుపు;
    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ఒకవేళ ((kbd_modifiers & MOD_ముసుగు_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    Screen_గమ్యం_Client(s_పాఠ్యం_Area, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#నిర్వచితమైతే USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_కొలత = పరిమాణం(NONCLIENTMETRICSW);
    ఒకవేళ (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    పరిమాణం(NONCLIENTMETRICSW),
	    &nm,
	    0))
	తిరిగి FAIL;
    *lf = nm.lfMenuFont;
    తిరిగి OK;
}
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    స్థిర శూన్య
సమితి_tabline_font(శూన్య)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    ఒకవేళ (gui_w32_get_menu_font(&lfSysmenu) != OK)
	తిరిగి;

    lfSysmenu.lf_ఎత్తు = adjust_fontsize_by_dpi(lfSysmenu.lf_ఎత్తు);
    font = Create_Font_పరోక్ష_W(&lfSysmenu);

    Send_సందేశం(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = Get_Desktop_కిటికీ();
    hdc = Get_కిటికీ_DC(hwnd);
    hfntOld = SelectFont(hdc, font);

    Get_పాఠ్యం_Metrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_ఎత్తు = tm.tm_ఎత్తు + tm.tm_అంతర్గత_Leading + 7;
}
#లేకపోతే
# define సమితి_tabline_font()
#అయితే_ముగింపు

/*
 * Invoked when a setting was changed.
 */
    స్థిర LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    ఎంపిక (param)
    {
	స్థితి SPI_SETWHEELSCROLLLINES:
	    mouse_సమితి_vert_scroll_అడుగు(mouse_vertical_scroll_అడుగు());
	    ఆపు;
	స్థితి SPI_SETWHEELSCROLLCHARS:
	    mouse_సమితి_hor_scroll_అడుగు(mouse_horizontal_scroll_అడుగు());
	    ఆపు;
	స్థితి SPI_SETNONCLIENTMETRICS:
	    సమితి_tabline_font();
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి 0;
}

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    స్థిర శూన్య
On_కిటికీ_Pos_మారింది(
    HWND hwnd,
    స్థిరరాశి LPWINDOWPOS lpwpos)
{
    స్థిర పూర్ణాంకం x = 0, y = 0, cx = 0, cy = 0;
    బాహ్య పూర్ణాంకం WSInitialized;

    ఒకవేళ (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_చట్రం_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, Def_కిటికీ_Proc_W);
}
#అయితే_ముగింపు


స్థిర HWND hwndTip = ఖాళీ;

    స్థిర శూన్య
show_sizing_tip(పూర్ణాంకం నిలువుల_సంఖ్య, పూర్ణాంకం అడ్డువరుసలు_2)
{
    TOOLINFOA ti;
    అక్షరం buf[32];

    ti.cb_కొలత = పరిమాణం(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpsz_పాఠ్యం = buf;
    sprintf(buf, "%dx%d", నిలువుల_సంఖ్య, అడ్డువరుసలు_2);
    ఒకవేళ (hwndTip == ఖాళీ)
    {
	hwndTip = Create_కిటికీ_Ex_A(0, TOOLTIPS_CLASSA, ఖాళీ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ఖాళీ, GetModuleHandle(ఖాళీ), ఖాళీ);
	Send_సందేశం(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	Send_సందేశం(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    లేకపోతే
    {
	Send_సందేశం(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    Send_సందేశం(hwndTip, TTM_POPUP, 0, 0);
}

    స్థిర శూన్య
destroy_sizing_tip(శూన్య)
{
    ఒకవేళ (hwndTip == ఖాళీ)
	తిరిగి;

    Destroy_కిటికీ(hwndTip);
    hwndTip = ఖాళీ;
}

    స్థిర పూర్ణాంకం
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    పూర్ణాంకం	    w, h;
    పూర్ణాంకం	    valid_w, valid_h;
    పూర్ణాంకం	    w_స్థానభ్రంశం, h_స్థానభ్రంశం;
    పూర్ణాంకం	    నిలువుల_సంఖ్య, అడ్డువరుసలు_2;

    w = lprc->కుడి_2 - lprc->ఎడమ_2;
    h = lprc->bottom - lprc->శిఖరం;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &నిలువుల_సంఖ్య, &అడ్డువరుసలు_2);
    w_స్థానభ్రంశం = w - valid_w;
    h_స్థానభ్రంశం = h - valid_h;

    ఒకవేళ (fwSide == WMSZ_ఎడమ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ఎడమ_2 += w_స్థానభ్రంశం;
    లేకపోతే ఒకవేళ (fwSide == WMSZ_కుడి || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->కుడి_2 -= w_స్థానభ్రంశం;

    ఒకవేళ (fwSide == WMSZ_శిఖరం || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->శిఖరం += h_స్థానభ్రంశం;
    లేకపోతే ఒకవేళ (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_స్థానభ్రంశం;

    show_sizing_tip(నిలువుల_సంఖ్య, అడ్డువరుసలు_2);
    తిరిగి TRUE;
}

#నిర్వచితమైతే FEAT_GUI_TABLINE
    స్థిర శూన్య
On_RButton_పైకి(HWND hwnd, పూర్ణాంకం x, పూర్ణాంకం y, UINT కీలకం_Flags)
{
    ఒకవేళ (gui_mch_showing_tabline())
    {
	బిందువు_2 pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rect);
	ఒకవేళ (pt.y < rect.శిఖరం)
	{
	    show_tabline_popup_menu();
	    తిరిగి;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, కీలకం_Flags, Def_కిటికీ_Proc_W);
}

    స్థిర శూన్య
On_LButton_కిందికి(HWND hwnd, BOOL fDoubleClick, పూర్ణాంకం x, పూర్ణాంకం y, UINT కీలకం_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    ఒకవేళ (gui_mch_showing_tabline())
    {
	బిందువు_2 pt;
	RECT rect;

	GetCursorPos(&pt);
	Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rect);
	ఒకవేళ (pt.y < rect.శిఖరం)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, కీలకం_Flags, Def_కిటికీ_Proc_W);
}
#అయితే_ముగింపు

    స్థిర UINT
_OnNCHitTest(HWND hwnd, పూర్ణాంకం xPos, పూర్ణాంకం yPos)
{
    UINT	ఫలితం;
    పూర్ణాంకం		x, y;

    ఫలితం = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, Def_కిటికీ_Proc_W);
    ఒకవేళ (ఫలితం != HTCLIENT)
	తిరిగి ఫలితం;

#నిర్వచితమైతే FEAT_GUI_TABLINE
    ఒకవేళ (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	Get_కిటికీ_Rect(s_పాఠ్యం_Area, &rct);
	ఒకవేళ (yPos < rct.శిఖరం)
	    తిరిగి ఫలితం;
    }
#అయితే_ముగింపు
    (శూన్య)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    ఒకవేళ (xPos < 48) // <VN> TODO should use system metric?
	తిరిగి HTBOTTOMLEFT;
    లేకపోతే
	తిరిగి HTBOTTOMRIGHT;
}

#ఒకవేళ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    స్థిర LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    ఎంపిక (hdr->సంకేతం_2)
    {
	స్థితి TTN_GETDISPINFOW:
	స్థితి TTN_GETDISPINFO:
	    {
		char_u		*str = ఖాళీ;
		స్థిర శూన్య	*tt_పాఠ్యం = ఖాళీ;

		VIM_CLEAR(tt_పాఠ్యం);

# ifdef FEAT_GUI_TABLINE
		ఒకవేళ (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    బిందువు_2	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    ఒకవేళ (Screen_గమ్యం_Client(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			పూర్ణాంకం idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = Tab_Ctrl_తగిలింది_Test(s_tabhwnd, &htinfo);
			ఒకవేళ (idx != -1)
			{
			    tabpage_T *tp;

			    tp = కనుగొను_tabpage(idx + 1);
			    ఒకవేళ (tp != ఖాళీ)
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
		లేకపోతే
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_కనుగొను_menu(మూలం_menu, idButton);
		    ఒకవేళ (pMenu)
			str = pMenu->అక్షరశ్రేణులు[MENU_సూచీ_TIP];
		}
# endif
		ఒకవేళ (str == ఖాళీ)
		    ఆపు;

		// Set the maximum width, this also enables using \n for
		// line break.
		Send_సందేశం(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		ఒకవేళ (hdr->సంకేతం_2 == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_పాఠ్యం = enc_గమ్యం_utf16(str, ఖాళీ);
		    lpdi->lpsz_పాఠ్యం = tt_పాఠ్యం;
		    // can't show tooltip if failed
		}
		లేకపోతే
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    ఒకవేళ (len < పరిమాణం(lpdi->sz_పాఠ్యం)
			    || ((tt_పాఠ్యం = vim_strnsave(str, len)) == ఖాళీ))
			vim_strncpy((char_u *)lpdi->sz_పాఠ్యం, str,
				పరిమాణం(lpdi->sz_పాఠ్యం) - 1);
		    లేకపోతే
			lpdi->lpsz_పాఠ్యం = tt_పాఠ్యం;
		}
	    }
	    ఆపు;

# ifdef FEAT_GUI_TABLINE
	స్థితి TCN_SELCHANGE:
	    ఒకవేళ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		తిరిగి 0L;
	    }
	    ఆపు;

	స్థితి NM_RCLICK:
	    ఒకవేళ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		తిరిగి 0L;
	    }
	    ఆపు;
# endif

	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి Def_కిటికీ_Proc_W(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#అయితే_ముగింపు

#ఒకవేళ defined(MENUHINTS) && defined(FEAT_MENU)
    స్థిర LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ఒకవేళ (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & బహుళకం_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	స్థిర పూర్ణాంకం  did_menu_tip = FALSE;

	ఒకవేళ (did_menu_tip)
	{
	    సందేశం_clr_cmdline();
	    setcursor();
	    బహిర్గతం_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_కనుగొను_menu(మూలం_menu, (UINT)LOWORD(wParam));
	ఒకవేళ (pMenu != ఖాళీ && pMenu->అక్షరశ్రేణులు[MENU_సూచీ_TIP] != ఖాళీ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_కొలత = పరిమాణం(MENUITEMINFO);
	    menuinfo.f_ముసుగు = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    ఒకవేళ (Get_Menu_అంశం_Info(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++సందేశం_hist_off;
		సందేశం((అక్షరం *)pMenu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
		--సందేశం_hist_off;
		setcursor();
		బహిర్గతం_flush();
		did_menu_tip = TRUE;
	    }
	}
	తిరిగి 0L;
    }
    తిరిగి Def_కిటికీ_Proc_W(hwnd, WM_MENUSELECT, wParam, lParam);
}
#అయితే_ముగింపు

    స్థిర BOOL
On_Get_Dpi_Scaled_కొలత(HWND hwnd వాడని, UINT dpi, కొలత_3 *కొలత)
{
    పూర్ణాంకం		old_వెడల్పు, old_ఎత్తు;
    పూర్ణాంకం		new_వెడల్పు, new_ఎత్తు;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_కొలత(gui.norm_font, &old_వెడల్పు, &old_ఎత్తు);    // Current size
    GetObjectW((HFONT)gui.norm_font, పరిమాణం(lf), &lf);
    lf.lf_ఎత్తు = lf.lf_ఎత్తు * (పూర్ణాంకం)dpi / s_dpi;
    font = Create_Font_పరోక్ష_W(&lf);
    ఒకవేళ (font)
    {
	Get_Font_కొలత((GuiFont)font, &new_వెడల్పు, &new_ఎత్తు);	// New size
	DeleteFont(font);
    }
    లేకపోతే
    {
	new_వెడల్పు = old_వెడల్పు;
	new_ఎత్తు = old_ఎత్తు;
    }
    కొలత->cx = కొలత->cx * new_వెడల్పు / old_వెడల్పు;
    కొలత->cy = కొలత->cy * new_ఎత్తు / old_ఎత్తు;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    తిరిగి TRUE;
}

    స్థిర LRESULT
On_Dpi_మారింది(HWND hwnd, UINT xdpi వాడని, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_లోపల_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_కొలత();
    update_toolbar_కొలత();
    సమితి_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_ఎత్తు(FALSE);
#నిర్వచితమైతే FEAT_MBYTE_IME
    im_సమితి_position(gui.అడ్డువరుస, gui.నిలువు);
#అయితే_ముగింపు
    InvalidateRect(hwnd, ఖాళీ, TRUE);

    s_లోపల_dpichanged = FALSE;
    తిరిగి 0L;
}


    స్థిర LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_సందేశం,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_సందేశం, lParam);

    s_u_సందేశం = u_సందేశం;
    s_wParam = wParam;

    ఎంపిక (u_సందేశం)
    {
	HANDLE_సందేశం(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_సందేశం(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_సందేశం(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_సందేశం(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_సందేశం(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_సందేశం(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_సందేశం(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#నిర్వచితమైతే FEAT_MENU
	HANDLE_సందేశం(hwnd, WM_COMMAND,	_OnMenu);
#అయితే_ముగింపు
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_సందేశం(hwnd, WM_SETFOCUS,	On_సమితి_Focus);
	HANDLE_సందేశం(hwnd, WM_కొలత,	On_కొలత);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_సందేశం(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_సందేశం(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	HANDLE_సందేశం(hwnd, WM_WINDOWPOSCHANGED, On_కిటికీ_Pos_మారింది);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
	HANDLE_సందేశం(hwnd, WM_RBUTTONUP,	On_RButton_పైకి);
	HANDLE_సందేశం(hwnd, WM_LBUTTONDBLCLK,  On_LButton_కిందికి);
#అయితే_ముగింపు
	HANDLE_సందేశం(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    స్థితి WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	తిరిగి FALSE;		// Do NOT allow system to go down.

    స్థితి WM_ENDSESSION:
	ఒకవేళ (wParam)	// system only really goes down when wParam is TRUE
	{
	    On_చివర_Session();
	    తిరిగి 0L;
	}
	ఆపు;

    స్థితి WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (పూర్ణాంకం)(పొట్టి)LOWORD(lParam));
	తిరిగి 0L;

    స్థితి WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#నిర్వచితమైతే FEAT_MENU
	ఒకవేళ (	!gui.menu_is_చురుకైన
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((పూర్ణాంకం)wParam))
		)
#అయితే_ముగింపు
	{
	    _OnSysChar(hwnd, (UINT)wParam, (పూర్ణాంకం)(పొట్టి)LOWORD(lParam));
	    తిరిగి 0L;
	}
#నిర్వచితమైతే FEAT_MENU
	లేకపోతే
	    తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
#అయితే_ముగింపు

    స్థితి WM_SYSKEYUP:
#నిర్వచితమైతే FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
#లేకపోతే
	తిరిగి 0L;
#అయితే_ముగింపు

    స్థితి WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	ఆపు;

    స్థితి WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	తిరిగి _DuringSizing((UINT)wParam, (LPRECT)lParam);

    స్థితి WM_MOUSEWHEEL:
    స్థితి WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_సందేశం == WM_MOUSEHWHEEL);
	తిరిగి 0L;

	// Notification for change in SystemParametersInfo()
    స్థితి WM_SETTINGCHANGE:
	తిరిగి _OnSettingChange((UINT)wParam);

#ఒకవేళ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    స్థితి WM_NOTIFY:
	తిరిగి _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#అయితే_ముగింపు

#ఒకవేళ defined(MENUHINTS) && defined(FEAT_MENU)
    స్థితి WM_MENUSELECT:
	తిరిగి _OnMenuSelect(hwnd, wParam, lParam);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MBYTE_IME
    స్థితి WM_IME_NOTIFY:
	ఒకవేళ (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
	తిరిగి 1L;

    స్థితి WM_IME_COMPOSITION:
	ఒకవేళ (!_OnImeComposition(hwnd, wParam, lParam))
	    తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
	తిరిగి 1L;
#అయితే_ముగింపు
    స్థితి WM_GETDPISCALEDSIZE:
	తిరిగి On_Get_Dpi_Scaled_కొలత(hwnd, (UINT)wParam, (కొలత_3 *)lParam);
    స్థితి WM_DPICHANGED:
	తిరిగి On_Dpi_మారింది(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    అప్రమేయ:
#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
	ఒకవేళ (u_సందేశం == s_findrep_సందేశం && s_findrep_సందేశం != 0)
	    On_కనుగొను_Repl();
#అయితే_ముగింపు
	ఆపు;
    }

    తిరిగి Def_కిటికీ_Proc_W(hwnd, u_సందేశం, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_మాతృ_hwnd = ఖాళీ;

    స్థిర BOOL CALLBACK
కనుగొను_కిటికీ_Title(HWND hwnd, LPARAM lParam)
{
    అక్షరం	buf[2048];
    అక్షరం	*title = (అక్షరం *)lParam;

    ఒకవేళ (Get_కిటికీ_పాఠ్యం(hwnd, buf, పరిమాణం(buf)))
    {
	ఒకవేళ (అక్షరశ్రేణి_శోధన(buf, title) != ఖాళీ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_మాతృ_hwnd = కనుగొను_కిటికీ_Ex(hwnd, ఖాళీ, "MDIClient", ఖాళీ);
	    ఒకవేళ (vim_మాతృ_hwnd != ఖాళీ)
		తిరిగి FALSE;
	}
    }
    తిరిగి TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    శూన్య
gui_mch_సమితి_మాతృ(అక్షరం *title)
{
    EnumWindows(కనుగొను_కిటికీ_Title, (LPARAM)title);
    ఒకవేళ (vim_మాతృ_hwnd == ఖాళీ)
    {
	semsg(_(e_cannot_కనుగొను_కిటికీ_title_str), title);
	mch_exit(2);
    }
}

#నిర్వచితం_కాకపోతే FEAT_OLE
    స్థిర శూన్య
ole_error(అక్షరం *arg)
{
    అక్షరం buf[IOSIZE];

# ifdef VIMDLL
    gui.లోపల_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_వాదం_not_supported_str_use_ole_సంస్కరణ), arg);
    mch_errmsg(buf);
}
#అయితే_ముగింపు

#ఒకవేళ defined(GUI_MAY_SPAWN)
    స్థిర అక్షరం *
gvim_error(శూన్య)
{
    అక్షరం *సందేశం = _(e_gui_cannot_be_వాడిన_cannot_execute_gvim_exe);

    ఒకవేళ (starting)
    {
	mch_errmsg(సందేశం);
	mch_errmsg("\n");
	mch_exit(2);
    }
    తిరిగి సందేశం;
}

    అక్షరం *
gui_mch_do_spawn(char_u *arg)
{
    పూర్ణాంకం			len;
# ఒకవేళ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ఖాళీ;
    LPWSTR		tofree1 = ఖాళీ;
# endif
    WCHAR		name[MAX_మార్గం];
    LPWSTR		cmd, newcmd = ఖాళీ, p, warg, tofree2 = ఖాళీ;
    STARTUPINFOW	si = {పరిమాణం(si)};
    PROCESS_INFORMATION pi;

    ఒకవేళ (!GetModuleFileNameW(g_hinst, name, MAX_మార్గం))
	వెళ్ళు error;
    p = wcsrchr(name, L'\\');
    ఒకవేళ (p == ఖాళీ)
	వెళ్ళు error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# లేకపోతే
    wcscpy(p + 1, L"gvim.exe");
# endif

# ఒకవేళ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ఒకవేళ (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_పంక్తి_W();
	// Skip 1st argument.
	అయితే (*p && *p != L' ' && *p != L'\t')
	{
	    ఒకవేళ (*p == L'"')
	    {
		// Skip quoted strings
		అయితే (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# ఒకవేళ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    లేకపోతే
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	పూర్ణాంకం	ret;

	session = vim_tempname('s', FALSE);
	ఒకవేళ (session == ఖాళీ)
	    వెళ్ళు error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	ఒకవేళ (p_bg == ఖాళీ)
	{
	    p_bg = savebg;
	    వెళ్ళు error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	ఒకవేళ (!ret)
	    వెళ్ళు error;
	wsession = enc_గమ్యం_utf16(session, ఖాళీ);
	ఒకవేళ (wsession == ఖాళీ)
	    వెళ్ళు error;
	len = (పూర్ణాంకం)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	ఒకవేళ (cmd == ఖాళీ)
	{
	    vim_free(wsession);
	    వెళ్ళు error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    ఒకవేళ (arg != ఖాళీ)
    {
	warg = enc_గమ్యం_utf16(arg, ఖాళీ);
	ఒకవేళ (warg == ఖాళీ)
	    వెళ్ళు error;
	tofree2 = warg;
    }
    లేకపోతే
	warg = L"";

    // Set up the new command line.
    len = (పూర్ణాంకం)wcslen(name) + (పూర్ణాంకం)wcslen(cmd) + (పూర్ణాంకం)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    ఒకవేళ (newcmd == ఖాళీ)
	వెళ్ళు error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    ఒకవేళ (!CreateProcessW(ఖాళీ, newcmd, ఖాళీ, ఖాళీ, TRUE, 0,
		ఖాళీ, ఖాళీ, &si, &pi))
	వెళ్ళు error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# ఒకవేళ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ఒకవేళ (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    తిరిగి gvim_error();
}
#అయితే_ముగింపు

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    శూన్య
gui_mch_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    పూర్ణాంకం		silent = FALSE;
    పూర్ణాంకం		idx;

    // Check for special OLE command line parameters
    ఒకవేళ ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	ఒకవేళ (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	లేకపోతే
	    idx = 1;

	// Register Vim as an OLE Automation server
	ఒకవేళ (STRICMP(argv[idx] + 1, "register") == 0)
	{
#నిర్వచితమైతే FEAT_OLE
	    రిజిస్టరు_Me(silent);
	    mch_exit(0);
#లేకపోతే
	    ఒకవేళ (!silent)
		ole_error("register");
	    mch_exit(2);
#అయితే_ముగింపు
	}

	// Unregister Vim as an OLE Automation server
	ఒకవేళ (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#నిర్వచితమైతే FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#లేకపోతే
	    ఒకవేళ (!silent)
		ole_error("unregister");
	    mch_exit(2);
#అయితే_ముగింపు
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	ఒకవేళ (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#నిర్వచితమైతే FEAT_OLE
	    *argc = 1;
#లేకపోతే
	    ole_error("embedding");
	    mch_exit(2);
#అయితే_ముగింపు
	}
    }

#నిర్వచితమైతే FEAT_OLE
# ifdef VIMDLL
    ఒకవేళ (mch_is_gui_executable())
# endif
    {
	పూర్ణాంకం	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	ఒకవేళ (bDoRestart)
	    mch_exit(0);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	పూర్ణాంకం arg;

	కొరకు (arg = 1; arg < *argc; arg++)
	    ఒకవేళ (అక్షరశ్రేణి_పోలిక("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * పరిమాణం(అక్షరం *));
		argv[*argc] = ఖాళీ;
		ఆపు;	// enough?
	    }
    }
#అయితే_ముగింపు
}

    స్థిర శూన్య
ఎక్కించు_dpi_func(శూన్య)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    ఒకవేళ (hUser32 == ఖాళీ)
	వెళ్ళు fail;

    pGetDpiForSystem = (UINT (WINAPI *)(శూన్య))Get_Proc_చిరునామా(hUser32, "GetDpiForSystem");
    p_Get_Dpi_For_కిటికీ = (UINT (WINAPI *)(HWND))Get_Proc_చిరునామా(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (పూర్ణాంకం (WINAPI *)(పూర్ణాంకం, UINT))Get_Proc_చిరునామా(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    p_సమితి_Thread_Dpi_Awareness_Context = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_చిరునామా(hUser32, "SetThreadDpiAwarenessContext");
#నిర్వచితమైతే FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_చిరునామా(hUser32, "GetAwarenessFromDpiAwarenessContext");
#అయితే_ముగింపు

    ఒకవేళ (p_సమితి_Thread_Dpi_Awareness_Context != ఖాళీ)
    {
	DPI_AWARENESS_CONTEXT oldctx = p_సమితి_Thread_Dpi_Awareness_Context(
		DPI_AWARENESS_CONTEXT_చొప్పున_MONITOR_AWARE_V2);
	ఒకవేళ (oldctx != ఖాళీ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#నిర్వచితమైతే FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    ఒకవేళ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#అయితే_ముగింపు
	    తిరిగి;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    p_Get_Dpi_For_కిటికీ = ఖాళీ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    p_సమితి_Thread_Dpi_Awareness_Context = ఖాళీ;
#నిర్వచితమైతే FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ఖాళీ;
#అయితే_ముగింపు
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    పూర్ణాంకం
gui_mch_init(శూన్య)
{
    స్థిరరాశి WCHAR szVimWndClassW[] = VIM_CLASSW;
    స్థిరరాశి WCHAR sz_పాఠ్యం_Area_Class_W[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    ఒకవేళ (s_hwnd != ఖాళీ)
	వెళ్ళు theend;

    /*
     * Load the tearoff bitmap
     */
#నిర్వచితమైతే FEAT_TEAROFF
    s_htearbitmap = ఎక్కించు_Bitmap(g_hinst, "IDB_TEAROFF");
#అయితే_ముగింపు

    ఎక్కించు_dpi_func();

#నిర్వచితమైతే FEAT_GUI_DARKTHEME
    dyn_uxtheme_ఎక్కించు();
#అయితే_ముగింపు

    dyn_dwm_ఎక్కించు();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_కొలత();

#నిర్వచితమైతే FEAT_MENU
    gui.menu_ఎత్తు = 0;	// Windows takes care of this
#అయితే_ముగింపు
    gui.border_వెడల్పు = 0;
#నిర్వచితమైతే FEAT_TOOLBAR
    gui.toolbar_ఎత్తు = TOOLBAR_BUTTON_ఎత్తు + TOOLBAR_BORDER_ఎత్తు;
#అయితే_ముగింపు

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    ఒకవేళ (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ఎక్కించు_Icon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = ఎక్కించు_Cursor(ఖాళీ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ఖాళీ;
	wndclassw.lpszClassName = szVimWndClassW;

	ఒకవేళ (రిజిస్టరు_Class_W(&wndclassw) == 0)
	    తిరిగి FAIL;
    }

    ఒకవేళ (vim_మాతృ_hwnd != ఖాళీ)
    {
#నిర్వచితమైతే HAVE_TRY_EXCEPT
	__try
	{
#అయితే_ముగింపు
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = Create_కిటికీ_Ex_W(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_బిడ్డ
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_మాతృ_hwnd, ఖాళీ,
		g_hinst, ఖాళీ);
#నిర్వచితమైతే HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#అయితే_ముగింపు
	ఒకవేళ (s_hwnd == ఖాళీ)
	{
	    emsg(_(e_unable_గమ్యం_open_కిటికీ_inside_mdi_application));
	    mch_exit(2);
	}
    }
    లేకపోతే
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	ఒకవేళ (Is_కిటికీ((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = Create_కిటికీ_W(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		ఖాళీ, ఖాళీ,
		g_hinst, ఖాళీ);
	ఒకవేళ (s_hwnd != ఖాళీ && win_socket_id != 0)
	{
	    సమితి_మాతృ(s_hwnd, (HWND)win_socket_id);
	    Show_కిటికీ(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    ఒకవేళ (s_hwnd == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (p_Get_Dpi_For_కిటికీ != ఖాళీ)
    {
	s_dpi = p_Get_Dpi_For_కిటికీ(s_hwnd);
	update_scrollbar_కొలత();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#ఒకవేళ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_ఎక్కించు();
#అయితే_ముగింపు

    // Create the text area window
    ఒకవేళ (GetClassInfoW(g_hinst, sz_పాఠ్యం_Area_Class_W, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = పాఠ్యం_Area_Wnd_Proc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ఖాళీ;
	wndclassw.hCursor = ఎక్కించు_Cursor(ఖాళీ, IDC_ARROW);
	wndclassw.hbrBackground = ఖాళీ;
	wndclassw.lpszMenuName = ఖాళీ;
	wndclassw.lpszClassName = sz_పాఠ్యం_Area_Class_W;

	ఒకవేళ (రిజిస్టరు_Class_W(&wndclassw) == 0)
	    తిరిగి FAIL;
    }

    s_పాఠ్యం_Area = Create_కిటికీ_Ex_W(
	0,
	sz_పాఠ్యం_Area_Class_W, L"Vim text area",
	WS_బిడ్డ | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ఖాళీ,
	g_hinst, ఖాళీ);

    ఒకవేళ (s_పాఠ్యం_Area == ఖాళీ)
	తిరిగి FAIL;

#నిర్వచితమైతే FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ఖాళీ;

	ఒకవేళ (mch_icon_ఎక్కించు(&hIcon) == OK && hIcon != ఖాళీ)
	    Send_సందేశం(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
    s_menuBar = CreateMenu();
#అయితే_ముగింపు
    s_hdc = GetDC(s_పాఠ్యం_Area);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    సమితి_సాధారణ_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_వెనుక_pixel = gui.వెనుక_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_స్థానభ్రంశం = gui.border_వెడల్పు;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#నిర్వచితమైతే FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#అయితే_ముగింపు
#నిర్వచితమైతే MSWIN_కనుగొను_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_సందేశం = రిజిస్టరు_కిటికీ_సందేశం(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_కొలత = పరిమాణం(s_findrep_struct);
    s_findrep_struct.lpstr_కనుగొను_What = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstr_కనుగొను_What[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.w_కనుగొను_What_Len = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_EVAL
    // set the v:windowid variable
    సమితి_vim_var_nr(VV_WINDOWID, Handle_గమ్యం_Long(s_hwnd));
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_RENDER_OPTIONS
    ఒకవేళ (p_rop)
	(శూన్య)gui_mch_సమితి_rendering_options(p_rop);
#అయితే_ముగింపు

theend:
    // Display any pending error messages
    display_errors();

    తిరిగి OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    స్థిర శూన్య
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = Monitor_From_కిటికీ(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    ఒకవేళ (mon != ఖాళీ)
    {
	moninfo.cb_కొలత = పరిమాణం(MONITORINFO);
	ఒకవేళ (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    తిరిగి;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    శూన్య
gui_mch_సమితి_shellsize(
	పూర్ణాంకం వెడల్పు,
	పూర్ణాంకం ఎత్తు,
	పూర్ణాంకం min_వెడల్పు వాడని,
	పూర్ణాంకం min_ఎత్తు వాడని,
	పూర్ణాంకం పునాది_వెడల్పు వాడని,
	పూర్ణాంకం పునాది_ఎత్తు వాడని,
	పూర్ణాంకం direction)
{
    RECT	workarea_rect;
    RECT	కిటికీ_rect;
    పూర్ణాంకం		win_వెడల్పు, win_ఎత్తు;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    ఒకవేళ (IsZoomed(s_hwnd) && starting == 0)
	Show_కిటికీ(s_hwnd, SW_SHOWNORMAL);

    ఒకవేళ (s_లోపల_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	కిటికీ_rect = s_suggested_rect;
    లేకపోతే
	// Use current position.
	Get_కిటికీ_Rect(s_hwnd, &కిటికీ_rect);

    // compute the size of the outside of the window
    win_వెడల్పు = వెడల్పు + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_ఎత్తు = ఎత్తు + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_ఎత్తు(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    కిటికీ_rect.కుడి_2 = కిటికీ_rect.ఎడమ_2 + win_వెడల్పు;
    కిటికీ_rect.bottom = కిటికీ_rect.శిఖరం + win_ఎత్తు;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    ఒకవేళ (!s_లోపల_dpichanged)
    {
	ఒకవేళ ((direction & RESIZE_HOR)
		&& కిటికీ_rect.కుడి_2 > workarea_rect.కుడి_2)
	    స్థానభ్రంశం_Rect(&కిటికీ_rect,
		    workarea_rect.కుడి_2 - కిటికీ_rect.కుడి_2, 0);

	ఒకవేళ ((direction & RESIZE_HOR)
		&& కిటికీ_rect.ఎడమ_2 < workarea_rect.ఎడమ_2)
	    స్థానభ్రంశం_Rect(&కిటికీ_rect,
		    workarea_rect.ఎడమ_2 - కిటికీ_rect.ఎడమ_2, 0);

	ఒకవేళ ((direction & RESIZE_VERT)
		&& కిటికీ_rect.bottom > workarea_rect.bottom)
	    స్థానభ్రంశం_Rect(&కిటికీ_rect,
		    0, workarea_rect.bottom - కిటికీ_rect.bottom);

	ఒకవేళ ((direction & RESIZE_VERT)
		&& కిటికీ_rect.శిఖరం < workarea_rect.శిఖరం)
	    స్థానభ్రంశం_Rect(&కిటికీ_rect,
		    0, workarea_rect.శిఖరం - కిటికీ_rect.శిఖరం);
    }

    Move_కిటికీ(s_hwnd, కిటికీ_rect.ఎడమ_2, కిటికీ_rect.శిఖరం,
						win_వెడల్పు, win_ఎత్తు, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    సమితి_చురుకైన_కిటికీ(s_hwnd);
    సమితి_Focus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_ఎత్తు(!gui.starting);
}


    శూన్య
gui_mch_సమితి_scrollbar_thumb(
    scrollbar_T *sb,
    దీర్ఘ	val,
    దీర్ఘ	కొలత,
    దీర్ఘ	max)
{
    SCROLLINFO	info;

    sb->scroll_జరుపు = 0;
    అయితే (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	కొలత >>= 1;
	++sb->scroll_జరుపు;
    }

    ఒకవేళ (sb->scroll_జరుపు > 0)
	++కొలత;

    info.cb_కొలత = పరిమాణం(info);
    info.f_ముసుగు = SIF_POS | SIF_RANGE | SIF_పుట;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.n_పుట = కొలత;
    సమితి_Scroll_Info(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    శూన్య
gui_mch_సమితి_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    శూన్య
gui_mch_సమితి_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    శూన్య
gui_mch_సమితి_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    శూన్య
gui_mch_సమితి_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#నిర్వచితమైతే FEAT_MBYTE_IME
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
    స్థిర LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_దత్తాంశం వాడని)
{
    LRESULT l_ఫలితం = 0;
    HIMC hImc;

    ఒకవేళ (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	తిరిగి l_ఫలితం;
    ఎంపిక (dwCommand)
    {
	స్థితి IMN_SETOPENSTATUS:
	    ఒకవేళ (p_Imm_Get_Open_స్థితి(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		ఒకవేళ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lf_ఎత్తు = lf.lf_ఎత్తు * DEFAULT_DPI / s_dpi;
		p_Imm_సమితి_Composition_Font_W(hImc, &lf);
		im_సమితి_position(gui.అడ్డువరుస, gui.నిలువు);

		// Disable langmap
		State &= ~బహుళకం_LANGMAP;
		ఒకవేళ (State & బహుళకం_చొప్పించు)
		{
# ఒకవేళ defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    ఒకవేళ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			పూర్ణాంకం old_అడ్డువరుస = gui.అడ్డువరుస;
			పూర్ణాంకం old_నిలువు = gui.నిలువు;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			స్థితి_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.అడ్డువరుస = old_అడ్డువరుస;
			gui.నిలువు = old_నిలువు;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_ఫలితం = 0;
	    ఆపు;
    }
    pImmReleaseContext(hWnd, hImc);
    తిరిగి l_ఫలితం;
}

    స్థిర LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs వాడని, LPARAM param)
{
    char_u	*ret;
    పూర్ణాంకం		len;

    ఒకవేళ ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	తిరిగి 0;

    ret = Get_ఫలితం_Str(hwnd, GCS_RESULTSTR, &len);
    ఒకవేళ (ret != ఖాళీ)
    {
	కూడు_గమ్యం_ఆదానం_buf_csi(ret, len);
	vim_free(ret);
	తిరిగి 1;
    }
    తిరిగి 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    స్థిర char_u *
Get_ఫలితం_Str(HWND hwnd, పూర్ణాంకం GCS, పూర్ణాంకం *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ఖాళీ;
    char_u	*convbuf = ఖాళీ;

    ఒకవేళ (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	తిరిగి ఖాళీ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ఖాళీ, 0);
    ఒకవేళ (ret <= 0)
	తిరిగి ఖాళీ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + పరిమాణం(WCHAR));
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / పరిమాణం(WCHAR);

    convbuf = utf16_గమ్యం_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    తిరిగి convbuf;
}
#అయితే_ముగింపు

// For global functions we need prototypes.
#ఒకవేళ defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    శూన్య
im_సమితి_font(LOGFONTW *lf)
{
    HIMC hImc;

    ఒకవేళ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	p_Imm_సమితి_Composition_Font_W(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    శూన్య
im_సమితి_position(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు)
{
    HIMC hImc;

    ఒకవేళ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_బిందువు;
	cfs.ptCurrentPos.x = FILL_X(నిలువు);
	cfs.ptCurrentPos.y = FILL_Y(అడ్డువరుస);
	Map_కిటికీ_Points(s_పాఠ్యం_Area, s_hwnd, &cfs.ptCurrentPos, 1);
	ఒకవేళ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	p_Imm_సమితి_Composition_కిటికీ(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    శూన్య
im_సమితి_చురుకైన(పూర్ణాంకం చురుకైన)
{
    HIMC	hImc;
    స్థిర HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
    {
	mbyte_im_సమితి_చురుకైన(చురుకైన);
	తిరిగి;
    }
# endif

    ఒకవేళ (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	తిరిగి;

    ఒకవేళ (p_imdisable)
    {
	ఒకవేళ (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    ఒకవేళ (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	చురుకైన = FALSE;
    }
    లేకపోతే ఒకవేళ (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    ఒకవేళ (!hImc)
	తిరిగి;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    ఒకవేళ (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	స్థిర DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	స్థిర BOOL bSaved = FALSE;

	ఒకవేళ (చురుకైన)
	{
	    // if we have a saved conversion status, restore it
	    ఒకవేళ (bSaved)
		p_Imm_సమితి_Conversion_స్థితి(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	లేకపోతే
	{
	    // save conversion status and disable korean
	    ఒకవేళ (p_Imm_Get_Conversion_స్థితి(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		p_Imm_సమితి_Conversion_స్థితి(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    p_Imm_సమితి_Open_స్థితి(hImc, చురుకైన);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    పూర్ణాంకం
im_get_స్థితి(శూన్య)
{
    పూర్ణాంకం		స్థితి_2 = 0;
    HIMC	hImc;

# ifdef VIMDLL
    ఒకవేళ (!gui.లోపల_use && !gui.starting)
	తిరిగి mbyte_im_get_స్థితి();
# endif

    ఒకవేళ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	స్థితి_2 = p_Imm_Get_Open_స్థితి(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    తిరిగి స్థితి_2;
}

#అయితే_ముగింపు // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    స్థిర శూన్య
latin9_గమ్యం_ucs(char_u *పాఠ్యం, పూర్ణాంకం len, WCHAR *unicodebuf)
{
    పూర్ణాంకం		c;

    అయితే (--len >= 0)
    {
	c = *పాఠ్యం++;
	ఎంపిక (c)
	{
	    స్థితి 0xa4: c = 0x20ac; ఆపు;   // euro
	    స్థితి 0xa6: c = 0x0160; ఆపు;   // S hat
	    స్థితి 0xa8: c = 0x0161; ఆపు;   // S -hat
	    స్థితి 0xb4: c = 0x017d; ఆపు;   // Z hat
	    స్థితి 0xb8: c = 0x017e; ఆపు;   // Z -hat
	    స్థితి 0xbc: c = 0x0152; ఆపు;   // OE
	    స్థితి 0xbd: c = 0x0153; ఆపు;   // oe
	    స్థితి 0xbe: c = 0x0178; ఆపు;   // Y
	}
	*unicodebuf++ = c;
    }
}

#నిర్వచితమైతే FEAT_RIGHTLEFT
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
    స్థిర శూన్య
Rev_బహిర్గతం( HDC hdc,
	పూర్ణాంకం నిలువు,
	పూర్ణాంకం అడ్డువరుస,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR పాఠ్యం,
	UINT len,
	CONST INT *padding)
{
    పూర్ణాంకం		ix;

    కొరకు (ix = 0; ix < (పూర్ణాంకం)len; ++ix)
	Ext_పాఠ్యం_బహిర్గతం(hdc, నిలువు + పాఠ్యం_X(ix), అడ్డువరుస, foptions,
					pcliprect, పాఠ్యం + ix, 1, padding);
}
#అయితే_ముగింపు

    స్థిర శూన్య
draw_పంక్తి(
    పూర్ణాంకం		x1,
    పూర్ణాంకం		y1,
    పూర్ణాంకం		x2,
    పూర్ణాంకం		y2,
    COLORREF	color)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_పంక్తి(s_dwc, x1, y1, x2, y2, color);
    లేకపోతే
#అయితే_ముగింపు
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	Move_గమ్యం_Ex(s_hdc, x1, y1, ఖాళీ);
	// Note: LineTo() excludes the last pixel in the line.
	పంక్తి_గమ్యం(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    స్థిర శూన్య
సమితి_pixel(
    పూర్ణాంకం		x,
    పూర్ణాంకం		y,
    COLORREF	color)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWrite_Context_సమితి_Pixel(s_dwc, x, y, color);
    లేకపోతే
#అయితే_ముగింపు
	సమితి_Pixel(s_hdc, x, y, color);
}

    స్థిర శూన్య
fill_rect(
    స్థిరరాశి RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    లేకపోతే
#అయితే_ముగింపు
    {
	HBRUSH	hbr2;

	ఒకవేళ (hbr == ఖాళీ)
	    hbr2 = CreateSolidBrush(color);
	లేకపోతే
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	ఒకవేళ (hbr == ఖాళీ)
	    DeleteBrush(hbr2);
    }
}

    శూన్య
gui_mch_draw_string(
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    char_u	*పాఠ్యం,
    పూర్ణాంకం		len,
    పూర్ణాంకం		flags)
{
    స్థిర పూర్ణాంకం	*padding = ఖాళీ;
    స్థిర పూర్ణాంకం	pad_కొలత = 0;
    స్థిరరాశి RECT	*pcliprect = ఖాళీ;
    UINT	foptions = 0;
    స్థిర WCHAR *unicodebuf = ఖాళీ;
    స్థిర పూర్ణాంకం   *unicodepdy = ఖాళీ;
    స్థిర పూర్ణాంకం	unibuflen = 0;
    పూర్ణాంకం		n = 0;
    పూర్ణాంకం		y;

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
    స్థిర HBRUSH	hbr_cache[2] = {ఖాళీ, ఖాళీ};
    స్థిర guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    స్థిర పూర్ణాంకం		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    ఒకవేళ (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ఎడమ_2 = FILL_X(నిలువు);
	rc.శిఖరం = FILL_Y(అడ్డువరుస);
	ఒకవేళ (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.కుడి_2 = FILL_X(నిలువు + mb_string2cells(పాఠ్యం, len));
	}
	లేకపోతే
	    rc.కుడి_2 = FILL_X(నిలువు + len);
	rc.bottom = FILL_Y(అడ్డువరుస + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	ఒకవేళ (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	లేకపోతే ఒకవేళ (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	లేకపోతే
	{
	    ఒకవేళ (hbr_cache[brush_lru] != ఖాళీ)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	సమితి_Bk_బహుళకం(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	ఒకవేళ (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#నిర్వచితమైతే FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	లేకపోతే ఒకవేళ (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#అయితే_ముగింపు
    }
    సమితి_పాఠ్యం_Color(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#నిర్వచితమైతే FEAT_DIRECTX
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWrite_Context_సమితి_Font(s_dwc, (HFONT)gui.currFont);
#అయితే_ముగింపు

    ఒకవేళ (pad_కొలత != Columns || padding == ఖాళీ || padding[0] != gui.char_వెడల్పు)
    {
	పూర్ణాంకం	i;

	vim_free(padding);
	pad_కొలత = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(పూర్ణాంకం, pad_కొలత);
	ఒకవేళ (padding != ఖాళీ)
	    కొరకు (i = 0; i < pad_కొలత; i++)
		padding[i] = gui.char_వెడల్పు;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    ఒకవేళ (enc_utf8)
	కొరకు (n = 0; n < len; ++n)
	    ఒకవేళ (పాఠ్యం[n] >= 0x80)
		ఆపు;

#ఒకవేళ defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    ఒకవేళ (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#అయితే_ముగింపు

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    ఒకవేళ ((enc_utf8
		|| (enc_codepage > 0 && (పూర్ణాంకం)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ఖాళీ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(పూర్ణాంకం, len);

	unibuflen = len;
    }

    ఒకవేళ (enc_utf8 && n < len && unicodebuf != ఖాళీ)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	పూర్ణాంకం		i;
	పూర్ణాంకం		wlen;	// string length in words
	పూర్ణాంకం		cells;	// cell width of string up to composing char
	పూర్ణాంకం		cw;	// width of current cell
	పూర్ణాంకం		c;

	wlen = 0;
	cells = 0;
	కొరకు (i = 0; i < len; )
	{
	    c = utf_ptr2char(పాఠ్యం + i);
	    ఒకవేళ (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    లేకపోతే
	    {
		unicodebuf[wlen++] = c;
	    }

	    ఒకవేళ (utf_iscomposing(c))
		cw = 0;
	    లేకపోతే
	    {
		cw = utf_char2cells(c);
		ఒకవేళ (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    ఒకవేళ (unicodepdy != ఖాళీ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ఒకవేళ (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_వెడల్పు;
		    unicodepdy[wlen - 1] = 0;
		}
		లేకపోతే
		    unicodepdy[wlen - 1] = cw * gui.char_వెడల్పు;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(పాఠ్యం + i, len - i);
	}
#ఒకవేళ defined(FEAT_DIRECTX)
	ఒకవేళ (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWrite_Context_Draw_పాఠ్యం(s_dwc, unicodebuf, wlen,
		    పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_వెడల్పు, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	లేకపోతే
#అయితే_ముగింపు
	    Ext_పాఠ్యం_బహిర్గతం_W(s_hdc, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    లేకపోతే ఒకవేళ ((enc_codepage > 0 && (పూర్ణాంకం)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	ఒకవేళ (unicodebuf != ఖాళీ)
	{
	    ఒకవేళ (enc_latin9)
		latin9_గమ్యం_ucs(పాఠ్యం, len, unicodebuf);
	    లేకపోతే
		len = Multi_Byte_గమ్యం_Wide_Char(enc_codepage,
			MB_PRECOMPOSED,
			(అక్షరం *)పాఠ్యం, len,
			(LPWSTR)unicodebuf, unibuflen);
	    ఒకవేళ (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ఒకవేళ (unicodepdy != ఖాళీ)
		{
		    పూర్ణాంకం i;
		    పూర్ణాంకం cw;

		    కొరకు (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			ఒకవేళ (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_వెడల్పు;
		    }
		}
		Ext_పాఠ్యం_బహిర్గతం_W(s_hdc, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    లేకపోతే
    {
#నిర్వచితమైతే FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	ఒకవేళ (curwin->w_p_rl)
	    Rev_బహిర్గతం(s_hdc, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస),
			 foptions, pcliprect, (అక్షరం *)పాఠ్యం, len, padding);
	లేకపోతే
#అయితే_ముగింపు
	    Ext_పాఠ్యం_బహిర్గతం(s_hdc, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస),
			 foptions, pcliprect, (అక్షరం *)పాఠ్యం, len, padding);
    }

    // Underline
    ఒకవేళ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(అడ్డువరుస + 1) - 1;
	ఒకవేళ (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_పంక్తి(FILL_X(నిలువు), y, FILL_X(నిలువు + len), y, gui.currFgColor);
    }

    // Strikethrough
    ఒకవేళ (flags & DRAW_STRIKE)
    {
	y = FILL_Y(అడ్డువరుస + 1) - gui.char_ఎత్తు/2;
	draw_పంక్తి(FILL_X(నిలువు), y, FILL_X(నిలువు + len), y, gui.currSpColor);
    }

    // Undercurl
    ఒకవేళ (flags & DRAW_UNDERC)
    {
	పూర్ణాంకం			x;
	పూర్ణాంకం			స్థానభ్రంశం;
	స్థిర స్థిరరాశి పూర్ణాంకం	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(అడ్డువరుస + 1) - 1;
	కొరకు (x = FILL_X(నిలువు); x < FILL_X(నిలువు + len); ++x)
	{
	    స్థానభ్రంశం = val[x % 8];
	    సమితి_pixel(x, y - స్థానభ్రంశం, gui.currSpColor);
	}
    }
}


/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    శూన్య
gui_mch_flush(శూన్య)
{
#ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#అయితే_ముగింపు

    GdiFlush();
}

    స్థిర శూన్య
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ఖాళీ, gui.వెనుక_pixel);
}


    శూన్య
gui_mch_get_screen_dimensions(పూర్ణాంకం *screen_w, పూర్ణాంకం *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.కుడి_2 - workarea_rect.ఎడమ_2
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.శిఖరం
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_ఎత్తు(FALSE);
}


#ఒకవేళ defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    శూన్య
gui_mch_కూడు_menu(
    vimmenu_T	*menu,
    పూర్ణాంకం		pos)
{
    vimmenu_T	*మాతృ = menu->మాతృ;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    ఒకవేళ (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_గమ్యం_utf16(menu->name, ఖాళీ);
	ఒకవేళ (wn == ఖాళీ)
	    తిరిగి;

	infow.cb_కొలత = పరిమాణం(infow);
	infow.f_ముసుగు = MIIM_దత్తాంశం | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_అంశం_దత్తాంశం = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_దత్తాంశం = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	చొప్పించు_Menu_అంశం_W((మాతృ == ఖాళీ)
		? s_menuBar : మాతృ->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    ఒకవేళ (మాతృ == ఖాళీ)
	gui_mswin_get_menu_ఎత్తు(!gui.starting);
# ifdef FEAT_TEAROFF
    లేకపోతే ఒకవేళ (Is_కిటికీ(మాతృ->tearoff_handle))
	rebuild_tearoff(మాతృ);
# endif
}

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    బిందువు_2 mp;

    (శూన్య)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (పూర్ణాంకం)mp.x, (పూర్ణాంకం)mp.y);
}

    శూన్య
gui_make_popup(char_u *మార్గం_name, పూర్ణాంకం mouse_pos)
{
    vimmenu_T	*menu = gui_కనుగొను_menu(మార్గం_name);

    ఒకవేళ (menu == ఖాళీ)
	తిరిగి;

    బిందువు_2	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    ఒకవేళ (mouse_pos)
    {
	పూర్ణాంకం	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    లేకపోతే ఒకవేళ (curwin != ఖాళీ)
    {
	p.x += పాఠ్యం_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += పాఠ్యం_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    సందేశం_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (పూర్ణాంకం)p.x, (పూర్ణాంకం)p.y);
}

# ఒకవేళ defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    శూన్య
gui_make_tearoff(char_u *మార్గం_name)
{
    vimmenu_T	*menu = gui_కనుగొను_menu(మార్గం_name);

    // Found the menu, so tear it off.
    ఒకవేళ (menu != ఖాళీ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    శూన్య
gui_mch_కూడు_menu_అంశం(
    vimmenu_T	*menu,
    పూర్ణాంకం		idx)
{
    vimmenu_T	*మాతృ = menu->మాతృ;

    menu->id = s_menu_id++;
    menu->submenu_id = ఖాళీ;

# ifdef FEAT_TEAROFF
    ఒకవేళ (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	చొప్పించు_Menu(మాతృ->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    లేకపోతే
# endif
# ifdef FEAT_TOOLBAR
    ఒకవేళ (menu_is_toolbar(మాతృ->name))
    {
	TBBUTTON newtb;

	CLEAR_క్షేత్రం(newtb);
	ఒకవేళ (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	లేకపోతే
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	Send_సందేశం(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    లేకపోతే
# endif
    {
	WCHAR	*wn;

	wn = enc_గమ్యం_utf16(menu->name, ఖాళీ);
	ఒకవేళ (wn != ఖాళీ)
	{
	    చొప్పించు_Menu_W(మాతృ->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	ఒకవేళ (Is_కిటికీ(మాతృ->tearoff_handle))
	    rebuild_tearoff(మాతృ);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    శూన్య
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ఒకవేళ (menu->submenu_id == (HMENU)-1)
    {
	పూర్ణాంకం iButton;

	iButton = (పూర్ణాంకం)Send_సందేశం(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	Send_సందేశం(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    లేకపోతే
# endif
    {
	ఒకవేళ (menu->మాతృ != ఖాళీ
		&& menu_is_popup(menu->మాతృ->dname)
		&& menu->మాతృ->submenu_id != ఖాళీ)
	    RemoveMenu(menu->మాతృ->submenu_id, menu->id, MF_BYCOMMAND);
	లేకపోతే
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	ఒకవేళ (menu->submenu_id != ఖాళీ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	ఒకవేళ (Is_కిటికీ(menu->tearoff_handle))
	    Destroy_కిటికీ(menu->tearoff_handle);
	ఒకవేళ (menu->మాతృ != ఖాళీ
		&& menu->మాతృ->children != ఖాళీ
		&& Is_కిటికీ(menu->మాతృ->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->మాతృ);
	}
# endif
    }
}

# ifdef FEAT_TEAROFF
    స్థిర శూన్య
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    పూర్ణాంకం		x, y;

    HWND thwnd = menu->tearoff_handle;

    Get_కిటికీ_పాఠ్యం(thwnd, (LPSTR)tbuf, 127);
    ఒకవేళ (Get_కిటికీ_Rect(thwnd, &trect)
	    && Get_కిటికీ_Rect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ఎడమ_2 - rct.ఎడమ_2;
	y = (trect.శిఖరం -  rct.bottom  + roct.bottom);
    }
    లేకపోతే
    {
	x = y = 0xffffL;
    }
    Destroy_కిటికీ(thwnd);
    ఒకవేళ (menu->children != ఖాళీ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	ఒకవేళ (Is_కిటికీ(menu->tearoff_handle))
	    (శూన్య) సమితి_కిటికీ_Pos(menu->tearoff_handle,
				ఖాళీ,
				(పూర్ణాంకం)trect.ఎడమ_2,
				(పూర్ణాంకం)trect.శిఖరం,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    శూన్య
gui_mch_menu_grey(
    vimmenu_T	*menu,
    పూర్ణాంకం	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ఒకవేళ (menu->submenu_id == (HMENU)-1)
	Send_సందేశం(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    లేకపోతే
# endif
	(శూన్య)Enable_Menu_అంశం(menu->మాతృ ? menu->మాతృ->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    ఒకవేళ ((menu->మాతృ != ఖాళీ) && (Is_కిటికీ(menu->మాతృ->tearoff_handle)))
    {
	మాట_2 menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	ఒకవేళ (menu->children == ఖాళీ)
	    menuID = (మాట_2)(menu->id);
	లేకపోతే
	    menuID = (మాట_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = Get_Dlg_అంశం(menu->మాతృ->tearoff_handle, menuID);
	ఒకవేళ (menuHandle)
	    Enable_కిటికీ(menuHandle, !grey);

    }
# endif
}

#అయితే_ముగింపు // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#నిర్వచించు కూడు_మాట(x)		*p++ = (x)
#నిర్వచించు కూడు_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (మాట_2 *)dwp

#ఒకవేళ defined(FEAT_GUI_DIALOG)
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
    స్థిర LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT సందేశం_2,
    WPARAM wParam,
    LPARAM lParam వాడని)
{
    ఒకవేళ (సందేశం_2 == WM_INITDIALOG)
    {
	Center_కిటికీ(hwnd, Get_కిటికీ(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(శూన్య)సమితి_Focus(hwnd);
	ఒకవేళ (dialog_default_button > IDCANCEL)
	    (శూన్య)సమితి_Focus(Get_Dlg_అంశం(hwnd, dialog_default_button));
	లేకపోతే
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (శూన్య)సమితి_Focus(Get_Dlg_అంశం(hwnd, DLG_NONBUTTON_CONTROL));
	తిరిగి FALSE;
    }

    ఒకవేళ (సందేశం_2 == WM_COMMAND)
    {
	పూర్ణాంకం	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	ఒకవేళ (button >= DLG_NONBUTTON_CONTROL)
	    తిరిగి TRUE;

	// If the edit box exists, copy the string.
	ఒకవేళ (s_textfield != ఖాళీ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    Get_Dlg_అంశం_పాఠ్యం_W(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_గమ్యం_enc(wp, ఖాళీ);
	    ఒకవేళ (p != ఖాళీ)
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
	ఒకవేళ (button == IDOK)
	{
	    ఒకవేళ (dialog_default_button > IDCANCEL)
		చివర_Dialog(hwnd, dialog_default_button);
	}
	లేకపోతే
	    చివర_Dialog(hwnd, button - IDCANCEL);
	తిరిగి TRUE;
    }

    ఒకవేళ ((సందేశం_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	చివర_Dialog(hwnd, 0);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
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

స్థిర స్థిరరాశి అక్షరం *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    పూర్ణాంకం
gui_mch_dialog(
    పూర్ణాంకం		 type,
    char_u	*title,
    char_u	*సందేశం_2,
    char_u	*buttons,
    పూర్ణాంకం		 dfltbutton,
    char_u	*textfield,
    పూర్ణాంకం		ex_cmd వాడని)
{
    మాట_2	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    పూర్ణాంకం		అంకె_Buttons;
    పూర్ణాంకం		*buttonWidths, *buttonPositions;
    పూర్ణాంకం		buttonYpos;
    పూర్ణాంకం		nchar, i;
    DWORD	lStyle;
    పూర్ణాంకం		dlgwidth = 0;
    పూర్ణాంకం		dlgheight;
    పూర్ణాంకం		editboxheight;
    పూర్ణాంకం		horiz_వెడల్పు = 0;
    పూర్ణాంకం		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*చివరి_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    పూర్ణాంకం		font_ఎత్తు_2;
    పూర్ణాంకం		పాఠ్యం_వెడల్పు_2, min_Button_వెడల్పు, సందేశం_వెడల్పు;
    పూర్ణాంకం		max_Dialog_వెడల్పు;
    పూర్ణాంకం		max_Dialog_ఎత్తు;
    పూర్ణాంకం		scroll_flag = 0;
    పూర్ణాంకం		vertical;
    పూర్ణాంకం		dlgPaddingX;
    పూర్ణాంకం		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    పూర్ణాంకం		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    పూర్ణాంకం		l;
    పూర్ణాంకం		dlg_icon_వెడల్పు;
    పూర్ణాంకం		dlg_icon_ఎత్తు;
    పూర్ణాంకం		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    ఒకవేళ (!(gui.లోపల_use || gui.starting))
#  endif
	ఒకవేళ (silent_బహుళకం)
	    తిరిగి dfltbutton;   // return default option
# endif

    ఒకవేళ (s_hwnd == ఖాళీ)
    {
	ఎక్కించు_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    లేకపోతే
	dpi = pGetDpiForSystem();

    ఒకవేళ ((type < 0) || (type > VIM_చివరి_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_కొలత + STRLEN(సందేశం_2) * 2);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    ఒకవేళ (tbuffer == ఖాళీ)
	తిరిగి -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    అంకె_Buttons = 1;
    కొరకు (i = 0; tbuffer[i] != '\0'; i++)
    {
	ఒకవేళ (tbuffer[i] == DLG_BUTTON_SEP)
	    అంకె_Buttons++;
    }
    ఒకవేళ (dfltbutton >= అంకె_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(పూర్ణాంకం, అంకె_Buttons);
    ఒకవేళ (buttonWidths == ఖాళీ)
	తిరిగి -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(పూర్ణాంకం, అంకె_Buttons);
    ఒకవేళ (buttonPositions == ఖాళీ)
	తిరిగి -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = Get_Desktop_కిటికీ();
    hdc = Get_కిటికీ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ఒకవేళ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_పరోక్ష_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    లేకపోతే
# endif
	font = CreateFont(-DLG_FONT_బిందువు_కొలత, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    Get_పాఠ్యం_Metrics(hdc, &fontInfo);
    font_ఎత్తు_2 = fontInfo.tm_ఎత్తు;

    // Minimum width for horizontal button
    min_Button_వెడల్పు = Get_పాఠ్యం_వెడల్పు(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    ఒకవేళ (s_hwnd == ఖాళీ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	max_Dialog_వెడల్పు = workarea_rect.కుడి_2 - workarea_rect.ఎడమ_2 - 100;
	ఒకవేళ (max_Dialog_వెడల్పు > adjust_by_system_dpi(600))
	    max_Dialog_వెడల్పు = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	max_Dialog_ఎత్తు = workarea_rect.bottom - workarea_rect.శిఖరం - 150;
    }
    లేకపోతే
    {
	// Use our own window for the size, unless it's very small.
	Get_కిటికీ_Rect(s_hwnd, &rect);
	max_Dialog_వెడల్పు = rect.కుడి_2 - rect.ఎడమ_2
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	ఒకవేళ (max_Dialog_వెడల్పు < adjust_by_system_dpi(DLG_MIN_MAX_వెడల్పు))
	    max_Dialog_వెడల్పు = adjust_by_system_dpi(DLG_MIN_MAX_వెడల్పు);

	max_Dialog_ఎత్తు = rect.bottom - rect.శిఖరం
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	ఒకవేళ (max_Dialog_ఎత్తు < adjust_by_system_dpi(DLG_MIN_MAX_ఎత్తు))
	    max_Dialog_ఎత్తు = adjust_by_system_dpi(DLG_MIN_MAX_ఎత్తు);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = సందేశం_2;
    సందేశం_వెడల్పు = 0;
    msgheight = 0;
    ga_init2(&ga, పరిమాణం(అక్షరం), 500);
    చేయి
    {
	msgheight += font_ఎత్తు_2;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	పాఠ్యం_వెడల్పు_2 = 0;
	చివరి_white = ఖాళీ;
	కొరకు (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    ఒకవేళ (l == 1 && VIM_ISWHITE(*pend)
					&& పాఠ్యం_వెడల్పు_2 > max_Dialog_వెడల్పు * 3 / 4)
		చివరి_white = pend;
	    పాఠ్యం_వెడల్పు_2 += Get_పాఠ్యం_వెడల్పు_Enc(hdc, pend, l);
	    ఒకవేళ (పాఠ్యం_వెడల్పు_2 >= max_Dialog_వెడల్పు)
	    {
		// Line will wrap.
		సందేశం_వెడల్పు = max_Dialog_వెడల్పు;
		msgheight += font_ఎత్తు_2;
		పాఠ్యం_వెడల్పు_2 = 0;

		ఒకవేళ (చివరి_white != ఖాళీ)
		{
		    // break the line just after a space
		    ఒకవేళ (pend > చివరి_white)
			ga.ga_len -= (పూర్ణాంకం)(pend - (చివరి_white + 1));
		    pend = చివరి_white + 1;
		    చివరి_white = ఖాళీ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		కొనసాగు;
	    }

	    అయితే (--l >= 0)
		ga_append(&ga, *pend++);
	}
	ఒకవేళ (పాఠ్యం_వెడల్పు_2 > సందేశం_వెడల్పు)
	    సందేశం_వెడల్పు = పాఠ్యం_వెడల్పు_2;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } అయితే (*pend != NUL);

    ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
	సందేశం_2 = ga.ga_దత్తాంశం;

    సందేశం_వెడల్పు += 10;		// roundoff space

    dlg_icon_వెడల్పు = adjust_by_system_dpi(DLG_ICON_వెడల్పు);
    dlg_icon_ఎత్తు = adjust_by_system_dpi(DLG_ICON_ఎత్తు);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = సందేశం_వెడల్పు + dlg_icon_వెడల్పు + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    ఒకవేళ (msgheight < dlg_icon_ఎత్తు)
	msgheight = dlg_icon_ఎత్తు;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ఖాళీ && vim_strchr(p_go, GO_VERTICAL) != ఖాళీ);
    ఒకవేళ (!vertical)
    {
	// Place buttons horizontally if they fit.
	horiz_వెడల్పు = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	చేయి
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ఒకవేళ (pend == ఖాళీ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    పాఠ్యం_వెడల్పు_2 = Get_పాఠ్యం_వెడల్పు_Enc(hdc, pstart, (పూర్ణాంకం)(pend - pstart));
	    ఒకవేళ (పాఠ్యం_వెడల్పు_2 < min_Button_వెడల్పు)
		పాఠ్యం_వెడల్పు_2 = min_Button_వెడల్పు;
	    పాఠ్యం_వెడల్పు_2 += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = పాఠ్యం_వెడల్పు_2;
	    buttonPositions[i++] = horiz_వెడల్పు;
	    horiz_వెడల్పు += పాఠ్యం_వెడల్పు_2 + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} అయితే (*pend != NUL);

	ఒకవేళ (horiz_వెడల్పు > max_Dialog_వెడల్పు)
	    vertical = TRUE;	// Too wide to fit on the screen.
	లేకపోతే ఒకవేళ (horiz_వెడల్పు > dlgwidth)
	    dlgwidth = horiz_వెడల్పు;
    }

    ఒకవేళ (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	చేయి
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ఒకవేళ (pend == ఖాళీ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    పాఠ్యం_వెడల్పు_2 = Get_పాఠ్యం_వెడల్పు_Enc(hdc, pstart, (పూర్ణాంకం)(pend - pstart));
	    పాఠ్యం_వెడల్పు_2 += dlgPaddingX;		// Padding within button
	    పాఠ్యం_వెడల్పు_2 += DLG_VERT_PADDING_X * 2; // Padding around button
	    ఒకవేళ (పాఠ్యం_వెడల్పు_2 > dlgwidth)
		dlgwidth = పాఠ్యం_వెడల్పు_2;
	    pstart = pend + 1;
	} అయితే (*pend != NUL);
    }

    ఒకవేళ (dlgwidth < DLG_MIN_వెడల్పు)
	dlgwidth = DLG_MIN_వెడల్పు;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    కూడు_long(lStyle);
    కూడు_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    కూడు_మాట(0);	// NumberOfItems(will change later)
    కూడు_మాట(10);	// x
    కూడు_మాట(10);	// y
    కూడు_మాట(Pixel_గమ్యం_Dialog_X(dlgwidth));	// cx

    // Dialog height.
    ఒకవేళ (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * font_ఎత్తు_2 * అంకె_Buttons;
    లేకపోతే
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * font_ఎత్తు_2;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = font_ఎత్తు_2 + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    ఒకవేళ (textfield != ఖాళీ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    ఒకవేళ (dlgheight > max_Dialog_ఎత్తు)
    {
	msgheight = msgheight - (dlgheight - max_Dialog_ఎత్తు);
	dlgheight = max_Dialog_ఎత్తు;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	సందేశం_వెడల్పు = dlgwidth - dlg_icon_వెడల్పు - 3 * dlgPaddingX;
    }

    కూడు_మాట(Pixel_గమ్యం_Dialog_Y(dlgheight));

    కూడు_మాట(0);	// Menu
    కూడు_మాట(0);	// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_గమ్యం_Wide_Char(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_సంస్కరణ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ఒకవేళ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ఎత్తు, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (పూర్ణాంకం)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    లేకపోతే
# endif
    {
	*p++ = DLG_FONT_బిందువు_కొలత;		// point size
	nchar = n_Copy_Ansi_గమ్యం_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    ఒకవేళ (textfield != ఖాళీ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    ఒకవేళ (!vertical)
	horiz_వెడల్పు = (dlgwidth - horiz_వెడల్పు) / 2;	// Now it's X offset
    కొరకు (i = 0; i < అంకె_Buttons; i++)
    {
	// get end of this button.
	కొరకు (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	ఒకవేళ (*pend)
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
	ఒకవేళ (vertical)
	{
	    p = కూడు_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_గమ్యం_Dialog_X(DLG_VERT_PADDING_X),
		    Pixel_గమ్యం_Dialog_Y(buttonYpos // TBK
				   + 2 * font_ఎత్తు_2 * i),
		    Pixel_గమ్యం_Dialog_X(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (మాట_2)(Pixel_గమ్యం_Dialog_Y(2 * font_ఎత్తు_2) - 1),
		    (మాట_2)(IDCANCEL + 1 + i), (మాట_2)0x0080, (అక్షరం *)pstart);
	}
	లేకపోతే
	{
	    p = కూడు_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_గమ్యం_Dialog_X(horiz_వెడల్పు + buttonPositions[i]),
		    Pixel_గమ్యం_Dialog_Y(buttonYpos), // TBK
		    Pixel_గమ్యం_Dialog_X(buttonWidths[i]),
		    (మాట_2)(Pixel_గమ్యం_Dialog_Y(2 * font_ఎత్తు_2) - 1),
		    (మాట_2)(IDCANCEL + 1 + i), (మాట_2)0x0080, (అక్షరం *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += అంకె_Buttons;

    // Vim icon
    p = కూడు_dialog_element(p, SS_ICON,
	    Pixel_గమ్యం_Dialog_X(dlgPaddingX),
	    Pixel_గమ్యం_Dialog_Y(dlgPaddingY),
	    Pixel_గమ్యం_Dialog_X(dlg_icon_వెడల్పు),
	    Pixel_గమ్యం_Dialog_Y(dlg_icon_ఎత్తు),
	    DLG_NONBUTTON_CONTROL + 0, (మాట_2)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = కూడు_dialog_element(p, ES_ఎడమ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    Pixel_గమ్యం_Dialog_X(2 * dlgPaddingX + dlg_icon_వెడల్పు),
	    Pixel_గమ్యం_Dialog_Y(dlgPaddingY),
	    (మాట_2)(Pixel_గమ్యం_Dialog_X(సందేశం_వెడల్పు) + 1),
	    Pixel_గమ్యం_Dialog_Y(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (మాట_2)0x0081, (అక్షరం *)సందేశం_2);

    // Edit box
    ఒకవేళ (textfield != ఖాళీ)
    {
	p = కూడు_dialog_element(p, ES_ఎడమ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		Pixel_గమ్యం_Dialog_X(2 * dlgPaddingX),
		Pixel_గమ్యం_Dialog_Y(2 * dlgPaddingY + msgheight),
		Pixel_గమ్యం_Dialog_X(dlgwidth - 4 * dlgPaddingX),
		Pixel_గమ్యం_Dialog_Y(font_ఎత్తు_2 + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (మాట_2)0x0081, (అక్షరం *)textfield);
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
    ఒకవేళ (textfield != ఖాళీ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    లేకపోతే
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ఖాళీ;
    }

    // show the dialog box modally and get a return value
    nchar = (పూర్ణాంకం)Dialog_Box_పరోక్ష(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_దత్తాంశం);

    // Focus back to our window (for when MDI is used).
    (శూన్య)సమితి_Focus(s_hwnd);

    తిరిగి nchar;
}

#అయితే_ముగింపు // FEAT_GUI_DIALOG

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
    స్థిర PWORD
కూడు_dialog_element(
    PWORD p,
    DWORD lStyle,
    మాట_2 x,
    మాట_2 y,
    మాట_2 w,
    మాట_2 h,
    మాట_2 Id,
    మాట_2 clss,
    స్థిరరాశి అక్షరం *caption)
{
    పూర్ణాంకం nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_బిడ్డ;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (మాట_2)0xffff;
    *p++ = clss;			//2 more here

    nchar = n_Copy_Ansi_గమ్యం_Wide_Char(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    తిరిగి p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    స్థిర LPWORD
lpwAlign(
    LPWORD lp_లోపల)
{
    long_u ul;

    ul = (long_u)lp_లోపల;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    తిరిగి (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    స్థిర పూర్ణాంకం
n_Copy_Ansi_గమ్యం_Wide_Char(
    LPWORD lpWCStr,
    LPSTR lp_Ansi_లోపల,
    BOOL use_enc)
{
    పూర్ణాంకం		nChar = 0;
    పూర్ణాంకం		len = lstrlen(lp_Ansi_లోపల) + 1;	// include NUL character
    పూర్ణాంకం		i;
    WCHAR	*wn;

    ఒకవేళ (use_enc && enc_codepage >= 0 && (పూర్ణాంకం)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_గమ్యం_utf16((char_u *)lp_Ansi_లోపల, ఖాళీ);
	ఒకవేళ (wn != ఖాళీ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (పూర్ణాంకం)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    ఒకవేళ (nChar == 0)
	// Use Win32 conversion function.
	nChar = Multi_Byte_గమ్యం_Wide_Char(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lp_Ansi_లోపల, len,
		lpWCStr, len);
    కొరకు (i = 0; i < nChar; ++i)
	ఒకవేళ (lpWCStr[i] == (మాట_2)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (మాట_2)' ';

    తిరిగి nChar;
}


#నిర్వచితమైతే FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    స్థిర HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    మాట_2 menu_id)
{
    కొరకు ( ; menu != ఖాళీ; menu = menu->తదుపరి)
    {
	ఒకవేళ (menu->modes == 0)	// this menu has just been deleted
	    కొనసాగు;
	ఒకవేళ (menu_is_separator(menu->dname))
	    కొనసాగు;
	ఒకవేళ ((మాట_2)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    తిరిగి menu->submenu_id;
    }
    తిరిగి ఖాళీ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    స్థిర LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT సందేశం_2,
    WPARAM wParam,
    LPARAM lParam)
{
    ఒకవేళ (సందేశం_2 == WM_INITDIALOG)
    {
	సమితి_కిటికీ_Long_Ptr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	తిరిగి (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(సందేశం_2, lParam);

    ఒకవేళ (సందేశం_2 == WM_COMMAND)
    {
	ఒకవేళ ((మాట_2)(LOWORD(wParam)) & 0x8000)
	{
	    బిందువు_2   mp;
	    RECT    rect;

	    ఒకవేళ (GetCursorPos(&mp) && Get_కిటికీ_Rect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)Get_కిటికీ_Long_Ptr(hwnd, DWLP_USER);
		(శూన్య)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (పూర్ణాంకం)rect.కుడి_2 - 8,
			 (పూర్ణాంకం)mp.y,
			 (పూర్ణాంకం)0,	    // reserved param
			 s_hwnd,
			 ఖాళీ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	లేకపోతే
	    // Pass on messages to the main Vim window
	    Post_సందేశం(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(శూన్య)సమితి_Focus(s_hwnd);
	తిరిగి TRUE;
    }
    ఒకవేళ ((సందేశం_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	Destroy_కిటికీ(hwnd);
	తిరిగి TRUE;
    }

    // When moved around, give main window the focus back.
    ఒకవేళ (సందేశం_2 == WM_EXITSIZEMOVE)
	(శూన్య)సమితి_చురుకైన_కిటికీ(s_hwnd);

    తిరిగి FALSE;
}
#అయితే_ముగింపు


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    స్థిర శూన్య
get_dialog_font_metrics(శూన్య)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    కొలత_3	    కొలత;
#నిర్వచితమైతే USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SYSMENU_FONT
    ఒకవేళ (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = Create_Font_పరోక్ష_W(&lfSysmenu);
    లేకపోతే
#అయితే_ముగింపు
	hfontTools = CreateFont(-DLG_FONT_బిందువు_కొలత, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_సరాసరి_Font_కొలత(hdc, &కొలత);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (మాట_2)కొలత.cx;
    s_dlgfntheight = (మాట_2)కొలత.cy;
}

#ఒకవేళ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    స్థిర శూన్య
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    పూర్ణాంకం		initX,
    పూర్ణాంకం		initY)
{
    మాట_2	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    పూర్ణాంకం		template_len;
    పూర్ణాంకం		nchar, పాఠ్యం_వెడల్పు_2, submenu_వెడల్పు;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    మాట_2	dlgwidth;
    మాట_2	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*శిఖరం_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    పూర్ణాంకం		నిలువు, space_వెడల్పు, len;
    పూర్ణాంకం		స్తంభం_Widths[2];
    char_u	*label, *పాఠ్యం;
    పూర్ణాంకం		acLen = 0;
    పూర్ణాంకం		nameLen;
    పూర్ణాంకం		padding0, padding1, padding2 = 0;
    పూర్ణాంకం		sepPadding=0;
    పూర్ణాంకం		x;
    పూర్ణాంకం		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    పూర్ణాంకం		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    ఒకవేళ (Is_కిటికీ(menu->tearoff_handle))
    {
	బిందువు_2 mp;
	ఒకవేళ (GetCursorPos(&mp))
	{
	    సమితి_కిటికీ_Pos(menu->tearoff_handle, ఖాళీ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	తిరిగి;
    }

    /*
     * Create a new tearoff.
     */
    ఒకవేళ (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_కొలత;
    pdlgtemplate = p = (మాట_2 *)LocalAlloc(LPTR, template_len);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;

    hwnd = Get_Desktop_కిటికీ();
    hdc = Get_కిటికీ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ఒకవేళ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_పరోక్ష_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    లేకపోతే
# endif
	font = CreateFont(-DLG_FONT_బిందువు_కొలత, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    space_వెడల్పు = Get_పాఠ్యం_వెడల్పు(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenu_వెడల్పు = 0;
    కొరకు (నిలువు = 0; నిలువు < 2; నిలువు++)
    {
	స్తంభం_Widths[నిలువు] = 0;
	FOR_ALL_బిడ్డ_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    పాఠ్యం = (నిలువు == 0) ? pmenu->dname : pmenu->actext;
	    ఒకవేళ (పాఠ్యం != ఖాళీ && *పాఠ్యం != NUL)
	    {
		పాఠ్యం_వెడల్పు_2 = Get_పాఠ్యం_వెడల్పు_Enc(hdc, పాఠ్యం, (పూర్ణాంకం)STRLEN(పాఠ్యం));
		ఒకవేళ (పాఠ్యం_వెడల్పు_2 > స్తంభం_Widths[నిలువు])
		    స్తంభం_Widths[నిలువు] = పాఠ్యం_వెడల్పు_2;
	    }
	    ఒకవేళ (pmenu->children != ఖాళీ)
		submenu_వెడల్పు = TEAROFF_స్తంభం_PADDING * space_వెడల్పు;
	}
    }
    ఒకవేళ (స్తంభం_Widths[1] == 0)
    {
	// no accelerators
	ఒకవేళ (submenu_వెడల్పు != 0)
	    స్తంభం_Widths[0] += submenu_వెడల్పు;
	లేకపోతే
	    స్తంభం_Widths[0] += space_వెడల్పు;
    }
    లేకపోతే
    {
	// there is an accelerator column
	స్తంభం_Widths[0] += TEAROFF_స్తంభం_PADDING * space_వెడల్పు;
	స్తంభం_Widths[1] += submenu_వెడల్పు;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    పాఠ్యం_వెడల్పు_2 = స్తంభం_Widths[0] + స్తంభం_Widths[1];
    ఒకవేళ (submenu_వెడల్పు != 0)
    {
	submenu_వెడల్పు = Get_పాఠ్యం_వెడల్పు(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (పూర్ణాంకం)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	పాఠ్యం_వెడల్పు_2 += submenu_వెడల్పు;
    }
    dlgwidth = Get_పాఠ్యం_వెడల్పు_Enc(hdc, title, (పూర్ణాంకం)STRLEN(title));
    ఒకవేళ (పాఠ్యం_వెడల్పు_2 > dlgwidth)
	dlgwidth = పాఠ్యం_వెడల్పు_2;
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
    ఒకవేళ (initX == 0xffffL)
	*p++ = Pixel_గమ్యం_Dialog_X(x); // x
    లేకపోతే
	*p++ = Pixel_గమ్యం_Dialog_X(initX); // x
    ఒకవేళ (initY == 0xffffL)
	*p++ = Pixel_గమ్యం_Dialog_Y(y); // y
    లేకపోతే
	*p++ = Pixel_గమ్యం_Dialog_Y(initY); // y
    *p++ = Pixel_గమ్యం_Dialog_X(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_గమ్యం_Wide_Char(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_సంస్కరణ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ఒకవేళ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ఎత్తు, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (పూర్ణాంకం)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    లేకపోతే
# endif
    {
	*p++ = DLG_FONT_బిందువు_కొలత;		// point size
	nchar = n_Copy_Ansi_గమ్యం_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    ఒకవేళ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->తదుపరి;
    లేకపోతే
	menu = menu->children;
    శిఖరం_menu = menu;
    కొరకు ( ; menu != ఖాళీ; menu = menu->తదుపరి)
    {
	ఒకవేళ (menu->modes == 0)	// this menu has just been deleted
	    కొనసాగు;
	ఒకవేళ (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    కొనసాగు;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	ఒకవేళ (((అక్షరం *)p - (అక్షరం *)pdlgtemplate) + 1000 > template_len)
	{
	    మాట_2    *newp;

	    newp = (మాట_2 *)LocalAlloc(LPTR, template_len + 4096);
	    ఒకవేళ (newp != ఖాళీ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (అక్షరం *)p - (అక్షరం *)pdlgtemplate);
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
	len = nameLen = (పూర్ణాంకం)STRLEN(menu->name);
	padding0 = (స్తంభం_Widths[0] - Get_పాఠ్యం_వెడల్పు_Enc(hdc, menu->dname,
				      (పూర్ణాంకం)STRLEN(menu->dname))) / space_వెడల్పు;
	len += padding0;

	ఒకవేళ (menu->actext != ఖాళీ)
	{
	    acLen = (పూర్ణాంకం)STRLEN(menu->actext);
	    len += acLen;
	    పాఠ్యం_వెడల్పు_2 = Get_పాఠ్యం_వెడల్పు_Enc(hdc, menu->actext, acLen);
	}
	లేకపోతే
	    పాఠ్యం_వెడల్పు_2 = 0;
	padding1 = (స్తంభం_Widths[1] - పాఠ్యం_వెడల్పు_2) / space_వెడల్పు;
	len += padding1;

	ఒకవేళ (menu->children == ఖాళీ)
	{
	    padding2 = submenu_వెడల్పు / space_వెడల్పు;
	    len += padding2;
	    menuID = (మాట_2)(menu->id);
	}
	లేకపోతే
	{
	    len += (పూర్ణాంకం)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (మాట_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	పాఠ్యం = label = alloc(len + 1);
	ఒకవేళ (label == ఖాళీ)
	    ఆపు;

	vim_strncpy(పాఠ్యం, menu->name, nameLen);
	పాఠ్యం = vim_strchr(పాఠ్యం, TAB);	    // stop at TAB before actext
	ఒకవేళ (పాఠ్యం == ఖాళీ)
	    పాఠ్యం = label + nameLen;	    // no actext, use whole name
	అయితే (padding0-- > 0)
	    *పాఠ్యం++ = ' ';
	ఒకవేళ (menu->actext != ఖాళీ)
	{
	    STRNCPY(పాఠ్యం, menu->actext, acLen);
	    పాఠ్యం += acLen;
	}
	అయితే (padding1-- > 0)
	    *పాఠ్యం++ = ' ';
	ఒకవేళ (menu->children != ఖాళీ)
	{
	    STRCPY(పాఠ్యం, TEAROFF_SUBMENU_LABEL);
	    పాఠ్యం += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	లేకపోతే
	{
	    అయితే (padding2-- > 0)
		*పాఠ్యం++ = ' ';
	}
	*పాఠ్యం = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = కూడు_dialog_element(p,
		BS_PUSHBUTTON|BS_ఎడమ,
		(మాట_2)Pixel_గమ్యం_Dialog_X(TEAROFF_PADDING_X),
		(మాట_2)(sepPadding + 1 + 13 * (*pnumitems)),
		(మాట_2)Pixel_గమ్యం_Dialog_X(dlgwidth - 2 * TEAROFF_PADDING_X),
		(మాట_2)12,
		menuID, (మాట_2)0x0080, (అక్షరం *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (మాట_2)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = Create_Dialog_పరోక్ష_Param(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback,
	    (LPARAM)శిఖరం_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (శూన్య)సమితి_చురుకైన_కిటికీ(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    స్థిర శూన్య
initialise_toolbar(శూన్య)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_బిడ్డ | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ఖాళీ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_వెడల్పు, //api guide is wrong!
		    TOOLBAR_BUTTON_ఎత్తు,
		    TOOLBAR_BUTTON_వెడల్పు,
		    TOOLBAR_BUTTON_ఎత్తు,
		    పరిమాణం(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    Send_సందేశం(s_toolbarhwnd, TB_SETSTYLE, 0,
	Send_సందేశం(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = Subclass_కిటికీ(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ);

    update_toolbar_కొలత();
}

    స్థిర శూన్య
update_toolbar_కొలత(శూన్య)
{
    పూర్ణాంకం		w, h;
    TBMETRICS	tbm;

    tbm.cb_కొలత = పరిమాణం(TBMETRICS);
    tbm.dw_ముసుగు = TBMF_PAD | TBMF_BUTTONSPACING;
    Send_సందేశం(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_వెడల్పు + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_ఎత్తు + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    Send_సందేశం(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_ఎత్తు = h + 6;

    //DWORD s = SendMessage(s_toolbarhwnd, TB_GETBUTTONSIZE, 0, 0);
    //TRACE("actual button size: %d, %d", LOWORD(s), HIWORD(s));

    // TODO:
    // Currently, this function only updates the size of toolbar buttons.
    // It would be nice if the toolbar images are resized based on DPI.
}

    స్థిర LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_సందేశం,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_సందేశం, lParam);
    తిరిగి Call_కిటికీ_Proc(s_toolbar_wndproc, hwnd, u_సందేశం, wParam, lParam);
}

    స్థిర పూర్ణాంకం
get_toolbar_bitmap(vimmenu_T *menu)
{
    పూర్ణాంకం i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    ఒకవేళ (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ఖాళీ;

	ఒకవేళ (menu->iconfile != ఖాళీ)
	{
	    gui_కనుగొను_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = ఎక్కించు_Image(
			ఖాళీ,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_వెడల్పు,
			TOOLBAR_BUTTON_ఎత్తు,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	ఒకవేళ (hbitmap == ఖాళీ
		&& (gui_కనుగొను_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ఖాళీ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = ఎక్కించు_Image(
		    ఖాళీ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_వెడల్పు,
		    TOOLBAR_BUTTON_ఎత్తు,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	ఒకవేళ (hbitmap != ఖాళీ)
	{
	    TBADDBITMAP tb_కూడు_Bitmap;

	    tb_కూడు_Bitmap.hInst = ఖాళీ;
	    tb_కూడు_Bitmap.nID = (long_u)hbitmap;

	    i = (పూర్ణాంకం)Send_సందేశం(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tb_కూడు_Bitmap);
	    // i will be set to -1 if it fails
	}
    }
    ఒకవేళ (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_లెక్క)
	i = menu->iconidx;

    తిరిగి i;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE)
    స్థిర శూన్య
initialise_tabline(శూన్య)
{
    InitCommonControls();

    s_tabhwnd = Create_కిటికీ(WC_TABCONTROL, "Vim tabline",
	    WS_బిడ్డ|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ఖాళీ, g_hinst, ఖాళీ);
    s_tabline_wndproc = Subclass_కిటికీ(s_tabhwnd, tabline_wndproc);

    gui.tabline_ఎత్తు = TABLINE_ఎత్తు;

    సమితి_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    స్థిర tabpage_T *
Get_Tab_From_బిందువు(
    HWND    hWnd,
    బిందువు_2   pt)
{
    tabpage_T	*ptp = ఖాళీ;

    ఒకవేళ (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	ఒకవేళ (s_tabhwnd == hWnd)
	{
	    పూర్ణాంకం idx = Tab_Ctrl_తగిలింది_Test(s_tabhwnd, &htinfo);
	    ఒకవేళ (idx != -1)
		ptp = కనుగొను_tabpage(idx + 1);
	}
    }
    తిరిగి ptp;
}

స్థిర బిందువు_2	    s_pt = {0, 0};
స్థిర HCURSOR      s_hCursor = ఖాళీ;

    స్థిర LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_సందేశం,
    WPARAM wParam,
    LPARAM lParam)
{
    బిందువు_2	pt;
    tabpage_T	*tp;
    RECT	rect;
    పూర్ణాంకం		nCenter;
    పూర్ణాంకం		idx0;
    పూర్ణాంకం		idx1;

    HandleMouseHide(u_సందేశం, lParam);

    ఎంపిక (u_సందేశం)
    {
	స్థితి WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		సమితి_Capture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		ఆపు;
	    }
	స్థితి WM_MOUSEMOVE:
	    ఒకవేళ (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		ఒకవేళ (నిరపేక్ష(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    సమితి_Cursor(ఎక్కించు_Cursor(ఖాళీ, IDC_SIZEWE));

		    tp = Get_Tab_From_బిందువు(hwnd, pt);
		    ఒకవేళ (tp != ఖాళీ)
		    {
			idx0 = tabpage_సూచీ(curtab) - 1;
			idx1 = tabpage_సూచీ(tp) - 1;

			Tab_Ctrl_Get_అంశం_Rect(hwnd, idx1, &rect);
			nCenter = rect.ఎడమ_2 + (rect.కుడి_2 - rect.ఎడమ_2) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			ఒకవేళ ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			లేకపోతే ఒకవేళ ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    ఆపు;
	స్థితి WM_LBUTTONUP:
	    {
		ఒకవేళ (GetCapture() == hwnd)
		{
		    సమితి_Cursor(s_hCursor);
		    ReleaseCapture();
		}
		ఆపు;
	    }
	స్థితి WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = Tab_Ctrl_తగిలింది_Test(hwnd, &htinfo);
		ఒకవేళ (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		ఆపు;
	    }
	అప్రమేయ:
	    ఆపు;
    }

    తిరిగి Call_కిటికీ_Proc(s_tabline_wndproc, hwnd, u_సందేశం, wParam, lParam);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    ఒకవేళ (IsIconic(s_hwnd))
	 Send_సందేశం(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    సమితి_Foreground_కిటికీ(s_hwnd);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    స్థిర శూన్య
dyn_imm_ఎక్కించు(శూన్య)
{
    hLibImm = vim_ఎక్కించు_Lib("imm32.dll");
    ఒకవేళ (hLibImm == ఖాళీ)
	తిరిగి;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))Get_Proc_చిరునామా(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))Get_Proc_చిరునామా(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))Get_Proc_చిరునామా(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))Get_Proc_చిరునామా(hLibImm, "ImmReleaseContext");
    p_Imm_Get_Open_స్థితి
	    = (BOOL (WINAPI *)(HIMC))Get_Proc_చిరునామా(hLibImm, "ImmGetOpenStatus");
    p_Imm_సమితి_Open_స్థితి
	    = (BOOL (WINAPI *)(HIMC, BOOL))Get_Proc_చిరునామా(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_చిరునామా(hLibImm, "ImmGetCompositionFontW");
    p_Imm_సమితి_Composition_Font_W
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_చిరునామా(hLibImm, "ImmSetCompositionFontW");
    p_Imm_సమితి_Composition_కిటికీ
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))Get_Proc_చిరునామా(hLibImm, "ImmSetCompositionWindow");
    p_Imm_Get_Conversion_స్థితి
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))Get_Proc_చిరునామా(hLibImm, "ImmGetConversionStatus");
    p_Imm_సమితి_Conversion_స్థితి
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))Get_Proc_చిరునామా(hLibImm, "ImmSetConversionStatus");

    ఒకవేళ (       pImmGetCompositionStringW == ఖాళీ
	    || pImmGetContext == ఖాళీ
	    || pImmAssociateContext == ఖాళీ
	    || pImmReleaseContext == ఖాళీ
	    || p_Imm_Get_Open_స్థితి == ఖాళీ
	    || p_Imm_సమితి_Open_స్థితి == ఖాళీ
	    || pImmGetCompositionFontW == ఖాళీ
	    || p_Imm_సమితి_Composition_Font_W == ఖాళీ
	    || p_Imm_సమితి_Composition_కిటికీ == ఖాళీ
	    || p_Imm_Get_Conversion_స్థితి == ఖాళీ
	    || p_Imm_సమితి_Conversion_స్థితి == ఖాళీ)
    {
	FreeLibrary(hLibImm);
	hLibImm = ఖాళీ;
	pImmGetContext = ఖాళీ;
	తిరిగి;
    }

    తిరిగి;
}

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

రకనిర్వచనం నిర్మాణం _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    శూన్య
gui_mch_drawsign(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం typenr)
{
    signicon_t *sign;
    పూర్ణాంకం x, y, w, h;

    ఒకవేళ (!gui.లోపల_use || (sign = (signicon_t *)sign_get_image(typenr)) == ఖాళీ)
	తిరిగి;

# ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = పాఠ్యం_X(నిలువు);
    y = పాఠ్యం_Y(అడ్డువరుస);
    w = gui.char_వెడల్పు * 2;
    h = gui.char_ఎత్తు;
    ఎంపిక (sign->uType)
    {
	స్థితి IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		బిట్టు_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ఆపు;
	స్థితి IMAGE_ICON:
	స్థితి IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ఖాళీ, DI_సాధారణ);
	    ఆపు;
# ifdef FEAT_XPM_W32
	స్థితి IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		బిట్టు_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		బిట్టు_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ఆపు;
# endif
    }
}

    స్థిర శూన్య
close_signicon_image(signicon_t *sign)
{
    ఒకవేళ (sign == ఖాళీ)
	తిరిగి;

    ఎంపిక (sign->uType)
    {
	స్థితి IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    ఆపు;
	స్థితి IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    ఆపు;
	స్థితి IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    ఆపు;
# ifdef FEAT_XPM_W32
	స్థితి IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    ఆపు;
# endif
    }
}

    శూన్య *
gui_mch_రిజిస్టరు_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ఖాళీ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    ఒకవేళ (ext > signfile)
    {
	పూర్ణాంకం do_ఎక్కించు = 1;

	ఒకవేళ (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	లేకపోతే ఒకవేళ (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	లేకపోతే ఒకవేళ (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	లేకపోతే
	    do_ఎక్కించు = 0;

	ఒకవేళ (do_ఎక్కించు)
	    sign.hImage = (HANDLE)ఎక్కించు_Image(ఖాళీ, (LPCSTR)signfile, sign.uType,
		    gui.char_వెడల్పు * 2, gui.char_ఎత్తు,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	ఒకవేళ (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    ఎక్కించు_Xpm_Image((అక్షరం *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ఖాళీ;
    ఒకవేళ (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ఖాళీ)
	*psign = sign;

    ఒకవేళ (!psign)
    {
	ఒకవేళ (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_లోపల_sign_దత్తాంశం));
    }
    తిరిగి (శూన్య *)psign;

}

    శూన్య
gui_mch_destroy_sign(శూన్య *sign)
{
    ఒకవేళ (sign == ఖాళీ)
	తిరిగి;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BEVAL_GUI)

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

    స్థిర శూన్య
make_tooltip(BalloonEval *beval, అక్షరం *పాఠ్యం, బిందువు_2 pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    ఒకవేళ (pti == ఖాళీ)
	తిరిగి;

    beval->balloon = Create_కిటికీ_Ex_W(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ఖాళీ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->లక్ష్యం, ఖాళీ, g_hinst, ఖాళీ);

    సమితి_కిటికీ_Pos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_కొలత = పరిమాణం(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->లక్ష్యం;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpsz_పాఠ్యం = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_గమ్యం_utf16((char_u*)పాఠ్యం, ఖాళీ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    ఒకవేళ (GetClientRect(s_పాఠ్యం_Area, &rect))
	Send_సందేశం_W(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.కుడి_2);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ఎడమ_2 = pt.x - 3;
    pti->rect.శిఖరం = pt.y - 3;
    pti->rect.కుడి_2 = pt.x + 3;
    pti->rect.bottom = pt.y + 3;

    Send_సందేశం_W(beval->balloon, TTM_ADDTOOLW, 0, (LPARAM)pti);
    // Make tooltip appear sooner.
    Send_సందేశం_W(beval->balloon, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
    // I've performed some tests and it seems the longest possible life time
    // of tooltip is 30 seconds.
    Send_సందేశం_W(beval->balloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
    /*
     * HACK: force tooltip to appear, because it'll not appear until
     * first mouse move. D*mn M$
     * Amazingly moving (2, 2) and then (-1, -1) the mouse doesn't move.
     */
    mouse_event(MOUSEEVENTF_MOVE, 2, 2, 0, 0);
    mouse_event(MOUSEEVENTF_MOVE, (DWORD)-1, (DWORD)-1, 0, 0);
    vim_free(pti);
}

    స్థిర శూన్య
delete_tooltip(BalloonEval *beval)
{
    Post_సందేశం(beval->balloon, WM_CLOSE, 0, 0);
}

    స్థిర VOID CALLBACK
beval_timer_proc(
    HWND	hwnd వాడని,
    UINT	u_సందేశం వాడని,
    UINT_PTR	idEvent వాడని,
    DWORD	dw_సమయం)
{
    బిందువు_2	pt;
    RECT	rect;

    ఒకవేళ (cur_beval == ఖాళీ || cur_beval->showState == ShS_SHOWING || !p_beval)
	తిరిగి;

    GetCursorPos(&pt);
    ఒకవేళ (కిటికీ_From_బిందువు(pt) != s_పాఠ్యం_Area)
	తిరిగి;

    Screen_గమ్యం_Client(s_పాఠ్యం_Area, &pt);
    GetClientRect(s_పాఠ్యం_Area, &rect);
    ఒకవేళ (!Pt_లోపల_Rect(&rect, pt))
	తిరిగి;

    ఒకవేళ (చివరి_user_activity > 0
	    && (dw_సమయం - చివరి_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| నిరపేక్ష(cur_beval->x - pt.x) > 3
		|| నిరపేక్ష(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	ఒకవేళ (cur_beval->సందేశం_CB != ఖాళీ)
	    (*cur_beval->సందేశం_CB)(cur_beval, 0);
    }
}

    శూన్య
gui_mch_disable_beval_area(BalloonEval *beval వాడని)
{
    KillTimer(s_పాఠ్యం_Area, beval_timer_id);
}

    శూన్య
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ఒకవేళ (beval == ఖాళీ)
	తిరిగి;
    beval_timer_id = సమితి_Timer(s_పాఠ్యం_Area, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    శూన్య
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    బిందువు_2   pt;

    vim_free(beval->సందేశం);
    beval->సందేశం = mesg == ఖాళీ ? ఖాళీ : vim_strsave(mesg);
    ఒకవేళ (beval->సందేశం == ఖాళీ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	తిరిగి;
    }

    ఒకవేళ (beval->showState == ShS_SHOWING)
	తిరిగి;
    GetCursorPos(&pt);
    Screen_గమ్యం_Client(s_పాఠ్యం_Area, &pt);

    ఒకవేళ (నిరపేక్ష(beval->x - pt.x) < 3 && నిరపేక్ష(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (అక్షరం *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    శూన్య	*లక్ష్యం వాడని,	// ignored, always use s_textArea
    char_u	*mesg,
    శూన్య	(*mesgCB)(BalloonEval *, పూర్ణాంకం),
    శూన్య	*client_దత్తాంశం)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    ఒకవేళ (mesg != ఖాళీ && mesgCB != ఖాళీ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_సందేశం_and_callback);
	తిరిగి ఖాళీ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ఒకవేళ (beval != ఖాళీ)
    {
	beval->లక్ష్యం = s_పాఠ్యం_Area;

	beval->showState = ShS_NEUTRAL;
	beval->సందేశం = mesg;
	beval->సందేశం_CB = mesgCB;
	beval->client_దత్తాంశం = client_దత్తాంశం;

	InitCommonControls();
	cur_beval = beval;

	ఒకవేళ (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    తిరిగి beval;
}

    స్థిర శూన్య
Handle_WM_Notify(HWND hwnd వాడని, LPNMHDR pnmh)
{
    ఒకవేళ (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	తిరిగి;

    ఒకవేళ (cur_beval == ఖాళీ)
	తిరిగి;

    ఎంపిక (pnmh->సంకేతం_2)
    {
	స్థితి TTN_SHOW:
	    ఆపు;
	స్థితి TTN_తీయి: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    ఆపు;
	స్థితి TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpsz_పాఠ్యం = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ఆపు;
	స్థితి TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpsz_పాఠ్యం = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ఆపు;
    }
}

    స్థిర శూన్య
track_user_activity(UINT u_సందేశం)
{
    ఒకవేళ ((u_సందేశం >= WM_MOUSEFIRST && u_సందేశం <= WM_MOUSELAST)
	    || (u_సందేశం >= WM_KEYFIRST && u_సందేశం <= WM_KEYLAST))
	చివరి_user_activity = Get_Tick_లెక్క();
}

    శూన్య
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#అయితే_ముగింపు // FEAT_BEVAL_GUI

#ఒకవేళ defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    శూన్య
netbeans_draw_multisign_indicator(పూర్ణాంకం అడ్డువరుస)
{
    పూర్ణాంకం i;
    పూర్ణాంకం y;
    పూర్ణాంకం x;

    ఒకవేళ (!netbeans_చురుకైన())
	తిరిగి;

    x = 0;
    y = పాఠ్యం_Y(అడ్డువరుస);

# ఒకవేళ defined(FEAT_DIRECTX)
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    కొరకు (i = 0; i < gui.char_ఎత్తు - 3; i++)
	సమితి_Pixel(s_hdc, x+2, y++, gui.currFgColor);

    సమితి_Pixel(s_hdc, x+0, y, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+2, y, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+4, y++, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+1, y, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+2, y, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+3, y++, gui.currFgColor);
    సమితి_Pixel(s_hdc, x+2, y, gui.currFgColor);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    స్థిర పూర్ణాంకం
test_gui_w32_sendevent_mouse(dict_T *వాదనలు)
{
    ఒకవేళ (!dict_has_కీలకం(వాదనలు, "row") || !dict_has_కీలకం(వాదనలు, "col"))
	తిరిగి FALSE;

    // Note: "move" is optional, requires fewer arguments
    పూర్ణాంకం move = (పూర్ణాంకం)dict_get_bool(వాదనలు, "move", FALSE);

    ఒకవేళ (!move && (!dict_has_కీలకం(వాదనలు, "button")
	    || !dict_has_కీలకం(వాదనలు, "multiclick")
	    || !dict_has_కీలకం(వాదనలు, "modifiers")))
	తిరిగి FALSE;

    పూర్ణాంకం అడ్డువరుస = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "row");
    పూర్ణాంకం నిలువు = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "col");

    ఒకవేళ (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ఒకవేళ (dict_get_bool(వాదనలు, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    పూర్ణాంకం pY = (అడ్డువరుస - 1) * gui.char_ఎత్తు + gui.char_ఎత్తు / 2;
	    పూర్ణాంకం pX = (నిలువు - 1) * gui.char_వెడల్పు + gui.char_వెడల్పు / 2;
	    gui_mouse_moved(pX, pY);
	}
	లేకపోతే
	    gui_mouse_moved(నిలువు, అడ్డువరుస);
    }
    లేకపోతే
    {
	పూర్ణాంకం button = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "button");
	పూర్ణాంకం repeated_click = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "multiclick");
	int_u mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_సమితి_hor_scroll_అడుగు(6);
	mouse_సమితి_vert_scroll_అడుగు(3);

	gui_send_mouse_event(button, పాఠ్యం_X(నిలువు - 1), పాఠ్యం_Y(అడ్డువరుస - 1),
							repeated_click, mods);
    }
    తిరిగి TRUE;
}

    స్థిర పూర్ణాంకం
test_gui_w32_sendevent_keyboard(dict_T *వాదనలు)
{
    ఆదానం_2 inputs[1];
    ఆదానం_2 modkeys[3];
    Secure_Zero_జ్ఞాపకం(inputs, పరిమాణం(ఆదానం_2));
    Secure_Zero_జ్ఞాపకం(modkeys, 3 * పరిమాణం(ఆదానం_2));

    char_u *event = dict_get_string(వాదనలు, "event", TRUE);

    ఒకవేళ (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	మాట_2 vk_సంకేతం = dict_get_సంఖ్య_def(వాదనలు, "keycode", 0);
	ఒకవేళ (vk_సంకేతం <= 0 || vk_సంకేతం >= 0xFF)
	{
	    semsg(_(e_invalid_వాదం_nr), (దీర్ఘ)vk_సంకేతం);
	    తిరిగి FALSE;
	}

	BOOL is_Mod_కీలకం = (vk_సంకేతం == VK_జరుపు || vk_సంకేతం == VK_CONTROL
	    || vk_సంకేతం == VK_MENU || vk_సంకేతం == VK_LSHIFT || vk_సంకేతం == VK_RSHIFT
	    || vk_సంకేతం == VK_LCONTROL || vk_సంకేతం == VK_RCONTROL
	    || vk_సంకేతం == VK_LMENU || vk_సంకేతం == VK_RMENU );

	BOOL unwrapMods = FALSE;
	పూర్ణాంకం mods = (పూర్ణాంకం)dict_get_సంఖ్య(వాదనలు, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	ఒకవేళ (mods && STRICMP(event, "keydown") == 0 && !is_Mod_కీలకం)
	{
	    పూర్ణాంకం n = 0;
	    ఒకవేళ (mods & MOD_ముసుగు_జరుపు)
	    {
		modkeys[n].type = ఆదానం_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    ఒకవేళ (mods & MOD_ముసుగు_CTRL)
	    {
		modkeys[n].type = ఆదానం_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    ఒకవేళ (mods & MOD_ముసుగు_ALT)
	    {
		modkeys[n].type = ఆదానం_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    ఒకవేళ (n)
	    {
		(శూన్య)సమితి_Foreground_కిటికీ(s_hwnd);
		Send_ఆదానం(n, modkeys, పరిమాణం(ఆదానం_2));
	    }
	}

	inputs[0].type = ఆదానం_KEYBOARD;
	inputs[0].ki.wVk = vk_సంకేతం;
	ఒకవేళ (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    ఒకవేళ (!is_Mod_కీలకం)
		unwrapMods = TRUE;
	}

	(శూన్య)సమితి_Foreground_కిటికీ(s_hwnd);
	Send_ఆదానం(ARRAYSIZE(inputs), inputs, పరిమాణం(ఆదానం_2));
	vim_free(event);

	ఒకవేళ (unwrapMods)
	{
	    modkeys[0].type = ఆదానం_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = ఆదానం_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = ఆదానం_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (శూన్య)సమితి_Foreground_కిటికీ(s_hwnd);
	    Send_ఆదానం(3, modkeys, పరిమాణం(ఆదానం_2));
	}
    }
    లేకపోతే
    {
	ఒకవేళ (event == ఖాళీ)
	{
	    semsg(_(e_missing_వాదం_str), "event");
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "event", event);
	    vim_free(event);
	}
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

    స్థిర పూర్ణాంకం
test_gui_w32_sendevent_సమితి_keycode_trans_strategy(dict_T *వాదనలు)
{
    పూర్ణాంకం handled = 0;
    char_u *strategy = dict_get_string(వాదనలు, "strategy", TRUE);

    ఒకవేళ (strategy)
    {
	ఒకవేళ (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_వాడిన = &keycode_trans_strategy_classic;
	}
	లేకపోతే ఒకవేళ (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_వాడిన = &keycode_trans_strategy_experimental;
	}
    }

    ఒకవేళ (!handled)
    {
	ఒకవేళ (strategy == ఖాళీ)
	{
	    semsg(_(e_missing_వాదం_str), "strategy");
	}
	లేకపోతే
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	తిరిగి FALSE;
    }
    తిరిగి TRUE;
}


    పూర్ణాంకం
test_gui_w32_sendevent(char_u *event, dict_T *వాదనలు)
{
    ఒకవేళ (STRICMP(event, "key") == 0)
	తిరిగి test_gui_w32_sendevent_keyboard(వాదనలు);
    లేకపోతే ఒకవేళ (STRICMP(event, "mouse") == 0)
	తిరిగి test_gui_w32_sendevent_mouse(వాదనలు);
    లేకపోతే ఒకవేళ (STRICMP(event, "set_keycode_trans_strategy") == 0)
	తిరిగి test_gui_w32_sendevent_సమితి_keycode_trans_strategy(వాదనలు);
    లేకపోతే
    {
	semsg(_(e_invalid_విలువ_for_వాదం_str_str), "event", event);
	తిరిగి FALSE;
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_IMAGE_GUI)

రకనిర్వచనం నిర్మాణం
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
    స్థిర శూన్య
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, పూర్ణాంకం iw, పూర్ణాంకం ih, image_format_T fmt)
{
    పూర్ణాంకం		y, x;
    పూర్ణాంకం		stride = iw * 4;
    పూర్ణాంకం		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.వెనుక_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    కొరకు (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	కొరకు (x = 0; x < iw; x++)
	{
	    ఒకవేళ (fmt == IMAGE_FORMAT_RGBA)
	    {
		పూర్ణాంకం a = s[3];

		ఒకవేళ (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		లేకపోతే ఒకవేళ (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		లేకపోతే
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    లేకపోతే
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

    పూర్ణాంకం
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    పూర్ణాంకం		iw, ih;
    శూన్య	*బిట్లు = ఖాళీ;

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_క్షేత్రం(bmi);
    bmi.bmi_శీర్షిక.bi_కొలత = పరిమాణం(BITMAPINFOHEADER);
    bmi.bmi_శీర్షిక.bi_వెడల్పు = iw;
    bmi.bmi_శీర్షిక.bi_ఎత్తు = -ih; // Top-down
    bmi.bmi_శీర్షిక.biPlanes = 1;
    bmi.bmi_శీర్షిక.bi_బిట్టు_లెక్క = 32;
    bmi.bmi_శీర్షిక.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &బిట్లు, ఖాళీ, 0);
    ఒకవేళ (hbm == ఖాళీ || బిట్లు == ఖాళీ)
    {
	ఒకవేళ (hbm != ఖాళీ)
	    DeleteObject(hbm);
	తిరిగి false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    ఒకవేళ (mem_dc == ఖాళీ)
    {
	DeleteObject(hbm);
	తిరిగి false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)బిట్లు, img->దత్తాంశం, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_దత్తాంశం;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    పూర్ణాంకం
image_placement_gui_init(image_placement_T *ఉంచు వాడని)
{
    తిరిగి OK;
}

    శూన్య
image_placement_gui_uninit(image_placement_T *ఉంచు వాడని)
{
}

    శూన్య
image_placement_gui_draw(image_placement_T *ఉంచు)
{
    image_T	    *img = ఉంచు->img;
    image_gdi_T	    *ictx = img->backend_దత్తాంశం;
    pixman_box32_t  *rects;
    పూర్ణాంకం		    n_rects;

    rects = pixman_region32_rectangles(&ఉంచు->visible, &n_rects);
    ఒకవేళ (rects == ఖాళీ || n_rects == 0)
	తిరిగి;

# ఒకవేళ defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    ఒకవేళ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	పూర్ణాంకం		అడ్డువరుస, నిలువు;
	పూర్ణాంకం		posx, posy;
	పూర్ణాంకం		x, y, w, h;

	image_placement_subrect(ఉంచు, rect, &అడ్డువరుస, &నిలువు, &x, &y, &w, &h, true);
	posx = FILL_X(నిలువు);
	posy = FILL_Y(అడ్డువరుస);

	బిట్టు_Blt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    శూన్య
image_placement_gui_clear(image_placement_T *ఉంచు వాడని)
{
}

#అయితే_ముగింపు // FEAT_IMAGE_GUI
