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

#جے_تعریف_شدہ FEAT_TOOLBAR

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
#جے_انت
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
#إذا defined(LESSTIF_نسخہ) || (Xm_نسخہ <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحيح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحيح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, سطر_Solid)
#جے_انت

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#إذا defined(VMS)
# إذا defined(HAVE_XOS_R_H)
#  define XTPROCESS_جندرا Xt_Process_جندرا()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# وإلا
#  define XTPROCESS_جندرا
#  define XTPROCESS_UNLOCK
# endif
#وإلا
# define XTPROCESS_جندرا Xt_Process_جندرا()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#جے_انت

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#جے_بنا_تعریف XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#جے_انت

خارجي Boolean Xm_Get_وچ_Drag_منوال(Widget widget);
خارجي فراغ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجي فراغ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجي فراغ Xm_سیٹ_Focus_Flag(Widget w, بنانشانی صحيح نقاب, Boolean قدر);
خارجي فراغ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ساكن فراغ Destroy(Widget w);
ساكن فراغ Initialize(Widget rq, Widget eb, ArgList دلیلاں, Cardinal *n);
ساكن Boolean سیٹ_اقدار(Widget current, Widget request, Widget new, ArgList دلیلاں, Cardinal *n);
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
#تعریف_کرو stipple_چوڑائی  4
#تعریف_کرو stipple_اچائی 4
ساكن حرف stipple_بٹ[] = { 0x0a, 0x05, 0x0a, 0x05 };
#تعریف_کرو STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

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
	Xm_Npixmap_انکڑے, XmCPixmap, XmRString, حجم(String),
	Xt_وتھ_دا(XmEnhancedButtonRec, enhancedbutton.pixmap_انکڑے),
	XmRImmediate, (Xt_اشارہ_گر) عدم
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, حجم(String),
	Xt_وتھ_دا(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_اشارہ_گر) عدم
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, حجم(Dimension),
	Xt_وتھ_دا(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_اشارہ_گر) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, حجم(صحيح),
	Xt_وتھ_دا(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_اشارہ_گر) Xm_دایاں
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ عدم,
    /* record_type	   */ NULLQUARK,
    /*نسخہ*/ Xm_Primitive_Class_Ext_نسخہ,
    /* record_size	   */ حجم(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ عدم
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ڈالو_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ حجم(XmEnhancedButtonRec),
	/* class_initialize	 */ عدم,
	/* class_part_initialize */ عدم,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ عدم,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_نمبر(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_نمبر(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ سیٹ_اقدار,
	/* set_values_hook	 */ عدم,
	/* set_values_almost	 */ Xt_Inherit_سیٹ_اقدار_Almost,
	/* get_values_hook	 */ عدم,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_نسخہ,
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
	/* extension		 */ (Xt_اشارہ_گر)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_سیٹ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ عدم,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_اشارہ_گر) عدم,
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
    ساكن بنانشانی چھوٹا
bump_color(بنانشانی چھوٹا قدر)
{
    صحيح tmp = 2 * (((صحيح) قدر - 65535) / 3) + 65535;

    أرجع tmp;
}

    ساكن صحيح
alloc_color(Display	*display,
	Colormap	colormap,
	حرف		*colorname,
	XColor		*xcolor,
	فراغ		*گھیرا بن_ورتیا)
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
سیٹ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    کھڑکی	    جڑ;
    ساكن Xpm_Color_علامت color[8] = {
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
    بنانشانی صحيح    اچائی, چوڑائی, border, ڈونگھائی;
    صحيح		    حالت = 0;
    Pixmap	    نقاب;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    بلند_pix = None;
    حرف	    **انکڑے = (حرف **) eb->enhancedbutton.pixmap_انکڑے;
    حرف	    *fname = (حرف *) eb->enhancedbutton.pixmap_file;
    صحيح		    کھسکاؤ;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    إذا (!انکڑے)
	أرجع;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, عدم);

    scr = DefaultScreen(dpy);
    جڑ = جڑ_کھڑکی_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_ڈونگھائی = 0;
    eb->enhancedbutton.pixmap_چوڑائی = 0;
    eb->enhancedbutton.pixmap_اچائی = 0;
    eb->enhancedbutton.عام_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.چوٹی_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.چوٹی_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_نمبر(color);

    إذا (fname)
	حالت = Xpm_Read_File_منزل_Pixmap(dpy, جڑ, fname, &pix, &نقاب, &attr);
    إذا (!fname || حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_انکڑے(dpy, جڑ, انکڑے, &pix, &نقاب, &attr);

    // If something failed, we will fill in the default pixmap.
    إذا (حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_انکڑے(dpy, جڑ, blank_xpm, &pix,
								&نقاب, &attr);

    Xpm_Free_گن(&attr);

    XGetGeometry(dpy, pix, &جڑ, &x, &y, &چوڑائی, &اچائی, &border, &ڈونگھائی);

    // TODO: does the shift depend on label_location somehow?
    کھسکاؤ = eb->primitive.shadow_thickness / 2;

    إذا (کھسکاؤ < 1)
	کھسکاؤ = 1;

    sen_pix = XCreatePixmap(dpy, جڑ, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ, ڈونگھائی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, کھسکاؤ, کھسکاؤ);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, چوڑائی, اچائی, کھسکاؤ, کھسکاؤ);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#جے_تعریف_شدہ XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#وإلا
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#جے_انت
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_نمبر(color);

    حالت = Xpm_Create_Pixmap_From_انکڑے(dpy, جڑ, انکڑے, &pix, عدم, &attr);
    Xpm_Free_گن(&attr);

    بلند_pix = XCreatePixmap(dpy, جڑ, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ, ڈونگھائی);

#إذا 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#وإلا
    XSetForeground(dpy, gc, eb->primitive.چوٹی_shadow_color);
#جے_انت
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, بلند_pix, gc, 0, 0, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, بلند_pix, gc, 0, 0, چوڑائی, اچائی, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ, ڈونگھائی);

    إذا (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    وإلا
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, کھسکاؤ, کھسکاؤ);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 2 * کھسکاؤ, 2 * کھسکاؤ);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, چوڑائی, اچائی, 2 * کھسکاؤ, 2 * کھسکاؤ);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, نقاب);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_کلید;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_لمائی(color);
    attr.color_کلید = XPM_MONO;
    حالت = Xpm_Create_Pixmap_From_انکڑے(dpy, جڑ, انکڑے, &pix, &نقاب, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, جڑ, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ, ڈونگھائی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetForeground(dpy, gc, eb->primitive.چوٹی_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * کھسکاؤ, 2 * کھسکاؤ);
    XFillRectangle(dpy, ins_pix, gc, 2 * کھسکاؤ, 2 * کھسکاؤ, چوڑائی, اچائی);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, کھسکاؤ, کھسکاؤ);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, چوڑائی + کھسکاؤ, اچائی + کھسکاؤ);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_گن(&attr);

    eb->enhancedbutton.pixmap_ڈونگھائی = ڈونگھائی;
    eb->enhancedbutton.pixmap_چوڑائی = چوڑائی;
    eb->enhancedbutton.pixmap_اچائی = اچائی;
    eb->enhancedbutton.عام_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = بلند_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, نقاب);
}

