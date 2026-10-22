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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_TOOLBAR

#அந்தர்பாவ <Xm/XmP.h>
#அந்தர்பாவ <Xm/DrawP.h>
#எனில் defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#இல்லையெனில்
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  இல்லையெனில்
#   include <X11/xpm.h>
#  endif
# endif
#endif
#அந்தர்பாவ <Xm/ManagerP.h>
#அந்தர்பாவ <Xm/Display.h>
#அந்தர்பாவ <Xm/DisplayP.h>

#அந்தர்பாவ <X11/Shell.h>
#அந்தர்பாவ <X11/ShellP.h>

#அந்தர்பாவ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#எனில் defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (முழு)XmMENU_POPUP || \
		    Lab_MenuType(w) == (முழு)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, வரி_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#எனில் defined(VMS)
# எனில் defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# இல்லையெனில்
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#இல்லையெனில்
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

வெளி Boolean _XmGetInDragMode(Widget widget);
வெளி வெறுமை _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * இலக்கம்_params);
வெளி வெறுமை _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * இலக்கம்_params);
வெளி வெறுமை _XmSetFocusFlag(Widget w, unsigned முழு mask, Boolean மதிப்பு);
வெளி வெறுமை _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
நிலையான வெறுமை Destroy(Widget w);
நிலையான வெறுமை Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
நிலையான Boolean Set_மதிப்புகள்(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
நிலையான வெறுமை Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
நிலையான வெறுமை Enter(Widget, XEvent *, String *, Cardinal *);
நிலையான வெறுமை Leave(Widget, XEvent *, String *, Cardinal *);
நிலையான வெறுமை BorderHighlight(Widget);
நிலையான வெறுமை BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
நிலையான எழுத்து stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
நிலையான XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

நிலையான XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, அளவு(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) வெற்று
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, அளவு(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) வெற்று
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, அளவு(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, அளவு(முழு),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_வலது
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ வெற்று,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ அளவு(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ வெற்று
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_தள்ளு_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ அளவு(XmEnhancedButtonRec),
	/* class_initialize	 */ வெற்று,
	/* class_part_initialize */ வெற்று,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ வெற்று,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_எண்(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_எண்(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_மதிப்புகள்,
	/* set_values_hook	 */ வெற்று,
	/* set_values_almost	 */ Xt_Inherit_Set_மதிப்புகள்_Almost,
	/* get_values_hook	 */ வெற்று,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ வெற்று,
	/* tm_table		 */ வெற்று,
	/* query_geometry	 */ வெற்று,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ வெற்று
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ வெற்று,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ வெற்று,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) வெற்று,
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
    நிலையான unsigned short
bump_color(unsigned short மதிப்பு)
{
    முழு tmp = 2 * (((முழு) மதிப்பு - 65535) / 3) + 65535;

    திரும்பு tmp;
}

    நிலையான முழு
alloc_color(Display	*display,
	Colormap	colormap,
	எழுத்து		*colorname,
	XColor		*xcolor,
	வெறுமை		*closure UNUSED)
{
    முழு status;

    எனில் (colorname)
	எனில் (!XParseColor(display, colormap, colorname, xcolor))
	    திரும்பு -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    திரும்பு status != 0 ? 1 : 0;
}

// XPM
நிலையான எழுத்து * blank_xpm[] =
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
    நிலையான வெறுமை
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    வேர்;
    நிலையான Xpm_Color_குறியீடு color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", வெற்று, 0},
	{"foreground", வெற்று, 0},
	{"bottomShadowColor", வெற்று, 0},
	{"topShadowColor", வெற்று, 0},
	{"highlightColor", வெற்று, 0},
	{"armColor", வெற்று, 0}
    };
    முழு		    scr;
    Display	    *dpy = XtDisplay(eb);
    முழு		    x;
    முழு		    y;
    unsigned முழு    height, width, border, depth;
    முழு		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    உயர்வு_pix = None;
    எழுத்து	    **data = (எழுத்து **) eb->enhancedbutton.pixmap_data;
    எழுத்து	    *fname = (எழுத்து *) eb->enhancedbutton.pixmap_file;
    முழு		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    எனில் (!data)
	திரும்பு;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, வெற்று);

    scr = DefaultScreen(dpy);
    வேர் = வேர்_Window(dpy, scr);

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
	    &eb->primitive.உச்சி_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.உச்சி_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_எண்(color);

    எனில் (fname)
	status = XpmReadFileToPixmap(dpy, வேர், fname, &pix, &mask, &attr);
    எனில் (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, வேர், data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    எனில் (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, வேர், blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &வேர், &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    எனில் (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, வேர், width + shift, height + shift, depth);

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
#இல்லையெனில்
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_எண்(color);

    status = XpmCreatePixmapFromData(dpy, வேர், data, &pix, வெற்று, &attr);
    XpmFreeAttributes(&attr);

    உயர்வு_pix = XCreatePixmap(dpy, வேர், width + shift, height + shift, depth);

#எனில் 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#இல்லையெனில்
    XSetForeground(dpy, gc, eb->primitive.உச்சி_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, உயர்வு_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, உயர்வு_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    எனில் (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    இல்லையெனில்
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_சாவி;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_சாவி = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, வேர், data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, வேர், width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.உச்சி_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = உயர்வு_pix;
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

    நிலையான வெறுமை
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		உச்சி_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    எனில் (!eb->primitive.shadow_thickness)
	திரும்பு;

    எனில் ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	திரும்பு;

#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#இல்லையெனில்
    etched_in = False;
#endif
    எனில் (!etched_in ^ eb->pushbutton.armed)
    {
	உச்சி_gc = eb->primitive.உச்சி_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    இல்லையெனில்
    {
	உச்சி_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.உச்சி_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    உச்சி_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    நிலையான வெறுமை
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    எனில் (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	திரும்பு;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    நிலையான வெறுமை
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    எனில் (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	திரும்பு;

    எனில் (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	எனில் (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != வெற்று) && (UnhighlightT->getUnhighlightGC != வெற்று))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	இல்லையெனில்
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#இல்லையெனில்
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	எனில் (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    இல்லையெனில்
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    நிலையான வெறுமை
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    முழு		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    முழு		x;
    முழு		y;

    எனில் (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    இல்லையெனில்
    {
	எனில் (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	இல்லையெனில் எனில் (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	இல்லையெனில்
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    எனில் (pix == None || !eb->enhancedbutton.pixmap_data)
	திரும்பு;

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
    எனில் (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    எனில் (h < height)
	height = h;
    எனில் (depth == (முழு)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    இல்லையெனில் எனில் (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    நிலையான வெறுமை
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = வெற்று;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#இல்லையெனில்
    Boolean	etched_in = False;
#endif

    எனில் (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	எனில் (eb->label.label_type == (முழு)XmSTRING
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

    எனில் (eb->label.label_type == (முழு)XmPIXMAP)
    {
	எனில் (eb->pushbutton.armed)
	{
	    எனில் (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    இல்லையெனில்
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	இல்லையெனில்
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    எனில் (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_இடது(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_வலது(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_உச்சி(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    எனில் (deadjusted)
    {
	Lab_Margin_இடது(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_வலது(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_உச்சி(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    எனில் (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    நிலையான வெறுமை
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *இலக்கம்_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_தள்ளு_Button_Callback_Struct call_மதிப்பு;

    எனில் (Lab_IsMenupane(eb))
    {
	எனில் ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#இல்லையெனில்
	    Boolean etched_in = False;
#endif

	    எனில் (eb->pushbutton.armed)
		திரும்பு;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    எனில் (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, வெற்று);
		draw_pixmap(eb, event, வெற்று);
	    }

	    எனில் ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.உச்சி_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    எனில் (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_மதிப்பு.reason = (முழு)XmCR_ARM;
		call_மதிப்பு.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_மதிப்பு);
	    }
	}
    }
    இல்லையெனில்
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, வெற்று, வெற்று);
	எனில் (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) வெற்று);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, வெற்று);
    }
}

    நிலையான வெறுமை
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *இலக்கம்_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_தள்ளு_Button_Callback_Struct call_மதிப்பு;

    எனில் (Lab_IsMenupane(eb))
    {
#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#இல்லையெனில்
	Boolean etched_in = False;
#endif

	எனில் (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = வெற்று;

	    எனில் (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, வெற்று);
		draw_pixmap(eb, event, வெற்று);
	    }
	    இல்லையெனில்
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    எனில் (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_மதிப்பு.reason = (முழு)XmCR_DISARM;
		call_மதிப்பு.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_மதிப்பு);
	    }
	}
    }
    இல்லையெனில்
    {
	_XmPrimitiveLeave((Widget) eb, event, வெற்று, வெற்று);

	எனில் (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)வெற்று);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, வெற்று);
	    eb->pushbutton.armed = TRUE;
	}
	இல்லையெனில்
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, வெற்று);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    நிலையான வெறுமை
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned முழு w = 0;
    unsigned முழு h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    எனில் (newtb->enhancedbutton.pixmap_data
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
    எனில் (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_இடது = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    இல்லையெனில்
    {
	newtb->label.margin_இடது = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	எனில் (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_இடது += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    எனில் (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    இல்லையெனில்
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#எனில் 0
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

    நிலையான வெறுமை
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    எனில் (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	வேர்_W = Default_வேர்_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, வேர்_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    எனில் (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_மதிப்புகள்((Widget) eb, XmNlabelString, str, வெற்று);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    எனில் (request->core.width == 0)
	eb->core.width = 0;
    எனில் (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    நிலையான வெறுமை
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap உயர்வு_pix = eb->enhancedbutton.highlight_pixmap;

    எனில் (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    எனில் (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    எனில் (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    எனில் (உயர்வு_pix != None && உயர்வு_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), உயர்வு_pix);
}

    நிலையான வெறுமை
Destroy(Widget w)
{
    எனில் (!XmIsEnhancedButton(w))
	திரும்பு;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    நிலையான Boolean
Set_மதிப்புகள்(Widget current,
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
    எனில் (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	எனில் (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	இல்லையெனில்
	{
	    முழு		    r_x;
	    முழு		    r_y;
	    unsigned முழு    r_height;
	    unsigned முழு    r_width;
	    unsigned முழு    r_border;
	    unsigned முழு    r_depth;
	    முழு		    வேர்_x;
	    முழு		    வேர்_y;
	    முழு		    win_x;
	    முழு		    win_y;
	    Window	    வேர்;
	    Window	    வேர்_q;
	    Window	    child;
	    unsigned முழு    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &வேர், &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &வேர்_q, &child,
			  &வேர்_x, &வேர்_y, &win_x, &win_y, &mask);

	    எனில் (வேர் == வேர்_q)
	    {
		எனில் ((win_x < 0) || (win_y < 0))
		    திரும்பு False;

		எனில் ((win_x > (முழு)r_width) || (win_y > (முழு)r_height))
		    திரும்பு False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	திரும்பு True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    எனில் (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    எனில் (NOT_EQUAL(primitive.foreground))
	redraw = True;
    எனில் (NOT_EQUAL(core.background_pixel))
	redraw = True;
    எனில் (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    எனில் (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    எனில் (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    எனில் (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    எனில் (redraw == True)
    {
	எனில் (change)
	    set_pixmap(eb);
	எனில் (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	இல்லையெனில்
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	எனில் (change)
	    set_size(eb);
	redraw = False;
    }

    திரும்பு redraw;
}

    நிலையான வெறுமை
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    முழு			    dx;
    முழு			    adjust;
    short		    fill = 0;

    எனில் (!XtIsRealized((Widget)eb))
	திரும்பு;

#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    எனில் ((eb->pushbutton.arm_color == eb->primitive.உச்சி_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    எனில் (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    இல்லையெனில்
	adjust = eb->pushbutton.default_button_shadow_thickness;

    எனில் (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    இல்லையெனில்
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
    எனில் (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	எனில் (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		எனில் (eb->label.label_type == (முழு)XmPIXMAP)
		{
		    எனில் (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    இல்லையெனில்
			gc = eb->pushbutton.background_gc;
		}
		இல்லையெனில்
		    gc = eb->pushbutton.fill_gc;
	}
	இல்லையெனில்
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	எனில் (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    எனில் (Lab_IsMenupane(eb))
    {
	எனில் (eb->pushbutton.armed)
	    (*(((Xm_தள்ளு_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    இல்லையெனில்
    {
	adjust = 0;

#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	தேர்வு (default_button_emphasis)
	{
	    நிலை XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		நிறுத்து;

	    நிலை XmINTERNAL_HIGHLIGHT:
		நிறுத்து;

	    இயல்பு:
		ப்ரதிஜ்ஞா(FALSE);
		திரும்பு;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	எனில் (box.x > adjust)
	{
	    முழு borderwidth =box.x - adjust;
	    முழு rectwidth = eb->core.width - 2 * adjust;
	    முழு rectheight = eb->core.height - 2 * adjust;

	    எனில் (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    இல்லையெனில்
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    தேர்வு (default_button_emphasis)
	    {
		நிலை XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    எனில் (eb->primitive.highlight_drawn)
			(*(((Xm_தள்ளு_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    நிறுத்து;

		இயல்பு:
		    நிறுத்து;
	    }
#endif
	}

	எனில் (eb->pushbutton.default_button_shadow_thickness)
	{
	    எனில் (eb->pushbutton.show_as_default)
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
		GC	    உச்சி_gc;
		GC	    bottom_gc;
		முழு	    default_button_shadow_thickness;
		முழு	    x, y, width, height, delta;
		Widget	    parent;

		எனில் (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    திரும்பு;

		எனில் (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    திரும்பு;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		எனில் (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_உச்சி_Shadow_GC(eb);
		    உச்சி_gc = XmParentBottomShadowGC(eb);
		}
		இல்லையெனில்
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.உச்சி_shadow_GC;
		    உச்சி_gc = eb->primitive.bottom_shadow_GC;
		}

		எனில் ((bottom_gc == None) || (உச்சி_gc == None))
		    திரும்பு;


		எனில் (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		இல்லையெனில்
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#எனில் !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		தேர்வு (default_button_emphasis)
		{
		    நிலை XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			நிறுத்து;

		    நிலை XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			நிறுத்து;

		    இயல்பு:
			ப்ரதிஜ்ஞா(FALSE);
			திரும்பு;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		எனில் ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    உச்சி_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	எனில் (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    நிலையான வெறுமை
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_தள்ளு_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, வெற்று, வெற்று);
}

    நிலையான வெறுமை
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_தள்ளு_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, வெற்று, வெற்று);
}

#endif // FEAT_TOOLBAR
