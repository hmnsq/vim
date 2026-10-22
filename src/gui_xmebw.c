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

#شامل "vim.h"

#اگر_تعریف_شدہ FEAT_TOOLBAR

#شامل <Xm/XmP.h>
#شامل <Xm/DrawP.h>
#اگر defined(HAVE_XM_TRAITP_H) && defined(HAVE_XM_MANAGER_H) \
	&& defined(HAVE_XM_UNHIGHLIGHTT_H) && defined(HAVE_XM_XPMP_H)
# include <Xm/TraitP.h>
# include <Xm/Manager.h>
# include <Xm/UnhighlightT.h>
# include <Xm/XpmP.h>
# define UNHIGHLIGHTT
#ورنہ
# ifdef HAVE_X11_XPM_H
#  ifdef VMS
#   include <xpm.h>
#  ورنہ
#   include <X11/xpm.h>
#  endif
# endif
#اگر_ختم
#شامل <Xm/ManagerP.h>
#شامل <Xm/Display.h>
#شامل <Xm/DisplayP.h>

#شامل <X11/Shell.h>
#شامل <X11/ShellP.h>

#شامل "gui_xmebwp.h"

// Provide some missing wrappers, which are missed from the LessTif
// implementation.  Also missing in Motif 1.2 and earlier.
//
// We neither use XmeGetPixmapData or _XmGetPixmapData, since with LessTif the
// pixmap will not appear in its caches properly. We cache the interesting
// values in XmEnhancedButtonPart instead ourself.
#اگر defined(LESSTIF_نسخہ) || (Xm_نسخہ <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحیح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحیح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, سطر_Solid)
#اگر_ختم

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#اگر defined(VMS)
# اگر defined(HAVE_XOS_R_H)
#  define XTPROCESS_تالا Xt_Process_تالا()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# ورنہ
#  define XTPROCESS_تالا
#  define XTPROCESS_UNLOCK
# endif
#ورنہ
# define XTPROCESS_تالا Xt_Process_تالا()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#اگر_ختم

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#اگر_غیر_تعریف_شدہ XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#اگر_ختم

خارجی Boolean Xm_Get_اندر_Drag_منوال(Widget widget);
خارجی خالی _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجی خالی _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجی خالی Xm_سیٹ_Focus_Flag(Widget w, غیرعلامتی صحیح نقاب, Boolean قدر);
خارجی خالی _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ساکن خالی Destroy(Widget w);
ساکن خالی Initialize(Widget rq, Widget eb, ArgList دلائل, Cardinal *n);
ساکن Boolean سیٹ_اقدار(Widget current, Widget request, Widget new, ArgList دلائل, Cardinal *n);
ساکن خالی Redisplay(Widget, XEvent *, Region);

/*
 * Declaration of action methods.
 */
ساکن خالی Enter(Widget, XEvent *, String *, Cardinal *);
ساکن خالی Leave(Widget, XEvent *, String *, Cardinal *);
ساکن خالی BorderHighlight(Widget);
ساکن خالی BorderUnhighlight(Widget);

/*
 * 4 x 4 stipple for desensitized widgets
 */
#تعریف_کرو stipple_چوڑائی  4
#تعریف_کرو stipple_اونچائی 4
ساکن حرف stipple_بٹ[] = { 0x0a, 0x05, 0x0a, 0x05 };
#تعریف_کرو STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

/*
 * Override actions.
 */
ساکن XtActionsRec actionsList[] =
{
    {"Enter", Enter},
    {"Leave", Leave},
};