#تعریف_کرو	BUTTON_نقاب ( \
	Button1_نقاب | Button2_نقاب | Button3_نقاب | Button4_نقاب | Button5_نقاب \
)

    ساكن فراغ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		چوٹی_gc;
    GC		bottom_gc;
    Boolean	etched_وچ;

    إذا (!eb->primitive.shadow_thickness)
	أرجع;

    إذا ((eb->core.چوڑائی <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.اچائی <= 2 * eb->primitive.highlight_thickness))
	أرجع;

#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_وچ = dpy->display.enable_etched_وچ_menu;
    }
#وإلا
    etched_وچ = False;
#جے_انت
    إذا (!etched_وچ ^ eb->pushbutton.armed)
    {
	چوٹی_gc = eb->primitive.چوٹی_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    وإلا
    {
	چوٹی_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.چوٹی_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_کھڑکی(eb),
	    چوٹی_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.چوڑائی - 2 * eb->primitive.highlight_thickness,
	    eb->core.اچائی - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (بنانشانی)(etched_وچ ? Xm_SHADOW_وچ : Xm_SHADOW_باہرلا));
}

    ساكن فراغ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    إذا (!Xt_چوڑائی(eb) || !Xt_اچائی(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_چوڑائی(eb), Xt_اچائی(eb),
	    eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    إذا (!Xt_چوڑائی(eb) || !Xt_اچائی(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    إذا (XmIsManager(eb->core.ماپا))
    {
#جے_تعریف_شدہ UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	إذا (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_اشارہ_گر)
			    XtClass(eb->core.ماپا), XmQTspecifyUnhighlight))
		    != عدم) && (UnhighlightT->getUnhighlightGC != عدم))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.ماپا, (Widget) eb);
	}
	وإلا
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.ماپا))->manager.background_GC;
	}
