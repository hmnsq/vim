/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 *
 * (C) 2002,2005 by Marcin Dalecki <martin@dalecki.de>
 *
 * MARCIN DALECKI ASSUMES NO RESPONSIBILITY FOR THE USE OR INABILITY TO USE ANY
 * OF THIS SOFTWARE . THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, AND MARCIN DALECKI EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * Enhanced Motif PushButton widget with move over behavior.
 */

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_TOOLBAR

#അന്തര്ഭാവ <Xm/XmP.h>
#അന്തര്ഭാവ <Xm/DrawP.h>
#എങ്കിൽ defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#അല്ലെങ്കിൽ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  അല്ലെങ്കിൽ
#   include <X11/xpm.h>
#  endif
# endif
#endif
#അന്തര്ഭാവ <Xm/ManagerP.h>
#അന്തര്ഭാവ <Xm/Display.h>
#അന്തര്ഭാവ <Xm/DisplayP.h>

#അന്തര്ഭാവ <X11/Shell.h>
#അന്തര്ഭാവ <X11/ShellP.h>

#അന്തര്ഭാവ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#എങ്കിൽ defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (പൂർണ്ണസംഖ്യ)XmMENU_POPUP || \
		    Lab_MenuType(w) == (പൂർണ്ണസംഖ്യ)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, വരി_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#എങ്കിൽ defined(VMS)
# എങ്കിൽ defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# അല്ലെങ്കിൽ
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#അല്ലെങ്കിൽ
# define XTPROCESS_LOCK XtProcessLock()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#endif

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#ifndef XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#endif

