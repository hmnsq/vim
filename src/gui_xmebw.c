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

#که_تعریف_شوی FEAT_TOOLBAR

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
#د_که_پای
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
#إذا defined(LESSTIF_نسخه) || (Xm_نسخه <= 1002)
# ifndef Lab_IsMenupane
#  define Lab_IsMenupane(w) (Lab_MenuType(w) == (صحيح)XmMENU_POPUP || \
		    Lab_MenuType(w) == (صحيح)XmMENU_PULLDOWN)
# endif
# define XmeClearBorder	    _XmClearBorder
# define XmeDrawShadows	    _XmDrawShadows
# define XmeDrawHighlight(a, b, c, d, e, f, g, h) \
    _XmDrawHighlight(a, b, c, d, e, f, g, h, کرښه_Solid)
#د_که_پای

// Older VMS systems do not have xos_r.h and cannot haldle XtProcessLocking
#إذا defined(VMS)
# إذا defined(HAVE_XOS_R_H)
#  define XTPROCESS_قلف Xt_Process_قلف()
#  define XTPROCESS_UNLOCK XtProcessUnlock()
# وإلا
#  define XTPROCESS_قلف
#  define XTPROCESS_UNLOCK
# endif
#وإلا
# define XTPROCESS_قلف Xt_Process_قلف()
# define XTPROCESS_UNLOCK XtProcessUnlock()
#د_که_پای

/*
 * Motif internals we have to cheat around with.
 */

// Hopefully this will never change...
#که_نه_تعریف_شوی XmFOCUS_IGNORE
# define XmFOCUS_IGNORE       1<<1
#د_که_پای

خارجي Boolean Xm_Get_دننه_Drag_ډېرپېښ(Widget widget);
خارجي فراغ _XmPrimitiveEnter(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجي فراغ _XmPrimitiveLeave(Widget wid,
			      XEvent * event,
			      String * params, Cardinal * عدد_params);
خارجي فراغ Xm_سېټ_Focus_Flag(Widget w, بینښه صحيح ماسک, Boolean ارزښت);
خارجي فراغ _XmCalcLabelDimensions(Widget wid);

/*
 * Declaration of class methods.
 */
ساكن فراغ Destroy(Widget w);
ساكن فراغ Initialize(Widget rq, Widget eb, ArgList دلیلونه, Cardinal *n);
ساكن Boolean سېټ_ارزښتونه(Widget current, Widget request, Widget new, ArgList دلیلونه, Cardinal *n);
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
#تعریف_کړه stipple_پلنوالی  4
#تعریف_کړه stipple_لوړوالی 4
ساكن حرف stipple_بټونه[] = { 0x0a, 0x05, 0x0a, 0x05 };
#تعریف_کړه STIPPLE_BITMAP	xmEnhancedButtonClassRec.enhancedbutton_class.stipple_bitmap

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
	Xt_دننه_واټن_د(XmEnhancedButtonRec, enhancedbutton.pixmap_معلومات),
	XmRImmediate, (Xt_اشاره_ګر) عدم
    }, {
	XmNpixmapFile, XmCPixmap, XmRString, حجم(String),
	Xt_دننه_واټن_د(XmEnhancedButtonRec, enhancedbutton.pixmap_file),
	XmRImmediate, (Xt_اشاره_ګر) عدم
    }, {
	XmNspacing, XmCSpacing, XmRHorizontalDimension, حجم(Dimension),
	Xt_دننه_واټن_د(XmEnhancedButtonRec, enhancedbutton.spacing),
	XmRImmediate, (Xt_اشاره_ګر) 2
    },
    {
	XmNlabelLocation, XmCLocation, XmRInt, حجم(صحيح),
	Xt_دننه_واټن_د(XmEnhancedButtonRec, enhancedbutton.label_location),
	XtRImmediate, (Xt_اشاره_ګر) Xm_ښی
    }
};

// This is needed to work around a bug in Lesstif 2, leaving the extension
// NULL somehow results in getting it set to an invalid pointer.
XmPrimitiveClassExtRec xmEnhancedButtonPrimClassExtRec =
{
    /* next_extension      */ عدم,
    /* record_type	   */ NULLQUARK,
    /*نسخه*/ Xm_Primitive_Class_Ext_نسخه,
    /* record_size	   */ حجم(XmPrimitiveClassExtRec),
    /* widget_baseline	   */ XmInheritBaselineProc,
    /* widget_display_rect */ XmInheritDisplayRectProc,
    /* widget_margins      */ عدم
};

