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

#ضمن "vim.h"

#ifdef FEAT_TOOLBAR

#ضمن <Xm/XmP.h>
#ضمن <Xm/DrawP.h>
#إذا defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#وإلا
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  وإلا
#   include <X11/xpm.h>
#  endif
# endif
#endif
#ضمن <Xm/ManagerP.h>
#ضمن <Xm/Display.h>
#ضمن <Xm/DisplayP.h>

#ضمن <X11/Shell.h>
#ضمن <X11/ShellP.h>

#ضمن "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#إذا defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحيح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحيح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, باريس_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#إذا defined(VMS)
# إذا defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# وإلا
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#وإلا
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

خارجي Boolean _XmGetInDragMode(Widget widget);
خارجي فراغ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * اڠک_params);
خارجي فراغ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * اڠک_params);
خارجي فراغ _XmSetFocusFlag(Widget w, unsigned صحيح mask, Boolean نيلاي);
خارجي فراغ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ساكن فراغ Destroy(Widget w);
ساكن فراغ Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
ساكن Boolean Set_نيلاي_سموا(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
ساكن فراغ Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ساكن فراغ Enter(Widget, XEvent *, String *, Cardinal *);
ساكن فراغ Leave(Widget, XEvent *, String *, Cardinal *);
ساكن فراغ BorderHighlight(Widget);
ساكن فراغ BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
ساكن حرف stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ساكن XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ساكن XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, حجم(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) عدم
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, حجم(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) عدم
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, حجم(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, حجم(صحيح),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_کانن
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ عدم,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ حجم(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ عدم
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_تولق_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ حجم(XmEnhancedButtonRec),
	/* class_initialize	 */ عدم,
	/* class_part_initialize */ عدم,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ عدم,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_نومبور(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_نومبور(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_نيلاي_سموا,
	/* set_values_hook	 */ عدم,
	/* set_values_almost	 */ Xt_Inherit_Set_نيلاي_سموا_Almost,
	/* get_values_hook	 */ عدم,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ عدم,
	/* tm_table		 */ عدم,
	/* query_geometry	 */ عدم,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ عدم
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ عدم,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ عدم,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) عدم,
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
    ساكن unsigned short
bump_color(unsigned short نيلاي)
{
    صحيح tmp = 2 * (((صحيح) نيلاي - 65535) / 3) + 65535;

    أرجع tmp;
}

    ساكن صحيح
alloc_color(Display	*display,
	Colormap	colormap,
	حرف		*colorname,
	XColor		*xcolor,
	فراغ		*closure UNUSED)
{
    صحيح status;

    إذا (colorname)
	إذا (!XParseColor(display, colormap, colorname, xcolor))
	    أرجع -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    أرجع status != 0 ? 1 : 0;
}

// XPM
ساكن حرف * blank_xpm[] =
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
    ساكن فراغ
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    اکر;
    ساكن Xpm_Color_سيمبول color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", عدم, 0},
	{"foreground", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"highlightColor", عدم, 0},
	{"armColor", عدم, 0}
    };
    صحيح		    scr;
    Display	    *dpy = XtDisplay(eb);
    صحيح		    x;
    صحيح		    y;
    unsigned صحيح    height, width, border, depth;
    صحيح		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    تيڠݢي_pix = None;
    حرف	    **data = (حرف **) eb->enhancedbutton.pixmap_data;
    حرف	    *fname = (حرف *) eb->enhancedbutton.pixmap_file;
    صحيح		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    إذا (!data)
	أرجع;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, عدم);

    scr = DefaultScreen(dpy);
    اکر = اکر_Window(dpy, scr);

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
	    &eb->primitive.اتس_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.اتس_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_نومبور(color);

    إذا (fname)
	status = XpmReadFileToPixmap(dpy, اکر, fname, &pix, &mask, &attr);
    إذا (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, اکر, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    إذا (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, اکر, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &اکر, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    إذا (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, اکر, width + shift, height + shift, depth);

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
#وإلا
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_نومبور(color);

    status = XpmCreatePixmapFromData(dpy, اکر, data, &pix, عدم, &attr);
    XpmFreeAttributes(&attr);

    تيڠݢي_pix = XCreatePixmap(dpy, اکر, width + shift, height + shift, depth);

#إذا 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#وإلا
    XSetForeground(dpy, gc, eb->primitive.اتس_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, تيڠݢي_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, تيڠݢي_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    إذا (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    وإلا
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_کونچي;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_کونچي = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, اکر, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, اکر, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.اتس_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = تيڠݢي_pix;
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

    ساكن فراغ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		اتس_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    إذا (!eb->primitive.shadow_thickness)
	أرجع;

    إذا ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	أرجع;

#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#وإلا
    etched_in = False;
#endif
    إذا (!etched_in ^ eb->pushbutton.armed)
    {
	اتس_gc = eb->primitive.اتس_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    وإلا
    {
	اتس_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.اتس_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    اتس_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    ساكن فراغ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    إذا (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    إذا (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    إذا (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	إذا (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != عدم) && (UnhighlightT->getUnhighlightGC != عدم))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	وإلا
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#وإلا
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	إذا (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    وإلا
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    صحيح		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    صحيح		x;
    صحيح		y;

    إذا (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    وإلا
    {
	إذا (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	وإلا إذا (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	وإلا
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    إذا (pix == None || !eb->enhancedbutton.pixmap_data)
	أرجع;

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
    إذا (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    إذا (h < height)
	height = h;
    إذا (depth == (صحيح)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    وإلا إذا (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ساكن فراغ
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = عدم;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#وإلا
    Boolean	etched_in = False;
#endif

    إذا (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	إذا (eb->label.label_type == (صحيح)XmSTRING
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

    إذا (eb->label.label_type == (صحيح)XmPIXMAP)
    {
	إذا (eb->pushbutton.armed)
	{
	    إذا (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    وإلا
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	وإلا
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    إذا (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_کيري(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_کانن(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_اتس(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    إذا (deadjusted)
    {
	Lab_Margin_کيري(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_کانن(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_اتس(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    إذا (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    ساكن فراغ
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *اڠک_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_تولق_Button_Callback_Struct call_نيلاي;

    إذا (Lab_IsMenupane(eb))
    {
	إذا ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#وإلا
	    Boolean etched_in = False;
#endif

	    إذا (eb->pushbutton.armed)
		أرجع;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    إذا (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }

	    إذا ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.اتس_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    إذا (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_نيلاي.reason = (صحيح)XmCR_ARM;
		call_نيلاي.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_نيلاي);
	    }
	}
    }
    وإلا
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, عدم, عدم);
	إذا (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) عدم);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, عدم);
    }
}

    ساكن فراغ
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *اڠک_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_تولق_Button_Callback_Struct call_نيلاي;

    إذا (Lab_IsMenupane(eb))
    {
#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#وإلا
	Boolean etched_in = False;
#endif

	إذا (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = عدم;

	    إذا (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }
	    وإلا
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    إذا (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_نيلاي.reason = (صحيح)XmCR_DISARM;
		call_نيلاي.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_نيلاي);
	    }
	}
    }
    وإلا
    {
	_XmPrimitiveLeave((Widget) eb, event, عدم, عدم);

	إذا (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)عدم);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, عدم);
	    eb->pushbutton.armed = TRUE;
	}
	وإلا
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, عدم);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ساكن فراغ
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned صحيح w = 0;
    unsigned صحيح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    إذا (newtb->enhancedbutton.pixmap_data
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
    إذا (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_کيري = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    وإلا
    {
	newtb->label.margin_کيري = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	إذا (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_کيري += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    إذا (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    وإلا
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#إذا 0
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

    ساكن فراغ
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    إذا (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	اکر_W = Default_اکر_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, اکر_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    إذا (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_نيلاي_سموا((Widget) eb, XmNlabelString, str, عدم);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    إذا (request->core.width == 0)
	eb->core.width = 0;
    إذا (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    ساكن فراغ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap تيڠݢي_pix = eb->enhancedbutton.highlight_pixmap;

    إذا (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    إذا (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    إذا (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    إذا (تيڠݢي_pix != None && تيڠݢي_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), تيڠݢي_pix);
}

    ساكن فراغ
Destroy(Widget w)
{
    إذا (!XmIsEnhancedButton(w))
	أرجع;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ساكن Boolean
Set_نيلاي_سموا(Widget current,
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
    إذا (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	إذا (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	وإلا
	{
	    صحيح		    r_x;
	    صحيح		    r_y;
	    unsigned صحيح    r_height;
	    unsigned صحيح    r_width;
	    unsigned صحيح    r_border;
	    unsigned صحيح    r_depth;
	    صحيح		    اکر_x;
	    صحيح		    اکر_y;
	    صحيح		    win_x;
	    صحيح		    win_y;
	    Window	    اکر;
	    Window	    اکر_q;
	    Window	    child;
	    unsigned صحيح    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &اکر, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &اکر_q, &child,
			  &اکر_x, &اکر_y, &win_x, &win_y, &mask);

	    إذا (اکر == اکر_q)
	    {
		إذا ((win_x < 0) || (win_y < 0))
		    أرجع False;

		إذا ((win_x > (صحيح)r_width) || (win_y > (صحيح)r_height))
		    أرجع False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	أرجع True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    إذا (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    إذا (NOT_EQUAL(primitive.foreground))
	redraw = True;
    إذا (NOT_EQUAL(core.background_pixel))
	redraw = True;
    إذا (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    إذا (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    إذا (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    إذا (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    إذا (redraw == True)
    {
	إذا (change)
	    set_pixmap(eb);
	إذا (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	وإلا
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	إذا (change)
	    set_size(eb);
	redraw = False;
    }

    أرجع redraw;
}

    ساكن فراغ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    صحيح			    dx;
    صحيح			    adjust;
    short		    fill = 0;

    إذا (!XtIsRealized((Widget)eb))
	أرجع;

#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    إذا ((eb->pushbutton.arm_color == eb->primitive.اتس_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    إذا (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    وإلا
	adjust = eb->pushbutton.default_button_shadow_thickness;

    إذا (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    وإلا
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
    إذا (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	إذا (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		إذا (eb->label.label_type == (صحيح)XmPIXMAP)
		{
		    إذا (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    وإلا
			gc = eb->pushbutton.background_gc;
		}
		وإلا
		    gc = eb->pushbutton.fill_gc;
	}
	وإلا
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	إذا (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    إذا (Lab_IsMenupane(eb))
    {
	إذا (eb->pushbutton.armed)
	    (*(((Xm_تولق_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    وإلا
    {
	adjust = 0;

#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	بدل (default_button_emphasis)
	{
	    حالة XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		اكسر;

	    حالة XmINTERNAL_HIGHLIGHT:
		اكسر;

	    افتراضي:
		أكد(FALSE);
		أرجع;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	إذا (box.x > adjust)
	{
	    صحيح borderwidth =box.x - adjust;
	    صحيح rectwidth = eb->core.width - 2 * adjust;
	    صحيح rectheight = eb->core.height - 2 * adjust;

	    إذا (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    وإلا
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    بدل (default_button_emphasis)
	    {
		حالة XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    إذا (eb->primitive.highlight_drawn)
			(*(((Xm_تولق_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    اكسر;

		افتراضي:
		    اكسر;
	    }
#endif
	}

	إذا (eb->pushbutton.default_button_shadow_thickness)
	{
	    إذا (eb->pushbutton.show_as_default)
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
		GC	    اتس_gc;
		GC	    bottom_gc;
		صحيح	    default_button_shadow_thickness;
		صحيح	    x, y, width, height, delta;
		Widget	    parent;

		إذا (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    أرجع;

		إذا (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    أرجع;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		إذا (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_اتس_Shadow_GC(eb);
		    اتس_gc = XmParentBottomShadowGC(eb);
		}
		وإلا
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.اتس_shadow_GC;
		    اتس_gc = eb->primitive.bottom_shadow_GC;
		}

		إذا ((bottom_gc == None) || (اتس_gc == None))
		    أرجع;


		إذا (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		وإلا
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#إذا !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		بدل (default_button_emphasis)
		{
		    حالة XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			اكسر;

		    حالة XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			اكسر;

		    افتراضي:
			أكد(FALSE);
			أرجع;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		إذا ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    اتس_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	إذا (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ساكن فراغ
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_تولق_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

    ساكن فراغ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_تولق_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

#endif // FEAT_TOOLBAR
