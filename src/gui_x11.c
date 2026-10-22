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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <X11/keysym.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Xatom.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/StringDefs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Intrinsic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Shell.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#ਜੇ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#ਨਹੀਂਤਾਂ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ਨਹੀਂਤਾਂ
#   include <X11/xpm.h>
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# ਨਹੀਂਤਾਂ
#  include <locale.h>
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_NAME	"vim"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM_CLASS	"Vim"

// Default resource values
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_FONT		"7x13"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FONTSET_ALWAYS
# define DFLT_MENU_FONT		Xt_Default_Font_ਸੈੱਟ
#ਨਹੀਂਤਾਂ
# define DFLT_MENU_FONT		XtDefaultFont
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_TOOLTIP_FONT	Xt_Default_Font_ਸੈੱਟ

// use the default (CDE) colors
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_MENU_BG_COLOR	""
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_MENU_FG_COLOR	""
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_SCROLL_BG_COLOR	""
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_SCROLL_FG_COLOR	""
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

ਸਥਿਰ Atom   wm_atoms[2];	// Window Manager Atoms
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DELETE_ਖਿੜਕੀ_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ਸਥਿਰ XFont_ਸੈੱਟ current_fontset = ਨਿਰਾ;
# ਜੇ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	ਕਰੋ \
	{ \
	    ਜੇ (current_fontset != ਨਿਰਾ) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    ਨਹੀਂਤਾਂ \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} ਜਦੋਂ (0)
# endif
# ਜੇ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	ਕਰੋ \
	{ \
	    ਜੇ (current_fontset != ਨਿਰਾ) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ਨਹੀਂਤਾਂ \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ਜਦੋਂ (0)
# endif
# ਜੇ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	ਕਰੋ \
	{ \
	    ਜੇ (current_fontset != ਨਿਰਾ) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ਨਹੀਂਤਾਂ \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ਜਦੋਂ (0)
# endif
ਸਥਿਰ ਪੂਰਨ check_fontset_sanity(XFont_ਸੈੱਟ fs);
ਸਥਿਰ ਪੂਰਨ fontset_ਚੌੜਾਈ(XFont_ਸੈੱਟ fs);
ਸਥਿਰ ਪੂਰਨ fontset_ascent(XFont_ਸੈੱਟ fs);
#ਜੇ_ਅੰਤ

ਸਥਿਰ guicolor_T	prev_fg_color = INVALCOLOR;
ਸਥਿਰ guicolor_T	prev_bg_color = INVALCOLOR;
ਸਥਿਰ guicolor_T	prev_sp_color = INVALCOLOR;

#ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ਸਥਿਰ XButtonPressedEvent ਆਖਰੀ_mouse_event;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ gui_x11_check_copy_area(ਖਾਲੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
ਸਥਿਰ ਖਾਲੀ gui_x11_send_event_handler(Widget, Xt_ਸੂਚਕ, XEvent *, Boolean *);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ gui_x11_wm_protocol_handler(Widget, Xt_ਸੂਚਕ, XEvent *, Boolean *);
ਸਥਿਰ Cursor gui_x11_create_blank_mouse(ਖਾਲੀ);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ਸਥਿਰ ਬਣਤਰ specialkey
{
    ਕੁੰਜੀ_Sym  ਕੁੰਜੀ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ਕੁੰਜੀਆਂ[] =
{
    {XK_ਉੱਪਰ,		'k', 'u'},
    {XK_ਥੱਲੇ,		'k', 'd'},
    {XK_ਖੱਬਾ,		'k', 'l'},
    {XK_ਸੱਜਾ,		'k', 'r'},

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#ਜੇ_ਅੰਤ

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_ਪਿੱਠ_Space,	'k', 'b'},
    {XK_ਪਾਓ,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_ਸਿਰਾ,		'@', '7'},
    {XK_ਪੂਰਵ,		'k', 'P'},
    {XK_ਅਗਲਾ,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ XK_KP_ਖੱਬਾ
    {XK_KP_ਖੱਬਾ,	'k', 'l'},
    {XK_KP_ਸੱਜਾ,	'k', 'r'},
    {XK_KP_ਉੱਪਰ,		'k', 'u'},
    {XK_KP_ਥੱਲੇ,	'k', 'd'},
    {XK_KP_ਪਾਓ,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_ਸਿਰਾ,		'K', '4'},
    {XK_KP_ਪੂਰਵ,	'K', '3'},
    {XK_KP_ਅਗਲਾ,	'K', '5'},

    {XK_KP_ਜੋੜੋ,		'K', '6'},
    {XK_KP_ਘਟਾਓ,	'K', '7'},
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
#ਜੇ_ਅੰਤ

    // End of list marker:
    {(ਕੁੰਜੀ_Sym)0,	    0, 0}
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNboldFont		"boldFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCBoldFont		"BoldFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNitalicFont		"italicFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCItalicFont		"ItalicFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNboldItalicFont	"boldItalicFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCBoldItalicFont	"BoldItalicFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Nscrollbar_ਚੌੜਾਈ	"scrollbarWidth"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_CScrollbar_ਚੌੜਾਈ	"ScrollbarWidth"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Nmenu_ਉਚਾਈ		"menuHeight"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_CMenu_ਉਚਾਈ		"MenuHeight"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNmenuFont		"menuFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCMenuFont		"MenuFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_Nmenu_Font_ਸੈੱਟ		"menuFontSet"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Xt_CMenu_Font_ਸੈੱਟ		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNmenuBackground	"menuBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCMenuBackground	"MenuBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNmenuForeground	"menuForeground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNscrollBackground	"scrollBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCScrollBackground	"ScrollBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNscrollForeground	"scrollForeground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNtooltipBackground	"tooltipBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCTooltipBackground	"TooltipBackground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNtooltipForeground	"tooltipForeground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCTooltipForeground	"TooltipForeground"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtNtooltipFont		"tooltipFont"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
ਸਥਿਰ XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	ਆਕਾਰ(Pixel),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	ਆਕਾਰ(Pixel),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, def_ਪਿੱਠ_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	ਆਕਾਰ(String *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	ਆਕਾਰ(String *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	ਆਕਾਰ(String *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	ਆਕਾਰ(String *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	ਆਕਾਰ(String *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	ਆਕਾਰ(Bool),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_rev_video),
	XtRImmediate,
	(Xt_ਸੂਚਕ)False
    },
    {
	Xt_Nborder_ਚੌੜਾਈ,
	Xt_CBorder_ਚੌੜਾਈ,
	XtRInt,
	ਆਕਾਰ(ਪੂਰਨ),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, border_ਚੌੜਾਈ),
	XtRImmediate,
	(Xt_ਸੂਚਕ)2
    },
    {
	Xt_Nscrollbar_ਚੌੜਾਈ,
	Xt_CScrollbar_ਚੌੜਾਈ,
	XtRInt,
	ਆਕਾਰ(ਪੂਰਨ),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, scrollbar_ਚੌੜਾਈ),
	XtRImmediate,
	(Xt_ਸੂਚਕ)SB_DEFAULT_ਚੌੜਾਈ
    },
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	Xt_Nmenu_Font_ਸੈੱਟ,
	Xt_CMenu_Font_ਸੈੱਟ,
# ਨਹੀਂਤਾਂ
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#ਜੇ_ਅੰਤ
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ *),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	Xt_CFont_ਸੈੱਟ,
	Xt_RFont_ਸੈੱਟ,
	ਆਕਾਰ(XFont_ਸੈੱਟ),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, tooltip_fontset),
	XtRImmediate,
	(Xt_ਸੂਚਕ)NOFONTSET
    },
#ਜੇ_ਅੰਤ // FEAT_BEVAL_GUI
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ*),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_preedit_type_name),
	XtRString,
	(Xt_ਸੂਚਕ)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	ਆਕਾਰ(ਅੱਖਰ*),
	Xt_ਵਿੱਥ_ਦਾ(gui_T, rsrc_ਨਿਵੇਸ਼_ਵਿਧੀ),
	XtRString,
	ਨਿਰਾ
    },
#ਜੇ_ਅੰਤ // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
ਸਥਿਰ XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-background",	".background",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-fn",		".font",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-font",		".font",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ਨਿਰਾ},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ਨਿਰਾ},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ਨਿਰਾ},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ਨਿਰਾ},
#ਨਹੀਂਤਾਂ
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ਨਿਰਾ},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ਨਿਰਾ},
#ਜੇ_ਅੰਤ
    {"-xrm",		ਨਿਰਾ,		    XrmoptionResArg,	ਨਿਰਾ}
};

ਸਥਿਰ ਪੂਰਨ gui_argc = 0;
ਸਥਿਰ ਅੱਖਰ **gui_argv = ਨਿਰਾ;

/*
 * Call-back routines.
 */

    ਸਥਿਰ ਖਾਲੀ
gui_x11_timer_cb(
    Xt_ਸੂਚਕ	    timed_ਬਾਹਰਲਾ,
    XtIntervalId    *interval_id ਅਣਵਰਤਿਆ)
{
    *((ਪੂਰਨ *)timed_ਬਾਹਰਲਾ) = TRUE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਸਥਿਰ ਖਾਲੀ
channel_poll_cb(
    Xt_ਸੂਚਕ	    client_ਅੰਕੜੇ_2,
    XtIntervalId    *interval_id ਅਣਵਰਤਿਆ)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_ਅੰਕੜੇ_2;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, (long_u)20,
						 channel_poll_cb, client_ਅੰਕੜੇ_2);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
