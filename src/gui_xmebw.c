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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>
#ಅನ್ತರ್ಭಾವ <Xm/DrawP.h>
#ಆದರೆ defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <X11/xpm.h>
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
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
#ಆದರೆ defined(LESSTIF_ಆವೃತ್ತಿ) || (Xm_ಆವೃತ್ತಿ <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (ಪೂರ್ಣಾಂಕ)XmMENU_POPUP || \
		    Lab_MenuType(w) == (ಪೂರ್ಣಾಂಕ)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, ಸಾಲು_Solid)
#ಆದರೆ_ಅಂತ್ಯ

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#ಆದರೆ defined(VMS)
# ಆದರೆ defined(HAVE_XOS_R_H)
#  define XTPROCESS_ಬೀಗ Xt_Process_ಬೀಗ()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# ಇಲ್ಲದಿದ್ದರೆ
#  define XTPROCESS_ಬೀಗ
#  define XTPROCESS_UNLOCK
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# define XTPROCESS_ಬೀಗ Xt_Process_ಬೀಗ()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#ಆದರೆ_ಅಂತ್ಯ

ಬಾಹ್ಯ Boolean Xm_Get_ಒಳ_Drag_ಬಹುಲಕ(Widget widget);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ಅಂಕೆ_params);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ಅಂಕೆ_params);
ಬಾಹ್ಯ ಶೂನ್ಯ Xm_ಗಣ_Focus_Flag(Widget w, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ, Boolean ಮೌಲ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ಸ್ಥಿರ ಶೂನ್ಯ Destroy(Widget w);
ಸ್ಥಿರ ಶೂನ್ಯ Initialize(Widget rq, Widget eb, ArgList ವಾದಗಳು, Cardinal *n);
ಸ್ಥಿರ Boolean ಗಣ_ಮೌಲ್ಯಗಳು(Widget current, Widget request, Widget new, ArgList ವಾದಗಳು, Cardinal *n);
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
#ವ್ಯಾಖ್ಯಾನಿಸು stipple_ಅಗಲ  4
#ವ್ಯಾಖ್ಯಾನಿಸು stipple_ಎತ್ತರ 4
ಸ್ಥಿರ ಅಕ್ಷರ stipple_ಬಿಟ್ಟುಗಳು[] = { 0x0a, 0x05, 0x0a, 0x05 };
#ವ್ಯಾಖ್ಯಾನಿಸು STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

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
	Xm_Npixmap_ದತ್ತಾಂಶ, XmCPixmap, XmRString, ಗಾತ್ರ(String),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(XmEnhancedButtonRec, enhancedbutton.pixmap_ದತ್ತಾಂಶ),
	XmRImmediate, (Xt_ಸೂಚಕ) ಖಾಲಿ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, ಗಾತ್ರ(String),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_ಸೂಚಕ) ಖಾಲಿ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, ಗಾತ್ರ(Dimension),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_ಸೂಚಕ) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ),
	Xt_ಸ್ಥಾನಾಂತರ_ದ(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_ಸೂಚಕ) Xm_ಬಲ
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ಖಾಲಿ,
    /* record_type	   */ NULLQUARK,
    /*ಆವೃತ್ತಿ*/ Xm_Primitive_Class_Ext_ಆವೃತ್ತಿ,
    /* record_size	   */ ಗಾತ್ರ(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ಖಾಲಿ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ತಳ್ಳು_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ ಗಾತ್ರ(XmEnhancedButtonRec),
	/* class_initialize	 */ ಖಾಲಿ,
	/* class_part_initialize */ ಖಾಲಿ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ಖಾಲಿ,
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
	/* set_values		 */ ಗಣ_ಮೌಲ್ಯಗಳು,
	/* set_values_hook	 */ ಖಾಲಿ,
	/* set_values_almost	 */ Xt_Inherit_ಗಣ_ಮೌಲ್ಯಗಳು_Almost,
	/* get_values_hook	 */ ಖಾಲಿ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_ಆವೃತ್ತಿ,
	/* callback_private	 */ ಖಾಲಿ,
	/* tm_table		 */ ಖಾಲಿ,
	/* query_geometry	 */ ಖಾಲಿ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ಖಾಲಿ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ಖಾಲಿ,
	/* number of syn res	 */ 0,
	/* extension		 */ (Xt_ಸೂಚಕ)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_ಗಣ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ಖಾಲಿ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_ಸೂಚಕ) ಖಾಲಿ,
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
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ
bump_color(ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಮೌಲ್ಯ)
{
    ಪೂರ್ಣಾಂಕ tmp = 2 * (((ಪೂರ್ಣಾಂಕ) ಮೌಲ್ಯ - 65535) / 3) + 65535;

    ಹಿಂತಿರುಗಿ tmp;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
alloc_color(Display	*display,
	Colormap	colormap,
	ಅಕ್ಷರ		*colorname,
	XColor		*xcolor,
	ಶೂನ್ಯ		*ಆವರಣ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;

    ಆದರೆ (colorname)
	ಆದರೆ (!XParseColor(display, colormap, colorname, xcolor))
	    ಹಿಂತಿರುಗಿ -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    ಸ್ಥಿತಿ_2 = XAllocColor(display, colormap, xcolor);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2 != 0 ? 1 : 0;
}

// XPM
ಸ್ಥಿರ ಅಕ್ಷರ * blank_xpm[] =
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
ಗಣ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    ಕಿಟಕಿ	    ಬೇರು;
    ಸ್ಥಿರ Xpm_Color_ಚಿಹ್ನೆ color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ಖಾಲಿ, 0},
	{"foreground", ಖಾಲಿ, 0},
	{"bottomShadowColor", ಖಾಲಿ, 0},
	{"topShadowColor", ಖಾಲಿ, 0},
	{"highlightColor", ಖಾಲಿ, 0},
	{"armColor", ಖಾಲಿ, 0}
    };
    ಪೂರ್ಣಾಂಕ		    scr;
    Display	    *dpy = XtDisplay(eb);
    ಪೂರ್ಣಾಂಕ		    x;
    ಪೂರ್ಣಾಂಕ		    y;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    ಎತ್ತರ, ಅಗಲ, border, ಆಳ;
    ಪೂರ್ಣಾಂಕ		    ಸ್ಥಿತಿ_2 = 0;
    Pixmap	    ಮುಖವಾಡ;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ಮೇಲಿನ_pix = None;
    ಅಕ್ಷರ	    **ದತ್ತಾಂಶ = (ಅಕ್ಷರ **) eb->enhancedbutton.pixmap_ದತ್ತಾಂಶ;
    ಅಕ್ಷರ	    *fname = (ಅಕ್ಷರ *) eb->enhancedbutton.pixmap_file;
    ಪೂರ್ಣಾಂಕ		    ಸರಿಸು;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    ಆದರೆ (!ದತ್ತಾಂಶ)
	ಹಿಂತಿರುಗಿ;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ಖಾಲಿ);

    scr = DefaultScreen(dpy);
    ಬೇರು = ಬೇರು_ಕಿಟಕಿ_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_ಆಳ = 0;
    eb->enhancedbutton.pixmap_ಅಗಲ = 0;
    eb->enhancedbutton.pixmap_ಎತ್ತರ = 0;
    eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap = None;
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

    ಆದರೆ (fname)
	ಸ್ಥಿತಿ_2 = Xpm_Read_File_ಗಮ್ಯ_Pixmap(dpy, ಬೇರು, fname, &pix, &ಮುಖವಾಡ, &attr);
    ಆದರೆ (!fname || ಸ್ಥಿತಿ_2 != XpmSuccess)
	ಸ್ಥಿತಿ_2 = Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dpy, ಬೇರು, ದತ್ತಾಂಶ, &pix, &ಮುಖವಾಡ, &attr);

    // If something failed, we will fill in the default pixmap.
    ಆದರೆ (ಸ್ಥಿತಿ_2 != XpmSuccess)
	ಸ್ಥಿತಿ_2 = Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dpy, ಬೇರು, blank_xpm, &pix,
								&ಮುಖವಾಡ, &attr);

    Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು(&attr);

    XGetGeometry(dpy, pix, &ಬೇರು, &x, &y, &ಅಗಲ, &ಎತ್ತರ, &border, &ಆಳ);

    // TODO: does the shift depend on label_location somehow?
    ಸರಿಸು = eb->primitive.shadow_thickness / 2;

    ಆದರೆ (ಸರಿಸು < 1)
	ಸರಿಸು = 1;

    sen_pix = XCreatePixmap(dpy, ಬೇರು, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು, ಆಳ);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, ಮುಖವಾಡ);
    XSetClipOrigin(dpy, gc, ಸರಿಸು, ಸರಿಸು);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, ಅಗಲ, ಎತ್ತರ, ಸರಿಸು, ಸರಿಸು);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#ಇಲ್ಲದಿದ್ದರೆ
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#ಆದರೆ_ಅಂತ್ಯ
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ಸಂಖ್ಯೆ(color);

    ಸ್ಥಿತಿ_2 = Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dpy, ಬೇರು, ದತ್ತಾಂಶ, &pix, ಖಾಲಿ, &attr);
    Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು(&attr);

    ಮೇಲಿನ_pix = XCreatePixmap(dpy, ಬೇರು, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು, ಆಳ);