ساکن XtResource resources[] =
{
    {
	Xm_Npixmap_ڈیٹا, XmCPixmap, XmRString, حجم(String),
	Xt_انحراف_کا(XmEnhancedButtonRec, enhancedbutton.pixmap_ڈیٹا),
	XmRImmediate, (Xt_اشارہ_گر) تہی
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, حجم(String),
	Xt_انحراف_کا(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_اشارہ_گر) تہی
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, حجم(Dimension),
	Xt_انحراف_کا(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_اشارہ_گر) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, حجم(صحیح),
	Xt_انحراف_کا(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_اشارہ_گر) Xm_دایاں
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ تہی,
    /* record_type	   */ NULLQUARK,
    /*نسخہ*/ Xm_Primitive_Class_Ext_نسخہ,
    /* record_size	   */ حجم(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ تہی
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ڈالو_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ حجم(XmEnhancedButtonRec),
	/* class_initialize	 */ تہی,
	/* class_part_initialize */ تہی,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ تہی,
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
	/* set_values_hook	 */ تہی,
	/* set_values_almost	 */ Xt_Inherit_سیٹ_اقدار_Almost,
	/* get_values_hook	 */ تہی,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_نسخہ,
	/* callback_private	 */ تہی,
	/* tm_table		 */ تہی,
	/* query_geometry	 */ تہی,
	/* display_accelerator	 */ XtInheritDisplayAccelerator,
	/* extension		 */ تہی
    },

    // primitive_class fields
    {
	/* border highlight	 */ BorderHighlight,
	/* border_unhighlight	 */ BorderUnhighlight,
	/* translations		 */ XtInheritTranslations,
	/* arm and activate	 */ XmInheritArmAndActivate,
	/* synthetic resources	 */ تہی,
	/* number of syn res	 */ 0,
	/* extension		 */ (Xt_اشارہ_گر)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_سیٹ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ تہی,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_اشارہ_گر) تہی,
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
    ساکن غیرعلامتی مختصر
bump_color(غیرعلامتی مختصر قدر)
{
    صحیح tmp = 2 * (((صحیح) قدر - 65535) / 3) + 65535;

    واپس tmp;
}

    ساکن صحیح
alloc_color(Display	*display,
	Colormap	colormap,
	حرف		*colorname,
	XColor		*xcolor,
	خالی		*احاطہ غیر_مستعمل)
{
    صحیح حالت_2;

    اگر (colorname)
	اگر (!XParseColor(display, colormap, colorname, xcolor))
	    واپس -1;

    xcolor->red = bump_color(xcolor->red);
    xcolor->green = bump_color(xcolor->green);
    xcolor->blue = bump_color(xcolor->blue);

    حالت_2 = XAllocColor(display, colormap, xcolor);
    واپس حالت_2 != 0 ? 1 : 0;
}

// XPM
ساکن حرف * blank_xpm[] =
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
    ساکن خالی
سیٹ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    کھڑکی	    جڑ;
    ساکن Xpm_Color_علامت color[8] = {
	{"none", "none", 0},
	{"None", "none", 0},
	{"background", تہی, 0},
	{"foreground", تہی, 0},
	{"bottomShadowColor", تہی, 0},
	{"topShadowColor", تہی, 0},
	{"highlightColor", تہی, 0},
	{"armColor", تہی, 0}
    };
    صحیح		    scr;
    Display	    *dpy = XtDisplay(eb);
    صحیح		    x;
    صحیح		    y;
    غیرعلامتی صحیح    اونچائی, چوڑائی, border, گہرائی;
    صحیح		    حالت_2 = 0;
    Pixmap	    نقاب;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    بلند_pix = None;
    حرف	    **ڈیٹا = (حرف **) eb->enhancedbutton.pixmap_ڈیٹا;
    حرف	    *fname = (حرف *) eb->enhancedbutton.pixmap_file;
    صحیح		    سرکاؤ;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    اگر (!ڈیٹا)
	واپس;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, تہی);

    scr = DefaultScreen(dpy);
    جڑ = جڑ_کھڑکی_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_گہرائی = 0;
    eb->enhancedbutton.pixmap_چوڑائی = 0;
    eb->enhancedbutton.pixmap_اونچائی = 0;
    eb->enhancedbutton.معمول_pixmap = None;
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

    اگر (fname)
	حالت_2 = Xpm_Read_File_مقصد_Pixmap(dpy, جڑ, fname, &pix, &نقاب, &attr);
    اگر (!fname || حالت_2 != XpmSuccess)
	حالت_2 = Xpm_Create_Pixmap_From_ڈیٹا(dpy, جڑ, ڈیٹا, &pix, &نقاب, &attr);

    // If something failed, we will fill in the default pixmap.
    اگر (حالت_2 != XpmSuccess)
	حالت_2 = Xpm_Create_Pixmap_From_ڈیٹا(dpy, جڑ, blank_xpm, &pix,
								&نقاب, &attr);

    Xpm_Free_اوصاف(&attr);

    XGetGeometry(dpy, pix, &جڑ, &x, &y, &چوڑائی, &اونچائی, &border, &گہرائی);

    // TODO: does the shift depend on label_location somehow?
    سرکاؤ = eb->primitive.shadow_thickness / 2;

    اگر (سرکاؤ < 1)
	سرکاؤ = 1;

    sen_pix = XCreatePixmap(dpy, جڑ, چوڑائی + سرکاؤ, اونچائی + سرکاؤ, گہرائی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, چوڑائی + سرکاؤ, اونچائی + سرکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, سرکاؤ, سرکاؤ);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, چوڑائی, اونچائی, سرکاؤ, سرکاؤ);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#اگر_تعریف_شدہ XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#ورنہ
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#اگر_ختم
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_نمبر(color);

    حالت_2 = Xpm_Create_Pixmap_From_ڈیٹا(dpy, جڑ, ڈیٹا, &pix, تہی, &attr);
    Xpm_Free_اوصاف(&attr);

    بلند_pix = XCreatePixmap(dpy, جڑ, چوڑائی + سرکاؤ, اونچائی + سرکاؤ, گہرائی);

