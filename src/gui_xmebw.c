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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_TOOLBAR

#అన్తర్భావ <Xm/XmP.h>
#అన్తర్భావ <Xm/DrawP.h>
#ఒకవేళ defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#లేకపోతే
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  లేకపోతే
#   include <X11/xpm.h>
#  endif
# endif
#అయితే_ముగింపు
#అన్తర్భావ <Xm/ManagerP.h>
#అన్తర్భావ <Xm/Display.h>
#అన్తర్భావ <Xm/DisplayP.h>

#అన్తర్భావ <X11/Shell.h>
#అన్తర్భావ <X11/ShellP.h>

#అన్తర్భావ "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#ఒకవేళ defined(LESSTIF_సంస్కరణ) || (Xm_సంస్కరణ <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (పూర్ణాంకం)XmMENU_POPUP || \
		    Lab_MenuType(w) == (పూర్ణాంకం)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, పంక్తి_Solid)
#అయితే_ముగింపు

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#ఒకవేళ defined(VMS)
# ఒకవేళ defined(HAVE_XOS_R_H)
#  define XTPROCESS_తాళం Xt_Process_తాళం()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# లేకపోతే
#  define XTPROCESS_తాళం
#  define XTPROCESS_UNLOCK
# endif
#లేకపోతే
# define XTPROCESS_తాళం Xt_Process_తాళం()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#అయితే_ముగింపు

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#నిర్వచితం_కాకపోతే XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#అయితే_ముగింపు

బాహ్య Boolean Xm_Get_లోపల_Drag_బహుళకం(Widget widget);
బాహ్య శూన్య _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * అంకె_params);
బాహ్య శూన్య _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * అంకె_params);
బాహ్య శూన్య Xm_సమితి_Focus_Flag(Widget w, గుర్తులేని పూర్ణాంకం ముసుగు, Boolean విలువ);
బాహ్య శూన్య _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
స్థిర శూన్య Destroy(Widget w);
స్థిర శూన్య Initialize(Widget rq, Widget eb, ArgList వాదనలు, Cardinal *n);
స్థిర Boolean సమితి_విలువలు(Widget current, Widget request, Widget new, ArgList వాదనలు, Cardinal *n);
స్థిర శూన్య Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
స్థిర శూన్య Enter(Widget, XEvent *, String *, Cardinal *);
స్థిర శూన్య Leave(Widget, XEvent *, String *, Cardinal *);
స్థిర శూన్య BorderHighlight(Widget);
స్థిర శూన్య BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#నిర్వచించు stipple_వెడల్పు  4
#నిర్వచించు stipple_ఎత్తు 4
స్థిర అక్షరం stipple_బిట్లు[] = { 0x0a, 0x05, 0x0a, 0x05 };
#నిర్వచించు STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
స్థిర XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