#ಆದರೆ 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#ಇಲ್ಲದಿದ್ದರೆ
    XSetForeground(dpy, gc, eb->primitive.ತುದಿ_shadow_color);
#ಆದರೆ_ಅಂತ್ಯ
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, None);
    XFillRectangle(dpy, ಮೇಲಿನ_pix, gc, 0, 0, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, ಮುಖವಾಡ);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ಮೇಲಿನ_pix, gc, 0, 0, ಅಗಲ, ಎತ್ತರ, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು, ಆಳ);

    ಆದರೆ (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    ಇಲ್ಲದಿದ್ದರೆ
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, ಸರಿಸು, ಸರಿಸು);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, ಮುಖವಾಡ);
    XSetClipOrigin(dpy, gc, 2 * ಸರಿಸು, 2 * ಸರಿಸು);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, ಅಗಲ, ಎತ್ತರ, 2 * ಸರಿಸು, 2 * ಸರಿಸು);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ಮುಖವಾಡ);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ಕೀಲಿ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_ಉದ್ದಳತೆ(color);
    attr.color_ಕೀಲಿ = XPM_MONO;
    ಸ್ಥಿತಿ_2 = Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dpy, ಬೇರು, ದತ್ತಾಂಶ, &pix, &ಮುಖವಾಡ, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ಬೇರು, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು, ಆಳ);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು);
    XSet_Clip_ಮುಖವಾಡ(dpy, gc, ಮುಖವಾಡ);
    XSetForeground(dpy, gc, eb->primitive.ತುದಿ_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * ಸರಿಸು, 2 * ಸರಿಸು);
    XFillRectangle(dpy, ins_pix, gc, 2 * ಸರಿಸು, 2 * ಸರಿಸು, ಅಗಲ, ಎತ್ತರ);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, ಸರಿಸು, ಸರಿಸು);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ಅಗಲ + ಸರಿಸು, ಎತ್ತರ + ಸರಿಸು);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು(&attr);

    eb->enhancedbutton.pixmap_ಆಳ = ಆಳ;
    eb->enhancedbutton.pixmap_ಅಗಲ = ಅಗಲ;
    eb->enhancedbutton.pixmap_ಎತ್ತರ = ಎತ್ತರ;
    eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = ಮೇಲಿನ_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ಮುಖವಾಡ);
}