gui_x11_visibility_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	dud ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    ਜੇ (event->type != VisibilityNotify)
	ਵਾਪਸ;

    gui.visibility = event->xvisibility.state;

    /*
     * When we do an XCopyArea(), and the window is partially obscured, we want
     * to receive an event to tell us whether it worked or not.
     */
    XSetGraphicsExposures(gui.dpy, gui.ਪਾਠ_gc,
	    gui.visibility != VisibilityUnobscured);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_expose_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	dud ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    XExposeEvent	*gevent;
    ਪੂਰਨ			new_x;

    ਜੇ (event->type != Expose)
	ਵਾਪਸ;

    ਬਾਹਰਲਾ_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->ਚੌੜਾਈ, gevent->ਉਚਾਈ);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    ਜੇ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    ਜੇ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    ਜੇ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((ਪੂਰਨ)Columns), 0, 0, 0, False);
    ਜੇ (gevent->y > FILL_Y(ਪੰਕਤੀਆਂ))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#ਜੇ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    ਸਥਿਰ ਖਾਲੀ
shellRectangle(Widget shell, XRectangle *r)
{
    ਖਿੜਕੀ		rootw, shellw, ਬੱਚਾ, parentw;
    ਪੂਰਨ			absx, absy;
    XWindow_ਗੁਣ	a;
    ਖਿੜਕੀ		*children;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	children_ਗਿਣਤੀ;

    shellw = Xt_ਖਿੜਕੀ(shell);
    ਜੇ (shellw == 0)
	ਵਾਪਸ;
    ਲਈ (;;)
    {
	XQuery_ਰੁੱਖ(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_ਗਿਣਤੀ);
	XFree(children);
	ਜੇ (parentw == rootw)
	    ਰੋਕੋ;
	shellw = parentw;
    }
    XGet_ਖਿੜਕੀ_ਗੁਣ(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.ਜੜ੍ਹ, 0, 0,
							&absx, &absy, &ਬੱਚਾ);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_ਮੁੱਲਾਂ(shell, XmNheight, &r->ਉਚਾਈ, XmNwidth, &r->ਚੌੜਾਈ, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
gui_x11_resize_ਖਿੜਕੀ_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	dud ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_ਚੌੜਾਈ_2, ਆਖਰੀ_ਉਚਾਈ_2;

    ਜੇ (event->type != ConfigureNotify)
	ਵਾਪਸ;

    ਜੇ (event->xconfigure.ਚੌੜਾਈ != ਆਖਰੀ_ਚੌੜਾਈ_2
	    || event->xconfigure.ਉਚਾਈ != ਆਖਰੀ_ਉਚਾਈ_2)
    {
	ਆਖਰੀ_ਚੌੜਾਈ_2 = event->xconfigure.ਚੌੜਾਈ;
	ਆਖਰੀ_ਉਚਾਈ_2 = event->xconfigure.ਉਚਾਈ;
	gui_resize_shell(event->xconfigure.ਚੌੜਾਈ, event->xconfigure.ਉਚਾਈ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
						- xim_get_ਸਥਿਤੀ_area_ਉਚਾਈ()
#ਜੇ_ਅੰਤ
		     );
    }
#ਜੇ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    ਜੇ (netbeans_ਸਰਗਰਮ())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_ਚੌਖਟਾ_moved(rec.x, rec.y);
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    xim_ਸੈੱਟ_preedit();
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_focus_change_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    gui_focus_change(event->type == Focus_ਵਿੱਚ);
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_enter_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    XEvent	*event ਅਣਵਰਤਿਆ,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    gui_focus_change(TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_leave_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    XEvent	*event ਅਣਵਰਤਿਆ,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    gui_focus_change(FALSE);
}

#ਜੇ defined(X_HAVE_UTF8_STRING)
# ਜੇ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_x11_ਕੁੰਜੀ_ਪ੍ਰਾਪਤੀ_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	dud ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    XKeyPressedEvent	*ev_press;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    ਸਥਿਤੀ_2		ਸਥਿਤੀ;
#ਨਹੀਂਤਾਂ
    char_u		string[4], string2[3];
#ਜੇ_ਅੰਤ
    ਕੁੰਜੀ_Sym		ਕੁੰਜੀ_sym;
    ਪੂਰਨ			len;
    ਪੂਰਨ			i;
    ਪੂਰਨ			modifiers;
    ਪੂਰਨ			ਕੁੰਜੀ;

    ev_press = (XKeyPressedEvent *)event;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਜੇ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	ਜੇ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (ਅੱਖਰ *)string,
				  ਆਕਾਰ(string_shortbuf), &ਕੁੰਜੀ_sym, &ਸਥਿਤੀ);
	ਨਹੀਂਤਾਂ
# endif
	    len = XmbLookupString(xic, ev_press, (ਅੱਖਰ *)string,
				  ਆਕਾਰ(string_shortbuf), &ਕੁੰਜੀ_sym, &ਸਥਿਤੀ);
	ਜੇ (ਸਥਿਤੀ == XBuffer_ਉਛਾਲ)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    ਜੇ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (ਅੱਖਰ *)string,
						      len, &ਕੁੰਜੀ_sym, &ਸਥਿਤੀ);
	    ਨਹੀਂਤਾਂ
