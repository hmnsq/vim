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

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR

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
#جيڪڏهن_پورو
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
#إذا defined(LESSTIF_نسخو) || (Xm_نسخو <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحيح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحيح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, سٽ_Solid)
#جيڪڏهن_پورو

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#إذا defined(VMS)
# إذا defined(HAVE_XOS_R_H)
#  define XTPROCESS_تالو Xt_Process_تالو()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# وإلا
#  define XTPROCESS_تالو
#  define XTPROCESS_UNLOCK
# endif
#وإلا
# define XTPROCESS_تالو Xt_Process_تالو()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#جيڪڏهن_پورو

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#جيڪڏهن_وصف_نه_ٿيل XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#جيڪڏهن_پورو

خارجي Boolean Xm_Get_اندريون_Drag_گهڻائي(Widget widget);
خارجي فراغ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * انگ_params);
خارجي فراغ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * انگ_params);
خارجي فراغ Xm_سيٽ_Focus_Flag(Widget w, بنانشان صحيح نقاب, Boolean قدر);
خارجي فراغ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ساكن فراغ Destroy(Widget w);
ساكن فراغ Initialize(Widget rq, Widget eb, ArgList دليلون, Cardinal *n);
ساكن Boolean سيٽ_قدرون(Widget current, Widget request, Widget new, ArgList دليلون, Cardinal *n);
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
#وصف_ڪريو stipple_ويڪر  4
#وصف_ڪريو stipple_اوچائي 4
ساكن حرف stipple_بٽ[] = { 0x0a, 0x05, 0x0a, 0x05 };
#وصف_ڪريو STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

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
	Xm_Npixmap_معلومات, XmCPixmap, XmRString, حجم(String),
	Xt_وٿي_جو(XmEnhancedButtonRec, enhancedbutton.pixmap_معلومات),
	XmRImmediate, (Xt_اشارو) عدم
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, حجم(String),
	Xt_وٿي_جو(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_اشارو) عدم
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, حجم(Dimension),
	Xt_وٿي_جو(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_اشارو) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, حجم(صحيح),
	Xt_وٿي_جو(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_اشارو) Xm_ساڄو
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ عدم,
    /* record_type	   */ NULLQUARK,
    /*نسخو*/ Xm_Primitive_Class_Ext_نسخو,
    /* record_size	   */ حجم(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ عدم
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ڌڪو_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ حجم(XmEnhancedButtonRec),
	/* class_initialize	 */ عدم,
	/* class_part_initialize */ عدم,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ عدم,
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
	/* set_values		 */ سيٽ_قدرون,
	/* set_values_hook	 */ عدم,
	/* set_values_almost	 */ Xt_Inherit_سيٽ_قدرون_Almost,
	/* get_values_hook	 */ عدم,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_نسخو,
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
	/* extension		 */ (Xt_اشارو)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_سيٽ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ عدم,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_اشارو) عدم,
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
    ساكن بنانشان ننڍو
bump_color(بنانشان ننڍو قدر)
{
    صحيح tmp = 2 * (((صحيح) قدر - 65535) / 3) + 65535;

    أرجع tmp;
}

    ساكن صحيح