#ವ್ಯಾಖ್ಯಾನಿಸು	BUTTON_ಮುಖವಾಡ ( \
	Button1_ಮುಖವಾಡ | Button2_ಮುಖವಾಡ | Button3_ಮುಖವಾಡ | Button4_ಮುಖವಾಡ | Button5_ಮುಖವಾಡ \
)

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		ತುದಿ_gc;
    GC		bottom_gc;
    Boolean	etched_ಒಳ;

    ಆದರೆ (!eb->primitive.shadow_thickness)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((eb->core.ಅಗಲ <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.ಎತ್ತರ <= 2 * eb->primitive.highlight_thickness))
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_ಒಳ = dpy->display.enable_etched_ಒಳ_menu;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    etched_ಒಳ = False;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!etched_ಒಳ ^ eb->pushbutton.armed)
    {
	ತುದಿ_gc = eb->primitive.ತುದಿ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ತುದಿ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.ತುದಿ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
	    ತುದಿ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.ಅಗಲ - 2 * eb->primitive.highlight_thickness,
	    eb->core.ಎತ್ತರ - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (ಚಿಹ್ನೆರಹಿತ)(etched_ಒಳ ? Xm_SHADOW_ಒಳ : Xm_SHADOW_ಹೊರಹೋಗು));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    ಆದರೆ (!Xt_ಅಗಲ(eb) || !Xt_ಎತ್ತರ(eb) || !eb->primitive.highlight_thickness)
	ಹಿಂತಿರುಗಿ;

    XmeDrawHighlight(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_ಅಗಲ(eb), Xt_ಎತ್ತರ(eb),
	    eb->primitive.highlight_thickness);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    ಆದರೆ (!Xt_ಅಗಲ(eb) || !Xt_ಎತ್ತರ(eb) || !eb->primitive.highlight_thickness)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (XmIsManager(eb->core.ಪೋಷಕ))
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	ಆದರೆ (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_ಸೂಚಕ)
			    XtClass(eb->core.ಪೋಷಕ), XmQTspecifyUnhighlight))
		    != ಖಾಲಿ) && (UnhighlightT->getUnhighlightGC != ಖಾಲಿ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.ಪೋಷಕ, (Widget) eb);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.ಪೋಷಕ))->manager.background_GC;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.ಪೋಷಕ))->manager.background_GC;
