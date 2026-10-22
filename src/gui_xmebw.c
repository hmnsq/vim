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

#অন্তর্ভাব "vim.h"

#ifdef FEAT_TOOLBAR

#অন্তর্ভাব <Xm/XmP.h>
#অন্তর্ভাব <Xm/DrawP.h>
#যদি defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#অন্যথা
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  অন্যথা
#   include <X11/xpm.h>
#  endif
# endif
#endif
#অন্তর্ভাব <Xm/ManagerP.h>
#অন্তর্ভাব <Xm/Display.h>
#অন্তর্ভাব <Xm/DisplayP.h>

#অন্তর্ভাব <X11/Shell.h>
#অন্তর্ভাব <X11/ShellP.h>

#অন্তর্ভাব "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#যদি defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (পূর্ণাঙ্ক)XmMENU_POPUP || \
		    Lab_MenuType(w) == (পূর্ণাঙ্ক)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, সারি_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#যদি defined(VMS)
# যদি defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# অন্যথা
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#অন্যথা
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

বাহ্য Boolean _XmGetInDragMode(Widget widget);
বাহ্য শূন্য _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * অঙ্ক_params);
বাহ্য শূন্য _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * অঙ্ক_params);
বাহ্য শূন্য _XmSetFocusFlag(Widget w, unsigned পূর্ণাঙ্ক mask, Boolean মান);
বাহ্য শূন্য _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
স্থির শূন্য Destroy(Widget w);
স্থির শূন্য Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
স্থির Boolean Set_মানসমূহ(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
স্থির শূন্য Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
স্থির শূন্য Enter(Widget, XEvent *, String *, Cardinal *);
স্থির শূন্য Leave(Widget, XEvent *, String *, Cardinal *);
স্থির শূন্য BorderHighlight(Widget);
স্থির শূন্য BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
স্থির বর্ণ stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
স্থির XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

স্থির XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, পরিমাণ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) রিক্ত
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, পরিমাণ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) রিক্ত
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, পরিমাণ(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, পরিমাণ(পূর্ণাঙ্ক),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_ডান
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ রিক্ত,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ পরিমাণ(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ রিক্ত
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ঠেলো_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ পরিমাণ(XmEnhancedButtonRec),
	/* class_initialize	 */ রিক্ত,
	/* class_part_initialize */ রিক্ত,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ রিক্ত,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_সংখ্যা(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_সংখ্যা(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_মানসমূহ,
	/* set_values_hook	 */ রিক্ত,
	/* set_values_almost	 */ Xt_Inherit_Set_মানসমূহ_Almost,
	/* get_values_hook	 */ রিক্ত,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ রিক্ত,
	/* tm_table		 */ রিক্ত,
	/* query_geometry	 */ রিক্ত,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ রিক্ত
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ রিক্ত,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ রিক্ত,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) রিক্ত,
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
    স্থির unsigned short
bump_color(unsigned short মান)
{
    পূর্ণাঙ্ক tmp = 2 * (((পূর্ণাঙ্ক) মান - 65535) / 3) + 65535;

    প্রত্যাগম tmp;
}

    স্থির পূর্ণাঙ্ক
alloc_color(Display	*display,
	Colormap	colormap,
	বর্ণ		*colorname,
	XColor		*xcolor,
	শূন্য		*closure UNUSED)
{
    পূর্ণাঙ্ক status;

    যদি (colorname)
	যদি (!XParseColor(display, colormap, colorname, xcolor))
	    প্রত্যাগম -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    প্রত্যাগম status != 0 ? 1 : 0;
}

// XPM
স্থির বর্ণ * blank_xpm[] =
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
    স্থির শূন্য
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    মূল;
    স্থির Xpm_Color_প্রতীক color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", রিক্ত, 0},
	{"foreground", রিক্ত, 0},
	{"bottomShadowColor", রিক্ত, 0},
	{"topShadowColor", রিক্ত, 0},
	{"highlightColor", রিক্ত, 0},
	{"armColor", রিক্ত, 0}
    };
    পূর্ণাঙ্ক		    scr;
    Display	    *dpy = XtDisplay(eb);
    পূর্ণাঙ্ক		    x;
    পূর্ণাঙ্ক		    y;
    unsigned পূর্ণাঙ্ক    height, width, border, depth;
    পূর্ণাঙ্ক		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    উপরের_pix = None;
    বর্ণ	    **data = (বর্ণ **) eb->enhancedbutton.pixmap_data;
    বর্ণ	    *fname = (বর্ণ *) eb->enhancedbutton.pixmap_file;
    পূর্ণাঙ্ক		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    যদি (!data)
	প্রত্যাগম;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, রিক্ত);

    scr = DefaultScreen(dpy);
    মূল = মূল_Window(dpy, scr);

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
	    &eb->primitive.শীর্ষ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.শীর্ষ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_সংখ্যা(color);

    যদি (fname)
	status = XpmReadFileToPixmap(dpy, মূল, fname, &pix, &mask, &attr);
    যদি (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, মূল, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    যদি (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, মূল, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &মূল, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    যদি (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, মূল, width + shift, height + shift, depth);

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
#অন্যথা
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_সংখ্যা(color);

    status = XpmCreatePixmapFromData(dpy, মূল, data, &pix, রিক্ত, &attr);
    XpmFreeAttributes(&attr);

    উপরের_pix = XCreatePixmap(dpy, মূল, width + shift, height + shift, depth);

#যদি 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#অন্যথা
    XSetForeground(dpy, gc, eb->primitive.শীর্ষ_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, উপরের_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, উপরের_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    যদি (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    অন্যথা
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_চাবি;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_চাবি = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, মূল, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, মূল, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.শীর্ষ_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = উপরের_pix;
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

    স্থির শূন্য
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		শীর্ষ_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    যদি (!eb->primitive.shadow_thickness)
	প্রত্যাগম;

    যদি ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	প্রত্যাগম;

#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#অন্যথা
    etched_in = False;
#endif
    যদি (!etched_in ^ eb->pushbutton.armed)
    {
	শীর্ষ_gc = eb->primitive.শীর্ষ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    অন্যথা
    {
	শীর্ষ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.শীর্ষ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    শীর্ষ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    স্থির শূন্য
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    যদি (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	প্রত্যাগম;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    স্থির শূন্য
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    যদি (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	প্রত্যাগম;

    যদি (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	যদি (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != রিক্ত) && (UnhighlightT->getUnhighlightGC != রিক্ত))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	অন্যথা
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#অন্যথা
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	যদি (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    অন্যথা
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    স্থির শূন্য
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    পূর্ণাঙ্ক		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    পূর্ণাঙ্ক		x;
    পূর্ণাঙ্ক		y;

    যদি (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    অন্যথা
    {
	যদি (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	অন্যথা যদি (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	অন্যথা
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    যদি (pix == None || !eb->enhancedbutton.pixmap_data)
	প্রত্যাগম;

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
    যদি (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    যদি (h < height)
	height = h;
    যদি (depth == (পূর্ণাঙ্ক)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    অন্যথা যদি (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    স্থির শূন্য
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = রিক্ত;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#অন্যথা
    Boolean	etched_in = False;
#endif

    যদি (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	যদি (eb->label.label_type == (পূর্ণাঙ্ক)XmSTRING
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

    যদি (eb->label.label_type == (পূর্ণাঙ্ক)XmPIXMAP)
    {
	যদি (eb->pushbutton.armed)
	{
	    যদি (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    অন্যথা
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	অন্যথা
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    যদি (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_বাম(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ডান(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_শীর্ষ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    যদি (deadjusted)
    {
	Lab_Margin_বাম(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ডান(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_শীর্ষ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    যদি (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    স্থির শূন্য
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *অঙ্ক_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ঠেলো_Button_Callback_Struct call_মান;

    যদি (Lab_IsMenupane(eb))
    {
	যদি ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#অন্যথা
	    Boolean etched_in = False;
#endif

	    যদি (eb->pushbutton.armed)
		প্রত্যাগম;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    যদি (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, রিক্ত);
		draw_pixmap(eb, event, রিক্ত);
	    }

	    যদি ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.শীর্ষ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    যদি (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_মান.reason = (পূর্ণাঙ্ক)XmCR_ARM;
		call_মান.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_মান);
	    }
	}
    }
    অন্যথা
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, রিক্ত, রিক্ত);
	যদি (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) রিক্ত);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, রিক্ত);
    }
}

    স্থির শূন্য
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *অঙ্ক_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ঠেলো_Button_Callback_Struct call_মান;

    যদি (Lab_IsMenupane(eb))
    {
#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#অন্যথা
	Boolean etched_in = False;
#endif

	যদি (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = রিক্ত;

	    যদি (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, রিক্ত);
		draw_pixmap(eb, event, রিক্ত);
	    }
	    অন্যথা
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    যদি (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_মান.reason = (পূর্ণাঙ্ক)XmCR_DISARM;
		call_মান.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_মান);
	    }
	}
    }
    অন্যথা
    {
	_XmPrimitiveLeave((Widget) eb, event, রিক্ত, রিক্ত);

	যদি (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)রিক্ত);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, রিক্ত);
	    eb->pushbutton.armed = TRUE;
	}
	অন্যথা
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, রিক্ত);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    স্থির শূন্য
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned পূর্ণাঙ্ক w = 0;
    unsigned পূর্ণাঙ্ক h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    যদি (newtb->enhancedbutton.pixmap_data
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
    যদি (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_বাম = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    অন্যথা
    {
	newtb->label.margin_বাম = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	যদি (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_বাম += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    যদি (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    অন্যথা
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#যদি 0
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

    স্থির শূন্য
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    যদি (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	মূল_W = Default_মূল_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, মূল_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    যদি (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_মানসমূহ((Widget) eb, XmNlabelString, str, রিক্ত);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    যদি (request->core.width == 0)
	eb->core.width = 0;
    যদি (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    স্থির শূন্য
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap উপরের_pix = eb->enhancedbutton.highlight_pixmap;

    যদি (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    যদি (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    যদি (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    যদি (উপরের_pix != None && উপরের_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), উপরের_pix);
}

    স্থির শূন্য
Destroy(Widget w)
{
    যদি (!XmIsEnhancedButton(w))
	প্রত্যাগম;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    স্থির Boolean
Set_মানসমূহ(Widget current,
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
    যদি (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	যদি (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক		    r_x;
	    পূর্ণাঙ্ক		    r_y;
	    unsigned পূর্ণাঙ্ক    r_height;
	    unsigned পূর্ণাঙ্ক    r_width;
	    unsigned পূর্ণাঙ্ক    r_border;
	    unsigned পূর্ণাঙ্ক    r_depth;
	    পূর্ণাঙ্ক		    মূল_x;
	    পূর্ণাঙ্ক		    মূল_y;
	    পূর্ণাঙ্ক		    win_x;
	    পূর্ণাঙ্ক		    win_y;
	    Window	    মূল;
	    Window	    মূল_q;
	    Window	    child;
	    unsigned পূর্ণাঙ্ক    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &মূল, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &মূল_q, &child,
			  &মূল_x, &মূল_y, &win_x, &win_y, &mask);

	    যদি (মূল == মূল_q)
	    {
		যদি ((win_x < 0) || (win_y < 0))
		    প্রত্যাগম False;

		যদি ((win_x > (পূর্ণাঙ্ক)r_width) || (win_y > (পূর্ণাঙ্ক)r_height))
		    প্রত্যাগম False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	প্রত্যাগম True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    যদি (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    যদি (NOT_EQUAL(primitive.foreground))
	redraw = True;
    যদি (NOT_EQUAL(core.background_pixel))
	redraw = True;
    যদি (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    যদি (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    যদি (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    যদি (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    যদি (redraw == True)
    {
	যদি (change)
	    set_pixmap(eb);
	যদি (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	অন্যথা
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	যদি (change)
	    set_size(eb);
	redraw = False;
    }

    প্রত্যাগম redraw;
}

    স্থির শূন্য
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    পূর্ণাঙ্ক			    dx;
    পূর্ণাঙ্ক			    adjust;
    short		    fill = 0;

    যদি (!XtIsRealized((Widget)eb))
	প্রত্যাগম;

#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    যদি ((eb->pushbutton.arm_color == eb->primitive.শীর্ষ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    যদি (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    অন্যথা
	adjust = eb->pushbutton.default_button_shadow_thickness;

    যদি (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    অন্যথা
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
    যদি (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	যদি (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		যদি (eb->label.label_type == (পূর্ণাঙ্ক)XmPIXMAP)
		{
		    যদি (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    অন্যথা
			gc = eb->pushbutton.background_gc;
		}
		অন্যথা
		    gc = eb->pushbutton.fill_gc;
	}
	অন্যথা
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	যদি (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    যদি (Lab_IsMenupane(eb))
    {
	যদি (eb->pushbutton.armed)
	    (*(((Xm_ঠেলো_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    অন্যথা
    {
	adjust = 0;

#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	বিকল্প (default_button_emphasis)
	{
	    পক্ষ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ভঙ্গ;

	    পক্ষ XmINTERNAL_HIGHLIGHT:
		ভঙ্গ;

	    স্বভাব:
		প্রতিজ্ঞা(FALSE);
		প্রত্যাগম;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	যদি (box.x > adjust)
	{
	    পূর্ণাঙ্ক borderwidth =box.x - adjust;
	    পূর্ণাঙ্ক rectwidth = eb->core.width - 2 * adjust;
	    পূর্ণাঙ্ক rectheight = eb->core.height - 2 * adjust;

	    যদি (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    অন্যথা
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    বিকল্প (default_button_emphasis)
	    {
		পক্ষ XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    যদি (eb->primitive.highlight_drawn)
			(*(((Xm_ঠেলো_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ভঙ্গ;

		স্বভাব:
		    ভঙ্গ;
	    }
#endif
	}

	যদি (eb->pushbutton.default_button_shadow_thickness)
	{
	    যদি (eb->pushbutton.show_as_default)
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
		GC	    শীর্ষ_gc;
		GC	    bottom_gc;
		পূর্ণাঙ্ক	    default_button_shadow_thickness;
		পূর্ণাঙ্ক	    x, y, width, height, delta;
		Widget	    parent;

		যদি (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    প্রত্যাগম;

		যদি (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    প্রত্যাগম;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		যদি (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_শীর্ষ_Shadow_GC(eb);
		    শীর্ষ_gc = XmParentBottomShadowGC(eb);
		}
		অন্যথা
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.শীর্ষ_shadow_GC;
		    শীর্ষ_gc = eb->primitive.bottom_shadow_GC;
		}

		যদি ((bottom_gc == None) || (শীর্ষ_gc == None))
		    প্রত্যাগম;


		যদি (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		অন্যথা
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#যদি !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		বিকল্প (default_button_emphasis)
		{
		    পক্ষ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ভঙ্গ;

		    পক্ষ XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ভঙ্গ;

		    স্বভাব:
			প্রতিজ্ঞা(FALSE);
			প্রত্যাগম;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		যদি ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    শীর্ষ_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	যদি (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    স্থির শূন্য
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ঠেলো_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, রিক্ত, রিক্ত);
}

    স্থির শূন্য
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ঠেলো_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, রিক্ত, রিক্ত);
}

#endif // FEAT_TOOLBAR
