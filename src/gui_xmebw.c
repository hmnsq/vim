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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/XmP.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/DrawP.h>
#ਜੇ defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#ਨਹੀਂਤਾਂ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ਨਹੀਂਤਾਂ
#   include <X11/xpm.h>
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/ManagerP.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Display.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/DisplayP.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Shell.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/ShellP.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#ਜੇ defined(LESSTIF_ਸੰਸਕਰਣ) || (Xm_ਸੰਸਕਰਣ <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (ਪੂਰਨ)XmMENU_POPUP || \
		    Lab_MenuType(w) == (ਪੂਰਨ)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, ਸਤਰ_Solid)
#ਜੇ_ਅੰਤ

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#ਜੇ defined(VMS)
# ਜੇ defined(HAVE_XOS_R_H)
#  define XTPROCESS_ਤਾਲਾ Xt_Process_ਤਾਲਾ()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# ਨਹੀਂਤਾਂ
#  define XTPROCESS_ਤਾਲਾ
#  define XTPROCESS_UNLOCK
# endif
#ਨਹੀਂਤਾਂ
# define XTPROCESS_ਤਾਲਾ Xt_Process_ਤਾਲਾ()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#ਜੇ_ਅੰਤ

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ Boolean Xm_Get_ਵਿੱਚ_Drag_ਬਹੁਲਕ(Widget widget);
ਬਾਹਰੀ ਖਾਲੀ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ਅੰਕ_params);
ਬਾਹਰੀ ਖਾਲੀ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * ਅੰਕ_params);
ਬਾਹਰੀ ਖਾਲੀ Xm_ਸੈੱਟ_Focus_Flag(Widget w, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ, Boolean ਮੁੱਲ);
ਬਾਹਰੀ ਖਾਲੀ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ਸਥਿਰ ਖਾਲੀ Destroy(Widget w);
ਸਥਿਰ ਖਾਲੀ Initialize(Widget rq, Widget eb, ArgList ਦਲੀਲਾਂ, Cardinal *n);
ਸਥਿਰ Boolean ਸੈੱਟ_ਮੁੱਲਾਂ(Widget current, Widget request, Widget new, ArgList ਦਲੀਲਾਂ, Cardinal *n);
ਸਥਿਰ ਖਾਲੀ Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ਸਥਿਰ ਖਾਲੀ Enter(Widget, XEvent *, String *, Cardinal *);
ਸਥਿਰ ਖਾਲੀ Leave(Widget, XEvent *, String *, Cardinal *);
ਸਥਿਰ ਖਾਲੀ BorderHighlight(Widget);
ਸਥਿਰ ਖਾਲੀ BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ stipple_ਚੌੜਾਈ  4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ stipple_ਉਚਾਈ 4
ਸਥਿਰ ਅੱਖਰ stipple_ਬਿੱਟ[] = { 0x0a, 0x05, 0x0a, 0x05 };
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ਸਥਿਰ XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ਸਥਿਰ XtResource resources[] =
{
    {
	Xm_Npixmap_ਅੰਕੜੇ, XmCPixmap, XmRString, ਆਕਾਰ(String),
	Xt_ਵਿੱਥ_ਦਾ(XmEnhancedButtonRec, enhancedbutton.pixmap_ਅੰਕੜੇ),
	XmRImmediate, (Xt_ਸੂਚਕ) ਨਿਰਾ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, ਆਕਾਰ(String),
	Xt_ਵਿੱਥ_ਦਾ(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_ਸੂਚਕ) ਨਿਰਾ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, ਆਕਾਰ(Dimension),
	Xt_ਵਿੱਥ_ਦਾ(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_ਸੂਚਕ) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, ਆਕਾਰ(ਪੂਰਨ),
	Xt_ਵਿੱਥ_ਦਾ(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_ਸੂਚਕ) Xm_ਸੱਜਾ
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ਨਿਰਾ,
    /* record_type	   */ NULLQUARK,
    /*ਸੰਸਕਰਣ*/ Xm_Primitive_Class_Ext_ਸੰਸਕਰਣ,
    /* record_size	   */ ਆਕਾਰ(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ਨਿਰਾ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ਧੱਕੋ_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ ਆਕਾਰ(XmEnhancedButtonRec),
	/* class_initialize	 */ ਨਿਰਾ,
	/* class_part_initialize */ ਨਿਰਾ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ਨਿਰਾ,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_ਸੰਖਿਆ(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_ਸੰਖਿਆ(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ ਸੈੱਟ_ਮੁੱਲਾਂ,
	/* set_values_hook	 */ ਨਿਰਾ,
	/* set_values_almost	 */ Xt_Inherit_ਸੈੱਟ_ਮੁੱਲਾਂ_Almost,
	/* get_values_hook	 */ ਨਿਰਾ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_ਸੰਸਕਰਣ,
	/* callback_private	 */ ਨਿਰਾ,
	/* tm_table		 */ ਨਿਰਾ,
	/* query_geometry	 */ ਨਿਰਾ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ਨਿਰਾ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ਨਿਰਾ,
	/* number of syn res	 */ 0,
	/* extension		 */ (Xt_ਸੂਚਕ)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_ਸੈੱਟ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ਨਿਰਾ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_ਸੂਚਕ) ਨਿਰਾ,
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
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ
bump_color(ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਮੁੱਲ)
{
    ਪੂਰਨ tmp = 2 * (((ਪੂਰਨ) ਮੁੱਲ - 65535) / 3) + 65535;

    ਵਾਪਸ tmp;
}

    ਸਥਿਰ ਪੂਰਨ
alloc_color(Display	*display,
	Colormap	colormap,
	ਅੱਖਰ		*colorname,
	XColor		*xcolor,
	ਖਾਲੀ		*ਘੇਰਾ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ ਸਥਿਤੀ;

    ਜੇ (colorname)
	ਜੇ (!XParseColor(display, colormap, colorname, xcolor))
	    ਵਾਪਸ -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    ਸਥਿਤੀ = XAllocColor(display, colormap, xcolor);
    ਵਾਪਸ ਸਥਿਤੀ != 0 ? 1 : 0;
}

// XPM
ਸਥਿਰ ਅੱਖਰ * blank_xpm[] =
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
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    ਖਿੜਕੀ	    ਜੜ੍ਹ;
    ਸਥਿਰ Xpm_Color_ਚਿੰਨ੍ਹ color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ਨਿਰਾ, 0},
	{"foreground", ਨਿਰਾ, 0},
	{"bottomShadowColor", ਨਿਰਾ, 0},
	{"topShadowColor", ਨਿਰਾ, 0},
	{"highlightColor", ਨਿਰਾ, 0},
	{"armColor", ਨਿਰਾ, 0}
    };
    ਪੂਰਨ		    scr;
    Display	    *dpy = XtDisplay(eb);
    ਪੂਰਨ		    x;
    ਪੂਰਨ		    y;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    ਉਚਾਈ, ਚੌੜਾਈ, border, ਡੂੰਘਾਈ;
    ਪੂਰਨ		    ਸਥਿਤੀ = 0;
    Pixmap	    ਨਕਾਬ;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ਉੱਚਾ_pix = None;
    ਅੱਖਰ	    **ਅੰਕੜੇ = (ਅੱਖਰ **) eb->enhancedbutton.pixmap_ਅੰਕੜੇ;
    ਅੱਖਰ	    *fname = (ਅੱਖਰ *) eb->enhancedbutton.pixmap_file;
    ਪੂਰਨ		    ਖਿਸਕਾਓ;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    ਜੇ (!ਅੰਕੜੇ)
	ਵਾਪਸ;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ਨਿਰਾ);

    scr = DefaultScreen(dpy);
    ਜੜ੍ਹ = ਜੜ੍ਹ_ਖਿੜਕੀ_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_ਡੂੰਘਾਈ = 0;
    eb->enhancedbutton.pixmap_ਚੌੜਾਈ = 0;
    eb->enhancedbutton.pixmap_ਉਚਾਈ = 0;
    eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.ਸਿਖਰ_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.ਸਿਖਰ_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ਸੰਖਿਆ(color);

    ਜੇ (fname)
	ਸਥਿਤੀ = Xpm_Read_File_ਮੰਜ਼ਿਲ_Pixmap(dpy, ਜੜ੍ਹ, fname, &pix, &ਨਕਾਬ, &attr);
    ਜੇ (!fname || ਸਥਿਤੀ != XpmSuccess)
	ਸਥਿਤੀ = Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dpy, ਜੜ੍ਹ, ਅੰਕੜੇ, &pix, &ਨਕਾਬ, &attr);

    // If something failed, we will fill in the default pixmap.
    ਜੇ (ਸਥਿਤੀ != XpmSuccess)
	ਸਥਿਤੀ = Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dpy, ਜੜ੍ਹ, blank_xpm, &pix,
								&ਨਕਾਬ, &attr);

    Xpm_Free_ਗੁਣ(&attr);

    XGetGeometry(dpy, pix, &ਜੜ੍ਹ, &x, &y, &ਚੌੜਾਈ, &ਉਚਾਈ, &border, &ਡੂੰਘਾਈ);

    // TODO: does the shift depend on label_location somehow?
    ਖਿਸਕਾਓ = eb->primitive.shadow_thickness / 2;

    ਜੇ (ਖਿਸਕਾਓ < 1)
	ਖਿਸਕਾਓ = 1;

    sen_pix = XCreatePixmap(dpy, ਜੜ੍ਹ, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ, ਡੂੰਘਾਈ);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ);
    XSet_Clip_ਨਕਾਬ(dpy, gc, ਨਕਾਬ);
    XSetClipOrigin(dpy, gc, ਖਿਸਕਾਓ, ਖਿਸਕਾਓ);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, ਚੌੜਾਈ, ਉਚਾਈ, ਖਿਸਕਾਓ, ਖਿਸਕਾਓ);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#ਨਹੀਂਤਾਂ
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#ਜੇ_ਅੰਤ
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_ਸੰਖਿਆ(color);

    ਸਥਿਤੀ = Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dpy, ਜੜ੍ਹ, ਅੰਕੜੇ, &pix, ਨਿਰਾ, &attr);
    Xpm_Free_ਗੁਣ(&attr);

    ਉੱਚਾ_pix = XCreatePixmap(dpy, ਜੜ੍ਹ, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ, ਡੂੰਘਾਈ);

