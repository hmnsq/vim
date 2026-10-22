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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_TOOLBAR

#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>
#ಅನ್ತರ್ಭಾವ <Xm/DrawP.h>
#ಯದಿ defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#ಅನ್ಯಥಾ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ಅನ್ಯಥಾ
#   include <X11/xpm.h>
#  endif
# endif
#endif
#ಅನ್ತರ್ಭಾವ <Xm/ManagerP.h>
#ಅನ್ತರ್ಭಾವ <Xm/Display.h>
#ಅನ್ತರ್ಭಾವ <Xm/DisplayP.h>

#ಅನ್ತರ್ಭಾವ <X11/Shell.h>
#ಅನ್ತರ್ಭಾವ <X11/ShellP.h>

#ಅನ್ತರ್ಭಾವ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#ಯದಿ defined(LESSTIF_VERSION) || (XmVersion <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (ಪೂರ್ಣಾಙ್ಕ)XmMENU_POPUP || \
		    Lab_MenuType(w) == (ಪೂರ್ಣಾಙ್ಕ)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, ಸಾಲು_Solid)
#endif

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#ಯದಿ defined(VMS)
# ಯದಿ defined(HAVE_XOS_R_H)
#  define XTPROCESS_LOCK XtProcessLock()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# ಅನ್ಯಥಾ
#  define XTPROCESS_LOCK
#  define XTPROCESS_UNLOCK
# endif
#ಅನ್ಯಥಾ
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