alloc_color(Display	*display,
	Colormap	colormap,
	حرف		*colorname,
	XColor		*xcolor,
	فراغ		*گهيرو اڻ_استعمال_ٿيل)
{
    صحيح حالت;

    إذا (colorname)
	إذا (!XParseColor(display, colormap, colorname, xcolor))
	    أرجع -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    حالت = XAllocColor(display, colormap, xcolor);
    أرجع حالت != 0 ? 1 : 0;
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
سيٽ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    دري	    پاڙ;
    ساكن Xpm_Color_نشان color[8] = {
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
    بنانشان صحيح    اوچائي, ويڪر, border, اونهائي;
    صحيح		    حالت = 0;
    Pixmap	    نقاب;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    مٿاهون_pix = None;
    حرف	    **معلومات = (حرف **) eb->enhancedbutton.pixmap_معلومات;
    حرف	    *fname = (حرف *) eb->enhancedbutton.pixmap_file;
    صحيح		    سرڪايو;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    إذا (!معلومات)
	أرجع;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, عدم);

    scr = DefaultScreen(dpy);
    پاڙ = پاڙ_دري_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_اونهائي = 0;
    eb->enhancedbutton.pixmap_ويڪر = 0;
    eb->enhancedbutton.pixmap_اوچائي = 0;
    eb->enhancedbutton.عام_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.چوٽي_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.چوٽي_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_عدد(color);

    إذا (fname)
	حالت = Xpm_Read_File_منزل_Pixmap(dpy, پاڙ, fname, &pix, &نقاب, &attr);
    إذا (!fname || حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_معلومات(dpy, پاڙ, معلومات, &pix, &نقاب, &attr);

    // If something failed, we will fill in the default pixmap.
    إذا (حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_معلومات(dpy, پاڙ, blank_xpm, &pix,
								&نقاب, &attr);

    Xpm_Free_خاصيتون(&attr);

    XGetGeometry(dpy, pix, &پاڙ, &x, &y, &ويڪر, &اوچائي, &border, &اونهائي);

    // TODO: does the shift depend on label_location somehow?
    سرڪايو = eb->primitive.shadow_thickness / 2;

    إذا (سرڪايو < 1)
	سرڪايو = 1;

    sen_pix = XCreatePixmap(dpy, پاڙ, ويڪر + سرڪايو, اوچائي + سرڪايو, اونهائي);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, ويڪر + سرڪايو, اوچائي + سرڪايو);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, سرڪايو, سرڪايو);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, ويڪر, اوچائي, سرڪايو, سرڪايو);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#جيڪڏهن_وصف_ٿيل XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#وإلا
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#جيڪڏهن_پورو
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_عدد(color);

    حالت = Xpm_Create_Pixmap_From_معلومات(dpy, پاڙ, معلومات, &pix, عدم, &attr);
    Xpm_Free_خاصيتون(&attr);

    مٿاهون_pix = XCreatePixmap(dpy, پاڙ, ويڪر + سرڪايو, اوچائي + سرڪايو, اونهائي);

#إذا 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#وإلا
    XSetForeground(dpy, gc, eb->primitive.چوٽي_shadow_color);
#جيڪڏهن_پورو
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, مٿاهون_pix, gc, 0, 0, ويڪر + سرڪايو, اوچائي + سرڪايو);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, مٿاهون_pix, gc, 0, 0, ويڪر, اوچائي, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, ويڪر + سرڪايو, اوچائي + سرڪايو, اونهائي);

    إذا (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    وإلا
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, سرڪايو, سرڪايو);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, ويڪر + سرڪايو, اوچائي + سرڪايو);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 2 * سرڪايو, 2 * سرڪايو);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, ويڪر, اوچائي, 2 * سرڪايو, 2 * سرڪايو);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, نقاب);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_ڪنجي;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_ڊيگهائي(color);
    attr.color_ڪنجي = XPM_MONO;
    حالت = Xpm_Create_Pixmap_From_معلومات(dpy, پاڙ, معلومات, &pix, &نقاب, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, پاڙ, ويڪر + سرڪايو, اوچائي + سرڪايو, اونهائي);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ويڪر + سرڪايو, اوچائي + سرڪايو);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetForeground(dpy, gc, eb->primitive.چوٽي_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * سرڪايو, 2 * سرڪايو);
    XFillRectangle(dpy, ins_pix, gc, 2 * سرڪايو, 2 * سرڪايو, ويڪر, اوچائي);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, سرڪايو, سرڪايو);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, ويڪر + سرڪايو, اوچائي + سرڪايو);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_خاصيتون(&attr);

    eb->enhancedbutton.pixmap_اونهائي = اونهائي;
    eb->enhancedbutton.pixmap_ويڪر = ويڪر;
    eb->enhancedbutton.pixmap_اوچائي = اوچائي;
    eb->enhancedbutton.عام_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = مٿاهون_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, نقاب);
}