#ਜੇ 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#ਨਹੀਂਤਾਂ
    XSetForeground(dpy, gc, eb->primitive.ਸਿਖਰ_shadow_color);
#ਜੇ_ਅੰਤ
    XSet_Clip_ਨਕਾਬ(dpy, gc, None);
    XFillRectangle(dpy, ਉੱਚਾ_pix, gc, 0, 0, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ);
    XSet_Clip_ਨਕਾਬ(dpy, gc, ਨਕਾਬ);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ਉੱਚਾ_pix, gc, 0, 0, ਚੌੜਾਈ, ਉਚਾਈ, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ, ਡੂੰਘਾਈ);

    ਜੇ (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    ਨਹੀਂਤਾਂ
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, ਖਿਸਕਾਓ, ਖਿਸਕਾਓ);
    XSet_Clip_ਨਕਾਬ(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ);
    XSet_Clip_ਨਕਾਬ(dpy, gc, ਨਕਾਬ);
    XSetClipOrigin(dpy, gc, 2 * ਖਿਸਕਾਓ, 2 * ਖਿਸਕਾਓ);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, ਚੌੜਾਈ, ਉਚਾਈ, 2 * ਖਿਸਕਾਓ, 2 * ਖਿਸਕਾਓ);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ਨਕਾਬ);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ਕੁੰਜੀ;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_ਲੰਮਾਈ(color);
    attr.color_ਕੁੰਜੀ = XPM_MONO;
    ਸਥਿਤੀ = Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dpy, ਜੜ੍ਹ, ਅੰਕੜੇ, &pix, &ਨਕਾਬ, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ਜੜ੍ਹ, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ, ਡੂੰਘਾਈ);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_ਨਕਾਬ(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ);
    XSet_Clip_ਨਕਾਬ(dpy, gc, ਨਕਾਬ);
    XSetForeground(dpy, gc, eb->primitive.ਸਿਖਰ_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * ਖਿਸਕਾਓ, 2 * ਖਿਸਕਾਓ);
    XFillRectangle(dpy, ins_pix, gc, 2 * ਖਿਸਕਾਓ, 2 * ਖਿਸਕਾਓ, ਚੌੜਾਈ, ਉਚਾਈ);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, ਖਿਸਕਾਓ, ਖਿਸਕਾਓ);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ਚੌੜਾਈ + ਖਿਸਕਾਓ, ਉਚਾਈ + ਖਿਸਕਾਓ);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_ਗੁਣ(&attr);

    eb->enhancedbutton.pixmap_ਡੂੰਘਾਈ = ਡੂੰਘਾਈ;
    eb->enhancedbutton.pixmap_ਚੌੜਾਈ = ਚੌੜਾਈ;
    eb->enhancedbutton.pixmap_ਉਚਾਈ = ਉਚਾਈ;
    eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = ਉੱਚਾ_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ਨਕਾਬ);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ	BUTTON_ਨਕਾਬ ( \
	Button1_ਨਕਾਬ | Button2_ਨਕਾਬ | Button3_ਨਕਾਬ | Button4_ਨਕਾਬ | Button5_ਨਕਾਬ \
)

    ਸਥਿਰ ਖਾਲੀ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		ਸਿਖਰ_gc;
    GC		bottom_gc;
    Boolean	etched_ਵਿੱਚ;

    ਜੇ (!eb->primitive.shadow_thickness)
	ਵਾਪਸ;

    ਜੇ ((eb->core.ਚੌੜਾਈ <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.ਉਚਾਈ <= 2 * eb->primitive.highlight_thickness))
	ਵਾਪਸ;