# endif
		len = XmbLookupString(xic, ev_press, (ਅੱਖਰ *)string,
						      len, &ਕੁੰਜੀ_sym, &ਸਥਿਤੀ);
	}
	ਜੇ (ਸਥਿਤੀ == XLookupNone || ਸਥਿਤੀ == XLookupChars)
	    ਕੁੰਜੀ_sym = XK_Void_ਚਿੰਨ੍ਹ;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	ਜੇ (len > 0 && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    ਪੂਰਨ		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    ਜੇ (string_alloced)
		XtFree((ਅੱਖਰ *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_ਨਿਵੇਸ਼(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	ਲਈ (i = 0; i < len; ++i)
	    ਜੇ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (ਪੂਰਨ)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		ਜੇ (string_alloced)
		    XtFree((ਅੱਖਰ *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	len = XLookupString(ev_press, (ਅੱਖਰ *)string, ਆਕਾਰ(string),
		&ਕੁੰਜੀ_sym, ਨਿਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    ਜੇ (ਕੁੰਜੀ_sym == SunXK_F36 || ਕੁੰਜੀ_sym == SunXK_F37)
	len = 0;
#ਜੇ_ਅੰਤ

    ਜੇ (ਕੁੰਜੀ_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    ਜੇ (ਕੁੰਜੀ_sym == XK_minus && (ev_press->state & Control_ਨਕਾਬ))
	string[0] = Ctrl__;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ XK_ISO_ਖੱਬਾ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    ਜੇ (ਕੁੰਜੀ_sym == XK_ISO_ਖੱਬਾ_Tab)
    {
	ਕੁੰਜੀ_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#ਜੇ_ਅੰਤ

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    ਜੇ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (ਪੂਰਨ)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ਜੇ (len == 0 || len == 1)
    {
	ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != (ਕੁੰਜੀ_Sym)0; i++)
	{
	    ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == ਕੁੰਜੀ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ਕੁੰਜੀਆਂ[i].vim_code0;
		string[2] = special_ਕੁੰਜੀਆਂ[i].vim_code1;
		len = -3;
		ਰੋਕੋ;
	    }
	}
    }

    // Unrecognised key is ignored.
    ਜੇ (len == 0)
	ਜਾਓ theend;

    // Handle modifiers.
    modifiers = 0;
    ਜੇ (ev_press->state & ਖਿਸਕਾਓ_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (ev_press->state & Control_ਨਕਾਬ)
    {
	modifiers |= MOD_ਨਕਾਬ_CTRL;
	ਜੇ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    ਜੇ (ev_press->state & Mod1_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ALT;
    ਜੇ (ev_press->state & Mod4_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    ਜੇ (len == -3)
	ਕੁੰਜੀ = ਮੰਜ਼ਿਲ_SPECIAL(string[1], string[2]);
    ਨਹੀਂਤਾਂ
    {
	string[len] = NUL;
	ਕੁੰਜੀ = mb_ptr2char(string);
    }
    ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);
    ਜੇ (ਕੁੰਜੀ == CSI)
	ਕੁੰਜੀ = K_CSI;
    ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
    {
	string[0] = CSI;
	string[1] = K_ਦੂਜਾ(ਕੁੰਜੀ);
	string[2] = K_ਤੀਜਾ(ਕੁੰਜੀ);
	len = 3;
    }
    ਨਹੀਂਤਾਂ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ਕੁੰਜੀ = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, ਕੁੰਜੀ);

	len = mb_char2bytes(ਕੁੰਜੀ, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ਕੁੰਜੀ);
    }

    ਜੇ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	ਪੂਰਨ int_ch = check_for_ਰੁਕਾਵਟ(ਕੁੰਜੀ, modifiers);

	ਜੇ (int_ch != NUL)
	{
	    trash_ਨਿਵੇਸ਼_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    ਜੇ (p_mh)
	gui_mch_mousehide(TRUE);

#ਜੇ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	ਜੇ ((be = gui_mch_currently_showing_beval()) != ਨਿਰਾ)
	    gui_mch_unpost_balloon(be);
    }
#ਜੇ_ਅੰਤ
theend:
    {}	    // some compilers need a statement here
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਜੇ (string_alloced)
	XtFree((ਅੱਖਰ *)string);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_mouse_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	dud ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ XtIntervalId timer = (XtIntervalId)0;
    ਸਥਿਰ ਪੂਰਨ	timed_ਬਾਹਰਲਾ = TRUE;

    ਪੂਰਨ		button;
    ਪੂਰਨ		repeated_click = FALSE;
    ਪੂਰਨ		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    ਜੇ (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1_ਨਕਾਬ | Button2_ਨਕਾਬ | Button3_ਨਕਾਬ))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	ਜੇ (button != MOUSE_DRAG)	// just moving the rodent
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
	    ਜੇ (dud)			// moved in vimForm
		y -= gui.menu_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
	    gui_mouse_moved(x, y);
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	ਜੇ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    ਜੇ (!timed_ਬਾਹਰਲਾ)
	    {
		Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(timer);
		repeated_click = TRUE;
	    }
	    timed_ਬਾਹਰਲਾ = FALSE;
	    timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_ਬਾਹਰਲਾ);
	    ਚੋਣ (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		ਹਾਲਤ Button1:	button = MOUSE_ਖੱਬਾ;	ਰੋਕੋ;
		ਹਾਲਤ Button2:	button = MOUSE_ਵਿਚਕਾਰ;	ਰੋਕੋ;
		ਹਾਲਤ Button3:	button = MOUSE_ਸੱਜਾ;	ਰੋਕੋ;
		ਹਾਲਤ Button4:	button = MOUSE_4;	ਰੋਕੋ;
		ਹਾਲਤ Button5:	button = MOUSE_5;	ਰੋਕੋ;
		ਹਾਲਤ 6:		button = MOUSE_7;	ਰੋਕੋ;
		ਹਾਲਤ 7:		button = MOUSE_6;	ਰੋਕੋ;
		ਹਾਲਤ 8:		button = MOUSE_X1;	ਰੋਕੋ;
		ਹਾਲਤ 9:		button = MOUSE_X2;	ਰੋਕੋ;
		ਮੂਲ:
		    ਵਾਪਸ;	// Unknown button
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	ਆਖਰੀ_mouse_event = event->xbutton;
#ਜੇ_ਅੰਤ
    }

    vim_modifiers = 0x0;
    ਜੇ (x_modifiers & ਖਿਸਕਾਓ_ਨਕਾਬ)
	vim_modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (x_modifiers & Control_ਨਕਾਬ)
	vim_modifiers |= MOUSE_CTRL;
    ਜੇ (x_modifiers & Mod1_ਨਕਾਬ)	    // Alt or Meta key
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
    ਖਾਲੀ
gui_mch_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    ਪੂਰਨ	    arg;
    ਪੂਰਨ	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(ਅੱਖਰ *, *argc);
    ਜੇ (gui_argv == ਨਿਰਾ)
	ਵਾਪਸ;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    ਜਦੋਂ (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	ਲਈ (i = 0; i < (ਪੂਰਨ)Xt_ਸੰਖਿਆ(cmdline_options); i++)
	    ਜੇ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		ਰੋਕੋ;

	ਜੇ (i < (ਪੂਰਨ)Xt_ਸੰਖਿਆ(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    ਜੇ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		ਮਿਲਿਆ_ਉਲਟ_arg = TRUE;
	    ਨਹੀਂਤਾਂ ਜੇ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_ਦਲੀਲ = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    ਜੇ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * ਆਕਾਰ(ਅੱਖਰ *));
		ਜੇ (cmdline_options[i].arg_ਕਿਸਮ != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    ਜੇ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * ਆਕਾਰ(ਅੱਖਰ *));
		}
	    }
	    argv[*argc] = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
	    ਜੇ (ਲੜੀ_ਤੁਲਨਾ("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ਆਕਾਰ(ਅੱਖਰ *));
	    argv[*argc] = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    arg++;
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XtSpecificationRelease
# define CARDINAL (Cardinal *)
#ਨਹੀਂਤਾਂ
# ਜੇ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# ਨਹੀਂਤਾਂ
#  define CARDINAL (ਪੂਰਨ *)
# endif
#ਜੇ_ਅੰਤ

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    ਪੂਰਨ
gui_mch_init_check(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    Xt_ਸੈੱਟ_Language_Proc(ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    open_app_context();
    ਜੇ (app_context != ਨਿਰਾ)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_ਸੰਖਿਆ(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#ਜੇ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	ਅੱਖਰ *p = setlocale(LC_NUMERIC, ਨਿਰਾ);

	ਜੇ (p == ਨਿਰਾ || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#ਜੇ_ਅੰਤ
    ਜੇ (app_context == ਨਿਰਾ || gui.dpy == ਨਿਰਾ)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ਸਥਿਰ Xt_ਨਿਵੇਸ਼_Id _xsmp_xtinputid;

    ਸਥਿਰ ਖਾਲੀ
local_xsmp_handle_ਬੇਨਤੀਆਂ(
    Xt_ਸੂਚਕ	c ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*s ਅਣਵਰਤਿਆ,
    Xt_ਨਿਵੇਸ਼_Id	*i ਅਣਵਰਤਿਆ)
{
    ਜੇ (xsmp_handle_ਬੇਨਤੀਆਂ() == FAIL)
	Xt_Remove_ਨਿਵੇਸ਼(_xsmp_xtinputid);
}
#ਜੇ_ਅੰਤ


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ਪੂਰਨ
gui_mch_init(ਖਾਲੀ)
{
    XtGCMask	gc_ਨਕਾਬ;
    XGCValues	gc_vals;
    ਪੂਰਨ		x, y, ਨਕਾਬ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	w, h;

#ਜੇ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#ਜੇ_ਅੰਤ

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ਨਿਰਾ);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (Xt_ਸੂਚਕ)&gui,
	vim_resources, Xt_ਸੰਖਿਆ(vim_resources), ਨਿਰਾ);

    gui.scrollbar_ਉਚਾਈ = gui.scrollbar_ਚੌੜਾਈ;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#ਜੇ_ਅੰਤ

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ਪਿੱਠ_pixel = gui.def_ਪਿੱਠ_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    ਜੇ (gui.rsrc_rev_video && gui_get_lightness(gui.ਪਿੱਠ_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_ਪਿੱਠ_pixel;
	gui.ਪਿੱਠ_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_ਪਿੱਠ_pixel = gui.ਪਿੱਠ_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_ਨਕਾਬ = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.ਪਿੱਠ_pixel;
    gui.ਪਾਠ_gc = XtGetGC(vimShell, gc_ਨਕਾਬ, &gc_vals);

    gc_vals.foreground = gui.ਪਿੱਠ_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.ਪਿੱਠ_gc = XtGetGC(vimShell, gc_ਨਕਾਬ, &gc_vals);

    gc_ਨਕਾਬ |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_ਨਕਾਬ, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_ਤਿਆਰੀ_atoms(gui.dpy);

    ਜੇ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_ਸੈੱਟ_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    ਜੇ (gui.geom != ਨਿਰਾ && *gui.geom != NUL)
    {
	ਨਕਾਬ = XParseGeometry((ਅੱਖਰ *)gui.geom, &x, &y, &w, &h);
	ਜੇ (ਨਕਾਬ & ਚੌੜਾਈ_ਮੁੱਲ)
	    Columns = w;
	ਜੇ (ਨਕਾਬ & ਉਚਾਈ_ਮੁੱਲ)
	{
	    ਜੇ (p_ਖਿੜਕੀ > (ਲੰਮਾ)h - 1 || !option_was_ਸੈੱਟ((char_u *)"window"))
		p_ਖਿੜਕੀ = h - 1;
	    ਪੰਕਤੀਆਂ = h;
	}
	ਹੱਦਬੰਦੀ_screen_ਮਾਪ();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	ਜੇ (ਨਕਾਬ & (XValue|YValue))
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell, XtNgeometry, gui.geom, ਨਿਰਾ);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    ਜੇ (vim_strchr(p_go, GO_ICON) != ਨਿਰਾ)
    {
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	Xt_ਸੈੱਟ_Arg(arg[0], XtNiconPixmap,
		XCreate_Bitmap_From_ਅੰਕੜੇ(gui.dpy,
		    Default_ਜੜ੍ਹ_ਖਿੜਕੀ(gui.dpy),
		    (ਅੱਖਰ *)vim_icon_ਬਿੱਟ,
		    vim_icon_ਚੌੜਾਈ,
		    vim_icon_ਉਚਾਈ));
	Xt_ਸੈੱਟ_Arg(arg[1], Xt_Nicon_ਨਕਾਬ,
		XCreate_Bitmap_From_ਅੰਕੜੇ(gui.dpy,
		    Default_ਜੜ੍ਹ_ਖਿੜਕੀ(gui.dpy),
		    (ਅੱਖਰ *)vim_ਨਕਾਬ_icon_ਬਿੱਟ,
		    vim_ਨਕਾਬ_icon_ਚੌੜਾਈ,
		    vim_ਨਕਾਬ_icon_ਉਚਾਈ));
	Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell, arg, (Cardinal)2);
#ਨਹੀਂਤਾਂ
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ਸਥਿਰ Pixmap	icon = 0;
    ਸਥਿਰ Pixmap	icon_ਨਕਾਬ = 0;
    ਸਥਿਰ ਅੱਖਰ		**magick = vim32x32;
    ਖਿੜਕੀ		ਜੜ੍ਹ_ਖਿੜਕੀ;
    XIcon_ਮਾਪ		*ਮਾਪ;
    ਪੂਰਨ			ਸੰਖਿਆ_sizes;
    Display		*dsp;
    Screen		*scr;
    Xpm_ਗੁਣ	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    ਜੜ੍ਹ_ਖਿੜਕੀ = XRoot_ਖਿੜਕੀ_ਦਾ_Screen(XtScreen(vimShell));
    ਜੇ (XGetIconSizes(XtDisplay(vimShell), ਜੜ੍ਹ_ਖਿੜਕੀ,
						   &ਮਾਪ, &ਸੰਖਿਆ_sizes) != 0)
    {
	ਜੇ (ਸੰਖਿਆ_sizes > 0)
	{
	    ਜੇ (ਮਾਪ->max_ਉਚਾਈ_2 >= 48 && ਮਾਪ->max_ਚੌੜਾਈ_2 >= 48)
		magick = vim48x48;
	    ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ->max_ਉਚਾਈ_2 >= 32 && ਮਾਪ->max_ਚੌੜਾਈ_2 >= 32)
		magick = vim32x32;
	    ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪ->max_ਉਚਾਈ_2 >= 16 && ਮਾਪ->max_ਚੌੜਾਈ_2 >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell, XtNcolormap, cmap, ਨਿਰਾ);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | Xpm_ਡੂੰਘਾਈ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.ਡੂੰਘਾਈ = Default_ਡੂੰਘਾਈ_ਦਾ_Screen(scr);

    ਜੇ (!icon)
    {
	Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dsp, ਜੜ੍ਹ_ਖਿੜਕੀ, magick, &icon,
							   &icon_ਨਕਾਬ, &attr);
	Xpm_Free_ਗੁਣ(&attr);
    }

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell, XmNiconPixmap, icon, Xm_Nicon_ਨਕਾਬ, icon_ਨਕਾਬ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    gui_init_menu_font();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
    // Attach listener on ICE connection
    ਜੇ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_ਜੋੜੋ_ਨਿਵੇਸ਼(app_context, xsmp_icefd,
		(Xt_ਸੂਚਕ)Xt_ਨਿਵੇਸ਼_Read_ਨਕਾਬ, local_xsmp_handle_ਬੇਨਤੀਆਂ, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    ਖਾਲੀ
gui_mch_uninit(ਖਾਲੀ)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ਨਿਰਾ;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    ਖਾਲੀ
gui_mch_new_colors(ਖਾਲੀ)
{
    long_u	gc_ਨਕਾਬ;
    XGCValues	gc_vals;

    gc_ਨਕਾਬ = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.ਪਿੱਠ_pixel;
    ਜੇ (gui.ਪਾਠ_gc != ਨਿਰਾ)
	XChangeGC(gui.dpy, gui.ਪਾਠ_gc, gc_ਨਕਾਬ, &gc_vals);

    gc_vals.foreground = gui.ਪਿੱਠ_pixel;
    gc_vals.background = gui.norm_pixel;
    ਜੇ (gui.ਪਿੱਠ_gc != ਨਿਰਾ)
	XChangeGC(gui.dpy, gui.ਪਿੱਠ_gc, gc_ਨਕਾਬ, &gc_vals);

    gc_ਨਕਾਬ |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    gc_vals.function   = GXxor;
    ਜੇ (gui.invert_gc != ਨਿਰਾ)
	XChangeGC(gui.dpy, gui.invert_gc, gc_ਨਕਾਬ, &gc_vals);

    gui_x11_ਸੈੱਟ_ਪਿੱਠ_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ਪੂਰਨ
gui_mch_open(ਖਾਲੀ)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    Xt_Manage_ਬੱਚਾ(Xt_Name_ਮੰਜ਼ਿਲ_Widget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_ਸੂਚਕ = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_ਖਿੜਕੀ_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, Xt_ਖਿੜਕੀ(vimShell), wm_atoms, 2);
    Xt_ਜੋੜੋ_Event_Handler(vimShell, No_Event_ਨਕਾਬ, True, gui_x11_wm_protocol_handler,
							     ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    Xt_ਜੋੜੋ_Event_Handler(vimShell, (Event_ਨਕਾਬ)0, True, _XEditResCheckMessages,
	    (Xt_ਸੂਚਕ)ਨਿਰਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
    ਜੇ (serverName == ਨਿਰਾ && server_Delayed_ਸ਼ੁਰੂ_Name != ਨਿਰਾ)
    {
	// This is a :gui command in a plain vim with no previous server
	comm_ਖਿੜਕੀ = Xt_ਖਿੜਕੀ(vimShell);
	(ਖਾਲੀ)server_ਰਜਿਸਟਰ_Name(gui.dpy, server_Delayed_ਸ਼ੁਰੂ_Name);
    }
    ਨਹੀਂਤਾਂ
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	server_Change_Registered_ਖਿੜਕੀ(gui.dpy, Xt_ਖਿੜਕੀ(vimShell));
    }
    Xt_ਜੋੜੋ_Event_Handler(vimShell, Property_Change_ਨਕਾਬ, False,
		      gui_x11_send_event_handler, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    xim_init();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    ਖਾਲੀ
gui_init_tooltip_font(ਖਾਲੀ)
{
    Xrm_ਮੁੱਲ from, ਮੰਜ਼ਿਲ;

    from.addr = (ਅੱਖਰ *)gui.rsrc_tooltip_font_name;
    from.ਮਾਪ = ਲੜੀ_ਲੰਬਾਈ(from.addr);
    ਮੰਜ਼ਿਲ.addr = (Xt_ਸੂਚਕ)&gui.tooltip_fontset;
    ਮੰਜ਼ਿਲ.ਮਾਪ = ਆਕਾਰ(XFont_ਸੈੱਟ);

    ਜੇ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_ਸੈੱਟ, &ਮੰਜ਼ਿਲ) == False)
    {
	// Failed. What to do?
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    ਖਾਲੀ
gui_init_menu_font(ਖਾਲੀ)
{
    Xrm_ਮੁੱਲ from, ਮੰਜ਼ਿਲ;

# ifdef FONTSET_ALWAYS
    from.addr = (ਅੱਖਰ *)gui.rsrc_menu_font_name;
    from.ਮਾਪ = ਲੜੀ_ਲੰਬਾਈ(from.addr);
    ਮੰਜ਼ਿਲ.addr = (Xt_ਸੂਚਕ)&gui.menu_fontset;
    ਮੰਜ਼ਿਲ.ਮਾਪ = ਆਕਾਰ(GuiFontset);

    ਜੇ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_ਸੈੱਟ, &ਮੰਜ਼ਿਲ) == False)
    {
	// Failed. What to do?
    }
# ਨਹੀਂਤਾਂ
    from.addr = (ਅੱਖਰ *)gui.rsrc_menu_font_name;
    from.ਮਾਪ = ਲੜੀ_ਲੰਬਾਈ(from.addr);
    ਮੰਜ਼ਿਲ.addr = (Xt_ਸੂਚਕ)&gui.menu_font;
    ਮੰਜ਼ਿਲ.ਮਾਪ = ਆਕਾਰ(GuiFont);

    ਜੇ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &ਮੰਜ਼ਿਲ) == False)
    {
	// Failed. What to do?
    }
# endif
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ rc ਅਣਵਰਤਿਆ)
{
#ਜੇ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#ਜੇ_ਅੰਤ
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_ਮੁੱਲਾਂ(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ਨਿਰਾ);
    *x = xpos;
    *y = ypos;
    ਵਾਪਸ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell,
	XtNx,	x,
	XtNy,	y,
	ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(
    ਪੂਰਨ		ਚੌੜਾਈ,
    ਪੂਰਨ		ਉਚਾਈ,
    ਪੂਰਨ		min_ਚੌੜਾਈ,
    ਪੂਰਨ		min_ਉਚਾਈ,
    ਪੂਰਨ		ਅਧਾਰ_ਚੌੜਾਈ,
    ਪੂਰਨ		ਅਧਾਰ_ਉਚਾਈ,
    ਪੂਰਨ		direction ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਉਚਾਈ += xim_get_ਸਥਿਤੀ_area_ਉਚਾਈ(),
#ਜੇ_ਅੰਤ
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell,
	XtNwidthInc,	gui.char_ਚੌੜਾਈ,
	XtNheightInc,	gui.char_ਉਚਾਈ,
#ਜੇ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	Xt_Nbase_ਚੌੜਾਈ,	ਅਧਾਰ_ਚੌੜਾਈ,
	Xt_Nbase_ਉਚਾਈ,	ਅਧਾਰ_ਉਚਾਈ,
#ਜੇ_ਅੰਤ
	Xt_Nmin_ਚੌੜਾਈ,	min_ਚੌੜਾਈ,
	Xt_Nmin_ਉਚਾਈ,	min_ਉਚਾਈ,
	XtNwidth,	ਚੌੜਾਈ,
	XtNheight,	ਉਚਾਈ,
	ਨਿਰਾ);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    ਖਾਲੀ
gui_mch_get_screen_dimensions(
    ਪੂਰਨ	    *screen_w,
    ਪੂਰਨ	    *screen_h)
{
    *screen_w = Display_ਚੌੜਾਈ(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = Display_ਉਚਾਈ(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    ਪੂਰਨ
gui_mch_init_font(
    char_u	*font_name,
    ਪੂਰਨ		do_fontset ਅਣਵਰਤਿਆ)
{
    XFontStruct	*font = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    XFont_ਸੈੱਟ	fontset = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    ਜੇ (font_name != ਨਿਰਾ && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	ਜੇ (font_name == ਨਿਰਾ && !gui.starting)
	    ਵਾਪਸ OK;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    ਜੇ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	ਜੇ (font_name != ਨਿਰਾ)
	{
	    fontset = (XFont_ਸੈੱਟ)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    ਜੇ (fontset == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	ਜੇ (font_name == ਨਿਰਾ)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    ਜੇ (font == ਨਿਰਾ)
		font_name = (char_u *)DFLT_FONT;
	}
	ਜੇ (font == ਨਿਰਾ)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	ਜੇ (font == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    ਜੇ (fontset != ਨਿਰਾ)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_ਚੌੜਾਈ = fontset_ਚੌੜਾਈ(fontset);
	gui.char_ਉਚਾਈ = fontset_ਉਚਾਈ(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	gui.norm_font = (GuiFont)font;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#ਜੇ_ਅੰਤ
	gui.char_ਚੌੜਾਈ = font->max_bounds.ਚੌੜਾਈ;
	gui.char_ਉਚਾਈ = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_ਸੈੱਟ_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    ਜੇ (font_name == gui.rsrc_font_name)
    {
	ਜੇ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ਨਿਰਾ
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	ਜੇ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ਨਿਰਾ
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	ਜੇ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ਨਿਰਾ
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    ਨਹੀਂਤਾਂ
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	ਜੇ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	ਜੇ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	ਜੇ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, ਪੂਰਨ giveErrorIfMissing)
{
    XFontStruct	*font;

    ਜੇ (!gui.ਵਿੱਚ_use || name == ਨਿਰਾ)	// can't do this when GUI not running
	ਵਾਪਸ NOFONT;

    font = XLoadQueryFont(gui.dpy, (ਅੱਖਰ *)name);

    ਜੇ (font == ਨਿਰਾ)
    {
	ਜੇ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ਵਾਪਸ NOFONT;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
    ਲਿਖੋ("Font Information for '%s':\n", name);
    ਲਿਖੋ("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.ਚੌੜਾਈ, font->ascent + font->descent,
	   font->ascent, font->descent);
    ਲਿਖੋ("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    ਲਿਖੋ("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    ਲਿਖੋ("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    ਲਿਖੋ("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.ਚੌੜਾਈ));
    ਲਿਖੋ("\n");
#ਜੇ_ਅੰਤ

    ਜੇ (font->max_bounds.ਚੌੜਾਈ != font->min_bounds.ਚੌੜਾਈ)
    {
	semsg(_(e_font_str_is_not_fixed_ਚੌੜਾਈ), name);
	XFreeFont(gui.dpy, font);
	ਵਾਪਸ NOFONT;
    }
    ਵਾਪਸ (GuiFont)font;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ਨਿਰਾ;

    ਜੇ (name != ਨਿਰਾ && font == ਨਿਰਾ)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    ਨਹੀਂਤਾਂ ਜੇ (font != ਨਿਰਾ)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਮੁੱਲ = 0L;

	ਜੇ (XGetFontProperty(font, XA_FONT, &ਮੁੱਲ))
	{
	    ਅੱਖਰ *xa_font_name = ਨਿਰਾ;

	    xa_font_name = XGetAtomName(gui.dpy, ਮੁੱਲ);
	    ਜੇ (xa_font_name != ਨਿਰਾ)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (name != ਨਿਰਾ)
		ret = vim_strsave(name);
	}
	ਨਹੀਂਤਾਂ ਜੇ (name != ਨਿਰਾ)
	    ret = vim_strsave(name);
    }
    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ਪੂਰਨ
gui_mch_adjust_charheight(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    ਜੇ (gui.fontset != NOFONTSET)
    {
	gui.char_ਉਚਾਈ = fontset_ਉਚਾਈ((XFont_ਸੈੱਟ)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFont_ਸੈੱਟ)gui.fontset)
							    + p_linespace / 2;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_ਉਚਾਈ = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    ਵਾਪਸ OK;
}

/*
 * Set the current text font.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_font(GuiFont font)
{
    ਸਥਿਰ Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    ਜੇ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.ਪਾਠ_gc, fid);
	XSetFont(gui.dpy, gui.ਪਿੱਠ_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    current_fontset = (XFont_ਸੈੱਟ)ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fontset(GuiFontset fontset)
{
    current_fontset = (XFont_ਸੈੱਟ)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#ਜੇ_ਅੰਤ

/*
 * If a font is not going to be used, free its structure.
 */
    ਖਾਲੀ
gui_mch_free_font(GuiFont font)
{
    ਜੇ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#ਜੇ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    ਖਾਲੀ
gui_mch_free_fontset(GuiFontset fontset)
{
    ਜੇ (fontset != NOFONTSET)
	XFree_Font_ਸੈੱਟ(gui.dpy, (XFont_ਸੈੱਟ)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    ਪੂਰਨ		giveErrorIfMissing,
    ਪੂਰਨ		fixed_ਚੌੜਾਈ)
{
    XFont_ਸੈੱਟ	fontset;
    ਅੱਖਰ	**missing, *def_str;
    ਪੂਰਨ		ਅੰਕ_missing;

    ਜੇ (!gui.ਵਿੱਚ_use || name == ਨਿਰਾ)
	ਵਾਪਸ NOFONTSET;

    fontset = XCreate_Font_ਸੈੱਟ(gui.dpy, (ਅੱਖਰ *)name, &missing, &ਅੰਕ_missing,
			     &def_str);
    ਜੇ (ਅੰਕ_missing > 0)
    {
	ਪੂਰਨ i;

	ਜੇ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_ਵਿੱਚ_fontset), name);
	    ਲਈ (i = 0; i < ਅੰਕ_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    ਜੇ (fontset == ਨਿਰਾ)
    {
	ਜੇ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	ਵਾਪਸ NOFONTSET;
    }

    ਜੇ (fixed_ਚੌੜਾਈ && check_fontset_sanity(fontset) == FAIL)
    {
	XFree_Font_ਸੈੱਟ(gui.dpy, fontset);
	ਵਾਪਸ NOFONTSET;
    }
    ਵਾਪਸ (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ਸਥਿਰ ਪੂਰਨ
check_fontset_sanity(XFont_ਸੈੱਟ fs)
{
    XFontStruct	**xfs;
    ਅੱਖਰ	**font_name;
    ਪੂਰਨ		fn;
    ਅੱਖਰ	*ਅਧਾਰ_name;
    ਪੂਰਨ		i;
    ਪੂਰਨ		min_ਚੌੜਾਈ;
    ਪੂਰਨ		min_font_idx = 0;

    ਅਧਾਰ_name = XBase_Font_Name_List_ਦਾ_Font_ਸੈੱਟ(fs);
    fn = XFonts_ਦਾ_Font_ਸੈੱਟ(fs, &xfs, &font_name);
    ਲਈ (i = 0; i < fn; i++)
    {
	ਜੇ (xfs[i]->max_bounds.ਚੌੜਾਈ != xfs[i]->min_bounds.ਚੌੜਾਈ)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_ਚੌੜਾਈ),
		    ਅਧਾਰ_name, font_name[i]);
	    ਵਾਪਸ FAIL;
	}
    }
    // scan base font width
    min_ਚੌੜਾਈ = 32767;
    ਲਈ (i = 0; i < fn; i++)
    {
	ਜੇ (xfs[i]->max_bounds.ਚੌੜਾਈ<min_ਚੌੜਾਈ)
	{
	    min_ਚੌੜਾਈ = xfs[i]->max_bounds.ਚੌੜਾਈ;
	    min_font_idx = i;
	}
    }
    ਲਈ (i = 0; i < fn; i++)
    {
	ਜੇ (	   xfs[i]->max_bounds.ਚੌੜਾਈ != 2 * min_ਚੌੜਾਈ
		&& xfs[i]->max_bounds.ਚੌੜਾਈ != min_ਚੌੜਾਈ)
	{
	    semsg(_(e_fontset_name_str), ਅਧਾਰ_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (ਪੂਰਨ)xfs[min_font_idx]->max_bounds.ਚੌੜਾਈ);
	    semsg(_("Font%d width: %d"), i, (ਪੂਰਨ)xfs[i]->max_bounds.ਚੌੜਾਈ);
	    ਵਾਪਸ FAIL;
	}
    }
    // it seems ok. Good Luck!!
    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
fontset_ਚੌੜਾਈ(XFont_ਸੈੱਟ fs)
{
 ਵਾਪਸ Xmb_ਪਾਠ_Escapement(fs, "Vim", 3) / 3;
}

    ਪੂਰਨ
fontset_ਉਚਾਈ(
    XFont_ਸੈੱਟ fs)
{
    XFont_ਸੈੱਟ_Extents *extents;

    extents = XExtents_ਦਾ_Font_ਸੈੱਟ(fs);
    ਵਾਪਸ extents->max_logical_extent.ਉਚਾਈ;
}

# ਜੇ 0
// NOT USED YET
    ਸਥਿਰ ਪੂਰਨ
fontset_descent(XFont_ਸੈੱਟ fs)
{
    XFont_ਸੈੱਟ_Extents *extents;

    extents = XExtents_ਦਾ_Font_ਸੈੱਟ (fs);
    ਵਾਪਸ extents->max_logical_extent.ਉਚਾਈ + extents->max_logical_extent.y;
}
# endif

    ਸਥਿਰ ਪੂਰਨ
fontset_ascent(XFont_ਸੈੱਟ fs)
{
    XFont_ਸੈੱਟ_Extents *extents;

    extents = XExtents_ਦਾ_Font_ਸੈੱਟ(fs);
    ਵਾਪਸ -extents->max_logical_extent.y;
}

#ਜੇ_ਅੰਤ // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    ਜੇ (!gui.ਵਿੱਚ_use || name == ਨਿਰਾ || *name == NUL)
	ਵਾਪਸ INVALCOLOR;

    requested = gui_get_color_cmn(name);
    ਜੇ (requested == INVALCOLOR)
	ਵਾਪਸ INVALCOLOR;

    ਵਾਪਸ gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
    XColor	ਉਪਲਬਧ;
    Colormap	colormap;

#ਜੇ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    ਅੱਖਰ	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, ਆਕਾਰ(spec), "#%.2x%.2x%.2x", r, g, b);
    ਜੇ (XParseColor(gui.dpy, colormap, (ਅੱਖਰ *)spec, &ਉਪਲਬਧ) != 0
	    && XAllocColor(gui.dpy, colormap, &ਉਪਲਬਧ) != 0)
	ਵਾਪਸ (guicolor_T)ਉਪਲਬਧ.pixel;
#ਜੇ_ਅੰਤ
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_ਖੇਤਰ(ਉਪਲਬਧ);
    ਉਪਲਬਧ.red = r << 8;
    ਉਪਲਬਧ.green = g << 8;
    ਉਪਲਬਧ.blue = b << 8;
    ਜੇ (XAllocColor(gui.dpy, colormap, &ਉਪਲਬਧ) != 0)
	ਵਾਪਸ (guicolor_T)ਉਪਲਬਧ.pixel;

    ਵਾਪਸ INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(guicolor_T color)
{
    ਜੇ (color == prev_fg_color)
	ਵਾਪਸ;

    XSetForeground(gui.dpy, gui.ਪਾਠ_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(guicolor_T color)
{
    ਜੇ (color == prev_bg_color)
	ਵਾਪਸ;

    XSetBackground(gui.dpy, gui.ਪਾਠ_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    ਸਥਿਰ Cursor
gui_x11_create_blank_mouse(ਖਾਲੀ)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0, (XGCValues*)0);

    ਜੇ (gc != ਨਿਰਾ)
    {
	XDraw_ਬਿੰਦੂ(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    ਵਾਪਸ XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_curl(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ cells)
{
    ਪੂਰਨ			i;
    ਪੂਰਨ			ਵਿੱਥ;
    ਸਥਿਰ ਅਚਲ ਪੂਰਨ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_sp_color);
    ਲਈ (i = FILL_X(ਖਾਨਾ); i < FILL_X(ਖਾਨਾ + cells); ++i)
    {
	ਵਿੱਥ = val[i % 8];
	XDraw_ਬਿੰਦੂ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, i,
						FILL_Y(ਪੰਕਤੀ + 1) - 1 - ਵਿੱਥ);
    }
    XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_fg_color);
}

    ਖਾਲੀ
gui_mch_draw_string(
    ਪੂਰਨ		ਪੰਕਤੀ,
    ਪੂਰਨ		ਖਾਨਾ,
    char_u	*s,
    ਪੂਰਨ		len,
    ਪੂਰਨ		flags)
{
    ਪੂਰਨ			cells = len;
    ਸਥਿਰ ਖਾਲੀ		*buf = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ		buflen = 0;
    char_u		*p;
    ਪੂਰਨ			wlen = 0;
    ਪੂਰਨ			c;

    ਜੇ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	ਜੇ (buflen < len)
	{
	    XtFree((ਅੱਖਰ *)buf);
	    buf = (ਖਾਲੀ *)XtMalloc(len * (ਆਕਾਰ(XChar2b) < ਆਕਾਰ(wchar_t)
					? ਆਕਾਰ(wchar_t) : ਆਕਾਰ(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	ਜਦੋਂ (p < s + len)
	{
	    c = utf_ptr2char(p);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	    ਜੇ (current_fontset != ਨਿਰਾ)
	    {
# ifdef SMALL_WCHAR_T
		ਜੇ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    {
		ਜੇ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (ਬਿਨਾਨਿਸ਼ਾਨੀ)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
    {
	cells = 0;
	ਲਈ (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    ਜੇ (current_fontset != ਨਿਰਾ)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.ਉਚਾਈ = gui.char_ਉਚਾਈ;
	clip.ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * cells + 1;
	XSetClipRectangles(gui.dpy, gui.ਪਾਠ_gc, FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ),
		&clip, 1, Unsorted);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (flags & DRAW_TRANSP)
    {
	ਜੇ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), buf, wlen);
	ਨਹੀਂਤਾਂ
	    XDrawString(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), (ਅੱਖਰ *)s, len);
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_linespace != 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
	    || current_fontset != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    )
    {
	XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.ਪਾਠ_gc, FILL_X(ਖਾਨਾ),
		FILL_Y(ਪੰਕਤੀ), gui.char_ਚੌੜਾਈ * cells, gui.char_ਉਚਾਈ);
	XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_fg_color);

	ਜੇ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), buf, wlen);
	ਨਹੀਂਤਾਂ
	    XDrawString(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), (ਅੱਖਰ *)s, len);
    }
    ਨਹੀਂਤਾਂ
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	ਜੇ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), buf, wlen);
	ਨਹੀਂਤਾਂ
	    XDrawImageString(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ), (ਅੱਖਰ *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    ਜੇ (flags & DRAW_BOLD)
    {
	ਜੇ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ) + 1,
		    ਪਾਠ_Y(ਪੰਕਤੀ), buf, wlen);
	ਨਹੀਂਤਾਂ
	    XDrawString(gui.dpy, gui.wid, gui.ਪਾਠ_gc, ਪਾਠ_X(ਖਾਨਾ) + 1,
		    ਪਾਠ_Y(ਪੰਕਤੀ), (ਅੱਖਰ *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    ਜੇ (flags & DRAW_UNDERC)
	draw_curl(ਪੰਕਤੀ, ਖਾਨਾ, cells);

    // Underline: draw a line at the bottom of the character cell.
    ਜੇ (flags & DRAW_UNDERL)
    {
	ਪੂਰਨ	y = FILL_Y(ਪੰਕਤੀ + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ਜੇ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_ਸਤਰ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, FILL_X(ਖਾਨਾ),
		y, FILL_X(ਖਾਨਾ + cells) - 1, y);
    }

    ਜੇ (flags & DRAW_STRIKE)
    {
	ਪੂਰਨ	y = FILL_Y(ਪੰਕਤੀ + 1) - gui.char_ਉਚਾਈ/2;

	XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_sp_color);
	XDraw_ਸਤਰ(gui.dpy, gui.wid, gui.ਪਾਠ_gc, FILL_X(ਖਾਨਾ),
		y, FILL_X(ਖਾਨਾ + cells) - 1, y);
	XSetForeground(gui.dpy, gui.ਪਾਠ_gc, prev_fg_color);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XFONTSET
    ਜੇ (current_fontset != ਨਿਰਾ)
	XSet_Clip_ਨਕਾਬ(gui.dpy, gui.ਪਾਠ_gc, None);
#ਜੇ_ਅੰਤ
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ਪੂਰਨ
gui_mch_haskey(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != (ਕੁੰਜੀ_Sym)0; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].vim_code0 &&
					 name[1] == special_ਕੁੰਜੀਆਂ[i].vim_code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ਪੂਰਨ
gui_get_x11_windis(ਖਿੜਕੀ *win, Display **dis)
{
    *win = Xt_ਖਿੜਕੀ(vimShell);
    *dis = gui.dpy;
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
gui_mch_beep(ਖਾਲੀ)
{
    XBell(gui.dpy, 0);
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ਪੂਰਨ)Columns) + gui.border_ਵਿੱਥ,
	    FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ) + gui.border_ਵਿੱਥ);
    XSync(gui.dpy, False);
    ui_delay((ਲੰਮਾ)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ਪੂਰਨ)Columns) + gui.border_ਵਿੱਥ,
	    FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ) + gui.border_ਵਿੱਥ);
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
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_ਚੌੜਾਈ, (nr) * gui.char_ਉਚਾਈ);
}

/*
 * Iconify the GUI window.
 */
    ਖਾਲੀ
gui_mch_iconify(ਖਾਲੀ)
{
    XIconify_ਖਿੜਕੀ(gui.dpy, Xt_ਖਿੜਕੀ(vimShell), DefaultScreen(gui.dpy));
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    XMapRaised(gui.dpy, Xt_ਖਿੜਕੀ(vimShell));
}
#ਜੇ_ਅੰਤ

/*
 * Draw a cursor without focus.
 */
    ਖਾਲੀ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ਪੂਰਨ		w = 1;

    ਜੇ (mb_lefthalve(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ))
	w = 2;
    gui_mch_ਸੈੱਟ_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.ਪਾਠ_gc, FILL_X(gui.ਖਾਨਾ),
	    FILL_Y(gui.ਪੰਕਤੀ), w * gui.char_ਚੌੜਾਈ - 1, gui.char_ਉਚਾਈ - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ਖਾਲੀ
gui_mch_draw_part_cursor(ਪੂਰਨ w, ਪੂਰਨ h, guicolor_T color)
{
    gui_mch_ਸੈੱਟ_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.ਪਾਠ_gc,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.ਖਾਨਾ + 1) - w :
#ਜੇ_ਅੰਤ
		FILL_X(gui.ਖਾਨਾ),
	    FILL_Y(gui.ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ਖਾਲੀ
gui_mch_update(ਖਾਲੀ)
{
    Xt_ਨਿਵੇਸ਼_ਨਕਾਬ ਨਕਾਬ, desired;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ALT_X_ਨਿਵੇਸ਼
    ਜੇ (suppress_alternate_ਨਿਵੇਸ਼)
	desired = (XtIMXEvent | XtIMTimer);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	desired = (XtIMAll);
    ਜਦੋਂ ((ਨਕਾਬ = XtAppPending(app_context)) && (ਨਕਾਬ & desired)
	    && !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ())
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
    ਪੂਰਨ
gui_mch_wait_for_chars(ਲੰਮਾ wtime)
{
    ਪੂਰਨ	    focus;
    ਪੂਰਨ	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ਸਥਿਰ ਪੂਰਨ	    timed_ਬਾਹਰਲਾ;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_ਨਿਵੇਸ਼_ਨਕਾਬ	    desired;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#ਜੇ_ਅੰਤ

    timed_ਬਾਹਰਲਾ = FALSE;

    ਜੇ (wtime >= 0)
	timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_ਬਾਹਰਲਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ਜੇ (channel_any_keep_open())
	channel_timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, (long_u)20,
				   channel_poll_cb, (Xt_ਸੂਚਕ)&channel_timer);
#ਜੇ_ਅੰਤ

    focus = gui.ਵਿੱਚ_focus;
    desired = (XtIMAll);
    ਜਦੋਂ (!timed_ਬਾਹਰਲਾ)
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
# ifdef FEAT_TIMERS
	did_ਜੋੜੋ_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ਜੇ (did_ਜੋੜੋ_timer)
	    // Need to recompute the waiting time.
	    ਰੋਕੋ;
# endif
#ਜੇ_ਅੰਤ

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    retval = OK;
	    ਰੋਕੋ;
	}
    }

    ਜੇ (timer != (XtIntervalId)0 && !timed_ਬਾਹਰਲਾ)
	Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(timer);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (channel_timer != (XtIntervalId)0)
	Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(channel_timer);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
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
    XFlush(gui.dpy);
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
    ਪੂਰਨ		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.ਪਿੱਠ_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_ਚੌੜਾਈ + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ਉਚਾਈ);
}

    ਖਾਲੀ
gui_mch_clear_all(ਖਾਲੀ)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    ਜੇ (gui.visibility == VisibilityFullyObscured)
	ਵਾਪਸ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.ਪਾਠ_gc,
	FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ),
	gui.char_ਚੌੜਾਈ * (gui.scroll_region_ਸੱਜਾ - gui.scroll_region_ਖੱਬਾ + 1)
			       + (gui.scroll_region_ਸੱਜਾ == Columns - 1),
	gui.char_ਉਚਾਈ * (gui.scroll_region_bot - ਪੰਕਤੀ - ਅੰਕ_ਸਤਰਾਂ + 1),
	FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ));

    gui_clear_ਖੰਡ(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1,
						       gui.scroll_region_ਖੱਬਾ,
			  gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
    ਜੇ (gui.visibility == VisibilityFullyObscured)
	ਵਾਪਸ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.ਪਾਠ_gc,
	FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	gui.char_ਚੌੜਾਈ * (gui.scroll_region_ਸੱਜਾ - gui.scroll_region_ਖੱਬਾ + 1)
			       + (gui.scroll_region_ਸੱਜਾ == Columns - 1),
	gui.char_ਉਚਾਈ * (gui.scroll_region_bot - ਪੰਕਤੀ - ਅੰਕ_ਸਤਰਾਂ + 1),
	FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ));

    gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
				ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_x11_check_copy_area(ਖਾਲੀ)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    ਜੇ (gui.visibility != VisibilityPartiallyObscured)
	ਵਾਪਸ;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    ਲਈ (;;)
    {
	ਜੇ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    ਵਾਪਸ;	// The scroll worked.

	ਜੇ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->ਚੌੜਾਈ, gevent->ਉਚਾਈ);
	    ਜੇ (gevent->ਗਿਣਤੀ_2 == 0)
		ਵਾਪਸ;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd)
{
    ਵਾਪਸ clip_x11_own_selection(vimShell, cbd);
}

    ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(
    Clipboard_T	*cbd)
{
    clip_x11_ਸੈੱਟ_selection(cbd);
}

#ਜੇ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    ਖਾਲੀ
gui_mch_menu_grey(vimmenu_T *menu, ਪੂਰਨ grey)
{
    ਜੇ (menu->id == (Widget)0)
	ਵਾਪਸ;

    gui_mch_menu_hidden(menu, False);
    ਜੇ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	Xt_ਸੈੱਟ_Sensitive(menu->id, False);
    ਨਹੀਂਤਾਂ
	Xt_ਸੈੱਟ_Sensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    ਖਾਲੀ
gui_mch_menu_hidden(vimmenu_T *menu, ਪੂਰਨ hidden)
{
    ਜੇ (menu->id == (Widget)0)
	ਵਾਪਸ;

    ਜੇ (hidden)
	Xt_Unmanage_ਬੱਚਾ(menu->id);
    ਨਹੀਂਤਾਂ
	Xt_Manage_ਬੱਚਾ(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ਖਾਲੀ
gui_mch_draw_menubar(ਖਾਲੀ)
{
    // Nothing to do in X
}

    ਖਾਲੀ
gui_x11_menu_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui_menu_cb((vimmenu_T *)client_ਅੰਕੜੇ_2);
}

#ਜੇ_ਅੰਤ // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    ਸਥਿਰ ਖਾਲੀ
gui_x11_wm_protocol_handler(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    /*
     * Only deal with Client messages.
     */
    ਜੇ (event->type != Client_ਸੁਨੇਹਾ)
	ਵਾਪਸ;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    ਜੇ ((Atom)((XClient_ਸੁਨੇਹਾ_Event *)event)->ਅੰਕੜੇ.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	ਬਾਹਰਲਾ_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, Xt_ਖਿੜਕੀ(vimShell), ਨਿਰਾ, 0);
	ਵਾਪਸ;
    }

    ਜੇ ((Atom)((XClient_ਸੁਨੇਹਾ_Event *)event)->ਅੰਕੜੇ.l[0] !=
						  wm_atoms[DELETE_ਖਿੜਕੀ_IDX])
	ਵਾਪਸ;

    gui_shell_closed();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ਸਥਿਰ ਖਾਲੀ
gui_x11_send_event_handler(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    XEvent	*event,
    Boolean	*dum ਅਣਵਰਤਿਆ)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    ਜੇ (e->type == PropertyNotify && e->ਖਿੜਕੀ_2 == comm_ਖਿੜਕੀ
	    && e->atom == commProperty &&  e->state == Property_New_ਮੁੱਲ)
	serverEventProc(gui.dpy, event, 0);
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
ਸਥਿਰ XtIntervalId	blink_timer = (XtIntervalId)0;

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
gui_mch_ਸੈੱਟ_blinking(ਲੰਮਾ waittime, ਲੰਮਾ on, ਲੰਮਾ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    ਜੇ (blink_timer != (XtIntervalId)0)
    {
	Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    ਜੇ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ਸਥਿਰ ਖਾਲੀ
gui_x11_blink_cb(
    Xt_ਸੂਚਕ	    timed_ਬਾਹਰਲਾ ਅਣਵਰਤਿਆ,
    XtIntervalId    *interval_id ਅਣਵਰਤਿਆ)
{
    ਜੇ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, blink_offtime,
						      gui_x11_blink_cb, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, blink_ontime,
						      gui_x11_blink_cb, ਨਿਰਾ);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink(ਖਾਲੀ)
{
    ਜੇ (blink_timer != (XtIntervalId)0)
	Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(blink_timer);
    // Only switch blinking on if none of the times is zero
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
    {
	blink_timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, blink_waittime,
						      gui_x11_blink_cb, ਨਿਰਾ);
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

    ਵਾਪਸ (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((ਬਿਨਾਨਿਸ਼ਾਨੀ)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    ਖਾਲੀ
gui_x11_callbacks(Widget ਪਾਠ_Area, Widget vimForm)
{
    Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, Visibility_Change_ਨਕਾਬ, FALSE,
	gui_x11_visibility_cb, (Xt_ਸੂਚਕ)0);

    Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, Exposure_ਨਕਾਬ, FALSE, gui_x11_expose_cb,
	(Xt_ਸੂਚਕ)0);

    Xt_ਜੋੜੋ_Event_Handler(vimShell, Structure_Notify_ਨਕਾਬ, FALSE,
	gui_x11_resize_ਖਿੜਕੀ_cb, (Xt_ਸੂਚਕ)0);

    Xt_ਜੋੜੋ_Event_Handler(vimShell, Focus_Change_ਨਕਾਬ, FALSE, gui_x11_focus_change_cb,
	(Xt_ਸੂਚਕ)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ਜੇ (vim_strchr(p_go, GO_ਸੂਚਕ) != ਨਿਰਾ)
    {
	Xt_ਜੋੜੋ_Event_Handler(vimShell, Leave_ਖਿੜਕੀ_ਨਕਾਬ, FALSE, gui_x11_leave_cb,
	    (Xt_ਸੂਚਕ)0);
	Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, Leave_ਖਿੜਕੀ_ਨਕਾਬ, FALSE, gui_x11_leave_cb,
	    (Xt_ਸੂਚਕ)0);
	Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, Enter_ਖਿੜਕੀ_ਨਕਾਬ, FALSE, gui_x11_enter_cb,
	    (Xt_ਸੂਚਕ)0);
	Xt_ਜੋੜੋ_Event_Handler(vimShell, Enter_ਖਿੜਕੀ_ਨਕਾਬ, FALSE, gui_x11_enter_cb,
	    (Xt_ਸੂਚਕ)0);
    }

    Xt_ਜੋੜੋ_Event_Handler(vimForm, ਕੁੰਜੀ_Press_ਨਕਾਬ, FALSE, gui_x11_ਕੁੰਜੀ_ਪ੍ਰਾਪਤੀ_cb,
	(Xt_ਸੂਚਕ)0);
    Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, ਕੁੰਜੀ_Press_ਨਕਾਬ, FALSE, gui_x11_ਕੁੰਜੀ_ਪ੍ਰਾਪਤੀ_cb,
	(Xt_ਸੂਚਕ)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    Xt_ਜੋੜੋ_Event_Handler(vimForm, ਸੂਚਕ_Motion_ਨਕਾਬ,
	FALSE, gui_x11_mouse_cb, (Xt_ਸੂਚਕ)1);
    Xt_ਜੋੜੋ_Event_Handler(ਪਾਠ_Area, Button_Press_ਨਕਾਬ | Button_Release_ਨਕਾਬ |
					 Button_Motion_ਨਕਾਬ | ਸੂਚਕ_Motion_ਨਕਾਬ,
	FALSE, gui_x11_mouse_cb, (Xt_ਸੂਚਕ)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    ਪੂਰਨ		rootx, rooty, winx, winy;
    ਖਿੜਕੀ	ਜੜ੍ਹ, ਬੱਚਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ;

    ਜੇ (gui.wid && XQuery_ਸੂਚਕ(gui.dpy, gui.wid, &ਜੜ੍ਹ, &ਬੱਚਾ,
					 &rootx, &rooty, &winx, &winy, &ਨਕਾਬ))
    {
	*x = winx;
	*y = winy;
    }
    ਨਹੀਂਤਾਂ
    {
	*x = -1;
	*y = -1;
    }
}

    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x, ਪੂਰਨ y)
{
    ਜੇ (gui.wid)
	XWarp_ਸੂਚਕ(gui.dpy, (ਖਿੜਕੀ)0, gui.wid, 0, 0, 0, 0, x, y);
}

#ਜੇ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_ਆਖਰੀ_mouse_event(ਖਾਲੀ)
{
    ਵਾਪਸ &ਆਖਰੀ_mouse_event;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_ਚੌੜਾਈ (gui.char_ਚੌੜਾਈ * 2)

    ਖਾਲੀ
gui_mch_drawsign(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ typenr)
{
    XImage	*sign;

    ਜੇ (!gui.ਵਿੱਚ_use || (sign = (XImage *)sign_get_image(typenr)) == ਨਿਰਾ)
	ਵਾਪਸ;

    XClearArea(gui.dpy, gui.wid, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ) - sign->ਉਚਾਈ,
	    SIGN_ਚੌੜਾਈ, gui.char_ਉਚਾਈ, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.ਪਾਠ_gc, sign, 0, 0,
	    ਪਾਠ_X(ਖਾਨਾ) + (SIGN_ਚੌੜਾਈ - sign->ਚੌੜਾਈ) / 2,
	    ਪਾਠ_Y(ਪੰਕਤੀ) - sign->ਉਚਾਈ,
	    sign->ਚੌੜਾਈ, sign->ਉਚਾਈ);
}

    ਖਾਲੀ *
gui_mch_ਰਜਿਸਟਰ_sign(char_u *signfile)
{
    Xpm_ਗੁਣ   attrs;
    XImage	    *sign = ਨਿਰਾ;
    ਪੂਰਨ		    ਸਥਿਤੀ;

    /*
     * Setup the color substitution table.
     */
    ਜੇ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_ਚਿੰਨ੍ਹ color[5] =
	{
	    {"none", ਨਿਰਾ, 0},
	    {"iconColor1", ਨਿਰਾ, 0},
	    {"bottomShadowColor", ਨਿਰਾ, 0},
	    {"topShadowColor", ਨਿਰਾ, 0},
	    {"selectColor", ਨਿਰਾ, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.ਪਿੱਠ_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	ਸਥਿਤੀ = Xpm_Read_File_ਮੰਜ਼ਿਲ_Image(gui.dpy, (ਅੱਖਰ *)signfile,
							 &sign, ਨਿਰਾ, &attrs);
	ਜੇ (ਸਥਿਤੀ == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	ਨਹੀਂਤਾਂ
	    emsg(_(e_couldnt_read_ਵਿੱਚ_sign_ਅੰਕੜੇ));
    }

    ਵਾਪਸ (ਖਾਲੀ *)sign;
}

    ਖਾਲੀ
gui_mch_destroy_sign(ਖਾਲੀ *sign)
{
    XDestroyImage((XImage*)sign);
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_shape = 0;
#ਜੇ_ਅੰਤ

/*
 * Use the blank mouse pointer or not.
 */
    ਖਾਲੀ
gui_mch_mousehide(
    ਪੂਰਨ		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    ਜੇ (gui.ਸੂਚਕ_hidden == hide)
	ਵਾਪਸ;

    gui.ਸੂਚਕ_hidden = hide;
    ਜੇ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_ਸੂਚਕ);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	mch_ਸੈੱਟ_mouse_shape(ਆਖਰੀ_shape);
#ਨਹੀਂਤਾਂ
    XUndefineCursor(gui.dpy, gui.wid);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ਸਥਿਰ ਪੂਰਨ mshape_ids[] =
{
    XC_ਖੱਬਾ_ptr,		// arrow
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
    XC_ਸੱਜਾ_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ਖੱਬਾ_ptr			// last one
};

    ਖਾਲੀ
mch_ਸੈੱਟ_mouse_shape(ਪੂਰਨ shape)
{
    ਪੂਰਨ	    id;

    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    ਜੇ (shape == MSHAPE_HIDE || gui.ਸੂਚਕ_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_ਸੂਚਕ);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ਜੇ (id >= XC_ਅੰਕ_glyphs)
		id = XC_ਖੱਬਾ_ptr;
	    ਨਹੀਂਤਾਂ
		id &= ~1;	// they are always even (why?)
	}
	ਨਹੀਂਤਾਂ
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    ਜੇ (shape != MSHAPE_HIDE)
	ਆਖਰੀ_shape = shape;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    ਖਾਲੀ
gui_mch_menu_ਸੈੱਟ_tip(vimmenu_T *menu)
{
    ਜੇ (menu->id == ਨਿਰਾ || menu->ਮਾਪਾ == ਨਿਰਾ
				|| !menu_is_toolbar(menu->ਮਾਪਾ->name))
	ਵਾਪਸ;

    // Always destroy and create the balloon, in case the string was
    // changed.
    ਜੇ (menu->tip != ਨਿਰਾ)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ਨਿਰਾ;
    }
    ਜੇ (menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP] != ਨਿਰਾ)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->ਲੜੀਆਂ[MENU_ਸੂਚਕਾਂਕ_TIP],
		ਨਿਰਾ,
		ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ
