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

#شامل "vim.h"

#شامل <X11/keysym.h>
#شامل <X11/Xatom.h>
#شامل <X11/StringDefs.h>
#شامل <X11/Intrinsic.h>
#شامل <X11/Shell.h>
#شامل <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#اگر defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#ورنہ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ورنہ
#   include <X11/xpm.h>
#  endif
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# ورنہ
#  include <locale.h>
# endif
#اگر_ختم

#اگر_تعریف_شدہ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#اگر_ختم

#تعریف_کرو VIM_NAME	"vim"
#تعریف_کرو VIM_CLASS	"Vim"

// Default resource values
#تعریف_کرو DFLT_FONT		"7x13"
#اگر_تعریف_شدہ FONTSET_ALWAYS
# define DFLT_MENU_FONT		Xt_Default_Font_سیٹ
#ورنہ
# define DFLT_MENU_FONT		XtDefaultFont
#اگر_ختم
#تعریف_کرو DFLT_TOOLTIP_FONT	Xt_Default_Font_سیٹ

// use the default (CDE) colors
#تعریف_کرو DFLT_MENU_BG_COLOR	""
#تعریف_کرو DFLT_MENU_FG_COLOR	""
#تعریف_کرو DFLT_SCROLL_BG_COLOR	""
#تعریف_کرو DFLT_SCROLL_FG_COLOR	""
#تعریف_کرو DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#تعریف_کرو DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

ساکن Atom   wm_atoms[2];	// Window Manager Atoms
#تعریف_کرو DELETE_کھڑکی_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#تعریف_کرو SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#اگر_تعریف_شدہ FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ساکن XFont_سیٹ current_fontset = تہی;
# اگر !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	کرو \
	{ \
	    اگر (current_fontset != تہی) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    ورنہ \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} جب (0)
# endif
# اگر !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	کرو \
	{ \
	    اگر (current_fontset != تہی) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ورنہ \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} جب (0)
# endif
# اگر !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	کرو \
	{ \
	    اگر (current_fontset != تہی) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ورنہ \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} جب (0)
# endif
ساکن صحیح check_fontset_sanity(XFont_سیٹ fs);
ساکن صحیح fontset_چوڑائی(XFont_سیٹ fs);
ساکن صحیح fontset_ascent(XFont_سیٹ fs);
#اگر_ختم

ساکن guicolor_T	پچھلا_fg_color = INVALCOLOR;
ساکن guicolor_T	پچھلا_bg_color = INVALCOLOR;
ساکن guicolor_T	پچھلا_sp_color = INVALCOLOR;

#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ساکن XButtonPressedEvent آخری_mouse_event;
#اگر_ختم

ساکن خالی gui_x11_check_copy_area(خالی);
#اگر_تعریف_شدہ FEAT_CLIENTSERVER
ساکن خالی gui_x11_send_event_handler(Widget, Xt_اشارہ_گر, XEvent *, Boolean *);
#اگر_ختم
ساکن خالی gui_x11_wm_protocol_handler(Widget, Xt_اشارہ_گر, XEvent *, Boolean *);
ساکن Cursor gui_x11_create_blank_mouse(خالی);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ساکن ساخت specialkey
{
    کلید_Sym  کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدیں[] =
{
    {XK_اوپر,		'k', 'u'},
    {XK_نیچے_کو,		'k', 'd'},
    {XK_بایاں,		'k', 'l'},
    {XK_دایاں,		'k', 'r'},

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
#اگر_تعریف_شدہ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#اگر_ختم

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_عقب_Space,	'k', 'b'},
    {XK_داخل_کرو,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_انتہا,		'@', '7'},
    {XK_پیشگی,		'k', 'P'},
    {XK_اگلا,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#اگر_تعریف_شدہ XK_KP_بایاں
    {XK_KP_بایاں,	'k', 'l'},
    {XK_KP_دایاں,	'k', 'r'},
    {XK_KP_اوپر,		'k', 'u'},
    {XK_KP_نیچے_کو,	'k', 'd'},
    {XK_KP_داخل_کرو,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_انتہا,		'K', '4'},
    {XK_KP_پیشگی,	'K', '3'},
    {XK_KP_اگلا,	'K', '5'},

    {XK_KP_جوڑو,		'K', '6'},
    {XK_KP_تفریق,	'K', '7'},
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
#اگر_ختم

    // End of list marker:
    {(کلید_Sym)0,	    0, 0}
};

#تعریف_کرو XtNboldFont		"boldFont"
#تعریف_کرو XtCBoldFont		"BoldFont"
#تعریف_کرو XtNitalicFont		"italicFont"
#تعریف_کرو XtCItalicFont		"ItalicFont"
#تعریف_کرو XtNboldItalicFont	"boldItalicFont"
#تعریف_کرو XtCBoldItalicFont	"BoldItalicFont"
#تعریف_کرو Xt_Nscrollbar_چوڑائی	"scrollbarWidth"
#تعریف_کرو Xt_CScrollbar_چوڑائی	"ScrollbarWidth"
#تعریف_کرو Xt_Nmenu_اونچائی		"menuHeight"
#تعریف_کرو Xt_CMenu_اونچائی		"MenuHeight"
#تعریف_کرو XtNmenuFont		"menuFont"
#تعریف_کرو XtCMenuFont		"MenuFont"
#تعریف_کرو Xt_Nmenu_Font_سیٹ		"menuFontSet"
#تعریف_کرو Xt_CMenu_Font_سیٹ		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#تعریف_کرو XtNmenuBackground	"menuBackground"
#تعریف_کرو XtCMenuBackground	"MenuBackground"
#تعریف_کرو XtNmenuForeground	"menuForeground"
#تعریف_کرو XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#تعریف_کرو XtNscrollBackground	"scrollBackground"
#تعریف_کرو XtCScrollBackground	"ScrollBackground"
#تعریف_کرو XtNscrollForeground	"scrollForeground"
#تعریف_کرو XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#تعریف_کرو XtNtooltipBackground	"tooltipBackground"
#تعریف_کرو XtCTooltipBackground	"TooltipBackground"
#تعریف_کرو XtNtooltipForeground	"tooltipForeground"
#تعریف_کرو XtCTooltipForeground	"TooltipForeground"
#تعریف_کرو XtNtooltipFont		"tooltipFont"
#تعریف_کرو XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
ساکن XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	حجم(Pixel),
	Xt_انحراف_کا(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	حجم(Pixel),
	Xt_انحراف_کا(gui_T, def_عقب_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	حجم(String *),
	Xt_انحراف_کا(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	حجم(String *),
	Xt_انحراف_کا(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	حجم(String *),
	Xt_انحراف_کا(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	حجم(String *),
	Xt_انحراف_کا(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	حجم(String *),
	Xt_انحراف_کا(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	حجم(Bool),
	Xt_انحراف_کا(gui_T, rsrc_rev_video),
	XtRImmediate,
	(Xt_اشارہ_گر)False
    },
    {
	Xt_Nborder_چوڑائی,
	Xt_CBorder_چوڑائی,
	XtRInt,
	حجم(صحیح),
	Xt_انحراف_کا(gui_T, border_چوڑائی),
	XtRImmediate,
	(Xt_اشارہ_گر)2
    },
    {
	Xt_Nscrollbar_چوڑائی,
	Xt_CScrollbar_چوڑائی,
	XtRInt,
	حجم(صحیح),
	Xt_انحراف_کا(gui_T, scrollbar_چوڑائی),
	XtRImmediate,
	(Xt_اشارہ_گر)SB_DEFAULT_چوڑائی
    },
#اگر_تعریف_شدہ FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	Xt_Nmenu_Font_سیٹ,
	Xt_CMenu_Font_سیٹ,
# ورنہ
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#اگر_ختم
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	حجم(حرف *),
	Xt_انحراف_کا(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	Xt_CFont_سیٹ,
	Xt_RFont_سیٹ,
	حجم(XFont_سیٹ),
	Xt_انحراف_کا(gui_T, tooltip_fontset),
	XtRImmediate,
	(Xt_اشارہ_گر)NOFONTSET
    },
#اگر_ختم // FEAT_BEVAL_GUI
#اگر_تعریف_شدہ FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	حجم(حرف*),
	Xt_انحراف_کا(gui_T, rsrc_preedit_type_name),
	XtRString,
	(Xt_اشارہ_گر)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	حجم(حرف*),
	Xt_انحراف_کا(gui_T, rsrc_داخلہ_طریقہ),
	XtRString,
	تہی
    },
#اگر_ختم // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
ساکن XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	تہی},
    {"-background",	".background",	    XrmoptionSepArg,	تہی},
    {"-fg",		".foreground",	    XrmoptionSepArg,	تہی},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	تہی},
    {"-fn",		".font",	    XrmoptionSepArg,	تہی},
    {"-font",		".font",	    XrmoptionSepArg,	تہی},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	تہی},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	تہی},
    {"-geom",		".geometry",	    XrmoptionSepArg,	تہی},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	تہی},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	تہی},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	تہی},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	تہی},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	تہی},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	تہی},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	تہی},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	تہی},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	تہی},
