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

#अन्तर्भाव "vim.h"

#अन्तर्भाव <X11/keysym.h>
#अन्तर्भाव <X11/Xatom.h>
#अन्तर्भाव <X11/StringDefs.h>
#अन्तर्भाव <X11/Intrinsic.h>
#अन्तर्भाव <X11/Shell.h>
#अन्तर्भाव <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#जइ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#अण्णहा
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  अण्णहा
#   include <X11/xpm.h>
#  endif
# endif
#endif

#ifdef FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# अण्णहा
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
#अण्णहा
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

स्थिर Atom   wm_atoms[2];	// Window Manager Atoms
#define DELETE_WINDOW_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#define SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#ifdef FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
स्थिर XFontSet current_fontset = रिक्त;
# जइ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	करेहि \
	{ \
	    जइ (current_fontset != रिक्त) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    अण्णहा \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} जाव (0)
# endif
# जइ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	करेहि \
	{ \
	    जइ (current_fontset != रिक्त) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    अण्णहा \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} जाव (0)
# endif
# जइ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	करेहि \
	{ \
	    जइ (current_fontset != रिक्त) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    अण्णहा \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} जाव (0)
# endif
स्थिर पूर्णाङ्क check_fontset_sanity(XFontSet fs);
स्थिर पूर्णाङ्क fontset_width(XFontSet fs);
स्थिर पूर्णाङ्क fontset_ascent(XFontSet fs);
#endif

स्थिर guicolor_T	prev_fg_color = INVALCOLOR;
स्थिर guicolor_T	prev_bg_color = INVALCOLOR;
स्थिर guicolor_T	prev_sp_color = INVALCOLOR;

#जइ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
स्थिर XButtonPressedEvent last_mouse_event;
#endif

स्थिर शून्य gui_x11_check_copy_area(शून्य);
#ifdef FEAT_CLIENTSERVER
स्थिर शून्य gui_x11_send_event_handler(Widget, XtPointer, XEvent *, Boolean *);
#endif
स्थिर शून्य gui_x11_wm_protocol_handler(Widget, XtPointer, XEvent *, Boolean *);
स्थिर Cursor gui_x11_create_blank_mouse(शून्य);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
स्थिर संरचना specialkey
{
    कुञ्चिका_Sym  कुञ्चिका_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_कुञ्चिकाः[] =
{
    {XK_Up,		'k', 'u'},
    {XK_Down,		'k', 'd'},
    {XK_वामः,		'k', 'l'},
    {XK_दक्षिणः,		'k', 'r'},

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
    {XK_निवेशय,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_End,		'@', '7'},
    {XK_Prior,		'k', 'P'},
    {XK_Next,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#ifdef XK_KP_वामः
    {XK_KP_वामः,	'k', 'l'},
    {XK_KP_दक्षिणः,	'k', 'r'},
    {XK_KP_Up,		'k', 'u'},
    {XK_KP_Down,	'k', 'd'},
    {XK_KP_निवेशय,	KS_EXTRA, (char_u)KE_KINS},
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
    {(कुञ्चिका_Sym)0,	    0, 0}
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
स्थिर XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	परिमाण(Pixel),
	XtOffsetOf(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	परिमाण(Pixel),
	XtOffsetOf(gui_T, def_back_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	परिमाण(String *),
	XtOffsetOf(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	परिमाण(String *),
	XtOffsetOf(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	परिमाण(String *),
	XtOffsetOf(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	परिमाण(String *),
	XtOffsetOf(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	परिमाण(String *),
	XtOffsetOf(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	परिमाण(Bool),
	XtOffsetOf(gui_T, rsrc_rev_video),
	XtRImmediate,
	(XtPointer)False
    },
    {
	XtNborderWidth,
	XtCBorderWidth,
	XtRInt,
	परिमाण(पूर्णाङ्क),
	XtOffsetOf(gui_T, border_width),
	XtRImmediate,
	(XtPointer)2
    },
    {
	XtNscrollbarWidth,
	XtCScrollbarWidth,
	XtRInt,
	परिमाण(पूर्णाङ्क),
	XtOffsetOf(gui_T, scrollbar_width),
	XtRImmediate,
	(XtPointer)SB_DEFAULT_WIDTH
    },
#ifdef FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	XtNmenuFontSet,
	XtCMenuFontSet,
# अण्णहा
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#endif
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#ifdef FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	परिमाण(वर्ण *),
	XtOffsetOf(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	XtCFontSet,
	XtRFontSet,
	परिमाण(XFontSet),
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
	परिमाण(वर्ण*),
	XtOffsetOf(gui_T, rsrc_preedit_type_name),
	XtRString,
	(XtPointer)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	परिमाण(वर्ण*),
	XtOffsetOf(gui_T, rsrc_निवेशः_method),
	XtRString,
	रिक्त
    },
#endif // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
स्थिर XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	रिक्त},
    {"-background",	".background",	    XrmoptionSepArg,	रिक्त},
    {"-fg",		".foreground",	    XrmoptionSepArg,	रिक्त},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	रिक्त},
    {"-fn",		".font",	    XrmoptionSepArg,	रिक्त},
    {"-font",		".font",	    XrmoptionSepArg,	रिक्त},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	रिक्त},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	रिक्त},
    {"-geom",		".geometry",	    XrmoptionSepArg,	रिक्त},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	रिक्त},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	रिक्त},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	रिक्त},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	रिक्त},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	रिक्त},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	रिक्त},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	रिक्त},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	रिक्त},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	रिक्त},
#ifdef FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	रिक्त},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	रिक्त},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	रिक्त},
#अण्णहा
    {"-mf",		".menuFont",	    XrmoptionSepArg,	रिक्त},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	रिक्त},
#endif
    {"-xrm",		रिक्त,		    XrmoptionResArg,	रिक्त}
};

स्थिर पूर्णाङ्क gui_argc = 0;
स्थिर वर्ण **gui_argv = रिक्त;

/*
 * Call-back routines.
 */

    स्थिर शून्य
gui_x11_timer_cb(
    XtPointer	    timed_out,
    XtIntervalId    *interval_id UNUSED)
{
    *((पूर्णाङ्क *)timed_out) = TRUE;
}

#ifdef FEAT_JOB_CHANNEL
    स्थिर शून्य
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

    स्थिर शून्य
gui_x11_visibility_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    जइ (event->type != VisibilityNotify)
	प्रत्यागम;

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

    स्थिर शून्य
gui_x11_expose_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XExposeEvent	*gevent;
    पूर्णाङ्क			new_x;

    जइ (event->type != Expose)
	प्रत्यागम;

    out_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    जइ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    जइ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    जइ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((पूर्णाङ्क)Columns), 0, 0, 0, False);
    जइ (gevent->y > FILL_Y(Rows))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((पूर्णाङ्क)Rows), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#जइ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    स्थिर शून्य
shellRectangle(Widget shell, XRectangle *r)
{
    Window		rootw, shellw, child, parentw;
    पूर्णाङ्क			absx, absy;
    XWindowAttributes	a;
    Window		*children;
    unsigned पूर्णाङ्क	children_गणनम्;

    shellw = XtWindow(shell);
    जइ (shellw == 0)
	प्रत्यागम;
    प्रति (;;)
    {
	XQuery_वृक्षः(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_गणनम्);
	XFree(children);
	जइ (parentw == rootw)
	    भङ्ग;
	shellw = parentw;
    }
    XGetWindowAttributes(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.मूलम्, 0, 0,
							&absx, &absy, &child);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_मानानि(shell, XmNheight, &r->height, XmNwidth, &r->width, रिक्त);
}
#endif

    स्थिर शून्य
gui_x11_resize_window_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    स्थिर पूर्णाङ्क lastWidth, lastHeight;

    जइ (event->type != ConfigureNotify)
	प्रत्यागम;

    जइ (event->xconfigure.width != lastWidth
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
#जइ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    जइ (netbeans_active())
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

    स्थिर शून्य
gui_x11_focus_change_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    gui_focus_change(event->type == FocusIn);
}

    स्थिर शून्य
gui_x11_enter_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(TRUE);
}

    स्थिर शून्य
gui_x11_leave_cb(
    Widget	w UNUSED,
    XtPointer	data UNUSED,
    XEvent	*event UNUSED,
    Boolean	*dum UNUSED)
{
    gui_focus_change(FALSE);
}

#जइ defined(X_HAVE_UTF8_STRING)
# जइ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#endif

    शून्य
gui_x11_कुञ्चिका_hit_cb(
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
#अण्णहा
    char_u		string[4], string2[3];
#endif
    कुञ्चिका_Sym		कुञ्चिका_sym;
    पूर्णाङ्क			len;
    पूर्णाङ्क			i;
    पूर्णाङ्क			modifiers;
    पूर्णाङ्क			कुञ्चिका;

    ev_press = (XKeyPressedEvent *)event;

#ifdef FEAT_XIM
    जइ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	जइ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (वर्ण *)string,
				  परिमाण(string_shortbuf), &कुञ्चिका_sym, &status);
	अण्णहा
# endif
	    len = XmbLookupString(xic, ev_press, (वर्ण *)string,
				  परिमाण(string_shortbuf), &कुञ्चिका_sym, &status);
	जइ (status == XBufferOverflow)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    जइ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (वर्ण *)string,
						      len, &कुञ्चिका_sym, &status);
	    अण्णहा
