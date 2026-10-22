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

#અન્તર્ભાવ "vim.h"

#અન્તર્ભાવ <X11/keysym.h>
#અન્તર્ભાવ <X11/Xatom.h>
#અન્તર્ભાવ <X11/StringDefs.h>
#અન્તર્ભાવ <X11/Intrinsic.h>
#અન્તર્ભાવ <X11/Shell.h>
#અન્તર્ભાવ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#જો defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#નહીંતર
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  નહીંતર
#   include <X11/xpm.h>
#  endif
# endif
#endif

#ifdef FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# નહીંતર
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
#નહીંતર
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

સ્થિર Atom   wm_atoms[2];	// Window Manager Atoms
#define DELETE_WINDOW_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#define SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ifdef FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
સ્થિર XFontSet current_fontset = ખાલી;
# જો !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	કરો \
	{ \
	    જો (current_fontset != ખાલી) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    નહીંતર \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} જ્યારે (0)
# endif
# જો !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	કરો \
	{ \
	    જો (current_fontset != ખાલી) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    નહીંતર \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} જ્યારે (0)
# endif
# જો !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	કરો \
	{ \
	    જો (current_fontset != ખાલી) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    નહીંતર \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} જ્યારે (0)
# endif
સ્થિર પૂર્ણાંક check_fontset_sanity(XFontSet fs);
સ્થિર પૂર્ણાંક fontset_width(XFontSet fs);
સ્થિર પૂર્ણાંક fontset_ascent(XFontSet fs);
#endif

સ્થિર guicolor_T	prev_fg_color = INVALCOLOR;
સ્થિર guicolor_T	prev_bg_color = INVALCOLOR;
સ્થિર guicolor_T	prev_sp_color = INVALCOLOR;

#જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
સ્થિર XButtonPressedEvent last_mouse_event;
#endif

સ્થિર શૂન્ય gui_x11_check_copy_area(શૂન્ય);
#ifdef FEAT_CLIENTSERVER
સ્થિર શૂન્ય gui_x11_send_event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif
સ્થિર શૂન્ય gui_x11_wm_protocol_handler(Widget, XtPointer, XEvent *, Boolean *);
સ્થિર Cursor gui_x11_create_blank_mouse(શૂન્ય);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
સ્થિર સંરચના specialkey
{
    ચાવી_Sym  ચાવી_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_ચાવીઓ[] =
{
    {XK_Up,		'k', 'u'},
    {XK_Down,		'k', 'd'},
    {XK_ડાબું,		'k', 'l'},
    {XK_જમણું,		'k', 'r'},

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
    {XK_ઉમેરો,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_End,		'@', '7'},
    {XK_Prior,		'k', 'P'},
    {XK_Next,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ifdef XK_KP_ડાબું
    {XK_KP_ડાબું,	'k', 'l'},
    {XK_KP_જમણું,	'k', 'r'},
    {XK_KP_Up,		'k', 'u'},
    {XK_KP_Down,	'k', 'd'},
    {XK_KP_ઉમેરો,	KS_EXTRA, (char_u)KE_KINS},
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
    {(ચાવી_Sym)0,	    0, 0}
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
સ્થિર XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	કદ(Pixel),
	XtOffsetOf(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	કદ(Pixel),
	XtOffsetOf(gui_T, def_back_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	કદ(String *),
	XtOffsetOf(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	કદ(String *),
	XtOffsetOf(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	કદ(String *),
	XtOffsetOf(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	કદ(String *),
	XtOffsetOf(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	કદ(String *),
	XtOffsetOf(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	કદ(Bool),
	XtOffsetOf(gui_T, rsrc_rev_video),
	XtRImmediate,
	(XtPointer)False
    },
    {
	XtNborderWidth,
	XtCBorderWidth,
	XtRInt,
	કદ(પૂર્ણાંક),
	XtOffsetOf(gui_T, border_width),
	XtRImmediate,
	(XtPointer)2
    },
    {
	XtNscrollbarWidth,
	XtCScrollbarWidth,
	XtRInt,
	કદ(પૂર્ણાંક),
	XtOffsetOf(gui_T, scrollbar_width),
	XtRImmediate,
	(XtPointer)SB_DEFAULT_WIDTH
    },
#ifdef FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	XtNmenuFontSet,
	XtCMenuFontSet,
# નહીંતર
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#endif
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ifdef FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	કદ(અક્ષર *),
	XtOffsetOf(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	XtCFontSet,
	XtRFontSet,
	કદ(XFontSet),
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
	કદ(અક્ષર*),
	XtOffsetOf(gui_T, rsrc_preedit_type_name),
	XtRString,
	(XtPointer)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	કદ(અક્ષર*),
	XtOffsetOf(gui_T, rsrc_આગત_method),
	XtRString,
	ખાલી
    },
#endif // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
સ્થિર XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ખાલી},
    {"-background",	".background",	    XrmoptionSepArg,	ખાલી},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ખાલી},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ખાલી},
    {"-fn",		".font",	    XrmoptionSepArg,	ખાલી},
    {"-font",		".font",	    XrmoptionSepArg,	ખાલી},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ખાલી},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ખાલી},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ખાલી},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ખાલી},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ખાલી},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ખાલી},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ખાલી},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ખાલી},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ખાલી},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ખાલી},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ખાલી},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ખાલી},
#ifdef FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ખાલી},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ખાલી},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ખાલી},
#નહીંતર
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ખાલી},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ખાલી},
#endif
    {"-xrm",		ખાલી,		    XrmoptionResArg,	ખાલી}
};

સ્થિર પૂર્ણાંક gui_argc = 0;
સ્થિર અક્ષર **gui_argv = ખાલી;

/*
 * Call-back routines.
 */

    સ્થિર શૂન્ય
gui_x11_timer_cb(
    XtPointer	    timed_out,
    XtIntervalId    *interval_id UNUSED)
{
    *((પૂર્ણાંક *)timed_out) = TRUE;
}

#ifdef FEAT_JOB_CHANNEL
    સ્થિર શૂન્ય
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

    સ્થિર શૂન્ય