XmEnhancedButtonClassRec xmEnhancedButtonClassRec =
{
    {
	// core_class fields
	/* superclass		 */ (WidgetClass) & xm_ټېل_Button_Class_Rec,
	/* class_name		 */ "XmEnhancedButton",
	/* widget_size		 */ حجم(XmEnhancedButtonRec),
	/* class_initialize	 */ عدم,
	/* class_part_initialize */ عدم,
	/* class_inited		 */ False,
	/* initialize		 */ Initialize,
	/* initialize_hook	 */ عدم,
	/* realize		 */ XtInheritRealize,
	/* actions		 */ actionsList,
	/* num_actions		 */ Xt_شمېره(actionsList),
	/* resources		 */ resources,
	/* num_resources	 */ Xt_شمېره(resources),
	/* xrm_class		 */ NULLQUARK,
	/* compress_motion	 */ True,
	/* compress_exposure	 */ XtExposeCompressMaximal,
	/* compress_enterleave	 */ True,
	/* visible_interest	 */ False,
	/* destroy		 */ Destroy,
	/* resize		 */ XtInheritResize,
	/* expose		 */ Redisplay,
	/* set_values		 */ سېټ_ارزښتونه,
	/* set_values_hook	 */ عدم,
	/* set_values_almost	 */ Xt_Inherit_سېټ_ارزښتونه_Almost,
	/* get_values_hook	 */ عدم,
	/* accept_focus		 */ XtInheritAcceptFocus,
	/* version		 */ Xt_نسخه,
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
	/* extension		 */ (Xt_اشاره_ګر)&xmEnhancedButtonPrimClassExtRec,
    },

    // label_class fields
    {
	/* setOverrideCallback	 */ Xm_Inherit_سېټ_Override_Callback,
	/* menuProcs		 */ XmInheritMenuProc,
	/* translations		 */ XtInheritTranslations,
	/* extension		 */ عدم,
    },

    // pushbutton_class record
    {
	/* extension		 */ (Xt_اشاره_ګر) عدم,
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
    ساكن بینښه لنډ
bump_color(بینښه لنډ ارزښت)
{
    صحيح tmp = 2 * (((صحيح) ارزښت - 65535) / 3) + 65535;

    أرجع tmp;
}

    ساكن صحيح
alloc_color(Display	*display,
	Colormap	colormap,
	حرف		*colorname,
	XColor		*xcolor,
	فراغ		*تړون ناکارول_شوی)
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
سېټ_pixmap(XmEnhancedButtonWidget eb)
{
    // Configure defines XPMATTRIBUTES_TYPE as XpmAttributes or as
    // XpmAttributes_21, depending on what is in Xm/XpmP.h.
    XPMATTRIBUTES_TYPE   attr;
    Pixmap	    sen_pix;
    کړکۍ	    ریښه;
    ساكن Xpm_Color_نښه color[8] = {
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
    بینښه صحيح    لوړوالی, پلنوالی, border, ژوروالی;
    صحيح		    حالت = 0;
    Pixmap	    ماسک;
    Pixmap	    pix = None;
    Pixmap	    arm_pix = None;
    Pixmap	    ins_pix = None;
    Pixmap	    لوړ_pix = None;
    حرف	    **معلومات = (حرف **) eb->enhancedbutton.pixmap_معلومات;
    حرف	    *fname = (حرف *) eb->enhancedbutton.pixmap_file;
    صحيح		    ښويدنه;
    GC		    gc;

    // Make sure there is a default value for the pixmap.
    إذا (!معلومات)
	أرجع;

    gc = XtGetGC((Widget)eb, (XtGCMask)0, عدم);

    scr = DefaultScreen(dpy);
    ریښه = ریښه_کړکۍ_2(dpy, scr);

    eb->label.pixmap = None;

    eb->enhancedbutton.pixmap_ژوروالی = 0;
    eb->enhancedbutton.pixmap_پلنوالی = 0;
    eb->enhancedbutton.pixmap_لوړوالی = 0;
    eb->enhancedbutton.نورمال_pixmap = None;
    eb->enhancedbutton.armed_pixmap = None;
    eb->enhancedbutton.highlight_pixmap = None;
    eb->enhancedbutton.insensitive_pixmap = None;

    // We use dynamic colors, get them now.
    motif_get_toolbar_colors(
	    &eb->core.background_pixel,
	    &eb->primitive.foreground,
	    &eb->primitive.bottom_shadow_color,
	    &eb->primitive.سر_shadow_color,
	    &eb->primitive.highlight_color);

    // Setup color substitution table.
    color[0].pixel = eb->core.background_pixel;
    color[1].pixel = eb->core.background_pixel;
    color[2].pixel = eb->core.background_pixel;
    color[3].pixel = eb->primitive.foreground;
    color[4].pixel = eb->core.background_pixel;
    color[5].pixel = eb->primitive.سر_shadow_color;
    color[6].pixel = eb->primitive.highlight_color;
    color[7].pixel = eb->pushbutton.arm_color;

    // Create the "sensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_شمېره(color);

    إذا (fname)
	حالت = Xpm_Read_File_منزل_Pixmap(dpy, ریښه, fname, &pix, &ماسک, &attr);
    إذا (!fname || حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_معلومات(dpy, ریښه, معلومات, &pix, &ماسک, &attr);

    // If something failed, we will fill in the default pixmap.
    إذا (حالت != XpmSuccess)
	حالت = Xpm_Create_Pixmap_From_معلومات(dpy, ریښه, blank_xpm, &pix,
								&ماسک, &attr);

    Xpm_Free_ځانګړتیاوې(&attr);

    XGetGeometry(dpy, pix, &ریښه, &x, &y, &پلنوالی, &لوړوالی, &border, &ژوروالی);

    // TODO: does the shift depend on label_location somehow?
    ښويدنه = eb->primitive.shadow_thickness / 2;

    إذا (ښويدنه < 1)
	ښويدنه = 1;

    sen_pix = XCreatePixmap(dpy, ریښه, پلنوالی + ښويدنه, لوړوالی + ښويدنه, ژوروالی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XFillRectangle(dpy, sen_pix, gc, 0, 0, پلنوالی + ښويدنه, لوړوالی + ښويدنه);
    XSet_Clip_ماسک(dpy, gc, ماسک);
    XSetClipOrigin(dpy, gc, ښويدنه, ښويدنه);
    XCopyArea(dpy, pix, sen_pix, gc, 0, 0, پلنوالی, لوړوالی, ښويدنه, ښويدنه);

    // Create the "highlight" pixmap.
    color[4].pixel = eb->primitive.bottom_shadow_color;
#که_تعریف_شوی XpmAllocColor // SGI doesn't have it
    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmAllocColor;
    attr.alloc_color = alloc_color;
#وإلا
    attr.valuemask = XpmColorSymbols | XpmCloseness;
#د_که_پای
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = Xt_شمېره(color);

    حالت = Xpm_Create_Pixmap_From_معلومات(dpy, ریښه, معلومات, &pix, عدم, &attr);
    Xpm_Free_ځانګړتیاوې(&attr);

    لوړ_pix = XCreatePixmap(dpy, ریښه, پلنوالی + ښويدنه, لوړوالی + ښويدنه, ژوروالی);

#إذا 1
    XSetForeground(dpy, gc, eb->core.background_pixel);
#وإلا
    XSetForeground(dpy, gc, eb->primitive.سر_shadow_color);
#د_که_پای
    XSet_Clip_ماسک(dpy, gc, None);
    XFillRectangle(dpy, لوړ_pix, gc, 0, 0, پلنوالی + ښويدنه, لوړوالی + ښويدنه);
    XSet_Clip_ماسک(dpy, gc, ماسک);
    XSetClipOrigin(dpy, gc, 0, 0);
    XCopyArea(dpy, pix, لوړ_pix, gc, 0, 0, پلنوالی, لوړوالی, 0, 0);

    arm_pix = XCreatePixmap(dpy, pix, پلنوالی + ښويدنه, لوړوالی + ښويدنه, ژوروالی);

    إذا (eb->pushbutton.fill_on_arm)
	XSetForeground(dpy, gc, eb->pushbutton.arm_color);
    وإلا
	XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, ښويدنه, ښويدنه);
    XSet_Clip_ماسک(dpy, gc, None);
    XFillRectangle(dpy, arm_pix, gc, 0, 0, پلنوالی + ښويدنه, لوړوالی + ښويدنه);
    XSet_Clip_ماسک(dpy, gc, ماسک);
    XSetClipOrigin(dpy, gc, 2 * ښويدنه, 2 * ښويدنه);
    XCopyArea(dpy, pix, arm_pix, gc, 0, 0, پلنوالی, لوړوالی, 2 * ښويدنه, 2 * ښويدنه);

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ماسک);

    // Create the "insensitive" pixmap.
    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_Color_کلی;
    attr.closeness = 65535;	// accuracy isn't crucial
    attr.colorsymbols = color;
    attr.numsymbols = ARRAY_اوږدتیا(color);
    attr.color_کلی = XPM_MONO;
    حالت = Xpm_Create_Pixmap_From_معلومات(dpy, ریښه, معلومات, &pix, &ماسک, &attr);

    // Need to create new Pixmaps with the mask applied.

    ins_pix = XCreatePixmap(dpy, ریښه, پلنوالی + ښويدنه, لوړوالی + ښويدنه, ژوروالی);

    XSetForeground(dpy, gc, eb->core.background_pixel);
    XSetClipOrigin(dpy, gc, 0, 0);
    XSet_Clip_ماسک(dpy, gc, None);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, پلنوالی + ښويدنه, لوړوالی + ښويدنه);
    XSet_Clip_ماسک(dpy, gc, ماسک);
    XSetForeground(dpy, gc, eb->primitive.سر_shadow_color);
    XSetClipOrigin(dpy, gc, 2 * ښويدنه, 2 * ښويدنه);
    XFillRectangle(dpy, ins_pix, gc, 2 * ښويدنه, 2 * ښويدنه, پلنوالی, لوړوالی);
    XSetForeground(dpy, gc, eb->primitive.bottom_shadow_color);
    XSetClipOrigin(dpy, gc, ښويدنه, ښويدنه);
    XFillRectangle(dpy, ins_pix, gc, 0, 0, پلنوالی + ښويدنه, لوړوالی + ښويدنه);
    XtReleaseGC((Widget) eb, gc);

    Xpm_Free_ځانګړتیاوې(&attr);

    eb->enhancedbutton.pixmap_ژوروالی = ژوروالی;
    eb->enhancedbutton.pixmap_پلنوالی = پلنوالی;
    eb->enhancedbutton.pixmap_لوړوالی = لوړوالی;
    eb->enhancedbutton.نورمال_pixmap = sen_pix;
    eb->enhancedbutton.highlight_pixmap = لوړ_pix;
    eb->enhancedbutton.insensitive_pixmap = ins_pix;
    eb->enhancedbutton.armed_pixmap = arm_pix;

    eb->enhancedbutton.doing_setvalues = True;
    eb->enhancedbutton.doing_setvalues = False;

    XFreePixmap(dpy, pix);
    XFreePixmap(dpy, ماسک);
}

#تعریف_کړه	BUTTON_ماسک ( \
	Button1_ماسک | Button2_ماسک | Button3_ماسک | Button4_ماسک | Button5_ماسک \
)

    ساكن فراغ
draw_shadows(XmEnhancedButtonWidget eb)
{
    GC		سر_gc;
    GC		bottom_gc;
    Boolean	etched_دننه;

    إذا (!eb->primitive.shadow_thickness)
	أرجع;

    إذا ((eb->core.پلنوالی <= 2 * eb->primitive.highlight_thickness)
	    || (eb->core.لوړوالی <= 2 * eb->primitive.highlight_thickness))
	أرجع;

#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
    {
	XmDisplay	dpy;

	dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(eb));
	etched_دننه = dpy->display.enable_etched_دننه_menu;
    }
#وإلا
    etched_دننه = False;
#د_که_پای
    إذا (!etched_دننه ^ eb->pushbutton.armed)
    {
	سر_gc = eb->primitive.سر_shadow_GC;
	bottom_gc = eb->primitive.bottom_shadow_GC;
    }
    وإلا
    {
	سر_gc = eb->primitive.bottom_shadow_GC;
	bottom_gc = eb->primitive.سر_shadow_GC;
    }

    XmeDrawShadows(XtDisplay(eb), Xt_کړکۍ(eb),
	    سر_gc,
	    bottom_gc,
	    eb->primitive.highlight_thickness,
	    eb->primitive.highlight_thickness,
	    eb->core.پلنوالی - 2 * eb->primitive.highlight_thickness,
	    eb->core.لوړوالی - 2 * eb->primitive.highlight_thickness,
	    eb->primitive.shadow_thickness,
	    (بینښه)(etched_دننه ? Xm_SHADOW_دننه : Xm_SHADOW_بهر));
}

    ساكن فراغ
