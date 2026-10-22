/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * Code for the Motif GUI.
 * Not used for GTK.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <X11/keysym.h>
#ಅನ್ತರ್ಭಾವ <X11/Xatom.h>
#ಅನ್ತರ್ಭಾವ <X11/StringDefs.h>
#ಅನ್ತರ್ಭಾವ <X11/Intrinsic.h>
#ಅನ್ತರ್ಭಾವ <X11/Shell.h>
#ಅನ್ತರ್ಭಾವ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#ಆದರೆ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <X11/xpm.h>
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <locale.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು VIM_NAME	"vim"
#ವ್ಯಾಖ್ಯಾನಿಸು VIM_CLASS	"Vim"

// Default resource values
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_FONT		"7x13"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FONTSET_ALWAYS
# define DFLT_MENU_FONT		Xt_Default_Font_ಗಣ
#ಇಲ್ಲದಿದ್ದರೆ
# define DFLT_MENU_FONT		XtDefaultFont
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_TOOLTIP_FONT	Xt_Default_Font_ಗಣ

// use the default (CDE) colors
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_MENU_BG_COLOR	""
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_MENU_FG_COLOR	""
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_SCROLL_BG_COLOR	""
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_SCROLL_FG_COLOR	""
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

ಸ್ಥಿರ Atom   wm_atoms[2];	// Window Manager Atoms
#ವ್ಯಾಖ್ಯಾನಿಸು DELETE_ಕಿಟಕಿ_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#ವ್ಯಾಖ್ಯಾನಿಸು SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ಸ್ಥಿರ XFont_ಗಣ current_fontset = ಖಾಲಿ;
# ಆದರೆ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	ಮಾಡು \
	{ \
	    ಆದರೆ (current_fontset != ಖಾಲಿ) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    ಇಲ್ಲದಿದ್ದರೆ \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} ಆಗ (0)
# endif
# ಆದರೆ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	ಮಾಡು \
	{ \
	    ಆದರೆ (current_fontset != ಖಾಲಿ) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ಇಲ್ಲದಿದ್ದರೆ \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ಆಗ (0)
# endif
# ಆದರೆ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	ಮಾಡು \
	{ \
	    ಆದರೆ (current_fontset != ಖಾಲಿ) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ಇಲ್ಲದಿದ್ದರೆ \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ಆಗ (0)
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ check_fontset_sanity(XFont_ಗಣ fs);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fontset_ಅಗಲ(XFont_ಗಣ fs);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fontset_ascent(XFont_ಗಣ fs);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ guicolor_T	prev_fg_color = INVALCOLOR;
ಸ್ಥಿರ guicolor_T	prev_bg_color = INVALCOLOR;
ಸ್ಥಿರ guicolor_T	prev_sp_color = INVALCOLOR;

#ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ಸ್ಥಿರ XButtonPressedEvent ಕೊನೆಯ_mouse_event;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_check_copy_area(ಶೂನ್ಯ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_send_event_handler(Widget, Xt_ಸೂಚಕ, XEvent *, Boolean *);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_wm_protocol_handler(Widget, Xt_ಸೂಚಕ, XEvent *, Boolean *);
ಸ್ಥಿರ Cursor gui_x11_create_blank_mouse(ಶೂನ್ಯ);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ಸ್ಥಿರ ರಚನೆ specialkey
{
    ಕೀಲಿ_Sym  ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {XK_ಮೇಲೆ,		'k', 'u'},
    {XK_ಕೆಳಕ್ಕೆ,		'k', 'd'},
    {XK_ಎಡ,		'k', 'l'},
    {XK_ಬಲ,		'k', 'r'},

    {XK_F1,		'k', '1'},
    {XK_F2,		'k', '2'},
    {XK_F3,		'k', '3'},
    {XK_F4,		'k', '4'},
    {XK_F5,		'k', '5'},
    {XK_F6,		'k', '6'},
    {XK_F7,		'k', '7'},
    {XK_F8,		'k', '8'},
    {XK_F9,		'k', '9'},
    {XK_F10,		'k', ';'},

    {XK_F11,		'F', '1'},
    {XK_F12,		'F', '2'},
    {XK_F13,		'F', '3'},
    {XK_F14,		'F', '4'},
    {XK_F15,		'F', '5'},
    {XK_F16,		'F', '6'},
    {XK_F17,		'F', '7'},
    {XK_F18,		'F', '8'},
    {XK_F19,		'F', '9'},
    {XK_F20,		'F', 'A'},

    {XK_F21,		'F', 'B'},
    {XK_F22,		'F', 'C'},
    {XK_F23,		'F', 'D'},
    {XK_F24,		'F', 'E'},
    {XK_F25,		'F', 'F'},
    {XK_F26,		'F', 'G'},
    {XK_F27,		'F', 'H'},
    {XK_F28,		'F', 'I'},
    {XK_F29,		'F', 'J'},
    {XK_F30,		'F', 'K'},

    {XK_F31,		'F', 'L'},
    {XK_F32,		'F', 'M'},
    {XK_F33,		'F', 'N'},
    {XK_F34,		'F', 'O'},
    {XK_F35,		'F', 'P'},	// keysymdef.h defines up to F35
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#ಆದರೆ_ಅಂತ್ಯ

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_ಹಿಂಭಾಗ_Space,	'k', 'b'},
    {XK_ಸೇರಿಸು,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_ಕೊನೆ,		'@', '7'},
    {XK_ಪೂರ್ವ,		'k', 'P'},
    {XK_ಮುಂದಿನ,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ XK_KP_ಎಡ
    {XK_KP_ಎಡ,	'k', 'l'},
    {XK_KP_ಬಲ,	'k', 'r'},
    {XK_KP_ಮೇಲೆ,		'k', 'u'},
    {XK_KP_ಕೆಳಕ್ಕೆ,	'k', 'd'},
    {XK_KP_ಸೇರಿಸು,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_ಕೊನೆ,		'K', '4'},
    {XK_KP_ಪೂರ್ವ,	'K', '3'},
    {XK_KP_ಮುಂದಿನ,	'K', '5'},

    {XK_KP_ಕೂಡಿಸು,		'K', '6'},
    {XK_KP_ಕಳೆ,	'K', '7'},
    {XK_KP_Divide,	'K', '8'},
    {XK_KP_Multiply,	'K', '9'},
    {XK_KP_Enter,	'K', 'A'},
    {XK_KP_Decimal,	'K', 'B'},

    {XK_KP_0,		'K', 'C'},
    {XK_KP_1,		'K', 'D'},
    {XK_KP_2,		'K', 'E'},
    {XK_KP_3,		'K', 'F'},
    {XK_KP_4,		'K', 'G'},
    {XK_KP_5,		'K', 'H'},
    {XK_KP_6,		'K', 'I'},
    {XK_KP_7,		'K', 'J'},
    {XK_KP_8,		'K', 'K'},
    {XK_KP_9,		'K', 'L'},
#ಆದರೆ_ಅಂತ್ಯ

    // End of list marker:
    {(ಕೀಲಿ_Sym)0,	    0, 0}
};

#ವ್ಯಾಖ್ಯಾನಿಸು XtNboldFont		"boldFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCBoldFont		"BoldFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNitalicFont		"italicFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCItalicFont		"ItalicFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNboldItalicFont	"boldItalicFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCBoldItalicFont	"BoldItalicFont"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Nscrollbar_ಅಗಲ	"scrollbarWidth"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_CScrollbar_ಅಗಲ	"ScrollbarWidth"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Nmenu_ಎತ್ತರ		"menuHeight"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_CMenu_ಎತ್ತರ		"MenuHeight"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNmenuFont		"menuFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCMenuFont		"MenuFont"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_Nmenu_Font_ಗಣ		"menuFontSet"
#ವ್ಯಾಖ್ಯಾನಿಸು Xt_CMenu_Font_ಗಣ		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#ವ್ಯಾಖ್ಯಾನಿಸು XtNmenuBackground	"menuBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCMenuBackground	"MenuBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNmenuForeground	"menuForeground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#ವ್ಯಾಖ್ಯಾನಿಸು XtNscrollBackground	"scrollBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCScrollBackground	"ScrollBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNscrollForeground	"scrollForeground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#ವ್ಯಾಖ್ಯಾನಿಸು XtNtooltipBackground	"tooltipBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCTooltipBackground	"TooltipBackground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNtooltipForeground	"tooltipForeground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCTooltipForeground	"TooltipForeground"
#ವ್ಯಾಖ್ಯಾನಿಸು XtNtooltipFont		"tooltipFont"
#ವ್ಯಾಖ್ಯಾನಿಸು XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
ಸ್ಥಿರ XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	ಗಾತ್ರ(Pixel),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	ಗಾತ್ರ(Pixel),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, def_ಹಿಂಭಾಗ_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	ಗಾತ್ರ(String *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	ಗಾತ್ರ(String *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	ಗಾತ್ರ(String *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	ಗಾತ್ರ(String *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	ಗಾತ್ರ(String *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	ಗಾತ್ರ(Bool),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_rev_video),
	XtRImmediate,
	(Xt_ಸೂಚಕ)False
    },
    {
	Xt_Nborder_ಅಗಲ,
	Xt_CBorder_ಅಗಲ,
	XtRInt,
	ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, border_ಅಗಲ),
	XtRImmediate,
	(Xt_ಸೂಚಕ)2
    },
    {
	Xt_Nscrollbar_ಅಗಲ,
	Xt_CScrollbar_ಅಗಲ,
	XtRInt,
	ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, scrollbar_ಅಗಲ),
	XtRImmediate,
	(Xt_ಸೂಚಕ)SB_DEFAULT_ಅಗಲ
    },
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	Xt_Nmenu_Font_ಗಣ,
	Xt_CMenu_Font_ಗಣ,
# ಇಲ್ಲದಿದ್ದರೆ
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#ಆದರೆ_ಅಂತ್ಯ
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ *),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	Xt_CFont_ಗಣ,
	Xt_RFont_ಗಣ,
	ಗಾತ್ರ(XFont_ಗಣ),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, tooltip_fontset),
	XtRImmediate,
	(Xt_ಸೂಚಕ)NOFONTSET
    },