gui_x11_visibility_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    જો (event->type != VisibilityNotify)
	પાછા;

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

    સ્થિર શૂન્ય
gui_x11_expose_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XExposeEvent	*gevent;
    પૂર્ણાંક			new_x;

    જો (event->type != Expose)
	પાછા;

    out_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    જો (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    જો (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    જો (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((પૂર્ણાંક)Columns), 0, 0, 0, False);
    જો (gevent->y > FILL_Y(Rows))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((પૂર્ણાંક)Rows), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#જો defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    સ્થિર શૂન્ય
shellRectangle(Widget shell, XRectangle *r)
{
    Window		rootw, shellw, child, parentw;
    પૂર્ણાંક			absx, absy;
    XWindowAttributes	a;
    Window		*children;
    unsigned પૂર્ણાંક	children_ગણતરી;

    shellw = XtWindow(shell);
    જો (shellw == 0)
	પાછા;
    માટે (;;)
    {
	XQuery_વૃક્ષ(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_ગણતરી);
	XFree(children);
	જો (parentw == rootw)
	    તોડો;
	shellw = parentw;
    }
    XGetWindowAttributes(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.મૂળ, 0, 0,
							&absx, &absy, &child);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_કિંમતો(shell, XmNheight, &r->height, XmNwidth, &r->width, ખાલી);
}
#endif

    સ્થિર શૂન્ય
gui_x11_resize_window_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    સ્થિર પૂર્ણાંક lastWidth, lastHeight;

    જો (event->type != ConfigureNotify)
	પાછા;

    જો (event->xconfigure.width != lastWidth
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
#જો defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    જો (netbeans_active())
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

    સ્થિર શૂન્ય
gui_x11_focus_change_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    gui_focus_change(event->type == FocusIn);
}

    સ્થિર શૂન્ય
gui_x11_enter_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(TRUE);
}

    સ્થિર શૂન્ય
gui_x11_leave_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(FALSE);
}

#જો defined(X_HAVE_UTF8_STRING)
# જો X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#endif

    શૂન્ય