#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_ਵਿੱਚ = dpy->display.enable_etched_ਵਿੱਚ_menu;
    }
#ਨਹੀਂਤਾਂ
    etched_ਵਿੱਚ = False;
#ਜੇ_ਅੰਤ
    ਜੇ (!etched_ਵਿੱਚ ^ eb->pushbutton.armed)
    {
	ਸਿਖਰ_gc = eb->primitive.ਸਿਖਰ_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    ਨਹੀਂਤਾਂ
    {
	ਸਿਖਰ_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.ਸਿਖਰ_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
	    ਸਿਖਰ_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.ਚੌੜਾਈ - 2 * eb->primitive.highlight_thickness,
	    eb->core.ਉਚਾਈ - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (ਬਿਨਾਨਿਸ਼ਾਨੀ)(etched_ਵਿੱਚ ? Xm_SHADOW_ਵਿੱਚ : Xm_SHADOW_ਬਾਹਰਲਾ));
}

    ਸਥਿਰ ਖਾਲੀ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    ਜੇ (!Xt_ਚੌੜਾਈ(eb) || !Xt_ਉਚਾਈ(eb) || !eb->primitive.highlight_thickness)
	ਵਾਪਸ;

    XmeDrawHighlight(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_ਚੌੜਾਈ(eb), Xt_ਉਚਾਈ(eb),
	    eb->primitive.highlight_thickness);
}

    ਸਥਿਰ ਖਾਲੀ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    ਜੇ (!Xt_ਚੌੜਾਈ(eb) || !Xt_ਉਚਾਈ(eb) || !eb->primitive.highlight_thickness)
	ਵਾਪਸ;

    ਜੇ (XmIsManager(eb->core.ਮਾਪਾ))
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	ਜੇ (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_ਸੂਚਕ)
			    XtClass(eb->core.ਮਾਪਾ), XmQTspecifyUnhighlight))
		    != ਨਿਰਾ) && (UnhighlightT->getUnhighlightGC != ਨਿਰਾ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.ਮਾਪਾ, (Widget) eb);
	}
	ਨਹੀਂਤਾਂ
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.ਮਾਪਾ))->manager.background_GC;
	}
