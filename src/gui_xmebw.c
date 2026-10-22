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

#شامل "vim.h"

#ifdef FEAT_TOOLBAR

#شامل <Xm/XmP.h>
#شامل <Xm/DrawP.h>
#اگر defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#وگرنه
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  وگرنه
#   include <X11/xpm.h>
#  endif
# endif
#endif
#شامل <Xm/ManagerP.h>
#شامل <Xm/Display.h>
#شامل <Xm/DisplayP.h>

#شامل <X11/Shell.h>
#شامل <X11/ShellP.h>

#شامل "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#اگر defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحیح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحیح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, سطر_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#اگر defined(VMS)
# اگر defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# وگرنه
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#وگرنه
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

بیرونی Boolean _XmGetInDragMode(Widget widget);
بیرونی تهی _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * رقم_params);
بیرونی تهی _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * رقم_params);
بیرونی تهی _XmSetFocusFlag(Widget w, unsigned صحیح mask, Boolean مقدار);
بیرونی تهی _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ایستا تهی Destroy(Widget w);
ایستا تهی Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
ایستا Boolean Set_مقادیر(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
ایستا تهی Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ایستا تهی Enter(Widget, XEvent *, String *, Cardinal *);
ایستا تهی Leave(Widget, XEvent *, String *, Cardinal *);
ایستا تهی BorderHighlight(Widget);
ایستا تهی BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
ایستا نویسه stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ایستا XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ایستا XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, اندازه(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) پوچ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, اندازه(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) پوچ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, اندازه(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, اندازه(صحیح),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_راست
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ پوچ,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ اندازه(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ پوچ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_افزودن_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ اندازه(XmEnhancedButtonRec),
	/* class_initialize	 */ پوچ,
	/* class_part_initialize */ پوچ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ پوچ,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_عدد(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_عدد(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_مقادیر,
	/* set_values_hook	 */ پوچ,
	/* set_values_almost	 */ Xt_Inherit_Set_مقادیر_Almost,
	/* get_values_hook	 */ پوچ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ پوچ,
	/* tm_table		 */ پوچ,
	/* query_geometry	 */ پوچ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ پوچ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ پوچ,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ پوچ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) پوچ,
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
    ایستا unsigned short
bump_color(unsigned short مقدار)
{
    صحیح tmp = 2 * (((صحیح) مقدار - 65535) / 3) + 65535;

    بازگشت tmp;
}

    ایستا صحیح
alloc_color(Display	*display,
	Colormap	colormap,
	نویسه		*colorname,
	XColor		*xcolor,
	تهی		*closure UNUSED)
{
    صحیح status;

    اگر (colorname)
	اگر (!XParseColor(display, colormap, colorname, xcolor))
	    بازگشت -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    بازگشت status != 0 ? 1 : 0;
}

// XPM
ایستا نویسه * blank_xpm[] =
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
    ایستا تهی
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    ریشه;
    ایستا Xpm_Color_نماد color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", پوچ, 0},
	{"foreground", پوچ, 0},
	{"bottomShadowColor", پوچ, 0},
	{"topShadowColor", پوچ, 0},
	{"highlightColor", پوچ, 0},
	{"armColor", پوچ, 0}
    };
    صحیح		    scr;
    Display	    *dpy = XtDisplay(eb);
    صحیح		    x;
    صحیح		    y;
    unsigned صحیح    height, width, border, depth;
    صحیح		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    بالا_pix = None;
    نویسه	    **data = (نویسه **) eb->enhancedbutton.pixmap_data;
    نویسه	    *fname = (نویسه *) eb->enhancedbutton.pixmap_file;
    صحیح		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    اگر (!data)
	بازگشت;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, پوچ);

    scr = DefaultScreen(dpy);
    ریشه = ریشه_Window(dpy, scr);

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
	    &eb->primitive.بالاترین_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.بالاترین_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_عدد(color);

    اگر (fname)
	status = XpmReadFileToPixmap(dpy, ریشه, fname, &pix, &mask, &attr);
    اگر (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ریشه, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    اگر (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ریشه, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &ریشه, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    اگر (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, ریشه, width + shift, height + shift, depth);

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
#وگرنه
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_عدد(color);

    status = XpmCreatePixmapFromData(dpy, ریشه, data, &pix, پوچ, &attr);
    XpmFreeAttributes(&attr);

    بالا_pix = XCreatePixmap(dpy, ریشه, width + shift, height + shift, depth);

#اگر 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#وگرنه
    XSetForeground(dpy, gc, eb->primitive.بالاترین_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, بالا_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, بالا_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    اگر (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    وگرنه
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_کلید;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_کلید = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, ریشه, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ریشه, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.بالاترین_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = بالا_pix;
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

    ایستا تهی
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		بالاترین_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    اگر (!eb->primitive.shadow_thickness)
	بازگشت;

    اگر ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	بازگشت;

#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#وگرنه
    etched_in = False;
#endif
    اگر (!etched_in ^ eb->pushbutton.armed)
    {
	بالاترین_gc = eb->primitive.بالاترین_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    وگرنه
    {
	بالاترین_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.بالاترین_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    بالاترین_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    ایستا تهی
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    اگر (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	بازگشت;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    ایستا تهی
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    اگر (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	بازگشت;

    اگر (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	اگر (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != پوچ) && (UnhighlightT->getUnhighlightGC != پوچ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	وگرنه
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#وگرنه
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	اگر (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    وگرنه
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    ایستا تهی
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    صحیح		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    صحیح		x;
    صحیح		y;

    اگر (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    وگرنه
    {
	اگر (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	وگرنه اگر (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	وگرنه
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    اگر (pix == None || !eb->enhancedbutton.pixmap_data)
	بازگشت;

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
    اگر (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    اگر (h < height)
	height = h;
    اگر (depth == (صحیح)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    وگرنه اگر (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ایستا تهی
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = پوچ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#وگرنه
    Boolean	etched_in = False;
#endif

    اگر (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	اگر (eb->label.label_type == (صحیح)XmSTRING
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

    اگر (eb->label.label_type == (صحیح)XmPIXMAP)
    {
	اگر (eb->pushbutton.armed)
	{
	    اگر (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    وگرنه
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	وگرنه
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    اگر (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_چپ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_راست(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_بالاترین(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    اگر (deadjusted)
    {
	Lab_Margin_چپ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_راست(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_بالاترین(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    اگر (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    ایستا تهی
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *رقم_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_افزودن_Button_Callback_Struct call_مقدار;

    اگر (Lab_IsMenupane(eb))
    {
	اگر ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#وگرنه
	    Boolean etched_in = False;
#endif

	    اگر (eb->pushbutton.armed)
		بازگشت;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    اگر (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, پوچ);
		draw_pixmap(eb, event, پوچ);
	    }

	    اگر ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.بالاترین_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    اگر (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_مقدار.reason = (صحیح)XmCR_ARM;
		call_مقدار.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_مقدار);
	    }
	}
    }
    وگرنه
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, پوچ, پوچ);
	اگر (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) پوچ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, پوچ);
    }
}

    ایستا تهی
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *رقم_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_افزودن_Button_Callback_Struct call_مقدار;

    اگر (Lab_IsMenupane(eb))
    {
#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#وگرنه
	Boolean etched_in = False;
#endif

	اگر (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = پوچ;

	    اگر (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, پوچ);
		draw_pixmap(eb, event, پوچ);
	    }
	    وگرنه
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    اگر (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_مقدار.reason = (صحیح)XmCR_DISARM;
		call_مقدار.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_مقدار);
	    }
	}
    }
    وگرنه
    {
	_XmPrimitiveLeave((Widget) eb, event, پوچ, پوچ);

	اگر (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)پوچ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, پوچ);
	    eb->pushbutton.armed = TRUE;
	}
	وگرنه
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, پوچ);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ایستا تهی
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned صحیح w = 0;
    unsigned صحیح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    اگر (newtb->enhancedbutton.pixmap_data
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
    اگر (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_چپ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    وگرنه
    {
	newtb->label.margin_چپ = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	اگر (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_چپ += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    اگر (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    وگرنه
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#اگر 0
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

    ایستا تهی
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    اگر (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	ریشه_W = Default_ریشه_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, ریشه_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    اگر (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_مقادیر((Widget) eb, XmNlabelString, str, پوچ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    اگر (request->core.width == 0)
	eb->core.width = 0;
    اگر (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    ایستا تهی
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap بالا_pix = eb->enhancedbutton.highlight_pixmap;

    اگر (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    اگر (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    اگر (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    اگر (بالا_pix != None && بالا_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), بالا_pix);
}

    ایستا تهی
Destroy(Widget w)
{
    اگر (!XmIsEnhancedButton(w))
	بازگشت;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ایستا Boolean
Set_مقادیر(Widget current,
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
    اگر (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	اگر (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	وگرنه
	{
	    صحیح		    r_x;
	    صحیح		    r_y;
	    unsigned صحیح    r_height;
	    unsigned صحیح    r_width;
	    unsigned صحیح    r_border;
	    unsigned صحیح    r_depth;
	    صحیح		    ریشه_x;
	    صحیح		    ریشه_y;
	    صحیح		    win_x;
	    صحیح		    win_y;
	    Window	    ریشه;
	    Window	    ریشه_q;
	    Window	    child;
	    unsigned صحیح    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &ریشه, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &ریشه_q, &child,
			  &ریشه_x, &ریشه_y, &win_x, &win_y, &mask);

	    اگر (ریشه == ریشه_q)
	    {
		اگر ((win_x < 0) || (win_y < 0))
		    بازگشت False;

		اگر ((win_x > (صحیح)r_width) || (win_y > (صحیح)r_height))
		    بازگشت False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	بازگشت True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    اگر (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    اگر (NOT_EQUAL(primitive.foreground))
	redraw = True;
    اگر (NOT_EQUAL(core.background_pixel))
	redraw = True;
    اگر (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    اگر (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    اگر (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    اگر (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    اگر (redraw == True)
    {
	اگر (change)
	    set_pixmap(eb);
	اگر (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	وگرنه
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	اگر (change)
	    set_size(eb);
	redraw = False;
    }

    بازگشت redraw;
}

    ایستا تهی
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    صحیح			    dx;
    صحیح			    adjust;
    short		    fill = 0;

    اگر (!XtIsRealized((Widget)eb))
	بازگشت;

#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    اگر ((eb->pushbutton.arm_color == eb->primitive.بالاترین_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    اگر (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    وگرنه
	adjust = eb->pushbutton.default_button_shadow_thickness;

    اگر (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    وگرنه
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
    اگر (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	اگر (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		اگر (eb->label.label_type == (صحیح)XmPIXMAP)
		{
		    اگر (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    وگرنه
			gc = eb->pushbutton.background_gc;
		}
		وگرنه
		    gc = eb->pushbutton.fill_gc;
	}
	وگرنه
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	اگر (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    اگر (Lab_IsMenupane(eb))
    {
	اگر (eb->pushbutton.armed)
	    (*(((Xm_افزودن_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    وگرنه
    {
	adjust = 0;

#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	انتخاب (default_button_emphasis)
	{
	    حالت XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		بشکن;

	    حالت XmINTERNAL_HIGHLIGHT:
		بشکن;

	    پیشفرض:
		أكد(FALSE);
		بازگشت;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	اگر (box.x > adjust)
	{
	    صحیح borderwidth =box.x - adjust;
	    صحیح rectwidth = eb->core.width - 2 * adjust;
	    صحیح rectheight = eb->core.height - 2 * adjust;

	    اگر (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    وگرنه
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    انتخاب (default_button_emphasis)
	    {
		حالت XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    اگر (eb->primitive.highlight_drawn)
			(*(((Xm_افزودن_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    بشکن;

		پیشفرض:
		    بشکن;
	    }
#endif
	}

	اگر (eb->pushbutton.default_button_shadow_thickness)
	{
	    اگر (eb->pushbutton.show_as_default)
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
		GC	    بالاترین_gc;
		GC	    bottom_gc;
		صحیح	    default_button_shadow_thickness;
		صحیح	    x, y, width, height, delta;
		Widget	    parent;

		اگر (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    بازگشت;

		اگر (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    بازگشت;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		اگر (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_بالاترین_Shadow_GC(eb);
		    بالاترین_gc = XmParentBottomShadowGC(eb);
		}
		وگرنه
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.بالاترین_shadow_GC;
		    بالاترین_gc = eb->primitive.bottom_shadow_GC;
		}

		اگر ((bottom_gc == None) || (بالاترین_gc == None))
		    بازگشت;


		اگر (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		وگرنه
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#اگر !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		انتخاب (default_button_emphasis)
		{
		    حالت XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			بشکن;

		    حالت XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			بشکن;

		    پیشفرض:
			أكد(FALSE);
			بازگشت;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		اگر ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    بالاترین_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	اگر (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ایستا تهی
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_افزودن_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, پوچ, پوچ);
}

    ایستا تهی
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_افزودن_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, پوچ, پوچ);
}

#endif // FEAT_TOOLBAR