ബാഹ്യ Boolean _XmGetInDragMode(Widget widget);
ബാഹ്യ ശൂന്യം _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * അക്കം_params);
ബാഹ്യ ശൂന്യം _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * അക്കം_params);
ബാഹ്യ ശൂന്യം _XmSetFocusFlag(Widget w, unsigned പൂർണ്ണസംഖ്യ mask, Boolean മൂല്യം);
ബാഹ്യ ശൂന്യം _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
സ്ഥിതം ശൂന്യം Destroy(Widget w);
സ്ഥിതം ശൂന്യം Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
സ്ഥിതം Boolean Set_മൂല്യങ്ങൾ(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
സ്ഥിതം ശൂന്യം Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
സ്ഥിതം ശൂന്യം Enter(Widget, XEvent *, String *, Cardinal *);
സ്ഥിതം ശൂന്യം Leave(Widget, XEvent *, String *, Cardinal *);
സ്ഥിതം ശൂന്യം BorderHighlight(Widget);
സ്ഥിതം ശൂന്യം BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
സ്ഥിതം അക്ഷരം stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
സ്ഥിതം XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

സ്ഥിതം XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, വലുപ്പം(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) ശൂന്യ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, വലുപ്പം(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) ശൂന്യ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, വലുപ്പം(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, വലുപ്പം(പൂർണ്ണസംഖ്യ),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_വലത്
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ശൂന്യ,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ വലുപ്പം(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ശൂന്യ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_തള്ളുക_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ വലുപ്പം(XmEnhancedButtonRec),
	/* class_initialize	 */ ശൂന്യ,
	/* class_part_initialize */ ശൂന്യ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ശൂന്യ,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_സംഖ്യ(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_സംഖ്യ(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_മൂല്യങ്ങൾ,
	/* set_values_hook	 */ ശൂന്യ,
	/* set_values_almost	 */ Xt_Inherit_Set_മൂല്യങ്ങൾ_Almost,
	/* get_values_hook	 */ ശൂന്യ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ ശൂന്യ,
	/* tm_table		 */ ശൂന്യ,
	/* query_geometry	 */ ശൂന്യ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ശൂന്യ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ശൂന്യ,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ശൂന്യ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) ശൂന്യ,
    },

    // enhancedbutton_class fields
    {
	/* stipple_bitmap	 */ None
    }
};


WidgetClass xmEnhancedButtonWidgetClass =
				       (WidgetClass)&xmEnhancedButtonClassRec;


/*
 * Create a slightly fainter pixmap to be shown on button entry.
 */
    സ്ഥിതം unsigned short
bump_color(unsigned short മൂല്യം)
{
    പൂർണ്ണസംഖ്യ tmp = 2 * (((പൂർണ്ണസംഖ്യ) മൂല്യം - 65535) / 3) + 65535;

    തിരികെ tmp;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
alloc_color(Display	*display,
	Colormap	colormap,
	അക്ഷരം		*colorname,
	XColor		*xcolor,
	ശൂന്യം		*closure UNUSED)
{
    പൂർണ്ണസംഖ്യ status;

    എങ്കിൽ (colorname)
	എങ്കിൽ (!XParseColor(display, colormap, colorname, xcolor))
	    തിരികെ -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    തിരികെ status != 0 ? 1 : 0;
}

// XPM
സ്ഥിതം അക്ഷരം * blank_xpm[] =
{
// width height ncolors cpp [x_hot y_hot]
"12 12 4 1 0 0",
// colors
"#	s iconColor1	m black	c #000000",
".	s none	m none	c none",
"X	s topShadowColor	m none	c #DCDEE5",
"o	s bottomShadowColor	m black	c #5D6069",
// pixels
"##########..",
"#XXXXXXXX#..",
"#X.......#o.",
"#X.......#o.",
"#X.......#o.",
"#X.......#o.",
"#X.......#o.",
"#X.......#o.",
"#X.......#o.",
"##########o.",
"..ooooooooo.",
"............"};

/*
 * Set the pixmap.
 */
    സ്ഥിതം ശൂന്യം
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    വേര്;
    സ്ഥിതം Xpm_Color_ചിഹ്നം color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ശൂന്യ, 0},
	{"foreground", ശൂന്യ, 0},
	{"bottomShadowColor", ശൂന്യ, 0},
	{"topShadowColor", ശൂന്യ, 0},
	{"highlightColor", ശൂന്യ, 0},
	{"armColor", ശൂന്യ, 0}
    };
    പൂർണ്ണസംഖ്യ		    scr;
    Display	    *dpy = XtDisplay(eb);
    പൂർണ്ണസംഖ്യ		    x;
    പൂർണ്ണസംഖ്യ		    y;
    unsigned പൂർണ്ണസംഖ്യ    height, width, border, depth;
    പൂർണ്ണസംഖ്യ		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    മുകളിലെ_pix = None;
    അക്ഷരം	    **data = (അക്ഷരം **) eb->enhancedbutton.pixmap_data;
    അക്ഷരം	    *fname = (അക്ഷരം *) eb->enhancedbutton.pixmap_file;
    പൂർണ്ണസംഖ്യ		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    എങ്കിൽ (!data)
	തിരികെ;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ശൂന്യ);

    scr = DefaultScreen(dpy);
    വേര് = വേര്_Window(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_depth = 0;
    eb->enhancedbutton.pixmap_width = 0;
    eb->enhancedbutton.pixmap_height = 0;
    eb->enhancedbutton.normal_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.മുകൾ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.മുകൾ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_സംഖ്യ(color);

    എങ്കിൽ (fname)
	status = XpmReadFileToPixmap(dpy, വേര്, fname, &pix, &mask, &attr);
    എങ്കിൽ (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, വേര്, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    എങ്കിൽ (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, വേര്, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &വേര്, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    എങ്കിൽ (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, വേര്, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, shift, shift);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, width, height, shift, shift);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#ifdef XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#അല്ലെങ്കിൽ
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_സംഖ്യ(color);

    status = XpmCreatePixmapFromData(dpy, വേര്, data, &pix, ശൂന്യ, &attr);
    XpmFreeAttributes(&attr);

    മുകളിലെ_pix = XCreatePixmap(dpy, വേര്, width + shift, height + shift, depth);

#എങ്കിൽ 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#അല്ലെങ്കിൽ
    XSetForeground(dpy, gc, eb->primitive.മുകൾ_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, മുകളിലെ_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, മുകളിലെ_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    എങ്കിൽ (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    അല്ലെങ്കിൽ
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, shift, shift);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 2 * shift, 2 * shift);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, width, height, 2 * shift, 2 * shift);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, mask);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_താക്കോൽ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_താക്കോൽ = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, വേര്, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, വേര്, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.മുകൾ_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * shift, 2 * shift);
    XFillRectangle(dpy, ins_pix, gc, 2 * shift, 2 * shift, width, height);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, shift, shift);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XtReleaseGC((Widget) eb, gc);

    XpmFreeAttributes(&attr);

    eb->enhancedbutton.pixmap_depth = depth;
    eb->enhancedbutton.pixmap_width = width;
    eb->enhancedbutton.pixmap_height = height;
    eb->enhancedbutton.normal_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = മുകളിലെ_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, mask);
}

#define	BUTTON_MASK ( \
	Button1Mask | Button2Mask | Button3Mask | Button4Mask | Button5Mask \
)

    സ്ഥിതം ശൂന്യം
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		മുകൾ_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    എങ്കിൽ (!eb->primitive.shadow_thickness)
	തിരികെ;

    എങ്കിൽ ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	തിരികെ;

#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#അല്ലെങ്കിൽ
    etched_in = False;
#endif
    എങ്കിൽ (!etched_in ^ eb->pushbutton.armed)
    {
	മുകൾ_gc = eb->primitive.മുകൾ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    അല്ലെങ്കിൽ
    {
	മുകൾ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.മുകൾ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    മുകൾ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    സ്ഥിതം ശൂന്യം
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    എങ്കിൽ (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	തിരികെ;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    സ്ഥിതം ശൂന്യം
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    എങ്കിൽ (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	തിരികെ;

    എങ്കിൽ (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	എങ്കിൽ (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != ശൂന്യ) && (UnhighlightT->getUnhighlightGC != ശൂന്യ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	അല്ലെങ്കിൽ
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#അല്ലെങ്കിൽ
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	എങ്കിൽ (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    അല്ലെങ്കിൽ
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    സ്ഥിതം ശൂന്യം
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    പൂർണ്ണസംഖ്യ		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    പൂർണ്ണസംഖ്യ		x;
    പൂർണ്ണസംഖ്യ		y;

    എങ്കിൽ (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	അല്ലെങ്കിൽ എങ്കിൽ (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	അല്ലെങ്കിൽ
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    എങ്കിൽ (pix == None || !eb->enhancedbutton.pixmap_data)
	തിരികെ;

    depth = eb->enhancedbutton.pixmap_depth;
    w = eb->enhancedbutton.pixmap_width;
    h = eb->enhancedbutton.pixmap_height;

    gc = eb->label.normal_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_width;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_height;
    width = eb->core.width - 2 * x;
    എങ്കിൽ (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    എങ്കിൽ (h < height)
	height = h;
    എങ്കിൽ (depth == (പൂർണ്ണസംഖ്യ)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    അല്ലെങ്കിൽ എങ്കിൽ (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    സ്ഥിതം ശൂന്യം
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ശൂന്യ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#അല്ലെങ്കിൽ
    Boolean	etched_in = False;
#endif

    എങ്കിൽ (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	എങ്കിൽ (eb->label.label_type == (പൂർണ്ണസംഖ്യ)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.normal_GC;
	    eb->label.normal_GC = eb->pushbutton.background_gc;
	    replaceGC = True;
	}
    }

    /*
     * If the button contains a labeled pixmap, we will take it instead of our
     * own pixmap.
     */

    എങ്കിൽ (eb->label.label_type == (പൂർണ്ണസംഖ്യ)XmPIXMAP)
    {
	എങ്കിൽ (eb->pushbutton.armed)
	{
	    എങ്കിൽ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    അല്ലെങ്കിൽ
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	അല്ലെങ്കിൽ
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    എങ്കിൽ (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ഇടത്(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_വലത്(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_മുകൾ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    എങ്കിൽ (deadjusted)
    {
	Lab_Margin_ഇടത്(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_വലത്(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_മുകൾ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    എങ്കിൽ (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    സ്ഥിതം ശൂന്യം
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *അക്കം_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_തള്ളുക_Button_Callback_Struct call_മൂല്യം;

    എങ്കിൽ (Lab_IsMenupane(eb))
    {
	എങ്കിൽ ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#അല്ലെങ്കിൽ
	    Boolean etched_in = False;
#endif

	    എങ്കിൽ (eb->pushbutton.armed)
		തിരികെ;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    എങ്കിൽ (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ശൂന്യ);
		draw_pixmap(eb, event, ശൂന്യ);
	    }

	    എങ്കിൽ ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.മുകൾ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    എങ്കിൽ (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_മൂല്യം.reason = (പൂർണ്ണസംഖ്യ)XmCR_ARM;
		call_മൂല്യം.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_മൂല്യം);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ശൂന്യ, ശൂന്യ);
	എങ്കിൽ (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ശൂന്യ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ശൂന്യ);
    }
}

    സ്ഥിതം ശൂന്യം
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *അക്കം_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_തള്ളുക_Button_Callback_Struct call_മൂല്യം;

    എങ്കിൽ (Lab_IsMenupane(eb))
    {
#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#അല്ലെങ്കിൽ
	Boolean etched_in = False;
#endif

	എങ്കിൽ (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = ശൂന്യ;

	    എങ്കിൽ (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ശൂന്യ);
		draw_pixmap(eb, event, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    എങ്കിൽ (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_മൂല്യം.reason = (പൂർണ്ണസംഖ്യ)XmCR_DISARM;
		call_മൂല്യം.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_മൂല്യം);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	_XmPrimitiveLeave((Widget) eb, event, ശൂന്യ, ശൂന്യ);

	എങ്കിൽ (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ശൂന്യ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ശൂന്യ);
	    eb->pushbutton.armed = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ശൂന്യ);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    സ്ഥിതം ശൂന്യം
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned പൂർണ്ണസംഖ്യ w = 0;
    unsigned പൂർണ്ണസംഖ്യ h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    എങ്കിൽ (newtb->enhancedbutton.pixmap_data
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.normal_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_width;
	h = newtb->enhancedbutton.pixmap_height;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    എങ്കിൽ (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ഇടത് = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    അല്ലെങ്കിൽ
    {
	newtb->label.margin_ഇടത് = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	എങ്കിൽ (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_ഇടത് += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    എങ്കിൽ (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    അല്ലെങ്കിൽ
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#എങ്കിൽ 0
    printf("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.normal_pixmap,
	    h, newtb->core.height,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_height,
	    newtb->core.width,
	    newtb->core.height);
#endif

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_LOCK;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    സ്ഥിതം ശൂന്യം
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    എങ്കിൽ (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	വേര്_W = Default_വേര്_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, വേര്_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    എങ്കിൽ (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_മൂല്യങ്ങൾ((Widget) eb, XmNlabelString, str, ശൂന്യ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    എങ്കിൽ (request->core.width == 0)
	eb->core.width = 0;
    എങ്കിൽ (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    സ്ഥിതം ശൂന്യം
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap മുകളിലെ_pix = eb->enhancedbutton.highlight_pixmap;

    എങ്കിൽ (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    എങ്കിൽ (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    എങ്കിൽ (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    എങ്കിൽ (മുകളിലെ_pix != None && മുകളിലെ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), മുകളിലെ_pix);
}

    സ്ഥിതം ശൂന്യം
Destroy(Widget w)
{
    എങ്കിൽ (!XmIsEnhancedButton(w))
	തിരികെ;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    സ്ഥിതം Boolean
Set_മൂല്യങ്ങൾ(Widget current,
	  Widget request UNUSED,
	  Widget new,
	  ArgList args UNUSED,
	  Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#define NOT_EQUAL(field)       (cur->field != eb->field)

    /*
     * Make sure that lost sensitivity is causing the border to vanish as well.
     */
    എങ്കിൽ (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	എങ്കിൽ (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ		    r_x;
	    പൂർണ്ണസംഖ്യ		    r_y;
	    unsigned പൂർണ്ണസംഖ്യ    r_height;
	    unsigned പൂർണ്ണസംഖ്യ    r_width;
	    unsigned പൂർണ്ണസംഖ്യ    r_border;
	    unsigned പൂർണ്ണസംഖ്യ    r_depth;
	    പൂർണ്ണസംഖ്യ		    വേര്_x;
	    പൂർണ്ണസംഖ്യ		    വേര്_y;
	    പൂർണ്ണസംഖ്യ		    win_x;
	    പൂർണ്ണസംഖ്യ		    win_y;
	    Window	    വേര്;
	    Window	    വേര്_q;
	    Window	    child;
	    unsigned പൂർണ്ണസംഖ്യ    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &വേര്, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &വേര്_q, &child,
			  &വേര്_x, &വേര്_y, &win_x, &win_y, &mask);

	    എങ്കിൽ (വേര് == വേര്_q)
	    {
		എങ്കിൽ ((win_x < 0) || (win_y < 0))
		    തിരികെ False;

		എങ്കിൽ ((win_x > (പൂർണ്ണസംഖ്യ)r_width) || (win_y > (പൂർണ്ണസംഖ്യ)r_height))
		    തിരികെ False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	തിരികെ True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    എങ്കിൽ (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    എങ്കിൽ (NOT_EQUAL(primitive.foreground))
	redraw = True;
    എങ്കിൽ (NOT_EQUAL(core.background_pixel))
	redraw = True;
    എങ്കിൽ (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    എങ്കിൽ (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    എങ്കിൽ (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    എങ്കിൽ (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    എങ്കിൽ (redraw == True)
    {
	എങ്കിൽ (change)
	    set_pixmap(eb);
	എങ്കിൽ (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	അല്ലെങ്കിൽ
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	എങ്കിൽ (change)
	    set_size(eb);
	redraw = False;
    }

    തിരികെ redraw;
}

    സ്ഥിതം ശൂന്യം
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    പൂർണ്ണസംഖ്യ			    dx;
    പൂർണ്ണസംഖ്യ			    adjust;
    short		    fill = 0;

    എങ്കിൽ (!XtIsRealized((Widget)eb))
	തിരികെ;

#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    എങ്കിൽ ((eb->pushbutton.arm_color == eb->primitive.മുകൾ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    എങ്കിൽ (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    അല്ലെങ്കിൽ
	adjust = eb->pushbutton.default_button_shadow_thickness;

    എങ്കിൽ (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    അല്ലെങ്കിൽ
	dx = (eb->primitive.highlight_thickness
		+ eb->primitive.shadow_thickness + fill);

    box.x = dx;
    box.y = dx;
    adjust = (dx << 1);
    box.width  = eb->core.width - adjust;
    box.height = eb->core.height - adjust;

    /*
     * Redraw the background.
     */
    എങ്കിൽ (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	എങ്കിൽ (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		എങ്കിൽ (eb->label.label_type == (പൂർണ്ണസംഖ്യ)XmPIXMAP)
		{
		    എങ്കിൽ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    അല്ലെങ്കിൽ
			gc = eb->pushbutton.background_gc;
		}
		അല്ലെങ്കിൽ
		    gc = eb->pushbutton.fill_gc;
	}
	അല്ലെങ്കിൽ
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	എങ്കിൽ (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    എങ്കിൽ (Lab_IsMenupane(eb))
    {
	എങ്കിൽ (eb->pushbutton.armed)
	    (*(((Xm_തള്ളുക_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    അല്ലെങ്കിൽ
    {
	adjust = 0;

#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	തിരഞ്ഞെടുപ്പ് (default_button_emphasis)
	{
	    അവസ്ഥ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		നിർത്തുക;

	    അവസ്ഥ XmINTERNAL_HIGHLIGHT:
		നിർത്തുക;

	    സ്ഥിരസ്ഥിതി:
		പ്രതിജ്ഞാ(FALSE);
		തിരികെ;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	എങ്കിൽ (box.x > adjust)
	{
	    പൂർണ്ണസംഖ്യ borderwidth =box.x - adjust;
	    പൂർണ്ണസംഖ്യ rectwidth = eb->core.width - 2 * adjust;
	    പൂർണ്ണസംഖ്യ rectheight = eb->core.height - 2 * adjust;

	    എങ്കിൽ (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    അല്ലെങ്കിൽ
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    തിരഞ്ഞെടുപ്പ് (default_button_emphasis)
	    {
		അവസ്ഥ XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    എങ്കിൽ (eb->primitive.highlight_drawn)
			(*(((Xm_തള്ളുക_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
		    നിർത്തുക;
	    }
#endif
	}

	എങ്കിൽ (eb->pushbutton.default_button_shadow_thickness)
	{
	    എങ്കിൽ (eb->pushbutton.show_as_default)
	    {
		/*
		 *  - get the topShadowColor and bottomShadowColor from the
		 *  parent; use those colors to construct top and bottom gc;
		 *  use these GCs to draw the shadows of the button.
		 *
		 *  - Should not be called if pushbutton is in a row column or
		 *  in a menu.
		 *
		 *  - Should be called only if a defaultbuttonshadow is to be
		 *  drawn.
		 */
		GC	    മുകൾ_gc;
		GC	    bottom_gc;
		പൂർണ്ണസംഖ്യ	    default_button_shadow_thickness;
		പൂർണ്ണസംഖ്യ	    x, y, width, height, delta;
		Widget	    parent;

		എങ്കിൽ (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    തിരികെ;

		എങ്കിൽ (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    തിരികെ;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		എങ്കിൽ (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_മുകൾ_Shadow_GC(eb);
		    മുകൾ_gc = XmParentBottomShadowGC(eb);
		}
		അല്ലെങ്കിൽ
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.മുകൾ_shadow_GC;
		    മുകൾ_gc = eb->primitive.bottom_shadow_GC;
		}

		എങ്കിൽ ((bottom_gc == None) || (മുകൾ_gc == None))
		    തിരികെ;


		എങ്കിൽ (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		അല്ലെങ്കിൽ
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#എങ്കിൽ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		തിരഞ്ഞെടുപ്പ് (default_button_emphasis)
		{
		    അവസ്ഥ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			നിർത്തുക;

		    അവസ്ഥ XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			നിർത്തുക;

		    സ്ഥിരസ്ഥിതി:
			പ്രതിജ്ഞാ(FALSE);
			തിരികെ;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		എങ്കിൽ ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    മുകൾ_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	എങ്കിൽ (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    സ്ഥിതം ശൂന്യം
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_തള്ളുക_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ശൂന്യ, ശൂന്യ);
}

    സ്ഥിതം ശൂന്യം
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_തള്ളുക_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ശൂന്യ, ശൂന്യ);
}

#endif // FEAT_TOOLBAR
