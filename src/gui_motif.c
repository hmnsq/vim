/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ضمن "vim.h"

#ضمن <Xm/Form.h>
#ضمن <Xm/RowColumn.h>
#ضمن <Xm/PushB.h>
#ضمن <Xm/Text.h>
#ضمن <Xm/TextF.h>
#ضمن <Xm/Separator.h>
#ضمن <Xm/Label.h>
#ضمن <Xm/CascadeB.h>
#ضمن <Xm/ScrollBar.h>
#ضمن <Xm/MenuShell.h>
#ضمن <Xm/DrawingA.h>
#إذا (Xm_نسخه >= 1002)
# include <Xm/RepType.h>
#پایان_اگر
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/XmP.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#اگر_تعریف_شده HAVE_X11_XPM_H
# إذا defined(VMS)
#  include <xpm.h>
# وإلا
#  include <X11/xpm.h>
# endif
#وإلا
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#پایان_اگر
#اگر_تعریف_شده HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#پایان_اگر

#ضمن "gui_xmebw.h"	// for our Enhanced Button Widget

#إذا defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#پایان_اگر

#تعریف_کن MOTIF_POPUP

خارجي Widget vimShell;

ساكن Widget vimForm;
ساكن Widget متن_Area_Form;
Widget متن_Area;
#اگر_تعریف_شده FEAT_TOOLBAR
ساكن Widget tool_Bar_قاب;
ساكن Widget toolBar;
#پایان_اگر
#اگر_تعریف_شده FEAT_GUI_TABLINE
ساكن Widget	tab_سطر;
ساكن Widget	tab_سطر_menu = 0;
ساكن صحيح	showing_tabline = 0;
#پایان_اگر
#اگر_تعریف_شده FEAT_MENU
# إذا (Xm_نسخه >= 1002)
// remember the last set value for the tearoff item
ساكن صحيح tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
# endif
ساكن Widget menuBar;
#پایان_اگر

#اگر_تعریف_شده FEAT_TOOLBAR
ساكن فراغ reset_focus(فراغ);
#پایان_اگر

ساكن فراغ gui_motif_menu_colors(Widget id);
ساكن فراغ gui_motif_scroll_colors(Widget id);

#إذا (Xm_نسخه >= 1002)
# define STRING_برچسب  Xm_FONTLIST_DEFAULT_برچسب
#وإلا
# define STRING_برچسب  XmSTRING_DEFAULT_CHARSET
#پایان_اگر

/*
 * Call-back routines.
 */

    ساكن فراغ
scroll_cb(Widget w استفادهنشده, Xt_اشاره_گر client_داده_2, Xt_اشاره_گر call_داده)
{
    scrollbar_T *sb;
    دراز	مقدار;
    صحيح		dragging;

    sb = gui_پیدا_کن_scrollbar((دراز)client_داده_2);

    مقدار = ((XmScrollBarCallbackStruct *)call_داده)->مقدار;
    dragging = (((XmScrollBarCallbackStruct *)call_داده)->reason ==
							      (صحيح)XmCR_DRAG);
    gui_drag_scrollbar(sb, مقدار, dragging);
}

#اگر_تعریف_شده FEAT_GUI_TABLINE
    ساكن فراغ
tabline_cb(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2 استفادهنشده,
    Xt_اشاره_گر	call_داده)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_داده;
    إذا (nptr->reason != (صحيح)XmCR_NONE)
	send_tabline_event(nptr->صفحه_عدد);
}

    ساكن فراغ