#وصف_ڪريو	BUTTON_نقاب ( \
	Button1_نقاب | Button2_نقاب | Button3_نقاب | Button4_نقاب | Button5_نقاب \
)

    ساكن فراغ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		چوٽي_gc;
    GC		bottom_gc;
    Boolean	etched_اندريون;

    إذا (!eb->primitive.shadow_thickness)
	أرجع;

    إذا ((eb->core.ويڪر <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.اوچائي <= 2 * eb->primitive.highlight_thickness))
	أرجع;

#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_اندريون = dpy->display.enable_etched_اندريون_menu;
    }
#وإلا
    etched_اندريون = False;
#جيڪڏهن_پورو
    إذا (!etched_اندريون ^ eb->pushbutton.armed)
    {
	چوٽي_gc = eb->primitive.چوٽي_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    وإلا
    {
	چوٽي_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.چوٽي_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_دري(eb),
	    چوٽي_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.ويڪر - 2 * eb->primitive.highlight_thickness,
	    eb->core.اوچائي - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (بنانشان)(etched_اندريون ? Xm_SHADOW_اندريون : Xm_SHADOW_ٻاهريون));
}

    ساكن فراغ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    إذا (!Xt_ويڪر(eb) || !Xt_اوچائي(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    XmeDrawHighlight(XtDisplay(eb), Xt_دري(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_ويڪر(eb), Xt_اوچائي(eb),
	    eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    إذا (!Xt_ويڪر(eb) || !Xt_اوچائي(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    إذا (XmIsManager(eb->core.والد))
    {
#جيڪڏهن_وصف_ٿيل UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	إذا (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_اشارو)
			    XtClass(eb->core.والد), XmQTspecifyUnhighlight))
		    != عدم) && (UnhighlightT->getUnhighlightGC != عدم))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.والد, (Widget) eb);
	}
	وإلا
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.والد))->manager.background_GC;
	}