స్థిర XtResource resources[] =
{
    {
	Xm_Npixmap_దత్తాంశం, XmCPixmap, XmRString, పరిమాణం(String),
	Xt_స్థానభ్రంశం_యొక్క(XmEnhancedButtonRec, enhancedbutton.pixmap_దత్తాంశం),
	XmRImmediate, (Xt_సూచిక) ఖాళీ
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, పరిమాణం(String),
	Xt_స్థానభ్రంశం_యొక్క(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_సూచిక) ఖాళీ
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, పరిమాణం(Dimension),
	Xt_స్థానభ్రంశం_యొక్క(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_సూచిక) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, పరిమాణం(పూర్ణాంకం),
	Xt_స్థానభ్రంశం_యొక్క(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_సూచిక) Xm_కుడి
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ ఖాళీ,
    /* record_type	   */ NULLQUARK,
    /*సంస్కరణ*/ Xm_Primitive_Class_Ext_సంస్కరణ,
    /* record_size	   */ పరిమాణం(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ ఖాళీ
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_నెట్టు_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ పరిమాణం(XmEnhancedButtonRec),
	/* class_initialize	 */ ఖాళీ,
	/* class_part_initialize */ ఖాళీ,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ ఖాళీ,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_సంఖ్య(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_సంఖ్య(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ సమితి_విలువలు,
	/* set_values_hook	 */ ఖాళీ,
	/* set_values_almost	 */ Xt_Inherit_సమితి_విలువలు_Almost,
	/* get_values_hook	 */ ఖాళీ,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_సంస్కరణ,
	/* callback_private	 */ ఖాళీ,
	/* tm_table		 */ ఖాళీ,
	/* query_geometry	 */ ఖాళీ,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ ఖాళీ
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ ఖాళీ,
	/* number of syn res	 */ 0,
	/* extension		 */ (Xt_సూచిక)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_సమితి_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ ఖాళీ,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_సూచిక) ఖాళీ,
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
    స్థిర గుర్తులేని పొట్టి
bump_color(గుర్తులేని పొట్టి విలువ)
{
    పూర్ణాంకం tmp = 2 * (((పూర్ణాంకం) విలువ - 65535) / 3) + 65535;

    తిరిగి tmp;
}

    స్థిర పూర్ణాంకం
alloc_color(Display	*display,
	Colormap	colormap,
	అక్షరం		*colorname,
	XColor		*xcolor,
	శూన్య		*సంవృతం వాడని)
{
    పూర్ణాంకం స్థితి_2;

    ఒకవేళ (colorname)
	ఒకవేళ (!XParseColor(display, colormap, colorname, xcolor))
	    తిరిగి -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    స్థితి_2 = XAllocColor(display, colormap, xcolor);
    తిరిగి స్థితి_2 != 0 ? 1 : 0;
}

// XPM
స్థిర అక్షరం * blank_xpm[] =
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
    స్థిర శూన్య
సమితి_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    కిటికీ	    మూలం;
    స్థిర Xpm_Color_చిహ్నం color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", ఖాళీ, 0},
	{"foreground", ఖాళీ, 0},
	{"bottomShadowColor", ఖాళీ, 0},
	{"topShadowColor", ఖాళీ, 0},
	{"highlightColor", ఖాళీ, 0},
	{"armColor", ఖాళీ, 0}
    };
    పూర్ణాంకం		    scr;
    Display	    *dpy = XtDisplay(eb);
    పూర్ణాంకం		    x;
    పూర్ణాంకం		    y;
    గుర్తులేని పూర్ణాంకం    ఎత్తు, వెడల్పు, border, లోతు;
    పూర్ణాంకం		    స్థితి_2 = 0;
    Pixmap	    ముసుగు;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    ఎగువ_pix = None;
    అక్షరం	    **దత్తాంశం = (అక్షరం **) eb->enhancedbutton.pixmap_దత్తాంశం;
    అక్షరం	    *fname = (అక్షరం *) eb->enhancedbutton.pixmap_file;
    పూర్ణాంకం		    జరుపు;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    ఒకవేళ (!దత్తాంశం)
	తిరిగి;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, ఖాళీ);

    scr = DefaultScreen(dpy);
    మూలం = మూలం_కిటికీ_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_లోతు = 0;
    eb->enhancedbutton.pixmap_వెడల్పు = 0;
    eb->enhancedbutton.pixmap_ఎత్తు = 0;
    eb->enhancedbutton.సాధారణ_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.శిఖరం_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.శిఖరం_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_సంఖ్య(color);

    ఒకవేళ (fname)
	స్థితి_2 = Xpm_Read_File_గమ్యం_Pixmap(dpy, మూలం, fname, &pix, &ముసుగు, &attr);
    ఒకవేళ (!fname || స్థితి_2 != XpmSuccess)
	స్థితి_2 = Xpm_Create_Pixmap_From_దత్తాంశం(dpy, మూలం, దత్తాంశం, &pix, &ముసుగు, &attr);

    // If something failed, we will fill in the default pixmap.
    ఒకవేళ (స్థితి_2 != XpmSuccess)
	స్థితి_2 = Xpm_Create_Pixmap_From_దత్తాంశం(dpy, మూలం, blank_xpm, &pix,
								&ముసుగు, &attr);

    Xpm_Free_లక్షణాలు(&attr);

    XGetGeometry(dpy, pix, &మూలం, &x, &y, &వెడల్పు, &ఎత్తు, &border, &లోతు);

    // TODO: does the shift depend on label_location somehow?
    జరుపు = eb->primitive.shadow_thickness / 2;

    ఒకవేళ (జరుపు < 1)
	జరుపు = 1;

    sen_pix = XCreatePixmap(dpy, మూలం, వెడల్పు + జరుపు, ఎత్తు + జరుపు, లోతు);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, వెడల్పు + జరుపు, ఎత్తు + జరుపు);
    XSet_Clip_ముసుగు(dpy, gc, ముసుగు);
    XSetClipOrigin(dpy, gc, జరుపు, జరుపు);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, వెడల్పు, ఎత్తు, జరుపు, జరుపు);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#నిర్వచితమైతే XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#లేకపోతే
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#అయితే_ముగింపు
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_సంఖ్య(color);

    స్థితి_2 = Xpm_Create_Pixmap_From_దత్తాంశం(dpy, మూలం, దత్తాంశం, &pix, ఖాళీ, &attr);
    Xpm_Free_లక్షణాలు(&attr);

    ఎగువ_pix = XCreatePixmap(dpy, మూలం, వెడల్పు + జరుపు, ఎత్తు + జరుపు, లోతు);

#ఒకవేళ 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#లేకపోతే
    XSetForeground(dpy, gc, eb->primitive.శిఖరం_shadow_color);
#అయితే_ముగింపు
    XSet_Clip_ముసుగు(dpy, gc, None);
    XFillRectangle(dpy, ఎగువ_pix, gc, 0, 0, వెడల్పు + జరుపు, ఎత్తు + జరుపు);
    XSet_Clip_ముసుగు(dpy, gc, ముసుగు);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, ఎగువ_pix, gc, 0, 0, వెడల్పు, ఎత్తు, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, వెడల్పు + జరుపు, ఎత్తు + జరుపు, లోతు);

    ఒకవేళ (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    లేకపోతే
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, జరుపు, జరుపు);
    XSet_Clip_ముసుగు(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, వెడల్పు + జరుపు, ఎత్తు + జరుపు);
    XSet_Clip_ముసుగు(dpy, gc, ముసుగు);
    XSetClipOrigin(dpy, gc, 2 * జరుపు, 2 * జరుపు);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, వెడల్పు, ఎత్తు, 2 * జరుపు, 2 * జరుపు);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ముసుగు);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_కీలకం;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_నిడివి(color);
    attr.color_కీలకం = XPM_MONO;
    స్థితి_2 = Xpm_Create_Pixmap_From_దత్తాంశం(dpy, మూలం, దత్తాంశం, &pix, &ముసుగు, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, మూలం, వెడల్పు + జరుపు, ఎత్తు + జరుపు, లోతు);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_ముసుగు(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, వెడల్పు + జరుపు, ఎత్తు + జరుపు);
    XSet_Clip_ముసుగు(dpy, gc, ముసుగు);
    XSetForeground(dpy, gc, eb->primitive.శిఖరం_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * జరుపు, 2 * జరుపు);
    XFillRectangle(dpy, ins_pix, gc, 2 * జరుపు, 2 * జరుపు, వెడల్పు, ఎత్తు);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, జరుపు, జరుపు);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, వెడల్పు + జరుపు, ఎత్తు + జరుపు);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_లక్షణాలు(&attr);

    eb->enhancedbutton.pixmap_లోతు = లోతు;
    eb->enhancedbutton.pixmap_వెడల్పు = వెడల్పు;
    eb->enhancedbutton.pixmap_ఎత్తు = ఎత్తు;
    eb->enhancedbutton.సాధారణ_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = ఎగువ_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ముసుగు);
}

#నిర్వచించు	BUTTON_ముసుగు ( \
	Button1_ముసుగు | Button2_ముసుగు | Button3_ముసుగు | Button4_ముసుగు | Button5_ముసుగు \
)

    స్థిర శూన్య
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		శిఖరం_gc;
    GC		bottom_gc;
    Boolean	etched_లోపల;

    ఒకవేళ (!eb->primitive.shadow_thickness)
	తిరిగి;

    ఒకవేళ ((eb->core.వెడల్పు <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.ఎత్తు <= 2 * eb->primitive.highlight_thickness))
	తిరిగి;

#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_లోపల = dpy->display.enable_etched_లోపల_menu;
    }
#లేకపోతే
    etched_లోపల = False;
