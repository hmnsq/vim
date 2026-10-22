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

#ಆದರೆ defined(FEAT_DIRECTX)
# include "gui_dwrite.h"
#ಆದರೆ_ಅಂತ್ಯ

// values for "dead_key"
#ವ್ಯಾಖ್ಯಾನಿಸು DEAD_ಕೀಲಿ_OFF			0	// no dead key
#ವ್ಯಾಖ್ಯಾನಿಸು DEAD_ಕೀಲಿ_ಗಣ_DEFAULT		1	// dead key pressed
#ವ್ಯಾಖ್ಯಾನಿಸು DEAD_ಕೀಲಿ_TRANSIENT_ಒಳ_ON_CHAR	2	// wait for next key press
#ವ್ಯಾಖ್ಯಾನಿಸು DEAD_ಕೀಲಿ_SKIP_ON_CHAR		3	// skip next _OnChar()

#ಆದರೆ defined(FEAT_DIRECTX)
ಸ್ಥಿರ DWriteContext *s_dwc = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_directx_enabled = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ s_directx_ತುಂಬು_attempted = 0;
# define IS_ENABLE_DIRECTX() (s_directx_enabled && s_dwc != ಖಾಲಿ && enc_utf8)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ directx_enabled(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ directx_binddc(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_mswin_get_menu_ಎತ್ತರ(ಪೂರ್ಣಾಂಕ fix_ಕಿಟಕಿ);
#ಇಲ್ಲದಿದ್ದರೆ
# define gui_mswin_get_menu_ಎತ್ತರ(fix_ಕಿಟಕಿ)	0
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ keycode_trans_strategy {
    ಶೂನ್ಯ (*ptr_on_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
    ಶೂನ್ಯ (*ptr_on_sys_char) (HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
    ಶೂನ್ಯ (*ptr_process_ಸಂದೇಶ_usual_ಕೀಲಿ) (UINT /*vk*/, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3* /*pmsg*/);
    ಪೂರ್ಣಾಂಕ  (*ptr_get_ಸಕ್ರಿಯ_modifiers)(ಶೂನ್ಯ);
    ಪೂರ್ಣಾಂಕ  (*is_experimental)(ಶೂನ್ಯ);
} keycode_trans_strategy;

// forward declarations for input instance initializer
ಸ್ಥಿರ ಶೂನ್ಯ _OnChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ _OnSysChar_experimental(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ process_ಸಂದೇಶ_usual_ಕೀಲಿ_experimental(UINT /*vk*/, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3* /*pmsg*/);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  get_ಸಕ್ರಿಯ_modifiers_experimental(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  is_experimental_true(ಶೂನ್ಯ);

keycode_trans_strategy keycode_trans_strategy_experimental = {
      _OnChar_experimental      // ptr_on_char
    , _OnSysChar_experimental // ptr_on_sys_char
    , process_ಸಂದೇಶ_usual_ಕೀಲಿ_experimental // ptr_process_message_usual_key
    , get_ಸಕ್ರಿಯ_modifiers_experimental
    , is_experimental_true
};

// forward declarations for input instance initializer
ಸ್ಥಿರ ಶೂನ್ಯ _OnChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ _OnSysChar_classic(HWND /*hwnd UNUSED*/, UINT /*cch*/, ಪೂರ್ಣಾಂಕ /*cRepeat UNUSED*/);
ಸ್ಥಿರ ಶೂನ್ಯ process_ಸಂದೇಶ_usual_ಕೀಲಿ_classic(UINT /*vk*/, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3* /*pmsg*/);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  get_ಸಕ್ರಿಯ_modifiers_classic(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  is_experimental_false(ಶೂನ್ಯ);

keycode_trans_strategy keycode_trans_strategy_classic = {
      _OnChar_classic      // ptr_on_char
    , _OnSysChar_classic // ptr_on_sys_char
    , process_ಸಂದೇಶ_usual_ಕೀಲಿ_classic // ptr_process_message_usual_key
    , get_ಸಕ್ರಿಯ_modifiers_classic
    , is_experimental_false
};

keycode_trans_strategy *keycode_trans_strategy_ಬಳಸಿದ = ಖಾಲಿ;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_experimental_true(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_experimental_false(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Initialize the keycode translation strategy.
 */
ಸ್ಥಿರ ಶೂನ್ಯ keycode_trans_strategy_init(ಶೂನ್ಯ)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strategy = ಖಾಲಿ;

    // set default value as fallback
    keycode_trans_strategy_ಬಳಸಿದ = &keycode_trans_strategy_classic;

    strategy = getenv("VIM_KEYCODE_TRANS_STRATEGY");
    ಆದರೆ (strategy == ಖಾಲಿ)
    {
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (STRICMP(strategy, "classic") == 0)
    {
	keycode_trans_strategy_ಬಳಸಿದ = &keycode_trans_strategy_classic;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (STRICMP(strategy, "experimental") == 0)
    {
	keycode_trans_strategy_ಬಳಸಿದ = &keycode_trans_strategy_experimental;
	ಹಿಂತಿರುಗಿ;
    }

}

#ಆದರೆ defined(FEAT_RENDER_OPTIONS)
    ಪೂರ್ಣಾಂಕ
gui_mch_ಗಣ_rendering_options(char_u *s)
{
# ifdef FEAT_DIRECTX
    char_u  *p, *q;

    ಪೂರ್ಣಾಂಕ	    dx_enable = 0;
    ಪೂರ್ಣಾಂಕ	    dx_flags = 0;
    ದಶಮಾಂಶ   dx_gamma = 0.0f;
    ದಶಮಾಂಶ   dx_contrast = 0.0f;
    ದಶಮಾಂಶ   dx_level = 0.0f;
    ಪೂರ್ಣಾಂಕ	    dx_geom = 0;
    ಪೂರ್ಣಾಂಕ	    dx_renmode = 0;
    ಪೂರ್ಣಾಂಕ	    dx_taamode = 0;
    // parse string as rendering options.
    ಗಾಗಿ (p = s; p != ಖಾಲಿ && *p != NUL; )
    {
	char_u  ಅಂಶ[256];
	char_u  name[128];
	char_u  ಮೌಲ್ಯ[128];

	copy_option_part(&p, ಅಂಶ, ಗಾತ್ರ(ಅಂಶ), ",");
	ಆದರೆ (p == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	q = &ಅಂಶ[0];
	copy_option_part(&q, name, ಗಾತ್ರ(name), ":");
	ಆದರೆ (q == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	copy_option_part(&q, ಮೌಲ್ಯ, ಗಾತ್ರ(ಮೌಲ್ಯ), ":");

	ಆದರೆ (STRCMP(name, "type") == 0)
	{
	    ಆದರೆ (STRCMP(ಮೌಲ್ಯ, "directx") == 0)
		dx_enable = 1;
	    ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "gamma") == 0)
	{
	    dx_flags |= 1 << 0;
	    dx_gamma = (ದಶಮಾಂಶ)atof((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "contrast") == 0)
	{
	    dx_flags |= 1 << 1;
	    dx_contrast = (ದಶಮಾಂಶ)atof((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "level") == 0)
	{
	    dx_flags |= 1 << 2;
	    dx_level = (ದಶಮಾಂಶ)atof((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "geom") == 0)
	{
	    dx_flags |= 1 << 3;
	    dx_geom = atoi((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	    ಆದರೆ (dx_geom < 0 || dx_geom > 2)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "renmode") == 0)
	{
	    dx_flags |= 1 << 4;
	    dx_renmode = atoi((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	    ಆದರೆ (dx_renmode < 0 || dx_renmode > 6)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "taamode") == 0)
	{
	    dx_flags |= 1 << 5;
	    dx_taamode = atoi((ಅಕ್ಷರ *)ಮೌಲ್ಯ);
	    ಆದರೆ (dx_taamode < 0 || dx_taamode > 3)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "scrlines") == 0)
	{
	    // Deprecated.  Simply ignore it.
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ OK;  // only checking the syntax of the value

    // Enable DirectX/DirectWrite
    ಆದರೆ (dx_enable)
    {
	ಆದರೆ (!directx_enabled())
	    ಹಿಂತಿರುಗಿ FAIL;
	DWrite_Context_ಗಣ_Rendering_Params(s_dwc, ಖಾಲಿ);
	ಆದರೆ (dx_flags)
	{
	    DWriteRenderingParams param;
	    DWriteContext_GetRenderingParams(s_dwc, &param);
	    ಆದರೆ (dx_flags & (1 << 0))
		param.gamma = dx_gamma;
	    ಆದರೆ (dx_flags & (1 << 1))
		param.enhancedContrast = dx_contrast;
	    ಆದರೆ (dx_flags & (1 << 2))
		param.clearTypeLevel = dx_level;
	    ಆದರೆ (dx_flags & (1 << 3))
		param.pixelGeometry = dx_geom;
	    ಆದರೆ (dx_flags & (1 << 4))
		param.rendering_ಬಹುಲಕ = dx_renmode;
	    ಆದರೆ (dx_flags & (1 << 5))
		param.ಪಠ್ಯ_Antialias_ಬಹುಲಕ = dx_taamode;
	    DWrite_Context_ಗಣ_Rendering_Params(s_dwc, &param);
	}
    }
    s_directx_enabled = dx_enable;
    gui.directx_enabled = IS_ENABLE_DIRECTX();

    ಹಿಂತಿರುಗಿ OK;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * These are new in Windows ME/XP, only defined in recent compilers.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HANDLE_WM_XBUTTONUP
# define HANDLE_WM_XBUTTONUP(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)LOWORD(lParam), (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HANDLE_WM_XBUTTONDOWN
# define HANDLE_WM_XBUTTONDOWN(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), FALSE, (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)LOWORD(lParam), (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HANDLE_WM_XBUTTONDBLCLK
# define HANDLE_WM_XBUTTONDBLCLK(hwnd, wParam, lParam, fn) \
   ((fn)((hwnd), TRUE, (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)LOWORD(lParam), (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)HIWORD(lParam), (UINT)(wParam)), 0L)
#ಆದರೆ_ಅಂತ್ಯ


#ಅನ್ತರ್ಭಾವ "version.h"	// used by dialog box routine for default title
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
# include <tchar.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <shellapi.h>
#ಅನ್ತರ್ಭಾವ <commctrl.h>
#ಅನ್ತರ್ಭಾವ <windowsx.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# define MENUHINTS		// show menu hints in command line
#ಆದರೆ_ಅಂತ್ಯ

// Some parameters for dialog boxes.  All in pixels.
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_PADDING_X		10
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_PADDING_Y		10
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_VERT_PADDING_X	4	// For vertical buttons
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_VERT_PADDING_Y	4
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_ICON_ಅಗಲ		34
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_ICON_ಎತ್ತರ		34
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_MIN_ಅಗಲ		150
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_FONT_NAME		"MS Shell Dlg"
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_FONT_ಬಿಂದು_ಗಾತ್ರ	8
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_MIN_MAX_ಅಗಲ	400
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_MIN_MAX_ಎತ್ತರ	400

#ವ್ಯಾಖ್ಯಾನಿಸು DLG_NONBUTTON_CONTROL	5000	// First ID of non-button controls

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WM_DPICHANGED
# define WM_DPICHANGED			0x02E0
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WM_GETDPISCALEDSIZE
# define WM_GETDPISCALEDSIZE		0x02E4
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ WM_MOUSEHWHEEL
# define WM_MOUSEHWHEEL			0x020E
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SPI_GETWHEELSCROLLCHARS
# define SPI_GETWHEELSCROLLCHARS	0x006C
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SPI_SETWHEELSCROLLCHARS
# define SPI_SETWHEELSCROLLCHARS	0x006D
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DWMWA_USE_IMMERSIVE_DARK_ಬಹುಲಕ
# define DWMWA_USE_IMMERSIVE_DARK_ಬಹುಲಕ	20
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DWMWA_CAPTION_COLOR
# define DWMWA_CAPTION_COLOR		35
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DWMWA_ಪಠ್ಯ_COLOR
# define DWMWA_ಪಠ್ಯ_COLOR		36
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ _OnPaint(HWND hwnd);
ಸ್ಥಿರ ಶೂನ್ಯ fill_rect(ಸ್ಥಿರಾಂಕ RECT *rcp, HBRUSH hbr, COLORREF color);
ಸ್ಥಿರ ಶೂನ್ಯ clear_rect(RECT *rcp);

ಸ್ಥಿರ ಶಬ್ದ_2		s_dlgfntheight;		// height of the dialog font
ಸ್ಥಿರ ಶಬ್ದ_2		s_dlgfntwidth;		// width of the dialog font

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಸ್ಥಿರ HMENU		s_menuBar = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TEAROFF
ಸ್ಥಿರ ಶೂನ್ಯ rebuild_tearoff(vimmenu_T *menu);
ಸ್ಥಿರ HBITMAP	s_htearbitmap;	    // bitmap used to indicate tearoff
#ಆದರೆ_ಅಂತ್ಯ

// Flag that is set while processing a message that must not be interrupted by
// processing another message.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		s_busy_processing = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		destroying = FALSE;	// call DestroyWindow() ourselves

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
ಸ್ಥಿರ UINT		s_findrep_ಸಂದೇಶ = 0;
ಸ್ಥಿರ FINDREPLACEW	s_findrep_struct;
ಸ್ಥಿರ HWND		s_findrep_hwnd = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		s_findrep_is_ಕಂಡುಹಿಡಿ;	// TRUE for find dialog, FALSE
						// for find/replace dialog
#ಆದರೆ_ಅಂತ್ಯ

HWND			s_hwnd = ಖಾಲಿ;
ಸ್ಥಿರ HDC		s_hdc = ಖಾಲಿ;
ಸ್ಥಿರ HBRUSH		s_brush = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
ಸ್ಥಿರ HWND		s_toolbarhwnd = ಖಾಲಿ;
ಸ್ಥಿರ WNDPROC		s_toolbar_wndproc = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
ಸ್ಥಿರ HWND		s_tabhwnd = ಖಾಲಿ;
ಸ್ಥಿರ WNDPROC		s_tabline_wndproc = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		showing_tabline = 0;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ WPARAM		s_wParam = 0;

ಸ್ಥಿರ HWND		s_ಪಠ್ಯ_Area = ಖಾಲಿ;

// Set when the text area was painted with the background color only, because
// the screen contents were not available yet.
ಸ್ಥಿರ bool		s_textarea_bg_only = false;

ಸ್ಥಿರ UINT		s_u_ಸಂದೇಶ = 0;

ಸ್ಥಿರ char_u		*s_textfield; // Used by dialogs to pass back strings

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		s_need_activate = FALSE;

// This variable is set when waiting for an event, which is the only moment
// scrollbar dragging can be done directly.  It's not allowed while commands
// are executed, because it may move the cursor and that may cause unexpected
// problems (e.g., while ":s" is working).
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ allow_scrollbar = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ _DPI_AWARENESS_CONTEXTS_
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HANDLE DPI_AWARENESS_CONTEXT;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ DPI_AWARENESS {
    DPI_AWARENESS_INVALID	    = -1,
    DPI_AWARENESS_UNAWARE	    = 0,
    DPI_AWARENESS_SYSTEM_AWARE	    = 1,
    DPI_AWARENESS_ಪ್ರತಿ_MONITOR_AWARE = 2
} DPI_AWARENESS;

# define DPI_AWARENESS_CONTEXT_UNAWARE		    ((DPI_AWARENESS_CONTEXT)-1)
# define DPI_AWARENESS_CONTEXT_SYSTEM_AWARE	    ((DPI_AWARENESS_CONTEXT)-2)
# define DPI_AWARENESS_CONTEXT_ಪ್ರತಿ_MONITOR_AWARE    ((DPI_AWARENESS_CONTEXT)-3)
# define DPI_AWARENESS_CONTEXT_ಪ್ರತಿ_MONITOR_AWARE_V2 ((DPI_AWARENESS_CONTEXT)-4)
# define DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED    ((DPI_AWARENESS_CONTEXT)-5)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_DPI	96
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		s_dpi = DEFAULT_DPI;
ಸ್ಥಿರ BOOL		s_ಒಳ_dpichanged = FALSE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
ಸ್ಥಿರ DPI_AWARENESS	s_process_dpi_aware = DPI_AWARENESS_INVALID;
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ RECT		s_suggested_rect;

ಸ್ಥಿರ UINT (WINAPI *pGetDpiForSystem)(ಶೂನ್ಯ) = ಖಾಲಿ;
ಸ್ಥಿರ UINT (WINAPI *p_Get_Dpi_For_ಕಿಟಕಿ)(HWND hwnd) = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (WINAPI *pGetSystemMetricsForDpi)(ಪೂರ್ಣಾಂಕ, UINT) = ಖಾಲಿ;
//static INT (WINAPI *pGetWindowDpiAwarenessContext)(HWND hwnd) = NULL;
ಸ್ಥಿರ DPI_AWARENESS_CONTEXT (WINAPI *p_ಗಣ_Thread_Dpi_Awareness_Context)(DPI_AWARENESS_CONTEXT dpiContext) = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
ಸ್ಥಿರ DPI_AWARENESS (WINAPI *pGetAwarenessFromDpiAwarenessContext)(DPI_AWARENESS_CONTEXT) = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ HINSTANCE hLibDwm = ಖಾಲಿ;
ಸ್ಥಿರ HRESULT (WINAPI *p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute)(HWND, DWORD, LPCVOID, DWORD);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_dwm_ತುಂಬು(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fullscreen_on = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME

ಸ್ಥಿರ HINSTANCE hUxThemeLib = ಖಾಲಿ;
ಸ್ಥಿರ DWORD (WINAPI *p_ಗಣ_Preferred_App_ಬಹುಲಕ)(DWORD) = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ (WINAPI *pFlushMenuThemes)(ಶೂನ್ಯ) = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ dyn_uxtheme_ತುಂಬು(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ WINAPI
stubGetSystemMetricsForDpi(ಪೂರ್ಣಾಂಕ n_ಸೂಚ್ಯಂಕ, UINT dpi ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ GetSystemMetrics(n_ಸೂಚ್ಯಂಕ);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
adjust_fontsize_by_dpi(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 * s_dpi / (ಪೂರ್ಣಾಂಕ)pGetDpiForSystem();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
adjust_by_system_dpi(ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2)
{
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 * (ಪೂರ್ಣಾಂಕ)pGetDpiForSystem() / DEFAULT_DPI;
}

#ಆದರೆ defined(FEAT_DIRECTX)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
directx_enabled(ಶೂನ್ಯ)
{
    ಆದರೆ (s_dwc != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_directx_ತುಂಬು_attempted)
	ಹಿಂತಿರುಗಿ 0;
    // load DirectX
    DWrite_Init();
    s_directx_ತುಂಬು_attempted = 1;
    s_dwc = DWriteContext_Open();
    directx_binddc();
    ಹಿಂತಿರುಗಿ s_dwc != ಖಾಲಿ ? 1 : 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
directx_binddc(ಶೂನ್ಯ)
{
    ಆದರೆ (s_ಪಠ್ಯ_Area == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    RECT	rect;
    GetClientRect(s_ಪಠ್ಯ_Area, &rect);
    DWriteContext_BindDC(s_dwc, s_hdc, &rect);
}
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ current_font_ಎತ್ತರ;	    // this is in os_mswin.c

ಸ್ಥಿರ ರಚನೆ
{
    UINT    ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {VK_ಮೇಲೆ,		'k', 'u'},
    {VK_ಕೆಳಕ್ಕೆ,		'k', 'd'},
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    {VK_PAUSE,		'F', 'B'},	// Pause == F21 (see gui_gtk_x11.c)
#ಆದರೆ_ಅಂತ್ಯ
    {VK_F22,		'F', 'C'},
    {VK_F23,		'F', 'D'},
    {VK_F24,		'F', 'E'},	// winuser.h defines up to F24

    {VK_HELP,		'%', '1'},
    {VK_ಹಿಂಭಾಗ,		'k', 'b'},
    {VK_ಸೇರಿಸು,		'k', 'I'},
    {VK_DELETE,		'k', 'D'},
    {VK_HOME,		'k', 'h'},
    {VK_ಕೊನೆ,		'@', '7'},
    {VK_ಪೂರ್ವ,		'k', 'P'},
    {VK_ಮುಂದಿನ,		'k', 'N'},
    {VK_PRINT,		'%', '9'},
    {VK_ಕೂಡಿಸು,		'K', '6'},
    {VK_ಕಳೆ,	'K', '7'},
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	s_button_pending = -1;

// s_getting_focus is set when we got focus but didn't see mouse-up event yet,
// so don't reset s_button_pending.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	s_getting_focus = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	s_x_pending;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	s_y_pending;
ಸ್ಥಿರ UINT	s_kFlags_pending;
ಸ್ಥಿರ UINT_PTR	s_wait_timer = 0;	  // Timer for get char from user
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	s_timed_ಹೊರಹೋಗು = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
ಸ್ಥಿರ UINT	surrogate_pending_ch = 0; // 0: no surrogate pending,
					  // else a high surrogate

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
// balloon-eval WM_NOTIFY_HANDLER
ಸ್ಥಿರ ಶೂನ್ಯ Handle_WM_Notify(HWND hwnd, LPNMHDR pnmh);
ಸ್ಥಿರ ಶೂನ್ಯ track_user_activity(UINT u_ಸಂದೇಶ);
#ಆದರೆ_ಅಂತ್ಯ

/*
 * For control IME.
 *
 * These LOGFONTW used for IME.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
// holds LOGFONTW for 'guifontwide' if available, otherwise 'guifont'
ಸ್ಥಿರ LOGFONTW norm_logfont;
// holds LOGFONTW for 'guifont' always.
ಸ್ಥಿರ LOGFONTW sub_logfont;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
ಸ್ಥಿರ LRESULT _OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BROWSE)
ಸ್ಥಿರ char_u *convert_filter(char_u *s);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_PRINT_ERROR
/*
 * Print out the last Windows error message
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
print_windows_error(ಶೂನ್ಯ)
{
    LPVOID  lp_ಸಂದೇಶ_Buf;

    Format_ಸಂದೇಶ(FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ | FORMAT_ಸಂದೇಶ_FROM_SYSTEM,
		  ಖಾಲಿ, Get_ಕೊನೆಯ_Error(),
		  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		  (LPTSTR) &lp_ಸಂದೇಶ_Buf, 0, ಖಾಲಿ);
    TRACE1("Error: %s\n", lp_ಸಂದೇಶ_Buf);
    LocalFree(lp_ಸಂದೇಶ_Buf);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_NONE  0
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_OFF   1
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_ON    2

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u		blink_waittime = 700;
ಸ್ಥಿರ long_u		blink_ontime = 400;
ಸ್ಥಿರ long_u		blink_offtime = 250;
ಸ್ಥಿರ UINT_PTR		blink_timer = 0;

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_blinking(ದೀರ್ಘ wait, ದೀರ್ಘ on, ದೀರ್ಘ off)
{
    blink_waittime = wait;
    blink_ontime = on;
    blink_offtime = off;
}

    ಸ್ಥಿರ VOID CALLBACK
_OnBlinkTimer(
    HWND hwnd,
    UINT u_ಸಂದೇಶ ಬಳಸದ,
    UINT_PTR idEvent,
    DWORD dw_ಸಮಯ ಬಳಸದ)
{
    ಸಂದೇಶ_3 ಸಂದೇಶ;

    /*
    TRACE2("Got timer event, id %d, blink_timer %d\n", idEvent, blink_timer);
    */

    KillTimer(ಖಾಲಿ, idEvent);

    // Eat spurious WM_TIMER messages
    ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;

    ಆದರೆ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = ಗಣ_Timer(ಖಾಲಿ, 0, (UINT)blink_offtime, _OnBlinkTimer);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = ಗಣ_Timer(ಖಾಲಿ, 0, (UINT)blink_ontime, _OnBlinkTimer);
    }
    gui_mch_flush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mswin_rm_blink_timer(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_3 ಸಂದೇಶ;

    ಆದರೆ (blink_timer == 0)
	ಹಿಂತಿರುಗಿ;

    KillTimer(ಖಾಲಿ, blink_timer);
    // Eat spurious WM_TIMER messages
    ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    blink_timer = 0;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    gui_mswin_rm_blink_timer();
    ಆದರೆ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
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
gui_mch_ಆರಂಭ_blink(ಶೂನ್ಯ)
{
    gui_mswin_rm_blink_timer();

    // Only switch blinking on if none of the times is zero
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	blink_timer = ಗಣ_Timer(ಖಾಲಿ, 0, (UINT)blink_waittime, _OnBlinkTimer);
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
    UINT u_ಸಂದೇಶ ಬಳಸದ,
    UINT_PTR idEvent,
    DWORD dw_ಸಮಯ ಬಳಸದ)
{
    ಸಂದೇಶ_3 ಸಂದೇಶ;

    /*
    TRACE2("Got timer event, id %d, s_wait_timer %d\n", idEvent, s_wait_timer);
    */
    KillTimer(ಖಾಲಿ, idEvent);
    s_timed_ಹೊರಹೋಗು = TRUE;

    // Eat spurious WM_TIMER messages
    ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	;
    ಆದರೆ (idEvent == s_wait_timer)
	s_wait_timer = 0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDeadChar(
    HWND hwnd ಬಳಸದ,
    UINT ch ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_ಗಣ_DEFAULT;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
char_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ ch, char_u *string, ಪೂರ್ಣಾಂಕ slen, ಪೂರ್ಣಾಂಕ had_alt)
{
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    WCHAR	wstring[2];
    char_u	*ws = ಖಾಲಿ;

    ಆದರೆ (surrogate_pending_ch != 0)
    {
	// We don't guarantee ch is a low surrogate to match the high surrogate
	// we already have; it should be, but if it isn't, tough luck.
	wstring[0] = surrogate_pending_ch;
	wstring[1] = ch;
	surrogate_pending_ch = 0;
	len = 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch >= 0xD800 && ch <= 0xDBFF)	// high surrogate
    {
	// We don't have the entire code point yet, only the first UTF-16 code
	// unit; so just remember it and use it in the next call.
	surrogate_pending_ch = ch;
	ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wstring[0] = ch;
	len = 1;
    }

    // "ch" is a UTF-16 character.  Convert it to a string of bytes.  When
    // "enc_codepage" is non-zero use the standard Win32 function,
    // otherwise use our own conversion function (e.g., for UTF-8).
    ಆದರೆ (enc_codepage > 0)
    {
	len = Wide_Char_ಗಮ್ಯ_Multi_Byte(enc_codepage, 0, wstring, len,
		(LPSTR)string, slen, 0, ಖಾಲಿ);
	// If we had included the ALT key into the character but now the
	// upper bit is no longer set, that probably means the conversion
	// failed.  Convert the original character and set the upper bit
	// afterwards.
	ಆದರೆ (had_alt && len == 1 && ch >= 0x80 && string[0] < 0x80)
	{
	    wstring[0] = ch & 0x7f;
	    len = Wide_Char_ಗಮ್ಯ_Multi_Byte(enc_codepage, 0, wstring, len,
		    (LPSTR)string, slen, 0, ಖಾಲಿ);
	    ಆದರೆ (len == 1) // safety check
		string[0] |= 0x80;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ws = utf16_ಗಮ್ಯ_enc(wstring, &len);
	ಆದರೆ (ws == ಖಾಲಿ)
	    len = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (len > slen)	// just in case
		len = slen;
	    mch_memmove(string, ws, len);
	    vim_free(ws);
	}
    }

    ಆದರೆ (len == 0)
    {
	string[0] = ch;
	len = 1;
    }

    ಗಾಗಿ (i = 0; i < len; ++i)
	ಆದರೆ (string[i] == CSI && len <= slen - 2)
	{
	    // Insert CSI as K_CSI.
	    mch_memmove(string + i + 3, string + i + 1, len - i - 1);
	    string[++i] = KS_EXTRA;
	    string[++i] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    len += 2;
	}

    ಹಿಂತಿರುಗಿ len;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಕ್ರಿಯ_modifiers_experimental(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_ಸರಿಸು) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    // Windows handles Ctrl + Alt as AltGr and vice-versa. We can distinguish
    // the two cases by checking whether the left or the right Alt key is
    // pressed.
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ ((modifiers & MOD_ಮುಖವಾಡ_CTRL) && (Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000))
	modifiers &= ~MOD_ಮುಖವಾಡ_CTRL;
    // Add RightALT only if it is hold alone (without Ctrl), because if AltGr
    // is pressed, Windows claims that Ctrl is hold as well. That way we can
    // recognize Right-ALT alone and be sure that not AltGr is hold.
    ಆದರೆ (!(Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	    &&  (Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)) // seems AltGr has both set
	modifiers |= MOD_ಮುಖವಾಡ_ALT;

    ಹಿಂತಿರುಗಿ modifiers;
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಕ್ರಿಯ_modifiers_classic(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (Get_ಕೀಲಿ_State(VK_ಸರಿಸು) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    /*
     * Don't use caps-lock as shift, because these are special keys
     * being considered here, and we only want letters to get
     * shifted -- webb
     */
    /*
    if (GetKeyState(VK_CAPITAL) & 0x0001)
	modifiers ^= MOD_MASK_SHIFT;
    */
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_MENU) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;

    ಹಿಂತಿರುಗಿ modifiers;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಕ್ರಿಯ_modifiers(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_ಸಕ್ರಿಯ_modifiers_experimental();
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಸಕ್ರಿಯ_modifiers_ಮೂಲಕ_ptr(ಶೂನ್ಯ)
{
    // marshal to corresponding implementation
    ಹಿಂತಿರುಗಿ keycode_trans_strategy_ಬಳಸಿದ->ptr_get_ಸಕ್ರಿಯ_modifiers();
}

/*
 * Key hit, add it to the input buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar(
    HWND hwnd ಬಳಸದ,
    UINT cch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ಬಳಸಿದ->ptr_on_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar_experimental(
    HWND hwnd ಬಳಸದ,
    UINT cch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    char_u	string[40];
    ಪೂರ್ಣಾಂಕ		len = 0;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ch = cch;   // special keys are negative

    ಆದರೆ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SKIP_ON_CHAR)
	ಹಿಂತಿರುಗಿ;

    //  keep DEAD_KEY_TRANSIENT_IN_ON_CHAR value for later handling in
    //  process_message()
    ಆದರೆ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_TRANSIENT_ಒಳ_ON_CHAR)
	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;

    modifiers = get_ಸಕ್ರಿಯ_modifiers_experimental();

    ch = simplify_ಕೀಲಿ(ch, &modifiers);

    // Some keys need adjustment when the Ctrl modifier is used.
    ++no_reduce_ಕೀಲಿಗಳು;
    ch = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ch);
    --no_reduce_ಕೀಲಿಗಳು;

    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ಹೊರತೆಗೆ_modifiers(ch, &modifiers, FALSE, ಖಾಲಿ);
    ಆದರೆ (ch == CSI)
	ch = K_CSI;

    ಆದರೆ (modifiers)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = modifiers;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 3);
    }

    len = char_ಗಮ್ಯ_string(ch, string, 40, FALSE);
    ಆದರೆ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ಒಳಹರಿವು_buf();
	got_int = TRUE;
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnChar_classic(
    HWND hwnd ಬಳಸದ,
    UINT ch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    char_u	string[40];
    ಪೂರ್ಣಾಂಕ		len = 0;

    dead_ಕೀಲಿ = 0;

    len = char_ಗಮ್ಯ_string(ch, string, 40, FALSE);
    ಆದರೆ (len == 1 && string[0] == Ctrl_C && ctrl_c_interrupts)
    {
	trash_ಒಳಹರಿವು_buf();
	got_int = TRUE;
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
}

/*
 * Alt-Key hit, add it to the input buffer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar(
    HWND hwnd ಬಳಸದ,
    UINT cch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ಬಳಸಿದ->ptr_on_sys_char(hwnd, cch, cRepeat);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar_experimental(
    HWND hwnd ಬಳಸದ,
    UINT cch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    char_u	string[40]; // Enough for multibyte character
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ch = cch;   // special keys are negative

    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = get_ಸಕ್ರಿಯ_modifiers_experimental();
    ch = simplify_ಕೀಲಿ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ಹೊರತೆಗೆ_modifiers(ch, &modifiers, FALSE, ಖಾಲಿ);
    ಆದರೆ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ಆದರೆ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ಆದರೆ (IS_SPECIAL((ಪೂರ್ಣಾಂಕ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_ಎರಡನೇ((ಪೂರ್ಣಾಂಕ)ch);
	string[len++] = K_ಮೂರನೇ((ಪೂರ್ಣಾಂಕ)ch);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_ಗಮ್ಯ_string(ch, string + len, 40 - len, TRUE);
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnSysChar_classic(
    HWND hwnd ಬಳಸದ,
    UINT cch,
    ಪೂರ್ಣಾಂಕ cRepeat ಬಳಸದ)
{
    char_u	string[40]; // Enough for multibyte character
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ch = cch;   // special keys are negative

    dead_ಕೀಲಿ = 0;

    // TRACE("OnSysChar(%d, %c)\n", ch, ch);

    // OK, we have a character key (given by ch) which was entered with the
    // ALT key pressed. Eg, if the user presses Alt-A, then ch == 'A'. Note
    // that the system distinguishes Alt-a and Alt-A (Alt-Shift-a unless
    // CAPSLOCK is pressed) at this point.
    modifiers = MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_ಸರಿಸು) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;

    ch = simplify_ಕೀಲಿ(ch, &modifiers);
    // remove the SHIFT modifier for keys where it's already included, e.g.,
    // '(' and '*'
    modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ch);

    // Unify modifiers somewhat.  No longer use ALT to set the 8th bit.
    ch = ಹೊರತೆಗೆ_modifiers(ch, &modifiers, FALSE, ಖಾಲಿ);
    ಆದರೆ (ch == CSI)
	ch = K_CSI;

    len = 0;
    ಆದರೆ (modifiers)
    {
	string[len++] = CSI;
	string[len++] = KS_MODIFIER;
	string[len++] = modifiers;
    }

    ಆದರೆ (IS_SPECIAL((ಪೂರ್ಣಾಂಕ)ch))
    {
	string[len++] = CSI;
	string[len++] = K_ಎರಡನೇ((ಪೂರ್ಣಾಂಕ)ch);
	string[len++] = K_ಮೂರನೇ((ಪೂರ್ಣಾಂಕ)ch);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Although the documentation isn't clear about it, we assume "ch" is
	// a Unicode character.
	len += char_ಗಮ್ಯ_string(ch, string + len, 40 - len, TRUE);
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseEvent(
    ಪೂರ್ಣಾಂಕ button,
    ಪೂರ್ಣಾಂಕ x,
    ಪೂರ್ಣಾಂಕ y,
    ಪೂರ್ಣಾಂಕ repeated_click,
    UINT ಕೀಲಿ_Flags)
{
    ಪೂರ್ಣಾಂಕ vim_modifiers = 0x0;

    s_getting_focus = FALSE;

    ಆದರೆ (ಕೀಲಿ_Flags & MK_ಸರಿಸು)
	vim_modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (ಕೀಲಿ_Flags & MK_CONTROL)
	vim_modifiers |= MOUSE_CTRL;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_LMENU) & 0x8000)
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
On_Mouse_Button_ಕೆಳಕ್ಕೆ(
    HWND hwnd ಬಳಸದ,
    BOOL fDoubleClick ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ x,
    ಪೂರ್ಣಾಂಕ y,
    UINT ಕೀಲಿ_Flags)
{
    ಸ್ಥಿರ LONG	s_prev_ಸಮಯ = 0;

    LONG    current_ಸಮಯ = Get_ಸಂದೇಶ_ಸಮಯ();
    ಪೂರ್ಣಾಂಕ	    button = -1;
    ಪೂರ್ಣಾಂಕ	    repeated_click;

    // Give main window the focus: this is so the cursor isn't hollow.
    (ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);

    ಆದರೆ (s_u_ಸಂದೇಶ == WM_LBUTTONDOWN || s_u_ಸಂದೇಶ == WM_LBUTTONDBLCLK)
	button = MOUSE_ಎಡ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_u_ಸಂದೇಶ == WM_MBUTTONDOWN || s_u_ಸಂದೇಶ == WM_MBUTTONDBLCLK)
	button = MOUSE_ಮಧ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_u_ಸಂದೇಶ == WM_RBUTTONDOWN || s_u_ಸಂದೇಶ == WM_RBUTTONDBLCLK)
	button = MOUSE_ಬಲ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_u_ಸಂದೇಶ == WM_XBUTTONDOWN || s_u_ಸಂದೇಶ == WM_XBUTTONDBLCLK)
    {
	button = ((GET_XBUTTON_WPARAM(s_wParam) == 1) ? MOUSE_X1 : MOUSE_X2);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_u_ಸಂದೇಶ == WM_CAPTURECHANGED)
    {
	// on W95/NT4, somehow you get in here with an odd Msg
	// if you press one button while holding down the other..
	ಆದರೆ (s_button_pending == MOUSE_ಎಡ)
	    button = MOUSE_ಬಲ;
	ಇಲ್ಲದಿದ್ದರೆ
	    button = MOUSE_ಎಡ;
    }

    ಆದರೆ (button < 0)
	ಹಿಂತಿರುಗಿ;

    repeated_click = ((ಪೂರ್ಣಾಂಕ)(current_ಸಮಯ - s_prev_ಸಮಯ) < p_mouset);

    /*
     * Holding down the left and right buttons simulates pushing the middle
     * button.
     */
    ಆದರೆ (repeated_click
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
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((repeated_click)
	    || (mouse_model_popup() && (button == MOUSE_ಬಲ)))
    {
	ಆದರೆ (s_button_pending > -1)
	{
	    _OnMouseEvent(s_button_pending, x, y, FALSE, ಕೀಲಿ_Flags);
	    s_button_pending = -1;
	}
	// TRACE("Button down at x %d, y %d\n", x, y);
	_OnMouseEvent(button, x, y, repeated_click, ಕೀಲಿ_Flags);
    }
    ಇಲ್ಲದಿದ್ದರೆ
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

    s_prev_ಸಮಯ = current_ಸಮಯ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseMoveOrRelease(
    HWND hwnd ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ x,
    ಪೂರ್ಣಾಂಕ y,
    UINT ಕೀಲಿ_Flags)
{
    ಪೂರ್ಣಾಂಕ button;

    s_getting_focus = FALSE;
    ಆದರೆ (s_button_pending > -1)
    {
	// Delayed action for mouse down event
	_OnMouseEvent(s_button_pending, s_x_pending,
					s_y_pending, FALSE, s_kFlags_pending);
	s_button_pending = -1;
    }
    ಆದರೆ (s_u_ಸಂದೇಶ == WM_MOUSEMOVE)
    {
	/*
	 * It's only a MOUSE_DRAG if one or more mouse buttons are being held
	 * down.
	 */
	ಆದರೆ (!(ಕೀಲಿ_Flags & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON
						| MK_XBUTTON1 | MK_XBUTTON2)))
	{
	    gui_mouse_moved(x, y);
	    ಹಿಂತಿರುಗಿ;
	}

	/*
	 * While button is down, keep grabbing mouse move events when
	 * the mouse goes outside the window
	 */
	ಗಣ_Capture(s_ಪಠ್ಯ_Area);
	button = MOUSE_DRAG;
	// TRACE("  move at x %d, y %d\n", x, y);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ReleaseCapture();
	button = MOUSE_RELEASE;
	// TRACE("  up at x %d, y %d\n", x, y);
    }

    _OnMouseEvent(button, x, y, FALSE, ಕೀಲಿ_Flags);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಗಾತ್ರ_ಪಠ್ಯ_Area(
    HWND hwnd ಬಳಸದ,
    UINT state ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ cx ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ cy ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	directx_binddc();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
/*
 * Find the vimmenu_T with the given id
 */
    ಸ್ಥಿರ vimmenu_T *
gui_mswin_ಕಂಡುಹಿಡಿ_menu(
    vimmenu_T	*pMenu,
    ಪೂರ್ಣಾಂಕ		id)
{
    vimmenu_T	*p_ಮಗು_Menu;

    ಆಗ (pMenu)
    {
	ಆದರೆ (pMenu->id == (UINT)id)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (pMenu->children != ಖಾಲಿ)
	{
	    p_ಮಗು_Menu = gui_mswin_ಕಂಡುಹಿಡಿ_menu(pMenu->children, id);
	    ಆದರೆ (p_ಮಗು_Menu)
	    {
		pMenu = p_ಮಗು_Menu;
		ನಿಲ್ಲಿಸು;
	    }
	}
	pMenu = pMenu->ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ pMenu;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMenu(
    HWND	hwnd ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		id,
    HWND	hwndCtl ಬಳಸದ,
    UINT	ಸಂಕೇತ_Notify ಬಳಸದ)
{
    vimmenu_T	*pMenu;

    pMenu = gui_mswin_ಕಂಡುಹಿಡಿ_menu(ಬೇರು_menu, id);
    ಆದರೆ (pMenu)
	gui_menu_cb(pMenu);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
/*
 * Handle a Find/Replace window message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಕಂಡುಹಿಡಿ_Repl(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ	    flags = 0;
    ಪೂರ್ಣಾಂಕ	    ಕೆಳಕ್ಕೆ;

    ಆದರೆ (s_findrep_struct.Flags & FR_DIALOGTERM)
	// Give main window the focus back.
	(ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);

    ಆದರೆ (s_findrep_struct.Flags & FR_FINDNEXT)
    {
	flags = FRD_FINDNEXT;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_findrep_struct.Flags & FR_REPLACE)
    {
	flags = FRD_REPLACE;

	// Give main window the focus back: this is so the cursor isn't
	// hollow.
	(ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (s_findrep_struct.Flags & FR_REPLACEALL)
    {
	flags = FRD_REPLACEALL;
    }

    ಆದರೆ (flags == 0)
	ಹಿಂತಿರುಗಿ;

    char_u	*p, *q;

    // Call the generic GUI function to do the actual work.
    ಆದರೆ (s_findrep_struct.Flags & FR_WHOLEWORD)
	flags |= FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ;
    ಆದರೆ (s_findrep_struct.Flags & FR_MATCHCASE)
	flags |= FRD_ಹೊಂದಿಕೆ_CASE;
    ಕೆಳಕ್ಕೆ = (s_findrep_struct.Flags & FR_ಕೆಳಕ್ಕೆ) != 0;
    p = utf16_ಗಮ್ಯ_enc(s_findrep_struct.lpstr_ಕಂಡುಹಿಡಿ_What, ಖಾಲಿ);
    q = utf16_ಗಮ್ಯ_enc(s_findrep_struct.lpstrReplaceWith, ಖಾಲಿ);
    ಆದರೆ (p != ಖಾಲಿ && q != ಖಾಲಿ)
	gui_do_findrepl(flags, p, q, ಕೆಳಕ್ಕೆ);
    vim_free(p);
    vim_free(q);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
HandleMouseHide(UINT u_ಸಂದೇಶ, LPARAM lParam)
{
    ಸ್ಥಿರ LPARAM ಕೊನೆಯ_l_Param = 0L;

    // We sometimes get a mousemove when the mouse didn't move...
    ಆದರೆ (u_ಸಂದೇಶ == WM_MOUSEMOVE || u_ಸಂದೇಶ == WM_NCMOUSEMOVE)
    {
	ಆದರೆ (lParam == ಕೊನೆಯ_l_Param)
	    ಹಿಂತಿರುಗಿ;
	ಕೊನೆಯ_l_Param = lParam;
    }

    // Handle specially, to centralise coding. We need to be sure we catch all
    // possible events which should cause us to restore the cursor (as it is a
    // shared resource, we take full responsibility for it).
    ಆಯ್ಕೆ (u_ಸಂದೇಶ)
    {
    ಸ್ಥಿತಿ WM_KEYUP:
    ಸ್ಥಿತಿ WM_CHAR:
	/*
	 * blank out the pointer if necessary
	 */
	ಆದರೆ (p_mh)
	    gui_mch_mousehide(TRUE);
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WM_SYSKEYUP:	 // show the pointer when a system-key is pressed
    ಸ್ಥಿತಿ WM_SYSCHAR:
    ಸ್ಥಿತಿ WM_MOUSEMOVE:	 // show the pointer on any mouse action
    ಸ್ಥಿತಿ WM_LBUTTONDOWN:
    ಸ್ಥಿತಿ WM_LBUTTONUP:
    ಸ್ಥಿತಿ WM_MBUTTONDOWN:
    ಸ್ಥಿತಿ WM_MBUTTONUP:
    ಸ್ಥಿತಿ WM_RBUTTONDOWN:
    ಸ್ಥಿತಿ WM_RBUTTONUP:
    ಸ್ಥಿತಿ WM_XBUTTONDOWN:
    ಸ್ಥಿತಿ WM_XBUTTONUP:
    ಸ್ಥಿತಿ WM_NCMOUSEMOVE:
    ಸ್ಥಿತಿ WM_NCLBUTTONDOWN:
    ಸ್ಥಿತಿ WM_NCLBUTTONUP:
    ಸ್ಥಿತಿ WM_NCMBUTTONDOWN:
    ಸ್ಥಿತಿ WM_NCMBUTTONUP:
    ಸ್ಥಿತಿ WM_NCRBUTTONDOWN:
    ಸ್ಥಿತಿ WM_NCRBUTTONUP:
    ಸ್ಥಿತಿ WM_KILLFOCUS:
	/*
	 * if the pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);
	ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ LRESULT CALLBACK
ಪಠ್ಯ_Area_Wnd_Proc(
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    track_user_activity(u_ಸಂದೇಶ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (u_ಸಂದೇಶ)
    {
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDBLCLK,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDOWN,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONDBLCLK,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONDOWN,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_MOUSEMOVE,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_PAINT,	_OnPaint);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONDBLCLK,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONDOWN,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONDBLCLK,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONDOWN,On_Mouse_Button_ಕೆಳಕ್ಕೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_XBUTTONUP,	_OnMouseMoveOrRelease);
	HANDLE_ಸಂದೇಶ(hwnd, WM_ಗಾತ್ರ,	On_ಗಾತ್ರ_ಪಠ್ಯ_Area);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
	ಸ್ಥಿತಿ WM_NOTIFY: Handle_WM_Notify(hwnd, (LPNMHDR)lParam);
	    ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_dwm_ತುಂಬು(ಶೂನ್ಯ)
{
    hLibDwm = vim_ತುಂಬು_Lib("dwmapi.dll");
    ಆದರೆ (hLibDwm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute = (HRESULT (WINAPI *)(HWND, DWORD, LPCVOID, DWORD))
	Get_Proc_ವಿಳಾಸ(hLibDwm, "DwmSetWindowAttribute");

    ಆದರೆ (p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute == ಖಾಲಿ)
    {
	FreeLibrary(hLibDwm);
	hLibDwm = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
}

ಬಾಹ್ಯ DWORD win_ಆವೃತ್ತಿ; // this is in os_mswin.c

/*
 * Set TitleBar's color. Handle hl-TitleBar and hl-TitleBarNC.
 *
 * Only enabled when 'guioptions' has 'C'.
 * if "TitleBar guibg=NONE guifg=NONE" reset the window back to using the
 * system's default behavior for the border color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_titlebar_colors(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿಸು DWMWA_COLOR_DEFAULT 0xFFFFFFFF
    ಆದರೆ (p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute == ಖಾಲಿ || win_ಆವೃತ್ತಿ < MAKE_VER(10, 0, 22000))
	ಹಿಂತಿರುಗಿ;

    guicolor_T captionColor = DWMWA_COLOR_DEFAULT;
    guicolor_T ಪಠ್ಯ_Color = DWMWA_COLOR_DEFAULT;

    ಆದರೆ (vim_strchr(p_go, GO_TITLEBAR) != ಖಾಲಿ)
    {
	ಆದರೆ (gui.ಒಳ_focus)
	{
	    captionColor = gui.title_bg_pixel;
	    ಪಠ್ಯ_Color = gui.title_fg_pixel;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    captionColor = gui.titlenc_bg_pixel;
	    ಪಠ್ಯ_Color = gui.titlenc_fg_pixel;
	}

	ಆದರೆ (captionColor == INVALCOLOR)
	    captionColor = DWMWA_COLOR_DEFAULT;
	ಆದರೆ (ಪಠ್ಯ_Color == INVALCOLOR)
	    ಪಠ್ಯ_Color = DWMWA_COLOR_DEFAULT;
    }

    p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute(s_hwnd, DWMWA_CAPTION_COLOR,
	    &captionColor, ಗಾತ್ರ(captionColor));
    p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute(s_hwnd, DWMWA_ಪಠ್ಯ_COLOR,
	    &ಪಠ್ಯ_Color, ಗಾತ್ರ(ಪಠ್ಯ_Color));
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    HBRUSH prevBrush;

    s_brush = CreateSolidBrush(gui.ಹಿಂಭಾಗ_pixel);
    prevBrush = (HBRUSH)ಗಣ_Class_Long_Ptr(
				s_hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)s_brush);
    InvalidateRect(s_hwnd, ಖಾಲಿ, TRUE);
    // The text area is a child window, the invalidation above does not
    // include it.
    ಆದರೆ (s_textarea_bg_only && s_ಪಠ್ಯ_Area != ಖಾಲಿ)
	InvalidateRect(s_ಪಠ್ಯ_Area, ಖಾಲಿ, TRUE);
    DeleteObject(prevBrush);
}

/*
 * Set the colors to their default values.
 */
    ಶೂನ್ಯ
gui_mch_def_colors(ಶೂನ್ಯ)
{
    gui.norm_pixel = GetSysColor(COLOR_WINDOWTEXT);
    gui.ಹಿಂಭಾಗ_pixel = GetSysColor(COLOR_ಕಿಟಕಿ);
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ಹಿಂಭಾಗ_pixel = gui.ಹಿಂಭಾಗ_pixel;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_open(ಶೂನ್ಯ)
{
    // Actually open the window, if not already visible
    // (may be done already in gui_mch_set_shellsize)
    ಆದರೆ (!Is_ಕಿಟಕಿ_Visible(s_hwnd))
    {
	Show_ಕಿಟಕಿ(s_hwnd, SW_SHOWDEFAULT);

	// Waiting for the message loop leaves the window undrawn while a
	// slow VimEnter autocommand runs. The flush presents the DirectX draw.
	ಆದರೆ (s_ಪಠ್ಯ_Area != ಖಾಲಿ)
	{
	    Update_ಕಿಟಕಿ(s_ಪಠ್ಯ_Area);
	    gui_mch_flush();
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    // Init replace string here, so that we keep it when re-opening the
    // dialog.
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    RECT    rect;

    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rect);
    *x = rect.ಎಡ_2;
    *y = rect.ತುದಿ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಗಣ_ಕಿಟಕಿ_Pos(s_hwnd, ಖಾಲಿ, x, y, 0, 0,
		 SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}
    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ oldx = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ oldy = 0;

    ಗಣ_ಕಿಟಕಿ_Pos(s_ಪಠ್ಯ_Area, ಖಾಲಿ, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
	Send_ಸಂದೇಶ(s_toolbarhwnd, WM_ಗಾತ್ರ,
	      (WPARAM)0, MAKELPARAM(w, gui.toolbar_ಎತ್ತರ));
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_TABLINE)
    ಆದರೆ (showing_tabline)
    {
	ಪೂರ್ಣಾಂಕ	ತುದಿ = 0;
	RECT	rect;

# ifdef FEAT_TOOLBAR
	ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
	    ತುದಿ = gui.toolbar_ಎತ್ತರ;
# endif
	GetClientRect(s_hwnd, &rect);
	Move_ಕಿಟಕಿ(s_tabhwnd, 0, ತುದಿ, rect.ಬಲ_2, gui.tabline_ಎತ್ತರ, TRUE);
    }
#ಆದರೆ_ಅಂತ್ಯ

    // When side scroll bar is unshown, the size of window will change.
    // then, the text area move left or right. thus client rect should be
    // forcedly redrawn. (Yasuhiro Matsumoto)
    ಆದರೆ (oldx != x || oldy != y)
    {
	InvalidateRect(s_hwnd, ಖಾಲಿ, FALSE);
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
    ಪೂರ್ಣಾಂಕ		    flag)
{
    ShowScrollBar(sb->id, SB_CTL, flag);

    // TODO: When the window is maximized, the size of the window stays the
    // same, thus the size of the text area changes.  On Win98 it's OK, on Win
    // NT 4.0 it's not...
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಂಕ		x,
    ಪೂರ್ಣಾಂಕ		y,
    ಪೂರ್ಣಾಂಕ		w,
    ಪೂರ್ಣಾಂಕ		h)
{
    ಗಣ_ಕಿಟಕಿ_Pos(sb->id, ಖಾಲಿ, x, y, w, h,
			      SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    RECT    rcTxt, rcWnd;
    ಪೂರ್ಣಾಂಕ	    xpad;

    Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rcTxt);
    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rcWnd);
    xpad = rcWnd.ಬಲ_2 - rcTxt.ಬಲ_2 - gui.scrollbar_ಅಗಲ
	- pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ಹಿಂತಿರುಗಿ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    RECT    rcTxt, rcWnd;
    ಪೂರ್ಣಾಂಕ	    ypad;

    Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rcTxt);
    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rcWnd);
    ypad = rcWnd.bottom - rcTxt.bottom - gui.scrollbar_ಎತ್ತರ
	- pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi)
	- pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi);
    ಹಿಂತಿರುಗಿ (ypad < 0) ? 0 : ypad;
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಂಕ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    sb->id = Create_ಕಿಟಕಿ(
	"SCROLLBAR", "Scrollbar",
	WS_ಮಗು | ((orient == SBAR_VERT) ? SBS_VERT : SBS_HORZ), 0, 0,
	10,				// Any value will do for now
	10,				// Any value will do for now
	s_hwnd, ಖಾಲಿ,
	g_hinst, ಖಾಲಿ);
}

/*
 * Find the scrollbar with the given hwnd.
 */
    ಸ್ಥಿರ scrollbar_T *
gui_mswin_ಕಂಡುಹಿಡಿ_scrollbar(HWND hwnd)
{
    win_T	*wp;

    ಆದರೆ (gui.bottom_sbar.id == hwnd)
	ಹಿಂತಿರುಗಿ &gui.bottom_sbar;
    FOR_ALL_WINDOWS(wp)
    {
	ಆದರೆ (wp->w_scrollbars[SBAR_ಎಡ].id == hwnd)
	    ಹಿಂತಿರುಗಿ &wp->w_scrollbars[SBAR_ಎಡ];
	ಆದರೆ (wp->w_scrollbars[SBAR_ಬಲ].id == hwnd)
	    ಹಿಂತಿರುಗಿ &wp->w_scrollbars[SBAR_ಬಲ];
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_scrollbar_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    gui.scrollbar_ಅಗಲ = pGetSystemMetricsForDpi(SM_CXVSCROLL, s_dpi);
    gui.scrollbar_ಎತ್ತರ = pGetSystemMetricsForDpi(SM_CYHSCROLL, s_dpi);
}

/*
 * Get the average character size of a font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Get_ಸರಾಸರಿ_ಮೌಲ್ಯ_Font_ಗಾತ್ರ(HDC hdc, ಗಾತ್ರ_4 *ಗಾತ್ರ_2)
{
    // GetTextMetrics() may not return the right value in tmAveCharWidth
    // for some fonts.  Do our own average computation.
    Get_ಪಠ್ಯ_Extent_ಬಿಂದು(hdc,
	    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
	    52, ಗಾತ್ರ_2);
    ಗಾತ್ರ_2->cx = (ಗಾತ್ರ_2->cx / 26 + 1) / 2;
}

/*
 * Get the character size of a font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Get_Font_ಗಾತ್ರ(GuiFont font, ಪೂರ್ಣಾಂಕ *char_ಅಗಲ, ಪೂರ್ಣಾಂಕ *char_ಎತ್ತರ)
{
    HWND    hwnd = Get_Desktop_ಕಿಟಕಿ();
    HDC	    hdc = Get_ಕಿಟಕಿ_DC(hwnd);
    HFONT   hfntOld = SelectFont(hdc, (HFONT)font);
    ಗಾತ್ರ_4    ಗಾತ್ರ_2;
    TEXTMETRIC tm;

    Get_ಪಠ್ಯ_Metrics(hdc, &tm);
    Get_ಸರಾಸರಿ_ಮೌಲ್ಯ_Font_ಗಾತ್ರ(hdc, &ಗಾತ್ರ_2);

    ಆದರೆ (char_ಅಗಲ)
	*char_ಅಗಲ = ಗಾತ್ರ_2.cx + tm.tmOverhang;
    ಆದರೆ (char_ಎತ್ತರ)
	*char_ಎತ್ತರ = tm.tm_ಎತ್ತರ + p_linespace;

    SelectFont(hdc, hfntOld);

    ReleaseDC(hwnd, hdc);
}

/*
 * Update the character size in "gui" structure with the specified font.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
Update_Font_ಗಾತ್ರ(GuiFont font)
{
    Get_Font_ಗಾತ್ರ(font, &gui.char_ಅಗಲ, &gui.char_ಎತ್ತರ);
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    Update_Font_ಗಾತ್ರ(gui.norm_font);
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ GuiFont
get_font_handle(LOGFONTW *lf)
{
    HFONT   font = ಖಾಲಿ;

    // Load the font
    font = Create_Font_ಪರೋಕ್ಷ_W(lf);

    ಆದರೆ (font == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ NOFONT;

    ಹಿಂತಿರುಗಿ (GuiFont)font;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
pixels_ಗಮ್ಯ_points(ಪೂರ್ಣಾಂಕ pixels, ಪೂರ್ಣಾಂಕ vertical)
{
    ಪೂರ್ಣಾಂಕ		points;
    HWND	hwnd;
    HDC		hdc;

    hwnd = Get_Desktop_ಕಿಟಕಿ();
    hdc = Get_ಕಿಟಕಿ_DC(hwnd);

    points = MulDiv(pixels, 72,
		    GetDeviceCaps(hdc, vertical ? LOGPIXELSY : LOGPIXELSX));

    ReleaseDC(hwnd, hdc);

    ಹಿಂತಿರುಗಿ points;
}

    GuiFont
gui_mch_get_font(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		giveErrorIfMissing)
{
    LOGFONTW	lf;
    GuiFont	font = NOFONT;

    ಆದರೆ (get_logfont(&lf, name, ಖಾಲಿ, giveErrorIfMissing) == OK)
    {
	lf.lf_ಎತ್ತರ = adjust_fontsize_by_dpi(lf.lf_ಎತ್ತರ);
	font = get_font_handle(&lf);
    }
    ಆದರೆ (font == NOFONT && giveErrorIfMissing)
	semsg(_(e_unknown_font_str), name);
    ಹಿಂತಿರುಗಿ font;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 * Don't know how to get the actual name, thus use the provided name.
 */
    char_u *
gui_mch_get_fontname(GuiFont font ಬಳಸದ, char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strsave(name);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಆದರೆ (font)
	DeleteObject((HFONT)font);
}

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Sys_Color_ಕೋಷ್ಟಕ
    {
	ಅಕ್ಷರ	    *name;
	ಪೂರ್ಣಾಂಕ	    color;
    } Sys_Color_ಕೋಷ್ಟಕ;

    ಸ್ಥಿರ Sys_Color_ಕೋಷ್ಟಕ sys_ಕೋಷ್ಟಕ[] =
    {
	{"SYS_3DDKSHADOW", COLOR_3DDKSHADOW},
	{"SYS_3DHILIGHT", COLOR_3DHILIGHT},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ COLOR_3DHIGHLIGHT
	{"SYS_3DHIGHLIGHT", COLOR_3DHIGHLIGHT},
#ಆದರೆ_ಅಂತ್ಯ
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
	{"SYS_WINDOW", COLOR_ಕಿಟಕಿ},
	{"SYS_WINDOWFRAME", COLOR_WINDOWFRAME},
	{"SYS_WINDOWTEXT", COLOR_WINDOWTEXT}
    };

    /*
     * Try to look up a system colour.
     */
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(sys_ಕೋಷ್ಟಕ); i++)
	ಆದರೆ (STRICMP(name, sys_ಕೋಷ್ಟಕ[i].name) == 0)
	    ಹಿಂತಿರುಗಿ GetSysColor(sys_ಕೋಷ್ಟಕ[i].color);

    ಹಿಂತಿರುಗಿ gui_get_color_cmn(name);
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    ಹಿಂತಿರುಗಿ gui_get_rgb_color_cmn(r, g, b);
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].vim_code1 != NUL; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0
				       && name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_Beep((UINT)-1);
}
/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ಶೂನ್ಯ
gui_mch_invert_rectangle(
    ಪೂರ್ಣಾಂಕ	    r,
    ಪೂರ್ಣಾಂಕ	    c,
    ಪೂರ್ಣಾಂಕ	    nr,
    ಪೂರ್ಣಾಂಕ	    nc)
{
    RECT    rc;

#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ_2 = FILL_X(c);
    rc.ತುದಿ = FILL_Y(r);
    rc.ಬಲ_2 = rc.ಎಡ_2 + nc * gui.char_ಅಗಲ;
    rc.bottom = rc.ತುದಿ + nr * gui.char_ಎತ್ತರ;
    InvertRect(s_hdc, &rc);
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    Show_ಕಿಟಕಿ(s_hwnd, SW_MINIMIZE);
}

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    HBRUSH  hbr;
    RECT    rc;

#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Note: FrameRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ_2 = FILL_X(gui.ಲಂಬಸಾಲು);
    rc.ತುದಿ = FILL_Y(gui.ಅಡ್ಡಸಾಲು);
    rc.ಬಲ_2 = rc.ಎಡ_2 + gui.char_ಅಗಲ;
    ಆದರೆ (mb_lefthalve(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು))
	rc.ಬಲ_2 += gui.char_ಅಗಲ;
    rc.bottom = rc.ತುದಿ + gui.char_ಎತ್ತರ;
    hbr = CreateSolidBrush(color);
    ಚೌಕಟ್ಟು_Rect(s_hdc, &rc, hbr);
    DeleteBrush(hbr);
}
/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ಶೂನ್ಯ
gui_mch_draw_part_cursor(
    ಪೂರ್ಣಾಂಕ		w,
    ಪೂರ್ಣಾಂಕ		h,
    guicolor_T	color)
{
    RECT	rc;

    /*
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ_2 =
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
		// vertical line should be on the right of current point
		CURSOR_BAR_ಬಲ ? FILL_X(gui.ಲಂಬಸಾಲು + 1) - w :
#ಆದರೆ_ಅಂತ್ಯ
		    FILL_X(gui.ಲಂಬಸಾಲು);
    rc.ತುದಿ = FILL_Y(gui.ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h;
    rc.ಬಲ_2 = rc.ಎಡ_2 + w;
    rc.bottom = rc.ತುದಿ + h;

    fill_rect(&rc, ಖಾಲಿ, color);
}


/*
 * Generates a VK_SPACE when the internal dead_key flag is set to output the
 * dead key's nominal character and re-post the original message.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
output_Dead_ಕೀಲಿ_re_Post_Ex(ಸಂದೇಶ_3 original_ಸಂದೇಶ, ಪೂರ್ಣಾಂಕ dead_key2set)
{
    ಸ್ಥಿರ ಸಂದೇಶ_3 deadCharExpel;

    ಆದರೆ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_OFF)
	ಹಿಂತಿರುಗಿ;

    dead_ಕೀಲಿ = dead_key2set;

    // Make Windows generate the dead key's character
    deadCharExpel.ಸಂದೇಶ_2 = original_ಸಂದೇಶ.ಸಂದೇಶ_2;
    deadCharExpel.hwnd    = original_ಸಂದೇಶ.hwnd;
    deadCharExpel.wParam  = VK_SPACE;

    Translate_ಸಂದೇಶ(&deadCharExpel);

    // re-generate the current character free of the dead char influence
    Post_ಸಂದೇಶ(original_ಸಂದೇಶ.hwnd, original_ಸಂದೇಶ.ಸಂದೇಶ_2, original_ಸಂದೇಶ.wParam,
							  original_ಸಂದೇಶ.lParam);
}

/*
 * Wrapper for outputDeadKey_rePost_Ex which always reset dead_key value.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ_3 original_ಸಂದೇಶ)
{
    output_Dead_ಕೀಲಿ_re_Post_Ex(original_ಸಂದೇಶ, DEAD_ಕೀಲಿ_OFF);
}

/*
 * Refactored out part of process_message(), responsible for
 * handling the case of "not a special key"
 */
ಸ್ಥಿರ ಶೂನ್ಯ process_ಸಂದೇಶ_usual_ಕೀಲಿ(UINT vk, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3 *pmsg)
{
    // marshal to corresponding implementation
    keycode_trans_strategy_ಬಳಸಿದ->ptr_process_ಸಂದೇಶ_usual_ಕೀಲಿ(vk, pmsg);
}

/*
 * Experimental implementation, introduced in v8.2.4807
 * "processing key event in Win32 GUI is not ideal"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_ಸಂದೇಶ_usual_ಕೀಲಿ_experimental(UINT vk, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3 *pmsg)
{
    WCHAR	ch[8];
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    UINT	ಪರಿಶೀಲನೆ_ಸಂಕೇತ;
    BYTE	keyboard_state[256];

    // Construct the state table with only a few modifiers, we don't
    // really care about the presence of Ctrl/Alt as those modifiers are
    // handled by Vim separately.
    memset(keyboard_state, 0, 256);
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_ಸರಿಸು) & 0x8000)
	keyboard_state[VK_ಸರಿಸು] = 0x80;
    ಆದರೆ (Get_ಕೀಲಿ_State(VK_CAPITAL) & 0x0001)
	keyboard_state[VK_CAPITAL] = 0x01;
    // Alt-Gr is synthesized as Alt + Ctrl.
    ಆದರೆ ((Get_ಕೀಲಿ_State(VK_RMENU) & 0x8000)
				 && (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000))
    {
	keyboard_state[VK_MENU] = 0x80;
	keyboard_state[VK_CONTROL] = 0x80;
    }

    // Translate the virtual key according to the current keyboard
    // layout.
    ಪರಿಶೀಲನೆ_ಸಂಕೇತ = Map_Virtual_ಕೀಲಿ(vk, MAPVK_VK_ಗಮ್ಯ_VSC);
    // Convert the scan-code into a sequence of zero or more unicode
    // codepoints.
    // If this is a dead key ToUnicode returns a negative value.
    len = ಗಮ್ಯ_Unicode(vk, ಪರಿಶೀಲನೆ_ಸಂಕೇತ, keyboard_state, ch, ARRAY_ಉದ್ದಳತೆ(ch),
	    0);
    ಆದರೆ (len < 0)
	dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_ಗಣ_DEFAULT;

    ಆದರೆ (len <= 0)
    {
	ಪೂರ್ಣಾಂಕ wm_char = NUL;

	ಆದರೆ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_ಗಣ_DEFAULT
		&& (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000))
	{
	    ಆದರೆ (   // AZERTY CTRL+dead_circumflex
		   (vk == 221 && ಪರಿಶೀಲನೆ_ಸಂಕೇತ == 26)
		   // QWERTZ CTRL+dead_circumflex
		|| (vk == 220 && ಪರಿಶೀಲನೆ_ಸಂಕೇತ == 41))
		wm_char = '[';
	    ಆದರೆ (   // QWERTZ CTRL+dead_two-overdots
		   (vk == 192 && ಪರಿಶೀಲನೆ_ಸಂಕೇತ == 27))
		wm_char = ']';
	}
	ಆದರೆ (wm_char != NUL)
	{
	    // post WM_CHAR='[' - which will be interpreted with CTRL
	    // still hold as ESC
	    Post_ಸಂದೇಶ_W(pmsg->hwnd, WM_CHAR, wm_char, pmsg->lParam);
	    // ask _OnChar() to not touch this state, wait for next key
	    // press and maintain knowledge that we are "poisoned" with
	    // "dead state"
	    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_TRANSIENT_ಒಳ_ON_CHAR;
	}
	ಹಿಂತಿರುಗಿ;
    }

    // Post the message as TranslateMessage would do.
    ಆದರೆ (pmsg->ಸಂದೇಶ_2 == WM_KEYDOWN)
    {
	ಗಾಗಿ (i = 0; i < len; i++)
	    Post_ಸಂದೇಶ_W(pmsg->hwnd, WM_CHAR, ch[i], pmsg->lParam);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < len; i++)
	    Post_ಸಂದೇಶ_W(pmsg->hwnd, WM_SYSCHAR, ch[i], pmsg->lParam);
    }
}

/*
 * "Classic" implementation, existing prior to v8.2.4807
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_ಸಂದೇಶ_usual_ಕೀಲಿ_classic(UINT vk, ಸ್ಥಿರಾಂಕ ಸಂದೇಶ_3 *pmsg)
{
    char_u	string[40];

    // Some keys need C-S- where they should only need C-.
    // Ignore 0xff, Windows XP sends it when NUMLOCK has changed since
    // system startup (Helmut Stiegler, 2003 Oct 3).
    ಆದರೆ (vk != 0xff
	    && (Get_ಕೀಲಿ_State(VK_CONTROL) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_ಸರಿಸು) & 0x8000)
	    && !(Get_ಕೀಲಿ_State(VK_MENU) & 0x8000))
    {
	// CTRL-6 is '^'; Japanese keyboard maps '^' to vk == 0xDE
	ಆದರೆ (vk == '6' || Map_Virtual_ಕೀಲಿ(vk, 2) == (UINT)'^')
	{
	    string[0] = Ctrl_HAT;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 1);
	}
	// vk == 0xBD AZERTY for CTRL-'-', but CTRL-[ for * QWERTY!
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk == 0xBD)	// QWERTY for CTRL-'-'
	{
	    string[0] = Ctrl__;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 1);
	}
	// CTRL-2 is '@'; Japanese keyboard maps '@' to vk == 0xC0
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vk == '2' || Map_Virtual_ಕೀಲಿ(vk, 2) == (UINT)'@')
	{
	    string[0] = Ctrl_AT;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    Translate_ಸಂದೇಶ(pmsg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	Translate_ಸಂದೇಶ(pmsg);
}

/*
 * Process a single Windows message.
 * If one is not available we hang until one is.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
process_ಸಂದೇಶ(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_3		ಸಂದೇಶ;
    UINT	vk = 0;		// Virtual key
    char_u	string[40];
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		modifiers = 0;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಸ್ಥಿರ char_u k10[] = {K_SPECIAL, 'k', ';', 0};
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  keycode_trans_strategy_initialized = 0;

    // lazy initialize - first time only
    ಆದರೆ (!keycode_trans_strategy_initialized)
    {
	keycode_trans_strategy_initialized = 1;
	keycode_trans_strategy_init();
    }

    Get_ಸಂದೇಶ_W(&ಸಂದೇಶ, ಖಾಲಿ, 0, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
    // Look after OLE Automation commands
    ಆದರೆ (ಸಂದೇಶ.ಸಂದೇಶ_2 == WM_OLE)
    {
	char_u *str = (char_u *)ಸಂದೇಶ.lParam;
	ಆದರೆ (str == ಖಾಲಿ || *str == NUL)
	{
	    // Message can't be ours, forward it.  Fixes problem with Ultramon
	    // 3.0.4
	    Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(str, (ಪೂರ್ಣಾಂಕ)STRLEN(str));
	    vim_free(str);  // was allocated in CVim::SendKeys()
	}
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    // Don't process messages used by the dialog
    ಆದರೆ (s_findrep_hwnd != ಖಾಲಿ && Is_Dialog_ಸಂದೇಶ_W(s_findrep_hwnd, &ಸಂದೇಶ))
    {
	HandleMouseHide(ಸಂದೇಶ.ಸಂದೇಶ_2, ಸಂದೇಶ.lParam);
	ಹಿಂತಿರುಗಿ;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Check if it's a special key that we recognise.  If not, call
     * TranslateMessage().
     */
    ಆದರೆ (ಸಂದೇಶ.ಸಂದೇಶ_2 == WM_KEYDOWN || ಸಂದೇಶ.ಸಂದೇಶ_2 == WM_SYSKEYDOWN)
    {
	vk = (ಪೂರ್ಣಾಂಕ) ಸಂದೇಶ.wParam;

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
	ಆದರೆ (keycode_trans_strategy_ಬಳಸಿದ->is_experimental()
		&& dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_SKIP_ON_CHAR)
	{
	    dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
	}

	ಆದರೆ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_OFF)
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
	    ಆದರೆ (keycode_trans_strategy_ಬಳಸಿದ->is_experimental() &&
		    dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_TRANSIENT_ಒಳ_ON_CHAR)
	    {
		output_Dead_ಕೀಲಿ_re_Post_Ex(ಸಂದೇಶ,
				       /*dead_key2set=*/DEAD_ಕೀಲಿ_SKIP_ON_CHAR);
		ಹಿಂತಿರುಗಿ;
	    }

	    ಆದರೆ (dead_ಕೀಲಿ != DEAD_ಕೀಲಿ_ಗಣ_DEFAULT)
	    {
		// should never happen - is there a way to make ASSERT here?
		ಹಿಂತಿರುಗಿ;
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
	    ಆದರೆ ((vk == VK_SPACE || vk == VK_ಹಿಂಭಾಗ || vk == VK_ESCAPE))
	    {
		dead_ಕೀಲಿ = DEAD_ಕೀಲಿ_OFF;
		Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ;
	    }
	    // In modes where we are not typing, dead keys should behave
	    // normally
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((get_real_state()
			    & (ಬಹುಲಕ_ಸೇರಿಸು | ಬಹುಲಕ_CMDLINE | ಬಹುಲಕ_SELECT)) == 0)
	    {
		output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ;
	    }
	}

	// Check for CTRL-BREAK
	ಆದರೆ (vk == VK_CANCEL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    got_int = TRUE;
	    ctrl_break_was_pressed = TRUE;
	    string[0] = Ctrl_C;
	    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 1);
	}

	// This is an IME event or a synthetic keystroke, let Windows handle it.
	ಆದರೆ (vk == VK_PROCESSKEY || vk == VK_PACKET)
	{
	    Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
	    ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    // ignore VK_SPACE when ALT key pressed: system menu
	    ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == vk
		    && (vk != VK_SPACE || !(Get_ಕೀಲಿ_State(VK_MENU) & 0x8000)))
	    {
		/*
		 * Behave as expected if we have a dead key and the special key
		 * is a key that would normally trigger the dead key nominal
		 * character output (such as a NUMPAD printable character or
		 * the TAB key, etc...).
		 */
		ಆದರೆ (dead_ಕೀಲಿ == DEAD_ಕೀಲಿ_ಗಣ_DEFAULT
			&& (special_ಕೀಲಿಗಳು[i].vim_code0 == 'K'
						|| vk == VK_TAB || vk == CAR))
		{
		    output_Dead_ಕೀಲಿ_re_Post(ಸಂದೇಶ);
		    ಹಿಂತಿರುಗಿ;
		}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
		// Check for <F10>: Windows selects the menu.  When <F10> is
		// mapped we want to use the mapping instead.
		ಆದರೆ (vk == VK_F10
			&& gui.menu_is_ಸಕ್ರಿಯ
			&& check_map(k10, State, FALSE, TRUE, FALSE,
							  ಖಾಲಿ, ಖಾಲಿ) == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		modifiers = get_ಸಕ್ರಿಯ_modifiers_ಮೂಲಕ_ptr();

		ಆದರೆ (special_ಕೀಲಿಗಳು[i].vim_code1 == NUL)
		    ಕೀಲಿ = special_ಕೀಲಿಗಳು[i].vim_code0;
		ಇಲ್ಲದಿದ್ದರೆ
		    ಕೀಲಿ = ಗಮ್ಯ_SPECIAL(special_ಕೀಲಿಗಳು[i].vim_code0,
						   special_ಕೀಲಿಗಳು[i].vim_code1);
		ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
		ಆದರೆ (ಕೀಲಿ == CSI)
		    ಕೀಲಿ = K_CSI;

		ಆದರೆ (modifiers)
		{
		    string[0] = CSI;
		    string[1] = KS_MODIFIER;
		    string[2] = modifiers;
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 3);
		}

		ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
		{
		    string[0] = CSI;
		    string[1] = K_ಎರಡನೇ(ಕೀಲಿ);
		    string[2] = K_ಮೂರನೇ(ಕೀಲಿ);
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 3);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ	len;

		    // Handle "key" as a Unicode character.
		    len = char_ಗಮ್ಯ_string(ಕೀಲಿ, string, 40, FALSE);
		    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
		}
		ನಿಲ್ಲಿಸು;
	    }
	}

	// Not a special key.
	ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == 0)
	{
	    process_ಸಂದೇಶ_usual_ಕೀಲಿ(vk, &ಸಂದೇಶ);
	}
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ.ಸಂದೇಶ_2 == WM_IME_NOTIFY)
	_OnImeNotify(ಸಂದೇಶ.hwnd, (DWORD)ಸಂದೇಶ.wParam, (DWORD)ಸಂದೇಶ.lParam);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಂದೇಶ.ಸಂದೇಶ_2 == WM_KEYUP && im_get_ಸ್ಥಿತಿ())
	// added for non-MS IME (Yasuhiro Matsumoto)
	Translate_ಸಂದೇಶ(&ಸಂದೇಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // Check for <F10>: Default effect is to select the menu.  When <F10> is
    // mapped we need to stop it here to avoid strange effects (e.g., for the
    // key-up event)
    ಆದರೆ (vk != VK_F10 || check_map(k10, State, FALSE, TRUE, FALSE,
							  ಖಾಲಿ, ಖಾಲಿ) == ಖಾಲಿ)
#ಆದರೆ_ಅಂತ್ಯ
	Dispatch_ಸಂದೇಶ_W(&ಸಂದೇಶ);
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
    ಸಂದೇಶ_3	    ಸಂದೇಶ;

    ಆದರೆ (!s_busy_processing)
	ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, ಖಾಲಿ, 0, 0, PM_NOREMOVE)
						  && !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ())
	    process_ಸಂದೇಶ();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_any_timer(ಶೂನ್ಯ)
{
    ಸಂದೇಶ_3		ಸಂದೇಶ;

    ಆದರೆ (s_wait_timer != 0 && !s_timed_ಹೊರಹೋಗು)
    {
	KillTimer(ಖಾಲಿ, s_wait_timer);

	// Eat spurious WM_TIMER messages
	ಆಗ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, s_hwnd, WM_TIMER, WM_TIMER, PM_REMOVE))
	    ;
	s_wait_timer = 0;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
// A client message may be handled re-entrantly, e.g. during a redraw while a
// command line is being read; inserting the keys into the typeahead buffer
// then corrupts it. Store them here until a safe point.
ಸ್ಥಿರ garray_T	server_pending_ga = {0, 0, 0, 0, ಖಾಲಿ};

/*
 * Store keys received from a client, to be inserted into the typeahead buffer
 * later by server_flush_input().
 */
    ಶೂನ್ಯ
server_ಕೂಡಿಸು_ಒಳಹರಿವು(char_u *str)
{
    ಆದರೆ (server_pending_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
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

    ಆದರೆ (server_pending_ga.ga_len == 0 || typebuf.tb_len != 0)
	ಹಿಂತಿರುಗಿ;
    str = vim_strnsave(server_pending_ga.ga_ದತ್ತಾಂಶ, server_pending_ga.ga_len);
    server_pending_ga.ga_len = 0;
    ಆದರೆ (str != ಖಾಲಿ)
    {
	server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(str);
	vim_free(str);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1	    Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_wait_for_chars(ದೀರ್ಘ wtime)
{
    ಪೂರ್ಣಾಂಕ		focus;

    s_timed_ಹೊರಹೋಗು = FALSE;

    ಆದರೆ (wtime >= 0)
    {
	// Don't do anything while processing a (scroll) message.
	ಆದರೆ (s_busy_processing)
	    ಹಿಂತಿರುಗಿ FAIL;

	// When called with "wtime" zero, just want one msec.
	s_wait_timer = ಗಣ_Timer(ಖಾಲಿ, 0, (UINT)(wtime == 0 ? 1 : wtime),
								_OnTimer);
    }

    allow_scrollbar = TRUE;

    focus = gui.ಒಳ_focus;
    ಆಗ (!s_timed_ಹೊರಹೋಗು)
    {
	// Stop or start blinking when focus changes
	ಆದರೆ (gui.ಒಳ_focus != focus)
	{
	    ಆದರೆ (gui.ಒಳ_focus)
		gui_mch_ಆರಂಭ_blink();
	    ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ಒಳ_focus;
	}

	ಆದರೆ (s_need_activate)
	{
	    (ಶೂನ್ಯ)ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
	    s_need_activate = FALSE;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
	did_ಕೂಡಿಸು_timer = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
	// Check channel I/O while waiting for a message.
	ಗಾಗಿ (;;)
	{
	    ಸಂದೇಶ_3 ಸಂದೇಶ;

	    parse_queued_messages();
# ifdef FEAT_CLIENTSERVER
	    // Insert keys from a client that were postponed to this safe point.
	    server_flush_ಒಳಹರಿವು();
# endif
# ifdef FEAT_TIMERS
	    ಆದರೆ (did_ಕೂಡಿಸು_timer)
		ನಿಲ್ಲಿಸು;
# endif
	    ಆದರೆ (Peek_ಸಂದೇಶ_W(&ಸಂದೇಶ, ಖಾಲಿ, 0, 0, PM_NOREMOVE))
	    {
		process_ಸಂದೇಶ();
		ನಿಲ್ಲಿಸು;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ()
		    // TODO: The 10 msec is a compromise between laggy response
		    // and consuming more CPU time.  Better would be to handle
		    // channel messages when they arrive.
		    || ಸಂದೇಶ_Wait_For_ಅಪವರ್ತ್ಯ_Objects(0, ಖಾಲಿ, FALSE, 10,
						  QS_ALLINPUT) != WAIT_TIMEOUT)
		ನಿಲ್ಲಿಸು;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	// Don't use gui_mch_update() because then we will spin-lock until a
	// char arrives, instead we use GetMessage() to hang until an
	// event arrives.  No need to check for input_buf_full because we are
	// returning as soon as it contains a single char -- webb
	process_ಸಂದೇಶ();
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    remove_any_timer();
	    allow_scrollbar = FALSE;

	    // Clear pending mouse button, the release event may have been
	    // taken by the dialog window.  But don't do this when getting
	    // focus, we need the mouse-up event then.
	    ಆದರೆ (!s_getting_focus)
		s_button_pending = -1;

	    ಹಿಂತಿರುಗಿ OK;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
	ಆದರೆ (did_ಕೂಡಿಸು_timer)
	{
	    // Need to recompute the waiting time.
	    remove_any_timer();
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    allow_scrollbar = FALSE;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ಶೂನ್ಯ
gui_mch_clear_ಖಂಡ(
    ಪೂರ್ಣಾಂಕ		row1,
    ಪೂರ್ಣಾಂಕ		col1,
    ಪೂರ್ಣಾಂಕ		row2,
    ಪೂರ್ಣಾಂಕ		col2)
{
    RECT	rc;

    /*
     * Clear one extra pixel at the far right, for when bold characters have
     * spilled over to the window border.
     * Note: FillRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ_2 = FILL_X(col1);
    rc.ತುದಿ = FILL_Y(row1);
    rc.ಬಲ_2 = FILL_X(col2 + 1) + (col2 == Columns - 1);
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

    rc.ಎಡ_2 = 0;
    rc.ತುದಿ = 0;
    rc.ಬಲ_2 = Columns * gui.char_ಅಗಲ + 2 * gui.border_ಅಗಲ;
    rc.bottom = ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ + 2 * gui.border_ಅಗಲ;
    clear_rect(&rc);
}
/*
 * Menu stuff.
 */

    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಂಕ flag)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಗಣ_Menu(s_hwnd, flag ? s_menuBar : ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_menu_pos(
    ಪೂರ್ಣಾಂಕ	    x ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    y ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    w ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    h ಬಳಸದ)
{
    // It will be in the right place anyway
}

#ಆದರೆ defined(FEAT_MENU)
/*
 * Make menu item hidden or not hidden
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಂಕ		hidden)
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
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ಹಿಂತಿರುಗಿ (guicolor_T)((GetRValue(pixel) << 16) + (GetGValue(pixel) << 8)
							   + GetBValue(pixel));
}

#ಆದರೆ defined(FEAT_GUI_DIALOG)
/*
 * Convert pixels in X to dialog units
 */
    ಸ್ಥಿರ ಶಬ್ದ_2
Pixel_ಗಮ್ಯ_Dialog_X(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Pixels)
{
    ಹಿಂತಿರುಗಿ (ಶಬ್ದ_2)((ಅಂಕೆ_Pixels * 4) / s_dlgfntwidth);
}

/*
 * Convert pixels in Y to dialog units
 */
    ಸ್ಥಿರ ಶಬ್ದ_2
Pixel_ಗಮ್ಯ_Dialog_Y(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Pixels)
{
    ಹಿಂತಿರುಗಿ (ಶಬ್ದ_2)((ಅಂಕೆ_Pixels * 8) / s_dlgfntheight);
}

/*
 * Return the width in pixels of the given text in the given DC.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Get_ಪಠ್ಯ_ಅಗಲ(HDC hdc, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಗಾತ್ರ_4    ಗಾತ್ರ_2;

    Get_ಪಠ್ಯ_Extent_ಬಿಂದು(hdc, (LPCSTR)str, len, &ಗಾತ್ರ_2);
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2.cx;
}

/*
 * Return the width in pixels of the given text in the given DC, taking care
 * of 'encoding' to active codepage conversion.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
Get_ಪಠ್ಯ_ಅಗಲ_Enc(HDC hdc, char_u *str, ಪೂರ್ಣಾಂಕ len)
{
    ಗಾತ್ರ_4	ಗಾತ್ರ_2;
    WCHAR	*wstr;
    ಪೂರ್ಣಾಂಕ		n;
    ಪೂರ್ಣಾಂಕ		wlen = len;

    wstr = enc_ಗಮ್ಯ_utf16(str, &wlen);
    ಆದರೆ (wstr == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    n = Get_ಪಠ್ಯ_Extent_ಬಿಂದು_W(hdc, wstr, wlen, &ಗಾತ್ರ_2);
    vim_free(wstr);
    ಆದರೆ (n)
	ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2.cx;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ get_work_area(RECT *spi_rect);

/*
 * A quick little routine that will center one window over another, handy for
 * dialog boxes.  Taken from the Win32SDK samples and modified for multiple
 * monitors.
 */
    ಸ್ಥಿರ BOOL
Center_ಕಿಟಕಿ(
    HWND hwnd_ಮಗು,
    HWND hwnd_ಪೋಷಕ)
{
    HMONITOR	    mon;
    MONITORINFO	    moninfo;
    RECT	    r_ಮಗು, r_ಪೋಷಕ, rScreen;
    ಪೂರ್ಣಾಂಕ		    w_ಮಗು, h_ಮಗು, w_ಪೋಷಕ, h_ಪೋಷಕ;
    ಪೂರ್ಣಾಂಕ		    xNew, yNew;
    HDC		    hdc;

    Get_ಕಿಟಕಿ_Rect(hwnd_ಮಗು, &r_ಮಗು);
    w_ಮಗು = r_ಮಗು.ಬಲ_2 - r_ಮಗು.ಎಡ_2;
    h_ಮಗು = r_ಮಗು.bottom - r_ಮಗು.ತುದಿ;

    // If Vim is minimized put the window in the middle of the screen.
    ಆದರೆ (hwnd_ಪೋಷಕ == ಖಾಲಿ || IsMinimized(hwnd_ಪೋಷಕ))
	get_work_area(&r_ಪೋಷಕ);
    ಇಲ್ಲದಿದ್ದರೆ
	Get_ಕಿಟಕಿ_Rect(hwnd_ಪೋಷಕ, &r_ಪೋಷಕ);
    w_ಪೋಷಕ = r_ಪೋಷಕ.ಬಲ_2 - r_ಪೋಷಕ.ಎಡ_2;
    h_ಪೋಷಕ = r_ಪೋಷಕ.bottom - r_ಪೋಷಕ.ತುದಿ;

    moninfo.cb_ಗಾತ್ರ = ಗಾತ್ರ(MONITORINFO);
    mon = Monitor_From_ಕಿಟಕಿ(hwnd_ಮಗು, MONITOR_DEFAULTTOPRIMARY);
    ಆದರೆ (mon != ಖಾಲಿ && GetMonitorInfo(mon, &moninfo))
    {
	rScreen = moninfo.rcWork;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	hdc = GetDC(hwnd_ಮಗು);
	rScreen.ಎಡ_2 = 0;
	rScreen.ತುದಿ = 0;
	rScreen.ಬಲ_2 = GetDeviceCaps(hdc, HORZRES);
	rScreen.bottom = GetDeviceCaps(hdc, VERTRES);
	ReleaseDC(hwnd_ಮಗು, hdc);
    }

    xNew = r_ಪೋಷಕ.ಎಡ_2 + ((w_ಪೋಷಕ - w_ಮಗು) / 2);
    ಆದರೆ (xNew < rScreen.ಎಡ_2)
	xNew = rScreen.ಎಡ_2;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((xNew + w_ಮಗು) > rScreen.ಬಲ_2)
	xNew = rScreen.ಬಲ_2 - w_ಮಗು;

    yNew = r_ಪೋಷಕ.ತುದಿ + ((h_ಪೋಷಕ - h_ಮಗು) / 2);
    ಆದರೆ (yNew < rScreen.ತುದಿ)
	yNew = rScreen.ತುದಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((yNew + h_ಮಗು) > rScreen.bottom)
	yNew = rScreen.bottom - h_ಮಗು;

    ಹಿಂತಿರುಗಿ ಗಣ_ಕಿಟಕಿ_Pos(hwnd_ಮಗು, ಖಾಲಿ, xNew, yNew, 0, 0,
						   SWP_NOSIZE | SWP_NOZORDER);
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG

#ಆದರೆ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (s_toolbarhwnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (showit)
    {
	// Enable unicode support
	Send_ಸಂದೇಶ(s_toolbarhwnd, TB_SETUNICODEFORMAT, (WPARAM)TRUE,
								(LPARAM)0);
	Show_ಕಿಟಕಿ(s_toolbarhwnd, SW_SHOW);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	Show_ಕಿಟಕಿ(s_toolbarhwnd, SW_HIDE);
}

// The number of bitmaps is fixed.  Exit is missing!
# define TOOLBAR_BITMAP_ಎಣಿಕೆ 31

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_tabline_popup_menu_ನಮೂದು(HMENU pmenu, UINT ಅಂಶ_id, char_u *ಅಂಶ_ಪಠ್ಯ)
{
    WCHAR	    *wn;
    MENUITEMINFOW   infow;

    wn = enc_ಗಮ್ಯ_utf16(ಅಂಶ_ಪಠ್ಯ, ಖಾಲಿ);
    ಆದರೆ (wn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    infow.cb_ಗಾತ್ರ = ಗಾತ್ರ(infow);
    infow.f_ಮುಖವಾಡ = MIIM_TYPE | MIIM_ID;
    infow.wID = ಅಂಶ_id;
    infow.fType = MFT_STRING;
    infow.dw_Type_ದತ್ತಾಂಶ = wn;
    infow.cch = (UINT)wcslen(wn);
    ಸೇರಿಸು_Menu_ಅಂಶ_W(pmenu, ಅಂಶ_id, FALSE, &infow);
    vim_free(wn);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_tabline_popup_menu(ಶೂನ್ಯ)
{
    HMENU	    tab_pmenu;
    ದೀರ್ಘ	    rval;
    ಬಿಂದು_2	    pt;

    // When ignoring events don't show the menu.
    ಆದರೆ (hold_gui_events || cmdwin_type != 0)
	ಹಿಂತಿರುಗಿ;

    tab_pmenu = CreatePopupMenu();
    ಆದರೆ (tab_pmenu == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಮೊದಲ_tabpage->tp_ಮುಂದಿನ != ಖಾಲಿ)
	ಕೂಡಿಸು_tabline_popup_menu_ನಮೂದು(tab_pmenu,
				TABLINE_MENU_CLOSE, (char_u *)_("Close tab"));
    ಕೂಡಿಸು_tabline_popup_menu_ನಮೂದು(tab_pmenu,
				TABLINE_MENU_NEW, (char_u *)_("New tab"));
    ಕೂಡಿಸು_tabline_popup_menu_ನಮೂದು(tab_pmenu,
				TABLINE_MENU_OPEN, (char_u *)_("Open tab..."));

    GetCursorPos(&pt);
    rval = TrackPopupMenuEx(tab_pmenu, TPM_RETURNCMD, pt.x, pt.y, s_tabhwnd,
									ಖಾಲಿ);

    DestroyMenu(tab_pmenu);

    // Add the string cmd into input buffer
    ಆದರೆ (rval > 0)
    {
	TCHITTESTINFO htinfo;
	ಪೂರ್ಣಾಂಕ idx;

	ಆದರೆ (Screen_ಗಮ್ಯ_Client(s_tabhwnd, &pt) == 0)
	    ಹಿಂತಿರುಗಿ;

	htinfo.pt.x = pt.x;
	htinfo.pt.y = pt.y;
	idx = Tab_Ctrl_ದೊರಕಿತು_Test(s_tabhwnd, &htinfo);
	ಆದರೆ (idx == -1)
	    idx = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    idx += 1;

	send_tabline_menu_event(idx, (ಪೂರ್ಣಾಂಕ)rval);
    }
}

/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (s_tabhwnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!showit != !showing_tabline)
    {
	ಆದರೆ (showit)
	    Show_ಕಿಟಕಿ(s_tabhwnd, SW_SHOW);
	ಇಲ್ಲದಿದ್ದರೆ
	    Show_ಕಿಟಕಿ(s_tabhwnd, SW_HIDE);
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ s_tabhwnd != ಖಾಲಿ && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    tabpage_T	*tp;
    TCITEM	tie;
    ಪೂರ್ಣಾಂಕ		nr = 0;
    ಪೂರ್ಣಾಂಕ		curtabidx = 0;
    ಪೂರ್ಣಾಂಕ		tabadded = 0;
    WCHAR	*wstr = ಖಾಲಿ;

    ಆದರೆ (s_tabhwnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Enable unicode support
    Send_ಸಂದೇಶ(s_tabhwnd, CCM_SETUNICODEFORMAT, (WPARAM)TRUE, (LPARAM)0);

    tie.ಮುಖವಾಡ = TCIF_ಪಠ್ಯ;
    tie.iImage = -1;

    // Disable redraw for tab updates to eliminate O(N^2) draws.
    Send_ಸಂದೇಶ(s_tabhwnd, WM_SETREDRAW, (WPARAM)FALSE, 0);

    // Add a label for each tab page.  They all contain the same text area.
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, ++nr)
    {
	ಆದರೆ (tp == curtab)
	    curtabidx = nr;

	ಆದರೆ (nr >= Tab_Ctrl_Get_ಅಂಶ_ಎಣಿಕೆ(s_tabhwnd))
	{
	    // Add the tab
	    tie.psz_ಪಠ್ಯ = "-Empty-";
	    Tab_Ctrl_ಸೇರಿಸು_ಅಂಶ(s_tabhwnd, nr, &tie);
	    tabadded = 1;
	}

	get_tabline_label(tp, FALSE);
	tie.psz_ಪಠ್ಯ = (LPSTR)NameBuff;

	wstr = enc_ಗಮ್ಯ_utf16(NameBuff, ಖಾಲಿ);
	ಆದರೆ (wstr != ಖಾಲಿ)
	{
	    TCITEMW		tiw;

	    tiw.ಮುಖವಾಡ = TCIF_ಪಠ್ಯ;
	    tiw.iImage = -1;
	    tiw.psz_ಪಠ್ಯ = wstr;
	    Send_ಸಂದೇಶ(s_tabhwnd, TCM_SETITEMW, (WPARAM)nr, (LPARAM)&tiw);
	    vim_free(wstr);
	}
    }

    // Remove any old labels.
    ಆಗ (nr < Tab_Ctrl_Get_ಅಂಶ_ಎಣಿಕೆ(s_tabhwnd))
	Tab_Ctrl_Delete_ಅಂಶ(s_tabhwnd, nr);

    ಆದರೆ (!tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_ಗಣ_Cur_Sel(s_tabhwnd, curtabidx);

    // Re-enable redraw and redraw.
    Send_ಸಂದೇಶ(s_tabhwnd, WM_SETREDRAW, (WPARAM)TRUE, 0);
    Redraw_ಕಿಟಕಿ(s_tabhwnd, ಖಾಲಿ, ಖಾಲಿ,
		    RDW_ERASE | RDW_ಚೌಕಟ್ಟು | RDW_INVALIDATE | RDW_ALLCHILDREN);

    ಆದರೆ (tabadded && TabCtrl_GetCurSel(s_tabhwnd) != curtabidx)
	Tab_Ctrl_ಗಣ_Cur_Sel(s_tabhwnd, curtabidx);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_curtab(ಪೂರ್ಣಾಂಕ nr)
{
    ಆದರೆ (s_tabhwnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (TabCtrl_GetCurSel(s_tabhwnd) != nr - 1)
	Tab_Ctrl_ಗಣ_Cur_Sel(s_tabhwnd, nr - 1);
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME
    ಶೂನ್ಯ
gui_mch_ಗಣ_dark_theme(ಪೂರ್ಣಾಂಕ dark)
{
    ಆದರೆ (p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute != ಖಾಲಿ && win_ಆವೃತ್ತಿ >= MAKE_VER(10, 0, 18985))
	p_Dwm_ಗಣ_ಕಿಟಕಿ_Attribute(s_hwnd, DWMWA_USE_IMMERSIVE_DARK_ಬಹುಲಕ, &dark,
		ಗಾತ್ರ(dark));

    ಆದರೆ (p_ಗಣ_Preferred_App_ಬಹುಲಕ != ಖಾಲಿ && win_ಆವೃತ್ತಿ >= MAKE_VER(10, 0, 18362))
	p_ಗಣ_Preferred_App_ಬಹುಲಕ(dark);

    ಆದರೆ (pFlushMenuThemes != ಖಾಲಿ && win_ಆವೃತ್ತಿ >= MAKE_VER(10, 0, 18362))
	pFlushMenuThemes();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_uxtheme_ತುಂಬು(ಶೂನ್ಯ)
{
    hUxThemeLib = vim_ತುಂಬು_Lib("uxtheme.dll");
    ಆದರೆ (hUxThemeLib == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    p_ಗಣ_Preferred_App_ಬಹುಲಕ = (DWORD (WINAPI *)(DWORD))
	Get_Proc_ವಿಳಾಸ(hUxThemeLib, MAKEINTRESOURCE(135));
    pFlushMenuThemes = (ಶೂನ್ಯ (WINAPI *)(ಶೂನ್ಯ))
	Get_Proc_ವಿಳಾಸ(hUxThemeLib, MAKEINTRESOURCE(136));

    ಆದರೆ (p_ಗಣ_Preferred_App_ಬಹುಲಕ == ಖಾಲಿ || pFlushMenuThemes == ಖಾಲಿ)
    {
	FreeLibrary(hUxThemeLib);
	hUxThemeLib = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DARKTHEME

/*
 * When flag is true, set fullscreen on.
 * When flag is false, set fullscreen off.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_fullscreen(ಪೂರ್ಣಾಂಕ flag)
{
    ಸ್ಥಿರ RECT ಸಾಮಾನ್ಯ_rect;
    ಸ್ಥಿರ LONG_PTR ಸಾಮಾನ್ಯ_style, ಸಾಮಾನ್ಯ_exstyle;
    HMONITOR	mon;
    MONITORINFO	moninfo;
    RECT	rc;

    ಆದರೆ (!ತುಂಬಿದ_screen) // Windows not set yet.
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (flag)
    {
	ಆದರೆ (fullscreen_on)
	    ಹಿಂತಿರುಗಿ;

	// Enter fullscreen mode
	Get_ಕಿಟಕಿ_Rect(s_hwnd, &rc);

	moninfo.cb_ಗಾತ್ರ = ಗಾತ್ರ(MONITORINFO);
	mon = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);
	ಆದರೆ (mon == ಖಾಲಿ || !GetMonitorInfo(mon, &moninfo))
	    ಹಿಂತಿರುಗಿ;

	// Save current window state
	Get_ಕಿಟಕಿ_Rect(s_hwnd, &ಸಾಮಾನ್ಯ_rect);
	ಸಾಮಾನ್ಯ_style = Get_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_STYLE);
	ಸಾಮಾನ್ಯ_exstyle = Get_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_EXSTYLE);

	// Set fullscreen styles
	ಗಣ_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_STYLE,
		ಸಾಮಾನ್ಯ_style & ~(WS_CAPTION | WS_THICKFRAME));
	ಗಣ_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_EXSTYLE,
		ಸಾಮಾನ್ಯ_exstyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE |
		    WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));
	ಗಣ_ಕಿಟಕಿ_Pos(s_hwnd, ಖಾಲಿ,
		moninfo.rcMonitor.ಎಡ_2,
		moninfo.rcMonitor.ತುದಿ,
		moninfo.rcMonitor.ಬಲ_2 - moninfo.rcMonitor.ಎಡ_2,
		moninfo.rcMonitor.bottom - moninfo.rcMonitor.ತುದಿ,
		SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	fullscreen_on = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!fullscreen_on)
	    ಹಿಂತಿರುಗಿ;

	// Exit fullscreen mode
	ಗಣ_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_STYLE, ಸಾಮಾನ್ಯ_style);
	ಗಣ_ಕಿಟಕಿ_Long_Ptr(s_hwnd, GWL_EXSTYLE, ಸಾಮಾನ್ಯ_exstyle);

	// Restore original window position and size
	ಗಣ_ಕಿಟಕಿ_Pos(s_hwnd, ಖಾಲಿ,
		ಸಾಮಾನ್ಯ_rect.ಎಡ_2,
		ಸಾಮಾನ್ಯ_rect.ತುದಿ,
		ಸಾಮಾನ್ಯ_rect.ಬಲ_2 - ಸಾಮಾನ್ಯ_rect.ಎಡ_2,
		ಸಾಮಾನ್ಯ_rect.bottom - ಸಾಮಾನ್ಯ_rect.ತುದಿ,
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
    ಪೂರ್ಣಾಂಕ		fill_typebuf = FALSE;
    char_u	ಕೀಲಿ_name[4];

    Post_ಸಂದೇಶ(s_hwnd, WM_SYSCOMMAND, (WPARAM)SC_KEYMENU, (LPARAM)0);
    ಆಗ (*ಕೀಲಿಗಳು)
    {
	ಆದರೆ (*ಕೀಲಿಗಳು == '~')
	    *ಕೀಲಿಗಳು = ' ';	    // for showing system menu
	Post_ಸಂದೇಶ(s_hwnd, WM_CHAR, (WPARAM)*ಕೀಲಿಗಳು, (LPARAM)0);
	ಕೀಲಿಗಳು++;
	fill_typebuf = TRUE;
    }
    ಆದರೆ (fill_typebuf)
    {
	// Put a NOP in the typeahead buffer so that the message will get
	// processed.
	ಕೀಲಿ_name[0] = K_SPECIAL;
	ಕೀಲಿ_name[1] = KS_EXTRA;
	ಕೀಲಿ_name[2] = KE_NOP;
	ಕೀಲಿ_name[3] = NUL;
#ಆದರೆ defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	typebuf_was_filled = TRUE;
#ಆದರೆ_ಅಂತ್ಯ
	(ಶೂನ್ಯ)ins_typebuf(ಕೀಲಿ_name, REMAP_NONE, 0, TRUE, FALSE);
    }
}

/*
 * Create the find & replace dialogs.
 * You can't have both at once: ":find" when replace is showing, destroys
 * the replace dialog first, and the other way around.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    ಸ್ಥಿರ ಶೂನ್ಯ
initialise_findrep(char_u *initial_string)
{
    ಪೂರ್ಣಾಂಕ		wword = FALSE;
    ಪೂರ್ಣಾಂಕ		mcase = !p_ic;
    char_u	*ನಮೂದು_ಪಠ್ಯ;

    // Get the search string to use.
    ನಮೂದು_ಪಠ್ಯ = get_ಕಂಡುಹಿಡಿ_dialog_ಪಠ್ಯ(initial_string, &wword, &mcase);

    s_findrep_struct.hwndOwner = s_hwnd;
    s_findrep_struct.Flags = FR_ಕೆಳಕ್ಕೆ;
    ಆದರೆ (mcase)
	s_findrep_struct.Flags |= FR_MATCHCASE;
    ಆದರೆ (wword)
	s_findrep_struct.Flags |= FR_WHOLEWORD;
    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ && *ನಮೂದು_ಪಠ್ಯ != NUL)
    {
	WCHAR *p = enc_ಗಮ್ಯ_utf16(ನಮೂದು_ಪಠ್ಯ, ಖಾಲಿ);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ len = s_findrep_struct.w_ಕಂಡುಹಿಡಿ_What_Len - 1;

	    wcsncpy(s_findrep_struct.lpstr_ಕಂಡುಹಿಡಿ_What, p, len);
	    s_findrep_struct.lpstr_ಕಂಡುಹಿಡಿ_What[len] = NUL;
	    vim_free(p);
	}
    }
    vim_free(ನಮೂದು_ಪಠ್ಯ);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಕಿಟಕಿ_title(HWND hwnd, ಅಕ್ಷರ *title)
{
    ಆದರೆ (title != ಖಾಲಿ)
    {
	WCHAR	*wbuf;

	// Convert the title from 'encoding' to UTF-16.
	wbuf = (WCHAR *)enc_ಗಮ್ಯ_utf16((char_u *)title, ಖಾಲಿ);
	ಆದರೆ (wbuf != ಖಾಲಿ)
	{
	    ಗಣ_ಕಿಟಕಿ_ಪಠ್ಯ_W(hwnd, wbuf);
	    vim_free(wbuf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)ಗಣ_ಕಿಟಕಿ_ಪಠ್ಯ_W(hwnd, ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_ಕಂಡುಹಿಡಿ_dialog(exarg_T *eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    ಆದರೆ (s_findrep_ಸಂದೇಶ != 0)
    {
	ಆದರೆ (Is_ಕಿಟಕಿ(s_findrep_hwnd) && !s_findrep_is_ಕಂಡುಹಿಡಿ)
	    Destroy_ಕಿಟಕಿ(s_findrep_hwnd);

	ಆದರೆ (!Is_ಕಿಟಕಿ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = ಕಂಡುಹಿಡಿ_ಪಠ್ಯ_W(&s_findrep_struct);
	}

	ಗಣ_ಕಿಟಕಿ_title(s_findrep_hwnd, _("Find string"));
	(ಶೂನ್ಯ)ಗಣ_Focus(s_findrep_hwnd);

	s_findrep_is_ಕಂಡುಹಿಡಿ = TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ
}


    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    ಆದರೆ (s_findrep_ಸಂದೇಶ != 0)
    {
	ಆದರೆ (Is_ಕಿಟಕಿ(s_findrep_hwnd) && s_findrep_is_ಕಂಡುಹಿಡಿ)
	    Destroy_ಕಿಟಕಿ(s_findrep_hwnd);

	ಆದರೆ (!Is_ಕಿಟಕಿ(s_findrep_hwnd))
	{
	    initialise_findrep(eap->arg);
	    s_findrep_hwnd = Replace_ಪಠ್ಯ_W(&s_findrep_struct);
	}

	ಗಣ_ಕಿಟಕಿ_title(s_findrep_hwnd, _("Find & Replace"));
	(ಶೂನ್ಯ)ಗಣ_Focus(s_findrep_hwnd);

	s_findrep_is_ಕಂಡುಹಿಡಿ = FALSE;
    }
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Set visibility of the pointer.
 */
    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಂಕ hide)
{
    ಆದರೆ (hide == gui.ಸೂಚಕ_hidden)
	ಹಿಂತಿರುಗಿ;

    ShowCursor(!hide);
    gui.ಸೂಚಕ_hidden = hide;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_show_popupmenu_at(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    // Unhide the mouse, we don't get move events here.
    gui_mch_mousehide(FALSE);

    (ಶೂನ್ಯ)TrackPopupMenu(
	(HMENU)menu->submenu_id,
	TPM_LEFTALIGN | TPM_LEFTBUTTON,
	x, y,
	(ಪೂರ್ಣಾಂಕ)0,	    //reserved param
	s_hwnd,
	ಖಾಲಿ);
    /*
     * NOTE: The pop-up menu can eat the mouse up event.
     * We deal with this in normal.c.
     */
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Got a message when the system will go down.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಕೊನೆ_Session(ಶೂನ್ಯ)
{
    getout_preserve_modified(1);
}

/*
 * Get this message when the user clicks on the cross in the top right corner
 * of a Windows95 window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnClose(HWND hwnd ಬಳಸದ)
{
    gui_shell_closed();
}

/*
 * Get a message when the window is being destroyed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDestroy(HWND hwnd)
{
    ಆದರೆ (!destroying)
	_OnClose(hwnd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnPaint(
    HWND hwnd)
{
    ಆದರೆ (IsMinimized(hwnd))
	ಹಿಂತಿರುಗಿ;

    PAINTSTRUCT ps;

    ಹೊರಹೋಗು_flush();	    // make sure all output has been processed
    (ಶೂನ್ಯ)BeginPaint(hwnd, &ps);

    // prevent multi-byte characters from misprinting on an invalid
    // rectangle
    ಆದರೆ (has_mbyte)
    {
	RECT rect;

	GetClientRect(hwnd, &rect);
	ps.rcPaint.ಎಡ_2 = rect.ಎಡ_2;
	ps.rcPaint.ಬಲ_2 = rect.ಬಲ_2;
    }

    ಆದರೆ (!Is_Rect_ಖಾಲಿಗಳು(&ps.rcPaint))
    {
	// The text area has no background brush, thus undefined pixels remain
	// visible until the screen contents are drawn.
	ಆದರೆ (!screen_cleared || Screen_ಸಾಲುಗಳು == ಖಾಲಿ)
	{
	    clear_rect(&ps.rcPaint);
	    s_textarea_bg_only = true;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    s_textarea_bg_only = false;
	gui_redraw(ps.rcPaint.ಎಡ_2, ps.rcPaint.ತುದಿ,
		ps.rcPaint.ಬಲ_2 - ps.rcPaint.ಎಡ_2 + 1,
		ps.rcPaint.bottom - ps.rcPaint.ತುದಿ + 1);
    }

    ಕೊನೆ_Paint(hwnd, &ps);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಗಾತ್ರ(
    HWND hwnd,
    UINT state ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ cx,
    ಪೂರ್ಣಾಂಕ cy)
{
    ಆದರೆ (!IsMinimized(hwnd) && !s_ಒಳ_dpichanged)
    {
	gui_resize_shell(cx, cy);

	// Menu bar may wrap differently now
	gui_mswin_get_menu_ಎತ್ತರ(TRUE);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಗಣ_Focus(
    HWND hwnd,
    HWND hwndOldFocus)
{
    gui_focus_change(TRUE);
    s_getting_focus = TRUE;
    (ಶೂನ್ಯ)Def_ಕಿಟಕಿ_Proc_W(hwnd, WM_SETFOCUS, (WPARAM)hwndOldFocus, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnKillFocus(
    HWND hwnd,
    HWND hwndNewFocus)
{
    ಆದರೆ (destroying)
	ಹಿಂತಿರುಗಿ;
    gui_focus_change(FALSE);
    s_getting_focus = FALSE;
    (ಶೂನ್ಯ)Def_ಕಿಟಕಿ_Proc_W(hwnd, WM_KILLFOCUS, (WPARAM)hwndNewFocus, 0);
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
    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, WM_ACTIVATEAPP, fActivate, (DWORD)dwThreadId);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    Destroy_ಕಿಟಕಿ(sb->id);
}

/*
 * Get current mouse coordinates in text window.
 */
    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    RECT rct;
    ಬಿಂದು_2 mp;

    (ಶೂನ್ಯ)Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rct);
    (ಶೂನ್ಯ)GetCursorPos(&mp);
    *x = (ಪೂರ್ಣಾಂಕ)(mp.x - rct.ಎಡ_2);
    *y = (ಪೂರ್ಣಾಂಕ)(mp.y - rct.ತುದಿ);
}

/*
 * Move mouse pointer to character at (x, y).
 */
    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    RECT rct;

    (ಶೂನ್ಯ)Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rct);
    (ಶೂನ್ಯ)ಗಣ_Cursor_Pos(x + gui.border_ಸ್ಥಾನಾಂತರ + rct.ಎಡ_2,
		       y + gui.border_ಸ್ಥಾನಾಂತರ + rct.ತುದಿ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mswin_get_valid_dimensions(
    ಪೂರ್ಣಾಂಕ w,
    ಪೂರ್ಣಾಂಕ h,
    ಪೂರ್ಣಾಂಕ *valid_w,
    ಪೂರ್ಣಾಂಕ *valid_h,
    ಪೂರ್ಣಾಂಕ *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
    ಪೂರ್ಣಾಂಕ *ಅಡ್ಡಸಾಲುಗಳು_2)
{
    ಪೂರ್ಣಾಂಕ	    ತಳ_ಅಗಲ, ತಳ_ಎತ್ತರ;

    ತಳ_ಅಗಲ = gui_get_ತಳ_ಅಗಲ()
	+ (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    ತಳ_ಎತ್ತರ = gui_get_ತಳ_ಎತ್ತರ()
	+ (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	+ gui_mswin_get_menu_ಎತ್ತರ(FALSE);
    *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = (w - ತಳ_ಅಗಲ) / gui.char_ಅಗಲ;
    *ಅಡ್ಡಸಾಲುಗಳು_2 = (h - ತಳ_ಎತ್ತರ) / gui.char_ಎತ್ತರ;
    *valid_w = ತಳ_ಅಗಲ + *ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ;
    *valid_h = ತಳ_ಎತ್ತರ + *ಅಡ್ಡಸಾಲುಗಳು_2 * gui.char_ಎತ್ತರ;
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
    RECT    rc;

#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Note: InvertRect() excludes right and bottom of rectangle.
     */
    rc.ಎಡ_2 = 0;
    rc.ತುದಿ = 0;
    rc.ಬಲ_2 = gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ;
    rc.bottom = gui.ಅಂಕೆ_ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ;
    InvertRect(s_hdc, &rc);
    gui_mch_flush();			// make sure it's displayed

    ui_delay((ದೀರ್ಘ)msec, TRUE);	// wait for a few msec

    InvertRect(s_hdc, &rc);
}

/*
 * Check if the specified point is on-screen. (multi-monitor aware)
 */
    ಸ್ಥಿರ BOOL
is_ಬಿಂದು_onscreen(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಬಿಂದು_2   pt = {x, y};

    ಹಿಂತಿರುಗಿ Monitor_From_ಬಿಂದು(pt, MONITOR_DEFAULTTONULL) != ಖಾಲಿ;
}

/*
 * Check if the whole client area of the specified window is on-screen.
 *
 * Note about DirectX: Windows 10 1809 or above no longer maintains image of
 * the window portion that is off-screen.  Scrolling by DWriteContext_Scroll()
 * only works when the whole window is on-screen.
 */
    ಸ್ಥಿರ BOOL
is_ಕಿಟಕಿ_onscreen(HWND hwnd)
{
    RECT    rc;
    ಬಿಂದು_2   p1, p2;

    GetClientRect(hwnd, &rc);
    p1.x = rc.ಎಡ_2;
    p1.y = rc.ತುದಿ;
    p2.x = rc.ಬಲ_2 - 1;
    p2.y = rc.bottom - 1;
    Client_ಗಮ್ಯ_Screen(hwnd, &p1);
    Client_ಗಮ್ಯ_Screen(hwnd, &p2);

    ಆದರೆ (!is_ಬಿಂದು_onscreen(p1.x, p1.y))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!is_ಬಿಂದು_onscreen(p1.x, p2.y))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!is_ಬಿಂದು_onscreen(p2.x, p1.y))
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (!is_ಬಿಂದು_onscreen(p2.x, p2.y))
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Return flags used for scrolling.
 * The SW_INVALIDATE is required when part of the window is covered or
 * off-screen. Refer to MS KB Q75236.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_scroll_flags(ಶೂನ್ಯ)
{
    HWND	hwnd;
    RECT	rcVim, rcOther, rcDest;

    // Check if the window is (partly) off-screen.
    ಆದರೆ (!is_ಕಿಟಕಿ_onscreen(s_hwnd))
	ಹಿಂತಿರುಗಿ SW_INVALIDATE;

    // Check if there is a window (partly) on top of us.
    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rcVim);
    ಗಾಗಿ (hwnd = s_hwnd; (hwnd = Get_ಕಿಟಕಿ(hwnd, GW_HWNDPREV)) != (HWND)0; )
	ಆದರೆ (Is_ಕಿಟಕಿ_Visible(hwnd))
	{
	    Get_ಕಿಟಕಿ_Rect(hwnd, &rcOther);
	    ಆದರೆ (IntersectRect(&rcDest, &rcVim, &rcOther))
		ಹಿಂತಿರುಗಿ SW_INVALIDATE;
	}
    ಹಿಂತಿರುಗಿ 0;
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
    GetPixel(s_hdc, FILL_X(gui.ಲಂಬಸಾಲು), FILL_Y(gui.ಅಡ್ಡಸಾಲು));
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಂಕ	    ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ_ಸಾಲುಗಳು)
{
    RECT	rc;

    rc.ಎಡ_2 = FILL_X(gui.scroll_region_ಎಡ);
    rc.ಬಲ_2 = FILL_X(gui.scroll_region_ಬಲ + 1);
    rc.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX() && is_ಕಿಟಕಿ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ, &rc);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(FEAT_DIRECTX)
	ಆದರೆ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ
	intel_gpu_workaround();
	Scroll_ಕಿಟಕಿ_Ex(s_ಪಠ್ಯ_Area, 0, -ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ,
				    &rc, &rc, ಖಾಲಿ, ಖಾಲಿ, get_scroll_flags());
	Update_ಕಿಟಕಿ(s_ಪಠ್ಯ_Area);
    }

    // This seems to be required to avoid the cursor disappearing when
    // scrolling such that the cursor ends up in the top-left character on
    // the screen...   But why?  (Webb)
    // It's probably fixed by disabling drawing the cursor while scrolling.
    // gui.cursor_is_valid = false;

    gui_clear_ಖಂಡ(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
						       gui.scroll_region_ಎಡ,
	gui.scroll_region_bot, gui.scroll_region_ಬಲ);
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_ಸಾಲುಗಳು)
{
    RECT	rc;

    rc.ಎಡ_2 = FILL_X(gui.scroll_region_ಎಡ);
    rc.ಬಲ_2 = FILL_X(gui.scroll_region_ಬಲ + 1);
    rc.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು);
    rc.bottom = FILL_Y(gui.scroll_region_bot + 1);

#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX() && is_ಕಿಟಕಿ_onscreen(s_hwnd))
    {
	DWriteContext_Scroll(s_dwc, 0, ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ, &rc);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(FEAT_DIRECTX)
	ಆದರೆ (IS_ENABLE_DIRECTX())
	    DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ
	intel_gpu_workaround();
	// The SW_INVALIDATE is required when part of the window is covered or
	// off-screen.  How do we avoid it when it's not needed?
	Scroll_ಕಿಟಕಿ_Ex(s_ಪಠ್ಯ_Area, 0, ಅಂಕೆ_ಸಾಲುಗಳು * gui.char_ಎತ್ತರ,
				    &rc, &rc, ಖಾಲಿ, ಖಾಲಿ, get_scroll_flags());
	Update_ಕಿಟಕಿ(s_ಪಠ್ಯ_Area);
    }

    gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
				ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
}


    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ rc ಬಳಸದ)
{
#ಆದರೆ defined(FEAT_DIRECTX)
    DWriteContext_Close(s_dwc);
    DWrite_Final();
    s_dwc = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

    ReleaseDC(s_ಪಠ್ಯ_Area, s_hdc);
    DeleteObject(s_brush);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TEAROFF
    // Unload the tearoff bitmap
    (ಶೂನ್ಯ)DeleteObject((HGDIOBJ)s_htearbitmap);
#ಆದರೆ_ಅಂತ್ಯ

    // Destroy our window (if we have one).
    ಆದರೆ (s_hwnd != ಖಾಲಿ)
    {
	destroying = TRUE;	// ignore WM_DESTROY message now
	Destroy_ಕಿಟಕಿ(s_hwnd);
    }
}

    ಸ್ಥಿರ char_u *
logfont2name(LOGFONTW lf)
{
    ಅಕ್ಷರ	*charset_name;
    ಅಕ್ಷರ	*quality_name;
    ಅಕ್ಷರ	*font_name;
    size_t	res_ಗಾತ್ರ;
    ಅಕ್ಷರ	*res;

    font_name = (ಅಕ್ಷರ *)utf16_ಗಮ್ಯ_enc(lf.lfFaceName, ಖಾಲಿ);
    ಆದರೆ (font_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    charset_name = charset_id2name((ಪೂರ್ಣಾಂಕ)lf.lf_Char_ಗಣ);
    quality_name = quality_id2name((ಪೂರ್ಣಾಂಕ)lf.lfQuality);

    res_ಗಾತ್ರ = STRLEN(font_name) + 30
		    + (charset_name == ಖಾಲಿ ? 0 : STRLEN(charset_name) + 2)
		    + (quality_name == ಖಾಲಿ ? 0 : STRLEN(quality_name) + 2);
    res = alloc(res_ಗಾತ್ರ);
    ಆದರೆ (res != ಖಾಲಿ)
    {
	ಅಕ್ಷರ	*p;
	ಪೂರ್ಣಾಂಕ	points;
	size_t	res_len;

	// replace spaces in font_name with underscores.
	ಗಾಗಿ (p = font_name; *p != NUL; ++p)
	{
	    ಆದರೆ (ಖಾಲಿಯೇ(*p))
		*p = '_';
	}

	// make a normal font string out of the lf thing:
	points = pixels_ಗಮ್ಯ_points(
			 lf.lf_ಎತ್ತರ < 0 ? -lf.lf_ಎತ್ತರ : lf.lf_ಎತ್ತರ, TRUE);
	ಆದರೆ (lf.lf_ತೂಕ == FW_ಸಾಮಾನ್ಯ || lf.lf_ತೂಕ == FW_BOLD)
	    res_len = vim_snprintf_safelen(
		(ಅಕ್ಷರ *)res, res_ಗಾತ್ರ, "%s:h%d", font_name, points);
	ಇಲ್ಲದಿದ್ದರೆ
	    res_len = vim_snprintf_safelen(
		(ಅಕ್ಷರ *)res, res_ಗಾತ್ರ, "%s:h%d:W%ld", font_name, points, lf.lf_ತೂಕ);

	res_len += vim_snprintf_safelen(
	    (ಅಕ್ಷರ *)res + res_len,
	    res_ಗಾತ್ರ - res_len,
	    "%s%s%s%s",
	    lf.lfItalic ? ":i" : "",
	    lf.lf_ತೂಕ == FW_BOLD ? ":b" : "",
	    lf.lfUnderline ? ":u" : "",
	    lf.lf_Strike_ಹೊರಹೋಗು ? ":s" : "");

	ಆದರೆ (charset_name != ಖಾಲಿ)
	    res_len += vim_snprintf_safelen((ಅಕ್ಷರ *)res + res_len,
		res_ಗಾತ್ರ - res_len, ":c%s", charset_name);
	ಆದರೆ (quality_name != ಖಾಲಿ)
	    vim_snprintf((ಅಕ್ಷರ *)res + res_len,
		res_ಗಾತ್ರ - res_len, ":q%s", quality_name);
    }

    vim_free(font_name);
    ಹಿಂತಿರುಗಿ (char_u *)res;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
/*
 * Set correct LOGFONTW to IME.  Use 'guifontwide' if available, otherwise use
 * 'guifont'.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
update_im_font(ಶೂನ್ಯ)
{
    LOGFONTW	lf_wide, lf;

    ಆದರೆ (p_guifontwide != ಖಾಲಿ && *p_guifontwide != NUL
	    && gui.wide_font != NOFONT
	    && GetObjectW((HFONT)gui.wide_font, ಗಾತ್ರ(lf_wide), &lf_wide))
	norm_logfont = lf_wide;
    ಇಲ್ಲದಿದ್ದರೆ
	norm_logfont = sub_logfont;

    lf = norm_logfont;
    ಆದರೆ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	// Work around when PerMonitorV2 is not enabled in the process level.
	lf.lf_ಎತ್ತರ = lf.lf_ಎತ್ತರ * DEFAULT_DPI / s_dpi;
    im_ಗಣ_font(&lf);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handler of gui.wide_font (p_guifontwide) changed notification.
 */
    ಶೂನ್ಯ
gui_mch_wide_font_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    LOGFONTW lf;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    update_im_font();
#ಆದರೆ_ಅಂತ್ಯ

    gui_mch_free_font(gui.wide_ital_font);
    gui.wide_ital_font = NOFONT;
    gui_mch_free_font(gui.wide_bold_font);
    gui.wide_bold_font = NOFONT;
    gui_mch_free_font(gui.wide_boldital_font);
    gui.wide_boldital_font = NOFONT;

    ಆದರೆ (gui.wide_font
	&& GetObjectW((HFONT)gui.wide_font, ಗಾತ್ರ(lf), &lf))
    {
	ಆದರೆ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.wide_ital_font = get_font_handle(&lf);
	    lf.lfItalic = FALSE;
	}
	ಆದರೆ (lf.lf_ತೂಕ < FW_BOLD)
	{
	    lf.lf_ತೂಕ = FW_BOLD;
	    gui.wide_bold_font = get_font_handle(&lf);
	    ಆದರೆ (!lf.lfItalic)
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
    ಪೂರ್ಣಾಂಕ
gui_mch_init_font(char_u *font_name, ಪೂರ್ಣಾಂಕ fontset ಬಳಸದ)
{
    LOGFONTW	lf, lfOrig;
    GuiFont	font = NOFONT;
    char_u	*p;

    // Load the font
    ಆದರೆ (get_logfont(&lf, font_name, ಖಾಲಿ, TRUE) == OK)
    {
	lfOrig = lf;
	lf.lf_ಎತ್ತರ = adjust_fontsize_by_dpi(lf.lf_ಎತ್ತರ);
	font = get_font_handle(&lf);
    }
    ಆದರೆ (font == NOFONT)
	ಹಿಂತಿರುಗಿ FAIL;

#ಆದರೆ defined(FEAT_DIRECTX)
    // Parse font features from guifont (e.g., ":fss19=1:fcalt=0:fliga=1").
    {
	DWriteFontFeature features[DWRITE_MAX_FONT_FEATURES];
	ಪೂರ್ಣಾಂಕ		    feat_ಎಣಿಕೆ = 0;
	char_u		    *fp;

	ಆದರೆ (font_name != ಖಾಲಿ)
	{
	    // Find each ":f" option in font_name.
	    ಗಾಗಿ (fp = font_name; *fp != NUL; fp++)
	    {
		ಆದರೆ (*fp == ':' && *(fp + 1) == 'f')
		{
		    char_u ಟ್ಯಾಗ್[5];
		    ಪೂರ್ಣಾಂಕ    ti = 0;
		    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ param = 1;

		    fp += 2;  // skip ":f"
		    ಆಗ (*fp != NUL && *fp != '=' && *fp != ':'
			    && ti < 4)
			ಟ್ಯಾಗ್[ti++] = *fp++;
		    ಟ್ಯಾಗ್[ti] = NUL;

		    ಆದರೆ (ti != 4)
			ಮುಂದುವರಿ;  // invalid tag length

		    ಆದರೆ (*fp == '=')
		    {
			fp++;
			param = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)atoi((ಅಕ್ಷರ *)fp);
			ಆಗ (*fp >= '0' && *fp <= '9')
			    fp++;
		    }

		    ಆದರೆ (feat_ಎಣಿಕೆ < DWRITE_MAX_FONT_FEATURES)
		    {
			features[feat_ಎಣಿಕೆ].ಟ್ಯಾಗ್ =
			    ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಟ್ಯಾಗ್[0])
			    | ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಟ್ಯಾಗ್[1] << 8)
			    | ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಟ್ಯಾಗ್[2] << 16)
			    | ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)ಟ್ಯಾಗ್[3] << 24);
			features[feat_ಎಣಿಕೆ].parameter = param;
			feat_ಎಣಿಕೆ++;
		    }

		    fp--;  // adjust for loop increment
		}
	    }
	}
	DWrite_Context_ಗಣ_Font_Features(s_dwc, features, feat_ಎಣಿಕೆ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (font_name == ಖಾಲಿ)
	font_name = (char_u *)"";
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    norm_logfont = lf;
    sub_logfont = lf;
    ಆದರೆ (!s_ಒಳ_dpichanged)
	update_im_font();
#ಆದರೆ_ಅಂತ್ಯ
    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font;
    current_font_ಎತ್ತರ = lfOrig.lf_ಎತ್ತರ;
    Update_Font_ಗಾತ್ರ(font);

    p = logfont2name(lfOrig);
    ಆದರೆ (p != ಖಾಲಿ)
    {
	hl_ಗಣ_font_name(p);

	// When setting 'guifont' to "*" replace it with the actual font name.
	ಆದರೆ (STRCMP(font_name, "*") == 0 && STRCMP(p_guifont, "*") == 0)
	{
	    vim_free(p_guifont);
	    p_guifont = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(p);
    }

    gui_mch_free_font(gui.ital_font);
    gui.ital_font = NOFONT;
    gui_mch_free_font(gui.bold_font);
    gui.bold_font = NOFONT;
    gui_mch_free_font(gui.boldital_font);
    gui.boldital_font = NOFONT;

    ಆದರೆ (!lf.lfItalic)
    {
	lf.lfItalic = TRUE;
	gui.ital_font = get_font_handle(&lf);
	lf.lfItalic = FALSE;
    }
    ಆದರೆ (lf.lf_ತೂಕ < FW_BOLD)
    {
	lf.lf_ತೂಕ = FW_BOLD;
	gui.bold_font = get_font_handle(&lf);
	ಆದರೆ (!lf.lfItalic)
	{
	    lf.lfItalic = TRUE;
	    gui.boldital_font = get_font_handle(&lf);
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if the GUI window is maximized, filling the whole screen.
 * Also return TRUE if the window is snapped.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_maximized(ಶೂನ್ಯ)
{
    WINDOWPLACEMENT wp;
    RECT	    rc;

    wp.ಉದ್ದಳತೆ = ಗಾತ್ರ(WINDOWPLACEMENT);
    ಆದರೆ (Get_ಕಿಟಕಿ_Placement(s_hwnd, &wp))
    {
	ಆದರೆ (wp.showCmd == SW_SHOWMAXIMIZED
	    || (wp.showCmd == SW_SHOWMINIMIZED
		    && wp.flags == WPF_RESTORETOMAXIMIZED))
	    ಹಿಂತಿರುಗಿ TRUE;
	ಆದರೆ (wp.showCmd == SW_SHOWMINIMIZED)
	    ಹಿಂತಿರುಗಿ FALSE;

	// Assume the window is snapped when the sizes from two APIs differ.
	Get_ಕಿಟಕಿ_Rect(s_hwnd, &rc);
	ಆದರೆ ((rc.ಬಲ_2 - rc.ಎಡ_2 !=
		    wp.rc_ಸಾಮಾನ್ಯ_Position.ಬಲ_2 - wp.rc_ಸಾಮಾನ್ಯ_Position.ಎಡ_2)
		|| (rc.bottom - rc.ತುದಿ !=
		    wp.rc_ಸಾಮಾನ್ಯ_Position.bottom - wp.rc_ಸಾಮಾನ್ಯ_Position.ತುದಿ))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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

    Get_ಕಿಟಕಿ_Rect(s_hwnd, &rect);
    ಆದರೆ (win_socket_id == 0)
    {
	gui_resize_shell(rect.ಬಲ_2 - rect.ಎಡ_2
	    - (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2,
	    rect.bottom - rect.ತುದಿ
	    - (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
	       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
	    - pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
	    - gui_mswin_get_menu_ಎತ್ತರ(FALSE));
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Inside another window, don't use the frame and border.
	gui_resize_shell(rect.ಬಲ_2 - rect.ಎಡ_2,
	    rect.bottom - rect.ತುದಿ - gui_mswin_get_menu_ಎತ್ತರ(FALSE));
    }
}

/*
 * Set the window title
 */
    ಶೂನ್ಯ
gui_mch_settitle(
    char_u  *title,
    char_u  *icon ಬಳಸದ)
{
    ಗಣ_ಕಿಟಕಿ_title(s_hwnd, (title == ಖಾಲಿ ? "VIM" : (ಅಕ್ಷರ *)title));
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)
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
mch_ಗಣ_mouse_shape(ಪೂರ್ಣಾಂಕ shape)
{
    LPCSTR idc;

    ಆದರೆ (shape == MSHAPE_HIDE)
	ShowCursor(FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shape >= MSHAPE_NUMBERED)
	    idc = IDC_ARROW;
	ಇಲ್ಲದಿದ್ದರೆ
	    idc = mshape_idcs[shape];
	ಗಣ_Class_Long_Ptr(s_ಪಠ್ಯ_Area, GCLP_HCURSOR, (LONG_PTR)ತುಂಬು_Cursor(ಖಾಲಿ, idc));
	ಆದರೆ (!p_mh)
	{
	    ಬಿಂದು_2 mp;

	    // Set the position to make it redrawn with the new shape.
	    (ಶೂನ್ಯ)GetCursorPos(&mp);
	    (ಶೂನ್ಯ)ಗಣ_Cursor_Pos(mp.x, mp.y);
	    ShowCursor(TRUE);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BROWSE)
/*
 * Wide version of convert_filter().
 */
    ಸ್ಥಿರ WCHAR *
convert_filterW(char_u *s)
{
    char_u *tmp;
    ಪೂರ್ಣಾಂಕ len;
    WCHAR *res;

    tmp = convert_filter(s);
    ಆದರೆ (tmp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    len = (ಪೂರ್ಣಾಂಕ)STRLEN(s) + 3;
    res = enc_ಗಮ್ಯ_utf16(tmp, &len);
    vim_free(tmp);
    ಹಿಂತಿರುಗಿ res;
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
	ಪೂರ್ಣಾಂಕ saving,
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
    ಪೂರ್ಣಾಂಕ			i;
    WCHAR		*titlep = ಖಾಲಿ;
    WCHAR		*extp = ಖಾಲಿ;
    WCHAR		*initdirp = ಖಾಲಿ;
    WCHAR		*filterp;
    char_u		*p, *q;
    BOOL		ret;

    ಆದರೆ (dflt == ಖಾಲಿ)
	fileBuf[0] = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	wp = enc_ಗಮ್ಯ_utf16(dflt, ಖಾಲಿ);
	ಆದರೆ (wp == ಖಾಲಿ)
	    fileBuf[0] = NUL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (i = 0; wp[i] != NUL && i < MAXPATHL - 1; ++i)
		fileBuf[i] = wp[i];
	    fileBuf[i] = NUL;
	    vim_free(wp);
	}
    }

    // Convert the filter to Windows format.
    filterp = convert_filterW(filter);

    CLEAR_ಕ್ಷೇತ್ರ(fileStruct);
# ifdef OPENFILENAME_ಗಾತ್ರ_ಆವೃತ್ತಿ_400_W
    // be compatible with Windows NT 4.0
    fileStruct.l_Struct_ಗಾತ್ರ = OPENFILENAME_ಗಾತ್ರ_ಆವೃತ್ತಿ_400_W;
# ಇಲ್ಲದಿದ್ದರೆ
    fileStruct.l_Struct_ಗಾತ್ರ = ಗಾತ್ರ(fileStruct);
# endif

    ಆದರೆ (title != ಖಾಲಿ)
	titlep = enc_ಗಮ್ಯ_utf16(title, ಖಾಲಿ);
    fileStruct.lpstrTitle = titlep;

    ಆದರೆ (ext != ಖಾಲಿ)
	extp = enc_ಗಮ್ಯ_utf16(ext, ಖಾಲಿ);
    fileStruct.lpstrDefExt = extp;

    fileStruct.lpstrFile = fileBuf;
    fileStruct.nMaxFile = MAXPATHL;
    fileStruct.lpstrFilter = filterp;
    fileStruct.hwndOwner = s_hwnd;		// main Vim window is owner
    // has an initial dir been specified?
    ಆದರೆ (initdir != ಖಾಲಿ && *initdir != NUL)
    {
	// Must have backslashes here, no matter what 'shellslash' says
	initdirp = enc_ಗಮ್ಯ_utf16(initdir, ಖಾಲಿ);
	ಆದರೆ (initdirp != ಖಾಲಿ)
	{
	    ಗಾಗಿ (wp = initdirp; *wp != NUL; ++wp)
		ಆದರೆ (*wp == '/')
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
    ಆದರೆ (curbuf->b_p_bin)
	fileStruct.Flags |= OFN_NODEREFERENCELINKS;
# endif
    ಆದರೆ (saving)
	ret = GetSaveFileNameW(&fileStruct);
    ಇಲ್ಲದಿದ್ದರೆ
	ret = GetOpenFileNameW(&fileStruct);

    vim_free(filterp);
    vim_free(initdirp);
    vim_free(titlep);
    vim_free(extp);

    ಆದರೆ (!ret)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Convert from UTF-16 to 'encoding'.
    p = utf16_ಗಮ್ಯ_enc(fileBuf, ಖಾಲಿ);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Give focus back to main window (when using MDI).
    ಗಣ_Focus(s_hwnd);

    // Shorten the file name if possible
    q = vim_strsave(shorten_fname1(p));
    vim_free(p);
    ಹಿಂತಿರುಗಿ q;
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
    ಚಿಹ್ನೆರಹಿತ	s_len = (ಚಿಹ್ನೆರಹಿತ)STRLEN(s);
    ಚಿಹ್ನೆರಹಿತ	i;

    res = alloc(s_len + 3);
    ಆದರೆ (res != ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < s_len; ++i)
	    ಆದರೆ (s[i] == '\t' || s[i] == '\n')
		res[i] = '\0';
	    ಇಲ್ಲದಿದ್ದರೆ
		res[i] = s[i];
	res[s_len] = NUL;
	// Add two extra NULs to make sure it's properly terminated.
	res[s_len + 1] = NUL;
	res[s_len + 2] = NUL;
    }
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Select a directory.
 */
    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    // We fake this: Use a filter that doesn't select anything and a default
    // file name that won't be used.
    ಹಿಂತಿರುಗಿ gui_mch_browse(0, title, (char_u *)_("Not Used"), ಖಾಲಿ,
			      initdir, (char_u *)_("Directory\t*.nothing\n"));
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_BROWSE

    ಸ್ಥಿರ ಶೂನ್ಯ
_OnDropFiles(
    HWND hwnd ಬಳಸದ,
    HDROP hDrop)
{
#ವ್ಯಾಖ್ಯಾನಿಸು BUFPATHLEN MAX_ಮಾರ್ಗ_2
#ವ್ಯಾಖ್ಯಾನಿಸು DRAGQVAL 0xFFFFFFFF
    WCHAR   wszFile[BUFPATHLEN];
    ಅಕ್ಷರ    szFile[BUFPATHLEN];
    UINT    cFiles = DragQueryFile(hDrop, DRAGQVAL, ಖಾಲಿ, 0);
    UINT    i;
    char_u  **fnames;
    ಬಿಂದು_2   pt;
    int_u   modifiers = 0;

    // Obtain dropped position
    Drag_Query_ಬಿಂದು(hDrop, &pt);
    Map_ಕಿಟಕಿ_Points(s_hwnd, s_ಪಠ್ಯ_Area, &pt, 1);

    reset_VIsual();

    fnames = ALLOC_MULT(char_u *, cFiles);

    ಆದರೆ (fnames != ಖಾಲಿ)
	ಗಾಗಿ (i = 0; i < cFiles; ++i)
	{
	    ಆದರೆ (DragQueryFileW(hDrop, i, wszFile, BUFPATHLEN) > 0)
		fnames[i] = utf16_ಗಮ್ಯ_enc(wszFile, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		DragQueryFile(hDrop, i, szFile, BUFPATHLEN);
		fnames[i] = vim_strsave((char_u *)szFile);
	    }
	}

    Drag_ಮುಕ್ತಾಯ(hDrop);

    ಆದರೆ (fnames == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ kbd_modifiers = get_ಸಕ್ರಿಯ_modifiers();

    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) != 0)
	modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_ALT) != 0)
	modifiers |= MOUSE_ALT;

    gui_handle_drop(pt.x, pt.y, modifiers, fnames, cFiles);

    s_need_activate = TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_OnScroll(
    HWND hwnd ಬಳಸದ,
    HWND hwndCtl,
    UINT ಸಂಕೇತ_2,
    ಪೂರ್ಣಾಂಕ pos)
{
    ಸ್ಥಿರ UINT	prev_ಸಂಕೇತ = 0;   // code of previous call
    scrollbar_T *sb, *sb_info;
    ದೀರ್ಘ	val;
    ಪೂರ್ಣಾಂಕ		dragging = FALSE;
    ಪೂರ್ಣಾಂಕ		dont_scroll_save = dont_scroll;
    SCROLLINFO	si;

    si.cb_ಗಾತ್ರ = ಗಾತ್ರ(si);
    si.f_ಮುಖವಾಡ = SIF_POS;

    sb = gui_mswin_ಕಂಡುಹಿಡಿ_scrollbar(hwndCtl);
    ಆದರೆ (sb == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (sb->wp != ಖಾಲಿ)		// Left or right scrollbar
    {
	/*
	 * Careful: need to get scrollbar info out of first (left) scrollbar
	 * for window, but keep real scrollbar too because we must pass it to
	 * gui_drag_scrollbar().
	 */
	sb_info = &sb->wp->w_scrollbars[0];
    }
    ಇಲ್ಲದಿದ್ದರೆ	    // Bottom scrollbar
	sb_info = sb;
    val = sb_info->ಮೌಲ್ಯ;

    ಆಯ್ಕೆ (ಸಂಕೇತ_2)
    {
	ಸ್ಥಿತಿ SB_THUMBTRACK:
	    val = pos;
	    dragging = TRUE;
	    ಆದರೆ (sb->scroll_ಸರಿಸು > 0)
		val <<= sb->scroll_ಸರಿಸು;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_LINEDOWN:
	    val++;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_LINEUP:
	    val--;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_PAGEDOWN:
	    val += (sb_info->ಗಾತ್ರ_2 > 2 ? sb_info->ಗಾತ್ರ_2 - 2 : 1);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_PAGEUP:
	    val -= (sb_info->ಗಾತ್ರ_2 > 2 ? sb_info->ಗಾತ್ರ_2 - 2 : 1);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_ತುದಿ:
	    val = 0;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_BOTTOM:
	    val = sb_info->max;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SB_ENDSCROLL:
	    ಆದರೆ (prev_ಸಂಕೇತ == SB_THUMBTRACK)
	    {
		/*
		 * "pos" only gives us 16-bit data.  In case of large file,
		 * use GetScrollPos() which returns 32-bit.  Unfortunately it
		 * is not valid while the scrollbar is being dragged.
		 */
		val = GetScrollPos(hwndCtl, SB_CTL);
		ಆದರೆ (sb->scroll_ಸರಿಸು > 0)
		    val <<= sb->scroll_ಸರಿಸು;
	    }
	    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
	    // TRACE("Unknown scrollbar event %d\n", code);
	    ಹಿಂತಿರುಗಿ 0;
    }
    prev_ಸಂಕೇತ = ಸಂಕೇತ_2;

    si.nPos = (sb->scroll_ಸರಿಸು > 0) ? val >> sb->scroll_ಸರಿಸು : val;
    ಗಣ_Scroll_Info(hwndCtl, SB_CTL, &si, TRUE);

    /*
     * When moving a vertical scrollbar, move the other vertical scrollbar too.
     */
    ಆದರೆ (sb->wp != ಖಾಲಿ)
    {
	scrollbar_T *sba = sb->wp->w_scrollbars;
	HWND    id = sba[ (sb == sba + SBAR_ಎಡ) ? SBAR_ಬಲ : SBAR_ಎಡ].id;

	ಗಣ_Scroll_Info(id, SB_CTL, &si, TRUE);
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

    ಹಿಂತಿರುಗಿ 0;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XPM_W32
# include "xpm_w32.h"
#ಆದರೆ_ಅಂತ್ಯ


// Some parameters for tearoff menus.  All in pixels.
#ವ್ಯಾಖ್ಯಾನಿಸು TEAROFF_PADDING_X	2
#ವ್ಯಾಖ್ಯಾನಿಸು TEAROFF_BUTTON_PAD_X	8
#ವ್ಯಾಖ್ಯಾನಿಸು TEAROFF_MIN_ಅಗಲ	200
#ವ್ಯಾಖ್ಯಾನಿಸು TEAROFF_SUBMENU_LABEL	">>"
#ವ್ಯಾಖ್ಯಾನಿಸು TEAROFF_ಸ್ತಂಭ_PADDING	3	// # spaces to pad column with.


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
# define ID_BEVAL_TOOLTIP   200
# define BEVAL_ಪಠ್ಯ_LEN	    MAXPATHL

ಸ್ಥಿರ BalloonEval  *cur_beval = ಖಾಲಿ;
ಸ್ಥಿರ UINT_PTR	    beval_timer_id = 0;
ಸ್ಥಿರ DWORD	    ಕೊನೆಯ_user_activity = 0;
#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_BEVAL_GUI)


// Local variables:

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
ಸ್ಥಿರ UINT	s_menu_id = 100;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Use the system font for dialogs and tear-off menus.  Remove this line to
 * use DLG_FONT_NAME.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು USE_SYSMENU_FONT

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_NAME	"vim"
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_CLASSW	L"Vim"

// Initial size for the dialog template.  For gui_mch_dialog() it's fixed,
// thus there should be room for every dialog.  For tearoffs it's made bigger
// when needed.
#ವ್ಯಾಖ್ಯಾನಿಸು DLG_ALLOC_ಗಾತ್ರ 16 * 1024

/*
 * stuff for dialogs, menus, tearoffs etc.
 */
ಸ್ಥಿರ PWORD
ಕೂಡಿಸು_dialog_element(
	PWORD p,
	DWORD lStyle,
	ಶಬ್ದ_2 x,
	ಶಬ್ದ_2 y,
	ಶಬ್ದ_2 w,
	ಶಬ್ದ_2 h,
	ಶಬ್ದ_2 Id,
	ಶಬ್ದ_2 clss,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *caption);
ಸ್ಥಿರ LPWORD lpwAlign(LPWORD);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(LPWORD, LPSTR, BOOL);
#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
ಸ್ಥಿರ ಶೂನ್ಯ gui_mch_tearoff(char_u *title, vimmenu_T *menu, ಪೂರ್ಣಾಂಕ initX, ಪೂರ್ಣಾಂಕ initY);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ get_dialog_font_metrics(ಶೂನ್ಯ);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dialog_default_button = -1;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
ಸ್ಥಿರ ಶೂನ್ಯ initialise_toolbar(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ update_toolbar_ಗಾತ್ರ(ಶೂನ್ಯ);
ಸ್ಥಿರ LRESULT CALLBACK toolbar_wndproc(HWND hwnd, UINT u_ಸಂದೇಶ, WPARAM wParam, LPARAM lParam);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_toolbar_bitmap(vimmenu_T *menu);
#ಇಲ್ಲದಿದ್ದರೆ
# define update_toolbar_ಗಾತ್ರ()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
ಸ್ಥಿರ ಶೂನ್ಯ initialise_tabline(ಶೂನ್ಯ);
ಸ್ಥಿರ LRESULT CALLBACK tabline_wndproc(HWND hwnd, UINT u_ಸಂದೇಶ, WPARAM wParam, LPARAM lParam);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
ಸ್ಥಿರ LRESULT _OnImeComposition(HWND hwnd, WPARAM dbcs, LPARAM param);
ಸ್ಥಿರ char_u *Get_ಫಲಿತಾಂಶ_Str(HWND hwnd, ಪೂರ್ಣಾಂಕ GCS, ಪೂರ್ಣಾಂಕ *lenp);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
# ifdef NOIME
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ಟ್ಯಾಗ್_COMPOSITIONFORM {
    DWORD dwStyle;
    ಬಿಂದು_2 ptCurrentPos;
    RECT  rcArea;
} COMPOSITIONFORM, *PCOMPOSITIONFORM, NEAR *NPCOMPOSITIONFORM, FAR *LPCOMPOSITIONFORM;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ HANDLE HIMC;
# endif

ಸ್ಥಿರ HINSTANCE hLibImm = ಖಾಲಿ;
ಸ್ಥಿರ LONG (WINAPI *pImmGetCompositionStringW)(HIMC, DWORD, LPVOID, DWORD);
ಸ್ಥಿರ HIMC (WINAPI *pImmGetContext)(HWND);
ಸ್ಥಿರ HIMC (WINAPI *pImmAssociateContext)(HWND, HIMC);
ಸ್ಥಿರ BOOL (WINAPI *pImmReleaseContext)(HWND, HIMC);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_Get_Open_ಸ್ಥಿತಿ)(HIMC);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_ಗಣ_Open_ಸ್ಥಿತಿ)(HIMC, BOOL);
ಸ್ಥಿರ BOOL (WINAPI *pImmGetCompositionFontW)(HIMC, LPLOGFONTW);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_ಗಣ_Composition_Font_W)(HIMC, LPLOGFONTW);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_ಗಣ_Composition_ಕಿಟಕಿ)(HIMC, LPCOMPOSITIONFORM);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_Get_Conversion_ಸ್ಥಿತಿ)(HIMC, LPDWORD, LPDWORD);
ಸ್ಥಿರ BOOL (WINAPI *p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ)(HIMC, DWORD, DWORD);
ಸ್ಥಿರ ಶೂನ್ಯ dyn_imm_ತುಂಬು(ಶೂನ್ಯ);
#ಇಲ್ಲದಿದ್ದರೆ
# define pImmGetCompositionStringW ImmGetCompositionStringW
# define pImmGetContext		  ImmGetContext
# define pImmAssociateContext	  ImmAssociateContext
# define pImmReleaseContext	  ImmReleaseContext
# define p_Imm_Get_Open_ಸ್ಥಿತಿ	  Imm_Get_Open_ಸ್ಥಿತಿ
# define p_Imm_ಗಣ_Open_ಸ್ಥಿತಿ	  Imm_ಗಣ_Open_ಸ್ಥಿತಿ
# define pImmGetCompositionFontW  ImmGetCompositionFontW
# define p_Imm_ಗಣ_Composition_Font_W  Imm_ಗಣ_Composition_Font_W
# define p_Imm_ಗಣ_Composition_ಕಿಟಕಿ Imm_ಗಣ_Composition_ಕಿಟಕಿ
# define p_Imm_Get_Conversion_ಸ್ಥಿತಿ  Imm_Get_Conversion_ಸ್ಥಿತಿ
# define p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ  Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
/*
 * Figure out how high the menu bar is at the moment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_mswin_get_menu_ಎತ್ತರ(
    ಪೂರ್ಣಾಂಕ	    fix_ಕಿಟಕಿ)	    // If TRUE, resize window if menu height changed
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	old_menu_ಎತ್ತರ = -1;

    RECT    rc1, rc2;
    ಪೂರ್ಣಾಂಕ	    ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ	    menu_ಎತ್ತರ;

    ಆದರೆ (gui.menu_is_ಸಕ್ರಿಯ)
	ಅಂಕೆ = Get_Menu_ಅಂಶ_ಎಣಿಕೆ(s_menuBar);
    ಇಲ್ಲದಿದ್ದರೆ
	ಅಂಕೆ = 0;

    ಆದರೆ (ಅಂಕೆ == 0)
	menu_ಎತ್ತರ = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IsMinimized(s_hwnd))
    {
	// The height of the menu cannot be determined while the window is
	// minimized.  Take the previous height if the menu is changed in that
	// state, to avoid that Vim's vertical window size accidentally
	// increases due to the unaccounted-for menu height.
	menu_ಎತ್ತರ = old_menu_ಎತ್ತರ == -1 ? 0 : old_menu_ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * In case 'lines' is set in _vimrc/_gvimrc window width doesn't
	 * seem to have been set yet, so menu wraps in default window
	 * width which is very narrow.  Instead just return height of a
	 * single menu item.  Will still be wrong when the menu really
	 * should wrap over more than one line.
	 */
	Get_Menu_ಅಂಶ_Rect(s_hwnd, s_menuBar, 0, &rc1);
	ಆದರೆ (gui.starting)
	    menu_ಎತ್ತರ = rc1.bottom - rc1.ತುದಿ + 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Get_Menu_ಅಂಶ_Rect(s_hwnd, s_menuBar, ಅಂಕೆ - 1, &rc2);
	    menu_ಎತ್ತರ = rc2.bottom - rc1.ತುದಿ + 1;
	}
    }

    ಆದರೆ (fix_ಕಿಟಕಿ && menu_ಎತ್ತರ != old_menu_ಎತ್ತರ)
	gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_VERT);
    old_menu_ಎತ್ತರ = menu_ಎತ್ತರ;

    ಹಿಂತಿರುಗಿ menu_ಎತ್ತರ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU


/*
 * Setup for the Intellimouse
 */
    ಸ್ಥಿರ ದೀರ್ಘ
mouse_vertical_scroll_ಹೆಜ್ಜೆ(ಶೂನ್ಯ)
{
    UINT val;
    ಆದರೆ (SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &val, 0))
	ಹಿಂತಿರುಗಿ (val != WHEEL_PAGESCROLL) ? (ದೀರ್ಘ)val : -1;
    ಹಿಂತಿರುಗಿ 3; // Safe default;
}

    ಸ್ಥಿರ ದೀರ್ಘ
mouse_horizontal_scroll_ಹೆಜ್ಜೆ(ಶೂನ್ಯ)
{
    UINT val;
    ಆದರೆ (SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &val, 0))
	ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)val;
    ಹಿಂತಿರುಗಿ 3; // Safe default;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
init_mouse_wheel(ಶೂನ್ಯ)
{
    // Get the default values for the horizontal and vertical scroll steps from
    // the system.
    mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(mouse_vertical_scroll_ಹೆಜ್ಜೆ());
    mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(mouse_horizontal_scroll_ಹೆಜ್ಜೆ());
}

/*
 * Mouse scroll event handler.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
_OnMouseWheel(HWND hwnd ಬಳಸದ, WPARAM wParam, LPARAM lParam, ಪೂರ್ಣಾಂಕ horizontal)
{
    ಪೂರ್ಣಾಂಕ		button;
    win_T	*wp;
    ಪೂರ್ಣಾಂಕ		modifiers = 0;
    ಪೂರ್ಣಾಂಕ		kbd_modifiers;
    ಪೂರ್ಣಾಂಕ		zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    ಬಿಂದು_2	pt;

    wp = gui_mouse_ಕಿಟಕಿ(ಕಂಡುಹಿಡಿ_POPUP);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROP_POPUP
    ಆದರೆ (wp != ಖಾಲಿ && popup_is_popup(wp))
    {
	cmdarg_T cap;
	oparg_T	oa;

	// Mouse hovers over popup window, scroll it if possible.
	mouse_ಅಡ್ಡಸಾಲು = wp->w_winrow;
	mouse_ಲಂಬಸಾಲು = wp->w_wincol;
	CLEAR_ಕ್ಷೇತ್ರ(cap);
	ಆದರೆ (horizontal)
	{
	    cap.arg = zDelta < 0 ? MSCR_ಎಡ : MSCR_ಬಲ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSELEFT : K_MOUSERIGHT;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    cap.arg = zDelta < 0 ? MSCR_ಮೇಲೆ : MSCR_ಕೆಳಕ್ಕೆ;
	    cap.cmdchar = zDelta < 0 ? K_MOUSEUP : K_MOUSEDOWN;
	}
	clear_oparg(&oa);
	cap.oap = &oa;
	nv_mousescroll(&cap);
	update_screen(0);
	setcursor();
	ಹೊರಹೋಗು_flush();
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (wp == ಖಾಲಿ || !p_scf)
	wp = curwin;

    // Translate the scroll event into an event that Vim can process so that
    // the user has a chance to map the scrollwheel buttons.
    ಆದರೆ (horizontal)
	button = zDelta >= 0 ? MOUSE_6 : MOUSE_7;
    ಇಲ್ಲದಿದ್ದರೆ
	button = zDelta >= 0 ? MOUSE_4 : MOUSE_5;

    kbd_modifiers = get_ಸಕ್ರಿಯ_modifiers();

    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_ಸರಿಸು) != 0)
	modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_CTRL) != 0)
	modifiers |= MOUSE_CTRL;
    ಆದರೆ ((kbd_modifiers & MOD_ಮುಖವಾಡ_ALT) != 0)
	modifiers |= MOUSE_ALT;

    // The cursor position is relative to the upper-left corner of the screen.
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);
    Screen_ಗಮ್ಯ_Client(s_ಪಠ್ಯ_Area, &pt);

    gui_send_mouse_event(button, pt.x, pt.y, FALSE, modifiers);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SYSMENU_FONT
/*
 * Get Menu Font.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gui_w32_get_menu_font(LOGFONTW *lf)
{
    NONCLIENTMETRICSW nm;

    nm.cb_ಗಾತ್ರ = ಗಾತ್ರ(NONCLIENTMETRICSW);
    ಆದರೆ (!SystemParametersInfoW(
	    SPI_GETNONCLIENTMETRICS,
	    ಗಾತ್ರ(NONCLIENTMETRICSW),
	    &nm,
	    0))
	ಹಿಂತಿರುಗಿ FAIL;
    *lf = nm.lfMenuFont;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_GUI_TABLINE) && defined(USE_SYSMENU_FONT)
/*
 * Set the GUI tabline font to the system menu font
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tabline_font(ಶೂನ್ಯ)
{
    LOGFONTW	lfSysmenu;
    HFONT	font;
    HWND	hwnd;
    HDC		hdc;
    HFONT	hfntOld;
    TEXTMETRIC	tm;

    ಆದರೆ (gui_w32_get_menu_font(&lfSysmenu) != OK)
	ಹಿಂತಿರುಗಿ;

    lfSysmenu.lf_ಎತ್ತರ = adjust_fontsize_by_dpi(lfSysmenu.lf_ಎತ್ತರ);
    font = Create_Font_ಪರೋಕ್ಷ_W(&lfSysmenu);

    Send_ಸಂದೇಶ(s_tabhwnd, WM_SETFONT, (WPARAM)font, TRUE);

    /*
     * Compute the height of the font used for the tab text
     */
    hwnd = Get_Desktop_ಕಿಟಕಿ();
    hdc = Get_ಕಿಟಕಿ_DC(hwnd);
    hfntOld = SelectFont(hdc, font);

    Get_ಪಠ್ಯ_Metrics(hdc, &tm);

    SelectFont(hdc, hfntOld);
    ReleaseDC(hwnd, hdc);

    /*
     * The space used by the tab border and the space between the tab label
     * and the tab border is included as 7.
     */
    gui.tabline_ಎತ್ತರ = tm.tm_ಎತ್ತರ + tm.tm_ಆಂತರಿಕ_Leading + 7;
}
#ಇಲ್ಲದಿದ್ದರೆ
# define ಗಣ_tabline_font()
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Invoked when a setting was changed.
 */
    ಸ್ಥಿರ LRESULT CALLBACK
_OnSettingChange(UINT param)
{
    ಆಯ್ಕೆ (param)
    {
	ಸ್ಥಿತಿ SPI_SETWHEELSCROLLLINES:
	    mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(mouse_vertical_scroll_ಹೆಜ್ಜೆ());
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SPI_SETWHEELSCROLLCHARS:
	    mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(mouse_horizontal_scroll_ಹೆಜ್ಜೆ());
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ SPI_SETNONCLIENTMETRICS:
	    ಗಣ_tabline_font();
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    ಸ್ಥಿರ ಶೂನ್ಯ
On_ಕಿಟಕಿ_Pos_ಬದಲಾಯಿತು(
    HWND hwnd,
    ಸ್ಥಿರಾಂಕ LPWINDOWPOS lpwpos)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ x = 0, y = 0, cx = 0, cy = 0;
    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ WSInitialized;

    ಆದರೆ (WSInitialized && (lpwpos->x != x || lpwpos->y != y
				     || lpwpos->cx != cx || lpwpos->cy != cy))
    {
	x = lpwpos->x;
	y = lpwpos->y;
	cx = lpwpos->cx;
	cy = lpwpos->cy;
	netbeans_ಚೌಕಟ್ಟು_moved(x, y);
    }
    // Allow to send WM_SIZE and WM_MOVE
    FORWARD_WM_WINDOWPOSCHANGED(hwnd, lpwpos, Def_ಕಿಟಕಿ_Proc_W);
}
#ಆದರೆ_ಅಂತ್ಯ


ಸ್ಥಿರ HWND hwndTip = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
show_sizing_tip(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2)
{
    TOOLINFOA ti;
    ಅಕ್ಷರ buf[32];

    ti.cb_ಗಾತ್ರ = ಗಾತ್ರ(ti);
    ti.hwnd = s_hwnd;
    ti.uId = (UINT_PTR)s_hwnd;
    ti.uFlags = TTF_SUBCLASS | TTF_IDISHWND;
    ti.lpsz_ಪಠ್ಯ = buf;
    sprintf(buf, "%dx%d", ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಅಡ್ಡಸಾಲುಗಳು_2);
    ಆದರೆ (hwndTip == ಖಾಲಿ)
    {
	hwndTip = Create_ಕಿಟಕಿ_Ex_A(0, TOOLTIPS_CLASSA, ಖಾಲಿ,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		s_hwnd, ಖಾಲಿ, GetModuleHandle(ಖಾಲಿ), ಖಾಲಿ);
	Send_ಸಂದೇಶ(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
	Send_ಸಂದೇಶ(hwndTip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Send_ಸಂದೇಶ(hwndTip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
    }
    Send_ಸಂದೇಶ(hwndTip, TTM_POPUP, 0, 0);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
destroy_sizing_tip(ಶೂನ್ಯ)
{
    ಆದರೆ (hwndTip == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    Destroy_ಕಿಟಕಿ(hwndTip);
    hwndTip = ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
_DuringSizing(
    UINT fwSide,
    LPRECT lprc)
{
    ಪೂರ್ಣಾಂಕ	    w, h;
    ಪೂರ್ಣಾಂಕ	    valid_w, valid_h;
    ಪೂರ್ಣಾಂಕ	    w_ಸ್ಥಾನಾಂತರ, h_ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ	    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಅಡ್ಡಸಾಲುಗಳು_2;

    w = lprc->ಬಲ_2 - lprc->ಎಡ_2;
    h = lprc->bottom - lprc->ತುದಿ;
    gui_mswin_get_valid_dimensions(w, h, &valid_w, &valid_h, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, &ಅಡ್ಡಸಾಲುಗಳು_2);
    w_ಸ್ಥಾನಾಂತರ = w - valid_w;
    h_ಸ್ಥಾನಾಂತರ = h - valid_h;

    ಆದರೆ (fwSide == WMSZ_ಎಡ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_BOTTOMLEFT)
	lprc->ಎಡ_2 += w_ಸ್ಥಾನಾಂತರ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fwSide == WMSZ_ಬಲ || fwSide == WMSZ_TOPRIGHT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->ಬಲ_2 -= w_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (fwSide == WMSZ_ತುದಿ || fwSide == WMSZ_TOPLEFT
			    || fwSide == WMSZ_TOPRIGHT)
	lprc->ತುದಿ += h_ಸ್ಥಾನಾಂತರ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMLEFT
			    || fwSide == WMSZ_BOTTOMRIGHT)
	lprc->bottom -= h_ಸ್ಥಾನಾಂತರ;

    show_sizing_tip(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಅಡ್ಡಸಾಲುಗಳು_2);
    ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಸ್ಥಿರ ಶೂನ್ಯ
On_RButton_ಮೇಲೆ(HWND hwnd, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, UINT ಕೀಲಿ_Flags)
{
    ಆದರೆ (gui_mch_showing_tabline())
    {
	ಬಿಂದು_2 pt;
	RECT rect;

	/*
	 * If the cursor is on the tabline, display the tab menu
	 */
	GetCursorPos(&pt);
	Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rect);
	ಆದರೆ (pt.y < rect.ತುದಿ)
	{
	    show_tabline_popup_menu();
	    ಹಿಂತಿರುಗಿ;
	}
    }
    FORWARD_WM_RBUTTONUP(hwnd, x, y, ಕೀಲಿ_Flags, Def_ಕಿಟಕಿ_Proc_W);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
On_LButton_ಕೆಳಕ್ಕೆ(HWND hwnd, BOOL fDoubleClick, ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, UINT ಕೀಲಿ_Flags)
{
    /*
     * If the user double clicked the tabline, create a new tab
     */
    ಆದರೆ (gui_mch_showing_tabline())
    {
	ಬಿಂದು_2 pt;
	RECT rect;

	GetCursorPos(&pt);
	Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rect);
	ಆದರೆ (pt.y < rect.ತುದಿ)
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
    }
    FORWARD_WM_LBUTTONDOWN(hwnd, fDoubleClick, x, y, ಕೀಲಿ_Flags, Def_ಕಿಟಕಿ_Proc_W);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ UINT
_OnNCHitTest(HWND hwnd, ಪೂರ್ಣಾಂಕ xPos, ಪೂರ್ಣಾಂಕ yPos)
{
    UINT	ಫಲಿತಾಂಶ;
    ಪೂರ್ಣಾಂಕ		x, y;

    ಫಲಿತಾಂಶ = FORWARD_WM_NCHITTEST(hwnd, xPos, yPos, Def_ಕಿಟಕಿ_Proc_W);
    ಆದರೆ (ಫಲಿತಾಂಶ != HTCLIENT)
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಆದರೆ (gui_mch_showing_tabline())
    {
	RECT rct;

	// If the cursor is on the GUI tabline, don't process this event
	Get_ಕಿಟಕಿ_Rect(s_ಪಠ್ಯ_Area, &rct);
	ಆದರೆ (yPos < rct.ತುದಿ)
	    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
#ಆದರೆ_ಅಂತ್ಯ
    (ಶೂನ್ಯ)gui_mch_get_winpos(&x, &y);
    xPos -= x;

    ಆದರೆ (xPos < 48) // <VN> TODO should use system metric?
	ಹಿಂತಿರುಗಿ HTBOTTOMLEFT;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ HTBOTTOMRIGHT;
}

#ಆದರೆ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ LRESULT
_OnNotify(HWND hwnd, UINT id, NMHDR *hdr)
{
    ಆಯ್ಕೆ (hdr->ಸಂಕೇತ_2)
    {
	ಸ್ಥಿತಿ TTN_GETDISPINFOW:
	ಸ್ಥಿತಿ TTN_GETDISPINFO:
	    {
		char_u		*str = ಖಾಲಿ;
		ಸ್ಥಿರ ಶೂನ್ಯ	*tt_ಪಠ್ಯ = ಖಾಲಿ;

		VIM_CLEAR(tt_ಪಠ್ಯ);

# ifdef FEAT_GUI_TABLINE
		ಆದರೆ (gui_mch_showing_tabline()
			&& hdr->hwndFrom == TabCtrl_GetToolTips(s_tabhwnd))
		{
		    ಬಿಂದು_2	pt;
		    /*
		     * Mouse is over the GUI tabline. Display the
		     * tooltip for the tab under the cursor
		     *
		     * Get the cursor position within the tab control
		     */
		    GetCursorPos(&pt);
		    ಆದರೆ (Screen_ಗಮ್ಯ_Client(s_tabhwnd, &pt) != 0)
		    {
			TCHITTESTINFO htinfo;
			ಪೂರ್ಣಾಂಕ idx;

			/*
			 * Get the tab under the cursor
			 */
			htinfo.pt.x = pt.x;
			htinfo.pt.y = pt.y;
			idx = Tab_Ctrl_ದೊರಕಿತು_Test(s_tabhwnd, &htinfo);
			ಆದರೆ (idx != -1)
			{
			    tabpage_T *tp;

			    tp = ಕಂಡುಹಿಡಿ_tabpage(idx + 1);
			    ಆದರೆ (tp != ಖಾಲಿ)
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
		ಇಲ್ಲದಿದ್ದರೆ
#  endif
		{
		    UINT	idButton;
		    vimmenu_T	*pMenu;

		    idButton = (UINT) hdr->idFrom;
		    pMenu = gui_mswin_ಕಂಡುಹಿಡಿ_menu(ಬೇರು_menu, idButton);
		    ಆದರೆ (pMenu)
			str = pMenu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP];
		}
# endif
		ಆದರೆ (str == ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;

		// Set the maximum width, this also enables using \n for
		// line break.
		Send_ಸಂದೇಶ(hdr->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 500);

		ಆದರೆ (hdr->ಸಂಕೇತ_2 == TTN_GETDISPINFOW)
		{
		    LPNMTTDISPINFOW	lpdi = (LPNMTTDISPINFOW)hdr;

		    tt_ಪಠ್ಯ = enc_ಗಮ್ಯ_utf16(str, ಖಾಲಿ);
		    lpdi->lpsz_ಪಠ್ಯ = tt_ಪಠ್ಯ;
		    // can't show tooltip if failed
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    LPNMTTDISPINFO	lpdi = (LPNMTTDISPINFO)hdr;
		    size_t		len = STRLEN(str);

		    ಆದರೆ (len < ಗಾತ್ರ(lpdi->sz_ಪಠ್ಯ)
			    || ((tt_ಪಠ್ಯ = vim_strnsave(str, len)) == ಖಾಲಿ))
			vim_strncpy((char_u *)lpdi->sz_ಪಠ್ಯ, str,
				ಗಾತ್ರ(lpdi->sz_ಪಠ್ಯ) - 1);
		    ಇಲ್ಲದಿದ್ದರೆ
			lpdi->lpsz_ಪಠ್ಯ = tt_ಪಠ್ಯ;
		}
	    }
	    ನಿಲ್ಲಿಸು;

# ifdef FEAT_GUI_TABLINE
	ಸ್ಥಿತಿ TCN_SELCHANGE:
	    ಆದರೆ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		send_tabline_event(TabCtrl_GetCurSel(s_tabhwnd) + 1);
		ಹಿಂತಿರುಗಿ 0L;
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ NM_RCLICK:
	    ಆದರೆ (gui_mch_showing_tabline() && (hdr->hwndFrom == s_tabhwnd))
	    {
		show_tabline_popup_menu();
		ಹಿಂತಿರುಗಿ 0L;
	    }
	    ನಿಲ್ಲಿಸು;
# endif

	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, WM_NOTIFY, (WPARAM)id, (LPARAM)hdr);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MENUHINTS) && defined(FEAT_MENU)
    ಸ್ಥಿರ LRESULT
_OnMenuSelect(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
    ಆದರೆ (((UINT) HIWORD(wParam)
		& (0xffff ^ (MF_MOUSESELECT + MF_BITMAP + MF_POPUP)))
	    == MF_HILITE
	    && (State & ಬಹುಲಕ_CMDLINE) == 0)
    {
	vimmenu_T   *pMenu;
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  did_menu_tip = FALSE;

	ಆದರೆ (did_menu_tip)
	{
	    ಸಂದೇಶ_clr_cmdline();
	    setcursor();
	    ಹೊರಹೋಗು_flush();
	    did_menu_tip = FALSE;
	}

	pMenu = gui_mswin_ಕಂಡುಹಿಡಿ_menu(ಬೇರು_menu, (UINT)LOWORD(wParam));
	ಆದರೆ (pMenu != ಖಾಲಿ && pMenu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP] != ಖಾಲಿ)
	{
	    MENUITEMINFO menuinfo;

	    menuinfo.cb_ಗಾತ್ರ = ಗಾತ್ರ(MENUITEMINFO);
	    menuinfo.f_ಮುಖವಾಡ = MIIM_ID;		    // We only want to check if the menu item exists,
						    // so retrieve something simple.
	    ಆದರೆ (Get_Menu_ಅಂಶ_Info(s_menuBar, pMenu->id, FALSE, &menuinfo))
	    {
		++ಸಂದೇಶ_hist_off;
		ಸಂದೇಶ((ಅಕ್ಷರ *)pMenu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP]);
		--ಸಂದೇಶ_hist_off;
		setcursor();
		ಹೊರಹೋಗು_flush();
		did_menu_tip = TRUE;
	    }
	}
	ಹಿಂತಿರುಗಿ 0L;
    }
    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, WM_MENUSELECT, wParam, lParam);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ BOOL
On_Get_Dpi_Scaled_ಗಾತ್ರ(HWND hwnd ಬಳಸದ, UINT dpi, ಗಾತ್ರ_4 *ಗಾತ್ರ_2)
{
    ಪೂರ್ಣಾಂಕ		old_ಅಗಲ, old_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		new_ಅಗಲ, new_ಎತ್ತರ;
    LOGFONTW	lf;
    HFONT	font;

    //TRACE("DPI: %d, SIZE=(%d,%d), s_dpi: %d", dpi, size->cx, size->cy, s_dpi);

    // Calculate new approximate size.
    Get_Font_ಗಾತ್ರ(gui.norm_font, &old_ಅಗಲ, &old_ಎತ್ತರ);    // Current size
    GetObjectW((HFONT)gui.norm_font, ಗಾತ್ರ(lf), &lf);
    lf.lf_ಎತ್ತರ = lf.lf_ಎತ್ತರ * (ಪೂರ್ಣಾಂಕ)dpi / s_dpi;
    font = Create_Font_ಪರೋಕ್ಷ_W(&lf);
    ಆದರೆ (font)
    {
	Get_Font_ಗಾತ್ರ((GuiFont)font, &new_ಅಗಲ, &new_ಎತ್ತರ);	// New size
	DeleteFont(font);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	new_ಅಗಲ = old_ಅಗಲ;
	new_ಎತ್ತರ = old_ಎತ್ತರ;
    }
    ಗಾತ್ರ_2->cx = ಗಾತ್ರ_2->cx * new_ಅಗಲ / old_ಅಗಲ;
    ಗಾತ್ರ_2->cy = ಗಾತ್ರ_2->cy * new_ಎತ್ತರ / old_ಎತ್ತರ;
    //TRACE("New approx. SIZE=(%d,%d)", size->cx, size->cy);

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ LRESULT
On_Dpi_ಬದಲಾಯಿತು(HWND hwnd, UINT xdpi ಬಳಸದ, UINT ydpi, RECT *rc)
{
    s_dpi = ydpi;
    s_ಒಳ_dpichanged = TRUE;
    //TRACE("DPI: %d", ydpi);

    s_suggested_rect = *rc;
    //TRACE("Suggested pos&size: %d,%d %d,%d", rc->left, rc->top,
    //		rc->right - rc->left, rc->bottom - rc->top);

    update_scrollbar_ಗಾತ್ರ();
    update_toolbar_ಗಾತ್ರ();
    ಗಣ_tabline_font();

    gui_init_font(*p_guifont == NUL ? hl_get_font_name() : p_guifont, FALSE);
    gui_get_wide_font();
    gui_mswin_get_menu_ಎತ್ತರ(FALSE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    im_ಗಣ_position(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು);
#ಆದರೆ_ಅಂತ್ಯ
    InvalidateRect(hwnd, ಖಾಲಿ, TRUE);

    s_ಒಳ_dpichanged = FALSE;
    ಹಿಂತಿರುಗಿ 0L;
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

    ಆಯ್ಕೆ (u_ಸಂದೇಶ)
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	HANDLE_ಸಂದೇಶ(hwnd, WM_COMMAND,	_OnMenu);
#ಆದರೆ_ಅಂತ್ಯ
	// HANDLE_MSG(hwnd, WM_MOVE,	    _OnMove);
	// HANDLE_MSG(hwnd, WM_NCACTIVATE,  _OnNCActivate);
	HANDLE_ಸಂದೇಶ(hwnd, WM_SETFOCUS,	On_ಗಣ_Focus);
	HANDLE_ಸಂದೇಶ(hwnd, WM_ಗಾತ್ರ,	On_ಗಾತ್ರ);
	// HANDLE_MSG(hwnd, WM_SYSCOMMAND,  _OnSysCommand);
	// HANDLE_MSG(hwnd, WM_SYSKEYDOWN,  _OnAltKey);
	HANDLE_ಸಂದೇಶ(hwnd, WM_VSCROLL,	_OnScroll);
	// HANDLE_MSG(hwnd, WM_WINDOWPOSCHANGING,	_OnWindowPosChanging);
	HANDLE_ಸಂದೇಶ(hwnd, WM_ACTIVATEAPP, _OnActivateApp);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	HANDLE_ಸಂದೇಶ(hwnd, WM_WINDOWPOSCHANGED, On_ಕಿಟಕಿ_Pos_ಬದಲಾಯಿತು);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	HANDLE_ಸಂದೇಶ(hwnd, WM_RBUTTONUP,	On_RButton_ಮೇಲೆ);
	HANDLE_ಸಂದೇಶ(hwnd, WM_LBUTTONDBLCLK,  On_LButton_ಕೆಳಕ್ಕೆ);
#ಆದರೆ_ಅಂತ್ಯ
	HANDLE_ಸಂದೇಶ(hwnd, WM_NCHITTEST,	_OnNCHitTest);

    ಸ್ಥಿತಿ WM_QUERYENDSESSION:	// System wants to go down.
	gui_shell_closed();	// Will exit when no changed buffers.
	ಹಿಂತಿರುಗಿ FALSE;		// Do NOT allow system to go down.

    ಸ್ಥಿತಿ WM_ENDSESSION:
	ಆದರೆ (wParam)	// system only really goes down when wParam is TRUE
	{
	    On_ಕೊನೆ_Session();
	    ಹಿಂತಿರುಗಿ 0L;
	}
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WM_CHAR:
	// Don't use HANDLE_MSG() for WM_CHAR, it truncates wParam to a single
	// byte while we want the UTF-16 character value.
	_OnChar(hwnd, (UINT)wParam, (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)LOWORD(lParam));
	ಹಿಂತಿರುಗಿ 0L;

    ಸ್ಥಿತಿ WM_SYSCHAR:
	/*
	 * if 'winaltkeys' is "no", or it's "menu" and it's not a menu
	 * shortcut key, handle like a typed ALT key, otherwise call Windows
	 * ALT key handling.
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಆದರೆ (	!gui.menu_is_ಸಕ್ರಿಯ
		|| p_wak[0] == 'n'
		|| (p_wak[0] == 'm' && !gui_is_menu_shortcut((ಪೂರ್ಣಾಂಕ)wParam))
		)
#ಆದರೆ_ಅಂತ್ಯ
	{
	    _OnSysChar(hwnd, (UINT)wParam, (ಪೂರ್ಣಾಂಕ)(ಗಿಡ್ಡ)LOWORD(lParam));
	    ಹಿಂತಿರುಗಿ 0L;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ WM_SYSKEYUP:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	// This used to be done only when menu is active: ALT key is used for
	// that.  But that caused problems when menu is disabled and using
	// Alt-Tab-Esc: get into a strange state where no mouse-moved events
	// are received, mouse pointer remains hidden.
	ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ 0L;
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ WM_EXITSIZEMOVE:
	destroy_sizing_tip();
	ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WM_SIZING:	// HANDLE_MSG doesn't seem to handle this one
	ಹಿಂತಿರುಗಿ _DuringSizing((UINT)wParam, (LPRECT)lParam);

    ಸ್ಥಿತಿ WM_MOUSEWHEEL:
    ಸ್ಥಿತಿ WM_MOUSEHWHEEL:
	_OnMouseWheel(hwnd, wParam, lParam, u_ಸಂದೇಶ == WM_MOUSEHWHEEL);
	ಹಿಂತಿರುಗಿ 0L;

	// Notification for change in SystemParametersInfo()
    ಸ್ಥಿತಿ WM_SETTINGCHANGE:
	ಹಿಂತಿರುಗಿ _OnSettingChange((UINT)wParam);

#ಆದರೆ defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿತಿ WM_NOTIFY:
	ಹಿಂತಿರುಗಿ _OnNotify(hwnd, (UINT)wParam, (NMHDR*)lParam);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MENUHINTS) && defined(FEAT_MENU)
    ಸ್ಥಿತಿ WM_MENUSELECT:
	ಹಿಂತಿರುಗಿ _OnMenuSelect(hwnd, wParam, lParam);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    ಸ್ಥಿತಿ WM_IME_NOTIFY:
	ಆದರೆ (!_OnImeNotify(hwnd, (DWORD)wParam, (DWORD)lParam))
	    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
	ಹಿಂತಿರುಗಿ 1L;

    ಸ್ಥಿತಿ WM_IME_COMPOSITION:
	ಆದರೆ (!_OnImeComposition(hwnd, wParam, lParam))
	    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
	ಹಿಂತಿರುಗಿ 1L;
#ಆದರೆ_ಅಂತ್ಯ
    ಸ್ಥಿತಿ WM_GETDPISCALEDSIZE:
	ಹಿಂತಿರುಗಿ On_Get_Dpi_Scaled_ಗಾತ್ರ(hwnd, (UINT)wParam, (ಗಾತ್ರ_4 *)lParam);
    ಸ್ಥಿತಿ WM_DPICHANGED:
	ಹಿಂತಿರುಗಿ On_Dpi_ಬದಲಾಯಿತು(hwnd, (UINT)LOWORD(wParam), (UINT)HIWORD(wParam),
		(RECT*)lParam);

    ಪೂರ್ವನಿಯೋಜಿತ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
	ಆದರೆ (u_ಸಂದೇಶ == s_findrep_ಸಂದೇಶ && s_findrep_ಸಂದೇಶ != 0)
	    On_ಕಂಡುಹಿಡಿ_Repl();
#ಆದರೆ_ಅಂತ್ಯ
	ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ Def_ಕಿಟಕಿ_Proc_W(hwnd, u_ಸಂದೇಶ, wParam, lParam);
}

/*
 * End of call-back routines
 */

// parent window, if specified with -P
HWND vim_ಪೋಷಕ_hwnd = ಖಾಲಿ;

    ಸ್ಥಿರ BOOL CALLBACK
ಕಂಡುಹಿಡಿ_ಕಿಟಕಿ_Title(HWND hwnd, LPARAM lParam)
{
    ಅಕ್ಷರ	buf[2048];
    ಅಕ್ಷರ	*title = (ಅಕ್ಷರ *)lParam;

    ಆದರೆ (Get_ಕಿಟಕಿ_ಪಠ್ಯ(hwnd, buf, ಗಾತ್ರ(buf)))
    {
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(buf, title) != ಖಾಲಿ)
	{
	    // Found it.  Store the window ref. and quit searching if MDI
	    // works.
	    vim_ಪೋಷಕ_hwnd = ಕಂಡುಹಿಡಿ_ಕಿಟಕಿ_Ex(hwnd, ಖಾಲಿ, "MDIClient", ಖಾಲಿ);
	    ಆದರೆ (vim_ಪೋಷಕ_hwnd != ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FALSE;
	}
    }
    ಹಿಂತಿರುಗಿ TRUE;	// continue searching
}

/*
 * Invoked for '-P "title"' argument: search for parent application to open
 * our window in.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪೋಷಕ(ಅಕ್ಷರ *title)
{
    EnumWindows(ಕಂಡುಹಿಡಿ_ಕಿಟಕಿ_Title, (LPARAM)title);
    ಆದರೆ (vim_ಪೋಷಕ_hwnd == ಖಾಲಿ)
    {
	semsg(_(e_cannot_ಕಂಡುಹಿಡಿ_ಕಿಟಕಿ_title_str), title);
	mch_exit(2);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_OLE
    ಸ್ಥಿರ ಶೂನ್ಯ
ole_error(ಅಕ್ಷರ *arg)
{
    ಅಕ್ಷರ buf[IOSIZE];

# ifdef VIMDLL
    gui.ಒಳ_use = mch_is_gui_executable();
# endif

    // Can't use emsg() here, we have not finished initialisation yet.
    vim_snprintf(buf, IOSIZE,
			 _(e_ವಾದ_not_supported_str_use_ole_ಆವೃತ್ತಿ), arg);
    mch_errmsg(buf);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(GUI_MAY_SPAWN)
    ಸ್ಥಿರ ಅಕ್ಷರ *
gvim_error(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ *ಸಂದೇಶ = _(e_gui_cannot_be_ಬಳಸಿದ_cannot_execute_gvim_exe);

    ಆದರೆ (starting)
    {
	mch_errmsg(ಸಂದೇಶ);
	mch_errmsg("\n");
	mch_exit(2);
    }
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ;
}

    ಅಕ್ಷರ *
gui_mch_do_spawn(char_u *arg)
{
    ಪೂರ್ಣಾಂಕ			len;
# ಆದರೆ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    char_u		*session = ಖಾಲಿ;
    LPWSTR		tofree1 = ಖಾಲಿ;
# endif
    WCHAR		name[MAX_ಮಾರ್ಗ];
    LPWSTR		cmd, newcmd = ಖಾಲಿ, p, warg, tofree2 = ಖಾಲಿ;
    STARTUPINFOW	si = {ಗಾತ್ರ(si)};
    PROCESS_INFORMATION pi;

    ಆದರೆ (!GetModuleFileNameW(g_hinst, name, MAX_ಮಾರ್ಗ))
	ಹೋಗು error;
    p = wcsrchr(name, L'\\');
    ಆದರೆ (p == ಖಾಲಿ)
	ಹೋಗು error;
    // Replace the executable name from vim(d).exe to gvim(d).exe.
# ifdef DEBUG
    wcscpy(p + 1, L"gvimd.exe");
# ಇಲ್ಲದಿದ್ದರೆ
    wcscpy(p + 1, L"gvim.exe");
# endif

# ಆದರೆ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಆದರೆ (starting)
# endif
    {
	// Pass the command line to the new process.
	p = Get_Command_ಸಾಲು_W();
	// Skip 1st argument.
	ಆಗ (*p && *p != L' ' && *p != L'\t')
	{
	    ಆದರೆ (*p == L'"')
	    {
		// Skip quoted strings
		ಆಗ (*++p && *p != L'"');
	    }

	    ++p;
	}
	cmd = p;
    }
# ಆದರೆ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Create a session file and pass it to the new process.
	LPWSTR	wsession;
	char_u	*savebg;
	ಪೂರ್ಣಾಂಕ	ret;

	session = vim_tempname('s', FALSE);
	ಆದರೆ (session == ಖಾಲಿ)
	    ಹೋಗು error;
	savebg = p_bg;
	p_bg = vim_strnsave((char_u *)"light", 5);	// Set 'bg' to "light".
	ಆದರೆ (p_bg == ಖಾಲಿ)
	{
	    p_bg = savebg;
	    ಹೋಗು error;
	}
	ret = write_session_file(session);
	vim_free(p_bg);
	p_bg = savebg;
	ಆದರೆ (!ret)
	    ಹೋಗು error;
	wsession = enc_ಗಮ್ಯ_utf16(session, ಖಾಲಿ);
	ಆದರೆ (wsession == ಖಾಲಿ)
	    ಹೋಗು error;
	len = (ಪೂರ್ಣಾಂಕ)wcslen(wsession) * 2 + 27 + 1;
	cmd = ALLOC_MULT(WCHAR, len);
	ಆದರೆ (cmd == ಖಾಲಿ)
	{
	    vim_free(wsession);
	    ಹೋಗು error;
	}
	tofree1 = cmd;
	_snwprintf(cmd, len, L" -S \"%s\" -c \"call delete('%s')\"",
		wsession, wsession);
	vim_free(wsession);
    }
# endif

    // Check additional arguments to the `:gui` command.
    ಆದರೆ (arg != ಖಾಲಿ)
    {
	warg = enc_ಗಮ್ಯ_utf16(arg, ಖಾಲಿ);
	ಆದರೆ (warg == ಖಾಲಿ)
	    ಹೋಗು error;
	tofree2 = warg;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	warg = L"";

    // Set up the new command line.
    len = (ಪೂರ್ಣಾಂಕ)wcslen(name) + (ಪೂರ್ಣಾಂಕ)wcslen(cmd) + (ಪೂರ್ಣಾಂಕ)wcslen(warg) + 4;
    newcmd = ALLOC_MULT(WCHAR, len);
    ಆದರೆ (newcmd == ಖಾಲಿ)
	ಹೋಗು error;
    _snwprintf(newcmd, len, L"\"%s\"%s %s", name, cmd, warg);

    // Spawn a new GUI process.
    ಆದರೆ (!CreateProcessW(ಖಾಲಿ, newcmd, ಖಾಲಿ, ಖಾಲಿ, TRUE, 0,
		ಖಾಲಿ, ಖಾಲಿ, &si, &pi))
	ಹೋಗು error;
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    mch_exit(0);

error:
# ಆದರೆ defined(FEAT_SESSION) && defined(EXPERIMENTAL_GUI_CMD)
    ಆದರೆ (session)
	mch_remove(session);
    vim_free(session);
    vim_free(tofree1);
# endif
    vim_free(newcmd);
    vim_free(tofree2);
    ಹಿಂತಿರುಗಿ gvim_error();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when Vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    ಪೂರ್ಣಾಂಕ		silent = FALSE;
    ಪೂರ್ಣಾಂಕ		idx;

    // Check for special OLE command line parameters
    ಆದರೆ ((*argc == 2 || *argc == 3) && (argv[1][0] == '-' || argv[1][0] == '/'))
    {
	// Check for a "-silent" argument first.
	ಆದರೆ (*argc == 3 && STRICMP(argv[1] + 1, "silent") == 0
		&& (argv[2][0] == '-' || argv[2][0] == '/'))
	{
	    silent = TRUE;
	    idx = 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    idx = 1;

	// Register Vim as an OLE Automation server
	ಆದರೆ (STRICMP(argv[idx] + 1, "register") == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
	    ರಿಜಿಸ್ಟರ್_Me(silent);
	    mch_exit(0);
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (!silent)
		ole_error("register");
	    mch_exit(2);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Unregister Vim as an OLE Automation server
	ಆದರೆ (STRICMP(argv[idx] + 1, "unregister") == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
	    UnregisterMe(!silent);
	    mch_exit(0);
#ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (!silent)
		ole_error("unregister");
	    mch_exit(2);
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Ignore an -embedding argument. It is only relevant if the
	// application wants to treat the case when it is started manually
	// differently from the case where it is started via automation (and
	// we don't).
	ಆದರೆ (STRICMP(argv[idx] + 1, "embedding") == 0)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
	    *argc = 1;
#ಇಲ್ಲದಿದ್ದರೆ
	    ole_error("embedding");
	    mch_exit(2);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_OLE
# ifdef VIMDLL
    ಆದರೆ (mch_is_gui_executable())
# endif
    {
	ಪೂರ್ಣಾಂಕ	bDoRestart = FALSE;

	InitOLE(&bDoRestart);
	// automatically exit after registering
	ಆದರೆ (bDoRestart)
	    mch_exit(0);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    {
	// stolen from gui_x11.c
	ಪೂರ್ಣಾಂಕ arg;

	ಗಾಗಿ (arg = 1; arg < *argc; arg++)
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("-nb", argv[arg], 3) == 0)
	    {
		netbeansArg = argv[arg];
		mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ಗಾತ್ರ(ಅಕ್ಷರ *));
		argv[*argc] = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;	// enough?
	    }
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ತುಂಬು_dpi_func(ಶೂನ್ಯ)
{
    HMODULE	hUser32;

    hUser32 = GetModuleHandle("user32.dll");
    ಆದರೆ (hUser32 == ಖಾಲಿ)
	ಹೋಗು fail;

    pGetDpiForSystem = (UINT (WINAPI *)(ಶೂನ್ಯ))Get_Proc_ವಿಳಾಸ(hUser32, "GetDpiForSystem");
    p_Get_Dpi_For_ಕಿಟಕಿ = (UINT (WINAPI *)(HWND))Get_Proc_ವಿಳಾಸ(hUser32, "GetDpiForWindow");
    pGetSystemMetricsForDpi = (ಪೂರ್ಣಾಂಕ (WINAPI *)(ಪೂರ್ಣಾಂಕ, UINT))Get_Proc_ವಿಳಾಸ(hUser32, "GetSystemMetricsForDpi");
    //pGetWindowDpiAwarenessContext = (void*)GetProcAddress(hUser32, "GetWindowDpiAwarenessContext");
    p_ಗಣ_Thread_Dpi_Awareness_Context = (DPI_AWARENESS_CONTEXT (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_ವಿಳಾಸ(hUser32, "SetThreadDpiAwarenessContext");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = (DPI_AWARENESS (WINAPI *)(DPI_AWARENESS_CONTEXT))Get_Proc_ವಿಳಾಸ(hUser32, "GetAwarenessFromDpiAwarenessContext");
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (p_ಗಣ_Thread_Dpi_Awareness_Context != ಖಾಲಿ)
    {
	DPI_AWARENESS_CONTEXT oldctx = p_ಗಣ_Thread_Dpi_Awareness_Context(
		DPI_AWARENESS_CONTEXT_ಪ್ರತಿ_MONITOR_AWARE_V2);
	ಆದರೆ (oldctx != ಖಾಲಿ)
	{
	    TRACE("DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 enabled");
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
	    s_process_dpi_aware = pGetAwarenessFromDpiAwarenessContext(oldctx);
# ifdef DEBUG
	    ಆದರೆ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	    {
		TRACE("WARNING: PerMonitorV2 is not enabled in the process level for some reasons. IME window may not shown correctly.");
	    }
# endif
#ಆದರೆ_ಅಂತ್ಯ
	    ಹಿಂತಿರುಗಿ;
	}
    }

fail:
    // Disable PerMonitorV2 APIs.
    pGetDpiForSystem = vimGetDpiForSystem;
    p_Get_Dpi_For_ಕಿಟಕಿ = ಖಾಲಿ;
    pGetSystemMetricsForDpi = stubGetSystemMetricsForDpi;
    p_ಗಣ_Thread_Dpi_Awareness_Context = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
    pGetAwarenessFromDpiAwarenessContext = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Initialise the GUI.	Create all the windows, set up all the call-backs
 * etc.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init(ಶೂನ್ಯ)
{
    ಸ್ಥಿರಾಂಕ WCHAR szVimWndClassW[] = VIM_CLASSW;
    ಸ್ಥಿರಾಂಕ WCHAR sz_ಪಠ್ಯ_Area_Class_W[] = L"VimTextArea";
    WNDCLASSW wndclassw;

    // Return here if the window was already opened (happens when
    // gui_mch_dialog() is called early).
    ಆದರೆ (s_hwnd != ಖಾಲಿ)
	ಹೋಗು theend;

    /*
     * Load the tearoff bitmap
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TEAROFF
    s_htearbitmap = ತುಂಬು_Bitmap(g_hinst, "IDB_TEAROFF");
#ಆದರೆ_ಅಂತ್ಯ

    ತುಂಬು_dpi_func();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DARKTHEME
    dyn_uxtheme_ತುಂಬು();
#ಆದರೆ_ಅಂತ್ಯ

    dyn_dwm_ತುಂಬು();

    s_dpi = pGetDpiForSystem();
    update_scrollbar_ಗಾತ್ರ();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui.menu_ಎತ್ತರ = 0;	// Windows takes care of this
#ಆದರೆ_ಅಂತ್ಯ
    gui.border_ಅಗಲ = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    gui.toolbar_ಎತ್ತರ = TOOLBAR_BUTTON_ಎತ್ತರ + TOOLBAR_BORDER_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ

    s_brush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

    // First try using the wide version, so that we can use any title.
    // Otherwise only characters in the active codepage will work.
    ಆದರೆ (GetClassInfoW(g_hinst, szVimWndClassW, &wndclassw) == 0)
    {
	wndclassw.style = CS_DBLCLKS;
	wndclassw.lpfnWndProc = _WndProc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ತುಂಬು_Icon(wndclassw.hInstance, "IDR_VIM");
	wndclassw.hCursor = ತುಂಬು_Cursor(ಖಾಲಿ, IDC_ARROW);
	wndclassw.hbrBackground = s_brush;
	wndclassw.lpszMenuName = ಖಾಲಿ;
	wndclassw.lpszClassName = szVimWndClassW;

	ಆದರೆ (ರಿಜಿಸ್ಟರ್_Class_W(&wndclassw) == 0)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (vim_ಪೋಷಕ_hwnd != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TRY_EXCEPT
	__try
	{
#ಆದರೆ_ಅಂತ್ಯ
	    // Open inside the specified parent window.
	    // TODO: last argument should point to a CLIENTCREATESTRUCT
	    // structure.
	    s_hwnd = Create_ಕಿಟಕಿ_Ex_W(
		WS_EX_MDICHILD,
		szVimWndClassW, L"Vim MSWindows GUI",
		WS_OVERLAPPEDWINDOW | WS_ಮಗು
				 | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | 0xC000,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		vim_ಪೋಷಕ_hwnd, ಖಾಲಿ,
		g_hinst, ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_TRY_EXCEPT
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	    // NOP
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (s_hwnd == ಖಾಲಿ)
	{
	    emsg(_(e_unable_ಗಮ್ಯ_open_ಕಿಟಕಿ_inside_mdi_application));
	    mch_exit(2);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// If the provided windowid is not valid reset it to zero, so that it
	// is ignored and we open our own window.
	ಆದರೆ (Is_ಕಿಟಕಿ((HWND)win_socket_id) <= 0)
	    win_socket_id = 0;

	// Create a window.  If win_socket_id is not zero without border and
	// titlebar, it will be reparented below.
	s_hwnd = Create_ಕಿಟಕಿ_W(
		szVimWndClassW, L"Vim MSWindows GUI",
		(win_socket_id == 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP)
					  | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		gui_win_x == -1 ? CW_USEDEFAULT : gui_win_x,
		gui_win_y == -1 ? CW_USEDEFAULT : gui_win_y,
		100,				// Any value will do
		100,				// Any value will do
		ಖಾಲಿ, ಖಾಲಿ,
		g_hinst, ಖಾಲಿ);
	ಆದರೆ (s_hwnd != ಖಾಲಿ && win_socket_id != 0)
	{
	    ಗಣ_ಪೋಷಕ(s_hwnd, (HWND)win_socket_id);
	    Show_ಕಿಟಕಿ(s_hwnd, SW_SHOWMAXIMIZED);
	}
    }

    ಆದರೆ (s_hwnd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (p_Get_Dpi_For_ಕಿಟಕಿ != ಖಾಲಿ)
    {
	s_dpi = p_Get_Dpi_For_ಕಿಟಕಿ(s_hwnd);
	update_scrollbar_ಗಾತ್ರ();
	//TRACE("System DPI: %d, DPI: %d", pGetDpiForSystem(), s_dpi);
    }

#ಆದರೆ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    dyn_imm_ತುಂಬು();
#ಆದರೆ_ಅಂತ್ಯ

    // Create the text area window
    ಆದರೆ (GetClassInfoW(g_hinst, sz_ಪಠ್ಯ_Area_Class_W, &wndclassw) == 0)
    {
	wndclassw.style = CS_OWNDC;
	wndclassw.lpfnWndProc = ಪಠ್ಯ_Area_Wnd_Proc;
	wndclassw.cbClsExtra = 0;
	wndclassw.cbWndExtra = 0;
	wndclassw.hInstance = g_hinst;
	wndclassw.hIcon = ಖಾಲಿ;
	wndclassw.hCursor = ತುಂಬು_Cursor(ಖಾಲಿ, IDC_ARROW);
	wndclassw.hbrBackground = ಖಾಲಿ;
	wndclassw.lpszMenuName = ಖಾಲಿ;
	wndclassw.lpszClassName = sz_ಪಠ್ಯ_Area_Class_W;

	ಆದರೆ (ರಿಜಿಸ್ಟರ್_Class_W(&wndclassw) == 0)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    s_ಪಠ್ಯ_Area = Create_ಕಿಟಕಿ_Ex_W(
	0,
	sz_ಪಠ್ಯ_Area_Class_W, L"Vim text area",
	WS_ಮಗು | WS_VISIBLE, 0, 0,
	100,				// Any value will do for now
	100,				// Any value will do for now
	s_hwnd, ಖಾಲಿ,
	g_hinst, ಖಾಲಿ);

    ಆದರೆ (s_ಪಠ್ಯ_Area == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_LIBCALL
    // Try loading an icon from $RUNTIMEPATH/bitmaps/vim.ico.
    {
	HANDLE	hIcon = ಖಾಲಿ;

	ಆದರೆ (mch_icon_ತುಂಬು(&hIcon) == OK && hIcon != ಖಾಲಿ)
	    Send_ಸಂದೇಶ(s_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    s_menuBar = CreateMenu();
#ಆದರೆ_ಅಂತ್ಯ
    s_hdc = GetDC(s_ಪಠ್ಯ_Area);

    DragAcceptFiles(s_hwnd, TRUE);

    // Do we need to bother with this?
    // m_fMouseAvail = pGetSystemMetricsForDpi(SM_MOUSEPRESENT, s_dpi);

    // Get background/foreground colors from the system
    gui_mch_def_colors();

    // Get the colors from the "Normal" group (set in syntax.c or in a vimrc
    // file)
    ಗಣ_ಸಾಮಾನ್ಯ_colors();

    /*
     * Check that none of the colors are the same as the background color.
     * Then store the current values as the defaults.
     */
    gui_check_colors();
    gui.def_norm_pixel = gui.norm_pixel;
    gui.def_ಹಿಂಭಾಗ_pixel = gui.ಹಿಂಭಾಗ_pixel;

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_ಸ್ಥಾನಾಂತರ = gui.border_ಅಗಲ;

    /*
     * Set up for Intellimouse processing
     */
    init_mouse_wheel();

    /*
     * compute a couple of metrics used for the dialogs
     */
    get_dialog_font_metrics();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    /*
     * Create the toolbar
     */
    initialise_toolbar();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    /*
     * Create the tabline
     */
    initialise_tabline();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN_ಕಂಡುಹಿಡಿ_REPLACE
    /*
     * Initialise the dialog box stuff
     */
    s_findrep_ಸಂದೇಶ = ರಿಜಿಸ್ಟರ್_ಕಿಟಕಿ_ಸಂದೇಶ(FINDMSGSTRING);

    // Initialise the struct
    s_findrep_struct.l_Struct_ಗಾತ್ರ = ಗಾತ್ರ(s_findrep_struct);
    s_findrep_struct.lpstr_ಕಂಡುಹಿಡಿ_What = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstr_ಕಂಡುಹಿಡಿ_What[0] = NUL;
    s_findrep_struct.lpstrReplaceWith = ALLOC_MULT(WCHAR, MSWIN_FR_BUFSIZE);
    s_findrep_struct.lpstrReplaceWith[0] = NUL;
    s_findrep_struct.w_ಕಂಡುಹಿಡಿ_What_Len = MSWIN_FR_BUFSIZE;
    s_findrep_struct.wReplaceWithLen = MSWIN_FR_BUFSIZE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // set the v:windowid variable
    ಗಣ_vim_var_nr(VV_WINDOWID, Handle_ಗಮ್ಯ_Long(s_hwnd));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RENDER_OPTIONS
    ಆದರೆ (p_rop)
	(ಶೂನ್ಯ)gui_mch_ಗಣ_rendering_options(p_rop);
#ಆದರೆ_ಅಂತ್ಯ

theend:
    // Display any pending error messages
    display_errors();

    ಹಿಂತಿರುಗಿ OK;
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
    mon = Monitor_From_ಕಿಟಕಿ(s_hwnd, MONITOR_DEFAULTTOPRIMARY);
    ಆದರೆ (mon != ಖಾಲಿ)
    {
	moninfo.cb_ಗಾತ್ರ = ಗಾತ್ರ(MONITORINFO);
	ಆದರೆ (GetMonitorInfo(mon, &moninfo))
	{
	    *spi_rect = moninfo.rcWork;
	    ಹಿಂತಿರುಗಿ;
	}
    }
    // this is the old method...
    SystemParametersInfo(SPI_GETWORKAREA, 0, spi_rect, 0);
}

/*
 * Set the size of the window to the given width and height in pixels.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(
	ಪೂರ್ಣಾಂಕ ಅಗಲ,
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
	ಪೂರ್ಣಾಂಕ min_ಅಗಲ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ min_ಎತ್ತರ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ ತಳ_ಅಗಲ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ ತಳ_ಎತ್ತರ ಬಳಸದ,
	ಪೂರ್ಣಾಂಕ direction)
{
    RECT	workarea_rect;
    RECT	ಕಿಟಕಿ_rect;
    ಪೂರ್ಣಾಂಕ		win_ಅಗಲ, win_ಎತ್ತರ;

    // Try to keep window completely on screen.
    // Get position of the screen work area.  This is the part that is not
    // used by the taskbar or appbars.
    get_work_area(&workarea_rect);

    // Resizing a maximized window looks very strange, unzoom it first.
    // But don't do it when still starting up, it may have been requested in
    // the shortcut.
    ಆದರೆ (IsZoomed(s_hwnd) && starting == 0)
	Show_ಕಿಟಕಿ(s_hwnd, SW_SHOWNORMAL);

    ಆದರೆ (s_ಒಳ_dpichanged)
	// Use the suggested position when in WM_DPICHANGED.
	ಕಿಟಕಿ_rect = s_suggested_rect;
    ಇಲ್ಲದಿದ್ದರೆ
	// Use current position.
	Get_ಕಿಟಕಿ_Rect(s_hwnd, &ಕಿಟಕಿ_rect);

    // compute the size of the outside of the window
    win_ಅಗಲ = ಅಗಲ + (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		     pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;
    win_ಎತ್ತರ = ಎತ್ತರ + (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		       pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
			+ pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
			+ gui_mswin_get_menu_ಎತ್ತರ(FALSE);

    // The following should take care of keeping Vim on the same monitor, no
    // matter if the secondary monitor is left or right of the primary
    // monitor.
    ಕಿಟಕಿ_rect.ಬಲ_2 = ಕಿಟಕಿ_rect.ಎಡ_2 + win_ಅಗಲ;
    ಕಿಟಕಿ_rect.bottom = ಕಿಟಕಿ_rect.ತುದಿ + win_ಎತ್ತರ;

    // If the window is going off the screen, move it on to the screen.
    // Don't adjust the position when in WM_DPICHANGED.
    ಆದರೆ (!s_ಒಳ_dpichanged)
    {
	ಆದರೆ ((direction & RESIZE_HOR)
		&& ಕಿಟಕಿ_rect.ಬಲ_2 > workarea_rect.ಬಲ_2)
	    ಸ್ಥಾನಾಂತರ_Rect(&ಕಿಟಕಿ_rect,
		    workarea_rect.ಬಲ_2 - ಕಿಟಕಿ_rect.ಬಲ_2, 0);

	ಆದರೆ ((direction & RESIZE_HOR)
		&& ಕಿಟಕಿ_rect.ಎಡ_2 < workarea_rect.ಎಡ_2)
	    ಸ್ಥಾನಾಂತರ_Rect(&ಕಿಟಕಿ_rect,
		    workarea_rect.ಎಡ_2 - ಕಿಟಕಿ_rect.ಎಡ_2, 0);

	ಆದರೆ ((direction & RESIZE_VERT)
		&& ಕಿಟಕಿ_rect.bottom > workarea_rect.bottom)
	    ಸ್ಥಾನಾಂತರ_Rect(&ಕಿಟಕಿ_rect,
		    0, workarea_rect.bottom - ಕಿಟಕಿ_rect.bottom);

	ಆದರೆ ((direction & RESIZE_VERT)
		&& ಕಿಟಕಿ_rect.ತುದಿ < workarea_rect.ತುದಿ)
	    ಸ್ಥಾನಾಂತರ_Rect(&ಕಿಟಕಿ_rect,
		    0, workarea_rect.ತುದಿ - ಕಿಟಕಿ_rect.ತುದಿ);
    }

    Move_ಕಿಟಕಿ(s_hwnd, ಕಿಟಕಿ_rect.ಎಡ_2, ಕಿಟಕಿ_rect.ತುದಿ,
						win_ಅಗಲ, win_ಎತ್ತರ, TRUE);

    //TRACE("New pos: %d,%d  New size: %d,%d",
    //	    window_rect.left, window_rect.top, win_width, win_height);

    ಗಣ_ಸಕ್ರಿಯ_ಕಿಟಕಿ(s_hwnd);
    ಗಣ_Focus(s_hwnd);

    // Menu may wrap differently now
    gui_mswin_get_menu_ಎತ್ತರ(!gui.starting);
}


    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(
    scrollbar_T *sb,
    ದೀರ್ಘ	val,
    ದೀರ್ಘ	ಗಾತ್ರ_2,
    ದೀರ್ಘ	max)
{
    SCROLLINFO	info;

    sb->scroll_ಸರಿಸು = 0;
    ಆಗ (max > 32767)
    {
	max = (max + 1) >> 1;
	val  >>= 1;
	ಗಾತ್ರ_2 >>= 1;
	++sb->scroll_ಸರಿಸು;
    }

    ಆದರೆ (sb->scroll_ಸರಿಸು > 0)
	++ಗಾತ್ರ_2;

    info.cb_ಗಾತ್ರ = ಗಾತ್ರ(info);
    info.f_ಮುಖವಾಡ = SIF_POS | SIF_RANGE | SIF_ಪುಟ;
    info.nPos = val;
    info.nMin = 0;
    info.nMax = max;
    info.n_ಪುಟ = ಗಾತ್ರ_2;
    ಗಣ_Scroll_Info(sb->id, SB_CTL, &info, TRUE);
}


/*
 * Set the current text font.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_font(GuiFont font)
{
    gui.currFont = font;
}


/*
 * Set the current text foreground color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(guicolor_T color)
{
    gui.currFgColor = color;
}

/*
 * Set the current text background color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(guicolor_T color)
{
    gui.currBgColor = color;
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T color)
{
    gui.currSpColor = color;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MBYTE_IME
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
_OnImeNotify(HWND hWnd, DWORD dwCommand, DWORD dw_ದತ್ತಾಂಶ ಬಳಸದ)
{
    LRESULT l_ಫಲಿತಾಂಶ = 0;
    HIMC hImc;

    ಆದರೆ (!pImmGetContext || (hImc = pImmGetContext(hWnd)) == (HIMC)0)
	ಹಿಂತಿರುಗಿ l_ಫಲಿತಾಂಶ;
    ಆಯ್ಕೆ (dwCommand)
    {
	ಸ್ಥಿತಿ IMN_SETOPENSTATUS:
	    ಆದರೆ (p_Imm_Get_Open_ಸ್ಥಿತಿ(hImc))
	    {
		LOGFONTW lf = norm_logfont;
		ಆದರೆ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
		    // Work around when PerMonitorV2 is not enabled in the process level.
		    lf.lf_ಎತ್ತರ = lf.lf_ಎತ್ತರ * DEFAULT_DPI / s_dpi;
		p_Imm_ಗಣ_Composition_Font_W(hImc, &lf);
		im_ಗಣ_position(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು);

		// Disable langmap
		State &= ~ಬಹುಲಕ_LANGMAP;
		ಆದರೆ (State & ಬಹುಲಕ_ಸೇರಿಸು)
		{
# ಆದರೆ defined(FEAT_KEYMAP)
		    // Unshown 'keymap' in status lines
		    ಆದರೆ (curbuf->b_p_iminsert == B_IMODE_LMAP)
		    {
			// Save cursor position
			ಪೂರ್ಣಾಂಕ old_ಅಡ್ಡಸಾಲು = gui.ಅಡ್ಡಸಾಲು;
			ಪೂರ್ಣಾಂಕ old_ಲಂಬಸಾಲು = gui.ಲಂಬಸಾಲು;

			// This must be called here before
			// status_redraw_curbuf(), otherwise the mode
			// message may appear in the wrong position.
			showmode();
			ಸ್ಥಿತಿ_redraw_curbuf();
			update_screen(0);
			// Restore cursor position
			gui.ಅಡ್ಡಸಾಲು = old_ಅಡ್ಡಸಾಲು;
			gui.ಲಂಬಸಾಲು = old_ಲಂಬಸಾಲು;
		    }
# endif
		}
	    }
	    gui_update_cursor(TRUE, FALSE);
	    gui_mch_flush();
	    l_ಫಲಿತಾಂಶ = 0;
	    ನಿಲ್ಲಿಸು;
    }
    pImmReleaseContext(hWnd, hImc);
    ಹಿಂತಿರುಗಿ l_ಫಲಿತಾಂಶ;
}

    ಸ್ಥಿರ LRESULT
_OnImeComposition(HWND hwnd, WPARAM dbcs ಬಳಸದ, LPARAM param)
{
    char_u	*ret;
    ಪೂರ್ಣಾಂಕ		len;

    ಆದರೆ ((param & GCS_RESULTSTR) == 0) // Composition unfinished.
	ಹಿಂತಿರುಗಿ 0;

    ret = Get_ಫಲಿತಾಂಶ_Str(hwnd, GCS_RESULTSTR, &len);
    ಆದರೆ (ret != ಖಾಲಿ)
    {
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf_csi(ret, len);
	vim_free(ret);
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * void GetResultStr()
 *
 * This handles WM_IME_COMPOSITION with GCS_RESULTSTR flag on.
 * get complete composition string
 */
    ಸ್ಥಿರ char_u *
Get_ಫಲಿತಾಂಶ_Str(HWND hwnd, ಪೂರ್ಣಾಂಕ GCS, ಪೂರ್ಣಾಂಕ *lenp)
{
    HIMC	hIMC;		// Input context handle.
    LONG	ret;
    WCHAR	*buf = ಖಾಲಿ;
    char_u	*convbuf = ಖಾಲಿ;

    ಆದರೆ (!pImmGetContext || (hIMC = pImmGetContext(hwnd)) == (HIMC)0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Get the length of the composition string.
    ret = pImmGetCompositionStringW(hIMC, GCS, ಖಾಲಿ, 0);
    ಆದರೆ (ret <= 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Allocate the requested buffer plus space for the NUL character.
    buf = alloc(ret + ಗಾತ್ರ(WCHAR));
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Reads in the composition string.
    pImmGetCompositionStringW(hIMC, GCS, buf, ret);
    *lenp = ret / ಗಾತ್ರ(WCHAR);

    convbuf = utf16_ಗಮ್ಯ_enc(buf, lenp);
    pImmReleaseContext(hwnd, hIMC);
    vim_free(buf);
    ಹಿಂತಿರುಗಿ convbuf;
}
#ಆದರೆ_ಅಂತ್ಯ

// For global functions we need prototypes.
#ಆದರೆ defined(FEAT_MBYTE_IME)

/*
 * set font to IM.
 */
    ಶೂನ್ಯ
im_ಗಣ_font(LOGFONTW *lf)
{
    HIMC hImc;

    ಆದರೆ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	p_Imm_ಗಣ_Composition_Font_W(hImc, lf);
	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Notify cursor position to IM.
 */
    ಶೂನ್ಯ
im_ಗಣ_position(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
    HIMC hImc;

    ಆದರೆ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	COMPOSITIONFORM	cfs;

	cfs.dwStyle = CFS_ಬಿಂದು;
	cfs.ptCurrentPos.x = FILL_X(ಲಂಬಸಾಲು);
	cfs.ptCurrentPos.y = FILL_Y(ಅಡ್ಡಸಾಲು);
	Map_ಕಿಟಕಿ_Points(s_ಪಠ್ಯ_Area, s_hwnd, &cfs.ptCurrentPos, 1);
	ಆದರೆ (s_process_dpi_aware == DPI_AWARENESS_UNAWARE)
	{
	    // Work around when PerMonitorV2 is not enabled in the process level.
	    cfs.ptCurrentPos.x = cfs.ptCurrentPos.x * DEFAULT_DPI / s_dpi;
	    cfs.ptCurrentPos.y = cfs.ptCurrentPos.y * DEFAULT_DPI / s_dpi;
	}
	p_Imm_ಗಣ_Composition_ಕಿಟಕಿ(hImc, &cfs);

	pImmReleaseContext(s_hwnd, hImc);
    }
}

/*
 * Set IM status on ("active" is TRUE) or off ("active" is FALSE).
 */
    ಶೂನ್ಯ
im_ಗಣ_ಸಕ್ರಿಯ(ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ)
{
    HIMC	hImc;
    ಸ್ಥಿರ HIMC	hImcOld = (HIMC)0;

# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
    {
	mbyte_im_ಗಣ_ಸಕ್ರಿಯ(ಸಕ್ರಿಯ);
	ಹಿಂತಿರುಗಿ;
    }
# endif

    ಆದರೆ (!pImmGetContext)    // if NULL imm32.dll wasn't loaded (yet)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (p_imdisable)
    {
	ಆದರೆ (hImcOld == (HIMC)0)
	{
	    hImcOld = pImmGetContext(s_hwnd);
	    ಆದರೆ (hImcOld)
		pImmAssociateContext(s_hwnd, (HIMC)0);
	}
	ಸಕ್ರಿಯ = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hImcOld != (HIMC)0)
    {
	pImmAssociateContext(s_hwnd, hImcOld);
	hImcOld = (HIMC)0;
    }

    hImc = pImmGetContext(s_hwnd);
    ಆದರೆ (!hImc)
	ಹಿಂತಿರುಗಿ;

    /*
     * for Korean ime
     */
    HKL hKL = GetKeyboardLayout(0);

    ಆದರೆ (LOWORD(hKL) == MAKELANGID(LANG_KOREAN, SUBLANG_KOREAN))
    {
	ಸ್ಥಿರ DWORD dwConversionSaved = 0, dwSentenceSaved = 0;
	ಸ್ಥಿರ BOOL bSaved = FALSE;

	ಆದರೆ (ಸಕ್ರಿಯ)
	{
	    // if we have a saved conversion status, restore it
	    ಆದರೆ (bSaved)
		p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ(hImc, dwConversionSaved,
			dwSentenceSaved);
	    bSaved = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // save conversion status and disable korean
	    ಆದರೆ (p_Imm_Get_Conversion_ಸ್ಥಿತಿ(hImc, &dwConversionSaved,
			&dwSentenceSaved))
	    {
		bSaved = TRUE;
		p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ(hImc,
			dwConversionSaved & ~(IME_CMODE_NATIVE
			    | IME_CMODE_FULLSHAPE),
			dwSentenceSaved);
	    }
	}
    }

    p_Imm_ಗಣ_Open_ಸ್ಥಿತಿ(hImc, ಸಕ್ರಿಯ);
    pImmReleaseContext(s_hwnd, hImc);
}

/*
 * Get IM status.  When IM is on, return not 0.  Else return 0.
 */
    ಪೂರ್ಣಾಂಕ
im_get_ಸ್ಥಿತಿ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = 0;
    HIMC	hImc;

# ifdef VIMDLL
    ಆದರೆ (!gui.ಒಳ_use && !gui.starting)
	ಹಿಂತಿರುಗಿ mbyte_im_get_ಸ್ಥಿತಿ();
# endif

    ಆದರೆ (pImmGetContext && (hImc = pImmGetContext(s_hwnd)) != (HIMC)0)
    {
	ಸ್ಥಿತಿ_2 = p_Imm_Get_Open_ಸ್ಥಿತಿ(hImc) ? 1 : 0;
	pImmReleaseContext(s_hwnd, hImc);
    }
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MBYTE_IME


/*
 * Convert latin9 text "text[len]" to ucs-2 in "unicodebuf".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
latin9_ಗಮ್ಯ_ucs(char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ len, WCHAR *unicodebuf)
{
    ಪೂರ್ಣಾಂಕ		c;

    ಆಗ (--len >= 0)
    {
	c = *ಪಠ್ಯ++;
	ಆಯ್ಕೆ (c)
	{
	    ಸ್ಥಿತಿ 0xa4: c = 0x20ac; ನಿಲ್ಲಿಸು;   // euro
	    ಸ್ಥಿತಿ 0xa6: c = 0x0160; ನಿಲ್ಲಿಸು;   // S hat
	    ಸ್ಥಿತಿ 0xa8: c = 0x0161; ನಿಲ್ಲಿಸು;   // S -hat
	    ಸ್ಥಿತಿ 0xb4: c = 0x017d; ನಿಲ್ಲಿಸು;   // Z hat
	    ಸ್ಥಿತಿ 0xb8: c = 0x017e; ನಿಲ್ಲಿಸು;   // Z -hat
	    ಸ್ಥಿತಿ 0xbc: c = 0x0152; ನಿಲ್ಲಿಸು;   // OE
	    ಸ್ಥಿತಿ 0xbd: c = 0x0153; ನಿಲ್ಲಿಸು;   // oe
	    ಸ್ಥಿತಿ 0xbe: c = 0x0178; ನಿಲ್ಲಿಸು;   // Y
	}
	*unicodebuf++ = c;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
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
Rev_ಹೊರಹೋಗು( HDC hdc,
	ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು,
	ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು,
	UINT foptions,
	CONST RECT *pcliprect,
	LPCTSTR ಪಠ್ಯ,
	UINT len,
	CONST INT *padding)
{
    ಪೂರ್ಣಾಂಕ		ix;

    ಗಾಗಿ (ix = 0; ix < (ಪೂರ್ಣಾಂಕ)len; ++ix)
	Ext_ಪಠ್ಯ_ಹೊರಹೋಗು(hdc, ಲಂಬಸಾಲು + ಪಠ್ಯ_X(ix), ಅಡ್ಡಸಾಲು, foptions,
					pcliprect, ಪಠ್ಯ + ix, 1, padding);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_ಸಾಲು(
    ಪೂರ್ಣಾಂಕ		x1,
    ಪೂರ್ಣಾಂಕ		y1,
    ಪೂರ್ಣಾಂಕ		x2,
    ಪೂರ್ಣಾಂಕ		y2,
    COLORREF	color)
{
#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWrite_Context_Draw_ಸಾಲು(s_dwc, x1, y1, x2, y2, color);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	HPEN	hpen = CreatePen(PS_SOLID, 1, color);
	HPEN	old_pen = SelectObject(s_hdc, hpen);
	Move_ಗಮ್ಯ_Ex(s_hdc, x1, y1, ಖಾಲಿ);
	// Note: LineTo() excludes the last pixel in the line.
	ಸಾಲು_ಗಮ್ಯ(s_hdc, x2, y2);
	DeleteObject(SelectObject(s_hdc, old_pen));
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_pixel(
    ಪೂರ್ಣಾಂಕ		x,
    ಪೂರ್ಣಾಂಕ		y,
    COLORREF	color)
{
#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWrite_Context_ಗಣ_Pixel(s_dwc, x, y, color);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಗಣ_Pixel(s_hdc, x, y, color);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fill_rect(
    ಸ್ಥಿರಾಂಕ RECT	*rcp,
    HBRUSH	hbr,
    COLORREF	color)
{
#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_FillRect(s_dwc, rcp, color);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	HBRUSH	hbr2;

	ಆದರೆ (hbr == ಖಾಲಿ)
	    hbr2 = CreateSolidBrush(color);
	ಇಲ್ಲದಿದ್ದರೆ
	    hbr2 = hbr;
	FillRect(s_hdc, rcp, hbr2);
	ಆದರೆ (hbr == ಖಾಲಿ)
	    DeleteBrush(hbr2);
    }
}

    ಶೂನ್ಯ
gui_mch_draw_string(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    char_u	*ಪಠ್ಯ,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	*padding = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	pad_ಗಾತ್ರ = 0;
    ಸ್ಥಿರಾಂಕ RECT	*pcliprect = ಖಾಲಿ;
    UINT	foptions = 0;
    ಸ್ಥಿರ WCHAR *unicodebuf = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ   *unicodepdy = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	unibuflen = 0;
    ಪೂರ್ಣಾಂಕ		n = 0;
    ಪೂರ್ಣಾಂಕ		y;

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
    ಸ್ಥಿರ HBRUSH	hbr_cache[2] = {ಖಾಲಿ, ಖಾಲಿ};
    ಸ್ಥಿರ guicolor_T	brush_color[2] = {INVALCOLOR, INVALCOLOR};
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		brush_lru = 0;
    HBRUSH		hbr;
    RECT		rc;

    ಆದರೆ (!(flags & DRAW_TRANSP))
    {
	/*
	 * Clear background first.
	 * Note: FillRect() excludes right and bottom of rectangle.
	 */
	rc.ಎಡ_2 = FILL_X(ಲಂಬಸಾಲು);
	rc.ತುದಿ = FILL_Y(ಅಡ್ಡಸಾಲು);
	ಆದರೆ (has_mbyte)
	{
	    // Compute the length in display cells.
	    rc.ಬಲ_2 = FILL_X(ಲಂಬಸಾಲು + mb_string2cells(ಪಠ್ಯ, len));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    rc.ಬಲ_2 = FILL_X(ಲಂಬಸಾಲು + len);
	rc.bottom = FILL_Y(ಅಡ್ಡಸಾಲು + 1);

	// Cache the created brush, that saves a lot of time.  We need two:
	// one for cursor background and one for the normal background.
	ಆದರೆ (gui.currBgColor == brush_color[0])
	{
	    hbr = hbr_cache[0];
	    brush_lru = 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gui.currBgColor == brush_color[1])
	{
	    hbr = hbr_cache[1];
	    brush_lru = 0;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (hbr_cache[brush_lru] != ಖಾಲಿ)
		DeleteBrush(hbr_cache[brush_lru]);
	    hbr_cache[brush_lru] = CreateSolidBrush(gui.currBgColor);
	    brush_color[brush_lru] = gui.currBgColor;
	    hbr = hbr_cache[brush_lru];
	    brush_lru = !brush_lru;
	}

	fill_rect(&rc, hbr, gui.currBgColor);

	ಗಣ_Bk_ಬಹುಲಕ(s_hdc, TRANSPARENT);

	/*
	 * When drawing block cursor, prevent inverted character spilling
	 * over character cell (can happen with bold/italic)
	 */
	ಆದರೆ (flags & DRAW_CURSOR)
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
	// DirectWrite anti-aliasing can extend glyph pixels beyond cell
	// boundaries, leaving artifacts when adjacent cells are not
	// redrawn.  Clip to the cell rect to prevent this.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (IS_ENABLE_DIRECTX())
	{
	    pcliprect = &rc;
	    foptions = ETO_CLIPPED;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಗಣ_ಪಠ್ಯ_Color(s_hdc, gui.currFgColor);
    SelectFont(s_hdc, gui.currFont);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIRECTX
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWrite_Context_ಗಣ_Font(s_dwc, (HFONT)gui.currFont);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (pad_ಗಾತ್ರ != Columns || padding == ಖಾಲಿ || padding[0] != gui.char_ಅಗಲ)
    {
	ಪೂರ್ಣಾಂಕ	i;

	vim_free(padding);
	pad_ಗಾತ್ರ = Columns;

	// Don't give an out-of-memory message here, it would call us
	// recursively.
	padding = LALLOC_MULT(ಪೂರ್ಣಾಂಕ, pad_ಗಾತ್ರ);
	ಆದರೆ (padding != ಖಾಲಿ)
	    ಗಾಗಿ (i = 0; i < pad_ಗಾತ್ರ; i++)
		padding[i] = gui.char_ಅಗಲ;
    }

    /*
     * We have to provide the padding argument because italic and bold versions
     * of fixed-width fonts are often one pixel or so wider than their normal
     * versions.
     * No check for DRAW_BOLD, Windows will have done it already.
     */

    // Check if there are any UTF-8 characters.  If not, use normal text
    // output to speed up output.
    ಆದರೆ (enc_utf8)
	ಗಾಗಿ (n = 0; n < len; ++n)
	    ಆದರೆ (ಪಠ್ಯ[n] >= 0x80)
		ನಿಲ್ಲಿಸು;

#ಆದರೆ defined(FEAT_DIRECTX)
    // Quick hack to enable DirectWrite.  To use DirectWrite (antialias), it is
    // required that unicode drawing routine, currently.  So this forces it
    // enabled.
    ಆದರೆ (IS_ENABLE_DIRECTX())
	n = 0; // Keep n < len, to enter block for unicode.
#ಆದರೆ_ಅಂತ್ಯ

    // Check if the Unicode buffer exists and is big enough.  Create it
    // with the same length as the multi-byte string, the number of wide
    // characters is always equal or smaller.
    ಆದರೆ ((enc_utf8
		|| (enc_codepage > 0 && (ಪೂರ್ಣಾಂಕ)GetACP() != enc_codepage)
		|| enc_latin9)
	    && (unicodebuf == ಖಾಲಿ || len > unibuflen))
    {
	vim_free(unicodebuf);
	unicodebuf = LALLOC_MULT(WCHAR, len);

	vim_free(unicodepdy);
	unicodepdy = LALLOC_MULT(ಪೂರ್ಣಾಂಕ, len);

	unibuflen = len;
    }

    ಆದರೆ (enc_utf8 && n < len && unicodebuf != ಖಾಲಿ)
    {
	// Output UTF-8 characters.  Composing characters should be
	// handled here.
	ಪೂರ್ಣಾಂಕ		i;
	ಪೂರ್ಣಾಂಕ		wlen;	// string length in words
	ಪೂರ್ಣಾಂಕ		cells;	// cell width of string up to composing char
	ಪೂರ್ಣಾಂಕ		cw;	// width of current cell
	ಪೂರ್ಣಾಂಕ		c;

	wlen = 0;
	cells = 0;
	ಗಾಗಿ (i = 0; i < len; )
	{
	    c = utf_ptr2char(ಪಠ್ಯ + i);
	    ಆದರೆ (c >= 0x10000)
	    {
		// Turn into UTF-16 encoding.
		unicodebuf[wlen++] = ((c - 0x10000) >> 10) + 0xD800;
		unicodebuf[wlen++] = ((c - 0x10000) & 0x3ff) + 0xDC00;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		unicodebuf[wlen++] = c;
	    }

	    ಆದರೆ (utf_iscomposing(c))
		cw = 0;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		cw = utf_char2cells(c);
		ಆದರೆ (cw > 2)		// don't use 4 for unprintable char
		    cw = 1;
	    }

	    ಆದರೆ (unicodepdy != ಖಾಲಿ)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ಆದರೆ (c >= 0x10000)
		{
		    unicodepdy[wlen - 2] = cw * gui.char_ಅಗಲ;
		    unicodepdy[wlen - 1] = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    unicodepdy[wlen - 1] = cw * gui.char_ಅಗಲ;
	    }
	    cells += cw;
	    i += utf_ptr2len_len(ಪಠ್ಯ + i, len - i);
	}
#ಆದರೆ defined(FEAT_DIRECTX)
	ಆದರೆ (IS_ENABLE_DIRECTX())
	{
	    // Add one to "cells" for italics.
	    DWrite_Context_Draw_ಪಠ್ಯ(s_dwc, unicodebuf, wlen,
		    ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
		    FILL_X(cells + 1), FILL_Y(1) - p_linespace,
		    gui.char_ಅಗಲ, gui.currFgColor,
		    foptions, pcliprect, unicodepdy);
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    Ext_ಪಠ್ಯ_ಹೊರಹೋಗು_W(s_hdc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
		    foptions, pcliprect, unicodebuf, wlen, unicodepdy);
	len = cells;	// used for underlining
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((enc_codepage > 0 && (ಪೂರ್ಣಾಂಕ)GetACP() != enc_codepage) || enc_latin9)
    {
	// If we want to display codepage data, and the current CP is not the
	// ANSI one, we need to go via Unicode.
	ಆದರೆ (unicodebuf != ಖಾಲಿ)
	{
	    ಆದರೆ (enc_latin9)
		latin9_ಗಮ್ಯ_ucs(ಪಠ್ಯ, len, unicodebuf);
	    ಇಲ್ಲದಿದ್ದರೆ
		len = Multi_Byte_ಗಮ್ಯ_Wide_Char(enc_codepage,
			MB_PRECOMPOSED,
			(ಅಕ್ಷರ *)ಪಠ್ಯ, len,
			(LPWSTR)unicodebuf, unibuflen);
	    ಆದರೆ (len != 0)
	    {
		// Use unicodepdy to make characters fit as we expect, even
		// when the font uses different widths (e.g., bold character
		// is wider).
		ಆದರೆ (unicodepdy != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ i;
		    ಪೂರ್ಣಾಂಕ cw;

		    ಗಾಗಿ (i = 0; i < len; ++i)
		    {
			cw = utf_char2cells(unicodebuf[i]);
			ಆದರೆ (cw > 2)
			    cw = 1;
			unicodepdy[i] = cw * gui.char_ಅಗಲ;
		    }
		}
		Ext_ಪಠ್ಯ_ಹೊರಹೋಗು_W(s_hdc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
			    foptions, pcliprect, unicodebuf, len, unicodepdy);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	// Windows will mess up RL text, so we have to draw it character by
	// character.  Only do this if RL is on, since it's slow.
	ಆದರೆ (curwin->w_p_rl)
	    Rev_ಹೊರಹೋಗು(s_hdc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
			 foptions, pcliprect, (ಅಕ್ಷರ *)ಪಠ್ಯ, len, padding);
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    Ext_ಪಠ್ಯ_ಹೊರಹೋಗು(s_hdc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
			 foptions, pcliprect, (ಅಕ್ಷರ *)ಪಠ್ಯ, len, padding);
    }

    // Underline
    ಆದರೆ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1;
	ಆದರೆ (p_linespace > 1)
	    y -= p_linespace - 1;
	draw_ಸಾಲು(FILL_X(ಲಂಬಸಾಲು), y, FILL_X(ಲಂಬಸಾಲು + len), y, gui.currFgColor);
    }

    // Strikethrough
    ಆದರೆ (flags & DRAW_STRIKE)
    {
	y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - gui.char_ಎತ್ತರ/2;
	draw_ಸಾಲು(FILL_X(ಲಂಬಸಾಲು), y, FILL_X(ಲಂಬಸಾಲು + len), y, gui.currSpColor);
    }

    // Undercurl
    ಆದರೆ (flags & DRAW_UNDERC)
    {
	ಪೂರ್ಣಾಂಕ			x;
	ಪೂರ್ಣಾಂಕ			ಸ್ಥಾನಾಂತರ;
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

	y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1;
	ಗಾಗಿ (x = FILL_X(ಲಂಬಸಾಲು); x < FILL_X(ಲಂಬಸಾಲು + len); ++x)
	{
	    ಸ್ಥಾನಾಂತರ = val[x % 8];
	    ಗಣ_pixel(x, y - ಸ್ಥಾನಾಂತರ, gui.currSpColor);
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
#ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
#ಆದರೆ_ಅಂತ್ಯ

    GdiFlush();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_rect(RECT *rcp)
{
    fill_rect(rcp, ಖಾಲಿ, gui.ಹಿಂಭಾಗ_pixel);
}


    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಂಕ *screen_w, ಪೂರ್ಣಾಂಕ *screen_h)
{
    RECT	workarea_rect;

    get_work_area(&workarea_rect);

    *screen_w = workarea_rect.ಬಲ_2 - workarea_rect.ಎಡ_2
		- (pGetSystemMetricsForDpi(SM_CXFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2;

    // FIXME: dirty trick: Because the gui_get_base_height() doesn't include
    // the menubar for MSwin, we subtract it from the screen height, so that
    // the window size can be made to fit on the screen.
    *screen_h = workarea_rect.bottom - workarea_rect.ತುದಿ
		- (pGetSystemMetricsForDpi(SM_CYFRAME, s_dpi) +
		   pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, s_dpi)) * 2
		- pGetSystemMetricsForDpi(SM_CYCAPTION, s_dpi)
		- gui_mswin_get_menu_ಎತ್ತರ(FALSE);
}


#ಆದರೆ defined(FEAT_MENU)
/*
 * Add a sub menu to the menu bar.
 */
    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಂಕ		pos)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

    menu->submenu_id = CreatePopupMenu();
    menu->id = s_menu_id++;

    ಆದರೆ (menu_is_menubar(menu->name))
    {
	WCHAR	*wn;
	MENUITEMINFOW	infow;

	wn = enc_ಗಮ್ಯ_utf16(menu->name, ಖಾಲಿ);
	ಆದರೆ (wn == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	infow.cb_ಗಾತ್ರ = ಗಾತ್ರ(infow);
	infow.f_ಮುಖವಾಡ = MIIM_ದತ್ತಾಂಶ | MIIM_TYPE | MIIM_ID
	    | MIIM_SUBMENU;
	infow.dw_ಅಂಶ_ದತ್ತಾಂಶ = (long_u)menu;
	infow.wID = menu->id;
	infow.fType = MFT_STRING;
	infow.dw_Type_ದತ್ತಾಂಶ = wn;
	infow.cch = (UINT)wcslen(wn);
	infow.hSubMenu = menu->submenu_id;
	ಸೇರಿಸು_Menu_ಅಂಶ_W((ಪೋಷಕ == ಖಾಲಿ)
		? s_menuBar : ಪೋಷಕ->submenu_id,
		(UINT)pos, TRUE, &infow);
	vim_free(wn);
    }

    // Fix window size if menu may have wrapped
    ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ)
	gui_mswin_get_menu_ಎತ್ತರ(!gui.starting);
# ifdef FEAT_TEAROFF
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (Is_ಕಿಟಕಿ(ಪೋಷಕ->tearoff_handle))
	rebuild_tearoff(ಪೋಷಕ);
# endif
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ಬಿಂದು_2 mp;

    (ಶೂನ್ಯ)GetCursorPos(&mp);
    gui_mch_show_popupmenu_at(menu, (ಪೂರ್ಣಾಂಕ)mp.x, (ಪೂರ್ಣಾಂಕ)mp.y);
}

    ಶೂನ್ಯ
gui_make_popup(char_u *ಮಾರ್ಗ_name, ಪೂರ್ಣಾಂಕ mouse_pos)
{
    vimmenu_T	*menu = gui_ಕಂಡುಹಿಡಿ_menu(ಮಾರ್ಗ_name);

    ಆದರೆ (menu == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಬಿಂದು_2	p;

    // Find the position of the current cursor
    GetDCOrgEx(s_hdc, &p);
    ಆದರೆ (mouse_pos)
    {
	ಪೂರ್ಣಾಂಕ	mx, my;

	gui_mch_getmouse(&mx, &my);
	p.x += mx;
	p.y += my;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (curwin != ಖಾಲಿ)
    {
	p.x += ಪಠ್ಯ_X(curwin->w_wincol + curwin->w_wcol + 1);
	p.y += ಪಠ್ಯ_Y(W_WINROW(curwin) + curwin->w_wrow + 1);
    }
    ಸಂದೇಶ_scroll = FALSE;
    gui_mch_show_popupmenu_at(menu, (ಪೂರ್ಣಾಂಕ)p.x, (ಪೂರ್ಣಾಂಕ)p.y);
}

# ಆದರೆ defined(FEAT_TEAROFF)
/*
 * Given a menu descriptor, e.g. "File.New", find it in the menu hierarchy and
 * create it as a pseudo-"tearoff menu".
 */
    ಶೂನ್ಯ
gui_make_tearoff(char_u *ಮಾರ್ಗ_name)
{
    vimmenu_T	*menu = gui_ಕಂಡುಹಿಡಿ_menu(ಮಾರ್ಗ_name);

    // Found the menu, so tear it off.
    ಆದರೆ (menu != ಖಾಲಿ)
	gui_mch_tearoff(menu->dname, menu, 0xffffL, 0xffffL);
}
# endif

/*
 * Add a menu item to a menu
 */
    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಂಕ		idx)
{
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

    menu->id = s_menu_id++;
    menu->submenu_id = ಖಾಲಿ;

# ifdef FEAT_TEAROFF
    ಆದರೆ (STRNCMP(menu->name, TEAR_STRING, TEAR_LEN) == 0)
    {
	ಸೇರಿಸು_Menu(ಪೋಷಕ->submenu_id, (UINT)idx, MF_BITMAP|MF_BYPOSITION,
		(UINT)menu->id, (LPCTSTR) s_htearbitmap);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
# ifdef FEAT_TOOLBAR
    ಆದರೆ (menu_is_toolbar(ಪೋಷಕ->name))
    {
	TBBUTTON newtb;

	CLEAR_ಕ್ಷೇತ್ರ(newtb);
	ಆದರೆ (menu_is_separator(menu->name))
	{
	    newtb.iBitmap = 0;
	    newtb.fsStyle = TBSTYLE_SEP;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    newtb.iBitmap = get_toolbar_bitmap(menu);
	    newtb.fsStyle = TBSTYLE_BUTTON;
	}
	newtb.idCommand = menu->id;
	newtb.fsState = TBSTATE_ENABLED;
	newtb.iString = 0;
	Send_ಸಂದೇಶ(s_toolbarhwnd, TB_INSERTBUTTON, (WPARAM)idx,
							     (LPARAM)&newtb);
	menu->submenu_id = (HMENU)-1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	WCHAR	*wn;

	wn = enc_ಗಮ್ಯ_utf16(menu->name, ಖಾಲಿ);
	ಆದರೆ (wn != ಖಾಲಿ)
	{
	    ಸೇರಿಸು_Menu_W(ಪೋಷಕ->submenu_id, (UINT)idx,
		    (menu_is_separator(menu->name)
		     ? MF_SEPARATOR : MF_STRING) | MF_BYPOSITION,
		    (UINT)menu->id, wn);
	    vim_free(wn);
	}
# ifdef FEAT_TEAROFF
	ಆದರೆ (Is_ಕಿಟಕಿ(ಪೋಷಕ->tearoff_handle))
	    rebuild_tearoff(ಪೋಷಕ);
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
    ಆದರೆ (menu->submenu_id == (HMENU)-1)
    {
	ಪೂರ್ಣಾಂಕ iButton;

	iButton = (ಪೂರ್ಣಾಂಕ)Send_ಸಂದೇಶ(s_toolbarhwnd, TB_COMMANDTOINDEX,
							 (WPARAM)menu->id, 0);
	Send_ಸಂದೇಶ(s_toolbarhwnd, TB_DELETEBUTTON, (WPARAM)iButton, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	ಆದರೆ (menu->ಪೋಷಕ != ಖಾಲಿ
		&& menu_is_popup(menu->ಪೋಷಕ->dname)
		&& menu->ಪೋಷಕ->submenu_id != ಖಾಲಿ)
	    RemoveMenu(menu->ಪೋಷಕ->submenu_id, menu->id, MF_BYCOMMAND);
	ಇಲ್ಲದಿದ್ದರೆ
	    RemoveMenu(s_menuBar, menu->id, MF_BYCOMMAND);
	ಆದರೆ (menu->submenu_id != ಖಾಲಿ)
	    DestroyMenu(menu->submenu_id);
# ifdef FEAT_TEAROFF
	ಆದರೆ (Is_ಕಿಟಕಿ(menu->tearoff_handle))
	    Destroy_ಕಿಟಕಿ(menu->tearoff_handle);
	ಆದರೆ (menu->ಪೋಷಕ != ಖಾಲಿ
		&& menu->ಪೋಷಕ->children != ಖಾಲಿ
		&& Is_ಕಿಟಕಿ(menu->ಪೋಷಕ->tearoff_handle))
	{
	    // This menu must not show up when rebuilding the tearoff window.
	    menu->modes = 0;
	    rebuild_tearoff(menu->ಪೋಷಕ);
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
    ಪೂರ್ಣಾಂಕ		x, y;

    HWND thwnd = menu->tearoff_handle;

    Get_ಕಿಟಕಿ_ಪಠ್ಯ(thwnd, (LPSTR)tbuf, 127);
    ಆದರೆ (Get_ಕಿಟಕಿ_Rect(thwnd, &trect)
	    && Get_ಕಿಟಕಿ_Rect(s_hwnd, &rct)
	    && GetClientRect(s_hwnd, &roct))
    {
	x = trect.ಎಡ_2 - rct.ಎಡ_2;
	y = (trect.ತುದಿ -  rct.bottom  + roct.bottom);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	x = y = 0xffffL;
    }
    Destroy_ಕಿಟಕಿ(thwnd);
    ಆದರೆ (menu->children != ಖಾಲಿ)
    {
	gui_mch_tearoff(tbuf, menu, x, y);
	ಆದರೆ (Is_ಕಿಟಕಿ(menu->tearoff_handle))
	    (ಶೂನ್ಯ) ಗಣ_ಕಿಟಕಿ_Pos(menu->tearoff_handle,
				ಖಾಲಿ,
				(ಪೂರ್ಣಾಂಕ)trect.ಎಡ_2,
				(ಪೂರ್ಣಾಂಕ)trect.ತುದಿ,
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
    ಪೂರ್ಣಾಂಕ	    grey)
{
# ifdef FEAT_TOOLBAR
    /*
     * is this a toolbar button?
     */
    ಆದರೆ (menu->submenu_id == (HMENU)-1)
	Send_ಸಂದೇಶ(s_toolbarhwnd, TB_ENABLEBUTTON,
		(WPARAM)menu->id, (LPARAM) MAKELONG((grey ? FALSE : TRUE), 0));
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	(ಶೂನ್ಯ)Enable_Menu_ಅಂಶ(menu->ಪೋಷಕ ? menu->ಪೋಷಕ->submenu_id : s_menuBar,
		menu->id, MF_BYCOMMAND | (grey ? MF_GRAYED : MF_ENABLED));

# ifdef FEAT_TEAROFF
    ಆದರೆ ((menu->ಪೋಷಕ != ಖಾಲಿ) && (Is_ಕಿಟಕಿ(menu->ಪೋಷಕ->tearoff_handle)))
    {
	ಶಬ್ದ_2 menuID;
	HWND menuHandle;

	/*
	 * A tearoff button has changed state.
	 */
	ಆದರೆ (menu->children == ಖಾಲಿ)
	    menuID = (ಶಬ್ದ_2)(menu->id);
	ಇಲ್ಲದಿದ್ದರೆ
	    menuID = (ಶಬ್ದ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	menuHandle = Get_Dlg_ಅಂಶ(menu->ಪೋಷಕ->tearoff_handle, menuID);
	ಆದರೆ (menuHandle)
	    Enable_ಕಿಟಕಿ(menuHandle, !grey);

    }
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU


// define some macros used to make the dialogue creation more readable

#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_ಶಬ್ದ(x)		*p++ = (x)
#ವ್ಯಾಖ್ಯಾನಿಸು ಕೂಡಿಸು_long(x)		dwp = (DWORD *)p; *dwp++ = (x); p = (ಶಬ್ದ_2 *)dwp

#ಆದರೆ defined(FEAT_GUI_DIALOG)
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
    UINT ಸಂದೇಶ_2,
    WPARAM wParam,
    LPARAM lParam ಬಳಸದ)
{
    ಆದರೆ (ಸಂದೇಶ_2 == WM_INITDIALOG)
    {
	Center_ಕಿಟಕಿ(hwnd, Get_ಕಿಟಕಿ(hwnd, GW_OWNER));
	// Set focus to the dialog.  Set the default button, if specified.
	(ಶೂನ್ಯ)ಗಣ_Focus(hwnd);
	ಆದರೆ (dialog_default_button > IDCANCEL)
	    (ಶೂನ್ಯ)ಗಣ_Focus(Get_Dlg_ಅಂಶ(hwnd, dialog_default_button));
	ಇಲ್ಲದಿದ್ದರೆ
	    // We don't have a default, set focus on another element of the
	    // dialog window, probably the icon
	    (ಶೂನ್ಯ)ಗಣ_Focus(Get_Dlg_ಅಂಶ(hwnd, DLG_NONBUTTON_CONTROL));
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (ಸಂದೇಶ_2 == WM_COMMAND)
    {
	ಪೂರ್ಣಾಂಕ	button = LOWORD(wParam);

	// Don't end the dialog if something was selected that was
	// not a button.
	ಆದರೆ (button >= DLG_NONBUTTON_CONTROL)
	    ಹಿಂತಿರುಗಿ TRUE;

	// If the edit box exists, copy the string.
	ಆದರೆ (s_textfield != ಖಾಲಿ)
	{
	    WCHAR  *wp = ALLOC_MULT(WCHAR, IOSIZE);
	    char_u *p;

	    Get_Dlg_ಅಂಶ_ಪಠ್ಯ_W(hwnd, DLG_NONBUTTON_CONTROL + 2, wp, IOSIZE);
	    p = utf16_ಗಮ್ಯ_enc(wp, ಖಾಲಿ);
	    ಆದರೆ (p != ಖಾಲಿ)
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
	ಆದರೆ (button == IDOK)
	{
	    ಆದರೆ (dialog_default_button > IDCANCEL)
		ಕೊನೆ_Dialog(hwnd, dialog_default_button);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಕೊನೆ_Dialog(hwnd, button - IDCANCEL);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ ((ಸಂದೇಶ_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	ಕೊನೆ_Dialog(hwnd, 0);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dlg_icons[] = // must match names in resource file
{
    "IDR_VIM",
    "IDR_VIM_ERROR",
    "IDR_VIM_ALERT",
    "IDR_VIM_INFO",
    "IDR_VIM_QUESTION"
};

    ಪೂರ್ಣಾಂಕ
gui_mch_dialog(
    ಪೂರ್ಣಾಂಕ		 type,
    char_u	*title,
    char_u	*ಸಂದೇಶ_2,
    char_u	*buttons,
    ಪೂರ್ಣಾಂಕ		 dfltbutton,
    char_u	*textfield,
    ಪೂರ್ಣಾಂಕ		ex_cmd ಬಳಸದ)
{
    ಶಬ್ದ_2	*p, *pdlgtemplate, *pnumitems;
    DWORD	*dwp;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_Buttons;
    ಪೂರ್ಣಾಂಕ		*buttonWidths, *buttonPositions;
    ಪೂರ್ಣಾಂಕ		buttonYpos;
    ಪೂರ್ಣಾಂಕ		nchar, i;
    DWORD	lStyle;
    ಪೂರ್ಣಾಂಕ		dlgwidth = 0;
    ಪೂರ್ಣಾಂಕ		dlgheight;
    ಪೂರ್ಣಾಂಕ		editboxheight;
    ಪೂರ್ಣಾಂಕ		horiz_ಅಗಲ = 0;
    ಪೂರ್ಣಾಂಕ		msgheight;
    char_u	*pstart;
    char_u	*pend;
    char_u	*ಕೊನೆಯ_white;
    char_u	*tbuffer;
    RECT	rect;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    TEXTMETRIC	fontInfo;
    ಪೂರ್ಣಾಂಕ		font_ಎತ್ತರ_2;
    ಪೂರ್ಣಾಂಕ		ಪಠ್ಯ_ಅಗಲ_2, min_Button_ಅಗಲ, ಸಂದೇಶ_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		max_Dialog_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		max_Dialog_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		scroll_flag = 0;
    ಪೂರ್ಣಾಂಕ		vertical;
    ಪೂರ್ಣಾಂಕ		dlgPaddingX;
    ಪೂರ್ಣಾಂಕ		dlgPaddingY;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ಪೂರ್ಣಾಂಕ		use_lfSysmenu = FALSE;
# endif
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		l;
    ಪೂರ್ಣಾಂಕ		dlg_icon_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		dlg_icon_ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ		dpi;

# ifndef NO_CONSOLE
    // Don't output anything in silent mode ("ex -s")
#  ifdef VIMDLL
    ಆದರೆ (!(gui.ಒಳ_use || gui.starting))
#  endif
	ಆದರೆ (silent_ಬಹುಲಕ)
	    ಹಿಂತಿರುಗಿ dfltbutton;   // return default option
# endif

    ಆದರೆ (s_hwnd == ಖಾಲಿ)
    {
	ತುಂಬು_dpi_func();
	s_dpi = dpi = pGetDpiForSystem();
	get_dialog_font_metrics();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	dpi = pGetDpiForSystem();

    ಆದರೆ ((type < 0) || (type > VIM_ಕೊನೆಯ_TYPE))
	type = 0;

    // allocate some memory for dialog template
    // TODO should compute this really
    pdlgtemplate = p = (PWORD)LocalAlloc(LPTR,
					DLG_ALLOC_ಗಾತ್ರ + STRLEN(ಸಂದೇಶ_2) * 2);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    /*
     * make a copy of 'buttons' to fiddle with it.  compiler grizzles because
     * vim_strsave() doesn't take a const arg (why not?), so cast away the
     * const.
     */
    tbuffer = vim_strsave(buttons);
    ಆದರೆ (tbuffer == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    --dfltbutton;   // Change from one-based to zero-based

    // Count buttons
    ಅಂಕೆ_Buttons = 1;
    ಗಾಗಿ (i = 0; tbuffer[i] != '\0'; i++)
    {
	ಆದರೆ (tbuffer[i] == DLG_BUTTON_SEP)
	    ಅಂಕೆ_Buttons++;
    }
    ಆದರೆ (dfltbutton >= ಅಂಕೆ_Buttons)
	dfltbutton = -1;

    // Allocate array to hold the width of each button
    buttonWidths = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಅಂಕೆ_Buttons);
    ಆದರೆ (buttonWidths == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    // Allocate array to hold the X position of each button
    buttonPositions = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಅಂಕೆ_Buttons);
    ಆದರೆ (buttonPositions == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    /*
     * Calculate how big the dialog must be.
     */
    hwnd = Get_Desktop_ಕಿಟಕಿ();
    hdc = Get_ಕಿಟಕಿ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ಆದರೆ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_ಪರೋಕ್ಷ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	font = CreateFont(-DLG_FONT_ಬಿಂದು_ಗಾತ್ರ, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);
    dlgPaddingX = DLG_PADDING_X;
    dlgPaddingY = DLG_PADDING_Y;

    Get_ಪಠ್ಯ_Metrics(hdc, &fontInfo);
    font_ಎತ್ತರ_2 = fontInfo.tm_ಎತ್ತರ;

    // Minimum width for horizontal button
    min_Button_ಅಗಲ = Get_ಪಠ್ಯ_ಅಗಲ(hdc, (char_u *)"Cancel", 6);

    // Maximum width of a dialog, if possible
    ಆದರೆ (s_hwnd == ಖಾಲಿ)
    {
	RECT	workarea_rect;

	// We don't have a window, use the desktop area.
	get_work_area(&workarea_rect);
	max_Dialog_ಅಗಲ = workarea_rect.ಬಲ_2 - workarea_rect.ಎಡ_2 - 100;
	ಆದರೆ (max_Dialog_ಅಗಲ > adjust_by_system_dpi(600))
	    max_Dialog_ಅಗಲ = adjust_by_system_dpi(600);
	// Leave some room for the taskbar.
	max_Dialog_ಎತ್ತರ = workarea_rect.bottom - workarea_rect.ತುದಿ - 150;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Use our own window for the size, unless it's very small.
	Get_ಕಿಟಕಿ_Rect(s_hwnd, &rect);
	max_Dialog_ಅಗಲ = rect.ಬಲ_2 - rect.ಎಡ_2
		       - (pGetSystemMetricsForDpi(SM_CXFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 2;
	ಆದರೆ (max_Dialog_ಅಗಲ < adjust_by_system_dpi(DLG_MIN_MAX_ಅಗಲ))
	    max_Dialog_ಅಗಲ = adjust_by_system_dpi(DLG_MIN_MAX_ಅಗಲ);

	max_Dialog_ಎತ್ತರ = rect.bottom - rect.ತುದಿ
		       - (pGetSystemMetricsForDpi(SM_CYFRAME, dpi) +
			  pGetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi)) * 4
		       - pGetSystemMetricsForDpi(SM_CYCAPTION, dpi);
	ಆದರೆ (max_Dialog_ಎತ್ತರ < adjust_by_system_dpi(DLG_MIN_MAX_ಎತ್ತರ))
	    max_Dialog_ಎತ್ತರ = adjust_by_system_dpi(DLG_MIN_MAX_ಎತ್ತರ);
    }

    // Set dlgwidth to width of message.
    // Copy the message into "ga", changing NL to CR-NL and inserting line
    // breaks where needed.
    pstart = ಸಂದೇಶ_2;
    ಸಂದೇಶ_ಅಗಲ = 0;
    msgheight = 0;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 500);
    ಮಾಡು
    {
	msgheight += font_ಎತ್ತರ_2;    // at least one line

	// Need to figure out where to break the string.  The system does it
	// at a word boundary, which would mean we can't compute the number of
	// wrapped lines.
	ಪಠ್ಯ_ಅಗಲ_2 = 0;
	ಕೊನೆಯ_white = ಖಾಲಿ;
	ಗಾಗಿ (pend = pstart; *pend != NUL && *pend != '\n'; )
	{
	    l = (*mb_ptr2len)(pend);
	    ಆದರೆ (l == 1 && VIM_ISWHITE(*pend)
					&& ಪಠ್ಯ_ಅಗಲ_2 > max_Dialog_ಅಗಲ * 3 / 4)
		ಕೊನೆಯ_white = pend;
	    ಪಠ್ಯ_ಅಗಲ_2 += Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, pend, l);
	    ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 >= max_Dialog_ಅಗಲ)
	    {
		// Line will wrap.
		ಸಂದೇಶ_ಅಗಲ = max_Dialog_ಅಗಲ;
		msgheight += font_ಎತ್ತರ_2;
		ಪಠ್ಯ_ಅಗಲ_2 = 0;

		ಆದರೆ (ಕೊನೆಯ_white != ಖಾಲಿ)
		{
		    // break the line just after a space
		    ಆದರೆ (pend > ಕೊನೆಯ_white)
			ga.ga_len -= (ಪೂರ್ಣಾಂಕ)(pend - (ಕೊನೆಯ_white + 1));
		    pend = ಕೊನೆಯ_white + 1;
		    ಕೊನೆಯ_white = ಖಾಲಿ;
		}
		ga_append(&ga, '\r');
		ga_append(&ga, '\n');
		ಮುಂದುವರಿ;
	    }

	    ಆಗ (--l >= 0)
		ga_append(&ga, *pend++);
	}
	ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 > ಸಂದೇಶ_ಅಗಲ)
	    ಸಂದೇಶ_ಅಗಲ = ಪಠ್ಯ_ಅಗಲ_2;

	ga_append(&ga, '\r');
	ga_append(&ga, '\n');
	pstart = pend + 1;
    } ಆಗ (*pend != NUL);

    ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
	ಸಂದೇಶ_2 = ga.ga_ದತ್ತಾಂಶ;

    ಸಂದೇಶ_ಅಗಲ += 10;		// roundoff space

    dlg_icon_ಅಗಲ = adjust_by_system_dpi(DLG_ICON_ಅಗಲ);
    dlg_icon_ಎತ್ತರ = adjust_by_system_dpi(DLG_ICON_ಎತ್ತರ);

    // Add width of icon to dlgwidth, and some space
    dlgwidth = ಸಂದೇಶ_ಅಗಲ + dlg_icon_ಅಗಲ + 3 * dlgPaddingX
			     + pGetSystemMetricsForDpi(SM_CXVSCROLL, dpi);

    ಆದರೆ (msgheight < dlg_icon_ಎತ್ತರ)
	msgheight = dlg_icon_ಎತ್ತರ;

    /*
     * Check button names.  A long one will make the dialog wider.
     * When called early (-register error message) p_go isn't initialized.
     */
    vertical = (p_go != ಖಾಲಿ && vim_strchr(p_go, GO_VERTICAL) != ಖಾಲಿ);
    ಆದರೆ (!vertical)
    {
	// Place buttons horizontally if they fit.
	horiz_ಅಗಲ = dlgPaddingX;
	pstart = tbuffer;
	i = 0;
	ಮಾಡು
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ಆದರೆ (pend == ಖಾಲಿ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    ಪಠ್ಯ_ಅಗಲ_2 = Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, pstart, (ಪೂರ್ಣಾಂಕ)(pend - pstart));
	    ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 < min_Button_ಅಗಲ)
		ಪಠ್ಯ_ಅಗಲ_2 = min_Button_ಅಗಲ;
	    ಪಠ್ಯ_ಅಗಲ_2 += dlgPaddingX;	    // Padding within button
	    buttonWidths[i] = ಪಠ್ಯ_ಅಗಲ_2;
	    buttonPositions[i++] = horiz_ಅಗಲ;
	    horiz_ಅಗಲ += ಪಠ್ಯ_ಅಗಲ_2 + dlgPaddingX; // Pad between buttons
	    pstart = pend + 1;
	} ಆಗ (*pend != NUL);

	ಆದರೆ (horiz_ಅಗಲ > max_Dialog_ಅಗಲ)
	    vertical = TRUE;	// Too wide to fit on the screen.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (horiz_ಅಗಲ > dlgwidth)
	    dlgwidth = horiz_ಅಗಲ;
    }

    ಆದರೆ (vertical)
    {
	// Stack buttons vertically.
	pstart = tbuffer;
	ಮಾಡು
	{
	    pend = vim_strchr(pstart, DLG_BUTTON_SEP);
	    ಆದರೆ (pend == ಖಾಲಿ)
		pend = pstart + STRLEN(pstart);	// Last button name.
	    ಪಠ್ಯ_ಅಗಲ_2 = Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, pstart, (ಪೂರ್ಣಾಂಕ)(pend - pstart));
	    ಪಠ್ಯ_ಅಗಲ_2 += dlgPaddingX;		// Padding within button
	    ಪಠ್ಯ_ಅಗಲ_2 += DLG_VERT_PADDING_X * 2; // Padding around button
	    ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 > dlgwidth)
		dlgwidth = ಪಠ್ಯ_ಅಗಲ_2;
	    pstart = pend + 1;
	} ಆಗ (*pend != NUL);
    }

    ಆದರೆ (dlgwidth < DLG_MIN_ಅಗಲ)
	dlgwidth = DLG_MIN_ಅಗಲ;	// Don't allow a really thin dialog!

    // start to fill in the dlgtemplate information.  addressing by WORDs
    lStyle = DS_MODALFRAME | WS_CAPTION | DS_3DLOOK | WS_VISIBLE | DS_SETFONT;

    ಕೂಡಿಸು_long(lStyle);
    ಕೂಡಿಸು_long(0);	// (lExtendedStyle)
    pnumitems = p;	//save where the number of items must be stored
    ಕೂಡಿಸು_ಶಬ್ದ(0);	// NumberOfItems(will change later)
    ಕೂಡಿಸು_ಶಬ್ದ(10);	// x
    ಕೂಡಿಸು_ಶಬ್ದ(10);	// y
    ಕೂಡಿಸು_ಶಬ್ದ(Pixel_ಗಮ್ಯ_Dialog_X(dlgwidth));	// cx

    // Dialog height.
    ಆದರೆ (vertical)
	dlgheight = msgheight + 2 * dlgPaddingY
			   + DLG_VERT_PADDING_Y + 2 * font_ಎತ್ತರ_2 * ಅಂಕೆ_Buttons;
    ಇಲ್ಲದಿದ್ದರೆ
	dlgheight = msgheight + 3 * dlgPaddingY + 2 * font_ಎತ್ತರ_2;

    // Dialog needs to be taller if contains an edit box.
    editboxheight = font_ಎತ್ತರ_2 + dlgPaddingY + 4 * DLG_VERT_PADDING_Y;
    ಆದರೆ (textfield != ಖಾಲಿ)
	dlgheight += editboxheight;

    // Restrict the size to a maximum.  Causes a scrollbar to show up.
    ಆದರೆ (dlgheight > max_Dialog_ಎತ್ತರ)
    {
	msgheight = msgheight - (dlgheight - max_Dialog_ಎತ್ತರ);
	dlgheight = max_Dialog_ಎತ್ತರ;
	scroll_flag = WS_VSCROLL;
	// Make sure scrollbar doesn't appear in the middle of the dialog
	ಸಂದೇಶ_ಅಗಲ = dlgwidth - dlg_icon_ಅಗಲ - 3 * dlgPaddingX;
    }

    ಕೂಡಿಸು_ಶಬ್ದ(Pixel_ಗಮ್ಯ_Dialog_Y(dlgheight));

    ಕೂಡಿಸು_ಶಬ್ದ(0);	// Menu
    ಕೂಡಿಸು_ಶಬ್ದ(0);	// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(p, (title ? (LPSTR)title
				   : (LPSTR)("Vim "VIM_ಆವೃತ್ತಿ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ಆದರೆ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ಎತ್ತರ, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ಪೂರ್ಣಾಂಕ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	*p++ = DLG_FONT_ಬಿಂದು_ಗಾತ್ರ;		// point size
	nchar = n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    buttonYpos = msgheight + 2 * dlgPaddingY;

    ಆದರೆ (textfield != ಖಾಲಿ)
	buttonYpos += editboxheight;

    pstart = tbuffer;
    ಆದರೆ (!vertical)
	horiz_ಅಗಲ = (dlgwidth - horiz_ಅಗಲ) / 2;	// Now it's X offset
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_Buttons; i++)
    {
	// get end of this button.
	ಗಾಗಿ (	pend = pstart;
		*pend && (*pend != DLG_BUTTON_SEP);
		pend++)
	    ;

	ಆದರೆ (*pend)
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
	ಆದರೆ (vertical)
	{
	    p = ಕೂಡಿಸು_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_ಗಮ್ಯ_Dialog_X(DLG_VERT_PADDING_X),
		    Pixel_ಗಮ್ಯ_Dialog_Y(buttonYpos // TBK
				   + 2 * font_ಎತ್ತರ_2 * i),
		    Pixel_ಗಮ್ಯ_Dialog_X(dlgwidth - 2 * DLG_VERT_PADDING_X),
		    (ಶಬ್ದ_2)(Pixel_ಗಮ್ಯ_Dialog_Y(2 * font_ಎತ್ತರ_2) - 1),
		    (ಶಬ್ದ_2)(IDCANCEL + 1 + i), (ಶಬ್ದ_2)0x0080, (ಅಕ್ಷರ *)pstart);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p = ಕೂಡಿಸು_dialog_element(p,
		    (i == dfltbutton
			    ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON) | WS_TABSTOP,
		    Pixel_ಗಮ್ಯ_Dialog_X(horiz_ಅಗಲ + buttonPositions[i]),
		    Pixel_ಗಮ್ಯ_Dialog_Y(buttonYpos), // TBK
		    Pixel_ಗಮ್ಯ_Dialog_X(buttonWidths[i]),
		    (ಶಬ್ದ_2)(Pixel_ಗಮ್ಯ_Dialog_Y(2 * font_ಎತ್ತರ_2) - 1),
		    (ಶಬ್ದ_2)(IDCANCEL + 1 + i), (ಶಬ್ದ_2)0x0080, (ಅಕ್ಷರ *)pstart);
	}
	pstart = pend + 1;	//next button
    }
    *pnumitems += ಅಂಕೆ_Buttons;

    // Vim icon
    p = ಕೂಡಿಸು_dialog_element(p, SS_ICON,
	    Pixel_ಗಮ್ಯ_Dialog_X(dlgPaddingX),
	    Pixel_ಗಮ್ಯ_Dialog_Y(dlgPaddingY),
	    Pixel_ಗಮ್ಯ_Dialog_X(dlg_icon_ಅಗಲ),
	    Pixel_ಗಮ್ಯ_Dialog_Y(dlg_icon_ಎತ್ತರ),
	    DLG_NONBUTTON_CONTROL + 0, (ಶಬ್ದ_2)0x0082,
	    dlg_icons[type]);

    // Dialog message
    p = ಕೂಡಿಸು_dialog_element(p, ES_ಎಡ|scroll_flag|ES_MULTILINE|ES_READONLY,
	    Pixel_ಗಮ್ಯ_Dialog_X(2 * dlgPaddingX + dlg_icon_ಅಗಲ),
	    Pixel_ಗಮ್ಯ_Dialog_Y(dlgPaddingY),
	    (ಶಬ್ದ_2)(Pixel_ಗಮ್ಯ_Dialog_X(ಸಂದೇಶ_ಅಗಲ) + 1),
	    Pixel_ಗಮ್ಯ_Dialog_Y(msgheight),
	    DLG_NONBUTTON_CONTROL + 1, (ಶಬ್ದ_2)0x0081, (ಅಕ್ಷರ *)ಸಂದೇಶ_2);

    // Edit box
    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	p = ಕೂಡಿಸು_dialog_element(p, ES_ಎಡ|ES_AUTOHSCROLL|WS_TABSTOP|WS_BORDER,
		Pixel_ಗಮ್ಯ_Dialog_X(2 * dlgPaddingX),
		Pixel_ಗಮ್ಯ_Dialog_Y(2 * dlgPaddingY + msgheight),
		Pixel_ಗಮ್ಯ_Dialog_X(dlgwidth - 4 * dlgPaddingX),
		Pixel_ಗಮ್ಯ_Dialog_Y(font_ಎತ್ತರ_2 + dlgPaddingY),
		DLG_NONBUTTON_CONTROL + 2, (ಶಬ್ದ_2)0x0081, (ಅಕ್ಷರ *)textfield);
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
    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	dialog_default_button = DLG_NONBUTTON_CONTROL + 2;
	s_textfield = textfield;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	dialog_default_button = IDCANCEL + 1 + dfltbutton;
	s_textfield = ಖಾಲಿ;
    }

    // show the dialog box modally and get a return value
    nchar = (ಪೂರ್ಣಾಂಕ)Dialog_Box_ಪರೋಕ್ಷ(
	    g_hinst,
	    (LPDLGTEMPLATE)pdlgtemplate,
	    s_hwnd,
	    (DLGPROC)dialog_callback);

    LocalFree(LocalHandle(pdlgtemplate));
    vim_free(tbuffer);
    vim_free(buttonWidths);
    vim_free(buttonPositions);
    vim_free(ga.ga_ದತ್ತಾಂಶ);

    // Focus back to our window (for when MDI is used).
    (ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);

    ಹಿಂತಿರುಗಿ nchar;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG

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
ಕೂಡಿಸು_dialog_element(
    PWORD p,
    DWORD lStyle,
    ಶಬ್ದ_2 x,
    ಶಬ್ದ_2 y,
    ಶಬ್ದ_2 w,
    ಶಬ್ದ_2 h,
    ಶಬ್ದ_2 Id,
    ಶಬ್ದ_2 clss,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *caption)
{
    ಪೂರ್ಣಾಂಕ nchar;

    p = lpwAlign(p);	// Align to dword boundary
    lStyle = lStyle | WS_VISIBLE | WS_ಮಗು;
    *p++ = LOWORD(lStyle);
    *p++ = HIWORD(lStyle);
    *p++ = 0;		// LOWORD (lExtendedStyle)
    *p++ = 0;		// HIWORD (lExtendedStyle)
    *p++ = x;
    *p++ = y;
    *p++ = w;
    *p++ = h;
    *p++ = Id;		//9 or 10 words in all

    *p++ = (ಶಬ್ದ_2)0xffff;
    *p++ = clss;			//2 more here

    nchar = n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(p, (LPSTR)caption, TRUE); //strlen(caption)+1
    p += nchar;

    *p++ = 0;  // advance pointer over nExtraStuff WORD   - 2 more

    ಹಿಂತಿರುಗಿ p;	// total = 15 + strlen(caption) words
		// bytes read = 2 * total
}


/*
 * Helper routine.  Take an input pointer, return closest pointer that is
 * aligned on a DWORD (4 byte) boundary.  Taken from the Win32SDK samples.
 */
    ಸ್ಥಿರ LPWORD
lpwAlign(
    LPWORD lp_ಒಳ)
{
    long_u ul;

    ul = (long_u)lp_ಒಳ;
    ul += 3;
    ul >>= 2;
    ul <<= 2;
    ಹಿಂತಿರುಗಿ (LPWORD)ul;
}

/*
 * Helper routine.  Takes second parameter as Ansi string, copies it to first
 * parameter as wide character (16-bits / char) string, and returns integer
 * number of wide characters (words) in string (including the trailing wide
 * char NULL).  Partly taken from the Win32SDK samples.
 * If "use_enc" is TRUE, 'encoding' is used for "lpAnsiIn". If FALSE, current
 * ACP is used for "lpAnsiIn". */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(
    LPWORD lpWCStr,
    LPSTR lp_Ansi_ಒಳ,
    BOOL use_enc)
{
    ಪೂರ್ಣಾಂಕ		nChar = 0;
    ಪೂರ್ಣಾಂಕ		len = lstrlen(lp_Ansi_ಒಳ) + 1;	// include NUL character
    ಪೂರ್ಣಾಂಕ		i;
    WCHAR	*wn;

    ಆದರೆ (use_enc && enc_codepage >= 0 && (ಪೂರ್ಣಾಂಕ)GetACP() != enc_codepage)
    {
	// Not a codepage, use our own conversion function.
	wn = enc_ಗಮ್ಯ_utf16((char_u *)lp_Ansi_ಒಳ, ಖಾಲಿ);
	ಆದರೆ (wn != ಖಾಲಿ)
	{
	    wcscpy(lpWCStr, wn);
	    nChar = (ಪೂರ್ಣಾಂಕ)wcslen(wn) + 1;
	    vim_free(wn);
	}
    }
    ಆದರೆ (nChar == 0)
	// Use Win32 conversion function.
	nChar = Multi_Byte_ಗಮ್ಯ_Wide_Char(
		enc_codepage > 0 ? enc_codepage : CP_ACP,
		MB_PRECOMPOSED,
		lp_Ansi_ಒಳ, len,
		lpWCStr, len);
    ಗಾಗಿ (i = 0; i < nChar; ++i)
	ಆದರೆ (lpWCStr[i] == (ಶಬ್ದ_2)'\t')	// replace tabs with spaces
	    lpWCStr[i] = (ಶಬ್ದ_2)' ';

    ಹಿಂತಿರುಗಿ nChar;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TEAROFF
/*
 * Lookup menu handle from "menu_id".
 */
    ಸ್ಥಿರ HMENU
tearoff_lookup_menuhandle(
    vimmenu_T *menu,
    ಶಬ್ದ_2 menu_id)
{
    ಗಾಗಿ ( ; menu != ಖಾಲಿ; menu = menu->ಮುಂದಿನ)
    {
	ಆದರೆ (menu->modes == 0)	// this menu has just been deleted
	    ಮುಂದುವರಿ;
	ಆದರೆ (menu_is_separator(menu->dname))
	    ಮುಂದುವರಿ;
	ಆದರೆ ((ಶಬ್ದ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000) == menu_id)
	    ಹಿಂತಿರುಗಿ menu->submenu_id;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * The callback function for all the modeless dialogs that make up the
 * "tearoff menus" Very simple - forward button presses (to fool Vim into
 * thinking its menus have been clicked), and go away when closed.
 */
    ಸ್ಥಿರ LRESULT CALLBACK
tearoff_callback(
    HWND hwnd,
    UINT ಸಂದೇಶ_2,
    WPARAM wParam,
    LPARAM lParam)
{
    ಆದರೆ (ಸಂದೇಶ_2 == WM_INITDIALOG)
    {
	ಗಣ_ಕಿಟಕಿ_Long_Ptr(hwnd, DWLP_USER, (LONG_PTR)lParam);
	ಹಿಂತಿರುಗಿ (TRUE);
    }

    // May show the mouse pointer again.
    HandleMouseHide(ಸಂದೇಶ_2, lParam);

    ಆದರೆ (ಸಂದೇಶ_2 == WM_COMMAND)
    {
	ಆದರೆ ((ಶಬ್ದ_2)(LOWORD(wParam)) & 0x8000)
	{
	    ಬಿಂದು_2   mp;
	    RECT    rect;

	    ಆದರೆ (GetCursorPos(&mp) && Get_ಕಿಟಕಿ_Rect(hwnd, &rect))
	    {
		vimmenu_T *menu;

		menu = (vimmenu_T*)Get_ಕಿಟಕಿ_Long_Ptr(hwnd, DWLP_USER);
		(ಶೂನ್ಯ)TrackPopupMenu(
			 tearoff_lookup_menuhandle(menu, LOWORD(wParam)),
			 TPM_LEFTALIGN | TPM_LEFTBUTTON,
			 (ಪೂರ್ಣಾಂಕ)rect.ಬಲ_2 - 8,
			 (ಪೂರ್ಣಾಂಕ)mp.y,
			 (ಪೂರ್ಣಾಂಕ)0,	    // reserved param
			 s_hwnd,
			 ಖಾಲಿ);
		/*
		 * NOTE: The pop-up menu can eat the mouse up event.
		 * We deal with this in normal.c.
		 */
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Pass on messages to the main Vim window
	    Post_ಸಂದೇಶ(s_hwnd, WM_COMMAND, LOWORD(wParam), 0);
	/*
	 * Give main window the focus back: this is so after
	 * choosing a tearoff button you can start typing again
	 * straight away.
	 */
	(ಶೂನ್ಯ)ಗಣ_Focus(s_hwnd);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಆದರೆ ((ಸಂದೇಶ_2 == WM_SYSCOMMAND) && (wParam == SC_CLOSE))
    {
	Destroy_ಕಿಟಕಿ(hwnd);
	ಹಿಂತಿರುಗಿ TRUE;
    }

    // When moved around, give main window the focus back.
    ಆದರೆ (ಸಂದೇಶ_2 == WM_EXITSIZEMOVE)
	(ಶೂನ್ಯ)ಗಣ_ಸಕ್ರಿಯ_ಕಿಟಕಿ(s_hwnd);

    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ


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
    ಗಾತ್ರ_4	    ಗಾತ್ರ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SYSMENU_FONT
    LOGFONTW	    lfSysmenu;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SYSMENU_FONT
    ಆದರೆ (gui_w32_get_menu_font(&lfSysmenu) == OK)
	hfontTools = Create_Font_ಪರೋಕ್ಷ_W(&lfSysmenu);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	hfontTools = CreateFont(-DLG_FONT_ಬಿಂದು_ಗಾತ್ರ, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    hdc = GetDC(s_hwnd);
    SelectObject(hdc, hfontTools);
    Get_ಸರಾಸರಿ_ಮೌಲ್ಯ_Font_ಗಾತ್ರ(hdc, &ಗಾತ್ರ_2);
    ReleaseDC(s_hwnd, hdc);

    s_dlgfntwidth = (ಶಬ್ದ_2)ಗಾತ್ರ_2.cx;
    s_dlgfntheight = (ಶಬ್ದ_2)ಗಾತ್ರ_2.cy;
}

#ಆದರೆ defined(FEAT_MENU) && defined(FEAT_TEAROFF)
/*
 * Create a pseudo-"tearoff menu" based on the child
 * items of a given menu pointer.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_tearoff(
    char_u	*title,
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಂಕ		initX,
    ಪೂರ್ಣಾಂಕ		initY)
{
    ಶಬ್ದ_2	*p, *pdlgtemplate, *pnumitems, *ptrueheight;
    ಪೂರ್ಣಾಂಕ		template_len;
    ಪೂರ್ಣಾಂಕ		nchar, ಪಠ್ಯ_ಅಗಲ_2, submenu_ಅಗಲ;
    DWORD	lStyle;
    DWORD	lExtendedStyle;
    ಶಬ್ದ_2	dlgwidth;
    ಶಬ್ದ_2	menuID;
    vimmenu_T	*pmenu;
    vimmenu_T	*ತುದಿ_menu;
    vimmenu_T	*the_menu = menu;
    HWND	hwnd;
    HDC		hdc;
    HFONT	font, oldFont;
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು, space_ಅಗಲ, len;
    ಪೂರ್ಣಾಂಕ		ಸ್ತಂಭ_Widths[2];
    char_u	*label, *ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		acLen = 0;
    ಪೂರ್ಣಾಂಕ		nameLen;
    ಪೂರ್ಣಾಂಕ		padding0, padding1, padding2 = 0;
    ಪೂರ್ಣಾಂಕ		sepPadding=0;
    ಪೂರ್ಣಾಂಕ		x;
    ಪೂರ್ಣಾಂಕ		y;
# ifdef USE_SYSMENU_FONT
    LOGFONTW	lfSysmenu;
    ಪೂರ್ಣಾಂಕ		use_lfSysmenu = FALSE;
# endif

    /*
     * If this menu is already torn off, move it to the mouse position.
     */
    ಆದರೆ (Is_ಕಿಟಕಿ(menu->tearoff_handle))
    {
	ಬಿಂದು_2 mp;
	ಆದರೆ (GetCursorPos(&mp))
	{
	    ಗಣ_ಕಿಟಕಿ_Pos(menu->tearoff_handle, ಖಾಲಿ, mp.x, mp.y, 0, 0,
		    SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	}
	ಹಿಂತಿರುಗಿ;
    }

    /*
     * Create a new tearoff.
     */
    ಆದರೆ (*title == MNU_HIDDEN_CHAR)
	title++;

    // Allocate memory to store the dialog template.  It's made bigger when
    // needed.
    template_len = DLG_ALLOC_ಗಾತ್ರ;
    pdlgtemplate = p = (ಶಬ್ದ_2 *)LocalAlloc(LPTR, template_len);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    hwnd = Get_Desktop_ಕಿಟಕಿ();
    hdc = Get_ಕಿಟಕಿ_DC(hwnd);
# ifdef USE_SYSMENU_FONT
    ಆದರೆ (gui_w32_get_menu_font(&lfSysmenu) == OK)
    {
	font = Create_Font_ಪರೋಕ್ಷ_W(&lfSysmenu);
	use_lfSysmenu = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	font = CreateFont(-DLG_FONT_ಬಿಂದು_ಗಾತ್ರ, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, VARIABLE_PITCH, DLG_FONT_NAME);

    oldFont = SelectFont(hdc, font);

    // Calculate width of a single space.  Used for padding columns to the
    // right width.
    space_ಅಗಲ = Get_ಪಠ್ಯ_ಅಗಲ(hdc, (char_u *)" ", 1);

    // Figure out max width of the text column, the accelerator column and the
    // optional submenu column.
    submenu_ಅಗಲ = 0;
    ಗಾಗಿ (ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < 2; ಲಂಬಸಾಲು++)
    {
	ಸ್ತಂಭ_Widths[ಲಂಬಸಾಲು] = 0;
	FOR_ALL_ಮಗು_MENUS(menu, pmenu)
	{
	    // Use "dname" here to compute the width of the visible text.
	    ಪಠ್ಯ = (ಲಂಬಸಾಲು == 0) ? pmenu->dname : pmenu->actext;
	    ಆದರೆ (ಪಠ್ಯ != ಖಾಲಿ && *ಪಠ್ಯ != NUL)
	    {
		ಪಠ್ಯ_ಅಗಲ_2 = Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, ಪಠ್ಯ, (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ));
		ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 > ಸ್ತಂಭ_Widths[ಲಂಬಸಾಲು])
		    ಸ್ತಂಭ_Widths[ಲಂಬಸಾಲು] = ಪಠ್ಯ_ಅಗಲ_2;
	    }
	    ಆದರೆ (pmenu->children != ಖಾಲಿ)
		submenu_ಅಗಲ = TEAROFF_ಸ್ತಂಭ_PADDING * space_ಅಗಲ;
	}
    }
    ಆದರೆ (ಸ್ತಂಭ_Widths[1] == 0)
    {
	// no accelerators
	ಆದರೆ (submenu_ಅಗಲ != 0)
	    ಸ್ತಂಭ_Widths[0] += submenu_ಅಗಲ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ತಂಭ_Widths[0] += space_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// there is an accelerator column
	ಸ್ತಂಭ_Widths[0] += TEAROFF_ಸ್ತಂಭ_PADDING * space_ಅಗಲ;
	ಸ್ತಂಭ_Widths[1] += submenu_ಅಗಲ;
    }

    /*
     * Now find the total width of our 'menu'.
     */
    ಪಠ್ಯ_ಅಗಲ_2 = ಸ್ತಂಭ_Widths[0] + ಸ್ತಂಭ_Widths[1];
    ಆದರೆ (submenu_ಅಗಲ != 0)
    {
	submenu_ಅಗಲ = Get_ಪಠ್ಯ_ಅಗಲ(hdc, (char_u *)TEAROFF_SUBMENU_LABEL,
				  (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL));
	ಪಠ್ಯ_ಅಗಲ_2 += submenu_ಅಗಲ;
    }
    dlgwidth = Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, title, (ಪೂರ್ಣಾಂಕ)STRLEN(title));
    ಆದರೆ (ಪಠ್ಯ_ಅಗಲ_2 > dlgwidth)
	dlgwidth = ಪಠ್ಯ_ಅಗಲ_2;
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
    ಆದರೆ (initX == 0xffffL)
	*p++ = Pixel_ಗಮ್ಯ_Dialog_X(x); // x
    ಇಲ್ಲದಿದ್ದರೆ
	*p++ = Pixel_ಗಮ್ಯ_Dialog_X(initX); // x
    ಆದರೆ (initY == 0xffffL)
	*p++ = Pixel_ಗಮ್ಯ_Dialog_Y(y); // y
    ಇಲ್ಲದಿದ್ದರೆ
	*p++ = Pixel_ಗಮ್ಯ_Dialog_Y(initY); // y
    *p++ = Pixel_ಗಮ್ಯ_Dialog_X(dlgwidth);    // cx
    ptrueheight = p;
    *p++ = 0;		// dialog height: changed later anyway
    *p++ = 0;		// Menu
    *p++ = 0;		// Class

    // copy the title of the dialog
    nchar = n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(p, ((*title)
			    ? (LPSTR)title
			    : (LPSTR)("Vim "VIM_ಆವೃತ್ತಿ_MEDIUM)), TRUE);
    p += nchar;

    // do the font, since DS_3DLOOK doesn't work properly
# ifdef USE_SYSMENU_FONT
    ಆದರೆ (use_lfSysmenu)
    {
	// point size
	*p++ = -MulDiv(lfSysmenu.lf_ಎತ್ತರ, 72,
		GetDeviceCaps(hdc, LOGPIXELSY));
	wcscpy(p, lfSysmenu.lfFaceName);
	nchar = (ಪೂರ್ಣಾಂಕ)wcslen(lfSysmenu.lfFaceName) + 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	*p++ = DLG_FONT_ಬಿಂದು_ಗಾತ್ರ;		// point size
	nchar = n_Copy_Ansi_ಗಮ್ಯ_Wide_Char(p, DLG_FONT_NAME, FALSE);
    }
    p += nchar;

    /*
     * Loop over all the items in the menu.
     * But skip over the tearbar.
     */
    ಆದರೆ (STRCMP(menu->children->name, TEAR_STRING) == 0)
	menu = menu->children->ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	menu = menu->children;
    ತುದಿ_menu = menu;
    ಗಾಗಿ ( ; menu != ಖಾಲಿ; menu = menu->ಮುಂದಿನ)
    {
	ಆದರೆ (menu->modes == 0)	// this menu has just been deleted
	    ಮುಂದುವರಿ;
	ಆದರೆ (menu_is_separator(menu->dname))
	{
	    sepPadding += 3;
	    ಮುಂದುವರಿ;
	}

	// Check if there still is plenty of room in the template.  Make it
	// larger when needed.
	ಆದರೆ (((ಅಕ್ಷರ *)p - (ಅಕ್ಷರ *)pdlgtemplate) + 1000 > template_len)
	{
	    ಶಬ್ದ_2    *newp;

	    newp = (ಶಬ್ದ_2 *)LocalAlloc(LPTR, template_len + 4096);
	    ಆದರೆ (newp != ಖಾಲಿ)
	    {
		template_len += 4096;
		mch_memmove(newp, pdlgtemplate,
					    (ಅಕ್ಷರ *)p - (ಅಕ್ಷರ *)pdlgtemplate);
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
	len = nameLen = (ಪೂರ್ಣಾಂಕ)STRLEN(menu->name);
	padding0 = (ಸ್ತಂಭ_Widths[0] - Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, menu->dname,
				      (ಪೂರ್ಣಾಂಕ)STRLEN(menu->dname))) / space_ಅಗಲ;
	len += padding0;

	ಆದರೆ (menu->actext != ಖಾಲಿ)
	{
	    acLen = (ಪೂರ್ಣಾಂಕ)STRLEN(menu->actext);
	    len += acLen;
	    ಪಠ್ಯ_ಅಗಲ_2 = Get_ಪಠ್ಯ_ಅಗಲ_Enc(hdc, menu->actext, acLen);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಪಠ್ಯ_ಅಗಲ_2 = 0;
	padding1 = (ಸ್ತಂಭ_Widths[1] - ಪಠ್ಯ_ಅಗಲ_2) / space_ಅಗಲ;
	len += padding1;

	ಆದರೆ (menu->children == ಖಾಲಿ)
	{
	    padding2 = submenu_ಅಗಲ / space_ಅಗಲ;
	    len += padding2;
	    menuID = (ಶಬ್ದ_2)(menu->id);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    len += (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	    menuID = (ಶಬ್ದ_2)((long_u)(menu->submenu_id) | (DWORD)0x8000);
	}

	// Allocate menu label and fill it in
	ಪಠ್ಯ = label = alloc(len + 1);
	ಆದರೆ (label == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	vim_strncpy(ಪಠ್ಯ, menu->name, nameLen);
	ಪಠ್ಯ = vim_strchr(ಪಠ್ಯ, TAB);	    // stop at TAB before actext
	ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
	    ಪಠ್ಯ = label + nameLen;	    // no actext, use whole name
	ಆಗ (padding0-- > 0)
	    *ಪಠ್ಯ++ = ' ';
	ಆದರೆ (menu->actext != ಖಾಲಿ)
	{
	    STRNCPY(ಪಠ್ಯ, menu->actext, acLen);
	    ಪಠ್ಯ += acLen;
	}
	ಆಗ (padding1-- > 0)
	    *ಪಠ್ಯ++ = ' ';
	ಆದರೆ (menu->children != ಖಾಲಿ)
	{
	    STRCPY(ಪಠ್ಯ, TEAROFF_SUBMENU_LABEL);
	    ಪಠ್ಯ += STRLEN_LITERAL(TEAROFF_SUBMENU_LABEL);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಗ (padding2-- > 0)
		*ಪಠ್ಯ++ = ' ';
	}
	*ಪಠ್ಯ = NUL;

	/*
	 * BS_LEFT will just be ignored on Win32s/NT3.5x - on
	 * W95/NT4 it makes the tear-off look more like a menu.
	 */
	p = ಕೂಡಿಸು_dialog_element(p,
		BS_PUSHBUTTON|BS_ಎಡ,
		(ಶಬ್ದ_2)Pixel_ಗಮ್ಯ_Dialog_X(TEAROFF_PADDING_X),
		(ಶಬ್ದ_2)(sepPadding + 1 + 13 * (*pnumitems)),
		(ಶಬ್ದ_2)Pixel_ಗಮ್ಯ_Dialog_X(dlgwidth - 2 * TEAROFF_PADDING_X),
		(ಶಬ್ದ_2)12,
		menuID, (ಶಬ್ದ_2)0x0080, (ಅಕ್ಷರ *)label);
	vim_free(label);
	(*pnumitems)++;
    }

    *ptrueheight = (ಶಬ್ದ_2)(sepPadding + 1 + 13 * (*pnumitems));


    // show modelessly
    the_menu->tearoff_handle = Create_Dialog_ಪರೋಕ್ಷ_Param(
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
    (ಶೂನ್ಯ)ಗಣ_ಸಕ್ರಿಯ_ಕಿಟಕಿ(s_hwnd);

    // make sure the right buttons are enabled
    force_menu_update = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR)
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
		    WS_ಮಗು | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT,
		    4000,		//any old big number
		    31,			//number of images in initial bitmap
		    g_hinst,
		    IDR_TOOLBAR1,	// id of initial bitmap
		    ಖಾಲಿ,
		    0,			// initial number of buttons
		    TOOLBAR_BUTTON_ಅಗಲ, //api guide is wrong!
		    TOOLBAR_BUTTON_ಎತ್ತರ,
		    TOOLBAR_BUTTON_ಅಗಲ,
		    TOOLBAR_BUTTON_ಎತ್ತರ,
		    ಗಾತ್ರ(TBBUTTON)
		    );

    // Remove transparency from the toolbar to prevent the main window
    // background colour showing through
    Send_ಸಂದೇಶ(s_toolbarhwnd, TB_SETSTYLE, 0,
	Send_ಸಂದೇಶ(s_toolbarhwnd, TB_GETSTYLE, 0, 0) & ~TBSTYLE_TRANSPARENT);

    s_toolbar_wndproc = Subclass_ಕಿಟಕಿ(s_toolbarhwnd, toolbar_wndproc);

    gui_mch_show_toolbar(vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ);

    update_toolbar_ಗಾತ್ರ();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
update_toolbar_ಗಾತ್ರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		w, h;
    TBMETRICS	tbm;

    tbm.cb_ಗಾತ್ರ = ಗಾತ್ರ(TBMETRICS);
    tbm.dw_ಮುಖವಾಡ = TBMF_PAD | TBMF_BUTTONSPACING;
    Send_ಸಂದೇಶ(s_toolbarhwnd, TB_GETMETRICS, 0, (LPARAM)&tbm);
    //TRACE("Pad: %d, %d", tbm.cxPad, tbm.cyPad);
    //TRACE("ButtonSpacing: %d, %d", tbm.cxButtonSpacing, tbm.cyButtonSpacing);

    w = (TOOLBAR_BUTTON_ಅಗಲ + tbm.cxPad) * s_dpi / DEFAULT_DPI;
    h = (TOOLBAR_BUTTON_ಎತ್ತರ + tbm.cyPad) * s_dpi / DEFAULT_DPI;
    //TRACE("button size: %d, %d", w, h);
    Send_ಸಂದೇಶ(s_toolbarhwnd, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
    gui.toolbar_ಎತ್ತರ = h + 6;

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
    ಹಿಂತಿರುಗಿ Call_ಕಿಟಕಿ_Proc(s_toolbar_wndproc, hwnd, u_ಸಂದೇಶ, wParam, lParam);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_toolbar_bitmap(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಂಕ i = -1;

    /*
     * Check user bitmaps first, unless builtin is specified.
     */
    ಆದರೆ (!menu->icon_builtin)
    {
	char_u fname[MAXPATHL];
	HANDLE hbitmap = ಖಾಲಿ;

	ಆದರೆ (menu->iconfile != ಖಾಲಿ)
	{
	    gui_ಕಂಡುಹಿಡಿ_iconfile(menu->iconfile, fname, "bmp");
	    hbitmap = ತುಂಬು_Image(
			ಖಾಲಿ,
			(LPCSTR)fname,
			IMAGE_BITMAP,
			TOOLBAR_BUTTON_ಅಗಲ,
			TOOLBAR_BUTTON_ಎತ್ತರ,
			LR_LOADFROMFILE |
			LR_LOADMAP3DCOLORS
			);
	}

	/*
	 * If the LoadImage call failed, or the "icon=" file
	 * didn't exist or wasn't specified, try the menu name
	 */
	ಆದರೆ (hbitmap == ಖಾಲಿ
		&& (gui_ಕಂಡುಹಿಡಿ_bitmap(
# ifdef FEAT_MULTI_LANG
			    menu->en_dname != ಖಾಲಿ ? menu->en_dname :
# endif
					menu->dname, fname, "bmp") == OK))
	    hbitmap = ತುಂಬು_Image(
		    ಖಾಲಿ,
		    (LPCSTR)fname,
		    IMAGE_BITMAP,
		    TOOLBAR_BUTTON_ಅಗಲ,
		    TOOLBAR_BUTTON_ಎತ್ತರ,
		    LR_LOADFROMFILE |
		    LR_LOADMAP3DCOLORS
		);

	ಆದರೆ (hbitmap != ಖಾಲಿ)
	{
	    TBADDBITMAP tb_ಕೂಡಿಸು_Bitmap;

	    tb_ಕೂಡಿಸು_Bitmap.hInst = ಖಾಲಿ;
	    tb_ಕೂಡಿಸು_Bitmap.nID = (long_u)hbitmap;

	    i = (ಪೂರ್ಣಾಂಕ)Send_ಸಂದೇಶ(s_toolbarhwnd, TB_ADDBITMAP,
			    (WPARAM)1, (LPARAM)&tb_ಕೂಡಿಸು_Bitmap);
	    // i will be set to -1 if it fails
	}
    }
    ಆದರೆ (i == -1 && menu->iconidx >= 0 && menu->iconidx < TOOLBAR_BITMAP_ಎಣಿಕೆ)
	i = menu->iconidx;

    ಹಿಂತಿರುಗಿ i;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಶೂನ್ಯ
initialise_tabline(ಶೂನ್ಯ)
{
    InitCommonControls();

    s_tabhwnd = Create_ಕಿಟಕಿ(WC_TABCONTROL, "Vim tabline",
	    WS_ಮಗು|TCS_FOCUSNEVER|TCS_TOOLTIPS,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, s_hwnd, ಖಾಲಿ, g_hinst, ಖಾಲಿ);
    s_tabline_wndproc = Subclass_ಕಿಟಕಿ(s_tabhwnd, tabline_wndproc);

    gui.tabline_ಎತ್ತರ = TABLINE_ಎತ್ತರ;

    ಗಣ_tabline_font();
}

/*
 * Get tabpage_T from POINT.
 */
    ಸ್ಥಿರ tabpage_T *
Get_Tab_From_ಬಿಂದು(
    HWND    hWnd,
    ಬಿಂದು_2   pt)
{
    tabpage_T	*ptp = ಖಾಲಿ;

    ಆದರೆ (gui_mch_showing_tabline())
    {
	TCHITTESTINFO htinfo;
	htinfo.pt = pt;
	// ignore if a window under cursor is not tabcontrol.
	ಆದರೆ (s_tabhwnd == hWnd)
	{
	    ಪೂರ್ಣಾಂಕ idx = Tab_Ctrl_ದೊರಕಿತು_Test(s_tabhwnd, &htinfo);
	    ಆದರೆ (idx != -1)
		ptp = ಕಂಡುಹಿಡಿ_tabpage(idx + 1);
	}
    }
    ಹಿಂತಿರುಗಿ ptp;
}

ಸ್ಥಿರ ಬಿಂದು_2	    s_pt = {0, 0};
ಸ್ಥಿರ HCURSOR      s_hCursor = ಖಾಲಿ;

    ಸ್ಥಿರ LRESULT CALLBACK
tabline_wndproc(
    HWND hwnd,
    UINT u_ಸಂದೇಶ,
    WPARAM wParam,
    LPARAM lParam)
{
    ಬಿಂದು_2	pt;
    tabpage_T	*tp;
    RECT	rect;
    ಪೂರ್ಣಾಂಕ		nCenter;
    ಪೂರ್ಣಾಂಕ		idx0;
    ಪೂರ್ಣಾಂಕ		idx1;

    HandleMouseHide(u_ಸಂದೇಶ, lParam);

    ಆಯ್ಕೆ (u_ಸಂದೇಶ)
    {
	ಸ್ಥಿತಿ WM_LBUTTONDOWN:
	    {
		s_pt.x = GET_X_LPARAM(lParam);
		s_pt.y = GET_Y_LPARAM(lParam);
		ಗಣ_Capture(hwnd);
		s_hCursor = GetCursor(); // backup default cursor
		ನಿಲ್ಲಿಸು;
	    }
	ಸ್ಥಿತಿ WM_MOUSEMOVE:
	    ಆದರೆ (GetCapture() == hwnd
		    && ((wParam & MK_LBUTTON)) != 0)
	    {
		pt.x = GET_X_LPARAM(lParam);
		pt.y = s_pt.y;
		ಆದರೆ (ನಿರಪೇಕ್ಷ(pt.x - s_pt.x) >
			pGetSystemMetricsForDpi(SM_CXDRAG, s_dpi))
		{
		    ಗಣ_Cursor(ತುಂಬು_Cursor(ಖಾಲಿ, IDC_SIZEWE));

		    tp = Get_Tab_From_ಬಿಂದು(hwnd, pt);
		    ಆದರೆ (tp != ಖಾಲಿ)
		    {
			idx0 = tabpage_ಸೂಚ್ಯಂಕ(curtab) - 1;
			idx1 = tabpage_ಸೂಚ್ಯಂಕ(tp) - 1;

			Tab_Ctrl_Get_ಅಂಶ_Rect(hwnd, idx1, &rect);
			nCenter = rect.ಎಡ_2 + (rect.ಬಲ_2 - rect.ಎಡ_2) / 2;

			// Check if the mouse cursor goes over the center of
			// the next tab to prevent "flickering".
			ಆದರೆ ((idx0 < idx1) && (nCenter < pt.x))
			{
			    tabpage_move(idx1 + 1);
			    update_screen(0);
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((idx1 < idx0) && (pt.x < nCenter))
			{
			    tabpage_move(idx1);
			    update_screen(0);
			}
		    }
		}
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WM_LBUTTONUP:
	    {
		ಆದರೆ (GetCapture() == hwnd)
		{
		    ಗಣ_Cursor(s_hCursor);
		    ReleaseCapture();
		}
		ನಿಲ್ಲಿಸು;
	    }
	ಸ್ಥಿತಿ WM_MBUTTONUP:
	    {
		TCHITTESTINFO htinfo;

		htinfo.pt.x = GET_X_LPARAM(lParam);
		htinfo.pt.y = GET_Y_LPARAM(lParam);
		idx0 = Tab_Ctrl_ದೊರಕಿತು_Test(hwnd, &htinfo);
		ಆದರೆ (idx0 != -1)
		{
		    idx0 += 1;
		    send_tabline_menu_event(idx0, TABLINE_MENU_CLOSE);
		}
		ನಿಲ್ಲಿಸು;
	    }
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ Call_ಕಿಟಕಿ_Proc(s_tabline_wndproc, hwnd, u_ಸಂದೇಶ, wParam, lParam);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_OLE) || defined(FEAT_EVAL)
/*
 * Make the GUI window come to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    ಆದರೆ (IsIconic(s_hwnd))
	 Send_ಸಂದೇಶ(s_hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
    ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MBYTE_IME) && defined(DYNAMIC_IME)
    ಸ್ಥಿರ ಶೂನ್ಯ
dyn_imm_ತುಂಬು(ಶೂನ್ಯ)
{
    hLibImm = vim_ತುಂಬು_Lib("imm32.dll");
    ಆದರೆ (hLibImm == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    pImmGetCompositionStringW
	    = (LONG (WINAPI *)(HIMC, DWORD, LPVOID, DWORD))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmGetCompositionStringW");
    pImmGetContext
	    = (HIMC (WINAPI *)(HWND))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmGetContext");
    pImmAssociateContext
	    = (HIMC (WINAPI *)(HWND, HIMC))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmAssociateContext");
    pImmReleaseContext
	    = (BOOL (WINAPI *)(HWND, HIMC))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmReleaseContext");
    p_Imm_Get_Open_ಸ್ಥಿತಿ
	    = (BOOL (WINAPI *)(HIMC))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmGetOpenStatus");
    p_Imm_ಗಣ_Open_ಸ್ಥಿತಿ
	    = (BOOL (WINAPI *)(HIMC, BOOL))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmSetOpenStatus");
    pImmGetCompositionFontW
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmGetCompositionFontW");
    p_Imm_ಗಣ_Composition_Font_W
	    = (BOOL (WINAPI *)(HIMC, LPLOGFONTW))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmSetCompositionFontW");
    p_Imm_ಗಣ_Composition_ಕಿಟಕಿ
	    = (BOOL (WINAPI *)(HIMC, LPCOMPOSITIONFORM))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmSetCompositionWindow");
    p_Imm_Get_Conversion_ಸ್ಥಿತಿ
	    = (BOOL (WINAPI *)(HIMC, LPDWORD, LPDWORD))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmGetConversionStatus");
    p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ
	    = (BOOL (WINAPI *)(HIMC, DWORD, DWORD))Get_Proc_ವಿಳಾಸ(hLibImm, "ImmSetConversionStatus");

    ಆದರೆ (       pImmGetCompositionStringW == ಖಾಲಿ
	    || pImmGetContext == ಖಾಲಿ
	    || pImmAssociateContext == ಖಾಲಿ
	    || pImmReleaseContext == ಖಾಲಿ
	    || p_Imm_Get_Open_ಸ್ಥಿತಿ == ಖಾಲಿ
	    || p_Imm_ಗಣ_Open_ಸ್ಥಿತಿ == ಖಾಲಿ
	    || pImmGetCompositionFontW == ಖಾಲಿ
	    || p_Imm_ಗಣ_Composition_Font_W == ಖಾಲಿ
	    || p_Imm_ಗಣ_Composition_ಕಿಟಕಿ == ಖಾಲಿ
	    || p_Imm_Get_Conversion_ಸ್ಥಿತಿ == ಖಾಲಿ
	    || p_Imm_ಗಣ_Conversion_ಸ್ಥಿತಿ == ಖಾಲಿ)
    {
	FreeLibrary(hLibImm);
	hLibImm = ಖಾಲಿ;
	pImmGetContext = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    ಹಿಂತಿರುಗಿ;
}

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SIGN_ICONS)

# ifdef FEAT_XPM_W32
#  define IMAGE_XPM   100
# endif

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ _signicon_t
{
    HANDLE	hImage;
    UINT	uType;
# ifdef FEAT_XPM_W32
    HANDLE	hShape;	// Mask bitmap handle
# endif
} signicon_t;

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ typenr)
{
    signicon_t *sign;
    ಪೂರ್ಣಾಂಕ x, y, w, h;

    ಆದರೆ (!gui.ಒಳ_use || (sign = (signicon_t *)sign_get_image(typenr)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    x = ಪಠ್ಯ_X(ಲಂಬಸಾಲು);
    y = ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು);
    w = gui.char_ಅಗಲ * 2;
    h = gui.char_ಎತ್ತರ;
    ಆಯ್ಕೆ (sign->uType)
    {
	ಸ್ಥಿತಿ IMAGE_BITMAP:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hImage);
		ಬಿಟ್_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCCOPY);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ IMAGE_ICON:
	ಸ್ಥಿತಿ IMAGE_CURSOR:
	    DrawIconEx(s_hdc, x, y, (HICON)sign->hImage, w, h, 0, ಖಾಲಿ, DI_ಸಾಮಾನ್ಯ);
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_XPM_W32
	ಸ್ಥಿತಿ IMAGE_XPM:
	    {
		HDC hdcMem;
		HBITMAP hbmpOld;

		hdcMem = CreateCompatibleDC(s_hdc);
		hbmpOld = (HBITMAP)SelectObject(hdcMem, sign->hShape);
		// Make hole
		ಬಿಟ್_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCAND);

		SelectObject(hdcMem, sign->hImage);
		// Paint sign
		ಬಿಟ್_Blt(s_hdc, x, y, w, h, hdcMem, 0, 0, SRCPAINT);
		SelectObject(hdcMem, hbmpOld);
		DeleteDC(hdcMem);
	    }
	    ನಿಲ್ಲಿಸು;
# endif
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
close_signicon_image(signicon_t *sign)
{
    ಆದರೆ (sign == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಯ್ಕೆ (sign->uType)
    {
	ಸ್ಥಿತಿ IMAGE_BITMAP:
	    DeleteObject((HGDIOBJ)sign->hImage);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ IMAGE_CURSOR:
	    DestroyCursor((HCURSOR)sign->hImage);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ IMAGE_ICON:
	    DestroyIcon((HICON)sign->hImage);
	    ನಿಲ್ಲಿಸು;
# ifdef FEAT_XPM_W32
	ಸ್ಥಿತಿ IMAGE_XPM:
	    DeleteObject((HBITMAP)sign->hImage);
	    DeleteObject((HBITMAP)sign->hShape);
	    ನಿಲ್ಲಿಸು;
# endif
    }
}

    ಶೂನ್ಯ *
gui_mch_ರಿಜಿಸ್ಟರ್_sign(char_u *signfile)
{
    signicon_t	sign, *psign;
    char_u	*ext;

    sign.hImage = ಖಾಲಿ;
    ext = signfile + STRLEN(signfile) - 4; // get extension
    ಆದರೆ (ext > signfile)
    {
	ಪೂರ್ಣಾಂಕ do_ತುಂಬು = 1;

	ಆದರೆ (STRICMP(ext, ".bmp") == 0)
	    sign.uType =  IMAGE_BITMAP;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(ext, ".ico") == 0)
	    sign.uType =  IMAGE_ICON;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(ext, ".cur") == 0 || STRICMP(ext, ".ani") == 0)
	    sign.uType =  IMAGE_CURSOR;
	ಇಲ್ಲದಿದ್ದರೆ
	    do_ತುಂಬು = 0;

	ಆದರೆ (do_ತುಂಬು)
	    sign.hImage = (HANDLE)ತುಂಬು_Image(ಖಾಲಿ, (LPCSTR)signfile, sign.uType,
		    gui.char_ಅಗಲ * 2, gui.char_ಎತ್ತರ,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION);
# ifdef FEAT_XPM_W32
	ಆದರೆ (STRICMP(ext, ".xpm") == 0)
	{
	    sign.uType = IMAGE_XPM;
	    ತುಂಬು_Xpm_Image((ಅಕ್ಷರ *)signfile, (HBITMAP *)&sign.hImage,
		    (HBITMAP *)&sign.hShape);
	}
# endif
    }

    psign = ಖಾಲಿ;
    ಆದರೆ (sign.hImage && (psign = ALLOC_ONE(signicon_t)) != ಖಾಲಿ)
	*psign = sign;

    ಆದರೆ (!psign)
    {
	ಆದರೆ (sign.hImage)
	    close_signicon_image(&sign);
	emsg(_(e_couldnt_read_ಒಳ_sign_ದತ್ತಾಂಶ));
    }
    ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)psign;

}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    ಆದರೆ (sign == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    close_signicon_image((signicon_t *)sign);
    vim_free(sign);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_GUI)

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
make_tooltip(BalloonEval *beval, ಅಕ್ಷರ *ಪಠ್ಯ, ಬಿಂದು_2 pt)
{
    TOOLINFOW	*pti;
    RECT	rect;

    pti = ALLOC_ONE(TOOLINFOW);
    ಆದರೆ (pti == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    beval->balloon = Create_ಕಿಟಕಿ_Ex_W(WS_EX_TOPMOST, TOOLTIPS_CLASSW,
	    ಖಾಲಿ, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    beval->ಗುರಿ, ಖಾಲಿ, g_hinst, ಖಾಲಿ);

    ಗಣ_ಕಿಟಕಿ_Pos(beval->balloon, HWND_TOPMOST, 0, 0, 0, 0,
	    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    pti->cb_ಗಾತ್ರ = ಗಾತ್ರ(TOOLINFOW);
    pti->uFlags = TTF_SUBCLASS;
    pti->hwnd = beval->ಗುರಿ;
    pti->hinst = 0; // Don't use string resources
    pti->uId = ID_BEVAL_TOOLTIP;

    pti->lpsz_ಪಠ್ಯ = LPSTR_TEXTCALLBACKW;
    beval->tofree = enc_ಗಮ್ಯ_utf16((char_u*)ಪಠ್ಯ, ಖಾಲಿ);
    pti->lParam = (LPARAM)beval->tofree;
    // switch multiline tooltips on
    ಆದರೆ (GetClientRect(s_ಪಠ್ಯ_Area, &rect))
	Send_ಸಂದೇಶ_W(beval->balloon, TTM_SETMAXTIPWIDTH, 0,
		(LPARAM)rect.ಬಲ_2);

    // Limit ballooneval bounding rect to CursorPos neighbourhood.
    pti->rect.ಎಡ_2 = pt.x - 3;
    pti->rect.ತುದಿ = pt.y - 3;
    pti->rect.ಬಲ_2 = pt.x + 3;
    pti->rect.bottom = pt.y + 3;

    Send_ಸಂದೇಶ_W(beval->balloon, TTM_ADDTOOLW, 0, (LPARAM)pti);
    // Make tooltip appear sooner.
    Send_ಸಂದೇಶ_W(beval->balloon, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
    // I've performed some tests and it seems the longest possible life time
    // of tooltip is 30 seconds.
    Send_ಸಂದೇಶ_W(beval->balloon, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30000);
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
    Post_ಸಂದೇಶ(beval->balloon, WM_CLOSE, 0, 0);
}

    ಸ್ಥಿರ VOID CALLBACK
beval_timer_proc(
    HWND	hwnd ಬಳಸದ,
    UINT	u_ಸಂದೇಶ ಬಳಸದ,
    UINT_PTR	idEvent ಬಳಸದ,
    DWORD	dw_ಸಮಯ)
{
    ಬಿಂದು_2	pt;
    RECT	rect;

    ಆದರೆ (cur_beval == ಖಾಲಿ || cur_beval->showState == ShS_SHOWING || !p_beval)
	ಹಿಂತಿರುಗಿ;

    GetCursorPos(&pt);
    ಆದರೆ (ಕಿಟಕಿ_From_ಬಿಂದು(pt) != s_ಪಠ್ಯ_Area)
	ಹಿಂತಿರುಗಿ;

    Screen_ಗಮ್ಯ_Client(s_ಪಠ್ಯ_Area, &pt);
    GetClientRect(s_ಪಠ್ಯ_Area, &rect);
    ಆದರೆ (!Pt_ಒಳ_Rect(&rect, pt))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಕೊನೆಯ_user_activity > 0
	    && (dw_ಸಮಯ - ಕೊನೆಯ_user_activity) >= (DWORD)p_bdlay
	    && (cur_beval->showState != ShS_PENDING
		|| ನಿರಪೇಕ್ಷ(cur_beval->x - pt.x) > 3
		|| ನಿರಪೇಕ್ಷ(cur_beval->y - pt.y) > 3))
    {
	// Pointer resting in one place long enough, it's time to show
	// the tooltip.
	cur_beval->showState = ShS_PENDING;
	cur_beval->x = pt.x;
	cur_beval->y = pt.y;

	ಆದರೆ (cur_beval->ಸಂದೇಶ_CB != ಖಾಲಿ)
	    (*cur_beval->ಸಂದೇಶ_CB)(cur_beval, 0);
    }
}

    ಶೂನ್ಯ
gui_mch_disable_beval_area(BalloonEval *beval ಬಳಸದ)
{
    KillTimer(s_ಪಠ್ಯ_Area, beval_timer_id);
}

    ಶೂನ್ಯ
gui_mch_enable_beval_area(BalloonEval *beval)
{
    ಆದರೆ (beval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    beval_timer_id = ಗಣ_Timer(s_ಪಠ್ಯ_Area, 0, (UINT)(p_bdlay / 2),
							     beval_timer_proc);
}

    ಶೂನ್ಯ
gui_mch_post_balloon(BalloonEval *beval, char_u *mesg)
{
    ಬಿಂದು_2   pt;

    vim_free(beval->ಸಂದೇಶ);
    beval->ಸಂದೇಶ = mesg == ಖಾಲಿ ? ಖಾಲಿ : vim_strsave(mesg);
    ಆದರೆ (beval->ಸಂದೇಶ == ಖಾಲಿ)
    {
	delete_tooltip(beval);
	beval->showState = ShS_NEUTRAL;
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (beval->showState == ShS_SHOWING)
	ಹಿಂತಿರುಗಿ;
    GetCursorPos(&pt);
    Screen_ಗಮ್ಯ_Client(s_ಪಠ್ಯ_Area, &pt);

    ಆದರೆ (ನಿರಪೇಕ್ಷ(beval->x - pt.x) < 3 && ನಿರಪೇಕ್ಷ(beval->y - pt.y) < 3)
    {
	// cursor is still here
	gui_mch_disable_beval_area(cur_beval);
	beval->showState = ShS_SHOWING;
	make_tooltip(beval, (ಅಕ್ಷರ *)mesg, pt);
    }
}

    BalloonEval *
gui_mch_create_beval_area(
    ಶೂನ್ಯ	*ಗುರಿ ಬಳಸದ,	// ignored, always use s_textArea
    char_u	*mesg,
    ಶೂನ್ಯ	(*mesgCB)(BalloonEval *, ಪೂರ್ಣಾಂಕ),
    ಶೂನ್ಯ	*client_ದತ್ತಾಂಶ)
{
    // partially stolen from gui_beval.c
    BalloonEval	*beval;

    ಆದರೆ (mesg != ಖಾಲಿ && mesgCB != ಖಾಲಿ)
    {
	iemsg(e_cannot_create_ballooneval_with_both_ಸಂದೇಶ_and_callback);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    beval = ALLOC_CLEAR_ONE(BalloonEval);
    ಆದರೆ (beval != ಖಾಲಿ)
    {
	beval->ಗುರಿ = s_ಪಠ್ಯ_Area;

	beval->showState = ShS_NEUTRAL;
	beval->ಸಂದೇಶ = mesg;
	beval->ಸಂದೇಶ_CB = mesgCB;
	beval->client_ದತ್ತಾಂಶ = client_ದತ್ತಾಂಶ;

	InitCommonControls();
	cur_beval = beval;

	ಆದರೆ (p_beval)
	    gui_mch_enable_beval_area(beval);
    }
    ಹಿಂತಿರುಗಿ beval;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Handle_WM_Notify(HWND hwnd ಬಳಸದ, LPNMHDR pnmh)
{
    ಆದರೆ (pnmh->idFrom != ID_BEVAL_TOOLTIP) // it is not our tooltip
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (cur_beval == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆಯ್ಕೆ (pnmh->ಸಂಕೇತ_2)
    {
	ಸ್ಥಿತಿ TTN_SHOW:
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TTN_ತೆಗೆ: // Before tooltip disappear
	    delete_tooltip(cur_beval);
	    gui_mch_enable_beval_area(cur_beval);

	    cur_beval->showState = ShS_NEUTRAL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TTN_GETDISPINFO:
	    {
		// if you get there then we have new common controls
		NMTTDISPINFO *info = (NMTTDISPINFO *)pnmh;
		info->lpsz_ಪಠ್ಯ = (LPSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TTN_GETDISPINFOW:
	    {
		// if we get here then we have new common controls
		NMTTDISPINFOW *info = (NMTTDISPINFOW *)pnmh;
		info->lpsz_ಪಠ್ಯ = (LPWSTR)info->lParam;
		info->uFlags |= TTF_DI_SETITEM;
	    }
	    ನಿಲ್ಲಿಸು;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
track_user_activity(UINT u_ಸಂದೇಶ)
{
    ಆದರೆ ((u_ಸಂದೇಶ >= WM_MOUSEFIRST && u_ಸಂದೇಶ <= WM_MOUSELAST)
	    || (u_ಸಂದೇಶ >= WM_KEYFIRST && u_ಸಂದೇಶ <= WM_KEYLAST))
	ಕೊನೆಯ_user_activity = Get_Tick_ಎಣಿಕೆ();
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
#ಆದರೆ_ಅಂತ್ಯ // FEAT_BEVAL_GUI

#ಆದರೆ defined(FEAT_NETBEANS_INTG)
/*
 * We have multiple signs to draw at the same location. Draw the
 * multi-sign indicator (down-arrow) instead. This is the Win32 version.
 */
    ಶೂನ್ಯ
netbeans_draw_multisign_indicator(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ y;
    ಪೂರ್ಣಾಂಕ x;

    ಆದರೆ (!netbeans_ಸಕ್ರಿಯ())
	ಹಿಂತಿರುಗಿ;

    x = 0;
    y = ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು);

# ಆದರೆ defined(FEAT_DIRECTX)
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ಗಾಗಿ (i = 0; i < gui.char_ಎತ್ತರ - 3; i++)
	ಗಣ_Pixel(s_hdc, x+2, y++, gui.currFgColor);

    ಗಣ_Pixel(s_hdc, x+0, y, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+4, y++, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+1, y, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+2, y, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+3, y++, gui.currFgColor);
    ಗಣ_Pixel(s_hdc, x+2, y, gui.currFgColor);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)

// TODO: at the moment, this is just a copy of test_gui_mouse_event.
// But, we could instead generate actual Win32 mouse event messages,
// ie. to make it consistent with test_gui_w32_sendevent_keyboard.
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_w32_sendevent_mouse(dict_T *ವಾದಗಳು)
{
    ಆದರೆ (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "row") || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "col"))
	ಹಿಂತಿರುಗಿ FALSE;

    // Note: "move" is optional, requires fewer arguments
    ಪೂರ್ಣಾಂಕ move = (ಪೂರ್ಣಾಂಕ)dict_get_bool(ವಾದಗಳು, "move", FALSE);

    ಆದರೆ (!move && (!dict_has_ಕೀಲಿ(ವಾದಗಳು, "button")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "multiclick")
	    || !dict_has_ಕೀಲಿ(ವಾದಗಳು, "modifiers")))
	ಹಿಂತಿರುಗಿ FALSE;

    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "row");
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "col");

    ಆದರೆ (move)
    {
	// the "move" argument expects row and col coordnates to be in pixels,
	// unless "cell" is specified and is TRUE.
	ಆದರೆ (dict_get_bool(ವಾದಗಳು, "cell", FALSE))
	{
	    // calculate the middle of the character cell
	    // Note: Cell coordinates are 1-based from Vim script
	    ಪೂರ್ಣಾಂಕ pY = (ಅಡ್ಡಸಾಲು - 1) * gui.char_ಎತ್ತರ + gui.char_ಎತ್ತರ / 2;
	    ಪೂರ್ಣಾಂಕ pX = (ಲಂಬಸಾಲು - 1) * gui.char_ಅಗಲ + gui.char_ಅಗಲ / 2;
	    gui_mouse_moved(pX, pY);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    gui_mouse_moved(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ button = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "button");
	ಪೂರ್ಣಾಂಕ repeated_click = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "multiclick");
	int_u mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");

	// Reset the scroll values to known values.
	// XXX: Remove this when/if the scroll step is made configurable.
	mouse_ಗಣ_hor_scroll_ಹೆಜ್ಜೆ(6);
	mouse_ಗಣ_vert_scroll_ಹೆಜ್ಜೆ(3);

	gui_send_mouse_event(button, ಪಠ್ಯ_X(ಲಂಬಸಾಲು - 1), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು - 1),
							repeated_click, mods);
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_w32_sendevent_keyboard(dict_T *ವಾದಗಳು)
{
    ಒಳಹರಿವು_2 inputs[1];
    ಒಳಹರಿವು_2 modkeys[3];
    Secure_Zero_ಸ್ಮರಣೆ(inputs, ಗಾತ್ರ(ಒಳಹರಿವು_2));
    Secure_Zero_ಸ್ಮರಣೆ(modkeys, 3 * ಗಾತ್ರ(ಒಳಹರಿವು_2));

    char_u *event = dict_get_string(ವಾದಗಳು, "event", TRUE);

    ಆದರೆ (event && (STRICMP(event, "keydown") == 0
				      || STRICMP(event, "keyup") == 0))
    {
	ಶಬ್ದ_2 vk_ಸಂಕೇತ = dict_get_ಸಂಖ್ಯೆ_def(ವಾದಗಳು, "keycode", 0);
	ಆದರೆ (vk_ಸಂಕೇತ <= 0 || vk_ಸಂಕೇತ >= 0xFF)
	{
	    semsg(_(e_invalid_ವಾದ_nr), (ದೀರ್ಘ)vk_ಸಂಕೇತ);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	BOOL is_Mod_ಕೀಲಿ = (vk_ಸಂಕೇತ == VK_ಸರಿಸು || vk_ಸಂಕೇತ == VK_CONTROL
	    || vk_ಸಂಕೇತ == VK_MENU || vk_ಸಂಕೇತ == VK_LSHIFT || vk_ಸಂಕೇತ == VK_RSHIFT
	    || vk_ಸಂಕೇತ == VK_LCONTROL || vk_ಸಂಕೇತ == VK_RCONTROL
	    || vk_ಸಂಕೇತ == VK_LMENU || vk_ಸಂಕೇತ == VK_RMENU );

	BOOL unwrapMods = FALSE;
	ಪೂರ್ಣಾಂಕ mods = (ಪೂರ್ಣಾಂಕ)dict_get_ಸಂಖ್ಯೆ(ವಾದಗಳು, "modifiers");

	// If there are modifiers in the args, and it is not a keyup event and
	// vkCode is not a modifier key, then we generate virtual modifier key
	// messages before sending the actual key message.
	ಆದರೆ (mods && STRICMP(event, "keydown") == 0 && !is_Mod_ಕೀಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ n = 0;
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ಸರಿಸು)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LSHIFT;
		n++;
	    }
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_CTRL)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LCONTROL;
		n++;
	    }
	    ಆದರೆ (mods & MOD_ಮುಖವಾಡ_ALT)
	    {
		modkeys[n].type = ಒಳಹರಿವು_KEYBOARD;
		modkeys[n].ki.wVk = VK_LMENU;
		n++;
	    }
	    ಆದರೆ (n)
	    {
		(ಶೂನ್ಯ)ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
		Send_ಒಳಹರಿವು(n, modkeys, ಗಾತ್ರ(ಒಳಹರಿವು_2));
	    }
	}

	inputs[0].type = ಒಳಹರಿವು_KEYBOARD;
	inputs[0].ki.wVk = vk_ಸಂಕೇತ;
	ಆದರೆ (STRICMP(event, "keyup") == 0)
	{
	    inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
	    ಆದರೆ (!is_Mod_ಕೀಲಿ)
		unwrapMods = TRUE;
	}

	(ಶೂನ್ಯ)ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
	Send_ಒಳಹರಿವು(ARRAYSIZE(inputs), inputs, ಗಾತ್ರ(ಒಳಹರಿವು_2));
	vim_free(event);

	ಆದರೆ (unwrapMods)
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

	    (ಶೂನ್ಯ)ಗಣ_Foreground_ಕಿಟಕಿ(s_hwnd);
	    Send_ಒಳಹರಿವು(3, modkeys, ಗಾತ್ರ(ಒಳಹರಿವು_2));
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (event == ಖಾಲಿ)
	{
	    semsg(_(e_missing_ವಾದ_str), "event");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "event", event);
	    vim_free(event);
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_gui_w32_sendevent_ಗಣ_keycode_trans_strategy(dict_T *ವಾದಗಳು)
{
    ಪೂರ್ಣಾಂಕ handled = 0;
    char_u *strategy = dict_get_string(ವಾದಗಳು, "strategy", TRUE);

    ಆದರೆ (strategy)
    {
	ಆದರೆ (STRICMP(strategy, "classic") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_ಬಳಸಿದ = &keycode_trans_strategy_classic;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(strategy, "experimental") == 0)
	{
	    handled = 1;
	    keycode_trans_strategy_ಬಳಸಿದ = &keycode_trans_strategy_experimental;
	}
    }

    ಆದರೆ (!handled)
    {
	ಆದರೆ (strategy == ಖಾಲಿ)
	{
	    semsg(_(e_missing_ವಾದ_str), "strategy");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "strategy", strategy);
	    vim_free(strategy);
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}


    ಪೂರ್ಣಾಂಕ
test_gui_w32_sendevent(char_u *event, dict_T *ವಾದಗಳು)
{
    ಆದರೆ (STRICMP(event, "key") == 0)
	ಹಿಂತಿರುಗಿ test_gui_w32_sendevent_keyboard(ವಾದಗಳು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(event, "mouse") == 0)
	ಹಿಂತಿರುಗಿ test_gui_w32_sendevent_mouse(ವಾದಗಳು);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(event, "set_keycode_trans_strategy") == 0)
	ಹಿಂತಿರುಗಿ test_gui_w32_sendevent_ಗಣ_keycode_trans_strategy(ವಾದಗಳು);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str_str), "event", event);
	ಹಿಂತಿರುಗಿ FALSE;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_IMAGE_GUI)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
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
upload_popup_image_pixels(uint8_t *dst, uint8_t *src, ಪೂರ್ಣಾಂಕ iw, ಪೂರ್ಣಾಂಕ ih, image_format_T fmt)
{
    ಪೂರ್ಣಾಂಕ		y, x;
    ಪೂರ್ಣಾಂಕ		stride = iw * 4;
    ಪೂರ್ಣಾಂಕ		src_bpp = fmt == IMAGE_FORMAT_RGBA ? 4 : 3;
    COLORREF	bg = fmt == IMAGE_FORMAT_RGBA ? gui_mch_get_rgb(gui.ಹಿಂಭಾಗ_pixel) : 0;
    BYTE	bg_r = GetRValue(bg);
    BYTE	bg_g = GetGValue(bg);
    BYTE	bg_b = GetBValue(bg);

    ಗಾಗಿ (y = 0; y < ih; y++)
    {
	uint8_t *s = src + (size_t)y * iw * src_bpp;
	uint8_t *d = dst + (size_t)y * stride;

	ಗಾಗಿ (x = 0; x < iw; x++)
	{
	    ಆದರೆ (fmt == IMAGE_FORMAT_RGBA)
	    {
		ಪೂರ್ಣಾಂಕ a = s[3];

		ಆದರೆ (a == 255)
		{
		    d[0] = s[2];
		    d[1] = s[1];
		    d[2] = s[0];
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (a == 0)
		{
		    d[0] = (uint8_t )bg_b;
		    d[1] = (uint8_t)bg_g;
		    d[2] = (uint8_t)bg_r;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    d[0] = (uint8_t)((s[2] * a + bg_b * (255 - a)) / 255);
		    d[1] = (uint8_t)((s[1] * a + bg_g * (255 - a)) / 255);
		    d[2] = (uint8_t)((s[0] * a + bg_r * (255 - a)) / 255);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
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

    ಪೂರ್ಣಾಂಕ
image_gui_init(image_T *img)
{
    image_gdi_T *ctx = ALLOC_CLEAR_ONE(image_gdi_T);
    BITMAPINFO	bmi;
    HBITMAP	hbm;
    HDC		mem_dc;
    ಪೂರ್ಣಾಂಕ		iw, ih;
    ಶೂನ್ಯ	*ಬಿಟ್ಟುಗಳು = ಖಾಲಿ;

    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    image_get_dimensions(img, &iw, &ih);

    CLEAR_ಕ್ಷೇತ್ರ(bmi);
    bmi.bmi_ಶೀರ್ಷಿಕೆ.bi_ಗಾತ್ರ = ಗಾತ್ರ(BITMAPINFOHEADER);
    bmi.bmi_ಶೀರ್ಷಿಕೆ.bi_ಅಗಲ = iw;
    bmi.bmi_ಶೀರ್ಷಿಕೆ.bi_ಎತ್ತರ = -ih; // Top-down
    bmi.bmi_ಶೀರ್ಷಿಕೆ.biPlanes = 1;
    bmi.bmi_ಶೀರ್ಷಿಕೆ.bi_ಬಿಟ್_ಎಣಿಕೆ = 32;
    bmi.bmi_ಶೀರ್ಷಿಕೆ.biCompression = BI_RGB;

    hbm = CreateDIBSection(s_hdc, &bmi, DIB_RGB_COLORS, &ಬಿಟ್ಟುಗಳು, ಖಾಲಿ, 0);
    ಆದರೆ (hbm == ಖಾಲಿ || ಬಿಟ್ಟುಗಳು == ಖಾಲಿ)
    {
	ಆದರೆ (hbm != ಖಾಲಿ)
	    DeleteObject(hbm);
	ಹಿಂತಿರುಗಿ false;
    }

    mem_dc = CreateCompatibleDC(s_hdc);
    ಆದರೆ (mem_dc == ಖಾಲಿ)
    {
	DeleteObject(hbm);
	ಹಿಂತಿರುಗಿ false;
    }

    SelectObject(mem_dc, hbm);
    upload_popup_image_pixels((uint8_t *)ಬಿಟ್ಟುಗಳು, img->ದತ್ತಾಂಶ, iw, ih, img->fmt);

    ctx->hbm = hbm;
    ctx->hdc = mem_dc;

    img->backend_ದತ್ತಾಂಶ = ctx;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
image_gui_uninit(image_T *img)
{
    image_gdi_T *ctx = img->backend_ದತ್ತಾಂಶ;

    DeleteObject(ctx->hbm);
    DeleteDC(ctx->hdc);
    vim_free(ctx);
}

    ಪೂರ್ಣಾಂಕ
image_placement_gui_init(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
image_placement_gui_uninit(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
}

    ಶೂನ್ಯ
image_placement_gui_draw(image_placement_T *ಇರಿಸು)
{
    image_T	    *img = ಇರಿಸು->img;
    image_gdi_T	    *ictx = img->backend_ದತ್ತಾಂಶ;
    pixman_box32_t  *rects;
    ಪೂರ್ಣಾಂಕ		    n_rects;

    rects = pixman_region32_rectangles(&ಇರಿಸು->visible, &n_rects);
    ಆದರೆ (rects == ಖಾಲಿ || n_rects == 0)
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ defined(FEAT_DIRECTX)
    // Commit any pending DirectWrite output so popup text and borders are on
    // s_hdc before we blit on top.
    ಆದರೆ (IS_ENABLE_DIRECTX())
	DWriteContext_Flush(s_dwc);
# endif

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ		posx, posy;
	ಪೂರ್ಣಾಂಕ		x, y, w, h;

	image_placement_subrect(ಇರಿಸು, rect, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &x, &y, &w, &h, true);
	posx = FILL_X(ಲಂಬಸಾಲು);
	posy = FILL_Y(ಅಡ್ಡಸಾಲು);

	ಬಿಟ್_Blt(s_hdc, posx, posy, w, h, ictx->hdc, x, y, SRCCOPY);

    }
}

    ಶೂನ್ಯ
image_placement_gui_clear(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_IMAGE_GUI