#اگر 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#ورنہ
    XSetForeground(dpy, gc, eb->primitive.چوٹی_shadow_color);
#اگر_ختم
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, بلند_pix, gc, 0, 0, چوڑائی + سرکاؤ, اونچائی + سرکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, بلند_pix, gc, 0, 0, چوڑائی, اونچائی, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, چوڑائی + سرکاؤ, اونچائی + سرکاؤ, گہرائی);

    اگر (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    ورنہ
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, سرکاؤ, سرکاؤ);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, چوڑائی + سرکاؤ, اونچائی + سرکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetClipOrigin(dpy, gc, 2 * سرکاؤ, 2 * سرکاؤ);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, چوڑائی, اونچائی, 2 * سرکاؤ, 2 * سرکاؤ);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, نقاب);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_کلید;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_لمبائی(color);
    attr.color_کلید = XPM_MONO;
    حالت_2 = Xpm_Create_Pixmap_From_ڈیٹا(dpy, جڑ, ڈیٹا, &pix, &نقاب, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, جڑ, چوڑائی + سرکاؤ, اونچائی + سرکاؤ, گہرائی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_نقاب(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, چوڑائی + سرکاؤ, اونچائی + سرکاؤ);
    XSet_Clip_نقاب(dpy, gc, نقاب);
    XSetForeground(dpy, gc, eb->primitive.چوٹی_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * سرکاؤ, 2 * سرکاؤ);
    XFillRectangle(dpy, ins_pix, gc, 2 * سرکاؤ, 2 * سرکاؤ, چوڑائی, اونچائی);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, سرکاؤ, سرکاؤ);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, چوڑائی + سرکاؤ, اونچائی + سرکاؤ);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_اوصاف(&attr);

    eb->enhancedbutton.pixmap_گہرائی = گہرائی;
    eb->enhancedbutton.pixmap_چوڑائی = چوڑائی;
    eb->enhancedbutton.pixmap_اونچائی = اونچائی;
    eb->enhancedbutton.معمول_pixmap = sen_pix;
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

    ساکن خالی
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		چوٹی_gc;
    GC		bottom_gc;
    Boolean	etched_اندر;

    اگر (!eb->primitive.shadow_thickness)
	واپس;

    اگر ((eb->core.چوڑائی <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.اونچائی <= 2 * eb->primitive.highlight_thickness))
	واپس;

#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_اندر = dpy->display.enable_etched_اندر_menu;
    }
#ورنہ
    etched_اندر = False;
