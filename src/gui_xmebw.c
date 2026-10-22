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

#अन्तर्भाव "vim.h"

#ifdef FEAT_TOOLBAR

#अन्तर्भाव <Xm/XmP.h>
#अन्तर्भाव <Xm/DrawP.h>
#जर defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#अन्यथा
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  अन्यथा
#   include <X11/xpm.h>
#  endif
# endif
#endif
#अन्तर्भाव <Xm/ManagerP.h>
#अन्तर्भाव <Xm/Display.h>
#अन्तर्भाव <Xm/DisplayP.h>

#अन्तर्भाव <X11/Shell.h>
#अन्तर्भाव <X11/ShellP.h>

#अन्तर्भाव "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#जर defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (पूर्णांक)XmMENU_POPUP || \
		    Lab_MenuType(w) == (पूर्णांक)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, ओळ_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#जर defined(VMS)
# जर defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# अन्यथा
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#अन्यथा
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

बाह्य Boolean _XmGetInDragMode(Widget widget);
बाह्य शून्य _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * अंक_params);
बाह्य शून्य _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * अंक_params);
बाह्य शून्य _XmSetFocusFlag(Widget w, अचिन्हित पूर्णांक mask, Boolean मूल्य);
बाह्य शून्य _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
स्थिर शून्य Destroy(Widget w);
स्थिर शून्य Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
स्थिर Boolean Set_मूल्ये(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
स्थिर शून्य Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
स्थिर शून्य Enter(Widget, XEvent *, String *, Cardinal *);
स्थिर शून्य Leave(Widget, XEvent *, String *, Cardinal *);
स्थिर शून्य BorderHighlight(Widget);
स्थिर शून्य BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
स्थिर अक्षर stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
स्थिर XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

स्थिर XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, आकार(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) रिक्त
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, आकार(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) रिक्त
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, आकार(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, आकार(पूर्णांक),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_उजवा
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ रिक्त,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ आकार(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ रिक्त
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ढकला_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ आकार(XmEnhancedButtonRec),
	/* class_initialize	 */ रिक्त,
	/* class_part_initialize */ रिक्त,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ रिक्त,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_संख्या(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_संख्या(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_मूल्ये,
	/* set_values_hook	 */ रिक्त,
	/* set_values_almost	 */ Xt_Inherit_Set_मूल्ये_Almost,
	/* get_values_hook	 */ रिक्त,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ रिक्त,
	/* tm_table		 */ रिक्त,
	/* query_geometry	 */ रिक्त,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ रिक्त
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ रिक्त,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ रिक्त,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) रिक्त,
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
    स्थिर अचिन्हित लघु
bump_color(अचिन्हित लघु मूल्य)
{
    पूर्णांक tmp = 2 * (((पूर्णांक) मूल्य - 65535) / 3) + 65535;

    परत tmp;
}

    स्थिर पूर्णांक
alloc_color(Display	*display,
	Colormap	colormap,
	अक्षर		*colorname,
	XColor		*xcolor,
	शून्य		*closure UNUSED)
{
    पूर्णांक status;

    जर (colorname)
	जर (!XParseColor(display, colormap, colorname, xcolor))
	    परत -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    परत status != 0 ? 1 : 0;
}

// XPM
स्थिर अक्षर * blank_xpm[] =
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
    स्थिर शून्य
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    मूळ;
    स्थिर Xpm_Color_चिन्ह color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", रिक्त, 0},
	{"foreground", रिक्त, 0},
	{"bottomShadowColor", रिक्त, 0},
	{"topShadowColor", रिक्त, 0},
	{"highlightColor", रिक्त, 0},
	{"armColor", रिक्त, 0}
    };
    पूर्णांक		    scr;
    Display	    *dpy = XtDisplay(eb);
    पूर्णांक		    x;
    पूर्णांक		    y;
    अचिन्हित पूर्णांक    height, width, border, depth;
    पूर्णांक		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    वरचा_pix = None;
    अक्षर	    **data = (अक्षर **) eb->enhancedbutton.pixmap_data;
    अक्षर	    *fname = (अक्षर *) eb->enhancedbutton.pixmap_file;
    पूर्णांक		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    जर (!data)
	परत;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, रिक्त);

    scr = DefaultScreen(dpy);
    मूळ = मूळ_Window(dpy, scr);

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
	    &eb->primitive.शीर्ष_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.शीर्ष_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_संख्या(color);

    जर (fname)
	status = XpmReadFileToPixmap(dpy, मूळ, fname, &pix, &mask, &attr);
    जर (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, मूळ, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    जर (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, मूळ, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &मूळ, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    जर (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, मूळ, width + shift, height + shift, depth);

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
#अन्यथा
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_संख्या(color);

    status = XpmCreatePixmapFromData(dpy, मूळ, data, &pix, रिक्त, &attr);
    XpmFreeAttributes(&attr);

    वरचा_pix = XCreatePixmap(dpy, मूळ, width + shift, height + shift, depth);

#जर 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#अन्यथा
    XSetForeground(dpy, gc, eb->primitive.शीर्ष_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, वरचा_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, वरचा_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    जर (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    अन्यथा
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_किल्ली;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_किल्ली = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, मूळ, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, मूळ, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.शीर्ष_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = वरचा_pix;
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

    स्थिर शून्य
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		शीर्ष_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    जर (!eb->primitive.shadow_thickness)
	परत;

    जर ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	परत;

#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#अन्यथा
    etched_in = False;
#endif
    जर (!etched_in ^ eb->pushbutton.armed)
    {
	शीर्ष_gc = eb->primitive.शीर्ष_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    अन्यथा
    {
	शीर्ष_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.शीर्ष_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    शीर्ष_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (अचिन्हित)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    स्थिर शून्य
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    जर (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	परत;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    स्थिर शून्य
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    जर (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	परत;

    जर (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	जर (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != रिक्त) && (UnhighlightT->getUnhighlightGC != रिक्त))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	अन्यथा
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#अन्यथा
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	जर (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    अन्यथा
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    स्थिर शून्य
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    पूर्णांक		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    पूर्णांक		x;
    पूर्णांक		y;

    जर (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    अन्यथा
    {
	जर (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	अन्यथा जर (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	अन्यथा
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    जर (pix == None || !eb->enhancedbutton.pixmap_data)
	परत;

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
    जर (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    जर (h < height)
	height = h;
    जर (depth == (पूर्णांक)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    अन्यथा जर (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (अचिन्हित दीर्घ)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    स्थिर शून्य
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = रिक्त;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
    Boolean	etched_in = False;
#endif

    जर (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	जर (eb->label.label_type == (पूर्णांक)XmSTRING
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

    जर (eb->label.label_type == (पूर्णांक)XmPIXMAP)
    {
	जर (eb->pushbutton.armed)
	{
	    जर (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    अन्यथा
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	अन्यथा
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    जर (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_डावा(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_उजवा(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_शीर्ष(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    जर (deadjusted)
    {
	Lab_Margin_डावा(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_उजवा(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_शीर्ष(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    जर (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    स्थिर शून्य
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *अंक_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ढकला_Button_Callback_Struct call_मूल्य;

    जर (Lab_IsMenupane(eb))
    {
	जर ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
	    Boolean etched_in = False;
#endif

	    जर (eb->pushbutton.armed)
		परत;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    जर (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, रिक्त);
		draw_pixmap(eb, event, रिक्त);
	    }

	    जर ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.शीर्ष_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(अचिन्हित)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    जर (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_मूल्य.reason = (पूर्णांक)XmCR_ARM;
		call_मूल्य.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_मूल्य);
	    }
	}
    }
    अन्यथा
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, रिक्त, रिक्त);
	जर (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) रिक्त);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, रिक्त);
    }
}

    स्थिर शून्य
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *अंक_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ढकला_Button_Callback_Struct call_मूल्य;

    जर (Lab_IsMenupane(eb))
    {
#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
	Boolean etched_in = False;
#endif

	जर (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = रिक्त;

	    जर (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, रिक्त);
		draw_pixmap(eb, event, रिक्त);
	    }
	    अन्यथा
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    जर (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_मूल्य.reason = (पूर्णांक)XmCR_DISARM;
		call_मूल्य.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_मूल्य);
	    }
	}
    }
    अन्यथा
    {
	_XmPrimitiveLeave((Widget) eb, event, रिक्त, रिक्त);

	जर (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)रिक्त);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, रिक्त);
	    eb->pushbutton.armed = TRUE;
	}
	अन्यथा
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, रिक्त);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    स्थिर शून्य
set_size(XmEnhancedButtonWidget newtb)
{
    अचिन्हित पूर्णांक w = 0;
    अचिन्हित पूर्णांक h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    जर (newtb->enhancedbutton.pixmap_data
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
    जर (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_डावा = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    अन्यथा
    {
	newtb->label.margin_डावा = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	जर (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_डावा += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    जर (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    अन्यथा
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#जर 0
    म_लिखो("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.normal_pixmap,
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

    स्थिर शून्य
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    जर (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	मूळ_W = Default_मूळ_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, मूळ_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    जर (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_मूल्ये((Widget) eb, XmNlabelString, str, रिक्त);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    जर (request->core.width == 0)
	eb->core.width = 0;
    जर (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    स्थिर शून्य
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap वरचा_pix = eb->enhancedbutton.highlight_pixmap;

    जर (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    जर (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    जर (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    जर (वरचा_pix != None && वरचा_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), वरचा_pix);
}

    स्थिर शून्य
Destroy(Widget w)
{
    जर (!XmIsEnhancedButton(w))
	परत;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    स्थिर Boolean
Set_मूल्ये(Widget current,
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
    जर (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	जर (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	अन्यथा
	{
	    पूर्णांक		    r_x;
	    पूर्णांक		    r_y;
	    अचिन्हित पूर्णांक    r_height;
	    अचिन्हित पूर्णांक    r_width;
	    अचिन्हित पूर्णांक    r_border;
	    अचिन्हित पूर्णांक    r_depth;
	    पूर्णांक		    मूळ_x;
	    पूर्णांक		    मूळ_y;
	    पूर्णांक		    win_x;
	    पूर्णांक		    win_y;
	    Window	    मूळ;
	    Window	    मूळ_q;
	    Window	    child;
	    अचिन्हित पूर्णांक    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &मूळ, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &मूळ_q, &child,
			  &मूळ_x, &मूळ_y, &win_x, &win_y, &mask);

	    जर (मूळ == मूळ_q)
	    {
		जर ((win_x < 0) || (win_y < 0))
		    परत False;

		जर ((win_x > (पूर्णांक)r_width) || (win_y > (पूर्णांक)r_height))
		    परत False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	परत True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    जर (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    जर (NOT_EQUAL(primitive.foreground))
	redraw = True;
    जर (NOT_EQUAL(core.background_pixel))
	redraw = True;
    जर (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    जर (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    जर (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    जर (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    जर (redraw == True)
    {
	जर (change)
	    set_pixmap(eb);
	जर (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	अन्यथा
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	जर (change)
	    set_size(eb);
	redraw = False;
    }

    परत redraw;
}

    स्थिर शून्य
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    पूर्णांक			    dx;
    पूर्णांक			    adjust;
    लघु		    fill = 0;

    जर (!XtIsRealized((Widget)eb))
	परत;

#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    जर ((eb->pushbutton.arm_color == eb->primitive.शीर्ष_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    जर (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    अन्यथा
	adjust = eb->pushbutton.default_button_shadow_thickness;

    जर (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    अन्यथा
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
    जर (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	जर (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		जर (eb->label.label_type == (पूर्णांक)XmPIXMAP)
		{
		    जर (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    अन्यथा
			gc = eb->pushbutton.background_gc;
		}
		अन्यथा
		    gc = eb->pushbutton.fill_gc;
	}
	अन्यथा
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	जर (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    जर (Lab_IsMenupane(eb))
    {
	जर (eb->pushbutton.armed)
	    (*(((Xm_ढकला_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    अन्यथा
    {
	adjust = 0;

#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	निवड (default_button_emphasis)
	{
	    स्थिती XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		थांबा;

	    स्थिती XmINTERNAL_HIGHLIGHT:
		थांबा;

	    पूर्वनिर्धारित:
		प्रतिज्ञा(FALSE);
		परत;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	जर (box.x > adjust)
	{
	    पूर्णांक borderwidth =box.x - adjust;
	    पूर्णांक rectwidth = eb->core.width - 2 * adjust;
	    पूर्णांक rectheight = eb->core.height - 2 * adjust;

	    जर (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    अन्यथा
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    निवड (default_button_emphasis)
	    {
		स्थिती XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    जर (eb->primitive.highlight_drawn)
			(*(((Xm_ढकला_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    थांबा;

		पूर्वनिर्धारित:
		    थांबा;
	    }
#endif
	}

	जर (eb->pushbutton.default_button_shadow_thickness)
	{
	    जर (eb->pushbutton.show_as_default)
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
		GC	    शीर्ष_gc;
		GC	    bottom_gc;
		पूर्णांक	    default_button_shadow_thickness;
		पूर्णांक	    x, y, width, height, delta;
		Widget	    parent;

		जर (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    परत;

		जर (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    परत;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		जर (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_शीर्ष_Shadow_GC(eb);
		    शीर्ष_gc = XmParentBottomShadowGC(eb);
		}
		अन्यथा
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.शीर्ष_shadow_GC;
		    शीर्ष_gc = eb->primitive.bottom_shadow_GC;
		}

		जर ((bottom_gc == None) || (शीर्ष_gc == None))
		    परत;


		जर (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		अन्यथा
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#जर !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		निवड (default_button_emphasis)
		{
		    स्थिती XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			थांबा;

		    स्थिती XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			थांबा;

		    पूर्वनिर्धारित:
			प्रतिज्ञा(FALSE);
			परत;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		जर ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    शीर्ष_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (अचिन्हित)XmSHADOW_OUT);
	    }
	}

	जर (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    स्थिर शून्य
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ढकला_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, रिक्त, रिक्त);
}

    स्थिर शून्य
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ढकला_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, रिक्त, रिक्त);
}

#endif // FEAT_TOOLBAR