#ਨਹੀਂਤਾਂ
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.ਮਾਪਾ))->manager.background_GC;
#ਜੇ_ਅੰਤ
	XmeDrawHighlight(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			 manager_background_GC,
			 0, 0, Xt_ਚੌੜਾਈ(eb), Xt_ਉਚਾਈ(eb),
			 eb->primitive.highlight_thickness);
	ਜੇ (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.ਚੌੜਾਈ - 2 * eb->primitive.highlight_thickness,
		    eb->core.ਉਚਾਈ - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    ਨਹੀਂਤਾਂ
	XmeClearBorder(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb), 0, 0, Xt_ਚੌੜਾਈ(eb),
		       Xt_ਉਚਾਈ(eb), eb->primitive.highlight_thickness);
}

    ਸਥਿਰ ਖਾਲੀ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event ਅਣਵਰਤਿਆ,
	    Region region ਅਣਵਰਤਿਆ)
{
    Pixmap	pix;
    GC		gc = eb->label.ਸਾਧਾਰਨ_GC;
    ਪੂਰਨ		ਡੂੰਘਾਈ;
    Cardinal	ਚੌੜਾਈ;
    Cardinal	ਉਚਾਈ;
    Cardinal	w;
    Cardinal	h;
    ਪੂਰਨ		x;
    ਪੂਰਨ		y;

    ਜੇ (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	ਨਹੀਂਤਾਂ ਜੇ (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	ਨਹੀਂਤਾਂ
	    pix = eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap;
    }

    ਜੇ (pix == None || !eb->enhancedbutton.pixmap_ਅੰਕੜੇ)
	ਵਾਪਸ;

    ਡੂੰਘਾਈ = eb->enhancedbutton.pixmap_ਡੂੰਘਾਈ;
    w = eb->enhancedbutton.pixmap_ਚੌੜਾਈ;
    h = eb->enhancedbutton.pixmap_ਉਚਾਈ;

    gc = eb->label.ਸਾਧਾਰਨ_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ਚੌੜਾਈ;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ਉਚਾਈ;
    ਚੌੜਾਈ = eb->core.ਚੌੜਾਈ - 2 * x;
    ਜੇ (w < ਚੌੜਾਈ)
	ਚੌੜਾਈ = w;
    ਉਚਾਈ = eb->core.ਉਚਾਈ - 2 * y;
    ਜੇ (h < ਉਚਾਈ)
	ਉਚਾਈ = h;
    ਜੇ (ਡੂੰਘਾਈ == (ਪੂਰਨ)eb->core.ਡੂੰਘਾਈ)
	XCopyArea(XtDisplay(eb), pix, Xt_ਖਿੜਕੀ(eb), gc, 0, 0,
		ਚੌੜਾਈ, ਉਚਾਈ, x, y);
    ਨਹੀਂਤਾਂ ਜੇ (ਡੂੰਘਾਈ == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_ਖਿੜਕੀ(eb), gc, 0, 0,
		ਚੌੜਾਈ, ਉਚਾਈ, x, y, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ਸਥਿਰ ਖਾਲੀ
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ਨਿਰਾ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_ਵਿੱਚ = dpy->display.enable_etched_ਵਿੱਚ_menu;
#ਨਹੀਂਤਾਂ
    Boolean	etched_ਵਿੱਚ = False;
#ਜੇ_ਅੰਤ

    ਜੇ (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_ਵਿੱਚ)))
    {
	ਜੇ (eb->label.label_type == (ਪੂਰਨ)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.ਸਾਧਾਰਨ_GC;
	    eb->label.ਸਾਧਾਰਨ_GC = eb->pushbutton.background_gc;
	    replaceGC = True;
	}
    }

    /*
     * If the button contains a labeled pixmap, we will take it instead of our
     * own pixmap.
     */

    ਜੇ (eb->label.label_type == (ਪੂਰਨ)XmPIXMAP)
    {
	ਜੇ (eb->pushbutton.armed)
	{
	    ਜੇ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    ਨਹੀਂਤਾਂ
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	ਨਹੀਂਤਾਂ
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    ਜੇ (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ਖੱਬਾ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ਸੱਜਾ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ਸਿਖਰ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_ਤਾਲਾ;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    ਜੇ (deadjusted)
    {
	Lab_Margin_ਖੱਬਾ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ਸੱਜਾ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ਸਿਖਰ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    ਜੇ (replaceGC)
	eb->label.ਸਾਧਾਰਨ_GC = tmp_gc;
}

    ਸਥਿਰ ਖਾਲੀ
Enter(Widget wid,
    XEvent *event,
    String *params ਅਣਵਰਤਿਆ,
    Cardinal *ਅੰਕ_params ਅਣਵਰਤਿਆ)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ਧੱਕੋ_Button_Callback_Struct call_ਮੁੱਲ;

    ਜੇ (Lab_IsMenupane(eb))
    {
	ਜੇ ((((ShellWidget) Xt_ਮਾਪਾ(Xt_ਮਾਪਾ(eb)))->shell.popped_ਉੱਪਰ)
		&& Xm_Get_ਵਿੱਚ_Drag_ਬਹੁਲਕ((Widget) eb))
	{
#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_ਵਿੱਚ = dpy->display.enable_etched_ਵਿੱਚ_menu;
#ਨਹੀਂਤਾਂ
	    Boolean etched_ਵਿੱਚ = False;
#ਜੇ_ਅੰਤ

	    ਜੇ (eb->pushbutton.armed)
		ਵਾਪਸ;

	    // ...so KHelp event is delivered correctly.
	    Xm_ਸੈੱਟ_Focus_Flag(Xt_ਮਾਪਾ(Xt_ਮਾਪਾ(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_ਸੈੱਟ_Keyboard_Focus(Xt_ਮਾਪਾ(Xt_ਮਾਪਾ(eb)), (Widget) eb);
	    Xm_ਸੈੱਟ_Focus_Flag(Xt_ਮਾਪਾ(Xt_ਮਾਪਾ(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_ਮਾਪਾ(wid))->manager.ਸਰਗਰਮ_ਬੱਚਾ = wid;

	    // etched in menu button
	    ਜੇ (etched_ਵਿੱਚ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.ਚੌੜਾਈ, eb->core.ਉਚਾਈ);
		draw_label(eb, event, ਨਿਰਾ);
		draw_pixmap(eb, event, ਨਿਰਾ);
	    }

	    ਜੇ ((eb->core.ਚੌੜਾਈ > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.ਉਚਾਈ >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			eb->primitive.ਸਿਖਰ_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.ਚੌੜਾਈ - 2 * eb->primitive.highlight_thickness,
			eb->core.ਉਚਾਈ - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(ਬਿਨਾਨਿਸ਼ਾਨੀ)(etched_ਵਿੱਚ ? Xm_SHADOW_ਵਿੱਚ : Xm_SHADOW_ਬਾਹਰਲਾ));
	    }

	    ਜੇ (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ਮੁੱਲ.reason = (ਪੂਰਨ)XmCR_ARM;
		call_ਮੁੱਲ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_ਮੁੱਲ);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ਨਿਰਾ, ਨਿਰਾ);
	ਜੇ (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_ਤਾਲਾ;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ਨਿਰਾ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ਨਿਰਾ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
Leave(Widget wid,
    XEvent *event,
    String *params ਅਣਵਰਤਿਆ,
    Cardinal *ਅੰਕ_params ਅਣਵਰਤਿਆ)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ਧੱਕੋ_Button_Callback_Struct call_ਮੁੱਲ;

    ਜੇ (Lab_IsMenupane(eb))
    {
#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_ਵਿੱਚ = dpy->display.enable_etched_ਵਿੱਚ_menu;
#ਨਹੀਂਤਾਂ
	Boolean etched_ਵਿੱਚ = False;
#ਜੇ_ਅੰਤ

	ਜੇ (Xm_Get_ਵਿੱਚ_Drag_ਬਹੁਲਕ((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.ਬਹੁਲਕ == Notify_ਸਾਧਾਰਨ))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_ਮਾਪਾ(wid))->manager.ਸਰਗਰਮ_ਬੱਚਾ = ਨਿਰਾ;

	    ਜੇ (etched_ਵਿੱਚ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.ਚੌੜਾਈ, eb->core.ਉਚਾਈ);
		draw_label(eb, event, ਨਿਰਾ);
		draw_pixmap(eb, event, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
		XmeClearBorder
		    (XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.ਚੌੜਾਈ -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.ਉਚਾਈ -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    ਜੇ (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ਮੁੱਲ.reason = (ਪੂਰਨ)XmCR_DISARM;
		call_ਮੁੱਲ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_ਮੁੱਲ);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	_XmPrimitiveLeave((Widget) eb, event, ਨਿਰਾ, ਨਿਰਾ);

	ਜੇ (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_ਤਾਲਾ;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ਨਿਰਾ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ਨਿਰਾ);
	    eb->pushbutton.armed = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ਨਿਰਾ);
	}
    }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਮਾਪ_2(XmEnhancedButtonWidget newtb)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ w = 0;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    ਜੇ (newtb->enhancedbutton.pixmap_ਅੰਕੜੇ
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.ਸਾਧਾਰਨ_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_ਚੌੜਾਈ;
	h = newtb->enhancedbutton.pixmap_ਉਚਾਈ;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    ਜੇ (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ਖੱਬਾ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_ਚੌੜਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	newtb->label.margin_ਖੱਬਾ = w;
	newtb->core.ਚੌੜਾਈ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ਚੌੜਾਈ)
	    + newtb->label.ਪਾਠ_Rect.ਚੌੜਾਈ;

	ਜੇ (newtb->label.ਪਾਠ_Rect.ਚੌੜਾਈ > 0)
	{
	    newtb->label.margin_ਖੱਬਾ += newtb->label.margin_ਚੌੜਾਈ
					  + newtb->primitive.shadow_thickness;
	    newtb->core.ਚੌੜਾਈ += newtb->label.margin_ਚੌੜਾਈ
					  + newtb->primitive.shadow_thickness;
	}
    }
    ਜੇ (newtb->label.ਪਾਠ_Rect.ਉਚਾਈ < h)
    {
	newtb->core.ਉਚਾਈ = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ਉਚਾਈ);
    }
    ਨਹੀਂਤਾਂ
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#ਜੇ 0
    ਲਿਖੋ("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.ਸਾਧਾਰਨ_pixmap,
	    h, newtb->core.ਉਚਾਈ,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_ਉਚਾਈ,
	    newtb->core.ਚੌੜਾਈ,
	    newtb->core.ਉਚਾਈ);
#ਜੇ_ਅੰਤ

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_ਤਾਲਾ;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ਸਥਿਰ ਖਾਲੀ
Initialize(Widget rq, Widget ebw, ArgList ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ, Cardinal *n ਅਣਵਰਤਿਆ)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_ਤਾਲਾ;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    ਜੇ (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	ਖਿੜਕੀ	ਜੜ੍ਹ_W = Default_ਜੜ੍ਹ_ਖਿੜਕੀ(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_ਅੰਕੜੇ(dpy, ਜੜ੍ਹ_W, stipple_ਬਿੱਟ,
		stipple_ਚੌੜਾਈ, stipple_ਉਚਾਈ);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    ਜੇ (eb->enhancedbutton.pixmap_ਅੰਕੜੇ)
    {
	XmString str;
	ਸੈੱਟ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ((Widget) eb, XmNlabelString, str, ਨਿਰਾ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap;

    ਜੇ (request->core.ਚੌੜਾਈ == 0)
	eb->core.ਚੌੜਾਈ = 0;
    ਜੇ (request->core.ਉਚਾਈ == 0)
	eb->core.ਉਚਾਈ = 0;
    ਸੈੱਟ_ਮਾਪ_2(eb);

    (* resize)((Widget)eb);
}

    ਸਥਿਰ ਖਾਲੀ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ਉੱਚਾ_pix = eb->enhancedbutton.highlight_pixmap;

    ਜੇ (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    ਜੇ (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    ਜੇ (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    ਜੇ (ਉੱਚਾ_pix != None && ਉੱਚਾ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ਉੱਚਾ_pix);
}

    ਸਥਿਰ ਖਾਲੀ
Destroy(Widget w)
{
    ਜੇ (!XmIsEnhancedButton(w))
	ਵਾਪਸ;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ਸਥਿਰ Boolean
ਸੈੱਟ_ਮੁੱਲਾਂ(Widget current,
	  Widget request ਅਣਵਰਤਿਆ,
	  Widget new,
	  ArgList ਦਲੀਲਾਂ ਅਣਵਰਤਿਆ,
	  Cardinal *n ਅਣਵਰਤਿਆ)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NOT_EQUAL(ਖੇਤਰ)       (cur->ਖੇਤਰ != eb->ਖੇਤਰ)

    /*
     * Make sure that lost sensitivity is causing the border to vanish as well.
     */
    ਜੇ (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	ਜੇ (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ		    r_x;
	    ਪੂਰਨ		    r_y;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    r_ਉਚਾਈ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    r_ਚੌੜਾਈ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    r_border;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    r_ਡੂੰਘਾਈ;
	    ਪੂਰਨ		    ਜੜ੍ਹ_x;
	    ਪੂਰਨ		    ਜੜ੍ਹ_y;
	    ਪੂਰਨ		    win_x;
	    ਪੂਰਨ		    win_y;
	    ਖਿੜਕੀ	    ਜੜ੍ਹ;
	    ਖਿੜਕੀ	    ਜੜ੍ਹ_q;
	    ਖਿੜਕੀ	    ਬੱਚਾ;
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    ਨਕਾਬ;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_ਖਿੜਕੀ(cur), &ਜੜ੍ਹ, &r_x, &r_y, &r_ਚੌੜਾਈ,
			 &r_ਉਚਾਈ, &r_border, &r_ਡੂੰਘਾਈ);
	    XQuery_ਸੂਚਕ(XtDisplay(cur), Xt_ਖਿੜਕੀ(cur), &ਜੜ੍ਹ_q, &ਬੱਚਾ,
			  &ਜੜ੍ਹ_x, &ਜੜ੍ਹ_y, &win_x, &win_y, &ਨਕਾਬ);

	    ਜੇ (ਜੜ੍ਹ == ਜੜ੍ਹ_q)
	    {
		ਜੇ ((win_x < 0) || (win_y < 0))
		    ਵਾਪਸ False;

		ਜੇ ((win_x > (ਪੂਰਨ)r_ਚੌੜਾਈ) || (win_y > (ਪੂਰਨ)r_ਉਚਾਈ))
		    ਵਾਪਸ False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	ਵਾਪਸ True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    ਜੇ (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    ਜੇ (NOT_EQUAL(primitive.foreground))
	redraw = True;
    ਜੇ (NOT_EQUAL(core.background_pixel))
	redraw = True;
    ਜੇ (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    ਜੇ (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    ਜੇ (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    ਜੇ (NOT_EQUAL(label._label))
    {
	redraw = True;
	ਸੈੱਟ_ਮਾਪ_2(eb);
    }

    ਜੇ (redraw == True)
    {
	ਜੇ (change)
	    ਸੈੱਟ_pixmap(eb);
	ਜੇ (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	ਨਹੀਂਤਾਂ
	    eb->label.pixmap = eb->enhancedbutton.ਸਾਧਾਰਨ_pixmap;
	ਜੇ (change)
	    ਸੈੱਟ_ਮਾਪ_2(eb);
	redraw = False;
    }

    ਵਾਪਸ redraw;
}

    ਸਥਿਰ ਖਾਲੀ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#ਜੇ_ਅੰਤ
    XRectangle		    box;
    ਪੂਰਨ			    dx;
    ਪੂਰਨ			    adjust;
    ਛੋਟਾ		    fill = 0;

    ਜੇ (!XtIsRealized((Widget)eb))
	ਵਾਪਸ;

#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#ਜੇ_ਅੰਤ

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    ਜੇ ((eb->pushbutton.arm_color == eb->primitive.ਸਿਖਰ_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    ਜੇ (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    ਨਹੀਂਤਾਂ
	adjust = eb->pushbutton.default_button_shadow_thickness;

    ਜੇ (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    ਨਹੀਂਤਾਂ
	dx = (eb->primitive.highlight_thickness
		+ eb->primitive.shadow_thickness + fill);

    box.x = dx;
    box.y = dx;
    adjust = (dx << 1);
    box.ਚੌੜਾਈ  = eb->core.ਚੌੜਾਈ - adjust;
    box.ਉਚਾਈ = eb->core.ਉਚਾਈ - adjust;

    /*
     * Redraw the background.
     */
    ਜੇ (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	ਜੇ (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		ਜੇ (eb->label.label_type == (ਪੂਰਨ)XmPIXMAP)
		{
		    ਜੇ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    ਨਹੀਂਤਾਂ
			gc = eb->pushbutton.background_gc;
		}
		ਨਹੀਂਤਾਂ
		    gc = eb->pushbutton.fill_gc;
	}
	ਨਹੀਂਤਾਂ
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	ਜੇ (gc)
	    XFillRectangle(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb), gc,
		    box.x, box.y, box.ਚੌੜਾਈ, box.ਉਚਾਈ);
    }

    draw_label(eb, event, region);

    ਜੇ (Lab_IsMenupane(eb))
    {
	ਜੇ (eb->pushbutton.armed)
	    (*(((Xm_ਧੱਕੋ_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    ਨਹੀਂਤਾਂ
    {
	adjust = 0;

#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	ਚੋਣ (default_button_emphasis)
	{
	    ਹਾਲਤ XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ਰੋਕੋ;

	    ਹਾਲਤ Xm_ਅੰਦਰੂਨੀ_HIGHLIGHT:
		ਰੋਕੋ;

	    ਮੂਲ:
		ਪ੍ਰਤਿਜ੍ਞਾ(FALSE);
		ਵਾਪਸ;
	}
#ਜੇ_ਅੰਤ

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	ਜੇ (box.x > adjust)
	{
	    ਪੂਰਨ borderwidth =box.x - adjust;
	    ਪੂਰਨ rectwidth = eb->core.ਚੌੜਾਈ - 2 * adjust;
	    ਪੂਰਨ rectheight = eb->core.ਉਚਾਈ - 2 * adjust;

	    ਜੇ (XmIsManager(Xt_ਮਾਪਾ(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
						     Xm_ਮਾਪਾ_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		XmeClearBorder(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
	    ਚੋਣ (default_button_emphasis)
	    {
		ਹਾਲਤ Xm_ਅੰਦਰੂਨੀ_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    ਜੇ (eb->primitive.highlight_drawn)
			(*(((Xm_ਧੱਕੋ_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ਰੋਕੋ;

		ਮੂਲ:
		    ਰੋਕੋ;
	    }
#ਜੇ_ਅੰਤ
	}

	ਜੇ (eb->pushbutton.default_button_shadow_thickness)
	{
	    ਜੇ (eb->pushbutton.show_as_default)
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
		GC	    ਸਿਖਰ_gc;
		GC	    bottom_gc;
		ਪੂਰਨ	    default_button_shadow_thickness;
		ਪੂਰਨ	    x, y, ਚੌੜਾਈ, ਉਚਾਈ, delta;
		Widget	    ਮਾਪਾ;

		ਜੇ (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    ਵਾਪਸ;

		ਜੇ (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    ਵਾਪਸ;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		ਮਾਪਾ = Xt_ਮਾਪਾ(eb);
		ਜੇ (XmIsManager(ਮਾਪਾ))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_ਮਾਪਾ_ਸਿਖਰ_Shadow_GC(eb);
		    ਸਿਖਰ_gc = Xm_ਮਾਪਾ_Bottom_Shadow_GC(eb);
		}
		ਨਹੀਂਤਾਂ
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.ਸਿਖਰ_shadow_GC;
		    ਸਿਖਰ_gc = eb->primitive.bottom_shadow_GC;
		}

		ਜੇ ((bottom_gc == None) || (ਸਿਖਰ_gc == None))
		    ਵਾਪਸ;


		ਜੇ (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		ਨਹੀਂਤਾਂ
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#ਜੇ !defined(LESSTIF_ਸੰਸਕਰਣ) && (Xm_ਸੰਸਕਰਣ > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		ਚੋਣ (default_button_emphasis)
		{
		    ਹਾਲਤ XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ਰੋਕੋ;

		    ਹਾਲਤ Xm_ਅੰਦਰੂਨੀ_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ਰੋਕੋ;

		    ਮੂਲ:
			ਪ੍ਰਤਿਜ੍ਞਾ(FALSE);
			ਵਾਪਸ;
		}
#ਜੇ_ਅੰਤ

		x = y = delta;
		ਚੌੜਾਈ = eb->core.ਚੌੜਾਈ - 2 * delta;
		ਉਚਾਈ = eb->core.ਉਚਾਈ - 2 * delta;

		ਜੇ ((ਚੌੜਾਈ > 0) && (ਉਚਾਈ > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_ਖਿੜਕੀ(eb),
			    ਸਿਖਰ_gc, bottom_gc, x, y, ਚੌੜਾਈ, ਉਚਾਈ,
			    default_button_shadow_thickness,
			    (ਬਿਨਾਨਿਸ਼ਾਨੀ)Xm_SHADOW_ਬਾਹਰਲਾ);
	    }
	}

	ਜੇ (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ਸਥਿਰ ਖਾਲੀ
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ਧੱਕੋ_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ਨਿਰਾ, ਨਿਰਾ);
}

    ਸਥਿਰ ਖਾਲੀ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ਧੱਕੋ_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ਨਿਰਾ, ਨਿਰਾ);
}

#ਜੇ_ਅੰਤ // FEAT_TOOLBAR
