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

#ضمن "vim.h"

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#ضمن <X11/Shell.h>
#ضمن <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#إذا defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#وإلا
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  وإلا
#   include <X11/xpm.h>
#  endif
# endif
#پایان_اگر

#اگر_تعریف_شده FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# وإلا
#  include <locale.h>
# endif
#پایان_اگر

#اگر_تعریف_شده HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#پایان_اگر

#تعریف_کن VIM_NAME	"vim"
#تعریف_کن VIM_CLASS	"Vim"

// Default resource values
#تعریف_کن DFLT_FONT		"7x13"
#اگر_تعریف_شده FONTSET_ALWAYS
# define DFLT_MENU_FONT		Xt_Default_Font_مجموعه
#وإلا
# define DFLT_MENU_FONT		XtDefaultFont
#پایان_اگر
#تعریف_کن DFLT_TOOLTIP_FONT	Xt_Default_Font_مجموعه

// use the default (CDE) colors
#تعریف_کن DFLT_MENU_BG_COLOR	""
#تعریف_کن DFLT_MENU_FG_COLOR	""
#تعریف_کن DFLT_SCROLL_BG_COLOR	""
#تعریف_کن DFLT_SCROLL_FG_COLOR	""
#تعریف_کن DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#تعریف_کن DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

ساكن Atom   wm_atoms[2];	// Window Manager Atoms
#تعریف_کن DELETE_پنجره_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#تعریف_کن SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#اگر_تعریف_شده FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ساكن XFont_مجموعه current_fontset = عدم;
# إذا !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	افعل \
	{ \
	    إذا (current_fontset != عدم) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    وإلا \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} طالما (0)
# endif
# إذا !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	افعل \
	{ \
	    إذا (current_fontset != عدم) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    وإلا \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} طالما (0)
# endif
# إذا !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	افعل \
	{ \
	    إذا (current_fontset != عدم) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    وإلا \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} طالما (0)
# endif
ساكن صحيح check_fontset_sanity(XFont_مجموعه fs);
ساكن صحيح fontset_عرض(XFont_مجموعه fs);
ساكن صحيح fontset_ascent(XFont_مجموعه fs);
#پایان_اگر

ساكن guicolor_T	prev_fg_color = INVALCOLOR;
ساكن guicolor_T	prev_bg_color = INVALCOLOR;
ساكن guicolor_T	prev_sp_color = INVALCOLOR;

#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ساكن XButtonPressedEvent آخرین_mouse_event;
#پایان_اگر

ساكن فراغ gui_x11_check_copy_area(فراغ);
#اگر_تعریف_شده FEAT_CLIENTSERVER
ساكن فراغ gui_x11_send_event_handler(Widget, Xt_اشاره_گر, XEvent *, Boolean *);
#پایان_اگر
ساكن فراغ gui_x11_wm_protocol_handler(Widget, Xt_اشاره_گر, XEvent *, Boolean *);
ساكن Cursor gui_x11_create_blank_mouse(فراغ);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ساكن بنية specialkey
{
    کلید_Sym  کلید_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_کلیدها[] =
{
    {XK_به_بالا,		'k', 'u'},
    {XK_پایین,		'k', 'd'},
    {XK_چپ,		'k', 'l'},
    {XK_راست,		'k', 'r'},

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
#اگر_تعریف_شده SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#پایان_اگر

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_پشت_Space,	'k', 'b'},
    {XK_درج,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_انتها,		'@', '7'},
    {XK_پیشین,		'k', 'P'},
    {XK_بعدی,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#اگر_تعریف_شده XK_KP_چپ
    {XK_KP_چپ,	'k', 'l'},
    {XK_KP_راست,	'k', 'r'},
    {XK_KP_به_بالا,		'k', 'u'},
    {XK_KP_پایین,	'k', 'd'},
    {XK_KP_درج,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_انتها,		'K', '4'},
    {XK_KP_پیشین,	'K', '3'},
    {XK_KP_بعدی,	'K', '5'},

    {XK_KP_جمع_کن,		'K', '6'},
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
#پایان_اگر

    // End of list marker:
    {(کلید_Sym)0,	    0, 0}
};

#تعریف_کن XtNboldFont		"boldFont"
#تعریف_کن XtCBoldFont		"BoldFont"
#تعریف_کن XtNitalicFont		"italicFont"
#تعریف_کن XtCItalicFont		"ItalicFont"
#تعریف_کن XtNboldItalicFont	"boldItalicFont"
#تعریف_کن XtCBoldItalicFont	"BoldItalicFont"
#تعریف_کن Xt_Nscrollbar_عرض	"scrollbarWidth"
#تعریف_کن Xt_CScrollbar_عرض	"ScrollbarWidth"
#تعریف_کن Xt_Nmenu_ارتفاع		"menuHeight"
#تعریف_کن Xt_CMenu_ارتفاع		"MenuHeight"
#تعریف_کن XtNmenuFont		"menuFont"
#تعریف_کن XtCMenuFont		"MenuFont"
#تعریف_کن Xt_Nmenu_Font_مجموعه		"menuFontSet"
#تعریف_کن Xt_CMenu_Font_مجموعه		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#تعریف_کن XtNmenuBackground	"menuBackground"
#تعریف_کن XtCMenuBackground	"MenuBackground"
#تعریف_کن XtNmenuForeground	"menuForeground"
#تعریف_کن XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#تعریف_کن XtNscrollBackground	"scrollBackground"
#تعریف_کن XtCScrollBackground	"ScrollBackground"
#تعریف_کن XtNscrollForeground	"scrollForeground"
#تعریف_کن XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#تعریف_کن XtNtooltipBackground	"tooltipBackground"
#تعریف_کن XtCTooltipBackground	"TooltipBackground"
#تعریف_کن XtNtooltipForeground	"tooltipForeground"
#تعریف_کن XtCTooltipForeground	"TooltipForeground"
#تعریف_کن XtNtooltipFont		"tooltipFont"
#تعریف_کن XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
ساكن XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	حجم(Pixel),
	Xt_جابه_جایی_از(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	حجم(Pixel),
	Xt_جابه_جایی_از(gui_T, def_پشت_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	حجم(String *),
	Xt_جابه_جایی_از(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	حجم(String *),
	Xt_جابه_جایی_از(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	حجم(String *),
	Xt_جابه_جایی_از(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	حجم(String *),
	Xt_جابه_جایی_از(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	حجم(String *),
	Xt_جابه_جایی_از(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	حجم(Bool),
	Xt_جابه_جایی_از(gui_T, rsrc_rev_video),
	XtRImmediate,
	(Xt_اشاره_گر)False
    },
    {
	Xt_Nborder_عرض,
	Xt_CBorder_عرض,
	XtRInt,
	حجم(صحيح),
	Xt_جابه_جایی_از(gui_T, border_عرض),
	XtRImmediate,
	(Xt_اشاره_گر)2
    },
    {
	Xt_Nscrollbar_عرض,
	Xt_CScrollbar_عرض,
	XtRInt,
	حجم(صحيح),
	Xt_جابه_جایی_از(gui_T, scrollbar_عرض),
	XtRImmediate,
	(Xt_اشاره_گر)SB_DEFAULT_عرض
    },
#اگر_تعریف_شده FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	Xt_Nmenu_Font_مجموعه,
	Xt_CMenu_Font_مجموعه,
# وإلا
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#پایان_اگر
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#اگر_تعریف_شده FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	حجم(حرف *),
	Xt_جابه_جایی_از(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	Xt_CFont_مجموعه,
	Xt_RFont_مجموعه,
	حجم(XFont_مجموعه),
	Xt_جابه_جایی_از(gui_T, tooltip_fontset),
	XtRImmediate,
	(Xt_اشاره_گر)NOFONTSET
    },
#پایان_اگر // FEAT_BEVAL_GUI
#اگر_تعریف_شده FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	حجم(حرف*),
	Xt_جابه_جایی_از(gui_T, rsrc_preedit_type_name),
	XtRString,
	(Xt_اشاره_گر)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	حجم(حرف*),
	Xt_جابه_جایی_از(gui_T, rsrc_درون_داد_روش),
	XtRString,
	عدم
    },
#پایان_اگر // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
ساكن XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	عدم},
    {"-background",	".background",	    XrmoptionSepArg,	عدم},
    {"-fg",		".foreground",	    XrmoptionSepArg,	عدم},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	عدم},
    {"-fn",		".font",	    XrmoptionSepArg,	عدم},
    {"-font",		".font",	    XrmoptionSepArg,	عدم},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	عدم},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	عدم},
    {"-geom",		".geometry",	    XrmoptionSepArg,	عدم},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	عدم},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	عدم},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	عدم},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	عدم},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	عدم},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	عدم},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	عدم},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	عدم},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	عدم},