gui_x11_ચાવી_hit_cb(
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
#નહીંતર
    char_u		string[4], string2[3];
#endif
    ચાવી_Sym		ચાવી_sym;
    પૂર્ણાંક			len;
    પૂર્ણાંક			i;
    પૂર્ણાંક			modifiers;
    પૂર્ણાંક			ચાવી;

    ev_press = (XKeyPressedEvent *)event;

#ifdef FEAT_XIM
    જો (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	જો (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (અક્ષર *)string,
				  કદ(string_shortbuf), &ચાવી_sym, &status);
	નહીંતર
# endif
	    len = XmbLookupString(xic, ev_press, (અક્ષર *)string,
				  કદ(string_shortbuf), &ચાવી_sym, &status);
	જો (status == XBufferOverflow)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    જો (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (અક્ષર *)string,
						      len, &ચાવી_sym, &status);
	    નહીંતર
# endif
		len = XmbLookupString(xic, ev_press, (અક્ષર *)string,
						      len, &ચાવી_sym, &status);
	}
	જો (status == XLookupNone || status == XLookupChars)
	    ચાવી_sym = XK_Void_પ્રતીક;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	જો (len > 0 && આગત_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    પૂર્ણાંક		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    જો (string_alloced)
		XtFree((અક્ષર *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_આગત(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	માટે (i = 0; i < len; ++i)
	    જો (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (પૂર્ણાંક)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		જો (string_alloced)
		    XtFree((અક્ષર *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    નહીંતર
#endif
	len = XLookupString(ev_press, (અક્ષર *)string, કદ(string),
		&ચાવી_sym, ખાલી);

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    જો (ચાવી_sym == SunXK_F36 || ચાવી_sym == SunXK_F37)
	len = 0;
#endif

    જો (ચાવી_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    જો (ચાવી_sym == XK_minus && (ev_press->state & ControlMask))
	string[0] = Ctrl__;

#ifdef XK_ISO_ડાબું_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    જો (ચાવી_sym == XK_ISO_ડાબું_Tab)
    {
	ચાવી_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    જો (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (પૂર્ણાંક)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    જો (len == 0 || len == 1)
    {
	માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != (ચાવી_Sym)0; i++)
	{
	    જો (special_ચાવીઓ[i].ચાવી_sym == ચાવી_sym)
	    {
		string[0] = CSI;
		string[1] = special_ચાવીઓ[i].vim_code0;
		string[2] = special_ચાવીઓ[i].vim_code1;
		len = -3;
		તોડો;
	    }
	}
    }

    // Unrecognised key is ignored.
    જો (len == 0)
	જાઓ theend;

    // Handle modifiers.
    modifiers = 0;
    જો (ev_press->state & ShiftMask)
	modifiers |= MOD_MASK_SHIFT;
    જો (ev_press->state & ControlMask)
    {
	modifiers |= MOD_MASK_CTRL;
	જો (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    જો (ev_press->state & Mod1Mask)
	modifiers |= MOD_MASK_ALT;
    જો (ev_press->state & Mod4Mask)
	modifiers |= MOD_MASK_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    જો (len == -3)
	ચાવી = TO_SPECIAL(string[1], string[2]);
    નહીંતર
    {
	string[len] = NUL;
	ચાવી = mb_ptr2char(string);
    }
    ચાવી = simplify_ચાવી(ચાવી, &modifiers);
    જો (ચાવી == CSI)
	ચાવી = K_CSI;
    જો (IS_SPECIAL(ચાવી))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ચાવી);
	string[2] = K_THIRD(ચાવી);
	len = 3;
    }
    નહીંતર
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ચાવી = may_adjust_ચાવી_for_ctrl(modifiers, ચાવી);

	len = mb_char2bytes(ચાવી, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_shift_modifier(modifiers, ચાવી);
    }

    જો (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_આગત_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	પૂર્ણાંક int_ch = check_for_interrupt(ચાવી, modifiers);

	જો (int_ch != NUL)
	{
	    trash_આગત_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_આગત_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    જો (p_mh)
	gui_mch_mousehide(TRUE);

#જો defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	જો ((be = gui_mch_currently_showing_beval()) != ખાલી)
	    gui_mch_unpost_balloon(be);
    }
#endif
theend:
    {}	    // some compilers need a statement here
#ifdef FEAT_XIM
    જો (string_alloced)
	XtFree((અક્ષર *)string);
#endif
}

    સ્થિર શૂન્ય
gui_x11_mouse_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    સ્થિર XtIntervalId timer = (XtIntervalId)0;
    સ્થિર પૂર્ણાંક	timed_out = TRUE;

    પૂર્ણાંક		button;
    પૂર્ણાંક		repeated_click = FALSE;
    પૂર્ણાંક		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    જો (event->type == MotionNotify)
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

	જો (button != MOUSE_DRAG)	// just moving the rodent
	{
#ifdef FEAT_MENU
	    જો (dud)			// moved in vimForm
		y -= gui.menu_height;
#endif
	    gui_mouse_moved(x, y);
	    પાછા;
	}
    }
    નહીંતર
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	જો (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    જો (!timed_out)
	    {
		XtRemoveTimeOut(timer);
		repeated_click = TRUE;
	    }
	    timed_out = FALSE;
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_out);
	    પસંદગી (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		સ્થિતિ Button1:	button = MOUSE_ડાબું;	તોડો;
		સ્થિતિ Button2:	button = MOUSE_મધ્ય;	તોડો;
		સ્થિતિ Button3:	button = MOUSE_જમણું;	તોડો;
		સ્થિતિ Button4:	button = MOUSE_4;	તોડો;
		સ્થિતિ Button5:	button = MOUSE_5;	તોડો;
		સ્થિતિ 6:		button = MOUSE_7;	તોડો;
		સ્થિતિ 7:		button = MOUSE_6;	તોડો;
		સ્થિતિ 8:		button = MOUSE_X1;	તોડો;
		સ્થિતિ 9:		button = MOUSE_X2;	તોડો;
		મૂળભૂત:
		    પાછા;	// Unknown button
	    }
	}
	નહીંતર જો (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	નહીંતર
	    પાછા;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	last_mouse_event = event->xbutton;
#endif
    }

    vim_modifiers = 0x0;
    જો (x_modifiers & ShiftMask)
	vim_modifiers |= MOUSE_SHIFT;
    જો (x_modifiers & ControlMask)
	vim_modifiers |= MOUSE_CTRL;
    જો (x_modifiers & Mod1Mask)	    // Alt or Meta key
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
    શૂન્ય
gui_mch_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
{
    પૂર્ણાંક	    arg;
    પૂર્ણાંક	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(અક્ષર *, *argc);
    જો (gui_argv == ખાલી)
	પાછા;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    જ્યારે (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	માટે (i = 0; i < (પૂર્ણાંક)Xt_સંખ્યા(cmdline_options); i++)
	    જો (strcmp(argv[arg], cmdline_options[i].option) == 0)
		તોડો;

	જો (i < (પૂર્ણાંક)Xt_સંખ્યા(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    જો (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		found_reverse_arg = TRUE;
	    નહીંતર જો ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_argument = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    જો (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * કદ(અક્ષર *));
		જો (cmdline_options[i].argKind != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    જો (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * કદ(અક્ષર *));
		}
	    }
	    argv[*argc] = ખાલી;
	}
	નહીંતર
#ifdef FEAT_NETBEANS_INTG
	    જો (strncmp("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * કદ(અક્ષર *));
	    argv[*argc] = ખાલી;
	}
	નહીંતર
#endif
	    arg++;
    }
}

#ifndef XtSpecificationRelease
# define CARDINAL (Cardinal *)
#નહીંતર
# જો XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# નહીંતર
#  define CARDINAL (પૂર્ણાંક *)
# endif
#endif

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    પૂર્ણાંક
gui_mch_init_check(શૂન્ય)
{
#ifdef FEAT_XIM
    XtSetLanguageProc(ખાલી, ખાલી, ખાલી);
#endif
    open_app_context();
    જો (app_context != ખાલી)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_સંખ્યા(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#જો defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	અક્ષર *p = setlocale(LC_NUMERIC, ખાલી);

	જો (p == ખાલી || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#endif
    જો (app_context == ખાલી || gui.dpy == ખાલી)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	પાછા FAIL;
    }
    પાછા OK;
}


#ifdef USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
સ્થિર Xt_આગત_Id _xsmp_xtinputid;

    સ્થિર શૂન્ય
local_xsmp_handle_requests(
    XtPointer	c UNUSED,
    પૂર્ણાંક		*s UNUSED,
    Xt_આગત_Id	*i UNUSED)
{
    જો (xsmp_handle_requests() == FAIL)
	Xt_Remove_આગત(_xsmp_xtinputid);
}
#endif


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    પૂર્ણાંક
gui_mch_init(શૂન્ય)
{
    XtGCMask	gc_mask;
    XGCValues	gc_vals;
    પૂર્ણાંક		x, y, mask;
    unsigned	w, h;

#જો 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#endif

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ખાલી);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (XtPointer)&gui,
	vim_resources, Xt_સંખ્યા(vim_resources), ખાલી);

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
    જો (gui.rsrc_rev_video && gui_get_lightness(gui.back_pixel)
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

    જો (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    જો (gui.geom != ખાલી && *gui.geom != NUL)
    {
	mask = XParseGeometry((અક્ષર *)gui.geom, &x, &y, &w, &h);
	જો (mask & Width_કિંમત)
	    Columns = w;
	જો (mask & Height_કિંમત)
	{
	    જો (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	જો (mask & (XValue|YValue))
	    Xt_Va_Set_કિંમતો(vimShell, XtNgeometry, gui.geom, ખાલી);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    જો (vim_strchr(p_go, GO_ICON) != ખાલી)
    {
#ifndef HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	XtSetArg(arg[0], XtNiconPixmap,
		XCreateBitmapFromData(gui.dpy,
		    Default_મૂળ_Window(gui.dpy),
		    (અક્ષર *)vim_icon_bits,
		    vim_icon_width,
		    vim_icon_height));
	XtSetArg(arg[1], XtNiconMask,
		XCreateBitmapFromData(gui.dpy,
		    Default_મૂળ_Window(gui.dpy),
		    (અક્ષર *)vim_mask_icon_bits,
		    vim_mask_icon_width,
		    vim_mask_icon_height));
	Xt_Set_કિંમતો(vimShell, arg, (Cardinal)2);
#નહીંતર
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    સ્થિર Pixmap	icon = 0;
    સ્થિર Pixmap	icon_mask = 0;
    સ્થિર અક્ષર		**magick = vim32x32;
    Window		મૂળ_window;
    XIconSize		*size;
    પૂર્ણાંક			સંખ્યા_sizes;
    Display		*dsp;
    Screen		*scr;
    XpmAttributes	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    મૂળ_window = XRootWindowOfScreen(XtScreen(vimShell));
    જો (XGetIconSizes(XtDisplay(vimShell), મૂળ_window,
						   &size, &સંખ્યા_sizes) != 0)
    {
	જો (સંખ્યા_sizes > 0)
	{
	    જો (size->max_height >= 48 && size->max_width >= 48)
		magick = vim48x48;
	    નહીંતર જો (size->max_height >= 32 && size->max_width >= 32)
		magick = vim32x32;
	    નહીંતર જો (size->max_height >= 16 && size->max_width >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_Set_કિંમતો(vimShell, XtNcolormap, cmap, ખાલી);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.depth = DefaultDepthOfScreen(scr);

    જો (!icon)
    {
	XpmCreatePixmapFromData(dsp, મૂળ_window, magick, &icon,
							   &icon_mask, &attr);
	XpmFreeAttributes(&attr);
    }

    Xt_Va_Set_કિંમતો(vimShell, XmNiconPixmap, icon, XmNiconMask, icon_mask, ખાલી);
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
    જો (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_Add_આગત(app_context, xsmp_icefd,
		(XtPointer)Xt_આગત_Read_Mask, local_xsmp_handle_requests, ખાલી);
#endif

    પાછા OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    શૂન્ય
gui_mch_uninit(શૂન્ય)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ખાલી;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    શૂન્ય
gui_mch_new_colors(શૂન્ય)
{
    long_u	gc_mask;
    XGCValues	gc_vals;

    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    જો (gui.text_gc != ખાલી)
	XChangeGC(gui.dpy, gui.text_gc, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    જો (gui.back_gc != ખાલી)
	XChangeGC(gui.dpy, gui.back_gc, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    જો (gui.invert_gc != ખાલી)
	XChangeGC(gui.dpy, gui.invert_gc, gc_mask, &gc_vals);

    gui_x11_set_back_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    પૂર્ણાંક
gui_mch_open(શૂન્ય)
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
							     ખાલી);
#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(vimShell, (EventMask)0, True, _XEditResCheckMessages,
	    (XtPointer)ખાલી);
#endif

#ifdef FEAT_CLIENTSERVER
    જો (serverName == ખાલી && serverDelayedStartName != ખાલી)
    {
	// This is a :gui command in a plain vim with no previous server
	commWindow = XtWindow(vimShell);
	(શૂન્ય)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    નહીંતર
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(gui.dpy, XtWindow(vimShell));
    }
    XtAddEventHandler(vimShell, PropertyChangeMask, False,
		      gui_x11_send_event_handler, ખાલી);
#endif

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ifdef FEAT_XIM
    xim_init();
#endif

    પાછા OK;
}

#જો defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    શૂન્ય
gui_init_tooltip_font(શૂન્ય)
{
    Xrm_કિંમત from, to;

    from.addr = (અક્ષર *)gui.rsrc_tooltip_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.tooltip_fontset;
    to.size = કદ(XFontSet);

    જો (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
}
#endif

#જો defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    શૂન્ય
gui_init_menu_font(શૂન્ય)
{
    Xrm_કિંમત from, to;

# ifdef FONTSET_ALWAYS
    from.addr = (અક્ષર *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_fontset;
    to.size = કદ(GuiFontset);

    જો (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
# નહીંતર
    from.addr = (અક્ષર *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_font;
    to.size = કદ(GuiFont);

    જો (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &to) == False)
    {
	// Failed. What to do?
    }
# endif
}
#endif

    શૂન્ય
gui_mch_exit(પૂર્ણાંક rc UNUSED)
{
#જો 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#endif
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_કિંમતો(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ખાલી);
    *x = xpos;
    *y = ypos;
    પાછા OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    Xt_Va_Set_કિંમતો(vimShell,
	XtNx,	x,
	XtNy,	y,
	ખાલી);
}

    શૂન્ય
gui_mch_set_shellsize(
    પૂર્ણાંક		width,
    પૂર્ણાંક		height,
    પૂર્ણાંક		min_width,
    પૂર્ણાંક		min_height,
    પૂર્ણાંક		base_width,
    પૂર્ણાંક		base_height,
    પૂર્ણાંક		direction UNUSED)
{
#ifdef FEAT_XIM
    height += xim_get_status_area_height(),
#endif
    Xt_Va_Set_કિંમતો(vimShell,
	XtNwidthInc,	gui.char_width,
	XtNheightInc,	gui.char_height,
#જો defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	XtNbaseWidth,	base_width,
	XtNbaseHeight,	base_height,
#endif
	XtNminWidth,	min_width,
	XtNminHeight,	min_height,
	XtNwidth,	width,
	XtNheight,	height,
	ખાલી);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    શૂન્ય
gui_mch_get_screen_dimensions(
    પૂર્ણાંક	    *screen_w,
    પૂર્ણાંક	    *screen_h)
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
    પૂર્ણાંક
gui_mch_init_font(
    char_u	*font_name,
    પૂર્ણાંક		do_fontset UNUSED)
{
    XFontStruct	*font = ખાલી;

#ifdef FEAT_XFONTSET
    XFontSet	fontset = ખાલી;
#endif

#ifdef FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    જો (font_name != ખાલી && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	જો (font_name == ખાલી && !gui.starting)
	    પાછા OK;
    }
#endif

#ifdef FEAT_XFONTSET
    જો (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	જો (font_name != ખાલી)
	{
	    fontset = (XFontSet)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    જો (fontset == ખાલી)
		પાછા FAIL;
	}
    }
    નહીંતર
#endif
    {
	જો (font_name == ખાલી)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    જો (font == ખાલી)
		font_name = (char_u *)DFLT_FONT;
	}
	જો (font == ખાલી)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	જો (font == ખાલી)
	    પાછા FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    જો (fontset != ખાલી)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_width = fontset_width(fontset);
	gui.char_height = fontset_height(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    નહીંતર
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
    જો (font_name == gui.rsrc_font_name)
    {
	જો (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ખાલી
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	જો (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ખાલી
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	જો (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ખાલી
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    નહીંતર
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	જો (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	જો (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	જો (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ifdef FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#endif

    પાછા OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, પૂર્ણાંક giveErrorIfMissing)
{
    XFontStruct	*font;

    જો (!gui.in_use || name == ખાલી)	// can't do this when GUI not running
	પાછા NOFONT;

    font = XLoadQueryFont(gui.dpy, (અક્ષર *)name);

    જો (font == ખાલી)
    {
	જો (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	પાછા NOFONT;
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

    જો (font->max_bounds.width != font->min_bounds.width)
    {
	semsg(_(e_font_str_is_not_fixed_width), name);
	XFreeFont(gui.dpy, font);
	પાછા NOFONT;
    }
    પાછા (GuiFont)font;
}

#જો defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ખાલી;

    જો (name != ખાલી && font == ખાલી)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    નહીંતર જો (font != ખાલી)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	unsigned long કિંમત = 0L;

	જો (XGetFontProperty(font, XA_FONT, &કિંમત))
	{
	    અક્ષર *xa_font_name = ખાલી;

	    xa_font_name = XGetAtomName(gui.dpy, કિંમત);
	    જો (xa_font_name != ખાલી)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    નહીંતર જો (name != ખાલી)
		ret = vim_strsave(name);
	}
	નહીંતર જો (name != ખાલી)
	    ret = vim_strsave(name);
    }
    પાછા ret;
}
#endif

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    પૂર્ણાંક
gui_mch_adjust_charheight(શૂન્ય)
{
#ifdef FEAT_XFONTSET
    જો (gui.fontset != NOFONTSET)
    {
	gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
							    + p_linespace / 2;
    }
    નહીંતર
#endif
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    પાછા OK;
}

/*
 * Set the current text font.
 */
    શૂન્ય
gui_mch_set_font(GuiFont font)
{
    સ્થિર Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    જો (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.text_gc, fid);
	XSetFont(gui.dpy, gui.back_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ifdef FEAT_XFONTSET
    current_fontset = (XFontSet)ખાલી;
#endif
}

#જો defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    શૂન્ય
gui_mch_set_fontset(GuiFontset fontset)
{
    current_fontset = (XFontSet)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    શૂન્ય
gui_mch_free_font(GuiFont font)
{
    જો (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#જો defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    શૂન્ય
gui_mch_free_fontset(GuiFontset fontset)
{
    જો (fontset != NOFONTSET)
	XFreeFontSet(gui.dpy, (XFontSet)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    પૂર્ણાંક		giveErrorIfMissing,
    પૂર્ણાંક		fixed_width)
{
    XFontSet	fontset;
    અક્ષર	**missing, *def_str;
    પૂર્ણાંક		અંક_missing;

    જો (!gui.in_use || name == ખાલી)
	પાછા NOFONTSET;

    fontset = XCreateFontSet(gui.dpy, (અક્ષર *)name, &missing, &અંક_missing,
			     &def_str);
    જો (અંક_missing > 0)
    {
	પૂર્ણાંક i;

	જો (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_in_fontset), name);
	    માટે (i = 0; i < અંક_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    જો (fontset == ખાલી)
    {
	જો (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	પાછા NOFONTSET;
    }

    જો (fixed_width && check_fontset_sanity(fontset) == FAIL)
    {
	XFreeFontSet(gui.dpy, fontset);
	પાછા NOFONTSET;
    }
    પાછા (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    સ્થિર પૂર્ણાંક
check_fontset_sanity(XFontSet fs)
{
    XFontStruct	**xfs;
    અક્ષર	**font_name;
    પૂર્ણાંક		fn;
    અક્ષર	*base_name;
    પૂર્ણાંક		i;
    પૂર્ણાંક		min_width;
    પૂર્ણાંક		min_font_idx = 0;

    base_name = XBaseFontNameListOfFontSet(fs);
    fn = XFontsOfFontSet(fs, &xfs, &font_name);
    માટે (i = 0; i < fn; i++)
    {
	જો (xfs[i]->max_bounds.width != xfs[i]->min_bounds.width)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_width),
		    base_name, font_name[i]);
	    પાછા FAIL;
	}
    }
    // scan base font width
    min_width = 32767;
    માટે (i = 0; i < fn; i++)
    {
	જો (xfs[i]->max_bounds.width<min_width)
	{
	    min_width = xfs[i]->max_bounds.width;
	    min_font_idx = i;
	}
    }
    માટે (i = 0; i < fn; i++)
    {
	જો (	   xfs[i]->max_bounds.width != 2 * min_width
		&& xfs[i]->max_bounds.width != min_width)
	{
	    semsg(_(e_fontset_name_str), base_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (પૂર્ણાંક)xfs[min_font_idx]->max_bounds.width);
	    semsg(_("Font%d width: %d"), i, (પૂર્ણાંક)xfs[i]->max_bounds.width);
	    પાછા FAIL;
	}
    }
    // it seems ok. Good Luck!!
    પાછા OK;
}

    સ્થિર પૂર્ણાંક
fontset_width(XFontSet fs)
{
 પાછા XmbTextEscapement(fs, "Vim", 3) / 3;
}

    પૂર્ણાંક
fontset_height(
    XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    પાછા extents->max_logical_extent.height;
}

# જો 0
// NOT USED YET
    સ્થિર પૂર્ણાંક
fontset_descent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet (fs);
    પાછા extents->max_logical_extent.height + extents->max_logical_extent.y;
}
# endif

    સ્થિર પૂર્ણાંક
fontset_ascent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    પાછા -extents->max_logical_extent.y;
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
    જો (!gui.in_use || name == ખાલી || *name == NUL)
	પાછા INVALCOLOR;

    requested = gui_get_color_cmn(name);
    જો (requested == INVALCOLOR)
	પાછા INVALCOLOR;

    પાછા gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
    XColor	available;
    Colormap	colormap;

#જો 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    અક્ષર	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, કદ(spec), "#%.2x%.2x%.2x", r, g, b);
    જો (XParseColor(gui.dpy, colormap, (અક્ષર *)spec, &available) != 0
	    && XAllocColor(gui.dpy, colormap, &available) != 0)
	પાછા (guicolor_T)available.pixel;
#endif
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_FIELD(available);
    available.red = r << 8;
    available.green = g << 8;
    available.blue = b << 8;
    જો (XAllocColor(gui.dpy, colormap, &available) != 0)
	પાછા (guicolor_T)available.pixel;

    પાછા INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    શૂન્ય
gui_mch_set_fg_color(guicolor_T color)
{
    જો (color == prev_fg_color)
	પાછા;

    XSetForeground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    શૂન્ય
gui_mch_set_bg_color(guicolor_T color)
{
    જો (color == prev_bg_color)
	પાછા;

    XSetBackground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    શૂન્ય
gui_mch_set_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    સ્થિર Cursor
gui_x11_create_blank_mouse(શૂન્ય)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (unsigned long)0, (XGCValues*)0);

    જો (gc != ખાલી)
    {
	XDrawPoint(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    પાછા XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    સ્થિર શૂન્ય
draw_curl(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક cells)
{
    પૂર્ણાંક			i;
    પૂર્ણાંક			offset;
    સ્થિર અચલ પૂર્ણાંક	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
    માટે (i = FILL_X(col); i < FILL_X(col + cells); ++i)
    {
	offset = val[i % 8];
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, i,
						FILL_Y(row + 1) - 1 - offset);
    }
    XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
}

    શૂન્ય
gui_mch_draw_string(
    પૂર્ણાંક		row,
    પૂર્ણાંક		col,
    char_u	*s,
    પૂર્ણાંક		len,
    પૂર્ણાંક		flags)
{
    પૂર્ણાંક			cells = len;
    સ્થિર શૂન્ય		*buf = ખાલી;
    સ્થિર પૂર્ણાંક		buflen = 0;
    char_u		*p;
    પૂર્ણાંક			wlen = 0;
    પૂર્ણાંક			c;

    જો (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	જો (buflen < len)
	{
	    XtFree((અક્ષર *)buf);
	    buf = (શૂન્ય *)XtMalloc(len * (કદ(XChar2b) < કદ(wchar_t)
					? કદ(wchar_t) : કદ(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	જ્યારે (p < s + len)
	{
	    c = utf_ptr2char(p);
#ifdef FEAT_XFONTSET
	    જો (current_fontset != ખાલી)
	    {
# ifdef SMALL_WCHAR_T
		જો (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    નહીંતર
#endif
	    {
		જો (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (unsigned)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    નહીંતર જો (has_mbyte)
    {
	cells = 0;
	માટે (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ifdef FEAT_XFONTSET
    જો (current_fontset != ખાલી)
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

    જો (flags & DRAW_TRANSP)
    {
	જો (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	નહીંતર
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (અક્ષર *)s, len);
    }
    નહીંતર જો (p_linespace != 0
#ifdef FEAT_XFONTSET
	    || current_fontset != ખાલી
#endif
	    )
    {
	XSetForeground(gui.dpy, gui.text_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		FILL_Y(row), gui.char_width * cells, gui.char_height);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);

	જો (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	નહીંતર
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (અક્ષર *)s, len);
    }
    નહીંતર
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	જો (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	નહીંતર
	    XDrawImageString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (અક્ષર *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    જો (flags & DRAW_BOLD)
    {
	જો (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), buf, wlen);
	નહીંતર
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), (અક્ષર *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    જો (flags & DRAW_UNDERC)
	draw_curl(row, col, cells);

    // Underline: draw a line at the bottom of the character cell.
    જો (flags & DRAW_UNDERL)
    {
	પૂર્ણાંક	y = FILL_Y(row + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	જો (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_લીટી(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
    }

    જો (flags & DRAW_STRIKE)
    {
	પૂર્ણાંક	y = FILL_Y(row + 1) - gui.char_height/2;

	XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
	XDraw_લીટી(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
    }

#ifdef FEAT_XFONTSET
    જો (current_fontset != ખાલી)
	XSetClipMask(gui.dpy, gui.text_gc, None);
#endif
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    પૂર્ણાંક
gui_mch_haskey(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != (ચાવી_Sym)0; i++)
	જો (name[0] == special_ચાવીઓ[i].vim_code0 &&
					 name[1] == special_ચાવીઓ[i].vim_code1)
	    પાછા OK;
    પાછા FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    પૂર્ણાંક
gui_get_x11_windis(Window *win, Display **dis)
{
    *win = XtWindow(vimShell);
    *dis = gui.dpy;
    પાછા OK;
}

    શૂન્ય
gui_mch_beep(શૂન્ય)
{
    XBell(gui.dpy, 0);
}

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((પૂર્ણાંક)Columns) + gui.border_offset,
	    FILL_Y((પૂર્ણાંક)Rows) + gui.border_offset);
    XSync(gui.dpy, False);
    ui_delay((long)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((પૂર્ણાંક)Columns) + gui.border_offset,
	    FILL_Y((પૂર્ણાંક)Rows) + gui.border_offset);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    શૂન્ય
gui_mch_invert_rectangle(
    પૂર્ણાંક	    r,
    પૂર્ણાંક	    c,
    પૂર્ણાંક	    nr,
    પૂર્ણાંક	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_width, (nr) * gui.char_height);
}

/*
 * Iconify the GUI window.
 */
    શૂન્ય
gui_mch_iconify(શૂન્ય)
{
    XIconifyWindow(gui.dpy, XtWindow(vimShell), DefaultScreen(gui.dpy));
}

#જો defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
{
    XMapRaised(gui.dpy, XtWindow(vimShell));
}
#endif

/*
 * Draw a cursor without focus.
 */
    શૂન્ય
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    પૂર્ણાંક		w = 1;

    જો (mb_lefthalve(gui.row, gui.col))
	w = 2;
    gui_mch_set_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(gui.col),
	    FILL_Y(gui.row), w * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    શૂન્ય
gui_mch_draw_part_cursor(પૂર્ણાંક w, પૂર્ણાંક h, guicolor_T color)
{
    gui_mch_set_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.text_gc,
#ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_જમણું ? FILL_X(gui.col + 1) - w :
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
    શૂન્ય
gui_mch_update(શૂન્ય)
{
    Xt_આગત_Mask mask, desired;

#ifdef ALT_X_આગત
    જો (suppress_alternate_આગત)
	desired = (XtIMXEvent | XtIMTimer);
    નહીંતર
#endif
	desired = (XtIMAll);
    જ્યારે ((mask = XtAppPending(app_context)) && (mask & desired)
	    && !vim_is_આગત_buf_full())
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
    પૂર્ણાંક
gui_mch_wait_for_chars(long wtime)
{
    પૂર્ણાંક	    focus;
    પૂર્ણાંક	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    સ્થિર પૂર્ણાંક	    timed_out;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_આગત_Mask	    desired;
#ifdef FEAT_JOB_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#endif

    timed_out = FALSE;

    જો (wtime >= 0)
	timer = XtAppAddTimeOut(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_out);
#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    જો (channel_any_keep_open())
	channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
				   channel_poll_cb, (XtPointer)&channel_timer);
#endif

    focus = gui.in_focus;
    desired = (XtIMAll);
    જ્યારે (!timed_out)
    {
	// Stop or start blinking when focus changes
	જો (gui.in_focus != focus)
	{
	    જો (gui.in_focus)
		gui_mch_start_blink();
	    નહીંતર
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	જો (did_add_timer)
	    // Need to recompute the waiting time.
	    તોડો;
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

	જો (આગત_available())
	{
	    retval = OK;
	    તોડો;
	}
    }

    જો (timer != (XtIntervalId)0 && !timed_out)
	XtRemoveTimeOut(timer);
#ifdef FEAT_JOB_CHANNEL
    જો (channel_timer != (XtIntervalId)0)
	XtRemoveTimeOut(channel_timer);
#endif

    પાછા retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    શૂન્ય
gui_mch_flush(શૂન્ય)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    શૂન્ય
gui_mch_clear_block(
    પૂર્ણાંક		row1,
    પૂર્ણાંક		col1,
    પૂર્ણાંક		row2,
    પૂર્ણાંક		col2)
{
    પૂર્ણાંક		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.back_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height);
}

    શૂન્ય
gui_mch_clear_all(શૂન્ય)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    શૂન્ય
gui_mch_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    જો (gui.visibility == VisibilityFullyObscured)
	પાછા;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ડાબું), FILL_Y(row + અંક_લીટીઓ),
	gui.char_width * (gui.scroll_region_જમણું - gui.scroll_region_ડાબું + 1)
			       + (gui.scroll_region_જમણું == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - અંક_લીટીઓ + 1),
	FILL_X(gui.scroll_region_ડાબું), FILL_Y(row));

    gui_clear_block(gui.scroll_region_bot - અંક_લીટીઓ + 1,
						       gui.scroll_region_ડાબું,
			  gui.scroll_region_bot, gui.scroll_region_જમણું);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
    જો (gui.visibility == VisibilityFullyObscured)
	પાછા;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	gui.char_width * (gui.scroll_region_જમણું - gui.scroll_region_ડાબું + 1)
			       + (gui.scroll_region_જમણું == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - અંક_લીટીઓ + 1),
	FILL_X(gui.scroll_region_ડાબું), FILL_Y(row + અંક_લીટીઓ));

    gui_clear_block(row, gui.scroll_region_ડાબું,
				row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    સ્થિર શૂન્ય
gui_x11_check_copy_area(શૂન્ય)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    જો (gui.visibility != VisibilityPartiallyObscured)
	પાછા;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    માટે (;;)
    {
	જો (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    પાછા;	// The scroll worked.

	જો (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    જો (gevent->ગણતરી == 0)
		પાછા;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    શૂન્ય
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    પૂર્ણાંક
clip_mch_own_selection(Clipboard_T *cbd)
{
    પાછા clip_x11_own_selection(vimShell, cbd);
}

    શૂન્ય
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    શૂન્ય
clip_mch_set_selection(
    Clipboard_T	*cbd)
{
    clip_x11_set_selection(cbd);
}

#જો defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    શૂન્ય
gui_mch_menu_grey(vimmenu_T *menu, પૂર્ણાંક grey)
{
    જો (menu->id == (Widget)0)
	પાછા;

    gui_mch_menu_hidden(menu, False);
    જો (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	XtSetSensitive(menu->id, False);
    નહીંતર
	XtSetSensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    શૂન્ય
gui_mch_menu_hidden(vimmenu_T *menu, પૂર્ણાંક hidden)
{
    જો (menu->id == (Widget)0)
	પાછા;

    જો (hidden)
	XtUnmanageChild(menu->id);
    નહીંતર
	XtManageChild(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    શૂન્ય
gui_mch_draw_menubar(શૂન્ય)
{
    // Nothing to do in X
}

    શૂન્ય
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
    સ્થિર શૂન્ય
gui_x11_wm_protocol_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    /*
     * Only deal with Client messages.
     */
    જો (event->type != ClientMessage)
	પાછા;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    જો ((Atom)((XClientMessageEvent *)event)->data.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	out_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, XtWindow(vimShell), ખાલી, 0);
	પાછા;
    }

    જો ((Atom)((XClientMessageEvent *)event)->data.l[0] !=
						  wm_atoms[DELETE_WINDOW_IDX])
	પાછા;

    gui_shell_closed();
}

#ifdef FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    સ્થિર શૂન્ય
gui_x11_send_event_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    જો (e->type == PropertyNotify && e->window == commWindow
	    && e->atom == commProperty &&  e->state == Property_New_કિંમત)
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

સ્થિર પૂર્ણાંક		blink_state = BLINK_NONE;
સ્થિર long_u		blink_waittime = 700;
સ્થિર long_u		blink_ontime = 400;
સ્થિર long_u		blink_offtime = 250;
સ્થિર XtIntervalId	blink_timer = (XtIntervalId)0;

    પૂર્ણાંક
gui_mch_is_blinking(શૂન્ય)
{
    પાછા blink_state != BLINK_NONE;
}

    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા blink_state == BLINK_OFF;
}

    શૂન્ય
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    જો (blink_timer != (XtIntervalId)0)
    {
	XtRemoveTimeOut(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    જો (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    સ્થિર શૂન્ય
gui_x11_blink_cb(
    XtPointer	    timed_out UNUSED,
    XtIntervalId    *interval_id UNUSED)
{
    જો (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = XtAppAddTimeOut(app_context, blink_offtime,
						      gui_x11_blink_cb, ખાલી);
    }
    નહીંતર
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = XtAppAddTimeOut(app_context, blink_ontime,
						      gui_x11_blink_cb, ખાલી);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    શૂન્ય
gui_mch_start_blink(શૂન્ય)
{
    જો (blink_timer != (XtIntervalId)0)
	XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = XtAppAddTimeOut(app_context, blink_waittime,
						      gui_x11_blink_cb, ખાલી);
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

    પાછા (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((unsigned)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    શૂન્ય
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
    જો (vim_strchr(p_go, GO_POINTER) != ખાલી)
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

    XtAddEventHandler(vimForm, ચાવી_Press_Mask, FALSE, gui_x11_ચાવી_hit_cb,
	(XtPointer)0);
    XtAddEventHandler(textArea, ચાવી_Press_Mask, FALSE, gui_x11_ચાવી_hit_cb,
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
    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    પૂર્ણાંક		rootx, rooty, winx, winy;
    Window	મૂળ, child;
    unsigned પૂર્ણાંક mask;

    જો (gui.wid && XQueryPointer(gui.dpy, gui.wid, &મૂળ, &child,
					 &rootx, &rooty, &winx, &winy, &mask))
    {
	*x = winx;
	*y = winy;
    }
    નહીંતર
    {
	*x = -1;
	*y = -1;
    }
}

    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x, પૂર્ણાંક y)
{
    જો (gui.wid)
	XWarpPointer(gui.dpy, (Window)0, gui.wid, 0, 0, 0, 0, x, y);
}

#જો defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_last_mouse_event(શૂન્ય)
{
    પાછા &last_mouse_event;
}
#endif

#જો defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_WIDTH (gui.char_width * 2)

    શૂન્ય
gui_mch_drawsign(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક typenr)
{
    XImage	*sign;

    જો (!gui.in_use || (sign = (XImage *)sign_get_image(typenr)) == ખાલી)
	પાછા;

    XClearArea(gui.dpy, gui.wid, TEXT_X(col), TEXT_Y(row) - sign->height,
	    SIGN_WIDTH, gui.char_height, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.text_gc, sign, 0, 0,
	    TEXT_X(col) + (SIGN_WIDTH - sign->width) / 2,
	    TEXT_Y(row) - sign->height,
	    sign->width, sign->height);
}

    શૂન્ય *
gui_mch_register_sign(char_u *signfile)
{
    XpmAttributes   attrs;
    XImage	    *sign = ખાલી;
    પૂર્ણાંક		    status;

    /*
     * Setup the color substitution table.
     */
    જો (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_પ્રતીક color[5] =
	{
	    {"none", ખાલી, 0},
	    {"iconColor1", ખાલી, 0},
	    {"bottomShadowColor", ખાલી, 0},
	    {"topShadowColor", ખાલી, 0},
	    {"selectColor", ખાલી, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.back_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	status = XpmReadFileToImage(gui.dpy, (અક્ષર *)signfile,
							 &sign, ખાલી, &attrs);
	જો (status == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	નહીંતર
	    emsg(_(e_couldnt_read_in_sign_data));
    }

    પાછા (શૂન્ય *)sign;
}

    શૂન્ય
gui_mch_destroy_sign(શૂન્ય *sign)
{
    XDestroyImage((XImage*)sign);
}
#endif


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
સ્થિર પૂર્ણાંક last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 */
    શૂન્ય
gui_mch_mousehide(
    પૂર્ણાંક		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    જો (gui.pointer_hidden == hide)
	પાછા;

    gui.pointer_hidden = hide;
    જો (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    નહીંતર
#ifdef FEAT_MOUSESHAPE
	mch_set_mouse_shape(last_shape);
#નહીંતર
    XUndefineCursor(gui.dpy, gui.wid);
#endif
}

#જો defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
સ્થિર પૂર્ણાંક mshape_ids[] =
{
    XC_ડાબું_ptr,		// arrow
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
    XC_જમણું_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ડાબું_ptr			// last one
};

    શૂન્ય
mch_set_mouse_shape(પૂર્ણાંક shape)
{
    પૂર્ણાંક	    id;

    જો (!gui.in_use)
	પાછા;

    જો (shape == MSHAPE_HIDE || gui.pointer_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    નહીંતર
    {
	જો (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    જો (id >= XC_અંક_glyphs)
		id = XC_ડાબું_ptr;
	    નહીંતર
		id &= ~1;	// they are always even (why?)
	}
	નહીંતર
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    જો (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

#જો defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    શૂન્ય
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    જો (menu->id == ખાલી || menu->parent == ખાલી
				|| !menu_is_toolbar(menu->parent->name))
	પાછા;

    // Always destroy and create the balloon, in case the string was
    // changed.
    જો (menu->tip != ખાલી)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ખાલી;
    }
    જો (menu->strings[MENU_INDEX_TIP] != ખાલી)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->strings[MENU_INDEX_TIP],
		ખાલી,
		ખાલી);
}
#endif