#اگر_ختم
    اگر (!etched_اندر ^ eb->pushbutton.armed)
    {
	چوٹی_gc = eb->primitive.چوٹی_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    ورنہ
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
	    eb->core.اونچائی - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (غیرعلامتی)(etched_اندر ? Xm_SHADOW_اندر : Xm_SHADOW_برآمد));
}

    ساکن خالی
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    اگر (!Xt_چوڑائی(eb) || !Xt_اونچائی(eb) || !eb->primitive.highlight_thickness)
	واپس;

    XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_چوڑائی(eb), Xt_اونچائی(eb),
	    eb->primitive.highlight_thickness);
}

    ساکن خالی
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    اگر (!Xt_چوڑائی(eb) || !Xt_اونچائی(eb) || !eb->primitive.highlight_thickness)
	واپس;

    اگر (XmIsManager(eb->core.والد))
    {
#اگر_تعریف_شدہ UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	اگر (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_اشارہ_گر)
			    XtClass(eb->core.والد), XmQTspecifyUnhighlight))
		    != تہی) && (UnhighlightT->getUnhighlightGC != تہی))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.والد, (Widget) eb);
	}
	ورنہ
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.والد))->manager.background_GC;
	}
#ورنہ
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.والد))->manager.background_GC;
#اگر_ختم
	XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
			 manager_background_GC,
			 0, 0, Xt_چوڑائی(eb), Xt_اونچائی(eb),
			 eb->primitive.highlight_thickness);
	اگر (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.چوڑائی - 2 * eb->primitive.highlight_thickness,
		    eb->core.اونچائی - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    ورنہ
	XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb), 0, 0, Xt_چوڑائی(eb),
		       Xt_اونچائی(eb), eb->primitive.highlight_thickness);
}

    ساکن خالی
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event غیر_مستعمل,
	    Region region غیر_مستعمل)
{
    Pixmap	pix;
    GC		gc = eb->label.معمول_GC;
    صحیح		گہرائی;
    Cardinal	چوڑائی;
    Cardinal	اونچائی;
    Cardinal	w;
    Cardinal	h;
    صحیح		x;
    صحیح		y;

    اگر (!XtIsSensitive((Widget) eb))
	pix = eb->enhancedbutton.insensitive_pixmap;
    ورنہ
    {
	اگر (eb->primitive.highlighted && !eb->pushbutton.armed)
	    pix = eb->enhancedbutton.highlight_pixmap;
	ورنہ اگر (eb->pushbutton.armed)
	    pix = eb->enhancedbutton.armed_pixmap;
	ورنہ
	    pix = eb->enhancedbutton.معمول_pixmap;
    }

    اگر (pix == None || !eb->enhancedbutton.pixmap_ڈیٹا)
	واپس;

    گہرائی = eb->enhancedbutton.pixmap_گہرائی;
    w = eb->enhancedbutton.pixmap_چوڑائی;
    h = eb->enhancedbutton.pixmap_اونچائی;

    gc = eb->label.معمول_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_چوڑائی;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_اونچائی;
    چوڑائی = eb->core.چوڑائی - 2 * x;
    اگر (w < چوڑائی)
	چوڑائی = w;
    اونچائی = eb->core.اونچائی - 2 * y;
    اگر (h < اونچائی)
	اونچائی = h;
    اگر (گہرائی == (صحیح)eb->core.گہرائی)
	XCopyArea(XtDisplay(eb), pix, Xt_کھڑکی(eb), gc, 0, 0,
		چوڑائی, اونچائی, x, y);
    ورنہ اگر (گہرائی == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_کھڑکی(eb), gc, 0, 0,
		چوڑائی, اونچائی, x, y, (غیرعلامتی طویل)1);
}

/*
 * Draw the label contained in the pushbutton.
 */
    ساکن خالی
