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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <X11/keysym.h>
#ଅନ୍ତର୍ଭାଵ <X11/Xatom.h>
#ଅନ୍ତର୍ଭାଵ <X11/StringDefs.h>
#ଅନ୍ତର୍ଭାଵ <X11/Intrinsic.h>
#ଅନ୍ତର୍ଭାଵ <X11/Shell.h>
#ଅନ୍ତର୍ଭାଵ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#ଯଦି defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#ଅନ୍ୟଥା
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ଅନ୍ୟଥା
#   include <X11/xpm.h>
#  endif
# endif
#endif

#ifdef FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# ଅନ୍ୟଥା
#  include <locale.h>
# endif
#endif

#ifdef HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#endif

#ifdef HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#endif

#define VIM_NAME	"vim"
#define VIM_CLASS	"Vim"

// Default resource values
#define DFLT_FONT		"7x13"
#ifdef FONTSET_ALWAYS
# define DFLT_MENU_FONT		XtDefaultFontSet
#ଅନ୍ୟଥା
# define DFLT_MENU_FONT		XtDefaultFont
#endif
#define DFLT_TOOLTIP_FONT	XtDefaultFontSet

// use the default (CDE) colors
#define DFLT_MENU_BG_COLOR	""
#define DFLT_MENU_FG_COLOR	""
#define DFLT_SCROLL_BG_COLOR	""
#define DFLT_SCROLL_FG_COLOR	""
#define DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#define DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

ସ୍ଥିର Atom   wm_atoms[2];	// Window Manager Atoms
#define DELETE_WINDOW_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#define SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ifdef FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ସ୍ଥିର XFontSet current_fontset = ଖାଲି;
# ଯଦି !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	କର \
	{ \
	    ଯଦି (current_fontset != ଖାଲି) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    ଅନ୍ୟଥା \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} ଯେତେବେଳେ (0)
# endif
# ଯଦି !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	କର \
	{ \
	    ଯଦି (current_fontset != ଖାଲି) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ଅନ୍ୟଥା \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ଯେତେବେଳେ (0)
# endif
# ଯଦି !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	କର \
	{ \
	    ଯଦି (current_fontset != ଖାଲି) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ଅନ୍ୟଥା \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ଯେତେବେଳେ (0)
# endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_fontset_sanity(XFontSet fs);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fontset_width(XFontSet fs);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fontset_ascent(XFontSet fs);
#endif

ସ୍ଥିର guicolor_T	prev_fg_color = INVALCOLOR;
ସ୍ଥିର guicolor_T	prev_bg_color = INVALCOLOR;
ସ୍ଥିର guicolor_T	prev_sp_color = INVALCOLOR;

#ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ସ୍ଥିର XButtonPressedEvent last_mouse_event;
#endif

ସ୍ଥିର ଶୂନ୍ୟ gui_x11_check_copy_area(ଶୂନ୍ୟ);
#ifdef FEAT_CLIENTSERVER
ସ୍ଥିର ଶୂନ୍ୟ gui_x11_send_event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif
ସ୍ଥିର ଶୂନ୍ୟ gui_x11_wm_protocol_handler(Widget, XtPointer, XEvent *, Boolean *);
ସ୍ଥିର Cursor gui_x11_create_blank_mouse(ଶୂନ୍ୟ);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ସ୍ଥିର ସଂରଚନା specialkey
{
    ଚାବି_Sym  ଚାବି_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ଚାବିଗୁଡ଼ିକ[] =
{
    {XK_Up,		'k', 'u'},
    {XK_Down,		'k', 'd'},
    {XK_ବାମ,		'k', 'l'},
    {XK_ଡାହାଣ,		'k', 'r'},

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
#ifdef SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#endif

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_BackSpace,	'k', 'b'},
    {XK_ଭର୍ତ୍ତି,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_End,		'@', '7'},
    {XK_Prior,		'k', 'P'},
    {XK_Next,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ifdef XK_KP_ବାମ
    {XK_KP_ବାମ,	'k', 'l'},
    {XK_KP_ଡାହାଣ,	'k', 'r'},
    {XK_KP_Up,		'k', 'u'},
    {XK_KP_Down,	'k', 'd'},
    {XK_KP_ଭର୍ତ୍ତି,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_End,		'K', '4'},
    {XK_KP_Prior,	'K', '3'},
    {XK_KP_Next,	'K', '5'},

    {XK_KP_Add,		'K', '6'},
    {XK_KP_Subtract,	'K', '7'},
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
#endif

    // End of list marker:
    {(ଚାବି_Sym)0,	    0, 0}
};

#define XtNboldFont		"boldFont"
#define XtCBoldFont		"BoldFont"
#define XtNitalicFont		"italicFont"
#define XtCItalicFont		"ItalicFont"
#define XtNboldItalicFont	"boldItalicFont"
#define XtCBoldItalicFont	"BoldItalicFont"
#define XtNscrollbarWidth	"scrollbarWidth"
#define XtCScrollbarWidth	"ScrollbarWidth"
#define XtNmenuHeight		"menuHeight"
#define XtCMenuHeight		"MenuHeight"
#define XtNmenuFont		"menuFont"
#define XtCMenuFont		"MenuFont"
#define XtNmenuFontSet		"menuFontSet"
#define XtCMenuFontSet		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#define XtNmenuBackground	"menuBackground"
#define XtCMenuBackground	"MenuBackground"
#define XtNmenuForeground	"menuForeground"
#define XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#define XtNscrollBackground	"scrollBackground"
#define XtCScrollBackground	"ScrollBackground"
#define XtNscrollForeground	"scrollForeground"
#define XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#define XtNtooltipBackground	"tooltipBackground"
#define XtCTooltipBackground	"TooltipBackground"
#define XtNtooltipForeground	"tooltipForeground"
#define XtCTooltipForeground	"TooltipForeground"
#define XtNtooltipFont		"tooltipFont"
#define XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
ସ୍ଥିର XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	ଆକାର(Pixel),
	XtOffsetOf(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	ଆକାର(Pixel),
	XtOffsetOf(gui_T, def_back_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	ଆକାର(String *),
	XtOffsetOf(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	ଆକାର(String *),
	XtOffsetOf(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	ଆକାର(String *),
	XtOffsetOf(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	ଆକାର(String *),
	XtOffsetOf(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	ଆକାର(String *),
	XtOffsetOf(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	ଆକାର(Bool),
	XtOffsetOf(gui_T, rsrc_rev_video),
	XtRImmediate,
	(XtPointer)False
    },
    {
	XtNborderWidth,
	XtCBorderWidth,
	XtRInt,
	ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
	XtOffsetOf(gui_T, border_width),
	XtRImmediate,
	(XtPointer)2
    },
    {
	XtNscrollbarWidth,
	XtCScrollbarWidth,
	XtRInt,
	ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
	XtOffsetOf(gui_T, scrollbar_width),
	XtRImmediate,
	(XtPointer)SB_DEFAULT_WIDTH
    },
#ifdef FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	XtNmenuFontSet,
	XtCMenuFontSet,
# ଅନ୍ୟଥା
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#endif
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ifdef FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	ଆକାର(ଅକ୍ଷର *),
	XtOffsetOf(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	XtCFontSet,
	XtRFontSet,
	ଆକାର(XFontSet),
	XtOffsetOf(gui_T, tooltip_fontset),
	XtRImmediate,
	(XtPointer)NOFONTSET
    },
#endif // FEAT_BEVAL_GUI
#ifdef FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	ଆକାର(ଅକ୍ଷର*),
	XtOffsetOf(gui_T, rsrc_preedit_type_name),
	XtRString,
	(XtPointer)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	ଆକାର(ଅକ୍ଷର*),
	XtOffsetOf(gui_T, rsrc_ନିବେଶ_method),
	XtRString,
	ଖାଲି
    },
#endif // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
ସ୍ଥିର XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ଖାଲି},
    {"-background",	".background",	    XrmoptionSepArg,	ଖାଲି},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ଖାଲି},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ଖାଲି},
    {"-fn",		".font",	    XrmoptionSepArg,	ଖାଲି},
    {"-font",		".font",	    XrmoptionSepArg,	ଖାଲି},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ଖାଲି},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ଖାଲି},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ଖାଲି},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ଖାଲି},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ଖାଲି},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ଖାଲି},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ଖାଲି},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ଖାଲି},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ଖାଲି},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ଖାଲି},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ଖାଲି},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ଖାଲି},