#وإلا
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.ماپا))->manager.background_GC;
#جے_انت
	XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
			 manager_background_GC,
			 0, 0, Xt_چوڑائی(eb), Xt_اچائی(eb),
			 eb->primitive.highlight_thickness);
	إذا (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.چوڑائی - 2 * eb->primitive.highlight_thickness,
		    eb->core.اچائی - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    وإلا
	XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb), 0, 0, Xt_چوڑائی(eb),
		       Xt_اچائی(eb), eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event بن_ورتیا,
	    Region region بن_ورتیا)
{
    Pixmap	pix;
    GC		gc = eb->label.عام_GC;
    صحيح		ڈونگھائی;
    Cardinal	چوڑائی;
    Cardinal	اچائی;
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

    إذا (pix == None || !eb->enhancedbutton.pixmap_انکڑے)
	أرجع;

    ڈونگھائی = eb->enhancedbutton.pixmap_ڈونگھائی;
    w = eb->enhancedbutton.pixmap_چوڑائی;
    h = eb->enhancedbutton.pixmap_اچائی;

    gc = eb->label.عام_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_چوڑائی;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_اچائی;
    چوڑائی = eb->core.چوڑائی - 2 * x;
    إذا (w < چوڑائی)
	چوڑائی = w;
    اچائی = eb->core.اچائی - 2 * y;
    إذا (h < اچائی)
	اچائی = h;
    إذا (ڈونگھائی == (صحيح)eb->core.ڈونگھائی)
	XCopyArea(XtDisplay(eb), pix, Xt_کھڑکی(eb), gc, 0, 0,
		چوڑائی, اچائی, x, y);
    وإلا إذا (ڈونگھائی == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_کھڑکی(eb), gc, 0, 0,
		چوڑائی, اچائی, x, y, (بنانشانی لما)1);
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
#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_وچ = dpy->display.enable_etched_وچ_menu;
#وإلا
    Boolean	etched_وچ = False;
#جے_انت

    إذا (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_وچ)))
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
	Lab_Margin_بایاں(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_دایاں(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٹی(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_جندرا;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    إذا (deadjusted)
    {
	Lab_Margin_بایاں(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_دایاں(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٹی(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    إذا (replaceGC)
	eb->label.عام_GC = tmp_gc;
}

    ساكن فراغ
Enter(Widget wid,
    XEvent *event,
    String *params بن_ورتیا,
    Cardinal *عدد_params بن_ورتیا)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ڈالو_Button_Callback_Struct call_قدر;

    إذا (Lab_IsMenupane(eb))
    {
	إذا ((((ShellWidget) Xt_ماپا(Xt_ماپا(eb)))->shell.popped_اتے)
		&& Xm_Get_وچ_Drag_منوال((Widget) eb))
	{
#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_وچ = dpy->display.enable_etched_وچ_menu;
#وإلا
	    Boolean etched_وچ = False;
#جے_انت

	    إذا (eb->pushbutton.armed)
		أرجع;

	    // ...so KHelp event is delivered correctly.
	    Xm_سیٹ_Focus_Flag(Xt_ماپا(Xt_ماپا(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_سیٹ_Keyboard_Focus(Xt_ماپا(Xt_ماپا(eb)), (Widget) eb);
	    Xm_سیٹ_Focus_Flag(Xt_ماپا(Xt_ماپا(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_ماپا(wid))->manager.سرگرم_بچہ = wid;

	    // etched in menu button
	    إذا (etched_وچ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.چوڑائی, eb->core.اچائی);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }

	    إذا ((eb->core.چوڑائی > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.اچائی >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_کھڑکی(eb),
			eb->primitive.چوٹی_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.چوڑائی - 2 * eb->primitive.highlight_thickness,
			eb->core.اچائی - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(بنانشانی)(etched_وچ ? Xm_SHADOW_وچ : Xm_SHADOW_باہرلا));
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
	    XTPROCESS_جندرا;
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
    String *params بن_ورتیا,
    Cardinal *عدد_params بن_ورتیا)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ڈالو_Button_Callback_Struct call_قدر;

    إذا (Lab_IsMenupane(eb))
    {
#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_وچ = dpy->display.enable_etched_وچ_menu;
#وإلا
	Boolean etched_وچ = False;
#جے_انت

	إذا (Xm_Get_وچ_Drag_منوال((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.منوال == Notify_عام))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_ماپا(wid))->manager.سرگرم_بچہ = عدم;

	    إذا (etched_وچ && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.چوڑائی, eb->core.اچائی);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }
	    وإلا
		XmeClearBorder
		    (XtDisplay(eb), Xt_کھڑکی(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.چوڑائی -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.اچائی -
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
	    XTPROCESS_جندرا;
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

#تعریف_کرو IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ساكن فراغ
سیٹ_ناپ_2(XmEnhancedButtonWidget newtb)
{
    بنانشانی صحيح w = 0;
    بنانشانی صحيح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    إذا (newtb->enhancedbutton.pixmap_انکڑے
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.عام_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_چوڑائی;
	h = newtb->enhancedbutton.pixmap_اچائی;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    إذا (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_بایاں = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_چوڑائی;
    }
    وإلا
    {
	newtb->label.margin_بایاں = w;
	newtb->core.چوڑائی = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_چوڑائی)
	    + newtb->label.متن_Rect.چوڑائی;

	إذا (newtb->label.متن_Rect.چوڑائی > 0)
	{
	    newtb->label.margin_بایاں += newtb->label.margin_چوڑائی
					  + newtb->primitive.shadow_thickness;
	    newtb->core.چوڑائی += newtb->label.margin_چوڑائی
					  + newtb->primitive.shadow_thickness;
	}
    }
    إذا (newtb->label.متن_Rect.اچائی < h)
    {
	newtb->core.اچائی = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_اچائی);
    }
    وإلا
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#إذا 0
    لکھو("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.عام_pixmap,
	    h, newtb->core.اچائی,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_اچائی,
	    newtb->core.چوڑائی,
	    newtb->core.اچائی);
#جے_انت

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_جندرا;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ساكن فراغ
Initialize(Widget rq, Widget ebw, ArgList دلیلاں بن_ورتیا, Cardinal *n بن_ورتیا)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_جندرا;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    إذا (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	کھڑکی	جڑ_W = Default_جڑ_کھڑکی(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_انکڑے(dpy, جڑ_W, stipple_بٹ,
		stipple_چوڑائی, stipple_اچائی);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    إذا (eb->enhancedbutton.pixmap_انکڑے)
    {
	XmString str;
	سیٹ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_سیٹ_اقدار((Widget) eb, XmNlabelString, str, عدم);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.عام_pixmap;

    إذا (request->core.چوڑائی == 0)
	eb->core.چوڑائی = 0;
    إذا (request->core.اچائی == 0)
	eb->core.اچائی = 0;
    سیٹ_ناپ_2(eb);

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
    Pixmap بلند_pix = eb->enhancedbutton.highlight_pixmap;

    إذا (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    إذا (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    إذا (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    إذا (بلند_pix != None && بلند_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), بلند_pix);
}

    ساكن فراغ
Destroy(Widget w)
{
    إذا (!XmIsEnhancedButton(w))
	أرجع;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ساكن Boolean
سیٹ_اقدار(Widget current,
	  Widget request بن_ورتیا,
	  Widget new,
	  ArgList دلیلاں بن_ورتیا,
	  Cardinal *n بن_ورتیا)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#تعریف_کرو NOT_EQUAL(میدان)       (cur->میدان != eb->میدان)

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
	    بنانشانی صحيح    r_اچائی;
	    بنانشانی صحيح    r_چوڑائی;
	    بنانشانی صحيح    r_border;
	    بنانشانی صحيح    r_ڈونگھائی;
	    صحيح		    جڑ_x;
	    صحيح		    جڑ_y;
	    صحيح		    win_x;
	    صحيح		    win_y;
	    کھڑکی	    جڑ;
	    کھڑکی	    جڑ_q;
	    کھڑکی	    بچہ;
	    بنانشانی صحيح    نقاب;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_کھڑکی(cur), &جڑ, &r_x, &r_y, &r_چوڑائی,
			 &r_اچائی, &r_border, &r_ڈونگھائی);
	    XQuery_اشارہ_گر(XtDisplay(cur), Xt_کھڑکی(cur), &جڑ_q, &بچہ,
			  &جڑ_x, &جڑ_y, &win_x, &win_y, &نقاب);

	    إذا (جڑ == جڑ_q)
	    {
		إذا ((win_x < 0) || (win_y < 0))
		    أرجع False;

		إذا ((win_x > (صحيح)r_چوڑائی) || (win_y > (صحيح)r_اچائی))
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
	سیٹ_ناپ_2(eb);
    }

    إذا (redraw == True)
    {
	إذا (change)
	    سیٹ_pixmap(eb);
	إذا (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	وإلا
	    eb->label.pixmap = eb->enhancedbutton.عام_pixmap;
	إذا (change)
	    سیٹ_ناپ_2(eb);
	redraw = False;
    }

    أرجع redraw;
}

    ساكن فراغ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#جے_انت
    XRectangle		    box;
    صحيح			    dx;
    صحيح			    adjust;
    چھوٹا		    fill = 0;

    إذا (!XtIsRealized((Widget)eb))
	أرجع;

#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#جے_انت

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    إذا ((eb->pushbutton.arm_color == eb->primitive.چوٹی_shadow_color)
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
    box.چوڑائی  = eb->core.چوڑائی - adjust;
    box.اچائی = eb->core.اچائی - adjust;

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
	    XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb), gc,
		    box.x, box.y, box.چوڑائی, box.اچائی);
    }

    draw_label(eb, event, region);

    إذا (Lab_IsMenupane(eb))
    {
	إذا (eb->pushbutton.armed)
	    (*(((Xm_ڈالو_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    وإلا
    {
	adjust = 0;

#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
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

	    حالة Xm_اندرونی_HIGHLIGHT:
		اكسر;

	    افتراضي:
		أكد(FALSE);
		أرجع;
	}
#جے_انت

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	إذا (box.x > adjust)
	{
	    صحيح borderwidth =box.x - adjust;
	    صحيح rectwidth = eb->core.چوڑائی - 2 * adjust;
	    صحيح rectheight = eb->core.اچائی - 2 * adjust;

	    إذا (XmIsManager(Xt_ماپا(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
						     Xm_ماپا_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    وإلا
	    {
		XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	    بدل (default_button_emphasis)
	    {
		حالة Xm_اندرونی_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    إذا (eb->primitive.highlight_drawn)
			(*(((Xm_ڈالو_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    اكسر;

		افتراضي:
		    اكسر;
	    }
#جے_انت
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
		GC	    چوٹی_gc;
		GC	    bottom_gc;
		صحيح	    default_button_shadow_thickness;
		صحيح	    x, y, چوڑائی, اچائی, delta;
		Widget	    ماپا;

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
		ماپا = Xt_ماپا(eb);
		إذا (XmIsManager(ماپا))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_ماپا_چوٹی_Shadow_GC(eb);
		    چوٹی_gc = Xm_ماپا_Bottom_Shadow_GC(eb);
		}
		وإلا
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.چوٹی_shadow_GC;
		    چوٹی_gc = eb->primitive.bottom_shadow_GC;
		}

		إذا ((bottom_gc == None) || (چوٹی_gc == None))
		    أرجع;


		إذا (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		وإلا
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#إذا !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		بدل (default_button_emphasis)
		{
		    حالة XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			اكسر;

		    حالة Xm_اندرونی_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			اكسر;

		    افتراضي:
			أكد(FALSE);
			أرجع;
		}
#جے_انت

		x = y = delta;
		چوڑائی = eb->core.چوڑائی - 2 * delta;
		اچائی = eb->core.اچائی - 2 * delta;

		إذا ((چوڑائی > 0) && (اچائی > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_کھڑکی(eb),
			    چوٹی_gc, bottom_gc, x, y, چوڑائی, اچائی,
			    default_button_shadow_thickness,
			    (بنانشانی)Xm_SHADOW_باہرلا);
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

    (*(xm_ڈالو_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

    ساكن فراغ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ڈالو_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

#جے_انت // FEAT_TOOLBAR