ಬಾಹ್ಯ Boolean _XmGetInDragMode(Widget widget);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ಅಂಕೆ_params);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ಅಂಕೆ_params);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmSetFocusFlag(Widget w, unsigned ಪೂರ್ಣಾಙ್ಕ mask, Boolean ಮೌಲ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ಸ್ಥಿರ ಶೂನ್ಯ Destroy(Widget w);
ಸ್ಥಿರ ಶೂನ್ಯ Initialize(Widget rq, Widget eb, ArgList args, Cardinal *n);
ಸ್ಥಿರ Boolean Set_ಮೌಲ್ಯಗಳು(Widget current, Widget request, Widget new, ArgList args, Cardinal *n);
ಸ್ಥಿರ ಶೂನ್ಯ Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ಸ್ಥಿರ ಶೂನ್ಯ Enter(Widget, XEvent *, String *, Cardinal *);
ಸ್ಥಿರ ಶೂನ್ಯ Leave(Widget, XEvent *, String *, Cardinal *);
ಸ್ಥಿರ ಶೂನ್ಯ BorderHighlight(Widget);
ಸ್ಥಿರ ಶೂನ್ಯ BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#define stipple_width  4
#define stipple_height 4
ಸ್ಥಿರ ವರ್ಣ stipple_bits[] = { 0x0a, 0x05, 0x0a, 0x05 };
#define STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ಸ್ಥಿರ XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ಸ್ಥಿರ XtResource resources[] =
{
    {
	XmNpixmapData, XmCPixmap, XmRString, ಪರಿಮಾಣ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_data),
	XmRImmediate, (XtPointer) ರಿಕ್ತ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, ಪರಿಮಾಣ(String),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (XtPointer) ರಿಕ್ತ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, ಪರಿಮಾಣ(Dimension),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (XtPointer) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ),
	XtOffsetOf(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (XtPointer) Xm_ಬಲ
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ರಿಕ್ತ,
    /* record_type	   */ NULLQUARK,
    /* version		   */ XmPrimitiveClassExtVersion,
    /* record_size	   */ ಪರಿಮಾಣ(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ರಿಕ್ತ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ತಳ್ಳು_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ ಪರಿಮಾಣ(XmEnhancedButtonRec),
	/* class_initialize	 */ ರಿಕ್ತ,
	/* class_part_initialize */ ರಿಕ್ತ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ರಿಕ್ತ,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_ಸಂಖ್ಯೆ(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_ಸಂಖ್ಯೆ(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ Set_ಮೌಲ್ಯಗಳು,
	/* set_values_hook	 */ ರಿಕ್ತ,
	/* set_values_almost	 */ Xt_Inherit_Set_ಮೌಲ್ಯಗಳು_Almost,
	/* get_values_hook	 */ ರಿಕ್ತ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ XtVersion,
	/* callback_private	 */ ರಿಕ್ತ,
	/* tm_table		 */ ರಿಕ್ತ,
	/* query_geometry	 */ ರಿಕ್ತ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ರಿಕ್ತ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ರಿಕ್ತ,
	/* number of syn res	 */ 0,
	/* extension		 */ (XtPointer)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ XmInheritSetOverrideCallback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ರಿಕ್ತ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (XtPointer) ರಿಕ್ತ,
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
    ಸ್ಥಿರ unsigned short
bump_color(unsigned short ಮೌಲ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ tmp = 2 * (((ಪೂರ್ಣಾಙ್ಕ) ಮೌಲ್ಯ - 65535) / 3) + 65535;

    ಪ್ರತ್ಯಾಗಮ tmp;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
alloc_color(Display	*display,
	Colormap	colormap,
	ವರ್ಣ		*colorname,
	XColor		*xcolor,
	ಶೂನ್ಯ		*closure UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ status;

    ಯದಿ (colorname)
	ಯದಿ (!XParseColor(display, colormap, colorname, xcolor))
	    ಪ್ರತ್ಯಾಗಮ -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    status = XAllocColor(display, colormap, xcolor);
    ಪ್ರತ್ಯಾಗಮ status != 0 ? 1 : 0;
}

// XPM
ಸ್ಥಿರ ವರ್ಣ * blank_xpm[] =
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
    ಸ್ಥಿರ ಶೂನ್ಯ
set_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    Window	    ಬೇರು;
    ಸ್ಥಿರ Xpm_Color_ಚಿಹ್ನೆ color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ರಿಕ್ತ, 0},
	{"foreground", ರಿಕ್ತ, 0},
	{"bottomShadowColor", ರಿಕ್ತ, 0},
	{"topShadowColor", ರಿಕ್ತ, 0},
	{"highlightColor", ರಿಕ್ತ, 0},
	{"armColor", ರಿಕ್ತ, 0}
    };
    ಪೂರ್ಣಾಙ್ಕ		    scr;
    Display	    *dpy = XtDisplay(eb);
    ಪೂರ್ಣಾಙ್ಕ		    x;
    ಪೂರ್ಣಾಙ್ಕ		    y;
    unsigned ಪೂರ್ಣಾಙ್ಕ    height, width, border, depth;
    ಪೂರ್ಣಾಙ್ಕ		    status = 0;
    Pixmap	    mask;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ಮೇಲಿನ_pix = None;
    ವರ್ಣ	    **data = (ವರ್ಣ **) eb->enhancedbutton.pixmap_data;
    ವರ್ಣ	    *fname = (ವರ್ಣ *) eb->enhancedbutton.pixmap_file;
    ಪೂರ್ಣಾಙ್ಕ		    shift;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    ಯದಿ (!data)
	ಪ್ರತ್ಯಾಗಮ;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ರಿಕ್ತ);

    scr = DefaultScreen(dpy);
    ಬೇರು = ಬೇರು_Window(dpy, scr);

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
	    &eb->primitive.ತುದಿ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.ತುದಿ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ಸಂಖ್ಯೆ(color);

    ಯದಿ (fname)
	status = XpmReadFileToPixmap(dpy, ಬೇರು, fname, &pix, &mask, &attr);
    ಯದಿ (!fname || status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ಬೇರು, data, &pix, &mask, &attr);

    // If something failed, we will fill in the default pixmap.
    ಯದಿ (status != XpmSuccess)
	status = XpmCreatePixmapFromData(dpy, ಬೇರು, blank_xpm, &pix,
								&mask, &attr);

    XpmFreeAttributes(&attr);

    XGetGeometry(dpy, pix, &ಬೇರು, &x, &y, &width, &height, &border, &depth);

    // TODO: does the shift depend on label_location somehow?
    shift = eb->primitive.shadow_thickness / 2;

    ಯದಿ (shift < 1)
	shift = 1;

    sen_pix = XCreatePixmap(dpy, ಬೇರು, width + shift, height + shift, depth);

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
#ಅನ್ಯಥಾ
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#endif
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ಸಂಖ್ಯೆ(color);

    status = XpmCreatePixmapFromData(dpy, ಬೇರು, data, &pix, ರಿಕ್ತ, &attr);
    XpmFreeAttributes(&attr);

    ಮೇಲಿನ_pix = XCreatePixmap(dpy, ಬೇರು, width + shift, height + shift, depth);

#ಯದಿ 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#ಅನ್ಯಥಾ
    XSetForeground(dpy, gc, eb->primitive.ತುದಿ_shadow_color);
#endif
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ಮೇಲಿನ_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ಮೇಲಿನ_pix, gc, 0, 0, width, height, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, width + shift, height + shift, depth);

    ಯದಿ (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    ಅನ್ಯಥಾ
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
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ಕೀಲಿ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_LENGTH(color);
    attr.color_ಕೀಲಿ = XPM_MONO;
    status = XpmCreatePixmapFromData(dpy, ಬೇರು, data, &pix, &mask, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ಬೇರು, width + shift, height + shift, depth);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSetClipMask(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, width + shift, height + shift);
    XSetClipMask(dpy, gc, mask);
    XSetForeground(dpy, gc, eb->primitive.ತುದಿ_shadow_color);
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
    eb->enhancedbutton.highlight_pixmap = ಮೇಲಿನ_pix;
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

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		ತುದಿ_gc;
    GC		bottom_gc;
    Boolean	etched_in;

    ಯದಿ (!eb->primitive.shadow_thickness)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((eb->core.width <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.height <= 2 * eb->primitive.highlight_thickness))
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_in = dpy->display.enable_etched_in_menu;
    }
#ಅನ್ಯಥಾ
    etched_in = False;
#endif
    ಯದಿ (!etched_in ^ eb->pushbutton.armed)
    {
	ತುದಿ_gc = eb->primitive.ತುದಿ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    ಅನ್ಯಥಾ
    {
	ತುದಿ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.ತುದಿ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
	    ತುದಿ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.width - 2 * eb->primitive.highlight_thickness,
	    eb->core.height - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    ಯದಿ (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ಪ್ರತ್ಯಾಗಮ;

    XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    XtWidth(eb), XtHeight(eb),
	    eb->primitive.highlight_thickness);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    ಯದಿ (!XtWidth(eb) || !XtHeight(eb) || !eb->primitive.highlight_thickness)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (XmIsManager(eb->core.parent))
    {
#ifdef UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	ಯದಿ (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((XtPointer)
			    XtClass(eb->core.parent), XmQTspecifyUnhighlight))
		    != ರಿಕ್ತ) && (UnhighlightT->getUnhighlightGC != ರಿಕ್ತ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.parent, (Widget) eb);
	}
	ಅನ್ಯಥಾ
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
	}
