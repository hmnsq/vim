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

#include "vim.h"

#ifdef FEAT_TOOLBAR

#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#གལ་སྲིད defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#དེ་མིན
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  དེ་མིན
#   include <X11/xpm.h>
#  endif
# endif
#endif
#include <Xm/ManagerP.h>
#include <Xm/Display.h>
#include <Xm/DisplayP.h>

#include <X11/Shell.h>
#include <X11/ShellP.h>

#include "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#གལ་སྲིད defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (ཧྲིལ)XmMENU_POPUP || \
		    Lab_MenuType(w) == (ཧྲིལ)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, LineSolid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#གལ་སྲིད defined(VMS)
# གལ་སྲིད defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# དེ་མིན
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#དེ་མིན
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

extern Boolean _XmGetInDragMode(Widget widget);
extern void _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * num_params);
extern void _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * num_params);
extern void _XmSetFocusFlag(Widget w, unsigned ཧྲིལ mask, Boolean value);
extern void _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
static void Destroy(Widget w);
static void Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
static Boolean SetValues(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
static void Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
static void Enter(Widget, XEvent *, String *, Cardinal *);
static void Leave(Widget, XEvent *, String *, Cardinal *);
static void BorderHighlight(Widget);
static void BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
static var_na stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
static XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

static XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, parimaa_na(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) NULL
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, parimaa_na(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) NULL
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, parimaa_na(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, parimaa_na(ཧྲིལ),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) XmRIGHT
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ NULL,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ parimaa_na(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ NULL
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xmPushButtonClassRec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ parimaa_na(XmEnhancedButtonRec),
	/* class_initialize	 */ NULL,
	/* class_part_initialize */ NULL,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ NULL,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ XtNumber(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ XtNumber(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ SetValues,
	/* set_values_hook	 */ NULL,
	/* set_values_almost	 */ XtInheritSetValuesAlmost,
	/* get_values_hook	 */ NULL,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ NULL,
	/* tm_table		 */ NULL,
	/* query_geometry	 */ NULL,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ NULL
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ NULL,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ NULL,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) NULL,
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
    static unsigned short
bump_color(unsigned short value)
{
    ཧྲིལ tmp = 2 * (((ཧྲིལ) value - 65535) / 3) + 65535;

    ལོག tmp;
}

    static ཧྲིལ
alloc_color(Display	*display,
	Colormap	colormap,
	var_na		*colorname,
	XColor		*xcolor,
	void		*closure UNUSED)
{
    ཧྲིལ status;

    གལ་སྲིད (colorname)
	གལ་སྲིད (!XParseColor(display, colormap, colorname, xcolor))
	    ལོག -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    ལོག status != 0 ? 1 : 0;
}

// XPM
static var_na * blank_xpm[] =
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
    static void
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    root;
    static XpmColorSymbol color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", NULL, 0},
	{"foreground", NULL, 0},
	{"bottomShadowColor", NULL, 0},
	{"topShadowColor", NULL, 0},
	{"highlightColor", NULL, 0},
	{"armColor", NULL, 0}
    };
    ཧྲིལ		    scr;
    Display	    *dpy = XtDisplay(eb);
    ཧྲིལ		    x;
    ཧྲིལ		    y;
    unsigned ཧྲིལ    height, width, border, depth;
    ཧྲིལ		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    high_pix = None;
    var_na	    **data = (var_na **) eb->enhancedbutton.pixmap_data;
    var_na	    *fname = (var_na *) eb->enhancedbutton.pixmap_file;
    ཧྲིལ		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    གལ་སྲིད (!data)
	ལོག;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, NULL);

    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);

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
	    &eb->primitive.top_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.top_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = XtNumber(color);

    གལ་སྲིད (fname)
	status = XpmReadFileToPixmap(dpy, root, fname, &pix, &mask, &attr);
    གལ་སྲིད (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, root, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    གལ་སྲིད (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, root, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &root, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    གལ་སྲིད (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, root, width + shift, height + shift, depth);

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
#དེ་མིན
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = XtNumber(color);

    status = XpmCreatePixmapFromData(dpy, root, data, &pix, NULL, &attr);
    XpmFreeAttributes(&attr);

    high_pix = XCreatePixmap(dpy, root, width + shift, height + shift, depth);

#གལ་སྲིད 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#དེ་མིན
    XSetForeground(dpy, gc, eb->primitive.top_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, high_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, high_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    གལ་སྲིད (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    དེ་མིན
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmColorKey;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_key = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, root, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, root, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.top_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = high_pix;
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

    static void
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		top_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    གལ་སྲིད (!eb->primitive.shadow_thickness)
	ལོག;

    གལ་སྲིད ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	ལོག;

#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#དེ་མིན
    etched_in = False;
#endif
    གལ་སྲིད (!etched_in ^ eb->pushbutton.armed)
    {
	top_gc = eb->primitive.top_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    དེ་མིན
    {
	top_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.top_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    top_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    static void
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    གལ་སྲིད (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ལོག;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    static void
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    གལ་སྲིད (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ལོག;

    གལ་སྲིད (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	གལ་སྲིད (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != NULL) && (UnhighlightT->getUnhighlightGC != NULL))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	དེ་མིན
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#དེ་མིན
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	གལ་སྲིད (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    དེ་མིན
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    static void
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    ཧྲིལ		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    ཧྲིལ		x;
    ཧྲིལ		y;

    གལ་སྲིད (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    དེ་མིན
    {
	གལ་སྲིད (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	དེ་མིན གལ་སྲིད (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	དེ་མིན
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    གལ་སྲིད (pix == None || !eb->enhancedbutton.pixmap_data)
	ལོག;

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
    གལ་སྲིད (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    གལ་སྲིད (h < height)
	height = h;
    གལ་སྲིད (depth == (ཧྲིལ)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    དེ་མིན གལ་སྲིད (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    static void
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = NULL;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#དེ་མིན
    Boolean	etched_in = False;
#endif

    གལ་སྲིད (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	གལ་སྲིད (eb->label.label_type == (ཧྲིལ)XmSTRING
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

    གལ་སྲིད (eb->label.label_type == (ཧྲིལ)XmPIXMAP)
    {
	གལ་སྲིད (eb->pushbutton.armed)
	{
	    གལ་སྲིད (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    དེ་མིན
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	དེ་མིན
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    གལ་སྲིད (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_MarginLeft(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginRight(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginTop(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    གལ་སྲིད (deadjusted)
    {
	Lab_MarginLeft(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginRight(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginTop(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    གལ་སྲིད (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    static void
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *num_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    XmPushButtonCallbackStruct call_value;

    གལ་སྲིད (Lab_IsMenupane(eb))
    {
	གལ་སྲིད ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#དེ་མིན
	    Boolean etched_in = False;
#endif

	    གལ་སྲིད (eb->pushbutton.armed)
		ལོག;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    གལ་སྲིད (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, NULL);
		draw_pixmap(eb, event, NULL);
	    }

	    གལ་སྲིད ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.top_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    གལ་སྲིད (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_value.reason = (ཧྲིལ)XmCR_ARM;
		call_value.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_value);
	    }
	}
    }
    དེ་མིན
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, NULL, NULL);
	གལ་སྲིད (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) NULL);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, NULL);
    }
}

    static void
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *num_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    XmPushButtonCallbackStruct call_value;

    གལ་སྲིད (Lab_IsMenupane(eb))
    {
#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#དེ་མིན
	Boolean etched_in = False;
#endif

	གལ་སྲིད (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = NULL;

	    གལ་སྲིད (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, NULL);
		draw_pixmap(eb, event, NULL);
	    }
	    དེ་མིན
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    གལ་སྲིད (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_value.reason = (ཧྲིལ)XmCR_DISARM;
		call_value.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_value);
	    }
	}
    }
    དེ་མིན
    {
	_XmPrimitiveLeave((Widget) eb, event, NULL, NULL);

	གལ་སྲིད (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)NULL);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, NULL);
	    eb->pushbutton.armed = TRUE;
	}
	དེ་མིན
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, NULL);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    static void
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned ཧྲིལ w = 0;
    unsigned ཧྲིལ h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    གལ་སྲིད (newtb->enhancedbutton.pixmap_data
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
    གལ་སྲིད (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_left = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    དེ་མིན
    {
	newtb->label.margin_left = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	གལ་སྲིད (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_left += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    གལ་སྲིད (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    དེ་མིན
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#གལ་སྲིད 0
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

    static void
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    གལ་སྲིད (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	rootW = DefaultRootWindow(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, rootW, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    གལ་སྲིད (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	XtVaSetValues((Widget) eb, XmNlabelString, str, NULL);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    གལ་སྲིད (request->core.width == 0)
	eb->core.width = 0;
    གལ་སྲིད (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    static void
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap high_pix = eb->enhancedbutton.highlight_pixmap;

    གལ་སྲིད (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    གལ་སྲིད (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    གལ་སྲིད (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    གལ་སྲིད (high_pix != None && high_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), high_pix);
}

    static void
Destroy(Widget w)
{
    གལ་སྲིད (!XmIsEnhancedButton(w))
	ལོག;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    static Boolean
SetValues(Widget current,
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
    གལ་སྲིད (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	གལ་སྲིད (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	དེ་མིན
	{
	    ཧྲིལ		    r_x;
	    ཧྲིལ		    r_y;
	    unsigned ཧྲིལ    r_height;
	    unsigned ཧྲིལ    r_width;
	    unsigned ཧྲིལ    r_border;
	    unsigned ཧྲིལ    r_depth;
	    ཧྲིལ		    root_x;
	    ཧྲིལ		    root_y;
	    ཧྲིལ		    win_x;
	    ཧྲིལ		    win_y;
	    Window	    root;
	    Window	    root_q;
	    Window	    child;
	    unsigned ཧྲིལ    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &root, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &root_q, &child,
			  &root_x, &root_y, &win_x, &win_y, &mask);

	    གལ་སྲིད (root == root_q)
	    {
		གལ་སྲིད ((win_x < 0) || (win_y < 0))
		    ལོག False;

		གལ་སྲིད ((win_x > (ཧྲིལ)r_width) || (win_y > (ཧྲིལ)r_height))
		    ལོག False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	ལོག True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    གལ་སྲིད (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    གལ་སྲིད (NOT_EQUAL(primitive.foreground))
	redraw = True;
    གལ་སྲིད (NOT_EQUAL(core.background_pixel))
	redraw = True;
    གལ་སྲིད (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    གལ་སྲིད (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    གལ་སྲིད (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    གལ་སྲིད (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    གལ་སྲིད (redraw == True)
    {
	གལ་སྲིད (change)
	    set_pixmap(eb);
	གལ་སྲིད (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	དེ་མིན
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	གལ་སྲིད (change)
	    set_size(eb);
	redraw = False;
    }

    ལོག redraw;
}

    static void
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    ཧྲིལ			    dx;
    ཧྲིལ			    adjust;
    short		    fill = 0;

    གལ་སྲིད (!XtIsRealized((Widget)eb))
	ལོག;

#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    གལ་སྲིད ((eb->pushbutton.arm_color == eb->primitive.top_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    གལ་སྲིད (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    དེ་མིན
	adjust = eb->pushbutton.default_button_shadow_thickness;

    གལ་སྲིད (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    དེ་མིན
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
    གལ་སྲིད (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	གལ་སྲིད (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		གལ་སྲིད (eb->label.label_type == (ཧྲིལ)XmPIXMAP)
		{
		    གལ་སྲིད (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    དེ་མིན
			gc = eb->pushbutton.background_gc;
		}
		དེ་མིན
		    gc = eb->pushbutton.fill_gc;
	}
	དེ་མིན
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	གལ་སྲིད (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    གལ་སྲིད (Lab_IsMenupane(eb))
    {
	གལ་སྲིད (eb->pushbutton.armed)
	    (*(((XmPushButtonWidgetClass)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    དེ་མིན
    {
	adjust = 0;

#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	switch (default_button_emphasis)
	{
	    pak_sa XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		bha_nga;

	    pak_sa XmINTERNAL_HIGHLIGHT:
		bha_nga;

	    default:
		assert(FALSE);
		ལོག;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	གལ་སྲིད (box.x > adjust)
	{
	    ཧྲིལ borderwidth =box.x - adjust;
	    ཧྲིལ rectwidth = eb->core.width - 2 * adjust;
	    ཧྲིལ rectheight = eb->core.height - 2 * adjust;

	    གལ་སྲིད (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    དེ་མིན
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    switch (default_button_emphasis)
	    {
		pak_sa XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    གལ་སྲིད (eb->primitive.highlight_drawn)
			(*(((XmPushButtonWidgetClass) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    bha_nga;

		default:
		    bha_nga;
	    }
#endif
	}

	གལ་སྲིད (eb->pushbutton.default_button_shadow_thickness)
	{
	    གལ་སྲིད (eb->pushbutton.show_as_default)
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
		GC	    top_gc;
		GC	    bottom_gc;
		ཧྲིལ	    default_button_shadow_thickness;
		ཧྲིལ	    x, y, width, height, delta;
		Widget	    parent;

		གལ་སྲིད (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    ལོག;

		གལ་སྲིད (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    ལོག;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		གལ་སྲིད (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = XmParentTopShadowGC(eb);
		    top_gc = XmParentBottomShadowGC(eb);
		}
		དེ་མིན
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.top_shadow_GC;
		    top_gc = eb->primitive.bottom_shadow_GC;
		}

		གལ་སྲིད ((bottom_gc == None) || (top_gc == None))
		    ལོག;


		གལ་སྲིད (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		དེ་མིན
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#གལ་སྲིད !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		switch (default_button_emphasis)
		{
		    pak_sa XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			bha_nga;

		    pak_sa XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			bha_nga;

		    default:
			assert(FALSE);
			ལོག;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		གལ་སྲིད ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    top_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	གལ་སྲིད (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    static void
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xmPushButtonClassRec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, NULL, NULL);
}

    static void
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xmPushButtonClassRec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, NULL, NULL);
}

#endif // FEAT_TOOLBAR