#اگر_تعریف_شدہ FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	تہی},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	تہی},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	تہی},
#ورنہ
    {"-mf",		".menuFont",	    XrmoptionSepArg,	تہی},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	تہی},
#اگر_ختم
    {"-xrm",		تہی,		    XrmoptionResArg,	تہی}
};

ساکن صحیح gui_argc = 0;
ساکن حرف **gui_argv = تہی;

/*
 * Call-back routines.
 */

    ساکن خالی
gui_x11_timer_cb(
    Xt_اشارہ_گر	    timed_برآمد,
    XtIntervalId    *interval_id غیر_مستعمل)
{
    *((صحیح *)timed_برآمد) = TRUE;
}

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    ساکن خالی
channel_poll_cb(
    Xt_اشارہ_گر	    client_ڈیٹا_2,
    XtIntervalId    *interval_id غیر_مستعمل)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_ڈیٹا_2;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = Xt_App_جوڑو_وقت_برآمد(app_context, (long_u)20,
						 channel_poll_cb, client_ڈیٹا_2);
}
#اگر_ختم

    ساکن خالی
gui_x11_visibility_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	dud غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    اگر (event->type != VisibilityNotify)
	واپس;

    gui.visibility = event->xvisibility.state;

    /*
     * When we do an XCopyArea(), and the window is partially obscured, we want
     * to receive an event to tell us whether it worked or not.
     */
    XSetGraphicsExposures(gui.dpy, gui.متن_gc,
	    gui.visibility != VisibilityUnobscured);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

    ساکن خالی
gui_x11_expose_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	dud غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    XExposeEvent	*gevent;
    صحیح			new_x;

    اگر (event->type != Expose)
	واپس;

    برآمد_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->چوڑائی, gevent->اونچائی);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    اگر (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    اگر (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    اگر (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((صحیح)Columns), 0, 0, 0, False);
    اگر (gevent->y > FILL_Y(صفیں))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((صحیح)صفیں), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#اگر defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    ساکن خالی
shellRectangle(Widget shell, XRectangle *r)
{
    کھڑکی		rootw, shellw, بچہ, parentw;
    صحیح			absx, absy;
    XWindow_اوصاف	a;
    کھڑکی		*children;
    غیرعلامتی صحیح	children_گنتی;

    shellw = Xt_کھڑکی(shell);
    اگر (shellw == 0)
	واپس;
    برائے (;;)
    {
	XQuery_درخت(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_گنتی);
	XFree(children);
	اگر (parentw == rootw)
	    توڑو;
	shellw = parentw;
    }
    XGet_کھڑکی_اوصاف(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.جڑ, 0, 0,
							&absx, &absy, &بچہ);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_اقدار(shell, XmNheight, &r->اونچائی, XmNwidth, &r->چوڑائی, تہی);
}
#اگر_ختم

    ساکن خالی
gui_x11_resize_کھڑکی_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	dud غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    ساکن صحیح آخری_چوڑائی_2, آخری_اونچائی_2;

    اگر (event->type != ConfigureNotify)
	واپس;

    اگر (event->xconfigure.چوڑائی != آخری_چوڑائی_2
	    || event->xconfigure.اونچائی != آخری_اونچائی_2)
    {
	آخری_چوڑائی_2 = event->xconfigure.چوڑائی;
	آخری_اونچائی_2 = event->xconfigure.اونچائی;
	gui_resize_shell(event->xconfigure.چوڑائی, event->xconfigure.اونچائی
#اگر_تعریف_شدہ FEAT_XIM
						- xim_get_حالت_area_اونچائی()
#اگر_ختم
		     );
    }
#اگر defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    اگر (netbeans_فعال())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_چوکھٹا_moved(rec.x, rec.y);
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XIM
    xim_سیٹ_preedit();
#اگر_ختم
}

    ساکن خالی
gui_x11_focus_change_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	ڈیٹا غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    gui_focus_change(event->type == Focus_اندر);
}

    ساکن خالی
gui_x11_enter_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	ڈیٹا غیر_مستعمل,
    XEvent	*event غیر_مستعمل,
    Boolean	*dum غیر_مستعمل)
{
    gui_focus_change(TRUE);
}

    ساکن خالی
gui_x11_leave_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	ڈیٹا غیر_مستعمل,
    XEvent	*event غیر_مستعمل,
    Boolean	*dum غیر_مستعمل)
{
    gui_focus_change(FALSE);
}

#اگر defined(X_HAVE_UTF8_STRING)
# اگر X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#اگر_ختم

    خالی
gui_x11_کلید_کامیابی_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	dud غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    XKeyPressedEvent	*ev_press;
#اگر_تعریف_شدہ FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    حالت_3		حالت_2;
#ورنہ
    char_u		string[4], string2[3];
#اگر_ختم
    کلید_Sym		کلید_sym;
    صحیح			len;
    صحیح			i;
    صحیح			modifiers;
    صحیح			کلید;

    ev_press = (XKeyPressedEvent *)event;

