/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#شامل "vim.h"

#شامل <Xm/Form.h>
#شامل <Xm/RowColumn.h>
#شامل <Xm/PushB.h>
#شامل <Xm/Text.h>
#شامل <Xm/TextF.h>
#شامل <Xm/Separator.h>
#شامل <Xm/Label.h>
#شامل <Xm/CascadeB.h>
#شامل <Xm/ScrollBar.h>
#شامل <Xm/MenuShell.h>
#شامل <Xm/DrawingA.h>
#اگر (Xm_نسخہ >= 1002)
# include <Xm/RepType.h>
#اگر_ختم
#شامل <Xm/Frame.h>
#شامل <Xm/LabelG.h>
#شامل <Xm/ToggleBG.h>
#شامل <Xm/SeparatoG.h>
#شامل <Xm/XmP.h>

#شامل <X11/keysym.h>
#شامل <X11/Xatom.h>
#شامل <X11/StringDefs.h>
#شامل <X11/Intrinsic.h>
#اگر_تعریف_شدہ HAVE_X11_XPM_H
# اگر defined(VMS)
#  include <xpm.h>
# ورنہ
#  include <X11/xpm.h>
# endif
#ورنہ
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#اگر_ختم
#اگر_تعریف_شدہ HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#اگر_ختم

#شامل "gui_xmebw.h"	// for our Enhanced Button Widget

#اگر defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#اگر_ختم

#تعریف_کرو MOTIF_POPUP

خارجی Widget vimShell;

ساکن Widget vimForm;
ساکن Widget متن_Area_Form;
Widget متن_Area;
#اگر_تعریف_شدہ FEAT_TOOLBAR
ساکن Widget tool_Bar_چوکھٹا;
ساکن Widget toolBar;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
ساکن Widget	tab_سطر;
ساکن Widget	tab_سطر_menu = 0;
ساکن صحیح	showing_tabline = 0;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MENU
# اگر (Xm_نسخہ >= 1002)
// remember the last set value for the tearoff item
ساکن صحیح tearoff_val = (صحیح)XmTEAR_OFF_ENABLED;
# endif
ساکن Widget menuBar;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
ساکن خالی reset_focus(خالی);
#اگر_ختم

ساکن خالی gui_motif_menu_colors(Widget id);
ساکن خالی gui_motif_scroll_colors(Widget id);

#اگر (Xm_نسخہ >= 1002)
# define STRING_لیبل  Xm_FONTLIST_DEFAULT_لیبل
#ورنہ
# define STRING_لیبل  XmSTRING_DEFAULT_CHARSET
#اگر_ختم

/*
 * Call-back routines.
 */

    ساکن خالی
scroll_cb(Widget w غیر_مستعمل, Xt_اشارہ_گر client_ڈیٹا_2, Xt_اشارہ_گر call_ڈیٹا)
{
    scrollbar_T *sb;
    طویل	قدر;
    صحیح		dragging;

    sb = gui_ڈھونڈو_scrollbar((طویل)client_ڈیٹا_2);

    قدر = ((XmScrollBarCallbackStruct *)call_ڈیٹا)->قدر;
    dragging = (((XmScrollBarCallbackStruct *)call_ڈیٹا)->reason ==
							      (صحیح)XmCR_DRAG);
    gui_drag_scrollbar(sb, قدر, dragging);
}

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    ساکن خالی
tabline_cb(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,
    Xt_اشارہ_گر	call_ڈیٹا)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_ڈیٹا;
    اگر (nptr->reason != (صحیح)XmCR_NONE)
	send_tabline_event(nptr->صفحہ_نمبر);
}

    ساکن خالی
tabline_button_cb(
    Widget	w,
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    Xt_اشارہ_گر	cmd, tab_idx;

    Xt_Va_Get_اقدار(w, Xm_Nuser_ڈیٹا, &cmd, تہی);
    Xt_Va_Get_اقدار(tab_سطر_menu, Xm_Nuser_ڈیٹا, &tab_idx, تہی);

    send_tabline_menu_event((صحیح)(طویل)tab_idx, (صحیح)(طویل)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساکن خالی
motif_tabline_timer_cb (
    Xt_اشارہ_گر		timed_برآمد,
    XtIntervalId	*interval_id غیر_مستعمل)
{
    *((صحیح *)timed_برآمد) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ساکن صحیح
tabline_scroller_clicked(
    حرف		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	چوڑائی, اونچائی;

    tab_scroll_w = Xt_Name_مقصد_Widget(tab_سطر, scroller_name);
    اگر (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_اقدار(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &چوڑائی, XmNheight, &اونچائی, تہی);
	اگر (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    اگر (event->x >= pos_x && event->x <= pos_x + چوڑائی
		    && event->y >= pos_y && event->y <= pos_y + اونچائی)
		// Clicked on the scroller
		واپس TRUE;
	}
    }
    واپس FALSE;
}

    ساکن خالی
tabline_menu_cb(
    Widget	w,
    Xt_اشارہ_گر	احاطہ غیر_مستعمل,
    XEvent	*e,
    Boolean	*continue_dispatch غیر_مستعمل)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحیح				tab_idx = 0;
    WidgetList			children;
    Cardinal			عدد_Children;
    ساکن XtIntervalId		timer = (XtIntervalId)0;
    ساکن صحیح			timed_برآمد = TRUE;

    event = (XButtonPressedEvent *)e;

    اگر (event->button == Button1)
    {
	اگر (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    واپس;

	اگر (!timed_برآمد)
	{
	    Xt_Remove_وقت_برآمد(timer);
	    timed_برآمد = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	ورنہ
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = Xt_App_جوڑو_وقت_برآمد(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_برآمد);
	    timed_برآمد = FALSE;
	}
	واپس;
    }

    اگر (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_اقدار(tab_سطر_menu, Xm_Nuser_ڈیٹا, &tab_idx, تہی);
	send_tabline_menu_event(tab_idx, (صحیح)TABLINE_MENU_CLOSE);
	واپس;
    }

    اگر (event->button != Button3)
	واپس;

    // When ignoring events don't show the menu.
    اگر (hold_gui_events || cmdwin_type != 0)
	واپس;

    اگر (event->subwindow != None)
    {
	tab_w = Xt_کھڑکی_مقصد_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	اگر (tab_w != (Widget)0 && Xm_Is_ڈالو_Button(tab_w))
	    Xt_Va_Get_اقدار(tab_w, Xm_Npage_نمبر, &tab_idx, تہی);
    }

    Xt_Va_سیٹ_اقدار(tab_سطر_menu, Xm_Nuser_ڈیٹا, (Xt_اشارہ_گر)(طویل)tab_idx, تہی);
    Xt_Va_Get_اقدار(tab_سطر_menu, XmNchildren, &children, XmNnumChildren,
		  &عدد_Children, تہی);
    XtManageChildren(children, عدد_Children);
    XmMenuPosition(tab_سطر_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_بچہ(tab_سطر_menu);
}

    ساکن خالی
tabline_balloon_cb(BalloonEval *beval, صحیح state غیر_مستعمل)
{
    صحیح		nr;
    tabpage_T	*tp;

    اگر (beval->ہدف == (Widget)0)
	واپس;

    Xt_Va_Get_اقدار(beval->ہدف, Xm_Npage_نمبر, &nr, تہی);
    tp = ڈھونڈو_tabpage(nr);
    اگر (tp == تہی)
	واپس;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#اگر_ختم

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#شامل <Xm/XmP.h>
#شامل <Xm/LabelP.h>

ساکن XtExposeProc old_label_expose = تہی;

    ساکن خالی
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    غیرعلامتی حرف   label_type = (صحیح)XmSTRING;

    Xt_Va_Get_اقدار(_w, XmNlabelType, &label_type, (Xt_اشارہ_گر)0);

    اگر (XtIsSensitive(_w) || label_type != (صحیح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    ورنہ
    {
	XGCValues   اقدار;
	XtGCMask    نقاب;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	نقاب = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_نقاب | GCClipXOrigin | GCClipYOrigin;
	اقدار.graphics_exposures = False;

	اگر (fs != 0)
	{
	    نقاب |= GCFont;
	    اقدار.font = fs->fid;
	}

	اگر (lw->primitive.چوٹی_shadow_pixmap != None
		&& lw->primitive.چوٹی_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    نقاب |= GCFillStyle | GCTile;
	    اقدار.fill_style = FillTiled;
	    اقدار.tile = lw->primitive.چوٹی_shadow_pixmap;
	}

	lw->label.متن_Rect.x += 1;
	lw->label.متن_Rect.y += 1;
	اگر (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x += 1;
	    lw->label.acc_متن_Rect.y += 1;
	}

	اقدار.foreground = lw->primitive.چوٹی_shadow_color;
	اقدار.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, نقاب,
					       &اقدار, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.متن_Rect.x -= 1;
	lw->label.متن_Rect.y -= 1;
	اگر (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x -= 1;
	    lw->label.acc_متن_Rect.y -= 1;
	}

	اقدار.foreground = lw->primitive.bottom_shadow_color;
	اقدار.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, نقاب,
					       &اقدار, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    خالی
gui_x11_create_widgets(خالی)
{
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		دلائل[10];
    صحیح		n;
    XmString	xms;
#اگر_ختم

    /*
     * Install the 3D shade effect drawing routines.
     */
    اگر (old_label_expose == تہی)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_انحراف = gui.border_چوڑائی;

    /*
     * Install the tearOffModel resource converter.
     */
#اگر (Xm_نسخہ >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#اگر_ختم

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_سیٹ_اقدار(vimShell, XmNdeleteResponse, XmDO_NOTHING, تہی);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_چوڑائی, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nmargin_اونچائی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	تہی);
    gui_motif_menu_colors(vimForm);

#اگر_تعریف_شدہ FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحیح ac = 0;

# اگر (Xm_نسخہ >= 1002)
	Xt_سیٹ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_سیٹ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_سیٹ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_سیٹ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_سیٹ_Arg(al[ac], Xm_Nright_انحراف, 0); ac++;
# endif
	Xt_سیٹ_Arg(al[ac], Xm_Nmargin_اونچائی, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_بچہ(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_چوکھٹا = XtVaCreateWidget("toolBarFrame",
	xm_چوکھٹا_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_اونچائی, 0,
	Xm_Nmargin_چوڑائی, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	تہی);
    gui_motif_menu_colors(tool_Bar_چوکھٹا);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_صف_عمود_Widget_Class, tool_Bar_چوکھٹا,
	XmNchildType, Xm_چوکھٹا_WORKAREA_بچہ,
	Xm_Nrow_عمود_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_اونچائی, 0,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nadjust_آخری, True,
	تہی);
    gui_motif_menu_colors(toolBar);

#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNbindingType, XmNONE); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nback_صفحہ_سائز, XmNONE); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nback_صفحہ_نمبر, 0); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nback_صفحہ_Placement, Xm_چوٹی_دایاں); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNmajorTabSpacing, 0); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNshadowThickness, 0); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سطر = XmCreateNotebook(vimForm, "Vim tabline", دلائل, n);

    Xt_جوڑو_Callback(tab_سطر, Xm_Npage_بدلا_Callback, (XtCallbackProc)tabline_cb,
			تہی);
    Xt_جوڑو_Event_Handler(tab_سطر, Button_Press_نقاب, False,
			(XtEventHandler)tabline_menu_cb, تہی);

    gui.tabline_اونچائی = TABLINE_اونچائی;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerNext");
    Xt_Va_سیٹ_اقدار(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, تہی);
    scroller = Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerPrevious");
    Xt_Va_سیٹ_اقدار(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, تہی);

    // Create the tabline popup menu
    tab_سطر_menu = XmCreatePopupMenu(tab_سطر, "tabline popup", تہی, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nuser_ڈیٹا, (Xt_اشارہ_گر)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_لیبل);
    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "Close", دلائل, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, تہی);
    XmStringFree(xms);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nuser_ڈیٹا, (Xt_اشارہ_گر)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_لیبل);
    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "New Tab", دلائل, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, تہی);
    XmStringFree(xms);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nuser_ڈیٹا, (Xt_اشارہ_گر)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_لیبل);
    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "Open tab...", دلائل, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, تہی);
    XmStringFree(xms);
#اگر_ختم

    متن_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nmargin_اونچائی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	تہی);
    gui_motif_scroll_colors(متن_Area_Form);

    متن_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, متن_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.عقب_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_چوڑائی, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	تہی);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(متن_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.اندر_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    خالی
gui_x11_destroy_widgets(خالی)
{
    متن_Area = تہی;
#اگر_تعریف_شدہ FEAT_MENU
    menuBar = تہی;
#اگر_ختم
}

    خالی
gui_mch_سیٹ_متن_area_pos(
    صحیح	    x غیر_مستعمل,
    صحیح	    y غیر_مستعمل,
    صحیح	    w غیر_مستعمل,
    صحیح	    h غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#اگر_ختم
}

    خالی
gui_x11_سیٹ_عقب_color(خالی)
{
    اگر (متن_Area != تہی)
#اگر (Xm_نسخہ >= 1002)
	XmChangeColor(متن_Area, gui.عقب_pixel);
#ورنہ
	Xt_Va_سیٹ_اقدار(متن_Area,
		  XmNbackground, gui.عقب_pixel,
		  تہی);
#اگر_ختم
}

/*
 * Manage dialog centered on pointer.
 */
    خالی
manage_centered(Widget dialog_بچہ)
{
    Widget shell = Xt_والد(dialog_بچہ);
    کھڑکی جڑ, بچہ;
    غیرعلامتی صحیح نقاب;
    غیرعلامتی صحیح چوڑائی, اونچائی, border_چوڑائی, گہرائی;
    صحیح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_اقدار(shell, XmNmappedWhenManaged, &mappedWhenManaged, تہی);
    Xt_Va_سیٹ_اقدار(shell, XmNmappedWhenManaged, False, تہی);

    Xt_Manage_بچہ(dialog_بچہ);

    // Get the pointer position (x, y)
    XQuery_اشارہ_گر(XtDisplay(shell), Xt_کھڑکی(shell), &جڑ, &بچہ,
		  &x, &y, &win_x, &win_y, &نقاب);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_کھڑکی(shell), &جڑ, &win_x, &win_y,
		 &چوڑائی, &اونچائی, &border_چوڑائی, &گہرائی);
    چوڑائی += 2 * border_چوڑائی;
    اونچائی += 2 * border_چوڑائی;
    x -= چوڑائی / 2;
    y -= اونچائی / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->چوڑائی - چوڑائی;
    maxY = XtScreen(shell)->اونچائی - اونچائی;
    اگر (x < 0)
	x = 0;
    اگر (x > maxX)
	x = maxX;
    اگر (y < 0)
	y = 0;
    اگر (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_سیٹ_اقدار(shell, XmNx, x, XmNy, y, تہی);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_سیٹ_اقدار(shell, XmNmappedWhenManaged, mappedWhenManaged, تہی);
}

#اگر defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# اگر (Xm_نسخہ <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_لیبل);
# ورنہ
    // Motif 1.2 method
    Xm_Font_List_اندراج font_list_اندراج;

    font_list_اندراج = Xm_Font_List_اندراج_Create(STRING_لیبل, XmFONT_IS_FONT,
					    (Xt_اشارہ_گر)font);
    font_list = Xm_Font_List_Append_اندراج(تہی, font_list_اندراج);
    Xm_Font_List_اندراج_Free(&font_list_اندراج);
# endif
    واپس font_list;
}

# اگر ((Xm_نسخہ > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_سیٹ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_اندراج font_list_اندراج;

    font_list_اندراج = Xm_Font_List_اندراج_Create(STRING_لیبل,
					    XmFONT_IS_FONTSET,
					    (Xt_اشارہ_گر)*fontset);
    font_list = Xm_Font_List_Append_اندراج(تہی, font_list_اندراج);
    Xm_Font_List_اندراج_Free(&font_list_اندراج);
    واپس font_list;
}
# endif

#اگر_ختم

#اگر defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساکن خالی gui_motif_جوڑو_actext(vimmenu_T *menu);
# اگر (Xm_نسخہ >= 1002)
ساکن خالی toggle_tearoff(Widget wid);
ساکن خالی gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساکن خالی submenu_change(vimmenu_T *mp, صحیح colors);

ساکن خالی do_سیٹ_mnemonics(صحیح enable);
ساکن صحیح menu_enabled = TRUE;

    خالی
gui_mch_enable_menu(صحیح flag)
{
    اگر (flag)
    {
	Xt_Manage_بچہ(menuBar);
# ifdef FEAT_TOOLBAR
	اگر (XtIsManaged(Xt_والد(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_سیٹ_اقدار(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		تہی);
#  ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
#  endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      تہی);
	}
	ورنہ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      تہی);
	}
    }
    ورنہ
    {
	Xt_Unmanage_بچہ(menuBar);
# ifdef FEAT_TOOLBAR
	اگر (XtIsManaged(Xt_والد(toolBar)))
	{
	    Xt_Va_سیٹ_اقدار(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		تہی);
#  ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
#  endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      تہی);
	}
	ورنہ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      تہی);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    خالی
gui_motif_سیٹ_mnemonics(صحیح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    اگر (!menu_enabled)
	enable = FALSE;
    do_سیٹ_mnemonics(enable);
}

    ساکن خالی
do_سیٹ_mnemonics(صحیح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	اگر (menu->id != (Widget)0)
	    Xt_Va_سیٹ_اقدار(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    تہی);
}

    خالی
gui_mch_جوڑو_menu(vimmenu_T *menu, صحیح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*والد = menu->والد;

# ifdef MOTIF_POPUP
    اگر (menu_is_popup(menu->name))
    {
	Arg arg[2];
	صحیح n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  اگر (Xm_نسخہ <= 1002)
	اگر (mouse_model_popup())
#  endif
	{
	    اگر (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_سیٹ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    اگر (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_سیٹ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(متن_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	واپس;
    }
# endif

    اگر (!menu_is_menubar(menu->name)
	    || (والد != تہی && والد->submenu_id == (Widget)0))
	واپس;

    label = XmStringCreate((حرف *)menu->dname, STRING_لیبل);
    اگر (label == تہی)
	واپس;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (والد == تہی) ? menuBar : والد->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# اگر (Xm_نسخہ >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_اشاریہ, idx + (والد != تہی
			   && tearoff_val == (صحیح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    تہی);
    XmStringFree(label);

    اگر (menu->id == (Widget)0)		// failed
	واپس;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    اگر (والد == تہی && menu->ترجیح >= 9999)
	Xt_Va_سیٹ_اقدار(menuBar,
		XmNmenuHelpWidget, menu->id,
		تہی);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_جوڑو_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	تہی);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_صف_عمود_Widget_Class, shell,
	Xm_Nrow_عمود_Type, XmMENU_PULLDOWN,
	تہی);
    gui_motif_menu_colors(menu->submenu_id);

    اگر (menu->submenu_id == (Widget)0)		// failed
	واپس;

# اگر (Xm_نسخہ >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_سیٹ_اقدار(menu->id,
	XmNsubMenuId, menu->submenu_id,
	تہی);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    اگر (والد == تہی)
	gui_mch_compute_menu_اونچائی(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساکن خالی
gui_motif_جوڑو_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    اگر (menu->actext == تہی || menu->id == (Widget)0)
	واپس;

    label = XmStringCreate((حرف *)menu->actext, STRING_لیبل);
    اگر (label == تہی)
	واپس;
    Xt_Va_سیٹ_اقدار(menu->id, Xm_Naccelerator_متن, label, تہی);
    XmStringFree(label);
}

    خالی
gui_mch_toggle_tearoffs(صحیح enable)
{
# اگر (Xm_نسخہ >= 1002)
    اگر (enable)
	tearoff_val = (صحیح)XmTEAR_OFF_ENABLED;
    ورنہ
	tearoff_val = (صحیح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(جڑ_menu);
# endif
}

# اگر (Xm_نسخہ >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ساکن خالی
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_سیٹ_اقدار(wid, XmNtearOffModel, tearoff_val, تہی);
    اگر (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ساکن خالی
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    جب (menu != تہی)
    {
	اگر (!menu_is_popup(menu->name))
	{
	    اگر (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->اگلا;
    }
}
# endif

    صحیح
gui_mch_متن_area_extra_اونچائی(خالی)
{
    Dimension	shadow_اونچائی;

    Xt_Va_Get_اقدار(متن_Area_Form, XmNshadowThickness, &shadow_اونچائی, تہی);
    واپس shadow_اونچائی;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    خالی
gui_mch_compute_menu_اونچائی(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساکن Dimension	اونچائی = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    اگر (id != (Widget)0)
	Xt_Va_Get_اقدار(id, XmNheight, &اونچائی, تہی);

    // Find any menu Widget, to be able to call XtManageChild()
    ورنہ
	FOR_ALL_MENUS(mp)
	    اگر (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		توڑو;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    اگر (id != (Widget)0)
	Xt_Manage_بچہ(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	اگر (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_اقدار(mp->id, XmNy, &y, تہی);
	    اگر (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_اقدار(menuBar,
	Xm_Nmargin_اونچائی, &margin,
	XmNshadowThickness, &shadow,
	تہی);

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
    gui.menu_اونچائی = maxy + اونچائی - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    اگر (gui.shell_created)
	Xt_Va_سیٹ_اقدار(menuBar, XmNheight, gui.menu_اونچائی, تہی);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ساکن حرف **get_toolbar_pixmap(vimmenu_T *menu, حرف **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ساکن صحیح
check_xpm(char_u *راستہ)
{
    Xpm_اوصاف attrs;
    صحیح		حالت_2;
    Pixmap	نقاب;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    حالت_2 = Xpm_Read_File_مقصد_Pixmap(gui.dpy,
	    جڑ_کھڑکی_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)راستہ, &map, &نقاب, &attrs);
    Xpm_Free_اوصاف(&attrs);

    اگر (حالت_2 == XpmSuccess)
	واپس OK;
    واپس FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ساکن حرف **
get_toolbar_pixmap(vimmenu_T *menu, حرف **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    حرف	**xpm = تہی;		// xpm array
    صحیح		res;

    *fname = تہی;
    buf[0] = NUL;			// start with NULL path

    اگر (menu->iconfile != تہی)
    {
	// Use the "icon="  argument.
	gui_ڈھونڈو_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	اگر (res == FAIL && gui_ڈھونڈو_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	اگر (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    واپس tb_blank_xpm;
	}
    }

    اگر (menu->icon_builtin || gui_ڈھونڈو_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	اگر (menu->iconidx >= 0 && menu->iconidx
	       < (صحیح)(حجم(built_اندر_pixmaps) / حجم(built_اندر_pixmaps[0])))
	    xpm = built_اندر_pixmaps[menu->iconidx];
	ورنہ
	    xpm = tb_blank_xpm;
    }

    واپس xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساکن صحیح
جوڑو_pixmap_دلائل(vimmenu_T *menu, Arg *دلائل, صحیح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    اگر (menu->xpm == تہی)
    {
	Xt_سیٹ_Arg(دلائل[n], XmNlabelType, XmSTRING); n++;
    }
    ورنہ
    {
	اگر (menu->xpm_fname != تہی)
	{
	    Xt_سیٹ_Arg(دلائل[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_سیٹ_Arg(دلائل[n], Xm_Npixmap_ڈیٹا, menu->xpm); n++;
	Xt_سیٹ_Arg(دلائل[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    واپس n;
}
# endif // FEAT_TOOLBAR

    خالی
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحیح idx)
{
    XmString	label;
    vimmenu_T	*والد = menu->والد;

# اگر (Xm_نسخہ <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    اگر (menu_is_بچہ_کا_popup(menu) && !mouse_model_popup())
	واپس;
# endif

# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(والد->name))
    {
	WidgetClass	type;
	XmString	xms = تہی;    // fallback label if pixmap not found
	صحیح		n;
	Arg		دلائل[18];

	n = 0;
	اگر (menu_is_separator(menu->name))
	{
	    حرف	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (حرف *)vim_strchr(menu->name, ':');
	    اگر (cp != تہی)
		wid = (Dimension)atoi(++cp);
	    ورنہ
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    Xt_سیٹ_Arg(دلائل[n], XmNwidth, wid); n++;
	    Xt_سیٹ_Arg(دلائل[n], Xm_Nmin_چوڑائی, wid); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNorientation, XmVERTICAL); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNseparatorType, Xm_SHADOW_ETCHED_اندر); n++;
	}
	ورنہ
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_سیٹ_Arg(دلائل[n], XmNhighlightThickness, 1); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNhighlightOnEnter, True); n++;
	    Xt_سیٹ_Arg(دلائل[n], Xm_Nmargin_چوڑائی, 0); n++;
	    Xt_سیٹ_Arg(دلائل[n], Xm_Nmargin_اونچائی, 0); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_لیبل);
	    Xt_سیٹ_Arg(دلائل[n], XmNlabelString, xms); n++;

	    n = جوڑو_pixmap_دلائل(menu, دلائل, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_سیٹ_Arg(دلائل[n], Xm_Nposition_اشاریہ, idx); n++;
	اگر (menu->id == تہی)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, دلائل, n);
	    اگر (menu->id != تہی && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_جوڑو_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	ورنہ
	    Xt_سیٹ_اقدار(menu->id, دلائل, n);
	اگر (xms != تہی)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_سیٹ_tip(menu);
#  endif

	menu->والد = والد;
	menu->submenu_id = تہی;
	// When adding first item to toolbar it might have to be enabled .
	اگر (!XtIsManaged(Xt_والد(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != تہی)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_اونچائی = gui_mch_compute_toolbar_اونچائی();
	واپس;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    اگر (والد->submenu_id == (Widget)0)
	واپس;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    اگر (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, والد->submenu_id,
# اگر (Xm_نسخہ >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_اشاریہ, idx + (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		تہی);
	gui_motif_menu_colors(menu->id);
	واپس;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_لیبل);
    اگر (label == تہی)
	واپس;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ڈالو_Button_Widget_Class, والد->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# اگر (Xm_نسخہ >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_اشاریہ, idx + (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	تہی);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    اگر (menu->id != (Widget)0)
    {
	Xt_جوڑو_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_اشارہ_گر)menu);
	// add accelerator text
	gui_motif_جوڑو_actext(menu);
    }
}

# اگر (Xm_نسخہ <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    خالی
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    صحیح		idx = 0;

    // When GUI hasn't started the menus have not been created.
    اگر (!gui.اندر_use)
	واپس;

    جب (menu)
    {
      اگر (menu->children != تہی)
      {
	  اگر (menu_is_popup(menu->name))
	  {
	      اگر (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_جوڑو_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      ورنہ
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      ورنہ اگر (menu_is_بچہ_کا_popup(menu))
      {
	  اگر (mouse_model_popup())
	      gui_mch_جوڑو_menu_شے(menu, idx);
	  ورنہ
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->اگلا;
      ++idx;
    }
}
# endif

    خالی
gui_mch_new_menu_colors(خالی)
{
    اگر (menuBar == (Widget)0)
	واپس;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(tool_Bar_چوکھٹا);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(جڑ_menu, TRUE);
}

    خالی
gui_mch_new_menu_font(خالی)
{
    اگر (menuBar == (Widget)0)
	واپس;
    submenu_change(جڑ_menu, FALSE);
    {
	Dimension   اونچائی;
	Position w, h;

	Xt_Va_Get_اقدار(menuBar, XmNheight, &اونچائی, تہی);
	gui.menu_اونچائی = اونچائی;

	Xt_Va_Get_اقدار(vimShell, XtNwidth, &w, XtNheight, &h, تہی);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_حالت_area_اونچائی()
# endif
		     );
    }
    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# اگر defined(FEAT_BEVAL_GUI)
    خالی
gui_mch_new_tooltip_font(خالی)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    اگر (toolBar == (Widget)0)
	واپس;

    menu = gui_ڈھونڈو_menu((char_u *)"ToolBar");
    اگر (menu != تہی)
	submenu_change(menu, FALSE);
#  endif
}

    خالی
gui_mch_new_tooltip_colors(خالی)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    اگر (toolBar == (Widget)0)
	واپس;

    toolbar = gui_ڈھونڈو_menu((char_u *)"ToolBar");
    اگر (toolbar != تہی)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ساکن خالی
submenu_change(
    vimmenu_T	*menu,
    صحیح		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    برائے (mp = menu; mp != تہی; mp = mp->اگلا)
    {
	اگر (mp->id != (Widget)0)
	{
	    اگر (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		اگر (mp->xpm != تہی)
		{
		    صحیح		n = 0;
		    Arg		دلائل[18];

		    n = جوڑو_pixmap_دلائل(mp, دلائل, n);
		    Xt_سیٹ_اقدار(mp->id, دلائل, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		اگر (mp->tip != تہی)
		{
		    Arg دلائل[2];

		    دلائل[0].name = XmNbackground;
		    دلائل[0].قدر = gui.tooltip_bg_pixel;
		    دلائل[1].name = XmNforeground;
		    دلائل[1].قدر = gui.tooltip_fg_pixel;
		    Xt_سیٹ_اقدار(mp->tip->balloonLabel, &دلائل[0], Xt_نمبر(دلائل));
		}
#  endif
# endif
	    }
	    ورنہ
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		اگر (mp->tip != تہی)
		{
		    Arg دلائل[1];

		    دلائل[0].name = XmNfontList;
		    دلائل[0].قدر = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_سیٹ_اقدار(mp->tip->balloonLabel, &دلائل[0], Xt_نمبر(دلائل));
		}
# endif
	    }
	}

	اگر (mp->children != تہی)
	{
# اگر (Xm_نسخہ >= 1002)
	    // Set the colors/font for the tear off widget
	    اگر (mp->submenu_id != (Widget)0)
	    {
		اگر (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		ورنہ
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
    خالی
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    اگر (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    اگر (menu->id == (Widget)0)
	واپس;

    Widget	    والد;

    والد = Xt_والد(menu->id);
# اگر defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    اگر (والد == toolBar && menu->tip != تہی)
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
	menu->tip = تہی;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    اگر (والد == menuBar)
	gui_mch_compute_menu_اونچائی((Widget)0);
# ifdef FEAT_TOOLBAR
    ورنہ اگر (والد == toolBar)
    {
	Cardinal    عدد_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_اقدار(toolBar, XmNnumChildren, &عدد_children, تہی);
	اگر (عدد_children == 0)
	    gui_mch_show_toolbar(FALSE);
	ورنہ
	    gui.toolbar_اونچائی = gui_mch_compute_toolbar_اونچائی();
    }
# endif
}

    خالی
gui_mch_show_popupmenu(vimmenu_T *menu غیر_مستعمل)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_آخری_mouse_event());
    Xt_Manage_بچہ(menu->submenu_id);
# endif
}

#اگر_ختم // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    خالی
gui_mch_def_colors(خالی)
{
    اگر (!gui.اندر_use)
	واپس;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#اگر_تعریف_شدہ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#اگر_ختم
}


/*
 * Scrollbar stuff.
 */

    خالی
gui_mch_سیٹ_scrollbar_thumb(
    scrollbar_T *sb,
    طویل	val,
    طویل	سائز,
    طویل	max)
{
    اگر (sb->id != (Widget)0)
	Xt_Va_سیٹ_اقدار(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_سائز, سائز,
		  XmNpageIncrement, (سائز > 2 ? سائز - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  تہی);
}

    خالی
gui_mch_سیٹ_scrollbar_pos(
    scrollbar_T *sb,
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    اگر (sb->id == (Widget)0)
	واپس;

    اگر (sb->type == SBAR_بایاں || sb->type == SBAR_دایاں)
    {
	اگر (y == 0)
	    h -= gui.border_انحراف;
	ورنہ
	    y -= gui.border_انحراف;
	Xt_Va_سیٹ_اقدار(sb->id,
		Xm_Ntop_انحراف, y,
		Xm_Nbottom_انحراف, -y - h,
		XmNwidth, w,
		تہی);
    }
    ورنہ
	Xt_Va_سیٹ_اقدار(sb->id,
		Xm_Ntop_انحراف, y,
		Xm_Nleft_انحراف, x,
		Xm_Nright_انحراف, gui.which_scrollbars[SBAR_دایاں]
						     ? gui.scrollbar_چوڑائی : 0,
		XmNheight, h,
		تہی);
    Xt_Manage_بچہ(sb->id);
}

    صحیح
gui_mch_get_scrollbar_xpadding(خالی)
{
    صحیح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_اقدار(متن_Area, XtNwidth, &tw, XtNx, &tx, تہی);
    Xt_Va_Get_اقدار(vimShell, XtNwidth, &ww, تہی);
    xpad = ww - tw - tx - gui.scrollbar_چوڑائی;
    واپس (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(خالی)
{
    صحیح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_اقدار(متن_Area, XtNheight, &th, XtNy, &ty, تہی);
    Xt_Va_Get_اقدار(vimShell, XtNheight, &wh, تہی);
    ypad = wh - th - ty - gui.scrollbar_اونچائی;
    واپس (ypad < 0) ? 0 : ypad;
}

    خالی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    Arg		دلائل[16];
    صحیح		n;

    اگر (sb->id == (Widget)0)
	واپس;

    n = 0;
    اگر (flag)
    {
	انتخاب (sb->type)
	{
	    حالت SBAR_بایاں:
		Xt_سیٹ_Arg(دلائل[n], Xm_Nleft_انحراف, gui.scrollbar_چوڑائی); n++;
		توڑو;

	    حالت SBAR_دایاں:
		Xt_سیٹ_Arg(دلائل[n], Xm_Nright_انحراف, gui.scrollbar_چوڑائی); n++;
		توڑو;

	    حالت SBAR_BOTTOM:
		Xt_سیٹ_Arg(دلائل[n], Xm_Nbottom_انحراف, gui.scrollbar_اونچائی);n++;
		توڑو;
	}
	Xt_سیٹ_اقدار(متن_Area, دلائل, n);
	Xt_Manage_بچہ(sb->id);
    }
    ورنہ
    {
	اگر (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    انتخاب (sb->type)
	    {
		حالت SBAR_بایاں:
		    Xt_سیٹ_Arg(دلائل[n], Xm_Nleft_انحراف, 0); n++;
		    توڑو;

		حالت SBAR_دایاں:
		    Xt_سیٹ_Arg(دلائل[n], Xm_Nright_انحراف, 0); n++;
		    توڑو;

		حالت SBAR_BOTTOM:
		    Xt_سیٹ_Arg(دلائل[n], Xm_Nbottom_انحراف, 0);n++;
		    توڑو;
	    }
	    Xt_سیٹ_اقدار(متن_Area, دلائل, n);
	}
	Xt_Unmanage_بچہ(sb->id);
    }
}

    خالی
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحیح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		دلائل[16];
    صحیح		n = 0;

    Xt_سیٹ_Arg(دلائل[n], XmNminimum, 0); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    انتخاب (sb->type)
    {
	حالت SBAR_بایاں:
	    Xt_سیٹ_Arg(دلائل[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNwidth, gui.scrollbar_چوڑائی); n++;
	    توڑو;

	حالت SBAR_دایاں:
	    Xt_سیٹ_Arg(دلائل[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNwidth, gui.scrollbar_چوڑائی); n++;
	    توڑو;

	حالت SBAR_BOTTOM:
	    Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNheight, gui.scrollbar_اونچائی); n++;
	    توڑو;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, متن_Area_Form, دلائل, n);
    اگر (sb->id == (Widget)0)
	واپس;

    gui_mch_سیٹ_scrollbar_colors(sb);
    Xt_جوڑو_Callback(sb->id, Xm_Nvalue_بدلا_Callback,
	    scroll_cb, (Xt_اشارہ_گر)sb->ident);
    Xt_جوڑو_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_اشارہ_گر)sb->ident);
    Xt_جوڑو_Event_Handler(sb->id, کلید_Press_نقاب, FALSE, gui_x11_کلید_کامیابی_cb,
	    (Xt_اشارہ_گر)0);
}

    خالی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    اگر (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    خالی
gui_mch_سیٹ_scrollbar_colors(scrollbar_T *sb)
{
    اگر (sb->id != (Widget)0)
    {
	اگر (gui.scroll_bg_pixel != INVALCOLOR)
	{
#اگر (Xm_نسخہ>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#اگر_ختم

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_سیٹ_اقدار(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    تہی);
	}

	اگر (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_سیٹ_اقدار(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    تہی);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    اگر (sb == &gui.bottom_sbar && متن_Area_Form != (Widget)0)
	gui_motif_scroll_colors(متن_Area_Form);
}

/*
 * Miscellaneous stuff:
 */

    کھڑکی
gui_x11_get_wid(خالی)
{
    واپس(Xt_کھڑکی(متن_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساکن خالی
do_mnemonic(Widget w, غیرعلامتی صحیح keycode)
{
    WidgetList	    children;
    صحیح		    عدد_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    user_ڈیٹا_2;
    غیرعلامتی حرف   صف_ستون_Type;

    اگر (Xt_Is_مرکب(w))
    {
	اگر (XtClass(w) == xm_صف_عمود_Widget_Class)
	{
	    Xt_Va_Get_اقدار(w, Xm_Nrow_عمود_Type, &صف_ستون_Type, تہی);
	    isMenu = (صف_ستون_Type != (غیرعلامتی حرف)XmWORK_AREA);
	}
	ورنہ
	    isMenu = False;
	اگر (!isMenu)
	{
	    Xt_Va_Get_اقدار(w, XmNchildren, &children, XmNnumChildren,
			  &عدد_Children, تہی);
	    برائے (i = 0; i < عدد_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    ورنہ
    {
	Xt_Va_Get_اقدار(w, XmNmnemonic, &mnemonic, تہی);
	اگر (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    اگر (XKeysym_مقصد_Keycode(XtDisplay(Xt_والد(w)),
				       XString_مقصد_Keysym(mneString)) == keycode)
	    {
		اگر (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_اقدار(w, Xm_Nuser_ڈیٹا, &user_ڈیٹا_2, تہی);
		    اگر (user_ڈیٹا_2 != تہی && XtIsWidget(user_ڈیٹا_2))
			XmProcessTraversal(user_ڈیٹا_2, XmTRAVERSE_CURRENT);
		}
		ورنہ
		{
		    XKeyPressedEvent کلید_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_میدان(کلید_Event);
		    کلید_Event.type = کلید_Press;
		    کلید_Event.serial = 1;
		    کلید_Event.send_event = True;
		    کلید_Event.display = XtDisplay(w);
		    کلید_Event.کھڑکی_2 = Xt_کھڑکی(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & کلید_Event,
								     تہی, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ساکن خالی
mnemonic_event(
	Widget	    w,
	Xt_اشارہ_گر   call_ڈیٹا غیر_مستعمل,
	XKeyEvent   *event,
	Boolean	    *b غیر_مستعمل)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ساکن خالی
جوڑو_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحیح		    عدد_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    غیرعلامتی حرف   صف_ستون_Type;

    اگر (Xt_Is_مرکب(w))
    {
	اگر (XtClass(w) == xm_صف_عمود_Widget_Class)
	{
	    Xt_Va_Get_اقدار(w, Xm_Nrow_عمود_Type, &صف_ستون_Type, تہی);
	    isMenu = (صف_ستون_Type != (غیرعلامتی حرف)XmWORK_AREA);
	}
	ورنہ
	    isMenu = False;
	اگر (!isMenu)
	{
	    Xt_Va_Get_اقدار(w, XmNchildren, &children, XmNnumChildren,
							  &عدد_Children, تہی);
	    برائے (i = 0; i < عدد_Children; i++)
		جوڑو_mnemonic_grabs(dialog, children[i]);
	}
    }
    ورنہ
    {
	Xt_Va_Get_اقدار(w, XmNmnemonic, &mnemonic, تہی);
	اگر (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_کلید(dialog, XKeysym_مقصد_Keycode(XtDisplay(dialog),
						  XString_مقصد_Keysym(mneString)),
		    Mod1_نقاب, True, Grab_منوال_Async, Grab_منوال_Async);
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
    ساکن خالی
activate_dialog_mnemonics(Widget dialog)
{
    اگر (!dialog)
	واپس;

    Xt_جوڑو_Event_Handler(dialog, کلید_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارہ_گر) تہی);
    جوڑو_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساکن خالی
suppress_dialog_mnemonics(Widget dialog)
{
    اگر (!dialog)
	واپس;

    Xt_Ungrab_کلید(dialog, Any_کلید, Mod1_نقاب);
    XtRemoveEventHandler(dialog, کلید_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارہ_گر) تہی);
}

#اگر defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساکن خالی
سیٹ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    اگر (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_سیٹ *)&gui.fontset);
	اگر (fl != تہی)
	{
	    اگر (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    ورنہ
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
	    XmFontListFree(fl);
	}
    }
# ورنہ
    اگر (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	اگر (fl != تہی)
	{
	    اگر (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    ورنہ
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
	    XmFontListFree(fl);
	}
    }
# endif
}
#اگر_ختم

#اگر defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

قسمتعریف ساخت dialog_callback_arg
{
    حرف *  دلائل;   // not used right now
    صحیح	    id;
} dcbarg_T;

ساکن Widget dialog_wgt;
ساکن حرف *browse_fname = تہی;
ساکن Xm_String_Char_سیٹ charset = (Xm_String_Char_سیٹ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساکن خالی DialogCancelCB(Widget, Xt_اشارہ_گر, Xt_اشارہ_گر);
ساکن خالی DialogAcceptCB(Widget, Xt_اشارہ_گر, Xt_اشارہ_گر);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ساکن خالی
سیٹ_predefined_label(Widget والد, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *اگلا;
    کلید_Sym	mnemonic = NUL;

    w = Xt_Name_مقصد_Widget(والد, name);

    اگر (!w)
	واپس;

    p = vim_strsave((char_u *)new_label);
    اگر (p == تہی)
	واپس;
    برائے (اگلا = p; *اگلا; ++اگلا)
    {
	اگر (*اگلا == DLG_HOTKEY_CHAR)
	{
	    صحیح len = STRLEN(اگلا);

	    اگر (len > 0)
	    {
		mch_memmove(اگلا, اگلا + 1, len);
		mnemonic = اگلا[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_لیبل);
    vim_free(p);

    اگر (str != تہی)
    {
	Xt_Va_سیٹ_اقدار(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		تہی);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساکن خالی
سیٹ_predefined_fontlist(Widget والد, String name)
{
    Widget w;
    w = Xt_Name_مقصد_Widget(والد, name);

    اگر (!w)
	واپس;

    سیٹ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحیح		saving غیر_مستعمل,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext غیر_مستعمل,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*نمونہ;
    char_u	*tofree = تہی;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", تہی, 0);

    اگر (initdir == تہی || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    اگر (dflt == تہی)
	dflt = (char_u *)"";
    ورنہ اگر (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	جوڑو_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    اگر (filter == تہی)
	نمونہ = (char_u *)"";
    ورنہ
    {
	char_u	*s, *p;

	s = filter;
	برائے (p = filter; *p != NUL; ++p)
	{
	    اگر (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    اگر (*p == ';' || *p == '\n')	// end of (first) pattern
		توڑو;
	}
	نمونہ = vim_strnsave(s, p - s);
	tofree = نمونہ;
	اگر (نمونہ == تہی)
	    نمونہ = (char_u *)"";
    }

    Xt_Va_سیٹ_اقدار(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (حرف *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (حرف *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (حرف *)نمونہ, STRLEN(نمونہ) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (حرف *)title, STRLEN(title) + 1,
	تہی);

    سیٹ_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    سیٹ_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    سیٹ_predefined_label(dialog_wgt, "Dir", _("Directories"));
    سیٹ_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    سیٹ_predefined_label(dialog_wgt, "Help", _("&Help"));
    سیٹ_predefined_label(dialog_wgt, "Items", _("Files"));
    سیٹ_predefined_label(dialog_wgt, "OK", _("&OK"));
    سیٹ_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    سیٹ_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    سیٹ_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    اگر (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, تہی);

    Xt_جوڑو_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_اشارہ_گر)0);
    Xt_جوڑو_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_اشارہ_گر)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_بچہ(Xm_File_Selection_Box_Get_بچہ(dialog_wgt,
					(غیرعلامتی حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    کرو
    {
	XtAppProcessEvent(Xt_Widget_مقصد_Application_Context(dialog_wgt),
	    (Xt_داخلہ_نقاب)XtIMAll);
    } جب (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    اگر (browse_fname == تہی)
	واپس تہی;
    واپس vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ساکن خالی
DialogCancelCB(
    Widget	w غیر_مستعمل,		//  widget id
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,	//  data from application
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)	//  data from widget class
{
    اگر (browse_fname != تہی)
    {
	XtFree(browse_fname);
	browse_fname = تہی;
    }
    Xt_Unmanage_بچہ(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساکن خالی
DialogAcceptCB(
    Widget	w غیر_مستعمل,		//  widget id
    Xt_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,	//  data from application
    Xt_اشارہ_گر	call_ڈیٹا)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    اگر (browse_fname != تہی)
    {
	XtFree(browse_fname);
	browse_fname = تہی;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_ڈیٹا;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->قدر, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_بچہ(dialog_wgt);
}

#اگر_ختم // FEAT_BROWSE

#اگر defined(FEAT_GUI_DIALOG)

ساکن صحیح	dialog_حالت;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساکن خالی
keyhit_callback(
    Widget		w,
    Xt_اشارہ_گر		client_ڈیٹا_2 غیر_مستعمل,
    XEvent		*event,
    Boolean		*cont غیر_مستعمل)
{
    حرف	buf[2];
    کلید_Sym	کلید_sym;

    اگر (XLookupString(&(event->xkey), buf, 2, &کلید_sym, تہی) == 1)
    {
	اگر (*buf == CAR)
	    dialog_حالت = 1;
	ورنہ اگر (*buf == ESC)
	    dialog_حالت = 2;
    }
    اگر ((کلید_sym == XK_بایاں || کلید_sym == XK_دایاں)
	    && !(event->xkey.state & سرکاؤ_نقاب))
	Xm_متن_میدان_Clear_Selection(w, Xt_آخری_Timestamp_Processed(gui.dpy));
}

    ساکن خالی
butproc(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    dialog_حالت = (صحیح)(طویل)client_ڈیٹا_2 + 1;
}

# ifdef HAVE_XPM

    ساکن Widget
create_pixmap_label(
    Widget	والد,
    String	name,
    حرف	**ڈیٹا,
    ArgList	دلائل,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحیح			گہرائی;
    Pixmap		pixmap = 0;
    Xpm_اوصاف	attr;
    Boolean		rs;
    Xpm_Color_علامت	color[5] =
    {
	{"none", تہی, 0},
	{"iconColor1", تہی, 0},
	{"bottomShadowColor", تہی, 0},
	{"topShadowColor", تہی, 0},
	{"selectColor", تہی, 0}
    };

    label = XmCreateLabelGadget(والد, name, دلائل, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_کا_Object(label);
    scr = Xt_Screen_کا_Object(label);
    Xt_Va_Get_اقدار(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_والد(label),
		  XmNdepth, &گہرائی,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  تہی);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_گہرائی;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.گہرائی = گہرائی;
    Xpm_Create_Pixmap_From_ڈیٹا(dsp, جڑ_کھڑکی_کا_Screen(scr),
		    ڈیٹا, &pixmap, تہی, &attr);

    Xt_Va_Get_اقدار(label, Xm_Nrecompute_سائز, &rs, تہی);
    Xt_Va_سیٹ_اقدار(label, Xm_Nrecompute_سائز, True, تہی);
    Xt_Va_سیٹ_اقدار(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    تہی);
    Xt_Va_سیٹ_اقدار(label, Xm_Nrecompute_سائز, rs, تہی);

    واپس label;
}
# endif

    صحیح
gui_mch_dialog(
    صحیح		type غیر_مستعمل,
    char_u	*title,
    char_u	*پیغام_2,
    char_u	*button_نام,
    صحیح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحیح		ex_cmd غیر_مستعمل)
{
    char_u		*buts;
    char_u		*p, *اگلا;
    XtAppContext	app;
    XmString		label;
    صحیح			butcount;
    Widget		w;
    Widget		dialogform = تہی;
    Widget		form = تہی;
    Widget		dialogtextfield = تہی;
    Widget		*buttons;
    Widget		sep_form = تہی;
    Boolean		vertical;
    Widget		separator = تہی;
    صحیح			n;
    Arg			دلائل[6];
# ifdef HAVE_XPM
    حرف		**icon_ڈیٹا_2 = تہی;
    Widget		dialogpixmap = تہی;
# endif

    اگر (title == تہی)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (حرف *)"dialog", تہی, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != تہی);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((حرف *)title);
    اگر (label == تہی)
	واپس -1;
    Xt_Va_سیٹ_اقدار(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    تہی);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_نام);
    اگر (buts == تہی)
	واپس -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    برائے (p = buts; *p; ++p)
	اگر (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    اگر (buttons == تہی)
    {
	vim_free(buts);
	واپس -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    برائے (butcount = 0; *p; ++butcount)
    {
	کلید_Sym mnemonic = NUL;

	برائے (اگلا = p; *اگلا; ++اگلا)
	{
	    اگر (*اگلا == DLG_HOTKEY_CHAR)
	    {
		صحیح len = STRLEN(اگلا);

		اگر (len > 0)
		{
		    mch_memmove(اگلا, اگلا + 1, len);
		    mnemonic = اگلا[0];
		}
	    }
	    اگر (*اگلا == DLG_BUTTON_SEP)
	    {
		*اگلا++ = NUL;
		توڑو;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_لیبل);
	اگر (label == تہی)
	    توڑو;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ڈالو_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_انحراف, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		تہی);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	اگر (butcount > 0)
	{
	    اگر (vertical)
		Xt_Va_سیٹ_اقدار(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			تہی);
	    ورنہ
	    {
		اگر (*اگلا == NUL)
		{
		    Xt_Va_سیٹ_اقدار(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_انحراف, 4,
			    تہی);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_انحراف, 4,
			    تہی);
		    Xt_Manage_بچہ(sep_form);
		}
		ورنہ
		{
		    Xt_Va_سیٹ_اقدار(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    تہی);
		}
	    }
	}
	ورنہ اگر (!vertical)
	{
	    اگر (*اگلا == NUL)
	    {
		Xt_Va_سیٹ_اقدار(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_انحراف, 4,
			تہی);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_انحراف, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_انحراف, 4,
			تہی);
		Xt_Manage_بچہ(sep_form);
	    }
	    ورنہ
		Xt_Va_سیٹ_اقدار(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_انحراف, 4,
			تہی);
	}

	Xt_جوڑو_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_اشارہ_گر)(طویل)butcount);
	p = اگلا;
    }
    vim_free(buts);

    separator = (Widget) 0;
    اگر (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_سیٹ_Arg(دلائل[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_سیٹ_Arg(دلائل[n], XmNbottomWidget, buttons[0]); n++;
	Xt_سیٹ_Arg(دلائل[n], Xm_Nbottom_انحراف, 4); n++;
	Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", دلائل, n);
	Xt_Manage_بچہ(separator);
    }

    اگر (textfield != تہی)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_متن_میدان_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		تہی);
	اگر (butcount > 0)
	    Xt_Va_سیٹ_اقدار(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    تہی);
	ورنہ
	    Xt_Va_سیٹ_اقدار(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    تہی);

	سیٹ_fontlist(dialogtextfield);
	Xm_متن_میدان_سیٹ_String(dialogtextfield, (حرف *)textfield);
	Xt_Manage_بچہ(dialogtextfield);
	Xt_جوڑو_Event_Handler(dialogtextfield, کلید_Press_نقاب, False,
			    (XtEventHandler)keyhit_callback, (Xt_اشارہ_گر)تہی);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    تہی);
    Xt_Manage_بچہ(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    انتخاب (type)
    {
	حالت VIM_GENERIC:
	    icon_ڈیٹا_2 = generic_xpm;
	    توڑو;
	حالت VIM_ERROR:
	    icon_ڈیٹا_2 = error_xpm;
	    توڑو;
	حالت VIM_WARNING:
	    icon_ڈیٹا_2 = alert_xpm;
	    توڑو;
	حالت VIM_INFO:
	    icon_ڈیٹا_2 = info_xpm;
	    توڑو;
	حالت VIM_QUESTION:
	    icon_ڈیٹا_2 = quest_xpm;
	    توڑو;
	طےشدہ:
	    icon_ڈیٹا_2 = generic_xpm;
    }

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Ntop_انحراف, 8); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nbottom_انحراف, 8); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nleft_انحراف, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_ڈیٹا_2, دلائل, n);
    Xt_Manage_بچہ(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)پیغام_2, STRING_لیبل);
    اگر (label == تہی)
    {
	vim_free(buttons);
	واپس -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				Xm_Ntop_انحراف, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# ورنہ
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_انحراف, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_انحراف, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_انحراف, 8,
				تہی);
    XmStringFree(label);
    سیٹ_fontlist(w);

    اگر (textfield != تہی)
    {
	Xt_Va_سیٹ_اقدار(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		تہی);
    }
    ورنہ
    {
	اگر (butcount > 0)
	    Xt_Va_سیٹ_اقدار(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    تہی);
	ورنہ
	    Xt_Va_سیٹ_اقدار(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    تہی);
    }

    اگر (dfltbutton < 1)
	dfltbutton = 1;
    اگر (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_سیٹ_اقدار(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], تہی);
    اگر (textfield != تہی)
	Xt_Va_سیٹ_اقدار(dialogform, XmNinitialFocus, dialogtextfield, تہی);
    ورنہ
	Xt_Va_سیٹ_اقدار(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									تہی);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    اگر (textfield != تہی && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_متن_میدان_سیٹ_Selection(dialogtextfield,
			 (Xm_متن_Position)0, (Xm_متن_Position)STRLEN(textfield),
					   Xt_آخری_Timestamp_Processed(gui.dpy));
	Xm_متن_میدان_سیٹ_Cursor_Position(dialogtextfield,
					   (Xm_متن_Position)STRLEN(textfield));
    }

    app = Xt_Widget_مقصد_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_حالت = -1;
    برائے (;;)
    {
	XtAppProcessEvent(app, (Xt_داخلہ_نقاب)XtIMAll);
	اگر (dialog_حالت >= 0 || !XtIsManaged(dialogform))
	    توڑو;
    }

    vim_free(buttons);

    اگر (textfield != تہی)
    {
	p = (char_u *)Xm_متن_Get_String(dialogtextfield);
	اگر (p == تہی || dialog_حالت < 0)
	    *textfield = NUL;
	ورنہ
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((حرف *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    واپس dialog_حالت;
}
#اگر_ختم // FEAT_GUI_DIALOG

#اگر defined(FEAT_TOOLBAR)
    خالی
gui_mch_show_toolbar(صحیح showit)
{
    Cardinal	عدد_Children;	    // how many children toolBar has

    اگر (toolBar == (Widget)0)
	واپس;
    Xt_Va_Get_اقدار(toolBar, XmNnumChildren, &عدد_Children, تہی);
    اگر (showit && عدد_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_اقدار(toolBar, XmNchildren, &children, تہی);
	{
	    خالی    (*action)(BalloonEval *);
	    صحیح	    متن = 0;

	    اگر (لڑی_تلاش((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    ورنہ
		action = &gui_mch_disable_beval_area;
	    اگر (لڑی_تلاش((ثابت حرف *)p_toolbar, "text"))
		متن = 1;
	    ورنہ اگر (لڑی_تلاش((ثابت حرف *)p_toolbar, "icons"))
		متن = -1;
	    اگر (متن != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    اگر (menu_is_toolbar(toolbar->dname))
			توڑو;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		اگر (toolbar)
		{
		    برائے (cur = toolbar->children; cur; cur = cur->اگلا)
		    {
			Arg	    دلائل[1];
			صحیح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			اگر (cur->tip != تہی)
			    (*action)(cur->tip);
			اگر (!menu_is_separator(cur->name))
			{
			    اگر (متن == 1 || cur->xpm == تہی)
			    {
				Xt_سیٹ_Arg(دلائل[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    اگر (cur->id != تہی)
			    {
				Xt_Unmanage_بچہ(cur->id);
				Xt_سیٹ_اقدار(cur->id, دلائل, n);
				Xt_Manage_بچہ(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_اونچائی = gui_mch_compute_toolbar_اونچائی();
	Xt_Manage_بچہ(Xt_والد(toolBar));
# ifdef FEAT_GUI_TABLINE
	اگر (showing_tabline)
	{
	    Xt_Va_سیٹ_اقدار(tab_سطر,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  تہی);
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  تہی);
	}
	ورنہ
# endif
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  تہی);
	اگر (XtIsManaged(menuBar))
	    Xt_Va_سیٹ_اقدار(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    تہی);
	ورنہ
	    Xt_Va_سیٹ_اقدار(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    تہی);
    }
    ورنہ
    {
	gui.toolbar_اونچائی = 0;
	اگر (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      تہی);
	}
	ورنہ
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      تہی);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      تہی);
	    }
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      تہی);
	}

	Xt_Unmanage_بچہ(Xt_والد(toolBar));
    }
    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    ساکن خالی
reset_focus(خالی)
{
    اگر (متن_Area != تہی)
	XmProcessTraversal(متن_Area, XmTRAVERSE_CURRENT);
}

    صحیح
gui_mch_compute_toolbar_اونچائی(خالی)
{
    Dimension	borders;
    Dimension	اونچائی;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	عدد_Children;	    // how many children toolBar has
    صحیح		i;

    borders = 0;
    اونچائی = 0;
    اگر (toolBar != (Widget)0 && tool_Bar_چوکھٹا != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_اقدار(tool_Bar_چوکھٹا,
		XmNshadowThickness, &fst,
		Xm_Nmargin_اونچائی, &fmh,
		تہی);
	borders += fst + fmh;
	Xt_Va_Get_اقدار(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_اونچائی, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &عدد_Children, تہی);
	borders += tst + tmh;
	برائے (i = 0; i < (صحیح)عدد_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_اقدار(children[i], XmNheight, &whgt, تہی);
	    اگر (اونچائی < whgt)
		اونچائی = whgt;
	}
    }
# ifdef LESSTIF_نسخہ
    // Hack: When starting up we get wrong dimensions.
    اگر (اونچائی < 10)
	اونچائی = 24;
# endif

    واپس (صحیح)(اونچائی + (borders << 1));
}

    خالی
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_اقدار(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    تہی);
}
#اگر_ختم

#اگر defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    خالی
gui_mch_show_tabline(صحیح showit)
{
    اگر (tab_سطر == (Widget)0)
	واپس;

    اگر (!showit != !showing_tabline)
    {
	اگر (showit)
	{
	    Xt_Manage_بچہ(tab_سطر);
	    Xt_Unmanage_بچہ(Xt_Name_مقصد_Widget(tab_سطر, "PageScroller"));
	    Xt_Unmanage_بچہ(Xt_Name_مقصد_Widget(tab_سطر, "MinorTabScrollerNext"));
	    Xt_Unmanage_بچہ(Xt_Name_مقصد_Widget(tab_سطر,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    اگر (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), تہی);
	    ورنہ
#  endif
		اگر (XtIsManaged(menuBar))
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, تہی);
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM, تہی);
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  تہی);
	}
	ورنہ
	{
	    Xt_Unmanage_بچہ(tab_سطر);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    اگر (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), تہی);
	    ورنہ
#  endif
		اگر (XtIsManaged(menuBar))
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, تہی);
	    ورنہ
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM, تہی);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(خالی)
{
    واپس tab_سطر != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    خالی
gui_mch_update_tabline(خالی)
{
    tabpage_T		*tp;
    صحیح			nr = 1, n;
    Arg			دلائل[10];
    صحیح			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_صفحہ_Info	صفحہ_info;
    Xm_Notebook_صفحہ_حالت صفحہ_حالت;
    صحیح			آخری_صفحہ, tab_گنتی;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    اگر (tab_سطر == (Widget)0)
	واپس;

    // Add a label for each tab page.  They all contain the same text area.
    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	صفحہ_حالت = Xm_Notebook_Get_صفحہ_Info(tab_سطر, nr, &صفحہ_info);
	اگر (صفحہ_حالت == Xm_صفحہ_INVALID
		|| صفحہ_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_سیٹ_Arg(دلائل[n], Xm_Nnotebook_بچہ_Type, XmMAJOR_TAB); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNtraversalOn, False); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNhighlightThickness, 1); n++;
	    Xt_سیٹ_Arg(دلائل[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ڈالو_Button(tab_سطر, "-Empty-", دلائل, n);
	    Xt_Manage_بچہ(tab);
	    beval = gui_mch_create_beval_area(tab, تہی, tabline_balloon_cb,
									تہی);
	    Xt_Va_سیٹ_اقدار(tab, Xm_Nuser_ڈیٹا, beval, تہی);
	}
	ورنہ
	    tab = صفحہ_info.major_tab_widget;

	Xt_Va_سیٹ_اقدار(tab, Xm_Npage_نمبر, nr, تہی);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_اقدار(tab, XmNlabelString, &label_str, تہی);
	اگر (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    اگر (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_سیٹ_اقدار(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, تہی);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_بچہ(tab);
		Xt_Manage_بچہ(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_گنتی = nr - 1;

    Xt_Va_Get_اقدار(tab_سطر, Xm_Nlast_صفحہ_نمبر, &آخری_صفحہ, تہی);

    // Remove any old labels.
    جب (nr <= آخری_صفحہ)
    {
	اگر (Xm_Notebook_Get_صفحہ_Info(tab_سطر, nr, &صفحہ_info) != Xm_صفحہ_INVALID
	    && صفحہ_info.صفحہ_نمبر == nr
	    && صفحہ_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_اقدار(صفحہ_info.major_tab_widget, Xm_Nuser_ڈیٹا, &beval, تہی);
	    اگر (beval != تہی)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_بچہ(صفحہ_info.major_tab_widget);
	    XtDestroyWidget(صفحہ_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Nlast_صفحہ_نمبر, tab_گنتی, تہی);

    Xt_Va_Get_اقدار(tab_سطر, Xm_Ncurrent_صفحہ_نمبر, &currentpage, تہی);
    اگر (currentpage != curtabidx)
	Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Ncurrent_صفحہ_نمبر, curtabidx, تہی);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    خالی
gui_mch_سیٹ_curtab(صحیح nr)
{
    صحیح		currentpage;

    اگر (tab_سطر == (Widget)0)
	واپس;

    Xt_Va_Get_اقدار(tab_سطر, Xm_Ncurrent_صفحہ_نمبر, &currentpage, تہی);
    اگر (currentpage != nr)
	Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Ncurrent_صفحہ_نمبر, nr, تہی);
}
#اگر_ختم

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ساکن خالی
gui_motif_menu_colors(Widget id)
{
    اگر (gui.menu_bg_pixel != INVALCOLOR)
#اگر (Xm_نسخہ >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#ورنہ
	Xt_Va_سیٹ_اقدار(id, XmNbackground, gui.menu_bg_pixel, تہی);
#اگر_ختم
    اگر (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(id, XmNforeground, gui.menu_fg_pixel, تہی);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ساکن خالی
gui_motif_scroll_colors(Widget id)
{
    اگر (gui.scroll_bg_pixel != INVALCOLOR)
#اگر (Xm_نسخہ >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#ورنہ
	Xt_Va_سیٹ_اقدار(id, XmNbackground, gui.scroll_bg_pixel, تہی);
#اگر_ختم
    اگر (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(id, XmNforeground, gui.scroll_fg_pixel, تہی);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    خالی
gui_motif_menu_fontlist(Widget id غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_MENU
# ifdef FONTSET_ALWAYS
    اگر (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_سیٹ *)&gui.menu_fontset);
	اگر (fl != تہی)
	{
	    اگر (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    ورنہ
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
	    XmFontListFree(fl);
	}
    }
# ورنہ
    اگر (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	اگر (fl != تہی)
	{
	    اگر (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    ورنہ
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, تہی);
	    XmFontListFree(fl);
	}
    }
# endif
#اگر_ختم
}


/*
 * We don't create it twice for the sake of speed.
 */

قسمتعریف ساخت Shared_ڈھونڈو_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget اوپر;		// search direction 'Up' radio button
    Widget نیچے_کو;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget ڈھونڈو;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_ڈھونڈو_Replace_2;

ساکن Shared_ڈھونڈو_Replace_2 ڈھونڈو_widgets = {تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی};
ساکن Shared_ڈھونڈو_Replace_2 repl_widgets = {تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی, تہی};

    ساکن خالی
ڈھونڈو_replace_destroy_callback(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    Shared_ڈھونڈو_Replace_2 *cd = (Shared_ڈھونڈو_Replace_2 *)client_ڈیٹا_2;

    اگر (cd != تہی)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساکن خالی
ڈھونڈو_replace_dismiss_callback(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    Shared_ڈھونڈو_Replace_2 *cd = (Shared_ڈھونڈو_Replace_2 *)client_ڈیٹا_2;

    اگر (cd != تہی)
	Xt_Unmanage_بچہ(cd->dialog);
}

    ساکن خالی
اندراج_activate_callback(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    XmProcessTraversal((Widget)client_ڈیٹا_2, XmTRAVERSE_CURRENT);
}

    ساکن خالی
ڈھونڈو_replace_callback(
    Widget	w غیر_مستعمل,
    Xt_اشارہ_گر	client_ڈیٹا_2,
    Xt_اشارہ_گر	call_ڈیٹا غیر_مستعمل)
{
    long_u	flags = (long_u)client_ڈیٹا_2;
    حرف	*ڈھونڈو_متن, *repl_متن;
    Boolean	direction_نیچے_کو = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_ڈھونڈو_Replace_2 *sfr;

    اگر (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = خالی_خانے_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	واپس;
    }

    // Get the search/replace strings from the dialog
    اگر (flags == FRD_FINDNEXT)
    {
	repl_متن = تہی;
	sfr = &ڈھونڈو_widgets;
    }
    ورنہ
    {
	repl_متن = Xm_متن_میدان_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    ڈھونڈو_متن = Xm_متن_میدان_Get_String(sfr->what);
    Xt_Va_Get_اقدار(sfr->نیچے_کو, XmNset, &direction_نیچے_کو, تہی);
    Xt_Va_Get_اقدار(sfr->wword, XmNset, &wword, تہی);
    Xt_Va_Get_اقدار(sfr->mcase, XmNset, &mcase, تہی);
    اگر (wword)
	flags |= FRD_پورا_لفظ;
    اگر (mcase)
	flags |= FRD_مطابقت_CASE;

    (خالی)gui_do_findrepl((صحیح)flags, (char_u *)ڈھونڈو_متن, (char_u *)repl_متن,
							      direction_نیچے_کو);

    اگر (ڈھونڈو_متن != تہی)
	XtFree(ڈھونڈو_متن);
    اگر (repl_متن != تہی)
	XtFree(repl_متن);
}

    ساکن خالی
ڈھونڈو_replace_keypress(
    Widget		w غیر_مستعمل,
    Shared_ڈھونڈو_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b غیر_مستعمل)
{
    کلید_Sym keysym;

    اگر (frdp == تہی)
	واپس;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    اگر (keysym == XK_Escape)
	Xt_Unmanage_بچہ(frdp->dialog);
}

    ساکن خالی
سیٹ_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *اگلا;
    کلید_Sym	mnemonic = NUL;

    اگر (!w)
	واپس;

    p = vim_strsave((char_u *)label);
    اگر (p == تہی)
	واپس;
    برائے (اگلا = p; *اگلا; ++اگلا)
    {
	اگر (*اگلا == DLG_HOTKEY_CHAR)
	{
	    صحیح len = STRLEN(اگلا);

	    اگر (len > 0)
	    {
		mch_memmove(اگلا, اگلا + 1, len);
		mnemonic = اگلا[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    اگر (str)
    {
	Xt_Va_سیٹ_اقدار(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		تہی);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساکن خالی
ڈھونڈو_replace_dialog_create(char_u *arg, صحیح do_replace)
{
    Shared_ڈھونڈو_Replace_2	*frdp;
    Widget		separator;
    Widget		داخلہ_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		چوکھٹا;
    XmString		str;
    صحیح			n;
    Arg			دلائل[6];
    صحیح			wword = FALSE;
    صحیح			mcase = !p_ic;
    Dimension		چوڑائی;
    Dimension		widest;
    char_u		*اندراج_متن;

    frdp = do_replace ? &repl_widgets : &ڈھونڈو_widgets;

    // Get the search string to use.
    اندراج_متن = get_ڈھونڈو_dialog_متن(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    اگر (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	اگر (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_کھڑکی(Xt_والد(frdp->dialog)));
	ورنہ
	    Xt_Manage_بچہ(frdp->dialog);
	XtPopup(Xt_والد(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	اگر (اندراج_متن != تہی)
	    Xm_متن_میدان_سیٹ_String(frdp->what, (حرف *)اندراج_متن);
	vim_free(اندراج_متن);

	Xt_Va_سیٹ_اقدار(frdp->wword, XmNset, wword, تہی);
	واپس;
    }

    // Create a fresh new dialog window
    اگر (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    ورنہ
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNautoUnmanage, False); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNnoResize, True); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", دلائل, n);
    XmStringFree(str);
    Xt_جوڑو_Callback(frdp->dialog, XmNdestroyCallback,
	    ڈھونڈو_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_انحراف, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_انحراف, 4,
	    تہی);

    frdp->ڈھونڈو = XtVaCreateManagedWidget("findButton",
	    xm_ڈالو_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    تہی);
    سیٹ_label(frdp->ڈھونڈو, _("Find &Next"));

    Xt_جوڑو_Callback(frdp->ڈھونڈو, XmNactivateCallback,
	    ڈھونڈو_replace_callback,
	    (do_replace ? (Xt_اشارہ_گر)FRD_R_FINDNEXT : (Xt_اشارہ_گر)FRD_FINDNEXT));

    اگر (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->ڈھونڈو,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		تہی);
	سیٹ_label(frdp->replace, _("&Replace"));
	Xt_جوڑو_Callback(frdp->replace, XmNactivateCallback,
		ڈھونڈو_replace_callback, (Xt_اشارہ_گر)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		تہی);
	سیٹ_label(frdp->all, _("Replace &All"));
	Xt_جوڑو_Callback(frdp->all, XmNactivateCallback,
		ڈھونڈو_replace_callback, (Xt_اشارہ_گر)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		تہی);
	سیٹ_label(frdp->undo, _("&Undo"));
	Xt_جوڑو_Callback(frdp->undo, XmNactivateCallback,
		ڈھونڈو_replace_callback, (Xt_اشارہ_گر)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ڈالو_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    تہی);
    سیٹ_label(frdp->cancel, _("&Cancel"));
    Xt_جوڑو_Callback(frdp->cancel, XmNactivateCallback,
	    ڈھونڈو_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_بچہ(button_form);

    n = 0;
    Xt_سیٹ_Arg(دلائل[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNrightWidget, button_form); n++;
    Xt_سیٹ_Arg(دلائل[n], Xm_Nright_انحراف, 4); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلائل[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", دلائل, n);
    Xt_Manage_بچہ(separator);

    داخلہ_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_انحراف, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 4,
	    تہی);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, داخلہ_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_انحراف, 4,
		تہی);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_متن_میدان_Widget_Class, داخلہ_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		تہی);

	اگر (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_متن_میدان_Widget_Class,	داخلہ_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_انحراف, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    تہی);

	    Xt_جوڑو_Callback(frdp->with, XmNactivateCallback,
		    ڈھونڈو_replace_callback, (Xt_اشارہ_گر) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, داخلہ_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_انحراف, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    تہی);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_جوڑو_Callback(frdp->what, XmNactivateCallback,
		    اندراج_activate_callback, frdp->with);
	    Xt_جوڑو_Event_Handler(frdp->with, کلید_Press_نقاب, False,
			    (XtEventHandler)ڈھونڈو_replace_keypress,
			    (Xt_اشارہ_گر) frdp);

	}
	ورنہ
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_جوڑو_Callback(frdp->what, XmNactivateCallback,
		    ڈھونڈو_replace_callback, (Xt_اشارہ_گر)FRD_FINDNEXT);
	}
	Xt_جوڑو_Event_Handler(frdp->what, کلید_Press_نقاب, False,
			    (XtEventHandler)ڈھونڈو_replace_keypress,
			    (Xt_اشارہ_گر)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_سیٹ_Arg(دلائل[n], XmNwidth, &چوڑائی); n++;
	Xt_Get_اقدار(label_what, دلائل, n);
	widest = چوڑائی;
	اگر (do_replace)
	{
	    Xt_Get_اقدار(label_with, دلائل, n);
	    اگر (چوڑائی > widest)
		widest = چوڑائی;
	}

	Xt_Va_سیٹ_اقدار(frdp->what, Xm_Nleft_انحراف, widest, تہی);
	اگر (do_replace)
	    Xt_Va_سیٹ_اقدار(frdp->with, Xm_Nleft_انحراف, widest, تہی);

    }

    Xt_Manage_بچہ(داخلہ_form);

    {
	Widget radio_box;
	Widget w;

	چوکھٹا = XtVaCreateWidget("directionFrame",
		xm_چوکھٹا_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, داخلہ_form,
		Xm_Ntop_انحراف, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_انحراف, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, داخلہ_form,
		تہی);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, چوکھٹا,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
		تہی);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(چوکھٹا, "radioBox",
		(ArgList)تہی, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->اوپر = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		تہی);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->اوپر);

	str = XmStringCreateSimple(_("Down"));
	frdp->نیچے_کو = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		تہی);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->نیچے_کو);

	Xt_Manage_بچہ(radio_box);
	Xt_Manage_بچہ(چوکھٹا);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, چوکھٹا,
	    Xm_Nright_انحراف, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, داخلہ_form,
	    Xm_Ntop_انحراف, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_انحراف, 4,
	    تہی);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_انحراف, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNset, wword,
	    تہی);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_انحراف, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_انحراف, 4,
	    XmNset, mcase,
	    تہی);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_بچہ(toggle_form);

    اگر (اندراج_متن != تہی)
	Xm_متن_میدان_سیٹ_String(frdp->what, (حرف *)اندراج_متن);
    vim_free(اندراج_متن);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   خالی
gui_mch_ڈھونڈو_dialog(exarg_T *eap)
{
    اگر (!gui.اندر_use)
	واپس;

    ڈھونڈو_replace_dialog_create(eap->arg, FALSE);
}


    خالی
gui_mch_replace_dialog(exarg_T *eap)
{
    اگر (!gui.اندر_use)
	واپس;

    ڈھونڈو_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    خالی
gui_motif_synch_fonts(خالی)
{
    Shared_ڈھونڈو_Replace_2 *frdp;
    صحیح		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    اگر (font == تہی)
	واپس;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    برائے (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&ڈھونڈو_widgets);
	اگر (frdp->dialog)
	{
	    Xt_Va_سیٹ_اقدار(frdp->what, XmNfontList, font_list, تہی);
	    اگر (do_replace)
		Xt_Va_سیٹ_اقدار(frdp->with, XmNfontList, font_list, تہی);
	}
    }

    XmFontListFree(font_list);
}