#ifdef FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ଖାଲି},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ଖାଲି},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ଖାଲି},
#ଅନ୍ୟଥା
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ଖାଲି},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ଖାଲି},
#endif
    {"-xrm",		ଖାଲି,		    XrmoptionResArg,	ଖାଲି}
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gui_argc = 0;
ସ୍ଥିର ଅକ୍ଷର **gui_argv = ଖାଲି;

/*
 * Call-back routines.
 */

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_timer_cb(
    XtPointer	    timed_out,
    XtIntervalId    *interval_id UNUSED)
{
    *((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)timed_out) = TRUE;
}

#ifdef FEAT_JOB_CHANNEL
    ସ୍ଥିର ଶୂନ୍ୟ
channel_poll_cb(
    XtPointer	    client_data,
    XtIntervalId    *interval_id UNUSED)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_data;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
						 channel_poll_cb, client_data);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_visibility_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ଯଦି (event->type != VisibilityNotify)
	ଫେରନ୍ତୁ;

    gui.visibility = event->xvisibility.state;

    /*
     * When we do an XCopyArea(), and the window is partially obscured, we want
     * to receive an event to tell us whether it worked or not.
     */
    XSetGraphicsExposures(gui.dpy, gui.text_gc,
	    gui.visibility != VisibilityUnobscured);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_expose_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XExposeEvent	*gevent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			new_x;

    ଯଦି (event->type != Expose)
	ଫେରନ୍ତୁ;

    out_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    ଯଦି (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    ଯଦି (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    ଯଦି (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns), 0, 0, 0, False);
    ଯଦି (gevent->y > FILL_Y(Rows))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#ଯଦି defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    ସ୍ଥିର ଶୂନ୍ୟ
shellRectangle(Widget shell, XRectangle *r)
{
    Window		rootw, shellw, child, parentw;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			absx, absy;
    XWindowAttributes	a;
    Window		*children;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	children_ଗଣନା;

    shellw = XtWindow(shell);
    ଯଦି (shellw == 0)
	ଫେରନ୍ତୁ;
    ପାଇଁ (;;)
    {
	XQuery_ବୃକ୍ଷ(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_ଗଣନା);
	XFree(children);
	ଯଦି (parentw == rootw)
	    ଭାଙ୍ଗ;
	shellw = parentw;
    }
    XGetWindowAttributes(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.ମୂଳ, 0, 0,
							&absx, &absy, &child);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNheight, &r->height, XmNwidth, &r->width, ଖାଲି);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_resize_window_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lastWidth, lastHeight;

    ଯଦି (event->type != ConfigureNotify)
	ଫେରନ୍ତୁ;

    ଯଦି (event->xconfigure.width != lastWidth
	    || event->xconfigure.height != lastHeight)
    {
	lastWidth = event->xconfigure.width;
	lastHeight = event->xconfigure.height;
	gui_resize_shell(event->xconfigure.width, event->xconfigure.height
#ifdef FEAT_XIM
						- xim_get_status_area_height()
#endif
		     );
    }
#ଯଦି defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    ଯଦି (netbeans_active())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_frame_moved(rec.x, rec.y);
    }
#endif
#ifdef FEAT_XIM
    xim_set_preedit();
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_focus_change_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    gui_focus_change(event->type == FocusIn);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_enter_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(TRUE);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_leave_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(FALSE);
}

#ଯଦି defined(X_HAVE_UTF8_STRING)
# ଯଦି X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#endif

    ଶୂନ୍ୟ
gui_x11_ଚାବି_hit_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XKeyPressedEvent	*ev_press;
#ifdef FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    Status		status;
#ଅନ୍ୟଥା
    char_u		string[4], string2[3];
#endif
    ଚାବି_Sym		ଚାବି_sym;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ଚାବି;

    ev_press = (XKeyPressedEvent *)event;