#اگر_تعریف_شده FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	عدم},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	عدم},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	عدم},
#وإلا
    {"-mf",		".menuFont",	    XrmoptionSepArg,	عدم},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	عدم},
#پایان_اگر
    {"-xrm",		عدم,		    XrmoptionResArg,	عدم}
};

ساكن صحيح gui_argc = 0;
ساكن حرف **gui_argv = عدم;

/*
 * Call-back routines.
 */

    ساكن فراغ
gui_x11_timer_cb(
    Xt_اشاره_گر	    timed_بیرون,
    XtIntervalId    *interval_id استفادهنشده)
{
    *((صحيح *)timed_بیرون) = TRUE;
}

#اگر_تعریف_شده FEAT_کار_CHANNEL
    ساكن فراغ
channel_poll_cb(
    Xt_اشاره_گر	    client_داده_2,
    XtIntervalId    *interval_id استفادهنشده)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_داده_2;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = Xt_App_جمع_کن_زمان_بیرون(app_context, (long_u)20,
						 channel_poll_cb, client_داده_2);
}
#پایان_اگر

    ساكن فراغ
gui_x11_visibility_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	dud استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    إذا (event->type != VisibilityNotify)
	أرجع;

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

    ساكن فراغ
gui_x11_expose_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	dud استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    XExposeEvent	*gevent;
    صحيح			new_x;

    إذا (event->type != Expose)
	أرجع;

    بیرون_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->عرض, gevent->ارتفاع);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    إذا (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    إذا (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    إذا (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((صحيح)Columns), 0, 0, 0, False);
    إذا (gevent->y > FILL_Y(ردیفها))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((صحيح)ردیفها), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#إذا defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    ساكن فراغ
shellRectangle(Widget shell, XRectangle *r)
{
    پنجره		rootw, shellw, فرزند, parentw;
    صحيح			absx, absy;
    XWindow_صفات	a;
    پنجره		*children;
    بیعلامت صحيح	children_شمار;

    shellw = Xt_پنجره(shell);
    إذا (shellw == 0)
	أرجع;
    لكل (;;)
    {
	XQuery_درخت(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_شمار);
	XFree(children);
	إذا (parentw == rootw)
	    اكسر;
	shellw = parentw;
    }
    XGet_پنجره_صفات(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.ریشه, 0, 0,
							&absx, &absy, &فرزند);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_مقادیر(shell, XmNheight, &r->ارتفاع, XmNwidth, &r->عرض, عدم);
}
#پایان_اگر

    ساكن فراغ
gui_x11_resize_پنجره_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	dud استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    ساكن صحيح آخرین_عرض_2, آخرین_ارتفاع_2;

    إذا (event->type != ConfigureNotify)
	أرجع;

    إذا (event->xconfigure.عرض != آخرین_عرض_2
	    || event->xconfigure.ارتفاع != آخرین_ارتفاع_2)
    {
	آخرین_عرض_2 = event->xconfigure.عرض;
	آخرین_ارتفاع_2 = event->xconfigure.ارتفاع;
	gui_resize_shell(event->xconfigure.عرض, event->xconfigure.ارتفاع
#اگر_تعریف_شده FEAT_XIM
						- xim_get_وضعیت_area_ارتفاع()
#پایان_اگر
		     );
    }
#إذا defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    إذا (netbeans_فعال())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_قاب_moved(rec.x, rec.y);
    }
#پایان_اگر
#اگر_تعریف_شده FEAT_XIM
    xim_مجموعه_preedit();
#پایان_اگر
}

    ساكن فراغ
gui_x11_focus_change_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	داده استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    gui_focus_change(event->type == Focus_درون);
}

    ساكن فراغ
gui_x11_enter_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	داده استفادهنشده,
    XEvent	*event استفادهنشده,
    Boolean	*dum استفادهنشده)
{
    gui_focus_change(TRUE);
}

    ساكن فراغ
gui_x11_leave_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	داده استفادهنشده,
    XEvent	*event استفادهنشده,
    Boolean	*dum استفادهنشده)
{
    gui_focus_change(FALSE);
}

#إذا defined(X_HAVE_UTF8_STRING)
# إذا X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#پایان_اگر

    فراغ
gui_x11_کلید_اصابت_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	dud استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    XKeyPressedEvent	*ev_press;
#اگر_تعریف_شده FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    وضعیت_2		وضعیت;
#وإلا
    char_u		string[4], string2[3];
#پایان_اگر
    کلید_Sym		کلید_sym;
    صحيح			len;
    صحيح			i;
    صحيح			modifiers;
    صحيح			کلید;

    ev_press = (XKeyPressedEvent *)event;

