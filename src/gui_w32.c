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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#ਜੇ_ਅੰਤ

// values for "dead_key"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEAD_ਕੁੰਜੀ_OFF			0	// no dead key
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT		1	// dead key pressed
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEAD_ਕੁੰਜੀ_TRANSIENT_ਵਿੱਚ_ON_CHAR	2	// wait for next key press
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEAD_ਕੁੰਜੀ_SKIP_ON_CHAR		3	// skip next _OnChar()

#ਜੇ defined(FEAT_DIRECTX)
ਸਥਿਰ DWriteContext *s_dwc = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ s_directx_enabled = 0;
ਸਥਿਰ ਪੂਰਨ s_directx_ਲੱਦੋ_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ਨਿਰਾ && enc_utf8)
ਸਥਿਰ ਪੂਰਨ directx_enabled(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ directx_binddc(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਸਥਿਰ ਪੂਰਨ gui_mswin_get_menu_ਉਚਾਈ(ਪੂਰਨ fix_ਖਿੜਕੀ);
#ਨਹੀਂਤਾਂ
# define gui_mswin_get_menu_ਉਚਾਈ(fix_ਖਿੜਕੀ)	0
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ keycode_trans_strategy {
    ਖਾਲੀ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
    ਖਾਲੀ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
    ਖਾਲੀ (*ptr_process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ) (UINT /*vk*/, ਅਚਲ ਸੁਨੇਹਾ_3* /*pmsg*/);
    ਪੂਰਨ  (*ptr_get_ਸਰਗਰਮ_modifiers)(ਖਾਲੀ);
    ਪੂਰਨ  (*is_experimental)(ਖਾਲੀ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ਸਥਿਰ ਖਾਲੀ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
ਸਥਿਰ ਖਾਲੀ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
ਸਥਿਰ ਖਾਲੀ process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_experimental(UINT /*vk*/, ਅਚਲ ਸੁਨੇਹਾ_3* /*pmsg*/);
ਸਥਿਰ ਪੂਰਨ  get_ਸਰਗਰਮ_modifiers_experimental(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  is_experimental_true(ਖਾਲੀ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_experimental // ptr_process_message_usual_key
    , get_ਸਰਗਰਮ_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ਸਥਿਰ ਖਾਲੀ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
ਸਥਿਰ ਖਾਲੀ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ਪੂਰਨ /*cRepeat UNUSED*/);
ਸਥਿਰ ਖਾਲੀ process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_classic(UINT /*vk*/, ਅਚਲ ਸੁਨੇਹਾ_3* /*pmsg*/);
ਸਥਿਰ ਪੂਰਨ  get_ਸਰਗਰਮ_modifiers_classic(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ  is_experimental_false(ਖਾਲੀ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_classic // ptr_process_message_usual_key
    , get_ਸਰਗਰਮ_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_ਵਰਤਿਆ = ਨਿਰਾ;

ਸਥਿਰ ਪੂਰਨ is_experimental_true(ਖਾਲੀ)
{
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ is_experimental_false(ਖਾਲੀ)
{
    ਵਾਪਸ 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ਸਥਿਰ ਖਾਲੀ keycode_trans_strategy_init(ਖਾਲੀ)
{
    ਅਚਲ ਅੱਖਰ *strategy = ਨਿਰਾ;

    // set default value as fallback
    keycode_trans_strategy_ਵਰਤਿਆ = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    ਜੇ (strategy == ਨਿਰਾ)
    {
	ਵਾਪਸ;
    }

    ਜੇ (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_ਵਰਤਿਆ = &keycode_trans_strategy_classic;
	ਵਾਪਸ;
    }

    ਜੇ (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_ਵਰਤਿਆ = &keycode_trans_strategy_experimental;
	ਵਾਪਸ;
    }

}

#ਜੇ defined(FEAT_RENDER_OPTIONS)
    ਪੂਰਨ
gui_mch_ਸੈੱਟ_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    ਪੂਰਨ	    dx_enable = 0;
    ਪੂਰਨ	    dx_flags = 0;
    ਦਸ਼ਮਲਵ   dx_gamma = 0.0f;
    ਦਸ਼ਮਲਵ   dx_contrast = 0.0f;
    ਦਸ਼ਮਲਵ   dx_level = 0.0f;
    ਪੂਰਨ	    dx_geom = 0;
    ਪੂਰਨ	    dx_renmode = 0;
    ਪੂਰਨ	    dx_taamode = 0;
    // parse string as rendering options.
    ਲਈ (p = s; p != ਨਿਰਾ && *p != NUL; )
    {
	char_u  ਵਸਤੂ[256];
	char_u  name[128];
	char_u  ਮੁੱਲ[128];

	copy_option_part(&p, ਵਸਤੂ, ਆਕਾਰ(ਵਸਤੂ), ",");
	ਜੇ (p == ਨਿਰਾ)
	    ਰੋਕੋ;
	q = &ਵਸਤੂ[0];
	copy_option_part(&q, name, ਆਕਾਰ(name), ":");
	ਜੇ (q == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	copy_option_part(&q, ਮੁੱਲ, ਆਕਾਰ(ਮੁੱਲ), ":");

	ਜੇ (STRCMP(name, "type") == 0)
	{
	    ਜੇ (STRCMP(ਮੁੱਲ, "directx") == 0)
		dx_enable = 1;
	    ਨਹੀਂਤਾਂ
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (ਦਸ਼ਮਲਵ)atof((ਅੱਖਰ *)ਮੁੱਲ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (ਦਸ਼ਮਲਵ)atof((ਅੱਖਰ *)ਮੁੱਲ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (ਦਸ਼ਮਲਵ)atof((ਅੱਖਰ *)ਮੁੱਲ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((ਅੱਖਰ *)ਮੁੱਲ);
	    ਜੇ (dx_geom < 0 || dx_geom > 2)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((ਅੱਖਰ *)ਮੁੱਲ);
	    ਜੇ (dx_renmode < 0 || dx_renmode > 6)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((ਅੱਖਰ *)ਮੁੱਲ);
	    ਜੇ (dx_taamode < 0 || dx_taamode > 3)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    ਜੇ (dx_enable)
    {
	ਜੇ (!directx_enabled())
	    ਵਾਪਸ FAIL;
	DWrite_Context_ਸੈੱਟ_Rendering_Params(s_dwc, ਨਿਰਾ);
	ਜੇ (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    ਜੇ (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    ਜੇ (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    ਜੇ (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    ਜੇ (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    ਜੇ (dx_flags & (1 << 4))
		param.rendering_ਬਹੁਲਕ = dx_renmode;
	    ਜੇ (dx_flags & (1 << 5))
		param.ਪਾਠ_Antialias_ਬਹੁਲਕ = dx_taamode;
	    DWrite_Context_ਸੈੱਟ_Rendering_Params(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ਵਾਪਸ OK;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
# endif
}
#ਜੇ_ਅੰਤ

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (ਪੂਰਨ)(ਛੋਟਾ)LOWORD(lParam), (ਪੂਰਨ)(ਛੋਟਾ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (ਪੂਰਨ)(ਛੋਟਾ)LOWORD(lParam), (ਪੂਰਨ)(ਛੋਟਾ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (ਪੂਰਨ)(ਛੋਟਾ)LOWORD(lParam), (ਪੂਰਨ)(ਛੋਟਾ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ਜੇ_ਅੰਤ


#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"	// used by dialog box routine for default title
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
# include <tchar.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <shellapi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <commctrl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <windowsx.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#ਜੇ_ਅੰਤ

// Some parameters for dialog boxes.  All in pixels.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_PADDING_X		10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_PADDING_Y		10
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_VERT_PADDING_X	4	// For vertical buttons
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_VERT_PADDING_Y	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_ICON_ਚੌੜਾਈ		34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_ICON_ਉਚਾਈ		34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_MIN_ਚੌੜਾਈ		150
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_FONT_NAME		"MS Shell Dlg"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_FONT_ਬਿੰਦੂ_ਮਾਪ	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_MIN_MAX_ਚੌੜਾਈ	400
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_MIN_MAX_ਉਚਾਈ	400

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DWMWA_USE_IMMERSIVE_DARK_ਬਹੁਲਕ
# define DWMWA_USE_IMMERSIVE_DARK_ਬਹੁਲਕ	20
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DWMWA_ਪਾਠ_COLOR
# define DWMWA_ਪਾਠ_COLOR		36
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ _OnPaint(HWND hwnd);
ਸਥਿਰ ਖਾਲੀ fill_rect(ਅਚਲ RECT *rcp, HBRUSH hbr, COLORREF color);
ਸਥਿਰ ਖਾਲੀ clear_rect(RECT *rcp);

ਸਥਿਰ ਸ਼ਬਦ_2		s_dlgfntheight;		// height of the dialog font
ਸਥਿਰ ਸ਼ਬਦ_2		s_dlgfntwidth;		// width of the dialog font

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਸਥਿਰ HMENU		s_menuBar = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TEAROFF
ਸਥਿਰ ਖਾਲੀ rebuild_tearoff(vimmenu_T *menu);
ਸਥਿਰ HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#ਜੇ_ਅੰਤ

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ਸਥਿਰ ਪੂਰਨ		s_busy_processing = FALSE;

ਸਥਿਰ ਪੂਰਨ		destroying = FALSE;	// call DestroyWindow() ourselves

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
ਸਥਿਰ UINT		s_findrep_ਸੁਨੇਹਾ = 0;
ਸਥਿਰ FINDREPLACEW	s_findrep_struct;
ਸਥਿਰ HWND		s_findrep_hwnd = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		s_findrep_is_ਲੱਭੋ;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#ਜੇ_ਅੰਤ

HWND			s_hwnd = ਨਿਰਾ;
ਸਥਿਰ HDC		s_hdc = ਨਿਰਾ;
ਸਥਿਰ HBRUSH		s_brush = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
ਸਥਿਰ HWND		s_toolbarhwnd = ਨਿਰਾ;
ਸਥਿਰ WNDPROC		s_toolbar_wndproc = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
ਸਥਿਰ HWND		s_tabhwnd = ਨਿਰਾ;
ਸਥਿਰ WNDPROC		s_tabline_wndproc = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		showing_tabline = 0;
#ਜੇ_ਅੰਤ

ਸਥਿਰ WPARAM		s_wParam = 0;

ਸਥਿਰ HWND		s_ਪਾਠ_Area = ਨਿਰਾ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ਸਥਿਰ bool		s_textarea_bg_only = false;

ਸਥਿਰ UINT		s_u_ਸੁਨੇਹਾ = 0;

ਸਥਿਰ char_u		*s_textfield; // Used by dialogs to pass back strings

ਸਥਿਰ ਪੂਰਨ		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ਸਥਿਰ ਪੂਰਨ allow_scrollbar = FALSE;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ _DPI_AWARENESS_CONTEXTS_
ਕਿਸਮਪਰਿਭਾਸ਼ਾ HANDLE DPI_AWARENESS_CONTEXT;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ DPI_AWARENESS {
    DPI_AWARENESS_INVALID	    = -1,
    DPI_AWARENESS_UNAWARE	    = 0,
    DPI_AWARENESS_SYSTEM_AWARE	    = 1,
    DPI_AWARENESS_ਪ੍ਰਤੀ_MONITOR_AWARE = 2
} DPI_AWARENESS;

# define DPI_AWARENESS_CONTEXT_UNAWARE		    ((DPI_AWARENESS_CONTEXT)-1)
# define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE	    ((DPI_AWARENESS_CONTEXT)-2)
# define DPI_AWARENESS_CONTEXT_ਪ੍ਰਤੀ_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
# define DPI_AWARENESS_CONTEXT_ਪ੍ਰਤੀ_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
# define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_DPI	96
ਸਥਿਰ ਪੂਰਨ		s_dpi = DEFAULT_DPI;
ਸਥਿਰ BOOL		s_ਵਿੱਚ_dpichanged = FALSE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
ਸਥਿਰ DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#ਜੇ_ਅੰਤ
ਸਥਿਰ RECT		s_suggested_rect;

ਸਥਿਰ UINT (WINAPI *pGetDpiForSystem)(ਖਾਲੀ) = ਨਿਰਾ;
ਸਥਿਰ UINT (WINAPI *p_Get_Dpi_For_ਖਿੜਕੀ)(HWND hwnd) = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ (WINAPI *pGetSystemMetricsForDpi)(ਪੂਰਨ, UINT) = ਨਿਰਾ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ਸਥਿਰ DPI_AWARENESS_CONTEXT (WINAPI *p_ਸੈੱਟ_Thread_Dpi_Awareness_Context)(DPI_AWARENESS_CONTEXT dpiContext) = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
ਸਥਿਰ DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ HINSTANCE hLibDwm = ਨਿਰਾ;
ਸਥਿਰ HRESULT (WINAPI *p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute)(HWND, DWORD, LPCVOID, DWORD);
ਸਥਿਰ ਖਾਲੀ dyn_dwm_ਲੱਦੋ(ਖਾਲੀ);

ਸਥਿਰ ਪੂਰਨ fullscreen_on = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME

ਸਥਿਰ HINSTANCE hUxThemeLib = ਨਿਰਾ;
ਸਥਿਰ DWORD (WINAPI *p_ਸੈੱਟ_Preferred_App_ਬਹੁਲਕ)(DWORD) = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ (WINAPI *pFlushMenuThemes)(ਖਾਲੀ) = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ dyn_uxtheme_ਲੱਦੋ(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ WINAPI
stubGetSystemMetricsForDpi(ਪੂਰਨ n_ਸੂਚਕਾਂਕ, UINT dpi ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ GetSystemMetrics(n_ਸੂਚਕਾਂਕ);
}

    ਸਥਿਰ ਪੂਰਨ
adjust_fontsize_by_dpi(ਪੂਰਨ ਮਾਪ)
{
    ਵਾਪਸ ਮਾਪ * s_dpi / (ਪੂਰਨ)pGetDpiForSystem();
}

    ਸਥਿਰ ਪੂਰਨ
adjust_by_system_dpi(ਪੂਰਨ ਮਾਪ)
{
    ਵਾਪਸ ਮਾਪ * (ਪੂਰਨ)pGetDpiForSystem() / DEFAULT_DPI;
}

#ਜੇ defined(FEAT_DIRECTX)
    ਸਥਿਰ ਪੂਰਨ
directx_enabled(ਖਾਲੀ)
{
    ਜੇ (s_dwc != ਨਿਰਾ)
	ਵਾਪਸ 1;
    ਨਹੀਂਤਾਂ ਜੇ (s_directx_ਲੱਦੋ_attempted)
	ਵਾਪਸ 0;
    // load DirectX
    DWrite_Init();
    s_directx_ਲੱਦੋ_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ਵਾਪਸ s_dwc != ਨਿਰਾ ? 1 : 0;
}

    ਸਥਿਰ ਖਾਲੀ
directx_binddc(ਖਾਲੀ)
{
    ਜੇ (s_ਪਾਠ_Area == ਨਿਰਾ)
	ਵਾਪਸ;

    RECT	rect;
    GetClientRect(s_ਪਾਠ_Area, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ ਪੂਰਨ current_font_ਉਚਾਈ;	    // this is in os_mswin.c

ਸਥਿਰ ਬਣਤਰ
{
    UINT    ਕੁੰਜੀ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ਕੁੰਜੀਆਂ[] =
{
    {VK_ਉੱਪਰ,		'k', 'u'},
    {VK_ਥੱਲੇ,		'k', 'd'},
    {VK_ਖੱਬਾ,		'k', 'l'},
    {VK_ਸੱਜਾ,		'k', 'r'},

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#ਜੇ_ਅੰਤ
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_ਪਿੱਠ,		'k', 'b'},
    {VK_ਪਾਓ,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_ਸਿਰਾ,		'@', '7'},
    {VK_ਪੂਰਵ,		'k', 'P'},
    {VK_ਅਗਲਾ,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_ਜੋੜੋ,		'K', '6'},
    {VK_ਘਟਾਓ,	'K', '7'},
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
ਸਥਿਰ ਪੂਰਨ	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ਸਥਿਰ ਪੂਰਨ	s_getting_focus = FALSE;

ਸਥਿਰ ਪੂਰਨ	s_x_pending;
ਸਥਿਰ ਪੂਰਨ	s_y_pending;
ਸਥਿਰ UINT	s_kFlags_pending;
ਸਥਿਰ UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ਸਥਿਰ ਪੂਰਨ	s_timed_ਬਾਹਰਲਾ = FALSE;
ਸਥਿਰ ਪੂਰਨ	dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_OFF;
ਸਥਿਰ UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ਸਥਿਰ ਖਾਲੀ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ਸਥਿਰ ਖਾਲੀ track_user_activity(UINT u_ਸੁਨੇਹਾ);
#ਜੇ_ਅੰਤ

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ਸਥਿਰ LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ਸਥਿਰ LOGFONTW sub_logfont;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
ਸਥਿਰ LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BROWSE)
ਸਥਿਰ char_u *convert_filter(char_u *s);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ਸਥਿਰ ਖਾਲੀ
print_windows_error(ਖਾਲੀ)
{
    LPVOID  lp_ਸੁਨੇਹਾ_Buf;

    Format_ਸੁਨੇਹਾ(FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ | FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM,
		  ਨਿਰਾ, Get_ਆਖਰੀ_Error(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_ਸੁਨੇਹਾ_Buf, 0, ਨਿਰਾ);
    TRACE1("Error: %s\n", lp_ਸੁਨੇਹਾ_Buf);
    LocalFree(lp_ਸੁਨੇਹਾ_Buf);
}
#ਜੇ_ਅੰਤ

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_NONE  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_OFF   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_ON    2

ਸਥਿਰ ਪੂਰਨ		blink_state = BLINK_NONE;
ਸਥਿਰ long_u		blink_waittime = 700;
ਸਥਿਰ long_u		blink_ontime = 400;
ਸਥਿਰ long_u		blink_offtime = 250;
ਸਥਿਰ UINT_PTR		blink_timer = 0;

    ਪੂਰਨ
gui_mch_is_blinking(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state != BLINK_NONE;
}

    ਪੂਰਨ
gui_mch_is_blink_off(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state == BLINK_OFF;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_blinking(ਲੰਮਾ wait, ਲੰਮਾ on, ਲੰਮਾ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ਸਥਿਰ VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ ਅਣਵਰਤਿਆ,
    UINT_PTR idEvent,
    DWORD dw_ਸਮਾਂ ਅਣਵਰਤਿਆ)
{
    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ਨਿਰਾ, idEvent);

    // Eat spurious WM_TIMER messages
    ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    ਜੇ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = ਸੈੱਟ_Timer(ਨਿਰਾ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = ਸੈੱਟ_Timer(ਨਿਰਾ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ਸਥਿਰ ਖਾਲੀ
gui_mswin_rm_blink_timer(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

    ਜੇ (blink_timer == 0)
	ਵਾਪਸ;

    KillTimer(ਨਿਰਾ, blink_timer);
    // Eat spurious WM_TIMER messages
    ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    ਜੇ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink(ਖਾਲੀ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
    {
	blink_timer = ਸੈੱਟ_Timer(ਨਿਰਾ, 0, (UINT)blink_waittime, _OnBlinkTimer);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_mch_flush();
    }
}

/*
 * Call-back routines.
 */

    ਸਥਿਰ VOID CALLBACK
_OnTimer(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ ਅਣਵਰਤਿਆ,
    UINT_PTR idEvent,
    DWORD dw_ਸਮਾਂ ਅਣਵਰਤਿਆ)
{
    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ਨਿਰਾ, idEvent);
    s_timed_ਬਾਹਰਲਾ = TRUE;

    // Eat spurious WM_TIMER messages
    ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    ਜੇ (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ਸਥਿਰ ਖਾਲੀ
_OnDeadChar(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT ch ਅਣਵਰਤਿਆ,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT;
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
    ਸਥਿਰ ਪੂਰਨ
char_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ ch, char_u *string, ਪੂਰਨ slen, ਪੂਰਨ had_alt)
{
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    WCHAR	wstring[2];
    char_u	*ws = ਨਿਰਾ;

    ਜੇ (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    ਜੇ (enc_codepage > 0)
    {
	len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ਨਿਰਾ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	ਜੇ (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = Wide_Char_ਮੰਜ਼ਿਲ_Multi_Byte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ਨਿਰਾ);
	    ਜੇ (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ws = utf16_ਮੰਜ਼ਿਲ_enc(wstring, &len);
	ਜੇ (ws == ਨਿਰਾ)
	    len = 0;
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    ਜੇ (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    ਲਈ (i = 0; i < len; ++i)
	ਜੇ (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (ਪੂਰਨ)KE_CSI;
	    len += 2;
	}

    ਵਾਪਸ len;
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
    ਸਥਿਰ ਪੂਰਨ
get_ਸਰਗਰਮ_modifiers_experimental(ਖਾਲੀ)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_ਖਿਸਕਾਓ) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_ALT;
    ਜੇ ((modifiers & MOD_ਨਕਾਬ_CTRL) && (Get_ਕੁੰਜੀ_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_ਨਕਾਬ_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    ਜੇ (!(Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000)
	    &&  (Get_ਕੁੰਜੀ_State(VK_RMENU) & 0x8000)
	    && !(Get_ਕੁੰਜੀ_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_ਨਕਾਬ_ALT;

    ਵਾਪਸ modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ਸਥਿਰ ਪੂਰਨ
get_ਸਰਗਰਮ_modifiers_classic(ਖਾਲੀ)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (Get_ਕੁੰਜੀ_State(VK_ਖਿਸਕਾਓ) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_MENU) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_ALT;

    ਵਾਪਸ modifiers;
}

    ਸਥਿਰ ਪੂਰਨ
get_ਸਰਗਰਮ_modifiers(ਖਾਲੀ)
{
    ਵਾਪਸ get_ਸਰਗਰਮ_modifiers_experimental();
}

    ਸਥਿਰ ਪੂਰਨ
get_ਸਰਗਰਮ_modifiers_ਰਾਹੀਂ_ptr(ਖਾਲੀ)
{
    // marshal to corresponding implementation
    ਵਾਪਸ keycode_trans_strategy_ਵਰਤਿਆ->ptr_get_ਸਰਗਰਮ_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
_OnChar(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT cch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ਵਰਤਿਆ->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ਸਥਿਰ ਖਾਲੀ
_OnChar_experimental(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT cch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    char_u	string[40];
    ਪੂਰਨ		len = 0;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ch = cch;   // special keys are negative

    ਜੇ (dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_SKIP_ON_CHAR)
	ਵਾਪਸ;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    ਜੇ (dead_ਕੁੰਜੀ != DEAD_ਕੁੰਜੀ_TRANSIENT_ਵਿੱਚ_ON_CHAR)
	dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_OFF;

    modifiers = get_ਸਰਗਰਮ_modifiers_experimental();

    ch = simplify_ਕੁੰਜੀ(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_ਕੁੰਜੀਆਂ;
    ch = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, ch);
    --no_reduce_ਕੁੰਜੀਆਂ;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ਕੱਢੋ_modifiers(ch, &modifiers, FALSE, ਨਿਰਾ);
    ਜੇ (ch == CSI)
	ch = K_CSI;

    ਜੇ (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 3);
    }

    len = char_ਮੰਜ਼ਿਲ_string(ch, string, 40, FALSE);
    ਜੇ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ਨਿਵੇਸ਼_buf();
	got_int = TRUE;
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ਸਥਿਰ ਖਾਲੀ
_OnChar_classic(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT ch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    char_u	string[40];
    ਪੂਰਨ		len = 0;

    dead_ਕੁੰਜੀ = 0;

    len = char_ਮੰਜ਼ਿਲ_string(ch, string, 40, FALSE);
    ਜੇ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ਨਿਵੇਸ਼_buf();
	got_int = TRUE;
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ਸਥਿਰ ਖਾਲੀ
_OnSysChar(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT cch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ਵਰਤਿਆ->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ਸਥਿਰ ਖਾਲੀ
_OnSysChar_experimental(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT cch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    char_u	string[40]; // Enough for multibyte character
    ਪੂਰਨ		len;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ch = cch;   // special keys are negative

    dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_ਸਰਗਰਮ_modifiers_experimental();
    ch = simplify_ਕੁੰਜੀ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ਕੱਢੋ_modifiers(ch, &modifiers, FALSE, ਨਿਰਾ);
    ਜੇ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ਜੇ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ਜੇ (IS_SPECIAL((ਪੂਰਨ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_ਦੂਜਾ((ਪੂਰਨ)ch);
	string[len++] = K_ਤੀਜਾ((ਪੂਰਨ)ch);
    }
    ਨਹੀਂਤਾਂ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_ਮੰਜ਼ਿਲ_string(ch, string + len, 40 - len, TRUE);
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ਸਥਿਰ ਖਾਲੀ
_OnSysChar_classic(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT cch,
    ਪੂਰਨ cRepeat ਅਣਵਰਤਿਆ)
{
    char_u	string[40]; // Enough for multibyte character
    ਪੂਰਨ		len;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ch = cch;   // special keys are negative

    dead_ਕੁੰਜੀ = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_ਨਕਾਬ_ALT;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_ਖਿਸਕਾਓ) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ਨਕਾਬ_CTRL;

    ch = simplify_ਕੁੰਜੀ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ਕੱਢੋ_modifiers(ch, &modifiers, FALSE, ਨਿਰਾ);
    ਜੇ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ਜੇ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ਜੇ (IS_SPECIAL((ਪੂਰਨ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_ਦੂਜਾ((ਪੂਰਨ)ch);
	string[len++] = K_ਤੀਜਾ((ਪੂਰਨ)ch);
    }
    ਨਹੀਂਤਾਂ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_ਮੰਜ਼ਿਲ_string(ch, string + len, 40 - len, TRUE);
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
}

    ਸਥਿਰ ਖਾਲੀ
_OnMouseEvent(
    ਪੂਰਨ button,
    ਪੂਰਨ x,
    ਪੂਰਨ y,
    ਪੂਰਨ repeated_click,
    UINT ਕੁੰਜੀ_Flags)
{
    ਪੂਰਨ vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    ਜੇ (ਕੁੰਜੀ_Flags & MK_ਖਿਸਕਾਓ)
	vim_modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (ਕੁੰਜੀ_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ਸਥਿਰ ਖਾਲੀ
On_Mouse_Button_ਥੱਲੇ(
    HWND hwnd ਅਣਵਰਤਿਆ,
    BOOL fDoubleClick ਅਣਵਰਤਿਆ,
    ਪੂਰਨ x,
    ਪੂਰਨ y,
    UINT ਕੁੰਜੀ_Flags)
{
    ਸਥਿਰ LONG	s_prev_ਸਮਾਂ = 0;

    LONG    current_ਸਮਾਂ = Get_ਸੁਨੇਹਾ_ਸਮਾਂ();
    ਪੂਰਨ	    button = -1;
    ਪੂਰਨ	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);

    ਜੇ (s_u_ਸੁਨੇਹਾ == WM_LBUTTONDOWN || s_u_ਸੁਨੇਹਾ == WM_LBUTTONDBLCLK)
	button = MOUSE_ਖੱਬਾ;
    ਨਹੀਂਤਾਂ ਜੇ (s_u_ਸੁਨੇਹਾ == WM_MBUTTONDOWN || s_u_ਸੁਨੇਹਾ == WM_MBUTTONDBLCLK)
	button = MOUSE_ਵਿਚਕਾਰ;
    ਨਹੀਂਤਾਂ ਜੇ (s_u_ਸੁਨੇਹਾ == WM_RBUTTONDOWN || s_u_ਸੁਨੇਹਾ == WM_RBUTTONDBLCLK)
	button = MOUSE_ਸੱਜਾ;
    ਨਹੀਂਤਾਂ ਜੇ (s_u_ਸੁਨੇਹਾ == WM_XBUTTONDOWN || s_u_ਸੁਨੇਹਾ == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    ਨਹੀਂਤਾਂ ਜੇ (s_u_ਸੁਨੇਹਾ == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	ਜੇ (s_button_pending == MOUSE_ਖੱਬਾ)
	    button = MOUSE_ਸੱਜਾ;
	ਨਹੀਂਤਾਂ
	    button = MOUSE_ਖੱਬਾ;
    }

    ਜੇ (button < 0)
	ਵਾਪਸ;

    repeated_click = ((ਪੂਰਨ)(current_ਸਮਾਂ - s_prev_ਸਮਾਂ) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    ਜੇ (repeated_click
	    && ((button == MOUSE_ਖੱਬਾ && s_button_pending == MOUSE_ਸੱਜਾ)
		|| (button == MOUSE_ਸੱਜਾ
		    && s_button_pending == MOUSE_ਖੱਬਾ)))
    {
	/*
	 * Hmm, gui.c will ignore more than one button down at a time, so
	 * pretend we let go of it first.
	 */
	gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, 0x0);
	button = MOUSE_ਵਿਚਕਾਰ;
	repeated_click = FALSE;
	s_button_pending = -1;
	_OnMouseEvent(button, x, y, repeated_click, ਕੁੰਜੀ_Flags);
    }
    ਨਹੀਂਤਾਂ ਜੇ ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ਸੱਜਾ)))
    {
	ਜੇ (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, ਕੁੰਜੀ_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, ਕੁੰਜੀ_Flags);
    }
    ਨਹੀਂਤਾਂ
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
	s_kFlags_pending = ਕੁੰਜੀ_Flags;
    }

    s_prev_ਸਮਾਂ = current_ਸਮਾਂ;
}

    ਸਥਿਰ ਖਾਲੀ
_OnMouseMoveOrRelease(
    HWND hwnd ਅਣਵਰਤਿਆ,
    ਪੂਰਨ x,
    ਪੂਰਨ y,
    UINT ਕੁੰਜੀ_Flags)
{
    ਪੂਰਨ button;

    s_getting_focus = FALSE;
    ਜੇ (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    ਜੇ (s_u_ਸੁਨੇਹਾ == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	ਜੇ (!(ਕੁੰਜੀ_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ਵਾਪਸ;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	ਸੈੱਟ_Capture(s_ਪਾਠ_Area);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    ਨਹੀਂਤਾਂ
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, ਕੁੰਜੀ_Flags);
}

    ਸਥਿਰ ਖਾਲੀ
On_ਮਾਪ_ਪਾਠ_Area(
    HWND hwnd ਅਣਵਰਤਿਆ,
    UINT state ਅਣਵਰਤਿਆ,
    ਪੂਰਨ cx ਅਣਵਰਤਿਆ,
    ਪੂਰਨ cy ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	directx_binddc();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ਸਥਿਰ vimmenu_T *
gui_mswin_ਲੱਭੋ_menu(
    vimmenu_T	*pMenu,
    ਪੂਰਨ		id)
{
    vimmenu_T	*p_ਬੱਚਾ_Menu;

    ਜਦੋਂ (pMenu)
    {
	ਜੇ (pMenu->id == (UINT)id)
	    ਰੋਕੋ;
	ਜੇ (pMenu->children != ਨਿਰਾ)
	{
	    p_ਬੱਚਾ_Menu = gui_mswin_ਲੱਭੋ_menu(pMenu->children, id);
	    ਜੇ (p_ਬੱਚਾ_Menu)
	    {
		pMenu = p_ਬੱਚਾ_Menu;
		ਰੋਕੋ;
	    }
	}
	pMenu = pMenu->ਅਗਲਾ;
    }
    ਵਾਪਸ pMenu;
}

    ਸਥਿਰ ਖਾਲੀ
_OnMenu(
    HWND	hwnd ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		id,
    HWND	hwndCtl ਅਣਵਰਤਿਆ,
    UINT	ਕੋਡ_Notify ਅਣਵਰਤਿਆ)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_ਲੱਭੋ_menu(ਜੜ੍ਹ_menu, id);
    ਜੇ (pMenu)
	gui_menu_cb(pMenu);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ਸਥਿਰ ਖਾਲੀ
On_ਲੱਭੋ_Repl(ਖਾਲੀ)
{
    ਪੂਰਨ	    flags = 0;
    ਪੂਰਨ	    ਥੱਲੇ;

    ਜੇ (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);

    ਜੇ (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);
    }
    ਨਹੀਂਤਾਂ ਜੇ (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);
    }
    ਨਹੀਂਤਾਂ ਜੇ (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    ਜੇ (flags == 0)
	ਵਾਪਸ;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    ਜੇ (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_ਸਾਰਾ_ਸ਼ਬਦ;
    ਜੇ (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_ਮੇਲ_CASE;
    ਥੱਲੇ = (s_findrep_struct.Flags & FR_ਥੱਲੇ) != 0;
    p = utf16_ਮੰਜ਼ਿਲ_enc(s_findrep_struct.lpstr_ਲੱਭੋ_What, ਨਿਰਾ);
    q = utf16_ਮੰਜ਼ਿਲ_enc(s_findrep_struct.lpstrReplaceWith, ਨਿਰਾ);
    ਜੇ (p != ਨਿਰਾ && q != ਨਿਰਾ)
	gui_do_findrepl(flags, p, q, ਥੱਲੇ);
    vim_free(p);
    vim_free(q);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
HandleMouseHide(UINT u_ਸੁਨੇਹਾ, LPARAM lParam)
{
    ਸਥਿਰ LPARAM ਆਖਰੀ_l_Param = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    ਜੇ (u_ਸੁਨੇਹਾ == WM_MOUSEMOVE || u_ਸੁਨੇਹਾ == WM_NCMOUSEMOVE)
    {
	ਜੇ (lParam == ਆਖਰੀ_l_Param)
	    ਵਾਪਸ;
	ਆਖਰੀ_l_Param = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    ਚੋਣ (u_ਸੁਨੇਹਾ)
    {
    ਹਾਲਤ WM_KEYUP:
    ਹਾਲਤ WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	ਜੇ (p_mh)
	    gui_mch_mousehide(TRUE);
	ਰੋਕੋ;

    ਹਾਲਤ WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    ਹਾਲਤ WM_SYSCHAR:
    ਹਾਲਤ WM_MOUSEMOVE:	 // show the pointer on any mouse action
    ਹਾਲਤ WM_LBUTTONDOWN:
    ਹਾਲਤ WM_LBUTTONUP:
    ਹਾਲਤ WM_MBUTTONDOWN:
    ਹਾਲਤ WM_MBUTTONUP:
    ਹਾਲਤ WM_RBUTTONDOWN:
    ਹਾਲਤ WM_RBUTTONUP:
    ਹਾਲਤ WM_XBUTTONDOWN:
    ਹਾਲਤ WM_XBUTTONUP:
    ਹਾਲਤ WM_NCMOUSEMOVE:
    ਹਾਲਤ WM_NCLBUTTONDOWN:
    ਹਾਲਤ WM_NCLBUTTONUP:
    ਹਾਲਤ WM_NCMBUTTONDOWN:
    ਹਾਲਤ WM_NCMBUTTONUP:
    ਹਾਲਤ WM_NCRBUTTONDOWN:
    ਹਾਲਤ WM_NCRBUTTONUP:
    ਹਾਲਤ WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	ਰੋਕੋ;
    }
}

    ਸਥਿਰ LRESULT CALLBACK
ਪਾਠ_Area_Wnd_Proc(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ,
    WPARAM wParam,
    LPARAM lParam)
{
    /*
    TRACE("TextAreaWndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x\n",
	  hwnd, uMsg, wParam, lParam);
    */

    HandleMouseHide(u_ਸੁਨੇਹਾ, lParam);

    s_u_ਸੁਨੇਹਾ = u_ਸੁਨੇਹਾ;
    s_wParam = wParam;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    track_user_activity(u_ਸੁਨੇਹਾ);
#ਜੇ_ਅੰਤ

    ਚੋਣ (u_ਸੁਨੇਹਾ)
    {
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_LBUTTONDBLCLK,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_LBUTTONDOWN,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_MBUTTONDBLCLK,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_MBUTTONDOWN,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_RBUTTONDBLCLK,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_RBUTTONDOWN,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_XBUTTONDBLCLK,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_XBUTTONDOWN,On_Mouse_Button_ਥੱਲੇ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_ਮਾਪ,	On_ਮਾਪ_ਪਾਠ_Area);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
	ਹਾਲਤ WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ
	ਮੂਲ:
	    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
    }
}

    ਸਥਿਰ ਖਾਲੀ
dyn_dwm_ਲੱਦੋ(ਖਾਲੀ)
{
    hLibDwm = vim_ਲੱਦੋ_Lib("dwmapi.dll");
    ਜੇ (hLibDwm == ਨਿਰਾ)
	ਵਾਪਸ;

    p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	Get_Proc_ਪਤਾ(hLibDwm, "DwmSetWindowAttribute");

    ਜੇ (p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute == ਨਿਰਾ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ਨਿਰਾ;
	ਵਾਪਸ;
    }
}

ਬਾਹਰੀ DWORD win_ਸੰਸਕਰਣ; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_titlebar_colors(ਖਾਲੀ)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    ਜੇ (p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute == ਨਿਰਾ || win_ਸੰਸਕਰਣ < MAKE_VER(10, 0, 22000))
	ਵਾਪਸ;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T ਪਾਠ_Color = DWMWA_COLOR_DEFAULT;

    ਜੇ (vim_strchr(p_go, GO_TITLEBAR) != ਨਿਰਾ)
    {
	ਜੇ (gui.ਵਿੱਚ_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    ਪਾਠ_Color = gui.title_fg_pixel;
	}
	ਨਹੀਂਤਾਂ
	{
	    captionColor = gui.titlenc_bg_pixel;
	    ਪਾਠ_Color = gui.titlenc_fg_pixel;
	}

	ਜੇ (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	ਜੇ (ਪਾਠ_Color == INVALCOLOR)
	    ਪਾਠ_Color = DWMWA_COLOR_DEFAULT;
    }

    p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, ਆਕਾਰ(captionColor));
    p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute(s_hwnd, DWMWA_ਪਾਠ_COLOR,
	    &ਪਾਠ_Color, ਆਕਾਰ(ਪਾਠ_Color));
}

/*
 * Called when the foreground or background color has been changed.
 */
    ਖਾਲੀ
gui_mch_new_colors(ਖਾਲੀ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.ਪਿੱਠ_pixel);
    prevBrush = (HBRUSH)ਸੈੱਟ_Class_Long_Ptr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ਨਿਰਾ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    ਜੇ (s_textarea_bg_only && s_ਪਾਠ_Area != ਨਿਰਾ)
	InvalidateRect(s_ਪਾਠ_Area, ਨਿਰਾ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    ਖਾਲੀ
gui_mch_def_colors(ਖਾਲੀ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.ਪਿੱਠ_pixel = GetSysColor(COLOR_ਖਿੜਕੀ);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ਪਿੱਠ_pixel = gui.ਪਿੱਠ_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ਪੂਰਨ
gui_mch_open(ਖਾਲੀ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    ਜੇ (!Is_ਖਿੜਕੀ_Visible(s_hwnd))
    {
	Show_ਖਿੜਕੀ(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	ਜੇ (s_ਪਾਠ_Area != ਨਿਰਾ)
	{
	    Update_ਖਿੜਕੀ(s_ਪਾਠ_Area);
	    gui_mch_flush();
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    RECT    rect;

    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rect);
    *x = rect.ਖੱਬਾ;
    *y = rect.ਸਿਖਰ;
    ਵਾਪਸ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਸੈੱਟ_ਖਿੜਕੀ_Pos(s_hwnd, ਨਿਰਾ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ w, ਪੂਰਨ h)
{
    ਸਥਿਰ ਪੂਰਨ oldx = 0;
    ਸਥਿਰ ਪੂਰਨ oldy = 0;

    ਸੈੱਟ_ਖਿੜਕੀ_Pos(s_ਪਾਠ_Area, ਨਿਰਾ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, WM_ਮਾਪ,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_ਉਚਾਈ));
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_TABLINE)
    ਜੇ (showing_tabline)
    {
	ਪੂਰਨ	ਸਿਖਰ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
	    ਸਿਖਰ = gui.toolbar_ਉਚਾਈ;
# endif
	GetClientRect(s_hwnd, &rect);
	Move_ਖਿੜਕੀ(s_tabhwnd, 0, ਸਿਖਰ, rect.ਸੱਜਾ, gui.tabline_ਉਚਾਈ, TRUE);
    }
#ਜੇ_ਅੰਤ

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    ਜੇ (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ਨਿਰਾ, FALSE);
	oldx = x;
	oldy = y;
    }
}


/*
 * Scrollbar stuff:
 */

    ਖਾਲੀ
gui_mch_enable_scrollbar(
    scrollbar_T     *sb,
    ਪੂਰਨ		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(
    scrollbar_T *sb,
    ਪੂਰਨ		x,
    ਪੂਰਨ		y,
    ਪੂਰਨ		w,
    ਪੂਰਨ		h)
{
    ਸੈੱਟ_ਖਿੜਕੀ_Pos(sb->id, ਨਿਰਾ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    RECT    rcTxt, rcWnd;
    ਪੂਰਨ	    xpad;

    Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rcTxt);
    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rcWnd);
    xpad = rcWnd.ਸੱਜਾ - rcTxt.ਸੱਜਾ - gui.scrollbar_ਚੌੜਾਈ
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ਵਾਪਸ (xpad < 0) ? 0 : xpad;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    RECT    rcTxt, rcWnd;
    ਪੂਰਨ	    ypad;

    Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rcTxt);
    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_ਉਚਾਈ
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ਵਾਪਸ (ypad < 0) ? 0 : ypad;
}

    ਖਾਲੀ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ਪੂਰਨ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = Create_ਖਿੜਕੀ(
	"SCROLLBAR", "Scrollbar",
	WS_ਬੱਚਾ | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ਨਿਰਾ,
	g_hinst, ਨਿਰਾ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ਸਥਿਰ scrollbar_T *
gui_mswin_ਲੱਭੋ_scrollbar(HWND hwnd)
{
    win_T	*wp;

    ਜੇ (gui.bottom_sbar.id == hwnd)
	ਵਾਪਸ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ਜੇ (wp->w_scrollbars[SBAR_ਖੱਬਾ].id == hwnd)
	    ਵਾਪਸ &wp->w_scrollbars[SBAR_ਖੱਬਾ];
	ਜੇ (wp->w_scrollbars[SBAR_ਸੱਜਾ].id == hwnd)
	    ਵਾਪਸ &wp->w_scrollbars[SBAR_ਸੱਜਾ];
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
update_scrollbar_ਮਾਪ(ਖਾਲੀ)
{
    gui.scrollbar_ਚੌੜਾਈ = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_ਉਚਾਈ = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ਸਥਿਰ ਖਾਲੀ
Get_ਔਸਤਨ_Font_ਮਾਪ(HDC hdc, ਮਾਪ_3 *ਮਾਪ)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    Get_ਪਾਠ_Extent_ਬਿੰਦੂ(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, ਮਾਪ);
    ਮਾਪ->cx = (ਮਾਪ->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    ਸਥਿਰ ਖਾਲੀ
Get_Font_ਮਾਪ(GuiFont font, ਪੂਰਨ *char_ਚੌੜਾਈ, ਪੂਰਨ *char_ਉਚਾਈ)
{
    HWND    hwnd = Get_Desktop_ਖਿੜਕੀ();
    HDC	    hdc = Get_ਖਿੜਕੀ_DC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    ਮਾਪ_3    ਮਾਪ;
    TEXTMETRIC tm;

    Get_ਪਾਠ_Metrics(hdc, &tm);
    Get_ਔਸਤਨ_Font_ਮਾਪ(hdc, &ਮਾਪ);

    ਜੇ (char_ਚੌੜਾਈ)
	*char_ਚੌੜਾਈ = ਮਾਪ.cx + tm.tmOverhang;
    ਜੇ (char_ਉਚਾਈ)
	*char_ਉਚਾਈ = tm.tm_ਉਚਾਈ + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ਸਥਿਰ ਖਾਲੀ
Update_Font_ਮਾਪ(GuiFont font)
{
    Get_Font_ਮਾਪ(font, &gui.char_ਚੌੜਾਈ, &gui.char_ਉਚਾਈ);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ਪੂਰਨ
gui_mch_adjust_charheight(ਖਾਲੀ)
{
    Update_Font_ਮਾਪ(gui.norm_font);
    ਵਾਪਸ OK;
}

    ਸਥਿਰ GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ਨਿਰਾ;

    // Load the font
    font = Create_Font_ਅਸਿੱਧਾ_W(lf);

    ਜੇ (font == ਨਿਰਾ)
	ਵਾਪਸ NOFONT;

    ਵਾਪਸ (GuiFont)font;
}

    ਸਥਿਰ ਪੂਰਨ
pixels_ਮੰਜ਼ਿਲ_points(ਪੂਰਨ pixels, ਪੂਰਨ vertical)
{
    ਪੂਰਨ		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = Get_Desktop_ਖਿੜਕੀ();
    hdc = Get_ਖਿੜਕੀ_DC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ਵਾਪਸ points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    ਪੂਰਨ		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    ਜੇ (get_logfont(&lf, name, ਨਿਰਾ, giveErrorIfMissing) == OK)
    {
	lf.lf_ਉਚਾਈ = adjust_fontsize_by_dpi(lf.lf_ਉਚਾਈ);
	font = get_font_handle(&lf);
    }
    ਜੇ (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ਵਾਪਸ font;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font ਅਣਵਰਤਿਆ, char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strsave(name);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_free_font(GuiFont font)
{
    ਜੇ (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    ਪੂਰਨ i;

    ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Sys_Color_ਸਾਰਣੀ
    {
	ਅੱਖਰ	    *name;
	ਪੂਰਨ	    color;
    } Sys_Color_ਸਾਰਣੀ;

    ਸਥਿਰ Sys_Color_ਸਾਰਣੀ sys_ਸਾਰਣੀ[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#ਜੇ_ਅੰਤ
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
	{"SYS_WINDOW", COLOR_ਖਿੜਕੀ},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    ਲਈ (i = 0; i < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(sys_ਸਾਰਣੀ); i++)
	ਜੇ (STRICMP(name, sys_ਸਾਰਣੀ[i].name) == 0)
	    ਵਾਪਸ GetSysColor(sys_ਸਾਰਣੀ[i].color);

    ਵਾਪਸ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    ਵਾਪਸ gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ਪੂਰਨ
gui_mch_haskey(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].vim_code1 != NUL; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].vim_code0
				       && name[1] == special_ਕੁੰਜੀਆਂ[i].vim_code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
gui_mch_beep(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_Beep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ਖਾਲੀ
gui_mch_invert_rectangle(
    ਪੂਰਨ	    r,
    ਪੂਰਨ	    c,
    ਪੂਰਨ	    nr,
    ਪੂਰਨ	    nc)
{
    RECT    rc;

#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ਖੱਬਾ = FILL_X(c);
    rc.ਸਿਖਰ = FILL_Y(r);
    rc.ਸੱਜਾ = rc.ਖੱਬਾ + nc * gui.char_ਚੌੜਾਈ;
    rc.bottom = rc.ਸਿਖਰ + nr * gui.char_ਉਚਾਈ;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    ਖਾਲੀ
gui_mch_iconify(ਖਾਲੀ)
{
    Show_ਖਿੜਕੀ(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    ਖਾਲੀ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ਖੱਬਾ = FILL_X(gui.ਖਾਨਾ);
    rc.ਸਿਖਰ = FILL_Y(gui.ਪੰਕਤੀ);
    rc.ਸੱਜਾ = rc.ਖੱਬਾ + gui.char_ਚੌੜਾਈ;
    ਜੇ (mb_lefthalve(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ))
	rc.ਸੱਜਾ += gui.char_ਚੌੜਾਈ;
    rc.bottom = rc.ਸਿਖਰ + gui.char_ਉਚਾਈ;
    hbr = CreateSolidBrush(color);
    ਚੌਖਟਾ_Rect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ਖਾਲੀ
gui_mch_draw_part_cursor(
    ਪੂਰਨ		w,
    ਪੂਰਨ		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ਖੱਬਾ =
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.ਖਾਨਾ + 1) - w :
#ਜੇ_ਅੰਤ
		    FILL_X(gui.ਖਾਨਾ);
    rc.ਸਿਖਰ = FILL_Y(gui.ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h;
    rc.ਸੱਜਾ = rc.ਖੱਬਾ + w;
    rc.bottom = rc.ਸਿਖਰ + h;

    fill_rect(&rc, ਨਿਰਾ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ਸਥਿਰ ਖਾਲੀ
output_Dead_ਕੁੰਜੀ_re_Post_Ex(ਸੁਨੇਹਾ_3 original_ਸੁਨੇਹਾ, ਪੂਰਨ dead_key2set)
{
    ਸਥਿਰ ਸੁਨੇਹਾ_3 deadCharExpel;

    ਜੇ (dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_OFF)
	ਵਾਪਸ;

    dead_ਕੁੰਜੀ = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.ਸੁਨੇਹਾ_2 = original_ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2;
    deadCharExpel.hwnd    = original_ਸੁਨੇਹਾ.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    Translate_ਸੁਨੇਹਾ(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    Post_ਸੁਨੇਹਾ(original_ਸੁਨੇਹਾ.hwnd, original_ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2, original_ਸੁਨੇਹਾ.wParam,
							  original_ਸੁਨੇਹਾ.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ਸਥਿਰ ਖਾਲੀ
output_Dead_ਕੁੰਜੀ_re_Post(ਸੁਨੇਹਾ_3 original_ਸੁਨੇਹਾ)
{
    output_Dead_ਕੁੰਜੀ_re_Post_Ex(original_ਸੁਨੇਹਾ, DEAD_ਕੁੰਜੀ_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ਸਥਿਰ ਖਾਲੀ process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ(UINT vk, ਅਚਲ ਸੁਨੇਹਾ_3 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ਵਰਤਿਆ->ptr_process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ਸਥਿਰ ਖਾਲੀ
process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_experimental(UINT vk, ਅਚਲ ਸੁਨੇਹਾ_3 *pmsg)
{
    WCHAR	ch[8];
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    UINT	ਜਾਂਚ_ਕੋਡ;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_ਖਿਸਕਾਓ) & 0x8000)
	keyboard_state[VK_ਖਿਸਕਾਓ] = 0x80;
    ਜੇ (Get_ਕੁੰਜੀ_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    ਜੇ ((Get_ਕੁੰਜੀ_State(VK_RMENU) & 0x8000)
				 && (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    ਜਾਂਚ_ਕੋਡ = Map_Virtual_ਕੁੰਜੀ(vk, MAPVK_VK_ਮੰਜ਼ਿਲ_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ਮੰਜ਼ਿਲ_Unicode(vk, ਜਾਂਚ_ਕੋਡ, keyboard_state, ch, ARRAY_ਲੰਮਾਈ(ch),
	    0);
    ਜੇ (len < 0)
	dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT;

    ਜੇ (len <= 0)
    {
	ਪੂਰਨ wm_char = NUL;

	ਜੇ (dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT
		&& (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000))
	{
	    ਜੇ (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && ਜਾਂਚ_ਕੋਡ == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && ਜਾਂਚ_ਕੋਡ == 41))
		wm_char = '[';
	    ਜੇ (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && ਜਾਂਚ_ਕੋਡ == 27))
		wm_char = ']';
	}
	ਜੇ (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    Post_ਸੁਨੇਹਾ_W(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_TRANSIENT_ਵਿੱਚ_ON_CHAR;
	}
	ਵਾਪਸ;
    }

    // Post the message as TranslateMessage would do.
    ਜੇ (pmsg->ਸੁਨੇਹਾ_2 == WM_KEYDOWN)
    {
	ਲਈ (i = 0; i < len; i++)
	    Post_ਸੁਨੇਹਾ_W(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < len; i++)
	    Post_ਸੁਨੇਹਾ_W(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ਸਥਿਰ ਖਾਲੀ
process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ_classic(UINT vk, ਅਚਲ ਸੁਨੇਹਾ_3 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    ਜੇ (vk != 0xff
	    && (Get_ਕੁੰਜੀ_State(VK_CONTROL) & 0x8000)
	    && !(Get_ਕੁੰਜੀ_State(VK_ਖਿਸਕਾਓ) & 0x8000)
	    && !(Get_ਕੁੰਜੀ_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	ਜੇ (vk == '6' || Map_Virtual_ਕੁੰਜੀ(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	ਨਹੀਂਤਾਂ ਜੇ (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	ਨਹੀਂਤਾਂ ਜੇ (vk == '2' || Map_Virtual_ਕੁੰਜੀ(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 1);
	}
	ਨਹੀਂਤਾਂ
	    Translate_ਸੁਨੇਹਾ(pmsg);
    }
    ਨਹੀਂਤਾਂ
	Translate_ਸੁਨੇਹਾ(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ਸਥਿਰ ਖਾਲੀ
process_ਸੁਨੇਹਾ(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_3		ਸੁਨੇਹਾ;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    ਪੂਰਨ		i;
    ਪੂਰਨ		modifiers = 0;
    ਪੂਰਨ		ਕੁੰਜੀ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਸਥਿਰ char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#ਜੇ_ਅੰਤ
    ਸਥਿਰ ਪੂਰਨ  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    ਜੇ (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    Get_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, ਨਿਰਾ, 0, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
    // Look after OLE Automation commands
    ਜੇ (ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2 == WM_OLE)
    {
	char_u *str = (char_u *)ਸੁਨੇਹਾ.lParam;
	ਜੇ (str == ਨਿਰਾ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(str, (ਪੂਰਨ)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    // Don't process messages used by the dialog
    ਜੇ (s_findrep_hwnd != ਨਿਰਾ && Is_Dialog_ਸੁਨੇਹਾ_W(s_findrep_hwnd, &ਸੁਨੇਹਾ))
    {
	HandleMouseHide(ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2, ਸੁਨੇਹਾ.lParam);
	ਵਾਪਸ;
    }
#ਜੇ_ਅੰਤ

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    ਜੇ (ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2 == WM_KEYDOWN || ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2 == WM_SYSKEYDOWN)
    {
	vk = (ਪੂਰਨ) ਸੁਨੇਹਾ.wParam;

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
	ਜੇ (keycode_trans_strategy_ਵਰਤਿਆ->is_experimental()
		&& dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_SKIP_ON_CHAR)
	{
	    dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_OFF;
	}

	ਜੇ (dead_ਕੁੰਜੀ != DEAD_ਕੁੰਜੀ_OFF)
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
	    ਜੇ (keycode_trans_strategy_ਵਰਤਿਆ->is_experimental() &&
		    dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_TRANSIENT_ਵਿੱਚ_ON_CHAR)
	    {
		output_Dead_ਕੁੰਜੀ_re_Post_Ex(ਸੁਨੇਹਾ,
				       /*dead_key2set=*/DEAD_ਕੁੰਜੀ_SKIP_ON_CHAR);
		ਵਾਪਸ;
	    }

	    ਜੇ (dead_ਕੁੰਜੀ != DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ਵਾਪਸ;
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
	    ਜੇ ((vk == VK_SPACE || vk == VK_ਪਿੱਠ || vk == VK_ESCAPE))
	    {
		dead_ਕੁੰਜੀ = DEAD_ਕੁੰਜੀ_OFF;
		Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
		ਵਾਪਸ;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    ਨਹੀਂਤਾਂ ਜੇ ((get_real_state()
			    & (ਬਹੁਲਕ_ਪਾਓ | ਬਹੁਲਕ_CMDLINE | ਬਹੁਲਕ_SELECT)) == 0)
	    {
		output_Dead_ਕੁੰਜੀ_re_Post(ਸੁਨੇਹਾ);
		ਵਾਪਸ;
	    }
	}

	// Check for CTRL-BREAK
	ਜੇ (vk == VK_CANCEL)
	{
	    trash_ਨਿਵੇਸ਼_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	ਜੇ (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
	    ਵਾਪਸ;
	}

	ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == vk
		    && (vk != VK_SPACE || !(Get_ਕੁੰਜੀ_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		ਜੇ (dead_ਕੁੰਜੀ == DEAD_ਕੁੰਜੀ_ਸੈੱਟ_DEFAULT
			&& (special_ਕੁੰਜੀਆਂ[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_ਕੁੰਜੀ_re_Post(ਸੁਨੇਹਾ);
		    ਵਾਪਸ;
		}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		ਜੇ (vk == VK_F10
			&& gui.menu_is_ਸਰਗਰਮ
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ਨਿਰਾ, ਨਿਰਾ) == ਨਿਰਾ)
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		modifiers = get_ਸਰਗਰਮ_modifiers_ਰਾਹੀਂ_ptr();

		ਜੇ (special_ਕੁੰਜੀਆਂ[i].vim_code1 == NUL)
		    ਕੁੰਜੀ = special_ਕੁੰਜੀਆਂ[i].vim_code0;
		ਨਹੀਂਤਾਂ
		    ਕੁੰਜੀ = ਮੰਜ਼ਿਲ_SPECIAL(special_ਕੁੰਜੀਆਂ[i].vim_code0,
						   special_ਕੁੰਜੀਆਂ[i].vim_code1);
		ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);
		ਜੇ (ਕੁੰਜੀ == CSI)
		    ਕੁੰਜੀ = K_CSI;

		ਜੇ (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 3);
		}

		ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
		{
		    string[0] = CSI;
		    string[1] = K_ਦੂਜਾ(ਕੁੰਜੀ);
		    string[2] = K_ਤੀਜਾ(ਕੁੰਜੀ);
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 3);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ	len;

		    // Handle "key" as a Unicode character.
		    len = char_ਮੰਜ਼ਿਲ_string(ਕੁੰਜੀ, string, 40, FALSE);
		    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
		}
		ਰੋਕੋ;
	    }
	}

	// Not a special key.
	ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == 0)
	{
	    process_ਸੁਨੇਹਾ_usual_ਕੁੰਜੀ(vk, &ਸੁਨੇਹਾ);
	}
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2 == WM_IME_NOTIFY)
	_OnImeNotify(ਸੁਨੇਹਾ.hwnd, (DWORD)ਸੁਨੇਹਾ.wParam, (DWORD)ਸੁਨੇਹਾ.lParam);
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਨੇਹਾ.ਸੁਨੇਹਾ_2 == WM_KEYUP && im_get_ਸਥਿਤੀ())
	// added for non-MS IME (Yasuhiro Matsumoto)
	Translate_ਸੁਨੇਹਾ(&ਸੁਨੇਹਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    ਜੇ (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ਨਿਰਾ, ਨਿਰਾ) == ਨਿਰਾ)
#ਜੇ_ਅੰਤ
	Dispatch_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ);
}

/*
 * Catch up with any queued events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the event queue (& no timers pending), then we return
 * immediately.
 */
    ਖਾਲੀ
gui_mch_update(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_3	    ਸੁਨੇਹਾ;

    ਜੇ (!s_busy_processing)
	ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, ਨਿਰਾ, 0, 0, PM_NOREMOVE)
						  && !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ())
	    process_ਸੁਨੇਹਾ();
}

    ਸਥਿਰ ਖਾਲੀ
remove_any_timer(ਖਾਲੀ)
{
    ਸੁਨੇਹਾ_3		ਸੁਨੇਹਾ;

    ਜੇ (s_wait_timer != 0 && !s_timed_ਬਾਹਰਲਾ)
    {
	KillTimer(ਨਿਰਾ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	ਜਦੋਂ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ਸਥਿਰ garray_T	server_pending_ga = {0, 0, 0, 0, ਨਿਰਾ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    ਖਾਲੀ
server_ਜੋੜੋ_ਨਿਵੇਸ਼(char_u *str)
{
    ਜੇ (server_pending_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	ga_init2(&server_pending_ga, 1, 200);
    ga_concat(&server_pending_ga, str);
}

/*
 * Insert postponed keys received from a client, but only when the typeahead
 * buffer is empty, so that they are not mixed into a command that is currently
 * being read.
 */
    ਸਥਿਰ ਖਾਲੀ
server_flush_ਨਿਵੇਸ਼(ਖਾਲੀ)
{
    char_u  *str;

    ਜੇ (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ਵਾਪਸ;
    str = vim_strnsave(server_pending_ga.ga_ਅੰਕੜੇ, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    ਜੇ (str != ਨਿਰਾ)
    {
	server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(str);
	vim_free(str);
    }
}
#ਜੇ_ਅੰਤ

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ਪੂਰਨ
gui_mch_wait_for_chars(ਲੰਮਾ wtime)
{
    ਪੂਰਨ		focus;

    s_timed_ਬਾਹਰਲਾ = FALSE;

    ਜੇ (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	ਜੇ (s_busy_processing)
	    ਵਾਪਸ FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = ਸੈੱਟ_Timer(ਨਿਰਾ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.ਵਿੱਚ_focus;
    ਜਦੋਂ (!s_timed_ਬਾਹਰਲਾ)
    {
	// Stop or start blinking when focus changes
	ਜੇ (gui.ਵਿੱਚ_focus != focus)
	{
	    ਜੇ (gui.ਵਿੱਚ_focus)
		gui_mch_ਸ਼ੁਰੂ_blink();
	    ਨਹੀਂਤਾਂ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ਵਿੱਚ_focus;
	}

	ਜੇ (s_need_activate)
	{
	    (ਖਾਲੀ)ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
	    s_need_activate = FALSE;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
	did_ਜੋੜੋ_timer = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
	// Check channel I/O while waiting for a message.
	ਲਈ (;;)
	{
	    ਸੁਨੇਹਾ_3 ਸੁਨੇਹਾ;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ਨਿਵੇਸ਼();
# endif
# ifdef FEAT_TIMERS
	    ਜੇ (did_ਜੋੜੋ_timer)
		ਰੋਕੋ;
# endif
	    ਜੇ (Peek_ਸੁਨੇਹਾ_W(&ਸੁਨੇਹਾ, ਨਿਰਾ, 0, 0, PM_NOREMOVE))
	    {
		process_ਸੁਨੇਹਾ();
		ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || ਸੁਨੇਹਾ_Wait_For_ਗੁਣਜ_Objects(0, ਨਿਰਾ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		ਰੋਕੋ;
	}
#ਨਹੀਂਤਾਂ
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_ਸੁਨੇਹਾ();
#ਜੇ_ਅੰਤ

	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    ਜੇ (!s_getting_focus)
		s_button_pending = -1;

	    ਵਾਪਸ OK;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
	ਜੇ (did_ਜੋੜੋ_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    }
    allow_scrollbar = FALSE;
    ਵਾਪਸ FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ਖਾਲੀ
gui_mch_clear_ਖੰਡ(
    ਪੂਰਨ		row1,
    ਪੂਰਨ		col1,
    ਪੂਰਨ		row2,
    ਪੂਰਨ		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ਖੱਬਾ = FILL_X(col1);
    rc.ਸਿਖਰ = FILL_Y(row1);
    rc.ਸੱਜਾ = FILL_X(col2 + 1) + (col2 == Columns - 1);
    rc.bottom = FILL_Y(row2 + 1);
    clear_rect(&rc);
}

/*
 * Clear the whole text window.
 */
    ਖਾਲੀ
gui_mch_clear_all(ਖਾਲੀ)
{
    RECT    rc;

    rc.ਖੱਬਾ = 0;
    rc.ਸਿਖਰ = 0;
    rc.ਸੱਜਾ = Columns * gui.char_ਚੌੜਾਈ + 2 * gui.border_ਚੌੜਾਈ;
    rc.bottom = ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ + 2 * gui.border_ਚੌੜਾਈ;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    ਖਾਲੀ
gui_mch_enable_menu(ਪੂਰਨ flag)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਸੈੱਟ_Menu(s_hwnd, flag ? s_menuBar : ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_menu_pos(
    ਪੂਰਨ	    x ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    y ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    w ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    h ਅਣਵਰਤਿਆ)
{
    // It will be in the right place anyway
}

#ਜੇ defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    ਖਾਲੀ
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    ਪੂਰਨ		hidden)
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
    ਖਾਲੀ
gui_mch_draw_menubar(ਖਾਲੀ)
{
    DrawMenuBar(s_hwnd);
}
#ਜੇ_ਅੰਤ // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ਵਾਪਸ (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#ਜੇ defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ਸਥਿਰ ਸ਼ਬਦ_2
Pixel_ਮੰਜ਼ਿਲ_Dialog_X(ਪੂਰਨ ਅੰਕ_Pixels)
{
    ਵਾਪਸ (ਸ਼ਬਦ_2)((ਅੰਕ_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ਸਥਿਰ ਸ਼ਬਦ_2
Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(ਪੂਰਨ ਅੰਕ_Pixels)
{
    ਵਾਪਸ (ਸ਼ਬਦ_2)((ਅੰਕ_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ਸਥਿਰ ਪੂਰਨ
Get_ਪਾਠ_ਚੌੜਾਈ(HDC hdc, char_u *str, ਪੂਰਨ len)
{
    ਮਾਪ_3    ਮਾਪ;

    Get_ਪਾਠ_Extent_ਬਿੰਦੂ(hdc, (LPCSTR)str, len, &ਮਾਪ);
    ਵਾਪਸ ਮਾਪ.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ਸਥਿਰ ਪੂਰਨ
Get_ਪਾਠ_ਚੌੜਾਈ_Enc(HDC hdc, char_u *str, ਪੂਰਨ len)
{
    ਮਾਪ_3	ਮਾਪ;
    WCHAR	*wstr;
    ਪੂਰਨ		n;
    ਪੂਰਨ		wlen = len;

    wstr = enc_ਮੰਜ਼ਿਲ_utf16(str, &wlen);
    ਜੇ (wstr == ਨਿਰਾ)
	ਵਾਪਸ 0;

    n = Get_ਪਾਠ_Extent_ਬਿੰਦੂ_W(hdc, wstr, wlen, &ਮਾਪ);
    vim_free(wstr);
    ਜੇ (n)
	ਵਾਪਸ ਮਾਪ.cx;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ਸਥਿਰ BOOL
Center_ਖਿੜਕੀ(
    HWND hwnd_ਬੱਚਾ,
    HWND hwnd_ਮਾਪਾ)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    r_ਬੱਚਾ, r_ਮਾਪਾ, rScreen;
    ਪੂਰਨ		    w_ਬੱਚਾ, h_ਬੱਚਾ, w_ਮਾਪਾ, h_ਮਾਪਾ;
    ਪੂਰਨ		    xNew, yNew;
    HDC		    hdc;

    Get_ਖਿੜਕੀ_Rect(hwnd_ਬੱਚਾ, &r_ਬੱਚਾ);
    w_ਬੱਚਾ = r_ਬੱਚਾ.ਸੱਜਾ - r_ਬੱਚਾ.ਖੱਬਾ;
    h_ਬੱਚਾ = r_ਬੱਚਾ.bottom - r_ਬੱਚਾ.ਸਿਖਰ;

    // If Vim is minimized put the window in the middle of the screen.
    ਜੇ (hwnd_ਮਾਪਾ == ਨਿਰਾ || IsMinimized(hwnd_ਮਾਪਾ))
	get_work_area(&r_ਮਾਪਾ);
    ਨਹੀਂਤਾਂ
	Get_ਖਿੜਕੀ_Rect(hwnd_ਮਾਪਾ, &r_ਮਾਪਾ);
    w_ਮਾਪਾ = r_ਮਾਪਾ.ਸੱਜਾ - r_ਮਾਪਾ.ਖੱਬਾ;
    h_ਮਾਪਾ = r_ਮਾਪਾ.bottom - r_ਮਾਪਾ.ਸਿਖਰ;

    moninfo.cb_ਮਾਪ = ਆਕਾਰ(MONITORINFO);
    mon = Monitor_From_ਖਿੜਕੀ(hwnd_ਬੱਚਾ, MONITOR_DEFAULTTOPRIMARY);
    ਜੇ (mon != ਨਿਰਾ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    ਨਹੀਂਤਾਂ
    {
	hdc = GetDC(hwnd_ਬੱਚਾ);
	rScreen.ਖੱਬਾ = 0;
	rScreen.ਸਿਖਰ = 0;
	rScreen.ਸੱਜਾ = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwnd_ਬੱਚਾ, hdc);
    }

    xNew = r_ਮਾਪਾ.ਖੱਬਾ + ((w_ਮਾਪਾ - w_ਬੱਚਾ) / 2);
    ਜੇ (xNew < rScreen.ਖੱਬਾ)
	xNew = rScreen.ਖੱਬਾ;
    ਨਹੀਂਤਾਂ ਜੇ ((xNew + w_ਬੱਚਾ) > rScreen.ਸੱਜਾ)
	xNew = rScreen.ਸੱਜਾ - w_ਬੱਚਾ;

    yNew = r_ਮਾਪਾ.ਸਿਖਰ + ((h_ਮਾਪਾ - h_ਬੱਚਾ) / 2);
    ਜੇ (yNew < rScreen.ਸਿਖਰ)
	yNew = rScreen.ਸਿਖਰ;
    ਨਹੀਂਤਾਂ ਜੇ ((yNew + h_ਬੱਚਾ) > rScreen.bottom)
	yNew = rScreen.bottom - h_ਬੱਚਾ;

    ਵਾਪਸ ਸੈੱਟ_ਖਿੜਕੀ_Pos(hwnd_ਬੱਚਾ, ਨਿਰਾ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG

#ਜੇ defined(FEAT_TOOLBAR)
    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    ਜੇ (s_toolbarhwnd == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (showit)
    {
	// Enable unicode support
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	Show_ਖਿੜਕੀ(s_toolbarhwnd, SW_SHOW);
    }
    ਨਹੀਂਤਾਂ
	Show_ਖਿੜਕੀ(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_ਗਿਣਤੀ 31

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_tabline_popup_menu_ਇੰਦਰਾਜ(HMENU pmenu, UINT ਵਸਤੂ_id, char_u *ਵਸਤੂ_ਪਾਠ)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_ਮੰਜ਼ਿਲ_utf16(ਵਸਤੂ_ਪਾਠ, ਨਿਰਾ);
    ਜੇ (wn == ਨਿਰਾ)
	ਵਾਪਸ;

    infow.cb_ਮਾਪ = ਆਕਾਰ(infow);
    infow.f_ਨਕਾਬ = MIIM_TYPE | MIIM_ID;
    infow.wID = ਵਸਤੂ_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_ਅੰਕੜੇ = wn;
    infow.cch = (UINT)wcslen(wn);
    ਪਾਓ_Menu_ਵਸਤੂ_W(pmenu, ਵਸਤੂ_id, FALSE, &infow);
    vim_free(wn);
}

    ਸਥਿਰ ਖਾਲੀ
show_tabline_popup_menu(ਖਾਲੀ)
{
    HMENU	    tab_pmenu;
    ਲੰਮਾ	    rval;
    ਬਿੰਦੂ_2	    pt;

    // When ignoring events don't show the menu.
    ਜੇ (hold_gui_events || cmdwin_type != 0)
	ਵਾਪਸ;

    tab_pmenu = CreatePopupMenu();
    ਜੇ (tab_pmenu == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (ਪਹਿਲਾ_tabpage->tp_ਅਗਲਾ != ਨਿਰਾ)
	ਜੋੜੋ_tabline_popup_menu_ਇੰਦਰਾਜ(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    ਜੋੜੋ_tabline_popup_menu_ਇੰਦਰਾਜ(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    ਜੋੜੋ_tabline_popup_menu_ਇੰਦਰਾਜ(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ਨਿਰਾ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    ਜੇ (rval > 0)
    {
	TCHITTESTINFO htinfo;
	ਪੂਰਨ idx;

	ਜੇ (Screen_ਮੰਜ਼ਿਲ_Client(s_tabhwnd, &pt) == 0)
	    ਵਾਪਸ;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = Tab_Ctrl_ਪ੍ਰਾਪਤੀ_Test(s_tabhwnd, &htinfo);
	ਜੇ (idx == -1)
	    idx = 0;
	ਨਹੀਂਤਾਂ
	    idx += 1;

	send_tabline_menu_event(idx, (ਪੂਰਨ)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    ਖਾਲੀ
gui_mch_show_tabline(ਪੂਰਨ showit)
{
    ਜੇ (s_tabhwnd == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (!showit != !showing_tabline)
    {
	ਜੇ (showit)
	    Show_ਖਿੜਕੀ(s_tabhwnd, SW_SHOW);
	ਨਹੀਂਤਾਂ
	    Show_ਖਿੜਕੀ(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ਪੂਰਨ
gui_mch_showing_tabline(ਖਾਲੀ)
{
    ਵਾਪਸ s_tabhwnd != ਨਿਰਾ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ਖਾਲੀ
gui_mch_update_tabline(ਖਾਲੀ)
{
    tabpage_T	*tp;
    TCITEM	tie;
    ਪੂਰਨ		nr = 0;
    ਪੂਰਨ		curtabidx = 0;
    ਪੂਰਨ		tabadded = 0;
    WCHAR	*wstr = ਨਿਰਾ;

    ਜੇ (s_tabhwnd == ਨਿਰਾ)
	ਵਾਪਸ;

    // Enable unicode support
    Send_ਸੁਨੇਹਾ(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.ਨਕਾਬ = TCIF_ਪਾਠ;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    Send_ਸੁਨੇਹਾ(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, ++nr)
    {
	ਜੇ (tp == curtab)
	    curtabidx = nr;

	ਜੇ (nr >= Tab_Ctrl_Get_ਵਸਤੂ_ਗਿਣਤੀ(s_tabhwnd))
	{
	    // Add the tab
	    tie.psz_ਪਾਠ = "-Empty-";
	    Tab_Ctrl_ਪਾਓ_ਵਸਤੂ(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.psz_ਪਾਠ = (LPSTR)NameBuff;

	wstr = enc_ਮੰਜ਼ਿਲ_utf16(NameBuff, ਨਿਰਾ);
	ਜੇ (wstr != ਨਿਰਾ)
	{
	    TCITEMW		tiw;

	    tiw.ਨਕਾਬ = TCIF_ਪਾਠ;
	    tiw.iImage = -1;
	    tiw.psz_ਪਾਠ = wstr;
	    Send_ਸੁਨੇਹਾ(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    ਜਦੋਂ (nr < Tab_Ctrl_Get_ਵਸਤੂ_ਗਿਣਤੀ(s_tabhwnd))
	Tab_Ctrl_Delete_ਵਸਤੂ(s_tabhwnd, nr);

    ਜੇ (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_ਸੈੱਟ_Cur_Sel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    Send_ਸੁਨੇਹਾ(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    Redraw_ਖਿੜਕੀ(s_tabhwnd, ਨਿਰਾ, ਨਿਰਾ,
		    RDW_ERASE | RDW_ਚੌਖਟਾ | RDW_INVALIDATE | RDW_ALLCHILDREN);

    ਜੇ (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_ਸੈੱਟ_Cur_Sel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_curtab(ਪੂਰਨ nr)
{
    ਜੇ (s_tabhwnd == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	Tab_Ctrl_ਸੈੱਟ_Cur_Sel(s_tabhwnd, nr - 1);
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_dark_theme(ਪੂਰਨ dark)
{
    ਜੇ (p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute != ਨਿਰਾ && win_ਸੰਸਕਰਣ >= MAKE_VER(10, 0, 18985))
	p_Dwm_ਸੈੱਟ_ਖਿੜਕੀ_Attribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_ਬਹੁਲਕ, &dark,
		ਆਕਾਰ(dark));

    ਜੇ (p_ਸੈੱਟ_Preferred_App_ਬਹੁਲਕ != ਨਿਰਾ && win_ਸੰਸਕਰਣ >= MAKE_VER(10, 0, 18362))
	p_ਸੈੱਟ_Preferred_App_ਬਹੁਲਕ(dark);

    ਜੇ (pFlushMenuThemes != ਨਿਰਾ && win_ਸੰਸਕਰਣ >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ਸਥਿਰ ਖਾਲੀ
dyn_uxtheme_ਲੱਦੋ(ਖਾਲੀ)
{
    hUxThemeLib = vim_ਲੱਦੋ_Lib("uxtheme.dll");
    ਜੇ (hUxThemeLib == ਨਿਰਾ)
	ਵਾਪਸ;

    p_ਸੈੱਟ_Preferred_App_ਬਹੁਲਕ = (DWORD (WINAPI *)(DWORD))
	Get_Proc_ਪਤਾ(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (ਖਾਲੀ (WINAPI *)(ਖਾਲੀ))
	Get_Proc_ਪਤਾ(hUxThemeLib, MAKEINTRESOURCE(136));

    ਜੇ (p_ਸੈੱਟ_Preferred_App_ਬਹੁਲਕ == ਨਿਰਾ || pFlushMenuThemes == ਨਿਰਾ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ਨਿਰਾ;
	ਵਾਪਸ;
    }
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fullscreen(ਪੂਰਨ flag)
{
    ਸਥਿਰ RECT ਸਾਧਾਰਨ_rect;
    ਸਥਿਰ LONG_PTR ਸਾਧਾਰਨ_style, ਸਾਧਾਰਨ_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    ਜੇ (!ਭਰਿਆ_screen) // Windows not set yet.
	ਵਾਪਸ;

    ਜੇ (flag)
    {
	ਜੇ (fullscreen_on)
	    ਵਾਪਸ;

	// Enter fullscreen mode
	Get_ਖਿੜਕੀ_Rect(s_hwnd, &rc);

	moninfo.cb_ਮਾਪ = ਆਕਾਰ(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	ਜੇ (mon == ਨਿਰਾ || !GetMonitorInfo(mon, &moninfo))
	    ਵਾਪਸ;

	// Save current window state
	Get_ਖਿੜਕੀ_Rect(s_hwnd, &ਸਾਧਾਰਨ_rect);
	ਸਾਧਾਰਨ_style = Get_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_STYLE);
	ਸਾਧਾਰਨ_exstyle = Get_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	ਸੈੱਟ_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_STYLE,
		ਸਾਧਾਰਨ_style & ~(WS_CAPTION | WS_THICKFRAME));
	ਸੈੱਟ_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_EXSTYLE,
		ਸਾਧਾਰਨ_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	ਸੈੱਟ_ਖਿੜਕੀ_Pos(s_hwnd, ਨਿਰਾ,
		moninfo.rcMonitor.ਖੱਬਾ,
		moninfo.rcMonitor.ਸਿਖਰ,
		moninfo.rcMonitor.ਸੱਜਾ - moninfo.rcMonitor.ਖੱਬਾ,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.ਸਿਖਰ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!fullscreen_on)
	    ਵਾਪਸ;

	// Exit fullscreen mode
	ਸੈੱਟ_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_STYLE, ਸਾਧਾਰਨ_style);
	ਸੈੱਟ_ਖਿੜਕੀ_Long_Ptr(s_hwnd, GWL_EXSTYLE, ਸਾਧਾਰਨ_exstyle);

	// Restore original window position and size
	ਸੈੱਟ_ਖਿੜਕੀ_Pos(s_hwnd, ਨਿਰਾ,
		ਸਾਧਾਰਨ_rect.ਖੱਬਾ,
		ਸਾਧਾਰਨ_rect.ਸਿਖਰ,
		ਸਾਧਾਰਨ_rect.ਸੱਜਾ - ਸਾਧਾਰਨ_rect.ਖੱਬਾ,
		ਸਾਧਾਰਨ_rect.bottom - ਸਾਧਾਰਨ_rect.ਸਿਖਰ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = FALSE;
    }
}

/*
 * ":simalt" command.
 */
    ਖਾਲੀ
ex_simalt(exarg_T *eap)
{
    char_u	*ਕੁੰਜੀਆਂ = eap->arg;
    ਪੂਰਨ		fill_typebuf = FALSE;
    char_u	ਕੁੰਜੀ_name[4];

    Post_ਸੁਨੇਹਾ(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    ਜਦੋਂ (*ਕੁੰਜੀਆਂ)
    {
	ਜੇ (*ਕੁੰਜੀਆਂ == '~')
	    *ਕੁੰਜੀਆਂ = ' ';	    // for showing system menu
	Post_ਸੁਨੇਹਾ(s_hwnd, WM_CHAR, (WPARAM)*ਕੁੰਜੀਆਂ, (LPARAM)0);
	ਕੁੰਜੀਆਂ++;
	fill_typebuf = TRUE;
    }
    ਜੇ (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	ਕੁੰਜੀ_name[0] = K_SPECIAL;
	ਕੁੰਜੀ_name[1] = KS_EXTRA;
	ਕੁੰਜੀ_name[2] = KE_NOP;
	ਕੁੰਜੀ_name[3] = NUL;
#ਜੇ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#ਜੇ_ਅੰਤ
	(ਖਾਲੀ)ins_typebuf(ਕੁੰਜੀ_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    ਸਥਿਰ ਖਾਲੀ
initialise_findrep(char_u *initial_string)
{
    ਪੂਰਨ		wword = FALSE;
    ਪੂਰਨ		mcase = !p_ic;
    char_u	*ਇੰਦਰਾਜ_ਪਾਠ;

    // Get the search string to use.
    ਇੰਦਰਾਜ_ਪਾਠ = get_ਲੱਭੋ_dialog_ਪਾਠ(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_ਥੱਲੇ;
    ਜੇ (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    ਜੇ (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ && *ਇੰਦਰਾਜ_ਪਾਠ != NUL)
    {
	WCHAR *p = enc_ਮੰਜ਼ਿਲ_utf16(ਇੰਦਰਾਜ_ਪਾਠ, ਨਿਰਾ);
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਪੂਰਨ len = s_findrep_struct.w_ਲੱਭੋ_What_Len - 1;

	    wcsncpy(s_findrep_struct.lpstr_ਲੱਭੋ_What, p, len);
	    s_findrep_struct.lpstr_ਲੱਭੋ_What[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(ਇੰਦਰਾਜ_ਪਾਠ);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਖਿੜਕੀ_title(HWND hwnd, ਅੱਖਰ *title)
{
    ਜੇ (title != ਨਿਰਾ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_ਮੰਜ਼ਿਲ_utf16((char_u *)title, ਨਿਰਾ);
	ਜੇ (wbuf != ਨਿਰਾ)
	{
	    ਸੈੱਟ_ਖਿੜਕੀ_ਪਾਠ_W(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)ਸੈੱਟ_ਖਿੜਕੀ_ਪਾਠ_W(hwnd, ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_ਲੱਭੋ_dialog(exarg_T *eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    ਜੇ (s_findrep_ਸੁਨੇਹਾ != 0)
    {
	ਜੇ (Is_ਖਿੜਕੀ(s_findrep_hwnd) && !s_findrep_is_ਲੱਭੋ)
	    Destroy_ਖਿੜਕੀ(s_findrep_hwnd);

	ਜੇ (!Is_ਖਿੜਕੀ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ਲੱਭੋ_ਪਾਠ_W(&s_findrep_struct);
	}

	ਸੈੱਟ_ਖਿੜਕੀ_title(s_findrep_hwnd, _("Find string"));
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_findrep_hwnd);

	s_findrep_is_ਲੱਭੋ = TRUE;
    }
#ਜੇ_ਅੰਤ
}


    ਖਾਲੀ
gui_mch_replace_dialog(exarg_T *eap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    ਜੇ (s_findrep_ਸੁਨੇਹਾ != 0)
    {
	ਜੇ (Is_ਖਿੜਕੀ(s_findrep_hwnd) && s_findrep_is_ਲੱਭੋ)
	    Destroy_ਖਿੜਕੀ(s_findrep_hwnd);

	ਜੇ (!Is_ਖਿੜਕੀ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = Replace_ਪਾਠ_W(&s_findrep_struct);
	}

	ਸੈੱਟ_ਖਿੜਕੀ_title(s_findrep_hwnd, _("Find & Replace"));
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_findrep_hwnd);

	s_findrep_is_ਲੱਭੋ = FALSE;
    }
#ਜੇ_ਅੰਤ
}


/*
 * Set visibility of the pointer.
 */
    ਖਾਲੀ
gui_mch_mousehide(ਪੂਰਨ hide)
{
    ਜੇ (hide == gui.ਸੂਚਕ_hidden)
	ਵਾਪਸ;

    ShowCursor(!hide);
    gui.ਸੂਚਕ_hidden = hide;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਸਥਿਰ ਖਾਲੀ
gui_mch_show_popupmenu_at(vimmenu_T *menu, ਪੂਰਨ x, ਪੂਰਨ y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (ਖਾਲੀ)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(ਪੂਰਨ)0,	    //reserved param
	s_hwnd,
	ਨਿਰਾ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#ਜੇ_ਅੰਤ

/*
 * Got a message when the system will go down.
 */
    ਸਥਿਰ ਖਾਲੀ
On_ਸਿਰਾ_Session(ਖਾਲੀ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ਸਥਿਰ ਖਾਲੀ
_OnClose(HWND hwnd ਅਣਵਰਤਿਆ)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ਸਥਿਰ ਖਾਲੀ
_OnDestroy(HWND hwnd)
{
    ਜੇ (!destroying)
	_OnClose(hwnd);
}

    ਸਥਿਰ ਖਾਲੀ
_OnPaint(
    HWND hwnd)
{
    ਜੇ (IsMinimized(hwnd))
	ਵਾਪਸ;

    PAINTSTRUCT ps;

    ਬਾਹਰਲਾ_flush();	    // make sure all output has been processed
    (ਖਾਲੀ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    ਜੇ (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ਖੱਬਾ = rect.ਖੱਬਾ;
	ps.rcPaint.ਸੱਜਾ = rect.ਸੱਜਾ;
    }

    ਜੇ (!Is_Rect_ਖਾਲੀ_ਥਾਵਾਂ(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	ਜੇ (!screen_cleared || Screen_ਸਤਰਾਂ == ਨਿਰਾ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	ਨਹੀਂਤਾਂ
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ਖੱਬਾ, ps.rcPaint.ਸਿਖਰ,
		ps.rcPaint.ਸੱਜਾ - ps.rcPaint.ਖੱਬਾ + 1,
		ps.rcPaint.bottom - ps.rcPaint.ਸਿਖਰ + 1);
    }

    ਸਿਰਾ_Paint(hwnd, &ps);
}

    ਸਥਿਰ ਖਾਲੀ
On_ਮਾਪ(
    HWND hwnd,
    UINT state ਅਣਵਰਤਿਆ,
    ਪੂਰਨ cx,
    ਪੂਰਨ cy)
{
    ਜੇ (!IsMinimized(hwnd) && !s_ਵਿੱਚ_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_ਉਚਾਈ(TRUE);
    }
}

    ਸਥਿਰ ਖਾਲੀ
On_ਸੈੱਟ_Focus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (ਖਾਲੀ)Def_ਖਿੜਕੀ_Proc_W(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ਸਥਿਰ ਖਾਲੀ
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    ਜੇ (destroying)
	ਵਾਪਸ;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (ਖਾਲੀ)Def_ਖਿੜਕੀ_Proc_W(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
}

/*
 * Get a message when the user switches back to vim
 */
    ਸਥਿਰ LRESULT
_OnActivateApp(
    HWND hwnd,
    BOOL fActivate,
    DWORD dwThreadId)
{
    // we call gui_focus_change() in _OnSetFocus()
    // gui_focus_change((int)fActivate);
    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    ਖਾਲੀ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    Destroy_ਖਿੜਕੀ(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    RECT rct;
    ਬਿੰਦੂ_2 mp;

    (ਖਾਲੀ)Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rct);
    (ਖਾਲੀ)GetCursorPos(&mp);
    *x = (ਪੂਰਨ)(mp.x - rct.ਖੱਬਾ);
    *y = (ਪੂਰਨ)(mp.y - rct.ਸਿਖਰ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x, ਪੂਰਨ y)
{
    RECT rct;

    (ਖਾਲੀ)Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rct);
    (ਖਾਲੀ)ਸੈੱਟ_Cursor_Pos(x + gui.border_ਵਿੱਥ + rct.ਖੱਬਾ,
		       y + gui.border_ਵਿੱਥ + rct.ਸਿਖਰ);
}

    ਸਥਿਰ ਖਾਲੀ
gui_mswin_get_valid_dimensions(
    ਪੂਰਨ w,
    ਪੂਰਨ h,
    ਪੂਰਨ *valid_w,
    ਪੂਰਨ *valid_h,
    ਪੂਰਨ *ਖਾਨੇ_ਗਿਣਤੀ,
    ਪੂਰਨ *ਪੰਕਤੀਆਂ_2)
{
    ਪੂਰਨ	    ਅਧਾਰ_ਚੌੜਾਈ, ਅਧਾਰ_ਉਚਾਈ;

    ਅਧਾਰ_ਚੌੜਾਈ = gui_get_ਅਧਾਰ_ਚੌੜਾਈ()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    ਅਧਾਰ_ਉਚਾਈ = gui_get_ਅਧਾਰ_ਉਚਾਈ()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_ਉਚਾਈ(FALSE);
    *ਖਾਨੇ_ਗਿਣਤੀ = (w - ਅਧਾਰ_ਚੌੜਾਈ) / gui.char_ਚੌੜਾਈ;
    *ਪੰਕਤੀਆਂ_2 = (h - ਅਧਾਰ_ਉਚਾਈ) / gui.char_ਉਚਾਈ;
    *valid_w = ਅਧਾਰ_ਚੌੜਾਈ + *ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ;
    *valid_h = ਅਧਾਰ_ਉਚਾਈ + *ਪੰਕਤੀਆਂ_2 * gui.char_ਉਚਾਈ;
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
    RECT    rc;

#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ਖੱਬਾ = 0;
    rc.ਸਿਖਰ = 0;
    rc.ਸੱਜਾ = gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ;
    rc.bottom = gui.ਅੰਕ_ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((ਲੰਮਾ)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ਸਥਿਰ BOOL
is_ਬਿੰਦੂ_onscreen(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਬਿੰਦੂ_2   pt = {x, y};

    ਵਾਪਸ Monitor_From_ਬਿੰਦੂ(pt, MONITOR_DEFAULTTONULL) != ਨਿਰਾ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ਸਥਿਰ BOOL
is_ਖਿੜਕੀ_onscreen(HWND hwnd)
{
    RECT    rc;
    ਬਿੰਦੂ_2   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ਖੱਬਾ;
    p1.y = rc.ਸਿਖਰ;
    p2.x = rc.ਸੱਜਾ - 1;
    p2.y = rc.bottom - 1;
    Client_ਮੰਜ਼ਿਲ_Screen(hwnd, &p1);
    Client_ਮੰਜ਼ਿਲ_Screen(hwnd, &p2);

    ਜੇ (!is_ਬਿੰਦੂ_onscreen(p1.x, p1.y))
	ਵਾਪਸ FALSE;
    ਜੇ (!is_ਬਿੰਦੂ_onscreen(p1.x, p2.y))
	ਵਾਪਸ FALSE;
    ਜੇ (!is_ਬਿੰਦੂ_onscreen(p2.x, p1.y))
	ਵਾਪਸ FALSE;
    ਜੇ (!is_ਬਿੰਦੂ_onscreen(p2.x, p2.y))
	ਵਾਪਸ FALSE;
    ਵਾਪਸ TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ਸਥਿਰ ਪੂਰਨ
get_scroll_flags(ਖਾਲੀ)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    ਜੇ (!is_ਖਿੜਕੀ_onscreen(s_hwnd))
	ਵਾਪਸ SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rcVim);
    ਲਈ (hwnd = s_hwnd; (hwnd = Get_ਖਿੜਕੀ(hwnd, GW_HWNDPREV)) != (HWND)0; )
	ਜੇ (Is_ਖਿੜਕੀ_Visible(hwnd))
	{
	    Get_ਖਿੜਕੀ_Rect(hwnd, &rcOther);
	    ਜੇ (IntersectRect(&rcDest, &rcVim, &rcOther))
		ਵਾਪਸ SW_INVALIDATE;
	}
    ਵਾਪਸ 0;
}

/*
 * On some Intel GPUs, the regions drawn just prior to ScrollWindowEx()
 * may not be scrolled out properly.
 * For gVim, when _OnScroll() is repeated, the character at the
 * previous cursor position may be left drawn after scroll.
 * The problem can be avoided by calling GetPixel() to get a pixel in
 * the region before ScrollWindowEx().
 */
    ਸਥਿਰ ਖਾਲੀ
intel_gpu_workaround(ਖਾਲੀ)
{
    GetPixel(s_hdc, FILL_X(gui.ਖਾਨਾ), FILL_Y(gui.ਪੰਕਤੀ));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(
    ਪੂਰਨ	    ਪੰਕਤੀ,
    ਪੂਰਨ	    ਅੰਕ_ਸਤਰਾਂ)
{
    RECT	rc;

    rc.ਖੱਬਾ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    rc.ਸੱਜਾ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1);
    rc.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX() && is_ਖਿੜਕੀ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ, &rc);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(FEAT_DIRECTX)
	ਜੇ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ
	intel_gpu_workaround();
	Scroll_ਖਿੜਕੀ_Ex(s_ਪਾਠ_Area, 0, -ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ,
				    &rc, &rc, ਨਿਰਾ, ਨਿਰਾ, get_scroll_flags());
	Update_ਖਿੜਕੀ(s_ਪਾਠ_Area);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_ਖੰਡ(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1,
						       gui.scroll_region_ਖੱਬਾ,
	gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਅੰਕ_ਸਤਰਾਂ)
{
    RECT	rc;

    rc.ਖੱਬਾ = FILL_X(gui.scroll_region_ਖੱਬਾ);
    rc.ਸੱਜਾ = FILL_X(gui.scroll_region_ਸੱਜਾ + 1);
    rc.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX() && is_ਖਿੜਕੀ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ, &rc);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(FEAT_DIRECTX)
	ਜੇ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	Scroll_ਖਿੜਕੀ_Ex(s_ਪਾਠ_Area, 0, ਅੰਕ_ਸਤਰਾਂ * gui.char_ਉਚਾਈ,
				    &rc, &rc, ਨਿਰਾ, ਨਿਰਾ, get_scroll_flags());
	Update_ਖਿੜਕੀ(s_ਪਾਠ_Area);
    }

    gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
				ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);
}


    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ rc ਅਣਵਰਤਿਆ)
{
#ਜੇ defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

    ReleaseDC(s_ਪਾਠ_Area, s_hdc);
    DeleteObject(s_brush);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TEAROFF
    // Unload the tearoff bitmap
    (ਖਾਲੀ)DeleteObject((HGDIOBJ)s_htearbitmap);
#ਜੇ_ਅੰਤ

    // Destroy our window (if we have one).
    ਜੇ (s_hwnd != ਨਿਰਾ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	Destroy_ਖਿੜਕੀ(s_hwnd);
    }
}

    ਸਥਿਰ char_u *
logfont2name(LOGFONTW lf)
{
    ਅੱਖਰ	*charset_name;
    ਅੱਖਰ	*quality_name;
    ਅੱਖਰ	*font_name;
    size_t	res_ਮਾਪ;
    ਅੱਖਰ	*res;

    font_name = (ਅੱਖਰ *)utf16_ਮੰਜ਼ਿਲ_enc(lf.lfFaceName, ਨਿਰਾ);
    ਜੇ (font_name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    charset_name = charset_id2name((ਪੂਰਨ)lf.lf_Char_ਸੈੱਟ);
    quality_name = quality_id2name((ਪੂਰਨ)lf.lfQuality);

    res_ਮਾਪ = STRLEN(font_name) + 30
		    + (charset_name == ਨਿਰਾ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ਨਿਰਾ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_ਮਾਪ);
    ਜੇ (res != ਨਿਰਾ)
    {
	ਅੱਖਰ	*p;
	ਪੂਰਨ	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	ਲਈ (p = font_name; *p != NUL; ++p)
	{
	    ਜੇ (ਖਾਲੀ_ਥਾਂ_ਹੈ(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_ਮੰਜ਼ਿਲ_points(
			 lf.lf_ਉਚਾਈ < 0 ? -lf.lf_ਉਚਾਈ : lf.lf_ਉਚਾਈ, TRUE);
	ਜੇ (lf.lf_ਭਾਰ == FW_ਸਾਧਾਰਨ || lf.lf_ਭਾਰ == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(ਅੱਖਰ *)res, res_ਮਾਪ, "%s:h%d", font_name, points);
	ਨਹੀਂਤਾਂ
	    res_len = vim_snprintf_safelen(
		(ਅੱਖਰ *)res, res_ਮਾਪ, "%s:h%d:W%ld", font_name, points, lf.lf_ਭਾਰ);

	res_len += vim_snprintf_safelen(
	    (ਅੱਖਰ *)res + res_len,
	    res_ਮਾਪ - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lf_ਭਾਰ == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lf_Strike_ਬਾਹਰਲਾ ? ":s" : "");

	ਜੇ (charset_name != ਨਿਰਾ)
	    res_len += vim_snprintf_safelen((ਅੱਖਰ *)res + res_len,
		res_ਮਾਪ - res_len, ":c%s", charset_name);
	ਜੇ (quality_name != ਨਿਰਾ)
	    vim_snprintf((ਅੱਖਰ *)res + res_len,
		res_ਮਾਪ - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ਵਾਪਸ (char_u *)res;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ਸਥਿਰ ਖਾਲੀ
update_im_font(ਖਾਲੀ)
{
    LOGFONTW	lf_wide, lf;

    ਜੇ (p_guifontwide != ਨਿਰਾ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, ਆਕਾਰ(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    ਨਹੀਂਤਾਂ
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    ਜੇ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lf_ਉਚਾਈ = lf.lf_ਉਚਾਈ * DEFAULT_DPI / s_dpi;
    im_ਸੈੱਟ_font(&lf);
}
#ਜੇ_ਅੰਤ

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    ਖਾਲੀ
gui_mch_wide_font_ਬਦਲਿਆ(ਖਾਲੀ)
{
    LOGFONTW lf;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    update_im_font();
#ਜੇ_ਅੰਤ

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    ਜੇ (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, ਆਕਾਰ(lf), &lf))
    {
	ਜੇ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	ਜੇ (lf.lf_ਭਾਰ < FW_BOLD)
	{
	    lf.lf_ਭਾਰ = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    ਜੇ (!lf.lfItalic)
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
    ਪੂਰਨ
gui_mch_init_font(char_u *font_name, ਪੂਰਨ fontset ਅਣਵਰਤਿਆ)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    ਜੇ (get_logfont(&lf, font_name, ਨਿਰਾ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lf_ਉਚਾਈ = adjust_fontsize_by_dpi(lf.lf_ਉਚਾਈ);
	font = get_font_handle(&lf);
    }
    ਜੇ (font == NOFONT)
	ਵਾਪਸ FAIL;

#ਜੇ defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	ਪੂਰਨ		    feat_ਗਿਣਤੀ = 0;
	char_u		    *fp;

	ਜੇ (font_name != ਨਿਰਾ)
	{
	    // Find each ":f" option in font_name.
	    ਲਈ (fp = font_name; *fp != NUL; fp++)
	    {
		ਜੇ (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u ਟੈਗ[5];
		    ਪੂਰਨ    ti = 0;
		    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ param = 1;

		    fp += 2;  // skip ":f"
		    ਜਦੋਂ (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			ਟੈਗ[ti++] = *fp++;
		    ਟੈਗ[ti] = NUL;

		    ਜੇ (ti != 4)
			ਜਾਰੀ;  // invalid tag length

		    ਜੇ (*fp == '=')
		    {
			fp++;
			param = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)atoi((ਅੱਖਰ *)fp);
			ਜਦੋਂ (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    ਜੇ (feat_ਗਿਣਤੀ < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_ਗਿਣਤੀ].ਟੈਗ =
			    ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਟੈਗ[0])
			    | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਟੈਗ[1] << 8)
			    | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਟੈਗ[2] << 16)
			    | ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਟੈਗ[3] << 24);
			features[feat_ਗਿਣਤੀ].parameter = param;
			feat_ਗਿਣਤੀ++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWrite_Context_ਸੈੱਟ_Font_Features(s_dwc, features, feat_ਗਿਣਤੀ);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (font_name == ਨਿਰਾ)
	font_name = (char_u *)"";
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    ਜੇ (!s_ਵਿੱਚ_dpichanged)
	update_im_font();
#ਜੇ_ਅੰਤ
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_ਉਚਾਈ = lfOrig.lf_ਉਚਾਈ;
    Update_Font_ਮਾਪ(font);

    p = logfont2name(lfOrig);
    ਜੇ (p != ਨਿਰਾ)
    {
	hl_ਸੈੱਟ_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	ਜੇ (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	ਨਹੀਂਤਾਂ
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    ਜੇ (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    ਜੇ (lf.lf_ਭਾਰ < FW_BOLD)
    {
	lf.lf_ਭਾਰ = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	ਜੇ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    ਪੂਰਨ
gui_mch_maximized(ਖਾਲੀ)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.ਲੰਮਾਈ = ਆਕਾਰ(WINDOWPLACEMENT);
    ਜੇ (Get_ਖਿੜਕੀ_Placement(s_hwnd, &wp))
    {
	ਜੇ (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ਵਾਪਸ TRUE;
	ਜੇ (wp.showCmd == SW_SHOWMINIMIZED)
	    ਵਾਪਸ FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	Get_ਖਿੜਕੀ_Rect(s_hwnd, &rc);
	ਜੇ ((rc.ਸੱਜਾ - rc.ਖੱਬਾ !=
		    wp.rc_ਸਾਧਾਰਨ_Position.ਸੱਜਾ - wp.rc_ਸਾਧਾਰਨ_Position.ਖੱਬਾ)
		|| (rc.bottom - rc.ਸਿਖਰ !=
		    wp.rc_ਸਾਧਾਰਨ_Position.bottom - wp.rc_ਸਾਧਾਰਨ_Position.ਸਿਖਰ))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ਖਾਲੀ
gui_mch_newfont(ਖਾਲੀ)
{
    RECT	rect;

    Get_ਖਿੜਕੀ_Rect(s_hwnd, &rect);
    ਜੇ (win_socket_id == 0)
    {
	gui_resize_shell(rect.ਸੱਜਾ - rect.ਖੱਬਾ
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.ਸਿਖਰ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_ਉਚਾਈ(FALSE));
    }
    ਨਹੀਂਤਾਂ
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ਸੱਜਾ - rect.ਖੱਬਾ,
	    rect.bottom - rect.ਸਿਖਰ - gui_mswin_get_menu_ਉਚਾਈ(FALSE));
    }
}

/*
 * Set the window title
 */
    ਖਾਲੀ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon ਅਣਵਰਤਿਆ)
{
    ਸੈੱਟ_ਖਿੜਕੀ_title(s_hwnd, (title == ਨਿਰਾ ? "VIM" : (ਅੱਖਰ *)title));
}

#ਜੇ defined(FEAT_MOUSESHAPE)
// Table for shape IDCs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ਸਥਿਰ LPCSTR mshape_idcs[] =
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

    ਖਾਲੀ
mch_ਸੈੱਟ_mouse_shape(ਪੂਰਨ shape)
{
    LPCSTR idc;

    ਜੇ (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	ਨਹੀਂਤਾਂ
	    idc = mshape_idcs[shape];
	ਸੈੱਟ_Class_Long_Ptr(s_ਪਾਠ_Area, GCLP_HCURSOR, (LONG_PTR)ਲੱਦੋ_Cursor(ਨਿਰਾ, idc));
	ਜੇ (!p_mh)
	{
	    ਬਿੰਦੂ_2 mp;

	    // Set the position to make it redrawn with the new shape.
	    (ਖਾਲੀ)GetCursorPos(&mp);
	    (ਖਾਲੀ)ਸੈੱਟ_Cursor_Pos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ਸਥਿਰ WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    ਪੂਰਨ len;
    WCHAR *res;

    tmp = convert_filter(s);
    ਜੇ (tmp == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    len = (ਪੂਰਨ)STRLEN(s) + 3;
    res = enc_ਮੰਜ਼ਿਲ_utf16(tmp, &len);
    vim_free(tmp);
    ਵਾਪਸ res;
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
	ਪੂਰਨ saving,
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
    ਪੂਰਨ			i;
    WCHAR		*titlep = ਨਿਰਾ;
    WCHAR		*extp = ਨਿਰਾ;
    WCHAR		*initdirp = ਨਿਰਾ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    ਜੇ (dflt == ਨਿਰਾ)
	fileBuf[0] = NUL;
    ਨਹੀਂਤਾਂ
    {
	wp = enc_ਮੰਜ਼ਿਲ_utf16(dflt, ਨਿਰਾ);
	ਜੇ (wp == ਨਿਰਾ)
	    fileBuf[0] = NUL;
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_ਖੇਤਰ(fileStruct);
# ifdef OPENFILENAME_ਮਾਪ_ਸੰਸਕਰਣ_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_ਮਾਪ = OPENFILENAME_ਮਾਪ_ਸੰਸਕਰਣ_400_W;
# ਨਹੀਂਤਾਂ
    fileStruct.l_Struct_ਮਾਪ = ਆਕਾਰ(fileStruct);
# endif

    ਜੇ (title != ਨਿਰਾ)
	titlep = enc_ਮੰਜ਼ਿਲ_utf16(title, ਨਿਰਾ);
    fileStruct.lpstrTitle = titlep;

    ਜੇ (ext != ਨਿਰਾ)
	extp = enc_ਮੰਜ਼ਿਲ_utf16(ext, ਨਿਰਾ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    ਜੇ (initdir != ਨਿਰਾ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_ਮੰਜ਼ਿਲ_utf16(initdir, ਨਿਰਾ);
	ਜੇ (initdirp != ਨਿਰਾ)
	{
	    ਲਈ (wp = initdirp; *wp != NUL; ++wp)
		ਜੇ (*wp == '/')
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
    ਜੇ (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    ਜੇ (saving)
	ret = GetSaveFileNameW(&fileStruct);
    ਨਹੀਂਤਾਂ
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    ਜੇ (!ret)
	ਵਾਪਸ ਨਿਰਾ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_ਮੰਜ਼ਿਲ_enc(fileBuf, ਨਿਰਾ);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Give focus back to main window (when using MDI).
    ਸੈੱਟ_Focus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ਵਾਪਸ q;
}


/*
 * Convert the string s to the proper format for a filter string by replacing
 * the \t and \n delimiters with \0.
 * Returns the converted string in allocated memory.
 *
 * Keep in sync with convert_filterW() above!
 */
    ਸਥਿਰ char_u *
convert_filter(char_u *s)
{
    char_u	*res;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	s_len = (ਬਿਨਾਨਿਸ਼ਾਨੀ)STRLEN(s);
    ਬਿਨਾਨਿਸ਼ਾਨੀ	i;

    res = alloc(s_len + 3);
    ਜੇ (res != ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < s_len; ++i)
	    ਜੇ (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    ਨਹੀਂਤਾਂ
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ਵਾਪਸ res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ਵਾਪਸ gui_mch_browse(0, title, (char_u *)_("Not Used"), ਨਿਰਾ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#ਜੇ_ਅੰਤ // FEAT_BROWSE

    ਸਥਿਰ ਖਾਲੀ
_OnDropFiles(
    HWND hwnd ਅਣਵਰਤਿਆ,
    HDROP hDrop)
{
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUFPATHLEN MAX_ਰਸਤਾ_2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    ਅੱਖਰ    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ਨਿਰਾ, 0);
    UINT    i;
    char_u  **fnames;
    ਬਿੰਦੂ_2   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    Drag_Query_ਬਿੰਦੂ(hDrop, &pt);
    Map_ਖਿੜਕੀ_Points(s_hwnd, s_ਪਾਠ_Area, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    ਜੇ (fnames != ਨਿਰਾ)
	ਲਈ (i = 0; i < cFiles; ++i)
	{
	    ਜੇ (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_ਮੰਜ਼ਿਲ_enc(wszFile, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    Drag_ਅੰਤ(hDrop);

    ਜੇ (fnames == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ kbd_modifiers = get_ਸਰਗਰਮ_modifiers();

    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) != 0)
	modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
_OnScroll(
    HWND hwnd ਅਣਵਰਤਿਆ,
    HWND hwndCtl,
    UINT ਕੋਡ,
    ਪੂਰਨ pos)
{
    ਸਥਿਰ UINT	prev_ਕੋਡ = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    ਲੰਮਾ	val;
    ਪੂਰਨ		dragging = FALSE;
    ਪੂਰਨ		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_ਮਾਪ = ਆਕਾਰ(si);
    si.f_ਨਕਾਬ = SIF_POS;

    sb = gui_mswin_ਲੱਭੋ_scrollbar(hwndCtl);
    ਜੇ (sb == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਜੇ (sb->wp != ਨਿਰਾ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    ਨਹੀਂਤਾਂ	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->ਮੁੱਲ;

    ਚੋਣ (ਕੋਡ)
    {
	ਹਾਲਤ SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    ਜੇ (sb->scroll_ਖਿਸਕਾਓ > 0)
		val <<= sb->scroll_ਖਿਸਕਾਓ;
	    ਰੋਕੋ;
	ਹਾਲਤ SB_LINEDOWN:
	    val++;
	    ਰੋਕੋ;
	ਹਾਲਤ SB_LINEUP:
	    val--;
	    ਰੋਕੋ;
	ਹਾਲਤ SB_PAGEDOWN:
	    val += (sb_info->ਮਾਪ > 2 ? sb_info->ਮਾਪ - 2 : 1);
	    ਰੋਕੋ;
	ਹਾਲਤ SB_PAGEUP:
	    val -= (sb_info->ਮਾਪ > 2 ? sb_info->ਮਾਪ - 2 : 1);
	    ਰੋਕੋ;
	ਹਾਲਤ SB_ਸਿਖਰ:
	    val = 0;
	    ਰੋਕੋ;
	ਹਾਲਤ SB_BOTTOM:
	    val = sb_info->max;
	    ਰੋਕੋ;
	ਹਾਲਤ SB_ENDSCROLL:
	    ਜੇ (prev_ਕੋਡ == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		ਜੇ (sb->scroll_ਖਿਸਕਾਓ > 0)
		    val <<= sb->scroll_ਖਿਸਕਾਓ;
	    }
	    ਰੋਕੋ;

	ਮੂਲ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ਵਾਪਸ 0;
    }
    prev_ਕੋਡ = ਕੋਡ;

    si.nPos = (sb->scroll_ਖਿਸਕਾਓ > 0) ? val >> sb->scroll_ਖਿਸਕਾਓ : val;
    ਸੈੱਟ_Scroll_Info(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    ਜੇ (sb->wp != ਨਿਰਾ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ਖੱਬਾ) ? SBAR_ਸੱਜਾ : SBAR_ਖੱਬਾ].id;

	ਸੈੱਟ_Scroll_Info(id, SB_CTL, &si, TRUE);
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

    ਵਾਪਸ 0;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XPM_W32
# include "xpm_w32.h"
#ਜੇ_ਅੰਤ


// Some parameters for tearoff menus.  All in pixels.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAROFF_PADDING_X	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAROFF_BUTTON_PAD_X	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAROFF_MIN_ਚੌੜਾਈ	200
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAROFF_SUBMENU_LABEL	">>"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEAROFF_ਕਾਲਮ_PADDING	3	// # spaces to pad column with.


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_ਪਾਠ_LEN	    MAXPATHL

ਸਥਿਰ BalloonEval  *cur_beval = ਨਿਰਾ;
ਸਥਿਰ UINT_PTR	    beval_timer_id = 0;
ਸਥਿਰ DWORD	    ਆਖਰੀ_user_activity = 0;
#ਜੇ_ਅੰਤ // defined(FEAT_BEVAL_GUI)


// Local variables:

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
ਸਥਿਰ UINT	s_menu_id = 100;
#ਜੇ_ਅੰਤ

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_SYSMENU_FONT

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_NAME	"vim"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLG_ALLOC_ਮਾਪ 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
ਸਥਿਰ PWORD
ਜੋੜੋ_dialog_element(
	PWORD p,
	DWORD lStyle,
	ਸ਼ਬਦ_2 x,
	ਸ਼ਬਦ_2 y,
	ਸ਼ਬਦ_2 w,
	ਸ਼ਬਦ_2 h,
	ਸ਼ਬਦ_2 Id,
	ਸ਼ਬਦ_2 clss,
	ਅਚਲ ਅੱਖਰ *caption);
ਸਥਿਰ LPWORD lpwAlign(LPWORD);
ਸਥਿਰ ਪੂਰਨ n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(LPWORD, LPSTR, BOOL);
#ਜੇ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ਸਥਿਰ ਖਾਲੀ gui_mch_tearoff(char_u *title, vimmenu_T *menu, ਪੂਰਨ initX, ਪੂਰਨ initY);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ get_dialog_font_metrics(ਖਾਲੀ);

ਸਥਿਰ ਪੂਰਨ dialog_default_button = -1;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
ਸਥਿਰ ਖਾਲੀ initialise_toolbar(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ update_toolbar_ਮਾਪ(ਖਾਲੀ);
ਸਥਿਰ LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_ਸੁਨੇਹਾ, WPARAM wParam, LPARAM lParam);
ਸਥਿਰ ਪੂਰਨ get_toolbar_bitmap(vimmenu_T *menu);
#ਨਹੀਂਤਾਂ
# define update_toolbar_ਮਾਪ()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
ਸਥਿਰ ਖਾਲੀ initialise_tabline(ਖਾਲੀ);
ਸਥਿਰ LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_ਸੁਨੇਹਾ, WPARAM wParam, LPARAM lParam);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
ਸਥਿਰ LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ਸਥਿਰ char_u *Get_ਨਤੀਜਾ_Str(HWND hwnd, ਪੂਰਨ GCS, ਪੂਰਨ *lenp);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ਟੈਗ_COMPOSITIONFORM {
    DWORD dwStyle;
    ਬਿੰਦੂ_2 ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ HANDLE HIMC;
# endif

ਸਥਿਰ HINSTANCE hLibImm = ਨਿਰਾ;
ਸਥਿਰ LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ਸਥਿਰ HIMC (WINAPI *pImmGetContext)(HWND);
ਸਥਿਰ HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ਸਥਿਰ BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ਸਥਿਰ BOOL (WINAPI *p_Imm_Get_Open_ਸਥਿਤੀ)(HIMC);
ਸਥਿਰ BOOL (WINAPI *p_Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ)(HIMC, BOOL);
ਸਥਿਰ BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ਸਥਿਰ BOOL (WINAPI *p_Imm_ਸੈੱਟ_Composition_Font_W)(HIMC, LPLOGFONTW);
ਸਥਿਰ BOOL (WINAPI *p_Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ)(HIMC, LPCOMPOSITIONFORM);
ਸਥਿਰ BOOL (WINAPI *p_Imm_Get_Conversion_ਸਥਿਤੀ)(HIMC, LPDWORD, LPDWORD);
ਸਥਿਰ BOOL (WINAPI *p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ)(HIMC, DWORD, DWORD);
ਸਥਿਰ ਖਾਲੀ dyn_imm_ਲੱਦੋ(ਖਾਲੀ);
#ਨਹੀਂਤਾਂ
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define p_Imm_Get_Open_ਸਥਿਤੀ	  Imm_Get_Open_ਸਥਿਤੀ
# define p_Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ	  Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define p_Imm_ਸੈੱਟ_Composition_Font_W  Imm_ਸੈੱਟ_Composition_Font_W
# define p_Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ
# define p_Imm_Get_Conversion_ਸਥਿਤੀ  Imm_Get_Conversion_ਸਥਿਤੀ
# define p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ  Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_mswin_get_menu_ਉਚਾਈ(
    ਪੂਰਨ	    fix_ਖਿੜਕੀ)	    // If TRUE, resize window if menu height changed
{
    ਸਥਿਰ ਪੂਰਨ	old_menu_ਉਚਾਈ = -1;

    RECT    rc1, rc2;
    ਪੂਰਨ	    ਅੰਕ;
    ਪੂਰਨ	    menu_ਉਚਾਈ;

    ਜੇ (gui.menu_is_ਸਰਗਰਮ)
	ਅੰਕ = Get_Menu_ਵਸਤੂ_ਗਿਣਤੀ(s_menuBar);
    ਨਹੀਂਤਾਂ
	ਅੰਕ = 0;

    ਜੇ (ਅੰਕ == 0)
	menu_ਉਚਾਈ = 0;
    ਨਹੀਂਤਾਂ ਜੇ (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_ਉਚਾਈ = old_menu_ਉਚਾਈ == -1 ? 0 : old_menu_ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	Get_Menu_ਵਸਤੂ_Rect(s_hwnd, s_menuBar, 0, &rc1);
	ਜੇ (gui.starting)
	    menu_ਉਚਾਈ = rc1.bottom - rc1.ਸਿਖਰ + 1;
	ਨਹੀਂਤਾਂ
	{
	    Get_Menu_ਵਸਤੂ_Rect(s_hwnd, s_menuBar, ਅੰਕ - 1, &rc2);
	    menu_ਉਚਾਈ = rc2.bottom - rc1.ਸਿਖਰ + 1;
	}
    }

    ਜੇ (fix_ਖਿੜਕੀ && menu_ਉਚਾਈ != old_menu_ਉਚਾਈ)
	gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_ਉਚਾਈ = menu_ਉਚਾਈ;

    ਵਾਪਸ menu_ਉਚਾਈ;
}
#ਜੇ_ਅੰਤ // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ਸਥਿਰ ਲੰਮਾ
mouse_vertical_scroll_ਕਦਮ(ਖਾਲੀ)
{
    UINT val;
    ਜੇ (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ਵਾਪਸ (val != WHEEL_PAGESCROLL) ? (ਲੰਮਾ)val : -1;
    ਵਾਪਸ 3; // Safe default;
}

    ਸਥਿਰ ਲੰਮਾ
mouse_horizontal_scroll_ਕਦਮ(ਖਾਲੀ)
{
    UINT val;
    ਜੇ (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ਵਾਪਸ (ਲੰਮਾ)val;
    ਵਾਪਸ 3; // Safe default;
}

    ਸਥਿਰ ਖਾਲੀ
init_mouse_wheel(ਖਾਲੀ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(mouse_vertical_scroll_ਕਦਮ());
    mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(mouse_horizontal_scroll_ਕਦਮ());
}

/*
 * Mouse scroll event handler.
 */
    ਸਥਿਰ ਖਾਲੀ
_OnMouseWheel(HWND hwnd ਅਣਵਰਤਿਆ, WPARAM wParam, LPARAM lParam, ਪੂਰਨ horizontal)
{
    ਪੂਰਨ		button;
    win_T	*wp;
    ਪੂਰਨ		modifiers = 0;
    ਪੂਰਨ		kbd_modifiers;
    ਪੂਰਨ		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    ਬਿੰਦੂ_2	pt;

    wp = gui_mouse_ਖਿੜਕੀ(ਲੱਭੋ_POPUP);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROP_POPUP
    ਜੇ (wp != ਨਿਰਾ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_ਪੰਕਤੀ = wp->w_winrow;
	mouse_ਖਾਨਾ = wp->w_wincol;
	CLEAR_ਖੇਤਰ(cap);
	ਜੇ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ਖੱਬਾ : MSCR_ਸੱਜਾ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ਨਹੀਂਤਾਂ
	{
	    cap.arg = zDelta < 0 ? MSCR_ਉੱਪਰ : MSCR_ਥੱਲੇ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	ਬਾਹਰਲਾ_flush();
    }
#ਜੇ_ਅੰਤ

    ਜੇ (wp == ਨਿਰਾ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    ਜੇ (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    ਨਹੀਂਤਾਂ
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_ਸਰਗਰਮ_modifiers();

    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ) != 0)
	modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ਜੇ ((kbd_modifiers & MOD_ਨਕਾਬ_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    Screen_ਮੰਜ਼ਿਲ_Client(s_ਪਾਠ_Area, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_ਮਾਪ = ਆਕਾਰ(NONCLIENTMETRICSW);
    ਜੇ (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    ਆਕਾਰ(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ਵਾਪਸ FAIL;
    *lf = nm.lfMenuFont;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tabline_font(ਖਾਲੀ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    ਜੇ (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ਵਾਪਸ;

    lfSysmenu.lf_ਉਚਾਈ = adjust_fontsize_by_dpi(lfSysmenu.lf_ਉਚਾਈ);
    font = Create_Font_ਅਸਿੱਧਾ_W(&lfSysmenu);

    Send_ਸੁਨੇਹਾ(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = Get_Desktop_ਖਿੜਕੀ();
    hdc = Get_ਖਿੜਕੀ_DC(hwnd);
    hfntOld = SelectFont(hdc, font);

    Get_ਪਾਠ_Metrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_ਉਚਾਈ = tm.tm_ਉਚਾਈ + tm.tm_ਅੰਦਰੂਨੀ_Leading + 7;
}
#ਨਹੀਂਤਾਂ
# define ਸੈੱਟ_tabline_font()
#ਜੇ_ਅੰਤ

/*
 * Invoked when a setting was changed.
 */
    ਸਥਿਰ LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    ਚੋਣ (param)
    {
	ਹਾਲਤ SPI_SETWHEELSCROLLLINES:
	    mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(mouse_vertical_scroll_ਕਦਮ());
	    ਰੋਕੋ;
	ਹਾਲਤ SPI_SETWHEELSCROLLCHARS:
	    mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(mouse_horizontal_scroll_ਕਦਮ());
	    ਰੋਕੋ;
	ਹਾਲਤ SPI_SETNONCLIENTMETRICS:
	    ਸੈੱਟ_tabline_font();
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ 0;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    ਸਥਿਰ ਖਾਲੀ
On_ਖਿੜਕੀ_Pos_ਬਦਲਿਆ(
    HWND hwnd,
    ਅਚਲ LPWINDOWPOS lpwpos)
{
    ਸਥਿਰ ਪੂਰਨ x = 0, y = 0, cx = 0, cy = 0;
    ਬਾਹਰੀ ਪੂਰਨ WSInitialized;

    ਜੇ (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_ਚੌਖਟਾ_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, Def_ਖਿੜਕੀ_Proc_W);
}
#ਜੇ_ਅੰਤ


ਸਥਿਰ HWND hwndTip = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
show_sizing_tip(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਪੂਰਨ ਪੰਕਤੀਆਂ_2)
{
    TOOLINFOA ti;
    ਅੱਖਰ buf[32];

    ti.cb_ਮਾਪ = ਆਕਾਰ(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpsz_ਪਾਠ = buf;
    sprintf(buf, "%dx%d", ਖਾਨੇ_ਗਿਣਤੀ, ਪੰਕਤੀਆਂ_2);
    ਜੇ (hwndTip == ਨਿਰਾ)
    {
	hwndTip = Create_ਖਿੜਕੀ_Ex_A(0, TOOLTIPS_CLASSA, ਨਿਰਾ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ਨਿਰਾ, GetModuleHandle(ਨਿਰਾ), ਨਿਰਾ);
	Send_ਸੁਨੇਹਾ(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	Send_ਸੁਨੇਹਾ(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    ਨਹੀਂਤਾਂ
    {
	Send_ਸੁਨੇਹਾ(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    Send_ਸੁਨੇਹਾ(hwndTip, TTM_POPUP, 0, 0);
}

    ਸਥਿਰ ਖਾਲੀ
destroy_sizing_tip(ਖਾਲੀ)
{
    ਜੇ (hwndTip == ਨਿਰਾ)
	ਵਾਪਸ;

    Destroy_ਖਿੜਕੀ(hwndTip);
    hwndTip = ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    ਪੂਰਨ	    w, h;
    ਪੂਰਨ	    valid_w, valid_h;
    ਪੂਰਨ	    w_ਵਿੱਥ, h_ਵਿੱਥ;
    ਪੂਰਨ	    ਖਾਨੇ_ਗਿਣਤੀ, ਪੰਕਤੀਆਂ_2;

    w = lprc->ਸੱਜਾ - lprc->ਖੱਬਾ;
    h = lprc->bottom - lprc->ਸਿਖਰ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &ਖਾਨੇ_ਗਿਣਤੀ, &ਪੰਕਤੀਆਂ_2);
    w_ਵਿੱਥ = w - valid_w;
    h_ਵਿੱਥ = h - valid_h;

    ਜੇ (fwSide == WMSZ_ਖੱਬਾ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ਖੱਬਾ += w_ਵਿੱਥ;
    ਨਹੀਂਤਾਂ ਜੇ (fwSide == WMSZ_ਸੱਜਾ || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ਸੱਜਾ -= w_ਵਿੱਥ;

    ਜੇ (fwSide == WMSZ_ਸਿਖਰ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->ਸਿਖਰ += h_ਵਿੱਥ;
    ਨਹੀਂਤਾਂ ਜੇ (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_ਵਿੱਥ;

    show_sizing_tip(ਖਾਨੇ_ਗਿਣਤੀ, ਪੰਕਤੀਆਂ_2);
    ਵਾਪਸ TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਸਥਿਰ ਖਾਲੀ
On_RButton_ਉੱਪਰ(HWND hwnd, ਪੂਰਨ x, ਪੂਰਨ y, UINT ਕੁੰਜੀ_Flags)
{
    ਜੇ (gui_mch_showing_tabline())
    {
	ਬਿੰਦੂ_2 pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rect);
	ਜੇ (pt.y < rect.ਸਿਖਰ)
	{
	    show_tabline_popup_menu();
	    ਵਾਪਸ;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, ਕੁੰਜੀ_Flags, Def_ਖਿੜਕੀ_Proc_W);
}

    ਸਥਿਰ ਖਾਲੀ
On_LButton_ਥੱਲੇ(HWND hwnd, BOOL fDoubleClick, ਪੂਰਨ x, ਪੂਰਨ y, UINT ਕੁੰਜੀ_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    ਜੇ (gui_mch_showing_tabline())
    {
	ਬਿੰਦੂ_2 pt;
	RECT rect;

	GetCursorPos(&pt);
	Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rect);
	ਜੇ (pt.y < rect.ਸਿਖਰ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, ਕੁੰਜੀ_Flags, Def_ਖਿੜਕੀ_Proc_W);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ UINT
_OnNCHitTest(HWND hwnd, ਪੂਰਨ xPos, ਪੂਰਨ yPos)
{
    UINT	ਨਤੀਜਾ;
    ਪੂਰਨ		x, y;

    ਨਤੀਜਾ = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, Def_ਖਿੜਕੀ_Proc_W);
    ਜੇ (ਨਤੀਜਾ != HTCLIENT)
	ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਜੇ (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	Get_ਖਿੜਕੀ_Rect(s_ਪਾਠ_Area, &rct);
	ਜੇ (yPos < rct.ਸਿਖਰ)
	    ਵਾਪਸ ਨਤੀਜਾ;
    }
#ਜੇ_ਅੰਤ
    (ਖਾਲੀ)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    ਜੇ (xPos < 48) // <VN> TODO should use system metric?
	ਵਾਪਸ HTBOTTOMLEFT;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ HTBOTTOMRIGHT;
}

#ਜੇ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    ਚੋਣ (hdr->ਕੋਡ)
    {
	ਹਾਲਤ TTN_GETDISPINFOW:
	ਹਾਲਤ TTN_GETDISPINFO:
	    {
		char_u		*str = ਨਿਰਾ;
		ਸਥਿਰ ਖਾਲੀ	*tt_ਪਾਠ = ਨਿਰਾ;

		VIM_CLEAR(tt_ਪਾਠ);

# ifdef FEAT_GUI_TABLINE
		ਜੇ (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    ਬਿੰਦੂ_2	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    ਜੇ (Screen_ਮੰਜ਼ਿਲ_Client(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			ਪੂਰਨ idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = Tab_Ctrl_ਪ੍ਰਾਪਤੀ_Test(s_tabhwnd, &htinfo);
			ਜੇ (idx != -1)
			{
			    tabpage_T *tp;

			    tp = ਲੱਭੋ_tabpage(idx + 1);
			    ਜੇ (tp != ਨਿਰਾ)
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
		ਨਹੀਂਤਾਂ
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_ਲੱਭੋ_menu(ਜੜ੍ਹ_menu, idButton);
		    ਜੇ (pMenu)
			str = pMenu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP];
		}
# endif
		ਜੇ (str == ਨਿਰਾ)
		    ਰੋਕੋ;

		// Set the maximum width, this also enables using \n for
		// line break.
		Send_ਸੁਨੇਹਾ(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		ਜੇ (hdr->ਕੋਡ == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_ਪਾਠ = enc_ਮੰਜ਼ਿਲ_utf16(str, ਨਿਰਾ);
		    lpdi->lpsz_ਪਾਠ = tt_ਪਾਠ;
		    // can't show tooltip if failed
		}
		ਨਹੀਂਤਾਂ
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    ਜੇ (len < ਆਕਾਰ(lpdi->sz_ਪਾਠ)
			    || ((tt_ਪਾਠ = vim_strnsave(str, len)) == ਨਿਰਾ))
			vim_strncpy((char_u *)lpdi->sz_ਪਾਠ, str,
				ਆਕਾਰ(lpdi->sz_ਪਾਠ) - 1);
		    ਨਹੀਂਤਾਂ
			lpdi->lpsz_ਪਾਠ = tt_ਪਾਠ;
		}
	    }
	    ਰੋਕੋ;

# ifdef FEAT_GUI_TABLINE
	ਹਾਲਤ TCN_SELCHANGE:
	    ਜੇ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ਵਾਪਸ 0L;
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ NM_RCLICK:
	    ਜੇ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ਵਾਪਸ 0L;
	    }
	    ਰੋਕੋ;
# endif

	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MENUHINTS) && defined(FEAT_MENU)
    ਸਥਿਰ LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ਜੇ (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & ਬਹੁਲਕ_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ਸਥਿਰ ਪੂਰਨ  did_menu_tip = FALSE;

	ਜੇ (did_menu_tip)
	{
	    ਸੁਨੇਹਾ_clr_cmdline();
	    setcursor();
	    ਬਾਹਰਲਾ_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_ਲੱਭੋ_menu(ਜੜ੍ਹ_menu, (UINT)LOWORD(wParam));
	ਜੇ (pMenu != ਨਿਰਾ && pMenu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP] != ਨਿਰਾ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_ਮਾਪ = ਆਕਾਰ(MENUITEMINFO);
	    menuinfo.f_ਨਕਾਬ = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    ਜੇ (Get_Menu_ਵਸਤੂ_Info(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++ਸੁਨੇਹਾ_hist_off;
		ਸੁਨੇਹਾ((ਅੱਖਰ *)pMenu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP]);
		--ਸੁਨੇਹਾ_hist_off;
		setcursor();
		ਬਾਹਰਲਾ_flush();
		did_menu_tip = TRUE;
	    }
	}
	ਵਾਪਸ 0L;
    }
    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, WM_MENUSELECT, wParam, lParam);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ BOOL
On_Get_Dpi_Scaled_ਮਾਪ(HWND hwnd ਅਣਵਰਤਿਆ, UINT dpi, ਮਾਪ_3 *ਮਾਪ)
{
    ਪੂਰਨ		old_ਚੌੜਾਈ, old_ਉਚਾਈ;
    ਪੂਰਨ		new_ਚੌੜਾਈ, new_ਉਚਾਈ;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_ਮਾਪ(gui.norm_font, &old_ਚੌੜਾਈ, &old_ਉਚਾਈ);    // Current size
    GetObjectW((HFONT)gui.norm_font, ਆਕਾਰ(lf), &lf);
    lf.lf_ਉਚਾਈ = lf.lf_ਉਚਾਈ * (ਪੂਰਨ)dpi / s_dpi;
    font = Create_Font_ਅਸਿੱਧਾ_W(&lf);
    ਜੇ (font)
    {
	Get_Font_ਮਾਪ((GuiFont)font, &new_ਚੌੜਾਈ, &new_ਉਚਾਈ);	// New size
	DeleteFont(font);
    }
    ਨਹੀਂਤਾਂ
    {
	new_ਚੌੜਾਈ = old_ਚੌੜਾਈ;
	new_ਉਚਾਈ = old_ਉਚਾਈ;
    }
    ਮਾਪ->cx = ਮਾਪ->cx * new_ਚੌੜਾਈ / old_ਚੌੜਾਈ;
    ਮਾਪ->cy = ਮਾਪ->cy * new_ਉਚਾਈ / old_ਉਚਾਈ;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ LRESULT
On_Dpi_ਬਦਲਿਆ(HWND hwnd, UINT xdpi ਅਣਵਰਤਿਆ, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_ਵਿੱਚ_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_ਮਾਪ();
    update_toolbar_ਮਾਪ();
    ਸੈੱਟ_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_ਉਚਾਈ(FALSE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    im_ਸੈੱਟ_position(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ);
#ਜੇ_ਅੰਤ
    InvalidateRect(hwnd, ਨਿਰਾ, TRUE);

    s_ਵਿੱਚ_dpichanged = FALSE;
    ਵਾਪਸ 0L;
}


    ਸਥਿਰ LRESULT CALLBACK
_WndProc(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ,
    WPARAM wParam,
    LPARAM lParam)
{
    // ch_log(NULL, "WndProc: hwnd = %08x, msg = %x, wParam = %x, lParam = %x",
	    // hwnd, uMsg, wParam, lParam);

    HandleMouseHide(u_ਸੁਨੇਹਾ, lParam);

    s_u_ਸੁਨੇਹਾ = u_ਸੁਨੇਹਾ;
    s_wParam = wParam;

    ਚੋਣ (u_ਸੁਨੇਹਾ)
    {
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_DEADCHAR,	_OnDeadChar);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_SYSDEADCHAR, _OnDeadChar);
	// HANDLE_MSG(hwnd, WM_ACTIVATE,    _OnActivate);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_CLOSE,	_OnClose);
	// HANDLE_MSG(hwnd, WM_COMMAND,	_OnCommand);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_DESTROY,	_OnDestroy);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_DROPFILES,	_OnDropFiles);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_HSCROLL,	_OnScroll);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_KILLFOCUS,	_OnKillFocus);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_COMMAND,	_OnMenu);
#ਜੇ_ਅੰਤ
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_SETFOCUS,	On_ਸੈੱਟ_Focus);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_ਮਾਪ,	On_ਮਾਪ);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_WINDOWPOSCHANGED, On_ਖਿੜਕੀ_Pos_ਬਦਲਿਆ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_RBUTTONUP,	On_RButton_ਉੱਪਰ);
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_LBUTTONDBLCLK,  On_LButton_ਥੱਲੇ);
#ਜੇ_ਅੰਤ
	HANDLE_ਸੁਨੇਹਾ(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    ਹਾਲਤ WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ਵਾਪਸ FALSE;		// Do NOT allow system to go down.

    ਹਾਲਤ WM_ENDSESSION:
	ਜੇ (wParam)	// system only really goes down when wParam is TRUE
	{
	    On_ਸਿਰਾ_Session();
	    ਵਾਪਸ 0L;
	}
	ਰੋਕੋ;

    ਹਾਲਤ WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (ਪੂਰਨ)(ਛੋਟਾ)LOWORD(lParam));
	ਵਾਪਸ 0L;

    ਹਾਲਤ WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਜੇ (	!gui.menu_is_ਸਰਗਰਮ
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((ਪੂਰਨ)wParam))
		)
#ਜੇ_ਅੰਤ
	{
	    _OnSysChar(hwnd, (UINT)wParam, (ਪੂਰਨ)(ਛੋਟਾ)LOWORD(lParam));
	    ਵਾਪਸ 0L;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
#ਜੇ_ਅੰਤ

    ਹਾਲਤ WM_SYSKEYUP:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ 0L;
#ਜੇ_ਅੰਤ

    ਹਾਲਤ WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	ਰੋਕੋ;

    ਹਾਲਤ WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ਵਾਪਸ _DuringSizing((UINT)wParam, (LPRECT)lParam);

    ਹਾਲਤ WM_MOUSEWHEEL:
    ਹਾਲਤ WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_ਸੁਨੇਹਾ == WM_MOUSEHWHEEL);
	ਵਾਪਸ 0L;

	// Notification for change in SystemParametersInfo()
    ਹਾਲਤ WM_SETTINGCHANGE:
	ਵਾਪਸ _OnSettingChange((UINT)wParam);

#ਜੇ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ਹਾਲਤ WM_NOTIFY:
	ਵਾਪਸ _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#ਜੇ_ਅੰਤ

#ਜੇ defined(MENUHINTS) && defined(FEAT_MENU)
    ਹਾਲਤ WM_MENUSELECT:
	ਵਾਪਸ _OnMenuSelect(hwnd, wParam, lParam);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    ਹਾਲਤ WM_IME_NOTIFY:
	ਜੇ (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
	ਵਾਪਸ 1L;

    ਹਾਲਤ WM_IME_COMPOSITION:
	ਜੇ (!_OnImeComposition(hwnd, wParam, lParam))
	    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
	ਵਾਪਸ 1L;
#ਜੇ_ਅੰਤ
    ਹਾਲਤ WM_GETDPISCALEDSIZE:
	ਵਾਪਸ On_Get_Dpi_Scaled_ਮਾਪ(hwnd, (UINT)wParam, (ਮਾਪ_3 *)lParam);
    ਹਾਲਤ WM_DPICHANGED:
	ਵਾਪਸ On_Dpi_ਬਦਲਿਆ(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    ਮੂਲ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
	ਜੇ (u_ਸੁਨੇਹਾ == s_findrep_ਸੁਨੇਹਾ && s_findrep_ਸੁਨੇਹਾ != 0)
	    On_ਲੱਭੋ_Repl();
#ਜੇ_ਅੰਤ
	ਰੋਕੋ;
    }

    ਵਾਪਸ Def_ਖਿੜਕੀ_Proc_W(hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_ਮਾਪਾ_hwnd = ਨਿਰਾ;

    ਸਥਿਰ BOOL CALLBACK
ਲੱਭੋ_ਖਿੜਕੀ_Title(HWND hwnd, LPARAM lParam)
{
    ਅੱਖਰ	buf[2048];
    ਅੱਖਰ	*title = (ਅੱਖਰ *)lParam;

    ਜੇ (Get_ਖਿੜਕੀ_ਪਾਠ(hwnd, buf, ਆਕਾਰ(buf)))
    {
	ਜੇ (ਲੜੀ_ਖੋਜ(buf, title) != ਨਿਰਾ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_ਮਾਪਾ_hwnd = ਲੱਭੋ_ਖਿੜਕੀ_Ex(hwnd, ਨਿਰਾ, "MDIClient", ਨਿਰਾ);
	    ਜੇ (vim_ਮਾਪਾ_hwnd != ਨਿਰਾ)
		ਵਾਪਸ FALSE;
	}
    }
    ਵਾਪਸ TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਮਾਪਾ(ਅੱਖਰ *title)
{
    EnumWindows(ਲੱਭੋ_ਖਿੜਕੀ_Title, (LPARAM)title);
    ਜੇ (vim_ਮਾਪਾ_hwnd == ਨਿਰਾ)
    {
	semsg(_(e_cannot_ਲੱਭੋ_ਖਿੜਕੀ_title_str), title);
	mch_exit(2);
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
    ਸਥਿਰ ਖਾਲੀ
ole_error(ਅੱਖਰ *arg)
{
    ਅੱਖਰ buf[IOSIZE];

# ifdef VIMDLL
    gui.ਵਿੱਚ_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_ਦਲੀਲ_not_supported_str_use_ole_ਸੰਸਕਰਣ), arg);
    mch_errmsg(buf);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(GUI_MAY_SPAWN)
    ਸਥਿਰ ਅੱਖਰ *
gvim_error(ਖਾਲੀ)
{
    ਅੱਖਰ *ਸੁਨੇਹਾ = _(e_gui_cannot_be_ਵਰਤਿਆ_cannot_execute_gvim_exe);

    ਜੇ (starting)
    {
	mch_errmsg(ਸੁਨੇਹਾ);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ਵਾਪਸ ਸੁਨੇਹਾ;
}

    ਅੱਖਰ *
gui_mch_do_spawn(char_u *arg)
{
    ਪੂਰਨ			len;
# ਜੇ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ਨਿਰਾ;
    LPWSTR		tofree1 = ਨਿਰਾ;
# endif
    WCHAR		name[MAX_ਰਸਤਾ];
    LPWSTR		cmd, newcmd = ਨਿਰਾ, p, warg, tofree2 = ਨਿਰਾ;
    STARTUPINFOW	si = {ਆਕਾਰ(si)};
    PROCESS_INFORMATION pi;

    ਜੇ (!GetModuleFileNameW(g_hinst, name, MAX_ਰਸਤਾ))
	ਜਾਓ error;
    p = wcsrchr(name, L'\\');
    ਜੇ (p == ਨਿਰਾ)
	ਜਾਓ error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# ਨਹੀਂਤਾਂ
    wcscpy(p + 1, L"gvim.exe");
# endif

# ਜੇ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ਜੇ (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_ਸਤਰ_W();
	// Skip 1st argument.
	ਜਦੋਂ (*p && *p != L' ' && *p != L'\t')
	{
	    ਜੇ (*p == L'"')
	    {
		// Skip quoted strings
		ਜਦੋਂ (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# ਜੇ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ਨਹੀਂਤਾਂ
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	ਪੂਰਨ	ret;

	session = vim_tempname('s', FALSE);
	ਜੇ (session == ਨਿਰਾ)
	    ਜਾਓ error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	ਜੇ (p_bg == ਨਿਰਾ)
	{
	    p_bg = savebg;
	    ਜਾਓ error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	ਜੇ (!ret)
	    ਜਾਓ error;
	wsession = enc_ਮੰਜ਼ਿਲ_utf16(session, ਨਿਰਾ);
	ਜੇ (wsession == ਨਿਰਾ)
	    ਜਾਓ error;
	len = (ਪੂਰਨ)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	ਜੇ (cmd == ਨਿਰਾ)
	{
	    vim_free(wsession);
	    ਜਾਓ error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    ਜੇ (arg != ਨਿਰਾ)
    {
	warg = enc_ਮੰਜ਼ਿਲ_utf16(arg, ਨਿਰਾ);
	ਜੇ (warg == ਨਿਰਾ)
	    ਜਾਓ error;
	tofree2 = warg;
    }
    ਨਹੀਂਤਾਂ
	warg = L"";

    // Set up the new command line.
    len = (ਪੂਰਨ)wcslen(name) + (ਪੂਰਨ)wcslen(cmd) + (ਪੂਰਨ)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    ਜੇ (newcmd == ਨਿਰਾ)
	ਜਾਓ error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    ਜੇ (!CreateProcessW(ਨਿਰਾ, newcmd, ਨਿਰਾ, ਨਿਰਾ, TRUE, 0,
		ਨਿਰਾ, ਨਿਰਾ, &si, &pi))
	ਜਾਓ error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# ਜੇ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ਜੇ (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ਵਾਪਸ gvim_error();
}
#ਜੇ_ਅੰਤ

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    ਖਾਲੀ
gui_mch_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    ਪੂਰਨ		silent = FALSE;
    ਪੂਰਨ		idx;

    // Check for special OLE command line parameters
    ਜੇ ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	ਜੇ (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	ਨਹੀਂਤਾਂ
	    idx = 1;

	// Register Vim as an OLE Automation server
	ਜੇ (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
	    ਰਜਿਸਟਰ_Me(silent);
	    mch_exit(0);
#ਨਹੀਂਤਾਂ
	    ਜੇ (!silent)
		ole_error("register");
	    mch_exit(2);
#ਜੇ_ਅੰਤ
	}

	// Unregister Vim as an OLE Automation server
	ਜੇ (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#ਨਹੀਂਤਾਂ
	    ਜੇ (!silent)
		ole_error("unregister");
	    mch_exit(2);
#ਜੇ_ਅੰਤ
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	ਜੇ (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
	    *argc = 1;
#ਨਹੀਂਤਾਂ
	    ole_error("embedding");
	    mch_exit(2);
#ਜੇ_ਅੰਤ
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_OLE
# ifdef VIMDLL
    ਜੇ (mch_is_gui_executable())
# endif
    {
	ਪੂਰਨ	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	ਜੇ (bDoRestart)
	    mch_exit(0);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	ਪੂਰਨ arg;

	ਲਈ (arg = 1; arg < *argc; arg++)
	    ਜੇ (ਲੜੀ_ਤੁਲਨਾ("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ਆਕਾਰ(ਅੱਖਰ *));
		argv[*argc] = ਨਿਰਾ;
		ਰੋਕੋ;	// enough?
	    }
    }
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਦੋ_dpi_func(ਖਾਲੀ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    ਜੇ (hUser32 == ਨਿਰਾ)
	ਜਾਓ fail;

    pGetDpiForSystem = (UINT (WINAPI *)(ਖਾਲੀ))Get_Proc_ਪਤਾ(hUser32, "GetDpiForSystem");
    p_Get_Dpi_For_ਖਿੜਕੀ = (UINT (WINAPI *)(HWND))Get_Proc_ਪਤਾ(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (ਪੂਰਨ (WINAPI *)(ਪੂਰਨ, UINT))Get_Proc_ਪਤਾ(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    p_ਸੈੱਟ_Thread_Dpi_Awareness_Context = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_ਪਤਾ(hUser32, "SetThreadDpiAwarenessContext");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_ਪਤਾ(hUser32, "GetAwarenessFromDpiAwarenessContext");
#ਜੇ_ਅੰਤ

    ਜੇ (p_ਸੈੱਟ_Thread_Dpi_Awareness_Context != ਨਿਰਾ)
    {
	DPI_AWARENESS_CONTEXT oldctx = p_ਸੈੱਟ_Thread_Dpi_Awareness_Context(
		DPI_AWARENESS_CONTEXT_ਪ੍ਰਤੀ_MONITOR_AWARE_V2);
	ਜੇ (oldctx != ਨਿਰਾ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    ਜੇ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#ਜੇ_ਅੰਤ
	    ਵਾਪਸ;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    p_Get_Dpi_For_ਖਿੜਕੀ = ਨਿਰਾ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    p_ਸੈੱਟ_Thread_Dpi_Awareness_Context = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ਪੂਰਨ
gui_mch_init(ਖਾਲੀ)
{
    ਅਚਲ WCHAR szVimWndClassW[] = VIM_CLASSW;
    ਅਚਲ WCHAR sz_ਪਾਠ_Area_Class_W[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    ਜੇ (s_hwnd != ਨਿਰਾ)
	ਜਾਓ theend;

    /*
     * Load the tearoff bitmap
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TEAROFF
    s_htearbitmap = ਲੱਦੋ_Bitmap(g_hinst, "IDB_TEAROFF");
#ਜੇ_ਅੰਤ

    ਲੱਦੋ_dpi_func();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DARKTHEME
    dyn_uxtheme_ਲੱਦੋ();
#ਜੇ_ਅੰਤ

    dyn_dwm_ਲੱਦੋ();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_ਮਾਪ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui.menu_ਉਚਾਈ = 0;	// Windows takes care of this
#ਜੇ_ਅੰਤ
    gui.border_ਚੌੜਾਈ = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    gui.toolbar_ਉਚਾਈ = TOOLBAR_BUTTON_ਉਚਾਈ + TOOLBAR_BORDER_ਉਚਾਈ;
#ਜੇ_ਅੰਤ

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    ਜੇ (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ਲੱਦੋ_Icon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = ਲੱਦੋ_Cursor(ਨਿਰਾ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ਨਿਰਾ;
	wndclassw.lpszClassName = szVimWndClassW;

	ਜੇ (ਰਜਿਸਟਰ_Class_W(&wndclassw) == 0)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (vim_ਮਾਪਾ_hwnd != ਨਿਰਾ)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TRY_EXCEPT
	__try
	{
#ਜੇ_ਅੰਤ
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = Create_ਖਿੜਕੀ_Ex_W(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_ਬੱਚਾ
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_ਮਾਪਾ_hwnd, ਨਿਰਾ,
		g_hinst, ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#ਜੇ_ਅੰਤ
	ਜੇ (s_hwnd == ਨਿਰਾ)
	{
	    emsg(_(e_unable_ਮੰਜ਼ਿਲ_open_ਖਿੜਕੀ_inside_mdi_application));
	    mch_exit(2);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	ਜੇ (Is_ਖਿੜਕੀ((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = Create_ਖਿੜਕੀ_W(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		ਨਿਰਾ, ਨਿਰਾ,
		g_hinst, ਨਿਰਾ);
	ਜੇ (s_hwnd != ਨਿਰਾ && win_socket_id != 0)
	{
	    ਸੈੱਟ_ਮਾਪਾ(s_hwnd, (HWND)win_socket_id);
	    Show_ਖਿੜਕੀ(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    ਜੇ (s_hwnd == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜੇ (p_Get_Dpi_For_ਖਿੜਕੀ != ਨਿਰਾ)
    {
	s_dpi = p_Get_Dpi_For_ਖਿੜਕੀ(s_hwnd);
	update_scrollbar_ਮਾਪ();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#ਜੇ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_ਲੱਦੋ();
#ਜੇ_ਅੰਤ

    // Create the text area window
    ਜੇ (GetClassInfoW(g_hinst, sz_ਪਾਠ_Area_Class_W, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = ਪਾਠ_Area_Wnd_Proc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ਨਿਰਾ;
	wndclassw.hCursor = ਲੱਦੋ_Cursor(ਨਿਰਾ, IDC_ARROW);
	wndclassw.hbrBackground = ਨਿਰਾ;
	wndclassw.lpszMenuName = ਨਿਰਾ;
	wndclassw.lpszClassName = sz_ਪਾਠ_Area_Class_W;

	ਜੇ (ਰਜਿਸਟਰ_Class_W(&wndclassw) == 0)
	    ਵਾਪਸ FAIL;
    }

    s_ਪਾਠ_Area = Create_ਖਿੜਕੀ_Ex_W(
	0,
	sz_ਪਾਠ_Area_Class_W, L"Vim text area",
	WS_ਬੱਚਾ | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ਨਿਰਾ,
	g_hinst, ਨਿਰਾ);

    ਜੇ (s_ਪਾਠ_Area == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ਨਿਰਾ;

	ਜੇ (mch_icon_ਲੱਦੋ(&hIcon) == OK && hIcon != ਨਿਰਾ)
	    Send_ਸੁਨੇਹਾ(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    s_menuBar = CreateMenu();
#ਜੇ_ਅੰਤ
    s_hdc = GetDC(s_ਪਾਠ_Area);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ਪਿੱਠ_pixel = gui.ਪਿੱਠ_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_ਵਿੱਥ = gui.border_ਚੌੜਾਈ;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN_ਲੱਭੋ_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_ਸੁਨੇਹਾ = ਰਜਿਸਟਰ_ਖਿੜਕੀ_ਸੁਨੇਹਾ(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_ਮਾਪ = ਆਕਾਰ(s_findrep_struct);
    s_findrep_struct.lpstr_ਲੱਭੋ_What = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstr_ਲੱਭੋ_What[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.w_ਲੱਭੋ_What_Len = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // set the v:windowid variable
    ਸੈੱਟ_vim_var_nr(VV_WINDOWID, Handle_ਮੰਜ਼ਿਲ_Long(s_hwnd));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RENDER_OPTIONS
    ਜੇ (p_rop)
	(ਖਾਲੀ)gui_mch_ਸੈੱਟ_rendering_options(p_rop);
#ਜੇ_ਅੰਤ

theend:
    // Display any pending error messages
    display_errors();

    ਵਾਪਸ OK;
}

/*
 * Get the size of the screen, taking position on multiple monitors into
 * account (if supported).
 */
    ਸਥਿਰ ਖਾਲੀ
get_work_area(RECT *spi_rect)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;

    // work out which monitor the window is on, and get *its* work area
    mon = Monitor_From_ਖਿੜਕੀ(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    ਜੇ (mon != ਨਿਰਾ)
    {
	moninfo.cb_ਮਾਪ = ਆਕਾਰ(MONITORINFO);
	ਜੇ (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ਵਾਪਸ;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(
	ਪੂਰਨ ਚੌੜਾਈ,
	ਪੂਰਨ ਉਚਾਈ,
	ਪੂਰਨ min_ਚੌੜਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ min_ਉਚਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ ਅਧਾਰ_ਚੌੜਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ ਅਧਾਰ_ਉਚਾਈ ਅਣਵਰਤਿਆ,
	ਪੂਰਨ direction)
{
    RECT	workarea_rect;
    RECT	ਖਿੜਕੀ_rect;
    ਪੂਰਨ		win_ਚੌੜਾਈ, win_ਉਚਾਈ;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    ਜੇ (IsZoomed(s_hwnd) && starting == 0)
	Show_ਖਿੜਕੀ(s_hwnd, SW_SHOWNORMAL);

    ਜੇ (s_ਵਿੱਚ_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	ਖਿੜਕੀ_rect = s_suggested_rect;
    ਨਹੀਂਤਾਂ
	// Use current position.
	Get_ਖਿੜਕੀ_Rect(s_hwnd, &ਖਿੜਕੀ_rect);

    // compute the size of the outside of the window
    win_ਚੌੜਾਈ = ਚੌੜਾਈ + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_ਉਚਾਈ = ਉਚਾਈ + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_ਉਚਾਈ(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    ਖਿੜਕੀ_rect.ਸੱਜਾ = ਖਿੜਕੀ_rect.ਖੱਬਾ + win_ਚੌੜਾਈ;
    ਖਿੜਕੀ_rect.bottom = ਖਿੜਕੀ_rect.ਸਿਖਰ + win_ਉਚਾਈ;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    ਜੇ (!s_ਵਿੱਚ_dpichanged)
    {
	ਜੇ ((direction & RESIZE_HOR)
		&& ਖਿੜਕੀ_rect.ਸੱਜਾ > workarea_rect.ਸੱਜਾ)
	    ਵਿੱਥ_Rect(&ਖਿੜਕੀ_rect,
		    workarea_rect.ਸੱਜਾ - ਖਿੜਕੀ_rect.ਸੱਜਾ, 0);

	ਜੇ ((direction & RESIZE_HOR)
		&& ਖਿੜਕੀ_rect.ਖੱਬਾ < workarea_rect.ਖੱਬਾ)
	    ਵਿੱਥ_Rect(&ਖਿੜਕੀ_rect,
		    workarea_rect.ਖੱਬਾ - ਖਿੜਕੀ_rect.ਖੱਬਾ, 0);

	ਜੇ ((direction & RESIZE_VERT)
		&& ਖਿੜਕੀ_rect.bottom > workarea_rect.bottom)
	    ਵਿੱਥ_Rect(&ਖਿੜਕੀ_rect,
		    0, workarea_rect.bottom - ਖਿੜਕੀ_rect.bottom);

	ਜੇ ((direction & RESIZE_VERT)
		&& ਖਿੜਕੀ_rect.ਸਿਖਰ < workarea_rect.ਸਿਖਰ)
	    ਵਿੱਥ_Rect(&ਖਿੜਕੀ_rect,
		    0, workarea_rect.ਸਿਖਰ - ਖਿੜਕੀ_rect.ਸਿਖਰ);
    }

    Move_ਖਿੜਕੀ(s_hwnd, ਖਿੜਕੀ_rect.ਖੱਬਾ, ਖਿੜਕੀ_rect.ਸਿਖਰ,
						win_ਚੌੜਾਈ, win_ਉਚਾਈ, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    ਸੈੱਟ_ਸਰਗਰਮ_ਖਿੜਕੀ(s_hwnd);
    ਸੈੱਟ_Focus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_ਉਚਾਈ(!gui.starting);
}


    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(
    scrollbar_T *sb,
    ਲੰਮਾ	val,
    ਲੰਮਾ	ਮਾਪ,
    ਲੰਮਾ	max)
{
    SCROLLINFO	info;

    sb->scroll_ਖਿਸਕਾਓ = 0;
    ਜਦੋਂ (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	ਮਾਪ >>= 1;
	++sb->scroll_ਖਿਸਕਾਓ;
    }

    ਜੇ (sb->scroll_ਖਿਸਕਾਓ > 0)
	++ਮਾਪ;

    info.cb_ਮਾਪ = ਆਕਾਰ(info);
    info.f_ਨਕਾਬ = SIF_POS | SIF_RANGE | SIF_ਪੰਨਾ;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.n_ਪੰਨਾ = ਮਾਪ;
    ਸੈੱਟ_Scroll_Info(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MBYTE_IME
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
    ਸਥਿਰ LRESULT
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    LRESULT l_ਨਤੀਜਾ = 0;
    HIMC hImc;

    ਜੇ (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ਵਾਪਸ l_ਨਤੀਜਾ;
    ਚੋਣ (dwCommand)
    {
	ਹਾਲਤ IMN_SETOPENSTATUS:
	    ਜੇ (p_Imm_Get_Open_ਸਥਿਤੀ(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		ਜੇ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lf_ਉਚਾਈ = lf.lf_ਉਚਾਈ * DEFAULT_DPI / s_dpi;
		p_Imm_ਸੈੱਟ_Composition_Font_W(hImc, &lf);
		im_ਸੈੱਟ_position(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ);

		// Disable langmap
		State &= ~ਬਹੁਲਕ_LANGMAP;
		ਜੇ (State & ਬਹੁਲਕ_ਪਾਓ)
		{
# ਜੇ defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    ਜੇ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			ਪੂਰਨ old_ਪੰਕਤੀ = gui.ਪੰਕਤੀ;
			ਪੂਰਨ old_ਖਾਨਾ = gui.ਖਾਨਾ;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			ਸਥਿਤੀ_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.ਪੰਕਤੀ = old_ਪੰਕਤੀ;
			gui.ਖਾਨਾ = old_ਖਾਨਾ;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_ਨਤੀਜਾ = 0;
	    ਰੋਕੋ;
    }
    pImmReleaseContext(hWnd, hImc);
    ਵਾਪਸ l_ਨਤੀਜਾ;
}

    ਸਥਿਰ LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs ਅਣਵਰਤਿਆ, LPARAM param)
{
    char_u	*ret;
    ਪੂਰਨ		len;

    ਜੇ ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ਵਾਪਸ 0;

    ret = Get_ਨਤੀਜਾ_Str(hwnd, GCS_RESULTSTR, &len);
    ਜੇ (ret != ਨਿਰਾ)
    {
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf_csi(ret, len);
	vim_free(ret);
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ਸਥਿਰ char_u *
Get_ਨਤੀਜਾ_Str(HWND hwnd, ਪੂਰਨ GCS, ਪੂਰਨ *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ਨਿਰਾ;
    char_u	*convbuf = ਨਿਰਾ;

    ਜੇ (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ਵਾਪਸ ਨਿਰਾ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ਨਿਰਾ, 0);
    ਜੇ (ret <= 0)
	ਵਾਪਸ ਨਿਰਾ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + ਆਕਾਰ(WCHAR));
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / ਆਕਾਰ(WCHAR);

    convbuf = utf16_ਮੰਜ਼ਿਲ_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ਵਾਪਸ convbuf;
}
#ਜੇ_ਅੰਤ

// For global functions we need prototypes.
#ਜੇ defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    ਖਾਲੀ
im_ਸੈੱਟ_font(LOGFONTW *lf)
{
    HIMC hImc;

    ਜੇ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	p_Imm_ਸੈੱਟ_Composition_Font_W(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    ਖਾਲੀ
im_ਸੈੱਟ_position(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ)
{
    HIMC hImc;

    ਜੇ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_ਬਿੰਦੂ;
	cfs.ptCurrentPos.x = FILL_X(ਖਾਨਾ);
	cfs.ptCurrentPos.y = FILL_Y(ਪੰਕਤੀ);
	Map_ਖਿੜਕੀ_Points(s_ਪਾਠ_Area, s_hwnd, &cfs.ptCurrentPos, 1);
	ਜੇ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	p_Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    ਖਾਲੀ
im_ਸੈੱਟ_ਸਰਗਰਮ(ਪੂਰਨ ਸਰਗਰਮ)
{
    HIMC	hImc;
    ਸਥਿਰ HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
    {
	mbyte_im_ਸੈੱਟ_ਸਰਗਰਮ(ਸਰਗਰਮ);
	ਵਾਪਸ;
    }
# endif

    ਜੇ (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ਵਾਪਸ;

    ਜੇ (p_imdisable)
    {
	ਜੇ (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    ਜੇ (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	ਸਰਗਰਮ = FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    ਜੇ (!hImc)
	ਵਾਪਸ;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    ਜੇ (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ਸਥਿਰ DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ਸਥਿਰ BOOL bSaved = FALSE;

	ਜੇ (ਸਰਗਰਮ)
	{
	    // if we have a saved conversion status, restore it
	    ਜੇ (bSaved)
		p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // save conversion status and disable korean
	    ਜੇ (p_Imm_Get_Conversion_ਸਥਿਤੀ(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    p_Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ(hImc, ਸਰਗਰਮ);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    ਪੂਰਨ
im_get_ਸਥਿਤੀ(ਖਾਲੀ)
{
    ਪੂਰਨ		ਸਥਿਤੀ = 0;
    HIMC	hImc;

# ifdef VIMDLL
    ਜੇ (!gui.ਵਿੱਚ_use && !gui.starting)
	ਵਾਪਸ mbyte_im_get_ਸਥਿਤੀ();
# endif

    ਜੇ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	ਸਥਿਤੀ = p_Imm_Get_Open_ਸਥਿਤੀ(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ਵਾਪਸ ਸਥਿਤੀ;
}

#ਜੇ_ਅੰਤ // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ਸਥਿਰ ਖਾਲੀ
latin9_ਮੰਜ਼ਿਲ_ucs(char_u *ਪਾਠ, ਪੂਰਨ len, WCHAR *unicodebuf)
{
    ਪੂਰਨ		c;

    ਜਦੋਂ (--len >= 0)
    {
	c = *ਪਾਠ++;
	ਚੋਣ (c)
	{
	    ਹਾਲਤ 0xa4: c = 0x20ac; ਰੋਕੋ;   // euro
	    ਹਾਲਤ 0xa6: c = 0x0160; ਰੋਕੋ;   // S hat
	    ਹਾਲਤ 0xa8: c = 0x0161; ਰੋਕੋ;   // S -hat
	    ਹਾਲਤ 0xb4: c = 0x017d; ਰੋਕੋ;   // Z hat
	    ਹਾਲਤ 0xb8: c = 0x017e; ਰੋਕੋ;   // Z -hat
	    ਹਾਲਤ 0xbc: c = 0x0152; ਰੋਕੋ;   // OE
	    ਹਾਲਤ 0xbd: c = 0x0153; ਰੋਕੋ;   // oe
	    ਹਾਲਤ 0xbe: c = 0x0178; ਰੋਕੋ;   // Y
	}
	*unicodebuf++ = c;
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
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
    ਸਥਿਰ ਖਾਲੀ
Rev_ਬਾਹਰਲਾ( HDC hdc,
	ਪੂਰਨ ਖਾਨਾ,
	ਪੂਰਨ ਪੰਕਤੀ,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR ਪਾਠ,
	UINT len,
	CONST INT *padding)
{
    ਪੂਰਨ		ix;

    ਲਈ (ix = 0; ix < (ਪੂਰਨ)len; ++ix)
	Ext_ਪਾਠ_ਬਾਹਰਲਾ(hdc, ਖਾਨਾ + ਪਾਠ_X(ix), ਪੰਕਤੀ, foptions,
					pcliprect, ਪਾਠ + ix, 1, padding);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
draw_ਸਤਰ(
    ਪੂਰਨ		x1,
    ਪੂਰਨ		y1,
    ਪੂਰਨ		x2,
    ਪੂਰਨ		y2,
    COLORREF	color)
{
#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_ਸਤਰ(s_dwc, x1, y1, x2, y2, color);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	Move_ਮੰਜ਼ਿਲ_Ex(s_hdc, x1, y1, ਨਿਰਾ);
	// Note: LineTo() excludes the last pixel in the line.
	ਸਤਰ_ਮੰਜ਼ਿਲ(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_pixel(
    ਪੂਰਨ		x,
    ਪੂਰਨ		y,
    COLORREF	color)
{
#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWrite_Context_ਸੈੱਟ_Pixel(s_dwc, x, y, color);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	ਸੈੱਟ_Pixel(s_hdc, x, y, color);
}

    ਸਥਿਰ ਖਾਲੀ
fill_rect(
    ਅਚਲ RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	HBRUSH	hbr2;

	ਜੇ (hbr == ਨਿਰਾ)
	    hbr2 = CreateSolidBrush(color);
	ਨਹੀਂਤਾਂ
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	ਜੇ (hbr == ਨਿਰਾ)
	    DeleteBrush(hbr2);
    }
}

    ਖਾਲੀ
gui_mch_draw_string(
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    char_u	*ਪਾਠ,
    ਪੂਰਨ		len,
    ਪੂਰਨ		flags)
{
    ਸਥਿਰ ਪੂਰਨ	*padding = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	pad_ਮਾਪ = 0;
    ਅਚਲ RECT	*pcliprect = ਨਿਰਾ;
    UINT	foptions = 0;
    ਸਥਿਰ WCHAR *unicodebuf = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ   *unicodepdy = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ	unibuflen = 0;
    ਪੂਰਨ		n = 0;
    ਪੂਰਨ		y;

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
    ਸਥਿਰ HBRUSH	hbr_cache[2] = {ਨਿਰਾ, ਨਿਰਾ};
    ਸਥਿਰ guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ਸਥਿਰ ਪੂਰਨ		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    ਜੇ (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ਖੱਬਾ = FILL_X(ਖਾਨਾ);
	rc.ਸਿਖਰ = FILL_Y(ਪੰਕਤੀ);
	ਜੇ (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ਸੱਜਾ = FILL_X(ਖਾਨਾ + mb_string2cells(ਪਾਠ, len));
	}
	ਨਹੀਂਤਾਂ
	    rc.ਸੱਜਾ = FILL_X(ਖਾਨਾ + len);
	rc.bottom = FILL_Y(ਪੰਕਤੀ + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	ਜੇ (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (hbr_cache[brush_lru] != ਨਿਰਾ)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	ਸੈੱਟ_Bk_ਬਹੁਲਕ(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	ਜੇ (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	ਨਹੀਂਤਾਂ ਜੇ (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ਜੇ_ਅੰਤ
    }
    ਸੈੱਟ_ਪਾਠ_Color(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIRECTX
    ਜੇ (IS_ENABLE_DIRECTX())
	DWrite_Context_ਸੈੱਟ_Font(s_dwc, (HFONT)gui.currFont);
#ਜੇ_ਅੰਤ

    ਜੇ (pad_ਮਾਪ != Columns || padding == ਨਿਰਾ || padding[0] != gui.char_ਚੌੜਾਈ)
    {
	ਪੂਰਨ	i;

	vim_free(padding);
	pad_ਮਾਪ = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(ਪੂਰਨ, pad_ਮਾਪ);
	ਜੇ (padding != ਨਿਰਾ)
	    ਲਈ (i = 0; i < pad_ਮਾਪ; i++)
		padding[i] = gui.char_ਚੌੜਾਈ;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    ਜੇ (enc_utf8)
	ਲਈ (n = 0; n < len; ++n)
	    ਜੇ (ਪਾਠ[n] >= 0x80)
		ਰੋਕੋ;

#ਜੇ defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    ਜੇ (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#ਜੇ_ਅੰਤ

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    ਜੇ ((enc_utf8
		|| (enc_codepage > 0 && (ਪੂਰਨ)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ਨਿਰਾ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(ਪੂਰਨ, len);

	unibuflen = len;
    }

    ਜੇ (enc_utf8 && n < len && unicodebuf != ਨਿਰਾ)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	ਪੂਰਨ		i;
	ਪੂਰਨ		wlen;	// string length in words
	ਪੂਰਨ		cells;	// cell width of string up to composing char
	ਪੂਰਨ		cw;	// width of current cell
	ਪੂਰਨ		c;

	wlen = 0;
	cells = 0;
	ਲਈ (i = 0; i < len; )
	{
	    c = utf_ptr2char(ਪਾਠ + i);
	    ਜੇ (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		unicodebuf[wlen++] = c;
	    }

	    ਜੇ (utf_iscomposing(c))
		cw = 0;
	    ਨਹੀਂਤਾਂ
	    {
		cw = utf_char2cells(c);
		ਜੇ (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    ਜੇ (unicodepdy != ਨਿਰਾ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ਜੇ (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_ਚੌੜਾਈ;
		    unicodepdy[wlen - 1] = 0;
		}
		ਨਹੀਂਤਾਂ
		    unicodepdy[wlen - 1] = cw * gui.char_ਚੌੜਾਈ;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(ਪਾਠ + i, len - i);
	}
#ਜੇ defined(FEAT_DIRECTX)
	ਜੇ (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWrite_Context_Draw_ਪਾਠ(s_dwc, unicodebuf, wlen,
		    ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_ਚੌੜਾਈ, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    Ext_ਪਾਠ_ਬਾਹਰਲਾ_W(s_hdc, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    ਨਹੀਂਤਾਂ ਜੇ ((enc_codepage > 0 && (ਪੂਰਨ)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	ਜੇ (unicodebuf != ਨਿਰਾ)
	{
	    ਜੇ (enc_latin9)
		latin9_ਮੰਜ਼ਿਲ_ucs(ਪਾਠ, len, unicodebuf);
	    ਨਹੀਂਤਾਂ
		len = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(enc_codepage,
			MB_PRECOMPOSED,
			(ਅੱਖਰ *)ਪਾਠ, len,
			(LPWSTR)unicodebuf, unibuflen);
	    ਜੇ (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ਜੇ (unicodepdy != ਨਿਰਾ)
		{
		    ਪੂਰਨ i;
		    ਪੂਰਨ cw;

		    ਲਈ (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			ਜੇ (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_ਚੌੜਾਈ;
		    }
		}
		Ext_ਪਾਠ_ਬਾਹਰਲਾ_W(s_hdc, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	ਜੇ (curwin->w_p_rl)
	    Rev_ਬਾਹਰਲਾ(s_hdc, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ),
			 foptions, pcliprect, (ਅੱਖਰ *)ਪਾਠ, len, padding);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    Ext_ਪਾਠ_ਬਾਹਰਲਾ(s_hdc, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ),
			 foptions, pcliprect, (ਅੱਖਰ *)ਪਾਠ, len, padding);
    }

    // Underline
    ਜੇ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(ਪੰਕਤੀ + 1) - 1;
	ਜੇ (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_ਸਤਰ(FILL_X(ਖਾਨਾ), y, FILL_X(ਖਾਨਾ + len), y, gui.currFgColor);
    }

    // Strikethrough
    ਜੇ (flags & DRAW_STRIKE)
    {
	y = FILL_Y(ਪੰਕਤੀ + 1) - gui.char_ਉਚਾਈ/2;
	draw_ਸਤਰ(FILL_X(ਖਾਨਾ), y, FILL_X(ਖਾਨਾ + len), y, gui.currSpColor);
    }

    // Undercurl
    ਜੇ (flags & DRAW_UNDERC)
    {
	ਪੂਰਨ			x;
	ਪੂਰਨ			ਵਿੱਥ;
	ਸਥਿਰ ਅਚਲ ਪੂਰਨ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(ਪੰਕਤੀ + 1) - 1;
	ਲਈ (x = FILL_X(ਖਾਨਾ); x < FILL_X(ਖਾਨਾ + len); ++x)
	{
	    ਵਿੱਥ = val[x % 8];
	    ਸੈੱਟ_pixel(x, y - ਵਿੱਥ, gui.currSpColor);
	}
    }
}


/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    ਖਾਲੀ
gui_mch_flush(ਖਾਲੀ)
{
#ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ਜੇ_ਅੰਤ

    GdiFlush();
}

    ਸਥਿਰ ਖਾਲੀ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ਨਿਰਾ, gui.ਪਿੱਠ_pixel);
}


    ਖਾਲੀ
gui_mch_get_screen_dimensions(ਪੂਰਨ *screen_w, ਪੂਰਨ *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ਸੱਜਾ - workarea_rect.ਖੱਬਾ
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.ਸਿਖਰ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_ਉਚਾਈ(FALSE);
}


#ਜੇ defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(
    vimmenu_T	*menu,
    ਪੂਰਨ		pos)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    ਜੇ (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_ਮੰਜ਼ਿਲ_utf16(menu->name, ਨਿਰਾ);
	ਜੇ (wn == ਨਿਰਾ)
	    ਵਾਪਸ;

	infow.cb_ਮਾਪ = ਆਕਾਰ(infow);
	infow.f_ਨਕਾਬ = MIIM_ਅੰਕੜੇ | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_ਵਸਤੂ_ਅੰਕੜੇ = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_ਅੰਕੜੇ = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ਪਾਓ_Menu_ਵਸਤੂ_W((ਮਾਪਾ == ਨਿਰਾ)
		? s_menuBar : ਮਾਪਾ->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    ਜੇ (ਮਾਪਾ == ਨਿਰਾ)
	gui_mswin_get_menu_ਉਚਾਈ(!gui.starting);
# ifdef FEAT_TEAROFF
    ਨਹੀਂਤਾਂ ਜੇ (Is_ਖਿੜਕੀ(ਮਾਪਾ->tearoff_handle))
	rebuild_tearoff(ਮਾਪਾ);
# endif
}

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ਬਿੰਦੂ_2 mp;

    (ਖਾਲੀ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (ਪੂਰਨ)mp.x, (ਪੂਰਨ)mp.y);
}

    ਖਾਲੀ
gui_make_popup(char_u *ਰਸਤਾ_name, ਪੂਰਨ mouse_pos)
{
    vimmenu_T	*menu = gui_ਲੱਭੋ_menu(ਰਸਤਾ_name);

    ਜੇ (menu == ਨਿਰਾ)
	ਵਾਪਸ;

    ਬਿੰਦੂ_2	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    ਜੇ (mouse_pos)
    {
	ਪੂਰਨ	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    ਨਹੀਂਤਾਂ ਜੇ (curwin != ਨਿਰਾ)
    {
	p.x += ਪਾਠ_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += ਪਾਠ_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    ਸੁਨੇਹਾ_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (ਪੂਰਨ)p.x, (ਪੂਰਨ)p.y);
}

# ਜੇ defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    ਖਾਲੀ
gui_make_tearoff(char_u *ਰਸਤਾ_name)
{
    vimmenu_T	*menu = gui_ਲੱਭੋ_menu(ਰਸਤਾ_name);

    // Found the menu, so tear it off.
    ਜੇ (menu != ਨਿਰਾ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(
    vimmenu_T	*menu,
    ਪੂਰਨ		idx)
{
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

    menu->id = s_menu_id++;
    menu->submenu_id = ਨਿਰਾ;

# ifdef FEAT_TEAROFF
    ਜੇ (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ਪਾਓ_Menu(ਮਾਪਾ->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    ਨਹੀਂਤਾਂ
# endif
# ifdef FEAT_TOOLBAR
    ਜੇ (menu_is_toolbar(ਮਾਪਾ->name))
    {
	TBBUTTON newtb;

	CLEAR_ਖੇਤਰ(newtb);
	ਜੇ (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	ਨਹੀਂਤਾਂ
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	WCHAR	*wn;

	wn = enc_ਮੰਜ਼ਿਲ_utf16(menu->name, ਨਿਰਾ);
	ਜੇ (wn != ਨਿਰਾ)
	{
	    ਪਾਓ_Menu_W(ਮਾਪਾ->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	ਜੇ (Is_ਖਿੜਕੀ(ਮਾਪਾ->tearoff_handle))
	    rebuild_tearoff(ਮਾਪਾ);
# endif
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    ਖਾਲੀ
gui_mch_destroy_menu(vimmenu_T *menu)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ਜੇ (menu->submenu_id == (HMENU)-1)
    {
	ਪੂਰਨ iButton;

	iButton = (ਪੂਰਨ)Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	ਜੇ (menu->ਮਾਪਾ != ਨਿਰਾ
		&& menu_is_popup(menu->ਮਾਪਾ->dname)
		&& menu->ਮਾਪਾ->submenu_id != ਨਿਰਾ)
	    RemoveMenu(menu->ਮਾਪਾ->submenu_id, menu->id, MF_BYCOMMAND);
	ਨਹੀਂਤਾਂ
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	ਜੇ (menu->submenu_id != ਨਿਰਾ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	ਜੇ (Is_ਖਿੜਕੀ(menu->tearoff_handle))
	    Destroy_ਖਿੜਕੀ(menu->tearoff_handle);
	ਜੇ (menu->ਮਾਪਾ != ਨਿਰਾ
		&& menu->ਮਾਪਾ->children != ਨਿਰਾ
		&& Is_ਖਿੜਕੀ(menu->ਮਾਪਾ->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->ਮਾਪਾ);
	}
# endif
    }
}

# ifdef FEAT_TEAROFF
    ਸਥਿਰ ਖਾਲੀ
rebuild_tearoff(vimmenu_T *menu)
{
    //hackish
    char_u	tbuf[128];
    RECT	trect;
    RECT	rct;
    RECT	roct;
    ਪੂਰਨ		x, y;

    HWND thwnd = menu->tearoff_handle;

    Get_ਖਿੜਕੀ_ਪਾਠ(thwnd, (LPSTR)tbuf, 127);
    ਜੇ (Get_ਖਿੜਕੀ_Rect(thwnd, &trect)
	    && Get_ਖਿੜਕੀ_Rect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ਖੱਬਾ - rct.ਖੱਬਾ;
	y = (trect.ਸਿਖਰ -  rct.bottom  + roct.bottom);
    }
    ਨਹੀਂਤਾਂ
    {
	x = y = 0xffffL;
    }
    Destroy_ਖਿੜਕੀ(thwnd);
    ਜੇ (menu->children != ਨਿਰਾ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	ਜੇ (Is_ਖਿੜਕੀ(menu->tearoff_handle))
	    (ਖਾਲੀ) ਸੈੱਟ_ਖਿੜਕੀ_Pos(menu->tearoff_handle,
				ਨਿਰਾ,
				(ਪੂਰਨ)trect.ਖੱਬਾ,
				(ਪੂਰਨ)trect.ਸਿਖਰ,
				0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}
# endif // FEAT_TEAROFF

/*
 * Make a menu either grey or not grey.
 */
    ਖਾਲੀ
gui_mch_menu_grey(
    vimmenu_T	*menu,
    ਪੂਰਨ	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ਜੇ (menu->submenu_id == (HMENU)-1)
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    ਨਹੀਂਤਾਂ
# endif
	(ਖਾਲੀ)Enable_Menu_ਵਸਤੂ(menu->ਮਾਪਾ ? menu->ਮਾਪਾ->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    ਜੇ ((menu->ਮਾਪਾ != ਨਿਰਾ) && (Is_ਖਿੜਕੀ(menu->ਮਾਪਾ->tearoff_handle)))
    {
	ਸ਼ਬਦ_2 menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	ਜੇ (menu->children == ਨਿਰਾ)
	    menuID = (ਸ਼ਬਦ_2)(menu->id);
	ਨਹੀਂਤਾਂ
	    menuID = (ਸ਼ਬਦ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = Get_Dlg_ਵਸਤੂ(menu->ਮਾਪਾ->tearoff_handle, menuID);
	ਜੇ (menuHandle)
	    Enable_ਖਿੜਕੀ(menuHandle, !grey);

    }
# endif
}

#ਜੇ_ਅੰਤ // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_ਸ਼ਬਦ(x)		*p++ = (x)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਜੋੜੋ_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (ਸ਼ਬਦ_2 *)dwp

#ਜੇ defined(FEAT_GUI_DIALOG)
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
    ਸਥਿਰ LRESULT CALLBACK
dialog_callback(
    HWND hwnd,
    UINT ਸੁਨੇਹਾ_2,
    WPARAM wParam,
    LPARAM lParam ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਸੁਨੇਹਾ_2 == WM_INITDIALOG)
    {
	Center_ਖਿੜਕੀ(hwnd, Get_ਖਿੜਕੀ(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(ਖਾਲੀ)ਸੈੱਟ_Focus(hwnd);
	ਜੇ (dialog_default_button > IDCANCEL)
	    (ਖਾਲੀ)ਸੈੱਟ_Focus(Get_Dlg_ਵਸਤੂ(hwnd, dialog_default_button));
	ਨਹੀਂਤਾਂ
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (ਖਾਲੀ)ਸੈੱਟ_Focus(Get_Dlg_ਵਸਤੂ(hwnd, DLG_NONBUTTON_CONTROL));
	ਵਾਪਸ FALSE;
    }

    ਜੇ (ਸੁਨੇਹਾ_2 == WM_COMMAND)
    {
	ਪੂਰਨ	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	ਜੇ (button >= DLG_NONBUTTON_CONTROL)
	    ਵਾਪਸ TRUE;

	// If the edit box exists, copy the string.
	ਜੇ (s_textfield != ਨਿਰਾ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    Get_Dlg_ਵਸਤੂ_ਪਾਠ_W(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_ਮੰਜ਼ਿਲ_enc(wp, ਨਿਰਾ);
	    ਜੇ (p != ਨਿਰਾ)
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
	ਜੇ (button == IDOK)
	{
	    ਜੇ (dialog_default_button > IDCANCEL)
		ਸਿਰਾ_Dialog(hwnd, dialog_default_button);
	}
	ਨਹੀਂਤਾਂ
	    ਸਿਰਾ_Dialog(hwnd, button - IDCANCEL);
	ਵਾਪਸ TRUE;
    }

    ਜੇ ((ਸੁਨੇਹਾ_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	ਸਿਰਾ_Dialog(hwnd, 0);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
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

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    ਪੂਰਨ
gui_mch_dialog(
    ਪੂਰਨ		 type,
    char_u	*title,
    char_u	*ਸੁਨੇਹਾ_2,
    char_u	*buttons,
    ਪੂਰਨ		 dfltbutton,
    char_u	*textfield,
    ਪੂਰਨ		ex_cmd ਅਣਵਰਤਿਆ)
{
    ਸ਼ਬਦ_2	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    ਪੂਰਨ		ਅੰਕ_Buttons;
    ਪੂਰਨ		*buttonWidths, *buttonPositions;
    ਪੂਰਨ		buttonYpos;
    ਪੂਰਨ		nchar, i;
    DWORD	lStyle;
    ਪੂਰਨ		dlgwidth = 0;
    ਪੂਰਨ		dlgheight;
    ਪੂਰਨ		editboxheight;
    ਪੂਰਨ		horiz_ਚੌੜਾਈ = 0;
    ਪੂਰਨ		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*ਆਖਰੀ_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    ਪੂਰਨ		font_ਉਚਾਈ_2;
    ਪੂਰਨ		ਪਾਠ_ਚੌੜਾਈ_2, min_Button_ਚੌੜਾਈ, ਸੁਨੇਹਾ_ਚੌੜਾਈ;
    ਪੂਰਨ		max_Dialog_ਚੌੜਾਈ;
    ਪੂਰਨ		max_Dialog_ਉਚਾਈ;
    ਪੂਰਨ		scroll_flag = 0;
    ਪੂਰਨ		vertical;
    ਪੂਰਨ		dlgPaddingX;
    ਪੂਰਨ		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ਪੂਰਨ		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    ਪੂਰਨ		l;
    ਪੂਰਨ		dlg_icon_ਚੌੜਾਈ;
    ਪੂਰਨ		dlg_icon_ਉਚਾਈ;
    ਪੂਰਨ		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    ਜੇ (!(gui.ਵਿੱਚ_use || gui.starting))
#  endif
	ਜੇ (silent_ਬਹੁਲਕ)
	    ਵਾਪਸ dfltbutton;   // return default option
# endif

    ਜੇ (s_hwnd == ਨਿਰਾ)
    {
	ਲੱਦੋ_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    ਨਹੀਂਤਾਂ
	dpi = pGetDpiForSystem();

    ਜੇ ((type < 0) || (type > VIM_ਆਖਰੀ_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_ਮਾਪ + STRLEN(ਸੁਨੇਹਾ_2) * 2);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    ਜੇ (tbuffer == ਨਿਰਾ)
	ਵਾਪਸ -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    ਅੰਕ_Buttons = 1;
    ਲਈ (i = 0; tbuffer[i] != '\0'; i++)
    {
	ਜੇ (tbuffer[i] == DLG_BUTTON_SEP)
	    ਅੰਕ_Buttons++;
    }
    ਜੇ (dfltbutton >= ਅੰਕ_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(ਪੂਰਨ, ਅੰਕ_Buttons);
    ਜੇ (buttonWidths == ਨਿਰਾ)
	ਵਾਪਸ -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(ਪੂਰਨ, ਅੰਕ_Buttons);
    ਜੇ (buttonPositions == ਨਿਰਾ)
	ਵਾਪਸ -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = Get_Desktop_ਖਿੜਕੀ();
    hdc = Get_ਖਿੜਕੀ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ਜੇ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_ਅਸਿੱਧਾ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ਨਹੀਂਤਾਂ
# endif
	font = CreateFont(-DLG_FONT_ਬਿੰਦੂ_ਮਾਪ, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    Get_ਪਾਠ_Metrics(hdc, &fontInfo);
    font_ਉਚਾਈ_2 = fontInfo.tm_ਉਚਾਈ;

    // Minimum width for horizontal button
    min_Button_ਚੌੜਾਈ = Get_ਪਾਠ_ਚੌੜਾਈ(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    ਜੇ (s_hwnd == ਨਿਰਾ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	max_Dialog_ਚੌੜਾਈ = workarea_rect.ਸੱਜਾ - workarea_rect.ਖੱਬਾ - 100;
	ਜੇ (max_Dialog_ਚੌੜਾਈ > adjust_by_system_dpi(600))
	    max_Dialog_ਚੌੜਾਈ = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	max_Dialog_ਉਚਾਈ = workarea_rect.bottom - workarea_rect.ਸਿਖਰ - 150;
    }
    ਨਹੀਂਤਾਂ
    {
	// Use our own window for the size, unless it's very small.
	Get_ਖਿੜਕੀ_Rect(s_hwnd, &rect);
	max_Dialog_ਚੌੜਾਈ = rect.ਸੱਜਾ - rect.ਖੱਬਾ
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	ਜੇ (max_Dialog_ਚੌੜਾਈ < adjust_by_system_dpi(DLG_MIN_MAX_ਚੌੜਾਈ))
	    max_Dialog_ਚੌੜਾਈ = adjust_by_system_dpi(DLG_MIN_MAX_ਚੌੜਾਈ);

	max_Dialog_ਉਚਾਈ = rect.bottom - rect.ਸਿਖਰ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	ਜੇ (max_Dialog_ਉਚਾਈ < adjust_by_system_dpi(DLG_MIN_MAX_ਉਚਾਈ))
	    max_Dialog_ਉਚਾਈ = adjust_by_system_dpi(DLG_MIN_MAX_ਉਚਾਈ);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = ਸੁਨੇਹਾ_2;
    ਸੁਨੇਹਾ_ਚੌੜਾਈ = 0;
    msgheight = 0;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 500);
    ਕਰੋ
    {
	msgheight += font_ਉਚਾਈ_2;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	ਪਾਠ_ਚੌੜਾਈ_2 = 0;
	ਆਖਰੀ_white = ਨਿਰਾ;
	ਲਈ (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    ਜੇ (l == 1 && VIM_ISWHITE(*pend)
					&& ਪਾਠ_ਚੌੜਾਈ_2 > max_Dialog_ਚੌੜਾਈ * 3 / 4)
		ਆਖਰੀ_white = pend;
	    ਪਾਠ_ਚੌੜਾਈ_2 += Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, pend, l);
	    ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 >= max_Dialog_ਚੌੜਾਈ)
	    {
		// Line will wrap.
		ਸੁਨੇਹਾ_ਚੌੜਾਈ = max_Dialog_ਚੌੜਾਈ;
		msgheight += font_ਉਚਾਈ_2;
		ਪਾਠ_ਚੌੜਾਈ_2 = 0;

		ਜੇ (ਆਖਰੀ_white != ਨਿਰਾ)
		{
		    // break the line just after a space
		    ਜੇ (pend > ਆਖਰੀ_white)
			ga.ga_len -= (ਪੂਰਨ)(pend - (ਆਖਰੀ_white + 1));
		    pend = ਆਖਰੀ_white + 1;
		    ਆਖਰੀ_white = ਨਿਰਾ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ਜਾਰੀ;
	    }

	    ਜਦੋਂ (--l >= 0)
		ga_append(&ga, *pend++);
	}
	ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 > ਸੁਨੇਹਾ_ਚੌੜਾਈ)
	    ਸੁਨੇਹਾ_ਚੌੜਾਈ = ਪਾਠ_ਚੌੜਾਈ_2;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } ਜਦੋਂ (*pend != NUL);

    ਜੇ (ga.ga_ਅੰਕੜੇ != ਨਿਰਾ)
	ਸੁਨੇਹਾ_2 = ga.ga_ਅੰਕੜੇ;

    ਸੁਨੇਹਾ_ਚੌੜਾਈ += 10;		// roundoff space

    dlg_icon_ਚੌੜਾਈ = adjust_by_system_dpi(DLG_ICON_ਚੌੜਾਈ);
    dlg_icon_ਉਚਾਈ = adjust_by_system_dpi(DLG_ICON_ਉਚਾਈ);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = ਸੁਨੇਹਾ_ਚੌੜਾਈ + dlg_icon_ਚੌੜਾਈ + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    ਜੇ (msgheight < dlg_icon_ਉਚਾਈ)
	msgheight = dlg_icon_ਉਚਾਈ;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ਨਿਰਾ && vim_strchr(p_go, GO_VERTICAL) != ਨਿਰਾ);
    ਜੇ (!vertical)
    {
	// Place buttons horizontally if they fit.
	horiz_ਚੌੜਾਈ = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	ਕਰੋ
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ਜੇ (pend == ਨਿਰਾ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    ਪਾਠ_ਚੌੜਾਈ_2 = Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, pstart, (ਪੂਰਨ)(pend - pstart));
	    ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 < min_Button_ਚੌੜਾਈ)
		ਪਾਠ_ਚੌੜਾਈ_2 = min_Button_ਚੌੜਾਈ;
	    ਪਾਠ_ਚੌੜਾਈ_2 += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = ਪਾਠ_ਚੌੜਾਈ_2;
	    buttonPositions[i++] = horiz_ਚੌੜਾਈ;
	    horiz_ਚੌੜਾਈ += ਪਾਠ_ਚੌੜਾਈ_2 + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} ਜਦੋਂ (*pend != NUL);

	ਜੇ (horiz_ਚੌੜਾਈ > max_Dialog_ਚੌੜਾਈ)
	    vertical = TRUE;	// Too wide to fit on the screen.
	ਨਹੀਂਤਾਂ ਜੇ (horiz_ਚੌੜਾਈ > dlgwidth)
	    dlgwidth = horiz_ਚੌੜਾਈ;
    }

    ਜੇ (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	ਕਰੋ
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ਜੇ (pend == ਨਿਰਾ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    ਪਾਠ_ਚੌੜਾਈ_2 = Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, pstart, (ਪੂਰਨ)(pend - pstart));
	    ਪਾਠ_ਚੌੜਾਈ_2 += dlgPaddingX;		// Padding within button
	    ਪਾਠ_ਚੌੜਾਈ_2 += DLG_VERT_PADDING_X * 2; // Padding around button
	    ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 > dlgwidth)
		dlgwidth = ਪਾਠ_ਚੌੜਾਈ_2;
	    pstart = pend + 1;
	} ਜਦੋਂ (*pend != NUL);
    }

    ਜੇ (dlgwidth < DLG_MIN_ਚੌੜਾਈ)
	dlgwidth = DLG_MIN_ਚੌੜਾਈ;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    ਜੋੜੋ_long(lStyle);
    ਜੋੜੋ_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    ਜੋੜੋ_ਸ਼ਬਦ(0);	// NumberOfItems(will change later)
    ਜੋੜੋ_ਸ਼ਬਦ(10);	// x
    ਜੋੜੋ_ਸ਼ਬਦ(10);	// y
    ਜੋੜੋ_ਸ਼ਬਦ(Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgwidth));	// cx

    // Dialog height.
    ਜੇ (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * font_ਉਚਾਈ_2 * ਅੰਕ_Buttons;
    ਨਹੀਂਤਾਂ
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * font_ਉਚਾਈ_2;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = font_ਉਚਾਈ_2 + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    ਜੇ (textfield != ਨਿਰਾ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    ਜੇ (dlgheight > max_Dialog_ਉਚਾਈ)
    {
	msgheight = msgheight - (dlgheight - max_Dialog_ਉਚਾਈ);
	dlgheight = max_Dialog_ਉਚਾਈ;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	ਸੁਨੇਹਾ_ਚੌੜਾਈ = dlgwidth - dlg_icon_ਚੌੜਾਈ - 3 * dlgPaddingX;
    }

    ਜੋੜੋ_ਸ਼ਬਦ(Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(dlgheight));

    ਜੋੜੋ_ਸ਼ਬਦ(0);	// Menu
    ਜੋੜੋ_ਸ਼ਬਦ(0);	// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_ਸੰਸਕਰਣ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ਜੇ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ਉਚਾਈ, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ਪੂਰਨ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	*p++ = DLG_FONT_ਬਿੰਦੂ_ਮਾਪ;		// point size
	nchar = n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    ਜੇ (textfield != ਨਿਰਾ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    ਜੇ (!vertical)
	horiz_ਚੌੜਾਈ = (dlgwidth - horiz_ਚੌੜਾਈ) / 2;	// Now it's X offset
    ਲਈ (i = 0; i < ਅੰਕ_Buttons; i++)
    {
	// get end of this button.
	ਲਈ (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	ਜੇ (*pend)
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
	ਜੇ (vertical)
	{
	    p = ਜੋੜੋ_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(DLG_VERT_PADDING_X),
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(buttonYpos // TBK
				   + 2 * font_ਉਚਾਈ_2 * i),
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (ਸ਼ਬਦ_2)(Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(2 * font_ਉਚਾਈ_2) - 1),
		    (ਸ਼ਬਦ_2)(IDCANCEL + 1 + i), (ਸ਼ਬਦ_2)0x0080, (ਅੱਖਰ *)pstart);
	}
	ਨਹੀਂਤਾਂ
	{
	    p = ਜੋੜੋ_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(horiz_ਚੌੜਾਈ + buttonPositions[i]),
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(buttonYpos), // TBK
		    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(buttonWidths[i]),
		    (ਸ਼ਬਦ_2)(Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(2 * font_ਉਚਾਈ_2) - 1),
		    (ਸ਼ਬਦ_2)(IDCANCEL + 1 + i), (ਸ਼ਬਦ_2)0x0080, (ਅੱਖਰ *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += ਅੰਕ_Buttons;

    // Vim icon
    p = ਜੋੜੋ_dialog_element(p, SS_ICON,
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgPaddingX),
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(dlgPaddingY),
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlg_icon_ਚੌੜਾਈ),
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(dlg_icon_ਉਚਾਈ),
	    DLG_NONBUTTON_CONTROL + 0, (ਸ਼ਬਦ_2)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = ਜੋੜੋ_dialog_element(p, ES_ਖੱਬਾ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_X(2 * dlgPaddingX + dlg_icon_ਚੌੜਾਈ),
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(dlgPaddingY),
	    (ਸ਼ਬਦ_2)(Pixel_ਮੰਜ਼ਿਲ_Dialog_X(ਸੁਨੇਹਾ_ਚੌੜਾਈ) + 1),
	    Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (ਸ਼ਬਦ_2)0x0081, (ਅੱਖਰ *)ਸੁਨੇਹਾ_2);

    // Edit box
    ਜੇ (textfield != ਨਿਰਾ)
    {
	p = ਜੋੜੋ_dialog_element(p, ES_ਖੱਬਾ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		Pixel_ਮੰਜ਼ਿਲ_Dialog_X(2 * dlgPaddingX),
		Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(2 * dlgPaddingY + msgheight),
		Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgwidth - 4 * dlgPaddingX),
		Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(font_ਉਚਾਈ_2 + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (ਸ਼ਬਦ_2)0x0081, (ਅੱਖਰ *)textfield);
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
    ਜੇ (textfield != ਨਿਰਾ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    ਨਹੀਂਤਾਂ
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ਨਿਰਾ;
    }

    // show the dialog box modally and get a return value
    nchar = (ਪੂਰਨ)Dialog_Box_ਅਸਿੱਧਾ(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_ਅੰਕੜੇ);

    // Focus back to our window (for when MDI is used).
    (ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);

    ਵਾਪਸ nchar;
}

#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG

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
    ਸਥਿਰ PWORD
ਜੋੜੋ_dialog_element(
    PWORD p,
    DWORD lStyle,
    ਸ਼ਬਦ_2 x,
    ਸ਼ਬਦ_2 y,
    ਸ਼ਬਦ_2 w,
    ਸ਼ਬਦ_2 h,
    ਸ਼ਬਦ_2 Id,
    ਸ਼ਬਦ_2 clss,
    ਅਚਲ ਅੱਖਰ *caption)
{
    ਪੂਰਨ nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_ਬੱਚਾ;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (ਸ਼ਬਦ_2)0xffff;
    *p++ = clss;			//2 more here

    nchar = n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    ਵਾਪਸ p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ਸਥਿਰ LPWORD
lpwAlign(
    LPWORD lp_ਵਿੱਚ)
{
    long_u ul;

    ul = (long_u)lp_ਵਿੱਚ;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ਵਾਪਸ (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ਸਥਿਰ ਪੂਰਨ
n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(
    LPWORD lpWCStr,
    LPSTR lp_Ansi_ਵਿੱਚ,
    BOOL use_enc)
{
    ਪੂਰਨ		nChar = 0;
    ਪੂਰਨ		len = lstrlen(lp_Ansi_ਵਿੱਚ) + 1;	// include NUL character
    ਪੂਰਨ		i;
    WCHAR	*wn;

    ਜੇ (use_enc && enc_codepage >= 0 && (ਪੂਰਨ)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_ਮੰਜ਼ਿਲ_utf16((char_u *)lp_Ansi_ਵਿੱਚ, ਨਿਰਾ);
	ਜੇ (wn != ਨਿਰਾ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (ਪੂਰਨ)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    ਜੇ (nChar == 0)
	// Use Win32 conversion function.
	nChar = Multi_Byte_ਮੰਜ਼ਿਲ_Wide_Char(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lp_Ansi_ਵਿੱਚ, len,
		lpWCStr, len);
    ਲਈ (i = 0; i < nChar; ++i)
	ਜੇ (lpWCStr[i] == (ਸ਼ਬਦ_2)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (ਸ਼ਬਦ_2)' ';

    ਵਾਪਸ nChar;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ਸਥਿਰ HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    ਸ਼ਬਦ_2 menu_id)
{
    ਲਈ ( ; menu != ਨਿਰਾ; menu = menu->ਅਗਲਾ)
    {
	ਜੇ (menu->modes == 0)	// this menu has just been deleted
	    ਜਾਰੀ;
	ਜੇ (menu_is_separator(menu->dname))
	    ਜਾਰੀ;
	ਜੇ ((ਸ਼ਬਦ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ਵਾਪਸ menu->submenu_id;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ਸਥਿਰ LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT ਸੁਨੇਹਾ_2,
    WPARAM wParam,
    LPARAM lParam)
{
    ਜੇ (ਸੁਨੇਹਾ_2 == WM_INITDIALOG)
    {
	ਸੈੱਟ_ਖਿੜਕੀ_Long_Ptr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ਵਾਪਸ (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(ਸੁਨੇਹਾ_2, lParam);

    ਜੇ (ਸੁਨੇਹਾ_2 == WM_COMMAND)
    {
	ਜੇ ((ਸ਼ਬਦ_2)(LOWORD(wParam)) & 0x8000)
	{
	    ਬਿੰਦੂ_2   mp;
	    RECT    rect;

	    ਜੇ (GetCursorPos(&mp) && Get_ਖਿੜਕੀ_Rect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)Get_ਖਿੜਕੀ_Long_Ptr(hwnd, DWLP_USER);
		(ਖਾਲੀ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (ਪੂਰਨ)rect.ਸੱਜਾ - 8,
			 (ਪੂਰਨ)mp.y,
			 (ਪੂਰਨ)0,	    // reserved param
			 s_hwnd,
			 ਨਿਰਾ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	ਨਹੀਂਤਾਂ
	    // Pass on messages to the main Vim window
	    Post_ਸੁਨੇਹਾ(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(ਖਾਲੀ)ਸੈੱਟ_Focus(s_hwnd);
	ਵਾਪਸ TRUE;
    }
    ਜੇ ((ਸੁਨੇਹਾ_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	Destroy_ਖਿੜਕੀ(hwnd);
	ਵਾਪਸ TRUE;
    }

    // When moved around, give main window the focus back.
    ਜੇ (ਸੁਨੇਹਾ_2 == WM_EXITSIZEMOVE)
	(ਖਾਲੀ)ਸੈੱਟ_ਸਰਗਰਮ_ਖਿੜਕੀ(s_hwnd);

    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ


/*
 * Computes the dialog base units based on the current dialog font.
 * We don't use the GetDialogBaseUnits() API, because we don't use the
 * (old-style) system font.
 */
    ਸਥਿਰ ਖਾਲੀ
get_dialog_font_metrics(ਖਾਲੀ)
{
    HDC		    hdc;
    HFONT	    hfontTools = 0;
    ਮਾਪ_3	    ਮਾਪ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SYSMENU_FONT
    ਜੇ (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = Create_Font_ਅਸਿੱਧਾ_W(&lfSysmenu);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	hfontTools = CreateFont(-DLG_FONT_ਬਿੰਦੂ_ਮਾਪ, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_ਔਸਤਨ_Font_ਮਾਪ(hdc, &ਮਾਪ);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (ਸ਼ਬਦ_2)ਮਾਪ.cx;
    s_dlgfntheight = (ਸ਼ਬਦ_2)ਮਾਪ.cy;
}

#ਜੇ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    ਪੂਰਨ		initX,
    ਪੂਰਨ		initY)
{
    ਸ਼ਬਦ_2	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    ਪੂਰਨ		template_len;
    ਪੂਰਨ		nchar, ਪਾਠ_ਚੌੜਾਈ_2, submenu_ਚੌੜਾਈ;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    ਸ਼ਬਦ_2	dlgwidth;
    ਸ਼ਬਦ_2	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*ਸਿਖਰ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    ਪੂਰਨ		ਖਾਨਾ, space_ਚੌੜਾਈ, len;
    ਪੂਰਨ		ਕਾਲਮ_Widths[2];
    char_u	*label, *ਪਾਠ;
    ਪੂਰਨ		acLen = 0;
    ਪੂਰਨ		nameLen;
    ਪੂਰਨ		padding0, padding1, padding2 = 0;
    ਪੂਰਨ		sepPadding=0;
    ਪੂਰਨ		x;
    ਪੂਰਨ		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ਪੂਰਨ		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    ਜੇ (Is_ਖਿੜਕੀ(menu->tearoff_handle))
    {
	ਬਿੰਦੂ_2 mp;
	ਜੇ (GetCursorPos(&mp))
	{
	    ਸੈੱਟ_ਖਿੜਕੀ_Pos(menu->tearoff_handle, ਨਿਰਾ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ਵਾਪਸ;
    }

    /*
     * Create a new tearoff.
     */
    ਜੇ (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_ਮਾਪ;
    pdlgtemplate = p = (ਸ਼ਬਦ_2 *)LocalAlloc(LPTR, template_len);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;

    hwnd = Get_Desktop_ਖਿੜਕੀ();
    hdc = Get_ਖਿੜਕੀ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ਜੇ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_ਅਸਿੱਧਾ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ਨਹੀਂਤਾਂ
# endif
	font = CreateFont(-DLG_FONT_ਬਿੰਦੂ_ਮਾਪ, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    space_ਚੌੜਾਈ = Get_ਪਾਠ_ਚੌੜਾਈ(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenu_ਚੌੜਾਈ = 0;
    ਲਈ (ਖਾਨਾ = 0; ਖਾਨਾ < 2; ਖਾਨਾ++)
    {
	ਕਾਲਮ_Widths[ਖਾਨਾ] = 0;
	FOR_ALL_ਬੱਚਾ_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    ਪਾਠ = (ਖਾਨਾ == 0) ? pmenu->dname : pmenu->actext;
	    ਜੇ (ਪਾਠ != ਨਿਰਾ && *ਪਾਠ != NUL)
	    {
		ਪਾਠ_ਚੌੜਾਈ_2 = Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, ਪਾਠ, (ਪੂਰਨ)STRLEN(ਪਾਠ));
		ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 > ਕਾਲਮ_Widths[ਖਾਨਾ])
		    ਕਾਲਮ_Widths[ਖਾਨਾ] = ਪਾਠ_ਚੌੜਾਈ_2;
	    }
	    ਜੇ (pmenu->children != ਨਿਰਾ)
		submenu_ਚੌੜਾਈ = TEAROFF_ਕਾਲਮ_PADDING * space_ਚੌੜਾਈ;
	}
    }
    ਜੇ (ਕਾਲਮ_Widths[1] == 0)
    {
	// no accelerators
	ਜੇ (submenu_ਚੌੜਾਈ != 0)
	    ਕਾਲਮ_Widths[0] += submenu_ਚੌੜਾਈ;
	ਨਹੀਂਤਾਂ
	    ਕਾਲਮ_Widths[0] += space_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	// there is an accelerator column
	ਕਾਲਮ_Widths[0] += TEAROFF_ਕਾਲਮ_PADDING * space_ਚੌੜਾਈ;
	ਕਾਲਮ_Widths[1] += submenu_ਚੌੜਾਈ;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    ਪਾਠ_ਚੌੜਾਈ_2 = ਕਾਲਮ_Widths[0] + ਕਾਲਮ_Widths[1];
    ਜੇ (submenu_ਚੌੜਾਈ != 0)
    {
	submenu_ਚੌੜਾਈ = Get_ਪਾਠ_ਚੌੜਾਈ(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (ਪੂਰਨ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	ਪਾਠ_ਚੌੜਾਈ_2 += submenu_ਚੌੜਾਈ;
    }
    dlgwidth = Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, title, (ਪੂਰਨ)STRLEN(title));
    ਜੇ (ਪਾਠ_ਚੌੜਾਈ_2 > dlgwidth)
	dlgwidth = ਪਾਠ_ਚੌੜਾਈ_2;
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
    ਜੇ (initX == 0xffffL)
	*p++ = Pixel_ਮੰਜ਼ਿਲ_Dialog_X(x); // x
    ਨਹੀਂਤਾਂ
	*p++ = Pixel_ਮੰਜ਼ਿਲ_Dialog_X(initX); // x
    ਜੇ (initY == 0xffffL)
	*p++ = Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(y); // y
    ਨਹੀਂਤਾਂ
	*p++ = Pixel_ਮੰਜ਼ਿਲ_Dialog_Y(initY); // y
    *p++ = Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_ਸੰਸਕਰਣ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ਜੇ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ਉਚਾਈ, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ਪੂਰਨ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	*p++ = DLG_FONT_ਬਿੰਦੂ_ਮਾਪ;		// point size
	nchar = n_Copy_Ansi_ਮੰਜ਼ਿਲ_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    ਜੇ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	menu = menu->children;
    ਸਿਖਰ_menu = menu;
    ਲਈ ( ; menu != ਨਿਰਾ; menu = menu->ਅਗਲਾ)
    {
	ਜੇ (menu->modes == 0)	// this menu has just been deleted
	    ਜਾਰੀ;
	ਜੇ (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ਜਾਰੀ;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	ਜੇ (((ਅੱਖਰ *)p - (ਅੱਖਰ *)pdlgtemplate) + 1000 > template_len)
	{
	    ਸ਼ਬਦ_2    *newp;

	    newp = (ਸ਼ਬਦ_2 *)LocalAlloc(LPTR, template_len + 4096);
	    ਜੇ (newp != ਨਿਰਾ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (ਅੱਖਰ *)p - (ਅੱਖਰ *)pdlgtemplate);
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
	len = nameLen = (ਪੂਰਨ)STRLEN(menu->name);
	padding0 = (ਕਾਲਮ_Widths[0] - Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, menu->dname,
				      (ਪੂਰਨ)STRLEN(menu->dname))) / space_ਚੌੜਾਈ;
	len += padding0;

	ਜੇ (menu->actext != ਨਿਰਾ)
	{
	    acLen = (ਪੂਰਨ)STRLEN(menu->actext);
	    len += acLen;
	    ਪਾਠ_ਚੌੜਾਈ_2 = Get_ਪਾਠ_ਚੌੜਾਈ_Enc(hdc, menu->actext, acLen);
	}
	ਨਹੀਂਤਾਂ
	    ਪਾਠ_ਚੌੜਾਈ_2 = 0;
	padding1 = (ਕਾਲਮ_Widths[1] - ਪਾਠ_ਚੌੜਾਈ_2) / space_ਚੌੜਾਈ;
	len += padding1;

	ਜੇ (menu->children == ਨਿਰਾ)
	{
	    padding2 = submenu_ਚੌੜਾਈ / space_ਚੌੜਾਈ;
	    len += padding2;
	    menuID = (ਸ਼ਬਦ_2)(menu->id);
	}
	ਨਹੀਂਤਾਂ
	{
	    len += (ਪੂਰਨ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (ਸ਼ਬਦ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	ਪਾਠ = label = alloc(len + 1);
	ਜੇ (label == ਨਿਰਾ)
	    ਰੋਕੋ;

	vim_strncpy(ਪਾਠ, menu->name, nameLen);
	ਪਾਠ = vim_strchr(ਪਾਠ, TAB);	    // stop at TAB before actext
	ਜੇ (ਪਾਠ == ਨਿਰਾ)
	    ਪਾਠ = label + nameLen;	    // no actext, use whole name
	ਜਦੋਂ (padding0-- > 0)
	    *ਪਾਠ++ = ' ';
	ਜੇ (menu->actext != ਨਿਰਾ)
	{
	    STRNCPY(ਪਾਠ, menu->actext, acLen);
	    ਪਾਠ += acLen;
	}
	ਜਦੋਂ (padding1-- > 0)
	    *ਪਾਠ++ = ' ';
	ਜੇ (menu->children != ਨਿਰਾ)
	{
	    STRCPY(ਪਾਠ, TEAROFF_SUBMENU_LABEL);
	    ਪਾਠ += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜਦੋਂ (padding2-- > 0)
		*ਪਾਠ++ = ' ';
	}
	*ਪਾਠ = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = ਜੋੜੋ_dialog_element(p,
		BS_PUSHBUTTON|BS_ਖੱਬਾ,
		(ਸ਼ਬਦ_2)Pixel_ਮੰਜ਼ਿਲ_Dialog_X(TEAROFF_PADDING_X),
		(ਸ਼ਬਦ_2)(sepPadding + 1 + 13 * (*pnumitems)),
		(ਸ਼ਬਦ_2)Pixel_ਮੰਜ਼ਿਲ_Dialog_X(dlgwidth - 2 * TEAROFF_PADDING_X),
		(ਸ਼ਬਦ_2)12,
		menuID, (ਸ਼ਬਦ_2)0x0080, (ਅੱਖਰ *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (ਸ਼ਬਦ_2)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = Create_Dialog_ਅਸਿੱਧਾ_Param(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)tearoff_callback,
	    (LPARAM)ਸਿਖਰ_menu);

    LocalFree(LocalHandle(pdlgtemplate));
    SelectFont(hdc, oldFont);
    DeleteObject(font);
    ReleaseDC(hwnd, hdc);

    /*
     * Reassert ourselves as the active window.  This is so that after creating
     * a tearoff, the user doesn't have to click with the mouse just to start
     * typing again!
     */
    (ਖਾਲੀ)ਸੈੱਟ_ਸਰਗਰਮ_ਖਿੜਕੀ(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR)
# include "gui_w32_rc.h"

/*
 * Create the toolbar, initially unpopulated.
 *  (just like the menu, there are no defaults, it's all
 *  set up through menu.vim)
 */
    ਸਥਿਰ ਖਾਲੀ
initialise_toolbar(ਖਾਲੀ)
{
    InitCommonControls();
    s_toolbarhwnd = CreateToolbarEx(
		    s_hwnd,
		    WS_ਬੱਚਾ | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ਨਿਰਾ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_ਚੌੜਾਈ, //api guide is wrong!
		    TOOLBAR_BUTTON_ਉਚਾਈ,
		    TOOLBAR_BUTTON_ਚੌੜਾਈ,
		    TOOLBAR_BUTTON_ਉਚਾਈ,
		    ਆਕਾਰ(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_SETSTYLE, 0,
	Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = Subclass_ਖਿੜਕੀ(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ);

    update_toolbar_ਮਾਪ();
}

    ਸਥਿਰ ਖਾਲੀ
update_toolbar_ਮਾਪ(ਖਾਲੀ)
{
    ਪੂਰਨ		w, h;
    TBMETRICS	tbm;

    tbm.cb_ਮਾਪ = ਆਕਾਰ(TBMETRICS);
    tbm.dw_ਨਕਾਬ = TBMF_PAD | TBMF_BUTTONSPACING;
    Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_ਚੌੜਾਈ + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_ਉਚਾਈ + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_ਉਚਾਈ = h + 6;

    //DWORD s = SendMessage(s_toolbarhwnd, TB_GETBUTTONSIZE, 0, 0);
    //TRACE("actual button size: %d, %d", LOWORD(s), HIWORD(s));

    // TODO:
    // Currently, this function only updates the size of toolbar buttons.
    // It would be nice if the toolbar images are resized based on DPI.
}

    ਸਥਿਰ LRESULT CALLBACK
toolbar_wndproc(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ,
    WPARAM wParam,
    LPARAM lParam)
{
    HandleMouseHide(u_ਸੁਨੇਹਾ, lParam);
    ਵਾਪਸ Call_ਖਿੜਕੀ_Proc(s_toolbar_wndproc, hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
}

    ਸਥਿਰ ਪੂਰਨ
get_toolbar_bitmap(vimmenu_T *menu)
{
    ਪੂਰਨ i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    ਜੇ (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ਨਿਰਾ;

	ਜੇ (menu->iconfile != ਨਿਰਾ)
	{
	    gui_ਲੱਭੋ_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = ਲੱਦੋ_Image(
			ਨਿਰਾ,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_ਚੌੜਾਈ,
			TOOLBAR_BUTTON_ਉਚਾਈ,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	ਜੇ (hbitmap == ਨਿਰਾ
		&& (gui_ਲੱਭੋ_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ਨਿਰਾ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = ਲੱਦੋ_Image(
		    ਨਿਰਾ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_ਚੌੜਾਈ,
		    TOOLBAR_BUTTON_ਉਚਾਈ,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	ਜੇ (hbitmap != ਨਿਰਾ)
	{
	    TBADDBITMAP tb_ਜੋੜੋ_Bitmap;

	    tb_ਜੋੜੋ_Bitmap.hInst = ਨਿਰਾ;
	    tb_ਜੋੜੋ_Bitmap.nID = (long_u)hbitmap;

	    i = (ਪੂਰਨ)Send_ਸੁਨੇਹਾ(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tb_ਜੋੜੋ_Bitmap);
	    // i will be set to -1 if it fails
	}
    }
    ਜੇ (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_ਗਿਣਤੀ)
	i = menu->iconidx;

    ਵਾਪਸ i;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ ਖਾਲੀ
initialise_tabline(ਖਾਲੀ)
{
    InitCommonControls();

    s_tabhwnd = Create_ਖਿੜਕੀ(WC_TABCONTROL, "Vim tabline",
	    WS_ਬੱਚਾ|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ਨਿਰਾ, g_hinst, ਨਿਰਾ);
    s_tabline_wndproc = Subclass_ਖਿੜਕੀ(s_tabhwnd, tabline_wndproc);

    gui.tabline_ਉਚਾਈ = TABLINE_ਉਚਾਈ;

    ਸੈੱਟ_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ਸਥਿਰ tabpage_T *
Get_Tab_From_ਬਿੰਦੂ(
    HWND    hWnd,
    ਬਿੰਦੂ_2   pt)
{
    tabpage_T	*ptp = ਨਿਰਾ;

    ਜੇ (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	ਜੇ (s_tabhwnd == hWnd)
	{
	    ਪੂਰਨ idx = Tab_Ctrl_ਪ੍ਰਾਪਤੀ_Test(s_tabhwnd, &htinfo);
	    ਜੇ (idx != -1)
		ptp = ਲੱਭੋ_tabpage(idx + 1);
	}
    }
    ਵਾਪਸ ptp;
}

ਸਥਿਰ ਬਿੰਦੂ_2	    s_pt = {0, 0};
ਸਥਿਰ HCURSOR      s_hCursor = ਨਿਰਾ;

    ਸਥਿਰ LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_ਸੁਨੇਹਾ,
    WPARAM wParam,
    LPARAM lParam)
{
    ਬਿੰਦੂ_2	pt;
    tabpage_T	*tp;
    RECT	rect;
    ਪੂਰਨ		nCenter;
    ਪੂਰਨ		idx0;
    ਪੂਰਨ		idx1;

    HandleMouseHide(u_ਸੁਨੇਹਾ, lParam);

    ਚੋਣ (u_ਸੁਨੇਹਾ)
    {
	ਹਾਲਤ WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		ਸੈੱਟ_Capture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		ਰੋਕੋ;
	    }
	ਹਾਲਤ WM_MOUSEMOVE:
	    ਜੇ (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		ਜੇ (ਨਿਰਪੇਖ(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    ਸੈੱਟ_Cursor(ਲੱਦੋ_Cursor(ਨਿਰਾ, IDC_SIZEWE));

		    tp = Get_Tab_From_ਬਿੰਦੂ(hwnd, pt);
		    ਜੇ (tp != ਨਿਰਾ)
		    {
			idx0 = tabpage_ਸੂਚਕਾਂਕ(curtab) - 1;
			idx1 = tabpage_ਸੂਚਕਾਂਕ(tp) - 1;

			Tab_Ctrl_Get_ਵਸਤੂ_Rect(hwnd, idx1, &rect);
			nCenter = rect.ਖੱਬਾ + (rect.ਸੱਜਾ - rect.ਖੱਬਾ) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			ਜੇ ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			ਨਹੀਂਤਾਂ ਜੇ ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ WM_LBUTTONUP:
	    {
		ਜੇ (GetCapture() == hwnd)
		{
		    ਸੈੱਟ_Cursor(s_hCursor);
		    ReleaseCapture();
		}
		ਰੋਕੋ;
	    }
	ਹਾਲਤ WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = Tab_Ctrl_ਪ੍ਰਾਪਤੀ_Test(hwnd, &htinfo);
		ਜੇ (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		ਰੋਕੋ;
	    }
	ਮੂਲ:
	    ਰੋਕੋ;
    }

    ਵਾਪਸ Call_ਖਿੜਕੀ_Proc(s_tabline_wndproc, hwnd, u_ਸੁਨੇਹਾ, wParam, lParam);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    ਜੇ (IsIconic(s_hwnd))
	 Send_ਸੁਨੇਹਾ(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ਸਥਿਰ ਖਾਲੀ
dyn_imm_ਲੱਦੋ(ਖਾਲੀ)
{
    hLibImm = vim_ਲੱਦੋ_Lib("imm32.dll");
    ਜੇ (hLibImm == ਨਿਰਾ)
	ਵਾਪਸ;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))Get_Proc_ਪਤਾ(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))Get_Proc_ਪਤਾ(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))Get_Proc_ਪਤਾ(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))Get_Proc_ਪਤਾ(hLibImm, "ImmReleaseContext");
    p_Imm_Get_Open_ਸਥਿਤੀ
	    = (BOOL (WINAPI *)(HIMC))Get_Proc_ਪਤਾ(hLibImm, "ImmGetOpenStatus");
    p_Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ
	    = (BOOL (WINAPI *)(HIMC, BOOL))Get_Proc_ਪਤਾ(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_ਪਤਾ(hLibImm, "ImmGetCompositionFontW");
    p_Imm_ਸੈੱਟ_Composition_Font_W
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_ਪਤਾ(hLibImm, "ImmSetCompositionFontW");
    p_Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))Get_Proc_ਪਤਾ(hLibImm, "ImmSetCompositionWindow");
    p_Imm_Get_Conversion_ਸਥਿਤੀ
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))Get_Proc_ਪਤਾ(hLibImm, "ImmGetConversionStatus");
    p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))Get_Proc_ਪਤਾ(hLibImm, "ImmSetConversionStatus");

    ਜੇ (       pImmGetCompositionStringW == ਨਿਰਾ
	    || pImmGetContext == ਨਿਰਾ
	    || pImmAssociateContext == ਨਿਰਾ
	    || pImmReleaseContext == ਨਿਰਾ
	    || p_Imm_Get_Open_ਸਥਿਤੀ == ਨਿਰਾ
	    || p_Imm_ਸੈੱਟ_Open_ਸਥਿਤੀ == ਨਿਰਾ
	    || pImmGetCompositionFontW == ਨਿਰਾ
	    || p_Imm_ਸੈੱਟ_Composition_Font_W == ਨਿਰਾ
	    || p_Imm_ਸੈੱਟ_Composition_ਖਿੜਕੀ == ਨਿਰਾ
	    || p_Imm_Get_Conversion_ਸਥਿਤੀ == ਨਿਰਾ
	    || p_Imm_ਸੈੱਟ_Conversion_ਸਥਿਤੀ == ਨਿਰਾ)
    {
	FreeLibrary(hLibImm);
	hLibImm = ਨਿਰਾ;
	pImmGetContext = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    ਵਾਪਸ;
}

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    ਖਾਲੀ
gui_mch_drawsign(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ typenr)
{
    signicon_t *sign;
    ਪੂਰਨ x, y, w, h;

    ਜੇ (!gui.ਵਿੱਚ_use || (sign = (signicon_t *)sign_get_image(typenr)) == ਨਿਰਾ)
	ਵਾਪਸ;

# ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = ਪਾਠ_X(ਖਾਨਾ);
    y = ਪਾਠ_Y(ਪੰਕਤੀ);
    w = gui.char_ਚੌੜਾਈ * 2;
    h = gui.char_ਉਚਾਈ;
    ਚੋਣ (sign->uType)
    {
	ਹਾਲਤ IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		ਦੋਆਧਾਰੀ_ਅੰਕ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ IMAGE_ICON:
	ਹਾਲਤ IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ਨਿਰਾ, DI_ਸਾਧਾਰਨ);
	    ਰੋਕੋ;
# ifdef FEAT_XPM_W32
	ਹਾਲਤ IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		ਦੋਆਧਾਰੀ_ਅੰਕ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		ਦੋਆਧਾਰੀ_ਅੰਕ_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ਰੋਕੋ;
# endif
    }
}

    ਸਥਿਰ ਖਾਲੀ
close_signicon_image(signicon_t *sign)
{
    ਜੇ (sign == ਨਿਰਾ)
	ਵਾਪਸ;

    ਚੋਣ (sign->uType)
    {
	ਹਾਲਤ IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    ਰੋਕੋ;
	ਹਾਲਤ IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    ਰੋਕੋ;
	ਹਾਲਤ IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    ਰੋਕੋ;
# ifdef FEAT_XPM_W32
	ਹਾਲਤ IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    ਰੋਕੋ;
# endif
    }
}

    ਖਾਲੀ *
gui_mch_ਰਜਿਸਟਰ_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ਨਿਰਾ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    ਜੇ (ext > signfile)
    {
	ਪੂਰਨ do_ਲੱਦੋ = 1;

	ਜੇ (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	ਨਹੀਂਤਾਂ
	    do_ਲੱਦੋ = 0;

	ਜੇ (do_ਲੱਦੋ)
	    sign.hImage = (HANDLE)ਲੱਦੋ_Image(ਨਿਰਾ, (LPCSTR)signfile, sign.uType,
		    gui.char_ਚੌੜਾਈ * 2, gui.char_ਉਚਾਈ,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	ਜੇ (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    ਲੱਦੋ_Xpm_Image((ਅੱਖਰ *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ਨਿਰਾ;
    ਜੇ (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ਨਿਰਾ)
	*psign = sign;

    ਜੇ (!psign)
    {
	ਜੇ (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_ਵਿੱਚ_sign_ਅੰਕੜੇ));
    }
    ਵਾਪਸ (ਖਾਲੀ *)psign;

}

    ਖਾਲੀ
gui_mch_destroy_sign(ਖਾਲੀ *sign)
{
    ਜੇ (sign == ਨਿਰਾ)
	ਵਾਪਸ;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BEVAL_GUI)

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

    ਸਥਿਰ ਖਾਲੀ
make_tooltip(BalloonEval *beval, ਅੱਖਰ *ਪਾਠ, ਬਿੰਦੂ_2 pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    ਜੇ (pti == ਨਿਰਾ)
	ਵਾਪਸ;

    beval->balloon = Create_ਖਿੜਕੀ_Ex_W(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ਨਿਰਾ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->ਨਿਸ਼ਾਨਾ, ਨਿਰਾ, g_hinst, ਨਿਰਾ);

    ਸੈੱਟ_ਖਿੜਕੀ_Pos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_ਮਾਪ = ਆਕਾਰ(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->ਨਿਸ਼ਾਨਾ;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpsz_ਪਾਠ = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_ਮੰਜ਼ਿਲ_utf16((char_u*)ਪਾਠ, ਨਿਰਾ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    ਜੇ (GetClientRect(s_ਪਾਠ_Area, &rect))
	Send_ਸੁਨੇਹਾ_W(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ਸੱਜਾ);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ਖੱਬਾ = pt.x - 3;
    pti->rect.ਸਿਖਰ = pt.y - 3;
    pti->rect.ਸੱਜਾ = pt.x + 3;
    pti->rect.bottom = pt.y + 3;

    Send_ਸੁਨੇਹਾ_W(beval->balloon, TTM_ADDTOOLW, 0, (LPARAM)pti);
    // Make tooltip appear sooner.
    Send_ਸੁਨੇਹਾ_W(beval->balloon, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
    // I've performed some tests and it seems the longest possible life time
    // of tooltip is 30 seconds.
    Send_ਸੁਨੇਹਾ_W(beval->balloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
    /*
     * HACK: force tooltip to appear, because it'll not appear until
     * first mouse move. D*mn M$
     * Amazingly moving (2, 2) and then (-1, -1) the mouse doesn't move.
     */
    mouse_event(MOUSEEVENTF_MOVE, 2, 2, 0, 0);
    mouse_event(MOUSEEVENTF_MOVE, (DWORD)-1, (DWORD)-1, 0, 0);
    vim_free(pti);
}

    ਸਥਿਰ ਖਾਲੀ
delete_tooltip(BalloonEval *beval)
{
    Post_ਸੁਨੇਹਾ(beval->balloon, WM_CLOSE, 0, 0);
}

    ਸਥਿਰ VOID CALLBACK
beval_timer_proc(
    HWND	hwnd ਅਣਵਰਤਿਆ,
    UINT	u_ਸੁਨੇਹਾ ਅਣਵਰਤਿਆ,
    UINT_PTR	idEvent ਅਣਵਰਤਿਆ,
    DWORD	dw_ਸਮਾਂ)
{
    ਬਿੰਦੂ_2	pt;
    RECT	rect;

    ਜੇ (cur_beval == ਨਿਰਾ || cur_beval->showState == ShS_SHOWING || !p_beval)
	ਵਾਪਸ;

    GetCursorPos(&pt);
    ਜੇ (ਖਿੜਕੀ_From_ਬਿੰਦੂ(pt) != s_ਪਾਠ_Area)
	ਵਾਪਸ;

    Screen_ਮੰਜ਼ਿਲ_Client(s_ਪਾਠ_Area, &pt);
    GetClientRect(s_ਪਾਠ_Area, &rect);
    ਜੇ (!Pt_ਵਿੱਚ_Rect(&rect, pt))
	ਵਾਪਸ;

    ਜੇ (ਆਖਰੀ_user_activity > 0
	    && (dw_ਸਮਾਂ - ਆਖਰੀ_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| ਨਿਰਪੇਖ(cur_beval->x - pt.x) > 3
		|| ਨਿਰਪੇਖ(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	ਜੇ (cur_beval->ਸੁਨੇਹਾ_CB != ਨਿਰਾ)
	    (*cur_beval->ਸੁਨੇਹਾ_CB)(cur_beval, 0);
    }
}

    ਖਾਲੀ
gui_mch_disable_beval_area(BalloonEval *beval ਅਣਵਰਤਿਆ)
{
    KillTimer(s_ਪਾਠ_Area, beval_timer_id);
}

    ਖਾਲੀ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ਜੇ (beval == ਨਿਰਾ)
	ਵਾਪਸ;
    beval_timer_id = ਸੈੱਟ_Timer(s_ਪਾਠ_Area, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    ਖਾਲੀ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    ਬਿੰਦੂ_2   pt;

    vim_free(beval->ਸੁਨੇਹਾ);
    beval->ਸੁਨੇਹਾ = mesg == ਨਿਰਾ ? ਨਿਰਾ : vim_strsave(mesg);
    ਜੇ (beval->ਸੁਨੇਹਾ == ਨਿਰਾ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ਵਾਪਸ;
    }

    ਜੇ (beval->showState == ShS_SHOWING)
	ਵਾਪਸ;
    GetCursorPos(&pt);
    Screen_ਮੰਜ਼ਿਲ_Client(s_ਪਾਠ_Area, &pt);

    ਜੇ (ਨਿਰਪੇਖ(beval->x - pt.x) < 3 && ਨਿਰਪੇਖ(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (ਅੱਖਰ *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    ਖਾਲੀ	*ਨਿਸ਼ਾਨਾ ਅਣਵਰਤਿਆ,	// ignored, always use s_textArea
    char_u	*mesg,
    ਖਾਲੀ	(*mesgCB)(BalloonEval *, ਪੂਰਨ),
    ਖਾਲੀ	*client_ਅੰਕੜੇ)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    ਜੇ (mesg != ਨਿਰਾ && mesgCB != ਨਿਰਾ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_ਸੁਨੇਹਾ_and_callback);
	ਵਾਪਸ ਨਿਰਾ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ਜੇ (beval != ਨਿਰਾ)
    {
	beval->ਨਿਸ਼ਾਨਾ = s_ਪਾਠ_Area;

	beval->showState = ShS_NEUTRAL;
	beval->ਸੁਨੇਹਾ = mesg;
	beval->ਸੁਨੇਹਾ_CB = mesgCB;
	beval->client_ਅੰਕੜੇ = client_ਅੰਕੜੇ;

	InitCommonControls();
	cur_beval = beval;

	ਜੇ (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ਵਾਪਸ beval;
}

    ਸਥਿਰ ਖਾਲੀ
Handle_WM_Notify(HWND hwnd ਅਣਵਰਤਿਆ, LPNMHDR pnmh)
{
    ਜੇ (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ਵਾਪਸ;

    ਜੇ (cur_beval == ਨਿਰਾ)
	ਵਾਪਸ;

    ਚੋਣ (pnmh->ਕੋਡ)
    {
	ਹਾਲਤ TTN_SHOW:
	    ਰੋਕੋ;
	ਹਾਲਤ TTN_ਕੱਢੋ: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    ਰੋਕੋ;
	ਹਾਲਤ TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpsz_ਪਾਠ = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ਰੋਕੋ;
	ਹਾਲਤ TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpsz_ਪਾਠ = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ਰੋਕੋ;
    }
}

    ਸਥਿਰ ਖਾਲੀ
track_user_activity(UINT u_ਸੁਨੇਹਾ)
{
    ਜੇ ((u_ਸੁਨੇਹਾ >= WM_MOUSEFIRST && u_ਸੁਨੇਹਾ <= WM_MOUSELAST)
	    || (u_ਸੁਨੇਹਾ >= WM_KEYFIRST && u_ਸੁਨੇਹਾ <= WM_KEYLAST))
	ਆਖਰੀ_user_activity = Get_Tick_ਗਿਣਤੀ();
}

    ਖਾਲੀ
gui_mch_destroy_beval_area(BalloonEval *beval)
{
# ifdef FEAT_VARTABS
    vim_free(beval->vts);
# endif
    vim_free(beval->tofree);
    vim_free(beval);
}
#ਜੇ_ਅੰਤ // FEAT_BEVAL_GUI

#ਜੇ defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    ਖਾਲੀ
netbeans_draw_multisign_indicator(ਪੂਰਨ ਪੰਕਤੀ)
{
    ਪੂਰਨ i;
    ਪੂਰਨ y;
    ਪੂਰਨ x;

    ਜੇ (!netbeans_ਸਰਗਰਮ())
	ਵਾਪਸ;

    x = 0;
    y = ਪਾਠ_Y(ਪੰਕਤੀ);

# ਜੇ defined(FEAT_DIRECTX)
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ਲਈ (i = 0; i < gui.char_ਉਚਾਈ - 3; i++)
	ਸੈੱਟ_Pixel(s_hdc, x+2, y++, gui.currFgColor);

    ਸੈੱਟ_Pixel(s_hdc, x+0, y, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+4, y++, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+1, y, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+3, y++, gui.currFgColor);
    ਸੈੱਟ_Pixel(s_hdc, x+2, y, gui.currFgColor);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ਸਥਿਰ ਪੂਰਨ
test_gui_w32_sendevent_mouse(dict_T *ਦਲੀਲਾਂ)
{
    ਜੇ (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "row") || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "col"))
	ਵਾਪਸ FALSE;

    // Note: "move" is optional, requires fewer arguments
    ਪੂਰਨ move = (ਪੂਰਨ)dict_get_bool(ਦਲੀਲਾਂ, "move", FALSE);

    ਜੇ (!move && (!dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "button")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "multiclick")
	    || !dict_has_ਕੁੰਜੀ(ਦਲੀਲਾਂ, "modifiers")))
	ਵਾਪਸ FALSE;

    ਪੂਰਨ ਪੰਕਤੀ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "row");
    ਪੂਰਨ ਖਾਨਾ = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "col");

    ਜੇ (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ਜੇ (dict_get_bool(ਦਲੀਲਾਂ, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    ਪੂਰਨ pY = (ਪੰਕਤੀ - 1) * gui.char_ਉਚਾਈ + gui.char_ਉਚਾਈ / 2;
	    ਪੂਰਨ pX = (ਖਾਨਾ - 1) * gui.char_ਚੌੜਾਈ + gui.char_ਚੌੜਾਈ / 2;
	    gui_mouse_moved(pX, pY);
	}
	ਨਹੀਂਤਾਂ
	    gui_mouse_moved(ਖਾਨਾ, ਪੰਕਤੀ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ button = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "button");
	ਪੂਰਨ repeated_click = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "multiclick");
	int_u mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ਸੈੱਟ_hor_scroll_ਕਦਮ(6);
	mouse_ਸੈੱਟ_vert_scroll_ਕਦਮ(3);

	gui_send_mouse_event(button, ਪਾਠ_X(ਖਾਨਾ - 1), ਪਾਠ_Y(ਪੰਕਤੀ - 1),
							repeated_click, mods);
    }
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
test_gui_w32_sendevent_keyboard(dict_T *ਦਲੀਲਾਂ)
{
    ਨਿਵੇਸ਼_2 inputs[1];
    ਨਿਵੇਸ਼_2 modkeys[3];
    Secure_Zero_ਸਮ੍ਰਿਤੀ(inputs, ਆਕਾਰ(ਨਿਵੇਸ਼_2));
    Secure_Zero_ਸਮ੍ਰਿਤੀ(modkeys, 3 * ਆਕਾਰ(ਨਿਵੇਸ਼_2));

    char_u *event = dict_get_string(ਦਲੀਲਾਂ, "event", TRUE);

    ਜੇ (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	ਸ਼ਬਦ_2 vk_ਕੋਡ = dict_get_ਸੰਖਿਆ_def(ਦਲੀਲਾਂ, "keycode", 0);
	ਜੇ (vk_ਕੋਡ <= 0 || vk_ਕੋਡ >= 0xFF)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_nr), (ਲੰਮਾ)vk_ਕੋਡ);
	    ਵਾਪਸ FALSE;
	}

	BOOL is_Mod_ਕੁੰਜੀ = (vk_ਕੋਡ == VK_ਖਿਸਕਾਓ || vk_ਕੋਡ == VK_CONTROL
	    || vk_ਕੋਡ == VK_MENU || vk_ਕੋਡ == VK_LSHIFT || vk_ਕੋਡ == VK_RSHIFT
	    || vk_ਕੋਡ == VK_LCONTROL || vk_ਕੋਡ == VK_RCONTROL
	    || vk_ਕੋਡ == VK_LMENU || vk_ਕੋਡ == VK_RMENU );

	BOOL unwrapMods = FALSE;
	ਪੂਰਨ mods = (ਪੂਰਨ)dict_get_ਸੰਖਿਆ(ਦਲੀਲਾਂ, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	ਜੇ (mods && STRICMP(event, "keydown") == 0 && !is_Mod_ਕੁੰਜੀ)
	{
	    ਪੂਰਨ n = 0;
	    ਜੇ (mods & MOD_ਨਕਾਬ_ਖਿਸਕਾਓ)
	    {
		modkeys[n].type = ਨਿਵੇਸ਼_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    ਜੇ (mods & MOD_ਨਕਾਬ_CTRL)
	    {
		modkeys[n].type = ਨਿਵੇਸ਼_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    ਜੇ (mods & MOD_ਨਕਾਬ_ALT)
	    {
		modkeys[n].type = ਨਿਵੇਸ਼_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    ਜੇ (n)
	    {
		(ਖਾਲੀ)ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
		Send_ਨਿਵੇਸ਼(n, modkeys, ਆਕਾਰ(ਨਿਵੇਸ਼_2));
	    }
	}

	inputs[0].type = ਨਿਵੇਸ਼_KEYBOARD;
	inputs[0].ki.wVk = vk_ਕੋਡ;
	ਜੇ (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    ਜੇ (!is_Mod_ਕੁੰਜੀ)
		unwrapMods = TRUE;
	}

	(ਖਾਲੀ)ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
	Send_ਨਿਵੇਸ਼(ARRAYSIZE(inputs), inputs, ਆਕਾਰ(ਨਿਵੇਸ਼_2));
	vim_free(event);

	ਜੇ (unwrapMods)
	{
	    modkeys[0].type = ਨਿਵੇਸ਼_KEYBOARD;
	    modkeys[0].ki.wVk = VK_LSHIFT;
	    modkeys[0].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[1].type = ਨਿਵੇਸ਼_KEYBOARD;
	    modkeys[1].ki.wVk = VK_LCONTROL;
	    modkeys[1].ki.dwFlags = KEYEVENTF_KEYUP;

	    modkeys[2].type = ਨਿਵੇਸ਼_KEYBOARD;
	    modkeys[2].ki.wVk = VK_LMENU;
	    modkeys[2].ki.dwFlags = KEYEVENTF_KEYUP;

	    (ਖਾਲੀ)ਸੈੱਟ_Foreground_ਖਿੜਕੀ(s_hwnd);
	    Send_ਨਿਵੇਸ਼(3, modkeys, ਆਕਾਰ(ਨਿਵੇਸ਼_2));
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (event == ਨਿਰਾ)
	{
	    semsg(_(e_missing_ਦਲੀਲ_str), "event");
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "event", event);
	    vim_free(event);
	}
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ ਪੂਰਨ
test_gui_w32_sendevent_ਸੈੱਟ_keycode_trans_strategy(dict_T *ਦਲੀਲਾਂ)
{
    ਪੂਰਨ handled = 0;
    char_u *strategy = dict_get_string(ਦਲੀਲਾਂ, "strategy", TRUE);

    ਜੇ (strategy)
    {
	ਜੇ (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_ਵਰਤਿਆ = &keycode_trans_strategy_classic;
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_ਵਰਤਿਆ = &keycode_trans_strategy_experimental;
	}
    }

    ਜੇ (!handled)
    {
	ਜੇ (strategy == ਨਿਰਾ)
	{
	    semsg(_(e_missing_ਦਲੀਲ_str), "strategy");
	}
	ਨਹੀਂਤਾਂ
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}


    ਪੂਰਨ
test_gui_w32_sendevent(char_u *event, dict_T *ਦਲੀਲਾਂ)
{
    ਜੇ (STRICMP(event, "key") == 0)
	ਵਾਪਸ test_gui_w32_sendevent_keyboard(ਦਲੀਲਾਂ);
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(event, "mouse") == 0)
	ਵਾਪਸ test_gui_w32_sendevent_mouse(ਦਲੀਲਾਂ);
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ਵਾਪਸ test_gui_w32_sendevent_ਸੈੱਟ_keycode_trans_strategy(ਦਲੀਲਾਂ);
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str_str), "event", event);
	ਵਾਪਸ FALSE;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_IMAGE_GUI)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
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
    ਸਥਿਰ ਖਾਲੀ
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, ਪੂਰਨ iw, ਪੂਰਨ ih, image_format_T fmt)
{
    ਪੂਰਨ		y, x;
    ਪੂਰਨ		stride = iw * 4;
    ਪੂਰਨ		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.ਪਿੱਠ_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    ਲਈ (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	ਲਈ (x = 0; x < iw; x++)
	{
	    ਜੇ (fmt == IMAGE_FORMAT_RGBA)
	    {
		ਪੂਰਨ a = s[3];

		ਜੇ (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		ਨਹੀਂਤਾਂ ਜੇ (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		ਨਹੀਂਤਾਂ
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    ਨਹੀਂਤਾਂ
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

    ਪੂਰਨ
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    ਪੂਰਨ		iw, ih;
    ਖਾਲੀ	*ਬਿੱਟ = ਨਿਰਾ;

    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_ਖੇਤਰ(bmi);
    bmi.bmi_ਸਿਰਲੇਖ.bi_ਮਾਪ = ਆਕਾਰ(BITMAPINFOHEADER);
    bmi.bmi_ਸਿਰਲੇਖ.bi_ਚੌੜਾਈ = iw;
    bmi.bmi_ਸਿਰਲੇਖ.bi_ਉਚਾਈ = -ih; // Top-down
    bmi.bmi_ਸਿਰਲੇਖ.biPlanes = 1;
    bmi.bmi_ਸਿਰਲੇਖ.bi_ਦੋਆਧਾਰੀ_ਅੰਕ_ਗਿਣਤੀ = 32;
    bmi.bmi_ਸਿਰਲੇਖ.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &ਬਿੱਟ, ਨਿਰਾ, 0);
    ਜੇ (hbm == ਨਿਰਾ || ਬਿੱਟ == ਨਿਰਾ)
    {
	ਜੇ (hbm != ਨਿਰਾ)
	    DeleteObject(hbm);
	ਵਾਪਸ false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    ਜੇ (mem_dc == ਨਿਰਾ)
    {
	DeleteObject(hbm);
	ਵਾਪਸ false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)ਬਿੱਟ, img->ਅੰਕੜੇ, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_ਅੰਕੜੇ = ctx;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_ਅੰਕੜੇ;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    ਪੂਰਨ
image_placement_gui_init(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
image_placement_gui_uninit(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
}

    ਖਾਲੀ
image_placement_gui_draw(image_placement_T *ਰੱਖੋ)
{
    image_T	    *img = ਰੱਖੋ->img;
    image_gdi_T	    *ictx = img->backend_ਅੰਕੜੇ;
    pixman_box32_t  *rects;
    ਪੂਰਨ		    n_rects;

    rects = pixman_region32_rectangles(&ਰੱਖੋ->visible, &n_rects);
    ਜੇ (rects == ਨਿਰਾ || n_rects == 0)
	ਵਾਪਸ;

# ਜੇ defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    ਜੇ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ਲਈ (ਪੂਰਨ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;
	ਪੂਰਨ		posx, posy;
	ਪੂਰਨ		x, y, w, h;

	image_placement_subrect(ਰੱਖੋ, rect, &ਪੰਕਤੀ, &ਖਾਨਾ, &x, &y, &w, &h, true);
	posx = FILL_X(ਖਾਨਾ);
	posy = FILL_Y(ਪੰਕਤੀ);

	ਦੋਆਧਾਰੀ_ਅੰਕ_Blt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    ਖਾਲੀ
image_placement_gui_clear(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
}

#ਜੇ_ਅੰਤ // FEAT_IMAGE_GUI