#ಆದರೆ_ಅಂತ್ಯ
	XmeDrawHighlight(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			 manager_background_GC,
			 0, 0, Xt_ಅಗಲ(eb), Xt_ಎತ್ತರ(eb),
			 eb->primitive.highlight_thickness);
	ಆದರೆ (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.ಅಗಲ - 2 * eb->primitive.highlight_thickness,
		    eb->core.ಎತ್ತರ - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	XmeClearBorder(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb), 0, 0, Xt_ಅಗಲ(eb),
		       Xt_ಎತ್ತರ(eb), eb->primitive.highlight_thickness);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event ಬಳಸದ,
	    Region region ಬಳಸದ)
{
    Pixmap	pix;
    GC		gc = eb->label.ಸಾಮಾನ್ಯ_GC;
    ಪೂರ್ಣಾಂಕ		ಆಳ;
    Cardinal	ಅಗಲ;
    Cardinal	ಎತ್ತರ;
    Cardinal	w;
    Cardinal	h;
    ಪೂರ್ಣಾಂಕ		x;
    ಪೂರ್ಣಾಂಕ		y;

    ಆದರೆ (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	ಇಲ್ಲದಿದ್ದರೆ
	    pix = eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap;
    }

    ಆದರೆ (pix == None || !eb->enhancedbutton.pixmap_ದತ್ತಾಂಶ)
	ಹಿಂತಿರುಗಿ;

    ಆಳ = eb->enhancedbutton.pixmap_ಆಳ;
    w = eb->enhancedbutton.pixmap_ಅಗಲ;
    h = eb->enhancedbutton.pixmap_ಎತ್ತರ;

    gc = eb->label.ಸಾಮಾನ್ಯ_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ಅಗಲ;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ಎತ್ತರ;
    ಅಗಲ = eb->core.ಅಗಲ - 2 * x;
    ಆದರೆ (w < ಅಗಲ)
	ಅಗಲ = w;
    ಎತ್ತರ = eb->core.ಎತ್ತರ - 2 * y;
    ಆದರೆ (h < ಎತ್ತರ)
	ಎತ್ತರ = h;
    ಆದರೆ (ಆಳ == (ಪೂರ್ಣಾಂಕ)eb->core.ಆಳ)
	XCopyArea(XtDisplay(eb), pix, Xt_ಕಿಟಕಿ(eb), gc, 0, 0,
		ಅಗಲ, ಎತ್ತರ, x, y);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆಳ == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_ಕಿಟಕಿ(eb), gc, 0, 0,
		ಅಗಲ, ಎತ್ತರ, x, y, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ಖಾಲಿ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_ಒಳ = dpy->display.enable_etched_ಒಳ_menu;
#ಇಲ್ಲದಿದ್ದರೆ
    Boolean	etched_ಒಳ = False;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_ಒಳ)))
    {
	ಆದರೆ (eb->label.label_type == (ಪೂರ್ಣಾಂಕ)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.ಸಾಮಾನ್ಯ_GC;
	    eb->label.ಸಾಮಾನ್ಯ_GC = eb->pushbutton.background_gc;
	    replaceGC = True;
	}
    }

    /*
     * If the button contains a labeled pixmap, we will take it instead of our
     * own pixmap.
     */

    ಆದರೆ (eb->label.label_type == (ಪೂರ್ಣಾಂಕ)XmPIXMAP)
    {
	ಆದರೆ (eb->pushbutton.armed)
	{
	    ಆದರೆ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    ಇಲ್ಲದಿದ್ದರೆ
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    ಆದರೆ (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ಎಡ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ಬಲ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ತುದಿ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_ಬೀಗ;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    ಆದರೆ (deadjusted)
    {
	Lab_Margin_ಎಡ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ಬಲ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ತುದಿ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    ಆದರೆ (replaceGC)
	eb->label.ಸಾಮಾನ್ಯ_GC = tmp_gc;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Enter(Widget wid,
    XEvent *event,
    String *params ಬಳಸದ,
    Cardinal *ಅಂಕೆ_params ಬಳಸದ)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ತಳ್ಳು_Button_Callback_Struct call_ಮೌಲ್ಯ;

    ಆದರೆ (Lab_IsMenupane(eb))
    {
	ಆದರೆ ((((ShellWidget) Xt_ಪೋಷಕ(Xt_ಪೋಷಕ(eb)))->shell.popped_ಮೇಲೆ)
		&& Xm_Get_ಒಳ_Drag_ಬಹುಲಕ((Widget) eb))
	{
#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_ಒಳ = dpy->display.enable_etched_ಒಳ_menu;
#ಇಲ್ಲದಿದ್ದರೆ
	    Boolean etched_ಒಳ = False;
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (eb->pushbutton.armed)
		ಹಿಂತಿರುಗಿ;

	    // ...so KHelp event is delivered correctly.
	    Xm_ಗಣ_Focus_Flag(Xt_ಪೋಷಕ(Xt_ಪೋಷಕ(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_ಗಣ_Keyboard_Focus(Xt_ಪೋಷಕ(Xt_ಪೋಷಕ(eb)), (Widget) eb);
	    Xm_ಗಣ_Focus_Flag(Xt_ಪೋಷಕ(Xt_ಪೋಷಕ(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_ಪೋಷಕ(wid))->manager.ಸಕ್ರಿಯ_ಮಗು = wid;

	    // etched in menu button
	    ಆದರೆ (etched_ಒಳ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.ಅಗಲ, eb->core.ಎತ್ತರ);
		draw_label(eb, event, ಖಾಲಿ);
		draw_pixmap(eb, event, ಖಾಲಿ);
	    }

	    ಆದರೆ ((eb->core.ಅಗಲ > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.ಎತ್ತರ >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			eb->primitive.ತುದಿ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.ಅಗಲ - 2 * eb->primitive.highlight_thickness,
			eb->core.ಎತ್ತರ - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(ಚಿಹ್ನೆರಹಿತ)(etched_ಒಳ ? Xm_SHADOW_ಒಳ : Xm_SHADOW_ಹೊರಹೋಗು));
	    }

	    ಆದರೆ (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ಮೌಲ್ಯ.reason = (ಪೂರ್ಣಾಂಕ)XmCR_ARM;
		call_ಮೌಲ್ಯ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_ಮೌಲ್ಯ);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ಖಾಲಿ, ಖಾಲಿ);
	ಆದರೆ (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_ಬೀಗ;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ಖಾಲಿ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ಖಾಲಿ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Leave(Widget wid,
    XEvent *event,
    String *params ಬಳಸದ,
    Cardinal *ಅಂಕೆ_params ಬಳಸದ)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ತಳ್ಳು_Button_Callback_Struct call_ಮೌಲ್ಯ;

    ಆದರೆ (Lab_IsMenupane(eb))
    {
#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_ಒಳ = dpy->display.enable_etched_ಒಳ_menu;
#ಇಲ್ಲದಿದ್ದರೆ
	Boolean etched_ಒಳ = False;
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (Xm_Get_ಒಳ_Drag_ಬಹುಲಕ((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.ಬಹುಲಕ == Notify_ಸಾಮಾನ್ಯ))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_ಪೋಷಕ(wid))->manager.ಸಕ್ರಿಯ_ಮಗು = ಖಾಲಿ;

	    ಆದರೆ (etched_ಒಳ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.ಅಗಲ, eb->core.ಎತ್ತರ);
		draw_label(eb, event, ಖಾಲಿ);
		draw_pixmap(eb, event, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		XmeClearBorder
		    (XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.ಅಗಲ -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.ಎತ್ತರ -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    ಆದರೆ (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ಮೌಲ್ಯ.reason = (ಪೂರ್ಣಾಂಕ)XmCR_DISARM;
		call_ಮೌಲ್ಯ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_ಮೌಲ್ಯ);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	_XmPrimitiveLeave((Widget) eb, event, ಖಾಲಿ, ಖಾಲಿ);

	ಆದರೆ (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_ಬೀಗ;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ಖಾಲಿ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ಖಾಲಿ);
	    eb->pushbutton.armed = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ಖಾಲಿ);
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿಸು IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಗಾತ್ರ_2(XmEnhancedButtonWidget newtb)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ w = 0;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    ಆದರೆ (newtb->enhancedbutton.pixmap_ದತ್ತಾಂಶ
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_ಅಗಲ;
	h = newtb->enhancedbutton.pixmap_ಎತ್ತರ;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    ಆದರೆ (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ಎಡ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_ಅಗಲ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	newtb->label.margin_ಎಡ = w;
	newtb->core.ಅಗಲ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ಅಗಲ)
	    + newtb->label.ಪಠ್ಯ_Rect.ಅಗಲ;

	ಆದರೆ (newtb->label.ಪಠ್ಯ_Rect.ಅಗಲ > 0)
	{
	    newtb->label.margin_ಎಡ += newtb->label.margin_ಅಗಲ
					  + newtb->primitive.shadow_thickness;
	    newtb->core.ಅಗಲ += newtb->label.margin_ಅಗಲ
					  + newtb->primitive.shadow_thickness;
	}
    }
    ಆದರೆ (newtb->label.ಪಠ್ಯ_Rect.ಎತ್ತರ < h)
    {
	newtb->core.ಎತ್ತರ = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ಎತ್ತರ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#ಆದರೆ 0
    ಬರೆ("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap,
	    h, newtb->core.ಎತ್ತರ,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_ಎತ್ತರ,
	    newtb->core.ಅಗಲ,
	    newtb->core.ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_ಬೀಗ;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Initialize(Widget rq, Widget ebw, ArgList ವಾದಗಳು ಬಳಸದ, Cardinal *n ಬಳಸದ)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_ಬೀಗ;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    ಆದರೆ (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	ಕಿಟಕಿ	ಬೇರು_W = Default_ಬೇರು_ಕಿಟಕಿ(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_ದತ್ತಾಂಶ(dpy, ಬೇರು_W, stipple_ಬಿಟ್ಟುಗಳು,
		stipple_ಅಗಲ, stipple_ಎತ್ತರ);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    ಆದರೆ (eb->enhancedbutton.pixmap_ದತ್ತಾಂಶ)
    {
	XmString str;
	ಗಣ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು((Widget) eb, XmNlabelString, str, ಖಾಲಿ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap;

    ಆದರೆ (request->core.ಅಗಲ == 0)
	eb->core.ಅಗಲ = 0;
    ಆದರೆ (request->core.ಎತ್ತರ == 0)
	eb->core.ಎತ್ತರ = 0;
    ಗಣ_ಗಾತ್ರ_2(eb);

    (* resize)((Widget)eb);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ಮೇಲಿನ_pix = eb->enhancedbutton.highlight_pixmap;

    ಆದರೆ (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    ಆದರೆ (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    ಆದರೆ (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    ಆದರೆ (ಮೇಲಿನ_pix != None && ಮೇಲಿನ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ಮೇಲಿನ_pix);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Destroy(Widget w)
{
    ಆದರೆ (!XmIsEnhancedButton(w))
	ಹಿಂತಿರುಗಿ;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ಸ್ಥಿರ Boolean
ಗಣ_ಮೌಲ್ಯಗಳು(Widget current,
	  Widget request ಬಳಸದ,
	  Widget new,
	  ArgList ವಾದಗಳು ಬಳಸದ,
	  Cardinal *n ಬಳಸದ)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#ವ್ಯಾಖ್ಯಾನಿಸು NOT_EQUAL(ಕ್ಷೇತ್ರ)       (cur->ಕ್ಷೇತ್ರ != eb->ಕ್ಷೇತ್ರ)

    /*
     * Make sure that lost sensitivity is causing the border to vanish as well.
     */
    ಆದರೆ (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	ಆದರೆ (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ		    r_x;
	    ಪೂರ್ಣಾಂಕ		    r_y;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    r_ಎತ್ತರ;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    r_ಅಗಲ;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    r_border;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    r_ಆಳ;
	    ಪೂರ್ಣಾಂಕ		    ಬೇರು_x;
	    ಪೂರ್ಣಾಂಕ		    ಬೇರು_y;
	    ಪೂರ್ಣಾಂಕ		    win_x;
	    ಪೂರ್ಣಾಂಕ		    win_y;
	    ಕಿಟಕಿ	    ಬೇರು;
	    ಕಿಟಕಿ	    ಬೇರು_q;
	    ಕಿಟಕಿ	    ಮಗು;
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    ಮುಖವಾಡ;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_ಕಿಟಕಿ(cur), &ಬೇರು, &r_x, &r_y, &r_ಅಗಲ,
			 &r_ಎತ್ತರ, &r_border, &r_ಆಳ);
	    XQuery_ಸೂಚಕ(XtDisplay(cur), Xt_ಕಿಟಕಿ(cur), &ಬೇರು_q, &ಮಗು,
			  &ಬೇರು_x, &ಬೇರು_y, &win_x, &win_y, &ಮುಖವಾಡ);

	    ಆದರೆ (ಬೇರು == ಬೇರು_q)
	    {
		ಆದರೆ ((win_x < 0) || (win_y < 0))
		    ಹಿಂತಿರುಗಿ False;

		ಆದರೆ ((win_x > (ಪೂರ್ಣಾಂಕ)r_ಅಗಲ) || (win_y > (ಪೂರ್ಣಾಂಕ)r_ಎತ್ತರ))
		    ಹಿಂತಿರುಗಿ False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	ಹಿಂತಿರುಗಿ True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    ಆದರೆ (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    ಆದರೆ (NOT_EQUAL(primitive.foreground))
	redraw = True;
    ಆದರೆ (NOT_EQUAL(core.background_pixel))
	redraw = True;
    ಆದರೆ (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    ಆದರೆ (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    ಆದರೆ (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    ಆದರೆ (NOT_EQUAL(label._label))
    {
	redraw = True;
	ಗಣ_ಗಾತ್ರ_2(eb);
    }

    ಆದರೆ (redraw == True)
    {
	ಆದರೆ (change)
	    ಗಣ_pixmap(eb);
	ಆದರೆ (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	ಇಲ್ಲದಿದ್ದರೆ
	    eb->label.pixmap = eb->enhancedbutton.ಸಾಮಾನ್ಯ_pixmap;
	ಆದರೆ (change)
	    ಗಣ_ಗಾತ್ರ_2(eb);
	redraw = False;
    }

    ಹಿಂತಿರುಗಿ redraw;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#ಆದರೆ_ಅಂತ್ಯ
    XRectangle		    box;
    ಪೂರ್ಣಾಂಕ			    dx;
    ಪೂರ್ಣಾಂಕ			    adjust;
    ಗಿಡ್ಡ		    fill = 0;

    ಆದರೆ (!XtIsRealized((Widget)eb))
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    ಆದರೆ ((eb->pushbutton.arm_color == eb->primitive.ತುದಿ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    ಆದರೆ (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    ಇಲ್ಲದಿದ್ದರೆ
	adjust = eb->pushbutton.default_button_shadow_thickness;

    ಆದರೆ (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	dx = (eb->primitive.highlight_thickness
		+ eb->primitive.shadow_thickness + fill);

    box.x = dx;
    box.y = dx;
    adjust = (dx << 1);
    box.ಅಗಲ  = eb->core.ಅಗಲ - adjust;
    box.ಎತ್ತರ = eb->core.ಎತ್ತರ - adjust;

    /*
     * Redraw the background.
     */
    ಆದರೆ (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	ಆದರೆ (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		ಆದರೆ (eb->label.label_type == (ಪೂರ್ಣಾಂಕ)XmPIXMAP)
		{
		    ಆದರೆ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    ಇಲ್ಲದಿದ್ದರೆ
			gc = eb->pushbutton.background_gc;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    gc = eb->pushbutton.fill_gc;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	ಆದರೆ (gc)
	    XFillRectangle(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb), gc,
		    box.x, box.y, box.ಅಗಲ, box.ಎತ್ತರ);
    }

    draw_label(eb, event, region);

    ಆದರೆ (Lab_IsMenupane(eb))
    {
	ಆದರೆ (eb->pushbutton.armed)
	    (*(((Xm_ತಳ್ಳು_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	adjust = 0;

#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	ಆಯ್ಕೆ (default_button_emphasis)
	{
	    ಸ್ಥಿತಿ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ Xm_ಆಂತರಿಕ_HIGHLIGHT:
		ನಿಲ್ಲಿಸು;

	    ಪೂರ್ವನಿಯೋಜಿತ:
		ಪ್ರತಿಜ್ಞಾ(FALSE);
		ಹಿಂತಿರುಗಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	ಆದರೆ (box.x > adjust)
	{
	    ಪೂರ್ಣಾಂಕ borderwidth =box.x - adjust;
	    ಪೂರ್ಣಾಂಕ rectwidth = eb->core.ಅಗಲ - 2 * adjust;
	    ಪೂರ್ಣಾಂಕ rectheight = eb->core.ಎತ್ತರ - 2 * adjust;

	    ಆದರೆ (XmIsManager(Xt_ಪೋಷಕ(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
						     Xm_ಪೋಷಕ_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		XmeClearBorder(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
	    ಆಯ್ಕೆ (default_button_emphasis)
	    {
		ಸ್ಥಿತಿ Xm_ಆಂತರಿಕ_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    ಆದರೆ (eb->primitive.highlight_drawn)
			(*(((Xm_ತಳ್ಳು_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
		    ನಿಲ್ಲಿಸು;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಆದರೆ (eb->pushbutton.default_button_shadow_thickness)
	{
	    ಆದರೆ (eb->pushbutton.show_as_default)
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
		ಪೂರ್ಣಾಂಕ	    default_button_shadow_thickness;
		ಪೂರ್ಣಾಂಕ	    x, y, ಅಗಲ, ಎತ್ತರ, delta;
		Widget	    ಪೋಷಕ;

		ಆದರೆ (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    ಹಿಂತಿರುಗಿ;

		ಆದರೆ (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    ಹಿಂತಿರುಗಿ;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		ಪೋಷಕ = Xt_ಪೋಷಕ(eb);
		ಆದರೆ (XmIsManager(ಪೋಷಕ))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_ಪೋಷಕ_ತುದಿ_Shadow_GC(eb);
		    ತುದಿ_gc = Xm_ಪೋಷಕ_Bottom_Shadow_GC(eb);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.ತುದಿ_shadow_GC;
		    ತುದಿ_gc = eb->primitive.bottom_shadow_GC;
		}

		ಆದರೆ ((bottom_gc == None) || (ತುದಿ_gc == None))
		    ಹಿಂತಿರುಗಿ;


		ಆದರೆ (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		ಇಲ್ಲದಿದ್ದರೆ
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#ಆದರೆ !defined(LESSTIF_ಆವೃತ್ತಿ) && (Xm_ಆವೃತ್ತಿ > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		ಆಯ್ಕೆ (default_button_emphasis)
		{
		    ಸ್ಥಿತಿ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ನಿಲ್ಲಿಸು;

		    ಸ್ಥಿತಿ Xm_ಆಂತರಿಕ_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ನಿಲ್ಲಿಸು;

		    ಪೂರ್ವನಿಯೋಜಿತ:
			ಪ್ರತಿಜ್ಞಾ(FALSE);
			ಹಿಂತಿರುಗಿ;
		}
#ಆದರೆ_ಅಂತ್ಯ

		x = y = delta;
		ಅಗಲ = eb->core.ಅಗಲ - 2 * delta;
		ಎತ್ತರ = eb->core.ಎತ್ತರ - 2 * delta;

		ಆದರೆ ((ಅಗಲ > 0) && (ಎತ್ತರ > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_ಕಿಟಕಿ(eb),
			    ತುದಿ_gc, bottom_gc, x, y, ಅಗಲ, ಎತ್ತರ,
			    default_button_shadow_thickness,
			    (ಚಿಹ್ನೆರಹಿತ)Xm_SHADOW_ಹೊರಹೋಗು);
	    }
	}

	ಆದರೆ (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ತಳ್ಳು_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ಖಾಲಿ, ಖಾಲಿ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ತಳ್ಳು_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ಖಾಲಿ, ಖಾಲಿ);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR
