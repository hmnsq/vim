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

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <X11/keysym.h>
#അന്തര്ഭാവ <X11/Xatom.h>
#അന്തര്ഭാവ <X11/StringDefs.h>
#അന്തര്ഭാവ <X11/Intrinsic.h>
#അന്തര്ഭാവ <X11/Shell.h>
#അന്തര്ഭാവ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#എങ്കിൽ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#അല്ലെങ്കിൽ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  അല്ലെങ്കിൽ
#   include <X11/xpm.h>
#  endif
# endif
#endif

#ifdef FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# അല്ലെങ്കിൽ
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
#അല്ലെങ്കിൽ
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

സ്ഥിതം Atom   wm_atoms[2];	// Window Manager Atoms
#define DELETE_WINDOW_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#define SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ifdef FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
സ്ഥിതം XFontSet current_fontset = ശൂന്യ;
# എങ്കിൽ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	ചെയ്യുക \
	{ \
	    എങ്കിൽ (current_fontset != ശൂന്യ) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    അല്ലെങ്കിൽ \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} ആകുമ്പോൾ (0)
# endif
# എങ്കിൽ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	ചെയ്യുക \
	{ \
	    എങ്കിൽ (current_fontset != ശൂന്യ) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    അല്ലെങ്കിൽ \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ആകുമ്പോൾ (0)
# endif
# എങ്കിൽ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	ചെയ്യുക \
	{ \
	    എങ്കിൽ (current_fontset != ശൂന്യ) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    അല്ലെങ്കിൽ \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ആകുമ്പോൾ (0)
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ check_fontset_sanity(XFontSet fs);
സ്ഥിതം പൂർണ്ണസംഖ്യ fontset_width(XFontSet fs);
സ്ഥിതം പൂർണ്ണസംഖ്യ fontset_ascent(XFontSet fs);
#endif

സ്ഥിതം guicolor_T	prev_fg_color = INVALCOLOR;
സ്ഥിതം guicolor_T	prev_bg_color = INVALCOLOR;
സ്ഥിതം guicolor_T	prev_sp_color = INVALCOLOR;

#എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
സ്ഥിതം XButtonPressedEvent last_mouse_event;
#endif

സ്ഥിതം ശൂന്യം gui_x11_check_copy_area(ശൂന്യം);
#ifdef FEAT_CLIENTSERVER
സ്ഥിതം ശൂന്യം gui_x11_send_event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif
സ്ഥിതം ശൂന്യം gui_x11_wm_protocol_handler(Widget, XtPointer, XEvent *, Boolean *);
സ്ഥിതം Cursor gui_x11_create_blank_mouse(ശൂന്യം);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
സ്ഥിതം ഘടന specialkey
{
    താക്കോൽ_Sym  താക്കോൽ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_താക്കോലുകൾ[] =
{
    {XK_Up,		'k', 'u'},
    {XK_Down,		'k', 'd'},
    {XK_ഇടത്,		'k', 'l'},
    {XK_വലത്,		'k', 'r'},

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
    {XK_തിരുകുക,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_End,		'@', '7'},
    {XK_Prior,		'k', 'P'},
    {XK_Next,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ifdef XK_KP_ഇടത്
    {XK_KP_ഇടത്,	'k', 'l'},
    {XK_KP_വലത്,	'k', 'r'},
    {XK_KP_Up,		'k', 'u'},
    {XK_KP_Down,	'k', 'd'},
    {XK_KP_തിരുകുക,	KS_EXTRA, (char_u)KE_KINS},
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
    {(താക്കോൽ_Sym)0,	    0, 0}
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
സ്ഥിതം XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	വലുപ്പം(Pixel),
	XtOffsetOf(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	വലുപ്പം(Pixel),
	XtOffsetOf(gui_T, def_back_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	വലുപ്പം(String *),
	XtOffsetOf(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	വലുപ്പം(String *),
	XtOffsetOf(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	വലുപ്പം(String *),
	XtOffsetOf(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	വലുപ്പം(String *),
	XtOffsetOf(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	വലുപ്പം(String *),
	XtOffsetOf(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	വലുപ്പം(Bool),
	XtOffsetOf(gui_T, rsrc_rev_video),
	XtRImmediate,
	(XtPointer)False
    },
    {
	XtNborderWidth,
	XtCBorderWidth,
	XtRInt,
	വലുപ്പം(പൂർണ്ണസംഖ്യ),
	XtOffsetOf(gui_T, border_width),
	XtRImmediate,
	(XtPointer)2
    },
    {
	XtNscrollbarWidth,
	XtCScrollbarWidth,
	XtRInt,
	വലുപ്പം(പൂർണ്ണസംഖ്യ),
	XtOffsetOf(gui_T, scrollbar_width),
	XtRImmediate,
	(XtPointer)SB_DEFAULT_WIDTH
    },
#ifdef FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	XtNmenuFontSet,
	XtCMenuFontSet,
# അല്ലെങ്കിൽ
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#endif
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ifdef FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	വലുപ്പം(അക്ഷരം *),
	XtOffsetOf(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	XtCFontSet,
	XtRFontSet,
	വലുപ്പം(XFontSet),
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
	വലുപ്പം(അക്ഷരം*),
	XtOffsetOf(gui_T, rsrc_preedit_type_name),
	XtRString,
	(XtPointer)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	വലുപ്പം(അക്ഷരം*),
	XtOffsetOf(gui_T, rsrc_നിവേശം_method),
	XtRString,
	ശൂന്യ
    },
#endif // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
സ്ഥിതം XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ശൂന്യ},
    {"-background",	".background",	    XrmoptionSepArg,	ശൂന്യ},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ശൂന്യ},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ശൂന്യ},
    {"-fn",		".font",	    XrmoptionSepArg,	ശൂന്യ},
    {"-font",		".font",	    XrmoptionSepArg,	ശൂന്യ},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ശൂന്യ},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ശൂന്യ},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ശൂന്യ},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ശൂന്യ},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ശൂന്യ},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ശൂന്യ},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ശൂന്യ},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ശൂന്യ},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ശൂന്യ},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ശൂന്യ},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ശൂന്യ},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ശൂന്യ},
#ifdef FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ശൂന്യ},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ശൂന്യ},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ശൂന്യ},
#അല്ലെങ്കിൽ
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ശൂന്യ},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ശൂന്യ},
#endif
    {"-xrm",		ശൂന്യ,		    XrmoptionResArg,	ശൂന്യ}
};

സ്ഥിതം പൂർണ്ണസംഖ്യ gui_argc = 0;
സ്ഥിതം അക്ഷരം **gui_argv = ശൂന്യ;

/*
 * Call-back routines.
 */

    സ്ഥിതം ശൂന്യം
gui_x11_timer_cb(
    XtPointer	    timed_out,
    XtIntervalId    *interval_id UNUSED)
{
    *((പൂർണ്ണസംഖ്യ *)timed_out) = TRUE;
}

#ifdef FEAT_JOB_CHANNEL
    സ്ഥിതം ശൂന്യം
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

    സ്ഥിതം ശൂന്യം