#ಅನ್ಯಥಾ
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.parent))->manager.background_GC;
#endif
	XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
			 manager_background_GC,
			 0, 0, XtWidth(eb), XtHeight(eb),
			 eb->primitive.highlight_thickness);
	ಯದಿ (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), XtWindow(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.width - 2 * eb->primitive.highlight_thickness,
		    eb->core.height - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    ಅನ್ಯಥಾ
	XmeClearBorder(XtDisplay(eb), XtWindow(eb), 0, 0, XtWidth(eb),
		       XtHeight(eb), eb->primitive.highlight_thickness);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event UNUSED,
	    Region region UNUSED)
{
    Pixmap	pix;
    GC		gc = eb->label.normal_GC;
    ಪೂರ್ಣಾಙ್ಕ		depth;
    Cardinal	width;
    Cardinal	height;
    Cardinal	w;
    Cardinal	h;
    ಪೂರ್ಣಾಙ್ಕ		x;
    ಪೂರ್ಣಾಙ್ಕ		y;

    ಯದಿ (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    ಅನ್ಯಥಾ
    {
	ಯದಿ (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	ಅನ್ಯಥಾ ಯದಿ (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	ಅನ್ಯಥಾ
	    pix = eb->enhancedbutton.normal_pixmap;
    }

    ಯದಿ (pix == None || !eb->enhancedbutton.pixmap_data)
	ಪ್ರತ್ಯಾಗಮ;

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
    ಯದಿ (w < width)
	width = w;
    height = eb->core.height - 2 * y;
    ಯದಿ (h < height)
	height = h;
    ಯದಿ (depth == (ಪೂರ್ಣಾಙ್ಕ)eb->core.depth)
	XCopyArea(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y);
    ಅನ್ಯಥಾ ಯದಿ (depth == 1)
	XCopyPlane(XtDisplay(eb), pix, XtWindow(eb), gc, 0, 0,
		width, height, x, y, (unsigned long)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ರಿಕ್ತ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_in = dpy->display.enable_etched_in_menu;
#ಅನ್ಯಥಾ
    Boolean	etched_in = False;
#endif

    ಯದಿ (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_in)))
    {
	ಯದಿ (eb->label.label_type == (ಪೂರ್ಣಾಙ್ಕ)XmSTRING
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

    ಯದಿ (eb->label.label_type == (ಪೂರ್ಣಾಙ್ಕ)XmPIXMAP)
    {
	ಯದಿ (eb->pushbutton.armed)
	{
	    ಯದಿ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    ಅನ್ಯಥಾ
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	ಅನ್ಯಥಾ
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    ಯದಿ (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ಎಡ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ಬಲ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ತುದಿ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_LOCK;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    ಯದಿ (deadjusted)
    {
	Lab_Margin_ಎಡ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ಬಲ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ತುದಿ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    ಯದಿ (replaceGC)
	eb->label.normal_GC = tmp_gc;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Enter(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *ಅಂಕೆ_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ತಳ್ಳು_Button_Callback_Struct call_ಮೌಲ್ಯ;

    ಯದಿ (Lab_IsMenupane(eb))
    {
	ಯದಿ ((((ShellWidget) XtParent(XtParent(eb)))->shell.popped_up)
		&& _XmGetInDragMode((Widget) eb))
	{
#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_in = dpy->display.enable_etched_in_menu;
#ಅನ್ಯಥಾ
	    Boolean etched_in = False;
#endif

	    ಯದಿ (eb->pushbutton.armed)
		ಪ್ರತ್ಯಾಗಮ;

	    // ...so KHelp event is delivered correctly.
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, TRUE);
	    XtSetKeyboardFocus(XtParent(XtParent(eb)), (Widget) eb);
	    _XmSetFocusFlag(XtParent(XtParent(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = wid;

	    // etched in menu button
	    ಯದಿ (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ರಿಕ್ತ);
		draw_pixmap(eb, event, ರಿಕ್ತ);
	    }

	    ಯದಿ ((eb->core.width > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.height >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			eb->primitive.ತುದಿ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.width - 2 * eb->primitive.highlight_thickness,
			eb->core.height - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(unsigned)(etched_in ? XmSHADOW_IN : XmSHADOW_OUT));
	    }

	    ಯದಿ (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ಮೌಲ್ಯ.reason = (ಪೂರ್ಣಾಙ್ಕ)XmCR_ARM;
		call_ಮೌಲ್ಯ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_ಮೌಲ್ಯ);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ರಿಕ್ತ, ರಿಕ್ತ);
	ಯದಿ (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ರಿಕ್ತ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ರಿಕ್ತ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Leave(Widget wid,
    XEvent *event,
    String *params UNUSED,
    Cardinal *ಅಂಕೆ_params UNUSED)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ತಳ್ಳು_Button_Callback_Struct call_ಮೌಲ್ಯ;

    ಯದಿ (Lab_IsMenupane(eb))
    {
#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_in = dpy->display.enable_etched_in_menu;
#ಅನ್ಯಥಾ
	Boolean etched_in = False;
#endif

	ಯದಿ (_XmGetInDragMode((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.mode == NotifyNormal))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) XtParent(wid))->manager.active_child = ರಿಕ್ತ;

	    ಯದಿ (etched_in && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), XtWindow(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.width, eb->core.height);
		draw_label(eb, event, ರಿಕ್ತ);
		draw_pixmap(eb, event, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
		XmeClearBorder
		    (XtDisplay(eb), XtWindow(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.width -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.height -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    ಯದಿ (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ಮೌಲ್ಯ.reason = (ಪೂರ್ಣಾಙ್ಕ)XmCR_DISARM;
		call_ಮೌಲ್ಯ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_ಮೌಲ್ಯ);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	_XmPrimitiveLeave((Widget) eb, event, ರಿಕ್ತ, ರಿಕ್ತ);

	ಯದಿ (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_LOCK;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ರಿಕ್ತ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ರಿಕ್ತ);
	    eb->pushbutton.armed = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ರಿಕ್ತ);
	}
    }
}

#define IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ಸ್ಥಿರ ಶೂನ್ಯ
set_size(XmEnhancedButtonWidget newtb)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ w = 0;
    unsigned ಪೂರ್ಣಾಙ್ಕ h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    ಯದಿ (newtb->enhancedbutton.pixmap_data
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
    ಯದಿ (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ಎಡ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_width;
    }
    ಅನ್ಯಥಾ
    {
	newtb->label.margin_ಎಡ = w;
	newtb->core.width = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_width)
	    + newtb->label.TextRect.width;

	ಯದಿ (newtb->label.TextRect.width > 0)
	{
	    newtb->label.margin_ಎಡ += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	    newtb->core.width += newtb->label.margin_width
					  + newtb->primitive.shadow_thickness;
	}
    }
    ಯದಿ (newtb->label.TextRect.height < h)
    {
	newtb->core.height = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_height);
    }
    ಅನ್ಯಥಾ
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#ಯದಿ 0
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

    ಸ್ಥಿರ ಶೂನ್ಯ
Initialize(Widget rq, Widget ebw, ArgList args UNUSED, Cardinal *n UNUSED)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_LOCK;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    ಯದಿ (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	Window	ಬೇರು_W = Default_ಬೇರು_Window(dpy);

	STIPPLE_BITMAP = XCreateBitmapFromData(dpy, ಬೇರು_W, stipple_bits,
		stipple_width, stipple_height);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    ಯದಿ (eb->enhancedbutton.pixmap_data)
    {
	XmString str;
	set_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_Set_ಮೌಲ್ಯಗಳು((Widget) eb, XmNlabelString, str, ರಿಕ್ತ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;

    ಯದಿ (request->core.width == 0)
	eb->core.width = 0;
    ಯದಿ (request->core.height == 0)
	eb->core.height = 0;
    set_size(eb);

    (* resize)((Widget)eb);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.normal_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ಮೇಲಿನ_pix = eb->enhancedbutton.highlight_pixmap;

    ಯದಿ (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    ಯದಿ (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    ಯದಿ (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    ಯದಿ (ಮೇಲಿನ_pix != None && ಮೇಲಿನ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ಮೇಲಿನ_pix);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Destroy(Widget w)
{
    ಯದಿ (!XmIsEnhancedButton(w))
	ಪ್ರತ್ಯಾಗಮ;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ಸ್ಥಿರ Boolean
Set_ಮೌಲ್ಯಗಳು(Widget current,
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
    ಯದಿ (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	ಯದಿ (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ		    r_x;
	    ಪೂರ್ಣಾಙ್ಕ		    r_y;
	    unsigned ಪೂರ್ಣಾಙ್ಕ    r_height;
	    unsigned ಪೂರ್ಣಾಙ್ಕ    r_width;
	    unsigned ಪೂರ್ಣಾಙ್ಕ    r_border;
	    unsigned ಪೂರ್ಣಾಙ್ಕ    r_depth;
	    ಪೂರ್ಣಾಙ್ಕ		    ಬೇರು_x;
	    ಪೂರ್ಣಾಙ್ಕ		    ಬೇರು_y;
	    ಪೂರ್ಣಾಙ್ಕ		    win_x;
	    ಪೂರ್ಣಾಙ್ಕ		    win_y;
	    Window	    ಬೇರು;
	    Window	    ಬೇರು_q;
	    Window	    child;
	    unsigned ಪೂರ್ಣಾಙ್ಕ    mask;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, XtWindow(cur), &ಬೇರು, &r_x, &r_y, &r_width,
			 &r_height, &r_border, &r_depth);
	    XQueryPointer(XtDisplay(cur), XtWindow(cur), &ಬೇರು_q, &child,
			  &ಬೇರು_x, &ಬೇರು_y, &win_x, &win_y, &mask);

	    ಯದಿ (ಬೇರು == ಬೇರು_q)
	    {
		ಯದಿ ((win_x < 0) || (win_y < 0))
		    ಪ್ರತ್ಯಾಗಮ False;

		ಯದಿ ((win_x > (ಪೂರ್ಣಾಙ್ಕ)r_width) || (win_y > (ಪೂರ್ಣಾಙ್ಕ)r_height))
		    ಪ್ರತ್ಯಾಗಮ False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	ಪ್ರತ್ಯಾಗಮ True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    ಯದಿ (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    ಯದಿ (NOT_EQUAL(primitive.foreground))
	redraw = True;
    ಯದಿ (NOT_EQUAL(core.background_pixel))
	redraw = True;
    ಯದಿ (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    ಯದಿ (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    ಯದಿ (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    ಯದಿ (NOT_EQUAL(label._label))
    {
	redraw = True;
	set_size(eb);
    }

    ಯದಿ (redraw == True)
    {
	ಯದಿ (change)
	    set_pixmap(eb);
	ಯದಿ (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	ಅನ್ಯಥಾ
	    eb->label.pixmap = eb->enhancedbutton.normal_pixmap;
	ಯದಿ (change)
	    set_size(eb);
	redraw = False;
    }

    ಪ್ರತ್ಯಾಗಮ redraw;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#endif
    XRectangle		    box;
    ಪೂರ್ಣಾಙ್ಕ			    dx;
    ಪೂರ್ಣಾಙ್ಕ			    adjust;
    short		    fill = 0;

    ಯದಿ (!XtIsRealized((Widget)eb))
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#endif

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    ಯದಿ ((eb->pushbutton.arm_color == eb->primitive.ತುದಿ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    ಯದಿ (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    ಅನ್ಯಥಾ
	adjust = eb->pushbutton.default_button_shadow_thickness;

    ಯದಿ (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    ಅನ್ಯಥಾ
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
    ಯದಿ (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	ಯದಿ (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		ಯದಿ (eb->label.label_type == (ಪೂರ್ಣಾಙ್ಕ)XmPIXMAP)
		{
		    ಯದಿ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    ಅನ್ಯಥಾ
			gc = eb->pushbutton.background_gc;
		}
		ಅನ್ಯಥಾ
		    gc = eb->pushbutton.fill_gc;
	}
	ಅನ್ಯಥಾ
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	ಯದಿ (gc)
	    XFillRectangle(XtDisplay(eb), XtWindow(eb), gc,
		    box.x, box.y, box.width, box.height);
    }

    draw_label(eb, event, region);

    ಯದಿ (Lab_IsMenupane(eb))
    {
	ಯದಿ (eb->pushbutton.armed)
	    (*(((Xm_ತಳ್ಳು_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    ಅನ್ಯಥಾ
    {
	adjust = 0;

#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	ವಿಕಲ್ಪ (default_button_emphasis)
	{
	    ಪಕ್ಷ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ಭಙ್ಗ;

	    ಪಕ್ಷ XmINTERNAL_HIGHLIGHT:
		ಭಙ್ಗ;

	    ಸ್ವಭಾವ:
		ಪ್ರತಿಜ್ಞಾ(FALSE);
		ಪ್ರತ್ಯಾಗಮ;
	}
#endif

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	ಯದಿ (box.x > adjust)
	{
	    ಪೂರ್ಣಾಙ್ಕ borderwidth =box.x - adjust;
	    ಪೂರ್ಣಾಙ್ಕ rectwidth = eb->core.width - 2 * adjust;
	    ಪೂರ್ಣಾಙ್ಕ rectheight = eb->core.height - 2 * adjust;

	    ಯದಿ (XmIsManager(XtParent(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), XtWindow(eb),
						     XmParentBackgroundGC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    ಅನ್ಯಥಾ
	    {
		XmeClearBorder(XtDisplay(eb), XtWindow(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
	    ವಿಕಲ್ಪ (default_button_emphasis)
	    {
		ಪಕ್ಷ XmINTERNAL_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    ಯದಿ (eb->primitive.highlight_drawn)
			(*(((Xm_ತಳ್ಳು_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ಭಙ್ಗ;

		ಸ್ವಭಾವ:
		    ಭಙ್ಗ;
	    }
#endif
	}

	ಯದಿ (eb->pushbutton.default_button_shadow_thickness)
	{
	    ಯದಿ (eb->pushbutton.show_as_default)
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
		GC	    ತುದಿ_gc;
		GC	    bottom_gc;
		ಪೂರ್ಣಾಙ್ಕ	    default_button_shadow_thickness;
		ಪೂರ್ಣಾಙ್ಕ	    x, y, width, height, delta;
		Widget	    parent;

		ಯದಿ (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    ಪ್ರತ್ಯಾಗಮ;

		ಯದಿ (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    ಪ್ರತ್ಯಾಗಮ;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		parent = XtParent(eb);
		ಯದಿ (XmIsManager(parent))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_Parent_ತುದಿ_Shadow_GC(eb);
		    ತುದಿ_gc = XmParentBottomShadowGC(eb);
		}
		ಅನ್ಯಥಾ
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.ತುದಿ_shadow_GC;
		    ತುದಿ_gc = eb->primitive.bottom_shadow_GC;
		}

		ಯದಿ ((bottom_gc == None) || (ತುದಿ_gc == None))
		    ಪ್ರತ್ಯಾಗಮ;


		ಯದಿ (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		ಅನ್ಯಥಾ
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#ಯದಿ !defined(LESSTIF_VERSION) && (XmVersion > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		ವಿಕಲ್ಪ (default_button_emphasis)
		{
		    ಪಕ್ಷ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ಭಙ್ಗ;

		    ಪಕ್ಷ XmINTERNAL_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ಭಙ್ಗ;

		    ಸ್ವಭಾವ:
			ಪ್ರತಿಜ್ಞಾ(FALSE);
			ಪ್ರತ್ಯಾಗಮ;
		}
#endif

		x = y = delta;
		width = eb->core.width - 2 * delta;
		height = eb->core.height - 2 * delta;

		ಯದಿ ((width > 0) && (height > 0))
		    XmeDrawShadows(XtDisplay(eb), XtWindow(eb),
			    ತುದಿ_gc, bottom_gc, x, y, width, height,
			    default_button_shadow_thickness,
			    (unsigned)XmSHADOW_OUT);
	    }
	}

	ಯದಿ (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ತಳ್ಳು_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ರಿಕ್ತ, ರಿಕ್ತ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ತಳ್ಳು_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ರಿಕ್ತ, ರಿಕ್ತ);
}

#endif // FEAT_TOOLBAR