#وإلا
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.والد))->manager.background_GC;
#جيڪڏهن_پورو
	XmeDrawHighlight(XtDisplay(eb), Xt_دري(eb),
			 manager_background_GC,
			 0, 0, Xt_ويڪر(eb), Xt_اوچائي(eb),
			 eb->primitive.highlight_thickness);
	إذا (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_دري(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.ويڪر - 2 * eb->primitive.highlight_thickness,
		    eb->core.اوچائي - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    وإلا
	XmeClearBorder(XtDisplay(eb), Xt_دري(eb), 0, 0, Xt_ويڪر(eb),
		       Xt_اوچائي(eb), eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event اڻ_استعمال_ٿيل,
	    Region region اڻ_استعمال_ٿيل)
{
    Pixmap	pix;
    GC		gc = eb->label.عام_GC;
    صحيح		اونهائي;
    Cardinal	ويڪر;
    Cardinal	اوچائي;
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
	    pix = eb->enhancedbutton.عام_pixmap;
    }

    إذا (pix == None || !eb->enhancedbutton.pixmap_معلومات)
	أرجع;

    اونهائي = eb->enhancedbutton.pixmap_اونهائي;
    w = eb->enhancedbutton.pixmap_ويڪر;
    h = eb->enhancedbutton.pixmap_اوچائي;

    gc = eb->label.عام_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_ويڪر;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_اوچائي;
    ويڪر = eb->core.ويڪر - 2 * x;
    إذا (w < ويڪر)
	ويڪر = w;
    اوچائي = eb->core.اوچائي - 2 * y;
    إذا (h < اوچائي)
	اوچائي = h;
    إذا (اونهائي == (صحيح)eb->core.اونهائي)
	XCopyArea(XtDisplay(eb), pix, Xt_دري(eb), gc, 0, 0,
		ويڪر, اوچائي, x, y);
    وإلا إذا (اونهائي == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_دري(eb), gc, 0, 0,
		ويڪر, اوچائي, x, y, (بنانشان ڊگهو)1);
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
#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_اندريون = dpy->display.enable_etched_اندريون_menu;
#وإلا
    Boolean	etched_اندريون = False;
#جيڪڏهن_پورو

    إذا (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_اندريون)))
    {
	إذا (eb->label.label_type == (صحيح)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.عام_GC;
	    eb->label.عام_GC = eb->pushbutton.background_gc;
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
	Lab_Margin_کاٻو(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ساڄو(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٽي(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_تالو;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    إذا (deadjusted)
    {
	Lab_Margin_کاٻو(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ساڄو(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٽي(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    إذا (replaceGC)
	eb->label.عام_GC = tmp_gc;
}

    ساكن فراغ
Enter(Widget wid,
    XEvent *event,
    String *params اڻ_استعمال_ٿيل,
    Cardinal *انگ_params اڻ_استعمال_ٿيل)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ڌڪو_Button_Callback_Struct call_قدر;

    إذا (Lab_IsMenupane(eb))
    {
	إذا ((((ShellWidget) Xt_والد(Xt_والد(eb)))->shell.popped_مٿي)
		&& Xm_Get_اندريون_Drag_گهڻائي((Widget) eb))
	{
#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_اندريون = dpy->display.enable_etched_اندريون_menu;
#وإلا
	    Boolean etched_اندريون = False;
#جيڪڏهن_پورو

	    إذا (eb->pushbutton.armed)
		أرجع;

	    // ...so KHelp event is delivered correctly.
	    Xm_سيٽ_Focus_Flag(Xt_والد(Xt_والد(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_سيٽ_Keyboard_Focus(Xt_والد(Xt_والد(eb)), (Widget) eb);
	    Xm_سيٽ_Focus_Flag(Xt_والد(Xt_والد(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_والد(wid))->manager.سرگرم_ٻار = wid;

	    // etched in menu button
	    إذا (etched_اندريون && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_دري(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.ويڪر, eb->core.اوچائي);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }

	    إذا ((eb->core.ويڪر > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.اوچائي >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_دري(eb),
			eb->primitive.چوٽي_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.ويڪر - 2 * eb->primitive.highlight_thickness,
			eb->core.اوچائي - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(بنانشان)(etched_اندريون ? Xm_SHADOW_اندريون : Xm_SHADOW_ٻاهريون));
	    }

	    إذا (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_قدر.reason = (صحيح)XmCR_ARM;
		call_قدر.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_قدر);
	    }
	}
    }
    وإلا
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, عدم, عدم);
	إذا (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_تالو;
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
    String *params اڻ_استعمال_ٿيل,
    Cardinal *انگ_params اڻ_استعمال_ٿيل)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ڌڪو_Button_Callback_Struct call_قدر;

    إذا (Lab_IsMenupane(eb))
    {
#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_اندريون = dpy->display.enable_etched_اندريون_menu;
#وإلا
	Boolean etched_اندريون = False;
#جيڪڏهن_پورو

	إذا (Xm_Get_اندريون_Drag_گهڻائي((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.گهڻائي == Notify_عام))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_والد(wid))->manager.سرگرم_ٻار = عدم;

	    إذا (etched_اندريون && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_دري(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.ويڪر, eb->core.اوچائي);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }
	    وإلا
		XmeClearBorder
		    (XtDisplay(eb), Xt_دري(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.ويڪر -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.اوچائي -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    إذا (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_قدر.reason = (صحيح)XmCR_DISARM;
		call_قدر.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_قدر);
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
	    XTPROCESS_تالو;
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

#وصف_ڪريو IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ساكن فراغ
سيٽ_سائيز_2(XmEnhancedButtonWidget newtb)
{
    بنانشان صحيح w = 0;
    بنانشان صحيح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    إذا (newtb->enhancedbutton.pixmap_معلومات
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.عام_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_ويڪر;
	h = newtb->enhancedbutton.pixmap_اوچائي;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    إذا (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_کاٻو = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_ويڪر;
    }
    وإلا
    {
	newtb->label.margin_کاٻو = w;
	newtb->core.ويڪر = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_ويڪر)
	    + newtb->label.متن_Rect.ويڪر;

	إذا (newtb->label.متن_Rect.ويڪر > 0)
	{
	    newtb->label.margin_کاٻو += newtb->label.margin_ويڪر
					  + newtb->primitive.shadow_thickness;
	    newtb->core.ويڪر += newtb->label.margin_ويڪر
					  + newtb->primitive.shadow_thickness;
	}
    }
    إذا (newtb->label.متن_Rect.اوچائي < h)
    {
	newtb->core.اوچائي = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_اوچائي);
    }
    وإلا
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#إذا 0
    لکو("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.عام_pixmap,
	    h, newtb->core.اوچائي,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_اوچائي,
	    newtb->core.ويڪر,
	    newtb->core.اوچائي);
#جيڪڏهن_پورو

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_تالو;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ساكن فراغ
Initialize(Widget rq, Widget ebw, ArgList دليلون اڻ_استعمال_ٿيل, Cardinal *n اڻ_استعمال_ٿيل)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_تالو;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    إذا (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	دري	پاڙ_W = Default_پاڙ_دري(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_معلومات(dpy, پاڙ_W, stipple_بٽ,
		stipple_ويڪر, stipple_اوچائي);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    إذا (eb->enhancedbutton.pixmap_معلومات)
    {
	XmString str;
	سيٽ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_سيٽ_قدرون((Widget) eb, XmNlabelString, str, عدم);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.عام_pixmap;

    إذا (request->core.ويڪر == 0)
	eb->core.ويڪر = 0;
    إذا (request->core.اوچائي == 0)
	eb->core.اوچائي = 0;
    سيٽ_سائيز_2(eb);

    (* resize)((Widget)eb);
}

    ساكن فراغ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.عام_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap مٿاهون_pix = eb->enhancedbutton.highlight_pixmap;

    إذا (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    إذا (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    إذا (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    إذا (مٿاهون_pix != None && مٿاهون_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), مٿاهون_pix);
}

    ساكن فراغ
Destroy(Widget w)
{
    إذا (!XmIsEnhancedButton(w))
	أرجع;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ساكن Boolean
سيٽ_قدرون(Widget current,
	  Widget request اڻ_استعمال_ٿيل,
	  Widget new,
	  ArgList دليلون اڻ_استعمال_ٿيل,
	  Cardinal *n اڻ_استعمال_ٿيل)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#وصف_ڪريو NOT_EQUAL(ميدان)       (cur->ميدان != eb->ميدان)

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
	    بنانشان صحيح    r_اوچائي;
	    بنانشان صحيح    r_ويڪر;
	    بنانشان صحيح    r_border;
	    بنانشان صحيح    r_اونهائي;
	    صحيح		    پاڙ_x;
	    صحيح		    پاڙ_y;
	    صحيح		    win_x;
	    صحيح		    win_y;
	    دري	    پاڙ;
	    دري	    پاڙ_q;
	    دري	    ٻار;
	    بنانشان صحيح    نقاب;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_دري(cur), &پاڙ, &r_x, &r_y, &r_ويڪر,
			 &r_اوچائي, &r_border, &r_اونهائي);
	    XQuery_اشارو(XtDisplay(cur), Xt_دري(cur), &پاڙ_q, &ٻار,
			  &پاڙ_x, &پاڙ_y, &win_x, &win_y, &نقاب);

	    إذا (پاڙ == پاڙ_q)
	    {
		إذا ((win_x < 0) || (win_y < 0))
		    أرجع False;

		إذا ((win_x > (صحيح)r_ويڪر) || (win_y > (صحيح)r_اوچائي))
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
	سيٽ_سائيز_2(eb);
    }

    إذا (redraw == True)
    {
	إذا (change)
	    سيٽ_pixmap(eb);
	إذا (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	وإلا
	    eb->label.pixmap = eb->enhancedbutton.عام_pixmap;
	إذا (change)
	    سيٽ_سائيز_2(eb);
	redraw = False;
    }

    أرجع redraw;
}

    ساكن فراغ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#جيڪڏهن_پورو
    XRectangle		    box;
    صحيح			    dx;
    صحيح			    adjust;
    ننڍو		    fill = 0;

    إذا (!XtIsRealized((Widget)eb))
	أرجع;

#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#جيڪڏهن_پورو

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    إذا ((eb->pushbutton.arm_color == eb->primitive.چوٽي_shadow_color)
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
    box.ويڪر  = eb->core.ويڪر - adjust;
    box.اوچائي = eb->core.اوچائي - adjust;

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
	    XFillRectangle(XtDisplay(eb), Xt_دري(eb), gc,
		    box.x, box.y, box.ويڪر, box.اوچائي);
    }

    draw_label(eb, event, region);

    إذا (Lab_IsMenupane(eb))
    {
	إذا (eb->pushbutton.armed)
	    (*(((Xm_ڌڪو_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    وإلا
    {
	adjust = 0;

#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
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

	    حالة Xm_اندروني_HIGHLIGHT:
		اكسر;

	    افتراضي:
		أكد(FALSE);
		أرجع;
	}
#جيڪڏهن_پورو

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	إذا (box.x > adjust)
	{
	    صحيح borderwidth =box.x - adjust;
	    صحيح rectwidth = eb->core.ويڪر - 2 * adjust;
	    صحيح rectheight = eb->core.اوچائي - 2 * adjust;

	    إذا (XmIsManager(Xt_والد(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_دري(eb),
						     Xm_والد_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    وإلا
	    {
		XmeClearBorder(XtDisplay(eb), Xt_دري(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
	    بدل (default_button_emphasis)
	    {
		حالة Xm_اندروني_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    إذا (eb->primitive.highlight_drawn)
			(*(((Xm_ڌڪو_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    اكسر;

		افتراضي:
		    اكسر;
	    }
#جيڪڏهن_پورو
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
		GC	    چوٽي_gc;
		GC	    bottom_gc;
		صحيح	    default_button_shadow_thickness;
		صحيح	    x, y, ويڪر, اوچائي, delta;
		Widget	    والد;

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
		والد = Xt_والد(eb);
		إذا (XmIsManager(والد))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_والد_چوٽي_Shadow_GC(eb);
		    چوٽي_gc = Xm_والد_Bottom_Shadow_GC(eb);
		}
		وإلا
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.چوٽي_shadow_GC;
		    چوٽي_gc = eb->primitive.bottom_shadow_GC;
		}

		إذا ((bottom_gc == None) || (چوٽي_gc == None))
		    أرجع;


		إذا (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		وإلا
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#إذا !defined(LESSTIF_نسخو) && (Xm_نسخو > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		بدل (default_button_emphasis)
		{
		    حالة XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			اكسر;

		    حالة Xm_اندروني_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			اكسر;

		    افتراضي:
			أكد(FALSE);
			أرجع;
		}
#جيڪڏهن_پورو

		x = y = delta;
		ويڪر = eb->core.ويڪر - 2 * delta;
		اوچائي = eb->core.اوچائي - 2 * delta;

		إذا ((ويڪر > 0) && (اوچائي > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_دري(eb),
			    چوٽي_gc, bottom_gc, x, y, ويڪر, اوچائي,
			    default_button_shadow_thickness,
			    (بنانشان)Xm_SHADOW_ٻاهريون);
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

    (*(xm_ڌڪو_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

    ساكن فراغ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ڌڪو_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

#جيڪڏهن_پورو // FEAT_TOOLBAR