#అయితే_ముగింపు
    ఒకవేళ (!etched_లోపల ^ eb->pushbutton.armed)
    {
	శిఖరం_gc = eb->primitive.శిఖరం_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    లేకపోతే
    {
	శిఖరం_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.శిఖరం_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_కిటికీ(eb),
	    శిఖరం_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.వెడల్పు - 2 * eb->primitive.highlight_thickness,
	    eb->core.ఎత్తు - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (గుర్తులేని)(etched_లోపల ? Xm_SHADOW_లోపల : Xm_SHADOW_బహిర్గతం));
}

    స్థిర శూన్య
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    ఒకవేళ (!Xt_వెడల్పు(eb) || !Xt_ఎత్తు(eb) || !eb->primitive.highlight_thickness)
	తిరిగి;

    XmeDrawHighlight(XtDisplay(eb), Xt_కిటికీ(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_వెడల్పు(eb), Xt_ఎత్తు(eb),
	    eb->primitive.highlight_thickness);
}

    స్థిర శూన్య
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    ఒకవేళ (!Xt_వెడల్పు(eb) || !Xt_ఎత్తు(eb) || !eb->primitive.highlight_thickness)
	తిరిగి;

    ఒకవేళ (XmIsManager(eb->core.మాతృ))
    {
#నిర్వచితమైతే UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	ఒకవేళ (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_సూచిక)
			    XtClass(eb->core.మాతృ), XmQTspecifyUnhighlight))
		    != ఖాళీ) && (UnhighlightT->getUnhighlightGC != ఖాళీ))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.మాతృ, (Widget) eb);
	}
	లేకపోతే
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.మాతృ))->manager.background_GC;
	}
#లేకపోతే
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.మాతృ))->manager.background_GC;
#అయితే_ముగింపు
	XmeDrawHighlight(XtDisplay(eb), Xt_కిటికీ(eb),
			 manager_background_GC,
			 0, 0, Xt_వెడల్పు(eb), Xt_ఎత్తు(eb),
			 eb->primitive.highlight_thickness);
	ఒకవేళ (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_కిటికీ(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.వెడల్పు - 2 * eb->primitive.highlight_thickness,
		    eb->core.ఎత్తు - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    లేకపోతే
	XmeClearBorder(XtDisplay(eb), Xt_కిటికీ(eb), 0, 0, Xt_వెడల్పు(eb),
		       Xt_ఎత్తు(eb), eb->primitive.highlight_thickness);
}

    స్థిర శూన్య
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event వాడని,
	    Region region వాడని)
{
    Pixmap	pix;
    GC		gc = eb->label.సాధారణ_GC;
    పూర్ణాంకం		లోతు;
    Cardinal	వెడల్పు;
    Cardinal	ఎత్తు;
    Cardinal	w;
    Cardinal	h;
    పూర్ణాంకం		x;
    పూర్ణాంకం		y;

    ఒకవేళ (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    లేకపోతే
    {
	ఒకవేళ (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	లేకపోతే ఒకవేళ (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	లేకపోతే
	    pix = eb->enhancedbutton.సాధారణ_pixmap;
    }

    ఒకవేళ (pix == None || !eb->enhancedbutton.pixmap_దత్తాంశం)
	తిరిగి;

    లోతు = eb->enhancedbutton.pixmap_లోతు;
    w = eb->enhancedbutton.pixmap_వెడల్పు;
    h = eb->enhancedbutton.pixmap_ఎత్తు;

    gc = eb->label.సాధారణ_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_వెడల్పు;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ఎత్తు;
    వెడల్పు = eb->core.వెడల్పు - 2 * x;
    ఒకవేళ (w < వెడల్పు)
	వెడల్పు = w;
    ఎత్తు = eb->core.ఎత్తు - 2 * y;
    ఒకవేళ (h < ఎత్తు)
	ఎత్తు = h;
    ఒకవేళ (లోతు == (పూర్ణాంకం)eb->core.లోతు)
	XCopyArea(XtDisplay(eb), pix, Xt_కిటికీ(eb), gc, 0, 0,
		వెడల్పు, ఎత్తు, x, y);
    లేకపోతే ఒకవేళ (లోతు == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_కిటికీ(eb), gc, 0, 0,
		వెడల్పు, ఎత్తు, x, y, (గుర్తులేని దీర్ఘ)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    స్థిర శూన్య
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = ఖాళీ;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_లోపల = dpy->display.enable_etched_లోపల_menu;
#లేకపోతే
    Boolean	etched_లోపల = False;
#అయితే_ముగింపు

    ఒకవేళ (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_లోపల)))
    {
	ఒకవేళ (eb->label.label_type == (పూర్ణాంకం)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.సాధారణ_GC;
	    eb->label.సాధారణ_GC = eb->pushbutton.background_gc;
	    replaceGC = True;
	}
    }

    /*
     * If the button contains a labeled pixmap, we will take it instead of our
     * own pixmap.
     */

    ఒకవేళ (eb->label.label_type == (పూర్ణాంకం)XmPIXMAP)
    {
	ఒకవేళ (eb->pushbutton.armed)
	{
	    ఒకవేళ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    లేకపోతే
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	లేకపోతే
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    ఒకవేళ (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_ఎడమ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_కుడి(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_శిఖరం(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_తాళం;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    ఒకవేళ (deadjusted)
    {
	Lab_Margin_ఎడమ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_కుడి(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_శిఖరం(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    ఒకవేళ (replaceGC)
	eb->label.సాధారణ_GC = tmp_gc;
}

    స్థిర శూన్య
Enter(Widget wid,
    XEvent *event,
    String *params వాడని,
    Cardinal *అంకె_params వాడని)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_నెట్టు_Button_Callback_Struct call_విలువ;

    ఒకవేళ (Lab_IsMenupane(eb))
    {
	ఒకవేళ ((((ShellWidget) Xt_మాతృ(Xt_మాతృ(eb)))->shell.popped_పైకి)
		&& Xm_Get_లోపల_Drag_బహుళకం((Widget) eb))
	{
#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_లోపల = dpy->display.enable_etched_లోపల_menu;
#లేకపోతే
	    Boolean etched_లోపల = False;
#అయితే_ముగింపు

	    ఒకవేళ (eb->pushbutton.armed)
		తిరిగి;

	    // ...so KHelp event is delivered correctly.
	    Xm_సమితి_Focus_Flag(Xt_మాతృ(Xt_మాతృ(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_సమితి_Keyboard_Focus(Xt_మాతృ(Xt_మాతృ(eb)), (Widget) eb);
	    Xm_సమితి_Focus_Flag(Xt_మాతృ(Xt_మాతృ(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_మాతృ(wid))->manager.చురుకైన_బిడ్డ = wid;

	    // etched in menu button
	    ఒకవేళ (etched_లోపల && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_కిటికీ(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.వెడల్పు, eb->core.ఎత్తు);
		draw_label(eb, event, ఖాళీ);
		draw_pixmap(eb, event, ఖాళీ);
	    }

	    ఒకవేళ ((eb->core.వెడల్పు > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.ఎత్తు >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_కిటికీ(eb),
			eb->primitive.శిఖరం_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.వెడల్పు - 2 * eb->primitive.highlight_thickness,
			eb->core.ఎత్తు - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(గుర్తులేని)(etched_లోపల ? Xm_SHADOW_లోపల : Xm_SHADOW_బహిర్గతం));
	    }

	    ఒకవేళ (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_విలువ.reason = (పూర్ణాంకం)XmCR_ARM;
		call_విలువ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_విలువ);
	    }
	}
    }
    లేకపోతే
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, ఖాళీ, ఖాళీ);
	ఒకవేళ (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_తాళం;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) ఖాళీ);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, ఖాళీ);
    }
}

    స్థిర శూన్య
Leave(Widget wid,
    XEvent *event,
    String *params వాడని,
    Cardinal *అంకె_params వాడని)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_నెట్టు_Button_Callback_Struct call_విలువ;

    ఒకవేళ (Lab_IsMenupane(eb))
    {
#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_లోపల = dpy->display.enable_etched_లోపల_menu;
#లేకపోతే
	Boolean etched_లోపల = False;
#అయితే_ముగింపు

	ఒకవేళ (Xm_Get_లోపల_Drag_బహుళకం((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.బహుళకం == Notify_సాధారణ))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_మాతృ(wid))->manager.చురుకైన_బిడ్డ = ఖాళీ;

	    ఒకవేళ (etched_లోపల && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_కిటికీ(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.వెడల్పు, eb->core.ఎత్తు);
		draw_label(eb, event, ఖాళీ);
		draw_pixmap(eb, event, ఖాళీ);
	    }
	    లేకపోతే
		XmeClearBorder
		    (XtDisplay(eb), Xt_కిటికీ(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.వెడల్పు -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.ఎత్తు -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    ఒకవేళ (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_విలువ.reason = (పూర్ణాంకం)XmCR_DISARM;
		call_విలువ.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_విలువ);
	    }
	}
    }
    లేకపోతే
    {
	_XmPrimitiveLeave((Widget) eb, event, ఖాళీ, ఖాళీ);

	ఒకవేళ (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_తాళం;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)ఖాళీ);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ఖాళీ);
	    eb->pushbutton.armed = TRUE;
	}
	లేకపోతే
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, ఖాళీ);
	}
    }
}

#నిర్వచించు IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    స్థిర శూన్య
సమితి_కొలత_2(XmEnhancedButtonWidget newtb)
{
    గుర్తులేని పూర్ణాంకం w = 0;
    గుర్తులేని పూర్ణాంకం h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    ఒకవేళ (newtb->enhancedbutton.pixmap_దత్తాంశం
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.సాధారణ_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_వెడల్పు;
	h = newtb->enhancedbutton.pixmap_ఎత్తు;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    ఒకవేళ (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_ఎడమ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_వెడల్పు;
    }
    లేకపోతే
    {
	newtb->label.margin_ఎడమ = w;
	newtb->core.వెడల్పు = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_వెడల్పు)
	    + newtb->label.పాఠ్యం_Rect.వెడల్పు;

	ఒకవేళ (newtb->label.పాఠ్యం_Rect.వెడల్పు > 0)
	{
	    newtb->label.margin_ఎడమ += newtb->label.margin_వెడల్పు
					  + newtb->primitive.shadow_thickness;
	    newtb->core.వెడల్పు += newtb->label.margin_వెడల్పు
					  + newtb->primitive.shadow_thickness;
	}
    }
    ఒకవేళ (newtb->label.పాఠ్యం_Rect.ఎత్తు < h)
    {
	newtb->core.ఎత్తు = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ఎత్తు);
    }
    లేకపోతే
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#ఒకవేళ 0
    రాయి("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.సాధారణ_pixmap,
	    h, newtb->core.ఎత్తు,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_ఎత్తు,
	    newtb->core.వెడల్పు,
	    newtb->core.ఎత్తు);
#అయితే_ముగింపు

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_తాళం;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    స్థిర శూన్య
Initialize(Widget rq, Widget ebw, ArgList వాదనలు వాడని, Cardinal *n వాడని)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_తాళం;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    ఒకవేళ (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	కిటికీ	మూలం_W = Default_మూలం_కిటికీ(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_దత్తాంశం(dpy, మూలం_W, stipple_బిట్లు,
		stipple_వెడల్పు, stipple_ఎత్తు);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    ఒకవేళ (eb->enhancedbutton.pixmap_దత్తాంశం)
    {
	XmString str;
	సమితి_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_సమితి_విలువలు((Widget) eb, XmNlabelString, str, ఖాళీ);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.సాధారణ_pixmap;

    ఒకవేళ (request->core.వెడల్పు == 0)
	eb->core.వెడల్పు = 0;
    ఒకవేళ (request->core.ఎత్తు == 0)
	eb->core.ఎత్తు = 0;
    సమితి_కొలత_2(eb);

    (* resize)((Widget)eb);
}

    స్థిర శూన్య
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.సాధారణ_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap ఎగువ_pix = eb->enhancedbutton.highlight_pixmap;

    ఒకవేళ (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    ఒకవేళ (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    ఒకవేళ (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    ఒకవేళ (ఎగువ_pix != None && ఎగువ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), ఎగువ_pix);
}

    స్థిర శూన్య
Destroy(Widget w)
{
    ఒకవేళ (!XmIsEnhancedButton(w))
	తిరిగి;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    స్థిర Boolean
సమితి_విలువలు(Widget current,
	  Widget request వాడని,
	  Widget new,
	  ArgList వాదనలు వాడని,
	  Cardinal *n వాడని)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#నిర్వచించు NOT_EQUAL(క్షేత్రం)       (cur->క్షేత్రం != eb->క్షేత్రం)

    /*
     * Make sure that lost sensitivity is causing the border to vanish as well.
     */
    ఒకవేళ (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	ఒకవేళ (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	లేకపోతే
	{
	    పూర్ణాంకం		    r_x;
	    పూర్ణాంకం		    r_y;
	    గుర్తులేని పూర్ణాంకం    r_ఎత్తు;
	    గుర్తులేని పూర్ణాంకం    r_వెడల్పు;
	    గుర్తులేని పూర్ణాంకం    r_border;
	    గుర్తులేని పూర్ణాంకం    r_లోతు;
	    పూర్ణాంకం		    మూలం_x;
	    పూర్ణాంకం		    మూలం_y;
	    పూర్ణాంకం		    win_x;
	    పూర్ణాంకం		    win_y;
	    కిటికీ	    మూలం;
	    కిటికీ	    మూలం_q;
	    కిటికీ	    బిడ్డ;
	    గుర్తులేని పూర్ణాంకం    ముసుగు;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_కిటికీ(cur), &మూలం, &r_x, &r_y, &r_వెడల్పు,
			 &r_ఎత్తు, &r_border, &r_లోతు);
	    XQuery_సూచిక(XtDisplay(cur), Xt_కిటికీ(cur), &మూలం_q, &బిడ్డ,
			  &మూలం_x, &మూలం_y, &win_x, &win_y, &ముసుగు);

	    ఒకవేళ (మూలం == మూలం_q)
	    {
		ఒకవేళ ((win_x < 0) || (win_y < 0))
		    తిరిగి False;

		ఒకవేళ ((win_x > (పూర్ణాంకం)r_వెడల్పు) || (win_y > (పూర్ణాంకం)r_ఎత్తు))
		    తిరిగి False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	తిరిగి True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    ఒకవేళ (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    ఒకవేళ (NOT_EQUAL(primitive.foreground))
	redraw = True;
    ఒకవేళ (NOT_EQUAL(core.background_pixel))
	redraw = True;
    ఒకవేళ (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    ఒకవేళ (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    ఒకవేళ (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    ఒకవేళ (NOT_EQUAL(label._label))
    {
	redraw = True;
	సమితి_కొలత_2(eb);
    }

    ఒకవేళ (redraw == True)
    {
	ఒకవేళ (change)
	    సమితి_pixmap(eb);
	ఒకవేళ (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	లేకపోతే
	    eb->label.pixmap = eb->enhancedbutton.సాధారణ_pixmap;
	ఒకవేళ (change)
	    సమితి_కొలత_2(eb);
	redraw = False;
    }

    తిరిగి redraw;
}

    స్థిర శూన్య
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#అయితే_ముగింపు
    XRectangle		    box;
    పూర్ణాంకం			    dx;
    పూర్ణాంకం			    adjust;
    పొట్టి		    fill = 0;

    ఒకవేళ (!XtIsRealized((Widget)eb))
	తిరిగి;

#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#అయితే_ముగింపు

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    ఒకవేళ ((eb->pushbutton.arm_color == eb->primitive.శిఖరం_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    ఒకవేళ (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    లేకపోతే
	adjust = eb->pushbutton.default_button_shadow_thickness;

    ఒకవేళ (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    లేకపోతే
	dx = (eb->primitive.highlight_thickness
		+ eb->primitive.shadow_thickness + fill);

    box.x = dx;
    box.y = dx;
    adjust = (dx << 1);
    box.వెడల్పు  = eb->core.వెడల్పు - adjust;
    box.ఎత్తు = eb->core.ఎత్తు - adjust;

    /*
     * Redraw the background.
     */
    ఒకవేళ (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	ఒకవేళ (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		ఒకవేళ (eb->label.label_type == (పూర్ణాంకం)XmPIXMAP)
		{
		    ఒకవేళ (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    లేకపోతే
			gc = eb->pushbutton.background_gc;
		}
		లేకపోతే
		    gc = eb->pushbutton.fill_gc;
	}
	లేకపోతే
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	ఒకవేళ (gc)
	    XFillRectangle(XtDisplay(eb), Xt_కిటికీ(eb), gc,
		    box.x, box.y, box.వెడల్పు, box.ఎత్తు);
    }

    draw_label(eb, event, region);

    ఒకవేళ (Lab_IsMenupane(eb))
    {
	ఒకవేళ (eb->pushbutton.armed)
	    (*(((Xm_నెట్టు_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    లేకపోతే
    {
	adjust = 0;

#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	ఎంపిక (default_button_emphasis)
	{
	    స్థితి XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		ఆపు;

	    స్థితి Xm_అంతర్గత_HIGHLIGHT:
		ఆపు;

	    అప్రమేయ:
		ప్రతిజ్ఞా(FALSE);
		తిరిగి;
	}
#అయితే_ముగింపు

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	ఒకవేళ (box.x > adjust)
	{
	    పూర్ణాంకం borderwidth =box.x - adjust;
	    పూర్ణాంకం rectwidth = eb->core.వెడల్పు - 2 * adjust;
	    పూర్ణాంకం rectheight = eb->core.ఎత్తు - 2 * adjust;

	    ఒకవేళ (XmIsManager(Xt_మాతృ(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_కిటికీ(eb),
						     Xm_మాతృ_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    లేకపోతే
	    {
		XmeClearBorder(XtDisplay(eb), Xt_కిటికీ(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
	    ఎంపిక (default_button_emphasis)
	    {
		స్థితి Xm_అంతర్గత_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    ఒకవేళ (eb->primitive.highlight_drawn)
			(*(((Xm_నెట్టు_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    ఆపు;

		అప్రమేయ:
		    ఆపు;
	    }
#అయితే_ముగింపు
	}

	ఒకవేళ (eb->pushbutton.default_button_shadow_thickness)
	{
	    ఒకవేళ (eb->pushbutton.show_as_default)
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
		GC	    శిఖరం_gc;
		GC	    bottom_gc;
		పూర్ణాంకం	    default_button_shadow_thickness;
		పూర్ణాంకం	    x, y, వెడల్పు, ఎత్తు, delta;
		Widget	    మాతృ;

		ఒకవేళ (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    తిరిగి;

		ఒకవేళ (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    తిరిగి;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		మాతృ = Xt_మాతృ(eb);
		ఒకవేళ (XmIsManager(మాతృ))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_మాతృ_శిఖరం_Shadow_GC(eb);
		    శిఖరం_gc = Xm_మాతృ_Bottom_Shadow_GC(eb);
		}
		లేకపోతే
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.శిఖరం_shadow_GC;
		    శిఖరం_gc = eb->primitive.bottom_shadow_GC;
		}

		ఒకవేళ ((bottom_gc == None) || (శిఖరం_gc == None))
		    తిరిగి;


		ఒకవేళ (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		లేకపోతే
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#ఒకవేళ !defined(LESSTIF_సంస్కరణ) && (Xm_సంస్కరణ > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		ఎంపిక (default_button_emphasis)
		{
		    స్థితి XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			ఆపు;

		    స్థితి Xm_అంతర్గత_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			ఆపు;

		    అప్రమేయ:
			ప్రతిజ్ఞా(FALSE);
			తిరిగి;
		}
#అయితే_ముగింపు

		x = y = delta;
		వెడల్పు = eb->core.వెడల్పు - 2 * delta;
		ఎత్తు = eb->core.ఎత్తు - 2 * delta;

		ఒకవేళ ((వెడల్పు > 0) && (ఎత్తు > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_కిటికీ(eb),
			    శిఖరం_gc, bottom_gc, x, y, వెడల్పు, ఎత్తు,
			    default_button_shadow_thickness,
			    (గుర్తులేని)Xm_SHADOW_బహిర్గతం);
	    }
	}

	ఒకవేళ (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    స్థిర శూన్య
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_నెట్టు_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, ఖాళీ, ఖాళీ);
}

    స్థిర శూన్య
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_నెట్టు_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, ఖాళీ, ఖాళీ);
}

#అయితే_ముగింపు // FEAT_TOOLBAR