#ಆದರೆ_ಅಂತ್ಯ // FEAT_BEVAL_GUI
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ*),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_preedit_type_name),
	XtRString,
	(Xt_ಸೂಚಕ)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	ಗಾತ್ರ(ಅಕ್ಷರ*),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(gui_T, rsrc_ಒಳಹರಿವು_ವಿಧಾನ),
	XtRString,
	ಖಾಲಿ
    },
#ಆದರೆ_ಅಂತ್ಯ // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
ಸ್ಥಿರ XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-background",	".background",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-fn",		".font",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-font",		".font",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ಖಾಲಿ},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ಖಾಲಿ},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ಖಾಲಿ},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ಖಾಲಿ},
#ಇಲ್ಲದಿದ್ದರೆ
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ಖಾಲಿ},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ಖಾಲಿ},
#ಆದರೆ_ಅಂತ್ಯ
    {"-xrm",		ಖಾಲಿ,		    XrmoptionResArg,	ಖಾಲಿ}
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gui_argc = 0;
ಸ್ಥಿರ ಅಕ್ಷರ **gui_argv = ಖಾಲಿ;

/*
 * Call-back routines.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_timer_cb(
    Xt_ಸೂಚಕ	    timed_ಹೊರಹೋಗು,
    XtIntervalId    *interval_id ಬಳಸದ)
{
    *((ಪೂರ್ಣಾಂಕ *)timed_ಹೊರಹೋಗು) = TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_poll_cb(
    Xt_ಸೂಚಕ	    client_ದತ್ತಾಂಶ_2,
    XtIntervalId    *interval_id ಬಳಸದ)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_ದತ್ತಾಂಶ_2;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, (long_u)20,
						 channel_poll_cb, client_ದತ್ತಾಂಶ_2);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_visibility_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	dud ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    ಆದರೆ (event->type != VisibilityNotify)
	ಹಿಂತಿರುಗಿ;

    gui.visibility = event->xvisibility.state;

    /*
     * When we do an XCopyArea(), and the window is partially obscured, we want
     * to receive an event to tell us whether it worked or not.
     */
    XSetGraphicsExposures(gui.dpy, gui.ಪಠ್ಯ_gc,
	    gui.visibility != VisibilityUnobscured);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_expose_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	dud ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    XExposeEvent	*gevent;
    ಪೂರ್ಣಾಂಕ			new_x;

    ಆದರೆ (event->type != Expose)
	ಹಿಂತಿರುಗಿ;

    ಹೊರಹೋಗು_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->ಅಗಲ, gevent->ಎತ್ತರ);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    ಆದರೆ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    ಆದರೆ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    ಆದರೆ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((ಪೂರ್ಣಾಂಕ)Columns), 0, 0, 0, False);
    ಆದರೆ (gevent->y > FILL_Y(ಅಡ್ಡಸಾಲುಗಳು))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#ಆದರೆ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
