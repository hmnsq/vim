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

#అన్తర్భావ "vim.h"

#అన్తర్భావ <X11/keysym.h>
#అన్తర్భావ <X11/Xatom.h>
#అన్తర్భావ <X11/StringDefs.h>
#అన్తర్భావ <X11/Intrinsic.h>
#అన్తర్భావ <X11/Shell.h>
#అన్తర్భావ <X11/cursorfont.h>

/*
 * XpmP.h is preferred, because it makes the signs drawn with a transparent
 * background instead of black.
 */
#ఒకవేళ defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF) \
	&& !defined(HAVE_X11_XPM_H)
# include <Xm/XpmP.h>
#లేకపోతే
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  లేకపోతే
#   include <X11/xpm.h>
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XFONTSET
# ifdef X_LOCALE
#  include <X11/Xlocale.h>
# లేకపోతే
#  include <locale.h>
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_X11_XMU_EDITRES_H
# include <X11/Xmu/Editres.h>
#అయితే_ముగింపు

#నిర్వచించు VIM_NAME	"vim"
#నిర్వచించు VIM_CLASS	"Vim"

// Default resource values
#నిర్వచించు DFLT_FONT		"7x13"
#నిర్వచితమైతే FONTSET_ALWAYS
# define DFLT_MENU_FONT		Xt_Default_Font_సమితి
#లేకపోతే
# define DFLT_MENU_FONT		XtDefaultFont
#అయితే_ముగింపు
#నిర్వచించు DFLT_TOOLTIP_FONT	Xt_Default_Font_సమితి

// use the default (CDE) colors
#నిర్వచించు DFLT_MENU_BG_COLOR	""
#నిర్వచించు DFLT_MENU_FG_COLOR	""
#నిర్వచించు DFLT_SCROLL_BG_COLOR	""
#నిర్వచించు DFLT_SCROLL_FG_COLOR	""
#నిర్వచించు DFLT_TOOLTIP_BG_COLOR	"#ffff91"
#నిర్వచించు DFLT_TOOLTIP_FG_COLOR	"#000000"

Widget vimShell = (Widget)0;

స్థిర Atom   wm_atoms[2];	// Window Manager Atoms
#నిర్వచించు DELETE_కిటికీ_IDX 0	// index in wm_atoms[] for WM_DELETE_WINDOW
#నిర్వచించు SAVE_YOURSELF_IDX 1	// index in wm_atoms[] for WM_SAVE_YOURSELF

#నిర్వచితమైతే FEAT_XFONTSET
/*
 * We either draw with a fontset (when current_fontset != NULL) or with a
 * normal font (current_fontset == NULL, use gui.text_gc and gui.back_gc).
 */
స్థిర XFont_సమితి current_fontset = ఖాళీ;
# ఒకవేళ !defined(XDrawString)
#  define XDrawString(dpy, win, gc, x, y, str, n) \
	చేయి \
	{ \
	    ఒకవేళ (current_fontset != ఖాళీ) \
		XmbDrawString(dpy, win, current_fontset, gc, x, y, str, n); \
	    లేకపోతే \
		XDrawString(dpy, win, gc, x, y, str, n); \
	} అయితే (0)
# endif
# ఒకవేళ !defined(XDrawString16)
#  define XDrawString16(dpy, win, gc, x, y, str, n) \
	చేయి \
	{ \
	    ఒకవేళ (current_fontset != ఖాళీ) \
		XwcDrawString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    లేకపోతే \
		XDrawString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} అయితే (0)
# endif
# ఒకవేళ !defined(XDrawImageString16)
#  define XDrawImageString16(dpy, win, gc, x, y, str, n) \
	చేయి \
	{ \
	    ఒకవేళ (current_fontset != ఖాళీ) \
		XwcDrawImageString(dpy, win, current_fontset, gc, x, y, (wchar_t *)str, n); \
	    లేకపోతే \
		XDrawImageString16(dpy, win, gc, x, y, (XChar2b *)str, n); \
	} అయితే (0)
# endif
స్థిర పూర్ణాంకం check_fontset_sanity(XFont_సమితి fs);
స్థిర పూర్ణాంకం fontset_వెడల్పు(XFont_సమితి fs);
స్థిర పూర్ణాంకం fontset_ascent(XFont_సమితి fs);
#అయితే_ముగింపు

స్థిర guicolor_T	prev_fg_color = INVALCOLOR;
స్థిర guicolor_T	prev_bg_color = INVALCOLOR;
స్థిర guicolor_T	prev_sp_color = INVALCOLOR;

#ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
స్థిర XButtonPressedEvent చివరి_mouse_event;
#అయితే_ముగింపు

స్థిర శూన్య gui_x11_check_copy_area(శూన్య);
#నిర్వచితమైతే FEAT_CLIENTSERVER
స్థిర శూన్య gui_x11_send_event_handler(Widget, Xt_సూచిక, XEvent *, Boolean *);
#అయితే_ముగింపు
స్థిర శూన్య gui_x11_wm_protocol_handler(Widget, Xt_సూచిక, XEvent *, Boolean *);
స్థిర Cursor gui_x11_create_blank_mouse(శూన్య);


/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_gtk_x11.c probably needs the
 * same change!
 */
స్థిర నిర్మాణం specialkey
{
    కీలకం_Sym  కీలకం_sym;
    char_u  vim_code0;
    char_u  vim_code1;
} special_కీలకాలు[] =
{
    {XK_పైకి,		'k', 'u'},
    {XK_కిందికి,		'k', 'd'},
    {XK_ఎడమ,		'k', 'l'},
    {XK_కుడి,		'k', 'r'},

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
#నిర్వచితమైతే SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#అయితే_ముగింపు

    {XK_Help,		'%', '1'},
    {XK_Undo,		'&', '8'},
    {XK_వెనుక_Space,	'k', 'b'},
    {XK_చొప్పించు,		'k', 'I'},
    {XK_Delete,		'k', 'D'},
    {XK_Home,		'k', 'h'},
    {XK_చివర,		'@', '7'},
    {XK_పూర్వ,		'k', 'P'},
    {XK_తదుపరి,		'k', 'N'},
    {XK_Print,		'%', '9'},

    // Keypad keys:
#నిర్వచితమైతే XK_KP_ఎడమ
    {XK_KP_ఎడమ,	'k', 'l'},
    {XK_KP_కుడి,	'k', 'r'},
    {XK_KP_పైకి,		'k', 'u'},
    {XK_KP_కిందికి,	'k', 'd'},
    {XK_KP_చొప్పించు,	KS_EXTRA, (char_u)KE_KINS},
    {XK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {XK_KP_Home,	'K', '1'},
    {XK_KP_చివర,		'K', '4'},
    {XK_KP_పూర్వ,	'K', '3'},
    {XK_KP_తదుపరి,	'K', '5'},

    {XK_KP_కూడు,		'K', '6'},
    {XK_KP_వ్యవకలనం,	'K', '7'},
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
#అయితే_ముగింపు

    // End of list marker:
    {(కీలకం_Sym)0,	    0, 0}
};

#నిర్వచించు XtNboldFont		"boldFont"
#నిర్వచించు XtCBoldFont		"BoldFont"
#నిర్వచించు XtNitalicFont		"italicFont"
#నిర్వచించు XtCItalicFont		"ItalicFont"
#నిర్వచించు XtNboldItalicFont	"boldItalicFont"
#నిర్వచించు XtCBoldItalicFont	"BoldItalicFont"
#నిర్వచించు Xt_Nscrollbar_వెడల్పు	"scrollbarWidth"
#నిర్వచించు Xt_CScrollbar_వెడల్పు	"ScrollbarWidth"
#నిర్వచించు Xt_Nmenu_ఎత్తు		"menuHeight"
#నిర్వచించు Xt_CMenu_ఎత్తు		"MenuHeight"
#నిర్వచించు XtNmenuFont		"menuFont"
#నిర్వచించు XtCMenuFont		"MenuFont"
#నిర్వచించు Xt_Nmenu_Font_సమితి		"menuFontSet"
#నిర్వచించు Xt_CMenu_Font_సమితి		"MenuFontSet"


// Resources for setting the foreground and background colors of menus
#నిర్వచించు XtNmenuBackground	"menuBackground"
#నిర్వచించు XtCMenuBackground	"MenuBackground"
#నిర్వచించు XtNmenuForeground	"menuForeground"
#నిర్వచించు XtCMenuForeground	"MenuForeground"

// Resources for setting the foreground and background colors of scrollbars
#నిర్వచించు XtNscrollBackground	"scrollBackground"
#నిర్వచించు XtCScrollBackground	"ScrollBackground"
#నిర్వచించు XtNscrollForeground	"scrollForeground"
#నిర్వచించు XtCScrollForeground	"ScrollForeground"

// Resources for setting the foreground and background colors of tooltip
#నిర్వచించు XtNtooltipBackground	"tooltipBackground"
#నిర్వచించు XtCTooltipBackground	"TooltipBackground"
#నిర్వచించు XtNtooltipForeground	"tooltipForeground"
#నిర్వచించు XtCTooltipForeground	"TooltipForeground"
#నిర్వచించు XtNtooltipFont		"tooltipFont"
#నిర్వచించు XtCTooltipFont		"TooltipFont"

/*
 * X Resources:
 */
స్థిర XtResource vim_resources[] =
{
    {
	XtNforeground,
	XtCForeground,
	XtRPixel,
	పరిమాణం(Pixel),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, def_norm_pixel),
	XtRString,
	XtDefaultForeground
    },
    {
	XtNbackground,
	XtCBackground,
	XtRPixel,
	పరిమాణం(Pixel),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, def_వెనుక_pixel),
	XtRString,
	XtDefaultBackground
    },
    {
	XtNfont,
	XtCFont,
	XtRString,
	పరిమాణం(String *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_font_name),
	XtRImmediate,
	XtDefaultFont
    },
    {
	XtNboldFont,
	XtCBoldFont,
	XtRString,
	పరిమాణం(String *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_bold_font_name),
	XtRImmediate,
	""
    },
    {
	XtNitalicFont,
	XtCItalicFont,
	XtRString,
	పరిమాణం(String *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_ital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNboldItalicFont,
	XtCBoldItalicFont,
	XtRString,
	పరిమాణం(String *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_boldital_font_name),
	XtRImmediate,
	""
    },
    {
	XtNgeometry,
	XtCGeometry,
	XtRString,
	పరిమాణం(String *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, geom),
	XtRImmediate,
	""
    },
    {
	XtNreverseVideo,
	XtCReverseVideo,
	XtRBool,
	పరిమాణం(Bool),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_rev_video),
	XtRImmediate,
	(Xt_సూచిక)False
    },
    {
	Xt_Nborder_వెడల్పు,
	Xt_CBorder_వెడల్పు,
	XtRInt,
	పరిమాణం(పూర్ణాంకం),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, border_వెడల్పు),
	XtRImmediate,
	(Xt_సూచిక)2
    },
    {
	Xt_Nscrollbar_వెడల్పు,
	Xt_CScrollbar_వెడల్పు,
	XtRInt,
	పరిమాణం(పూర్ణాంకం),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, scrollbar_వెడల్పు),
	XtRImmediate,
	(Xt_సూచిక)SB_DEFAULT_వెడల్పు
    },
#నిర్వచితమైతే FEAT_MENU
    {
# ifdef FONTSET_ALWAYS
	Xt_Nmenu_Font_సమితి,
	Xt_CMenu_Font_సమితి,
# లేకపోతే
	XtNmenuFont,
	XtCMenuFont,
# endif
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_menu_font_name),
	XtRString,
	DFLT_MENU_FONT
    },
#అయితే_ముగింపు
    {
	XtNmenuForeground,
	XtCMenuForeground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_menu_fg_name),
	XtRString,
	DFLT_MENU_FG_COLOR
    },
    {
	XtNmenuBackground,
	XtCMenuBackground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_menu_bg_name),
	XtRString,
	DFLT_MENU_BG_COLOR
    },
    {
	XtNscrollForeground,
	XtCScrollForeground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_scroll_fg_name),
	XtRString,
	DFLT_SCROLL_FG_COLOR
    },
    {
	XtNscrollBackground,
	XtCScrollBackground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_scroll_bg_name),
	XtRString,
	DFLT_SCROLL_BG_COLOR
    },
#నిర్వచితమైతే FEAT_BEVAL_GUI
    {
	XtNtooltipForeground,
	XtCTooltipForeground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_tooltip_fg_name),
	XtRString,
	DFLT_TOOLTIP_FG_COLOR
    },
    {
	XtNtooltipBackground,
	XtCTooltipBackground,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_tooltip_bg_name),
	XtRString,
	DFLT_TOOLTIP_BG_COLOR
    },
    {
	XtNtooltipFont,
	XtCTooltipFont,
	XtRString,
	పరిమాణం(అక్షరం *),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_tooltip_font_name),
	XtRString,
	DFLT_TOOLTIP_FONT
    },
    // This one may not be really needed?
    {
	"balloonEvalFontSet",
	Xt_CFont_సమితి,
	Xt_RFont_సమితి,
	పరిమాణం(XFont_సమితి),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, tooltip_fontset),
	XtRImmediate,
	(Xt_సూచిక)NOFONTSET
    },
#అయితే_ముగింపు // FEAT_BEVAL_GUI
#నిర్వచితమైతే FEAT_XIM
    {
	"preeditType",
	"PreeditType",
	XtRString,
	పరిమాణం(అక్షరం*),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_preedit_type_name),
	XtRString,
	(Xt_సూచిక)"OverTheSpot,OffTheSpot,Root"
    },
    {
	"inputMethod",
	"InputMethod",
	XtRString,
	పరిమాణం(అక్షరం*),
	Xt_స్థానభ్రంశం_యొక్క(gui_T, rsrc_ఆదానం_పద్ధతి),
	XtRString,
	ఖాళీ
    },
#అయితే_ముగింపు // FEAT_XIM
};

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when vim is started without the
 * GUI (but the GUI might start up later).
 * When changing this, also update doc/vim_gui.txt and the usage message!!!
 */
స్థిర XrmOptionDescRec cmdline_options[] =
{
    // We handle these options ourselves
    {"-bg",		".background",	    XrmoptionSepArg,	ఖాళీ},
    {"-background",	".background",	    XrmoptionSepArg,	ఖాళీ},
    {"-fg",		".foreground",	    XrmoptionSepArg,	ఖాళీ},
    {"-foreground",	".foreground",	    XrmoptionSepArg,	ఖాళీ},
    {"-fn",		".font",	    XrmoptionSepArg,	ఖాళీ},
    {"-font",		".font",	    XrmoptionSepArg,	ఖాళీ},
    {"-boldfont",	".boldFont",	    XrmoptionSepArg,	ఖాళీ},
    {"-italicfont",	".italicFont",	    XrmoptionSepArg,	ఖాళీ},
    {"-geom",		".geometry",	    XrmoptionSepArg,	ఖాళీ},
    {"-geometry",	".geometry",	    XrmoptionSepArg,	ఖాళీ},
    {"-reverse",	"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"-rv",		"*reverseVideo",    XrmoptionNoArg,	"True"},
    {"+reverse",	"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"+rv",		"*reverseVideo",    XrmoptionNoArg,	"False"},
    {"-display",	".display",	    XrmoptionSepArg,	ఖాళీ},
    {"-iconic",		".iconic",	    XrmoptionNoArg,	"True"},
    {"-name",		".name",	    XrmoptionSepArg,	ఖాళీ},
    {"-bw",		".borderWidth",	    XrmoptionSepArg,	ఖాళీ},
    {"-borderwidth",	".borderWidth",	    XrmoptionSepArg,	ఖాళీ},
    {"-sw",		".scrollbarWidth",  XrmoptionSepArg,	ఖాళీ},
    {"-scrollbarwidth",	".scrollbarWidth",  XrmoptionSepArg,	ఖాళీ},
    {"-mh",		".menuHeight",	    XrmoptionSepArg,	ఖాళీ},
    {"-menuheight",	".menuHeight",	    XrmoptionSepArg,	ఖాళీ},
#నిర్వచితమైతే FONTSET_ALWAYS
    {"-mf",		".menuFontSet",	    XrmoptionSepArg,	ఖాళీ},
    {"-menufont",	".menuFontSet",	    XrmoptionSepArg,	ఖాళీ},
    {"-menufontset",	".menuFontSet",	    XrmoptionSepArg,	ఖాళీ},
#లేకపోతే
    {"-mf",		".menuFont",	    XrmoptionSepArg,	ఖాళీ},
    {"-menufont",	".menuFont",	    XrmoptionSepArg,	ఖాళీ},
#అయితే_ముగింపు
    {"-xrm",		ఖాళీ,		    XrmoptionResArg,	ఖాళీ}
};

స్థిర పూర్ణాంకం gui_argc = 0;
స్థిర అక్షరం **gui_argv = ఖాళీ;

/*
 * Call-back routines.
 */

    స్థిర శూన్య
gui_x11_timer_cb(
    Xt_సూచిక	    timed_బహిర్గతం,
    XtIntervalId    *interval_id వాడని)
{
    *((పూర్ణాంకం *)timed_బహిర్గతం) = TRUE;
}

#నిర్వచితమైతే FEAT_పని_CHANNEL
    స్థిర శూన్య
channel_poll_cb(
    Xt_సూచిక	    client_దత్తాంశం_2,
    XtIntervalId    *interval_id వాడని)
{
    XtIntervalId    *channel_timer = (XtIntervalId *)client_దత్తాంశం_2;

    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    // repeat
    *channel_timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, (long_u)20,
						 channel_poll_cb, client_దత్తాంశం_2);
}
#అయితే_ముగింపు

    స్థిర శూన్య
gui_x11_visibility_cb(
    Widget	w వాడని,
    Xt_సూచిక	dud వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    ఒకవేళ (event->type != VisibilityNotify)
	తిరిగి;

    gui.visibility = event->xvisibility.state;

    /*
     * When we do an XCopyArea(), and the window is partially obscured, we want
     * to receive an event to tell us whether it worked or not.
     */
    XSetGraphicsExposures(gui.dpy, gui.పాఠ్యం_gc,
	    gui.visibility != VisibilityUnobscured);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

    స్థిర శూన్య
gui_x11_expose_cb(
    Widget	w వాడని,
    Xt_సూచిక	dud వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    XExposeEvent	*gevent;
    పూర్ణాంకం			new_x;

    ఒకవేళ (event->type != Expose)
	తిరిగి;

    బహిర్గతం_flush();	    // make sure all output has been processed

    gevent = (XExposeEvent *)event;
    gui_redraw(gevent->x, gevent->y, gevent->వెడల్పు, gevent->ఎత్తు);

    new_x = FILL_X(0);

    // Clear the border areas if needed
    ఒకవేళ (gevent->x < new_x)
	XClearArea(gui.dpy, gui.wid, 0, 0, new_x, 0, False);
    ఒకవేళ (gevent->y < FILL_Y(0))
	XClearArea(gui.dpy, gui.wid, 0, 0, 0, FILL_Y(0), False);
    ఒకవేళ (gevent->x > FILL_X(Columns))
	XClearArea(gui.dpy, gui.wid, FILL_X((పూర్ణాంకం)Columns), 0, 0, 0, False);
    ఒకవేళ (gevent->y > FILL_Y(అడ్డువరుసలు))
	XClearArea(gui.dpy, gui.wid, 0, FILL_Y((పూర్ణాంకం)అడ్డువరుసలు), 0, 0, False);

    // This is needed for when redrawing is slow.
    gui_mch_update();
}

#ఒకవేళ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
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
    స్థిర శూన్య
shellRectangle(Widget shell, XRectangle *r)
{
    కిటికీ		rootw, shellw, బిడ్డ, parentw;
    పూర్ణాంకం			absx, absy;
    XWindow_లక్షణాలు	a;
    కిటికీ		*children;
    గుర్తులేని పూర్ణాంకం	children_లెక్క;

    shellw = Xt_కిటికీ(shell);
    ఒకవేళ (shellw == 0)
	తిరిగి;
    కొరకు (;;)
    {
	XQuery_వృక్షం(XtDisplay(shell), shellw, &rootw, &parentw,
						   &children, &children_లెక్క);
	XFree(children);
	ఒకవేళ (parentw == rootw)
	    ఆపు;
	shellw = parentw;
    }
    XGet_కిటికీ_లక్షణాలు(XtDisplay(shell), shellw, &a);
    XTranslateCoordinates(XtDisplay(shell), shellw, a.మూలం, 0, 0,
							&absx, &absy, &బిడ్డ);
    r->x = absx;
    r->y = absy;
    Xt_Va_Get_విలువలు(shell, XmNheight, &r->ఎత్తు, XmNwidth, &r->వెడల్పు, ఖాళీ);
}
#అయితే_ముగింపు

    స్థిర శూన్య
gui_x11_resize_కిటికీ_cb(
    Widget	w వాడని,
    Xt_సూచిక	dud వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    స్థిర పూర్ణాంకం చివరి_వెడల్పు_2, చివరి_ఎత్తు_2;

    ఒకవేళ (event->type != ConfigureNotify)
	తిరిగి;

    ఒకవేళ (event->xconfigure.వెడల్పు != చివరి_వెడల్పు_2
	    || event->xconfigure.ఎత్తు != చివరి_ఎత్తు_2)
    {
	చివరి_వెడల్పు_2 = event->xconfigure.వెడల్పు;
	చివరి_ఎత్తు_2 = event->xconfigure.ఎత్తు;
	gui_resize_shell(event->xconfigure.వెడల్పు, event->xconfigure.ఎత్తు
#నిర్వచితమైతే FEAT_XIM
						- xim_get_స్థితి_area_ఎత్తు()
#అయితే_ముగింపు
		     );
    }
#ఒకవేళ defined(FEAT_NETBEANS_INTG) && defined(FEAT_GUI_MOTIF)
    ఒకవేళ (netbeans_చురుకైన())
    {
	XRectangle  rec;

	shellRectangle(w, &rec);
	netbeans_చట్రం_moved(rec.x, rec.y);
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XIM
    xim_సమితి_preedit();
#అయితే_ముగింపు
}

    స్థిర శూన్య
gui_x11_focus_change_cb(
    Widget	w వాడని,
    Xt_సూచిక	దత్తాంశం వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    gui_focus_change(event->type == Focus_లోపల);
}

    స్థిర శూన్య
gui_x11_enter_cb(
    Widget	w వాడని,
    Xt_సూచిక	దత్తాంశం వాడని,
    XEvent	*event వాడని,
    Boolean	*dum వాడని)
{
    gui_focus_change(TRUE);
}

    స్థిర శూన్య
gui_x11_leave_cb(
    Widget	w వాడని,
    Xt_సూచిక	దత్తాంశం వాడని,
    XEvent	*event వాడని,
    Boolean	*dum వాడని)
{
    gui_focus_change(FALSE);
}

#ఒకవేళ defined(X_HAVE_UTF8_STRING)
# ఒకవేళ X_HAVE_UTF8_STRING
#  define USE_UTF8LOOKUP
# endif
#అయితే_ముగింపు

    శూన్య
gui_x11_కీలకం_తగిలింది_cb(
    Widget	w వాడని,
    Xt_సూచిక	dud వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    XKeyPressedEvent	*ev_press;
#నిర్వచితమైతే FEAT_XIM
    char_u		string2[256];
    char_u		string_shortbuf[256];
    char_u		*string = string_shortbuf;
    Boolean		string_alloced = False;
    స్థితి_3		స్థితి_2;
#లేకపోతే
    char_u		string[4], string2[3];
#అయితే_ముగింపు
    కీలకం_Sym		కీలకం_sym;
    పూర్ణాంకం			len;
    పూర్ణాంకం			i;
    పూర్ణాంకం			modifiers;
    పూర్ణాంకం			కీలకం;

    ev_press = (XKeyPressedEvent *)event;

#నిర్వచితమైతే FEAT_XIM
    ఒకవేళ (xic)
    {
# ifdef USE_UTF8LOOKUP
	// XFree86 4.0.2 or newer: Be able to get UTF-8 characters even when
	// the locale isn't utf-8.
	ఒకవేళ (enc_utf8)
	    len = Xutf8LookupString(xic, ev_press, (అక్షరం *)string,
				  పరిమాణం(string_shortbuf), &కీలకం_sym, &స్థితి_2);
	లేకపోతే
# endif
	    len = XmbLookupString(xic, ev_press, (అక్షరం *)string,
				  పరిమాణం(string_shortbuf), &కీలకం_sym, &స్థితి_2);
	ఒకవేళ (స్థితి_2 == XBuffer_అధికప్రవాహం)
	{
	    string = (char_u *)XtMalloc(len + 1);
	    string_alloced = True;
# ifdef USE_UTF8LOOKUP
	    // XFree86 4.0.2 or newer: Be able to get UTF-8 characters even
	    // when the locale isn't utf-8.
	    ఒకవేళ (enc_utf8)
		len = Xutf8LookupString(xic, ev_press, (అక్షరం *)string,
						      len, &కీలకం_sym, &స్థితి_2);
	    లేకపోతే
# endif
		len = XmbLookupString(xic, ev_press, (అక్షరం *)string,
						      len, &కీలకం_sym, &స్థితి_2);
	}
	ఒకవేళ (స్థితి_2 == XLookupNone || స్థితి_2 == XLookupChars)
	    కీలకం_sym = XK_Void_చిహ్నం;

	// Do conversion from 'termencoding' to 'encoding'.  When using
	// Xutf8LookupString() it has already been done.
	ఒకవేళ (len > 0 && ఆదానం_conv.vc_type != CONV_NONE
# ifdef USE_UTF8LOOKUP
		&& !enc_utf8
# endif
		)
	{
	    పూర్ణాంకం		maxlen = len * 4 + 40;	// guessed
	    char_u	*p = (char_u *)XtMalloc(maxlen);

	    mch_memmove(p, string, len);
	    ఒకవేళ (string_alloced)
		XtFree((అక్షరం *)string);
	    string = p;
	    string_alloced = True;
	    len = convert_ఆదానం(p, len, maxlen);
	}

	// Translate CSI to K_CSI, otherwise it could be recognized as the
	// start of a special key.
	కొరకు (i = 0; i < len; ++i)
	    ఒకవేళ (string[i] == CSI)
	    {
		char_u	*p = (char_u *)XtMalloc(len + 3);

		mch_memmove(p, string, i + 1);
		p[i + 1] = KS_EXTRA;
		p[i + 2] = (పూర్ణాంకం)KE_CSI;
		mch_memmove(p + i + 3, string + i + 1, len - i);
		ఒకవేళ (string_alloced)
		    XtFree((అక్షరం *)string);
		string = p;
		string_alloced = True;
		i += 2;
		len += 2;
	    }
    }
    లేకపోతే
#అయితే_ముగింపు
	len = XLookupString(ev_press, (అక్షరం *)string, పరిమాణం(string),
		&కీలకం_sym, ఖాళీ);

#నిర్వచితమైతే SunXK_F36
    /*
    * These keys have bogus lookup strings, and trapping them here is
    * easier than trying to XRebindKeysym() on them with every possible
    * combination of modifiers.
    */
    ఒకవేళ (కీలకం_sym == SunXK_F36 || కీలకం_sym == SunXK_F37)
	len = 0;
#అయితే_ముగింపు

    ఒకవేళ (కీలకం_sym == XK_space)
	string[0] = ' ';	// Otherwise Ctrl-Space doesn't work

    /*
     * Only on some machines ^_ requires Ctrl+Shift+minus.  For consistency,
     * allow just Ctrl+minus too.
     */
    ఒకవేళ (కీలకం_sym == XK_minus && (ev_press->state & Control_ముసుగు))
	string[0] = Ctrl__;

#నిర్వచితమైతే XK_ISO_ఎడమ_Tab
    // why do we get XK_ISO_Left_Tab instead of XK_Tab for shift-tab?
    ఒకవేళ (కీలకం_sym == XK_ISO_ఎడమ_Tab)
    {
	కీలకం_sym = XK_Tab;
	string[0] = TAB;
	len = 1;
    }
#అయితే_ముగింపు

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    ఒకవేళ (len == 1 && string[0] == CSI)
    {
	string[1] = KS_EXTRA;
	string[2] = (పూర్ణాంకం)KE_CSI;
	len = -3;
    }

    // Check for special keys.  Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ఒకవేళ (len == 0 || len == 1)
    {
	కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != (కీలకం_Sym)0; i++)
	{
	    ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == కీలకం_sym)
	    {
		string[0] = CSI;
		string[1] = special_కీలకాలు[i].vim_code0;
		string[2] = special_కీలకాలు[i].vim_code1;
		len = -3;
		ఆపు;
	    }
	}
    }

    // Unrecognised key is ignored.
    ఒకవేళ (len == 0)
	వెళ్ళు theend;

    // Handle modifiers.
    modifiers = 0;
    ఒకవేళ (ev_press->state & జరుపు_ముసుగు)
	modifiers |= MOD_ముసుగు_జరుపు;
    ఒకవేళ (ev_press->state & Control_ముసుగు)
    {
	modifiers |= MOD_ముసుగు_CTRL;
	ఒకవేళ (len == 1 && string[0] < 0x20)
	    // Use the character before applyng CTRL.
	    string[0] += 0x40;
    }
    ఒకవేళ (ev_press->state & Mod1_ముసుగు)
	modifiers |= MOD_ముసుగు_ALT;
    ఒకవేళ (ev_press->state & Mod4_ముసుగు)
	modifiers |= MOD_ముసుగు_META;

    /*
     * For some keys a shift modifier is translated into another key
     * code.
     */
    ఒకవేళ (len == -3)
	కీలకం = గమ్యం_SPECIAL(string[1], string[2]);
    లేకపోతే
    {
	string[len] = NUL;
	కీలకం = mb_ptr2char(string);
    }
    కీలకం = simplify_కీలకం(కీలకం, &modifiers);
    ఒకవేళ (కీలకం == CSI)
	కీలకం = K_CSI;
    ఒకవేళ (IS_SPECIAL(కీలకం))
    {
	string[0] = CSI;
	string[1] = K_రెండవ(కీలకం);
	string[2] = K_మూడవ(కీలకం);
	len = 3;
    }
    లేకపోతే
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	కీలకం = may_adjust_కీలకం_for_ctrl(modifiers, కీలకం);

	len = mb_char2bytes(కీలకం, string);

	// Remove the SHIFT modifier for keys where it's already included,
	// e.g., '(', '!' and '*'.
	modifiers = may_remove_జరుపు_modifier(modifiers, కీలకం);
    }

    ఒకవేళ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	కూడు_గమ్యం_ఆదానం_buf(string2, 3);
    }

    // Check if the key interrupts.
    {
	పూర్ణాంకం int_ch = check_for_అంతరాయం(కీలకం, modifiers);

	ఒకవేళ (int_ch != NUL)
	{
	    trash_ఆదానం_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);

    /*
     * blank out the pointer if necessary
     */
    ఒకవేళ (p_mh)
	gui_mch_mousehide(TRUE);

#ఒకవేళ defined(FEAT_BEVAL_TIP)
    {
	BalloonEval *be;

	ఒకవేళ ((be = gui_mch_currently_showing_beval()) != ఖాళీ)
	    gui_mch_unpost_balloon(be);
    }
#అయితే_ముగింపు
theend:
    {}	    // some compilers need a statement here
#నిర్వచితమైతే FEAT_XIM
    ఒకవేళ (string_alloced)
	XtFree((అక్షరం *)string);
#అయితే_ముగింపు
}

    స్థిర శూన్య
gui_x11_mouse_cb(
    Widget	w వాడని,
    Xt_సూచిక	dud వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    స్థిర XtIntervalId timer = (XtIntervalId)0;
    స్థిర పూర్ణాంకం	timed_బహిర్గతం = TRUE;

    పూర్ణాంకం		button;
    పూర్ణాంకం		repeated_click = FALSE;
    పూర్ణాంకం		x, y;
    int_u	x_modifiers;
    int_u	vim_modifiers;

    ఒకవేళ (event->type == MotionNotify)
    {
	// Get the latest position, avoids lagging behind on a drag.
	x = event->xmotion.x;
	y = event->xmotion.y;
	x_modifiers = event->xmotion.state;
	button = (x_modifiers & (Button1_ముసుగు | Button2_ముసుగు | Button3_ముసుగు))
		? MOUSE_DRAG : ' ';

	/*
	 * if our pointer is currently hidden, then we should show it.
	 */
	gui_mch_mousehide(FALSE);

	ఒకవేళ (button != MOUSE_DRAG)	// just moving the rodent
	{
#నిర్వచితమైతే FEAT_MENU
	    ఒకవేళ (dud)			// moved in vimForm
		y -= gui.menu_ఎత్తు;
#అయితే_ముగింపు
	    gui_mouse_moved(x, y);
	    తిరిగి;
	}
    }
    లేకపోతే
    {
	x = event->xbutton.x;
	y = event->xbutton.y;
	ఒకవేళ (event->type == ButtonPress)
	{
	    // Handle multiple clicks
	    ఒకవేళ (!timed_బహిర్గతం)
	    {
		Xt_Remove_సమయం_బహిర్గతం(timer);
		repeated_click = TRUE;
	    }
	    timed_బహిర్గతం = FALSE;
	    timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, (long_u)p_mouset,
			gui_x11_timer_cb, &timed_బహిర్గతం);
	    ఎంపిక (event->xbutton.button)
	    {
		// keep in sync with gui_gtk_x11.c
		స్థితి Button1:	button = MOUSE_ఎడమ;	ఆపు;
		స్థితి Button2:	button = MOUSE_మధ్య;	ఆపు;
		స్థితి Button3:	button = MOUSE_కుడి;	ఆపు;
		స్థితి Button4:	button = MOUSE_4;	ఆపు;
		స్థితి Button5:	button = MOUSE_5;	ఆపు;
		స్థితి 6:		button = MOUSE_7;	ఆపు;
		స్థితి 7:		button = MOUSE_6;	ఆపు;
		స్థితి 8:		button = MOUSE_X1;	ఆపు;
		స్థితి 9:		button = MOUSE_X2;	ఆపు;
		అప్రమేయ:
		    తిరిగి;	// Unknown button
	    }
	}
	లేకపోతే ఒకవేళ (event->type == ButtonRelease)
	    button = MOUSE_RELEASE;
	లేకపోతే
	    తిరిగి;	// Unknown mouse event type

	x_modifiers = event->xbutton.state;
#ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
	చివరి_mouse_event = event->xbutton;
#అయితే_ముగింపు
    }

    vim_modifiers = 0x0;
    ఒకవేళ (x_modifiers & జరుపు_ముసుగు)
	vim_modifiers |= MOUSE_జరుపు;
    ఒకవేళ (x_modifiers & Control_ముసుగు)
	vim_modifiers |= MOUSE_CTRL;
    ఒకవేళ (x_modifiers & Mod1_ముసుగు)	    // Alt or Meta key
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
    శూన్య
gui_mch_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    పూర్ణాంకం	    arg;
    పూర్ణాంకం	    i;

    /*
     * Move all the entries in argv which are relevant to X into gui_argv.
     */
    gui_argc = 0;
    gui_argv = LALLOC_MULT(అక్షరం *, *argc);
    ఒకవేళ (gui_argv == ఖాళీ)
	తిరిగి;
    gui_argv[gui_argc++] = argv[0];
    arg = 1;
    అయితే (arg < *argc)
    {
	// Look for argv[arg] in cmdline_options[] table
	కొరకు (i = 0; i < (పూర్ణాంకం)Xt_సంఖ్య(cmdline_options); i++)
	    ఒకవేళ (strcmp(argv[arg], cmdline_options[i].option) == 0)
		ఆపు;

	ఒకవేళ (i < (పూర్ణాంకం)Xt_సంఖ్య(cmdline_options))
	{
	    // Remember finding "-rv" or "-reverse"
	    ఒకవేళ (strcmp("-rv", argv[arg]) == 0
		    || strcmp("-reverse", argv[arg]) == 0)
		దొరికింది_తిరగేయి_arg = TRUE;
	    లేకపోతే ఒకవేళ ((strcmp("-fn", argv[arg]) == 0
			|| strcmp("-font", argv[arg]) == 0)
		    && arg + 1 < *argc)
		font_వాదం = argv[arg + 1];

	    // Found match in table, so move it into gui_argv
	    gui_argv[gui_argc++] = argv[arg];
	    ఒకవేళ (--*argc > arg)
	    {
		mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
						    * పరిమాణం(అక్షరం *));
		ఒకవేళ (cmdline_options[i].arg_రకం != XrmoptionNoArg)
		{
		    // Move the options argument as well
		    gui_argv[gui_argc++] = argv[arg];
		    ఒకవేళ (--*argc > arg)
			mch_memmove(&argv[arg], &argv[arg + 1], (*argc - arg)
							    * పరిమాణం(అక్షరం *));
		}
	    }
	    argv[*argc] = ఖాళీ;
	}
	లేకపోతే
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	    ఒకవేళ (అక్షరశ్రేణి_పోలిక("-nb", argv[arg], 3) == 0)
	{
	    gui.dofork = false;	// don't fork() when starting GUI
	    netbeansArg = argv[arg];
	    mch_memmove(&argv[arg], &argv[arg + 1],
					    (--*argc - arg) * పరిమాణం(అక్షరం *));
	    argv[*argc] = ఖాళీ;
	}
	లేకపోతే
#అయితే_ముగింపు
	    arg++;
    }
}

#నిర్వచితం_కాకపోతే XtSpecificationRelease
# define CARDINAL (Cardinal *)
#లేకపోతే
# ఒకవేళ XtSpecificationRelease == 4
#  define CARDINAL (Cardinal *)
# లేకపోతే
#  define CARDINAL (పూర్ణాంకం *)
# endif
#అయితే_ముగింపు

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
gui_mch_init_check(శూన్య)
{
#నిర్వచితమైతే FEAT_XIM
    Xt_సమితి_Language_Proc(ఖాళీ, ఖాళీ, ఖాళీ);
#అయితే_ముగింపు
    open_app_context();
    ఒకవేళ (app_context != ఖాళీ)
	gui.dpy = XtOpenDisplay(app_context, 0, VIM_NAME, VIM_CLASS,
		cmdline_options, Xt_సంఖ్య(cmdline_options),
		CARDINAL &gui_argc, gui_argv);

#ఒకవేళ defined(LC_NUMERIC)
    {
	// The call to XtOpenDisplay() may have set the locale from the
	// environment. Set LC_NUMERIC to "C" to make sure that strtod() uses a
	// decimal point, not a comma.
	అక్షరం *p = setlocale(LC_NUMERIC, ఖాళీ);

	ఒకవేళ (p == ఖాళీ || strcmp(p, "C") != 0)
	   setlocale(LC_NUMERIC, "C");
    }
#అయితే_ముగింపు
    ఒకవేళ (app_context == ఖాళీ || gui.dpy == ఖాళీ)
    {
	gui.dying = true;
	emsg(_(e_cannot_open_display));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}


#నిర్వచితమైతే USE_XSMP
/*
 * Handle XSMP processing, de-registering the attachment upon error
 */
స్థిర Xt_ఆదానం_Id _xsmp_xtinputid;

    స్థిర శూన్య
local_xsmp_handle_అభ్యర్థనలు(
    Xt_సూచిక	c వాడని,
    పూర్ణాంకం		*s వాడని,
    Xt_ఆదానం_Id	*i వాడని)
{
    ఒకవేళ (xsmp_handle_అభ్యర్థనలు() == FAIL)
	Xt_Remove_ఆదానం(_xsmp_xtinputid);
}
#అయితే_ముగింపు


/*
 * Initialise the X GUI.  Create all the windows, set up all the call-backs etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    పూర్ణాంకం
gui_mch_init(శూన్య)
{
    XtGCMask	gc_ముసుగు;
    XGCValues	gc_vals;
    పూర్ణాంకం		x, y, ముసుగు;
    గుర్తులేని	w, h;

#ఒకవేళ 0
    // Uncomment this to enable synchronous mode for debugging
    XSynchronize(gui.dpy, True);
#అయితే_ముగింపు

    vimShell = XtVaAppCreateShell(VIM_NAME, VIM_CLASS,
	    applicationShellWidgetClass, gui.dpy, ఖాళీ);

    /*
     * Get the application resources
     */
    XtVaGetApplicationResources(vimShell, (Xt_సూచిక)&gui,
	vim_resources, Xt_సంఖ్య(vim_resources), ఖాళీ);

    gui.scrollbar_ఎత్తు = gui.scrollbar_వెడల్పు;

    /*
     * Get the colors ourselves.  Using the automatic conversion doesn't
     * handle looking for approximate colors.
     */
    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#నిర్వచితమైతే FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel = gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#అయితే_ముగింపు

    // Set default foreground and background colours
    gui.norm_pixel = gui.def_norm_pixel;
    gui.వెనుక_pixel = gui.def_వెనుక_pixel;

    // Check if reverse video needs to be applied (on Sun it's done by X)
    ఒకవేళ (gui.rsrc_rev_video && gui_get_lightness(gui.వెనుక_pixel)
					  > gui_get_lightness(gui.norm_pixel))
    {
	gui.norm_pixel = gui.def_వెనుక_pixel;
	gui.వెనుక_pixel = gui.def_norm_pixel;
	gui.def_norm_pixel = gui.norm_pixel;
	gui.def_వెనుక_pixel = gui.వెనుక_pixel;
    }

    // Get the colors from the "Normal", "Tooltip", "Scrollbar" and "Menu"
    // group (set in syntax.c or in a vimrc file)
    సమితి_సాధారణ_colors();

    /*
     * Check that none of the colors are the same as the background color
     */
    gui_check_colors();

    /*
     * Set up the GCs.	The font attributes will be set in gui_init_font().
     */
    gc_ముసుగు = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.వెనుక_pixel;
    gui.పాఠ్యం_gc = XtGetGC(vimShell, gc_ముసుగు, &gc_vals);

    gc_vals.foreground = gui.వెనుక_pixel;
    gc_vals.background = gui.norm_pixel;
    gui.వెనుక_gc = XtGetGC(vimShell, gc_ముసుగు, &gc_vals);

    gc_ముసుగు |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.వెనుక_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.వెనుక_pixel;
    gc_vals.function   = GXxor;
    gui.invert_gc = XtGetGC(vimShell, gc_ముసుగు, &gc_vals);

    gui.visibility = VisibilityUnobscured;
    x11_అమరిక_atoms(gui.dpy);

    ఒకవేళ (gui_win_x != -1 && gui_win_y != -1)
	gui_mch_సమితి_winpos(gui_win_x, gui_win_y);

    // Now adapt the supplied(?) geometry-settings
    // Added by Kjetil Jacobsen <kjetilja@stud.cs.uit.no>
    ఒకవేళ (gui.geom != ఖాళీ && *gui.geom != NUL)
    {
	ముసుగు = XParseGeometry((అక్షరం *)gui.geom, &x, &y, &w, &h);
	ఒకవేళ (ముసుగు & వెడల్పు_విలువ)
	    Columns = w;
	ఒకవేళ (ముసుగు & ఎత్తు_విలువ)
	{
	    ఒకవేళ (p_కిటికీ > (దీర్ఘ)h - 1 || !option_was_సమితి((char_u *)"window"))
		p_కిటికీ = h - 1;
	    అడ్డువరుసలు = h;
	}
	హద్దు_screen_కొలత();
	/*
	 * Set the (x,y) position of the main window only if specified in the
	 * users geometry, so we get good defaults when they don't. This needs
	 * to be done before the shell is popped up.
	 */
	ఒకవేళ (ముసుగు & (XValue|YValue))
	    Xt_Va_సమితి_విలువలు(vimShell, XtNgeometry, gui.geom, ఖాళీ);
    }

    gui_x11_create_widgets();

   /*
    * Add an icon to Vim (Marcel Douben: 11 May 1998).
    */
    ఒకవేళ (vim_strchr(p_go, GO_ICON) != ఖాళీ)
    {
#నిర్వచితం_కాకపోతే HAVE_XPM
# include "vim_icon.xbm"
# include "vim_mask.xbm"

	Arg	arg[2];

	Xt_సమితి_Arg(arg[0], XtNiconPixmap,
		XCreate_Bitmap_From_దత్తాంశం(gui.dpy,
		    Default_మూలం_కిటికీ(gui.dpy),
		    (అక్షరం *)vim_icon_బిట్లు,
		    vim_icon_వెడల్పు,
		    vim_icon_ఎత్తు));
	Xt_సమితి_Arg(arg[1], Xt_Nicon_ముసుగు,
		XCreate_Bitmap_From_దత్తాంశం(gui.dpy,
		    Default_మూలం_కిటికీ(gui.dpy),
		    (అక్షరం *)vim_ముసుగు_icon_బిట్లు,
		    vim_ముసుగు_icon_వెడల్పు,
		    vim_ముసుగు_icon_ఎత్తు));
	Xt_సమితి_విలువలు(vimShell, arg, (Cardinal)2);
#లేకపోతే
// Use Pixmaps, looking much nicer.

# include "../runtime/vim32x32.xpm"
# include "../runtime/vim16x16.xpm"
# include "../runtime/vim48x48.xpm"

    స్థిర Pixmap	icon = 0;
    స్థిర Pixmap	icon_ముసుగు = 0;
    స్థిర అక్షరం		**magick = vim32x32;
    కిటికీ		మూలం_కిటికీ;
    XIcon_కొలత		*కొలత;
    పూర్ణాంకం			సంఖ్య_sizes;
    Display		*dsp;
    Screen		*scr;
    Xpm_లక్షణాలు	attr;
    Colormap		cmap;

    /*
     * Adjust the icon to the preferences of the actual window manager.
     */
    మూలం_కిటికీ = XRoot_కిటికీ_యొక్క_Screen(XtScreen(vimShell));
    ఒకవేళ (XGetIconSizes(XtDisplay(vimShell), మూలం_కిటికీ,
						   &కొలత, &సంఖ్య_sizes) != 0)
    {
	ఒకవేళ (సంఖ్య_sizes > 0)
	{
	    ఒకవేళ (కొలత->max_ఎత్తు_2 >= 48 && కొలత->max_వెడల్పు_2 >= 48)
		magick = vim48x48;
	    లేకపోతే ఒకవేళ (కొలత->max_ఎత్తు_2 >= 32 && కొలత->max_వెడల్పు_2 >= 32)
		magick = vim32x32;
	    లేకపోతే ఒకవేళ (కొలత->max_ఎత్తు_2 >= 16 && కొలత->max_వెడల్పు_2 >= 16)
		magick = vim16x16;
	}
    }

    dsp = XtDisplay(vimShell);
    scr = XtScreen(vimShell);

    cmap = DefaultColormap(dsp, DefaultScreen(dsp));
    Xt_Va_సమితి_విలువలు(vimShell, XtNcolormap, cmap, ఖాళీ);

    attr.valuemask = 0L;
    attr.valuemask = XpmCloseness | XpmReturnPixels | XpmColormap | Xpm_లోతు;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colormap = cmap;
    attr.లోతు = Default_లోతు_యొక్క_Screen(scr);

    ఒకవేళ (!icon)
    {
	Xpm_Create_Pixmap_From_దత్తాంశం(dsp, మూలం_కిటికీ, magick, &icon,
							   &icon_ముసుగు, &attr);
	Xpm_Free_లక్షణాలు(&attr);
    }

    Xt_Va_సమితి_విలువలు(vimShell, XmNiconPixmap, icon, Xm_Nicon_ముసుగు, icon_ముసుగు, ఖాళీ);
#అయితే_ముగింపు
    }

#నిర్వచితమైతే FEAT_BEVAL_GUI
    gui_init_tooltip_font();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
    gui_init_menu_font();
#అయితే_ముగింపు

#నిర్వచితమైతే USE_XSMP
    // Attach listener on ICE connection
    ఒకవేళ (-1 != xsmp_icefd)
	_xsmp_xtinputid = Xt_App_కూడు_ఆదానం(app_context, xsmp_icefd,
		(Xt_సూచిక)Xt_ఆదానం_Read_ముసుగు, local_xsmp_handle_అభ్యర్థనలు, ఖాళీ);
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Called when starting the GUI fails after calling gui_mch_init().
 */
    శూన్య
gui_mch_uninit(శూన్య)
{
    gui_x11_destroy_widgets();
    XtCloseDisplay(gui.dpy);
    gui.dpy = ఖాళీ;
    vimShell = (Widget)0;
    VIM_CLEAR(gui_argv);
}

/*
 * Called when the foreground or background color has been changed.
 */
    శూన్య
gui_mch_new_colors(శూన్య)
{
    long_u	gc_ముసుగు;
    XGCValues	gc_vals;

    gc_ముసుగు = GCForeground | GCBackground;
    gc_vals.foreground = gui.norm_pixel;
    gc_vals.background = gui.వెనుక_pixel;
    ఒకవేళ (gui.పాఠ్యం_gc != ఖాళీ)
	XChangeGC(gui.dpy, gui.పాఠ్యం_gc, gc_ముసుగు, &gc_vals);

    gc_vals.foreground = gui.వెనుక_pixel;
    gc_vals.background = gui.norm_pixel;
    ఒకవేళ (gui.వెనుక_gc != ఖాళీ)
	XChangeGC(gui.dpy, gui.వెనుక_gc, gc_ముసుగు, &gc_vals);

    gc_ముసుగు |= GCFunction;
    gc_vals.foreground = gui.norm_pixel ^ gui.వెనుక_pixel;
    gc_vals.background = gui.norm_pixel ^ gui.వెనుక_pixel;
    gc_vals.function   = GXxor;
    ఒకవేళ (gui.invert_gc != ఖాళీ)
	XChangeGC(gui.dpy, gui.invert_gc, gc_ముసుగు, &gc_vals);

    gui_x11_సమితి_వెనుక_color();
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    పూర్ణాంకం
gui_mch_open(శూన్య)
{
    // Actually open the window
    XtRealizeWidget(vimShell);
    Xt_Manage_బిడ్డ(Xt_Name_గమ్యం_Widget(vimShell, "*vimForm"));

    gui.wid = gui_x11_get_wid();
    gui.blank_సూచిక = gui_x11_create_blank_mouse();

    /*
     * Add a callback for the Close item on the window managers menu, and the
     * save-yourself event.
     */
    wm_atoms[SAVE_YOURSELF_IDX] =
			      XInternAtom(gui.dpy, "WM_SAVE_YOURSELF", False);
    wm_atoms[DELETE_కిటికీ_IDX] =
			      XInternAtom(gui.dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(gui.dpy, Xt_కిటికీ(vimShell), wm_atoms, 2);
    Xt_కూడు_Event_Handler(vimShell, No_Event_ముసుగు, True, gui_x11_wm_protocol_handler,
							     ఖాళీ);
#నిర్వచితమైతే HAVE_X11_XMU_EDITRES_H
    /*
     * Enable editres protocol (see "man editres").
     * Usually will need to add -lXmu to the linker line as well.
     */
    Xt_కూడు_Event_Handler(vimShell, (Event_ముసుగు)0, True, _XEditResCheckMessages,
	    (Xt_సూచిక)ఖాళీ);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER
    ఒకవేళ (serverName == ఖాళీ && server_Delayed_ప్రారంభం_Name != ఖాళీ)
    {
	// This is a :gui command in a plain vim with no previous server
	comm_కిటికీ = Xt_కిటికీ(vimShell);
	(శూన్య)server_రిజిస్టరు_Name(gui.dpy, server_Delayed_ప్రారంభం_Name);
    }
    లేకపోతే
    {
	/*
	 * Cannot handle "widget-less" windows with XtProcessEvent() we'll
	 * have to change the "server" registration to that of the main window
	 * If we have not registered a name yet, remember the window
	 */
	server_Change_Registered_కిటికీ(gui.dpy, Xt_కిటికీ(vimShell));
    }
    Xt_కూడు_Event_Handler(vimShell, Property_Change_ముసుగు, False,
		      gui_x11_send_event_handler, ఖాళీ);
#అయితే_ముగింపు

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them)
    highlight_gui_started();		// re-init colors and fonts

#నిర్వచితమైతే FEAT_XIM
    xim_init();
#అయితే_ముగింపు

    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_BEVAL_GUI)
/*
 * Convert the tooltip fontset name to an XFontSet.
 */
    శూన్య
gui_init_tooltip_font(శూన్య)
{
    Xrm_విలువ from, గమ్యం;

    from.addr = (అక్షరం *)gui.rsrc_tooltip_font_name;
    from.కొలత = అక్షరశ్రేణి_పొడవు(from.addr);
    గమ్యం.addr = (Xt_సూచిక)&gui.tooltip_fontset;
    గమ్యం.కొలత = పరిమాణం(XFont_సమితి);

    ఒకవేళ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_సమితి, &గమ్యం) == False)
    {
	// Failed. What to do?
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MENU)
// Convert the menu font/fontset name to an XFontStruct/XFontset
    శూన్య
gui_init_menu_font(శూన్య)
{
    Xrm_విలువ from, గమ్యం;

# ifdef FONTSET_ALWAYS
    from.addr = (అక్షరం *)gui.rsrc_menu_font_name;
    from.కొలత = అక్షరశ్రేణి_పొడవు(from.addr);
    గమ్యం.addr = (Xt_సూచిక)&gui.menu_fontset;
    గమ్యం.కొలత = పరిమాణం(GuiFontset);

    ఒకవేళ (XtConvertAndStore(vimShell, XtRString, &from, Xt_RFont_సమితి, &గమ్యం) == False)
    {
	// Failed. What to do?
    }
# లేకపోతే
    from.addr = (అక్షరం *)gui.rsrc_menu_font_name;
    from.కొలత = అక్షరశ్రేణి_పొడవు(from.addr);
    గమ్యం.addr = (Xt_సూచిక)&gui.menu_font;
    గమ్యం.కొలత = పరిమాణం(GuiFont);

    ఒకవేళ (XtConvertAndStore(vimShell, XtRString, &from, XtRFontStruct, &గమ్యం) == False)
    {
	// Failed. What to do?
    }
# endif
}
#అయితే_ముగింపు

    శూన్య
gui_mch_exit(పూర్ణాంకం rc వాడని)
{
#ఒకవేళ 0
    // Lesstif gives an error message here, and so does Solaris.  The man page
    // says that this isn't needed when exiting, so just skip it.
    XtCloseDisplay(gui.dpy);
#అయితే_ముగింపు
    VIM_CLEAR(gui_argv);
}

/*
 * Get the position of the top left corner of the window.
 */
    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    Dimension	xpos, ypos;

    Xt_Va_Get_విలువలు(vimShell,
	XtNx,	&xpos,
	XtNy,	&ypos,
	ఖాళీ);
    *x = xpos;
    *y = ypos;
    తిరిగి OK;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    Xt_Va_సమితి_విలువలు(vimShell,
	XtNx,	x,
	XtNy,	y,
	ఖాళీ);
}

    శూన్య
gui_mch_సమితి_shellsize(
    పూర్ణాంకం		వెడల్పు,
    పూర్ణాంకం		ఎత్తు,
    పూర్ణాంకం		min_వెడల్పు,
    పూర్ణాంకం		min_ఎత్తు,
    పూర్ణాంకం		పునాది_వెడల్పు,
    పూర్ణాంకం		పునాది_ఎత్తు,
    పూర్ణాంకం		direction వాడని)
{
#నిర్వచితమైతే FEAT_XIM
    ఎత్తు += xim_get_స్థితి_area_ఎత్తు(),
#అయితే_ముగింపు
    Xt_Va_సమితి_విలువలు(vimShell,
	XtNwidthInc,	gui.char_వెడల్పు,
	XtNheightInc,	gui.char_ఎత్తు,
#ఒకవేళ defined(XtSpecificationRelease) && XtSpecificationRelease >= 4
	Xt_Nbase_వెడల్పు,	పునాది_వెడల్పు,
	Xt_Nbase_ఎత్తు,	పునాది_ఎత్తు,
#అయితే_ముగింపు
	Xt_Nmin_వెడల్పు,	min_వెడల్పు,
	Xt_Nmin_ఎత్తు,	min_ఎత్తు,
	XtNwidth,	వెడల్పు,
	XtNheight,	ఎత్తు,
	ఖాళీ);
}

/*
 * Allow 10 pixels for horizontal borders, 'guiheadroom' for vertical borders.
 * Is there no way in X to find out how wide the borders really are?
 */
    శూన్య
gui_mch_get_screen_dimensions(
    పూర్ణాంకం	    *screen_w,
    పూర్ణాంకం	    *screen_h)
{
    *screen_w = Display_వెడల్పు(gui.dpy, DefaultScreen(gui.dpy)) - 10;
    *screen_h = Display_ఎత్తు(gui.dpy, DefaultScreen(gui.dpy)) - p_ghr;
}

/*
 * Initialise vim to use the font "font_name".  If it's NULL, pick a default
 * font.
 * If "fontset" is TRUE, load the "font_name" as a fontset.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    పూర్ణాంకం
gui_mch_init_font(
    char_u	*font_name,
    పూర్ణాంకం		do_fontset వాడని)
{
    XFontStruct	*font = ఖాళీ;

#నిర్వచితమైతే FEAT_XFONTSET
    XFont_సమితి	fontset = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_MOTIF
    // A font name equal "*" is indicating, that we should activate the font
    // selection dialogue to get a new font name. So let us do it here.
    ఒకవేళ (font_name != ఖాళీ && STRCMP(font_name, "*") == 0)
    {
	font_name = gui_xm_select_font(hl_get_font_name());

	// Do not reset to default font except on GUI startup.
	ఒకవేళ (font_name == ఖాళీ && !gui.starting)
	    తిరిగి OK;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XFONTSET
    ఒకవేళ (do_fontset)
    {
	// If 'guifontset' is set, VIM treats all font specifications as if
	// they were fontsets, and 'guifontset' becomes the default.
	ఒకవేళ (font_name != ఖాళీ)
	{
	    fontset = (XFont_సమితి)gui_mch_get_fontset(font_name, FALSE, TRUE);
	    ఒకవేళ (fontset == ఖాళీ)
		తిరిగి FAIL;
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	ఒకవేళ (font_name == ఖాళీ)
	{
	    /*
	     * If none of the fonts in 'font' could be loaded, try the one set
	     * in the X resource, and finally just try using DFLT_FONT, which
	     * will hopefully always be there.
	     */
	    font_name = gui.rsrc_font_name;
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	    ఒకవేళ (font == ఖాళీ)
		font_name = (char_u *)DFLT_FONT;
	}
	ఒకవేళ (font == ఖాళీ)
	    font = (XFontStruct *)gui_mch_get_font(font_name, FALSE);
	ఒకవేళ (font == ఖాళీ)
	    తిరిగి FAIL;
    }

    gui_mch_free_font(gui.norm_font);
#నిర్వచితమైతే FEAT_XFONTSET
    gui_mch_free_fontset(gui.fontset);

    ఒకవేళ (fontset != ఖాళీ)
    {
	gui.norm_font = NOFONT;
	gui.fontset = (GuiFontset)fontset;
	gui.char_వెడల్పు = fontset_వెడల్పు(fontset);
	gui.char_ఎత్తు = fontset_ఎత్తు(fontset) + p_linespace;
	gui.char_ascent = fontset_ascent(fontset) + p_linespace / 2;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	gui.norm_font = (GuiFont)font;
#నిర్వచితమైతే FEAT_XFONTSET
	gui.fontset = NOFONTSET;
#అయితే_ముగింపు
	gui.char_వెడల్పు = font->max_bounds.వెడల్పు;
	gui.char_ఎత్తు = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }

    hl_సమితి_font_name(font_name);

    /*
     * Try to load other fonts for bold, italic, and bold-italic.
     * We should also try to work out what font to use for these when they are
     * not specified by X resources, but we don't yet.
     */
    ఒకవేళ (font_name == gui.rsrc_font_name)
    {
	ఒకవేళ (gui.bold_font == NOFONT
		&& gui.rsrc_bold_font_name != ఖాళీ
		&& *gui.rsrc_bold_font_name != NUL)
	    gui.bold_font = gui_mch_get_font(gui.rsrc_bold_font_name, FALSE);
	ఒకవేళ (gui.ital_font == NOFONT
		&& gui.rsrc_ital_font_name != ఖాళీ
		&& *gui.rsrc_ital_font_name != NUL)
	    gui.ital_font = gui_mch_get_font(gui.rsrc_ital_font_name, FALSE);
	ఒకవేళ (gui.boldital_font == NOFONT
		&& gui.rsrc_boldital_font_name != ఖాళీ
		&& *gui.rsrc_boldital_font_name != NUL)
	    gui.boldital_font = gui_mch_get_font(gui.rsrc_boldital_font_name,
								       FALSE);
    }
    లేకపోతే
    {
	// When not using the font specified by the resources, also don't use
	// the bold/italic fonts, otherwise setting 'guifont' will look very
	// strange.
	ఒకవేళ (gui.bold_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.bold_font);
	    gui.bold_font = NOFONT;
	}
	ఒకవేళ (gui.ital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.ital_font);
	    gui.ital_font = NOFONT;
	}
	ఒకవేళ (gui.boldital_font != NOFONT)
	{
	    XFreeFont(gui.dpy, (XFontStruct *)gui.boldital_font);
	    gui.boldital_font = NOFONT;
	}
    }

#నిర్వచితమైతే FEAT_GUI_MOTIF
    gui_motif_synch_fonts();
#అయితే_ముగింపు

    తిరిగి OK;
}

/*
 * Get a font structure for highlighting.
 */
    GuiFont
gui_mch_get_font(char_u *name, పూర్ణాంకం giveErrorIfMissing)
{
    XFontStruct	*font;

    ఒకవేళ (!gui.లోపల_use || name == ఖాళీ)	// can't do this when GUI not running
	తిరిగి NOFONT;

    font = XLoadQueryFont(gui.dpy, (అక్షరం *)name);

    ఒకవేళ (font == ఖాళీ)
    {
	ఒకవేళ (giveErrorIfMissing)
	    semsg(_(e_unknown_font_str), name);
	తిరిగి NOFONT;
    }

#నిర్వచితమైతే DEBUG
    రాయి("Font Information for '%s':\n", name);
    రాయి("  w = %d, h = %d, ascent = %d, descent = %d\n",
	   font->max_bounds.వెడల్పు, font->ascent + font->descent,
	   font->ascent, font->descent);
    రాయి("  max ascent = %d, max descent = %d, max h = %d\n",
	   font->max_bounds.ascent, font->max_bounds.descent,
	   font->max_bounds.ascent + font->max_bounds.descent);
    రాయి("  min lbearing = %d, min rbearing = %d\n",
	   font->min_bounds.lbearing, font->min_bounds.rbearing);
    రాయి("  max lbearing = %d, max rbearing = %d\n",
	   font->max_bounds.lbearing, font->max_bounds.rbearing);
    రాయి("  leftink = %d, rightink = %d\n",
	   (font->min_bounds.lbearing < 0),
	   (font->max_bounds.rbearing > font->max_bounds.వెడల్పు));
    రాయి("\n");
#అయితే_ముగింపు

    ఒకవేళ (font->max_bounds.వెడల్పు != font->min_bounds.వెడల్పు)
    {
	semsg(_(e_font_str_is_not_fixed_వెడల్పు), name);
	XFreeFont(gui.dpy, font);
	తిరిగి NOFONT;
    }
    తిరిగి (GuiFont)font;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name)
{
    char_u *ret = ఖాళీ;

    ఒకవేళ (name != ఖాళీ && font == ఖాళీ)
    {
	// In this case, there's no way other than doing this.
	ret = vim_strsave(name);
    }
    లేకపోతే ఒకవేళ (font != ఖాళీ)
    {
	// In this case, try to retrieve the XLFD corresponding to 'font'->fid;
	// if failed, use 'name' unless it's NULL.
	గుర్తులేని దీర్ఘ విలువ = 0L;

	ఒకవేళ (XGetFontProperty(font, XA_FONT, &విలువ))
	{
	    అక్షరం *xa_font_name = ఖాళీ;

	    xa_font_name = XGetAtomName(gui.dpy, విలువ);
	    ఒకవేళ (xa_font_name != ఖాళీ)
	    {
		ret = vim_strsave((char_u *)xa_font_name);
		XFree(xa_font_name);
	    }
	    లేకపోతే ఒకవేళ (name != ఖాళీ)
		ret = vim_strsave(name);
	}
	లేకపోతే ఒకవేళ (name != ఖాళీ)
	    ret = vim_strsave(name);
    }
    తిరిగి ret;
}
#అయితే_ముగింపు

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    పూర్ణాంకం
gui_mch_adjust_charheight(శూన్య)
{
#నిర్వచితమైతే FEAT_XFONTSET
    ఒకవేళ (gui.fontset != NOFONTSET)
    {
	gui.char_ఎత్తు = fontset_ఎత్తు((XFont_సమితి)gui.fontset) + p_linespace;
	gui.char_ascent = fontset_ascent((XFont_సమితి)gui.fontset)
							    + p_linespace / 2;
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	XFontStruct *font = (XFontStruct *)gui.norm_font;

	gui.char_ఎత్తు = font->ascent + font->descent + p_linespace;
	gui.char_ascent = font->ascent + p_linespace / 2;
    }
    తిరిగి OK;
}

/*
 * Set the current text font.
 */
    శూన్య
gui_mch_సమితి_font(GuiFont font)
{
    స్థిర Font	prev_font = (Font)-1;
    Font	fid = ((XFontStruct *)font)->fid;

    ఒకవేళ (fid != prev_font)
    {
	XSetFont(gui.dpy, gui.పాఠ్యం_gc, fid);
	XSetFont(gui.dpy, gui.వెనుక_gc, fid);
	prev_font = fid;
	gui.char_ascent = ((XFontStruct *)font)->ascent + p_linespace / 2;
    }
#నిర్వచితమైతే FEAT_XFONTSET
    current_fontset = (XFont_సమితి)ఖాళీ;
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_XFONTSET)
/*
 * Set the current text fontset.
 * Adjust the ascent, in case it's different.
 */
    శూన్య
gui_mch_సమితి_fontset(GuiFontset fontset)
{
    current_fontset = (XFont_సమితి)fontset;
    gui.char_ascent = fontset_ascent(current_fontset) + p_linespace / 2;
}
#అయితే_ముగింపు

/*
 * If a font is not going to be used, free its structure.
 */
    శూన్య
gui_mch_free_font(GuiFont font)
{
    ఒకవేళ (font != NOFONT)
	XFreeFont(gui.dpy, (XFontStruct *)font);
}

#ఒకవేళ defined(FEAT_XFONTSET)
/*
 * If a fontset is not going to be used, free its structure.
 */
    శూన్య
gui_mch_free_fontset(GuiFontset fontset)
{
    ఒకవేళ (fontset != NOFONTSET)
	XFree_Font_సమితి(gui.dpy, (XFont_సమితి)fontset);
}

/*
 * Load the fontset "name".
 * Return a reference to the fontset, or NOFONTSET when failing.
 */
    GuiFontset
gui_mch_get_fontset(
    char_u	*name,
    పూర్ణాంకం		giveErrorIfMissing,
    పూర్ణాంకం		fixed_వెడల్పు)
{
    XFont_సమితి	fontset;
    అక్షరం	**missing, *def_str;
    పూర్ణాంకం		అంకె_missing;

    ఒకవేళ (!gui.లోపల_use || name == ఖాళీ)
	తిరిగి NOFONTSET;

    fontset = XCreate_Font_సమితి(gui.dpy, (అక్షరం *)name, &missing, &అంకె_missing,
			     &def_str);
    ఒకవేళ (అంకె_missing > 0)
    {
	పూర్ణాంకం i;

	ఒకవేళ (giveErrorIfMissing)
	{
	    semsg(_(e_fonts_for_the_following_charsets_are_missing_లోపల_fontset), name);
	    కొరకు (i = 0; i < అంకె_missing; i++)
		semsg("%s", missing[i]);
	}
	XFreeStringList(missing);
    }

    ఒకవేళ (fontset == ఖాళీ)
    {
	ఒకవేళ (giveErrorIfMissing)
	    semsg(_(e_unknown_fontset_str), name);
	తిరిగి NOFONTSET;
    }

    ఒకవేళ (fixed_వెడల్పు && check_fontset_sanity(fontset) == FAIL)
    {
	XFree_Font_సమితి(gui.dpy, fontset);
	తిరిగి NOFONTSET;
    }
    తిరిగి (GuiFontset)fontset;
}

/*
 * Check if fontset "fs" is fixed width.
 */
    స్థిర పూర్ణాంకం
check_fontset_sanity(XFont_సమితి fs)
{
    XFontStruct	**xfs;
    అక్షరం	**font_name;
    పూర్ణాంకం		fn;
    అక్షరం	*పునాది_name;
    పూర్ణాంకం		i;
    పూర్ణాంకం		min_వెడల్పు;
    పూర్ణాంకం		min_font_idx = 0;

    పునాది_name = XBase_Font_Name_List_యొక్క_Font_సమితి(fs);
    fn = XFonts_యొక్క_Font_సమితి(fs, &xfs, &font_name);
    కొరకు (i = 0; i < fn; i++)
    {
	ఒకవేళ (xfs[i]->max_bounds.వెడల్పు != xfs[i]->min_bounds.వెడల్పు)
	{
	    semsg(_(e_fontsent_name_str_font_str_is_not_fixed_వెడల్పు),
		    పునాది_name, font_name[i]);
	    తిరిగి FAIL;
	}
    }
    // scan base font width
    min_వెడల్పు = 32767;
    కొరకు (i = 0; i < fn; i++)
    {
	ఒకవేళ (xfs[i]->max_bounds.వెడల్పు<min_వెడల్పు)
	{
	    min_వెడల్పు = xfs[i]->max_bounds.వెడల్పు;
	    min_font_idx = i;
	}
    }
    కొరకు (i = 0; i < fn; i++)
    {
	ఒకవేళ (	   xfs[i]->max_bounds.వెడల్పు != 2 * min_వెడల్పు
		&& xfs[i]->max_bounds.వెడల్పు != min_వెడల్పు)
	{
	    semsg(_(e_fontset_name_str), పునాది_name);
	    semsg(_("Font0: %s"), font_name[min_font_idx]);
	    semsg(_("Font%d: %s"), i, font_name[i]);
	    semsg(_("Font%d width is not twice that of font0"), i);
	    semsg(_("Font0 width: %d"),
				     (పూర్ణాంకం)xfs[min_font_idx]->max_bounds.వెడల్పు);
	    semsg(_("Font%d width: %d"), i, (పూర్ణాంకం)xfs[i]->max_bounds.వెడల్పు);
	    తిరిగి FAIL;
	}
    }
    // it seems ok. Good Luck!!
    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
fontset_వెడల్పు(XFont_సమితి fs)
{
 తిరిగి Xmb_పాఠ్యం_Escapement(fs, "Vim", 3) / 3;
}

    పూర్ణాంకం
fontset_ఎత్తు(
    XFont_సమితి fs)
{
    XFont_సమితి_Extents *extents;

    extents = XExtents_యొక్క_Font_సమితి(fs);
    తిరిగి extents->max_logical_extent.ఎత్తు;
}

# ఒకవేళ 0
// NOT USED YET
    స్థిర పూర్ణాంకం
fontset_descent(XFont_సమితి fs)
{
    XFont_సమితి_Extents *extents;

    extents = XExtents_యొక్క_Font_సమితి (fs);
    తిరిగి extents->max_logical_extent.ఎత్తు + extents->max_logical_extent.y;
}
# endif

    స్థిర పూర్ణాంకం
fontset_ascent(XFont_సమితి fs)
{
    XFont_సమితి_Extents *extents;

    extents = XExtents_యొక్క_Font_సమితి(fs);
    తిరిగి -extents->max_logical_extent.y;
}

#అయితే_ముగింపు // FEAT_XFONTSET

/*
 * Return the Pixel value (color) for the given color name.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T	requested;

    // can't do this when GUI not running
    ఒకవేళ (!gui.లోపల_use || name == ఖాళీ || *name == NUL)
	తిరిగి INVALCOLOR;

    requested = gui_get_color_cmn(name);
    ఒకవేళ (requested == INVALCOLOR)
	తిరిగి INVALCOLOR;

    తిరిగి gui_mch_get_rgb_color(
	    (requested & 0xff0000) >> 16,
	    (requested & 0xff00) >> 8,
	    requested & 0xff);
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    XColor	అందుబాటు;
    Colormap	colormap;

#ఒకవేళ 0
// Using XParseColor() is very slow, put rgb in XColor directly.

    అక్షరం	spec[8]; // space enough to hold "#RRGGBB"
    vim_snprintf(spec, పరిమాణం(spec), "#%.2x%.2x%.2x", r, g, b);
    ఒకవేళ (XParseColor(gui.dpy, colormap, (అక్షరం *)spec, &అందుబాటు) != 0
	    && XAllocColor(gui.dpy, colormap, &అందుబాటు) != 0)
	తిరిగి (guicolor_T)అందుబాటు.pixel;
#అయితే_ముగింపు
    colormap = DefaultColormap(gui.dpy, DefaultScreen(gui.dpy));
    CLEAR_క్షేత్రం(అందుబాటు);
    అందుబాటు.red = r << 8;
    అందుబాటు.green = g << 8;
    అందుబాటు.blue = b << 8;
    ఒకవేళ (XAllocColor(gui.dpy, colormap, &అందుబాటు) != 0)
	తిరిగి (guicolor_T)అందుబాటు.pixel;

    తిరిగి INVALCOLOR;
}

/*
 * Set the current text foreground color.
 */
    శూన్య
gui_mch_సమితి_fg_color(guicolor_T color)
{
    ఒకవేళ (color == prev_fg_color)
	తిరిగి;

    XSetForeground(gui.dpy, gui.పాఠ్యం_gc, (Pixel)color);
    prev_fg_color = color;
}

/*
 * Set the current text background color.
 */
    శూన్య
gui_mch_సమితి_bg_color(guicolor_T color)
{
    ఒకవేళ (color == prev_bg_color)
	తిరిగి;

    XSetBackground(gui.dpy, gui.పాఠ్యం_gc, (Pixel)color);
    prev_bg_color = color;
}

/*
 * Set the current text special color.
 */
    శూన్య
gui_mch_సమితి_sp_color(guicolor_T color)
{
    prev_sp_color = color;
}

/*
 * create a mouse pointer that is blank
 */
    స్థిర Cursor
gui_x11_create_blank_mouse(శూన్య)
{
    Pixmap blank_pixmap = XCreatePixmap(gui.dpy, gui.wid, 1, 1, 1);
    GC gc = XCreateGC(gui.dpy, blank_pixmap, (గుర్తులేని దీర్ఘ)0, (XGCValues*)0);

    ఒకవేళ (gc != ఖాళీ)
    {
	XDraw_బిందువు(gui.dpy, blank_pixmap, gc, 0, 0);
	XFreeGC(gui.dpy, gc);
    }
    తిరిగి XCreatePixmapCursor(gui.dpy, blank_pixmap, blank_pixmap,
		     (XColor*)&gui.norm_pixel, (XColor*)&gui.norm_pixel, 0, 0);
}

/*
 * Draw a curled line at the bottom of the character cell.
 */
    స్థిర శూన్య
draw_curl(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం cells)
{
    పూర్ణాంకం			i;
    పూర్ణాంకం			స్థానభ్రంశం;
    స్థిర స్థిరరాశి పూర్ణాంకం	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };

    XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_sp_color);
    కొరకు (i = FILL_X(నిలువు); i < FILL_X(నిలువు + cells); ++i)
    {
	స్థానభ్రంశం = val[i % 8];
	XDraw_బిందువు(gui.dpy, gui.wid, gui.పాఠ్యం_gc, i,
						FILL_Y(అడ్డువరుస + 1) - 1 - స్థానభ్రంశం);
    }
    XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_fg_color);
}

    శూన్య
gui_mch_draw_string(
    పూర్ణాంకం		అడ్డువరుస,
    పూర్ణాంకం		నిలువు,
    char_u	*s,
    పూర్ణాంకం		len,
    పూర్ణాంకం		flags)
{
    పూర్ణాంకం			cells = len;
    స్థిర శూన్య		*buf = ఖాళీ;
    స్థిర పూర్ణాంకం		buflen = 0;
    char_u		*p;
    పూర్ణాంకం			wlen = 0;
    పూర్ణాంకం			c;

    ఒకవేళ (enc_utf8)
    {
	// Convert UTF-8 byte sequence to 16 bit characters for the X
	// functions.  Need a buffer for the 16 bit characters.  Keep it
	// between calls, because allocating it each time is slow.
	ఒకవేళ (buflen < len)
	{
	    XtFree((అక్షరం *)buf);
	    buf = (శూన్య *)XtMalloc(len * (పరిమాణం(XChar2b) < పరిమాణం(wchar_t)
					? పరిమాణం(wchar_t) : పరిమాణం(XChar2b)));
	    buflen = len;
	}
	p = s;
	cells = 0;
	అయితే (p < s + len)
	{
	    c = utf_ptr2char(p);
#నిర్వచితమైతే FEAT_XFONTSET
	    ఒకవేళ (current_fontset != ఖాళీ)
	    {
# ifdef SMALL_WCHAR_T
		ఒకవేళ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
# endif
		((wchar_t *)buf)[wlen] = c;
	    }
	    లేకపోతే
#అయితే_ముగింపు
	    {
		ఒకవేళ (c >= 0x10000)
		    c = 0xbf;		// show chars > 0xffff as ?
		((XChar2b *)buf)[wlen].byte1 = (గుర్తులేని)c >> 8;
		((XChar2b *)buf)[wlen].byte2 = c;
	    }
	    ++wlen;
	    cells += utf_char2cells(c);
	    p += utf_ptr2len(p);
	}
    }
    లేకపోతే ఒకవేళ (has_mbyte)
    {
	cells = 0;
	కొరకు (p = s; p < s + len; )
	{
	    cells += ptr2cells(p);
	    p += (*mb_ptr2len)(p);
	}
    }

#నిర్వచితమైతే FEAT_XFONTSET
    ఒకవేళ (current_fontset != ఖాళీ)
    {
	// Setup a clip rectangle to avoid spilling over in the next or
	// previous line.  This is apparently needed for some fonts which are
	// used in a fontset.
	XRectangle	clip;

	clip.x = 0;
	clip.y = 0;
	clip.ఎత్తు = gui.char_ఎత్తు;
	clip.వెడల్పు = gui.char_వెడల్పు * cells + 1;
	XSetClipRectangles(gui.dpy, gui.పాఠ్యం_gc, FILL_X(నిలువు), FILL_Y(అడ్డువరుస),
		&clip, 1, Unsorted);
    }
#అయితే_ముగింపు

    ఒకవేళ (flags & DRAW_TRANSP)
    {
	ఒకవేళ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), buf, wlen);
	లేకపోతే
	    XDrawString(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), (అక్షరం *)s, len);
    }
    లేకపోతే ఒకవేళ (p_linespace != 0
#నిర్వచితమైతే FEAT_XFONTSET
	    || current_fontset != ఖాళీ
#అయితే_ముగింపు
	    )
    {
	XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_bg_color);
	XFillRectangle(gui.dpy, gui.wid, gui.పాఠ్యం_gc, FILL_X(నిలువు),
		FILL_Y(అడ్డువరుస), gui.char_వెడల్పు * cells, gui.char_ఎత్తు);
	XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_fg_color);

	ఒకవేళ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), buf, wlen);
	లేకపోతే
	    XDrawString(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), (అక్షరం *)s, len);
    }
    లేకపోతే
    {
	// XmbDrawImageString has bug, don't use it for fontset.
	ఒకవేళ (enc_utf8)
	    XDrawImageString16(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), buf, wlen);
	లేకపోతే
	    XDrawImageString(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస), (అక్షరం *)s, len);
    }

    // Bold trick: draw the text again with a one-pixel offset.
    ఒకవేళ (flags & DRAW_BOLD)
    {
	ఒకవేళ (enc_utf8)
	    XDrawString16(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు) + 1,
		    పాఠ్యం_Y(అడ్డువరుస), buf, wlen);
	లేకపోతే
	    XDrawString(gui.dpy, gui.wid, gui.పాఠ్యం_gc, పాఠ్యం_X(నిలువు) + 1,
		    పాఠ్యం_Y(అడ్డువరుస), (అక్షరం *)s, len);
    }

    // Undercurl: draw curl at the bottom of the character cell.
    ఒకవేళ (flags & DRAW_UNDERC)
	draw_curl(అడ్డువరుస, నిలువు, cells);

    // Underline: draw a line at the bottom of the character cell.
    ఒకవేళ (flags & DRAW_UNDERL)
    {
	పూర్ణాంకం	y = FILL_Y(అడ్డువరుస + 1) - 1;

	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ఒకవేళ (p_linespace > 1)
	    y -= p_linespace - 1;
	XDraw_పంక్తి(gui.dpy, gui.wid, gui.పాఠ్యం_gc, FILL_X(నిలువు),
		y, FILL_X(నిలువు + cells) - 1, y);
    }

    ఒకవేళ (flags & DRAW_STRIKE)
    {
	పూర్ణాంకం	y = FILL_Y(అడ్డువరుస + 1) - gui.char_ఎత్తు/2;

	XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_sp_color);
	XDraw_పంక్తి(gui.dpy, gui.wid, gui.పాఠ్యం_gc, FILL_X(నిలువు),
		y, FILL_X(నిలువు + cells) - 1, y);
	XSetForeground(gui.dpy, gui.పాఠ్యం_gc, prev_fg_color);
    }

#నిర్వచితమైతే FEAT_XFONTSET
    ఒకవేళ (current_fontset != ఖాళీ)
	XSet_Clip_ముసుగు(gui.dpy, gui.పాఠ్యం_gc, None);
#అయితే_ముగింపు
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    పూర్ణాంకం
gui_mch_haskey(char_u *name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != (కీలకం_Sym)0; i++)
	ఒకవేళ (name[0] == special_కీలకాలు[i].vim_code0 &&
					 name[1] == special_కీలకాలు[i].vim_code1)
	    తిరిగి OK;
    తిరిగి FAIL;
}

/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    పూర్ణాంకం
gui_get_x11_windis(కిటికీ *win, Display **dis)
{
    *win = Xt_కిటికీ(vimShell);
    *dis = gui.dpy;
    తిరిగి OK;
}

    శూన్య
gui_mch_beep(శూన్య)
{
    XBell(gui.dpy, 0);
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
    // Do a visual beep by reversing the foreground and background colors
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((పూర్ణాంకం)Columns) + gui.border_స్థానభ్రంశం,
	    FILL_Y((పూర్ణాంకం)అడ్డువరుసలు) + gui.border_స్థానభ్రంశం);
    XSync(gui.dpy, False);
    ui_delay((దీర్ఘ)msec, TRUE);	// wait for a few msec
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc, 0, 0,
	    FILL_X((పూర్ణాంకం)Columns) + gui.border_స్థానభ్రంశం,
	    FILL_Y((పూర్ణాంకం)అడ్డువరుసలు) + gui.border_స్థానభ్రంశం);
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    శూన్య
gui_mch_invert_rectangle(
    పూర్ణాంకం	    r,
    పూర్ణాంకం	    c,
    పూర్ణాంకం	    nr,
    పూర్ణాంకం	    nc)
{
    XFillRectangle(gui.dpy, gui.wid, gui.invert_gc,
	FILL_X(c), FILL_Y(r), (nc) * gui.char_వెడల్పు, (nr) * gui.char_ఎత్తు);
}

/*
 * Iconify the GUI window.
 */
    శూన్య
gui_mch_iconify(శూన్య)
{
    XIconify_కిటికీ(gui.dpy, Xt_కిటికీ(vimShell), DefaultScreen(gui.dpy));
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    XMapRaised(gui.dpy, Xt_కిటికీ(vimShell));
}
#అయితే_ముగింపు

/*
 * Draw a cursor without focus.
 */
    శూన్య
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    పూర్ణాంకం		w = 1;

    ఒకవేళ (mb_lefthalve(gui.అడ్డువరుస, gui.నిలువు))
	w = 2;
    gui_mch_సమితి_fg_color(color);
    XDrawRectangle(gui.dpy, gui.wid, gui.పాఠ్యం_gc, FILL_X(gui.నిలువు),
	    FILL_Y(gui.అడ్డువరుస), w * gui.char_వెడల్పు - 1, gui.char_ఎత్తు - 1);
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    శూన్య
gui_mch_draw_part_cursor(పూర్ణాంకం w, పూర్ణాంకం h, guicolor_T color)
{
    gui_mch_సమితి_fg_color(color);

    XFillRectangle(gui.dpy, gui.wid, gui.పాఠ్యం_gc,
#నిర్వచితమైతే FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_కుడి ? FILL_X(gui.నిలువు + 1) - w :
#అయితే_ముగింపు
		FILL_X(gui.నిలువు),
	    FILL_Y(gui.అడ్డువరుస) + gui.char_ఎత్తు - h,
	    w, h);
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    శూన్య
gui_mch_update(శూన్య)
{
    Xt_ఆదానం_ముసుగు ముసుగు, desired;

#నిర్వచితమైతే ALT_X_ఆదానం
    ఒకవేళ (suppress_alternate_ఆదానం)
	desired = (XtIMXEvent | XtIMTimer);
    లేకపోతే
#అయితే_ముగింపు
	desired = (XtIMAll);
    అయితే ((ముసుగు = XtAppPending(app_context)) && (ముసుగు & desired)
	    && !vim_is_ఆదానం_buf_నిండిన())
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
    పూర్ణాంకం
gui_mch_wait_for_chars(దీర్ఘ wtime)
{
    పూర్ణాంకం	    focus;
    పూర్ణాంకం	    retval = FAIL;

    /*
     * Make this static, in case gui_x11_timer_cb is called after leaving
     * this function (otherwise a random value on the stack may be changed).
     */
    స్థిర పూర్ణాంకం	    timed_బహిర్గతం;
    XtIntervalId    timer = (XtIntervalId)0;
    Xt_ఆదానం_ముసుగు	    desired;
#నిర్వచితమైతే FEAT_పని_CHANNEL
    XtIntervalId    channel_timer = (XtIntervalId)0;
#అయితే_ముగింపు

    timed_బహిర్గతం = FALSE;

    ఒకవేళ (wtime >= 0)
	timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context,
				(long_u)(wtime == 0 ? 1L : wtime),
						 gui_x11_timer_cb, &timed_బహిర్గతం);
#నిర్వచితమైతే FEAT_పని_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ఒకవేళ (channel_any_keep_open())
	channel_timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, (long_u)20,
				   channel_poll_cb, (Xt_సూచిక)&channel_timer);
#అయితే_ముగింపు

    focus = gui.లోపల_focus;
    desired = (XtIMAll);
    అయితే (!timed_బహిర్గతం)
    {
	// Stop or start blinking when focus changes
	ఒకవేళ (gui.లోపల_focus != focus)
	{
	    ఒకవేళ (gui.లోపల_focus)
		gui_mch_ప్రారంభం_blink();
	    లేకపోతే
		gui_mch_stop_blink(TRUE);
	    focus = gui.లోపల_focus;
	}

#నిర్వచితమైతే సందేశం_వరుస
# ifdef FEAT_TIMERS
	did_కూడు_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ఒకవేళ (did_కూడు_timer)
	    // Need to recompute the waiting time.
	    ఆపు;
# endif
#అయితే_ముగింపు

	/*
	 * Don't use gui_mch_update() because then we will spin-lock until a
	 * char arrives, instead we use XtAppProcessEvent() to hang until an
	 * event arrives.  No need to check for input_buf_full because we are
	 * returning as soon as it contains a single char.  Note that
	 * XtAppNextEvent() may not be used because it will not return after a
	 * timer event has arrived -- webb
	 */
	XtAppProcessEvent(app_context, desired);

	ఒకవేళ (ఆదానం_అందుబాటు())
	{
	    retval = OK;
	    ఆపు;
	}
    }

    ఒకవేళ (timer != (XtIntervalId)0 && !timed_బహిర్గతం)
	Xt_Remove_సమయం_బహిర్గతం(timer);
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (channel_timer != (XtIntervalId)0)
	Xt_Remove_సమయం_బహిర్గతం(channel_timer);
#అయితే_ముగింపు

    తిరిగి retval;
}

/*
 * Output routines.
 */

/*
 * Flush any output to the screen
 */
    శూన్య
gui_mch_flush(శూన్య)
{
    XFlush(gui.dpy);
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    శూన్య
gui_mch_clear_ఖండం(
    పూర్ణాంకం		row1,
    పూర్ణాంకం		col1,
    పూర్ణాంకం		row2,
    పూర్ణాంకం		col2)
{
    పూర్ణాంకం		x;

    x = FILL_X(col1);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the next column.
    XFillRectangle(gui.dpy, gui.wid, gui.వెనుక_gc, x, FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_వెడల్పు + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ఎత్తు);
}

    శూన్య
gui_mch_clear_all(శూన్య)
{
    XClearArea(gui.dpy, gui.wid, 0, 0, 0, 0, False);
}

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    శూన్య
gui_mch_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    ఒకవేళ (gui.visibility == VisibilityFullyObscured)
	తిరిగి;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.పాఠ్యం_gc,
	FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస + అంకె_పంక్తులు),
	gui.char_వెడల్పు * (gui.scroll_region_కుడి - gui.scroll_region_ఎడమ + 1)
			       + (gui.scroll_region_కుడి == Columns - 1),
	gui.char_ఎత్తు * (gui.scroll_region_bot - అడ్డువరుస - అంకె_పంక్తులు + 1),
	FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస));

    gui_clear_ఖండం(gui.scroll_region_bot - అంకె_పంక్తులు + 1,
						       gui.scroll_region_ఎడమ,
			  gui.scroll_region_bot, gui.scroll_region_కుడి);
    gui_x11_check_copy_area();
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    శూన్య
gui_mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    ఒకవేళ (gui.visibility == VisibilityFullyObscured)
	తిరిగి;	    // Can't see the window

    // copy one extra pixel at the far right, for when bold has spilled
    // over
    XCopyArea(gui.dpy, gui.wid, gui.wid, gui.పాఠ్యం_gc,
	FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	gui.char_వెడల్పు * (gui.scroll_region_కుడి - gui.scroll_region_ఎడమ + 1)
			       + (gui.scroll_region_కుడి == Columns - 1),
	gui.char_ఎత్తు * (gui.scroll_region_bot - అడ్డువరుస - అంకె_పంక్తులు + 1),
	FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస + అంకె_పంక్తులు));

    gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
				అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);
    gui_x11_check_copy_area();
}

/*
 * Update the region revealed by scrolling up/down.
 */
    స్థిర శూన్య
gui_x11_check_copy_area(శూన్య)
{
    XEvent		    event;
    XGraphicsExposeEvent    *gevent;

    ఒకవేళ (gui.visibility != VisibilityPartiallyObscured)
	తిరిగి;

    XFlush(gui.dpy);

    // Wait to check whether the scroll worked or not
    కొరకు (;;)
    {
	ఒకవేళ (XCheckTypedEvent(gui.dpy, NoExpose, &event))
	    తిరిగి;	// The scroll worked.

	ఒకవేళ (XCheckTypedEvent(gui.dpy, GraphicsExpose, &event))
	{
	    gevent = (XGraphicsExposeEvent *)&event;
	    gui_redraw(gevent->x, gevent->y, gevent->వెడల్పు, gevent->ఎత్తు);
	    ఒకవేళ (gevent->లెక్క == 0)
		తిరిగి;		// This was the last expose event
	}
	XSync(gui.dpy, False);
    }
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */

    శూన్య
clip_mch_lose_selection(Clipboard_T *cbd)
{
    clip_x11_lose_selection(vimShell, cbd);
}

    పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd)
{
    తిరిగి clip_x11_own_selection(vimShell, cbd);
}

    శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
 clip_x11_request_selection(vimShell, gui.dpy, cbd);
}

    శూన్య
clip_mch_సమితి_selection(
    Clipboard_T	*cbd)
{
    clip_x11_సమితి_selection(cbd);
}

#ఒకవేళ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

/*
 * Make a menu either grey or not grey.
 */
    శూన్య
gui_mch_menu_grey(vimmenu_T *menu, పూర్ణాంకం grey)
{
    ఒకవేళ (menu->id == (Widget)0)
	తిరిగి;

    gui_mch_menu_hidden(menu, False);
    ఒకవేళ (grey
# ifdef FEAT_GUI_MOTIF
	    || !menu->sensitive
# endif
       )
	Xt_సమితి_Sensitive(menu->id, False);
    లేకపోతే
	Xt_సమితి_Sensitive(menu->id, True);
}

/*
 * Make menu item hidden or not hidden
 */
    శూన్య