draw_label(XmEnhancedButtonWidget eb, XEvent *event, Region region)
{
    GC		tmp_gc = تہی;
    Boolean	replaceGC = False;
    Boolean	deadjusted = False;
#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_اندر = dpy->display.enable_etched_اندر_menu;
#ورنہ
    Boolean	etched_اندر = False;
#اگر_ختم

    اگر (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_اندر)))
    {
	اگر (eb->label.label_type == (صحیح)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.معمول_GC;
	    eb->label.معمول_GC = eb->pushbutton.background_gc;
	    replaceGC = True;
	}
    }

    /*
     * If the button contains a labeled pixmap, we will take it instead of our
     * own pixmap.
     */

    اگر (eb->label.label_type == (صحیح)XmPIXMAP)
    {
	اگر (eb->pushbutton.armed)
	{
	    اگر (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
		eb->label.pixmap = eb->pushbutton.arm_pixmap;
	    ورنہ
		eb->label.pixmap = eb->pushbutton.unarm_pixmap;
	}
	ورنہ
	    // pushbutton is not armed
	    eb->label.pixmap = eb->pushbutton.unarm_pixmap;
    }

    /*
     *	Temporarily remove the Xm3D_ENHANCE_PIXEL hack ("adjustment") from the
     *	margin values, so we don't confuse Label.
     */
    اگر (eb->pushbutton.default_button_shadow_thickness > 0)
    {
	deadjusted = True;
	Lab_Margin_بایاں(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_دایاں(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٹی(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_تالا;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    اگر (deadjusted)
    {
	Lab_Margin_بایاں(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_دایاں(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_چوٹی(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    اگر (replaceGC)
	eb->label.معمول_GC = tmp_gc;
}

    ساکن خالی
Enter(Widget wid,
    XEvent *event,
    String *params غیر_مستعمل,
    Cardinal *عدد_params غیر_مستعمل)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ڈالو_Button_Callback_Struct call_قدر;

    اگر (Lab_IsMenupane(eb))
    {
	اگر ((((ShellWidget) Xt_والد(Xt_والد(eb)))->shell.popped_اوپر)
		&& Xm_Get_اندر_Drag_منوال((Widget) eb))
	{
#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_اندر = dpy->display.enable_etched_اندر_menu;
#ورنہ
	    Boolean etched_اندر = False;
#اگر_ختم

	    اگر (eb->pushbutton.armed)
		واپس;

	    // ...so KHelp event is delivered correctly.
	    Xm_سیٹ_Focus_Flag(Xt_والد(Xt_والد(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_سیٹ_Keyboard_Focus(Xt_والد(Xt_والد(eb)), (Widget) eb);
	    Xm_سیٹ_Focus_Flag(Xt_والد(Xt_والد(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_والد(wid))->manager.فعال_بچہ = wid;

	    // etched in menu button
	    اگر (etched_اندر && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.چوڑائی, eb->core.اونچائی);
		draw_label(eb, event, تہی);
		draw_pixmap(eb, event, تہی);
	    }

	    اگر ((eb->core.چوڑائی > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.اونچائی >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_کھڑکی(eb),
			eb->primitive.چوٹی_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.چوڑائی - 2 * eb->primitive.highlight_thickness,
			eb->core.اونچائی - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(غیرعلامتی)(etched_اندر ? Xm_SHADOW_اندر : Xm_SHADOW_برآمد));
	    }

	    اگر (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_قدر.reason = (صحیح)XmCR_ARM;
		call_قدر.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_قدر);
	    }
	}
    }
    ورنہ
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, تہی, تہی);
	اگر (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_تالا;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region) تہی);
	}

	draw_highlight(eb);
	draw_shadows(eb);
	draw_pixmap(eb, event, تہی);
    }
}

    ساکن خالی
Leave(Widget wid,
    XEvent *event,
    String *params غیر_مستعمل,
    Cardinal *عدد_params غیر_مستعمل)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ڈالو_Button_Callback_Struct call_قدر;

    اگر (Lab_IsMenupane(eb))
    {
#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_اندر = dpy->display.enable_etched_اندر_menu;
#ورنہ
	Boolean etched_اندر = False;
#اگر_ختم

	اگر (Xm_Get_اندر_Drag_منوال((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.منوال == Notify_معمول))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_والد(wid))->manager.فعال_بچہ = تہی;

	    اگر (etched_اندر && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.چوڑائی, eb->core.اونچائی);
		draw_label(eb, event, تہی);
		draw_pixmap(eb, event, تہی);
	    }
	    ورنہ
		XmeClearBorder
		    (XtDisplay(eb), Xt_کھڑکی(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.چوڑائی -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.اونچائی -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    اگر (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_قدر.reason = (صحیح)XmCR_DISARM;
		call_قدر.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_قدر);
	    }
	}
    }
    ورنہ
    {
	_XmPrimitiveLeave((Widget) eb, event, تہی, تہی);

	اگر (eb->pushbutton.armed == TRUE)
	{
	    XtExposeProc expose;
	    eb->pushbutton.armed = FALSE;
	    XTPROCESS_تالا;
	    expose = XtClass(eb)->core_class.expose;
	    XTPROCESS_UNLOCK;
	    (*expose) (wid, event, (Region)تہی);
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, تہی);
	    eb->pushbutton.armed = TRUE;
	}
	ورنہ
	{
	    draw_unhighlight(eb);
	    draw_pixmap(eb, event, تہی);
	}
    }
}

#تعریف_کرو IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ساکن خالی
سیٹ_سائز_2(XmEnhancedButtonWidget newtb)
{
    غیرعلامتی صحیح w = 0;
    غیرعلامتی صحیح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    اگر (newtb->enhancedbutton.pixmap_ڈیٹا
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.معمول_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_چوڑائی;
	h = newtb->enhancedbutton.pixmap_اونچائی;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    اگر (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_بایاں = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_چوڑائی;
    }
    ورنہ
    {
	newtb->label.margin_بایاں = w;
	newtb->core.چوڑائی = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_چوڑائی)
	    + newtb->label.متن_Rect.چوڑائی;

	اگر (newtb->label.متن_Rect.چوڑائی > 0)
	{
	    newtb->label.margin_بایاں += newtb->label.margin_چوڑائی
					  + newtb->primitive.shadow_thickness;
	    newtb->core.چوڑائی += newtb->label.margin_چوڑائی
					  + newtb->primitive.shadow_thickness;
	}
    }
    اگر (newtb->label.متن_Rect.اونچائی < h)
    {
	newtb->core.اونچائی = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_اونچائی);
    }
    ورنہ
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#اگر 0
    لکھو("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.معمول_pixmap,
	    h, newtb->core.اونچائی,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_اونچائی,
	    newtb->core.چوڑائی,
	    newtb->core.اونچائی);
#اگر_ختم

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_تالا;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ساکن خالی
Initialize(Widget rq, Widget ebw, ArgList دلائل غیر_مستعمل, Cardinal *n غیر_مستعمل)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_تالا;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    اگر (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	کھڑکی	جڑ_W = Default_جڑ_کھڑکی(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_ڈیٹا(dpy, جڑ_W, stipple_بٹ,
		stipple_چوڑائی, stipple_اونچائی);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    اگر (eb->enhancedbutton.pixmap_ڈیٹا)
    {
	XmString str;
	سیٹ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_سیٹ_اقدار((Widget) eb, XmNlabelString, str, تہی);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.معمول_pixmap;

    اگر (request->core.چوڑائی == 0)
	eb->core.چوڑائی = 0;
    اگر (request->core.اونچائی == 0)
	eb->core.اونچائی = 0;
    سیٹ_سائز_2(eb);

    (* resize)((Widget)eb);
}

    ساکن خالی
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.معمول_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap بلند_pix = eb->enhancedbutton.highlight_pixmap;

    اگر (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    اگر (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    اگر (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    اگر (بلند_pix != None && بلند_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), بلند_pix);
}

    ساکن خالی
Destroy(Widget w)
{
    اگر (!XmIsEnhancedButton(w))
	واپس;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ساکن Boolean
سیٹ_اقدار(Widget current,
	  Widget request غیر_مستعمل,
	  Widget new,
	  ArgList دلائل غیر_مستعمل,
	  Cardinal *n غیر_مستعمل)
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
    اگر (NOT_EQUAL(core.sensitive) && !Lab_IsMenupane(current))
    {
	اگر (cur->core.sensitive == True)
	{
	    draw_unhighlight(eb);
	}
	ورنہ
	{
	    صحیح		    r_x;
	    صحیح		    r_y;
	    غیرعلامتی صحیح    r_اونچائی;
	    غیرعلامتی صحیح    r_چوڑائی;
	    غیرعلامتی صحیح    r_border;
	    غیرعلامتی صحیح    r_گہرائی;
	    صحیح		    جڑ_x;
	    صحیح		    جڑ_y;
	    صحیح		    win_x;
	    صحیح		    win_y;
	    کھڑکی	    جڑ;
	    کھڑکی	    جڑ_q;
	    کھڑکی	    بچہ;
	    غیرعلامتی صحیح    نقاب;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_کھڑکی(cur), &جڑ, &r_x, &r_y, &r_چوڑائی,
			 &r_اونچائی, &r_border, &r_گہرائی);
	    XQuery_اشارہ_گر(XtDisplay(cur), Xt_کھڑکی(cur), &جڑ_q, &بچہ,
			  &جڑ_x, &جڑ_y, &win_x, &win_y, &نقاب);

	    اگر (جڑ == جڑ_q)
	    {
		اگر ((win_x < 0) || (win_y < 0))
		    واپس False;

		اگر ((win_x > (صحیح)r_چوڑائی) || (win_y > (صحیح)r_اونچائی))
		    واپس False;
		draw_highlight(eb);
		draw_shadows(eb);
	    }
	}

	واپس True;
    }

    /*
     * Check for changed ExtLabelString.
     */
    اگر (NOT_EQUAL(primitive.shadow_thickness))
    {
	redraw = True;
	// Don't change the pixmaps
	change = False;
    }

    اگر (NOT_EQUAL(primitive.foreground))
	redraw = True;
    اگر (NOT_EQUAL(core.background_pixel))
	redraw = True;
    اگر (NOT_EQUAL(pushbutton.fill_on_arm))
	redraw = True;
    اگر (NOT_EQUAL(enhancedbutton.spacing))
	redraw = True;
    اگر (NOT_EQUAL(enhancedbutton.label_location))
    {
	redraw = True;
	change = False;
    }
    اگر (NOT_EQUAL(label._label))
    {
	redraw = True;
	سیٹ_سائز_2(eb);
    }

    اگر (redraw == True)
    {
	اگر (change)
	    سیٹ_pixmap(eb);
	اگر (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	ورنہ
	    eb->label.pixmap = eb->enhancedbutton.معمول_pixmap;
	اگر (change)
	    سیٹ_سائز_2(eb);
	redraw = False;
    }

    واپس redraw;
}

    ساکن خالی
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#اگر_ختم
    XRectangle		    box;
    صحیح			    dx;
    صحیح			    adjust;
    مختصر		    fill = 0;

    اگر (!XtIsRealized((Widget)eb))
	واپس;

#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#اگر_ختم

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    اگر ((eb->pushbutton.arm_color == eb->primitive.چوٹی_shadow_color)
	    || (eb->pushbutton.arm_color == eb->primitive.bottom_shadow_color))
	fill = 1;

    اگر (eb->pushbutton.compatible)
	adjust = eb->pushbutton.show_as_default;
    ورنہ
	adjust = eb->pushbutton.default_button_shadow_thickness;

    اگر (adjust > 0)
    {
	adjust = adjust + eb->primitive.shadow_thickness;
	adjust = (adjust << 1);
	dx = eb->primitive.highlight_thickness + adjust + fill;
    }
    ورنہ
	dx = (eb->primitive.highlight_thickness
		+ eb->primitive.shadow_thickness + fill);

    box.x = dx;
    box.y = dx;
    adjust = (dx << 1);
    box.چوڑائی  = eb->core.چوڑائی - adjust;
    box.اونچائی = eb->core.اونچائی - adjust;

    /*
     * Redraw the background.
     */
    اگر (!Lab_IsMenupane(eb))
    {
	GC  gc;

	// Don't shade if the button contains a label with a pixmap, since
	// there is no variant of the label available with the needed
	// background.
	اگر (eb->pushbutton.armed && eb->pushbutton.fill_on_arm)
	{
		اگر (eb->label.label_type == (صحیح)XmPIXMAP)
		{
		    اگر (eb->pushbutton.arm_pixmap != XmUNSPECIFIED_PIXMAP)
			gc = eb->pushbutton.fill_gc;
		    ورنہ
			gc = eb->pushbutton.background_gc;
		}
		ورنہ
		    gc = eb->pushbutton.fill_gc;
	}
	ورنہ
	    gc = eb->pushbutton.background_gc;
	// really need to fill with background if not armed ?
	اگر (gc)
	    XFillRectangle(XtDisplay(eb), Xt_کھڑکی(eb), gc,
		    box.x, box.y, box.چوڑائی, box.اونچائی);
    }

    draw_label(eb, event, region);

    اگر (Lab_IsMenupane(eb))
    {
	اگر (eb->pushbutton.armed)
	    (*(((Xm_ڈالو_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    ورنہ
    {
	adjust = 0;

#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	/*
	 *  NOTE: PushButton has two types of shadows: primitive-shadow and
	 *  default-button-shadow.  If pushbutton is in a menu only primitive
	 *  shadows are drawn.
	 */
	انتخاب (default_button_emphasis)
	{
	    حالت XmEXTERNAL_HIGHLIGHT:
		adjust = (eb->primitive.highlight_thickness -
			 (eb->pushbutton.default_button_shadow_thickness
			  ?  Xm3D_ENHANCE_PIXEL : 0));
		توڑو;

	    حالت Xm_اندرونی_HIGHLIGHT:
		توڑو;

	    طےشدہ:
		أكد(FALSE);
		واپس;
	}
#اگر_ختم

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	اگر (box.x > adjust)
	{
	    صحیح borderwidth =box.x - adjust;
	    صحیح rectwidth = eb->core.چوڑائی - 2 * adjust;
	    صحیح rectheight = eb->core.اونچائی - 2 * adjust;

	    اگر (XmIsManager(Xt_والد(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_کھڑکی(eb),
						     Xm_والد_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    ورنہ
	    {
		XmeClearBorder(XtDisplay(eb), Xt_کھڑکی(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
	    انتخاب (default_button_emphasis)
	    {
		حالت Xm_اندرونی_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    اگر (eb->primitive.highlight_drawn)
			(*(((Xm_ڈالو_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    توڑو;

		طےشدہ:
		    توڑو;
	    }
#اگر_ختم
	}

	اگر (eb->pushbutton.default_button_shadow_thickness)
	{
	    اگر (eb->pushbutton.show_as_default)
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
		صحیح	    default_button_shadow_thickness;
		صحیح	    x, y, چوڑائی, اونچائی, delta;
		Widget	    والد;

		اگر (eb->pushbutton.compatible
				     && (eb->pushbutton.show_as_default == 0))
		    واپس;

		اگر (!eb->pushbutton.compatible
			&& (eb->pushbutton.default_button_shadow_thickness
									== 0))
		    واپس;

		delta = eb->primitive.highlight_thickness;

		/*
		 * May need more complex computation for getting the GCs.
		 */
		والد = Xt_والد(eb);
		اگر (XmIsManager(والد))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_والد_چوٹی_Shadow_GC(eb);
		    چوٹی_gc = Xm_والد_Bottom_Shadow_GC(eb);
		}
		ورنہ
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.چوٹی_shadow_GC;
		    چوٹی_gc = eb->primitive.bottom_shadow_GC;
		}

		اگر ((bottom_gc == None) || (چوٹی_gc == None))
		    واپس;


		اگر (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		ورنہ
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#اگر !defined(LESSTIF_نسخہ) && (Xm_نسخہ > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		انتخاب (default_button_emphasis)
		{
		    حالت XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			توڑو;

		    حالت Xm_اندرونی_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			توڑو;

		    طےشدہ:
			أكد(FALSE);
			واپس;
		}
#اگر_ختم

		x = y = delta;
		چوڑائی = eb->core.چوڑائی - 2 * delta;
		اونچائی = eb->core.اونچائی - 2 * delta;

		اگر ((چوڑائی > 0) && (اونچائی > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_کھڑکی(eb),
			    چوٹی_gc, bottom_gc, x, y, چوڑائی, اونچائی,
			    default_button_shadow_thickness,
			    (غیرعلامتی)Xm_SHADOW_برآمد);
	    }
	}

	اگر (eb->primitive.highlight_drawn)
	    draw_shadows(eb);
	draw_pixmap(eb, event, region);
    }
}

    ساکن خالی
BorderHighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ڈالو_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, تہی, تہی);
}

    ساکن خالی
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ڈالو_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, تہی, تہی);
}

#اگر_ختم // FEAT_TOOLBAR