#ifdef FEAT_XIM
    ଯଦି (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	ଯଦି (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (ଅକ୍ଷର *)string,
				  ଆକାର(string_shortbuf), &ଚାବି_sym, &status);
	ଅନ୍ୟଥା
# endif
	    len = XmbLookupString(xic, ev_press, (ଅକ୍ଷର *)string,
				  ଆକାର(string_shortbuf), &ଚାବି_sym, &status);
	ଯଦି (status == XBufferOverflow)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    ଯଦି (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (ଅକ୍ଷର *)string,
						      len, &ଚାବି_sym, &status);
	    ଅନ୍ୟଥା
# endif
		len = XmbLookupString(xic, ev_press, (ଅକ୍ଷର *)string,
						      len, &ଚାବି_sym, &status);
	}
	ଯଦି (status == XLookupNone || status == XLookupChars)
	    ଚାବି_sym = XK_Void_ପ୍ରତୀକ;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	ଯଦି (len > 0 && ନିବେଶ_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    ଯଦି (string_alloced)
		XtFree((ଅକ୍ଷର *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_ନିବେଶ(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	ପାଇଁ (i = 0; i < len; ++i)
	    ଯଦି (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		ଯଦି (string_alloced)
		    XtFree((ଅକ୍ଷର *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    ଅନ୍ୟଥା
#endif
	len = XLookupString(ev_press, (ଅକ୍ଷର *)string, ଆକାର(string),
		&ଚାବି_sym, ଖାଲି);

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    ଯଦି (ଚାବି_sym == SunXK_F36 || ଚାବି_sym == SunXK_F37)
	len = 0;
#endif

    ଯଦି (ଚାବି_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    ଯଦି (ଚାବି_sym == XK_minus && (ev_press->state & ControlMask))
	string[0] = Ctrl__;

#ifdef XK_ISO_ବାମ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    ଯଦି (ଚାବି_sym == XK_ISO_ବାମ_Tab)
    {
	ଚାବି_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    ଯଦି (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ଯଦି (len == 0 || len == 1)
    {
	ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != (ଚାବି_Sym)0; i++)
	{
	    ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == ଚାବି_sym)
	    {
		string[0] = CSI;
		string[1] = special_ଚାବିଗୁଡ଼ିକ[i].vim_code0;
		string[2] = special_ଚାବିଗୁଡ଼ିକ[i].vim_code1;
		len = -3;
		ଭାଙ୍ଗ;
	    }
	}
    }

    // Unrecognised key is ignored.
    ଯଦି (len == 0)
	ଯାଅ theend;

    // Handle modifiers.
    modifiers = 0;
    ଯଦି (ev_press->state & ShiftMask)
	modifiers |= MOD_MASK_SHIFT;
    ଯଦି (ev_press->state & ControlMask)
    {
	modifiers |= MOD_MASK_CTRL;
	ଯଦି (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    ଯଦି (ev_press->state & Mod1Mask)
	modifiers |= MOD_MASK_ALT;
    ଯଦି (ev_press->state & Mod4Mask)
	modifiers |= MOD_MASK_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    ଯଦି (len == -3)
	ଚାବି = TO_SPECIAL(string[1], string[2]);
    ଅନ୍ୟଥା
    {
	string[len] = NUL;
	ଚାବି = mb_ptr2char(string);
    }
    ଚାବି = simplify_ଚାବି(ଚାବି, &modifiers);
    ଯଦି (ଚାବି == CSI)
	ଚାବି = K_CSI;
    ଯଦି (IS_SPECIAL(ଚାବି))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ଚାବି);
	string[2] = K_THIRD(ଚାବି);
	len = 3;
    }
    ଅନ୍ୟଥା
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ଚାବି = may_adjust_ଚାବି_for_ctrl(modifiers, ଚାବି);

	len = mb_char2bytes(ଚାବି, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_shift_modifier(modifiers, ଚାବି);
    }

    ଯଦି (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_ନିବେଶ_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା int_ch = check_for_interrupt(ଚାବି, modifiers);

	ଯଦି (int_ch != NUL)
	{
	    trash_ନିବେଶ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_ନିବେଶ_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    ଯଦି (p_mh)
	gui_mch_mousehide(TRUE);

#ଯଦି defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	ଯଦି ((be = gui_mch_currently_showing_beval()) != ଖାଲି)
	    gui_mch_unpost_balloon(be);
    }
#endif
theend:
    {}	    // some compilers need a statement here
#ifdef FEAT_XIM
    ଯଦି (string_alloced)
	XtFree((ଅକ୍ଷର *)string);
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_mouse_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ସ୍ଥିର XtIntervalId timer = (XtIntervalId)0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	timed_out = TRUE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		repeated_click = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    ଯଦି (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1Mask | Button2Mask | Button3Mask))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	ଯଦି (button != MOUSE_DRAG)	// just moving the rodent
	{
#ifdef FEAT_MENU
	    ଯଦି (dud)			// moved in vimForm
		y -= gui.menu_height;
#endif
	    gui_mouse_moved(x, y);
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	ଯଦି (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    ଯଦି (!timed_out)
	    {
		XtRemoveTimeOut(timer);
		repeated_click = TRUE;
	    }
	    timed_out = FALSE;
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_out);
	    ବାଛ (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		ସ୍ଥିତି Button1:	button = MOUSE_ବାମ;	ଭାଙ୍ଗ;
		ସ୍ଥିତି Button2:	button = MOUSE_ମଧ୍ୟ;	ଭାଙ୍ଗ;
		ସ୍ଥିତି Button3:	button = MOUSE_ଡାହାଣ;	ଭାଙ୍ଗ;
		ସ୍ଥିତି Button4:	button = MOUSE_4;	ଭାଙ୍ଗ;
		ସ୍ଥିତି Button5:	button = MOUSE_5;	ଭାଙ୍ଗ;
		ସ୍ଥିତି 6:		button = MOUSE_7;	ଭାଙ୍ଗ;
		ସ୍ଥିତି 7:		button = MOUSE_6;	ଭାଙ୍ଗ;
		ସ୍ଥିତି 8:		button = MOUSE_X1;	ଭାଙ୍ଗ;
		ସ୍ଥିତି 9:		button = MOUSE_X2;	ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଫେରନ୍ତୁ;	// Unknown button
	    }
	}
	ଅନ୍ୟଥା ଯଦି (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	last_mouse_event = event->xbutton;
#endif
    }

    vim_modifiers = 0x0;
    ଯଦି (x_modifiers & ShiftMask)
	vim_modifiers |= MOUSE_SHIFT;
    ଯଦି (x_modifiers & ControlMask)
	vim_modifiers |= MOUSE_CTRL;
    ଯଦି (x_modifiers & Mod1Mask)	    // Alt or Meta key
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
    ଶୂନ୍ୟ
gui_mch_prepare(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argc, ଅକ୍ଷର **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(ଅକ୍ଷର *, *argc);
    ଯଦି (gui_argv == ଖାଲି)
	ଫେରନ୍ତୁ;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    ଯେତେବେଳେ (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Xt_ସଂଖ୍ୟା(cmdline_options); i++)
	    ଯଦି (strcmp(argv[arg], cmdline_options[i].option) == 0)
		ଭାଙ୍ଗ;

	ଯଦି (i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Xt_ସଂଖ୍ୟା(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    ଯଦି (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		found_reverse_arg = TRUE;
	    ଅନ୍ୟଥା ଯଦି ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_argument = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    ଯଦି (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * ଆକାର(ଅକ୍ଷର *));
		ଯଦି (cmdline_options[i].argKind != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    ଯଦି (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * ଆକାର(ଅକ୍ଷର *));
		}
	    }
	    argv[*argc] = ଖାଲି;
	}
	ଅନ୍ୟଥା
#ifdef FEAT_NETBEANS_INTG
	    ଯଦି (strncmp("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ଆକାର(ଅକ୍ଷର *));
	    argv[*argc] = ଖାଲି;
	}
	ଅନ୍ୟଥା
#endif
	    arg++;
    }
}

#ifndef XtSpecificationRelease
# define CARDINAL (Cardinal *)
#ଅନ୍ୟଥା
# ଯଦି XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# ଅନ୍ୟଥା
#  define CARDINAL (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)
# endif
#endif

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_check(ଶୂନ୍ୟ)
{
#ifdef FEAT_XIM
    XtSetLanguageProc(ଖାଲି, ଖାଲି, ଖାଲି);
#endif
    open_app_context();
    ଯଦି (app_context != ଖାଲି)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_ସଂଖ୍ୟା(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#ଯଦି defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	ଅକ୍ଷର *p = setlocale(LC_NUMERIC, ଖାଲି);

	ଯଦି (p == ଖାଲି || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#endif
    ଯଦି (app_context == ଖାଲି || gui.dpy == ଖାଲି)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}


#ifdef USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ସ୍ଥିର Xt_ନିବେଶ_Id _xsmp_xtinputid;

    ସ୍ଥିର ଶୂନ୍ୟ
local_xsmp_handle_requests(
    XtPointer	c UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*s UNUSED,
    Xt_ନିବେଶ_Id	*i UNUSED)
{
    ଯଦି (xsmp_handle_requests() == FAIL)
	Xt_Remove_ନିବେଶ(_xsmp_xtinputid);
}
#endif


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init(ଶୂନ୍ୟ)
{
    XtGCMask	gc_mask;
    XGCValues	gc_vals;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y, mask;
    unsigned	w, h;

#ଯଦି 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#endif

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ଖାଲି);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (XtPointer)&gui,
	vim_resources, Xt_ସଂଖ୍ୟା(vim_resources), ଖାଲି);

    gui.scrollbar_height = gui.scrollbar_width;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ifdef FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#endif

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    ଯଦି (gui.rsrc_rev_video && gui_get_lightness(gui.back_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_back_pixel;
	gui.back_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_back_pixel = gui.back_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    set_normal_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    gui.text_gc = XtGetGC(vimShell, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.back_gc = XtGetGC(vimShell, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_mask, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_setup_atoms(gui.dpy);

    ଯଦି (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    ଯଦି (gui.geom != ଖାଲି && *gui.geom != NUL)
    {
	mask = XParseGeometry((ଅକ୍ଷର *)gui.geom, &x, &y, &w, &h);
	ଯଦି (mask & Width_ମୂଲ୍ୟ)
	    Columns = w;
	ଯଦି (mask & Height_ମୂଲ୍ୟ)
	{
	    ଯଦି (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	ଯଦି (mask & (XValue|YValue))
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XtNgeometry, gui.geom, ଖାଲି);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    ଯଦି (vim_strchr(p_go, GO_ICON) != ଖାଲି)
    {
#ifndef HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	XtSetArg(arg[0], XtNiconPixmap,
		XCreateBitmapFromData(gui.dpy,
		    Default_ମୂଳ_Window(gui.dpy),
		    (ଅକ୍ଷର *)vim_icon_bits,
		    vim_icon_width,
		    vim_icon_height));
	XtSetArg(arg[1], XtNiconMask,
		XCreateBitmapFromData(gui.dpy,
		    Default_ମୂଳ_Window(gui.dpy),
		    (ଅକ୍ଷର *)vim_mask_icon_bits,
		    vim_mask_icon_width,
		    vim_mask_icon_height));
	Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, arg, (Cardinal)2);
#ଅନ୍ୟଥା
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ସ୍ଥିର Pixmap	icon = 0;
    ସ୍ଥିର Pixmap	icon_mask = 0;
    ସ୍ଥିର ଅକ୍ଷର		**magick = vim32x32;
    Window		ମୂଳ_window;
    XIconSize		*size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ସଂଖ୍ୟା_sizes;
    Display		*dsp;
    Screen		*scr;
    XpmAttributes	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    ମୂଳ_window = XRootWindowOfScreen(XtScreen(vimShell));
    ଯଦି (XGetIconSizes(XtDisplay(vimShell), ମୂଳ_window,
						   &size, &ସଂଖ୍ୟା_sizes) != 0)
    {
	ଯଦି (ସଂଖ୍ୟା_sizes > 0)
	{
	    ଯଦି (size->max_height >= 48 && size->max_width >= 48)
		magick = vim48x48;
	    ଅନ୍ୟଥା ଯଦି (size->max_height >= 32 && size->max_width >= 32)
		magick = vim32x32;
	    ଅନ୍ୟଥା ଯଦି (size->max_height >= 16 && size->max_width >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XtNcolormap, cmap, ଖାଲି);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.depth = DefaultDepthOfScreen(scr);

    ଯଦି (!icon)
    {
	XpmCreatePixmapFromData(dsp, ମୂଳ_window, magick, &icon,
							   &icon_mask, &attr);
	XpmFreeAttributes(&attr);
    }

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XmNiconPixmap, icon, XmNiconMask, icon_mask, ଖାଲି);
#endif
    }

#ifdef FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#endif
#ifdef FEAT_MENU
    gui_init_menu_font();
#endif

#ifdef USE_XSMP
    // Attach listener on ICE connection
    ଯଦି (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_Add_ନିବେଶ(app_context, xsmp_icefd,
		(XtPointer)Xt_ନିବେଶ_Read_Mask, local_xsmp_handle_requests, ଖାଲି);
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    ଶୂନ୍ୟ
gui_mch_uninit(ଶୂନ୍ୟ)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ଖାଲି;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    ଶୂନ୍ୟ
gui_mch_new_colors(ଶୂନ୍ୟ)
{
    long_u	gc_mask;
    XGCValues	gc_vals;

    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    ଯଦି (gui.text_gc != ଖାଲି)
	XChangeGC(gui.dpy, gui.text_gc, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    ଯଦି (gui.back_gc != ଖାଲି)
	XChangeGC(gui.dpy, gui.back_gc, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    ଯଦି (gui.invert_gc != ଖାଲି)
	XChangeGC(gui.dpy, gui.invert_gc, gc_mask, &gc_vals);

    gui_x11_set_back_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_open(ଶୂନ୍ୟ)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    XtManageChild(XtNameToWidget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_pointer = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_WINDOW_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, XtWindow(vimShell), wm_atoms, 2);
    XtAddEventHandler(vimShell, NoEventMask, True, gui_x11_wm_protocol_handler,
							     ଖାଲି);
#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(vimShell, (EventMask)0, True, _XEditResCheckMessages,
	    (XtPointer)ଖାଲି);
#endif

#ifdef FEAT_CLIENTSERVER
    ଯଦି (serverName == ଖାଲି && serverDelayedStartName != ଖାଲି)
    {
	// This is a :gui command in a plain vim with no previous server
	commWindow = XtWindow(vimShell);
	(ଶୂନ୍ୟ)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    ଅନ୍ୟଥା
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(gui.dpy, XtWindow(vimShell));
    }
    XtAddEventHandler(vimShell, PropertyChangeMask, False,
		      gui_x11_send_event_handler, ଖାଲି);
#endif

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ifdef FEAT_XIM
    xim_init();
#endif

    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    ଶୂନ୍ୟ
gui_init_tooltip_font(ଶୂନ୍ୟ)
{
    Xrm_ମୂଲ୍ୟ from, to;

    from.addr = (ଅକ୍ଷର *)gui.rsrc_tooltip_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.tooltip_fontset;
    to.size = ଆକାର(XFontSet);

    ଯଦି (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
}
#endif

#ଯଦି defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    ଶୂନ୍ୟ
gui_init_menu_font(ଶୂନ୍ୟ)
{
    Xrm_ମୂଲ୍ୟ from, to;

# ifdef FONTSET_ALWAYS
    from.addr = (ଅକ୍ଷର *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_fontset;
    to.size = ଆକାର(GuiFontset);

    ଯଦି (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
# ଅନ୍ୟଥା
    from.addr = (ଅକ୍ଷର *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_font;
    to.size = ଆକାର(GuiFont);

    ଯଦି (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &to) == False)
    {
	// Failed. What to do?
    }
# endif
}
#endif

    ଶୂନ୍ୟ
gui_mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc UNUSED)
{
#ଯଦି 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#endif
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ଖାଲି);
    *x = xpos;
    *y = ypos;
    ଫେରନ୍ତୁ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ଶୂନ୍ୟ
gui_mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell,
	XtNx,	x,
	XtNy,	y,
	ଖାଲି);
}

    ଶୂନ୍ୟ
gui_mch_set_shellsize(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_width,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_height,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		base_width,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		base_height,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		direction UNUSED)
{
#ifdef FEAT_XIM
    height += xim_get_status_area_height(),
#endif
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell,
	XtNwidthInc,	gui.char_width,
	XtNheightInc,	gui.char_height,
#ଯଦି defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	XtNbaseWidth,	base_width,
	XtNbaseHeight,	base_height,
#endif
	XtNminWidth,	min_width,
	XtNminHeight,	min_height,
	XtNwidth,	width,
	XtNheight,	height,
	ଖାଲି);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    ଶୂନ୍ୟ
gui_mch_get_screen_dimensions(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *screen_h)
{
    *screen_w = DisplayWidth(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = DisplayHeight(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_font(
    char_u	*font_name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		do_fontset UNUSED)
{
    XFontStruct	*font = ଖାଲି;

#ifdef FEAT_XFONTSET
    XFontSet	fontset = ଖାଲି;
#endif

#ifdef FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    ଯଦି (font_name != ଖାଲି && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	ଯଦି (font_name == ଖାଲି && !gui.starting)
	    ଫେରନ୍ତୁ OK;
    }
#endif

#ifdef FEAT_XFONTSET
    ଯଦି (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	ଯଦି (font_name != ଖାଲି)
	{
	    fontset = (XFontSet)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    ଯଦି (fontset == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
#endif
    {
	ଯଦି (font_name == ଖାଲି)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    ଯଦି (font == ଖାଲି)
		font_name = (char_u *)DFLT_FONT;
	}
	ଯଦି (font == ଖାଲି)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	ଯଦି (font == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    ଯଦି (fontset != ଖାଲି)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_width = fontset_width(fontset);
	gui.char_height = fontset_height(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    ଅନ୍ୟଥା
#endif
    {
	gui.norm_font = (GuiFont)font;
#ifdef FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#endif
	gui.char_width = font->max_bounds.width;
	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_set_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    ଯଦି (font_name == gui.rsrc_font_name)
    {
	ଯଦି (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ଖାଲି
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	ଯଦି (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ଖାଲି
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	ଯଦି (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ଖାଲି
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    ଅନ୍ୟଥା
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	ଯଦି (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	ଯଦି (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	ଯଦି (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ifdef FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#endif

    ଫେରନ୍ତୁ OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା giveErrorIfMissing)
{
    XFontStruct	*font;

    ଯଦି (!gui.in_use || name == ଖାଲି)	// can't do this when GUI not running
	ଫେରନ୍ତୁ NOFONT;

    font = XLoadQueryFont(gui.dpy, (ଅକ୍ଷର *)name);

    ଯଦି (font == ଖାଲି)
    {
	ଯଦି (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ଫେରନ୍ତୁ NOFONT;
    }

#ifdef DEBUG
    printf("Font Information for '%s':\n", name);
    printf("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.width, font->ascent + font->descent,
	   font->ascent, font->descent);
    printf("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    printf("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    printf("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    printf("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.width));
    printf("\n");
#endif

    ଯଦି (font->max_bounds.width != font->min_bounds.width)
    {
	semsg(_(e_font_str_is_not_fixed_width), name);
	XFreeFont(gui.dpy, font);
	ଫେରନ୍ତୁ NOFONT;
    }
    ଫେରନ୍ତୁ (GuiFont)font;
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ଖାଲି;

    ଯଦି (name != ଖାଲି && font == ଖାଲି)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    ଅନ୍ୟଥା ଯଦି (font != ଖାଲି)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	unsigned long ମୂଲ୍ୟ = 0L;

	ଯଦି (XGetFontProperty(font, XA_FONT, &ମୂଲ୍ୟ))
	{
	    ଅକ୍ଷର *xa_font_name = ଖାଲି;

	    xa_font_name = XGetAtomName(gui.dpy, ମୂଲ୍ୟ);
	    ଯଦି (xa_font_name != ଖାଲି)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    ଅନ୍ୟଥା ଯଦି (name != ଖାଲି)
		ret = vim_strsave(name);
	}
	ଅନ୍ୟଥା ଯଦି (name != ଖାଲି)
	    ret = vim_strsave(name);
    }
    ଫେରନ୍ତୁ ret;
}
#endif

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_adjust_charheight(ଶୂନ୍ୟ)
{
#ifdef FEAT_XFONTSET
    ଯଦି (gui.fontset != NOFONTSET)
    {
	gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
							    + p_linespace / 2;
    }
    ଅନ୍ୟଥା
#endif
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Set the current text font.
 */
    ଶୂନ୍ୟ
gui_mch_set_font(GuiFont font)
{
    ସ୍ଥିର Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    ଯଦି (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.text_gc, fid);
	XSetFont(gui.dpy, gui.back_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ifdef FEAT_XFONTSET
    current_fontset = (XFontSet)ଖାଲି;
#endif
}

#ଯଦି defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    ଶୂନ୍ୟ
gui_mch_set_fontset(GuiFontset fontset)
{
    current_fontset = (XFontSet)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    ଶୂନ୍ୟ
gui_mch_free_font(GuiFont font)
{
    ଯଦି (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#ଯଦି defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    ଶୂନ୍ୟ
gui_mch_free_fontset(GuiFontset fontset)
{
    ଯଦି (fontset != NOFONTSET)
	XFreeFontSet(gui.dpy, (XFontSet)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		giveErrorIfMissing,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fixed_width)
{
    XFontSet	fontset;
    ଅକ୍ଷର	**missing, *def_str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_missing;

    ଯଦି (!gui.in_use || name == ଖାଲି)
	ଫେରନ୍ତୁ NOFONTSET;

    fontset = XCreateFontSet(gui.dpy, (ଅକ୍ଷର *)name, &missing, &ଅଙ୍କ_missing,
			     &def_str);
    ଯଦି (ଅଙ୍କ_missing > 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	ଯଦି (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_in_fontset), name);
	    ପାଇଁ (i = 0; i < ଅଙ୍କ_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    ଯଦି (fontset == ଖାଲି)
    {
	ଯଦି (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	ଫେରନ୍ତୁ NOFONTSET;
    }

    ଯଦି (fixed_width && check_fontset_sanity(fontset) == FAIL)
    {
	XFreeFontSet(gui.dpy, fontset);
	ଫେରନ୍ତୁ NOFONTSET;
    }
    ଫେରନ୍ତୁ (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_fontset_sanity(XFontSet fs)
{
    XFontStruct	**xfs;
    ଅକ୍ଷର	**font_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fn;
    ଅକ୍ଷର	*base_name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_width;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		min_font_idx = 0;

    base_name = XBaseFontNameListOfFontSet(fs);
    fn = XFontsOfFontSet(fs, &xfs, &font_name);
    ପାଇଁ (i = 0; i < fn; i++)
    {
	ଯଦି (xfs[i]->max_bounds.width != xfs[i]->min_bounds.width)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_width),
		    base_name, font_name[i]);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    // scan base font width
    min_width = 32767;
    ପାଇଁ (i = 0; i < fn; i++)
    {
	ଯଦି (xfs[i]->max_bounds.width<min_width)
	{
	    min_width = xfs[i]->max_bounds.width;
	    min_font_idx = i;
	}
    }
    ପାଇଁ (i = 0; i < fn; i++)
    {
	ଯଦି (	   xfs[i]->max_bounds.width != 2 * min_width
		&& xfs[i]->max_bounds.width != min_width)
	{
	    semsg(_(e_fontset_name_str), base_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)xfs[min_font_idx]->max_bounds.width);
	    semsg(_("Font%d width: %d"), i, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)xfs[i]->max_bounds.width);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    // it seems ok. Good Luck!!
    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fontset_width(XFontSet fs)
{
 ଫେରନ୍ତୁ XmbTextEscapement(fs, "Vim", 3) / 3;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fontset_height(
    XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    ଫେରନ୍ତୁ extents->max_logical_extent.height;
}

# ଯଦି 0
// NOT USED YET
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fontset_descent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet (fs);
    ଫେରନ୍ତୁ extents->max_logical_extent.height + extents->max_logical_extent.y;
}
# endif

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fontset_ascent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    ଫେରନ୍ତୁ -extents->max_logical_extent.y;
}

#endif // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    ଯଦି (!gui.in_use || name == ଖାଲି || *name == NUL)
	ଫେରନ୍ତୁ INVALCOLOR;

    requested = gui_get_color_cmn(name);
    ଯଦି (requested == INVALCOLOR)
	ଫେରନ୍ତୁ INVALCOLOR;

    ଫେରନ୍ତୁ gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    XColor	available;
    Colormap	colormap;

#ଯଦି 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    ଅକ୍ଷର	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, ଆକାର(spec), "#%.2x%.2x%.2x", r, g, b);
    ଯଦି (XParseColor(gui.dpy, colormap, (ଅକ୍ଷର *)spec, &available) != 0
	    && XAllocColor(gui.dpy, colormap, &available) != 0)
	ଫେରନ୍ତୁ (guicolor_T)available.pixel;
#endif
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_FIELD(available);
    available.red = r << 8;
    available.green = g << 8;
    available.blue = b << 8;
    ଯଦି (XAllocColor(gui.dpy, colormap, &available) != 0)
	ଫେରନ୍ତୁ (guicolor_T)available.pixel;

    ଫେରନ୍ତୁ INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    ଶୂନ୍ୟ
gui_mch_set_fg_color(guicolor_T color)
{
    ଯଦି (color == prev_fg_color)
	ଫେରନ୍ତୁ;

    XSetForeground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    ଶୂନ୍ୟ
gui_mch_set_bg_color(guicolor_T color)
{
    ଯଦି (color == prev_bg_color)
	ଫେରନ୍ତୁ;

    XSetBackground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    ଶୂନ୍ୟ
gui_mch_set_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    ସ୍ଥିର Cursor
gui_x11_create_blank_mouse(ଶୂନ୍ୟ)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (unsigned long)0, (XGCValues*)0);

    ଯଦି (gc != ଖାଲି)
    {
	XDrawPoint(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    ଫେରନ୍ତୁ XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
draw_curl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			offset;
    ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
    ପାଇଁ (i = FILL_X(col); i < FILL_X(col + cells); ++i)
    {
	offset = val[i % 8];
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, i,
						FILL_Y(row + 1) - 1 - offset);
    }
    XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
}

    ଶୂନ୍ୟ
gui_mch_draw_string(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
    char_u	*s,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			cells = len;
    ସ୍ଥିର ଶୂନ୍ୟ		*buf = ଖାଲି;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen = 0;
    char_u		*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			wlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			c;

    ଯଦି (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	ଯଦି (buflen < len)
	{
	    XtFree((ଅକ୍ଷର *)buf);
	    buf = (ଶୂନ୍ୟ *)XtMalloc(len * (ଆକାର(XChar2b) < ଆକାର(wchar_t)
					? ଆକାର(wchar_t) : ଆକାର(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	ଯେତେବେଳେ (p < s + len)
	{
	    c = utf_ptr2char(p);
#ifdef FEAT_XFONTSET
	    ଯଦି (current_fontset != ଖାଲି)
	    {
# ifdef SMALL_WCHAR_T
		ଯଦି (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    ଅନ୍ୟଥା
#endif
	    {
		ଯଦି (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (unsigned)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    ଅନ୍ୟଥା ଯଦି (has_mbyte)
    {
	cells = 0;
	ପାଇଁ (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ifdef FEAT_XFONTSET
    ଯଦି (current_fontset != ଖାଲି)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.height = gui.char_height;
	clip.width = gui.char_width * cells + 1;
	XSetClipRectangles(gui.dpy, gui.text_gc, FILL_X(col), FILL_Y(row),
		&clip, 1, Unsorted);
    }
#endif

    ଯଦି (flags & DRAW_TRANSP)
    {
	ଯଦି (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ଅନ୍ୟଥା
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ଅକ୍ଷର *)s, len);
    }
    ଅନ୍ୟଥା ଯଦି (p_linespace != 0
#ifdef FEAT_XFONTSET
	    || current_fontset != ଖାଲି
#endif
	    )
    {
	XSetForeground(gui.dpy, gui.text_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		FILL_Y(row), gui.char_width * cells, gui.char_height);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);

	ଯଦି (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ଅନ୍ୟଥା
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ଅକ୍ଷର *)s, len);
    }
    ଅନ୍ୟଥା
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	ଯଦି (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ଅନ୍ୟଥା
	    XDrawImageString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ଅକ୍ଷର *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    ଯଦି (flags & DRAW_BOLD)
    {
	ଯଦି (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), buf, wlen);
	ଅନ୍ୟଥା
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), (ଅକ୍ଷର *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    ଯଦି (flags & DRAW_UNDERC)
	draw_curl(row, col, cells);

    // Underline: draw a line at the bottom of the character cell.
    ଯଦି (flags & DRAW_UNDERL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y = FILL_Y(row + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ଯଦି (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_ଧାଡ଼ି(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
    }

    ଯଦି (flags & DRAW_STRIKE)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	y = FILL_Y(row + 1) - gui.char_height/2;

	XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
	XDraw_ଧାଡ଼ି(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
    }

#ifdef FEAT_XFONTSET
    ଯଦି (current_fontset != ଖାଲି)
	XSetClipMask(gui.dpy, gui.text_gc, None);
#endif
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_haskey(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != (ଚାବି_Sym)0; i++)
	ଯଦି (name[0] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code0 &&
					 name[1] == special_ଚାବିଗୁଡ଼ିକ[i].vim_code1)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_get_x11_windis(Window *win, Display **dis)
{
    *win = XtWindow(vimShell);
    *dis = gui.dpy;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
gui_mch_beep(ଶୂନ୍ୟ)
{
    XBell(gui.dpy, 0);
}

    ଶୂନ୍ୟ
gui_mch_flash(ପୂର୍ଣ୍ଣସଂଖ୍ୟା msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns) + gui.border_offset,
	    FILL_Y((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows) + gui.border_offset);
    XSync(gui.dpy, False);
    ui_delay((long)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns) + gui.border_offset,
	    FILL_Y((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows) + gui.border_offset);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ଶୂନ୍ୟ
gui_mch_invert_rectangle(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nr,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_width, (nr) * gui.char_height);
}

/*
 * Iconify the GUI window.
 */
    ଶୂନ୍ୟ
gui_mch_iconify(ଶୂନ୍ୟ)
{
    XIconifyWindow(gui.dpy, XtWindow(vimShell), DefaultScreen(gui.dpy));
}

#ଯଦି defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ଶୂନ୍ୟ
gui_mch_set_foreground(ଶୂନ୍ୟ)
{
    XMapRaised(gui.dpy, XtWindow(vimShell));
}
#endif

/*
 * Draw a cursor without focus.
 */
    ଶୂନ୍ୟ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w = 1;

    ଯଦି (mb_lefthalve(gui.row, gui.col))
	w = 2;
    gui_mch_set_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(gui.col),
	    FILL_Y(gui.row), w * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ଶୂନ୍ୟ
gui_mch_draw_part_cursor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h, guicolor_T color)
{
    gui_mch_set_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.text_gc,
#ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ଡାହାଣ ? FILL_X(gui.col + 1) - w :
#endif
		FILL_X(gui.col),
	    FILL_Y(gui.row) + gui.char_height - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ଶୂନ୍ୟ
gui_mch_update(ଶୂନ୍ୟ)
{
    Xt_ନିବେଶ_Mask mask, desired;

#ifdef ALT_X_ନିବେଶ
    ଯଦି (suppress_alternate_ନିବେଶ)
	desired = (XtIMXEvent | XtIMTimer);
    ଅନ୍ୟଥା
#endif
	desired = (XtIMAll);
    ଯେତେବେଳେ ((mask = XtAppPending(app_context)) && (mask & desired)
	    && !vim_is_ନିବେଶ_buf_full())
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_wait_for_chars(long wtime)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    focus;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    timed_out;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_ନିବେଶ_Mask	    desired;
#ifdef FEAT_JOB_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#endif

    timed_out = FALSE;

    ଯଦି (wtime >= 0)
	timer = XtAppAddTimeOut(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_out);
#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ଯଦି (channel_any_keep_open())
	channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
				   channel_poll_cb, (XtPointer)&channel_timer);
#endif

    focus = gui.in_focus;
    desired = (XtIMAll);
    ଯେତେବେଳେ (!timed_out)
    {
	// Stop or start blinking when focus changes
	ଯଦି (gui.in_focus != focus)
	{
	    ଯଦି (gui.in_focus)
		gui_mch_start_blink();
	    ଅନ୍ୟଥା
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ଯଦି (did_add_timer)
	    // Need to recompute the waiting time.
	    ଭାଙ୍ଗ;
# endif
#endif

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	ଯଦି (ନିବେଶ_available())
	{
	    retval = OK;
	    ଭାଙ୍ଗ;
	}
    }

    ଯଦି (timer != (XtIntervalId)0 && !timed_out)
	XtRemoveTimeOut(timer);
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (channel_timer != (XtIntervalId)0)
	XtRemoveTimeOut(channel_timer);
#endif

    ଫେରନ୍ତୁ retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    ଶୂନ୍ୟ
gui_mch_flush(ଶୂନ୍ୟ)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ଶୂନ୍ୟ
gui_mch_clear_block(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col1,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.back_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height);
}

    ଶୂନ୍ୟ
gui_mch_clear_all(ଶୂନ୍ୟ)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ଶୂନ୍ୟ
gui_mch_delete_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    ଯଦି (gui.visibility == VisibilityFullyObscured)
	ଫେରନ୍ତୁ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ବାମ), FILL_Y(row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ),
	gui.char_width * (gui.scroll_region_ଡାହାଣ - gui.scroll_region_ବାମ + 1)
			       + (gui.scroll_region_ଡାହାଣ == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1),
	FILL_X(gui.scroll_region_ବାମ), FILL_Y(row));

    gui_clear_block(gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1,
						       gui.scroll_region_ବାମ,
			  gui.scroll_region_bot, gui.scroll_region_ଡାହାଣ);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ଶୂନ୍ୟ
gui_mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    ଯଦି (gui.visibility == VisibilityFullyObscured)
	ଫେରନ୍ତୁ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ବାମ), FILL_Y(row),
	gui.char_width * (gui.scroll_region_ଡାହାଣ - gui.scroll_region_ବାମ + 1)
			       + (gui.scroll_region_ଡାହାଣ == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1),
	FILL_X(gui.scroll_region_ବାମ), FILL_Y(row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ));

    gui_clear_block(row, gui.scroll_region_ବାମ,
				row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ - 1, gui.scroll_region_ଡାହାଣ);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_check_copy_area(ଶୂନ୍ୟ)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    ଯଦି (gui.visibility != VisibilityPartiallyObscured)
	ଫେରନ୍ତୁ;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    ପାଇଁ (;;)
    {
	ଯଦି (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    ଫେରନ୍ତୁ;	// The scroll worked.

	ଯଦି (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    ଯଦି (gevent->ଗଣନା_2 == 0)
		ଫେରନ୍ତୁ;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ଶୂନ୍ୟ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_mch_own_selection(Clipboard_T *cbd)
{
    ଫେରନ୍ତୁ clip_x11_own_selection(vimShell, cbd);
}

    ଶୂନ୍ୟ
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    ଶୂନ୍ୟ
clip_mch_set_selection(
    Clipboard_T	*cbd)
{
    clip_x11_set_selection(cbd);
}

#ଯଦି defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    ଶୂନ୍ୟ
gui_mch_menu_grey(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା grey)
{
    ଯଦି (menu->id == (Widget)0)
	ଫେରନ୍ତୁ;

    gui_mch_menu_hidden(menu, False);
    ଯଦି (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	XtSetSensitive(menu->id, False);
    ଅନ୍ୟଥା
	XtSetSensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    ଶୂନ୍ୟ
gui_mch_menu_hidden(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hidden)
{
    ଯଦି (menu->id == (Widget)0)
	ଫେରନ୍ତୁ;

    ଯଦି (hidden)
	XtUnmanageChild(menu->id);
    ଅନ୍ୟଥା
	XtManageChild(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ଶୂନ୍ୟ
gui_mch_draw_menubar(ଶୂନ୍ୟ)
{
    // Nothing to do in X
}

    ଶୂନ୍ୟ
gui_x11_menu_cb(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    gui_menu_cb((vimmenu_T *)client_data);
}

#endif // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_wm_protocol_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    /*
     * Only deal with Client messages.
     */
    ଯଦି (event->type != ClientMessage)
	ଫେରନ୍ତୁ;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    ଯଦି ((Atom)((XClientMessageEvent *)event)->data.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	out_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, XtWindow(vimShell), ଖାଲି, 0);
	ଫେରନ୍ତୁ;
    }

    ଯଦି ((Atom)((XClientMessageEvent *)event)->data.l[0] !=
						  wm_atoms[DELETE_WINDOW_IDX])
	ଫେରନ୍ତୁ;

    gui_shell_closed();
}

#ifdef FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_send_event_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    ଯଦି (e->type == PropertyNotify && e->window == commWindow
	    && e->atom == commProperty &&  e->state == Property_New_ମୂଲ୍ୟ)
	serverEventProc(gui.dpy, event, 0);
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

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		blink_state = BLINK_NONE;
ସ୍ଥିର long_u		blink_waittime = 700;
ସ୍ଥିର long_u		blink_ontime = 400;
ସ୍ଥିର long_u		blink_offtime = 250;
ସ୍ଥିର XtIntervalId	blink_timer = (XtIntervalId)0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blinking(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state != BLINK_NONE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blink_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state == BLINK_OFF;
}

    ଶୂନ୍ୟ
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ଶୂନ୍ୟ
gui_mch_stop_blink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_call_gui_update_cursor)
{
    ଯଦି (blink_timer != (XtIntervalId)0)
    {
	XtRemoveTimeOut(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    ଯଦି (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_x11_blink_cb(
    XtPointer	    timed_out UNUSED,
    XtIntervalId    *interval_id UNUSED)
{
    ଯଦି (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = XtAppAddTimeOut(app_context, blink_offtime,
						      gui_x11_blink_cb, ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = XtAppAddTimeOut(app_context, blink_ontime,
						      gui_x11_blink_cb, ଖାଲି);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ଶୂନ୍ୟ
gui_mch_start_blink(ଶୂନ୍ୟ)
{
    ଯଦି (blink_timer != (XtIntervalId)0)
	XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ଯଦି (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = XtAppAddTimeOut(app_context, blink_waittime,
						      gui_x11_blink_cb, ଖାଲି);
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

    ଫେରନ୍ତୁ (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((unsigned)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    ଶୂନ୍ୟ
gui_x11_callbacks(Widget textArea, Widget vimForm)
{
    XtAddEventHandler(textArea, VisibilityChangeMask, FALSE,
	gui_x11_visibility_cb, (XtPointer)0);

    XtAddEventHandler(textArea, ExposureMask, FALSE, gui_x11_expose_cb,
	(XtPointer)0);

    XtAddEventHandler(vimShell, StructureNotifyMask, FALSE,
	gui_x11_resize_window_cb, (XtPointer)0);

    XtAddEventHandler(vimShell, FocusChangeMask, FALSE, gui_x11_focus_change_cb,
	(XtPointer)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ଯଦି (vim_strchr(p_go, GO_POINTER) != ଖାଲି)
    {
	XtAddEventHandler(vimShell, LeaveWindowMask, FALSE, gui_x11_leave_cb,
	    (XtPointer)0);
	XtAddEventHandler(textArea, LeaveWindowMask, FALSE, gui_x11_leave_cb,
	    (XtPointer)0);
	XtAddEventHandler(textArea, EnterWindowMask, FALSE, gui_x11_enter_cb,
	    (XtPointer)0);
	XtAddEventHandler(vimShell, EnterWindowMask, FALSE, gui_x11_enter_cb,
	    (XtPointer)0);
    }

    XtAddEventHandler(vimForm, ଚାବି_Press_Mask, FALSE, gui_x11_ଚାବି_hit_cb,
	(XtPointer)0);
    XtAddEventHandler(textArea, ଚାବି_Press_Mask, FALSE, gui_x11_ଚାବି_hit_cb,
	(XtPointer)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    XtAddEventHandler(vimForm, PointerMotionMask,
	FALSE, gui_x11_mouse_cb, (XtPointer)1);
    XtAddEventHandler(textArea, ButtonPressMask | ButtonReleaseMask |
					 ButtonMotionMask | PointerMotionMask,
	FALSE, gui_x11_mouse_cb, (XtPointer)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    ଶୂନ୍ୟ
gui_mch_getmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rootx, rooty, winx, winy;
    Window	ମୂଳ, child;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask;

    ଯଦି (gui.wid && XQueryPointer(gui.dpy, gui.wid, &ମୂଳ, &child,
					 &rootx, &rooty, &winx, &winy, &mask))
    {
	*x = winx;
	*y = winy;
    }
    ଅନ୍ୟଥା
    {
	*x = -1;
	*y = -1;
    }
}

    ଶୂନ୍ୟ
gui_mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    ଯଦି (gui.wid)
	XWarpPointer(gui.dpy, (Window)0, gui.wid, 0, 0, 0, 0, x, y);
}

#ଯଦି defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_last_mouse_event(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ &last_mouse_event;
}
#endif

#ଯଦି defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_WIDTH (gui.char_width * 2)

    ଶୂନ୍ୟ
gui_mch_drawsign(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr)
{
    XImage	*sign;

    ଯଦି (!gui.in_use || (sign = (XImage *)sign_get_image(typenr)) == ଖାଲି)
	ଫେରନ୍ତୁ;

    XClearArea(gui.dpy, gui.wid, TEXT_X(col), TEXT_Y(row) - sign->height,
	    SIGN_WIDTH, gui.char_height, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.text_gc, sign, 0, 0,
	    TEXT_X(col) + (SIGN_WIDTH - sign->width) / 2,
	    TEXT_Y(row) - sign->height,
	    sign->width, sign->height);
}

    ଶୂନ୍ୟ *
gui_mch_register_sign(char_u *signfile)
{
    XpmAttributes   attrs;
    XImage	    *sign = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    status;

    /*
     * Setup the color substitution table.
     */
    ଯଦି (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_ପ୍ରତୀକ color[5] =
	{
	    {"none", ଖାଲି, 0},
	    {"iconColor1", ଖାଲି, 0},
	    {"bottomShadowColor", ଖାଲି, 0},
	    {"topShadowColor", ଖାଲି, 0},
	    {"selectColor", ଖାଲି, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.back_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	status = XpmReadFileToImage(gui.dpy, (ଅକ୍ଷର *)signfile,
							 &sign, ଖାଲି, &attrs);
	ଯଦି (status == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	ଅନ୍ୟଥା
	    emsg(_(e_couldnt_read_in_sign_data));
    }

    ଫେରନ୍ତୁ (ଶୂନ୍ୟ *)sign;
}

    ଶୂନ୍ୟ
gui_mch_destroy_sign(ଶୂନ୍ୟ *sign)
{
    XDestroyImage((XImage*)sign);
}
#endif


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 */
    ଶୂନ୍ୟ
gui_mch_mousehide(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    ଯଦି (gui.pointer_hidden == hide)
	ଫେରନ୍ତୁ;

    gui.pointer_hidden = hide;
    ଯଦି (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    ଅନ୍ୟଥା
#ifdef FEAT_MOUSESHAPE
	mch_set_mouse_shape(last_shape);
#ଅନ୍ୟଥା
    XUndefineCursor(gui.dpy, gui.wid);
#endif
}

#ଯଦି defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mshape_ids[] =
{
    XC_ବାମ_ptr,		// arrow
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
    XC_ଡାହାଣ_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ବାମ_ptr			// last one
};

    ଶୂନ୍ୟ
mch_set_mouse_shape(ପୂର୍ଣ୍ଣସଂଖ୍ୟା shape)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;

    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

    ଯଦି (shape == MSHAPE_HIDE || gui.pointer_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    ଅନ୍ୟଥା
    {
	ଯଦି (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ଯଦି (id >= XC_ଅଙ୍କ_glyphs)
		id = XC_ବାମ_ptr;
	    ଅନ୍ୟଥା
		id &= ~1;	// they are always even (why?)
	}
	ଅନ୍ୟଥା
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    ଯଦି (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

#ଯଦି defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    ଶୂନ୍ୟ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    ଯଦି (menu->id == ଖାଲି || menu->parent == ଖାଲି
				|| !menu_is_toolbar(menu->parent->name))
	ଫେରନ୍ତୁ;

    // Always destroy and create the balloon, in case the string was
    // changed.
    ଯଦି (menu->tip != ଖାଲି)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ଖାଲି;
    }
    ଯଦି (menu->strings[MENU_INDEX_TIP] != ଖାଲି)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->strings[MENU_INDEX_TIP],
		ଖାଲି,
		ଖାଲି);
}
#endif
