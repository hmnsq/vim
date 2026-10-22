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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_TOOLBAR

#અન્તર્ભાવ <Xm/XmP.h>
#અન્તર્ભાવ <Xm/DrawP.h>
#જો defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#નહીંતર
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  નહીંતર
#   include <X11/xpm.h>
#  endif
# endif
#endif
#અન્તર્ભાવ <Xm/ManagerP.h>
#અન્તર્ભાવ <Xm/Display.h>
#અન્તર્ભાવ <Xm/DisplayP.h>

#અન્તર્ભાવ <X11/Shell.h>
#અન્તર્ભાવ <X11/ShellP.h>

#અન્તર્ભાવ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#જો defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (પૂર્ણાંક)XmMENU_POPUP || \
		    Lab_MenuType(w) == (પૂર્ણાંક)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, લીટી_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#જો defined(VMS)
# જો defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# નહીંતર
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#નહીંતર
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

બાહ્ય Boolean _XmGetInDragMode(Widget widget);
બાહ્ય શૂન્ય _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * અંક_params);
બાહ્ય શૂન્ય _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * અંક_params);
બાહ્ય શૂન્ય _XmSetFocusFlag(Widget w, unsigned પૂર્ણાંક mask, Boolean કિંમત);
બાહ્ય શૂન્ય _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
સ્થિર શૂન્ય Destroy(Widget w);
સ્થિર શૂન્ય Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
સ્થિર Boolean Set_કિંમતો(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
સ્થિર શૂન્ય Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
સ્થિર શૂન્ય Enter(Widget, XEvent *, String *, Cardinal *);
સ્થિર શૂન્ય Leave(Widget, XEvent *, String *, Cardinal *);
સ્થિર શૂન્ય BorderHighlight(Widget);
સ્થિર શૂન્ય BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
સ્થિર અક્ષર stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
સ્થિર XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

સ્થિર XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, કદ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) ખાલી
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, કદ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) ખાલી
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, કદ(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, કદ(પૂર્ણાંક),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_જમણું
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ખાલી,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ કદ(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ખાલી
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ધકેલો_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ કદ(XmEnhancedButtonRec),
	/* class_initialize	 */ ખાલી,
	/* class_part_initialize */ ખાલી,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ખાલી,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_સંખ્યા(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_સંખ્યા(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_કિંમતો,
	/* set_values_hook	 */ ખાલી,
	/* set_values_almost	 */ Xt_Inherit_Set_કિંમતો_Almost,
	/* get_values_hook	 */ ખાલી,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ ખાલી,
	/* tm_table		 */ ખાલી,
	/* query_geometry	 */ ખાલી,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ખાલી
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ખાલી,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ખાલી,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) ખાલી,
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
    સ્થિર unsigned short
bump_color(unsigned short કિંમત)
{
    પૂર્ણાંક tmp = 2 * (((પૂર્ણાંક) કિંમત - 65535) / 3) + 65535;

    પાછા tmp;
}

    સ્થિર પૂર્ણાંક
alloc_color(Display	*display,
	Colormap	colormap,
	અક્ષર		*colorname,
	XColor		*xcolor,
	શૂન્ય		*closure UNUSED)
{
    પૂર્ણાંક status;

    જો (colorname)
	જો (!XParseColor(display, colormap, colorname, xcolor))
	    પાછા -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    પાછા status != 0 ? 1 : 0;
}

// XPM
સ્થિર અક્ષર * blank_xpm[] =
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
    સ્થિર શૂન્ય
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    મૂળ;
    સ્થિર Xpm_Color_પ્રતીક color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ખાલી, 0},
	{"foreground", ખાલી, 0},
	{"bottomShadowColor", ખાલી, 0},
	{"topShadowColor", ખાલી, 0},
	{"highlightColor", ખાલી, 0},
	{"armColor", ખાલી, 0}
    };
    પૂર્ણાંક		    scr;
    Display	    *dpy = XtDisplay(eb);
    પૂર્ણાંક		    x;
    પૂર્ણાંક		    y;
    unsigned પૂર્ણાંક    height, width, border, depth;
    પૂર્ણાંક		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ઉપલું_pix = None;
    અક્ષર	    **data = (અક્ષર **) eb->enhancedbutton.pixmap_data;
    અક્ષર	    *fname = (અક્ષર *) eb->enhancedbutton.pixmap_file;
    પૂર્ણાંક		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    જો (!data)
	પાછા;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ખાલી);

    scr = DefaultScreen(dpy);
    મૂળ = મૂળ_Window(dpy, scr);

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
	    &eb->primitive.ટોચ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.ટોચ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_સંખ્યા(color);

    જો (fname)
	status = XpmReadFileToPixmap(dpy, મૂળ, fname, &pix, &mask, &attr);
    જો (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, મૂળ, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    જો (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, મૂળ, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &મૂળ, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    જો (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, મૂળ, width + shift, height + shift, depth);

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
#નહીંતર
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_સંખ્યા(color);

    status = XpmCreatePixmapFromData(dpy, મૂળ, data, &pix, ખાલી, &attr);
    XpmFreeAttributes(&attr);

    ઉપલું_pix = XCreatePixmap(dpy, મૂળ, width + shift, height + shift, depth);

#જો 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#નહીંતર
    XSetForeground(dpy, gc, eb->primitive.ટોચ_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ઉપલું_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ઉપલું_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    જો (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    નહીંતર
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ચાવી;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_ચાવી = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, મૂળ, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, મૂળ, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.ટોચ_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = ઉપલું_pix;
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

    સ્થિર શૂન્ય
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		ટોચ_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    જો (!eb->primitive.shadow_thickness)
	પાછા;

    જો ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	પાછા;

#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#નહીંતર
    etched_in = False;
#endif
    જો (!etched_in ^ eb->pushbutton.armed)
    {
	ટોચ_gc = eb->primitive.ટોચ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    નહીંતર
    {
	ટોચ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.ટોચ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    ટોચ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    સ્થિર શૂન્ય
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    જો (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	પાછા;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    સ્થિર શૂન્ય
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    જો (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	પાછા;

    જો (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	જો (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != ખાલી) && (UnhighlightT->getUnhighlightGC != ખાલી))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	નહીંતર
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#નહીંતર
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	જો (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    નહીંતર
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    સ્થિર શૂન્ય
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    પૂર્ણાંક		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    પૂર્ણાંક		x;
    પૂર્ણાંક		y;

    જો (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    નહીંતર
    {
	જો (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	નહીંતર જો (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	નહીંતર
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    જો (pix == None || !eb->enhancedbutton.pixmap_data)
	પાછા;

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
    જો (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    જો (h < height)
	height = h;
    જો (depth == (પૂર્ણાંક)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    નહીંતર જો (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    સ્થિર શૂન્ય
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ખાલી;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#નહીંતર
    Boolean	etched_in = False;
#endif

    જો (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	જો (eb->label.label_type == (પૂર્ણાંક)XmSTRING
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

    જો (eb->label.label_type == (પૂર્ણાંક)XmPIXMAP)
    {
	જો (eb->pushbutton.armed)
	{
	    જો (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    નહીંતર
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	નહીંતર
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    જો (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ડાબું(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_જમણું(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ટોચ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    જો (deadjusted)
    {
	Lab_Margin_ડાબું(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_જમણું(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ટોચ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    જો (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    સ્થિર શૂન્ય
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *અંક_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ધકેલો_Button_Callback_Struct call_કિંમત;

    જો (Lab_IsMenupane(eb))
    {
	જો ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#નહીંતર
	    Boolean etched_in = False;
#endif

	    જો (eb->pushbutton.armed)
		પાછા;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    જો (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ખાલી);
		draw_pixmap(eb, event, ખાલી);
	    }

	    જો ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.ટોચ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    જો (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_કિંમત.reason = (પૂર્ણાંક)XmCR_ARM;
		call_કિંમત.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_કિંમત);
	    }
	}
    }
    નહીંતર
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ખાલી, ખાલી);
	જો (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ખાલી);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ખાલી);
    }
}

    સ્થિર શૂન્ય
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *અંક_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ધકેલો_Button_Callback_Struct call_કિંમત;

    જો (Lab_IsMenupane(eb))
    {
#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#નહીંતર
	Boolean etched_in = False;
#endif

	જો (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = ખાલી;

	    જો (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ખાલી);
		draw_pixmap(eb, event, ખાલી);
	    }
	    નહીંતર
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    જો (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_કિંમત.reason = (પૂર્ણાંક)XmCR_DISARM;
		call_કિંમત.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_કિંમત);
	    }
	}
    }
    નહીંતર
    {
	_XmPrimitiveLeave((Widget) eb, event, ખાલી, ખાલી);

	જો (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ખાલી);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ખાલી);
	    eb->pushbutton.armed = TRUE;
	}
	નહીંતર
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ખાલી);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    સ્થિર શૂન્ય
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned પૂર્ણાંક w = 0;
    unsigned પૂર્ણાંક h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    જો (newtb->enhancedbutton.pixmap_data
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
    જો (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ડાબું = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    નહીંતર
    {
	newtb->label.margin_ડાબું = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	જો (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_ડાબું += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    જો (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    નહીંતર
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#જો 0
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

    સ્થિર શૂન્ય
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    જો (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	મૂળ_W = Default_મૂળ_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, મૂળ_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    જો (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_કિંમતો((Widget) eb, XmNlabelString, str, ખાલી);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    જો (request->core.width == 0)
	eb->core.width = 0;
    જો (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    સ્થિર શૂન્ય
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ઉપલું_pix = eb->enhancedbutton.highlight_pixmap;

    જો (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    જો (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    જો (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    જો (ઉપલું_pix != None && ઉપલું_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ઉપલું_pix);
}

    સ્થિર શૂન્ય
Destroy(Widget w)
{
    જો (!XmIsEnhancedButton(w))
	પાછા;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    સ્થિર Boolean
Set_કિંમતો(Widget current,
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
    જો (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	જો (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	નહીંતર
	{
	    પૂર્ણાંક		    r_x;
	    પૂર્ણાંક		    r_y;
	    unsigned પૂર્ણાંક    r_height;
	    unsigned પૂર્ણાંક    r_width;
	    unsigned પૂર્ણાંક    r_border;
	    unsigned પૂર્ણાંક    r_depth;
	    પૂર્ણાંક		    મૂળ_x;
	    પૂર્ણાંક		    મૂળ_y;
	    પૂર્ણાંક		    win_x;
	    પૂર્ણાંક		    win_y;
	    Window	    મૂળ;
	    Window	    મૂળ_q;
	    Window	    child;
	    unsigned પૂર્ણાંક    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &મૂળ, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &મૂળ_q, &child,
			  &મૂળ_x, &મૂળ_y, &win_x, &win_y, &mask);

	    જો (મૂળ == મૂળ_q)
	    {
		જો ((win_x < 0) || (win_y < 0))
		    પાછા False;

		જો ((win_x > (પૂર્ણાંક)r_width) || (win_y > (પૂર્ણાંક)r_height))
		    પાછા False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	પાછા True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    જો (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    જો (NOT_EQUAL(primitive.foreground))
	redraw = True;
    જો (NOT_EQUAL(core.background_pixel))
	redraw = True;
    જો (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    જો (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    જો (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    જો (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    જો (redraw == True)
    {
	જો (change)
	    set_pixmap(eb);
	જો (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	નહીંતર
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	જો (change)
	    set_size(eb);
	redraw = False;
    }

    પાછા redraw;
}

    સ્થિર શૂન્ય
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    પૂર્ણાંક			    dx;
    પૂર્ણાંક			    adjust;
    short		    fill = 0;

    જો (!XtIsRealized((Widget)eb))
	પાછા;

#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    જો ((eb->pushbutton.arm_color == eb->primitive.ટોચ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    જો (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    નહીંતર
	adjust = eb->pushbutton.default_button_shadow_thickness;

    જો (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    નહીંતર
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
    જો (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	જો (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		જો (eb->label.label_type == (પૂર્ણાંક)XmPIXMAP)
		{
		    જો (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    નહીંતર
			gc = eb->pushbutton.background_gc;
		}
		નહીંતર
		    gc = eb->pushbutton.fill_gc;
	}
	નહીંતર
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	જો (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    જો (Lab_IsMenupane(eb))
    {
	જો (eb->pushbutton.armed)
	    (*(((Xm_ધકેલો_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    નહીંતર
    {
	adjust = 0;

#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	પસંદગી (default_button_emphasis)
	{
	    સ્થિતિ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		તોડો;

	    સ્થિતિ XmINTERNAL_HIGHLIGHT:
		તોડો;

	    મૂળભૂત:
		પ્રતિજ્ઞા(FALSE);
		પાછા;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	જો (box.x > adjust)
	{
	    પૂર્ણાંક borderwidth =box.x - adjust;
	    પૂર્ણાંક rectwidth = eb->core.width - 2 * adjust;
	    પૂર્ણાંક rectheight = eb->core.height - 2 * adjust;

	    જો (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    નહીંતર
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    પસંદગી (default_button_emphasis)
	    {
		સ્થિતિ XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    જો (eb->primitive.highlight_drawn)
			(*(((Xm_ધકેલો_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    તોડો;

		મૂળભૂત:
		    તોડો;
	    }
#endif
	}

	જો (eb->pushbutton.default_button_shadow_thickness)
	{
	    જો (eb->pushbutton.show_as_default)
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
		GC	    ટોચ_gc;
		GC	    bottom_gc;
		પૂર્ણાંક	    default_button_shadow_thickness;
		પૂર્ણાંક	    x, y, width, height, delta;
		Widget	    parent;

		જો (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    પાછા;

		જો (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    પાછા;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		જો (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_ટોચ_Shadow_GC(eb);
		    ટોચ_gc = XmParentBottomShadowGC(eb);
		}
		નહીંતર
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.ટોચ_shadow_GC;
		    ટોચ_gc = eb->primitive.bottom_shadow_GC;
		}

		જો ((bottom_gc == None) || (ટોચ_gc == None))
		    પાછા;


		જો (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		નહીંતર
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#જો !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		પસંદગી (default_button_emphasis)
		{
		    સ્થિતિ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			તોડો;

		    સ્થિતિ XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			તોડો;

		    મૂળભૂત:
			પ્રતિજ્ઞા(FALSE);
			પાછા;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		જો ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    ટોચ_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	જો (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    સ્થિર શૂન્ય
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ધકેલો_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ખાલી, ખાલી);
}

    સ્થિર શૂન્ય
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ધકેલો_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ખાલી, ખાલી);
}

#endif // FEAT_TOOLBAR