#اگر_تعریف_شدہ FEAT_XIM
    اگر (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	اگر (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (حرف *)string,
				  حجم(string_shortbuf), &کلید_sym, &حالت_2);
	ورنہ
# endif
	    len = XmbLookupString(xic, ev_press, (حرف *)string,
				  حجم(string_shortbuf), &کلید_sym, &حالت_2);
	اگر (حالت_2 == XBuffer_حد_سے_تجاوز)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    اگر (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (حرف *)string,
						      len, &کلید_sym, &حالت_2);
	    ورنہ
# endif
		len = XmbLookupString(xic, ev_press, (حرف *)string,
						      len, &کلید_sym, &حالت_2);
	}
	اگر (حالت_2 == XLookupNone || حالت_2 == XLookupChars)
	    کلید_sym = XK_Void_علامت;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	اگر (len > 0 && داخلہ_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    صحیح		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    اگر (string_alloced)
		XtFree((حرف *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_داخلہ(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	برائے (i = 0; i < len; ++i)
	    اگر (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (صحیح)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		اگر (string_alloced)
		    XtFree((حرف *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    ورنہ
#اگر_ختم
	len = XLookupString(ev_press, (حرف *)string, حجم(string),
		&کلید_sym, تہی);

#اگر_تعریف_شدہ SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    اگر (کلید_sym == SunXK_F36 || کلید_sym == SunXK_F37)
	len = 0;
#اگر_ختم

    اگر (کلید_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    اگر (کلید_sym == XK_minus && (ev_press->state & Control_نقاب))
	string[0] = Ctrl__;

#اگر_تعریف_شدہ XK_ISO_بایاں_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    اگر (کلید_sym == XK_ISO_بایاں_Tab)
    {
	کلید_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#اگر_ختم

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    اگر (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (صحیح)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    اگر (len == 0 || len == 1)
    {
	برائے (i = 0; special_کلیدیں[i].کلید_sym != (کلید_Sym)0; i++)
	{
	    اگر (special_کلیدیں[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدیں[i].vim_code0;
		string[2] = special_کلیدیں[i].vim_code1;
		len = -3;
		توڑو;
	    }
	}
    }

    // Unrecognised key is ignored.
    اگر (len == 0)
	جاؤ theend;

    // Handle modifiers.
    modifiers = 0;
    اگر (ev_press->state & سرکاؤ_نقاب)
	modifiers |= MOD_نقاب_سرکاؤ;
    اگر (ev_press->state & Control_نقاب)
    {
	modifiers |= MOD_نقاب_CTRL;
	اگر (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    اگر (ev_press->state & Mod1_نقاب)
	modifiers |= MOD_نقاب_ALT;
    اگر (ev_press->state & Mod4_نقاب)
	modifiers |= MOD_نقاب_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    اگر (len == -3)
	کلید = مقصد_SPECIAL(string[1], string[2]);
    ورنہ
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }
    کلید = simplify_کلید(کلید, &modifiers);
    اگر (کلید == CSI)
	کلید = K_CSI;
    اگر (IS_SPECIAL(کلید))
    {
	string[0] = CSI;
	string[1] = K_دوسرا(کلید);
	string[2] = K_تیسرا(کلید);
	len = 3;
    }
    ورنہ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

	len = mb_char2bytes(کلید, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_سرکاؤ_modifier(modifiers, کلید);
    }

    اگر (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڑو_مقصد_داخلہ_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	صحیح int_ch = check_for_مداخلت(کلید, modifiers);

	اگر (int_ch != NUL)
	{
	    trash_داخلہ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    جوڑو_مقصد_داخلہ_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    اگر (p_mh)
	gui_mch_mousehide(TRUE);

#اگر defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	اگر ((be = gui_mch_currently_showing_beval()) != تہی)
	    gui_mch_unpost_balloon(be);
    }
#اگر_ختم
theend:
    {}	    // some compilers need a statement here
#اگر_تعریف_شدہ FEAT_XIM
    اگر (string_alloced)
	XtFree((حرف *)string);
#اگر_ختم
}

    ساکن خالی
gui_x11_mouse_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	dud غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    ساکن XtIntervalId timer = (XtIntervalId)0;
    ساکن صحیح	timed_برآمد = TRUE;

    صحیح		button;
    صحیح		repeated_click = FALSE;
    صحیح		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    اگر (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1_نقاب | Button2_نقاب | Button3_نقاب))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	اگر (button != MOUSE_DRAG)	// just moving the rodent
	{
#اگر_تعریف_شدہ FEAT_MENU
	    اگر (dud)			// moved in vimForm
		y -= gui.menu_اونچائی;
#اگر_ختم
	    gui_mouse_moved(x, y);
	    واپس;
	}
    }
    ورنہ
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	اگر (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    اگر (!timed_برآمد)
	    {
		Xt_Remove_وقت_برآمد(timer);
		repeated_click = TRUE;
	    }
	    timed_برآمد = FALSE;
	    timer = Xt_App_جوڑو_وقت_برآمد(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_برآمد);
	    انتخاب (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		حالت Button1:	button = MOUSE_بایاں;	توڑو;
		حالت Button2:	button = MOUSE_درمیان;	توڑو;
		حالت Button3:	button = MOUSE_دایاں;	توڑو;
		حالت Button4:	button = MOUSE_4;	توڑو;
		حالت Button5:	button = MOUSE_5;	توڑو;
		حالت 6:		button = MOUSE_7;	توڑو;
		حالت 7:		button = MOUSE_6;	توڑو;
		حالت 8:		button = MOUSE_X1;	توڑو;
		حالت 9:		button = MOUSE_X2;	توڑو;
		طےشدہ:
		    واپس;	// Unknown button
	    }
	}
	ورنہ اگر (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	ورنہ
	    واپس;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	آخری_mouse_event = event->xbutton;
#اگر_ختم
    }

    vim_modifiers = 0x0;
    اگر (x_modifiers & سرکاؤ_نقاب)
	vim_modifiers |= MOUSE_سرکاؤ;
    اگر (x_modifiers & Control_نقاب)
	vim_modifiers |= MOUSE_CTRL;
    اگر (x_modifiers & Mod1_نقاب)	    // Alt or Meta key
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
    خالی
gui_mch_prepare(صحیح *argc, حرف **argv)
{
    صحیح	    arg;
    صحیح	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(حرف *, *argc);
    اگر (gui_argv == تہی)
	واپس;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    جب (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	برائے (i = 0; i < (صحیح)Xt_نمبر(cmdline_options); i++)
	    اگر (strcmp(argv[arg], cmdline_options[i].option) == 0)
		توڑو;

	اگر (i < (صحیح)Xt_نمبر(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    اگر (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		ملا_الٹ_arg = TRUE;
	    ورنہ اگر ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_دلیل = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    اگر (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * حجم(حرف *));
		اگر (cmdline_options[i].arg_قسم != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    اگر (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * حجم(حرف *));
		}
	    }
	    argv[*argc] = تہی;
	}
	ورنہ
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
	    اگر (لڑی_موازنہ("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * حجم(حرف *));
	    argv[*argc] = تہی;
	}
	ورنہ
#اگر_ختم
	    arg++;
    }
}

#اگر_غیر_تعریف_شدہ XtSpecificationRelease
# define CARDINAL (Cardinal *)
#ورنہ
# اگر XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# ورنہ
#  define CARDINAL (صحیح *)
# endif
#اگر_ختم

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    صحیح
gui_mch_init_check(خالی)
{
#اگر_تعریف_شدہ FEAT_XIM
    Xt_سیٹ_Language_Proc(تہی, تہی, تہی);
#اگر_ختم
    open_app_context();
    اگر (app_context != تہی)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_نمبر(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#اگر defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	حرف *p = setlocale(LC_NUMERIC, تہی);

	اگر (p == تہی || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#اگر_ختم
    اگر (app_context == تہی || gui.dpy == تہی)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	واپس FAIL;
    }
    واپس OK;
}


#اگر_تعریف_شدہ USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ساکن Xt_داخلہ_Id _xsmp_xtinputid;

    ساکن خالی
local_xsmp_handle_درخواستیں(
    Xt_اشارہ_گر	c غیر_مستعمل,
    صحیح		*s غیر_مستعمل,
    Xt_داخلہ_Id	*i غیر_مستعمل)
{
    اگر (xsmp_handle_درخواستیں() == FAIL)
	Xt_Remove_داخلہ(_xsmp_xtinputid);
}
#اگر_ختم


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحیح
gui_mch_init(خالی)
{
    XtGCMask	gc_نقاب;
    XGCValues	gc_vals;
    صحیح		x, y, نقاب;
    غیرعلامتی	w, h;

#اگر 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#اگر_ختم

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, تہی);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (Xt_اشارہ_گر)&gui,
	vim_resources, Xt_نمبر(vim_resources), تہی);

    gui.scrollbar_اونچائی = gui.scrollbar_چوڑائی;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#اگر_ختم

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.عقب_pixel = gui.def_عقب_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    اگر (gui.rsrc_rev_video && gui_get_lightness(gui.عقب_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_عقب_pixel;
	gui.عقب_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_عقب_pixel = gui.عقب_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    سیٹ_معمول_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_نقاب = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.عقب_pixel;
    gui.متن_gc = XtGetGC(vimShell, gc_نقاب, &gc_vals);

    gc_vals.foreground = gui.عقب_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.عقب_gc = XtGetGC(vimShell, gc_نقاب, &gc_vals);

    gc_نقاب |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.عقب_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.عقب_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_نقاب, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_تیاری_atoms(gui.dpy);

    اگر (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_سیٹ_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    اگر (gui.geom != تہی && *gui.geom != NUL)
    {
	نقاب = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);
	اگر (نقاب & چوڑائی_قدر)
	    Columns = w;
	اگر (نقاب & اونچائی_قدر)
	{
	    اگر (p_کھڑکی > (طویل)h - 1 || !option_was_سیٹ((char_u *)"window"))
		p_کھڑکی = h - 1;
	    صفیں = h;
	}
	حد_بندی_screen_سائز();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	اگر (نقاب & (XValue|YValue))
	    Xt_Va_سیٹ_اقدار(vimShell, XtNgeometry, gui.geom, تہی);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    اگر (vim_strchr(p_go, GO_ICON) != تہی)
    {
#اگر_غیر_تعریف_شدہ HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	Xt_سیٹ_Arg(arg[0], XtNiconPixmap,
		XCreate_Bitmap_From_ڈیٹا(gui.dpy,
		    Default_جڑ_کھڑکی(gui.dpy),
		    (حرف *)vim_icon_بٹ,
		    vim_icon_چوڑائی,
		    vim_icon_اونچائی));
	Xt_سیٹ_Arg(arg[1], Xt_Nicon_نقاب,
		XCreate_Bitmap_From_ڈیٹا(gui.dpy,
		    Default_جڑ_کھڑکی(gui.dpy),
		    (حرف *)vim_نقاب_icon_بٹ,
		    vim_نقاب_icon_چوڑائی,
		    vim_نقاب_icon_اونچائی));
	Xt_سیٹ_اقدار(vimShell, arg, (Cardinal)2);
#ورنہ
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ساکن Pixmap	icon = 0;
    ساکن Pixmap	icon_نقاب = 0;
    ساکن حرف		**magick = vim32x32;
    کھڑکی		جڑ_کھڑکی;
    XIcon_سائز		*سائز;
    صحیح			نمبر_sizes;
    Display		*dsp;
    Screen		*scr;
    Xpm_اوصاف	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    جڑ_کھڑکی = XRoot_کھڑکی_کا_Screen(XtScreen(vimShell));
    اگر (XGetIconSizes(XtDisplay(vimShell), جڑ_کھڑکی,
						   &سائز, &نمبر_sizes) != 0)
    {
	اگر (نمبر_sizes > 0)
	{
	    اگر (سائز->max_اونچائی_2 >= 48 && سائز->max_چوڑائی_2 >= 48)
		magick = vim48x48;
	    ورنہ اگر (سائز->max_اونچائی_2 >= 32 && سائز->max_چوڑائی_2 >= 32)
		magick = vim32x32;
	    ورنہ اگر (سائز->max_اونچائی_2 >= 16 && سائز->max_چوڑائی_2 >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_سیٹ_اقدار(vimShell, XtNcolormap, cmap, تہی);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | Xpm_گہرائی;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.گہرائی = Default_گہرائی_کا_Screen(scr);

    اگر (!icon)
    {
	Xpm_Create_Pixmap_From_ڈیٹا(dsp, جڑ_کھڑکی, magick, &icon,
							   &icon_نقاب, &attr);
	Xpm_Free_اوصاف(&attr);
    }

    Xt_Va_سیٹ_اقدار(vimShell, XmNiconPixmap, icon, Xm_Nicon_نقاب, icon_نقاب, تہی);
#اگر_ختم
    }

#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
    gui_init_menu_font();
#اگر_ختم

#اگر_تعریف_شدہ USE_XSMP
    // Attach listener on ICE connection
    اگر (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_جوڑو_داخلہ(app_context, xsmp_icefd,
		(Xt_اشارہ_گر)Xt_داخلہ_Read_نقاب, local_xsmp_handle_درخواستیں, تہی);
#اگر_ختم

    واپس OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    خالی
gui_mch_uninit(خالی)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = تہی;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    خالی
gui_mch_new_colors(خالی)
{
    long_u	gc_نقاب;
    XGCValues	gc_vals;

    gc_نقاب = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.عقب_pixel;
    اگر (gui.متن_gc != تہی)
	XChangeGC(gui.dpy, gui.متن_gc, gc_نقاب, &gc_vals);

    gc_vals.foreground = gui.عقب_pixel;
    gc_vals.background = gui.norm_pixel;
    اگر (gui.عقب_gc != تہی)
	XChangeGC(gui.dpy, gui.عقب_gc, gc_نقاب, &gc_vals);

    gc_نقاب |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.عقب_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.عقب_pixel;
    gc_vals.function   = GXxor;
    اگر (gui.invert_gc != تہی)
	XChangeGC(gui.dpy, gui.invert_gc, gc_نقاب, &gc_vals);

    gui_x11_سیٹ_عقب_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(خالی)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    Xt_Manage_بچہ(Xt_Name_مقصد_Widget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_اشارہ_گر = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_کھڑکی_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, Xt_کھڑکی(vimShell), wm_atoms, 2);
    Xt_جوڑو_Event_Handler(vimShell, No_Event_نقاب, True, gui_x11_wm_protocol_handler,
							     تہی);
#اگر_تعریف_شدہ HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    Xt_جوڑو_Event_Handler(vimShell, (Event_نقاب)0, True, _XEditResCheckMessages,
	    (Xt_اشارہ_گر)تہی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
    اگر (serverName == تہی && server_Delayed_آغاز_Name != تہی)
    {
	// This is a :gui command in a plain vim with no previous server
	comm_کھڑکی = Xt_کھڑکی(vimShell);
	(خالی)server_رجسٹر_Name(gui.dpy, server_Delayed_آغاز_Name);
    }
    ورنہ
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	server_Change_Registered_کھڑکی(gui.dpy, Xt_کھڑکی(vimShell));
    }
    Xt_جوڑو_Event_Handler(vimShell, Property_Change_نقاب, False,
		      gui_x11_send_event_handler, تہی);
#اگر_ختم

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#اگر_تعریف_شدہ FEAT_XIM
    xim_init();
#اگر_ختم

    واپس OK;
}

#اگر defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    خالی
gui_init_tooltip_font(خالی)
{
    Xrm_قدر from, مقصد;

    from.addr = (حرف *)gui.rsrc_tooltip_font_name;
    from.سائز = لڑی_لمبائی(from.addr);
    مقصد.addr = (Xt_اشارہ_گر)&gui.tooltip_fontset;
    مقصد.سائز = حجم(XFont_سیٹ);

    اگر (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_سیٹ, &مقصد) == False)
    {
	// Failed. What to do?
    }
}
#اگر_ختم

#اگر defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    خالی
gui_init_menu_font(خالی)
{
    Xrm_قدر from, مقصد;

# ifdef FONTSET_ALWAYS
    from.addr = (حرف *)gui.rsrc_menu_font_name;
    from.سائز = لڑی_لمبائی(from.addr);
    مقصد.addr = (Xt_اشارہ_گر)&gui.menu_fontset;
    مقصد.سائز = حجم(GuiFontset);

    اگر (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_سیٹ, &مقصد) == False)
    {
	// Failed. What to do?
    }
# ورنہ
    from.addr = (حرف *)gui.rsrc_menu_font_name;
    from.سائز = لڑی_لمبائی(from.addr);
    مقصد.addr = (Xt_اشارہ_گر)&gui.menu_font;
    مقصد.سائز = حجم(GuiFont);

    اگر (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &مقصد) == False)
    {
	// Failed. What to do?
    }
# endif
}
#اگر_ختم

    خالی
gui_mch_exit(صحیح rc غیر_مستعمل)
{
#اگر 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#اگر_ختم
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_اقدار(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	تہی);
    *x = xpos;
    *y = ypos;
    واپس OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    خالی
gui_mch_سیٹ_winpos(صحیح x, صحیح y)
{
    Xt_Va_سیٹ_اقدار(vimShell,
	XtNx,	x,
	XtNy,	y,
	تہی);
}

    خالی
gui_mch_سیٹ_shellsize(
    صحیح		چوڑائی,
    صحیح		اونچائی,
    صحیح		min_چوڑائی,
    صحیح		min_اونچائی,
    صحیح		بنیاد_چوڑائی,
    صحیح		بنیاد_اونچائی,
    صحیح		direction غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_XIM
    اونچائی += xim_get_حالت_area_اونچائی(),
#اگر_ختم
    Xt_Va_سیٹ_اقدار(vimShell,
	XtNwidthInc,	gui.char_چوڑائی,
	XtNheightInc,	gui.char_اونچائی,
#اگر defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	Xt_Nbase_چوڑائی,	بنیاد_چوڑائی,
	Xt_Nbase_اونچائی,	بنیاد_اونچائی,
#اگر_ختم
	Xt_Nmin_چوڑائی,	min_چوڑائی,
	Xt_Nmin_اونچائی,	min_اونچائی,
	XtNwidth,	چوڑائی,
	XtNheight,	اونچائی,
	تہی);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    خالی
gui_mch_get_screen_dimensions(
    صحیح	    *screen_w,
    صحیح	    *screen_h)
{
    *screen_w = Display_چوڑائی(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = Display_اونچائی(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحیح
gui_mch_init_font(
    char_u	*font_name,
    صحیح		do_fontset غیر_مستعمل)
{
    XFontStruct	*font = تہی;

#اگر_تعریف_شدہ FEAT_XFONTSET
    XFont_سیٹ	fontset = تہی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    اگر (font_name != تہی && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	اگر (font_name == تہی && !gui.starting)
	    واپس OK;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_XFONTSET
    اگر (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	اگر (font_name != تہی)
	{
	    fontset = (XFont_سیٹ)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    اگر (fontset == تہی)
		واپس FAIL;
	}
    }
    ورنہ
#اگر_ختم
    {
	اگر (font_name == تہی)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    اگر (font == تہی)
		font_name = (char_u *)DFLT_FONT;
	}
	اگر (font == تہی)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	اگر (font == تہی)
	    واپس FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#اگر_تعریف_شدہ FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    اگر (fontset != تہی)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_چوڑائی = fontset_چوڑائی(fontset);
	gui.char_اونچائی = fontset_اونچائی(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    ورنہ
#اگر_ختم
    {
	gui.norm_font = (GuiFont)font;
#اگر_تعریف_شدہ FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#اگر_ختم
	gui.char_چوڑائی = font->max_bounds.چوڑائی;
	gui.char_اونچائی = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_سیٹ_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    اگر (font_name == gui.rsrc_font_name)
    {
	اگر (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != تہی
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	اگر (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != تہی
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	اگر (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != تہی
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    ورنہ
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	اگر (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	اگر (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	اگر (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#اگر_تعریف_شدہ FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#اگر_ختم

    واپس OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, صحیح giveErrorIfMissing)
{
    XFontStruct	*font;

    اگر (!gui.اندر_use || name == تہی)	// can't do this when GUI not running
	واپس NOFONT;

    font = XLoadQueryFont(gui.dpy, (حرف *)name);

    اگر (font == تہی)
    {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	واپس NOFONT;
    }

#اگر_تعریف_شدہ DEBUG
    لکھو("Font Information for '%s':\n", name);
    لکھو("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.چوڑائی, font->ascent + font->descent,
	   font->ascent, font->descent);
    لکھو("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    لکھو("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    لکھو("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    لکھو("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.چوڑائی));
    لکھو("\n");
#اگر_ختم

    اگر (font->max_bounds.چوڑائی != font->min_bounds.چوڑائی)
    {
	semsg(_(e_font_str_is_not_fixed_چوڑائی), name);
	XFreeFont(gui.dpy, font);
	واپس NOFONT;
    }
    واپس (GuiFont)font;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = تہی;

    اگر (name != تہی && font == تہی)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    ورنہ اگر (font != تہی)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	غیرعلامتی طویل قدر = 0L;

	اگر (XGetFontProperty(font, XA_FONT, &قدر))
	{
	    حرف *xa_font_name = تہی;

	    xa_font_name = XGetAtomName(gui.dpy, قدر);
	    اگر (xa_font_name != تہی)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    ورنہ اگر (name != تہی)
		ret = vim_strsave(name);
	}
	ورنہ اگر (name != تہی)
	    ret = vim_strsave(name);
    }
    واپس ret;
}
#اگر_ختم

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(خالی)
{
#اگر_تعریف_شدہ FEAT_XFONTSET
    اگر (gui.fontset != NOFONTSET)
    {
	gui.char_اونچائی = fontset_اونچائی((XFont_سیٹ)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFont_سیٹ)gui.fontset)
							    + p_linespace / 2;
    }
    ورنہ
#اگر_ختم
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_اونچائی = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    واپس OK;
}

/*
 * Set the current text font.
 */
    خالی
gui_mch_سیٹ_font(GuiFont font)
{
    ساکن Font	پچھلا_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    اگر (fid != پچھلا_font)
    {
	XSetFont(gui.dpy, gui.متن_gc, fid);
	XSetFont(gui.dpy, gui.عقب_gc, fid);
	پچھلا_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#اگر_تعریف_شدہ FEAT_XFONTSET
    current_fontset = (XFont_سیٹ)تہی;
#اگر_ختم
}

#اگر defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    خالی
gui_mch_سیٹ_fontset(GuiFontset fontset)
{
    current_fontset = (XFont_سیٹ)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#اگر_ختم

/*
 * If a font is not going to be used, free its structure.
 */
    خالی
gui_mch_free_font(GuiFont font)
{
    اگر (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#اگر defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    خالی
gui_mch_free_fontset(GuiFontset fontset)
{
    اگر (fontset != NOFONTSET)
	XFree_Font_سیٹ(gui.dpy, (XFont_سیٹ)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    صحیح		giveErrorIfMissing,
    صحیح		fixed_چوڑائی)
{
    XFont_سیٹ	fontset;
    حرف	**missing, *def_str;
    صحیح		عدد_missing;

    اگر (!gui.اندر_use || name == تہی)
	واپس NOFONTSET;

    fontset = XCreate_Font_سیٹ(gui.dpy, (حرف *)name, &missing, &عدد_missing,
			     &def_str);
    اگر (عدد_missing > 0)
    {
	صحیح i;

	اگر (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_اندر_fontset), name);
	    برائے (i = 0; i < عدد_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    اگر (fontset == تہی)
    {
	اگر (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	واپس NOFONTSET;
    }

    اگر (fixed_چوڑائی && check_fontset_sanity(fontset) == FAIL)
    {
	XFree_Font_سیٹ(gui.dpy, fontset);
	واپس NOFONTSET;
    }
    واپس (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ساکن صحیح
check_fontset_sanity(XFont_سیٹ fs)
{
    XFontStruct	**xfs;
    حرف	**font_name;
    صحیح		fn;
    حرف	*بنیاد_name;
    صحیح		i;
    صحیح		min_چوڑائی;
    صحیح		min_font_idx = 0;

    بنیاد_name = XBase_Font_Name_List_کا_Font_سیٹ(fs);
    fn = XFonts_کا_Font_سیٹ(fs, &xfs, &font_name);
    برائے (i = 0; i < fn; i++)
    {
	اگر (xfs[i]->max_bounds.چوڑائی != xfs[i]->min_bounds.چوڑائی)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_چوڑائی),
		    بنیاد_name, font_name[i]);
	    واپس FAIL;
	}
    }
    // scan base font width
    min_چوڑائی = 32767;
    برائے (i = 0; i < fn; i++)
    {
	اگر (xfs[i]->max_bounds.چوڑائی<min_چوڑائی)
	{
	    min_چوڑائی = xfs[i]->max_bounds.چوڑائی;
	    min_font_idx = i;
	}
    }
    برائے (i = 0; i < fn; i++)
    {
	اگر (	   xfs[i]->max_bounds.چوڑائی != 2 * min_چوڑائی
		&& xfs[i]->max_bounds.چوڑائی != min_چوڑائی)
	{
	    semsg(_(e_fontset_name_str), بنیاد_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (صحیح)xfs[min_font_idx]->max_bounds.چوڑائی);
	    semsg(_("Font%d width: %d"), i, (صحیح)xfs[i]->max_bounds.چوڑائی);
	    واپس FAIL;
	}
    }
    // it seems ok. Good Luck!!
    واپس OK;
}

    ساکن صحیح
fontset_چوڑائی(XFont_سیٹ fs)
{
 واپس Xmb_متن_Escapement(fs, "Vim", 3) / 3;
}

    صحیح
fontset_اونچائی(
    XFont_سیٹ fs)
{
    XFont_سیٹ_Extents *extents;

    extents = XExtents_کا_Font_سیٹ(fs);
    واپس extents->max_logical_extent.اونچائی;
}

# اگر 0
// NOT USED YET
    ساکن صحیح
fontset_descent(XFont_سیٹ fs)
{
    XFont_سیٹ_Extents *extents;

    extents = XExtents_کا_Font_سیٹ (fs);
    واپس extents->max_logical_extent.اونچائی + extents->max_logical_extent.y;
}
# endif

    ساکن صحیح
fontset_ascent(XFont_سیٹ fs)
{
    XFont_سیٹ_Extents *extents;

    extents = XExtents_کا_Font_سیٹ(fs);
    واپس -extents->max_logical_extent.y;
}

#اگر_ختم // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    اگر (!gui.اندر_use || name == تہی || *name == NUL)
	واپس INVALCOLOR;

    requested = gui_get_color_cmn(name);
    اگر (requested == INVALCOLOR)
	واپس INVALCOLOR;

    واپس gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
    XColor	دستیاب;
    Colormap	colormap;

#اگر 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    حرف	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, حجم(spec), "#%.2x%.2x%.2x", r, g, b);
    اگر (XParseColor(gui.dpy, colormap, (حرف *)spec, &دستیاب) != 0
	    && XAllocColor(gui.dpy, colormap, &دستیاب) != 0)
	واپس (guicolor_T)دستیاب.pixel;
#اگر_ختم
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_میدان(دستیاب);
    دستیاب.red = r << 8;
    دستیاب.green = g << 8;
    دستیاب.blue = b << 8;
    اگر (XAllocColor(gui.dpy, colormap, &دستیاب) != 0)
	واپس (guicolor_T)دستیاب.pixel;

    واپس INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    خالی
gui_mch_سیٹ_fg_color(guicolor_T color)
{
    اگر (color == پچھلا_fg_color)
	واپس;

    XSetForeground(gui.dpy, gui.متن_gc, (Pixel)color);
    پچھلا_fg_color = color;
}

/*
 * Set the current text background color.
 */
    خالی
gui_mch_سیٹ_bg_color(guicolor_T color)
{
    اگر (color == پچھلا_bg_color)
	واپس;

    XSetBackground(gui.dpy, gui.متن_gc, (Pixel)color);
    پچھلا_bg_color = color;
}

/*
 * Set the current text special color.
 */
    خالی
gui_mch_سیٹ_sp_color(guicolor_T color)
{
    پچھلا_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    ساکن Cursor
gui_x11_create_blank_mouse(خالی)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (غیرعلامتی طویل)0, (XGCValues*)0);

    اگر (gc != تہی)
    {
	XDraw_نقطہ(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    واپس XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ساکن خالی
draw_curl(صحیح صف, صحیح ستون, صحیح cells)
{
    صحیح			i;
    صحیح			انحراف;
    ساکن ثابت صحیح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.متن_gc, پچھلا_sp_color);
    برائے (i = FILL_X(ستون); i < FILL_X(ستون + cells); ++i)
    {
	انحراف = val[i % 8];
	XDraw_نقطہ(gui.dpy, gui.wid, gui.متن_gc, i,
						FILL_Y(صف + 1) - 1 - انحراف);
    }
    XSetForeground(gui.dpy, gui.متن_gc, پچھلا_fg_color);
}

    خالی
gui_mch_draw_string(
    صحیح		صف,
    صحیح		ستون,
    char_u	*s,
    صحیح		len,
    صحیح		flags)
{
    صحیح			cells = len;
    ساکن خالی		*buf = تہی;
    ساکن صحیح		buflen = 0;
    char_u		*p;
    صحیح			wlen = 0;
    صحیح			c;

    اگر (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	اگر (buflen < len)
	{
	    XtFree((حرف *)buf);
	    buf = (خالی *)XtMalloc(len * (حجم(XChar2b) < حجم(wchar_t)
					? حجم(wchar_t) : حجم(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	جب (p < s + len)
	{
	    c = utf_ptr2char(p);
#اگر_تعریف_شدہ FEAT_XFONTSET
	    اگر (current_fontset != تہی)
	    {
# ifdef SMALL_WCHAR_T
		اگر (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    ورنہ
#اگر_ختم
	    {
		اگر (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (غیرعلامتی)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    ورنہ اگر (has_mbyte)
    {
	cells = 0;
	برائے (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#اگر_تعریف_شدہ FEAT_XFONTSET
    اگر (current_fontset != تہی)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.اونچائی = gui.char_اونچائی;
	clip.چوڑائی = gui.char_چوڑائی * cells + 1;
	XSetClipRectangles(gui.dpy, gui.متن_gc, FILL_X(ستون), FILL_Y(صف),
		&clip, 1, Unsorted);
    }
#اگر_ختم

    اگر (flags & DRAW_TRANSP)
    {
	اگر (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), buf, wlen);
	ورنہ
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), (حرف *)s, len);
    }
    ورنہ اگر (p_linespace != 0
#اگر_تعریف_شدہ FEAT_XFONTSET
	    || current_fontset != تہی
#اگر_ختم
	    )
    {
	XSetForeground(gui.dpy, gui.متن_gc, پچھلا_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		FILL_Y(صف), gui.char_چوڑائی * cells, gui.char_اونچائی);
	XSetForeground(gui.dpy, gui.متن_gc, پچھلا_fg_color);

	اگر (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), buf, wlen);
	ورنہ
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), (حرف *)s, len);
    }
    ورنہ
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	اگر (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), buf, wlen);
	ورنہ
	    XDrawImageString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(صف), (حرف *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    اگر (flags & DRAW_BOLD)
    {
	اگر (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون) + 1,
		    متن_Y(صف), buf, wlen);
	ورنہ
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون) + 1,
		    متن_Y(صف), (حرف *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    اگر (flags & DRAW_UNDERC)
	draw_curl(صف, ستون, cells);

    // Underline: draw a line at the bottom of the character cell.
    اگر (flags & DRAW_UNDERL)
    {
	صحیح	y = FILL_Y(صف + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	اگر (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_سطر(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		y, FILL_X(ستون + cells) - 1, y);
    }

    اگر (flags & DRAW_STRIKE)
    {
	صحیح	y = FILL_Y(صف + 1) - gui.char_اونچائی/2;

	XSetForeground(gui.dpy, gui.متن_gc, پچھلا_sp_color);
	XDraw_سطر(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		y, FILL_X(ستون + cells) - 1, y);
	XSetForeground(gui.dpy, gui.متن_gc, پچھلا_fg_color);
    }

#اگر_تعریف_شدہ FEAT_XFONTSET
    اگر (current_fontset != تہی)
	XSet_Clip_نقاب(gui.dpy, gui.متن_gc, None);
#اگر_ختم
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برائے (i = 0; special_کلیدیں[i].کلید_sym != (کلید_Sym)0; i++)
	اگر (name[0] == special_کلیدیں[i].vim_code0 &&
					 name[1] == special_کلیدیں[i].vim_code1)
	    واپس OK;
    واپس FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحیح
gui_get_x11_windis(کھڑکی *win, Display **dis)
{
    *win = Xt_کھڑکی(vimShell);
    *dis = gui.dpy;
    واپس OK;
}

    خالی
gui_mch_beep(خالی)
{
    XBell(gui.dpy, 0);
}

    خالی
gui_mch_flash(صحیح msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((صحیح)Columns) + gui.border_انحراف,
	    FILL_Y((صحیح)صفیں) + gui.border_انحراف);
    XSync(gui.dpy, False);
    ui_delay((طویل)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((صحیح)Columns) + gui.border_انحراف,
	    FILL_Y((صحیح)صفیں) + gui.border_انحراف);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    خالی
gui_mch_invert_rectangle(
    صحیح	    r,
    صحیح	    c,
    صحیح	    nr,
    صحیح	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_چوڑائی, (nr) * gui.char_اونچائی);
}

/*
 * Iconify the GUI window.
 */
    خالی
gui_mch_iconify(خالی)
{
    XIconify_کھڑکی(gui.dpy, Xt_کھڑکی(vimShell), DefaultScreen(gui.dpy));
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    خالی
gui_mch_سیٹ_foreground(خالی)
{
    XMapRaised(gui.dpy, Xt_کھڑکی(vimShell));
}
#اگر_ختم

/*
 * Draw a cursor without focus.
 */
    خالی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحیح		w = 1;

    اگر (mb_lefthalve(gui.صف, gui.ستون))
	w = 2;
    gui_mch_سیٹ_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.متن_gc, FILL_X(gui.ستون),
	    FILL_Y(gui.صف), w * gui.char_چوڑائی - 1, gui.char_اونچائی - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    خالی
gui_mch_draw_part_cursor(صحیح w, صحیح h, guicolor_T color)
{
    gui_mch_سیٹ_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.متن_gc,
#اگر_تعریف_شدہ FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_دایاں ? FILL_X(gui.ستون + 1) - w :
#اگر_ختم
		FILL_X(gui.ستون),
	    FILL_Y(gui.صف) + gui.char_اونچائی - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    خالی
gui_mch_update(خالی)
{
    Xt_داخلہ_نقاب نقاب, desired;

#اگر_تعریف_شدہ ALT_X_داخلہ
    اگر (suppress_alternate_داخلہ)
	desired = (XtIMXEvent | XtIMTimer);
    ورنہ
#اگر_ختم
	desired = (XtIMAll);
    جب ((نقاب = XtAppPending(app_context)) && (نقاب & desired)
	    && !vim_is_داخلہ_buf_بھرا())
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
    صحیح
gui_mch_wait_for_chars(طویل wtime)
{
    صحیح	    focus;
    صحیح	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ساکن صحیح	    timed_برآمد;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_داخلہ_نقاب	    desired;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#اگر_ختم

    timed_برآمد = FALSE;

    اگر (wtime >= 0)
	timer = Xt_App_جوڑو_وقت_برآمد(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_برآمد);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    اگر (channel_any_keep_open())
	channel_timer = Xt_App_جوڑو_وقت_برآمد(app_context, (long_u)20,
				   channel_poll_cb, (Xt_اشارہ_گر)&channel_timer);
#اگر_ختم

    focus = gui.اندر_focus;
    desired = (XtIMAll);
    جب (!timed_برآمد)
    {
	// Stop or start blinking when focus changes
	اگر (gui.اندر_focus != focus)
	{
	    اگر (gui.اندر_focus)
		gui_mch_آغاز_blink();
	    ورنہ
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندر_focus;
	}

#اگر_تعریف_شدہ پیغام_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	اگر (did_جوڑو_timer)
	    // Need to recompute the waiting time.
	    توڑو;
# endif
#اگر_ختم

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	اگر (داخلہ_دستیاب())
	{
	    retval = OK;
	    توڑو;
	}
    }

    اگر (timer != (XtIntervalId)0 && !timed_برآمد)
	Xt_Remove_وقت_برآمد(timer);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (channel_timer != (XtIntervalId)0)
	Xt_Remove_وقت_برآمد(channel_timer);
#اگر_ختم

    واپس retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    خالی
gui_mch_flush(خالی)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    خالی
gui_mch_clear_بلاک(
    صحیح		row1,
    صحیح		col1,
    صحیح		row2,
    صحیح		col2)
{
    صحیح		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.عقب_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_چوڑائی + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_اونچائی);
}

    خالی
gui_mch_clear_all(خالی)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    خالی
gui_mch_delete_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    اگر (gui.visibility == VisibilityFullyObscured)
	واپس;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.متن_gc,
	FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	gui.char_چوڑائی * (gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1)
			       + (gui.scroll_region_دایاں == Columns - 1),
	gui.char_اونچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1),
	FILL_X(gui.scroll_region_بایاں), FILL_Y(صف));

    gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
						       gui.scroll_region_بایاں,
			  gui.scroll_region_bot, gui.scroll_region_دایاں);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    خالی
gui_mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں)
{
    اگر (gui.visibility == VisibilityFullyObscured)
	واپس;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.متن_gc,
	FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	gui.char_چوڑائی * (gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1)
			       + (gui.scroll_region_دایاں == Columns - 1),
	gui.char_اونچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1),
	FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں));

    gui_clear_بلاک(صف, gui.scroll_region_بایاں,
				صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    ساکن خالی
gui_x11_check_copy_area(خالی)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    اگر (gui.visibility != VisibilityPartiallyObscured)
	واپس;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    برائے (;;)
    {
	اگر (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    واپس;	// The scroll worked.

	اگر (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->چوڑائی, gevent->اونچائی);
	    اگر (gevent->گنتی == 0)
		واپس;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    خالی
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    واپس clip_x11_own_selection(vimShell, cbd);
}

    خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    خالی
clip_mch_سیٹ_selection(
    Clipboard_T	*cbd)
{
    clip_x11_سیٹ_selection(cbd);
}

#اگر defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    خالی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->id == (Widget)0)
	واپس;

    gui_mch_menu_hidden(menu, False);
    اگر (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	Xt_سیٹ_Sensitive(menu->id, False);
    ورنہ
	Xt_سیٹ_Sensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    خالی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    اگر (menu->id == (Widget)0)
	واپس;

    اگر (hidden)
	Xt_Unmanage_بچہ(menu->id);
    ورنہ
	Xt_Manage_بچہ(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    خالی
gui_mch_draw_menubar(خالی)
{
    // Nothing to do in X
}

    خالی
gui_x11_menu_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    gui_menu_cb((vimmenu_T *)client_ڈیٹا_2);
}

#اگر_ختم // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    ساکن خالی
gui_x11_wm_protocol_handler(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    /*
     * Only deal with Client messages.
     */
    اگر (event->type != Client_پیغام)
	واپس;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    اگر ((Atom)((XClient_پیغام_Event *)event)->ڈیٹا.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	برآمد_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, Xt_کھڑکی(vimShell), تہی, 0);
	واپس;
    }

    اگر ((Atom)((XClient_پیغام_Event *)event)->ڈیٹا.l[0] !=
						  wm_atoms[DELETE_کھڑکی_IDX])
	واپس;

    gui_shell_closed();
}

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ساکن خالی
gui_x11_send_event_handler(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,
    XEvent	*event,
    Boolean	*dum غیر_مستعمل)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    اگر (e->type == PropertyNotify && e->کھڑکی_2 == comm_کھڑکی
	    && e->atom == commProperty &&  e->state == Property_New_قدر)
	serverEventProc(gui.dpy, event, 0);
}
#اگر_ختم

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کرو BLINK_NONE  0
#تعریف_کرو BLINK_OFF   1
#تعریف_کرو BLINK_ON    2

ساکن صحیح		blink_state = BLINK_NONE;
ساکن long_u		blink_waittime = 700;
ساکن long_u		blink_ontime = 400;
ساکن long_u		blink_offtime = 250;
ساکن XtIntervalId	blink_timer = (XtIntervalId)0;

    صحیح
gui_mch_is_blinking(خالی)
{
    واپس blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(خالی)
{
    واپس blink_state == BLINK_OFF;
}

    خالی
gui_mch_سیٹ_blinking(طویل waittime, طویل on, طویل off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    خالی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    اگر (blink_timer != (XtIntervalId)0)
    {
	Xt_Remove_وقت_برآمد(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ساکن خالی
gui_x11_blink_cb(
    Xt_اشارہ_گر	    timed_برآمد غیر_مستعمل,
    XtIntervalId    *interval_id غیر_مستعمل)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = Xt_App_جوڑو_وقت_برآمد(app_context, blink_offtime,
						      gui_x11_blink_cb, تہی);
    }
    ورنہ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = Xt_App_جوڑو_وقت_برآمد(app_context, blink_ontime,
						      gui_x11_blink_cb, تہی);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    خالی
gui_mch_آغاز_blink(خالی)
{
    اگر (blink_timer != (XtIntervalId)0)
	Xt_Remove_وقت_برآمد(blink_timer);
    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
    {
	blink_timer = Xt_App_جوڑو_وقت_برآمد(app_context, blink_waittime,
						      gui_x11_blink_cb, تہی);
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

    واپس (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((غیرعلامتی)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    خالی
gui_x11_callbacks(Widget متن_Area, Widget vimForm)
{
    Xt_جوڑو_Event_Handler(متن_Area, Visibility_Change_نقاب, FALSE,
	gui_x11_visibility_cb, (Xt_اشارہ_گر)0);

    Xt_جوڑو_Event_Handler(متن_Area, Exposure_نقاب, FALSE, gui_x11_expose_cb,
	(Xt_اشارہ_گر)0);

    Xt_جوڑو_Event_Handler(vimShell, Structure_Notify_نقاب, FALSE,
	gui_x11_resize_کھڑکی_cb, (Xt_اشارہ_گر)0);

    Xt_جوڑو_Event_Handler(vimShell, Focus_Change_نقاب, FALSE, gui_x11_focus_change_cb,
	(Xt_اشارہ_گر)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    اگر (vim_strchr(p_go, GO_اشارہ_گر) != تہی)
    {
	Xt_جوڑو_Event_Handler(vimShell, Leave_کھڑکی_نقاب, FALSE, gui_x11_leave_cb,
	    (Xt_اشارہ_گر)0);
	Xt_جوڑو_Event_Handler(متن_Area, Leave_کھڑکی_نقاب, FALSE, gui_x11_leave_cb,
	    (Xt_اشارہ_گر)0);
	Xt_جوڑو_Event_Handler(متن_Area, Enter_کھڑکی_نقاب, FALSE, gui_x11_enter_cb,
	    (Xt_اشارہ_گر)0);
	Xt_جوڑو_Event_Handler(vimShell, Enter_کھڑکی_نقاب, FALSE, gui_x11_enter_cb,
	    (Xt_اشارہ_گر)0);
    }

    Xt_جوڑو_Event_Handler(vimForm, کلید_Press_نقاب, FALSE, gui_x11_کلید_کامیابی_cb,
	(Xt_اشارہ_گر)0);
    Xt_جوڑو_Event_Handler(متن_Area, کلید_Press_نقاب, FALSE, gui_x11_کلید_کامیابی_cb,
	(Xt_اشارہ_گر)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    Xt_جوڑو_Event_Handler(vimForm, اشارہ_گر_Motion_نقاب,
	FALSE, gui_x11_mouse_cb, (Xt_اشارہ_گر)1);
    Xt_جوڑو_Event_Handler(متن_Area, Button_Press_نقاب | Button_Release_نقاب |
					 Button_Motion_نقاب | اشارہ_گر_Motion_نقاب,
	FALSE, gui_x11_mouse_cb, (Xt_اشارہ_گر)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    صحیح		rootx, rooty, winx, winy;
    کھڑکی	جڑ, بچہ;
    غیرعلامتی صحیح نقاب;

    اگر (gui.wid && XQuery_اشارہ_گر(gui.dpy, gui.wid, &جڑ, &بچہ,
					 &rootx, &rooty, &winx, &winy, &نقاب))
    {
	*x = winx;
	*y = winy;
    }
    ورنہ
    {
	*x = -1;
	*y = -1;
    }
}

    خالی
gui_mch_setmouse(صحیح x, صحیح y)
{
    اگر (gui.wid)
	XWarp_اشارہ_گر(gui.dpy, (کھڑکی)0, gui.wid, 0, 0, 0, 0, x, y);
}

#اگر defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_آخری_mouse_event(خالی)
{
    واپس &آخری_mouse_event;
}
#اگر_ختم

#اگر defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_چوڑائی (gui.char_چوڑائی * 2)

    خالی
gui_mch_drawsign(صحیح صف, صحیح ستون, صحیح typenr)
{
    XImage	*sign;

    اگر (!gui.اندر_use || (sign = (XImage *)sign_get_image(typenr)) == تہی)
	واپس;

    XClearArea(gui.dpy, gui.wid, متن_X(ستون), متن_Y(صف) - sign->اونچائی,
	    SIGN_چوڑائی, gui.char_اونچائی, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.متن_gc, sign, 0, 0,
	    متن_X(ستون) + (SIGN_چوڑائی - sign->چوڑائی) / 2,
	    متن_Y(صف) - sign->اونچائی,
	    sign->چوڑائی, sign->اونچائی);
}

    خالی *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    Xpm_اوصاف   attrs;
    XImage	    *sign = تہی;
    صحیح		    حالت_2;

    /*
     * Setup the color substitution table.
     */
    اگر (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_علامت color[5] =
	{
	    {"none", تہی, 0},
	    {"iconColor1", تہی, 0},
	    {"bottomShadowColor", تہی, 0},
	    {"topShadowColor", تہی, 0},
	    {"selectColor", تہی, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.عقب_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	حالت_2 = Xpm_Read_File_مقصد_Image(gui.dpy, (حرف *)signfile,
							 &sign, تہی, &attrs);
	اگر (حالت_2 == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	ورنہ
	    emsg(_(e_couldnt_read_اندر_sign_ڈیٹا));
    }

    واپس (خالی *)sign;
}

    خالی
gui_mch_destroy_sign(خالی *sign)
{
    XDestroyImage((XImage*)sign);
}
#اگر_ختم


#اگر_تعریف_شدہ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساکن صحیح آخری_shape = 0;
#اگر_ختم

/*
 * Use the blank mouse pointer or not.
 */
    خالی
gui_mch_mousehide(
    صحیح		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    اگر (gui.اشارہ_گر_hidden == hide)
	واپس;

    gui.اشارہ_گر_hidden = hide;
    اگر (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_اشارہ_گر);
    ورنہ
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	mch_سیٹ_mouse_shape(آخری_shape);
#ورنہ
    XUndefineCursor(gui.dpy, gui.wid);
#اگر_ختم
}

#اگر defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساکن صحیح mshape_ids[] =
{
    XC_بایاں_ptr,		// arrow
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
    XC_دایاں_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_بایاں_ptr			// last one
};

    خالی
mch_سیٹ_mouse_shape(صحیح shape)
{
    صحیح	    id;

    اگر (!gui.اندر_use)
	واپس;

    اگر (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_اشارہ_گر);
    ورنہ
    {
	اگر (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    اگر (id >= XC_عدد_glyphs)
		id = XC_بایاں_ptr;
	    ورنہ
		id &= ~1;	// they are always even (why?)
	}
	ورنہ
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    اگر (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}
#اگر_ختم

#اگر defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    خالی
gui_mch_menu_سیٹ_tip(vimmenu_T *menu)
{
    اگر (menu->id == تہی || menu->والد == تہی
				|| !menu_is_toolbar(menu->والد->name))
	واپس;

    // Always destroy and create the balloon, in case the string was
    // changed.
    اگر (menu->tip != تہی)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = تہی;
    }
    اگر (menu->لڑیاں[MENU_اشاریہ_TIP] != تہی)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->لڑیاں[MENU_اشاریہ_TIP],
		تہی,
		تہی);
}
#اگر_ختم