gui_mch_menu_hidden(vimmenu_T *menu, పూర్ణాంకం hidden)
{
    ఒకవేళ (menu->id == (Widget)0)
	తిరిగి;

    ఒకవేళ (hidden)
	Xt_Unmanage_బిడ్డ(menu->id);
    లేకపోతే
	Xt_Manage_బిడ్డ(menu->id);
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    శూన్య
gui_mch_draw_menubar(శూన్య)
{
    // Nothing to do in X
}

    శూన్య
gui_x11_menu_cb(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    gui_menu_cb((vimmenu_T *)client_దత్తాంశం_2);
}

#అయితే_ముగింపు // FEAT_MENU



/*
 * Function called when window closed.	Works like ":qa".
 * Should put up a requester!
 */
    స్థిర శూన్య
gui_x11_wm_protocol_handler(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    /*
     * Only deal with Client messages.
     */
    ఒకవేళ (event->type != Client_సందేశం)
	తిరిగి;

    /*
     * The WM_SAVE_YOURSELF event arrives when the window manager wants to
     * exit.  That can be cancelled though, thus Vim shouldn't exit here.
     * Just sync our swap files.
     */
    ఒకవేళ ((Atom)((XClient_సందేశం_Event *)event)->దత్తాంశం.l[0] ==
						  wm_atoms[SAVE_YOURSELF_IDX])
    {
	బహిర్గతం_flush();
	ml_sync_all(FALSE, FALSE);	// preserve all swap files

	// Set the window's WM_COMMAND property, to let the window manager
	// know we are done saving ourselves.  We don't want to be restarted,
	// thus set argv to NULL.
	XSetCommand(gui.dpy, Xt_కిటికీ(vimShell), ఖాళీ, 0);
	తిరిగి;
    }

    ఒకవేళ ((Atom)((XClient_సందేశం_Event *)event)->దత్తాంశం.l[0] !=
						  wm_atoms[DELETE_కిటికీ_IDX])
	తిరిగి;

    gui_shell_closed();
}

#నిర్వచితమైతే FEAT_CLIENTSERVER
/*
 * Function called when property changed. Check for incoming commands
 */
    స్థిర శూన్య
gui_x11_send_event_handler(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,
    XEvent	*event,
    Boolean	*dum వాడని)
{
    XPropertyEvent *e = (XPropertyEvent *) event;

    ఒకవేళ (e->type == PropertyNotify && e->కిటికీ_2 == comm_కిటికీ
	    && e->atom == commProperty &&  e->state == Property_New_విలువ)
	serverEventProc(gui.dpy, event, 0);
}
#అయితే_ముగింపు

/*
 * Cursor blink functions.
 *
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#నిర్వచించు BLINK_NONE  0
#నిర్వచించు BLINK_OFF   1
#నిర్వచించు BLINK_ON    2

స్థిర పూర్ణాంకం		blink_state = BLINK_NONE;
స్థిర long_u		blink_waittime = 700;
స్థిర long_u		blink_ontime = 400;
స్థిర long_u		blink_offtime = 250;
స్థిర XtIntervalId	blink_timer = (XtIntervalId)0;

    పూర్ణాంకం
gui_mch_is_blinking(శూన్య)
{
    తిరిగి blink_state != BLINK_NONE;
}

    పూర్ణాంకం
gui_mch_is_blink_off(శూన్య)
{
    తిరిగి blink_state == BLINK_OFF;
}

    శూన్య
gui_mch_సమితి_blinking(దీర్ఘ waittime, దీర్ఘ on, దీర్ఘ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    ఒకవేళ (blink_timer != (XtIntervalId)0)
    {
	Xt_Remove_సమయం_బహిర్గతం(blink_timer);
	blink_timer = (XtIntervalId)0;
    }
    ఒకవేళ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    స్థిర శూన్య
gui_x11_blink_cb(
    Xt_సూచిక	    timed_బహిర్గతం వాడని,
    XtIntervalId    *interval_id వాడని)
{
    ఒకవేళ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, blink_offtime,
						      gui_x11_blink_cb, ఖాళీ);
    }
    లేకపోతే
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, blink_ontime,
						      gui_x11_blink_cb, ఖాళీ);
    }
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    శూన్య
gui_mch_ప్రారంభం_blink(శూన్య)
{
    ఒకవేళ (blink_timer != (XtIntervalId)0)
	Xt_Remove_సమయం_బహిర్గతం(blink_timer);
    // Only switch blinking on if none of the times is zero
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
    {
	blink_timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, blink_waittime,
						      gui_x11_blink_cb, ఖాళీ);
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

    తిరిగి (guicolor_T)(((xc.red & 0xff00) << 8) + (xc.green & 0xff00)
						   + ((గుర్తులేని)xc.blue >> 8));
}

/*
 * Add the callback functions.
 */
    శూన్య
gui_x11_callbacks(Widget పాఠ్యం_Area, Widget vimForm)
{
    Xt_కూడు_Event_Handler(పాఠ్యం_Area, Visibility_Change_ముసుగు, FALSE,
	gui_x11_visibility_cb, (Xt_సూచిక)0);

    Xt_కూడు_Event_Handler(పాఠ్యం_Area, Exposure_ముసుగు, FALSE, gui_x11_expose_cb,
	(Xt_సూచిక)0);

    Xt_కూడు_Event_Handler(vimShell, Structure_Notify_ముసుగు, FALSE,
	gui_x11_resize_కిటికీ_cb, (Xt_సూచిక)0);

    Xt_కూడు_Event_Handler(vimShell, Focus_Change_ముసుగు, FALSE, gui_x11_focus_change_cb,
	(Xt_సూచిక)0);
    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ఒకవేళ (vim_strchr(p_go, GO_సూచిక) != ఖాళీ)
    {
	Xt_కూడు_Event_Handler(vimShell, Leave_కిటికీ_ముసుగు, FALSE, gui_x11_leave_cb,
	    (Xt_సూచిక)0);
	Xt_కూడు_Event_Handler(పాఠ్యం_Area, Leave_కిటికీ_ముసుగు, FALSE, gui_x11_leave_cb,
	    (Xt_సూచిక)0);
	Xt_కూడు_Event_Handler(పాఠ్యం_Area, Enter_కిటికీ_ముసుగు, FALSE, gui_x11_enter_cb,
	    (Xt_సూచిక)0);
	Xt_కూడు_Event_Handler(vimShell, Enter_కిటికీ_ముసుగు, FALSE, gui_x11_enter_cb,
	    (Xt_సూచిక)0);
    }

    Xt_కూడు_Event_Handler(vimForm, కీలకం_Press_ముసుగు, FALSE, gui_x11_కీలకం_తగిలింది_cb,
	(Xt_సూచిక)0);
    Xt_కూడు_Event_Handler(పాఠ్యం_Area, కీలకం_Press_ముసుగు, FALSE, gui_x11_కీలకం_తగిలింది_cb,
	(Xt_సూచిక)0);

    // get pointer moved events from scrollbar, needed for 'mousefocus'
    Xt_కూడు_Event_Handler(vimForm, సూచిక_Motion_ముసుగు,
	FALSE, gui_x11_mouse_cb, (Xt_సూచిక)1);
    Xt_కూడు_Event_Handler(పాఠ్యం_Area, Button_Press_ముసుగు | Button_Release_ముసుగు |
					 Button_Motion_ముసుగు | సూచిక_Motion_ముసుగు,
	FALSE, gui_x11_mouse_cb, (Xt_సూచిక)0);
}

/*
 * Get current mouse coordinates in text window.
 */
    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    పూర్ణాంకం		rootx, rooty, winx, winy;
    కిటికీ	మూలం, బిడ్డ;
    గుర్తులేని పూర్ణాంకం ముసుగు;

    ఒకవేళ (gui.wid && XQuery_సూచిక(gui.dpy, gui.wid, &మూలం, &బిడ్డ,
					 &rootx, &rooty, &winx, &winy, &ముసుగు))
    {
	*x = winx;
	*y = winy;
    }
    లేకపోతే
    {
	*x = -1;
	*y = -1;
    }
}

    శూన్య
gui_mch_setmouse(పూర్ణాంకం x, పూర్ణాంకం y)
{
    ఒకవేళ (gui.wid)
	XWarp_సూచిక(gui.dpy, (కిటికీ)0, gui.wid, 0, 0, 0, 0, x, y);
}

#ఒకవేళ defined(FEAT_GUI_MOTIF) && defined(FEAT_MENU)
    XButtonPressedEvent *
gui_x11_get_చివరి_mouse_event(శూన్య)
{
    తిరిగి &చివరి_mouse_event;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_SIGN_ICONS)

// Signs are currently always 2 chars wide.  Hopefully the font is big enough
// to provide room for the bitmap!
# define SIGN_వెడల్పు (gui.char_వెడల్పు * 2)

    శూన్య
gui_mch_drawsign(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం typenr)
{
    XImage	*sign;

    ఒకవేళ (!gui.లోపల_use || (sign = (XImage *)sign_get_image(typenr)) == ఖాళీ)
	తిరిగి;

    XClearArea(gui.dpy, gui.wid, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస) - sign->ఎత్తు,
	    SIGN_వెడల్పు, gui.char_ఎత్తు, FALSE);
    XPutImage(gui.dpy, gui.wid, gui.పాఠ్యం_gc, sign, 0, 0,
	    పాఠ్యం_X(నిలువు) + (SIGN_వెడల్పు - sign->వెడల్పు) / 2,
	    పాఠ్యం_Y(అడ్డువరుస) - sign->ఎత్తు,
	    sign->వెడల్పు, sign->ఎత్తు);
}

    శూన్య *
gui_mch_రిజిస్టరు_sign(char_u *signfile)
{
    Xpm_లక్షణాలు   attrs;
    XImage	    *sign = ఖాళీ;
    పూర్ణాంకం		    స్థితి_2;

    /*
     * Setup the color substitution table.
     */
    ఒకవేళ (signfile[0] != NUL && signfile[0] != '-')
    {
	Xpm_Color_చిహ్నం color[5] =
	{
	    {"none", ఖాళీ, 0},
	    {"iconColor1", ఖాళీ, 0},
	    {"bottomShadowColor", ఖాళీ, 0},
	    {"topShadowColor", ఖాళీ, 0},
	    {"selectColor", ఖాళీ, 0}
	};
	attrs.valuemask = XpmColorSymbols;
	attrs.numsymbols = 2;
	attrs.colorsymbols = color;
	attrs.colorsymbols[0].pixel = gui.వెనుక_pixel;
	attrs.colorsymbols[1].pixel = gui.norm_pixel;
	స్థితి_2 = Xpm_Read_File_గమ్యం_Image(gui.dpy, (అక్షరం *)signfile,
							 &sign, ఖాళీ, &attrs);
	ఒకవేళ (స్థితి_2 == 0)
	{
	    // Sign width is fixed at two columns now.
	    // if (sign->width > gui.sign_width)
	    //     gui.sign_width = sign->width + 8;
	}
	లేకపోతే
	    emsg(_(e_couldnt_read_లోపల_sign_దత్తాంశం));
    }

    తిరిగి (శూన్య *)sign;
}

    శూన్య
gui_mch_destroy_sign(శూన్య *sign)
{
    XDestroyImage((XImage*)sign);
}
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
స్థిర పూర్ణాంకం చివరి_shape = 0;
#అయితే_ముగింపు

/*
 * Use the blank mouse pointer or not.
 */
    శూన్య
gui_mch_mousehide(
    పూర్ణాంకం		hide)	// TRUE = use blank ptr, FALSE = use parent ptr
{
    ఒకవేళ (gui.సూచిక_hidden == hide)
	తిరిగి;

    gui.సూచిక_hidden = hide;
    ఒకవేళ (hide)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_సూచిక);
    లేకపోతే
#నిర్వచితమైతే FEAT_MOUSESHAPE
	mch_సమితి_mouse_shape(చివరి_shape);
#లేకపోతే
    XUndefineCursor(gui.dpy, gui.wid);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)

// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
స్థిర పూర్ణాంకం mshape_ids[] =
{
    XC_ఎడమ_ptr,		// arrow
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
    XC_కుడి_ptr,		// right-arrow
    XC_center_ptr,		// up-arrow
    XC_ఎడమ_ptr			// last one
};

    శూన్య
mch_సమితి_mouse_shape(పూర్ణాంకం shape)
{
    పూర్ణాంకం	    id;

    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    ఒకవేళ (shape == MSHAPE_HIDE || gui.సూచిక_hidden)
	XDefineCursor(gui.dpy, gui.wid, gui.blank_సూచిక);
    లేకపోతే
    {
	ఒకవేళ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ఒకవేళ (id >= XC_అంకె_glyphs)
		id = XC_ఎడమ_ptr;
	    లేకపోతే
		id &= ~1;	// they are always even (why?)
	}
	లేకపోతే
	    id = mshape_ids[shape];

	XDefineCursor(gui.dpy, gui.wid, XCreateFontCursor(gui.dpy, id));
    }
    ఒకవేళ (shape != MSHAPE_HIDE)
	చివరి_shape = shape;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
/*
 * Set the balloon-eval used for the tooltip of a toolbar menu item.
 * The check for a non-toolbar item was added, because there is a crash when
 * passing a normal menu item here.  Can't explain that, but better avoid it.
 */
    శూన్య
gui_mch_menu_సమితి_tip(vimmenu_T *menu)
{
    ఒకవేళ (menu->id == ఖాళీ || menu->మాతృ == ఖాళీ
				|| !menu_is_toolbar(menu->మాతృ->name))
	తిరిగి;

    // Always destroy and create the balloon, in case the string was
    // changed.
    ఒకవేళ (menu->tip != ఖాళీ)
    {
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = ఖాళీ;
    }
    ఒకవేళ (menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP] != ఖాళీ)
	menu->tip = gui_mch_create_beval_area(
		menu->id,
		menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP],
		ఖాళీ,
		ఖాళీ);
}
#అయితే_ముగింపు