#اگر_تعریف_شده FEAT_XIM
    إذا (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	إذا (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (حرف *)string,
				  حجم(string_shortbuf), &کلید_sym, &وضعیت);
	وإلا
# endif
	    len = XmbLookupString(xic, ev_press, (حرف *)string,
				  حجم(string_shortbuf), &کلید_sym, &وضعیت);
	إذا (وضعیت == XBuffer_سرریز)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    إذا (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (حرف *)string,
						      len, &کلید_sym, &وضعیت);
	    وإلا
# endif
		len = XmbLookupString(xic, ev_press, (حرف *)string,
						      len, &کلید_sym, &وضعیت);
	}
	إذا (وضعیت == XLookupNone || وضعیت == XLookupChars)
	    کلید_sym = XK_Void_نماد;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	إذا (len > 0 && درون_داد_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    صحيح		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    إذا (string_alloced)
		XtFree((حرف *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_درون_داد(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	لكل (i = 0; i < len; ++i)
	    إذا (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (صحيح)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		إذا (string_alloced)
		    XtFree((حرف *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    وإلا
#پایان_اگر
	len = XLookupString(ev_press, (حرف *)string, حجم(string),
		&کلید_sym, عدم);

#اگر_تعریف_شده SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    إذا (کلید_sym == SunXK_F36 || کلید_sym == SunXK_F37)
	len = 0;
#پایان_اگر

    إذا (کلید_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    إذا (کلید_sym == XK_minus && (ev_press->state & Control_ماسک))
	string[0] = Ctrl__;

#اگر_تعریف_شده XK_ISO_چپ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    إذا (کلید_sym == XK_ISO_چپ_Tab)
    {
	کلید_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#پایان_اگر

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    إذا (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (صحيح)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیدها[i].کلید_sym != (کلید_Sym)0; i++)
	{
	    إذا (special_کلیدها[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدها[i].vim_code0;
		string[2] = special_کلیدها[i].vim_code1;
		len = -3;
		اكسر;
	    }
	}
    }

    // Unrecognised key is ignored.
    إذا (len == 0)
	اذهب theend;

    // Handle modifiers.
    modifiers = 0;
    إذا (ev_press->state & لغزش_ماسک)
	modifiers |= MOD_ماسک_لغزش;
    إذا (ev_press->state & Control_ماسک)
    {
	modifiers |= MOD_ماسک_CTRL;
	إذا (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    إذا (ev_press->state & Mod1_ماسک)
	modifiers |= MOD_ماسک_ALT;
    إذا (ev_press->state & Mod4_ماسک)
	modifiers |= MOD_ماسک_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    إذا (len == -3)
	کلید = مقصد_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }
    کلید = simplify_کلید(کلید, &modifiers);
    إذا (کلید == CSI)
	کلید = K_CSI;
    إذا (IS_SPECIAL(کلید))
    {
	string[0] = CSI;
	string[1] = K_دومی(کلید);
	string[2] = K_سوم(کلید);
	len = 3;
    }
    وإلا
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

	len = mb_char2bytes(کلید, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_لغزش_modifier(modifiers, کلید);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جمع_کن_مقصد_درون_داد_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	صحيح int_ch = check_for_وقفه(کلید, modifiers);

	إذا (int_ch != NUL)
	{
	    trash_درون_داد_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    جمع_کن_مقصد_درون_داد_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    إذا (p_mh)
	gui_mch_mousehide(TRUE);

#إذا defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	إذا ((be = gui_mch_currently_showing_beval()) != عدم)
	    gui_mch_unpost_balloon(be);
    }
#پایان_اگر
theend:
    {}	    // some compilers need a statement here
#اگر_تعریف_شده FEAT_XIM
    إذا (string_alloced)
	XtFree((حرف *)string);
#پایان_اگر
}

    ساكن فراغ
gui_x11_mouse_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	dud استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    ساكن XtIntervalId timer = (XtIntervalId)0;
    ساكن صحيح	timed_بیرون = TRUE;

    صحيح		button;
    صحيح		repeated_click = FALSE;
    صحيح		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    إذا (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1_ماسک | Button2_ماسک | Button3_ماسک))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	إذا (button != MOUSE_DRAG)	// just moving the rodent
	{
#اگر_تعریف_شده FEAT_MENU
	    إذا (dud)			// moved in vimForm
		y -= gui.menu_ارتفاع;
#پایان_اگر
	    gui_mouse_moved(x, y);
	    أرجع;
	}
    }
    وإلا
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	إذا (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    إذا (!timed_بیرون)
	    {
		Xt_Remove_زمان_بیرون(timer);
		repeated_click = TRUE;
	    }
	    timed_بیرون = FALSE;
	    timer = Xt_App_جمع_کن_زمان_بیرون(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_بیرون);
	    بدل (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		حالة Button1:	button = MOUSE_چپ;	اكسر;
		حالة Button2:	button = MOUSE_میانه;	اكسر;
		حالة Button3:	button = MOUSE_راست;	اكسر;
		حالة Button4:	button = MOUSE_4;	اكسر;
		حالة Button5:	button = MOUSE_5;	اكسر;
		حالة 6:		button = MOUSE_7;	اكسر;
		حالة 7:		button = MOUSE_6;	اكسر;
		حالة 8:		button = MOUSE_X1;	اكسر;
		حالة 9:		button = MOUSE_X2;	اكسر;
		افتراضي:
		    أرجع;	// Unknown button
	    }
	}
	وإلا إذا (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	وإلا
	    أرجع;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	آخرین_mouse_event = event->xbutton;
#پایان_اگر
    }

    vim_modifiers = 0x0;
    إذا (x_modifiers & لغزش_ماسک)
	vim_modifiers |= MOUSE_لغزش;
    إذا (x_modifiers & Control_ماسک)
	vim_modifiers |= MOUSE_CTRL;
    إذا (x_modifiers & Mod1_ماسک)	    // Alt or Meta key
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
    فراغ
gui_mch_prepare(صحيح *argc, حرف **argv)
{
    صحيح	    arg;
    صحيح	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(حرف *, *argc);
    إذا (gui_argv == عدم)
	أرجع;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    طالما (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	لكل (i = 0; i < (صحيح)Xt_عدد(cmdline_options); i++)
	    إذا (strcmp(argv[arg], cmdline_options[i].option) == 0)
		اكسر;

	إذا (i < (صحيح)Xt_عدد(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    إذا (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		یافت_معکوس_arg = TRUE;
	    وإلا إذا ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_آرگومان = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    إذا (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * حجم(حرف *));
		إذا (cmdline_options[i].arg_نوع != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    إذا (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * حجم(حرف *));
		}
	    }
	    argv[*argc] = عدم;
	}
	وإلا
#اگر_تعریف_شده FEAT_NETBEANS_INTG
	    إذا (مقایسه_رشته("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * حجم(حرف *));
	    argv[*argc] = عدم;
	}
	وإلا
#پایان_اگر
	    arg++;
    }
}

#اگر_تعریف_نشده XtSpecificationRelease
# define CARDINAL (Cardinal *)
#وإلا
# إذا XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# وإلا
#  define CARDINAL (صحيح *)
# endif
#پایان_اگر

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    صحيح
gui_mch_init_check(فراغ)
{
#اگر_تعریف_شده FEAT_XIM
    Xt_مجموعه_Language_Proc(عدم, عدم, عدم);
#پایان_اگر
    open_app_context();
    إذا (app_context != عدم)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_عدد(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#إذا defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	حرف *p = setlocale(LC_NUMERIC, عدم);

	إذا (p == عدم || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#پایان_اگر
    إذا (app_context == عدم || gui.dpy == عدم)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	أرجع FAIL;
    }
    أرجع OK;
}


#اگر_تعریف_شده USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ساكن Xt_درون_داد_Id _xsmp_xtinputid;

    ساكن فراغ
local_xsmp_handle_درخواستها(
    Xt_اشاره_گر	c استفادهنشده,
    صحيح		*s استفادهنشده,
    Xt_درون_داد_Id	*i استفادهنشده)
{
    إذا (xsmp_handle_درخواستها() == FAIL)
	Xt_Remove_درون_داد(_xsmp_xtinputid);
}
#پایان_اگر


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحيح
gui_mch_init(فراغ)
{
    XtGCMask	gc_ماسک;
    XGCValues	gc_vals;
    صحيح		x, y, ماسک;
    بیعلامت	w, h;

#إذا 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#پایان_اگر

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, عدم);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (Xt_اشاره_گر)&gui,
	vim_resources, Xt_عدد(vim_resources), عدم);

    gui.scrollbar_ارتفاع = gui.scrollbar_عرض;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#اگر_تعریف_شده FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#پایان_اگر

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پشت_pixel = gui.def_پشت_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    إذا (gui.rsrc_rev_video && gui_get_lightness(gui.پشت_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_پشت_pixel;
	gui.پشت_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_پشت_pixel = gui.پشت_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    مجموعه_نرمال_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_ماسک = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.پشت_pixel;
    gui.متن_gc = XtGetGC(vimShell, gc_ماسک, &gc_vals);

    gc_vals.foreground = gui.پشت_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.پشت_gc = XtGetGC(vimShell, gc_ماسک, &gc_vals);

    gc_ماسک |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.پشت_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.پشت_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_ماسک, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_راهاندازی_atoms(gui.dpy);

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_مجموعه_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    إذا (gui.geom != عدم && *gui.geom != NUL)
    {
	ماسک = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);
	إذا (ماسک & عرض_مقدار)
	    Columns = w;
	إذا (ماسک & ارتفاع_مقدار)
	{
	    إذا (p_پنجره > (دراز)h - 1 || !option_was_مجموعه((char_u *)"window"))
		p_پنجره = h - 1;
	    ردیفها = h;
	}
	حد_screen_اندازه();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	إذا (ماسک & (XValue|YValue))
	    Xt_Va_مجموعه_مقادیر(vimShell, XtNgeometry, gui.geom, عدم);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    إذا (vim_strchr(p_go, GO_ICON) != عدم)
    {
#اگر_تعریف_نشده HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	Xt_مجموعه_Arg(arg[0], XtNiconPixmap,
		XCreate_Bitmap_From_داده(gui.dpy,
		    Default_ریشه_پنجره(gui.dpy),
		    (حرف *)vim_icon_بیتها,
		    vim_icon_عرض,
		    vim_icon_ارتفاع));
	Xt_مجموعه_Arg(arg[1], Xt_Nicon_ماسک,
		XCreate_Bitmap_From_داده(gui.dpy,
		    Default_ریشه_پنجره(gui.dpy),
		    (حرف *)vim_ماسک_icon_بیتها,
		    vim_ماسک_icon_عرض,
		    vim_ماسک_icon_ارتفاع));
	Xt_مجموعه_مقادیر(vimShell, arg, (Cardinal)2);
#وإلا
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ساكن Pixmap	icon = 0;
    ساكن Pixmap	icon_ماسک = 0;
    ساكن حرف		**magick = vim32x32;
    پنجره		ریشه_پنجره;
    XIcon_اندازه		*اندازه;
    صحيح			عدد_sizes;
    Display		*dsp;
    Screen		*scr;
    Xpm_صفات	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    ریشه_پنجره = XRoot_پنجره_از_Screen(XtScreen(vimShell));
    إذا (XGetIconSizes(XtDisplay(vimShell), ریشه_پنجره,
						   &اندازه, &عدد_sizes) != 0)
    {
	إذا (عدد_sizes > 0)
	{
	    إذا (اندازه->max_ارتفاع_2 >= 48 && اندازه->max_عرض_2 >= 48)
		magick = vim48x48;
	    وإلا إذا (اندازه->max_ارتفاع_2 >= 32 && اندازه->max_عرض_2 >= 32)
		magick = vim32x32;
	    وإلا إذا (اندازه->max_ارتفاع_2 >= 16 && اندازه->max_عرض_2 >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_مجموعه_مقادیر(vimShell, XtNcolormap, cmap, عدم);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | Xpm_عمق;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.عمق = Default_عمق_از_Screen(scr);

    إذا (!icon)
    {
	Xpm_Create_Pixmap_From_داده(dsp, ریشه_پنجره, magick, &icon,
							   &icon_ماسک, &attr);
	Xpm_Free_صفات(&attr);
    }

    Xt_Va_مجموعه_مقادیر(vimShell, XmNiconPixmap, icon, Xm_Nicon_ماسک, icon_ماسک, عدم);
#پایان_اگر
    }

#اگر_تعریف_شده FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
    gui_init_menu_font();
#پایان_اگر

#اگر_تعریف_شده USE_XSMP
    // Attach listener on ICE connection
    إذا (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_جمع_کن_درون_داد(app_context, xsmp_icefd,
		(Xt_اشاره_گر)Xt_درون_داد_Read_ماسک, local_xsmp_handle_درخواستها, عدم);
#پایان_اگر

    أرجع OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    فراغ
gui_mch_uninit(فراغ)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = عدم;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    long_u	gc_ماسک;
    XGCValues	gc_vals;

    gc_ماسک = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.پشت_pixel;
    إذا (gui.متن_gc != عدم)
	XChangeGC(gui.dpy, gui.متن_gc, gc_ماسک, &gc_vals);

    gc_vals.foreground = gui.پشت_pixel;
    gc_vals.background = gui.norm_pixel;
    إذا (gui.پشت_gc != عدم)
	XChangeGC(gui.dpy, gui.پشت_gc, gc_ماسک, &gc_vals);

    gc_ماسک |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.پشت_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.پشت_pixel;
    gc_vals.function   = GXxor;
    إذا (gui.invert_gc != عدم)
	XChangeGC(gui.dpy, gui.invert_gc, gc_ماسک, &gc_vals);

    gui_x11_مجموعه_پشت_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    Xt_Manage_فرزند(Xt_Name_مقصد_Widget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_اشاره_گر = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_پنجره_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, Xt_پنجره(vimShell), wm_atoms, 2);
    Xt_جمع_کن_Event_Handler(vimShell, No_Event_ماسک, True, gui_x11_wm_protocol_handler,
							     عدم);
#اگر_تعریف_شده HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    Xt_جمع_کن_Event_Handler(vimShell, (Event_ماسک)0, True, _XEditResCheckMessages,
	    (Xt_اشاره_گر)عدم);
#پایان_اگر

#اگر_تعریف_شده FEAT_CLIENTSERVER
    إذا (serverName == عدم && server_Delayed_شروع_Name != عدم)
    {
	// This is a :gui command in a plain vim with no previous server
	comm_پنجره = Xt_پنجره(vimShell);
	(فراغ)server_ثبات_Name(gui.dpy, server_Delayed_شروع_Name);
    }
    وإلا
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	server_Change_Registered_پنجره(gui.dpy, Xt_پنجره(vimShell));
    }
    Xt_جمع_کن_Event_Handler(vimShell, Property_Change_ماسک, False,
		      gui_x11_send_event_handler, عدم);
#پایان_اگر

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#اگر_تعریف_شده FEAT_XIM
    xim_init();
#پایان_اگر

    أرجع OK;
}

#إذا defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    فراغ
gui_init_tooltip_font(فراغ)
{
    Xrm_مقدار from, مقصد;

    from.addr = (حرف *)gui.rsrc_tooltip_font_name;
    from.اندازه = طول_رشته(from.addr);
    مقصد.addr = (Xt_اشاره_گر)&gui.tooltip_fontset;
    مقصد.اندازه = حجم(XFont_مجموعه);

    إذا (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_مجموعه, &مقصد) == False)
    {
	// Failed. What to do?
    }
}
#پایان_اگر

#إذا defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    فراغ
gui_init_menu_font(فراغ)
{
    Xrm_مقدار from, مقصد;

# ifdef FONTSET_ALWAYS
    from.addr = (حرف *)gui.rsrc_menu_font_name;
    from.اندازه = طول_رشته(from.addr);
    مقصد.addr = (Xt_اشاره_گر)&gui.menu_fontset;
    مقصد.اندازه = حجم(GuiFontset);

    إذا (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_مجموعه, &مقصد) == False)
    {
	// Failed. What to do?
    }
# وإلا
    from.addr = (حرف *)gui.rsrc_menu_font_name;
    from.اندازه = طول_رشته(from.addr);
    مقصد.addr = (Xt_اشاره_گر)&gui.menu_font;
    مقصد.اندازه = حجم(GuiFont);

    إذا (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &مقصد) == False)
    {
	// Failed. What to do?
    }
# endif
}
#پایان_اگر

    فراغ
gui_mch_exit(صحيح rc استفادهنشده)
{
#إذا 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#پایان_اگر
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_مقادیر(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	عدم);
    *x = xpos;
    *y = ypos;
    أرجع OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_مجموعه_winpos(صحيح x, صحيح y)
{
    Xt_Va_مجموعه_مقادیر(vimShell,
	XtNx,	x,
	XtNy,	y,
	عدم);
}

    فراغ
gui_mch_مجموعه_shellsize(
    صحيح		عرض,
    صحيح		ارتفاع,
    صحيح		min_عرض,
    صحيح		min_ارتفاع,
    صحيح		پایه_عرض,
    صحيح		پایه_ارتفاع,
    صحيح		direction استفادهنشده)
{
#اگر_تعریف_شده FEAT_XIM
    ارتفاع += xim_get_وضعیت_area_ارتفاع(),
#پایان_اگر
    Xt_Va_مجموعه_مقادیر(vimShell,
	XtNwidthInc,	gui.char_عرض,
	XtNheightInc,	gui.char_ارتفاع,
#إذا defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	Xt_Nbase_عرض,	پایه_عرض,
	Xt_Nbase_ارتفاع,	پایه_ارتفاع,
#پایان_اگر
	Xt_Nmin_عرض,	min_عرض,
	Xt_Nmin_ارتفاع,	min_ارتفاع,
	XtNwidth,	عرض,
	XtNheight,	ارتفاع,
	عدم);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    فراغ
gui_mch_get_screen_dimensions(
    صحيح	    *screen_w,
    صحيح	    *screen_h)
{
    *screen_w = Display_عرض(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = Display_ارتفاع(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحيح
gui_mch_init_font(
    char_u	*font_name,
    صحيح		do_fontset استفادهنشده)
{
    XFontStruct	*font = عدم;

#اگر_تعریف_شده FEAT_XFONTSET
    XFont_مجموعه	fontset = عدم;
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    إذا (font_name != عدم && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	إذا (font_name == عدم && !gui.starting)
	    أرجع OK;
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_XFONTSET
    إذا (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	إذا (font_name != عدم)
	{
	    fontset = (XFont_مجموعه)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    إذا (fontset == عدم)
		أرجع FAIL;
	}
    }
    وإلا
#پایان_اگر
    {
	إذا (font_name == عدم)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    إذا (font == عدم)
		font_name = (char_u *)DFLT_FONT;
	}
	إذا (font == عدم)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	إذا (font == عدم)
	    أرجع FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#اگر_تعریف_شده FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    إذا (fontset != عدم)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_عرض = fontset_عرض(fontset);
	gui.char_ارتفاع = fontset_ارتفاع(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    وإلا
#پایان_اگر
    {
	gui.norm_font = (GuiFont)font;
#اگر_تعریف_شده FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#پایان_اگر
	gui.char_عرض = font->max_bounds.عرض;
	gui.char_ارتفاع = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_مجموعه_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    إذا (font_name == gui.rsrc_font_name)
    {
	إذا (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != عدم
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	إذا (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != عدم
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	إذا (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != عدم
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    وإلا
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	إذا (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	إذا (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	إذا (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#اگر_تعریف_شده FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#پایان_اگر

    أرجع OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, صحيح giveErrorIfMissing)
{
    XFontStruct	*font;

    إذا (!gui.درون_use || name == عدم)	// can't do this when GUI not running
	أرجع NOFONT;

    font = XLoadQueryFont(gui.dpy, (حرف *)name);

    إذا (font == عدم)
    {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	أرجع NOFONT;
    }

#اگر_تعریف_شده DEBUG
    چاپ_کن("Font Information for '%s':\n", name);
    چاپ_کن("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.عرض, font->ascent + font->descent,
	   font->ascent, font->descent);
    چاپ_کن("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    چاپ_کن("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    چاپ_کن("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    چاپ_کن("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.عرض));
    چاپ_کن("\n");
#پایان_اگر

    إذا (font->max_bounds.عرض != font->min_bounds.عرض)
    {
	semsg(_(e_font_str_is_not_fixed_عرض), name);
	XFreeFont(gui.dpy, font);
	أرجع NOFONT;
    }
    أرجع (GuiFont)font;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = عدم;

    إذا (name != عدم && font == عدم)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    وإلا إذا (font != عدم)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	بیعلامت دراز مقدار = 0L;

	إذا (XGetFontProperty(font, XA_FONT, &مقدار))
	{
	    حرف *xa_font_name = عدم;

	    xa_font_name = XGetAtomName(gui.dpy, مقدار);
	    إذا (xa_font_name != عدم)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    وإلا إذا (name != عدم)
		ret = vim_strsave(name);
	}
	وإلا إذا (name != عدم)
	    ret = vim_strsave(name);
    }
    أرجع ret;
}
#پایان_اگر

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
gui_mch_adjust_charheight(فراغ)
{
#اگر_تعریف_شده FEAT_XFONTSET
    إذا (gui.fontset != NOFONTSET)
    {
	gui.char_ارتفاع = fontset_ارتفاع((XFont_مجموعه)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFont_مجموعه)gui.fontset)
							    + p_linespace / 2;
    }
    وإلا
#پایان_اگر
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_ارتفاع = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    أرجع OK;
}

/*
 * Set the current text font.
 */
    فراغ
gui_mch_مجموعه_font(GuiFont font)
{
    ساكن Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    إذا (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.متن_gc, fid);
	XSetFont(gui.dpy, gui.پشت_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#اگر_تعریف_شده FEAT_XFONTSET
    current_fontset = (XFont_مجموعه)عدم;
#پایان_اگر
}

#إذا defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    فراغ
gui_mch_مجموعه_fontset(GuiFontset fontset)
{
    current_fontset = (XFont_مجموعه)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#پایان_اگر

/*
 * If a font is not going to be used, free its structure.
 */
    فراغ
gui_mch_free_font(GuiFont font)
{
    إذا (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#إذا defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    فراغ
gui_mch_free_fontset(GuiFontset fontset)
{
    إذا (fontset != NOFONTSET)
	XFree_Font_مجموعه(gui.dpy, (XFont_مجموعه)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    صحيح		giveErrorIfMissing,
    صحيح		fixed_عرض)
{
    XFont_مجموعه	fontset;
    حرف	**missing, *def_str;
    صحيح		رقم_missing;

    إذا (!gui.درون_use || name == عدم)
	أرجع NOFONTSET;

    fontset = XCreate_Font_مجموعه(gui.dpy, (حرف *)name, &missing, &رقم_missing,
			     &def_str);
    إذا (رقم_missing > 0)
    {
	صحيح i;

	إذا (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_درون_fontset), name);
	    لكل (i = 0; i < رقم_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    إذا (fontset == عدم)
    {
	إذا (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	أرجع NOFONTSET;
    }

    إذا (fixed_عرض && check_fontset_sanity(fontset) == FAIL)
    {
	XFree_Font_مجموعه(gui.dpy, fontset);
	أرجع NOFONTSET;
    }
    أرجع (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ساكن صحيح
check_fontset_sanity(XFont_مجموعه fs)
{
    XFontStruct	**xfs;
    حرف	**font_name;
    صحيح		fn;
    حرف	*پایه_name;
    صحيح		i;
    صحيح		min_عرض;
    صحيح		min_font_idx = 0;

    پایه_name = XBase_Font_Name_List_از_Font_مجموعه(fs);
    fn = XFonts_از_Font_مجموعه(fs, &xfs, &font_name);
    لكل (i = 0; i < fn; i++)
    {
	إذا (xfs[i]->max_bounds.عرض != xfs[i]->min_bounds.عرض)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_عرض),
		    پایه_name, font_name[i]);
	    أرجع FAIL;
	}
    }
    // scan base font width
    min_عرض = 32767;
    لكل (i = 0; i < fn; i++)
    {
	إذا (xfs[i]->max_bounds.عرض<min_عرض)
	{
	    min_عرض = xfs[i]->max_bounds.عرض;
	    min_font_idx = i;
	}
    }
    لكل (i = 0; i < fn; i++)
    {
	إذا (	   xfs[i]->max_bounds.عرض != 2 * min_عرض
		&& xfs[i]->max_bounds.عرض != min_عرض)
	{
	    semsg(_(e_fontset_name_str), پایه_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (صحيح)xfs[min_font_idx]->max_bounds.عرض);
	    semsg(_("Font%d width: %d"), i, (صحيح)xfs[i]->max_bounds.عرض);
	    أرجع FAIL;
	}
    }
    // it seems ok. Good Luck!!
    أرجع OK;
}

    ساكن صحيح
fontset_عرض(XFont_مجموعه fs)
{
 أرجع Xmb_متن_Escapement(fs, "Vim", 3) / 3;
}

    صحيح
fontset_ارتفاع(
    XFont_مجموعه fs)
{
    XFont_مجموعه_Extents *extents;

    extents = XExtents_از_Font_مجموعه(fs);
    أرجع extents->max_logical_extent.ارتفاع;
}

# إذا 0
// NOT USED YET
    ساكن صحيح
fontset_descent(XFont_مجموعه fs)
{
    XFont_مجموعه_Extents *extents;

    extents = XExtents_از_Font_مجموعه (fs);
    أرجع extents->max_logical_extent.ارتفاع + extents->max_logical_extent.y;
}
# endif

    ساكن صحيح
fontset_ascent(XFont_مجموعه fs)
{
    XFont_مجموعه_Extents *extents;

    extents = XExtents_از_Font_مجموعه(fs);
    أرجع -extents->max_logical_extent.y;
}

#پایان_اگر // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    إذا (!gui.درون_use || name == عدم || *name == NUL)
	أرجع INVALCOLOR;

    requested = gui_get_color_cmn(name);
    إذا (requested == INVALCOLOR)
	أرجع INVALCOLOR;

    أرجع gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
    XColor	موجود;
    Colormap	colormap;

#إذا 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    حرف	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, حجم(spec), "#%.2x%.2x%.2x", r, g, b);
    إذا (XParseColor(gui.dpy, colormap, (حرف *)spec, &موجود) != 0
	    && XAllocColor(gui.dpy, colormap, &موجود) != 0)
	أرجع (guicolor_T)موجود.pixel;
#پایان_اگر
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_فیلد(موجود);
    موجود.red = r << 8;
    موجود.green = g << 8;
    موجود.blue = b << 8;
    إذا (XAllocColor(gui.dpy, colormap, &موجود) != 0)
	أرجع (guicolor_T)موجود.pixel;

    أرجع INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_مجموعه_fg_color(guicolor_T color)
{
    إذا (color == prev_fg_color)
	أرجع;

    XSetForeground(gui.dpy, gui.متن_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_مجموعه_bg_color(guicolor_T color)
{
    إذا (color == prev_bg_color)
	أرجع;

    XSetBackground(gui.dpy, gui.متن_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_مجموعه_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    ساكن Cursor
gui_x11_create_blank_mouse(فراغ)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (بیعلامت دراز)0, (XGCValues*)0);

    إذا (gc != عدم)
    {
	XDraw_نقطه(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    أرجع XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ساكن فراغ
draw_curl(صحيح ردیف, صحيح ستون, صحيح cells)
{
    صحيح			i;
    صحيح			جابه_جایی;
    ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.متن_gc, prev_sp_color);
    لكل (i = FILL_X(ستون); i < FILL_X(ستون + cells); ++i)
    {
	جابه_جایی = val[i % 8];
	XDraw_نقطه(gui.dpy, gui.wid, gui.متن_gc, i,
						FILL_Y(ردیف + 1) - 1 - جابه_جایی);
    }
    XSetForeground(gui.dpy, gui.متن_gc, prev_fg_color);
}

    فراغ
gui_mch_draw_string(
    صحيح		ردیف,
    صحيح		ستون,
    char_u	*s,
    صحيح		len,
    صحيح		flags)
{
    صحيح			cells = len;
    ساكن فراغ		*buf = عدم;
    ساكن صحيح		buflen = 0;
    char_u		*p;
    صحيح			wlen = 0;
    صحيح			c;

    إذا (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	إذا (buflen < len)
	{
	    XtFree((حرف *)buf);
	    buf = (فراغ *)XtMalloc(len * (حجم(XChar2b) < حجم(wchar_t)
					? حجم(wchar_t) : حجم(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	طالما (p < s + len)
	{
	    c = utf_ptr2char(p);
#اگر_تعریف_شده FEAT_XFONTSET
	    إذا (current_fontset != عدم)
	    {
# ifdef SMALL_WCHAR_T
		إذا (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    وإلا
#پایان_اگر
	    {
		إذا (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (بیعلامت)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    وإلا إذا (has_mbyte)
    {
	cells = 0;
	لكل (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#اگر_تعریف_شده FEAT_XFONTSET
    إذا (current_fontset != عدم)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.ارتفاع = gui.char_ارتفاع;
	clip.عرض = gui.char_عرض * cells + 1;
	XSetClipRectangles(gui.dpy, gui.متن_gc, FILL_X(ستون), FILL_Y(ردیف),
		&clip, 1, Unsorted);
    }
#پایان_اگر

    إذا (flags & DRAW_TRANSP)
    {
	إذا (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), buf, wlen);
	وإلا
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), (حرف *)s, len);
    }
    وإلا إذا (p_linespace != 0
#اگر_تعریف_شده FEAT_XFONTSET
	    || current_fontset != عدم
#پایان_اگر
	    )
    {
	XSetForeground(gui.dpy, gui.متن_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		FILL_Y(ردیف), gui.char_عرض * cells, gui.char_ارتفاع);
	XSetForeground(gui.dpy, gui.متن_gc, prev_fg_color);

	إذا (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), buf, wlen);
	وإلا
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), (حرف *)s, len);
    }
    وإلا
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	إذا (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), buf, wlen);
	وإلا
	    XDrawImageString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون),
		    متن_Y(ردیف), (حرف *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    إذا (flags & DRAW_BOLD)
    {
	إذا (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون) + 1,
		    متن_Y(ردیف), buf, wlen);
	وإلا
	    XDrawString(gui.dpy, gui.wid, gui.متن_gc, متن_X(ستون) + 1,
		    متن_Y(ردیف), (حرف *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    إذا (flags & DRAW_UNDERC)
	draw_curl(ردیف, ستون, cells);

    // Underline: draw a line at the bottom of the character cell.
    إذا (flags & DRAW_UNDERL)
    {
	صحيح	y = FILL_Y(ردیف + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_سطر(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		y, FILL_X(ستون + cells) - 1, y);
    }

    إذا (flags & DRAW_STRIKE)
    {
	صحيح	y = FILL_Y(ردیف + 1) - gui.char_ارتفاع/2;

	XSetForeground(gui.dpy, gui.متن_gc, prev_sp_color);
	XDraw_سطر(gui.dpy, gui.wid, gui.متن_gc, FILL_X(ستون),
		y, FILL_X(ستون + cells) - 1, y);
	XSetForeground(gui.dpy, gui.متن_gc, prev_fg_color);
    }

#اگر_تعریف_شده FEAT_XFONTSET
    إذا (current_fontset != عدم)
	XSet_Clip_ماسک(gui.dpy, gui.متن_gc, None);
#پایان_اگر
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیدها[i].کلید_sym != (کلید_Sym)0; i++)
	إذا (name[0] == special_کلیدها[i].vim_code0 &&
					 name[1] == special_کلیدها[i].vim_code1)
	    أرجع OK;
    أرجع FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحيح
gui_get_x11_windis(پنجره *win, Display **dis)
{
    *win = Xt_پنجره(vimShell);
    *dis = gui.dpy;
    أرجع OK;
}

    فراغ
gui_mch_beep(فراغ)
{
    XBell(gui.dpy, 0);
}

    فراغ
gui_mch_flash(صحيح msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((صحيح)Columns) + gui.border_جابه_جایی,
	    FILL_Y((صحيح)ردیفها) + gui.border_جابه_جایی);
    XSync(gui.dpy, False);
    ui_delay((دراز)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((صحيح)Columns) + gui.border_جابه_جایی,
	    FILL_Y((صحيح)ردیفها) + gui.border_جابه_جایی);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    فراغ
gui_mch_invert_rectangle(
    صحيح	    r,
    صحيح	    c,
    صحيح	    nr,
    صحيح	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_عرض, (nr) * gui.char_ارتفاع);
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    XIconify_پنجره(gui.dpy, Xt_پنجره(vimShell), DefaultScreen(gui.dpy));
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_مجموعه_foreground(فراغ)
{
    XMapRaised(gui.dpy, Xt_پنجره(vimShell));
}
#پایان_اگر

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحيح		w = 1;

    إذا (mb_lefthalve(gui.ردیف, gui.ستون))
	w = 2;
    gui_mch_مجموعه_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.متن_gc, FILL_X(gui.ستون),
	    FILL_Y(gui.ردیف), w * gui.char_عرض - 1, gui.char_ارتفاع - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
{
    gui_mch_مجموعه_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.متن_gc,
#اگر_تعریف_شده FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_راست ? FILL_X(gui.ستون + 1) - w :
#پایان_اگر
		FILL_X(gui.ستون),
	    FILL_Y(gui.ردیف) + gui.char_ارتفاع - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    فراغ
gui_mch_update(فراغ)
{
    Xt_درون_داد_ماسک ماسک, desired;

#اگر_تعریف_شده ALT_X_درون_داد
    إذا (suppress_alternate_درون_داد)
	desired = (XtIMXEvent | XtIMTimer);
    وإلا
#پایان_اگر
	desired = (XtIMAll);
    طالما ((ماسک = XtAppPending(app_context)) && (ماسک & desired)
	    && !vim_is_درون_داد_buf_پر())
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
    صحيح
gui_mch_wait_for_chars(دراز wtime)
{
    صحيح	    focus;
    صحيح	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ساكن صحيح	    timed_بیرون;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_درون_داد_ماسک	    desired;
#اگر_تعریف_شده FEAT_کار_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#پایان_اگر

    timed_بیرون = FALSE;

    إذا (wtime >= 0)
	timer = Xt_App_جمع_کن_زمان_بیرون(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_بیرون);
#اگر_تعریف_شده FEAT_کار_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = Xt_App_جمع_کن_زمان_بیرون(app_context, (long_u)20,
				   channel_poll_cb, (Xt_اشاره_گر)&channel_timer);
#پایان_اگر

    focus = gui.درون_focus;
    desired = (XtIMAll);
    طالما (!timed_بیرون)
    {
	// Stop or start blinking when focus changes
	إذا (gui.درون_focus != focus)
	{
	    إذا (gui.درون_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.درون_focus;
	}

#اگر_تعریف_شده پیام_صف
# ifdef FEAT_TIMERS
	did_جمع_کن_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جمع_کن_timer)
	    // Need to recompute the waiting time.
	    اكسر;
# endif
#پایان_اگر

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	إذا (درون_داد_موجود())
	{
	    retval = OK;
	    اكسر;
	}
    }

    إذا (timer != (XtIntervalId)0 && !timed_بیرون)
	Xt_Remove_زمان_بیرون(timer);
#اگر_تعریف_شده FEAT_کار_CHANNEL
    إذا (channel_timer != (XtIntervalId)0)
	Xt_Remove_زمان_بیرون(channel_timer);
#پایان_اگر

    أرجع retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    فراغ
gui_mch_flush(فراغ)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_بلوک(
    صحيح		row1,
    صحيح		col1,
    صحيح		row2,
    صحيح		col2)
{
    صحيح		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.پشت_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_عرض + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ارتفاع);
}

    فراغ
gui_mch_clear_all(فراغ)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    إذا (gui.visibility == VisibilityFullyObscured)
	أرجع;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.متن_gc,
	FILL_X(gui.scroll_region_چپ), FILL_Y(ردیف + رقم_سطرها),
	gui.char_عرض * (gui.scroll_region_راست - gui.scroll_region_چپ + 1)
			       + (gui.scroll_region_راست == Columns - 1),
	gui.char_ارتفاع * (gui.scroll_region_bot - ردیف - رقم_سطرها + 1),
	FILL_X(gui.scroll_region_چپ), FILL_Y(ردیف));

    gui_clear_بلوک(gui.scroll_region_bot - رقم_سطرها + 1,
						       gui.scroll_region_چپ,
			  gui.scroll_region_bot, gui.scroll_region_راست);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_درج_سطرها(صحيح ردیف, صحيح رقم_سطرها)
{
    إذا (gui.visibility == VisibilityFullyObscured)
	أرجع;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.متن_gc,
	FILL_X(gui.scroll_region_چپ), FILL_Y(ردیف),
	gui.char_عرض * (gui.scroll_region_راست - gui.scroll_region_چپ + 1)
			       + (gui.scroll_region_راست == Columns - 1),
	gui.char_ارتفاع * (gui.scroll_region_bot - ردیف - رقم_سطرها + 1),
	FILL_X(gui.scroll_region_چپ), FILL_Y(ردیف + رقم_سطرها));

    gui_clear_بلوک(ردیف, gui.scroll_region_چپ,
				ردیف + رقم_سطرها - 1, gui.scroll_region_راست);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    ساكن فراغ
gui_x11_check_copy_area(فراغ)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    إذا (gui.visibility != VisibilityPartiallyObscured)
	أرجع;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    لكل (;;)
    {
	إذا (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    أرجع;	// The scroll worked.

	إذا (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->عرض, gevent->ارتفاع);
	    إذا (gevent->شمار == 0)
		أرجع;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    فراغ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    أرجع clip_x11_own_selection(vimShell, cbd);
}

    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    فراغ
clip_mch_مجموعه_selection(
    Clipboard_T	*cbd)
{
    clip_x11_مجموعه_selection(cbd);
}

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    فراغ
gui_mch_menu_grey(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->id == (Widget)0)
	أرجع;

    gui_mch_menu_hidden(menu, False);
    إذا (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	Xt_مجموعه_Sensitive(menu->id, False);
    وإلا
	Xt_مجموعه_Sensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    فراغ
gui_mch_menu_hidden(vimmenu_T *menu, صحيح hidden)
{
    إذا (menu->id == (Widget)0)
	أرجع;

    إذا (hidden)
	Xt_Unmanage_فرزند(menu->id);
    وإلا
	Xt_Manage_فرزند(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    فراغ
gui_mch_draw_menubar(فراغ)
{
    // Nothing to do in X
}

    فراغ
gui_x11_menu_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    gui_menu_cb((vimmenu_T *)client_داده_2);
}

#پایان_اگر // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    ساكن فراغ
gui_x11_wm_protocol_handler(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2 استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    /*
     * Only deal with Client messages.
     */
    إذا (event->type != Client_پیام)
	أرجع;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    إذا ((Atom)((XClient_پیام_Event *)event)->داده.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	بیرون_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, Xt_پنجره(vimShell), عدم, 0);
	أرجع;
    }

    إذا ((Atom)((XClient_پیام_Event *)event)->داده.l[0] !=
						  wm_atoms[DELETE_پنجره_IDX])
	أرجع;

    gui_shell_closed();
}

#اگر_تعریف_شده FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ساكن فراغ
gui_x11_send_event_handler(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2 استفادهنشده,
    XEvent	*event,
    Boolean	*dum استفادهنشده)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    إذا (e->type == PropertyNotify && e->پنجره_2 == comm_پنجره
	    && e->atom == commProperty &&  e->state == Property_New_مقدار)
	serverEventProc(gui.dpy, event, 0);
}
#پایان_اگر

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کن BLINK_NONE  0
#تعریف_کن BLINK_OFF   1
#تعریف_کن BLINK_ON    2

ساكن صحيح		blink_state = BLINK_NONE;
ساكن long_u		blink_waittime = 700;
ساكن long_u		blink_ontime = 400;
ساكن long_u		blink_offtime = 250;
ساكن XtIntervalId	blink_timer = (XtIntervalId)0;

    صحيح
gui_mch_is_blinking(فراغ)
{
    أرجع blink_state != BLINK_NONE;
}

    صحيح
gui_mch_is_blink_off(فراغ)
{
    أرجع blink_state == BLINK_OFF;
}

    فراغ
gui_mch_مجموعه_blinking(دراز waittime, دراز on, دراز off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    إذا (blink_timer != (XtIntervalId)0)
    {
	Xt_Remove_زمان_بیرون(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    إذا (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ساكن فراغ
gui_x11_blink_cb(
    Xt_اشاره_گر	    timed_بیرون استفادهنشده,
    XtIntervalId    *interval_id استفادهنشده)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = Xt_App_جمع_کن_زمان_بیرون(app_context, blink_offtime,
						      gui_x11_blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = Xt_App_جمع_کن_زمان_بیرون(app_context, blink_ontime,
						      gui_x11_blink_cb, عدم);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_شروع_blink(فراغ)
{
    إذا (blink_timer != (XtIntervalId)0)
	Xt_Remove_زمان_بیرون(blink_timer);
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.درون_focus)
    {
	blink_timer = Xt_App_جمع_کن_زمان_بیرون(app_context, blink_waittime,
						      gui_x11_blink_cb, عدم);
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

    أرجع (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((بیعلامت)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    فراغ
gui_x11_callbacks(Widget متن_Area, Widget vimForm)
{
    Xt_جمع_کن_Event_Handler(متن_Area, Visibility_Change_ماسک, FALSE,
	gui_x11_visibility_cb, (Xt_اشاره_گر)0);

    Xt_جمع_کن_Event_Handler(متن_Area, Exposure_ماسک, FALSE, gui_x11_expose_cb,
	(Xt_اشاره_گر)0);

    Xt_جمع_کن_Event_Handler(vimShell, Structure_Notify_ماسک, FALSE,
	gui_x11_resize_پنجره_cb, (Xt_اشاره_گر)0);

    Xt_جمع_کن_Event_Handler(vimShell, Focus_Change_ماسک, FALSE, gui_x11_focus_change_cb,
	(Xt_اشاره_گر)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    إذا (vim_strchr(p_go, GO_اشاره_گر) != عدم)
    {
	Xt_جمع_کن_Event_Handler(vimShell, Leave_پنجره_ماسک, FALSE, gui_x11_leave_cb,
	    (Xt_اشاره_گر)0);
	Xt_جمع_کن_Event_Handler(متن_Area, Leave_پنجره_ماسک, FALSE, gui_x11_leave_cb,
	    (Xt_اشاره_گر)0);
	Xt_جمع_کن_Event_Handler(متن_Area, Enter_پنجره_ماسک, FALSE, gui_x11_enter_cb,
	    (Xt_اشاره_گر)0);
	Xt_جمع_کن_Event_Handler(vimShell, Enter_پنجره_ماسک, FALSE, gui_x11_enter_cb,
	    (Xt_اشاره_گر)0);
    }

    Xt_جمع_کن_Event_Handler(vimForm, کلید_Press_ماسک, FALSE, gui_x11_کلید_اصابت_cb,
	(Xt_اشاره_گر)0);
    Xt_جمع_کن_Event_Handler(متن_Area, کلید_Press_ماسک, FALSE, gui_x11_کلید_اصابت_cb,
	(Xt_اشاره_گر)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    Xt_جمع_کن_Event_Handler(vimForm, اشاره_گر_Motion_ماسک,
	FALSE, gui_x11_mouse_cb, (Xt_اشاره_گر)1);
    Xt_جمع_کن_Event_Handler(متن_Area, Button_Press_ماسک | Button_Release_ماسک |
					 Button_Motion_ماسک | اشاره_گر_Motion_ماسک,
	FALSE, gui_x11_mouse_cb, (Xt_اشاره_گر)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    صحيح		rootx, rooty, winx, winy;
    پنجره	ریشه, فرزند;
    بیعلامت صحيح ماسک;

    إذا (gui.wid && XQuery_اشاره_گر(gui.dpy, gui.wid, &ریشه, &فرزند,
					 &rootx, &rooty, &winx, &winy, &ماسک))
    {
	*x = winx;
	*y = winy;
    }
    وإلا
    {
	*x = -1;
	*y = -1;
    }
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    إذا (gui.wid)
	XWarp_اشاره_گر(gui.dpy, (پنجره)0, gui.wid, 0, 0, 0, 0, x, y);
}

#إذا defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_آخرین_mouse_event(فراغ)
{
    أرجع &آخرین_mouse_event;
}
#پایان_اگر

#إذا defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_عرض (gui.char_عرض * 2)

    فراغ
gui_mch_drawsign(صحيح ردیف, صحيح ستون, صحيح typenr)
{
    XImage	*sign;

    إذا (!gui.درون_use || (sign = (XImage *)sign_get_image(typenr)) == عدم)
	أرجع;

    XClearArea(gui.dpy, gui.wid, متن_X(ستون), متن_Y(ردیف) - sign->ارتفاع,
	    SIGN_عرض, gui.char_ارتفاع, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.متن_gc, sign, 0, 0,
	    متن_X(ستون) + (SIGN_عرض - sign->عرض) / 2,
	    متن_Y(ردیف) - sign->ارتفاع,
	    sign->عرض, sign->ارتفاع);
}

    فراغ *
gui_mch_ثبات_sign(char_u *signfile)
{
    Xpm_صفات   attrs;
    XImage	    *sign = عدم;
    صحيح		    وضعیت;

    /*
     * Setup the color substitution table.
     */
    إذا (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_نماد color[5] =
	{
	    {"none", عدم, 0},
	    {"iconColor1", عدم, 0},
	    {"bottomShadowColor", عدم, 0},
	    {"topShadowColor", عدم, 0},
	    {"selectColor", عدم, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.پشت_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	وضعیت = Xpm_Read_File_مقصد_Image(gui.dpy, (حرف *)signfile,
							 &sign, عدم, &attrs);
	إذا (وضعیت == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	وإلا
	    emsg(_(e_couldnt_read_درون_sign_داده));
    }

    أرجع (فراغ *)sign;
}

    فراغ
gui_mch_destroy_sign(فراغ *sign)
{
    XDestroyImage((XImage*)sign);
}
#پایان_اگر


#اگر_تعریف_شده FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساكن صحيح آخرین_shape = 0;
#پایان_اگر

/*
 * Use the blank mouse pointer or not.
 */
    فراغ
gui_mch_mousehide(
    صحيح		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    إذا (gui.اشاره_گر_hidden == hide)
	أرجع;

    gui.اشاره_گر_hidden = hide;
    إذا (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_اشاره_گر);
    وإلا
#اگر_تعریف_شده FEAT_MOUSESHAPE
	mch_مجموعه_mouse_shape(آخرین_shape);
#وإلا
    XUndefineCursor(gui.dpy, gui.wid);
#پایان_اگر
}

#إذا defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساكن صحيح mshape_ids[] =
{
    XC_چپ_ptr,		// arrow
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
    XC_راست_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_چپ_ptr			// last one
};

    فراغ
mch_مجموعه_mouse_shape(صحيح shape)
{
    صحيح	    id;

    إذا (!gui.درون_use)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشاره_گر_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_اشاره_گر);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    إذا (id >= XC_رقم_glyphs)
		id = XC_چپ_ptr;
	    وإلا
		id &= ~1;	// they are always even (why?)
	}
	وإلا
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    إذا (shape != MSHAPE_HIDE)
	آخرین_shape = shape;
}
#پایان_اگر

#إذا defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    فراغ
gui_mch_menu_مجموعه_tip(vimmenu_T *menu)
{
    إذا (menu->id == عدم || menu->والد == عدم
				|| !menu_is_toolbar(menu->والد->name))
	أرجع;

    // Always destroy and create the balloon, in case the string was
    // changed.
    إذا (menu->tip != عدم)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = عدم;
    }
    إذا (menu->رشتهها[MENU_شاخص_TIP] != عدم)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->رشتهها[MENU_شاخص_TIP],
		عدم,
		عدم);
}
#پایان_اگر