/*
 * This function fills in the XRectangle object with the current x,y
 * coordinates and height, width so that an XtVaSetValues to the same shell of
 * those resources will restore the window to its former position and
 * dimensions.
 *
 * Note: This function may fail, in which case the XRectangle will be
 * unchanged.  Be sure to have the XRectangle set with the proper values for a
 * failed condition prior to calling this function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
shellRectangle(Widget shell, XRectangle *r)
{
    ಕಿಟಕಿ		rootw, shellw, ಮಗು, parentw;
    ಪೂರ್ಣಾಂಕ			absx, absy;
    XWindow_ಗುಣಲಕ್ಷಣಗಳು	a;
    ಕಿಟಕಿ		*children;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	children_ಎಣಿಕೆ;

    shellw = Xt_ಕಿಟಕಿ(shell);
    ಆದರೆ (shellw == 0)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (;;)
    {
	XQuery_ವೃಕ್ಷ(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_ಎಣಿಕೆ);
	XFree(children);
	ಆದರೆ (parentw == rootw)
	    ನಿಲ್ಲಿಸು;
	shellw = parentw;
    }
    XGet_ಕಿಟಕಿ_ಗುಣಲಕ್ಷಣಗಳು(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.ಬೇರು, 0, 0,
							&absx, &absy, &ಮಗು);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(shell, XmNheight, &r->ಎತ್ತರ, XmNwidth, &r->ಅಗಲ, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_resize_ಕಿಟಕಿ_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	dud ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಅಗಲ_2, ಕೊನೆಯ_ಎತ್ತರ_2;

    ಆದರೆ (event->type != ConfigureNotify)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (event->xconfigure.ಅಗಲ != ಕೊನೆಯ_ಅಗಲ_2
	    || event->xconfigure.ಎತ್ತರ != ಕೊನೆಯ_ಎತ್ತರ_2)
    {
	ಕೊನೆಯ_ಅಗಲ_2 = event->xconfigure.ಅಗಲ;
	ಕೊನೆಯ_ಎತ್ತರ_2 = event->xconfigure.ಎತ್ತರ;
	gui_resize_shell(event->xconfigure.ಅಗಲ, event->xconfigure.ಎತ್ತರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
						- xim_get_ಸ್ಥಿತಿ_area_ಎತ್ತರ()
#ಆದರೆ_ಅಂತ್ಯ
		     );
    }
#ಆದರೆ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    ಆದರೆ (netbeans_ಸಕ್ರಿಯ())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_ಚೌಕಟ್ಟು_moved(rec.x, rec.y);
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    xim_ಗಣ_preedit();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_focus_change_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	ದತ್ತಾಂಶ ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    gui_focus_change(event->type == Focus_ಒಳ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_enter_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	ದತ್ತಾಂಶ ಬಳಸದ,
    XEvent	*event ಬಳಸದ,
    Boolean	*dum ಬಳಸದ)
{
    gui_focus_change(TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_leave_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	ದತ್ತಾಂಶ ಬಳಸದ,
    XEvent	*event ಬಳಸದ,
    Boolean	*dum ಬಳಸದ)
{
    gui_focus_change(FALSE);
}

#ಆದರೆ defined(X_HAVE_UTF8_STRING)
# ಆದರೆ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_x11_ಕೀಲಿ_ದೊರಕಿತು_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	dud ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    XKeyPressedEvent	*ev_press;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    ಸ್ಥಿತಿ_3		ಸ್ಥಿತಿ_2;
#ಇಲ್ಲದಿದ್ದರೆ
    char_u		string[4], string2[3];
#ಆದರೆ_ಅಂತ್ಯ
    ಕೀಲಿ_Sym		ಕೀಲಿ_sym;
    ಪೂರ್ಣಾಂಕ			len;
    ಪೂರ್ಣಾಂಕ			i;
    ಪೂರ್ಣಾಂಕ			modifiers;
    ಪೂರ್ಣಾಂಕ			ಕೀಲಿ;

    ev_press = (XKeyPressedEvent *)event;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಆದರೆ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	ಆದರೆ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (ಅಕ್ಷರ *)string,
				  ಗಾತ್ರ(string_shortbuf), &ಕೀಲಿ_sym, &ಸ್ಥಿತಿ_2);
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    len = XmbLookupString(xic, ev_press, (ಅಕ್ಷರ *)string,
				  ಗಾತ್ರ(string_shortbuf), &ಕೀಲಿ_sym, &ಸ್ಥಿತಿ_2);
	ಆದರೆ (ಸ್ಥಿತಿ_2 == XBuffer_ಅತಿಪ್ರವಾಹ)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    ಆದರೆ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (ಅಕ್ಷರ *)string,
						      len, &ಕೀಲಿ_sym, &ಸ್ಥಿತಿ_2);
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		len = XmbLookupString(xic, ev_press, (ಅಕ್ಷರ *)string,
						      len, &ಕೀಲಿ_sym, &ಸ್ಥಿತಿ_2);
	}
	ಆದರೆ (ಸ್ಥಿತಿ_2 == XLookupNone || ಸ್ಥಿತಿ_2 == XLookupChars)
	    ಕೀಲಿ_sym = XK_Void_ಚಿಹ್ನೆ;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	ಆದರೆ (len > 0 && ಒಳಹರಿವು_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    ಪೂರ್ಣಾಂಕ		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    ಆದರೆ (string_alloced)
		XtFree((ಅಕ್ಷರ *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_ಒಳಹರಿವು(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ಆದರೆ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		ಆದರೆ (string_alloced)
		    XtFree((ಅಕ್ಷರ *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	len = XLookupString(ev_press, (ಅಕ್ಷರ *)string, ಗಾತ್ರ(string),
		&ಕೀಲಿ_sym, ಖಾಲಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    ಆದರೆ (ಕೀಲಿ_sym == SunXK_F36 || ಕೀಲಿ_sym == SunXK_F37)
	len = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಕೀಲಿ_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    ಆದರೆ (ಕೀಲಿ_sym == XK_minus && (ev_press->state & Control_ಮುಖವಾಡ))
	string[0] = Ctrl__;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ XK_ISO_ಎಡ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    ಆದರೆ (ಕೀಲಿ_sym == XK_ISO_ಎಡ_Tab)
    {
	ಕೀಲಿ_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#ಆದರೆ_ಅಂತ್ಯ

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    ಆದರೆ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ಆದರೆ (len == 0 || len == 1)
    {
	ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != (ಕೀಲಿ_Sym)0; i++)
	{
	    ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ಕೀಲಿಗಳು[i].vim_code0;
		string[2] = special_ಕೀಲಿಗಳು[i].vim_code1;
		len = -3;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // Unrecognised key is ignored.
    ಆದರೆ (len == 0)
	ಹೋಗು theend;

    // Handle modifiers.
    modifiers = 0;
    ಆದರೆ (ev_press->state & ಸರಿಸು_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (ev_press->state & Control_ಮುಖವಾಡ)
    {
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
	ಆದರೆ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    ಆದರೆ (ev_press->state & Mod1_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;
    ಆದರೆ (ev_press->state & Mod4_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    ಆದರೆ (len == -3)
	ಕೀಲಿ = ಗಮ್ಯ_SPECIAL(string[1], string[2]);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	string[len] = NUL;
	ಕೀಲಿ = mb_ptr2char(string);
    }
    ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
    ಆದರೆ (ಕೀಲಿ == CSI)
	ಕೀಲಿ = K_CSI;
    ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
    {
	string[0] = CSI;
	string[1] = K_ಎರಡನೇ(ಕೀಲಿ);
	string[2] = K_ಮೂರನೇ(ಕೀಲಿ);
	len = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);

	len = mb_char2bytes(ಕೀಲಿ, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ಕೀಲಿ);
    }

    ಆದರೆ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	ಪೂರ್ಣಾಂಕ int_ch = check_for_ಅಡಚಣೆ(ಕೀಲಿ, modifiers);

	ಆದರೆ (int_ch != NUL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    ಆದರೆ (p_mh)
	gui_mch_mousehide(TRUE);

#ಆದರೆ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	ಆದರೆ ((be = gui_mch_currently_showing_beval()) != ಖಾಲಿ)
	    gui_mch_unpost_balloon(be);
    }
#ಆದರೆ_ಅಂತ್ಯ
theend:
    {}	    // some compilers need a statement here
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಆದರೆ (string_alloced)
	XtFree((ಅಕ್ಷರ *)string);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_mouse_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	dud ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    ಸ್ಥಿರ XtIntervalId timer = (XtIntervalId)0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	timed_ಹೊರಹೋಗು = TRUE;

    ಪೂರ್ಣಾಂಕ		button;
    ಪೂರ್ಣಾಂಕ		repeated_click = FALSE;
    ಪೂರ್ಣಾಂಕ		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    ಆದರೆ (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1_ಮುಖವಾಡ | Button2_ಮುಖವಾಡ | Button3_ಮುಖವಾಡ))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	ಆದರೆ (button != MOUSE_DRAG)	// just moving the rodent
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
	    ಆದರೆ (dud)			// moved in vimForm
		y -= gui.menu_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
	    gui_mouse_moved(x, y);
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	ಆದರೆ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    ಆದರೆ (!timed_ಹೊರಹೋಗು)
	    {
		Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(timer);
		repeated_click = TRUE;
	    }
	    timed_ಹೊರಹೋಗು = FALSE;
	    timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_ಹೊರಹೋಗು);
	    ಆಯ್ಕೆ (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		ಸ್ಥಿತಿ Button1:	button = MOUSE_ಎಡ;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ Button2:	button = MOUSE_ಮಧ್ಯ;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ Button3:	button = MOUSE_ಬಲ;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ Button4:	button = MOUSE_4;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ Button5:	button = MOUSE_5;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 6:		button = MOUSE_7;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 7:		button = MOUSE_6;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 8:		button = MOUSE_X1;	ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 9:		button = MOUSE_X2;	ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಹಿಂತಿರುಗಿ;	// Unknown button
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ಕೊನೆಯ_mouse_event = event->xbutton;
#ಆದರೆ_ಅಂತ್ಯ
    }

    vim_modifiers = 0x0;
    ಆದರೆ (x_modifiers & ಸರಿಸು_ಮುಖವಾಡ)
	vim_modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (x_modifiers & Control_ಮುಖವಾಡ)
	vim_modifiers |= MOUSE_CTRL;
    ಆದರೆ (x_modifiers & Mod1_ಮುಖವಾಡ)	    // Alt or Meta key
	vim_modifiers |= MOUSE_ALT;

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);
}

/*
 * End of call-back routines
 */

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    ಪೂರ್ಣಾಂಕ	    arg;
    ಪೂರ್ಣಾಂಕ	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(ಅಕ್ಷರ *, *argc);
    ಆದರೆ (gui_argv == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    ಆಗ (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)Xt_ಸಂಖ್ಯೆ(cmdline_options); i++)
	    ಆದರೆ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		ನಿಲ್ಲಿಸು;

	ಆದರೆ (i < (ಪೂರ್ಣಾಂಕ)Xt_ಸಂಖ್ಯೆ(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    ಆದರೆ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_ವಾದ = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    ಆದರೆ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * ಗಾತ್ರ(ಅಕ್ಷರ *));
		ಆದರೆ (cmdline_options[i].arg_ಬಗೆ != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    ಆದರೆ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * ಗಾತ್ರ(ಅಕ್ಷರ *));
		}
	    }
	    argv[*argc] = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ಗಾತ್ರ(ಅಕ್ಷರ *));
	    argv[*argc] = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    arg++;
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XtSpecificationRelease
# define CARDINAL (Cardinal *)
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# ಇಲ್ಲದಿದ್ದರೆ
#  define CARDINAL (ಪೂರ್ಣಾಂಕ *)
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    Xt_ಗಣ_Language_Proc(ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    open_app_context();
    ಆದರೆ (app_context != ಖಾಲಿ)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_ಸಂಖ್ಯೆ(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#ಆದರೆ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	ಅಕ್ಷರ *p = setlocale(LC_NUMERIC, ಖಾಲಿ);

	ಆದರೆ (p == ಖಾಲಿ || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (app_context == ಖಾಲಿ || gui.dpy == ಖಾಲಿ)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ಸ್ಥಿರ Xt_ಒಳಹರಿವು_Id _xsmp_xtinputid;

    ಸ್ಥಿರ ಶೂನ್ಯ
local_xsmp_handle_ವಿನಂತಿಗಳು(
    Xt_ಸೂಚಕ	c ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*s ಬಳಸದ,
    Xt_ಒಳಹರಿವು_Id	*i ಬಳಸದ)
{
    ಆದರೆ (xsmp_handle_ವಿನಂತಿಗಳು() == FAIL)
	Xt_Remove_ಒಳಹರಿವು(_xsmp_xtinputid);
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init(ಶೂನ್ಯ)
{
    XtGCMask	gc_ಮುಖವಾಡ;
    XGCValues	gc_vals;
    ಪೂರ್ಣಾಂಕ		x, y, ಮುಖವಾಡ;
    ಚಿಹ್ನೆರಹಿತ	w, h;

#ಆದರೆ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#ಆದರೆ_ಅಂತ್ಯ

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ಖಾಲಿ);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (Xt_ಸೂಚಕ)&gui,
	vim_resources, Xt_ಸಂಖ್ಯೆ(vim_resources), ಖಾಲಿ);

    gui.scrollbar_ಎತ್ತರ = gui.scrollbar_ಅಗಲ;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#ಆದರೆ_ಅಂತ್ಯ

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ಹಿಂಭಾಗ_pixel = gui.def_ಹಿಂಭಾಗ_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    ಆದರೆ (gui.rsrc_rev_video && gui_get_lightness(gui.ಹಿಂಭಾಗ_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_ಹಿಂಭಾಗ_pixel;
	gui.ಹಿಂಭಾಗ_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_ಹಿಂಭಾಗ_pixel = gui.ಹಿಂಭಾಗ_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    ಗಣ_ಸಾಮಾನ್ಯ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_ಮುಖವಾಡ = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.ಹಿಂಭಾಗ_pixel;
    gui.ಪಠ್ಯ_gc = XtGetGC(vimShell, gc_ಮುಖವಾಡ, &gc_vals);

    gc_vals.foreground = gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.ಹಿಂಭಾಗ_gc = XtGetGC(vimShell, gc_ಮುಖವಾಡ, &gc_vals);

    gc_ಮುಖವಾಡ |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_ಮುಖವಾಡ, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_ಸಿದ್ಧತೆ_atoms(gui.dpy);

    ಆದರೆ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ಗಣ_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    ಆದರೆ (gui.geom != ಖಾಲಿ && *gui.geom != NUL)
    {
	ಮುಖವಾಡ = XParseGeometry((ಅಕ್ಷರ *)gui.geom, &x, &y, &w, &h);
	ಆದರೆ (ಮುಖವಾಡ & ಅಗಲ_ಮೌಲ್ಯ)
	    Columns = w;
	ಆದರೆ (ಮುಖವಾಡ & ಎತ್ತರ_ಮೌಲ್ಯ)
	{
	    ಆದರೆ (p_ಕಿಟಕಿ > (ದೀರ್ಘ)h - 1 || !option_was_ಗಣ((char_u *)"window"))
		p_ಕಿಟಕಿ = h - 1;
	    ಅಡ್ಡಸಾಲುಗಳು = h;
	}
	ಎಲ್ಲೆ_screen_ಗಾತ್ರ();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	ಆದರೆ (ಮುಖವಾಡ & (XValue|YValue))
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell, XtNgeometry, gui.geom, ಖಾಲಿ);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    ಆದರೆ (vim_strchr(p_go, GO_ICON) != ಖಾಲಿ)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	Xt_ಗಣ_Arg(arg[0], XtNiconPixmap,
		XCreate_Bitmap_From_ದತ್ತಾಂಶ(gui.dpy,
		    Default_ಬೇರು_ಕಿಟಕಿ(gui.dpy),
		    (ಅಕ್ಷರ *)vim_icon_ಬಿಟ್ಟುಗಳು,
		    vim_icon_ಅಗಲ,
		    vim_icon_ಎತ್ತರ));
	Xt_ಗಣ_Arg(arg[1], Xt_Nicon_ಮುಖವಾಡ,
		XCreate_Bitmap_From_ದತ್ತಾಂಶ(gui.dpy,
		    Default_ಬೇರು_ಕಿಟಕಿ(gui.dpy),
		    (ಅಕ್ಷರ *)vim_ಮುಖವಾಡ_icon_ಬಿಟ್ಟುಗಳು,
		    vim_ಮುಖವಾಡ_icon_ಅಗಲ,
		    vim_ಮುಖವಾಡ_icon_ಎತ್ತರ));
	Xt_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell, arg, (Cardinal)2);
#ಇಲ್ಲದಿದ್ದರೆ
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ಸ್ಥಿರ Pixmap	icon = 0;
    ಸ್ಥಿರ Pixmap	icon_ಮುಖವಾಡ = 0;
    ಸ್ಥಿರ ಅಕ್ಷರ		**magick = vim32x32;
    ಕಿಟಕಿ		ಬೇರು_ಕಿಟಕಿ;
    XIcon_ಗಾತ್ರ		*ಗಾತ್ರ_2;
    ಪೂರ್ಣಾಂಕ			ಸಂಖ್ಯೆ_sizes;
    Display		*dsp;
    Screen		*scr;
    Xpm_ಗುಣಲಕ್ಷಣಗಳು	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    ಬೇರು_ಕಿಟಕಿ = XRoot_ಕಿಟಕಿ_ದ_Screen(XtScreen(vimShell));
    ಆದರೆ (XGetIconSizes(XtDisplay(vimShell), ಬೇರು_ಕಿಟಕಿ,
						   &ಗಾತ್ರ_2, &ಸಂಖ್ಯೆ_sizes) != 0)
    {
	ಆದರೆ (ಸಂಖ್ಯೆ_sizes > 0)
	{
	    ಆದರೆ (ಗಾತ್ರ_2->max_ಎತ್ತರ_2 >= 48 && ಗಾತ್ರ_2->max_ಅಗಲ_2 >= 48)
		magick = vim48x48;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_2->max_ಎತ್ತರ_2 >= 32 && ಗಾತ್ರ_2->max_ಅಗಲ_2 >= 32)
		magick = vim32x32;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಗಾತ್ರ_2->max_ಎತ್ತರ_2 >= 16 && ಗಾತ್ರ_2->max_ಅಗಲ_2 >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell, XtNcolormap, cmap, ಖಾಲಿ);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | Xpm_ಆಳ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.ಆಳ = Default_ಆಳ_ದ_Screen(scr);

    ಆದರೆ (!icon)
    {
	Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dsp, ಬೇರು_ಕಿಟಕಿ, magick, &icon,
							   &icon_ಮುಖವಾಡ, &attr);
	Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು(&attr);
    }

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell, XmNiconPixmap, icon, Xm_Nicon_ಮುಖವಾಡ, icon_ಮುಖವಾಡ, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    gui_init_menu_font();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
    // Attach listener on ICE connection
    ಆದರೆ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_ಕೂಡಿಸು_ಒಳಹರಿವು(app_context, xsmp_icefd,
		(Xt_ಸೂಚಕ)Xt_ಒಳಹರಿವು_Read_ಮುಖವಾಡ, local_xsmp_handle_ವಿನಂತಿಗಳು, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    ಶೂನ್ಯ
gui_mch_uninit(ಶೂನ್ಯ)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ಖಾಲಿ;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    long_u	gc_ಮುಖವಾಡ;
    XGCValues	gc_vals;

    gc_ಮುಖವಾಡ = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.ಹಿಂಭಾಗ_pixel;
    ಆದರೆ (gui.ಪಠ್ಯ_gc != ಖಾಲಿ)
	XChangeGC(gui.dpy, gui.ಪಠ್ಯ_gc, gc_ಮುಖವಾಡ, &gc_vals);

    gc_vals.foreground = gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.background = gui.norm_pixel;
    ಆದರೆ (gui.ಹಿಂಭಾಗ_gc != ಖಾಲಿ)
	XChangeGC(gui.dpy, gui.ಹಿಂಭಾಗ_gc, gc_ಮುಖವಾಡ, &gc_vals);

    gc_ಮುಖವಾಡ |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    gc_vals.function   = GXxor;
    ಆದರೆ (gui.invert_gc != ಖಾಲಿ)
	XChangeGC(gui.dpy, gui.invert_gc, gc_ಮುಖವಾಡ, &gc_vals);

    gui_x11_ಗಣ_ಹಿಂಭಾಗ_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_open(ಶೂನ್ಯ)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    Xt_Manage_ಮಗು(Xt_Name_ಗಮ್ಯ_Widget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_ಸೂಚಕ = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_ಕಿಟಕಿ_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, Xt_ಕಿಟಕಿ(vimShell), wm_atoms, 2);
    Xt_ಕೂಡಿಸು_Event_Handler(vimShell, No_Event_ಮುಖವಾಡ, True, gui_x11_wm_protocol_handler,
							     ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    Xt_ಕೂಡಿಸು_Event_Handler(vimShell, (Event_ಮುಖವಾಡ)0, True, _XEditResCheckMessages,
	    (Xt_ಸೂಚಕ)ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
    ಆದರೆ (serverName == ಖಾಲಿ && server_Delayed_ಆರಂಭ_Name != ಖಾಲಿ)
    {
	// This is a :gui command in a plain vim with no previous server
	comm_ಕಿಟಕಿ = Xt_ಕಿಟಕಿ(vimShell);
	(ಶೂನ್ಯ)server_ರಿಜಿಸ್ಟರ್_Name(gui.dpy, server_Delayed_ಆರಂಭ_Name);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	server_Change_Registered_ಕಿಟಕಿ(gui.dpy, Xt_ಕಿಟಕಿ(vimShell));
    }
    Xt_ಕೂಡಿಸು_Event_Handler(vimShell, Property_Change_ಮುಖವಾಡ, False,
		      gui_x11_send_event_handler, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    xim_init();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    ಶೂನ್ಯ
gui_init_tooltip_font(ಶೂನ್ಯ)
{
    Xrm_ಮೌಲ್ಯ from, ಗಮ್ಯ;

    from.addr = (ಅಕ್ಷರ *)gui.rsrc_tooltip_font_name;
    from.ಗಾತ್ರ_2 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(from.addr);
    ಗಮ್ಯ.addr = (Xt_ಸೂಚಕ)&gui.tooltip_fontset;
    ಗಮ್ಯ.ಗಾತ್ರ_2 = ಗಾತ್ರ(XFont_ಗಣ);

    ಆದರೆ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_ಗಣ, &ಗಮ್ಯ) == False)
    {
	// Failed. What to do?
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    ಶೂನ್ಯ
gui_init_menu_font(ಶೂನ್ಯ)
{
    Xrm_ಮೌಲ್ಯ from, ಗಮ್ಯ;

# ifdef FONTSET_ALWAYS
    from.addr = (ಅಕ್ಷರ *)gui.rsrc_menu_font_name;
    from.ಗಾತ್ರ_2 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(from.addr);
    ಗಮ್ಯ.addr = (Xt_ಸೂಚಕ)&gui.menu_fontset;
    ಗಮ್ಯ.ಗಾತ್ರ_2 = ಗಾತ್ರ(GuiFontset);

    ಆದರೆ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_ಗಣ, &ಗಮ್ಯ) == False)
    {
	// Failed. What to do?
    }
# ಇಲ್ಲದಿದ್ದರೆ
    from.addr = (ಅಕ್ಷರ *)gui.rsrc_menu_font_name;
    from.ಗಾತ್ರ_2 = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(from.addr);
    ಗಮ್ಯ.addr = (Xt_ಸೂಚಕ)&gui.menu_font;
    ಗಮ್ಯ.ಗಾತ್ರ_2 = ಗಾತ್ರ(GuiFont);

    ಆದರೆ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &ಗಮ್ಯ) == False)
    {
	// Failed. What to do?
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ rc ಬಳಸದ)
{
#ಆದರೆ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#ಆದರೆ_ಅಂತ್ಯ
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ಖಾಲಿ);
    *x = xpos;
    *y = ypos;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell,
	XtNx,	x,
	XtNy,	y,
	ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(
    ಪೂರ್ಣಾಂಕ		ಅಗಲ,
    ಪೂರ್ಣಾಂಕ		ಎತ್ತರ,
    ಪೂರ್ಣಾಂಕ		min_ಅಗಲ,
    ಪೂರ್ಣಾಂಕ		min_ಎತ್ತರ,
    ಪೂರ್ಣಾಂಕ		ತಳ_ಅಗಲ,
    ಪೂರ್ಣಾಂಕ		ತಳ_ಎತ್ತರ,
    ಪೂರ್ಣಾಂಕ		direction ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಎತ್ತರ += xim_get_ಸ್ಥಿತಿ_area_ಎತ್ತರ(),
#ಆದರೆ_ಅಂತ್ಯ
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell,
	XtNwidthInc,	gui.char_ಅಗಲ,
	XtNheightInc,	gui.char_ಎತ್ತರ,
#ಆದರೆ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	Xt_Nbase_ಅಗಲ,	ತಳ_ಅಗಲ,
	Xt_Nbase_ಎತ್ತರ,	ತಳ_ಎತ್ತರ,
#ಆದರೆ_ಅಂತ್ಯ
	Xt_Nmin_ಅಗಲ,	min_ಅಗಲ,
	Xt_Nmin_ಎತ್ತರ,	min_ಎತ್ತರ,
	XtNwidth,	ಅಗಲ,
	XtNheight,	ಎತ್ತರ,
	ಖಾಲಿ);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(
    ಪೂರ್ಣಾಂಕ	    *screen_w,
    ಪೂರ್ಣಾಂಕ	    *screen_h)
{
    *screen_w = Display_ಅಗಲ(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = Display_ಎತ್ತರ(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init_font(
    char_u	*font_name,
    ಪೂರ್ಣಾಂಕ		do_fontset ಬಳಸದ)
{
    XFontStruct	*font = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    XFont_ಗಣ	fontset = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    ಆದರೆ (font_name != ಖಾಲಿ && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	ಆದರೆ (font_name == ಖಾಲಿ && !gui.starting)
	    ಹಿಂತಿರುಗಿ OK;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    ಆದರೆ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	ಆದರೆ (font_name != ಖಾಲಿ)
	{
	    fontset = (XFont_ಗಣ)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    ಆದರೆ (fontset == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	ಆದರೆ (font_name == ಖಾಲಿ)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    ಆದರೆ (font == ಖಾಲಿ)
		font_name = (char_u *)DFLT_FONT;
	}
	ಆದರೆ (font == ಖಾಲಿ)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	ಆದರೆ (font == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    ಆದರೆ (fontset != ಖಾಲಿ)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_ಅಗಲ = fontset_ಅಗಲ(fontset);
	gui.char_ಎತ್ತರ = fontset_ಎತ್ತರ(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	gui.norm_font = (GuiFont)font;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#ಆದರೆ_ಅಂತ್ಯ
	gui.char_ಅಗಲ = font->max_bounds.ಅಗಲ;
	gui.char_ಎತ್ತರ = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_ಗಣ_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    ಆದರೆ (font_name == gui.rsrc_font_name)
    {
	ಆದರೆ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ಖಾಲಿ
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	ಆದರೆ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ಖಾಲಿ
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	ಆದರೆ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ಖಾಲಿ
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	ಆದರೆ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	ಆದರೆ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	ಆದರೆ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, ಪೂರ್ಣಾಂಕ giveErrorIfMissing)
{
    XFontStruct	*font;

    ಆದರೆ (!gui.ಒಳ_use || name == ಖಾಲಿ)	// can't do this when GUI not running
	ಹಿಂತಿರುಗಿ NOFONT;

    font = XLoadQueryFont(gui.dpy, (ಅಕ್ಷರ *)name);

    ಆದರೆ (font == ಖಾಲಿ)
    {
	ಆದರೆ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ಹಿಂತಿರುಗಿ NOFONT;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
    ಬರೆ("Font Information for '%s':\n", name);
    ಬರೆ("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.ಅಗಲ, font->ascent + font->descent,
	   font->ascent, font->descent);
    ಬರೆ("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    ಬರೆ("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    ಬರೆ("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    ಬರೆ("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.ಅಗಲ));
    ಬರೆ("\n");
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (font->max_bounds.ಅಗಲ != font->min_bounds.ಅಗಲ)
    {
	semsg(_(e_font_str_is_not_fixed_ಅಗಲ), name);
	XFreeFont(gui.dpy, font);
	ಹಿಂತಿರುಗಿ NOFONT;
    }
    ಹಿಂತಿರುಗಿ (GuiFont)font;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ಖಾಲಿ;

    ಆದರೆ (name != ಖಾಲಿ && font == ಖಾಲಿ)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (font != ಖಾಲಿ)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಮೌಲ್ಯ = 0L;

	ಆದರೆ (XGetFontProperty(font, XA_FONT, &ಮೌಲ್ಯ))
	{
	    ಅಕ್ಷರ *xa_font_name = ಖಾಲಿ;

	    xa_font_name = XGetAtomName(gui.dpy, ಮೌಲ್ಯ);
	    ಆದರೆ (xa_font_name != ಖಾಲಿ)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name != ಖಾಲಿ)
		ret = vim_strsave(name);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (name != ಖಾಲಿ)
	    ret = vim_strsave(name);
    }
    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    ಆದರೆ (gui.fontset != NOFONTSET)
    {
	gui.char_ಎತ್ತರ = fontset_ಎತ್ತರ((XFont_ಗಣ)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFont_ಗಣ)gui.fontset)
							    + p_linespace / 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_ಎತ್ತರ = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set the current text font.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_font(GuiFont font)
{
    ಸ್ಥಿರ Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    ಆದರೆ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.ಪಠ್ಯ_gc, fid);
	XSetFont(gui.dpy, gui.ಹಿಂಭಾಗ_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    current_fontset = (XFont_ಗಣ)ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_fontset(GuiFontset fontset)
{
    current_fontset = (XFont_ಗಣ)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If a font is not going to be used, free its structure.
 */
    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಆದರೆ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#ಆದರೆ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    ಶೂನ್ಯ
gui_mch_free_fontset(GuiFontset fontset)
{
    ಆದರೆ (fontset != NOFONTSET)
	XFree_Font_ಗಣ(gui.dpy, (XFont_ಗಣ)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		giveErrorIfMissing,
    ಪೂರ್ಣಾಂಕ		fixed_ಅಗಲ)
{
    XFont_ಗಣ	fontset;
    ಅಕ್ಷರ	**missing, *def_str;
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_missing;

    ಆದರೆ (!gui.ಒಳ_use || name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ NOFONTSET;

    fontset = XCreate_Font_ಗಣ(gui.dpy, (ಅಕ್ಷರ *)name, &missing, &ಅಂಕೆ_missing,
			     &def_str);
    ಆದರೆ (ಅಂಕೆ_missing > 0)
    {
	ಪೂರ್ಣಾಂಕ i;

	ಆದರೆ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_ಒಳ_fontset), name);
	    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    ಆದರೆ (fontset == ಖಾಲಿ)
    {
	ಆದರೆ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	ಹಿಂತಿರುಗಿ NOFONTSET;
    }

    ಆದರೆ (fixed_ಅಗಲ && check_fontset_sanity(fontset) == FAIL)
    {
	XFree_Font_ಗಣ(gui.dpy, fontset);
	ಹಿಂತಿರುಗಿ NOFONTSET;
    }
    ಹಿಂತಿರುಗಿ (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_fontset_sanity(XFont_ಗಣ fs)
{
    XFontStruct	**xfs;
    ಅಕ್ಷರ	**font_name;
    ಪೂರ್ಣಾಂಕ		fn;
    ಅಕ್ಷರ	*ತಳ_name;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		min_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ		min_font_idx = 0;

    ತಳ_name = XBase_Font_Name_List_ದ_Font_ಗಣ(fs);
    fn = XFonts_ದ_Font_ಗಣ(fs, &xfs, &font_name);
    ಗಾಗಿ (i = 0; i < fn; i++)
    {
	ಆದರೆ (xfs[i]->max_bounds.ಅಗಲ != xfs[i]->min_bounds.ಅಗಲ)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_ಅಗಲ),
		    ತಳ_name, font_name[i]);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    // scan base font width
    min_ಅಗಲ = 32767;
    ಗಾಗಿ (i = 0; i < fn; i++)
    {
	ಆದರೆ (xfs[i]->max_bounds.ಅಗಲ<min_ಅಗಲ)
	{
	    min_ಅಗಲ = xfs[i]->max_bounds.ಅಗಲ;
	    min_font_idx = i;
	}
    }
    ಗಾಗಿ (i = 0; i < fn; i++)
    {
	ಆದರೆ (	   xfs[i]->max_bounds.ಅಗಲ != 2 * min_ಅಗಲ
		&& xfs[i]->max_bounds.ಅಗಲ != min_ಅಗಲ)
	{
	    semsg(_(e_fontset_name_str), ತಳ_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (ಪೂರ್ಣಾಂಕ)xfs[min_font_idx]->max_bounds.ಅಗಲ);
	    semsg(_("Font%d width: %d"), i, (ಪೂರ್ಣಾಂಕ)xfs[i]->max_bounds.ಅಗಲ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    // it seems ok. Good Luck!!
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fontset_ಅಗಲ(XFont_ಗಣ fs)
{
 ಹಿಂತಿರುಗಿ Xmb_ಪಠ್ಯ_Escapement(fs, "Vim", 3) / 3;
}

    ಪೂರ್ಣಾಂಕ
fontset_ಎತ್ತರ(
    XFont_ಗಣ fs)
{
    XFont_ಗಣ_Extents *extents;

    extents = XExtents_ದ_Font_ಗಣ(fs);
    ಹಿಂತಿರುಗಿ extents->max_logical_extent.ಎತ್ತರ;
}

# ಆದರೆ 0
// NOT USED YET
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fontset_descent(XFont_ಗಣ fs)
{
    XFont_ಗಣ_Extents *extents;

    extents = XExtents_ದ_Font_ಗಣ (fs);
    ಹಿಂತಿರುಗಿ extents->max_logical_extent.ಎತ್ತರ + extents->max_logical_extent.y;
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fontset_ascent(XFont_ಗಣ fs)
{
    XFont_ಗಣ_Extents *extents;

    extents = XExtents_ದ_Font_ಗಣ(fs);
    ಹಿಂತಿರುಗಿ -extents->max_logical_extent.y;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    ಆದರೆ (!gui.ಒಳ_use || name == ಖಾಲಿ || *name == NUL)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    requested = gui_get_color_cmn(name);
    ಆದರೆ (requested == INVALCOLOR)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    ಹಿಂತಿರುಗಿ gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
    XColor	ಲಭ್ಯ;
    Colormap	colormap;

#ಆದರೆ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    ಅಕ್ಷರ	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, ಗಾತ್ರ(spec), "#%.2x%.2x%.2x", r, g, b);
    ಆದರೆ (XParseColor(gui.dpy, colormap, (ಅಕ್ಷರ *)spec, &ಲಭ್ಯ) != 0
	    && XAllocColor(gui.dpy, colormap, &ಲಭ್ಯ) != 0)
	ಹಿಂತಿರುಗಿ (guicolor_T)ಲಭ್ಯ.pixel;
#ಆದರೆ_ಅಂತ್ಯ
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_ಕ್ಷೇತ್ರ(ಲಭ್ಯ);
    ಲಭ್ಯ.red = r << 8;
    ಲಭ್ಯ.green = g << 8;
    ಲಭ್ಯ.blue = b << 8;
    ಆದರೆ (XAllocColor(gui.dpy, colormap, &ಲಭ್ಯ) != 0)
	ಹಿಂತಿರುಗಿ (guicolor_T)ಲಭ್ಯ.pixel;

    ಹಿಂತಿರುಗಿ INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(guicolor_T color)
{
    ಆದರೆ (color == prev_fg_color)
	ಹಿಂತಿರುಗಿ;

    XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(guicolor_T color)
{
    ಆದರೆ (color == prev_bg_color)
	ಹಿಂತಿರುಗಿ;

    XSetBackground(gui.dpy, gui.ಪಠ್ಯ_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    ಸ್ಥಿರ Cursor
gui_x11_create_blank_mouse(ಶೂನ್ಯ)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0, (XGCValues*)0);

    ಆದರೆ (gc != ಖಾಲಿ)
    {
	XDraw_ಬಿಂದು(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    ಹಿಂತಿರುಗಿ XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_curl(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ cells)
{
    ಪೂರ್ಣಾಂಕ			i;
    ಪೂರ್ಣಾಂಕ			ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_sp_color);
    ಗಾಗಿ (i = FILL_X(ಲಂಬಸಾಲು); i < FILL_X(ಲಂಬಸಾಲು + cells); ++i)
    {
	ಸ್ಥಾನಾಂತರ = val[i % 8];
	XDraw_ಬಿಂದು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, i,
						FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1 - ಸ್ಥಾನಾಂತರ);
    }
    XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_fg_color);
}

    ಶೂನ್ಯ
gui_mch_draw_string(
    ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು,
    ಪೂರ್ಣಾಂಕ		ಲಂಬಸಾಲು,
    char_u	*s,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		flags)
{
    ಪೂರ್ಣಾಂಕ			cells = len;
    ಸ್ಥಿರ ಶೂನ್ಯ		*buf = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		buflen = 0;
    char_u		*p;
    ಪೂರ್ಣಾಂಕ			wlen = 0;
    ಪೂರ್ಣಾಂಕ			c;

    ಆದರೆ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	ಆದರೆ (buflen < len)
	{
	    XtFree((ಅಕ್ಷರ *)buf);
	    buf = (ಶೂನ್ಯ *)XtMalloc(len * (ಗಾತ್ರ(XChar2b) < ಗಾತ್ರ(wchar_t)
					? ಗಾತ್ರ(wchar_t) : ಗಾತ್ರ(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	ಆಗ (p < s + len)
	{
	    c = utf_ptr2char(p);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	    ಆದರೆ (current_fontset != ಖಾಲಿ)
	    {
# ifdef SMALL_WCHAR_T
		ಆದರೆ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    {
		ಆದರೆ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (ಚಿಹ್ನೆರಹಿತ)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
    {
	cells = 0;
	ಗಾಗಿ (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    ಆದರೆ (current_fontset != ಖಾಲಿ)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.ಎತ್ತರ = gui.char_ಎತ್ತರ;
	clip.ಅಗಲ = gui.char_ಅಗಲ * cells + 1;
	XSetClipRectangles(gui.dpy, gui.ಪಠ್ಯ_gc, FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು),
		&clip, 1, Unsorted);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (flags & DRAW_TRANSP)
    {
	ಆದರೆ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), buf, wlen);
	ಇಲ್ಲದಿದ್ದರೆ
	    XDrawString(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), (ಅಕ್ಷರ *)s, len);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_linespace != 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
	    || current_fontset != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, FILL_X(ಲಂಬಸಾಲು),
		FILL_Y(ಅಡ್ಡಸಾಲು), gui.char_ಅಗಲ * cells, gui.char_ಎತ್ತರ);
	XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_fg_color);

	ಆದರೆ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), buf, wlen);
	ಇಲ್ಲದಿದ್ದರೆ
	    XDrawString(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), (ಅಕ್ಷರ *)s, len);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	ಆದರೆ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), buf, wlen);
	ಇಲ್ಲದಿದ್ದರೆ
	    XDrawImageString(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), (ಅಕ್ಷರ *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    ಆದರೆ (flags & DRAW_BOLD)
    {
	ಆದರೆ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು) + 1,
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), buf, wlen);
	ಇಲ್ಲದಿದ್ದರೆ
	    XDrawString(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, ಪಠ್ಯ_X(ಲಂಬಸಾಲು) + 1,
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು), (ಅಕ್ಷರ *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    ಆದರೆ (flags & DRAW_UNDERC)
	draw_curl(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, cells);

    // Underline: draw a line at the bottom of the character cell.
    ಆದರೆ (flags & DRAW_UNDERL)
    {
	ಪೂರ್ಣಾಂಕ	y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ಆದರೆ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_ಸಾಲು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, FILL_X(ಲಂಬಸಾಲು),
		y, FILL_X(ಲಂಬಸಾಲು + cells) - 1, y);
    }

    ಆದರೆ (flags & DRAW_STRIKE)
    {
	ಪೂರ್ಣಾಂಕ	y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - gui.char_ಎತ್ತರ/2;

	XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_sp_color);
	XDraw_ಸಾಲು(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, FILL_X(ಲಂಬಸಾಲು),
		y, FILL_X(ಲಂಬಸಾಲು + cells) - 1, y);
	XSetForeground(gui.dpy, gui.ಪಠ್ಯ_gc, prev_fg_color);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XFONTSET
    ಆದರೆ (current_fontset != ಖಾಲಿ)
	XSet_Clip_ಮುಖವಾಡ(gui.dpy, gui.ಪಠ್ಯ_gc, None);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != (ಕೀಲಿ_Sym)0; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
					 name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ಪೂರ್ಣಾಂಕ
gui_get_x11_windis(ಕಿಟಕಿ *win, Display **dis)
{
    *win = Xt_ಕಿಟಕಿ(vimShell);
    *dis = gui.dpy;
    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    XBell(gui.dpy, 0);
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ಪೂರ್ಣಾಂಕ)Columns) + gui.border_ಸ್ಥಾನಾಂತರ,
	    FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು) + gui.border_ಸ್ಥಾನಾಂತರ);
    XSync(gui.dpy, False);
    ui_delay((ದೀರ್ಘ)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ಪೂರ್ಣಾಂಕ)Columns) + gui.border_ಸ್ಥಾನಾಂತರ,
	    FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು) + gui.border_ಸ್ಥಾನಾಂತರ);
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
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_ಅಗಲ, (nr) * gui.char_ಎತ್ತರ);
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    XIconify_ಕಿಟಕಿ(gui.dpy, Xt_ಕಿಟಕಿ(vimShell), DefaultScreen(gui.dpy));
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    XMapRaised(gui.dpy, Xt_ಕಿಟಕಿ(vimShell));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ಪೂರ್ಣಾಂಕ		w = 1;

    ಆದರೆ (mb_lefthalve(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು))
	w = 2;
    gui_mch_ಗಣ_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, FILL_X(gui.ಲಂಬಸಾಲು),
	    FILL_Y(gui.ಅಡ್ಡಸಾಲು), w * gui.char_ಅಗಲ - 1, gui.char_ಎತ್ತರ - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ಶೂನ್ಯ
gui_mch_draw_part_cursor(ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h, guicolor_T color)
{
    gui_mch_ಗಣ_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ಬಲ ? FILL_X(gui.ಲಂಬಸಾಲು + 1) - w :
#ಆದರೆ_ಅಂತ್ಯ
		FILL_X(gui.ಲಂಬಸಾಲು),
	    FILL_Y(gui.ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    Xt_ಒಳಹರಿವು_ಮುಖವಾಡ ಮುಖವಾಡ, desired;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ALT_X_ಒಳಹರಿವು
    ಆದರೆ (suppress_alternate_ಒಳಹರಿವು)
	desired = (XtIMXEvent | XtIMTimer);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	desired = (XtIMAll);
    ಆಗ ((ಮುಖವಾಡ = XtAppPending(app_context)) && (ಮುಖವಾಡ & desired)
	    && !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ())
	XtAppProcessEvent(app_context, desired);
}

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
    ಪೂರ್ಣಾಂಕ	    focus;
    ಪೂರ್ಣಾಂಕ	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    timed_ಹೊರಹೋಗು;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_ಒಳಹರಿವು_ಮುಖವಾಡ	    desired;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#ಆದರೆ_ಅಂತ್ಯ

    timed_ಹೊರಹೋಗು = FALSE;

    ಆದರೆ (wtime >= 0)
	timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_ಹೊರಹೋಗು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ಆದರೆ (channel_any_keep_open())
	channel_timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, (long_u)20,
				   channel_poll_cb, (Xt_ಸೂಚಕ)&channel_timer);
#ಆದರೆ_ಅಂತ್ಯ

    focus = gui.ಒಳ_focus;
    desired = (XtIMAll);
    ಆಗ (!timed_ಹೊರಹೋಗು)
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
# ifdef FEAT_TIMERS
	did_ಕೂಡಿಸು_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಆದರೆ (did_ಕೂಡಿಸು_timer)
	    // Need to recompute the waiting time.
	    ನಿಲ್ಲಿಸು;
# endif
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    retval = OK;
	    ನಿಲ್ಲಿಸು;
	}
    }

    ಆದರೆ (timer != (XtIntervalId)0 && !timed_ಹೊರಹೋಗು)
	Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(timer);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (channel_timer != (XtIntervalId)0)
	Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(channel_timer);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
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
    XFlush(gui.dpy);
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
    ಪೂರ್ಣಾಂಕ		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.ಹಿಂಭಾಗ_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_ಅಗಲ + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ಎತ್ತರ);
}

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    ಆದರೆ (gui.visibility == VisibilityFullyObscured)
	ಹಿಂತಿರುಗಿ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.ಪಠ್ಯ_gc,
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು),
	gui.char_ಅಗಲ * (gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1)
			       + (gui.scroll_region_ಬಲ == Columns - 1),
	gui.char_ಎತ್ತರ * (gui.scroll_region_bot - ಅಡ್ಡಸಾಲು - ಅಂಕೆ_ಸಾಲುಗಳು + 1),
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು));

    gui_clear_ಖಂಡ(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
						       gui.scroll_region_ಎಡ,
			  gui.scroll_region_bot, gui.scroll_region_ಬಲ);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    ಆದರೆ (gui.visibility == VisibilityFullyObscured)
	ಹಿಂತಿರುಗಿ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.ಪಠ್ಯ_gc,
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	gui.char_ಅಗಲ * (gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1)
			       + (gui.scroll_region_ಬಲ == Columns - 1),
	gui.char_ಎತ್ತರ * (gui.scroll_region_bot - ಅಡ್ಡಸಾಲು - ಅಂಕೆ_ಸಾಲುಗಳು + 1),
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು));

    gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
				ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_check_copy_area(ಶೂನ್ಯ)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    ಆದರೆ (gui.visibility != VisibilityPartiallyObscured)
	ಹಿಂತಿರುಗಿ;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    ಹಿಂತಿರುಗಿ;	// The scroll worked.

	ಆದರೆ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->ಅಗಲ, gevent->ಎತ್ತರ);
	    ಆದರೆ (gevent->ಎಣಿಕೆ_2 == 0)
		ಹಿಂತಿರುಗಿ;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    ಪೂರ್ಣಾಂಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    ಹಿಂತಿರುಗಿ clip_x11_own_selection(vimShell, cbd);
}

    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    ಶೂನ್ಯ
clip_mch_ಗಣ_selection(
    Clipboard_T	*cbd)
{
    clip_x11_ಗಣ_selection(cbd);
}

#ಆದರೆ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey)
{
    ಆದರೆ (menu->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    gui_mch_menu_hidden(menu, False);
    ಆದರೆ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	Xt_ಗಣ_Sensitive(menu->id, False);
    ಇಲ್ಲದಿದ್ದರೆ
	Xt_ಗಣ_Sensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ hidden)
{
    ಆದರೆ (menu->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (hidden)
	Xt_Unmanage_ಮಗು(menu->id);
    ಇಲ್ಲದಿದ್ದರೆ
	Xt_Manage_ಮಗು(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
{
    // Nothing to do in X
}

    ಶೂನ್ಯ
gui_x11_menu_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui_menu_cb((vimmenu_T *)client_ದತ್ತಾಂಶ_2);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_wm_protocol_handler(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    /*
     * Only deal with Client messages.
     */
    ಆದರೆ (event->type != Client_ಸಂದೇಶ)
	ಹಿಂತಿರುಗಿ;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    ಆದರೆ ((Atom)((XClient_ಸಂದೇಶ_Event *)event)->ದತ್ತಾಂಶ.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	ಹೊರಹೋಗು_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, Xt_ಕಿಟಕಿ(vimShell), ಖಾಲಿ, 0);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ ((Atom)((XClient_ಸಂದೇಶ_Event *)event)->ದತ್ತಾಂಶ.l[0] !=
						  wm_atoms[DELETE_ಕಿಟಕಿ_IDX])
	ಹಿಂತಿರುಗಿ;

    gui_shell_closed();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_send_event_handler(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    XEvent	*event,
    Boolean	*dum ಬಳಸದ)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    ಆದರೆ (e->type == PropertyNotify && e->ಕಿಟಕಿ_2 == comm_ಕಿಟಕಿ
	    && e->atom == commProperty &&  e->state == Property_New_ಮೌಲ್ಯ)
	serverEventProc(gui.dpy, event, 0);
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
ಸ್ಥಿರ XtIntervalId	blink_timer = (XtIntervalId)0;

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
gui_mch_ಗಣ_blinking(ದೀರ್ಘ waittime, ದೀರ್ಘ on, ದೀರ್ಘ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    ಆದರೆ (blink_timer != (XtIntervalId)0)
    {
	Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    ಆದರೆ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_blink_cb(
    Xt_ಸೂಚಕ	    timed_ಹೊರಹೋಗು ಬಳಸದ,
    XtIntervalId    *interval_id ಬಳಸದ)
{
    ಆದರೆ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, blink_offtime,
						      gui_x11_blink_cb, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, blink_ontime,
						      gui_x11_blink_cb, ಖಾಲಿ);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ಶೂನ್ಯ
gui_mch_ಆರಂಭ_blink(ಶೂನ್ಯ)
{
    ಆದರೆ (blink_timer != (XtIntervalId)0)
	Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(blink_timer);
    // Only switch blinking on if none of the times is zero
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	blink_timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, blink_waittime,
						      gui_x11_blink_cb, ಖಾಲಿ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

/*
 * Return the RGB value of a pixel as a long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    XColor	xc;
    Colormap	colormap;

    colormap = DefaultColormap(gui.dpy, XDefaultScreen(gui.dpy));
    xc.pixel = pixel;
    XQueryColor(gui.dpy, colormap, &xc);

    ಹಿಂತಿರುಗಿ (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((ಚಿಹ್ನೆರಹಿತ)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    ಶೂನ್ಯ
gui_x11_callbacks(Widget ಪಠ್ಯ_Area, Widget vimForm)
{
    Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, Visibility_Change_ಮುಖವಾಡ, FALSE,
	gui_x11_visibility_cb, (Xt_ಸೂಚಕ)0);

    Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, Exposure_ಮುಖವಾಡ, FALSE, gui_x11_expose_cb,
	(Xt_ಸೂಚಕ)0);

    Xt_ಕೂಡಿಸು_Event_Handler(vimShell, Structure_Notify_ಮುಖವಾಡ, FALSE,
	gui_x11_resize_ಕಿಟಕಿ_cb, (Xt_ಸೂಚಕ)0);

    Xt_ಕೂಡಿಸು_Event_Handler(vimShell, Focus_Change_ಮುಖವಾಡ, FALSE, gui_x11_focus_change_cb,
	(Xt_ಸೂಚಕ)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ಆದರೆ (vim_strchr(p_go, GO_ಸೂಚಕ) != ಖಾಲಿ)
    {
	Xt_ಕೂಡಿಸು_Event_Handler(vimShell, Leave_ಕಿಟಕಿ_ಮುಖವಾಡ, FALSE, gui_x11_leave_cb,
	    (Xt_ಸೂಚಕ)0);
	Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, Leave_ಕಿಟಕಿ_ಮುಖವಾಡ, FALSE, gui_x11_leave_cb,
	    (Xt_ಸೂಚಕ)0);
	Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, Enter_ಕಿಟಕಿ_ಮುಖವಾಡ, FALSE, gui_x11_enter_cb,
	    (Xt_ಸೂಚಕ)0);
	Xt_ಕೂಡಿಸು_Event_Handler(vimShell, Enter_ಕಿಟಕಿ_ಮುಖವಾಡ, FALSE, gui_x11_enter_cb,
	    (Xt_ಸೂಚಕ)0);
    }

    Xt_ಕೂಡಿಸು_Event_Handler(vimForm, ಕೀಲಿ_Press_ಮುಖವಾಡ, FALSE, gui_x11_ಕೀಲಿ_ದೊರಕಿತು_cb,
	(Xt_ಸೂಚಕ)0);
    Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, ಕೀಲಿ_Press_ಮುಖವಾಡ, FALSE, gui_x11_ಕೀಲಿ_ದೊರಕಿತು_cb,
	(Xt_ಸೂಚಕ)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    Xt_ಕೂಡಿಸು_Event_Handler(vimForm, ಸೂಚಕ_Motion_ಮುಖವಾಡ,
	FALSE, gui_x11_mouse_cb, (Xt_ಸೂಚಕ)1);
    Xt_ಕೂಡಿಸು_Event_Handler(ಪಠ್ಯ_Area, Button_Press_ಮುಖವಾಡ | Button_Release_ಮುಖವಾಡ |
					 Button_Motion_ಮುಖವಾಡ | ಸೂಚಕ_Motion_ಮುಖವಾಡ,
	FALSE, gui_x11_mouse_cb, (Xt_ಸೂಚಕ)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    ಪೂರ್ಣಾಂಕ		rootx, rooty, winx, winy;
    ಕಿಟಕಿ	ಬೇರು, ಮಗು;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ;

    ಆದರೆ (gui.wid && XQuery_ಸೂಚಕ(gui.dpy, gui.wid, &ಬೇರು, &ಮಗು,
					 &rootx, &rooty, &winx, &winy, &ಮುಖವಾಡ))
    {
	*x = winx;
	*y = winy;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*x = -1;
	*y = -1;
    }
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    ಆದರೆ (gui.wid)
	XWarp_ಸೂಚಕ(gui.dpy, (ಕಿಟಕಿ)0, gui.wid, 0, 0, 0, 0, x, y);
}

#ಆದರೆ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_ಕೊನೆಯ_mouse_event(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ &ಕೊನೆಯ_mouse_event;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_ಅಗಲ (gui.char_ಅಗಲ * 2)

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ typenr)
{
    XImage	*sign;

    ಆದರೆ (!gui.ಒಳ_use || (sign = (XImage *)sign_get_image(typenr)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    XClearArea(gui.dpy, gui.wid, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು) - sign->ಎತ್ತರ,
	    SIGN_ಅಗಲ, gui.char_ಎತ್ತರ, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.ಪಠ್ಯ_gc, sign, 0, 0,
	    ಪಠ್ಯ_X(ಲಂಬಸಾಲು) + (SIGN_ಅಗಲ - sign->ಅಗಲ) / 2,
	    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು) - sign->ಎತ್ತರ,
	    sign->ಅಗಲ, sign->ಎತ್ತರ);
}

    ಶೂನ್ಯ *
gui_mch_ರಿಜಿಸ್ಟರ್_sign(char_u *signfile)
{
    Xpm_ಗುಣಲಕ್ಷಣಗಳು   attrs;
    XImage	    *sign = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    ಸ್ಥಿತಿ_2;

    /*
     * Setup the color substitution table.
     */
    ಆದರೆ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_ಚಿಹ್ನೆ color[5] =
	{
	    {"none", ಖಾಲಿ, 0},
	    {"iconColor1", ಖಾಲಿ, 0},
	    {"bottomShadowColor", ಖಾಲಿ, 0},
	    {"topShadowColor", ಖಾಲಿ, 0},
	    {"selectColor", ಖಾಲಿ, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.ಹಿಂಭಾಗ_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	ಸ್ಥಿತಿ_2 = Xpm_Read_File_ಗಮ್ಯ_Image(gui.dpy, (ಅಕ್ಷರ *)signfile,
							 &sign, ಖಾಲಿ, &attrs);
	ಆದರೆ (ಸ್ಥಿತಿ_2 == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_couldnt_read_ಒಳ_sign_ದತ್ತಾಂಶ));
    }

    ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ *)sign;
}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    XDestroyImage((XImage*)sign);
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_shape = 0;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Use the blank mouse pointer or not.
 */
    ಶೂನ್ಯ
gui_mch_mousehide(
    ಪೂರ್ಣಾಂಕ		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    ಆದರೆ (gui.ಸೂಚಕ_hidden == hide)
	ಹಿಂತಿರುಗಿ;

    gui.ಸೂಚಕ_hidden = hide;
    ಆದರೆ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_ಸೂಚಕ);
    ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	mch_ಗಣ_mouse_shape(ಕೊನೆಯ_shape);
#ಇಲ್ಲದಿದ್ದರೆ
    XUndefineCursor(gui.dpy, gui.wid);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mshape_ids[] =
{
    XC_ಎಡ_ptr,		// arrow
    0,				// blank
    XC_xterm,			// beam
    XC_sb_v_double_arrow,	// updown
    XC_sizing,			// udsizing
    XC_sb_h_double_arrow,	// leftright
    XC_sizing,			// lrsizing
    XC_watch,			// busy
    XC_X_cursor,		// no
    XC_crosshair,		// crosshair
    XC_hand1,			// hand1
    XC_hand2,			// hand2
    XC_pencil,			// pencil
    XC_question_arrow,		// question
    XC_ಬಲ_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ಎಡ_ptr			// last one
};

    ಶೂನ್ಯ
mch_ಗಣ_mouse_shape(ಪೂರ್ಣಾಂಕ shape)
{
    ಪೂರ್ಣಾಂಕ	    id;

    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (shape == MSHAPE_HIDE || gui.ಸೂಚಕ_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_ಸೂಚಕ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ಆದರೆ (id >= XC_ಅಂಕೆ_glyphs)
		id = XC_ಎಡ_ptr;
	    ಇಲ್ಲದಿದ್ದರೆ
		id &= ~1;	// they are always even (why?)
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    ಆದರೆ (shape != MSHAPE_HIDE)
	ಕೊನೆಯ_shape = shape;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    ಶೂನ್ಯ
gui_mch_menu_ಗಣ_tip(vimmenu_T *menu)
{
    ಆದರೆ (menu->id == ಖಾಲಿ || menu->ಪೋಷಕ == ಖಾಲಿ
				|| !menu_is_toolbar(menu->ಪೋಷಕ->name))
	ಹಿಂತಿರುಗಿ;

    // Always destroy and create the balloon, in case the string was
    // changed.
    ಆದರೆ (menu->tip != ಖಾಲಿ)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ಖಾಲಿ;
    }
    ಆದರೆ (menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP] != ಖಾಲಿ)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->ಅಕ್ಷರಸರಣಿಗಳು[MENU_ಸೂಚ್ಯಂಕ_TIP],
		ಖಾಲಿ,
		ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ
