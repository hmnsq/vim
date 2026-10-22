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
#यदि defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
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
#यदि defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (पूर्णाङ्क)XmMENU_POPUP || \
		    Lab_MenuType(w) == (पूर्णाङ्क)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, पंक्ति_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#यदि defined(VMS)
# यदि defined(HAVE_XOS_R_H)
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
बाह्य शून्य _XmSetFocusFlag(Widget w, unsigned पूर्णाङ्क mask, Boolean मूल्य);
बाह्य शून्य _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
स्थिर शून्य Destroy(Widget w);
स्थिर शून्य Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
स्थिर Boolean Set_मूल्यावली(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
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
स्थिर वर्ण stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
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
	XmNpixmapData, XmCPixmap, XmRString, परिमाण(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) रिक्त
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, परिमाण(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) रिक्त
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, परिमाण(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, परिमाण(पूर्णाङ्क),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_दायाँ
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ रिक्त,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ परिमाण(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ रिक्त
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_धकेलो_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ परिमाण(XmEnhancedButtonRec),
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
	/* set_values		 */ Set_मूल्यावली,
	/* set_values_hook	 */ रिक्त,
	/* set_values_almost	 */ Xt_Inherit_Set_मूल्यावली_Almost,
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
    स्थिर unsigned short
bump_color(unsigned short मूल्य)
{
    पूर्णाङ्क tmp = 2 * (((पूर्णाङ्क) मूल्य - 65535) / 3) + 65535;

    प्रत्यागम tmp;
}

    स्थिर पूर्णाङ्क
alloc_color(Display	*display,
	Colormap	colormap,
	वर्ण		*colorname,
	XColor		*xcolor,
	शून्य		*closure UNUSED)
{
    पूर्णाङ्क status;

    यदि (colorname)
	यदि (!XParseColor(display, colormap, colorname, xcolor))
	    प्रत्यागम -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    प्रत्यागम status != 0 ? 1 : 0;
}

// XPM
स्थिर वर्ण * blank_xpm[] =
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
    Window	    मूल;
    स्थिर Xpm_Color_प्रतीक color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", रिक्त, 0},
	{"foreground", रिक्त, 0},
	{"bottomShadowColor", रिक्त, 0},
	{"topShadowColor", रिक्त, 0},
	{"highlightColor", रिक्त, 0},
	{"armColor", रिक्त, 0}
    };
    पूर्णाङ्क		    scr;
    Display	    *dpy = XtDisplay(eb);
    पूर्णाङ्क		    x;
    पूर्णाङ्क		    y;
    unsigned पूर्णाङ्क    height, width, border, depth;
    पूर्णाङ्क		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ऊपरी_pix = None;
    वर्ण	    **data = (वर्ण **) eb->enhancedbutton.pixmap_data;
    वर्ण	    *fname = (वर्ण *) eb->enhancedbutton.pixmap_file;
    पूर्णाङ्क		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    यदि (!data)
	प्रत्यागम;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, रिक्त);

    scr = DefaultScreen(dpy);
    मूल = मूल_Window(dpy, scr);

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

    यदि (fname)
	status = XpmReadFileToPixmap(dpy, मूल, fname, &pix, &mask, &attr);
    यदि (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, मूल, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    यदि (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, मूल, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &मूल, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    यदि (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, मूल, width + shift, height + shift, depth);

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

    status = XpmCreatePixmapFromData(dpy, मूल, data, &pix, रिक्त, &attr);
    XpmFreeAttributes(&attr);

    ऊपरी_pix = XCreatePixmap(dpy, मूल, width + shift, height + shift, depth);

#यदि 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#अन्यथा
    XSetForeground(dpy, gc, eb->primitive.शीर्ष_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ऊपरी_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ऊपरी_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    यदि (eb->pushbutton.fill_on_arm)
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_कुंजी;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_कुंजी = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, मूल, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, मूल, width + shift, height + shift, depth);

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
    eb->enhancedbutton.highlight_pixmap = ऊपरी_pix;
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

    यदि (!eb->primitive.shadow_thickness)
	प्रत्यागम;

    यदि ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	प्रत्यागम;

#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#अन्यथा
    etched_in = False;
#endif
    यदि (!etched_in ^ eb->pushbutton.armed)
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
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    स्थिर शून्य
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    यदि (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	प्रत्यागम;

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

    यदि (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	प्रत्यागम;

    यदि (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	यदि (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
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
	यदि (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
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
    पूर्णाङ्क		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    पूर्णाङ्क		x;
    पूर्णाङ्क		y;

    यदि (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    अन्यथा
    {
	यदि (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	अन्यथा यदि (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	अन्यथा
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    यदि (pix == None || !eb->enhancedbutton.pixmap_data)
	प्रत्यागम;

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
    यदि (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    यदि (h < height)
	height = h;
    यदि (depth == (पूर्णाङ्क)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    अन्यथा यदि (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
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
#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
    Boolean	etched_in = False;
#endif

    यदि (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	यदि (eb->label.label_type == (पूर्णाङ्क)XmSTRING
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

    यदि (eb->label.label_type == (पूर्णाङ्क)XmPIXMAP)
    {
	यदि (eb->pushbutton.armed)
	{
	    यदि (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
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
    यदि (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_बायाँ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_दायाँ(eb) -= Xm3D_ENHANCE_PIXEL;
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

    यदि (deadjusted)
    {
	Lab_Margin_बायाँ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_दायाँ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_शीर्ष(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    यदि (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    स्थिर शून्य
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *अंक_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_धकेलो_Button_Callback_Struct call_मूल्य;

    यदि (Lab_IsMenupane(eb))
    {
	यदि ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
	    Boolean etched_in = False;
#endif

	    यदि (eb->pushbutton.armed)
		प्रत्यागम;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    यदि (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, रिक्त);
		draw_pixmap(eb, event, रिक्त);
	    }

	    यदि ((eb->core.width > 2 * eb->primitive.highlight_thickness)
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
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    यदि (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_मूल्य.reason = (पूर्णाङ्क)XmCR_ARM;
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
	यदि (eb->pushbutton.armed == TRUE)
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
    Xm_धकेलो_Button_Callback_Struct call_मूल्य;

    यदि (Lab_IsMenupane(eb))
    {
#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#अन्यथा
	Boolean etched_in = False;
#endif

	यदि (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = रिक्त;

	    यदि (etched_in && !XmIsTearOffButton(eb))
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

	    यदि (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_मूल्य.reason = (पूर्णाङ्क)XmCR_DISARM;
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

	यदि (eb->pushbutton.armed == TRUE)
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
    unsigned पूर्णाङ्क w = 0;
    unsigned पूर्णाङ्क h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    यदि (newtb->enhancedbutton.pixmap_data
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
    यदि (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_बायाँ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    अन्यथा
    {
	newtb->label.margin_बायाँ = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	यदि (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_बायाँ += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    यदि (newtb->label.TextRect.height < h)
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

#यदि 0
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
    यदि (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	मूल_W = Default_मूल_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, मूल_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    यदि (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_मूल्यावली((Widget) eb, XmNlabelString, str, रिक्त);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    यदि (request->core.width == 0)
	eb->core.width = 0;
    यदि (request->core.height == 0)
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
    Pixmap ऊपरी_pix = eb->enhancedbutton.highlight_pixmap;

    यदि (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    यदि (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    यदि (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    यदि (ऊपरी_pix != None && ऊपरी_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ऊपरी_pix);
}

    स्थिर शून्य
Destroy(Widget w)
{
    यदि (!XmIsEnhancedButton(w))
	प्रत्यागम;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    स्थिर Boolean
Set_मूल्यावली(Widget current,
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
    यदि (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	यदि (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	अन्यथा
	{
	    पूर्णाङ्क		    r_x;
	    पूर्णाङ्क		    r_y;
	    unsigned पूर्णाङ्क    r_height;
	    unsigned पूर्णाङ्क    r_width;
	    unsigned पूर्णाङ्क    r_border;
	    unsigned पूर्णाङ्क    r_depth;
	    पूर्णाङ्क		    मूल_x;
	    पूर्णाङ्क		    मूल_y;
	    पूर्णाङ्क		    win_x;
	    पूर्णाङ्क		    win_y;
	    Window	    मूल;
	    Window	    मूल_q;
	    Window	    child;
	    unsigned पूर्णाङ्क    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &मूल, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &मूल_q, &child,
			  &मूल_x, &मूल_y, &win_x, &win_y, &mask);

	    यदि (मूल == मूल_q)
	    {
		यदि ((win_x < 0) || (win_y < 0))
		    प्रत्यागम False;

		यदि ((win_x > (पूर्णाङ्क)r_width) || (win_y > (पूर्णाङ्क)r_height))
		    प्रत्यागम False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	प्रत्यागम True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    यदि (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    यदि (NOT_EQUAL(primitive.foreground))
	redraw = True;
    यदि (NOT_EQUAL(core.background_pixel))
	redraw = True;
    यदि (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    यदि (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    यदि (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    यदि (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    यदि (redraw == True)
    {
	यदि (change)
	    set_pixmap(eb);
	यदि (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	अन्यथा
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	यदि (change)
	    set_size(eb);
	redraw = False;
    }

    प्रत्यागम redraw;
}

    स्थिर शून्य
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    पूर्णाङ्क			    dx;
    पूर्णाङ्क			    adjust;
    short		    fill = 0;

    यदि (!XtIsRealized((Widget)eb))
	प्रत्यागम;

#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    यदि ((eb->pushbutton.arm_color == eb->primitive.शीर्ष_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    यदि (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    अन्यथा
	adjust = eb->pushbutton.default_button_shadow_thickness;

    यदि (adjust > 0)
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
    यदि (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	यदि (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		यदि (eb->label.label_type == (पूर्णाङ्क)XmPIXMAP)
		{
		    यदि (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
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
	यदि (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    यदि (Lab_IsMenupane(eb))
    {
	यदि (eb->pushbutton.armed)
	    (*(((Xm_धकेलो_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    अन्यथा
    {
	adjust = 0;

#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	विकल्प (default_button_emphasis)
	{
	    पक्ष XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		भङ्ग;

	    पक्ष XmINTERNAL_HIGHLIGHT:
		भङ्ग;

	    स्वभाव:
		प्रतिज्ञा(FALSE);
		प्रत्यागम;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	यदि (box.x > adjust)
	{
	    पूर्णाङ्क borderwidth =box.x - adjust;
	    पूर्णाङ्क rectwidth = eb->core.width - 2 * adjust;
	    पूर्णाङ्क rectheight = eb->core.height - 2 * adjust;

	    यदि (XmIsManager(XtParent(eb)))
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

#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    विकल्प (default_button_emphasis)
	    {
		पक्ष XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    यदि (eb->primitive.highlight_drawn)
			(*(((Xm_धकेलो_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    भङ्ग;

		स्वभाव:
		    भङ्ग;
	    }
#endif
	}

	यदि (eb->pushbutton.default_button_shadow_thickness)
	{
	    यदि (eb->pushbutton.show_as_default)
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
		पूर्णाङ्क	    default_button_shadow_thickness;
		पूर्णाङ्क	    x, y, width, height, delta;
		Widget	    parent;

		यदि (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    प्रत्यागम;

		यदि (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    प्रत्यागम;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		यदि (XmIsManager(parent))
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

		यदि ((bottom_gc == None) || (शीर्ष_gc == None))
		    प्रत्यागम;


		यदि (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		अन्यथा
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#यदि !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		विकल्प (default_button_emphasis)
		{
		    पक्ष XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			भङ्ग;

		    पक्ष XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			भङ्ग;

		    स्वभाव:
			प्रतिज्ञा(FALSE);
			प्रत्यागम;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		यदि ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    शीर्ष_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	यदि (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    स्थिर शून्य
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_धकेलो_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, रिक्त, रिक्त);
}

    स्थिर शून्य
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_धकेलो_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, रिक्त, रिक्त);
}

#endif // FEAT_TOOLBAR