draw_highlight(XmEnhancedButtonWidget eb)
{
    eb->primitive.highlighted = True;
    eb->primitive.highlight_drawn = True;

    إذا (!Xt_پلنوالی(eb) || !Xt_لوړوالی(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    XmeDrawHighlight(XtDisplay(eb), Xt_کړکۍ(eb),
	    eb->primitive.highlight_GC, 0, 0,
	    Xt_پلنوالی(eb), Xt_لوړوالی(eb),
	    eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_unhighlight(XmEnhancedButtonWidget eb)
{
    GC manager_background_GC;

    eb->primitive.highlighted = False;
    eb->primitive.highlight_drawn = False;

    إذا (!Xt_پلنوالی(eb) || !Xt_لوړوالی(eb) || !eb->primitive.highlight_thickness)
	أرجع;

    إذا (XmIsManager(eb->core.پلار))
    {
#که_تعریف_شوی UNHIGHLIGHTT
	XmSpecifyUnhighlightTrait UnhighlightT;

	إذا (((UnhighlightT = (XmSpecifyUnhighlightTrait) XmeTraitGet((Xt_اشاره_ګر)
			    XtClass(eb->core.پلار), XmQTspecifyUnhighlight))
		    != عدم) && (UnhighlightT->getUnhighlightGC != عدم))
	{
	    // if unhighlight trait in parent use specified GC...
	    manager_background_GC =
		 UnhighlightT->getUnhighlightGC(eb->core.پلار, (Widget) eb);
	}
	وإلا
	{
	    // ...otherwise, use parent's background GC
	    manager_background_GC = ((XmManagerWidget)
				    (eb->core.پلار))->manager.background_GC;
	}
#وإلا
	manager_background_GC = ((XmManagerWidget)
				    (eb->core.پلار))->manager.background_GC;
#د_که_پای
	XmeDrawHighlight(XtDisplay(eb), Xt_کړکۍ(eb),
			 manager_background_GC,
			 0, 0, Xt_پلنوالی(eb), Xt_لوړوالی(eb),
			 eb->primitive.highlight_thickness);
	إذا (!eb->pushbutton.armed && eb->primitive.shadow_thickness)
	    XmeClearBorder(XtDisplay(eb), Xt_کړکۍ(eb),
		    eb->primitive.highlight_thickness,
		    eb->primitive.highlight_thickness,
		    eb->core.پلنوالی - 2 * eb->primitive.highlight_thickness,
		    eb->core.لوړوالی - 2 * eb->primitive.highlight_thickness,
		    eb->primitive.shadow_thickness);
    }
    وإلا
	XmeClearBorder(XtDisplay(eb), Xt_کړکۍ(eb), 0, 0, Xt_پلنوالی(eb),
		       Xt_لوړوالی(eb), eb->primitive.highlight_thickness);
}

    ساكن فراغ
draw_pixmap(XmEnhancedButtonWidget eb,
	    XEvent *event ناکارول_شوی,
	    Region region ناکارول_شوی)
{
    Pixmap	pix;
    GC		gc = eb->label.نورمال_GC;
    صحيح		ژوروالی;
    Cardinal	پلنوالی;
    Cardinal	لوړوالی;
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
	    pix = eb->enhancedbutton.نورمال_pixmap;
    }

    إذا (pix == None || !eb->enhancedbutton.pixmap_معلومات)
	أرجع;

    ژوروالی = eb->enhancedbutton.pixmap_ژوروالی;
    w = eb->enhancedbutton.pixmap_پلنوالی;
    h = eb->enhancedbutton.pixmap_لوړوالی;

    gc = eb->label.نورمال_GC;
    x = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_پلنوالی;
    y = eb->primitive.highlight_thickness
	+ eb->primitive.shadow_thickness
	+ eb->label.margin_لوړوالی;
    پلنوالی = eb->core.پلنوالی - 2 * x;
    إذا (w < پلنوالی)
	پلنوالی = w;
    لوړوالی = eb->core.لوړوالی - 2 * y;
    إذا (h < لوړوالی)
	لوړوالی = h;
    إذا (ژوروالی == (صحيح)eb->core.ژوروالی)
	XCopyArea(XtDisplay(eb), pix, Xt_کړکۍ(eb), gc, 0, 0,
		پلنوالی, لوړوالی, x, y);
    وإلا إذا (ژوروالی == 1)
	XCopyPlane(XtDisplay(eb), pix, Xt_کړکۍ(eb), gc, 0, 0,
		پلنوالی, لوړوالی, x, y, (بینښه اوږد)1);
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
#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
    XmDisplay	dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    Boolean	etched_دننه = dpy->display.enable_etched_دننه_menu;
#وإلا
    Boolean	etched_دننه = False;
#د_که_پای

    إذا (eb->pushbutton.armed
	    && ((!Lab_IsMenupane(eb) && eb->pushbutton.fill_on_arm)
		|| (Lab_IsMenupane(eb) && etched_دننه)))
    {
	إذا (eb->label.label_type == (صحيح)XmSTRING
		&& eb->pushbutton.arm_color == eb->primitive.foreground)
	{
	    tmp_gc = eb->label.نورمال_GC;
	    eb->label.نورمال_GC = eb->pushbutton.background_gc;
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
	Lab_Margin_کیڼ(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ښی(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_Margin_سر(eb) -= Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) -= Xm3D_ENHANCE_PIXEL;
    }

    {
	XtExposeProc expose;

	XTPROCESS_قلف;
	expose = xmLabelClassRec.core_class.expose;
	XTPROCESS_UNLOCK;
	(*expose)((Widget) eb, event, region);
    }

    إذا (deadjusted)
    {
	Lab_Margin_کیڼ(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_ښی(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_Margin_سر(eb) += Xm3D_ENHANCE_PIXEL;
	Lab_MarginBottom(eb) += Xm3D_ENHANCE_PIXEL;
    }

    إذا (replaceGC)
	eb->label.نورمال_GC = tmp_gc;
}

    ساكن فراغ
Enter(Widget wid,
    XEvent *event,
    String *params ناکارول_شوی,
    Cardinal *عدد_params ناکارول_شوی)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget) wid;
    Xm_ټېل_Button_Callback_Struct call_ارزښت;

    إذا (Lab_IsMenupane(eb))
    {
	إذا ((((ShellWidget) Xt_پلار(Xt_پلار(eb)))->shell.popped_پورته)
		&& Xm_Get_دننه_Drag_ډېرپېښ((Widget) eb))
	{
#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
	    XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	    Boolean etched_دننه = dpy->display.enable_etched_دننه_menu;
#وإلا
	    Boolean etched_دننه = False;
#د_که_پای

	    إذا (eb->pushbutton.armed)
		أرجع;

	    // ...so KHelp event is delivered correctly.
	    Xm_سېټ_Focus_Flag(Xt_پلار(Xt_پلار(eb)), XmFOCUS_IGNORE, TRUE);
	    Xt_سېټ_Keyboard_Focus(Xt_پلار(Xt_پلار(eb)), (Widget) eb);
	    Xm_سېټ_Focus_Flag(Xt_پلار(Xt_پلار(eb)), XmFOCUS_IGNORE, FALSE);

	    eb->pushbutton.armed = TRUE;

	    ((XmManagerWidget) Xt_پلار(wid))->manager.فعال_ماشوم = wid;

	    // etched in menu button
	    إذا (etched_دننه && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کړکۍ(eb),
			       eb->pushbutton.fill_gc,
			       0, 0, eb->core.پلنوالی, eb->core.لوړوالی);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }

	    إذا ((eb->core.پلنوالی > 2 * eb->primitive.highlight_thickness)
		    && (eb->core.لوړوالی >
				       2 * eb->primitive.highlight_thickness))
	    {
		XmeDrawShadows(XtDisplay(eb), Xt_کړکۍ(eb),
			eb->primitive.سر_shadow_GC,
			eb->primitive.bottom_shadow_GC,
			eb->primitive.highlight_thickness,
			eb->primitive.highlight_thickness,
			eb->core.پلنوالی - 2 * eb->primitive.highlight_thickness,
			eb->core.لوړوالی - 2 * eb->primitive.highlight_thickness,
			eb->primitive.shadow_thickness,
			(بینښه)(etched_دننه ? Xm_SHADOW_دننه : Xm_SHADOW_بهر));
	    }

	    إذا (eb->pushbutton.arm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ارزښت.reason = (صحيح)XmCR_ARM;
		call_ارزښت.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.arm_callback,
				   &call_ارزښت);
	    }
	}
    }
    وإلا
    {
	XtExposeProc expose;

	_XmPrimitiveEnter((Widget) eb, event, عدم, عدم);
	إذا (eb->pushbutton.armed == TRUE)
	{
	    XTPROCESS_قلف;
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
    String *params ناکارول_شوی,
    Cardinal *عدد_params ناکارول_شوی)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)wid;
    Xm_ټېل_Button_Callback_Struct call_ارزښت;

    إذا (Lab_IsMenupane(eb))
    {
#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
	XmDisplay dpy = (XmDisplay) XmGetXmDisplay(XtDisplay(wid));
	Boolean etched_دننه = dpy->display.enable_etched_دننه_menu;
#وإلا
	Boolean etched_دننه = False;
#د_که_پای

	إذا (Xm_Get_دننه_Drag_ډېرپېښ((Widget)eb)
		&& eb->pushbutton.armed
		&& ( // !ActiveTearOff ||
				       event->xcrossing.ډېرپېښ == Notify_نورمال))
	{
	    eb->pushbutton.armed = FALSE;

	    ((XmManagerWidget) Xt_پلار(wid))->manager.فعال_ماشوم = عدم;

	    إذا (etched_دننه && !XmIsTearOffButton(eb))
	    {
		XFillRectangle(XtDisplay(eb), Xt_کړکۍ(eb),
			       eb->pushbutton.background_gc,
			       0, 0, eb->core.پلنوالی, eb->core.لوړوالی);
		draw_label(eb, event, عدم);
		draw_pixmap(eb, event, عدم);
	    }
	    وإلا
		XmeClearBorder
		    (XtDisplay(eb), Xt_کړکۍ(eb),
		     eb->primitive.highlight_thickness,
		     eb->primitive.highlight_thickness,
		     eb->core.پلنوالی -
		     2 * eb->primitive.highlight_thickness,
		     eb->core.لوړوالی -
		     2 * eb->primitive.highlight_thickness,
		     eb->primitive.shadow_thickness);

	    إذا (eb->pushbutton.disarm_callback)
	    {
		XFlush(XtDisplay(eb));

		call_ارزښت.reason = (صحيح)XmCR_DISARM;
		call_ارزښت.event = event;
		XtCallCallbackList((Widget) eb,
				   eb->pushbutton.disarm_callback,
				   &call_ارزښت);
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
	    XTPROCESS_قلف;
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

#تعریف_کړه IsNull(p)   ((p) == XmUNSPECIFIED_PIXMAP)

    ساكن فراغ
سېټ_اندازه_2(XmEnhancedButtonWidget newtb)
{
    بینښه صحيح w = 0;
    بینښه صحيح h = 0;

    _XmCalcLabelDimensions((Widget) newtb);

    // Find out how big the pixmap is
    إذا (newtb->enhancedbutton.pixmap_معلومات
	    && !IsNull(newtb->label.pixmap)
	    && !IsNull(newtb->enhancedbutton.نورمال_pixmap))
    {
	w = newtb->enhancedbutton.pixmap_پلنوالی;
	h = newtb->enhancedbutton.pixmap_لوړوالی;
    }

    /*
     * Please note that we manipulate the width only in case of push buttons
     * not used in the context of a menu pane.
     */
    إذا (Lab_IsMenupane(newtb))
    {
	newtb->label.margin_کیڼ = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness)
	    + newtb->label.margin_پلنوالی;
    }
    وإلا
    {
	newtb->label.margin_کیڼ = w;
	newtb->core.پلنوالی = w + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_پلنوالی)
	    + newtb->label.متن_Rect.پلنوالی;

	إذا (newtb->label.متن_Rect.پلنوالی > 0)
	{
	    newtb->label.margin_کیڼ += newtb->label.margin_پلنوالی
					  + newtb->primitive.shadow_thickness;
	    newtb->core.پلنوالی += newtb->label.margin_پلنوالی
					  + newtb->primitive.shadow_thickness;
	}
    }
    إذا (newtb->label.متن_Rect.لوړوالی < h)
    {
	newtb->core.لوړوالی = h  + 2 * (newtb->primitive.shadow_thickness
		+ newtb->primitive.highlight_thickness
		+ newtb->label.margin_لوړوالی);
    }
    وإلا
    {
	// FIXME: We should calculate a drawing offset for the pixmap here to
	// adjust it.
    }

#إذا 0
    چاپ_کړه("%d %d %d %d %d %d - %d %d\n", newtb->enhancedbutton.نورمال_pixmap,
	    h, newtb->core.لوړوالی,
	    newtb->primitive.shadow_thickness,
	    newtb->primitive.highlight_thickness,
	    newtb->label.margin_لوړوالی,
	    newtb->core.پلنوالی,
	    newtb->core.لوړوالی);
#د_که_پای

    // Invoke Label's Resize procedure.
    {
	XtWidgetProc resize;
	XTPROCESS_قلف;
	resize = xmLabelClassRec.core_class.resize;
	XTPROCESS_UNLOCK;

	(* resize) ((Widget) newtb);
    }
}

    ساكن فراغ
Initialize(Widget rq, Widget ebw, ArgList دلیلونه ناکارول_شوی, Cardinal *n ناکارول_شوی)
{
    XmEnhancedButtonWidget  request = (XmEnhancedButtonWidget)rq;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget)ebw;
    XtWidgetProc	    resize;

    XTPROCESS_قلف;
    resize = xmLabelClassRec.core_class.resize;
    XTPROCESS_UNLOCK;

    // Create a bitmap for stippling (Drawable resources are cheap).
    إذا (STIPPLE_BITMAP == None)
    {
	Display *dpy = XtDisplay((Widget) request);
	کړکۍ	ریښه_W = Default_ریښه_کړکۍ(dpy);

	STIPPLE_BITMAP = XCreate_Bitmap_From_معلومات(dpy, ریښه_W, stipple_بټونه,
		stipple_پلنوالی, stipple_لوړوالی);
    }
    eb->enhancedbutton.doing_setvalues = False;

    // First see what type of extended label this is.
    إذا (eb->enhancedbutton.pixmap_معلومات)
    {
	XmString str;
	سېټ_pixmap(eb);

	// FIXME: this is not the perfect way to deal with menus, which do not
	// have any string set right now.
	str = XmStringCreateLocalized("");
	Xt_Va_سېټ_ارزښتونه((Widget) eb, XmNlabelString, str, عدم);
	XmStringFree(str);
    }
    eb->label.pixmap = eb->enhancedbutton.نورمال_pixmap;

    إذا (request->core.پلنوالی == 0)
	eb->core.پلنوالی = 0;
    إذا (request->core.لوړوالی == 0)
	eb->core.لوړوالی = 0;
    سېټ_اندازه_2(eb);

    (* resize)((Widget)eb);
}

    ساكن فراغ
free_pixmaps(XmEnhancedButtonWidget eb)
{
    /*
     * Clear the old pixmaps.
     */
    Pixmap norm_pix = eb->enhancedbutton.نورمال_pixmap;
    Pixmap arm_pix = eb->enhancedbutton.armed_pixmap;
    Pixmap insen_pix = eb->enhancedbutton.insensitive_pixmap;
    Pixmap لوړ_pix = eb->enhancedbutton.highlight_pixmap;

    إذا (norm_pix != None && norm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), norm_pix);

    إذا (arm_pix != None && arm_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), arm_pix);

    إذا (insen_pix != None && insen_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), insen_pix);

    إذا (لوړ_pix != None && لوړ_pix != XmUNSPECIFIED_PIXMAP)
	XFreePixmap(XtDisplay(eb), لوړ_pix);
}

    ساكن فراغ
Destroy(Widget w)
{
    إذا (!XmIsEnhancedButton(w))
	أرجع;

    free_pixmaps((XmEnhancedButtonWidget)w);
}

    ساكن Boolean
سېټ_ارزښتونه(Widget current,
	  Widget request ناکارول_شوی,
	  Widget new,
	  ArgList دلیلونه ناکارول_شوی,
	  Cardinal *n ناکارول_شوی)
{
    XmEnhancedButtonWidget  cur = (XmEnhancedButtonWidget) current;
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) new;
    Boolean		    redraw = False;
    Boolean		    change = True;
    Display		    *dpy = XtDisplay(current);

#تعریف_کړه NOT_EQUAL(ډګر)       (cur->ډګر != eb->ډګر)

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
	    بینښه صحيح    r_لوړوالی;
	    بینښه صحيح    r_پلنوالی;
	    بینښه صحيح    r_border;
	    بینښه صحيح    r_ژوروالی;
	    صحيح		    ریښه_x;
	    صحيح		    ریښه_y;
	    صحيح		    win_x;
	    صحيح		    win_y;
	    کړکۍ	    ریښه;
	    کړکۍ	    ریښه_q;
	    کړکۍ	    ماشوم;
	    بینښه صحيح    ماسک;

	    /*
	     * Artificially let the highlight appear if the mouse is over us.
	     */
	    // Best way to get the root window of object:
	    XGetGeometry(dpy, Xt_کړکۍ(cur), &ریښه, &r_x, &r_y, &r_پلنوالی,
			 &r_لوړوالی, &r_border, &r_ژوروالی);
	    XQuery_اشاره_ګر(XtDisplay(cur), Xt_کړکۍ(cur), &ریښه_q, &ماشوم,
			  &ریښه_x, &ریښه_y, &win_x, &win_y, &ماسک);

	    إذا (ریښه == ریښه_q)
	    {
		إذا ((win_x < 0) || (win_y < 0))
		    أرجع False;

		إذا ((win_x > (صحيح)r_پلنوالی) || (win_y > (صحيح)r_لوړوالی))
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
	سېټ_اندازه_2(eb);
    }

    إذا (redraw == True)
    {
	إذا (change)
	    سېټ_pixmap(eb);
	إذا (eb->primitive.highlighted)
	    eb->label.pixmap = eb->enhancedbutton.highlight_pixmap;
	وإلا
	    eb->label.pixmap = eb->enhancedbutton.نورمال_pixmap;
	إذا (change)
	    سېټ_اندازه_2(eb);
	redraw = False;
    }

    أرجع redraw;
}

    ساكن فراغ