# endif
		len = XmbLookupString(xic, ev_press, (वर्ण *)string,
						      len, &कुञ्चिका_sym, &status);
	}
	जइ (status == XLookupNone || status == XLookupChars)
	    कुञ्चिका_sym = XK_Void_चिह्नम्;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	जइ (len > 0 && निवेशः_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    पूर्णाङ्क		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    जइ (string_alloced)
		XtFree((वर्ण *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_निवेशः(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	प्रति (i = 0; i < len; ++i)
	    जइ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (पूर्णाङ्क)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		जइ (string_alloced)
		    XtFree((वर्ण *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    अण्णहा
#endif
	len = XLookupString(ev_press, (वर्ण *)string, परिमाण(string),
		&कुञ्चिका_sym, रिक्त);

#ifdef SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    जइ (कुञ्चिका_sym == SunXK_F36 || कुञ्चिका_sym == SunXK_F37)
	len = 0;
#endif

    जइ (कुञ्चिका_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    जइ (कुञ्चिका_sym == XK_minus && (ev_press->state & ControlMask))
	string[0] = Ctrl__;

#ifdef XK_ISO_वामः_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    जइ (कुञ्चिका_sym == XK_ISO_वामः_Tab)
    {
	कुञ्चिका_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    जइ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (पूर्णाङ्क)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    जइ (len == 0 || len == 1)
    {
	प्रति (i = 0; special_कुञ्चिकाः[i].कुञ्चिका_sym != (कुञ्चिका_Sym)0; i++)
	{
	    जइ (special_कुञ्चिकाः[i].कुञ्चिका_sym == कुञ्चिका_sym)
	    {
		string[0] = CSI;
		string[1] = special_कुञ्चिकाः[i].vim_code0;
		string[2] = special_कुञ्चिकाः[i].vim_code1;
		len = -3;
		भङ्ग;
	    }
	}
    }

    // Unrecognised key is ignored.
    जइ (len == 0)
	गच्छ theend;

    // Handle modifiers.
    modifiers = 0;
    जइ (ev_press->state & ShiftMask)
	modifiers |= MOD_MASK_SHIFT;
    जइ (ev_press->state & ControlMask)
    {
	modifiers |= MOD_MASK_CTRL;
	जइ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    जइ (ev_press->state & Mod1Mask)
	modifiers |= MOD_MASK_ALT;
    जइ (ev_press->state & Mod4Mask)
	modifiers |= MOD_MASK_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    जइ (len == -3)
	कुञ्चिका = TO_SPECIAL(string[1], string[2]);
    अण्णहा
    {
	string[len] = NUL;
	कुञ्चिका = mb_ptr2char(string);
    }
    कुञ्चिका = simplify_कुञ्चिका(कुञ्चिका, &modifiers);
    जइ (कुञ्चिका == CSI)
	कुञ्चिका = K_CSI;
    जइ (IS_SPECIAL(कुञ्चिका))
    {
	string[0] = CSI;
	string[1] = K_SECOND(कुञ्चिका);
	string[2] = K_THIRD(कुञ्चिका);
	len = 3;
    }
    अण्णहा
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	कुञ्चिका = may_adjust_कुञ्चिका_for_ctrl(modifiers, कुञ्चिका);

	len = mb_char2bytes(कुञ्चिका, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_shift_modifier(modifiers, कुञ्चिका);
    }

    जइ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_निवेशः_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	पूर्णाङ्क int_ch = check_for_interrupt(कुञ्चिका, modifiers);

	जइ (int_ch != NUL)
	{
	    trash_निवेशः_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_निवेशः_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    जइ (p_mh)
	gui_mch_mousehide(TRUE);

#जइ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	जइ ((be = gui_mch_currently_showing_beval()) != रिक्त)
	    gui_mch_unpost_balloon(be);
    }
#endif
theend:
    {}	    // some compilers need a statement here
#ifdef FEAT_XIM
    जइ (string_alloced)
	XtFree((वर्ण *)string);
#endif
}

    स्थिर शून्य
gui_x11_mouse_cb(
    Widget	w UNUSED,
    XtPointer	dud UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    स्थिर XtIntervalId timer = (XtIntervalId)0;
    स्थिर पूर्णाङ्क	timed_out = TRUE;

    पूर्णाङ्क		button;
    पूर्णाङ्क		repeated_click = FALSE;
    पूर्णाङ्क		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    जइ (event->type == MotionNotify)
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

	जइ (button != MOUSE_DRAG)	// just moving the rodent
	{
#ifdef FEAT_MENU
	    जइ (dud)			// moved in vimForm
		y -= gui.menu_height;
#endif
	    gui_mouse_moved(x, y);
	    प्रत्यागम;
	}
    }
    अण्णहा
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	जइ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    जइ (!timed_out)
	    {
		XtRemoveTimeOut(timer);
		repeated_click = TRUE;
	    }
	    timed_out = FALSE;
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_out);
	    विकल्प (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		पक्ष Button1:	button = MOUSE_वामः;	भङ्ग;
		पक्ष Button2:	button = MOUSE_मध्यः;	भङ्ग;
		पक्ष Button3:	button = MOUSE_दक्षिणः;	भङ्ग;
		पक्ष Button4:	button = MOUSE_4;	भङ्ग;
		पक्ष Button5:	button = MOUSE_5;	भङ्ग;
		पक्ष 6:		button = MOUSE_7;	भङ्ग;
		पक्ष 7:		button = MOUSE_6;	भङ्ग;
		पक्ष 8:		button = MOUSE_X1;	भङ्ग;
		पक्ष 9:		button = MOUSE_X2;	भङ्ग;
		स्वभाव:
		    प्रत्यागम;	// Unknown button
	    }
	}
	अण्णहा जइ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	अण्णहा
	    प्रत्यागम;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#जइ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	last_mouse_event = event->xbutton;
#endif
    }

    vim_modifiers = 0x0;
    जइ (x_modifiers & ShiftMask)
	vim_modifiers |= MOUSE_SHIFT;
    जइ (x_modifiers & ControlMask)
	vim_modifiers |= MOUSE_CTRL;
    जइ (x_modifiers & Mod1Mask)	    // Alt or Meta key
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
    शून्य
gui_mch_prepare(पूर्णाङ्क *argc, वर्ण **argv)
{
    पूर्णाङ्क	    arg;
    पूर्णाङ्क	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(वर्ण *, *argc);
    जइ (gui_argv == रिक्त)
	प्रत्यागम;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    जाव (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	प्रति (i = 0; i < (पूर्णाङ्क)Xt_सङ्ख्या(cmdline_options); i++)
	    जइ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		भङ्ग;

	जइ (i < (पूर्णाङ्क)Xt_सङ्ख्या(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    जइ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		found_reverse_arg = TRUE;
	    अण्णहा जइ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_argument = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    जइ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * परिमाण(वर्ण *));
		जइ (cmdline_options[i].argKind != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    जइ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * परिमाण(वर्ण *));
		}
	    }
	    argv[*argc] = रिक्त;
	}
	अण्णहा
#ifdef FEAT_NETBEANS_INTG
	    जइ (strncmp("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * परिमाण(वर्ण *));
	    argv[*argc] = रिक्त;
	}
	अण्णहा
#endif
	    arg++;
    }
}

#ifndef XtSpecificationRelease
# define CARDINAL (Cardinal *)
#अण्णहा
# जइ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# अण्णहा
#  define CARDINAL (पूर्णाङ्क *)
# endif
#endif

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
gui_mch_init_check(शून्य)
{
#ifdef FEAT_XIM
    XtSetLanguageProc(रिक्त, रिक्त, रिक्त);
#endif
    open_app_context();
    जइ (app_context != रिक्त)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_सङ्ख्या(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#जइ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	वर्ण *p = setlocale(LC_NUMERIC, रिक्त);

	जइ (p == रिक्त || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#endif
    जइ (app_context == रिक्त || gui.dpy == रिक्त)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}


#ifdef USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
स्थिर Xt_निवेशः_Id _xsmp_xtinputid;

    स्थिर शून्य
local_xsmp_handle_requests(
    XtPointer	c UNUSED,
    पूर्णाङ्क		*s UNUSED,
    Xt_निवेशः_Id	*i UNUSED)
{
    जइ (xsmp_handle_requests() == FAIL)
	Xt_Remove_निवेशः(_xsmp_xtinputid);
}
#endif


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    पूर्णाङ्क
gui_mch_init(शून्य)
{
    XtGCMask	gc_mask;
    XGCValues	gc_vals;
    पूर्णाङ्क		x, y, mask;
    unsigned	w, h;

#जइ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#endif

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, रिक्त);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (XtPointer)&gui,
	vim_resources, Xt_सङ्ख्या(vim_resources), रिक्त);

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
    जइ (gui.rsrc_rev_video && gui_get_lightness(gui.back_pixel)
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

    जइ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_set_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    जइ (gui.geom != रिक्त && *gui.geom != NUL)
    {
	mask = XParseGeometry((वर्ण *)gui.geom, &x, &y, &w, &h);
	जइ (mask & Width_मानम्)
	    Columns = w;
	जइ (mask & Height_मानम्)
	{
	    जइ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	जइ (mask & (XValue|YValue))
	    Xt_Va_Set_मानानि(vimShell, XtNgeometry, gui.geom, रिक्त);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    जइ (vim_strchr(p_go, GO_ICON) != रिक्त)
    {
#ifndef HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	XtSetArg(arg[0], XtNiconPixmap,
		XCreateBitmapFromData(gui.dpy,
		    Default_मूलम्_Window(gui.dpy),
		    (वर्ण *)vim_icon_bits,
		    vim_icon_width,
		    vim_icon_height));
	XtSetArg(arg[1], XtNiconMask,
		XCreateBitmapFromData(gui.dpy,
		    Default_मूलम्_Window(gui.dpy),
		    (वर्ण *)vim_mask_icon_bits,
		    vim_mask_icon_width,
		    vim_mask_icon_height));
	Xt_Set_मानानि(vimShell, arg, (Cardinal)2);
#अण्णहा
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    स्थिर Pixmap	icon = 0;
    स्थिर Pixmap	icon_mask = 0;
    स्थिर वर्ण		**magick = vim32x32;
    Window		मूलम्_window;
    XIconSize		*size;
    पूर्णाङ्क			सङ्ख्या_sizes;
    Display		*dsp;
    Screen		*scr;
    XpmAttributes	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    मूलम्_window = XRootWindowOfScreen(XtScreen(vimShell));
    जइ (XGetIconSizes(XtDisplay(vimShell), मूलम्_window,
						   &size, &सङ्ख्या_sizes) != 0)
    {
	जइ (सङ्ख्या_sizes > 0)
	{
	    जइ (size->max_height >= 48 && size->max_width >= 48)
		magick = vim48x48;
	    अण्णहा जइ (size->max_height >= 32 && size->max_width >= 32)
		magick = vim32x32;
	    अण्णहा जइ (size->max_height >= 16 && size->max_width >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_Set_मानानि(vimShell, XtNcolormap, cmap, रिक्त);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | XpmDepth;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.depth = DefaultDepthOfScreen(scr);

    जइ (!icon)
    {
	XpmCreatePixmapFromData(dsp, मूलम्_window, magick, &icon,
							   &icon_mask, &attr);
	XpmFreeAttributes(&attr);
    }

    Xt_Va_Set_मानानि(vimShell, XmNiconPixmap, icon, XmNiconMask, icon_mask, रिक्त);
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
    जइ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_Add_निवेशः(app_context, xsmp_icefd,
		(XtPointer)Xt_निवेशः_Read_Mask, local_xsmp_handle_requests, रिक्त);
#endif

    प्रत्यागम OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    शून्य
gui_mch_uninit(शून्य)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = रिक्त;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    शून्य
gui_mch_new_colors(शून्य)
{
    long_u	gc_mask;
    XGCValues	gc_vals;

    gc_mask = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.back_pixel;
    जइ (gui.text_gc != रिक्त)
	XChangeGC(gui.dpy, gui.text_gc, gc_mask, &gc_vals);

    gc_vals.foreground = gui.back_pixel;
    gc_vals.background = gui.norm_pixel;
    जइ (gui.back_gc != रिक्त)
	XChangeGC(gui.dpy, gui.back_gc, gc_mask, &gc_vals);

    gc_mask |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.back_pixel;
    gc_vals.function   = GXxor;
    जइ (gui.invert_gc != रिक्त)
	XChangeGC(gui.dpy, gui.invert_gc, gc_mask, &gc_vals);

    gui_x11_set_back_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    पूर्णाङ्क
gui_mch_open(शून्य)
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
							     रिक्त);
#ifdef HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    XtAddEventHandler(vimShell, (EventMask)0, True, _XEditResCheckMessages,
	    (XtPointer)रिक्त);
#endif

#ifdef FEAT_CLIENTSERVER
    जइ (serverName == रिक्त && serverDelayedStartName != रिक्त)
    {
	// This is a :gui command in a plain vim with no previous server
	commWindow = XtWindow(vimShell);
	(शून्य)serverRegisterName(gui.dpy, serverDelayedStartName);
    }
    अण्णहा
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	serverChangeRegisteredWindow(gui.dpy, XtWindow(vimShell));
    }
    XtAddEventHandler(vimShell, PropertyChangeMask, False,
		      gui_x11_send_event_handler, रिक्त);
#endif

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#ifdef FEAT_XIM
    xim_init();
#endif

    प्रत्यागम OK;
}

#जइ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    शून्य
gui_init_tooltip_font(शून्य)
{
    Xrm_मानम् from, to;

    from.addr = (वर्ण *)gui.rsrc_tooltip_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.tooltip_fontset;
    to.size = परिमाण(XFontSet);

    जइ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
}
#endif

#जइ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    शून्य
gui_init_menu_font(शून्य)
{
    Xrm_मानम् from, to;

# ifdef FONTSET_ALWAYS
    from.addr = (वर्ण *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_fontset;
    to.size = परिमाण(GuiFontset);

    जइ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontSet, &to) == False)
    {
	// Failed. What to do?
    }
# अण्णहा
    from.addr = (वर्ण *)gui.rsrc_menu_font_name;
    from.size = strlen(from.addr);
    to.addr = (XtPointer)&gui.menu_font;
    to.size = परिमाण(GuiFont);

    जइ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &to) == False)
    {
	// Failed. What to do?
    }
# endif
}
#endif

    शून्य
gui_mch_exit(पूर्णाङ्क rc UNUSED)
{
#जइ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#endif
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    पूर्णाङ्क
gui_mch_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_मानानि(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	रिक्त);
    *x = xpos;
    *y = ypos;
    प्रत्यागम OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    शून्य
gui_mch_set_winpos(पूर्णाङ्क x, पूर्णाङ्क y)
{
    Xt_Va_Set_मानानि(vimShell,
	XtNx,	x,
	XtNy,	y,
	रिक्त);
}

    शून्य
gui_mch_set_shellsize(
    पूर्णाङ्क		width,
    पूर्णाङ्क		height,
    पूर्णाङ्क		min_width,
    पूर्णाङ्क		min_height,
    पूर्णाङ्क		base_width,
    पूर्णाङ्क		base_height,
    पूर्णाङ्क		direction UNUSED)
{
#ifdef FEAT_XIM
    height += xim_get_status_area_height(),
#endif
    Xt_Va_Set_मानानि(vimShell,
	XtNwidthInc,	gui.char_width,
	XtNheightInc,	gui.char_height,
#जइ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	XtNbaseWidth,	base_width,
	XtNbaseHeight,	base_height,
#endif
	XtNminWidth,	min_width,
	XtNminHeight,	min_height,
	XtNwidth,	width,
	XtNheight,	height,
	रिक्त);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    शून्य
gui_mch_get_screen_dimensions(
    पूर्णाङ्क	    *screen_w,
    पूर्णाङ्क	    *screen_h)
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
    पूर्णाङ्क
gui_mch_init_font(
    char_u	*font_name,
    पूर्णाङ्क		do_fontset UNUSED)
{
    XFontStruct	*font = रिक्त;

#ifdef FEAT_XFONTSET
    XFontSet	fontset = रिक्त;
#endif

#ifdef FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    जइ (font_name != रिक्त && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	जइ (font_name == रिक्त && !gui.starting)
	    प्रत्यागम OK;
    }
#endif

#ifdef FEAT_XFONTSET
    जइ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	जइ (font_name != रिक्त)
	{
	    fontset = (XFontSet)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    जइ (fontset == रिक्त)
		प्रत्यागम FAIL;
	}
    }
    अण्णहा
#endif
    {
	जइ (font_name == रिक्त)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    जइ (font == रिक्त)
		font_name = (char_u *)DFLT_FONT;
	}
	जइ (font == रिक्त)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	जइ (font == रिक्त)
	    प्रत्यागम FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#ifdef FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    जइ (fontset != रिक्त)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_width = fontset_width(fontset);
	gui.char_height = fontset_height(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    अण्णहा
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
    जइ (font_name == gui.rsrc_font_name)
    {
	जइ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != रिक्त
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	जइ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != रिक्त
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	जइ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != रिक्त
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    अण्णहा
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	जइ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	जइ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	जइ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#ifdef FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#endif

    प्रत्यागम OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, पूर्णाङ्क giveErrorIfMissing)
{
    XFontStruct	*font;

    जइ (!gui.in_use || name == रिक्त)	// can't do this when GUI not running
	प्रत्यागम NOFONT;

    font = XLoadQueryFont(gui.dpy, (वर्ण *)name);

    जइ (font == रिक्त)
    {
	जइ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	प्रत्यागम NOFONT;
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

    जइ (font->max_bounds.width != font->min_bounds.width)
    {
	semsg(_(e_font_str_is_not_fixed_width), name);
	XFreeFont(gui.dpy, font);
	प्रत्यागम NOFONT;
    }
    प्रत्यागम (GuiFont)font;
}

#जइ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = रिक्त;

    जइ (name != रिक्त && font == रिक्त)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    अण्णहा जइ (font != रिक्त)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	unsigned long मानम् = 0L;

	जइ (XGetFontProperty(font, XA_FONT, &मानम्))
	{
	    वर्ण *xa_font_name = रिक्त;

	    xa_font_name = XGetAtomName(gui.dpy, मानम्);
	    जइ (xa_font_name != रिक्त)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    अण्णहा जइ (name != रिक्त)
		ret = vim_strsave(name);
	}
	अण्णहा जइ (name != रिक्त)
	    ret = vim_strsave(name);
    }
    प्रत्यागम ret;
}
#endif

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    पूर्णाङ्क
gui_mch_adjust_charheight(शून्य)
{
#ifdef FEAT_XFONTSET
    जइ (gui.fontset != NOFONTSET)
    {
	gui.char_height = fontset_height((XFontSet)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFontSet)gui.fontset)
							    + p_linespace / 2;
    }
    अण्णहा
#endif
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_height = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    प्रत्यागम OK;
}

/*
 * Set the current text font.
 */
    शून्य
gui_mch_set_font(GuiFont font)
{
    स्थिर Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    जइ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.text_gc, fid);
	XSetFont(gui.dpy, gui.back_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#ifdef FEAT_XFONTSET
    current_fontset = (XFontSet)रिक्त;
#endif
}

#जइ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    शून्य
gui_mch_set_fontset(GuiFontset fontset)
{
    current_fontset = (XFontSet)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    शून्य
gui_mch_free_font(GuiFont font)
{
    जइ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#जइ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    शून्य
gui_mch_free_fontset(GuiFontset fontset)
{
    जइ (fontset != NOFONTSET)
	XFreeFontSet(gui.dpy, (XFontSet)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    पूर्णाङ्क		giveErrorIfMissing,
    पूर्णाङ्क		fixed_width)
{
    XFontSet	fontset;
    वर्ण	**missing, *def_str;
    पूर्णाङ्क		अङ्कः_missing;

    जइ (!gui.in_use || name == रिक्त)
	प्रत्यागम NOFONTSET;

    fontset = XCreateFontSet(gui.dpy, (वर्ण *)name, &missing, &अङ्कः_missing,
			     &def_str);
    जइ (अङ्कः_missing > 0)
    {
	पूर्णाङ्क i;

	जइ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_in_fontset), name);
	    प्रति (i = 0; i < अङ्कः_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    जइ (fontset == रिक्त)
    {
	जइ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	प्रत्यागम NOFONTSET;
    }

    जइ (fixed_width && check_fontset_sanity(fontset) == FAIL)
    {
	XFreeFontSet(gui.dpy, fontset);
	प्रत्यागम NOFONTSET;
    }
    प्रत्यागम (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    स्थिर पूर्णाङ्क
check_fontset_sanity(XFontSet fs)
{
    XFontStruct	**xfs;
    वर्ण	**font_name;
    पूर्णाङ्क		fn;
    वर्ण	*base_name;
    पूर्णाङ्क		i;
    पूर्णाङ्क		min_width;
    पूर्णाङ्क		min_font_idx = 0;

    base_name = XBaseFontNameListOfFontSet(fs);
    fn = XFontsOfFontSet(fs, &xfs, &font_name);
    प्रति (i = 0; i < fn; i++)
    {
	जइ (xfs[i]->max_bounds.width != xfs[i]->min_bounds.width)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_width),
		    base_name, font_name[i]);
	    प्रत्यागम FAIL;
	}
    }
    // scan base font width
    min_width = 32767;
    प्रति (i = 0; i < fn; i++)
    {
	जइ (xfs[i]->max_bounds.width<min_width)
	{
	    min_width = xfs[i]->max_bounds.width;
	    min_font_idx = i;
	}
    }
    प्रति (i = 0; i < fn; i++)
    {
	जइ (	   xfs[i]->max_bounds.width != 2 * min_width
		&& xfs[i]->max_bounds.width != min_width)
	{
	    semsg(_(e_fontset_name_str), base_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (पूर्णाङ्क)xfs[min_font_idx]->max_bounds.width);
	    semsg(_("Font%d width: %d"), i, (पूर्णाङ्क)xfs[i]->max_bounds.width);
	    प्रत्यागम FAIL;
	}
    }
    // it seems ok. Good Luck!!
    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
fontset_width(XFontSet fs)
{
 प्रत्यागम XmbTextEscapement(fs, "Vim", 3) / 3;
}

    पूर्णाङ्क
fontset_height(
    XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    प्रत्यागम extents->max_logical_extent.height;
}

# जइ 0
// NOT USED YET
    स्थिर पूर्णाङ्क
fontset_descent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet (fs);
    प्रत्यागम extents->max_logical_extent.height + extents->max_logical_extent.y;
}
# endif

    स्थिर पूर्णाङ्क
fontset_ascent(XFontSet fs)
{
    XFontSetExtents *extents;

    extents = XExtentsOfFontSet(fs);
    प्रत्यागम -extents->max_logical_extent.y;
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
    जइ (!gui.in_use || name == रिक्त || *name == NUL)
	प्रत्यागम INVALCOLOR;

    requested = gui_get_color_cmn(name);
    जइ (requested == INVALCOLOR)
	प्रत्यागम INVALCOLOR;

    प्रत्यागम gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(पूर्णाङ्क r, पूर्णाङ्क g, पूर्णाङ्क b)
{
    XColor	available;
    Colormap	colormap;

#जइ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    वर्ण	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, परिमाण(spec), "#%.2x%.2x%.2x", r, g, b);
    जइ (XParseColor(gui.dpy, colormap, (वर्ण *)spec, &available) != 0
	    && XAllocColor(gui.dpy, colormap, &available) != 0)
	प्रत्यागम (guicolor_T)available.pixel;
#endif
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_FIELD(available);
    available.red = r << 8;
    available.green = g << 8;
    available.blue = b << 8;
    जइ (XAllocColor(gui.dpy, colormap, &available) != 0)
	प्रत्यागम (guicolor_T)available.pixel;

    प्रत्यागम INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    शून्य
gui_mch_set_fg_color(guicolor_T color)
{
    जइ (color == prev_fg_color)
	प्रत्यागम;

    XSetForeground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    शून्य
gui_mch_set_bg_color(guicolor_T color)
{
    जइ (color == prev_bg_color)
	प्रत्यागम;

    XSetBackground(gui.dpy, gui.text_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    शून्य
gui_mch_set_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    स्थिर Cursor
gui_x11_create_blank_mouse(शून्य)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (unsigned long)0, (XGCValues*)0);

    जइ (gc != रिक्त)
    {
	XDrawPoint(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    प्रत्यागम XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    स्थिर शून्य
draw_curl(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क cells)
{
    पूर्णाङ्क			i;
    पूर्णाङ्क			offset;
    स्थिर ध्रुव पूर्णाङ्क	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
    प्रति (i = FILL_X(col); i < FILL_X(col + cells); ++i)
    {
	offset = val[i % 8];
	XDrawPoint(gui.dpy, gui.wid, gui.text_gc, i,
						FILL_Y(row + 1) - 1 - offset);
    }
    XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
}

    शून्य
gui_mch_draw_string(
    पूर्णाङ्क		row,
    पूर्णाङ्क		col,
    char_u	*s,
    पूर्णाङ्क		len,
    पूर्णाङ्क		flags)
{
    पूर्णाङ्क			cells = len;
    स्थिर शून्य		*buf = रिक्त;
    स्थिर पूर्णाङ्क		buflen = 0;
    char_u		*p;
    पूर्णाङ्क			wlen = 0;
    पूर्णाङ्क			c;

    जइ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	जइ (buflen < len)
	{
	    XtFree((वर्ण *)buf);
	    buf = (शून्य *)XtMalloc(len * (परिमाण(XChar2b) < परिमाण(wchar_t)
					? परिमाण(wchar_t) : परिमाण(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	जाव (p < s + len)
	{
	    c = utf_ptr2char(p);
#ifdef FEAT_XFONTSET
	    जइ (current_fontset != रिक्त)
	    {
# ifdef SMALL_WCHAR_T
		जइ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    अण्णहा
#endif
	    {
		जइ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (unsigned)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    अण्णहा जइ (has_mbyte)
    {
	cells = 0;
	प्रति (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#ifdef FEAT_XFONTSET
    जइ (current_fontset != रिक्त)
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

    जइ (flags & DRAW_TRANSP)
    {
	जइ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	अण्णहा
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (वर्ण *)s, len);
    }
    अण्णहा जइ (p_linespace != 0
#ifdef FEAT_XFONTSET
	    || current_fontset != रिक्त
#endif
	    )
    {
	XSetForeground(gui.dpy, gui.text_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		FILL_Y(row), gui.char_width * cells, gui.char_height);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);

	जइ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	अण्णहा
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (वर्ण *)s, len);
    }
    अण्णहा
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	जइ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), buf, wlen);
	अण्णहा
	    XDrawImageString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col),
		    TEXT_Y(row), (वर्ण *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    जइ (flags & DRAW_BOLD)
    {
	जइ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), buf, wlen);
	अण्णहा
	    XDrawString(gui.dpy, gui.wid, gui.text_gc, TEXT_X(col) + 1,
		    TEXT_Y(row), (वर्ण *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    जइ (flags & DRAW_UNDERC)
	draw_curl(row, col, cells);

    // Underline: draw a line at the bottom of the character cell.
    जइ (flags & DRAW_UNDERL)
    {
	पूर्णाङ्क	y = FILL_Y(row + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	जइ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_पङ्क्तिः(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
    }

    जइ (flags & DRAW_STRIKE)
    {
	पूर्णाङ्क	y = FILL_Y(row + 1) - gui.char_height/2;

	XSetForeground(gui.dpy, gui.text_gc, prev_sp_color);
	XDraw_पङ्क्तिः(gui.dpy, gui.wid, gui.text_gc, FILL_X(col),
		y, FILL_X(col + cells) - 1, y);
	XSetForeground(gui.dpy, gui.text_gc, prev_fg_color);
    }

#ifdef FEAT_XFONTSET
    जइ (current_fontset != रिक्त)
	XSetClipMask(gui.dpy, gui.text_gc, None);
#endif
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    पूर्णाङ्क
gui_mch_haskey(char_u *name)
{
    पूर्णाङ्क i;

    प्रति (i = 0; special_कुञ्चिकाः[i].कुञ्चिका_sym != (कुञ्चिका_Sym)0; i++)
	जइ (name[0] == special_कुञ्चिकाः[i].vim_code0 &&
					 name[1] == special_कुञ्चिकाः[i].vim_code1)
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    पूर्णाङ्क
gui_get_x11_windis(Window *win, Display **dis)
{
    *win = XtWindow(vimShell);
    *dis = gui.dpy;
    प्रत्यागम OK;
}

    शून्य
gui_mch_beep(शून्य)
{
    XBell(gui.dpy, 0);
}

    शून्य
gui_mch_flash(पूर्णाङ्क msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((पूर्णाङ्क)Columns) + gui.border_offset,
	    FILL_Y((पूर्णाङ्क)Rows) + gui.border_offset);
    XSync(gui.dpy, False);
    ui_delay((long)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((पूर्णाङ्क)Columns) + gui.border_offset,
	    FILL_Y((पूर्णाङ्क)Rows) + gui.border_offset);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    शून्य
gui_mch_invert_rectangle(
    पूर्णाङ्क	    r,
    पूर्णाङ्क	    c,
    पूर्णाङ्क	    nr,
    पूर्णाङ्क	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_width, (nr) * gui.char_height);
}

/*
 * Iconify the GUI window.
 */
    शून्य
gui_mch_iconify(शून्य)
{
    XIconifyWindow(gui.dpy, XtWindow(vimShell), DefaultScreen(gui.dpy));
}

#जइ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    शून्य
gui_mch_set_foreground(शून्य)
{
    XMapRaised(gui.dpy, XtWindow(vimShell));
}
#endif

/*
 * Draw a cursor without focus.
 */
    शून्य
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    पूर्णाङ्क		w = 1;

    जइ (mb_lefthalve(gui.row, gui.col))
	w = 2;
    gui_mch_set_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.text_gc, FILL_X(gui.col),
	    FILL_Y(gui.row), w * gui.char_width - 1, gui.char_height - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    शून्य
gui_mch_draw_part_cursor(पूर्णाङ्क w, पूर्णाङ्क h, guicolor_T color)
{
    gui_mch_set_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.text_gc,
#ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_दक्षिणः ? FILL_X(gui.col + 1) - w :
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
    शून्य
gui_mch_update(शून्य)
{
    Xt_निवेशः_Mask mask, desired;

#ifdef ALT_X_निवेशः
    जइ (suppress_alternate_निवेशः)
	desired = (XtIMXEvent | XtIMTimer);
    अण्णहा
#endif
	desired = (XtIMAll);
    जाव ((mask = XtAppPending(app_context)) && (mask & desired)
	    && !vim_is_निवेशः_buf_full())
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
    पूर्णाङ्क
gui_mch_wait_for_chars(long wtime)
{
    पूर्णाङ्क	    focus;
    पूर्णाङ्क	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    स्थिर पूर्णाङ्क	    timed_out;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_निवेशः_Mask	    desired;
#ifdef FEAT_JOB_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#endif

    timed_out = FALSE;

    जइ (wtime >= 0)
	timer = XtAppAddTimeOut(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_out);
#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    जइ (channel_any_keep_open())
	channel_timer = XtAppAddTimeOut(app_context, (long_u)20,
				   channel_poll_cb, (XtPointer)&channel_timer);
#endif

    focus = gui.in_focus;
    desired = (XtIMAll);
    जाव (!timed_out)
    {
	// Stop or start blinking when focus changes
	जइ (gui.in_focus != focus)
	{
	    जइ (gui.in_focus)
		gui_mch_start_blink();
	    अण्णहा
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	जइ (did_add_timer)
	    // Need to recompute the waiting time.
	    भङ्ग;
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

	जइ (निवेशः_available())
	{
	    retval = OK;
	    भङ्ग;
	}
    }

    जइ (timer != (XtIntervalId)0 && !timed_out)
	XtRemoveTimeOut(timer);
#ifdef FEAT_JOB_CHANNEL
    जइ (channel_timer != (XtIntervalId)0)
	XtRemoveTimeOut(channel_timer);
#endif

    प्रत्यागम retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    शून्य
gui_mch_flush(शून्य)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    शून्य
gui_mch_clear_block(
    पूर्णाङ्क		row1,
    पूर्णाङ्क		col1,
    पूर्णाङ्क		row2,
    पूर्णाङ्क		col2)
{
    पूर्णाङ्क		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.back_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height);
}

    शून्य
gui_mch_clear_all(शून्य)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    शून्य
gui_mch_delete_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    जइ (gui.visibility == VisibilityFullyObscured)
	प्रत्यागम;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_वामः), FILL_Y(row + अङ्कः_पङ्क्तयः),
	gui.char_width * (gui.scroll_region_दक्षिणः - gui.scroll_region_वामः + 1)
			       + (gui.scroll_region_दक्षिणः == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - अङ्कः_पङ्क्तयः + 1),
	FILL_X(gui.scroll_region_वामः), FILL_Y(row));

    gui_clear_block(gui.scroll_region_bot - अङ्कः_पङ्क्तयः + 1,
						       gui.scroll_region_वामः,
			  gui.scroll_region_bot, gui.scroll_region_दक्षिणः);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    शून्य
gui_mch_निवेशय_पङ्क्तयः(पूर्णाङ्क row, पूर्णाङ्क अङ्कः_पङ्क्तयः)
{
    जइ (gui.visibility == VisibilityFullyObscured)
	प्रत्यागम;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.text_gc,
	FILL_X(gui.scroll_region_वामः), FILL_Y(row),
	gui.char_width * (gui.scroll_region_दक्षिणः - gui.scroll_region_वामः + 1)
			       + (gui.scroll_region_दक्षिणः == Columns - 1),
	gui.char_height * (gui.scroll_region_bot - row - अङ्कः_पङ्क्तयः + 1),
	FILL_X(gui.scroll_region_वामः), FILL_Y(row + अङ्कः_पङ्क्तयः));

    gui_clear_block(row, gui.scroll_region_वामः,
				row + अङ्कः_पङ्क्तयः - 1, gui.scroll_region_दक्षिणः);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    स्थिर शून्य
gui_x11_check_copy_area(शून्य)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    जइ (gui.visibility != VisibilityPartiallyObscured)
	प्रत्यागम;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    प्रति (;;)
    {
	जइ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    प्रत्यागम;	// The scroll worked.

	जइ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->width, gevent->height);
	    जइ (gevent->गणनम् == 0)
		प्रत्यागम;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    पूर्णाङ्क
clip_mch_own_selection(Clipboard_T *cbd)
{
    प्रत्यागम clip_x11_own_selection(vimShell, cbd);
}

    शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    शून्य
clip_mch_set_selection(
    Clipboard_T	*cbd)
{
    clip_x11_set_selection(cbd);
}

#जइ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    शून्य
gui_mch_menu_grey(vimmenu_T *menu, पूर्णाङ्क grey)
{
    जइ (menu->id == (Widget)0)
	प्रत्यागम;

    gui_mch_menu_hidden(menu, False);
    जइ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	XtSetSensitive(menu->id, False);
    अण्णहा
	XtSetSensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    शून्य
gui_mch_menu_hidden(vimmenu_T *menu, पूर्णाङ्क hidden)
{
    जइ (menu->id == (Widget)0)
	प्रत्यागम;

    जइ (hidden)
	XtUnmanageChild(menu->id);
    अण्णहा
	XtManageChild(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    शून्य
gui_mch_draw_menubar(शून्य)
{
    // Nothing to do in X
}

    शून्य
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
    स्थिर शून्य
gui_x11_wm_protocol_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    /*
     * Only deal with Client messages.
     */
    जइ (event->type != ClientMessage)
	प्रत्यागम;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    जइ ((Atom)((XClientMessageEvent *)event)->data.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	out_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, XtWindow(vimShell), रिक्त, 0);
	प्रत्यागम;
    }

    जइ ((Atom)((XClientMessageEvent *)event)->data.l[0] !=
						  wm_atoms[DELETE_WINDOW_IDX])
	प्रत्यागम;

    gui_shell_closed();
}

#ifdef FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    स्थिर शून्य
gui_x11_send_event_handler(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XEvent	*event,
    Boolean	*dum UNUSED)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    जइ (e->type == PropertyNotify && e->window == commWindow
	    && e->atom == commProperty &&  e->state == Property_New_मानम्)
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

स्थिर पूर्णाङ्क		blink_state = BLINK_NONE;
स्थिर long_u		blink_waittime = 700;
स्थिर long_u		blink_ontime = 400;
स्थिर long_u		blink_offtime = 250;
स्थिर XtIntervalId	blink_timer = (XtIntervalId)0;

    पूर्णाङ्क
gui_mch_is_blinking(शून्य)
{
    प्रत्यागम blink_state != BLINK_NONE;
}

    पूर्णाङ्क
gui_mch_is_blink_off(शून्य)
{
    प्रत्यागम blink_state == BLINK_OFF;
}

    शून्य
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    शून्य
gui_mch_stop_blink(पूर्णाङ्क may_call_gui_update_cursor)
{
    जइ (blink_timer != (XtIntervalId)0)
    {
	XtRemoveTimeOut(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    जइ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    स्थिर शून्य
gui_x11_blink_cb(
    XtPointer	    timed_out UNUSED,
    XtIntervalId    *interval_id UNUSED)
{
    जइ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = XtAppAddTimeOut(app_context, blink_offtime,
						      gui_x11_blink_cb, रिक्त);
    }
    अण्णहा
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = XtAppAddTimeOut(app_context, blink_ontime,
						      gui_x11_blink_cb, रिक्त);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    शून्य
gui_mch_start_blink(शून्य)
{
    जइ (blink_timer != (XtIntervalId)0)
	XtRemoveTimeOut(blink_timer);
    // Only switch blinking on if none of the times is zero
    जइ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = XtAppAddTimeOut(app_context, blink_waittime,
						      gui_x11_blink_cb, रिक्त);
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

    प्रत्यागम (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((unsigned)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    शून्य
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
    जइ (vim_strchr(p_go, GO_POINTER) != रिक्त)
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

    XtAddEventHandler(vimForm, कुञ्चिका_Press_Mask, FALSE, gui_x11_कुञ्चिका_hit_cb,
	(XtPointer)0);
    XtAddEventHandler(textArea, कुञ्चिका_Press_Mask, FALSE, gui_x11_कुञ्चिका_hit_cb,
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
    शून्य
gui_mch_getmouse(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    पूर्णाङ्क		rootx, rooty, winx, winy;
    Window	मूलम्, child;
    unsigned पूर्णाङ्क mask;

    जइ (gui.wid && XQueryPointer(gui.dpy, gui.wid, &मूलम्, &child,
					 &rootx, &rooty, &winx, &winy, &mask))
    {
	*x = winx;
	*y = winy;
    }
    अण्णहा
    {
	*x = -1;
	*y = -1;
    }
}

    शून्य
gui_mch_setmouse(पूर्णाङ्क x, पूर्णाङ्क y)
{
    जइ (gui.wid)
	XWarpPointer(gui.dpy, (Window)0, gui.wid, 0, 0, 0, 0, x, y);
}

#जइ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_last_mouse_event(शून्य)
{
    प्रत्यागम &last_mouse_event;
}
#endif

#जइ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_WIDTH (gui.char_width * 2)

    शून्य
gui_mch_drawsign(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क typenr)
{
    XImage	*sign;

    जइ (!gui.in_use || (sign = (XImage *)sign_get_image(typenr)) == रिक्त)
	प्रत्यागम;

    XClearArea(gui.dpy, gui.wid, TEXT_X(col), TEXT_Y(row) - sign->height,
	    SIGN_WIDTH, gui.char_height, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.text_gc, sign, 0, 0,
	    TEXT_X(col) + (SIGN_WIDTH - sign->width) / 2,
	    TEXT_Y(row) - sign->height,
	    sign->width, sign->height);
}

    शून्य *
gui_mch_register_sign(char_u *signfile)
{
    XpmAttributes   attrs;
    XImage	    *sign = रिक्त;
    पूर्णाङ्क		    status;

    /*
     * Setup the color substitution table.
     */
    जइ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_चिह्नम् color[5] =
	{
	    {"none", रिक्त, 0},
	    {"iconColor1", रिक्त, 0},
	    {"bottomShadowColor", रिक्त, 0},
	    {"topShadowColor", रिक्त, 0},
	    {"selectColor", रिक्त, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.back_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	status = XpmReadFileToImage(gui.dpy, (वर्ण *)signfile,
							 &sign, रिक्त, &attrs);
	जइ (status == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	अण्णहा
	    emsg(_(e_couldnt_read_in_sign_data));
    }

    प्रत्यागम (शून्य *)sign;
}

    शून्य
gui_mch_destroy_sign(शून्य *sign)
{
    XDestroyImage((XImage*)sign);
}
#endif


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
स्थिर पूर्णाङ्क last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 */
    शून्य
gui_mch_mousehide(
    पूर्णाङ्क		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    जइ (gui.pointer_hidden == hide)
	प्रत्यागम;

    gui.pointer_hidden = hide;
    जइ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    अण्णहा
#ifdef FEAT_MOUSESHAPE
	mch_set_mouse_shape(last_shape);
#अण्णहा
    XUndefineCursor(gui.dpy, gui.wid);
#endif
}

#जइ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
स्थिर पूर्णाङ्क mshape_ids[] =
{
    XC_वामः_ptr,		// arrow
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
    XC_दक्षिणः_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_वामः_ptr			// last one
};

    शून्य
mch_set_mouse_shape(पूर्णाङ्क shape)
{
    पूर्णाङ्क	    id;

    जइ (!gui.in_use)
	प्रत्यागम;

    जइ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_pointer);
    अण्णहा
    {
	जइ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    जइ (id >= XC_अङ्कः_glyphs)
		id = XC_वामः_ptr;
	    अण्णहा
		id &= ~1;	// they are always even (why?)
	}
	अण्णहा
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    जइ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif

#जइ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    शून्य
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    जइ (menu->id == रिक्त || menu->parent == रिक्त
				|| !menu_is_toolbar(menu->parent->name))
	प्रत्यागम;

    // Always destroy and create the balloon, in case the string was
    // changed.
    जइ (menu->tip != रिक्त)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = रिक्त;
    }
    जइ (menu->strings[MENU_INDEX_TIP] != रिक्त)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->strings[MENU_INDEX_TIP],
		रिक्त,
		रिक्त);
}
#endif
