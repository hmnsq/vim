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
#ಯದಿ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#ಅನ್ಯಥಾ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ಅನ್ಯಥಾ
#   include <X11/xpm.h>
#  endif
# endif
#endif

#ifdef FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# ಅನ್ಯಥಾ
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
#ಅನ್ಯಥಾ
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

ಸ್ಥಿರ Atom   wm_atoms[2];	// Window Manager Atoms
#define DELETE_WINDOW_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#define SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ifdef FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
ಸ್ಥಿರ XFontSet current_fontset = ರಿಕ್ತ;
# ಯದಿ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	ಕುರು \
	{ \
	    ಯದಿ (current_fontset != ರಿಕ್ತ) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    ಅನ್ಯಥಾ \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} ಯಾವತ್ (0)
# endif
# ಯದಿ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	ಕುರು \
	{ \
	    ಯದಿ (current_fontset != ರಿಕ್ತ) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ಅನ್ಯಥಾ \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ಯಾವತ್ (0)
# endif
# ಯದಿ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	ಕುರು \
	{ \
	    ಯದಿ (current_fontset != ರಿಕ್ತ) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    ಅನ್ಯಥಾ \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} ಯಾವತ್ (0)
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ check_fontset_sanity(XFontSet fs);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fontset_width(XFontSet fs);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fontset_ascent(XFontSet fs);
#endif

ಸ್ಥಿರ guicolor_T	prev_fg_color = INVALCOLOR;
ಸ್ಥಿರ guicolor_T	prev_bg_color = INVALCOLOR;
ಸ್ಥಿರ guicolor_T	prev_sp_color = INVALCOLOR;

#ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
ಸ್ಥಿರ XButtonPressedEvent last_mouse_event;
#endif

ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_check_copy_area(ಶೂನ್ಯ);
#ifdef FEAT_CLIENTSERVER
ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_send_event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ gui_x11_wm_protocol_handler(Widget, XtPointer, XEvent *, Boolean *);
ಸ್ಥಿರ Cursor gui_x11_create_blank_mouse(ಶೂನ್ಯ);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
ಸ್ಥಿರ ಸಂರಚನಾ specialkey
{
    ಕೀಲಿ_Sym  ಕೀಲಿ_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ಕೀಲಿಗಳು[] =
{
    {XK_Up,		'k', 'u'},
    {XK_Down,		'k', 'd'},
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
#ifdef SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#endif

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_BackSpace,	'k', 'b'},
    {XK_ಸೇರಿಸು,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_End,		'@', '7'},
    {XK_Prior,		'k', 'P'},
    {XK_Next,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ifdef XK_KP_ಎಡ
    {XK_KP_ಎಡ,	'k', 'l'},
    {XK_KP_ಬಲ,	'k', 'r'},
    {XK_KP_Up,		'k', 'u'},
    {XK_KP_Down,	'k', 'd'},
    {XK_KP_ಸೇರಿಸು,	KS_EXTRA, (char_u)KE_KINS},
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
    {(ಕೀಲಿ_Sym)0,	    0, 0}
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
ಸ್ಥಿರ XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	ಪರಿಮಾಣ(Pixel),
	XtOffsetOf(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	ಪರಿಮಾಣ(Pixel),
	XtOffsetOf(gui_T, def_back_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	ಪರಿಮಾಣ(String *),
	XtOffsetOf(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	ಪರಿಮಾಣ(String *),
	XtOffsetOf(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	ಪರಿಮಾಣ(String *),
	XtOffsetOf(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	ಪರಿಮಾಣ(String *),
	XtOffsetOf(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	ಪರಿಮಾಣ(String *),
	XtOffsetOf(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	ಪರಿಮಾಣ(Bool),
	XtOffsetOf(gui_T, rsrc_rev_video),
	XtRImmediate,
	(XtPointer)False
    },
    {
	XtNborderWidth,
	XtCBorderWidth,
	XtRInt,
	ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ),
	XtOffsetOf(gui_T, border_width),
	XtRImmediate,
	(XtPointer)2
    },
    {
	XtNscrollbarWidth,
	XtCScrollbarWidth,
	XtRInt,
	ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ),
	XtOffsetOf(gui_T, scrollbar_width),
	XtRImmediate,
	(XtPointer)SB_DEFAULT_WIDTH
    },
#ifdef FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	XtNmenuFontSet,
	XtCMenuFontSet,
# ಅನ್ಯಥಾ
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#endif
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ifdef FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ *),
	XtOffsetOf(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	XtCFontSet,
	XtRFontSet,
	ಪರಿಮಾಣ(XFontSet),
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
	ಪರಿಮಾಣ(ವರ್ಣ*),
	XtOffsetOf(gui_T, rsrc_preedit_type_name),
	XtRString,
	(XtPointer)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	ಪರಿಮಾಣ(ವರ್ಣ*),
	XtOffsetOf(gui_T, rsrc_ಒಳಹರಿವು_method),
	XtRString,
	ರಿಕ್ತ
    },
#endif // FEAT_XIM
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
    {"-bg",		".background",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-background",	".background",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-fn",		".font",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-font",		".font",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ರಿಕ್ತ},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ರಿಕ್ತ},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ರಿಕ್ತ},
#ifdef FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ರಿಕ್ತ},
#ಅನ್ಯಥಾ
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ರಿಕ್ತ},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ರಿಕ್ತ},
#endif
    {"-xrm",		ರಿಕ್ತ,		    XrmoptionResArg,	ರಿಕ್ತ}
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gui_argc = 0;
ಸ್ಥಿರ ವರ್ಣ **gui_argv = ರಿಕ್ತ;

/*
 * Call-back routines.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_timer_cb(
    XtPointer	    timed_out,
    XtIntervalId    *interval_id UNUSED)
{
    *((ಪೂರ್ಣಾಙ್ಕ *)timed_out) = TRUE;
}

#ifdef FEAT_JOB_CHANNEL
    ಸ್ಥಿರ ಶೂನ್ಯ
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

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_visibility_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ಯದಿ (event->type != VisibilityNotify)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_expose_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XExposeEvent	*gevent;
    ಪೂರ್ಣಾಙ್ಕ			new_x;

    ಯದಿ (event->type != Expose)
	ಪ್ರತ್ಯಾಗಮ;

    out_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    ಯದಿ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    ಯದಿ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    ಯದಿ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((ಪೂರ್ಣಾಙ್ಕ)Columns), 0, 0, 0, False);
    ಯದಿ (gevent->y > FILL_Y(Rows))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((ಪೂರ್ಣಾಙ್ಕ)Rows), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#ಯದಿ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    Window		rootw, shellw, child, parentw;
    ಪೂರ್ಣಾಙ್ಕ			absx, absy;
    XWindowAttributes	a;
    Window		*children;
    unsigned ಪೂರ್ಣಾಙ್ಕ	children_ಎಣಿಕೆ;

    shellw = XtWindow(shell);
    ಯದಿ (shellw == 0)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (;;)
    {
	XQuery_ವೃಕ್ಷ(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_ಎಣಿಕೆ);
	XFree(children);
	ಯದಿ (parentw == rootw)
	    ಭಙ್ಗ;
	shellw = parentw;
    }
    XGetWindowAttributes(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.ಬೇರು, 0, 0,
							&absx, &absy, &child);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(shell, XmNheight, &r->height, XmNwidth, &r->width, ರಿಕ್ತ);
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_resize_window_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lastWidth, lastHeight;

    ಯದಿ (event->type != ConfigureNotify)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (event->xconfigure.width != lastWidth
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
#ಯದಿ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    ಯದಿ (netbeans_active())
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

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_focus_change_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    gui_focus_change(event->type == FocusIn);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_enter_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(TRUE);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_leave_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(FALSE);
}

#ಯದಿ defined(X_HAVE_UTF8_STRING)
# ಯದಿ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#endif

    ಶೂನ್ಯ
gui_x11_ಕೀಲಿ_hit_cb(
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
#ಅನ್ಯಥಾ
    char_u		string[4], string2[3];
#endif
    ಕೀಲಿ_Sym		ಕೀಲಿ_sym;
    ಪೂರ್ಣಾಙ್ಕ			len;
    ಪೂರ್ಣಾಙ್ಕ			i;
    ಪೂರ್ಣಾಙ್ಕ			modifiers;
    ಪೂರ್ಣಾಙ್ಕ			ಕೀಲಿ;

    ev_press = (XKeyPressedEvent *)event;

#ifdef FEAT_XIM
    ಯದಿ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	ಯದಿ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (ವರ್ಣ *)string,
				  ಪರಿಮಾಣ(string_shortbuf), &ಕೀಲಿ_sym, &status);
	ಅನ್ಯಥಾ
# endif
	    len = XmbLookupString(xic, ev_press, (ವರ್ಣ *)string,
				  ಪರಿಮಾಣ(string_shortbuf), &ಕೀಲಿ_sym, &status);
	ಯದಿ (status == XBufferOverflow)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    ಯದಿ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (ವರ್ಣ *)string,
						      len, &ಕೀಲಿ_sym, &status);
	    ಅನ್ಯಥಾ
# endif
		len = XmbLookupString(xic, ev_press, (ವರ್ಣ *)string,
						      len, &ಕೀಲಿ_sym, &status);
	}
	ಯದಿ (status == XLookupNone || status == XLookupChars)
	    ಕೀಲಿ_sym = XK_Void_ಚಿಹ್ನೆ;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	ಯದಿ (len > 0 && ಒಳಹರಿವು_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    ಪೂರ್ಣಾಙ್ಕ		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    ಯದಿ (string_alloced)
		XtFree((ವರ್ಣ *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_ಒಳಹರಿವು(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ಯದಿ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		ಯದಿ (string_alloced)
		    XtFree((ವರ್ಣ *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    ಅನ್ಯಥಾ
#endif
	len = XLookupString(ev_press, (ವರ್ಣ *)string, ಪರಿಮಾಣ(string),
		&ಕೀಲಿ_sym, ರಿಕ್ತ);

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    ಯದಿ (ಕೀಲಿ_sym == SunXK_F36 || ಕೀಲಿ_sym == SunXK_F37)
	len = 0;
#endif

    ಯದಿ (ಕೀಲಿ_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    ಯದಿ (ಕೀಲಿ_sym == XK_minus && (ev_press->state & ControlMask))
	string[0] = Ctrl__;

#ifdef XK_ISO_ಎಡ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    ಯದಿ (ಕೀಲಿ_sym == XK_ISO_ಎಡ_Tab)
    {
	ಕೀಲಿ_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    ಯದಿ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ಯದಿ (len == 0 || len == 1)
    {
	ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != (ಕೀಲಿ_Sym)0; i++)
	{
	    ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ಕೀಲಿಗಳು[i].vim_code0;
		string[2] = special_ಕೀಲಿಗಳು[i].vim_code1;
		len = -3;
		ಭಙ್ಗ;
	    }
	}
    }

    // Unrecognised key is ignored.
    ಯದಿ (len == 0)
	ಗಚ್ಛ theend;

    // Handle modifiers.
    modifiers = 0;
    ಯದಿ (ev_press->state & ShiftMask)
	modifiers |= MOD_MASK_SHIFT;
    ಯದಿ (ev_press->state & ControlMask)
    {
	modifiers |= MOD_MASK_CTRL;
	ಯದಿ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    ಯದಿ (ev_press->state & Mod1Mask)
	modifiers |= MOD_MASK_ALT;
    ಯದಿ (ev_press->state & Mod4Mask)
	modifiers |= MOD_MASK_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    ಯದಿ (len == -3)
	ಕೀಲಿ = TO_SPECIAL(string[1], string[2]);
    ಅನ್ಯಥಾ
    {
	string[len] = NUL;
	ಕೀಲಿ = mb_ptr2char(string);
    }
    ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
    ಯದಿ (ಕೀಲಿ == CSI)
	ಕೀಲಿ = K_CSI;
    ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ಕೀಲಿ);
	string[2] = K_THIRD(ಕೀಲಿ);
	len = 3;
    }
    ಅನ್ಯಥಾ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);

	len = mb_char2bytes(ಕೀಲಿ, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_shift_modifier(modifiers, ಕೀಲಿ);
    }

    ಯದಿ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_ಒಳಹರಿವು_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	ಪೂರ್ಣಾಙ್ಕ int_ch = check_for_interrupt(ಕೀಲಿ, modifiers);

	ಯದಿ (int_ch != NUL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_ಒಳಹರಿವು_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    ಯದಿ (p_mh)
	gui_mch_mousehide(TRUE);

#ಯದಿ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	ಯದಿ ((be = gui_mch_currently_showing_beval()) != ರಿಕ್ತ)
	    gui_mch_unpost_balloon(be);
    }
#endif
theend:
    {}	    // some compilers need a statement here
#ifdef FEAT_XIM
    ಯದಿ (string_alloced)
	XtFree((ವರ್ಣ *)string);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_mouse_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    ಸ್ಥಿರ XtIntervalId timer = (XtIntervalId)0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	timed_out = TRUE;

    ಪೂರ್ಣಾಙ್ಕ		button;
    ಪೂರ್ಣಾಙ್ಕ		repeated_click = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    ಯದಿ (event->type == MotionNotify)
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

	ಯದಿ (button != MOUSE_DRAG)	// just moving the rodent
	{
#ifdef FEAT_MENU
	    ಯದಿ (dud)			// moved in vimForm
		y -= gui.menu_height;
#endif
	    gui_mouse_moved(x, y);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	ಯದಿ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    ಯದಿ (!timed_out)
	    {
		XtRemoveTimeOut(timer);
		repeated_click = TRUE;
	    }
	    timed_out = FALSE;
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_out);
	    ವಿಕಲ್ಪ (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		ಪಕ್ಷ Button1:	button = MOUSE_ಎಡ;	ಭಙ್ಗ;
		ಪಕ್ಷ Button2:	button = MOUSE_ಮಧ್ಯ;	ಭಙ್ಗ;
		ಪಕ್ಷ Button3:	button = MOUSE_ಬಲ;	ಭಙ್ಗ;
		ಪಕ್ಷ Button4:	button = MOUSE_4;	ಭಙ್ಗ;
		ಪಕ್ಷ Button5:	button = MOUSE_5;	ಭಙ್ಗ;
		ಪಕ್ಷ 6:		button = MOUSE_7;	ಭಙ್ಗ;
		ಪಕ್ಷ 7:		button = MOUSE_6;	ಭಙ್ಗ;
		ಪಕ್ಷ 8:		button = MOUSE_X1;	ಭಙ್ಗ;
		ಪಕ್ಷ 9:		button = MOUSE_X2;	ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    ಪ್ರತ್ಯಾಗಮ;	// Unknown button
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	last_mouse_event = event->xbutton;
#endif
    }

    vim_modifiers = 0x0;
    ಯದಿ (x_modifiers & ShiftMask)
	vim_modifiers |= MOUSE_SHIFT;
    ಯದಿ (x_modifiers & ControlMask)
	vim_modifiers |= MOUSE_CTRL;
    ಯದಿ (x_modifiers & Mod1Mask)	    // Alt or Meta key
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
gui_mch_prepare(ಪೂರ್ಣಾಙ್ಕ *argc, ವರ್ಣ **argv)
{
    ಪೂರ್ಣಾಙ್ಕ	    arg;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(ವರ್ಣ *, *argc);
    ಯದಿ (gui_argv == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    ಯಾವತ್ (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)Xt_ಸಂಖ್ಯೆ(cmdline_options); i++)
	    ಯದಿ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		ಭಙ್ಗ;

	ಯದಿ (i < (ಪೂರ್ಣಾಙ್ಕ)Xt_ಸಂಖ್ಯೆ(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    ಯದಿ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		found_reverse_arg = TRUE;
	    ಅನ್ಯಥಾ ಯದಿ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_argument = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    ಯದಿ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * ಪರಿಮಾಣ(ವರ್ಣ *));
		ಯದಿ (cmdline_options[i].argKind != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    ಯದಿ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * ಪರಿಮಾಣ(ವರ್ಣ *));
		}
	    }
	    argv[*argc] = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
#ifdef FEAT_NETBEANS_INTG
	    ಯದಿ (strncmp("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * ಪರಿಮಾಣ(ವರ್ಣ *));
	    argv[*argc] = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
#endif
	    arg++;
    }
}

#ifndef XtSpecificationRelease
# define CARDINAL (Cardinal *)
#ಅನ್ಯಥಾ
# ಯದಿ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# ಅನ್ಯಥಾ
#  define CARDINAL (ಪೂರ್ಣಾಙ್ಕ *)
# endif
#endif

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
#ifdef FEAT_XIM
    XtSetLanguageProc(ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#endif
    open_app_context();
    ಯದಿ (app_context != ರಿಕ್ತ)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_ಸಂಖ್ಯೆ(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#ಯದಿ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	ವರ್ಣ *p = setlocale(LC_NUMERIC, ರಿಕ್ತ);

	ಯದಿ (p == ರಿಕ್ತ || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#endif
    ಯದಿ (app_context == ರಿಕ್ತ || gui.dpy == ರಿಕ್ತ)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}


#ifdef USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
ಸ್ಥಿರ Xt_ಒಳಹರಿವು_Id _xsmp_xtinputid;

    ಸ್ಥಿರ ಶೂನ್ಯ
local_xsmp_handle_requests(
    XtPointer	c UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*s UNUSED,
    Xt_ಒಳಹರಿವು_Id	*i UNUSED)
{
    ಯದಿ (xsmp_handle_requests() == FAIL)
	Xt_Remove_ಒಳಹರಿವು(_xsmp_xtinputid);
}
#endif


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init(ಶೂನ್ಯ)
{
    XtGCMask	gc_mask;
    XGCValues	gc_vals;
    ಪೂರ್ಣಾಙ್ಕ		x, y, mask;
    unsigned	w, h;

#ಯದಿ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#endif

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ರಿಕ್ತ);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (XtPointer)&gui,
	vim_resources, Xt_ಸಂಖ್ಯೆ(vim_resources), ರಿಕ್ತ);

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
    ಯದಿ (gui.rsrc_rev_video && gui_get_lightness(gui.back_pixel)
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

    ಯದಿ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    ಯದಿ (gui.geom != ರಿಕ್ತ && *gui.geom != NUL)
    {
	mask = XParseGeometry((ವರ್ಣ *)gui.geom, &x, &y, &w, &h);
	ಯದಿ (mask & Width_ಮೌಲ್ಯ)
	    Columns = w;
	ಯದಿ (mask & Height_ಮೌಲ್ಯ)
	{
	    ಯದಿ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	ಯದಿ (mask & (XValue|YValue))
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell, XtNgeometry, gui.geom, ರಿಕ್ತ);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    ಯದಿ (vim_strchr(p_go, GO_ICON) != ರಿಕ್ತ)
    {
#ifndef HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	XtSetArg(arg[0], XtNiconPixmap,
		XCreateBitmapFromData(gui.dpy,
		    Default_ಬೇರು_Window(gui.dpy),
		    (ವರ್ಣ *)vim_icon_bits,
		    vim_icon_width,
		    vim_icon_height));
	XtSetArg(arg[1], XtNiconMask,
		XCreateBitmapFromData(gui.dpy,
		    Default_ಬೇರು_Window(gui.dpy),
		    (ವರ್ಣ *)vim_mask_icon_bits,
		    vim_mask_icon_width,
		    vim_mask_icon_height));
	Xt_Set_ಮೌಲ್ಯಗಳು(vimShell, arg, (Cardinal)2);
#ಅನ್ಯಥಾ
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    ಸ್ಥಿರ Pixmap	icon = 0;
    ಸ್ಥಿರ Pixmap	icon_mask = 0;
    ಸ್ಥಿರ ವರ್ಣ		**magick = vim32x32;
    Window		ಬೇರು_window;
    XIconSize		*size;
    ಪೂರ್ಣಾಙ್ಕ			ಸಂಖ್ಯೆ_sizes;
    Display		*dsp;
    Screen		*scr;
    XpmAttributes	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    ಬೇರು_window = XRootWindowOfScreen(XtScreen(vimShell));
    ಯದಿ (XGetIconSizes(XtDisplay(vimShell), ಬೇರು_window,
						   &size, &ಸಂಖ್ಯೆ_sizes) != 0)
    {
	ಯದಿ (ಸಂಖ್ಯೆ_sizes > 0)
	{
	    ಯದಿ (size->max_height >= 48 && size->max_width >= 48)
		magick = vim48x48;
	    ಅನ್ಯಥಾ ಯದಿ (size->max_height >= 32 && size->max_width >= 32)
		magick = vim32x32;
	    ಅನ್ಯಥಾ ಯದಿ (size->max_height >= 16 && size->max_width >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell, XtNcolormap, cmap, ರಿಕ್ತ);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.depth = DefaultDepthOfScreen(scr);

    ಯದಿ (!icon)
    {
	XpmCreatePixmapFromData(dsp, ಬೇರು_window, magick, &icon,
							   &icon_mask, &attr);
	XpmFreeAttributes(&attr);
    }

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell, XmNiconPixmap, icon, XmNiconMask, icon_mask, ರಿಕ್ತ);
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
    ಯದಿ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_Add_ಒಳಹರಿವು(app_context, xsmp_icefd,
		(XtPointer)Xt_ಒಳಹರಿವು_Read_Mask, local_xsmp_handle_requests, ರಿಕ್ತ);
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    ಶೂನ್ಯ
gui_mch_uninit(ಶೂನ್ಯ)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ರಿಕ್ತ;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    long_u	gc_mask;
    XGCValues	gc_vals;

    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    ಯದಿ (gui.text_gc != ರಿಕ್ತ)
	XChangeGC(gui.dpy, gui.text_gc, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    ಯದಿ (gui.back_gc != ರಿಕ್ತ)
	XChangeGC(gui.dpy, gui.back_gc, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    ಯದಿ (gui.invert_gc != ರಿಕ್ತ)
	XChangeGC(gui.dpy, gui.invert_gc, gc_mask, &gc_vals);

    gui_x11_set_back_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_open(ಶೂನ್ಯ)
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
							     ರಿಕ್ತ);
#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(vimShell, (EventMask)0, True, _XEditResCheckMessages,
	    (XtPointer)ರಿಕ್ತ);
#endif

#ifdef FEAT_CLIENTSERVER
    ಯದಿ (serverName == ರಿಕ್ತ && serverDelayedStartName != ರಿಕ್ತ)
    {
	// This is a :gui command in a plain vim with no previous server
	commWindow = XtWindow(vimShell);
	(ಶೂನ್ಯ)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    ಅನ್ಯಥಾ
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(gui.dpy, XtWindow(vimShell));
    }
    XtAddEventHandler(vimShell, PropertyChangeMask, False,
		      gui_x11_send_event_handler, ರಿಕ್ತ);
#endif

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ifdef FEAT_XIM
    xim_init();
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    ಶೂನ್ಯ
gui_init_tooltip_font(ಶೂನ್ಯ)
{
    Xrm_ಮೌಲ್ಯ from, to;

    from.addr = (ವರ್ಣ *)gui.rsrc_tooltip_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.tooltip_fontset;
    to.size = ಪರಿಮಾಣ(XFontSet);

    ಯದಿ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
}
#endif

#ಯದಿ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    ಶೂನ್ಯ
gui_init_menu_font(ಶೂನ್ಯ)
{
    Xrm_ಮೌಲ್ಯ from, to;

# ifdef FONTSET_ALWAYS
    from.addr = (ವರ್ಣ *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_fontset;
    to.size = ಪರಿಮಾಣ(GuiFontset);

    ಯದಿ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
# ಅನ್ಯಥಾ
    from.addr = (ವರ್ಣ *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_font;
    to.size = ಪರಿಮಾಣ(GuiFont);

    ಯದಿ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &to) == False)
    {
	// Failed. What to do?
    }
# endif
}
#endif

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಙ್ಕ rc UNUSED)
{
#ಯದಿ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#endif
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ರಿಕ್ತ);
    *x = xpos;
    *y = ypos;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell,
	XtNx,	x,
	XtNy,	y,
	ರಿಕ್ತ);
}

    ಶೂನ್ಯ
gui_mch_set_shellsize(
    ಪೂರ್ಣಾಙ್ಕ		width,
    ಪೂರ್ಣಾಙ್ಕ		height,
    ಪೂರ್ಣಾಙ್ಕ		min_width,
    ಪೂರ್ಣಾಙ್ಕ		min_height,
    ಪೂರ್ಣಾಙ್ಕ		base_width,
    ಪೂರ್ಣಾಙ್ಕ		base_height,
    ಪೂರ್ಣಾಙ್ಕ		direction UNUSED)
{
#ifdef FEAT_XIM
    height += xim_get_status_area_height(),
#endif
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell,
	XtNwidthInc,	gui.char_width,
	XtNheightInc,	gui.char_height,
#ಯದಿ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	XtNbaseWidth,	base_width,
	XtNbaseHeight,	base_height,
#endif
	XtNminWidth,	min_width,
	XtNminHeight,	min_height,
	XtNwidth,	width,
	XtNheight,	height,
	ರಿಕ್ತ);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(
    ಪೂರ್ಣಾಙ್ಕ	    *screen_w,
    ಪೂರ್ಣಾಙ್ಕ	    *screen_h)
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
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_font(
    char_u	*font_name,
    ಪೂರ್ಣಾಙ್ಕ		do_fontset UNUSED)
{
    XFontStruct	*font = ರಿಕ್ತ;

#ifdef FEAT_XFONTSET
    XFontSet	fontset = ರಿಕ್ತ;
#endif

#ifdef FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    ಯದಿ (font_name != ರಿಕ್ತ && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	ಯದಿ (font_name == ರಿಕ್ತ && !gui.starting)
	    ಪ್ರತ್ಯಾಗಮ OK;
    }
#endif

#ifdef FEAT_XFONTSET
    ಯದಿ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	ಯದಿ (font_name != ರಿಕ್ತ)
	{
	    fontset = (XFontSet)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    ಯದಿ (fontset == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಅನ್ಯಥಾ
#endif
    {
	ಯದಿ (font_name == ರಿಕ್ತ)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    ಯದಿ (font == ರಿಕ್ತ)
		font_name = (char_u *)DFLT_FONT;
	}
	ಯದಿ (font == ರಿಕ್ತ)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	ಯದಿ (font == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    ಯದಿ (fontset != ರಿಕ್ತ)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_width = fontset_width(fontset);
	gui.char_height = fontset_height(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (font_name == gui.rsrc_font_name)
    {
	ಯದಿ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ರಿಕ್ತ
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	ಯದಿ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ರಿಕ್ತ
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	ಯದಿ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ರಿಕ್ತ
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    ಅನ್ಯಥಾ
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	ಯದಿ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	ಯದಿ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	ಯದಿ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ifdef FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#endif

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, ಪೂರ್ಣಾಙ್ಕ giveErrorIfMissing)
{
    XFontStruct	*font;

    ಯದಿ (!gui.in_use || name == ರಿಕ್ತ)	// can't do this when GUI not running
	ಪ್ರತ್ಯಾಗಮ NOFONT;

    font = XLoadQueryFont(gui.dpy, (ವರ್ಣ *)name);

    ಯದಿ (font == ರಿಕ್ತ)
    {
	ಯದಿ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	ಪ್ರತ್ಯಾಗಮ NOFONT;
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

    ಯದಿ (font->max_bounds.width != font->min_bounds.width)
    {
	semsg(_(e_font_str_is_not_fixed_width), name);
	XFreeFont(gui.dpy, font);
	ಪ್ರತ್ಯಾಗಮ NOFONT;
    }
    ಪ್ರತ್ಯಾಗಮ (GuiFont)font;
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ರಿಕ್ತ;

    ಯದಿ (name != ರಿಕ್ತ && font == ರಿಕ್ತ)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    ಅನ್ಯಥಾ ಯದಿ (font != ರಿಕ್ತ)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	unsigned long ಮೌಲ್ಯ = 0L;

	ಯದಿ (XGetFontProperty(font, XA_FONT, &ಮೌಲ್ಯ))
	{
	    ವರ್ಣ *xa_font_name = ರಿಕ್ತ;

	    xa_font_name = XGetAtomName(gui.dpy, ಮೌಲ್ಯ);
	    ಯದಿ (xa_font_name != ರಿಕ್ತ)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (name != ರಿಕ್ತ)
		ret = vim_strsave(name);
	}
	ಅನ್ಯಥಾ ಯದಿ (name != ರಿಕ್ತ)
	    ret = vim_strsave(name);
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}
#endif

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
#ifdef FEAT_XFONTSET
    ಯದಿ (gui.fontset != NOFONTSET)
    {
	gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
							    + p_linespace / 2;
    }
    ಅನ್ಯಥಾ
#endif
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set the current text font.
 */
    ಶೂನ್ಯ
gui_mch_set_font(GuiFont font)
{
    ಸ್ಥಿರ Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    ಯದಿ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.text_gc, fid);
	XSetFont(gui.dpy, gui.back_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ifdef FEAT_XFONTSET
    current_fontset = (XFontSet)ರಿಕ್ತ;
#endif
}

#ಯದಿ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    ಶೂನ್ಯ
gui_mch_set_fontset(GuiFontset fontset)
{
    current_fontset = (XFontSet)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಯದಿ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#ಯದಿ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    ಶೂನ್ಯ
gui_mch_free_fontset(GuiFontset fontset)
{
    ಯದಿ (fontset != NOFONTSET)
	XFreeFontSet(gui.dpy, (XFontSet)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		giveErrorIfMissing,
    ಪೂರ್ಣಾಙ್ಕ		fixed_width)
{
    XFontSet	fontset;
    ವರ್ಣ	**missing, *def_str;
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_missing;

    ಯದಿ (!gui.in_use || name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ NOFONTSET;

    fontset = XCreateFontSet(gui.dpy, (ವರ್ಣ *)name, &missing, &ಅಂಕೆ_missing,
			     &def_str);
    ಯದಿ (ಅಂಕೆ_missing > 0)
    {
	ಪೂರ್ಣಾಙ್ಕ i;

	ಯದಿ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_in_fontset), name);
	    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    ಯದಿ (fontset == ರಿಕ್ತ)
    {
	ಯದಿ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	ಪ್ರತ್ಯಾಗಮ NOFONTSET;
    }

    ಯದಿ (fixed_width && check_fontset_sanity(fontset) == FAIL)
    {
	XFreeFontSet(gui.dpy, fontset);
	ಪ್ರತ್ಯಾಗಮ NOFONTSET;
    }
    ಪ್ರತ್ಯಾಗಮ (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_fontset_sanity(XFontSet fs)
{
    XFontStruct	**xfs;
    ವರ್ಣ	**font_name;
    ಪೂರ್ಣಾಙ್ಕ		fn;
    ವರ್ಣ	*base_name;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		min_width;
    ಪೂರ್ಣಾಙ್ಕ		min_font_idx = 0;

    base_name = XBaseFontNameListOfFontSet(fs);
    fn = XFontsOfFontSet(fs, &xfs, &font_name);
    ಪ್ರತಿ (i = 0; i < fn; i++)
    {
	ಯದಿ (xfs[i]->max_bounds.width != xfs[i]->min_bounds.width)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_width),
		    base_name, font_name[i]);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    // scan base font width
    min_width = 32767;
    ಪ್ರತಿ (i = 0; i < fn; i++)
    {
	ಯದಿ (xfs[i]->max_bounds.width<min_width)
	{
	    min_width = xfs[i]->max_bounds.width;
	    min_font_idx = i;
	}
    }
    ಪ್ರತಿ (i = 0; i < fn; i++)
    {
	ಯದಿ (	   xfs[i]->max_bounds.width != 2 * min_width
		&& xfs[i]->max_bounds.width != min_width)
	{
	    semsg(_(e_fontset_name_str), base_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (ಪೂರ್ಣಾಙ್ಕ)xfs[min_font_idx]->max_bounds.width);
	    semsg(_("Font%d width: %d"), i, (ಪೂರ್ಣಾಙ್ಕ)xfs[i]->max_bounds.width);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    // it seems ok. Good Luck!!
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fontset_width(XFontSet fs)
{
 ಪ್ರತ್ಯಾಗಮ XmbTextEscapement(fs, "Vim", 3) / 3;
}

    ಪೂರ್ಣಾಙ್ಕ
fontset_height(
    XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    ಪ್ರತ್ಯಾಗಮ extents->max_logical_extent.height;
}

# ಯದಿ 0
// NOT USED YET
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fontset_descent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet (fs);
    ಪ್ರತ್ಯಾಗಮ extents->max_logical_extent.height + extents->max_logical_extent.y;
}
# endif

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fontset_ascent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    ಪ್ರತ್ಯಾಗಮ -extents->max_logical_extent.y;
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
    ಯದಿ (!gui.in_use || name == ರಿಕ್ತ || *name == NUL)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    requested = gui_get_color_cmn(name);
    ಯದಿ (requested == INVALCOLOR)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    ಪ್ರತ್ಯಾಗಮ gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    XColor	available;
    Colormap	colormap;

#ಯದಿ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    ವರ್ಣ	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, ಪರಿಮಾಣ(spec), "#%.2x%.2x%.2x", r, g, b);
    ಯದಿ (XParseColor(gui.dpy, colormap, (ವರ್ಣ *)spec, &available) != 0
	    && XAllocColor(gui.dpy, colormap, &available) != 0)
	ಪ್ರತ್ಯಾಗಮ (guicolor_T)available.pixel;
#endif
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_FIELD(available);
    available.red = r << 8;
    available.green = g << 8;
    available.blue = b << 8;
    ಯದಿ (XAllocColor(gui.dpy, colormap, &available) != 0)
	ಪ್ರತ್ಯಾಗಮ (guicolor_T)available.pixel;

    ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    ಶೂನ್ಯ
gui_mch_set_fg_color(guicolor_T color)
{
    ಯದಿ (color == prev_fg_color)
	ಪ್ರತ್ಯಾಗಮ;

    XSetForeground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    ಶೂನ್ಯ
gui_mch_set_bg_color(guicolor_T color)
{
    ಯದಿ (color == prev_bg_color)
	ಪ್ರತ್ಯಾಗಮ;

    XSetBackground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_set_sp_color(guicolor_T color)
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
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (unsigned long)0, (XGCValues*)0);

    ಯದಿ (gc != ರಿಕ್ತ)
    {
	XDrawPoint(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    ಪ್ರತ್ಯಾಗಮ XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_curl(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ cells)
{
    ಪೂರ್ಣಾಙ್ಕ			i;
    ಪೂರ್ಣಾಙ್ಕ			offset;
    ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
    ಪ್ರತಿ (i = FILL_X(col); i < FILL_X(col + cells); ++i)
    {
	offset = val[i % 8];
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, i,
						FILL_Y(row + 1) - 1 - offset);
    }
    XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
}

    ಶೂನ್ಯ
gui_mch_draw_string(
    ಪೂರ್ಣಾಙ್ಕ		row,
    ಪೂರ್ಣಾಙ್ಕ		col,
    char_u	*s,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		flags)
{
    ಪೂರ್ಣಾಙ್ಕ			cells = len;
    ಸ್ಥಿರ ಶೂನ್ಯ		*buf = ರಿಕ್ತ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		buflen = 0;
    char_u		*p;
    ಪೂರ್ಣಾಙ್ಕ			wlen = 0;
    ಪೂರ್ಣಾಙ್ಕ			c;

    ಯದಿ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	ಯದಿ (buflen < len)
	{
	    XtFree((ವರ್ಣ *)buf);
	    buf = (ಶೂನ್ಯ *)XtMalloc(len * (ಪರಿಮಾಣ(XChar2b) < ಪರಿಮಾಣ(wchar_t)
					? ಪರಿಮಾಣ(wchar_t) : ಪರಿಮಾಣ(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	ಯಾವತ್ (p < s + len)
	{
	    c = utf_ptr2char(p);
#ifdef FEAT_XFONTSET
	    ಯದಿ (current_fontset != ರಿಕ್ತ)
	    {
# ifdef SMALL_WCHAR_T
		ಯದಿ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    ಅನ್ಯಥಾ
#endif
	    {
		ಯದಿ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (unsigned)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
    {
	cells = 0;
	ಪ್ರತಿ (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ifdef FEAT_XFONTSET
    ಯದಿ (current_fontset != ರಿಕ್ತ)
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

    ಯದಿ (flags & DRAW_TRANSP)
    {
	ಯದಿ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ಅನ್ಯಥಾ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ವರ್ಣ *)s, len);
    }
    ಅನ್ಯಥಾ ಯದಿ (p_linespace != 0
#ifdef FEAT_XFONTSET
	    || current_fontset != ರಿಕ್ತ
#endif
	    )
    {
	XSetForeground(gui.dpy, gui.text_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		FILL_Y(row), gui.char_width * cells, gui.char_height);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);

	ಯದಿ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ಅನ್ಯಥಾ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ವರ್ಣ *)s, len);
    }
    ಅನ್ಯಥಾ
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	ಯದಿ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	ಅನ್ಯಥಾ
	    XDrawImageString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (ವರ್ಣ *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    ಯದಿ (flags & DRAW_BOLD)
    {
	ಯದಿ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), buf, wlen);
	ಅನ್ಯಥಾ
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), (ವರ್ಣ *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    ಯದಿ (flags & DRAW_UNDERC)
	draw_curl(row, col, cells);

    // Underline: draw a line at the bottom of the character cell.
    ಯದಿ (flags & DRAW_UNDERL)
    {
	ಪೂರ್ಣಾಙ್ಕ	y = FILL_Y(row + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ಯದಿ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_ಸಾಲು(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
    }

    ಯದಿ (flags & DRAW_STRIKE)
    {
	ಪೂರ್ಣಾಙ್ಕ	y = FILL_Y(row + 1) - gui.char_height/2;

	XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
	XDraw_ಸಾಲು(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
    }

#ifdef FEAT_XFONTSET
    ಯದಿ (current_fontset != ರಿಕ್ತ)
	XSetClipMask(gui.dpy, gui.text_gc, None);
#endif
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != (ಕೀಲಿ_Sym)0; i++)
	ಯದಿ (name[0] == special_ಕೀಲಿಗಳು[i].vim_code0 &&
					 name[1] == special_ಕೀಲಿಗಳು[i].vim_code1)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_get_x11_windis(Window *win, Display **dis)
{
    *win = XtWindow(vimShell);
    *dis = gui.dpy;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    XBell(gui.dpy, 0);
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಙ್ಕ msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ಪೂರ್ಣಾಙ್ಕ)Columns) + gui.border_offset,
	    FILL_Y((ಪೂರ್ಣಾಙ್ಕ)Rows) + gui.border_offset);
    XSync(gui.dpy, False);
    ui_delay((long)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((ಪೂರ್ಣಾಙ್ಕ)Columns) + gui.border_offset,
	    FILL_Y((ಪೂರ್ಣಾಙ್ಕ)Rows) + gui.border_offset);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ಶೂನ್ಯ
gui_mch_invert_rectangle(
    ಪೂರ್ಣಾಙ್ಕ	    r,
    ಪೂರ್ಣಾಙ್ಕ	    c,
    ಪೂರ್ಣಾಙ್ಕ	    nr,
    ಪೂರ್ಣಾಙ್ಕ	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_width, (nr) * gui.char_height);
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    XIconifyWindow(gui.dpy, XtWindow(vimShell), DefaultScreen(gui.dpy));
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_set_foreground(ಶೂನ್ಯ)
{
    XMapRaised(gui.dpy, XtWindow(vimShell));
}
#endif

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ಪೂರ್ಣಾಙ್ಕ		w = 1;

    ಯದಿ (mb_lefthalve(gui.row, gui.col))
	w = 2;
    gui_mch_set_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(gui.col),
	    FILL_Y(gui.row), w * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ಶೂನ್ಯ
gui_mch_draw_part_cursor(ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h, guicolor_T color)
{
    gui_mch_set_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.text_gc,
#ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ಬಲ ? FILL_X(gui.col + 1) - w :
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
    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    Xt_ಒಳಹರಿವು_Mask mask, desired;

#ifdef ALT_X_ಒಳಹರಿವು
    ಯದಿ (suppress_alternate_ಒಳಹರಿವು)
	desired = (XtIMXEvent | XtIMTimer);
    ಅನ್ಯಥಾ
#endif
	desired = (XtIMAll);
    ಯಾವತ್ ((mask = XtAppPending(app_context)) && (mask & desired)
	    && !vim_is_ಒಳಹರಿವು_buf_full())
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
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_wait_for_chars(long wtime)
{
    ಪೂರ್ಣಾಙ್ಕ	    focus;
    ಪೂರ್ಣಾಙ್ಕ	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    timed_out;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_ಒಳಹರಿವು_Mask	    desired;
#ifdef FEAT_JOB_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#endif

    timed_out = FALSE;

    ಯದಿ (wtime >= 0)
	timer = XtAppAddTimeOut(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_out);
#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ಯದಿ (channel_any_keep_open())
	channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
				   channel_poll_cb, (XtPointer)&channel_timer);
#endif

    focus = gui.in_focus;
    desired = (XtIMAll);
    ಯಾವತ್ (!timed_out)
    {
	// Stop or start blinking when focus changes
	ಯದಿ (gui.in_focus != focus)
	{
	    ಯದಿ (gui.in_focus)
		gui_mch_start_blink();
	    ಅನ್ಯಥಾ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಯದಿ (did_add_timer)
	    // Need to recompute the waiting time.
	    ಭಙ್ಗ;
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

	ಯದಿ (ಒಳಹರಿವು_available())
	{
	    retval = OK;
	    ಭಙ್ಗ;
	}
    }

    ಯದಿ (timer != (XtIntervalId)0 && !timed_out)
	XtRemoveTimeOut(timer);
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (channel_timer != (XtIntervalId)0)
	XtRemoveTimeOut(channel_timer);
#endif

    ಪ್ರತ್ಯಾಗಮ retval;
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
gui_mch_clear_block(
    ಪೂರ್ಣಾಙ್ಕ		row1,
    ಪೂರ್ಣಾಙ್ಕ		col1,
    ಪೂರ್ಣಾಙ್ಕ		row2,
    ಪೂರ್ಣಾಙ್ಕ		col2)
{
    ಪೂರ್ಣಾಙ್ಕ		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.back_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height);
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
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    ಯದಿ (gui.visibility == VisibilityFullyObscured)
	ಪ್ರತ್ಯಾಗಮ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(row + ಅಂಕೆ_ಸಾಲುಗಳು),
	gui.char_width * (gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1)
			       + (gui.scroll_region_ಬಲ == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - ಅಂಕೆ_ಸಾಲುಗಳು + 1),
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(row));

    gui_clear_block(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
						       gui.scroll_region_ಎಡ,
			  gui.scroll_region_bot, gui.scroll_region_ಬಲ);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    ಯದಿ (gui.visibility == VisibilityFullyObscured)
	ಪ್ರತ್ಯಾಗಮ;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(row),
	gui.char_width * (gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1)
			       + (gui.scroll_region_ಬಲ == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - ಅಂಕೆ_ಸಾಲುಗಳು + 1),
	FILL_X(gui.scroll_region_ಎಡ), FILL_Y(row + ಅಂಕೆ_ಸಾಲುಗಳು));

    gui_clear_block(row, gui.scroll_region_ಎಡ,
				row + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
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

    ಯದಿ (gui.visibility != VisibilityPartiallyObscured)
	ಪ್ರತ್ಯಾಗಮ;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    ಪ್ರತ್ಯಾಗಮ;	// The scroll worked.

	ಯದಿ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    ಯದಿ (gevent->ಎಣಿಕೆ == 0)
		ಪ್ರತ್ಯಾಗಮ;		// This was the last expose event
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

    ಪೂರ್ಣಾಙ್ಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    ಪ್ರತ್ಯಾಗಮ clip_x11_own_selection(vimShell, cbd);
}

    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    ಶೂನ್ಯ
clip_mch_set_selection(
    Clipboard_T	*cbd)
{
    clip_x11_set_selection(cbd);
}

#ಯದಿ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ grey)
{
    ಯದಿ (menu->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    gui_mch_menu_hidden(menu, False);
    ಯದಿ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	XtSetSensitive(menu->id, False);
    ಅನ್ಯಥಾ
	XtSetSensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ hidden)
{
    ಯದಿ (menu->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (hidden)
	XtUnmanageChild(menu->id);
    ಅನ್ಯಥಾ
	XtManageChild(menu->id);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_wm_protocol_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    /*
     * Only deal with Client messages.
     */
    ಯದಿ (event->type != ClientMessage)
	ಪ್ರತ್ಯಾಗಮ;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    ಯದಿ ((Atom)((XClientMessageEvent *)event)->data.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	out_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, XtWindow(vimShell), ರಿಕ್ತ, 0);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ ((Atom)((XClientMessageEvent *)event)->data.l[0] !=
						  wm_atoms[DELETE_WINDOW_IDX])
	ಪ್ರತ್ಯಾಗಮ;

    gui_shell_closed();
}

#ifdef FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_send_event_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    ಯದಿ (e->type == PropertyNotify && e->window == commWindow
	    && e->atom == commProperty &&  e->state == Property_New_ಮೌಲ್ಯ)
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u		blink_waittime = 700;
ಸ್ಥಿರ long_u		blink_ontime = 400;
ಸ್ಥಿರ long_u		blink_offtime = 250;
ಸ್ಥಿರ XtIntervalId	blink_timer = (XtIntervalId)0;

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಙ್ಕ may_call_gui_update_cursor)
{
    ಯದಿ (blink_timer != (XtIntervalId)0)
    {
	XtRemoveTimeOut(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    ಯದಿ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_x11_blink_cb(
    XtPointer	    timed_out UNUSED,
    XtIntervalId    *interval_id UNUSED)
{
    ಯದಿ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = XtAppAddTimeOut(app_context, blink_offtime,
						      gui_x11_blink_cb, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = XtAppAddTimeOut(app_context, blink_ontime,
						      gui_x11_blink_cb, ರಿಕ್ತ);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ಶೂನ್ಯ
gui_mch_start_blink(ಶೂನ್ಯ)
{
    ಯದಿ (blink_timer != (XtIntervalId)0)
	XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    ಯದಿ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = XtAppAddTimeOut(app_context, blink_waittime,
						      gui_x11_blink_cb, ರಿಕ್ತ);
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

    ಪ್ರತ್ಯಾಗಮ (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((unsigned)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    ಶೂನ್ಯ
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
    ಯದಿ (vim_strchr(p_go, GO_POINTER) != ರಿಕ್ತ)
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

    XtAddEventHandler(vimForm, ಕೀಲಿ_Press_Mask, FALSE, gui_x11_ಕೀಲಿ_hit_cb,
	(XtPointer)0);
    XtAddEventHandler(textArea, ಕೀಲಿ_Press_Mask, FALSE, gui_x11_ಕೀಲಿ_hit_cb,
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
    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    ಪೂರ್ಣಾಙ್ಕ		rootx, rooty, winx, winy;
    Window	ಬೇರು, child;
    unsigned ಪೂರ್ಣಾಙ್ಕ mask;

    ಯದಿ (gui.wid && XQueryPointer(gui.dpy, gui.wid, &ಬೇರು, &child,
					 &rootx, &rooty, &winx, &winy, &mask))
    {
	*x = winx;
	*y = winy;
    }
    ಅನ್ಯಥಾ
    {
	*x = -1;
	*y = -1;
    }
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    ಯದಿ (gui.wid)
	XWarpPointer(gui.dpy, (Window)0, gui.wid, 0, 0, 0, 0, x, y);
}

#ಯದಿ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_last_mouse_event(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ &last_mouse_event;
}
#endif

#ಯದಿ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_WIDTH (gui.char_width * 2)

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ typenr)
{
    XImage	*sign;

    ಯದಿ (!gui.in_use || (sign = (XImage *)sign_get_image(typenr)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    XClearArea(gui.dpy, gui.wid, TEXT_X(col), TEXT_Y(row) - sign->height,
	    SIGN_WIDTH, gui.char_height, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.text_gc, sign, 0, 0,
	    TEXT_X(col) + (SIGN_WIDTH - sign->width) / 2,
	    TEXT_Y(row) - sign->height,
	    sign->width, sign->height);
}

    ಶೂನ್ಯ *
gui_mch_register_sign(char_u *signfile)
{
    XpmAttributes   attrs;
    XImage	    *sign = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    status;

    /*
     * Setup the color substitution table.
     */
    ಯದಿ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_ಚಿಹ್ನೆ color[5] =
	{
	    {"none", ರಿಕ್ತ, 0},
	    {"iconColor1", ರಿಕ್ತ, 0},
	    {"bottomShadowColor", ರಿಕ್ತ, 0},
	    {"topShadowColor", ರಿಕ್ತ, 0},
	    {"selectColor", ರಿಕ್ತ, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.back_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	status = XpmReadFileToImage(gui.dpy, (ವರ್ಣ *)signfile,
							 &sign, ರಿಕ್ತ, &attrs);
	ಯದಿ (status == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	ಅನ್ಯಥಾ
	    emsg(_(e_couldnt_read_in_sign_data));
    }

    ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ *)sign;
}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    XDestroyImage((XImage*)sign);
}
#endif


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 */
    ಶೂನ್ಯ
gui_mch_mousehide(
    ಪೂರ್ಣಾಙ್ಕ		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    ಯದಿ (gui.pointer_hidden == hide)
	ಪ್ರತ್ಯಾಗಮ;

    gui.pointer_hidden = hide;
    ಯದಿ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    ಅನ್ಯಥಾ
#ifdef FEAT_MOUSESHAPE
	mch_set_mouse_shape(last_shape);
#ಅನ್ಯಥಾ
    XUndefineCursor(gui.dpy, gui.wid);
#endif
}

#ಯದಿ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mshape_ids[] =
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
mch_set_mouse_shape(ಪೂರ್ಣಾಙ್ಕ shape)
{
    ಪೂರ್ಣಾಙ್ಕ	    id;

    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ಯದಿ (id >= XC_ಅಂಕೆ_glyphs)
		id = XC_ಎಡ_ptr;
	    ಅನ್ಯಥಾ
		id &= ~1;	// they are always even (why?)
	}
	ಅನ್ಯಥಾ
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    ಯದಿ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

#ಯದಿ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    ಶೂನ್ಯ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    ಯದಿ (menu->id == ರಿಕ್ತ || menu->parent == ರಿಕ್ತ
				|| !menu_is_toolbar(menu->parent->name))
	ಪ್ರತ್ಯಾಗಮ;

    // Always destroy and create the balloon, in case the string was
    // changed.
    ಯದಿ (menu->tip != ರಿಕ್ತ)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ರಿಕ್ತ;
    }
    ಯದಿ (menu->strings[MENU_INDEX_TIP] != ರಿಕ್ತ)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->strings[MENU_INDEX_TIP],
		ರಿಕ್ತ,
		ರಿಕ್ತ);
}
#endif