Redisplay(Widget w, XEvent *event, Region region)
{
    XmEnhancedButtonWidget  eb = (XmEnhancedButtonWidget) w;
#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
    XmDisplay		    dpy;
    XtEnum		    default_button_emphasis;
#د_که_پای
    XRectangle		    box;
    صحيح			    dx;
    صحيح			    adjust;
    لنډ		    fill = 0;

    إذا (!XtIsRealized((Widget)eb))
	أرجع;

#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
    dpy = (XmDisplay)XmGetXmDisplay(XtDisplay(eb));
    default_button_emphasis = dpy->display.default_button_emphasis;
#د_که_پای

    /*
     * Compute the area allocated to the label of the pushbutton; fill in the
     * dimensions in the box.
     */

    إذا ((eb->pushbutton.arm_color == eb->primitive.سر_shadow_color)
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
    box.پلنوالی  = eb->core.پلنوالی - adjust;
    box.لوړوالی = eb->core.لوړوالی - adjust;

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
	    XFillRectangle(XtDisplay(eb), Xt_کړکۍ(eb), gc,
		    box.x, box.y, box.پلنوالی, box.لوړوالی);
    }

    draw_label(eb, event, region);

    إذا (Lab_IsMenupane(eb))
    {
	إذا (eb->pushbutton.armed)
	    (*(((Xm_ټېل_Button_Widget_Class)XtClass(eb))
	       ->primitive_class.border_highlight))(w);
	draw_pixmap(eb, event, region);
    }
    وإلا
    {
	adjust = 0;

#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
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

	    حالة Xm_دننی_HIGHLIGHT:
		اكسر;

	    افتراضي:
		أكد(FALSE);
		أرجع;
	}
#د_که_پای

	/*
	 * Clear the area not occupied by label with parents background color.
	 * Label will invoke BorderUnhighlight() on the highlight_thickness
	 * area, which is redundant when XmEXTERNAL_HIGHLIGHT default button
	 * shadow emphasis is used.
	 */
	إذا (box.x > adjust)
	{
	    صحيح borderwidth =box.x - adjust;
	    صحيح rectwidth = eb->core.پلنوالی - 2 * adjust;
	    صحيح rectheight = eb->core.لوړوالی - 2 * adjust;

	    إذا (XmIsManager(Xt_پلار(eb)))
	    {
		XmeDrawHighlight(XtDisplay(eb), Xt_کړکۍ(eb),
						     Xm_پلار_Background_GC(eb),
			adjust, adjust, rectwidth, rectheight, borderwidth);
	    }
	    وإلا
	    {
		XmeClearBorder(XtDisplay(eb), Xt_کړکۍ(eb),
			  adjust, adjust, rectwidth, rectheight, borderwidth);
	    }

#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
	    بدل (default_button_emphasis)
	    {
		حالة Xm_دننی_HIGHLIGHT:
		    // The call above erases the border highlighting.
		    إذا (eb->primitive.highlight_drawn)
			(*(((Xm_ټېل_Button_Widget_Class) XtClass (eb))
			   ->primitive_class.border_highlight)) ((Widget) eb) ;
		    اكسر;

		افتراضي:
		    اكسر;
	    }
#د_که_پای
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
		GC	    سر_gc;
		GC	    bottom_gc;
		صحيح	    default_button_shadow_thickness;
		صحيح	    x, y, پلنوالی, لوړوالی, delta;
		Widget	    پلار;

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
		پلار = Xt_پلار(eb);
		إذا (XmIsManager(پلار))
		{
		    // Use the parent's GC so monochrome works.
		    bottom_gc = Xm_پلار_سر_Shadow_GC(eb);
		    سر_gc = Xm_پلار_Bottom_Shadow_GC(eb);
		}
		وإلا
		{
		    // Use your own pixel for drawing.
		    bottom_gc = eb->primitive.سر_shadow_GC;
		    سر_gc = eb->primitive.bottom_shadow_GC;
		}

		إذا ((bottom_gc == None) || (سر_gc == None))
		    أرجع;


		إذا (eb->pushbutton.compatible)
		    default_button_shadow_thickness =
					       eb->pushbutton.show_as_default;
		وإلا
		    default_button_shadow_thickness =
			       eb->pushbutton.default_button_shadow_thickness;

#إذا !defined(LESSTIF_نسخه) && (Xm_نسخه > 1002)
		/*
		 * Compute location of bounding box to contain the
		 * defaultButtonShadow.
		 */
		بدل (default_button_emphasis)
		{
		    حالة XmEXTERNAL_HIGHLIGHT:
			delta = eb->primitive.highlight_thickness;
			اكسر;

		    حالة Xm_دننی_HIGHLIGHT:
			delta = Xm3D_ENHANCE_PIXEL;
			اكسر;

		    افتراضي:
			أكد(FALSE);
			أرجع;
		}
#د_که_پای

		x = y = delta;
		پلنوالی = eb->core.پلنوالی - 2 * delta;
		لوړوالی = eb->core.لوړوالی - 2 * delta;

		إذا ((پلنوالی > 0) && (لوړوالی > 0))
		    XmeDrawShadows(XtDisplay(eb), Xt_کړکۍ(eb),
			    سر_gc, bottom_gc, x, y, پلنوالی, لوړوالی,
			    default_button_shadow_thickness,
			    (بینښه)Xm_SHADOW_بهر);
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

    (*(xm_ټېل_Button_Class_Rec.primitive_class.border_highlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

    ساكن فراغ
BorderUnhighlight(Widget w)
{
    XmEnhancedButtonWidget eb = (XmEnhancedButtonWidget)w;

    (*(xm_ټېل_Button_Class_Rec.primitive_class.border_unhighlight))(w);
    draw_pixmap(eb, عدم, عدم);
}

#د_که_پای // FEAT_TOOLBAR
