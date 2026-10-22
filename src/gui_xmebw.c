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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_TOOLBAR

#ଅନ୍ତର୍ଭାଵ <Xm/XmP.h>
#ଅନ୍ତର୍ଭାଵ <Xm/DrawP.h>
#ଯଦି defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#ଅନ୍ୟଥା
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ଅନ୍ୟଥା
#   include <X11/xpm.h>
#  endif
# endif
#endif
#ଅନ୍ତର୍ଭାଵ <Xm/ManagerP.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Display.h>
#ଅନ୍ତର୍ଭାଵ <Xm/DisplayP.h>

#ଅନ୍ତର୍ଭାଵ <X11/Shell.h>
#ଅନ୍ତର୍ଭାଵ <X11/ShellP.h>

#ଅନ୍ତର୍ଭାଵ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#ଯଦି defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmMENU_POPUP || \
		    Lab_MenuType(w) == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, ଧାଡ଼ି_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#ଯଦି defined(VMS)
# ଯଦି defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# ଅନ୍ୟଥା
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#ଅନ୍ୟଥା
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

ବାହ୍ୟ Boolean _XmGetInDragMode(Widget widget);
ବାହ୍ୟ ଶୂନ୍ୟ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ଅଙ୍କ_params);
ବାହ୍ୟ ଶୂନ୍ୟ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ଅଙ୍କ_params);
ବାହ୍ୟ ଶୂନ୍ୟ _XmSetFocusFlag(Widget w, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask, Boolean ମୂଲ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ସ୍ଥିର ଶୂନ୍ୟ Destroy(Widget w);
ସ୍ଥିର ଶୂନ୍ୟ Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
ସ୍ଥିର Boolean Set_ମୂଲ୍ୟଗୁଡ଼ିକ(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
ସ୍ଥିର ଶୂନ୍ୟ Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ସ୍ଥିର ଶୂନ୍ୟ Enter(Widget, XEvent *, String *, Cardinal *);
ସ୍ଥିର ଶୂନ୍ୟ Leave(Widget, XEvent *, String *, Cardinal *);
ସ୍ଥିର ଶୂନ୍ୟ BorderHighlight(Widget);
ସ୍ଥିର ଶୂନ୍ୟ BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
ସ୍ଥିର ଅକ୍ଷର stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ସ୍ଥିର XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ସ୍ଥିର XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, ଆକାର(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) ଖାଲି
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, ଆକାର(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) ଖାଲି
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, ଆକାର(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_ଡାହାଣ
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ଖାଲି,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ ଆକାର(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ଖାଲି
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ଠେଲ_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ ଆକାର(XmEnhancedButtonRec),
	/* class_initialize	 */ ଖାଲି,
	/* class_part_initialize */ ଖାଲି,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ଖାଲି,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_ସଂଖ୍ୟା(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_ସଂଖ୍ୟା(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_ମୂଲ୍ୟଗୁଡ଼ିକ,
	/* set_values_hook	 */ ଖାଲି,
	/* set_values_almost	 */ Xt_Inherit_Set_ମୂଲ୍ୟଗୁଡ଼ିକ_Almost,
	/* get_values_hook	 */ ଖାଲି,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ ଖାଲି,
	/* tm_table		 */ ଖାଲି,
	/* query_geometry	 */ ଖାଲି,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ଖାଲି
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ଖାଲି,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ଖାଲି,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) ଖାଲି,
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
    ସ୍ଥିର unsigned short
bump_color(unsigned short ମୂଲ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = 2 * (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା) ମୂଲ୍ୟ - 65535) / 3) + 65535;

    ଫେରନ୍ତୁ tmp;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
alloc_color(Display	*display,
	Colormap	colormap,
	ଅକ୍ଷର		*colorname,
	XColor		*xcolor,
	ଶୂନ୍ୟ		*closure UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;

    ଯଦି (colorname)
	ଯଦି (!XParseColor(display, colormap, colorname, xcolor))
	    ଫେରନ୍ତୁ -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    ଫେରନ୍ତୁ status != 0 ? 1 : 0;
}

// XPM
ସ୍ଥିର ଅକ୍ଷର * blank_xpm[] =
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
    ସ୍ଥିର ଶୂନ୍ୟ
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    ମୂଳ;
    ସ୍ଥିର Xpm_Color_ପ୍ରତୀକ color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ଖାଲି, 0},
	{"foreground", ଖାଲି, 0},
	{"bottomShadowColor", ଖାଲି, 0},
	{"topShadowColor", ଖାଲି, 0},
	{"highlightColor", ଖାଲି, 0},
	{"armColor", ଖାଲି, 0}
    };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    scr;
    Display	    *dpy = XtDisplay(eb);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    y;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    height, width, border, depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ଉପର_pix = None;
    ଅକ୍ଷର	    **data = (ଅକ୍ଷର **) eb->enhancedbutton.pixmap_data;
    ଅକ୍ଷର	    *fname = (ଅକ୍ଷର *) eb->enhancedbutton.pixmap_file;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    ଯଦି (!data)
	ଫେରନ୍ତୁ;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ଖାଲି);

    scr = DefaultScreen(dpy);
    ମୂଳ = ମୂଳ_Window(dpy, scr);

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
	    &eb->primitive.ଶୀର୍ଷ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.ଶୀର୍ଷ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ସଂଖ୍ୟା(color);

    ଯଦି (fname)
	status = XpmReadFileToPixmap(dpy, ମୂଳ, fname, &pix, &mask, &attr);
    ଯଦି (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ମୂଳ, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    ଯଦି (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ମୂଳ, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &ମୂଳ, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    ଯଦି (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, ମୂଳ, width + shift, height + shift, depth);

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
#ଅନ୍ୟଥା
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ସଂଖ୍ୟା(color);

    status = XpmCreatePixmapFromData(dpy, ମୂଳ, data, &pix, ଖାଲି, &attr);
    XpmFreeAttributes(&attr);

    ଉପର_pix = XCreatePixmap(dpy, ମୂଳ, width + shift, height + shift, depth);

#ଯଦି 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#ଅନ୍ୟଥା
    XSetForeground(dpy, gc, eb->primitive.ଶୀର୍ଷ_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ଉପର_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ଉପର_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    ଯଦି (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    ଅନ୍ୟଥା
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ଚାବି;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_ଚାବି = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, ମୂଳ, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ମୂଳ, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.ଶୀର୍ଷ_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = ଉପର_pix;
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

    ସ୍ଥିର ଶୂନ୍ୟ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		ଶୀର୍ଷ_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    ଯଦି (!eb->primitive.shadow_thickness)
	ଫେରନ୍ତୁ;

    ଯଦି ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	ଫେରନ୍ତୁ;

#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#ଅନ୍ୟଥା
    etched_in = False;
#endif
    ଯଦି (!etched_in ^ eb->pushbutton.armed)
    {
	ଶୀର୍ଷ_gc = eb->primitive.ଶୀର୍ଷ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    ଅନ୍ୟଥା
    {
	ଶୀର୍ଷ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.ଶୀର୍ଷ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    ଶୀର୍ଷ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    ସ୍ଥିର ଶୂନ୍ୟ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    ଯଦି (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ଫେରନ୍ତୁ;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    ସ୍ଥିର ଶୂନ୍ୟ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    ଯଦି (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ଫେରନ୍ତୁ;

    ଯଦି (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	ଯଦି (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != ଖାଲି) && (UnhighlightT->getUnhighlightGC != ଖାଲି))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	ଅନ୍ୟଥା
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#ଅନ୍ୟଥା
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	ଯଦି (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    ଅନ୍ୟଥା
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    ସ୍ଥିର ଶୂନ୍ୟ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;

    ଯଦି (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    ଅନ୍ୟଥା
    {
	ଯଦି (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	ଅନ୍ୟଥା ଯଦି (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	ଅନ୍ୟଥା
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    ଯଦି (pix == None || !eb->enhancedbutton.pixmap_data)
	ଫେରନ୍ତୁ;

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
    ଯଦି (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    ଯଦି (h < height)
	height = h;
    ଯଦି (depth == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    ଅନ୍ୟଥା ଯଦି (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ଖାଲି;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#ଅନ୍ୟଥା
    Boolean	etched_in = False;
#endif

    ଯଦି (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	ଯଦି (eb->label.label_type == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmSTRING
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

    ଯଦି (eb->label.label_type == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmPIXMAP)
    {
	ଯଦି (eb->pushbutton.armed)
	{
	    ଯଦି (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    ଅନ୍ୟଥା
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	ଅନ୍ୟଥା
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    ଯଦି (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ବାମ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ଡାହାଣ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ଶୀର୍ଷ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    ଯଦି (deadjusted)
    {
	Lab_Margin_ବାମ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ଡାହାଣ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ଶୀର୍ଷ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    ଯଦି (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    ସ୍ଥିର ଶୂନ୍ୟ
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *ଅଙ୍କ_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ଠେଲ_Button_Callback_Struct call_ମୂଲ୍ୟ;

    ଯଦି (Lab_IsMenupane(eb))
    {
	ଯଦି ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#ଅନ୍ୟଥା
	    Boolean etched_in = False;
#endif

	    ଯଦି (eb->pushbutton.armed)
		ଫେରନ୍ତୁ;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    ଯଦି (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ଖାଲି);
		draw_pixmap(eb, event, ଖାଲି);
	    }

	    ଯଦି ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.ଶୀର୍ଷ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    ଯଦି (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ମୂଲ୍ୟ.reason = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmCR_ARM;
		call_ମୂଲ୍ୟ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_ମୂଲ୍ୟ);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ଖାଲି, ଖାଲି);
	ଯଦି (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ଖାଲି);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ଖାଲି);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *ଅଙ୍କ_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ଠେଲ_Button_Callback_Struct call_ମୂଲ୍ୟ;

    ଯଦି (Lab_IsMenupane(eb))
    {
#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#ଅନ୍ୟଥା
	Boolean etched_in = False;
#endif

	ଯଦି (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = ଖାଲି;

	    ଯଦି (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ଖାଲି);
		draw_pixmap(eb, event, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    ଯଦି (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ମୂଲ୍ୟ.reason = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmCR_DISARM;
		call_ମୂଲ୍ୟ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_ମୂଲ୍ୟ);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	_XmPrimitiveLeave((Widget) eb, event, ଖାଲି, ଖାଲି);

	ଯଦି (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ଖାଲି);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ଖାଲି);
	    eb->pushbutton.armed = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ଖାଲି);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ସ୍ଥିର ଶୂନ୍ୟ
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା w = 0;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    ଯଦି (newtb->enhancedbutton.pixmap_data
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
    ଯଦି (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ବାମ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    ଅନ୍ୟଥା
    {
	newtb->label.margin_ବାମ = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	ଯଦି (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_ବାମ += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    ଯଦି (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    ଅନ୍ୟଥା
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#ଯଦି 0
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

    ସ୍ଥିର ଶୂନ୍ୟ
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    ଯଦି (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	ମୂଳ_W = Default_ମୂଳ_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, ମୂଳ_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    ଯଦି (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ((Widget) eb, XmNlabelString, str, ଖାଲି);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    ଯଦି (request->core.width == 0)
	eb->core.width = 0;
    ଯଦି (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    ସ୍ଥିର ଶୂନ୍ୟ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ଉପର_pix = eb->enhancedbutton.highlight_pixmap;

    ଯଦି (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    ଯଦି (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    ଯଦି (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    ଯଦି (ଉପର_pix != None && ଉପର_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ଉପର_pix);
}

    ସ୍ଥିର ଶୂନ୍ୟ
Destroy(Widget w)
{
    ଯଦି (!XmIsEnhancedButton(w))
	ଫେରନ୍ତୁ;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ସ୍ଥିର Boolean
Set_ମୂଲ୍ୟଗୁଡ଼ିକ(Widget current,
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
    ଯଦି (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	ଯଦି (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    r_x;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    r_y;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    r_height;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    r_width;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    r_border;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    r_depth;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ମୂଳ_x;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ମୂଳ_y;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    win_x;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    win_y;
	    Window	    ମୂଳ;
	    Window	    ମୂଳ_q;
	    Window	    child;
	    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &ମୂଳ, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &ମୂଳ_q, &child,
			  &ମୂଳ_x, &ମୂଳ_y, &win_x, &win_y, &mask);

	    ଯଦି (ମୂଳ == ମୂଳ_q)
	    {
		ଯଦି ((win_x < 0) || (win_y < 0))
		    ଫେରନ୍ତୁ False;

		ଯଦି ((win_x > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)r_width) || (win_y > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)r_height))
		    ଫେରନ୍ତୁ False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	ଫେରନ୍ତୁ True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    ଯଦି (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    ଯଦି (NOT_EQUAL(primitive.foreground))
	redraw = True;
    ଯଦି (NOT_EQUAL(core.background_pixel))
	redraw = True;
    ଯଦି (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    ଯଦି (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    ଯଦି (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    ଯଦି (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    ଯଦି (redraw == True)
    {
	ଯଦି (change)
	    set_pixmap(eb);
	ଯଦି (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	ଅନ୍ୟଥା
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	ଯଦି (change)
	    set_size(eb);
	redraw = False;
    }

    ଫେରନ୍ତୁ redraw;
}

    ସ୍ଥିର ଶୂନ୍ୟ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    dx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    adjust;
    short		    fill = 0;

    ଯଦି (!XtIsRealized((Widget)eb))
	ଫେରନ୍ତୁ;

#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    ଯଦି ((eb->pushbutton.arm_color == eb->primitive.ଶୀର୍ଷ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    ଯଦି (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    ଅନ୍ୟଥା
	adjust = eb->pushbutton.default_button_shadow_thickness;

    ଯଦି (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    ଅନ୍ୟଥା
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
    ଯଦି (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	ଯଦି (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		ଯଦି (eb->label.label_type == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmPIXMAP)
		{
		    ଯଦି (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    ଅନ୍ୟଥା
			gc = eb->pushbutton.background_gc;
		}
		ଅନ୍ୟଥା
		    gc = eb->pushbutton.fill_gc;
	}
	ଅନ୍ୟଥା
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	ଯଦି (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    ଯଦି (Lab_IsMenupane(eb))
    {
	ଯଦି (eb->pushbutton.armed)
	    (*(((Xm_ଠେଲ_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    ଅନ୍ୟଥା
    {
	adjust = 0;

#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	ବାଛ (default_button_emphasis)
	{
	    ସ୍ଥିତି XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି XmINTERNAL_HIGHLIGHT:
		ଭାଙ୍ଗ;

	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ପ୍ରତିଜ୍ଞା(FALSE);
		ଫେରନ୍ତୁ;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	ଯଦି (box.x > adjust)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା borderwidth =box.x - adjust;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rectwidth = eb->core.width - 2 * adjust;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rectheight = eb->core.height - 2 * adjust;

	    ଯଦି (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    ଅନ୍ୟଥା
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    ବାଛ (default_button_emphasis)
	    {
		ସ୍ଥିତି XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    ଯଦି (eb->primitive.highlight_drawn)
			(*(((Xm_ଠେଲ_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଭାଙ୍ଗ;
	    }
#endif
	}

	ଯଦି (eb->pushbutton.default_button_shadow_thickness)
	{
	    ଯଦି (eb->pushbutton.show_as_default)
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
		GC	    ଶୀର୍ଷ_gc;
		GC	    bottom_gc;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    default_button_shadow_thickness;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x, y, width, height, delta;
		Widget	    parent;

		ଯଦି (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    ଫେରନ୍ତୁ;

		ଯଦି (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    ଫେରନ୍ତୁ;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		ଯଦି (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_ଶୀର୍ଷ_Shadow_GC(eb);
		    ଶୀର୍ଷ_gc = XmParentBottomShadowGC(eb);
		}
		ଅନ୍ୟଥା
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.ଶୀର୍ଷ_shadow_GC;
		    ଶୀର୍ଷ_gc = eb->primitive.bottom_shadow_GC;
		}

		ଯଦି ((bottom_gc == None) || (ଶୀର୍ଷ_gc == None))
		    ଫେରନ୍ତୁ;


		ଯଦି (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		ଅନ୍ୟଥା
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#ଯଦି !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		ବାଛ (default_button_emphasis)
		{
		    ସ୍ଥିତି XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ଭାଙ୍ଗ;

		    ସ୍ଥିତି XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ଭାଙ୍ଗ;

		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ପ୍ରତିଜ୍ଞା(FALSE);
			ଫେରନ୍ତୁ;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		ଯଦି ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    ଶୀର୍ଷ_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	ଯଦି (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ଠେଲ_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ଖାଲି, ଖାଲି);
}

    ସ୍ଥିର ଶୂନ୍ୟ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ଠେଲ_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ଖାଲି, ଖାଲି);
}

#endif // FEAT_TOOLBAR