gui_x11_visibility_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    എങ്കിൽ (event->type != VisibilityNotify)
	തിരികെ;

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

    സ്ഥിതം ശൂന്യം
gui_x11_expose_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XExposeEvent	*gevent;
    പൂർണ്ണസംഖ്യ			new_x;

    എങ്കിൽ (event->type != Expose)
	തിരികെ;

    out_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    എങ്കിൽ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    എങ്കിൽ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    എങ്കിൽ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((പൂർണ്ണസംഖ്യ)Columns), 0, 0, 0, False);
    എങ്കിൽ (gevent->y > FILL_Y(Rows))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((പൂർണ്ണസംഖ്യ)Rows), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#എങ്കിൽ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    സ്ഥിതം ശൂന്യം
shellRectangle(Widget shell, XRectangle *r)
{
    Window		rootw, shellw, child, parentw;
    പൂർണ്ണസംഖ്യ			absx, absy;
    XWindowAttributes	a;
    Window		*children;
    unsigned പൂർണ്ണസംഖ്യ	children_എണ്ണം;

    shellw = XtWindow(shell);
    എങ്കിൽ (shellw == 0)
	തിരികെ;
    വേണ്ടി (;;)
    {
	XQuery_വൃക്ഷം(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_എണ്ണം);
	XFree(children);
	എങ്കിൽ (parentw == rootw)
	    നിർത്തുക;
	shellw = parentw;
    }
    XGetWindowAttributes(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.വേര്, 0, 0,
							&absx, &absy, &child);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_മൂല്യങ്ങൾ(shell, XmNheight, &r->height, XmNwidth, &r->width, ശൂന്യ);
}
#endif

    സ്ഥിതം ശൂന്യം
gui_x11_resize_window_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ lastWidth, lastHeight;

    എങ്കിൽ (event->type != ConfigureNotify)
	തിരികെ;

    എങ്കിൽ (event->xconfigure.width != lastWidth
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
#എങ്കിൽ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    എങ്കിൽ (netbeans_active())
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

    സ്ഥിതം ശൂന്യം
gui_x11_focus_change_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    gui_focus_change(event->type == FocusIn);
}

    സ്ഥിതം ശൂന്യം
gui_x11_enter_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(TRUE);
}

    സ്ഥിതം ശൂന്യം
gui_x11_leave_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(FALSE);
}

#എങ്കിൽ defined(X_HAVE_UTF8_STRING)
# എങ്കിൽ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#endif

    ശൂന്യം
gui_x11_താക്കോൽ_hit_cb(
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
#അല്ലെങ്കിൽ
    char_u		string[4], string2[3];
#endif
    താക്കോൽ_Sym		താക്കോൽ_sym;
    പൂർണ്ണസംഖ്യ			len;
    പൂർണ്ണസംഖ്യ			i;
    പൂർണ്ണസംഖ്യ			modifiers;
    പൂർണ്ണസംഖ്യ			താക്കോൽ;

    ev_press = (XKeyPressedEvent *)event;

#ifdef FEAT_XIM
    എങ്കിൽ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	എങ്കിൽ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (അക്ഷരം *)string,
				  വലുപ്പം(string_shortbuf), &താക്കോൽ_sym, &status);
	അല്ലെങ്കിൽ
# endif
	    len = XmbLookupString(xic, ev_press, (അക്ഷരം *)string,
				  വലുപ്പം(string_shortbuf), &താക്കോൽ_sym, &status);
	എങ്കിൽ (status == XBufferOverflow)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    എങ്കിൽ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (അക്ഷരം *)string,
						      len, &താക്കോൽ_sym, &status);
	    അല്ലെങ്കിൽ