tabline_button_cb(
    Widget	w,
    Xt_اشاره_گر	client_داده_2 استفادهنشده,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    Xt_اشاره_گر	cmd, tab_idx;

    Xt_Va_Get_مقادیر(w, Xm_Nuser_داده, &cmd, عدم);
    Xt_Va_Get_مقادیر(tab_سطر_menu, Xm_Nuser_داده, &tab_idx, عدم);

    send_tabline_menu_event((صحيح)(دراز)tab_idx, (صحيح)(دراز)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساكن فراغ
motif_tabline_timer_cb (
    Xt_اشاره_گر		timed_بیرون,
    XtIntervalId	*interval_id استفادهنشده)
{
    *((صحيح *)timed_بیرون) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ساكن صحيح
tabline_scroller_clicked(
    حرف		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	عرض, ارتفاع;

    tab_scroll_w = Xt_Name_مقصد_Widget(tab_سطر, scroller_name);
    إذا (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_مقادیر(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &عرض, XmNheight, &ارتفاع, عدم);
	إذا (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    إذا (event->x >= pos_x && event->x <= pos_x + عرض
		    && event->y >= pos_y && event->y <= pos_y + ارتفاع)
		// Clicked on the scroller
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
tabline_menu_cb(
    Widget	w,
    Xt_اشاره_گر	بستگی استفادهنشده,
    XEvent	*e,
    Boolean	*continue_dispatch استفادهنشده)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحيح				tab_idx = 0;
    WidgetList			children;
    Cardinal			رقم_Children;
    ساكن XtIntervalId		timer = (XtIntervalId)0;
    ساكن صحيح			timed_بیرون = TRUE;

    event = (XButtonPressedEvent *)e;

    إذا (event->button == Button1)
    {
	إذا (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    أرجع;

	إذا (!timed_بیرون)
	{
	    Xt_Remove_زمان_بیرون(timer);
	    timed_بیرون = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	وإلا
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = Xt_App_جمع_کن_زمان_بیرون(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_بیرون);
	    timed_بیرون = FALSE;
	}
	أرجع;
    }

    إذا (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_مقادیر(tab_سطر_menu, Xm_Nuser_داده, &tab_idx, عدم);
	send_tabline_menu_event(tab_idx, (صحيح)TABLINE_MENU_CLOSE);
	أرجع;
    }

    إذا (event->button != Button3)
	أرجع;

    // When ignoring events don't show the menu.
    إذا (hold_gui_events || cmdwin_type != 0)
	أرجع;

    إذا (event->subwindow != None)
    {
	tab_w = Xt_پنجره_مقصد_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	إذا (tab_w != (Widget)0 && Xm_Is_افزودن_Button(tab_w))
	    Xt_Va_Get_مقادیر(tab_w, Xm_Npage_عدد, &tab_idx, عدم);
    }

    Xt_Va_مجموعه_مقادیر(tab_سطر_menu, Xm_Nuser_داده, (Xt_اشاره_گر)(دراز)tab_idx, عدم);
    Xt_Va_Get_مقادیر(tab_سطر_menu, XmNchildren, &children, XmNnumChildren,
		  &رقم_Children, عدم);
    XtManageChildren(children, رقم_Children);
    XmMenuPosition(tab_سطر_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_فرزند(tab_سطر_menu);
}

    ساكن فراغ
tabline_balloon_cb(BalloonEval *beval, صحيح state استفادهنشده)
{
    صحيح		nr;
    tabpage_T	*tp;

    إذا (beval->هدف == (Widget)0)
	أرجع;

    Xt_Va_Get_مقادیر(beval->هدف, Xm_Npage_عدد, &nr, عدم);
    tp = پیدا_کن_tabpage(nr);
    إذا (tp == عدم)
	أرجع;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#پایان_اگر

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#ضمن <Xm/XmP.h>
#ضمن <Xm/LabelP.h>

ساكن XtExposeProc old_label_expose = عدم;

    ساكن فراغ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    بیعلامت حرف   label_type = (صحيح)XmSTRING;

    Xt_Va_Get_مقادیر(_w, XmNlabelType, &label_type, (Xt_اشاره_گر)0);

    إذا (XtIsSensitive(_w) || label_type != (صحيح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وإلا
    {
	XGCValues   مقادیر;
	XtGCMask    ماسک;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	ماسک = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_ماسک | GCClipXOrigin | GCClipYOrigin;
	مقادیر.graphics_exposures = False;

	إذا (fs != 0)
	{
	    ماسک |= GCFont;
	    مقادیر.font = fs->fid;
	}

	إذا (lw->primitive.بالاترین_shadow_pixmap != None
		&& lw->primitive.بالاترین_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    ماسک |= GCFillStyle | GCTile;
	    مقادیر.fill_style = FillTiled;
	    مقادیر.tile = lw->primitive.بالاترین_shadow_pixmap;
	}

	lw->label.متن_Rect.x += 1;
	lw->label.متن_Rect.y += 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x += 1;
	    lw->label.acc_متن_Rect.y += 1;
	}

	مقادیر.foreground = lw->primitive.بالاترین_shadow_color;
	مقادیر.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, ماسک,
					       &مقادیر, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.متن_Rect.x -= 1;
	lw->label.متن_Rect.y -= 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x -= 1;
	    lw->label.acc_متن_Rect.y -= 1;
	}

	مقادیر.foreground = lw->primitive.bottom_shadow_color;
	مقادیر.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, ماسک,
					       &مقادیر, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    فراغ
gui_x11_create_widgets(فراغ)
{
#اگر_تعریف_شده FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		آرگومانها[10];
    صحيح		n;
    XmString	xms;
#پایان_اگر

    /*
     * Install the 3D shade effect drawing routines.
     */
    إذا (old_label_expose == عدم)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_جابه_جایی = gui.border_عرض;

    /*
     * Install the tearOffModel resource converter.
     */
#إذا (Xm_نسخه >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#پایان_اگر

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_مجموعه_مقادیر(vimShell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_عرض, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_عرض, 0,
	Xm_Nmargin_ارتفاع, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_menu_colors(vimForm);

#اگر_تعریف_شده FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحيح ac = 0;

# إذا (Xm_نسخه >= 1002)
	Xt_مجموعه_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_مجموعه_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_مجموعه_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_مجموعه_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_مجموعه_Arg(al[ac], Xm_Nright_جابه_جایی, 0); ac++;
# endif
	Xt_مجموعه_Arg(al[ac], Xm_Nmargin_ارتفاع, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_فرزند(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#پایان_اگر

#اگر_تعریف_شده FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_قاب = XtVaCreateWidget("toolBarFrame",
	xm_قاب_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_ارتفاع, 0,
	Xm_Nmargin_عرض, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	عدم);
    gui_motif_menu_colors(tool_Bar_قاب);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_ردیف_ستونی_Widget_Class, tool_Bar_قاب,
	XmNchildType, Xm_قاب_WORKAREA_فرزند,
	Xm_Nrow_ستونی_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_ارتفاع, 0,
	Xm_Nmargin_عرض, 0,
	Xm_Nadjust_آخرین, True,
	عدم);
    gui_motif_menu_colors(toolBar);

#پایان_اگر

#اگر_تعریف_شده FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbindingType, XmNONE); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNorientation, XmVERTICAL); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nback_صفحه_اندازه, XmNONE); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nback_صفحه_عدد, 0); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nback_صفحه_Placement, Xm_بالاترین_راست); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNmajorTabSpacing, 0); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNshadowThickness, 0); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سطر = XmCreateNotebook(vimForm, "Vim tabline", آرگومانها, n);

    Xt_جمع_کن_Callback(tab_سطر, Xm_Npage_تغییریافته_Callback, (XtCallbackProc)tabline_cb,
			عدم);
    Xt_جمع_کن_Event_Handler(tab_سطر, Button_Press_ماسک, False,
			(XtEventHandler)tabline_menu_cb, عدم);

    gui.tabline_ارتفاع = TABLINE_ارتفاع;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerNext");
    Xt_Va_مجموعه_مقادیر(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);
    scroller = Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerPrevious");
    Xt_Va_مجموعه_مقادیر(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);

    // Create the tabline popup menu
    tab_سطر_menu = XmCreatePopupMenu(tab_سطر, "tabline popup", عدم, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nuser_داده, (Xt_اشاره_گر)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_برچسب);
    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "Close", آرگومانها, n);
    Xt_جمع_کن_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nuser_داده, (Xt_اشاره_گر)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_برچسب);
    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "New Tab", آرگومانها, n);
    Xt_جمع_کن_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nuser_داده, (Xt_اشاره_گر)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_برچسب);
    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "Open tab...", آرگومانها, n);
    Xt_جمع_کن_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);
#پایان_اگر

    متن_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_عرض, 0,
	Xm_Nmargin_ارتفاع, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_scroll_colors(متن_Area_Form);

    متن_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, متن_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.پشت_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_عرض, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	عدم);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(متن_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.درون_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    فراغ
gui_x11_destroy_widgets(فراغ)
{
    متن_Area = عدم;
#اگر_تعریف_شده FEAT_MENU
    menuBar = عدم;
#پایان_اگر
}

    فراغ
gui_mch_مجموعه_متن_area_pos(
    صحيح	    x استفادهنشده,
    صحيح	    y استفادهنشده,
    صحيح	    w استفادهنشده,
    صحيح	    h استفادهنشده)
{
#اگر_تعریف_شده FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#پایان_اگر
}

    فراغ
gui_x11_مجموعه_پشت_color(فراغ)
{
    إذا (متن_Area != عدم)
#إذا (Xm_نسخه >= 1002)
	XmChangeColor(متن_Area, gui.پشت_pixel);
#وإلا
	Xt_Va_مجموعه_مقادیر(متن_Area,
		  XmNbackground, gui.پشت_pixel,
		  عدم);
#پایان_اگر
}

/*
 * Manage dialog centered on pointer.
 */
    فراغ
manage_centered(Widget dialog_فرزند)
{
    Widget shell = Xt_والد(dialog_فرزند);
    پنجره ریشه, فرزند;
    بیعلامت صحيح ماسک;
    بیعلامت صحيح عرض, ارتفاع, border_عرض, عمق;
    صحيح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_مقادیر(shell, XmNmappedWhenManaged, &mappedWhenManaged, عدم);
    Xt_Va_مجموعه_مقادیر(shell, XmNmappedWhenManaged, False, عدم);

    Xt_Manage_فرزند(dialog_فرزند);

    // Get the pointer position (x, y)
    XQuery_اشاره_گر(XtDisplay(shell), Xt_پنجره(shell), &ریشه, &فرزند,
		  &x, &y, &win_x, &win_y, &ماسک);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_پنجره(shell), &ریشه, &win_x, &win_y,
		 &عرض, &ارتفاع, &border_عرض, &عمق);
    عرض += 2 * border_عرض;
    ارتفاع += 2 * border_عرض;
    x -= عرض / 2;
    y -= ارتفاع / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->عرض - عرض;
    maxY = XtScreen(shell)->ارتفاع - ارتفاع;
    إذا (x < 0)
	x = 0;
    إذا (x > maxX)
	x = maxX;
    إذا (y < 0)
	y = 0;
    إذا (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_مجموعه_مقادیر(shell, XmNx, x, XmNy, y, عدم);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_مجموعه_مقادیر(shell, XmNmappedWhenManaged, mappedWhenManaged, عدم);
}

#إذا defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# إذا (Xm_نسخه <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_برچسب);
# وإلا
    // Motif 1.2 method
    Xm_Font_List_مدخل font_list_مدخل;

    font_list_مدخل = Xm_Font_List_مدخل_Create(STRING_برچسب, XmFONT_IS_FONT,
					    (Xt_اشاره_گر)font);
    font_list = Xm_Font_List_Append_مدخل(عدم, font_list_مدخل);
    Xm_Font_List_مدخل_Free(&font_list_مدخل);
# endif
    أرجع font_list;
}

# إذا ((Xm_نسخه > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_مجموعه *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_مدخل font_list_مدخل;

    font_list_مدخل = Xm_Font_List_مدخل_Create(STRING_برچسب,
					    XmFONT_IS_FONTSET,
					    (Xt_اشاره_گر)*fontset);
    font_list = Xm_Font_List_Append_مدخل(عدم, font_list_مدخل);
    Xm_Font_List_مدخل_Free(&font_list_مدخل);
    أرجع font_list;
}
# endif

#پایان_اگر

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساكن فراغ gui_motif_جمع_کن_actext(vimmenu_T *menu);
# إذا (Xm_نسخه >= 1002)
ساكن فراغ toggle_tearoff(Widget wid);
ساكن فراغ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساكن فراغ submenu_change(vimmenu_T *mp, صحيح colors);

ساكن فراغ do_مجموعه_mnemonics(صحيح enable);
ساكن صحيح menu_enabled = TRUE;

    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag)
    {
	Xt_Manage_فرزند(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_والد(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_مجموعه_مقادیر(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
    }
    وإلا
    {
	Xt_Unmanage_فرزند(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_والد(toolBar)))
	{
	    Xt_Va_مجموعه_مقادیر(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    فراغ
gui_motif_مجموعه_mnemonics(صحيح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    إذا (!menu_enabled)
	enable = FALSE;
    do_مجموعه_mnemonics(enable);
}

    ساكن فراغ
do_مجموعه_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	إذا (menu->id != (Widget)0)
	    Xt_Va_مجموعه_مقادیر(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    عدم);
}

    فراغ
gui_mch_جمع_کن_menu(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*والد = menu->والد;

# ifdef MOTIF_POPUP
    إذا (menu_is_popup(menu->name))
    {
	Arg arg[2];
	صحيح n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  إذا (Xm_نسخه <= 1002)
	إذا (mouse_model_popup())
#  endif
	{
	    إذا (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_مجموعه_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    إذا (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_مجموعه_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(متن_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	أرجع;
    }
# endif

    إذا (!menu_is_menubar(menu->name)
	    || (والد != عدم && والد->submenu_id == (Widget)0))
	أرجع;

    label = XmStringCreate((حرف *)menu->dname, STRING_برچسب);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (والد == عدم) ? menuBar : والد->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# إذا (Xm_نسخه >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_شاخص, idx + (والد != عدم
			   && tearoff_val == (صحيح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    عدم);
    XmStringFree(label);

    إذا (menu->id == (Widget)0)		// failed
	أرجع;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    إذا (والد == عدم && menu->اولویت >= 9999)
	Xt_Va_مجموعه_مقادیر(menuBar,
		XmNmenuHelpWidget, menu->id,
		عدم);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_جمع_کن_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	عدم);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_ردیف_ستونی_Widget_Class, shell,
	Xm_Nrow_ستونی_Type, XmMENU_PULLDOWN,
	عدم);
    gui_motif_menu_colors(menu->submenu_id);

    إذا (menu->submenu_id == (Widget)0)		// failed
	أرجع;

# إذا (Xm_نسخه >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_مجموعه_مقادیر(menu->id,
	XmNsubMenuId, menu->submenu_id,
	عدم);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    إذا (والد == عدم)
	gui_mch_compute_menu_ارتفاع(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساكن فراغ
gui_motif_جمع_کن_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    إذا (menu->actext == عدم || menu->id == (Widget)0)
	أرجع;

    label = XmStringCreate((حرف *)menu->actext, STRING_برچسب);
    إذا (label == عدم)
	أرجع;
    Xt_Va_مجموعه_مقادیر(menu->id, Xm_Naccelerator_متن, label, عدم);
    XmStringFree(label);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
# إذا (Xm_نسخه >= 1002)
    إذا (enable)
	tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
    وإلا
	tearoff_val = (صحيح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ریشه_menu);
# endif
}

# إذا (Xm_نسخه >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ساكن فراغ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_مجموعه_مقادیر(wid, XmNtearOffModel, tearoff_val, عدم);
    إذا (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ساكن فراغ
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    طالما (menu != عدم)
    {
	إذا (!menu_is_popup(menu->name))
	{
	    إذا (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->بعدی;
    }
}
# endif

    صحيح
gui_mch_متن_area_extra_ارتفاع(فراغ)
{
    Dimension	shadow_ارتفاع;

    Xt_Va_Get_مقادیر(متن_Area_Form, XmNshadowThickness, &shadow_ارتفاع, عدم);
    أرجع shadow_ارتفاع;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    فراغ
gui_mch_compute_menu_ارتفاع(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساكن Dimension	ارتفاع = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    إذا (id != (Widget)0)
	Xt_Va_Get_مقادیر(id, XmNheight, &ارتفاع, عدم);

    // Find any menu Widget, to be able to call XtManageChild()
    وإلا
	FOR_ALL_MENUS(mp)
	    إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		اكسر;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    إذا (id != (Widget)0)
	Xt_Manage_فرزند(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_مقادیر(mp->id, XmNy, &y, عدم);
	    إذا (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_مقادیر(menuBar,
	Xm_Nmargin_ارتفاع, &margin,
	XmNshadowThickness, &shadow,
	عدم);

    /*
     * This computation is the result of trial-and-error:
     * maxy =	The maximum position of an item; required for when there are
     *		two or more rows
     * height = height of an item, before managing it;	Hopefully this will
     *		change with the font height.  Includes shadow-border.
     * shadow =	shadow-border; must be subtracted from the height.
     * margin = margin around the menu buttons;  Must be added.
     * Add 4 for the underlining of shortcut keys.
     */
    gui.menu_ارتفاع = maxy + ارتفاع - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    إذا (gui.shell_created)
	Xt_Va_مجموعه_مقادیر(menuBar, XmNheight, gui.menu_ارتفاع, عدم);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ساكن حرف **get_toolbar_pixmap(vimmenu_T *menu, حرف **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ساكن صحيح
check_xpm(char_u *مسیر)
{
    Xpm_صفات attrs;
    صحيح		وضعیت;
    Pixmap	ماسک;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    وضعیت = Xpm_Read_File_مقصد_Pixmap(gui.dpy,
	    ریشه_پنجره_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)مسیر, &map, &ماسک, &attrs);
    Xpm_Free_صفات(&attrs);

    إذا (وضعیت == XpmSuccess)
	أرجع OK;
    أرجع FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ساكن حرف **
get_toolbar_pixmap(vimmenu_T *menu, حرف **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    حرف	**xpm = عدم;		// xpm array
    صحيح		res;

    *fname = عدم;
    buf[0] = NUL;			// start with NULL path

    إذا (menu->iconfile != عدم)
    {
	// Use the "icon="  argument.
	gui_پیدا_کن_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	إذا (res == FAIL && gui_پیدا_کن_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	إذا (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    أرجع tb_blank_xpm;
	}
    }

    إذا (menu->icon_builtin || gui_پیدا_کن_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	إذا (menu->iconidx >= 0 && menu->iconidx
	       < (صحيح)(حجم(built_درون_pixmaps) / حجم(built_درون_pixmaps[0])))
	    xpm = built_درون_pixmaps[menu->iconidx];
	وإلا
	    xpm = tb_blank_xpm;
    }

    أرجع xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساكن صحيح
جمع_کن_pixmap_آرگومانها(vimmenu_T *menu, Arg *آرگومانها, صحيح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    إذا (menu->xpm == عدم)
    {
	Xt_مجموعه_Arg(آرگومانها[n], XmNlabelType, XmSTRING); n++;
    }
    وإلا
    {
	إذا (menu->xpm_fname != عدم)
	{
	    Xt_مجموعه_Arg(آرگومانها[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_مجموعه_Arg(آرگومانها[n], Xm_Npixmap_داده, menu->xpm); n++;
	Xt_مجموعه_Arg(آرگومانها[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    أرجع n;
}
# endif // FEAT_TOOLBAR

    فراغ
gui_mch_جمع_کن_menu_قلم(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    vimmenu_T	*والد = menu->والد;

# إذا (Xm_نسخه <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    إذا (menu_is_فرزند_از_popup(menu) && !mouse_model_popup())
	أرجع;
# endif

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(والد->name))
    {
	WidgetClass	type;
	XmString	xms = عدم;    // fallback label if pixmap not found
	صحيح		n;
	Arg		آرگومانها[18];

	n = 0;
	إذا (menu_is_separator(menu->name))
	{
	    حرف	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (حرف *)vim_strchr(menu->name, ':');
	    إذا (cp != عدم)
		wid = (Dimension)atoi(++cp);
	    وإلا
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNwidth, wid); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nmin_عرض, wid); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNorientation, XmVERTICAL); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNseparatorType, Xm_SHADOW_ETCHED_درون); n++;
	}
	وإلا
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_مجموعه_Arg(آرگومانها[n], XmNhighlightThickness, 1); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNhighlightOnEnter, True); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nmargin_عرض, 0); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nmargin_ارتفاع, 0); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_برچسب);
	    Xt_مجموعه_Arg(آرگومانها[n], XmNlabelString, xms); n++;

	    n = جمع_کن_pixmap_آرگومانها(menu, آرگومانها, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_مجموعه_Arg(آرگومانها[n], Xm_Nposition_شاخص, idx); n++;
	إذا (menu->id == عدم)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, آرگومانها, n);
	    إذا (menu->id != عدم && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_جمع_کن_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وإلا
	    Xt_مجموعه_مقادیر(menu->id, آرگومانها, n);
	إذا (xms != عدم)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_مجموعه_tip(menu);
#  endif

	menu->والد = والد;
	menu->submenu_id = عدم;
	// When adding first item to toolbar it might have to be enabled .
	إذا (!XtIsManaged(Xt_والد(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_ارتفاع = gui_mch_compute_toolbar_ارتفاع();
	أرجع;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    إذا (والد->submenu_id == (Widget)0)
	أرجع;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    إذا (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, والد->submenu_id,
# إذا (Xm_نسخه >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_شاخص, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		عدم);
	gui_motif_menu_colors(menu->id);
	أرجع;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_برچسب);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	xm_افزودن_Button_Widget_Class, والد->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# إذا (Xm_نسخه >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_شاخص, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	عدم);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    إذا (menu->id != (Widget)0)
    {
	Xt_جمع_کن_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_اشاره_گر)menu);
	// add accelerator text
	gui_motif_جمع_کن_actext(menu);
    }
}

# إذا (Xm_نسخه <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    فراغ
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    صحيح		idx = 0;

    // When GUI hasn't started the menus have not been created.
    إذا (!gui.درون_use)
	أرجع;

    طالما (menu)
    {
      إذا (menu->children != عدم)
      {
	  إذا (menu_is_popup(menu->name))
	  {
	      إذا (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_جمع_کن_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      وإلا
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      وإلا إذا (menu_is_فرزند_از_popup(menu))
      {
	  إذا (mouse_model_popup())
	      gui_mch_جمع_کن_menu_قلم(menu, idx);
	  وإلا
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->بعدی;
      ++idx;
    }
}
# endif

    فراغ
gui_mch_new_menu_colors(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(tool_Bar_قاب);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ریشه_menu, TRUE);
}

    فراغ
gui_mch_new_menu_font(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    submenu_change(ریشه_menu, FALSE);
    {
	Dimension   ارتفاع;
	Position w, h;

	Xt_Va_Get_مقادیر(menuBar, XmNheight, &ارتفاع, عدم);
	gui.menu_ارتفاع = ارتفاع;

	Xt_Va_Get_مقادیر(vimShell, XtNwidth, &w, XtNheight, &h, عدم);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_وضعیت_area_ارتفاع()
# endif
		     );
    }
    gui_مجموعه_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# إذا defined(FEAT_BEVAL_GUI)
    فراغ
gui_mch_new_tooltip_font(فراغ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    إذا (toolBar == (Widget)0)
	أرجع;

    menu = gui_پیدا_کن_menu((char_u *)"ToolBar");
    إذا (menu != عدم)
	submenu_change(menu, FALSE);
#  endif
}

    فراغ
gui_mch_new_tooltip_colors(فراغ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    إذا (toolBar == (Widget)0)
	أرجع;

    toolbar = gui_پیدا_کن_menu((char_u *)"ToolBar");
    إذا (toolbar != عدم)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ساكن فراغ
submenu_change(
    vimmenu_T	*menu,
    صحيح		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    لكل (mp = menu; mp != عدم; mp = mp->بعدی)
    {
	إذا (mp->id != (Widget)0)
	{
	    إذا (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		إذا (mp->xpm != عدم)
		{
		    صحيح		n = 0;
		    Arg		آرگومانها[18];

		    n = جمع_کن_pixmap_آرگومانها(mp, آرگومانها, n);
		    Xt_مجموعه_مقادیر(mp->id, آرگومانها, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg آرگومانها[2];

		    آرگومانها[0].name = XmNbackground;
		    آرگومانها[0].مقدار = gui.tooltip_bg_pixel;
		    آرگومانها[1].name = XmNforeground;
		    آرگومانها[1].مقدار = gui.tooltip_fg_pixel;
		    Xt_مجموعه_مقادیر(mp->tip->balloonLabel, &آرگومانها[0], Xt_عدد(آرگومانها));
		}
#  endif
# endif
	    }
	    وإلا
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg آرگومانها[1];

		    آرگومانها[0].name = XmNfontList;
		    آرگومانها[0].مقدار = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_مجموعه_مقادیر(mp->tip->balloonLabel, &آرگومانها[0], Xt_عدد(آرگومانها));
		}
# endif
	    }
	}

	إذا (mp->children != عدم)
	{
# إذا (Xm_نسخه >= 1002)
	    // Set the colors/font for the tear off widget
	    إذا (mp->submenu_id != (Widget)0)
	    {
		إذا (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		وإلا
		    gui_motif_menu_fontlist(mp->submenu_id);
		toggle_tearoff(mp->submenu_id);
	    }
# endif
	    // Set the colors for the children
	    submenu_change(mp->children, colors);
	}
    }
}

/*
 * Destroy the machine specific menu widget.
 */
    فراغ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    إذا (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    إذا (menu->id == (Widget)0)
	أرجع;

    Widget	    والد;

    والد = Xt_والد(menu->id);
# إذا defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    إذا (والد == toolBar && menu->tip != عدم)
    {
	// We try to destroy this before the actual menu, because there are
	// callbacks, etc. that will be unregistered during the tooltip
	// destruction.
	//
	// If you call "gui_mch_destroy_beval_area()" after destroying
	// menu->id, then the tooltip's window will have already been
	// deallocated by Xt, and unknown behaviour will ensue (probably
	// a core dump).
	gui_mch_destroy_beval_area(menu->tip);
	menu->tip = عدم;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    إذا (والد == menuBar)
	gui_mch_compute_menu_ارتفاع((Widget)0);
# ifdef FEAT_TOOLBAR
    وإلا إذا (والد == toolBar)
    {
	Cardinal    رقم_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_مقادیر(toolBar, XmNnumChildren, &رقم_children, عدم);
	إذا (رقم_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وإلا
	    gui.toolbar_ارتفاع = gui_mch_compute_toolbar_ارتفاع();
    }
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu استفادهنشده)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_آخرین_mouse_event());
    Xt_Manage_فرزند(menu->submenu_id);
# endif
}

#پایان_اگر // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    إذا (!gui.درون_use)
	أرجع;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#اگر_تعریف_شده FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#پایان_اگر
}


/*
 * Scrollbar stuff.
 */

    فراغ
gui_mch_مجموعه_scrollbar_thumb(
    scrollbar_T *sb,
    دراز	val,
    دراز	اندازه,
    دراز	max)
{
    إذا (sb->id != (Widget)0)
	Xt_Va_مجموعه_مقادیر(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_اندازه, اندازه,
		  XmNpageIncrement, (اندازه > 2 ? اندازه - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  عدم);
}

    فراغ
gui_mch_مجموعه_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    إذا (sb->id == (Widget)0)
	أرجع;

    إذا (sb->type == SBAR_چپ || sb->type == SBAR_راست)
    {
	إذا (y == 0)
	    h -= gui.border_جابه_جایی;
	وإلا
	    y -= gui.border_جابه_جایی;
	Xt_Va_مجموعه_مقادیر(sb->id,
		Xm_Ntop_جابه_جایی, y,
		Xm_Nbottom_جابه_جایی, -y - h,
		XmNwidth, w,
		عدم);
    }
    وإلا
	Xt_Va_مجموعه_مقادیر(sb->id,
		Xm_Ntop_جابه_جایی, y,
		Xm_Nleft_جابه_جایی, x,
		Xm_Nright_جابه_جایی, gui.which_scrollbars[SBAR_راست]
						     ? gui.scrollbar_عرض : 0,
		XmNheight, h,
		عدم);
    Xt_Manage_فرزند(sb->id);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_مقادیر(متن_Area, XtNwidth, &tw, XtNx, &tx, عدم);
    Xt_Va_Get_مقادیر(vimShell, XtNwidth, &ww, عدم);
    xpad = ww - tw - tx - gui.scrollbar_عرض;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_مقادیر(متن_Area, XtNheight, &th, XtNy, &ty, عدم);
    Xt_Va_Get_مقادیر(vimShell, XtNheight, &wh, عدم);
    ypad = wh - th - ty - gui.scrollbar_ارتفاع;
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    Arg		آرگومانها[16];
    صحيح		n;

    إذا (sb->id == (Widget)0)
	أرجع;

    n = 0;
    إذا (flag)
    {
	بدل (sb->type)
	{
	    حالة SBAR_چپ:
		Xt_مجموعه_Arg(آرگومانها[n], Xm_Nleft_جابه_جایی, gui.scrollbar_عرض); n++;
		اكسر;

	    حالة SBAR_راست:
		Xt_مجموعه_Arg(آرگومانها[n], Xm_Nright_جابه_جایی, gui.scrollbar_عرض); n++;
		اكسر;

	    حالة SBAR_BOTTOM:
		Xt_مجموعه_Arg(آرگومانها[n], Xm_Nbottom_جابه_جایی, gui.scrollbar_ارتفاع);n++;
		اكسر;
	}
	Xt_مجموعه_مقادیر(متن_Area, آرگومانها, n);
	Xt_Manage_فرزند(sb->id);
    }
    وإلا
    {
	إذا (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    بدل (sb->type)
	    {
		حالة SBAR_چپ:
		    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nleft_جابه_جایی, 0); n++;
		    اكسر;

		حالة SBAR_راست:
		    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nright_جابه_جایی, 0); n++;
		    اكسر;

		حالة SBAR_BOTTOM:
		    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nbottom_جابه_جایی, 0);n++;
		    اكسر;
	    }
	    Xt_مجموعه_مقادیر(متن_Area, آرگومانها, n);
	}
	Xt_Unmanage_فرزند(sb->id);
    }
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		آرگومانها[16];
    صحيح		n = 0;

    Xt_مجموعه_Arg(آرگومانها[n], XmNminimum, 0); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    بدل (sb->type)
    {
	حالة SBAR_چپ:
	    Xt_مجموعه_Arg(آرگومانها[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNwidth, gui.scrollbar_عرض); n++;
	    اكسر;

	حالة SBAR_راست:
	    Xt_مجموعه_Arg(آرگومانها[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNwidth, gui.scrollbar_عرض); n++;
	    اكسر;

	حالة SBAR_BOTTOM:
	    Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNheight, gui.scrollbar_ارتفاع); n++;
	    اكسر;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, متن_Area_Form, آرگومانها, n);
    إذا (sb->id == (Widget)0)
	أرجع;

    gui_mch_مجموعه_scrollbar_colors(sb);
    Xt_جمع_کن_Callback(sb->id, Xm_Nvalue_تغییریافته_Callback,
	    scroll_cb, (Xt_اشاره_گر)sb->ident);
    Xt_جمع_کن_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_اشاره_گر)sb->ident);
    Xt_جمع_کن_Event_Handler(sb->id, کلید_Press_ماسک, FALSE, gui_x11_کلید_اصابت_cb,
	    (Xt_اشاره_گر)0);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    فراغ
gui_mch_مجموعه_scrollbar_colors(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
    {
	إذا (gui.scroll_bg_pixel != INVALCOLOR)
	{
#إذا (Xm_نسخه>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#پایان_اگر

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_مجموعه_مقادیر(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    عدم);
	}

	إذا (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_مجموعه_مقادیر(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    عدم);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    إذا (sb == &gui.bottom_sbar && متن_Area_Form != (Widget)0)
	gui_motif_scroll_colors(متن_Area_Form);
}

/*
 * Miscellaneous stuff:
 */

    پنجره
gui_x11_get_wid(فراغ)
{
    أرجع(Xt_پنجره(متن_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساكن فراغ
do_mnemonic(Widget w, بیعلامت صحيح keycode)
{
    WidgetList	    children;
    صحيح		    رقم_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    user_داده_2;
    بیعلامت حرف   ردیف_ستون_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_ردیف_ستونی_Widget_Class)
	{
	    Xt_Va_Get_مقادیر(w, Xm_Nrow_ستونی_Type, &ردیف_ستون_Type, عدم);
	    isMenu = (ردیف_ستون_Type != (بیعلامت حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_مقادیر(w, XmNchildren, &children, XmNnumChildren,
			  &رقم_Children, عدم);
	    لكل (i = 0; i < رقم_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وإلا
    {
	Xt_Va_Get_مقادیر(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    إذا (XKeysym_مقصد_Keycode(XtDisplay(Xt_والد(w)),
				       XString_مقصد_Keysym(mneString)) == keycode)
	    {
		إذا (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_مقادیر(w, Xm_Nuser_داده, &user_داده_2, عدم);
		    إذا (user_داده_2 != عدم && XtIsWidget(user_داده_2))
			XmProcessTraversal(user_داده_2, XmTRAVERSE_CURRENT);
		}
		وإلا
		{
		    XKeyPressedEvent کلید_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_فیلد(کلید_Event);
		    کلید_Event.type = کلید_Press;
		    کلید_Event.serial = 1;
		    کلید_Event.send_event = True;
		    کلید_Event.display = XtDisplay(w);
		    کلید_Event.پنجره_2 = Xt_پنجره(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & کلید_Event,
								     عدم, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ساكن فراغ
mnemonic_event(
	Widget	    w,
	Xt_اشاره_گر   call_داده استفادهنشده,
	XKeyEvent   *event,
	Boolean	    *b استفادهنشده)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ساكن فراغ
جمع_کن_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحيح		    رقم_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    بیعلامت حرف   ردیف_ستون_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_ردیف_ستونی_Widget_Class)
	{
	    Xt_Va_Get_مقادیر(w, Xm_Nrow_ستونی_Type, &ردیف_ستون_Type, عدم);
	    isMenu = (ردیف_ستون_Type != (بیعلامت حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_مقادیر(w, XmNchildren, &children, XmNnumChildren,
							  &رقم_Children, عدم);
	    لكل (i = 0; i < رقم_Children; i++)
		جمع_کن_mnemonic_grabs(dialog, children[i]);
	}
    }
    وإلا
    {
	Xt_Va_Get_مقادیر(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_کلید(dialog, XKeysym_مقصد_Keycode(XtDisplay(dialog),
						  XString_مقصد_Keysym(mneString)),
		    Mod1_ماسک, True, Grab_نما_Async, Grab_نما_Async);
	}
    }
}

/*
 * Add a handler for mnemonics in a dialog.  Motif itself only handles
 * mnemonics in menus. Mnemonics added or changed after this call will be
 * ignored.
 *
 * To add a mnemonic to a text field or list, set the XmNmnemonic resource on
 * the appropriate label and set the XmNuserData resource of the label to the
 * widget to get the focus when the mnemonic is typed.
 */
    ساكن فراغ
activate_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_جمع_کن_Event_Handler(dialog, کلید_Press_ماسک, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشاره_گر) عدم);
    جمع_کن_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساكن فراغ
suppress_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_Ungrab_کلید(dialog, Any_کلید, Mod1_ماسک);
    XtRemoveEventHandler(dialog, کلید_Press_ماسک, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشاره_گر) عدم);
}

#إذا defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساكن فراغ
مجموعه_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    إذا (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_مجموعه *)&gui.fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_فرزند(id);
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_فرزند(id);
	    }
	    وإلا
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# وإلا
    إذا (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_فرزند(id);
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_فرزند(id);
	    }
	    وإلا
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
}
#پایان_اگر

#إذا defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعريفنوع بنية dialog_callback_arg
{
    حرف *  آرگومانها;   // not used right now
    صحيح	    id;
} dcbarg_T;

ساكن Widget dialog_wgt;
ساكن حرف *browse_fname = عدم;
ساكن Xm_String_Char_مجموعه charset = (Xm_String_Char_مجموعه) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساكن فراغ DialogCancelCB(Widget, Xt_اشاره_گر, Xt_اشاره_گر);
ساكن فراغ DialogAcceptCB(Widget, Xt_اشاره_گر, Xt_اشاره_گر);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ساكن فراغ
مجموعه_predefined_label(Widget والد, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *بعدی;
    کلید_Sym	mnemonic = NUL;

    w = Xt_Name_مقصد_Widget(والد, name);

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)new_label);
    إذا (p == عدم)
	أرجع;
    لكل (بعدی = p; *بعدی; ++بعدی)
    {
	إذا (*بعدی == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(بعدی);

	    إذا (len > 0)
	    {
		mch_memmove(بعدی, بعدی + 1, len);
		mnemonic = بعدی[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_برچسب);
    vim_free(p);

    إذا (str != عدم)
    {
	Xt_Va_مجموعه_مقادیر(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
مجموعه_predefined_fontlist(Widget والد, String name)
{
    Widget w;
    w = Xt_Name_مقصد_Widget(والد, name);

    إذا (!w)
	أرجع;

    مجموعه_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحيح		saving استفادهنشده,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext استفادهنشده,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*الگو;
    char_u	*tofree = عدم;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", عدم, 0);

    إذا (initdir == عدم || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    إذا (dflt == عدم)
	dflt = (char_u *)"";
    وإلا إذا (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	جمع_کن_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    إذا (filter == عدم)
	الگو = (char_u *)"";
    وإلا
    {
	char_u	*s, *p;

	s = filter;
	لكل (p = filter; *p != NUL; ++p)
	{
	    إذا (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    إذا (*p == ';' || *p == '\n')	// end of (first) pattern
		اكسر;
	}
	الگو = vim_strnsave(s, p - s);
	tofree = الگو;
	إذا (الگو == عدم)
	    الگو = (char_u *)"";
    }

    Xt_Va_مجموعه_مقادیر(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (حرف *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (حرف *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (حرف *)الگو, STRLEN(الگو) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (حرف *)title, STRLEN(title) + 1,
	عدم);

    مجموعه_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    مجموعه_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    مجموعه_predefined_label(dialog_wgt, "Dir", _("Directories"));
    مجموعه_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    مجموعه_predefined_label(dialog_wgt, "Help", _("&Help"));
    مجموعه_predefined_label(dialog_wgt, "Items", _("Files"));
    مجموعه_predefined_label(dialog_wgt, "OK", _("&OK"));
    مجموعه_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    مجموعه_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    مجموعه_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_مجموعه_مقادیر(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, عدم);

    Xt_جمع_کن_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_اشاره_گر)0);
    Xt_جمع_کن_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_اشاره_گر)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_فرزند(Xm_File_Selection_Box_Get_فرزند(dialog_wgt,
					(بیعلامت حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    افعل
    {
	XtAppProcessEvent(Xt_Widget_مقصد_Application_Context(dialog_wgt),
	    (Xt_درون_داد_ماسک)XtIMAll);
    } طالما (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    إذا (browse_fname == عدم)
	أرجع عدم;
    أرجع vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ساكن فراغ
DialogCancelCB(
    Widget	w استفادهنشده,		//  widget id
    Xt_اشاره_گر	client_داده_2 استفادهنشده,	//  data from application
    Xt_اشاره_گر	call_داده استفادهنشده)	//  data from widget class
{
    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    Xt_Unmanage_فرزند(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساكن فراغ
DialogAcceptCB(
    Widget	w استفادهنشده,		//  widget id
    Xt_اشاره_گر	client_داده_2 استفادهنشده,	//  data from application
    Xt_اشاره_گر	call_داده)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_داده;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->مقدار, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_فرزند(dialog_wgt);
}

#پایان_اگر // FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

ساكن صحيح	dialog_وضعیت;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساكن فراغ
keyhit_callback(
    Widget		w,
    Xt_اشاره_گر		client_داده_2 استفادهنشده,
    XEvent		*event,
    Boolean		*cont استفادهنشده)
{
    حرف	buf[2];
    کلید_Sym	کلید_sym;

    إذا (XLookupString(&(event->xkey), buf, 2, &کلید_sym, عدم) == 1)
    {
	إذا (*buf == CAR)
	    dialog_وضعیت = 1;
	وإلا إذا (*buf == ESC)
	    dialog_وضعیت = 2;
    }
    إذا ((کلید_sym == XK_چپ || کلید_sym == XK_راست)
	    && !(event->xkey.state & لغزش_ماسک))
	Xm_متن_فیلد_Clear_Selection(w, Xt_آخرین_Timestamp_Processed(gui.dpy));
}

    ساكن فراغ
butproc(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    dialog_وضعیت = (صحيح)(دراز)client_داده_2 + 1;
}

# ifdef HAVE_XPM

    ساكن Widget
create_pixmap_label(
    Widget	والد,
    String	name,
    حرف	**داده,
    ArgList	آرگومانها,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحيح			عمق;
    Pixmap		pixmap = 0;
    Xpm_صفات	attr;
    Boolean		rs;
    Xpm_Color_نماد	color[5] =
    {
	{"none", عدم, 0},
	{"iconColor1", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"selectColor", عدم, 0}
    };

    label = XmCreateLabelGadget(والد, name, آرگومانها, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_از_Object(label);
    scr = Xt_Screen_از_Object(label);
    Xt_Va_Get_مقادیر(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_والد(label),
		  XmNdepth, &عمق,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  عدم);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_عمق;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.عمق = عمق;
    Xpm_Create_Pixmap_From_داده(dsp, ریشه_پنجره_از_Screen(scr),
		    داده, &pixmap, عدم, &attr);

    Xt_Va_Get_مقادیر(label, Xm_Nrecompute_اندازه, &rs, عدم);
    Xt_Va_مجموعه_مقادیر(label, Xm_Nrecompute_اندازه, True, عدم);
    Xt_Va_مجموعه_مقادیر(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    عدم);
    Xt_Va_مجموعه_مقادیر(label, Xm_Nrecompute_اندازه, rs, عدم);

    أرجع label;
}
# endif

    صحيح
gui_mch_dialog(
    صحيح		type استفادهنشده,
    char_u	*title,
    char_u	*پیام_2,
    char_u	*button_نامها,
    صحيح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحيح		ex_cmd استفادهنشده)
{
    char_u		*buts;
    char_u		*p, *بعدی;
    XtAppContext	app;
    XmString		label;
    صحيح			butcount;
    Widget		w;
    Widget		dialogform = عدم;
    Widget		form = عدم;
    Widget		dialogtextfield = عدم;
    Widget		*buttons;
    Widget		sep_form = عدم;
    Boolean		vertical;
    Widget		separator = عدم;
    صحيح			n;
    Arg			آرگومانها[6];
# ifdef HAVE_XPM
    حرف		**icon_داده_2 = عدم;
    Widget		dialogpixmap = عدم;
# endif

    إذا (title == عدم)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (حرف *)"dialog", عدم, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != عدم);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((حرف *)title);
    إذا (label == عدم)
	أرجع -1;
    Xt_Va_مجموعه_مقادیر(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    عدم);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_نامها);
    إذا (buts == عدم)
	أرجع -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    لكل (p = buts; *p; ++p)
	إذا (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    إذا (buttons == عدم)
    {
	vim_free(buts);
	أرجع -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    لكل (butcount = 0; *p; ++butcount)
    {
	کلید_Sym mnemonic = NUL;

	لكل (بعدی = p; *بعدی; ++بعدی)
	{
	    إذا (*بعدی == DLG_HOTKEY_CHAR)
	    {
		صحيح len = STRLEN(بعدی);

		إذا (len > 0)
		{
		    mch_memmove(بعدی, بعدی + 1, len);
		    mnemonic = بعدی[0];
		}
	    }
	    إذا (*بعدی == DLG_BUTTON_SEP)
	    {
		*بعدی++ = NUL;
		اكسر;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_برچسب);
	إذا (label == عدم)
	    اكسر;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_افزودن_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_جابه_جایی, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		عدم);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	إذا (butcount > 0)
	{
	    إذا (vertical)
		Xt_Va_مجموعه_مقادیر(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			عدم);
	    وإلا
	    {
		إذا (*بعدی == NUL)
		{
		    Xt_Va_مجموعه_مقادیر(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_جابه_جایی, 4,
			    عدم);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_جابه_جایی, 4,
			    عدم);
		    Xt_Manage_فرزند(sep_form);
		}
		وإلا
		{
		    Xt_Va_مجموعه_مقادیر(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    عدم);
		}
	    }
	}
	وإلا إذا (!vertical)
	{
	    إذا (*بعدی == NUL)
	    {
		Xt_Va_مجموعه_مقادیر(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_جابه_جایی, 4,
			عدم);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_جابه_جایی, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_جابه_جایی, 4,
			عدم);
		Xt_Manage_فرزند(sep_form);
	    }
	    وإلا
		Xt_Va_مجموعه_مقادیر(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_جابه_جایی, 4,
			عدم);
	}

	Xt_جمع_کن_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_اشاره_گر)(دراز)butcount);
	p = بعدی;
    }
    vim_free(buts);

    separator = (Widget) 0;
    إذا (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_مجموعه_Arg(آرگومانها[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_مجموعه_Arg(آرگومانها[n], XmNbottomWidget, buttons[0]); n++;
	Xt_مجموعه_Arg(آرگومانها[n], Xm_Nbottom_جابه_جایی, 4); n++;
	Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", آرگومانها, n);
	Xt_Manage_فرزند(separator);
    }

    إذا (textfield != عدم)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_متن_فیلد_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	إذا (butcount > 0)
	    Xt_Va_مجموعه_مقادیر(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_مجموعه_مقادیر(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	مجموعه_fontlist(dialogtextfield);
	Xm_متن_فیلد_مجموعه_String(dialogtextfield, (حرف *)textfield);
	Xt_Manage_فرزند(dialogtextfield);
	Xt_جمع_کن_Event_Handler(dialogtextfield, کلید_Press_ماسک, False,
			    (XtEventHandler)keyhit_callback, (Xt_اشاره_گر)عدم);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    عدم);
    Xt_Manage_فرزند(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    بدل (type)
    {
	حالة VIM_GENERIC:
	    icon_داده_2 = generic_xpm;
	    اكسر;
	حالة VIM_ERROR:
	    icon_داده_2 = error_xpm;
	    اكسر;
	حالة VIM_WARNING:
	    icon_داده_2 = alert_xpm;
	    اكسر;
	حالة VIM_INFO:
	    icon_داده_2 = info_xpm;
	    اكسر;
	حالة VIM_QUESTION:
	    icon_داده_2 = quest_xpm;
	    اكسر;
	افتراضي:
	    icon_داده_2 = generic_xpm;
    }

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Ntop_جابه_جایی, 8); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nbottom_جابه_جایی, 8); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nleft_جابه_جایی, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_داده_2, آرگومانها, n);
    Xt_Manage_فرزند(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)پیام_2, STRING_برچسب);
    إذا (label == عدم)
    {
	vim_free(buttons);
	أرجع -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				Xm_Ntop_جابه_جایی, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# وإلا
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_جابه_جایی, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_جابه_جایی, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_جابه_جایی, 8,
				عدم);
    XmStringFree(label);
    مجموعه_fontlist(w);

    إذا (textfield != عدم)
    {
	Xt_Va_مجموعه_مقادیر(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		عدم);
    }
    وإلا
    {
	إذا (butcount > 0)
	    Xt_Va_مجموعه_مقادیر(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_مجموعه_مقادیر(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
    }

    إذا (dfltbutton < 1)
	dfltbutton = 1;
    إذا (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_مجموعه_مقادیر(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], عدم);
    إذا (textfield != عدم)
	Xt_Va_مجموعه_مقادیر(dialogform, XmNinitialFocus, dialogtextfield, عدم);
    وإلا
	Xt_Va_مجموعه_مقادیر(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									عدم);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    إذا (textfield != عدم && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_متن_فیلد_مجموعه_Selection(dialogtextfield,
			 (Xm_متن_Position)0, (Xm_متن_Position)STRLEN(textfield),
					   Xt_آخرین_Timestamp_Processed(gui.dpy));
	Xm_متن_فیلد_مجموعه_Cursor_Position(dialogtextfield,
					   (Xm_متن_Position)STRLEN(textfield));
    }

    app = Xt_Widget_مقصد_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_وضعیت = -1;
    لكل (;;)
    {
	XtAppProcessEvent(app, (Xt_درون_داد_ماسک)XtIMAll);
	إذا (dialog_وضعیت >= 0 || !XtIsManaged(dialogform))
	    اكسر;
    }

    vim_free(buttons);

    إذا (textfield != عدم)
    {
	p = (char_u *)Xm_متن_Get_String(dialogtextfield);
	إذا (p == عدم || dialog_وضعیت < 0)
	    *textfield = NUL;
	وإلا
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((حرف *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    أرجع dialog_وضعیت;
}
#پایان_اگر // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    Cardinal	رقم_Children;	    // how many children toolBar has

    إذا (toolBar == (Widget)0)
	أرجع;
    Xt_Va_Get_مقادیر(toolBar, XmNnumChildren, &رقم_Children, عدم);
    إذا (showit && رقم_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_مقادیر(toolBar, XmNchildren, &children, عدم);
	{
	    فراغ    (*action)(BalloonEval *);
	    صحيح	    متن = 0;

	    إذا (جستجوی_رشته((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وإلا
		action = &gui_mch_disable_beval_area;
	    إذا (جستجوی_رشته((ثابت حرف *)p_toolbar, "text"))
		متن = 1;
	    وإلا إذا (جستجوی_رشته((ثابت حرف *)p_toolbar, "icons"))
		متن = -1;
	    إذا (متن != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    إذا (menu_is_toolbar(toolbar->dname))
			اكسر;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		إذا (toolbar)
		{
		    لكل (cur = toolbar->children; cur; cur = cur->بعدی)
		    {
			Arg	    آرگومانها[1];
			صحيح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			إذا (cur->tip != عدم)
			    (*action)(cur->tip);
			إذا (!menu_is_separator(cur->name))
			{
			    إذا (متن == 1 || cur->xpm == عدم)
			    {
				Xt_مجموعه_Arg(آرگومانها[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    إذا (cur->id != عدم)
			    {
				Xt_Unmanage_فرزند(cur->id);
				Xt_مجموعه_مقادیر(cur->id, آرگومانها, n);
				Xt_Manage_فرزند(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_ارتفاع = gui_mch_compute_toolbar_ارتفاع();
	Xt_Manage_فرزند(Xt_والد(toolBar));
# ifdef FEAT_GUI_TABLINE
	إذا (showing_tabline)
	{
	    Xt_Va_مجموعه_مقادیر(tab_سطر,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  عدم);
	    Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
# endif
	    Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  عدم);
	إذا (XtIsManaged(menuBar))
	    Xt_Va_مجموعه_مقادیر(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    عدم);
	وإلا
	    Xt_Va_مجموعه_مقادیر(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    عدم);
    }
    وإلا
    {
	gui.toolbar_ارتفاع = 0;
	إذا (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
	وإلا
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}

	Xt_Unmanage_فرزند(Xt_والد(toolBar));
    }
    gui_مجموعه_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    ساكن فراغ
reset_focus(فراغ)
{
    إذا (متن_Area != عدم)
	XmProcessTraversal(متن_Area, XmTRAVERSE_CURRENT);
}

    صحيح
gui_mch_compute_toolbar_ارتفاع(فراغ)
{
    Dimension	borders;
    Dimension	ارتفاع;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	رقم_Children;	    // how many children toolBar has
    صحيح		i;

    borders = 0;
    ارتفاع = 0;
    إذا (toolBar != (Widget)0 && tool_Bar_قاب != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_مقادیر(tool_Bar_قاب,
		XmNshadowThickness, &fst,
		Xm_Nmargin_ارتفاع, &fmh,
		عدم);
	borders += fst + fmh;
	Xt_Va_Get_مقادیر(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_ارتفاع, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &رقم_Children, عدم);
	borders += tst + tmh;
	لكل (i = 0; i < (صحيح)رقم_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_مقادیر(children[i], XmNheight, &whgt, عدم);
	    إذا (ارتفاع < whgt)
		ارتفاع = whgt;
	}
    }
# ifdef LESSTIF_نسخه
    // Hack: When starting up we get wrong dimensions.
    إذا (ارتفاع < 10)
	ارتفاع = 24;
# endif

    أرجع (صحيح)(ارتفاع + (borders << 1));
}

    فراغ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_مقادیر(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    عدم);
}
#پایان_اگر

#إذا defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (tab_سطر == (Widget)0)
	أرجع;

    إذا (!showit != !showing_tabline)
    {
	إذا (showit)
	{
	    Xt_Manage_فرزند(tab_سطر);
	    Xt_Unmanage_فرزند(Xt_Name_مقصد_Widget(tab_سطر, "PageScroller"));
	    Xt_Unmanage_فرزند(Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerNext"));
	    Xt_Unmanage_فرزند(Xt_Name_مقصد_Widget(tab_سطر,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	    Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
	{
	    Xt_Unmanage_فرزند(tab_سطر);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_مجموعه_مقادیر(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline(فراغ)
{
    أرجع tab_سطر != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline(فراغ)
{
    tabpage_T		*tp;
    صحيح			nr = 1, n;
    Arg			آرگومانها[10];
    صحيح			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_صفحه_Info	صفحه_info;
    Xm_Notebook_صفحه_وضعیت صفحه_وضعیت;
    صحيح			آخرین_صفحه, tab_شمار;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = اولی_tabpage; tp != عدم; tp = tp->tp_بعدی, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	صفحه_وضعیت = Xm_Notebook_Get_صفحه_Info(tab_سطر, nr, &صفحه_info);
	إذا (صفحه_وضعیت == Xm_صفحه_INVALID
		|| صفحه_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nnotebook_فرزند_Type, XmMAJOR_TAB); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNtraversalOn, False); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNhighlightThickness, 1); n++;
	    Xt_مجموعه_Arg(آرگومانها[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_افزودن_Button(tab_سطر, "-Empty-", آرگومانها, n);
	    Xt_Manage_فرزند(tab);
	    beval = gui_mch_create_beval_area(tab, عدم, tabline_balloon_cb,
									عدم);
	    Xt_Va_مجموعه_مقادیر(tab, Xm_Nuser_داده, beval, عدم);
	}
	وإلا
	    tab = صفحه_info.major_tab_widget;

	Xt_Va_مجموعه_مقادیر(tab, Xm_Npage_عدد, nr, عدم);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_مقادیر(tab, XmNlabelString, &label_str, عدم);
	إذا (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    إذا (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_مجموعه_مقادیر(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, عدم);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_فرزند(tab);
		Xt_Manage_فرزند(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_شمار = nr - 1;

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Nlast_صفحه_عدد, &آخرین_صفحه, عدم);

    // Remove any old labels.
    طالما (nr <= آخرین_صفحه)
    {
	إذا (Xm_Notebook_Get_صفحه_Info(tab_سطر, nr, &صفحه_info) != Xm_صفحه_INVALID
	    && صفحه_info.صفحه_عدد == nr
	    && صفحه_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_مقادیر(صفحه_info.major_tab_widget, Xm_Nuser_داده, &beval, عدم);
	    إذا (beval != عدم)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_فرزند(صفحه_info.major_tab_widget);
	    XtDestroyWidget(صفحه_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_مجموعه_مقادیر(tab_سطر, Xm_Nlast_صفحه_عدد, tab_شمار, عدم);

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Ncurrent_صفحه_عدد, &currentpage, عدم);
    إذا (currentpage != curtabidx)
	Xt_Va_مجموعه_مقادیر(tab_سطر, Xm_Ncurrent_صفحه_عدد, curtabidx, عدم);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_مجموعه_curtab(صحيح nr)
{
    صحيح		currentpage;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Ncurrent_صفحه_عدد, &currentpage, عدم);
    إذا (currentpage != nr)
	Xt_Va_مجموعه_مقادیر(tab_سطر, Xm_Ncurrent_صفحه_عدد, nr, عدم);
}
#پایان_اگر

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ساكن فراغ
gui_motif_menu_colors(Widget id)
{
    إذا (gui.menu_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخه >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#وإلا
	Xt_Va_مجموعه_مقادیر(id, XmNbackground, gui.menu_bg_pixel, عدم);
#پایان_اگر
    إذا (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_مجموعه_مقادیر(id, XmNforeground, gui.menu_fg_pixel, عدم);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ساكن فراغ
gui_motif_scroll_colors(Widget id)
{
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخه >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#وإلا
	Xt_Va_مجموعه_مقادیر(id, XmNbackground, gui.scroll_bg_pixel, عدم);
#پایان_اگر
    إذا (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_مجموعه_مقادیر(id, XmNforeground, gui.scroll_fg_pixel, عدم);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    فراغ
gui_motif_menu_fontlist(Widget id استفادهنشده)
{
#اگر_تعریف_شده FEAT_MENU
# ifdef FONTSET_ALWAYS
    إذا (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_مجموعه *)&gui.menu_fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_فرزند(id);
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_فرزند(id);
	    }
	    وإلا
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# وإلا
    إذا (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_فرزند(id);
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_فرزند(id);
	    }
	    وإلا
		Xt_Va_مجموعه_مقادیر(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
#پایان_اگر
}


/*
 * We don't create it twice for the sake of speed.
 */

تعريفنوع بنية Shared_پیدا_کن_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget به_بالا;		// search direction 'Up' radio button
    Widget پایین;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget پیدا_کن;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_پیدا_کن_Replace_2;

ساكن Shared_پیدا_کن_Replace_2 پیدا_کن_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن Shared_پیدا_کن_Replace_2 repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن فراغ
پیدا_کن_replace_destroy_callback(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    Shared_پیدا_کن_Replace_2 *cd = (Shared_پیدا_کن_Replace_2 *)client_داده_2;

    إذا (cd != عدم)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساكن فراغ
پیدا_کن_replace_dismiss_callback(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    Shared_پیدا_کن_Replace_2 *cd = (Shared_پیدا_کن_Replace_2 *)client_داده_2;

    إذا (cd != عدم)
	Xt_Unmanage_فرزند(cd->dialog);
}

    ساكن فراغ
مدخل_activate_callback(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    XmProcessTraversal((Widget)client_داده_2, XmTRAVERSE_CURRENT);
}

    ساكن فراغ
پیدا_کن_replace_callback(
    Widget	w استفادهنشده,
    Xt_اشاره_گر	client_داده_2,
    Xt_اشاره_گر	call_داده استفادهنشده)
{
    long_u	flags = (long_u)client_داده_2;
    حرف	*پیدا_کن_متن, *repl_متن;
    Boolean	direction_پایین = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_پیدا_کن_Replace_2 *sfr;

    إذا (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = خالی_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	أرجع;
    }

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &پیدا_کن_widgets;
    }
    وإلا
    {
	repl_متن = Xm_متن_فیلد_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    پیدا_کن_متن = Xm_متن_فیلد_Get_String(sfr->what);
    Xt_Va_Get_مقادیر(sfr->پایین, XmNset, &direction_پایین, عدم);
    Xt_Va_Get_مقادیر(sfr->wword, XmNset, &wword, عدم);
    Xt_Va_Get_مقادیر(sfr->mcase, XmNset, &mcase, عدم);
    إذا (wword)
	flags |= FRD_کامل_کلمه;
    إذا (mcase)
	flags |= FRD_تطابق_CASE;

    (فراغ)gui_do_findrepl((صحيح)flags, (char_u *)پیدا_کن_متن, (char_u *)repl_متن,
							      direction_پایین);

    إذا (پیدا_کن_متن != عدم)
	XtFree(پیدا_کن_متن);
    إذا (repl_متن != عدم)
	XtFree(repl_متن);
}

    ساكن فراغ
پیدا_کن_replace_keypress(
    Widget		w استفادهنشده,
    Shared_پیدا_کن_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b استفادهنشده)
{
    کلید_Sym keysym;

    إذا (frdp == عدم)
	أرجع;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    إذا (keysym == XK_Escape)
	Xt_Unmanage_فرزند(frdp->dialog);
}

    ساكن فراغ
مجموعه_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *بعدی;
    کلید_Sym	mnemonic = NUL;

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)label);
    إذا (p == عدم)
	أرجع;
    لكل (بعدی = p; *بعدی; ++بعدی)
    {
	إذا (*بعدی == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(بعدی);

	    إذا (len > 0)
	    {
		mch_memmove(بعدی, بعدی + 1, len);
		mnemonic = بعدی[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    إذا (str)
    {
	Xt_Va_مجموعه_مقادیر(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
پیدا_کن_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_پیدا_کن_Replace_2	*frdp;
    Widget		separator;
    Widget		درون_داد_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		قاب;
    XmString		str;
    صحيح			n;
    Arg			آرگومانها[6];
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    Dimension		عرض;
    Dimension		widest;
    char_u		*مدخل_متن;

    frdp = do_replace ? &repl_widgets : &پیدا_کن_widgets;

    // Get the search string to use.
    مدخل_متن = get_پیدا_کن_dialog_متن(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	إذا (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_پنجره(Xt_والد(frdp->dialog)));
	وإلا
	    Xt_Manage_فرزند(frdp->dialog);
	XtPopup(Xt_والد(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	إذا (مدخل_متن != عدم)
	    Xm_متن_فیلد_مجموعه_String(frdp->what, (حرف *)مدخل_متن);
	vim_free(مدخل_متن);

	Xt_Va_مجموعه_مقادیر(frdp->wword, XmNset, wword, عدم);
	أرجع;
    }

    // Create a fresh new dialog window
    إذا (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وإلا
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNautoUnmanage, False); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNnoResize, True); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", آرگومانها, n);
    XmStringFree(str);
    Xt_جمع_کن_Callback(frdp->dialog, XmNdestroyCallback,
	    پیدا_کن_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_جابه_جایی, 4,
	    عدم);

    frdp->پیدا_کن = XtVaCreateManagedWidget("findButton",
	    xm_افزودن_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);
    مجموعه_label(frdp->پیدا_کن, _("Find &Next"));

    Xt_جمع_کن_Callback(frdp->پیدا_کن, XmNactivateCallback,
	    پیدا_کن_replace_callback,
	    (do_replace ? (Xt_اشاره_گر)FRD_R_FINDNEXT : (Xt_اشاره_گر)FRD_FINDNEXT));

    إذا (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->پیدا_کن,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	مجموعه_label(frdp->replace, _("&Replace"));
	Xt_جمع_کن_Callback(frdp->replace, XmNactivateCallback,
		پیدا_کن_replace_callback, (Xt_اشاره_گر)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	مجموعه_label(frdp->all, _("Replace &All"));
	Xt_جمع_کن_Callback(frdp->all, XmNactivateCallback,
		پیدا_کن_replace_callback, (Xt_اشاره_گر)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	مجموعه_label(frdp->undo, _("&Undo"));
	Xt_جمع_کن_Callback(frdp->undo, XmNactivateCallback,
		پیدا_کن_replace_callback, (Xt_اشاره_گر)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_افزودن_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    عدم);
    مجموعه_label(frdp->cancel, _("&Cancel"));
    Xt_جمع_کن_Callback(frdp->cancel, XmNactivateCallback,
	    پیدا_کن_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_فرزند(button_form);

    n = 0;
    Xt_مجموعه_Arg(آرگومانها[n], XmNorientation, XmVERTICAL); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNrightWidget, button_form); n++;
    Xt_مجموعه_Arg(آرگومانها[n], Xm_Nright_جابه_جایی, 4); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_مجموعه_Arg(آرگومانها[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", آرگومانها, n);
    Xt_Manage_فرزند(separator);

    درون_داد_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 4,
	    عدم);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, درون_داد_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_جابه_جایی, 4,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_متن_فیلد_Widget_Class, درون_داد_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		عدم);

	إذا (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_متن_فیلد_Widget_Class,	درون_داد_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_جابه_جایی, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	    Xt_جمع_کن_Callback(frdp->with, XmNactivateCallback,
		    پیدا_کن_replace_callback, (Xt_اشاره_گر) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, درون_داد_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_جابه_جایی, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_جمع_کن_Callback(frdp->what, XmNactivateCallback,
		    مدخل_activate_callback, frdp->with);
	    Xt_جمع_کن_Event_Handler(frdp->with, کلید_Press_ماسک, False,
			    (XtEventHandler)پیدا_کن_replace_keypress,
			    (Xt_اشاره_گر) frdp);

	}
	وإلا
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_جمع_کن_Callback(frdp->what, XmNactivateCallback,
		    پیدا_کن_replace_callback, (Xt_اشاره_گر)FRD_FINDNEXT);
	}
	Xt_جمع_کن_Event_Handler(frdp->what, کلید_Press_ماسک, False,
			    (XtEventHandler)پیدا_کن_replace_keypress,
			    (Xt_اشاره_گر)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_مجموعه_Arg(آرگومانها[n], XmNwidth, &عرض); n++;
	Xt_Get_مقادیر(label_what, آرگومانها, n);
	widest = عرض;
	إذا (do_replace)
	{
	    Xt_Get_مقادیر(label_with, آرگومانها, n);
	    إذا (عرض > widest)
		widest = عرض;
	}

	Xt_Va_مجموعه_مقادیر(frdp->what, Xm_Nleft_جابه_جایی, widest, عدم);
	إذا (do_replace)
	    Xt_Va_مجموعه_مقادیر(frdp->with, Xm_Nleft_جابه_جایی, widest, عدم);

    }

    Xt_Manage_فرزند(درون_داد_form);

    {
	Widget radio_box;
	Widget w;

	قاب = XtVaCreateWidget("directionFrame",
		xm_قاب_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, درون_داد_form,
		Xm_Ntop_جابه_جایی, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_جابه_جایی, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, درون_داد_form,
		عدم);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, قاب,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_قاب_TITLE_فرزند,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(قاب, "radioBox",
		(ArgList)عدم, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->به_بالا = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->به_بالا);

	str = XmStringCreateSimple(_("Down"));
	frdp->پایین = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->پایین);

	Xt_Manage_فرزند(radio_box);
	Xt_Manage_فرزند(قاب);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, قاب,
	    Xm_Nright_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, درون_داد_form,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_جابه_جایی, 4,
	    عدم);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNset, wword,
	    عدم);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_جابه_جایی, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_جابه_جایی, 4,
	    XmNset, mcase,
	    عدم);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_فرزند(toggle_form);

    إذا (مدخل_متن != عدم)
	Xm_متن_فیلد_مجموعه_String(frdp->what, (حرف *)مدخل_متن);
    vim_free(مدخل_متن);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   فراغ
gui_mch_پیدا_کن_dialog(exarg_T *eap)
{
    إذا (!gui.درون_use)
	أرجع;

    پیدا_کن_replace_dialog_create(eap->arg, FALSE);
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (!gui.درون_use)
	أرجع;

    پیدا_کن_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    فراغ
gui_motif_synch_fonts(فراغ)
{
    Shared_پیدا_کن_Replace_2 *frdp;
    صحيح		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    إذا (font == عدم)
	أرجع;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    لكل (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&پیدا_کن_widgets);
	إذا (frdp->dialog)
	{
	    Xt_Va_مجموعه_مقادیر(frdp->what, XmNfontList, font_list, عدم);
	    إذا (do_replace)
		Xt_Va_مجموعه_مقادیر(frdp->with, XmNfontList, font_list, عدم);
	}
    }

    XmFontListFree(font_list);
}