# endif
		len = XmbLookupString(xic, ev_press, (അക്ഷരം *)string,
						      len, &താക്കോൽ_sym, &status);
	}
	എങ്കിൽ (status == XLookupNone || status == XLookupChars)
	    താക്കോൽ_sym = XK_Void_ചിഹ്നം;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	എങ്കിൽ (len > 0 && നിവേശം_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    പൂർണ്ണസംഖ്യ		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    എങ്കിൽ (string_alloced)
		XtFree((അക്ഷരം *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_നിവേശം(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	വേണ്ടി (i = 0; i < len; ++i)
	    എങ്കിൽ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (പൂർണ്ണസംഖ്യ)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		എങ്കിൽ (string_alloced)
		    XtFree((അക്ഷരം *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    അല്ലെങ്കിൽ
#endif
	len = XLookupString(ev_press, (അക്ഷരം *)string, വലുപ്പം(string),
		&താക്കോൽ_sym, ശൂന്യ);

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    എങ്കിൽ (താക്കോൽ_sym == SunXK_F36 || താക്കോൽ_sym == SunXK_F37)
	len = 0;
#endif

    എങ്കിൽ (താക്കോൽ_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    എങ്കിൽ (താക്കോൽ_sym == XK_minus && (ev_press->state & ControlMask))
	string[0] = Ctrl__;

#ifdef XK_ISO_ഇടത്_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    എങ്കിൽ (താക്കോൽ_sym == XK_ISO_ഇടത്_Tab)
    {
	താക്കോൽ_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    എങ്കിൽ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (പൂർണ്ണസംഖ്യ)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    എങ്കിൽ (len == 0 || len == 1)
    {
	വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != (താക്കോൽ_Sym)0; i++)
	{
	    എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == താക്കോൽ_sym)
	    {
		string[0] = CSI;
		string[1] = special_താക്കോലുകൾ[i].vim_code0;
		string[2] = special_താക്കോലുകൾ[i].vim_code1;
		len = -3;
		നിർത്തുക;
	    }
	}
    }

    // Unrecognised key is ignored.
    എങ്കിൽ (len == 0)
	പോകുക theend;

    // Handle modifiers.
    modifiers = 0;
    എങ്കിൽ (ev_press->state & ShiftMask)
	modifiers |= MOD_MASK_SHIFT;
    എങ്കിൽ (ev_press->state & ControlMask)
    {
	modifiers |= MOD_MASK_CTRL;
	എങ്കിൽ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    എങ്കിൽ (ev_press->state & Mod1Mask)
	modifiers |= MOD_MASK_ALT;
    എങ്കിൽ (ev_press->state & Mod4Mask)
	modifiers |= MOD_MASK_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    എങ്കിൽ (len == -3)
	താക്കോൽ = TO_SPECIAL(string[1], string[2]);
    അല്ലെങ്കിൽ
    {
	string[len] = NUL;
	താക്കോൽ = mb_ptr2char(string);
    }
    താക്കോൽ = simplify_താക്കോൽ(താക്കോൽ, &modifiers);
    എങ്കിൽ (താക്കോൽ == CSI)
	താക്കോൽ = K_CSI;
    എങ്കിൽ (IS_SPECIAL(താക്കോൽ))
    {
	string[0] = CSI;
	string[1] = K_SECOND(താക്കോൽ);
	string[2] = K_THIRD(താക്കോൽ);
	len = 3;
    }
    അല്ലെങ്കിൽ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	താക്കോൽ = may_adjust_താക്കോൽ_for_ctrl(modifiers, താക്കോൽ);

	len = mb_char2bytes(താക്കോൽ, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_shift_modifier(modifiers, താക്കോൽ);
    }

    എങ്കിൽ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_നിവേശം_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	പൂർണ്ണസംഖ്യ int_ch = check_for_interrupt(താക്കോൽ, modifiers);

	എങ്കിൽ (int_ch != NUL)
	{
	    trash_നിവേശം_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_നിവേശം_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    എങ്കിൽ (p_mh)
	gui_mch_mousehide(TRUE);

#എങ്കിൽ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	എങ്കിൽ ((be = gui_mch_currently_showing_beval()) != ശൂന്യ)
	    gui_mch_unpost_balloon(be);
    }
#endif
theend:
    {}	    // some compilers need a statement here
#ifdef FEAT_XIM
    എങ്കിൽ (string_alloced)
	XtFree((അക്ഷരം *)string);
#endif
}

    സ്ഥിതം ശൂന്യം
gui_x11_mouse_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    സ്ഥിതം XtIntervalId timer = (XtIntervalId)0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	timed_out = TRUE;

    പൂർണ്ണസംഖ്യ		button;
    പൂർണ്ണസംഖ്യ		repeated_click = FALSE;
    പൂർണ്ണസംഖ്യ		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    എങ്കിൽ (event->type == MotionNotify)
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

	എങ്കിൽ (button != MOUSE_DRAG)	// just moving the rodent
	{
#ifdef FEAT_MENU
	    എങ്കിൽ (dud)			// moved in vimForm
		y -= gui.menu_height;
#endif
	    gui_mouse_moved(x, y);
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	എങ്കിൽ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    എങ്കിൽ (!timed_out)
	    {
		XtRemoveTimeOut(timer);
		repeated_click = TRUE;
	    }
	    timed_out = FALSE;
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_out);
	    തിരഞ്ഞെടുപ്പ് (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		അവസ്ഥ Button1:	button = MOUSE_ഇടത്;	നിർത്തുക;
		അവസ്ഥ Button2:	button = MOUSE_നടുവ്;	നിർത്തുക;
		അവസ്ഥ Button3:	button = MOUSE_വലത്;	നിർത്തുക;
		അവസ്ഥ Button4:	button = MOUSE_4;	നിർത്തുക;
		അവസ്ഥ Button5:	button = MOUSE_5;	നിർത്തുക;
		അവസ്ഥ 6:		button = MOUSE_7;	നിർത്തുക;
		അവസ്ഥ 7:		button = MOUSE_6;	നിർത്തുക;
		അവസ്ഥ 8:		button = MOUSE_X1;	നിർത്തുക;
		അവസ്ഥ 9:		button = MOUSE_X2;	നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    തിരികെ;	// Unknown button
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	അല്ലെങ്കിൽ
	    തിരികെ;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	last_mouse_event = event->xbutton;
#endif
    }

    vim_modifiers = 0x0;
    എങ്കിൽ (x_modifiers & ShiftMask)
	vim_modifiers |= MOUSE_SHIFT;
    എങ്കിൽ (x_modifiers & ControlMask)
	vim_modifiers |= MOUSE_CTRL;
    എങ്കിൽ (x_modifiers & Mod1Mask)	    // Alt or Meta key
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
    ശൂന്യം
gui_mch_prepare(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv)
{
    പൂർണ്ണസംഖ്യ	    arg;
    പൂർണ്ണസംഖ്യ	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(അക്ഷരം *, *argc);
    എങ്കിൽ (gui_argv == ശൂന്യ)
	തിരികെ;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    ആകുമ്പോൾ (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)Xt_സംഖ്യ(cmdline_options); i++)
	    എങ്കിൽ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		നിർത്തുക;

	എങ്കിൽ (i < (പൂർണ്ണസംഖ്യ)Xt_സംഖ്യ(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    എങ്കിൽ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		found_reverse_arg = TRUE;
	    അല്ലെങ്കിൽ എങ്കിൽ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_argument = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    എങ്കിൽ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * വലുപ്പം(അക്ഷരം *));
		എങ്കിൽ (cmdline_options[i].argKind != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    എങ്കിൽ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * വലുപ്പം(അക്ഷരം *));
		}
	    }
	    argv[*argc] = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
#ifdef FEAT_NETBEANS_INTG
	    എങ്കിൽ (strncmp("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * വലുപ്പം(അക്ഷരം *));
	    argv[*argc] = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
#endif
	    arg++;
    }
}

#ifndef XtSpecificationRelease
# define CARDINAL (Cardinal *)
#അല്ലെങ്കിൽ
# എങ്കിൽ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# അല്ലെങ്കിൽ
#  define CARDINAL (പൂർണ്ണസംഖ്യ *)
# endif
#endif

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init_check(ശൂന്യം)
{
#ifdef FEAT_XIM
    XtSetLanguageProc(ശൂന്യ, ശൂന്യ, ശൂന്യ);
#endif
    open_app_context();
    എങ്കിൽ (app_context != ശൂന്യ)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_സംഖ്യ(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#എങ്കിൽ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	അക്ഷരം *p = setlocale(LC_NUMERIC, ശൂന്യ);

	എങ്കിൽ (p == ശൂന്യ || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#endif
    എങ്കിൽ (app_context == ശൂന്യ || gui.dpy == ശൂന്യ)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}


#ifdef USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
സ്ഥിതം Xt_നിവേശം_Id _xsmp_xtinputid;

    സ്ഥിതം ശൂന്യം
local_xsmp_handle_requests(
    XtPointer	c UNUSED,
    പൂർണ്ണസംഖ്യ		*s UNUSED,
    Xt_നിവേശം_Id	*i UNUSED)
{
    എങ്കിൽ (xsmp_handle_requests() == FAIL)
	Xt_Remove_നിവേശം(_xsmp_xtinputid);
}
#endif


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init(ശൂന്യം)
{
    XtGCMask	gc_mask;
    XGCValues	gc_vals;
    പൂർണ്ണസംഖ്യ		x, y, mask;
    unsigned	w, h;

#എങ്കിൽ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#endif

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ശൂന്യ);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (XtPointer)&gui,
	vim_resources, Xt_സംഖ്യ(vim_resources), ശൂന്യ);

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
    എങ്കിൽ (gui.rsrc_rev_video && gui_get_lightness(gui.back_pixel)
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

    എങ്കിൽ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    എങ്കിൽ (gui.geom != ശൂന്യ && *gui.geom != NUL)
    {
	mask = XParseGeometry((അക്ഷരം *)gui.geom, &x, &y, &w, &h);
	എങ്കിൽ (mask & Width_മൂല്യം)
	    Columns = w;
	എങ്കിൽ (mask & Height_മൂല്യം)
	{
	    എങ്കിൽ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	എങ്കിൽ (mask & (XValue|YValue))
	    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell, XtNgeometry, gui.geom, ശൂന്യ);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    എങ്കിൽ (vim_strchr(p_go, GO_ICON) != ശൂന്യ)
    {
#ifndef HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	XtSetArg(arg[0], XtNiconPixmap,
		XCreateBitmapFromData(gui.dpy,
		    Default_വേര്_Window(gui.dpy),
		    (അക്ഷരം *)vim_icon_bits,
		    vim_icon_width,
		    vim_icon_height));
	XtSetArg(arg[1], XtNiconMask,
		XCreateBitmapFromData(gui.dpy,
		    Default_വേര്_Window(gui.dpy),
		    (അക്ഷരം *)vim_mask_icon_bits,
		    vim_mask_icon_width,
		    vim_mask_icon_height));
	Xt_Set_മൂല്യങ്ങൾ(vimShell, arg, (Cardinal)2);
#അല്ലെങ്കിൽ
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    സ്ഥിതം Pixmap	icon = 0;
    സ്ഥിതം Pixmap	icon_mask = 0;
    സ്ഥിതം അക്ഷരം		**magick = vim32x32;
    Window		വേര്_window;
    XIconSize		*size;
    പൂർണ്ണസംഖ്യ			സംഖ്യ_sizes;
    Display		*dsp;
    Screen		*scr;
    XpmAttributes	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    വേര്_window = XRootWindowOfScreen(XtScreen(vimShell));
    എങ്കിൽ (XGetIconSizes(XtDisplay(vimShell), വേര്_window,
						   &size, &സംഖ്യ_sizes) != 0)
    {
	എങ്കിൽ (സംഖ്യ_sizes > 0)
	{
	    എങ്കിൽ (size->max_height >= 48 && size->max_width >= 48)
		magick = vim48x48;
	    അല്ലെങ്കിൽ എങ്കിൽ (size->max_height >= 32 && size->max_width >= 32)
		magick = vim32x32;
	    അല്ലെങ്കിൽ എങ്കിൽ (size->max_height >= 16 && size->max_width >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell, XtNcolormap, cmap, ശൂന്യ);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.depth = DefaultDepthOfScreen(scr);

    എങ്കിൽ (!icon)
    {
	XpmCreatePixmapFromData(dsp, വേര്_window, magick, &icon,
							   &icon_mask, &attr);
	XpmFreeAttributes(&attr);
    }

    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell, XmNiconPixmap, icon, XmNiconMask, icon_mask, ശൂന്യ);
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
    എങ്കിൽ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_Add_നിവേശം(app_context, xsmp_icefd,
		(XtPointer)Xt_നിവേശം_Read_Mask, local_xsmp_handle_requests, ശൂന്യ);
#endif

    തിരികെ OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    ശൂന്യം
gui_mch_uninit(ശൂന്യം)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ശൂന്യ;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    ശൂന്യം
gui_mch_new_colors(ശൂന്യം)
{
    long_u	gc_mask;
    XGCValues	gc_vals;

    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    എങ്കിൽ (gui.text_gc != ശൂന്യ)
	XChangeGC(gui.dpy, gui.text_gc, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    എങ്കിൽ (gui.back_gc != ശൂന്യ)
	XChangeGC(gui.dpy, gui.back_gc, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    എങ്കിൽ (gui.invert_gc != ശൂന്യ)
	XChangeGC(gui.dpy, gui.invert_gc, gc_mask, &gc_vals);

    gui_x11_set_back_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    പൂർണ്ണസംഖ്യ
gui_mch_open(ശൂന്യം)
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
							     ശൂന്യ);
#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(vimShell, (EventMask)0, True, _XEditResCheckMessages,
	    (XtPointer)ശൂന്യ);
#endif

#ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (serverName == ശൂന്യ && serverDelayedStartName != ശൂന്യ)
    {
	// This is a :gui command in a plain vim with no previous server
	commWindow = XtWindow(vimShell);
	(ശൂന്യം)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    അല്ലെങ്കിൽ
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(gui.dpy, XtWindow(vimShell));
    }
    XtAddEventHandler(vimShell, PropertyChangeMask, False,
		      gui_x11_send_event_handler, ശൂന്യ);
#endif

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ifdef FEAT_XIM
    xim_init();
#endif

    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    ശൂന്യം
gui_init_tooltip_font(ശൂന്യം)
{
    Xrm_മൂല്യം from, to;

    from.addr = (അക്ഷരം *)gui.rsrc_tooltip_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.tooltip_fontset;
    to.size = വലുപ്പം(XFontSet);

    എങ്കിൽ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
}
#endif

#എങ്കിൽ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    ശൂന്യം
gui_init_menu_font(ശൂന്യം)
{
    Xrm_മൂല്യം from, to;

# ifdef FONTSET_ALWAYS
    from.addr = (അക്ഷരം *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_fontset;
    to.size = വലുപ്പം(GuiFontset);

    എങ്കിൽ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
# അല്ലെങ്കിൽ
    from.addr = (അക്ഷരം *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_font;
    to.size = വലുപ്പം(GuiFont);

    എങ്കിൽ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &to) == False)
    {
	// Failed. What to do?
    }
# endif
}
#endif

    ശൂന്യം
gui_mch_exit(പൂർണ്ണസംഖ്യ rc UNUSED)
{
#എങ്കിൽ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#endif
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_മൂല്യങ്ങൾ(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ശൂന്യ);
    *x = xpos;
    *y = ypos;
    തിരികെ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ശൂന്യം
gui_mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell,
	XtNx,	x,
	XtNy,	y,
	ശൂന്യ);
}

    ശൂന്യം
gui_mch_set_shellsize(
    പൂർണ്ണസംഖ്യ		width,
    പൂർണ്ണസംഖ്യ		height,
    പൂർണ്ണസംഖ്യ		min_width,
    പൂർണ്ണസംഖ്യ		min_height,
    പൂർണ്ണസംഖ്യ		base_width,
    പൂർണ്ണസംഖ്യ		base_height,
    പൂർണ്ണസംഖ്യ		direction UNUSED)
{
#ifdef FEAT_XIM
    height += xim_get_status_area_height(),
#endif
    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell,
	XtNwidthInc,	gui.char_width,
	XtNheightInc,	gui.char_height,
#എങ്കിൽ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	XtNbaseWidth,	base_width,
	XtNbaseHeight,	base_height,
#endif
	XtNminWidth,	min_width,
	XtNminHeight,	min_height,
	XtNwidth,	width,
	XtNheight,	height,
	ശൂന്യ);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    ശൂന്യം
gui_mch_get_screen_dimensions(
    പൂർണ്ണസംഖ്യ	    *screen_w,
    പൂർണ്ണസംഖ്യ	    *screen_h)
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
    പൂർണ്ണസംഖ്യ
gui_mch_init_font(
    char_u	*font_name,
    പൂർണ്ണസംഖ്യ		do_fontset UNUSED)
{
    XFontStruct	*font = ശൂന്യ;

#ifdef FEAT_XFONTSET
    XFontSet	fontset = ശൂന്യ;
#endif

#ifdef FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    എങ്കിൽ (font_name != ശൂന്യ && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	എങ്കിൽ (font_name == ശൂന്യ && !gui.starting)
	    തിരികെ OK;
    }
#endif

#ifdef FEAT_XFONTSET
    എങ്കിൽ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	എങ്കിൽ (font_name != ശൂന്യ)
	{
	    fontset = (XFontSet)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    എങ്കിൽ (fontset == ശൂന്യ)
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
#endif
    {
	എങ്കിൽ (font_name == ശൂന്യ)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    എങ്കിൽ (font == ശൂന്യ)
		font_name = (char_u *)DFLT_FONT;
	}
	എങ്കിൽ (font == ശൂന്യ)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	എങ്കിൽ (font == ശൂന്യ)
	    തിരികെ FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    എങ്കിൽ (fontset != ശൂന്യ)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_width = fontset_width(fontset);
	gui.char_height = fontset_height(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    അല്ലെങ്കിൽ
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
    എങ്കിൽ (font_name == gui.rsrc_font_name)
    {
	എങ്കിൽ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ശൂന്യ
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	എങ്കിൽ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ശൂന്യ
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	എങ്കിൽ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ശൂന്യ
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    അല്ലെങ്കിൽ
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	എങ്കിൽ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	എങ്കിൽ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	എങ്കിൽ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ifdef FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#endif

    തിരികെ OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, പൂർണ്ണസംഖ്യ giveErrorIfMissing)
{
    XFontStruct	*font;

    എങ്കിൽ (!gui.in_use || name == ശൂന്യ)	// can't do this when GUI not running
	തിരികെ NOFONT;

    font = XLoadQueryFont(gui.dpy, (അക്ഷരം *)name);

    എങ്കിൽ (font == ശൂന്യ)
    {
	എങ്കിൽ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	തിരികെ NOFONT;
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

    എങ്കിൽ (font->max_bounds.width != font->min_bounds.width)
    {
	semsg(_(e_font_str_is_not_fixed_width), name);
	XFreeFont(gui.dpy, font);
	തിരികെ NOFONT;
    }
    തിരികെ (GuiFont)font;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ശൂന്യ;

    എങ്കിൽ (name != ശൂന്യ && font == ശൂന്യ)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (font != ശൂന്യ)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	unsigned long മൂല്യം = 0L;

	എങ്കിൽ (XGetFontProperty(font, XA_FONT, &മൂല്യം))
	{
	    അക്ഷരം *xa_font_name = ശൂന്യ;

	    xa_font_name = XGetAtomName(gui.dpy, മൂല്യം);
	    എങ്കിൽ (xa_font_name != ശൂന്യ)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (name != ശൂന്യ)
		ret = vim_strsave(name);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (name != ശൂന്യ)
	    ret = vim_strsave(name);
    }
    തിരികെ ret;
}
#endif

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charheight(ശൂന്യം)
{
#ifdef FEAT_XFONTSET
    എങ്കിൽ (gui.fontset != NOFONTSET)
    {
	gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
							    + p_linespace / 2;
    }
    അല്ലെങ്കിൽ
#endif
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    തിരികെ OK;
}

/*
 * Set the current text font.
 */
    ശൂന്യം
gui_mch_set_font(GuiFont font)
{
    സ്ഥിതം Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    എങ്കിൽ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.text_gc, fid);
	XSetFont(gui.dpy, gui.back_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ifdef FEAT_XFONTSET
    current_fontset = (XFontSet)ശൂന്യ;
#endif
}

#എങ്കിൽ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    ശൂന്യം
gui_mch_set_fontset(GuiFontset fontset)
{
    current_fontset = (XFontSet)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    ശൂന്യം
gui_mch_free_font(GuiFont font)
{
    എങ്കിൽ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#എങ്കിൽ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    ശൂന്യം
gui_mch_free_fontset(GuiFontset fontset)
{
    എങ്കിൽ (fontset != NOFONTSET)
	XFreeFontSet(gui.dpy, (XFontSet)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		giveErrorIfMissing,
    പൂർണ്ണസംഖ്യ		fixed_width)
{
    XFontSet	fontset;
    അക്ഷരം	**missing, *def_str;
    പൂർണ്ണസംഖ്യ		അക്കം_missing;

    എങ്കിൽ (!gui.in_use || name == ശൂന്യ)
	തിരികെ NOFONTSET;

    fontset = XCreateFontSet(gui.dpy, (അക്ഷരം *)name, &missing, &അക്കം_missing,
			     &def_str);
    എങ്കിൽ (അക്കം_missing > 0)
    {
	പൂർണ്ണസംഖ്യ i;

	എങ്കിൽ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_in_fontset), name);
	    വേണ്ടി (i = 0; i < അക്കം_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    എങ്കിൽ (fontset == ശൂന്യ)
    {
	എങ്കിൽ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	തിരികെ NOFONTSET;
    }

    എങ്കിൽ (fixed_width && check_fontset_sanity(fontset) == FAIL)
    {
	XFreeFontSet(gui.dpy, fontset);
	തിരികെ NOFONTSET;
    }
    തിരികെ (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_fontset_sanity(XFontSet fs)
{
    XFontStruct	**xfs;
    അക്ഷരം	**font_name;
    പൂർണ്ണസംഖ്യ		fn;
    അക്ഷരം	*base_name;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		min_width;
    പൂർണ്ണസംഖ്യ		min_font_idx = 0;

    base_name = XBaseFontNameListOfFontSet(fs);
    fn = XFontsOfFontSet(fs, &xfs, &font_name);
    വേണ്ടി (i = 0; i < fn; i++)
    {
	എങ്കിൽ (xfs[i]->max_bounds.width != xfs[i]->min_bounds.width)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_width),
		    base_name, font_name[i]);
	    തിരികെ FAIL;
	}
    }
    // scan base font width
    min_width = 32767;
    വേണ്ടി (i = 0; i < fn; i++)
    {
	എങ്കിൽ (xfs[i]->max_bounds.width<min_width)
	{
	    min_width = xfs[i]->max_bounds.width;
	    min_font_idx = i;
	}
    }
    വേണ്ടി (i = 0; i < fn; i++)
    {
	എങ്കിൽ (	   xfs[i]->max_bounds.width != 2 * min_width
		&& xfs[i]->max_bounds.width != min_width)
	{
	    semsg(_(e_fontset_name_str), base_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (പൂർണ്ണസംഖ്യ)xfs[min_font_idx]->max_bounds.width);
	    semsg(_("Font%d width: %d"), i, (പൂർണ്ണസംഖ്യ)xfs[i]->max_bounds.width);
	    തിരികെ FAIL;
	}
    }
    // it seems ok. Good Luck!!
    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
fontset_width(XFontSet fs)
{
 തിരികെ XmbTextEscapement(fs, "Vim", 3) / 3;
}

    പൂർണ്ണസംഖ്യ
fontset_height(
    XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    തിരികെ extents->max_logical_extent.height;
}

# എങ്കിൽ 0
// NOT USED YET
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fontset_descent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet (fs);
    തിരികെ extents->max_logical_extent.height + extents->max_logical_extent.y;
}
# endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
fontset_ascent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    തിരികെ -extents->max_logical_extent.y;
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
    എങ്കിൽ (!gui.in_use || name == ശൂന്യ || *name == NUL)
	തിരികെ INVALCOLOR;

    requested = gui_get_color_cmn(name);
    എങ്കിൽ (requested == INVALCOLOR)
	തിരികെ INVALCOLOR;

    തിരികെ gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
    XColor	available;
    Colormap	colormap;

#എങ്കിൽ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    അക്ഷരം	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, വലുപ്പം(spec), "#%.2x%.2x%.2x", r, g, b);
    എങ്കിൽ (XParseColor(gui.dpy, colormap, (അക്ഷരം *)spec, &available) != 0
	    && XAllocColor(gui.dpy, colormap, &available) != 0)
	തിരികെ (guicolor_T)available.pixel;
#endif
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_FIELD(available);
    available.red = r << 8;
    available.green = g << 8;
    available.blue = b << 8;
    എങ്കിൽ (XAllocColor(gui.dpy, colormap, &available) != 0)
	തിരികെ (guicolor_T)available.pixel;

    തിരികെ INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    ശൂന്യം
gui_mch_set_fg_color(guicolor_T color)
{
    എങ്കിൽ (color == prev_fg_color)
	തിരികെ;

    XSetForeground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    ശൂന്യം
gui_mch_set_bg_color(guicolor_T color)
{
    എങ്കിൽ (color == prev_bg_color)
	തിരികെ;

    XSetBackground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    ശൂന്യം
gui_mch_set_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    സ്ഥിതം Cursor
gui_x11_create_blank_mouse(ശൂന്യം)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (unsigned long)0, (XGCValues*)0);

    എങ്കിൽ (gc != ശൂന്യ)
    {
	XDrawPoint(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    തിരികെ XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    സ്ഥിതം ശൂന്യം
draw_curl(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ cells)
{
    പൂർണ്ണസംഖ്യ			i;
    പൂർണ്ണസംഖ്യ			offset;
    സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
    വേണ്ടി (i = FILL_X(col); i < FILL_X(col + cells); ++i)
    {
	offset = val[i % 8];
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, i,
						FILL_Y(row + 1) - 1 - offset);
    }
    XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
}

    ശൂന്യം
gui_mch_draw_string(
    പൂർണ്ണസംഖ്യ		row,
    പൂർണ്ണസംഖ്യ		col,
    char_u	*s,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		flags)
{
    പൂർണ്ണസംഖ്യ			cells = len;
    സ്ഥിതം ശൂന്യം		*buf = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		buflen = 0;
    char_u		*p;
    പൂർണ്ണസംഖ്യ			wlen = 0;
    പൂർണ്ണസംഖ്യ			c;

    എങ്കിൽ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	എങ്കിൽ (buflen < len)
	{
	    XtFree((അക്ഷരം *)buf);
	    buf = (ശൂന്യം *)XtMalloc(len * (വലുപ്പം(XChar2b) < വലുപ്പം(wchar_t)
					? വലുപ്പം(wchar_t) : വലുപ്പം(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	ആകുമ്പോൾ (p < s + len)
	{
	    c = utf_ptr2char(p);
#ifdef FEAT_XFONTSET
	    എങ്കിൽ (current_fontset != ശൂന്യ)
	    {
# ifdef SMALL_WCHAR_T
		എങ്കിൽ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    അല്ലെങ്കിൽ
#endif
	    {
		എങ്കിൽ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (unsigned)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
    {
	cells = 0;
	വേണ്ടി (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ifdef FEAT_XFONTSET
    എങ്കിൽ (current_fontset != ശൂന്യ)
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

    എങ്കിൽ (flags & DRAW_TRANSP)
    {
	എങ്കിൽ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	അല്ലെങ്കിൽ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (അക്ഷരം *)s, len);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_linespace != 0
#ifdef FEAT_XFONTSET
	    || current_fontset != ശൂന്യ
#endif
	    )
    {
	XSetForeground(gui.dpy, gui.text_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		FILL_Y(row), gui.char_width * cells, gui.char_height);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);

	എങ്കിൽ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	അല്ലെങ്കിൽ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (അക്ഷരം *)s, len);
    }
    അല്ലെങ്കിൽ
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	എങ്കിൽ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	അല്ലെങ്കിൽ
	    XDrawImageString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (അക്ഷരം *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    എങ്കിൽ (flags & DRAW_BOLD)
    {
	എങ്കിൽ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), buf, wlen);
	അല്ലെങ്കിൽ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), (അക്ഷരം *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    എങ്കിൽ (flags & DRAW_UNDERC)
	draw_curl(row, col, cells);

    // Underline: draw a line at the bottom of the character cell.
    എങ്കിൽ (flags & DRAW_UNDERL)
    {
	പൂർണ്ണസംഖ്യ	y = FILL_Y(row + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	എങ്കിൽ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_വരി(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
    }

    എങ്കിൽ (flags & DRAW_STRIKE)
    {
	പൂർണ്ണസംഖ്യ	y = FILL_Y(row + 1) - gui.char_height/2;

	XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
	XDraw_വരി(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
    }

#ifdef FEAT_XFONTSET
    എങ്കിൽ (current_fontset != ശൂന്യ)
	XSetClipMask(gui.dpy, gui.text_gc, None);
#endif
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_haskey(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != (താക്കോൽ_Sym)0; i++)
	എങ്കിൽ (name[0] == special_താക്കോലുകൾ[i].vim_code0 &&
					 name[1] == special_താക്കോലുകൾ[i].vim_code1)
	    തിരികെ OK;
    തിരികെ FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    പൂർണ്ണസംഖ്യ
gui_get_x11_windis(Window *win, Display **dis)
{
    *win = XtWindow(vimShell);
    *dis = gui.dpy;
    തിരികെ OK;
}

    ശൂന്യം
gui_mch_beep(ശൂന്യം)
{
    XBell(gui.dpy, 0);
}

    ശൂന്യം
gui_mch_flash(പൂർണ്ണസംഖ്യ msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((പൂർണ്ണസംഖ്യ)Columns) + gui.border_offset,
	    FILL_Y((പൂർണ്ണസംഖ്യ)Rows) + gui.border_offset);
    XSync(gui.dpy, False);
    ui_delay((long)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((പൂർണ്ണസംഖ്യ)Columns) + gui.border_offset,
	    FILL_Y((പൂർണ്ണസംഖ്യ)Rows) + gui.border_offset);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ശൂന്യം
gui_mch_invert_rectangle(
    പൂർണ്ണസംഖ്യ	    r,
    പൂർണ്ണസംഖ്യ	    c,
    പൂർണ്ണസംഖ്യ	    nr,
    പൂർണ്ണസംഖ്യ	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_width, (nr) * gui.char_height);
}

/*
 * Iconify the GUI window.
 */
    ശൂന്യം
gui_mch_iconify(ശൂന്യം)
{
    XIconifyWindow(gui.dpy, XtWindow(vimShell), DefaultScreen(gui.dpy));
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ശൂന്യം
gui_mch_set_foreground(ശൂന്യം)
{
    XMapRaised(gui.dpy, XtWindow(vimShell));
}
#endif

/*
 * Draw a cursor without focus.
 */
    ശൂന്യം
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    പൂർണ്ണസംഖ്യ		w = 1;

    എങ്കിൽ (mb_lefthalve(gui.row, gui.col))
	w = 2;
    gui_mch_set_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(gui.col),
	    FILL_Y(gui.row), w * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ശൂന്യം
gui_mch_draw_part_cursor(പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h, guicolor_T color)
{
    gui_mch_set_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.text_gc,
#ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_വലത് ? FILL_X(gui.col + 1) - w :
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
    ശൂന്യം
gui_mch_update(ശൂന്യം)
{
    Xt_നിവേശം_Mask mask, desired;

#ifdef ALT_X_നിവേശം
    എങ്കിൽ (suppress_alternate_നിവേശം)
	desired = (XtIMXEvent | XtIMTimer);
    അല്ലെങ്കിൽ
#endif
	desired = (XtIMAll);
    ആകുമ്പോൾ ((mask = XtAppPending(app_context)) && (mask & desired)
	    && !vim_is_നിവേശം_buf_full())
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
    പൂർണ്ണസംഖ്യ
gui_mch_wait_for_chars(long wtime)
{
    പൂർണ്ണസംഖ്യ	    focus;
    പൂർണ്ണസംഖ്യ	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    timed_out;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_നിവേശം_Mask	    desired;
#ifdef FEAT_JOB_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#endif

    timed_out = FALSE;

    എങ്കിൽ (wtime >= 0)
	timer = XtAppAddTimeOut(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_out);
#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    എങ്കിൽ (channel_any_keep_open())
	channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
				   channel_poll_cb, (XtPointer)&channel_timer);
#endif

    focus = gui.in_focus;
    desired = (XtIMAll);
    ആകുമ്പോൾ (!timed_out)
    {
	// Stop or start blinking when focus changes
	എങ്കിൽ (gui.in_focus != focus)
	{
	    എങ്കിൽ (gui.in_focus)
		gui_mch_start_blink();
	    അല്ലെങ്കിൽ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	എങ്കിൽ (did_add_timer)
	    // Need to recompute the waiting time.
	    നിർത്തുക;
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

	എങ്കിൽ (നിവേശം_available())
	{
	    retval = OK;
	    നിർത്തുക;
	}
    }

    എങ്കിൽ (timer != (XtIntervalId)0 && !timed_out)
	XtRemoveTimeOut(timer);
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (channel_timer != (XtIntervalId)0)
	XtRemoveTimeOut(channel_timer);
#endif

    തിരികെ retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    ശൂന്യം
gui_mch_flush(ശൂന്യം)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ശൂന്യം
gui_mch_clear_block(
    പൂർണ്ണസംഖ്യ		row1,
    പൂർണ്ണസംഖ്യ		col1,
    പൂർണ്ണസംഖ്യ		row2,
    പൂർണ്ണസംഖ്യ		col2)
{
    പൂർണ്ണസംഖ്യ		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.back_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height);
}

    ശൂന്യം
gui_mch_clear_all(ശൂന്യം)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ശൂന്യം
gui_mch_delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    എങ്കിൽ (gui.visibility == VisibilityFullyObscured)
	തിരികെ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row + അക്കം_വരികൾ),
	gui.char_width * (gui.scroll_region_വലത് - gui.scroll_region_ഇടത് + 1)
			       + (gui.scroll_region_വലത് == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - അക്കം_വരികൾ + 1),
	FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row));

    gui_clear_block(gui.scroll_region_bot - അക്കം_വരികൾ + 1,
						       gui.scroll_region_ഇടത്,
			  gui.scroll_region_bot, gui.scroll_region_വലത്);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ശൂന്യം
gui_mch_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
    എങ്കിൽ (gui.visibility == VisibilityFullyObscured)
	തിരികെ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	gui.char_width * (gui.scroll_region_വലത് - gui.scroll_region_ഇടത് + 1)
			       + (gui.scroll_region_വലത് == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - അക്കം_വരികൾ + 1),
	FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row + അക്കം_വരികൾ));

    gui_clear_block(row, gui.scroll_region_ഇടത്,
				row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    സ്ഥിതം ശൂന്യം
gui_x11_check_copy_area(ശൂന്യം)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    എങ്കിൽ (gui.visibility != VisibilityPartiallyObscured)
	തിരികെ;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    വേണ്ടി (;;)
    {
	എങ്കിൽ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    തിരികെ;	// The scroll worked.

	എങ്കിൽ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    എങ്കിൽ (gevent->എണ്ണം == 0)
		തിരികെ;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    ശൂന്യം
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    പൂർണ്ണസംഖ്യ
clip_mch_own_selection(Clipboard_T *cbd)
{
    തിരികെ clip_x11_own_selection(vimShell, cbd);
}

    ശൂന്യം
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    ശൂന്യം
clip_mch_set_selection(
    Clipboard_T	*cbd)
{
    clip_x11_set_selection(cbd);
}

#എങ്കിൽ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    ശൂന്യം
gui_mch_menu_grey(vimmenu_T *menu, പൂർണ്ണസംഖ്യ grey)
{
    എങ്കിൽ (menu->id == (Widget)0)
	തിരികെ;

    gui_mch_menu_hidden(menu, False);
    എങ്കിൽ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	XtSetSensitive(menu->id, False);
    അല്ലെങ്കിൽ
	XtSetSensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    ശൂന്യം
gui_mch_menu_hidden(vimmenu_T *menu, പൂർണ്ണസംഖ്യ hidden)
{
    എങ്കിൽ (menu->id == (Widget)0)
	തിരികെ;

    എങ്കിൽ (hidden)
	XtUnmanageChild(menu->id);
    അല്ലെങ്കിൽ
	XtManageChild(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ശൂന്യം
gui_mch_draw_menubar(ശൂന്യം)
{
    // Nothing to do in X
}

    ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
gui_x11_wm_protocol_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    /*
     * Only deal with Client messages.
     */
    എങ്കിൽ (event->type != ClientMessage)
	തിരികെ;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    എങ്കിൽ ((Atom)((XClientMessageEvent *)event)->data.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	out_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, XtWindow(vimShell), ശൂന്യ, 0);
	തിരികെ;
    }

    എങ്കിൽ ((Atom)((XClientMessageEvent *)event)->data.l[0] !=
						  wm_atoms[DELETE_WINDOW_IDX])
	തിരികെ;

    gui_shell_closed();
}

#ifdef FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    സ്ഥിതം ശൂന്യം
gui_x11_send_event_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    എങ്കിൽ (e->type == PropertyNotify && e->window == commWindow
	    && e->atom == commProperty &&  e->state == Property_New_മൂല്യം)
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

സ്ഥിതം പൂർണ്ണസംഖ്യ		blink_state = BLINK_NONE;
സ്ഥിതം long_u		blink_waittime = 700;
സ്ഥിതം long_u		blink_ontime = 400;
സ്ഥിതം long_u		blink_offtime = 250;
സ്ഥിതം XtIntervalId	blink_timer = (XtIntervalId)0;

    പൂർണ്ണസംഖ്യ
gui_mch_is_blinking(ശൂന്യം)
{
    തിരികെ blink_state != BLINK_NONE;
}

    പൂർണ്ണസംഖ്യ
gui_mch_is_blink_off(ശൂന്യം)
{
    തിരികെ blink_state == BLINK_OFF;
}

    ശൂന്യം
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ശൂന്യം
gui_mch_stop_blink(പൂർണ്ണസംഖ്യ may_call_gui_update_cursor)
{
    എങ്കിൽ (blink_timer != (XtIntervalId)0)
    {
	XtRemoveTimeOut(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    എങ്കിൽ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    സ്ഥിതം ശൂന്യം
gui_x11_blink_cb(
    XtPointer	    timed_out UNUSED,
    XtIntervalId    *interval_id UNUSED)
{
    എങ്കിൽ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = XtAppAddTimeOut(app_context, blink_offtime,
						      gui_x11_blink_cb, ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = XtAppAddTimeOut(app_context, blink_ontime,
						      gui_x11_blink_cb, ശൂന്യ);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ശൂന്യം
gui_mch_start_blink(ശൂന്യം)
{
    എങ്കിൽ (blink_timer != (XtIntervalId)0)
	XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    എങ്കിൽ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = XtAppAddTimeOut(app_context, blink_waittime,
						      gui_x11_blink_cb, ശൂന്യ);
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

    തിരികെ (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((unsigned)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    ശൂന്യം
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
    എങ്കിൽ (vim_strchr(p_go, GO_POINTER) != ശൂന്യ)
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

    XtAddEventHandler(vimForm, താക്കോൽ_Press_Mask, FALSE, gui_x11_താക്കോൽ_hit_cb,
	(XtPointer)0);
    XtAddEventHandler(textArea, താക്കോൽ_Press_Mask, FALSE, gui_x11_താക്കോൽ_hit_cb,
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
    ശൂന്യം
gui_mch_getmouse(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    പൂർണ്ണസംഖ്യ		rootx, rooty, winx, winy;
    Window	വേര്, child;
    unsigned പൂർണ്ണസംഖ്യ mask;

    എങ്കിൽ (gui.wid && XQueryPointer(gui.dpy, gui.wid, &വേര്, &child,
					 &rootx, &rooty, &winx, &winy, &mask))
    {
	*x = winx;
	*y = winy;
    }
    അല്ലെങ്കിൽ
    {
	*x = -1;
	*y = -1;
    }
}

    ശൂന്യം
gui_mch_setmouse(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    എങ്കിൽ (gui.wid)
	XWarpPointer(gui.dpy, (Window)0, gui.wid, 0, 0, 0, 0, x, y);
}

#എങ്കിൽ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_last_mouse_event(ശൂന്യം)
{
    തിരികെ &last_mouse_event;
}
#endif

#എങ്കിൽ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_WIDTH (gui.char_width * 2)

    ശൂന്യം
gui_mch_drawsign(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ typenr)
{
    XImage	*sign;

    എങ്കിൽ (!gui.in_use || (sign = (XImage *)sign_get_image(typenr)) == ശൂന്യ)
	തിരികെ;

    XClearArea(gui.dpy, gui.wid, TEXT_X(col), TEXT_Y(row) - sign->height,
	    SIGN_WIDTH, gui.char_height, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.text_gc, sign, 0, 0,
	    TEXT_X(col) + (SIGN_WIDTH - sign->width) / 2,
	    TEXT_Y(row) - sign->height,
	    sign->width, sign->height);
}

    ശൂന്യം *
gui_mch_register_sign(char_u *signfile)
{
    XpmAttributes   attrs;
    XImage	    *sign = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    status;

    /*
     * Setup the color substitution table.
     */
    എങ്കിൽ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_ചിഹ്നം color[5] =
	{
	    {"none", ശൂന്യ, 0},
	    {"iconColor1", ശൂന്യ, 0},
	    {"bottomShadowColor", ശൂന്യ, 0},
	    {"topShadowColor", ശൂന്യ, 0},
	    {"selectColor", ശൂന്യ, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.back_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	status = XpmReadFileToImage(gui.dpy, (അക്ഷരം *)signfile,
							 &sign, ശൂന്യ, &attrs);
	എങ്കിൽ (status == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	അല്ലെങ്കിൽ
	    emsg(_(e_couldnt_read_in_sign_data));
    }

    തിരികെ (ശൂന്യം *)sign;
}

    ശൂന്യം
gui_mch_destroy_sign(ശൂന്യം *sign)
{
    XDestroyImage((XImage*)sign);
}
#endif


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
സ്ഥിതം പൂർണ്ണസംഖ്യ last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 */
    ശൂന്യം
gui_mch_mousehide(
    പൂർണ്ണസംഖ്യ		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    എങ്കിൽ (gui.pointer_hidden == hide)
	തിരികെ;

    gui.pointer_hidden = hide;
    എങ്കിൽ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    അല്ലെങ്കിൽ
#ifdef FEAT_MOUSESHAPE
	mch_set_mouse_shape(last_shape);
#അല്ലെങ്കിൽ
    XUndefineCursor(gui.dpy, gui.wid);
#endif
}

#എങ്കിൽ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
സ്ഥിതം പൂർണ്ണസംഖ്യ mshape_ids[] =
{
    XC_ഇടത്_ptr,		// arrow
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
    XC_വലത്_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ഇടത്_ptr			// last one
};

    ശൂന്യം
mch_set_mouse_shape(പൂർണ്ണസംഖ്യ shape)
{
    പൂർണ്ണസംഖ്യ	    id;

    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    എങ്കിൽ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    എങ്കിൽ (id >= XC_അക്കം_glyphs)
		id = XC_ഇടത്_ptr;
	    അല്ലെങ്കിൽ
		id &= ~1;	// they are always even (why?)
	}
	അല്ലെങ്കിൽ
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    എങ്കിൽ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

#എങ്കിൽ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    ശൂന്യം
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    എങ്കിൽ (menu->id == ശൂന്യ || menu->parent == ശൂന്യ
				|| !menu_is_toolbar(menu->parent->name))
	തിരികെ;

    // Always destroy and create the balloon, in case the string was
    // changed.
    എങ്കിൽ (menu->tip != ശൂന്യ)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ശൂന്യ;
    }
    എങ്കിൽ (menu->strings[MENU_INDEX_TIP] != ശൂന്യ)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->strings[MENU_INDEX_TIP],
		ശൂന്യ,
		ശൂന്യ);
}
#endif
