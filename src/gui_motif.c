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
#د_که_پای
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/XmP.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#که_تعریف_شوی HAVE_X11_XPM_H
# إذا defined(VMS)
#  include <xpm.h>
# وإلا
#  include <X11/xpm.h>
# endif
#وإلا
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#د_که_پای
#که_تعریف_شوی HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#د_که_پای

#ضمن "gui_xmebw.h"	// for our Enhanced Button Widget

#إذا defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#د_که_پای

#تعریف_کړه MOTIF_POPUP

خارجي Widget vimShell;

ساكن Widget vimForm;
ساكن Widget متن_Area_Form;
Widget متن_Area;
#که_تعریف_شوی FEAT_TOOLBAR
ساكن Widget tool_Bar_چوکاټ;
ساكن Widget toolBar;
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
ساكن Widget	tab_کرښه;
ساكن Widget	tab_کرښه_menu = 0;
ساكن صحيح	showing_tabline = 0;
#د_که_پای
#که_تعریف_شوی FEAT_MENU
# إذا (Xm_نسخه >= 1002)
// remember the last set value for the tearoff item
ساكن صحيح tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
# endif
ساكن Widget menuBar;
#د_که_پای

#که_تعریف_شوی FEAT_TOOLBAR
ساكن فراغ reset_focus(فراغ);
#د_که_پای

ساكن فراغ gui_motif_menu_colors(Widget id);
ساكن فراغ gui_motif_scroll_colors(Widget id);

#إذا (Xm_نسخه >= 1002)
# define STRING_نښان  Xm_FONTLIST_DEFAULT_نښان
#وإلا
# define STRING_نښان  XmSTRING_DEFAULT_CHARSET
#د_که_پای

/*
 * Call-back routines.
 */

    ساكن فراغ
scroll_cb(Widget w ناکارول_شوی, Xt_اشاره_ګر client_معلومات_2, Xt_اشاره_ګر call_معلومات)
{
    scrollbar_T *sb;
    اوږد	ارزښت;
    صحيح		dragging;

    sb = gui_پیدا_کړه_scrollbar((اوږد)client_معلومات_2);

    ارزښت = ((XmScrollBarCallbackStruct *)call_معلومات)->ارزښت;
    dragging = (((XmScrollBarCallbackStruct *)call_معلومات)->reason ==
							      (صحيح)XmCR_DRAG);
    gui_drag_scrollbar(sb, ارزښت, dragging);
}

#که_تعریف_شوی FEAT_GUI_TABLINE
    ساكن فراغ
tabline_cb(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2 ناکارول_شوی,
    Xt_اشاره_ګر	call_معلومات)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_معلومات;
    إذا (nptr->reason != (صحيح)XmCR_NONE)
	send_tabline_event(nptr->پاڼه_شمېره);
}

    ساكن فراغ
tabline_button_cb(
    Widget	w,
    Xt_اشاره_ګر	client_معلومات_2 ناکارول_شوی,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    Xt_اشاره_ګر	cmd, tab_idx;

    Xt_Va_Get_ارزښتونه(w, Xm_Nuser_معلومات, &cmd, عدم);
    Xt_Va_Get_ارزښتونه(tab_کرښه_menu, Xm_Nuser_معلومات, &tab_idx, عدم);

    send_tabline_menu_event((صحيح)(اوږد)tab_idx, (صحيح)(اوږد)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساكن فراغ
motif_tabline_timer_cb (
    Xt_اشاره_ګر		timed_بهر,
    XtIntervalId	*interval_id ناکارول_شوی)
{
    *((صحيح *)timed_بهر) = TRUE;
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
    Dimension	پلنوالی, لوړوالی;

    tab_scroll_w = Xt_Name_منزل_Widget(tab_کرښه, scroller_name);
    إذا (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_ارزښتونه(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &پلنوالی, XmNheight, &لوړوالی, عدم);
	إذا (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    إذا (event->x >= pos_x && event->x <= pos_x + پلنوالی
		    && event->y >= pos_y && event->y <= pos_y + لوړوالی)
		// Clicked on the scroller
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
tabline_menu_cb(
    Widget	w,
    Xt_اشاره_ګر	تړون ناکارول_شوی,
    XEvent	*e,
    Boolean	*continue_dispatch ناکارول_شوی)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحيح				tab_idx = 0;
    WidgetList			children;
    Cardinal			عدد_Children;
    ساكن XtIntervalId		timer = (XtIntervalId)0;
    ساكن صحيح			timed_بهر = TRUE;

    event = (XButtonPressedEvent *)e;

    إذا (event->button == Button1)
    {
	إذا (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    أرجع;

	إذا (!timed_بهر)
	{
	    Xt_Remove_وخت_بهر(timer);
	    timed_بهر = TRUE;

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
	    timer = Xt_App_جمع_کړه_وخت_بهر(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_بهر);
	    timed_بهر = FALSE;
	}
	أرجع;
    }

    إذا (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_ارزښتونه(tab_کرښه_menu, Xm_Nuser_معلومات, &tab_idx, عدم);
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
	tab_w = Xt_کړکۍ_منزل_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	إذا (tab_w != (Widget)0 && Xm_Is_ټېل_Button(tab_w))
	    Xt_Va_Get_ارزښتونه(tab_w, Xm_Npage_شمېره, &tab_idx, عدم);
    }

    Xt_Va_سېټ_ارزښتونه(tab_کرښه_menu, Xm_Nuser_معلومات, (Xt_اشاره_ګر)(اوږد)tab_idx, عدم);
    Xt_Va_Get_ارزښتونه(tab_کرښه_menu, XmNchildren, &children, XmNnumChildren,
		  &عدد_Children, عدم);
    XtManageChildren(children, عدد_Children);
    XmMenuPosition(tab_کرښه_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_ماشوم(tab_کرښه_menu);
}

    ساكن فراغ
tabline_balloon_cb(BalloonEval *beval, صحيح state ناکارول_شوی)
{
    صحيح		nr;
    tabpage_T	*tp;

    إذا (beval->موخه == (Widget)0)
	أرجع;

    Xt_Va_Get_ارزښتونه(beval->موخه, Xm_Npage_شمېره, &nr, عدم);
    tp = پیدا_کړه_tabpage(nr);
    إذا (tp == عدم)
	أرجع;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#د_که_پای

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
    بینښه حرف   label_type = (صحيح)XmSTRING;

    Xt_Va_Get_ارزښتونه(_w, XmNlabelType, &label_type, (Xt_اشاره_ګر)0);

    إذا (XtIsSensitive(_w) || label_type != (صحيح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وإلا
    {
	XGCValues   ارزښتونه;
	XtGCMask    ماسک;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	ماسک = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_ماسک | GCClipXOrigin | GCClipYOrigin;
	ارزښتونه.graphics_exposures = False;

	إذا (fs != 0)
	{
	    ماسک |= GCFont;
	    ارزښتونه.font = fs->fid;
	}

	إذا (lw->primitive.سر_shadow_pixmap != None
		&& lw->primitive.سر_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    ماسک |= GCFillStyle | GCTile;
	    ارزښتونه.fill_style = FillTiled;
	    ارزښتونه.tile = lw->primitive.سر_shadow_pixmap;
	}

	lw->label.متن_Rect.x += 1;
	lw->label.متن_Rect.y += 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x += 1;
	    lw->label.acc_متن_Rect.y += 1;
	}

	ارزښتونه.foreground = lw->primitive.سر_shadow_color;
	ارزښتونه.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, ماسک,
					       &ارزښتونه, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.متن_Rect.x -= 1;
	lw->label.متن_Rect.y -= 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x -= 1;
	    lw->label.acc_متن_Rect.y -= 1;
	}

	ارزښتونه.foreground = lw->primitive.bottom_shadow_color;
	ارزښتونه.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, ماسک,
					       &ارزښتونه, dynamic, (XtGCMask)0);
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
#که_تعریف_شوی FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		دلیلونه[10];
    صحيح		n;
    XmString	xms;
#د_که_پای

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
    gui.border_دننه_واټن = gui.border_پلنوالی;

    /*
     * Install the tearOffModel resource converter.
     */
#إذا (Xm_نسخه >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#د_که_پای

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_سېټ_ارزښتونه(vimShell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_پلنوالی, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_پلنوالی, 0,
	Xm_Nmargin_لوړوالی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_menu_colors(vimForm);

#که_تعریف_شوی FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحيح ac = 0;

# إذا (Xm_نسخه >= 1002)
	Xt_سېټ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_سېټ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_سېټ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_سېټ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_سېټ_Arg(al[ac], Xm_Nright_دننه_واټن, 0); ac++;
# endif
	Xt_سېټ_Arg(al[ac], Xm_Nmargin_لوړوالی, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_ماشوم(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#د_که_پای

#که_تعریف_شوی FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_چوکاټ = XtVaCreateWidget("toolBarFrame",
	xm_چوکاټ_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_لوړوالی, 0,
	Xm_Nmargin_پلنوالی, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	عدم);
    gui_motif_menu_colors(tool_Bar_چوکاټ);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_لیکه_ستنه_Widget_Class, tool_Bar_چوکاټ,
	XmNchildType, Xm_چوکاټ_WORKAREA_ماشوم,
	Xm_Nrow_ستنه_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_لوړوالی, 0,
	Xm_Nmargin_پلنوالی, 0,
	Xm_Nadjust_وروستی, True,
	عدم);
    gui_motif_menu_colors(toolBar);

#د_که_پای

#که_تعریف_شوی FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNbindingType, XmNONE); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNorientation, XmVERTICAL); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nback_پاڼه_اندازه, XmNONE); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nback_پاڼه_شمېره, 0); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nback_پاڼه_Placement, Xm_سر_ښی); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNmajorTabSpacing, 0); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNshadowThickness, 0); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_کرښه = XmCreateNotebook(vimForm, "Vim tabline", دلیلونه, n);

    Xt_جمع_کړه_Callback(tab_کرښه, Xm_Npage_بدل_شوی_Callback, (XtCallbackProc)tabline_cb,
			عدم);
    Xt_جمع_کړه_Event_Handler(tab_کرښه, Button_Press_ماسک, False,
			(XtEventHandler)tabline_menu_cb, عدم);

    gui.tabline_لوړوالی = TABLINE_لوړوالی;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_منزل_Widget(tab_کرښه, "MinorTabScrollerNext");
    Xt_Va_سېټ_ارزښتونه(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);
    scroller = Xt_Name_منزل_Widget(tab_کرښه, "MinorTabScrollerPrevious");
    Xt_Va_سېټ_ارزښتونه(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);

    // Create the tabline popup menu
    tab_کرښه_menu = XmCreatePopupMenu(tab_کرښه, "tabline popup", عدم, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nuser_معلومات, (Xt_اشاره_ګر)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_نښان);
    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, xms); n++;
    button = Xm_Create_ټېل_Button(tab_کرښه_menu, "Close", دلیلونه, n);
    Xt_جمع_کړه_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nuser_معلومات, (Xt_اشاره_ګر)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_نښان);
    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, xms); n++;
    button = Xm_Create_ټېل_Button(tab_کرښه_menu, "New Tab", دلیلونه, n);
    Xt_جمع_کړه_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nuser_معلومات, (Xt_اشاره_ګر)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_نښان);
    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, xms); n++;
    button = Xm_Create_ټېل_Button(tab_کرښه_menu, "Open tab...", دلیلونه, n);
    Xt_جمع_کړه_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);
#د_که_پای

    متن_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_پلنوالی, 0,
	Xm_Nmargin_لوړوالی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_scroll_colors(متن_Area_Form);

    متن_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, متن_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.شا_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_پلنوالی, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	عدم);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(متن_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.دننه_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    فراغ
gui_x11_destroy_widgets(فراغ)
{
    متن_Area = عدم;
#که_تعریف_شوی FEAT_MENU
    menuBar = عدم;
#د_که_پای
}

    فراغ
gui_mch_سېټ_متن_area_pos(
    صحيح	    x ناکارول_شوی,
    صحيح	    y ناکارول_شوی,
    صحيح	    w ناکارول_شوی,
    صحيح	    h ناکارول_شوی)
{
#که_تعریف_شوی FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#د_که_پای
}

    فراغ
gui_x11_سېټ_شا_color(فراغ)
{
    إذا (متن_Area != عدم)
#إذا (Xm_نسخه >= 1002)
	XmChangeColor(متن_Area, gui.شا_pixel);
#وإلا
	Xt_Va_سېټ_ارزښتونه(متن_Area,
		  XmNbackground, gui.شا_pixel,
		  عدم);
#د_که_پای
}

/*
 * Manage dialog centered on pointer.
 */
    فراغ
manage_centered(Widget dialog_ماشوم)
{
    Widget shell = Xt_پلار(dialog_ماشوم);
    کړکۍ ریښه, ماشوم;
    بینښه صحيح ماسک;
    بینښه صحيح پلنوالی, لوړوالی, border_پلنوالی, ژوروالی;
    صحيح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_ارزښتونه(shell, XmNmappedWhenManaged, &mappedWhenManaged, عدم);
    Xt_Va_سېټ_ارزښتونه(shell, XmNmappedWhenManaged, False, عدم);

    Xt_Manage_ماشوم(dialog_ماشوم);

    // Get the pointer position (x, y)
    XQuery_اشاره_ګر(XtDisplay(shell), Xt_کړکۍ(shell), &ریښه, &ماشوم,
		  &x, &y, &win_x, &win_y, &ماسک);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_کړکۍ(shell), &ریښه, &win_x, &win_y,
		 &پلنوالی, &لوړوالی, &border_پلنوالی, &ژوروالی);
    پلنوالی += 2 * border_پلنوالی;
    لوړوالی += 2 * border_پلنوالی;
    x -= پلنوالی / 2;
    y -= لوړوالی / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->پلنوالی - پلنوالی;
    maxY = XtScreen(shell)->لوړوالی - لوړوالی;
    إذا (x < 0)
	x = 0;
    إذا (x > maxX)
	x = maxX;
    إذا (y < 0)
	y = 0;
    إذا (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_سېټ_ارزښتونه(shell, XmNx, x, XmNy, y, عدم);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_سېټ_ارزښتونه(shell, XmNmappedWhenManaged, mappedWhenManaged, عدم);
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
    font_list = XmFontListCreate(font, STRING_نښان);
# وإلا
    // Motif 1.2 method
    Xm_Font_List_ثبت font_list_ثبت;

    font_list_ثبت = Xm_Font_List_ثبت_Create(STRING_نښان, XmFONT_IS_FONT,
					    (Xt_اشاره_ګر)font);
    font_list = Xm_Font_List_Append_ثبت(عدم, font_list_ثبت);
    Xm_Font_List_ثبت_Free(&font_list_ثبت);
# endif
    أرجع font_list;
}

# إذا ((Xm_نسخه > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_سېټ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_ثبت font_list_ثبت;

    font_list_ثبت = Xm_Font_List_ثبت_Create(STRING_نښان,
					    XmFONT_IS_FONTSET,
					    (Xt_اشاره_ګر)*fontset);
    font_list = Xm_Font_List_Append_ثبت(عدم, font_list_ثبت);
    Xm_Font_List_ثبت_Free(&font_list_ثبت);
    أرجع font_list;
}
# endif

#د_که_پای

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساكن فراغ gui_motif_جمع_کړه_actext(vimmenu_T *menu);
# إذا (Xm_نسخه >= 1002)
ساكن فراغ toggle_tearoff(Widget wid);
ساكن فراغ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساكن فراغ submenu_change(vimmenu_T *mp, صحيح colors);

ساكن فراغ do_سېټ_mnemonics(صحيح enable);
ساكن صحيح menu_enabled = TRUE;

    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag)
    {
	Xt_Manage_ماشوم(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_پلار(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_سېټ_ارزښتونه(Xt_پلار(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar),
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
    }
    وإلا
    {
	Xt_Unmanage_ماشوم(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_پلار(toolBar)))
	{
	    Xt_Va_سېټ_ارزښتونه(Xt_پلار(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar),
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    فراغ
gui_motif_سېټ_mnemonics(صحيح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    إذا (!menu_enabled)
	enable = FALSE;
    do_سېټ_mnemonics(enable);
}

    ساكن فراغ
do_سېټ_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	إذا (menu->id != (Widget)0)
	    Xt_Va_سېټ_ارزښتونه(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    عدم);
}

    فراغ
gui_mch_جمع_کړه_menu(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*پلار = menu->پلار;

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
		Xt_سېټ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    إذا (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_سېټ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(متن_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	أرجع;
    }
# endif

    إذا (!menu_is_menubar(menu->name)
	    || (پلار != عدم && پلار->submenu_id == (Widget)0))
	أرجع;

    label = XmStringCreate((حرف *)menu->dname, STRING_نښان);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (پلار == عدم) ? menuBar : پلار->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# إذا (Xm_نسخه >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_شاخص, idx + (پلار != عدم
			   && tearoff_val == (صحيح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    عدم);
    XmStringFree(label);

    إذا (menu->id == (Widget)0)		// failed
	أرجع;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    إذا (پلار == عدم && menu->لومړیتوب >= 9999)
	Xt_Va_سېټ_ارزښتونه(menuBar,
		XmNmenuHelpWidget, menu->id,
		عدم);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_جمع_کړه_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	عدم);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_لیکه_ستنه_Widget_Class, shell,
	Xm_Nrow_ستنه_Type, XmMENU_PULLDOWN,
	عدم);
    gui_motif_menu_colors(menu->submenu_id);

    إذا (menu->submenu_id == (Widget)0)		// failed
	أرجع;

# إذا (Xm_نسخه >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_سېټ_ارزښتونه(menu->id,
	XmNsubMenuId, menu->submenu_id,
	عدم);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    إذا (پلار == عدم)
	gui_mch_compute_menu_لوړوالی(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساكن فراغ
gui_motif_جمع_کړه_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    إذا (menu->actext == عدم || menu->id == (Widget)0)
	أرجع;

    label = XmStringCreate((حرف *)menu->actext, STRING_نښان);
    إذا (label == عدم)
	أرجع;
    Xt_Va_سېټ_ارزښتونه(menu->id, Xm_Naccelerator_متن, label, عدم);
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
    gui_mch_recurse_tearoffs(ریښه_menu);
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

    Xt_Va_سېټ_ارزښتونه(wid, XmNtearOffModel, tearoff_val, عدم);
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
	menu = menu->بل;
    }
}
# endif

    صحيح
gui_mch_متن_area_extra_لوړوالی(فراغ)
{
    Dimension	shadow_لوړوالی;

    Xt_Va_Get_ارزښتونه(متن_Area_Form, XmNshadowThickness, &shadow_لوړوالی, عدم);
    أرجع shadow_لوړوالی;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    فراغ
gui_mch_compute_menu_لوړوالی(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساكن Dimension	لوړوالی = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    إذا (id != (Widget)0)
	Xt_Va_Get_ارزښتونه(id, XmNheight, &لوړوالی, عدم);

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
	Xt_Manage_ماشوم(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_ارزښتونه(mp->id, XmNy, &y, عدم);
	    إذا (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_ارزښتونه(menuBar,
	Xm_Nmargin_لوړوالی, &margin,
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
    gui.menu_لوړوالی = maxy + لوړوالی - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    إذا (gui.shell_created)
	Xt_Va_سېټ_ارزښتونه(menuBar, XmNheight, gui.menu_لوړوالی, عدم);
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
check_xpm(char_u *لار)
{
    Xpm_ځانګړتیاوې attrs;
    صحيح		حالت;
    Pixmap	ماسک;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    حالت = Xpm_Read_File_منزل_Pixmap(gui.dpy,
	    ریښه_کړکۍ_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)لار, &map, &ماسک, &attrs);
    Xpm_Free_ځانګړتیاوې(&attrs);

    إذا (حالت == XpmSuccess)
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
	gui_پیدا_کړه_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	إذا (res == FAIL && gui_پیدا_کړه_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	إذا (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    أرجع tb_blank_xpm;
	}
    }

    إذا (menu->icon_builtin || gui_پیدا_کړه_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	إذا (menu->iconidx >= 0 && menu->iconidx
	       < (صحيح)(حجم(built_دننه_pixmaps) / حجم(built_دننه_pixmaps[0])))
	    xpm = built_دننه_pixmaps[menu->iconidx];
	وإلا
	    xpm = tb_blank_xpm;
    }

    أرجع xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساكن صحيح
جمع_کړه_pixmap_دلیلونه(vimmenu_T *menu, Arg *دلیلونه, صحيح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    إذا (menu->xpm == عدم)
    {
	Xt_سېټ_Arg(دلیلونه[n], XmNlabelType, XmSTRING); n++;
    }
    وإلا
    {
	إذا (menu->xpm_fname != عدم)
	{
	    Xt_سېټ_Arg(دلیلونه[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_سېټ_Arg(دلیلونه[n], Xm_Npixmap_معلومات, menu->xpm); n++;
	Xt_سېټ_Arg(دلیلونه[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    أرجع n;
}
# endif // FEAT_TOOLBAR

    فراغ
gui_mch_جمع_کړه_menu_توکی(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    vimmenu_T	*پلار = menu->پلار;

# إذا (Xm_نسخه <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    إذا (menu_is_ماشوم_د_popup(menu) && !mouse_model_popup())
	أرجع;
# endif

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(پلار->name))
    {
	WidgetClass	type;
	XmString	xms = عدم;    // fallback label if pixmap not found
	صحيح		n;
	Arg		دلیلونه[18];

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
	    Xt_سېټ_Arg(دلیلونه[n], XmNwidth, wid); n++;
	    Xt_سېټ_Arg(دلیلونه[n], Xm_Nmin_پلنوالی, wid); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNorientation, XmVERTICAL); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNseparatorType, Xm_SHADOW_ETCHED_دننه); n++;
	}
	وإلا
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_سېټ_Arg(دلیلونه[n], XmNhighlightThickness, 1); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNhighlightOnEnter, True); n++;
	    Xt_سېټ_Arg(دلیلونه[n], Xm_Nmargin_پلنوالی, 0); n++;
	    Xt_سېټ_Arg(دلیلونه[n], Xm_Nmargin_لوړوالی, 0); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_نښان);
	    Xt_سېټ_Arg(دلیلونه[n], XmNlabelString, xms); n++;

	    n = جمع_کړه_pixmap_دلیلونه(menu, دلیلونه, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_سېټ_Arg(دلیلونه[n], Xm_Nposition_شاخص, idx); n++;
	إذا (menu->id == عدم)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, دلیلونه, n);
	    إذا (menu->id != عدم && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_جمع_کړه_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وإلا
	    Xt_سېټ_ارزښتونه(menu->id, دلیلونه, n);
	إذا (xms != عدم)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_سېټ_tip(menu);
#  endif

	menu->پلار = پلار;
	menu->submenu_id = عدم;
	// When adding first item to toolbar it might have to be enabled .
	إذا (!XtIsManaged(Xt_پلار(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_لوړوالی = gui_mch_compute_toolbar_لوړوالی();
	أرجع;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    إذا (پلار->submenu_id == (Widget)0)
	أرجع;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    إذا (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, پلار->submenu_id,
# إذا (Xm_نسخه >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_شاخص, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		عدم);
	gui_motif_menu_colors(menu->id);
	أرجع;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_نښان);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ټېل_Button_Widget_Class, پلار->submenu_id,
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
	Xt_جمع_کړه_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_اشاره_ګر)menu);
	// add accelerator text
	gui_motif_جمع_کړه_actext(menu);
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
    إذا (!gui.دننه_use)
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
		  gui_mch_جمع_کړه_menu(menu, idx);
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
      وإلا إذا (menu_is_ماشوم_د_popup(menu))
      {
	  إذا (mouse_model_popup())
	      gui_mch_جمع_کړه_menu_توکی(menu, idx);
	  وإلا
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->بل;
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
    gui_motif_menu_colors(tool_Bar_چوکاټ);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ریښه_menu, TRUE);
}

    فراغ
gui_mch_new_menu_font(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    submenu_change(ریښه_menu, FALSE);
    {
	Dimension   لوړوالی;
	Position w, h;

	Xt_Va_Get_ارزښتونه(menuBar, XmNheight, &لوړوالی, عدم);
	gui.menu_لوړوالی = لوړوالی;

	Xt_Va_Get_ارزښتونه(vimShell, XtNwidth, &w, XtNheight, &h, عدم);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_حالت_area_لوړوالی()
# endif
		     );
    }
    gui_سېټ_shellsize(FALSE, TRUE, RESIZE_VERT);
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

    menu = gui_پیدا_کړه_menu((char_u *)"ToolBar");
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

    toolbar = gui_پیدا_کړه_menu((char_u *)"ToolBar");
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

    لكل (mp = menu; mp != عدم; mp = mp->بل)
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
		    Arg		دلیلونه[18];

		    n = جمع_کړه_pixmap_دلیلونه(mp, دلیلونه, n);
		    Xt_سېټ_ارزښتونه(mp->id, دلیلونه, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg دلیلونه[2];

		    دلیلونه[0].name = XmNbackground;
		    دلیلونه[0].ارزښت = gui.tooltip_bg_pixel;
		    دلیلونه[1].name = XmNforeground;
		    دلیلونه[1].ارزښت = gui.tooltip_fg_pixel;
		    Xt_سېټ_ارزښتونه(mp->tip->balloonLabel, &دلیلونه[0], Xt_شمېره(دلیلونه));
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
		    Arg دلیلونه[1];

		    دلیلونه[0].name = XmNfontList;
		    دلیلونه[0].ارزښت = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_سېټ_ارزښتونه(mp->tip->balloonLabel, &دلیلونه[0], Xt_شمېره(دلیلونه));
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

    Widget	    پلار;

    پلار = Xt_پلار(menu->id);
# إذا defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    إذا (پلار == toolBar && menu->tip != عدم)
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
    إذا (پلار == menuBar)
	gui_mch_compute_menu_لوړوالی((Widget)0);
# ifdef FEAT_TOOLBAR
    وإلا إذا (پلار == toolBar)
    {
	Cardinal    عدد_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_ارزښتونه(toolBar, XmNnumChildren, &عدد_children, عدم);
	إذا (عدد_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وإلا
	    gui.toolbar_لوړوالی = gui_mch_compute_toolbar_لوړوالی();
    }
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu ناکارول_شوی)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_وروستی_mouse_event());
    Xt_Manage_ماشوم(menu->submenu_id);
# endif
}

#د_که_پای // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    إذا (!gui.دننه_use)
	أرجع;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#که_تعریف_شوی FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#د_که_پای
}


/*
 * Scrollbar stuff.
 */

    فراغ
gui_mch_سېټ_scrollbar_thumb(
    scrollbar_T *sb,
    اوږد	val,
    اوږد	اندازه,
    اوږد	max)
{
    إذا (sb->id != (Widget)0)
	Xt_Va_سېټ_ارزښتونه(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_اندازه, اندازه,
		  XmNpageIncrement, (اندازه > 2 ? اندازه - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  عدم);
}

    فراغ
gui_mch_سېټ_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    إذا (sb->id == (Widget)0)
	أرجع;

    إذا (sb->type == SBAR_کیڼ || sb->type == SBAR_ښی)
    {
	إذا (y == 0)
	    h -= gui.border_دننه_واټن;
	وإلا
	    y -= gui.border_دننه_واټن;
	Xt_Va_سېټ_ارزښتونه(sb->id,
		Xm_Ntop_دننه_واټن, y,
		Xm_Nbottom_دننه_واټن, -y - h,
		XmNwidth, w,
		عدم);
    }
    وإلا
	Xt_Va_سېټ_ارزښتونه(sb->id,
		Xm_Ntop_دننه_واټن, y,
		Xm_Nleft_دننه_واټن, x,
		Xm_Nright_دننه_واټن, gui.which_scrollbars[SBAR_ښی]
						     ? gui.scrollbar_پلنوالی : 0,
		XmNheight, h,
		عدم);
    Xt_Manage_ماشوم(sb->id);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_ارزښتونه(متن_Area, XtNwidth, &tw, XtNx, &tx, عدم);
    Xt_Va_Get_ارزښتونه(vimShell, XtNwidth, &ww, عدم);
    xpad = ww - tw - tx - gui.scrollbar_پلنوالی;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_ارزښتونه(متن_Area, XtNheight, &th, XtNy, &ty, عدم);
    Xt_Va_Get_ارزښتونه(vimShell, XtNheight, &wh, عدم);
    ypad = wh - th - ty - gui.scrollbar_لوړوالی;
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    Arg		دلیلونه[16];
    صحيح		n;

    إذا (sb->id == (Widget)0)
	أرجع;

    n = 0;
    إذا (flag)
    {
	بدل (sb->type)
	{
	    حالة SBAR_کیڼ:
		Xt_سېټ_Arg(دلیلونه[n], Xm_Nleft_دننه_واټن, gui.scrollbar_پلنوالی); n++;
		اكسر;

	    حالة SBAR_ښی:
		Xt_سېټ_Arg(دلیلونه[n], Xm_Nright_دننه_واټن, gui.scrollbar_پلنوالی); n++;
		اكسر;

	    حالة SBAR_BOTTOM:
		Xt_سېټ_Arg(دلیلونه[n], Xm_Nbottom_دننه_واټن, gui.scrollbar_لوړوالی);n++;
		اكسر;
	}
	Xt_سېټ_ارزښتونه(متن_Area, دلیلونه, n);
	Xt_Manage_ماشوم(sb->id);
    }
    وإلا
    {
	إذا (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    بدل (sb->type)
	    {
		حالة SBAR_کیڼ:
		    Xt_سېټ_Arg(دلیلونه[n], Xm_Nleft_دننه_واټن, 0); n++;
		    اكسر;

		حالة SBAR_ښی:
		    Xt_سېټ_Arg(دلیلونه[n], Xm_Nright_دننه_واټن, 0); n++;
		    اكسر;

		حالة SBAR_BOTTOM:
		    Xt_سېټ_Arg(دلیلونه[n], Xm_Nbottom_دننه_واټن, 0);n++;
		    اكسر;
	    }
	    Xt_سېټ_ارزښتونه(متن_Area, دلیلونه, n);
	}
	Xt_Unmanage_ماشوم(sb->id);
    }
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		دلیلونه[16];
    صحيح		n = 0;

    Xt_سېټ_Arg(دلیلونه[n], XmNminimum, 0); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    بدل (sb->type)
    {
	حالة SBAR_کیڼ:
	    Xt_سېټ_Arg(دلیلونه[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNwidth, gui.scrollbar_پلنوالی); n++;
	    اكسر;

	حالة SBAR_ښی:
	    Xt_سېټ_Arg(دلیلونه[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNwidth, gui.scrollbar_پلنوالی); n++;
	    اكسر;

	حالة SBAR_BOTTOM:
	    Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNheight, gui.scrollbar_لوړوالی); n++;
	    اكسر;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, متن_Area_Form, دلیلونه, n);
    إذا (sb->id == (Widget)0)
	أرجع;

    gui_mch_سېټ_scrollbar_colors(sb);
    Xt_جمع_کړه_Callback(sb->id, Xm_Nvalue_بدل_شوی_Callback,
	    scroll_cb, (Xt_اشاره_ګر)sb->ident);
    Xt_جمع_کړه_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_اشاره_ګر)sb->ident);
    Xt_جمع_کړه_Event_Handler(sb->id, کلی_Press_ماسک, FALSE, gui_x11_کلی_موندنه_cb,
	    (Xt_اشاره_ګر)0);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    فراغ
gui_mch_سېټ_scrollbar_colors(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
    {
	إذا (gui.scroll_bg_pixel != INVALCOLOR)
	{
#إذا (Xm_نسخه>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#د_که_پای

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_سېټ_ارزښتونه(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    عدم);
	}

	إذا (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_سېټ_ارزښتونه(sb->id,
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

    کړکۍ
gui_x11_get_wid(فراغ)
{
    أرجع(Xt_کړکۍ(متن_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساكن فراغ
do_mnemonic(Widget w, بینښه صحيح keycode)
{
    WidgetList	    children;
    صحيح		    عدد_Children, i;
    Boolean	    isMenu;
    کلی_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    user_معلومات_2;
    بینښه حرف   لیکه_ستون_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_لیکه_ستنه_Widget_Class)
	{
	    Xt_Va_Get_ارزښتونه(w, Xm_Nrow_ستنه_Type, &لیکه_ستون_Type, عدم);
	    isMenu = (لیکه_ستون_Type != (بینښه حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_ارزښتونه(w, XmNchildren, &children, XmNnumChildren,
			  &عدد_Children, عدم);
	    لكل (i = 0; i < عدد_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وإلا
    {
	Xt_Va_Get_ارزښتونه(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    إذا (XKeysym_منزل_Keycode(XtDisplay(Xt_پلار(w)),
				       XString_منزل_Keysym(mneString)) == keycode)
	    {
		إذا (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_ارزښتونه(w, Xm_Nuser_معلومات, &user_معلومات_2, عدم);
		    إذا (user_معلومات_2 != عدم && XtIsWidget(user_معلومات_2))
			XmProcessTraversal(user_معلومات_2, XmTRAVERSE_CURRENT);
		}
		وإلا
		{
		    XKeyPressedEvent کلی_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_ډګر(کلی_Event);
		    کلی_Event.type = کلی_Press;
		    کلی_Event.serial = 1;
		    کلی_Event.send_event = True;
		    کلی_Event.display = XtDisplay(w);
		    کلی_Event.کړکۍ_2 = Xt_کړکۍ(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & کلی_Event,
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
	Xt_اشاره_ګر   call_معلومات ناکارول_شوی,
	XKeyEvent   *event,
	Boolean	    *b ناکارول_شوی)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ساكن فراغ
جمع_کړه_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحيح		    عدد_Children, i;
    Boolean	    isMenu;
    کلی_Sym	    mnemonic = '\0';
    بینښه حرف   لیکه_ستون_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_لیکه_ستنه_Widget_Class)
	{
	    Xt_Va_Get_ارزښتونه(w, Xm_Nrow_ستنه_Type, &لیکه_ستون_Type, عدم);
	    isMenu = (لیکه_ستون_Type != (بینښه حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_ارزښتونه(w, XmNchildren, &children, XmNnumChildren,
							  &عدد_Children, عدم);
	    لكل (i = 0; i < عدد_Children; i++)
		جمع_کړه_mnemonic_grabs(dialog, children[i]);
	}
    }
    وإلا
    {
	Xt_Va_Get_ارزښتونه(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_کلی(dialog, XKeysym_منزل_Keycode(XtDisplay(dialog),
						  XString_منزل_Keysym(mneString)),
		    Mod1_ماسک, True, Grab_ډېرپېښ_Async, Grab_ډېرپېښ_Async);
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

    Xt_جمع_کړه_Event_Handler(dialog, کلی_Press_ماسک, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشاره_ګر) عدم);
    جمع_کړه_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساكن فراغ
suppress_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_Ungrab_کلی(dialog, Any_کلی, Mod1_ماسک);
    XtRemoveEventHandler(dialog, کلی_Press_ماسک, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشاره_ګر) عدم);
}

#إذا defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساكن فراغ
سېټ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    إذا (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_سېټ *)&gui.fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_ماشوم(id);
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ماشوم(id);
	    }
	    وإلا
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_ماشوم(id);
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ماشوم(id);
	    }
	    وإلا
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
}
#د_که_پای

#إذا defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعريفنوع بنية dialog_callback_arg
{
    حرف *  دلیلونه;   // not used right now
    صحيح	    id;
} dcbarg_T;

ساكن Widget dialog_wgt;
ساكن حرف *browse_fname = عدم;
ساكن Xm_String_Char_سېټ charset = (Xm_String_Char_سېټ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساكن فراغ DialogCancelCB(Widget, Xt_اشاره_ګر, Xt_اشاره_ګر);
ساكن فراغ DialogAcceptCB(Widget, Xt_اشاره_ګر, Xt_اشاره_ګر);

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
سېټ_predefined_label(Widget پلار, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *بل;
    کلی_Sym	mnemonic = NUL;

    w = Xt_Name_منزل_Widget(پلار, name);

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)new_label);
    إذا (p == عدم)
	أرجع;
    لكل (بل = p; *بل; ++بل)
    {
	إذا (*بل == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(بل);

	    إذا (len > 0)
	    {
		mch_memmove(بل, بل + 1, len);
		mnemonic = بل[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_نښان);
    vim_free(p);

    إذا (str != عدم)
    {
	Xt_Va_سېټ_ارزښتونه(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
سېټ_predefined_fontlist(Widget پلار, String name)
{
    Widget w;
    w = Xt_Name_منزل_Widget(پلار, name);

    إذا (!w)
	أرجع;

    سېټ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحيح		saving ناکارول_شوی,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext ناکارول_شوی,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*نمونه;
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
	جمع_کړه_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    إذا (filter == عدم)
	نمونه = (char_u *)"";
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
	نمونه = vim_strnsave(s, p - s);
	tofree = نمونه;
	إذا (نمونه == عدم)
	    نمونه = (char_u *)"";
    }

    Xt_Va_سېټ_ارزښتونه(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (حرف *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (حرف *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (حرف *)نمونه, STRLEN(نمونه) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (حرف *)title, STRLEN(title) + 1,
	عدم);

    سېټ_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    سېټ_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    سېټ_predefined_label(dialog_wgt, "Dir", _("Directories"));
    سېټ_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    سېټ_predefined_label(dialog_wgt, "Help", _("&Help"));
    سېټ_predefined_label(dialog_wgt, "Items", _("Files"));
    سېټ_predefined_label(dialog_wgt, "OK", _("&OK"));
    سېټ_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    سېټ_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    سېټ_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_سېټ_ارزښتونه(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, عدم);

    Xt_جمع_کړه_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_اشاره_ګر)0);
    Xt_جمع_کړه_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_اشاره_ګر)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_ماشوم(Xm_File_Selection_Box_Get_ماشوم(dialog_wgt,
					(بینښه حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    افعل
    {
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(dialog_wgt),
	    (Xt_ننوت_ماسک)XtIMAll);
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
    Widget	w ناکارول_شوی,		//  widget id
    Xt_اشاره_ګر	client_معلومات_2 ناکارول_شوی,	//  data from application
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)	//  data from widget class
{
    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    Xt_Unmanage_ماشوم(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساكن فراغ
DialogAcceptCB(
    Widget	w ناکارول_شوی,		//  widget id
    Xt_اشاره_ګر	client_معلومات_2 ناکارول_شوی,	//  data from application
    Xt_اشاره_ګر	call_معلومات)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_معلومات;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->ارزښت, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_ماشوم(dialog_wgt);
}

#د_که_پای // FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

ساكن صحيح	dialog_حالت;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساكن فراغ
keyhit_callback(
    Widget		w,
    Xt_اشاره_ګر		client_معلومات_2 ناکارول_شوی,
    XEvent		*event,
    Boolean		*cont ناکارول_شوی)
{
    حرف	buf[2];
    کلی_Sym	کلی_sym;

    إذا (XLookupString(&(event->xkey), buf, 2, &کلی_sym, عدم) == 1)
    {
	إذا (*buf == CAR)
	    dialog_حالت = 1;
	وإلا إذا (*buf == ESC)
	    dialog_حالت = 2;
    }
    إذا ((کلی_sym == XK_کیڼ || کلی_sym == XK_ښی)
	    && !(event->xkey.state & ښويدنه_ماسک))
	Xm_متن_ډګر_Clear_Selection(w, Xt_وروستی_Timestamp_Processed(gui.dpy));
}

    ساكن فراغ
butproc(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    dialog_حالت = (صحيح)(اوږد)client_معلومات_2 + 1;
}

# ifdef HAVE_XPM

    ساكن Widget
create_pixmap_label(
    Widget	پلار,
    String	name,
    حرف	**معلومات,
    ArgList	دلیلونه,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحيح			ژوروالی;
    Pixmap		pixmap = 0;
    Xpm_ځانګړتیاوې	attr;
    Boolean		rs;
    Xpm_Color_نښه	color[5] =
    {
	{"none", عدم, 0},
	{"iconColor1", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"selectColor", عدم, 0}
    };

    label = XmCreateLabelGadget(پلار, name, دلیلونه, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_د_Object(label);
    scr = Xt_Screen_د_Object(label);
    Xt_Va_Get_ارزښتونه(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_پلار(label),
		  XmNdepth, &ژوروالی,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  عدم);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_ژوروالی;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.ژوروالی = ژوروالی;
    Xpm_Create_Pixmap_From_معلومات(dsp, ریښه_کړکۍ_د_Screen(scr),
		    معلومات, &pixmap, عدم, &attr);

    Xt_Va_Get_ارزښتونه(label, Xm_Nrecompute_اندازه, &rs, عدم);
    Xt_Va_سېټ_ارزښتونه(label, Xm_Nrecompute_اندازه, True, عدم);
    Xt_Va_سېټ_ارزښتونه(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    عدم);
    Xt_Va_سېټ_ارزښتونه(label, Xm_Nrecompute_اندازه, rs, عدم);

    أرجع label;
}
# endif

    صحيح
gui_mch_dialog(
    صحيح		type ناکارول_شوی,
    char_u	*title,
    char_u	*پیغام_2,
    char_u	*button_نومونه,
    صحيح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحيح		ex_cmd ناکارول_شوی)
{
    char_u		*buts;
    char_u		*p, *بل;
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
    Arg			دلیلونه[6];
# ifdef HAVE_XPM
    حرف		**icon_معلومات_2 = عدم;
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
    Xt_Va_سېټ_ارزښتونه(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    عدم);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_نومونه);
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
	کلی_Sym mnemonic = NUL;

	لكل (بل = p; *بل; ++بل)
	{
	    إذا (*بل == DLG_HOTKEY_CHAR)
	    {
		صحيح len = STRLEN(بل);

		إذا (len > 0)
		{
		    mch_memmove(بل, بل + 1, len);
		    mnemonic = بل[0];
		}
	    }
	    إذا (*بل == DLG_BUTTON_SEP)
	    {
		*بل++ = NUL;
		اكسر;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_نښان);
	إذا (label == عدم)
	    اكسر;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ټېل_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_دننه_واټن, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		عدم);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	إذا (butcount > 0)
	{
	    إذا (vertical)
		Xt_Va_سېټ_ارزښتونه(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			عدم);
	    وإلا
	    {
		إذا (*بل == NUL)
		{
		    Xt_Va_سېټ_ارزښتونه(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_دننه_واټن, 4,
			    عدم);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_دننه_واټن, 4,
			    عدم);
		    Xt_Manage_ماشوم(sep_form);
		}
		وإلا
		{
		    Xt_Va_سېټ_ارزښتونه(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    عدم);
		}
	    }
	}
	وإلا إذا (!vertical)
	{
	    إذا (*بل == NUL)
	    {
		Xt_Va_سېټ_ارزښتونه(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_دننه_واټن, 4,
			عدم);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_دننه_واټن, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_دننه_واټن, 4,
			عدم);
		Xt_Manage_ماشوم(sep_form);
	    }
	    وإلا
		Xt_Va_سېټ_ارزښتونه(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_دننه_واټن, 4,
			عدم);
	}

	Xt_جمع_کړه_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_اشاره_ګر)(اوږد)butcount);
	p = بل;
    }
    vim_free(buts);

    separator = (Widget) 0;
    إذا (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_سېټ_Arg(دلیلونه[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_سېټ_Arg(دلیلونه[n], XmNbottomWidget, buttons[0]); n++;
	Xt_سېټ_Arg(دلیلونه[n], Xm_Nbottom_دننه_واټن, 4); n++;
	Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", دلیلونه, n);
	Xt_Manage_ماشوم(separator);
    }

    إذا (textfield != عدم)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_متن_ډګر_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	إذا (butcount > 0)
	    Xt_Va_سېټ_ارزښتونه(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سېټ_ارزښتونه(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	سېټ_fontlist(dialogtextfield);
	Xm_متن_ډګر_سېټ_String(dialogtextfield, (حرف *)textfield);
	Xt_Manage_ماشوم(dialogtextfield);
	Xt_جمع_کړه_Event_Handler(dialogtextfield, کلی_Press_ماسک, False,
			    (XtEventHandler)keyhit_callback, (Xt_اشاره_ګر)عدم);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    عدم);
    Xt_Manage_ماشوم(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    بدل (type)
    {
	حالة VIM_GENERIC:
	    icon_معلومات_2 = generic_xpm;
	    اكسر;
	حالة VIM_ERROR:
	    icon_معلومات_2 = error_xpm;
	    اكسر;
	حالة VIM_WARNING:
	    icon_معلومات_2 = alert_xpm;
	    اكسر;
	حالة VIM_INFO:
	    icon_معلومات_2 = info_xpm;
	    اكسر;
	حالة VIM_QUESTION:
	    icon_معلومات_2 = quest_xpm;
	    اكسر;
	افتراضي:
	    icon_معلومات_2 = generic_xpm;
    }

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Ntop_دننه_واټن, 8); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nbottom_دننه_واټن, 8); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nleft_دننه_واټن, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_معلومات_2, دلیلونه, n);
    Xt_Manage_ماشوم(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)پیغام_2, STRING_نښان);
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
				Xm_Ntop_دننه_واټن, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# وإلا
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_دننه_واټن, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_دننه_واټن, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_دننه_واټن, 8,
				عدم);
    XmStringFree(label);
    سېټ_fontlist(w);

    إذا (textfield != عدم)
    {
	Xt_Va_سېټ_ارزښتونه(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		عدم);
    }
    وإلا
    {
	إذا (butcount > 0)
	    Xt_Va_سېټ_ارزښتونه(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سېټ_ارزښتونه(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
    }

    إذا (dfltbutton < 1)
	dfltbutton = 1;
    إذا (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_سېټ_ارزښتونه(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], عدم);
    إذا (textfield != عدم)
	Xt_Va_سېټ_ارزښتونه(dialogform, XmNinitialFocus, dialogtextfield, عدم);
    وإلا
	Xt_Va_سېټ_ارزښتونه(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									عدم);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    إذا (textfield != عدم && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_متن_ډګر_سېټ_Selection(dialogtextfield,
			 (Xm_متن_Position)0, (Xm_متن_Position)STRLEN(textfield),
					   Xt_وروستی_Timestamp_Processed(gui.dpy));
	Xm_متن_ډګر_سېټ_Cursor_Position(dialogtextfield,
					   (Xm_متن_Position)STRLEN(textfield));
    }

    app = Xt_Widget_منزل_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_حالت = -1;
    لكل (;;)
    {
	XtAppProcessEvent(app, (Xt_ننوت_ماسک)XtIMAll);
	إذا (dialog_حالت >= 0 || !XtIsManaged(dialogform))
	    اكسر;
    }

    vim_free(buttons);

    إذا (textfield != عدم)
    {
	p = (char_u *)Xm_متن_Get_String(dialogtextfield);
	إذا (p == عدم || dialog_حالت < 0)
	    *textfield = NUL;
	وإلا
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((حرف *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    أرجع dialog_حالت;
}
#د_که_پای // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    Cardinal	عدد_Children;	    // how many children toolBar has

    إذا (toolBar == (Widget)0)
	أرجع;
    Xt_Va_Get_ارزښتونه(toolBar, XmNnumChildren, &عدد_Children, عدم);
    إذا (showit && عدد_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_ارزښتونه(toolBar, XmNchildren, &children, عدم);
	{
	    فراغ    (*action)(BalloonEval *);
	    صحيح	    متن = 0;

	    إذا (مزي_پلټنه((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وإلا
		action = &gui_mch_disable_beval_area;
	    إذا (مزي_پلټنه((ثابت حرف *)p_toolbar, "text"))
		متن = 1;
	    وإلا إذا (مزي_پلټنه((ثابت حرف *)p_toolbar, "icons"))
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
		    لكل (cur = toolbar->children; cur; cur = cur->بل)
		    {
			Arg	    دلیلونه[1];
			صحيح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			إذا (cur->tip != عدم)
			    (*action)(cur->tip);
			إذا (!menu_is_separator(cur->name))
			{
			    إذا (متن == 1 || cur->xpm == عدم)
			    {
				Xt_سېټ_Arg(دلیلونه[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    إذا (cur->id != عدم)
			    {
				Xt_Unmanage_ماشوم(cur->id);
				Xt_سېټ_ارزښتونه(cur->id, دلیلونه, n);
				Xt_Manage_ماشوم(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_لوړوالی = gui_mch_compute_toolbar_لوړوالی();
	Xt_Manage_ماشوم(Xt_پلار(toolBar));
# ifdef FEAT_GUI_TABLINE
	إذا (showing_tabline)
	{
	    Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_پلار(toolBar),
			  عدم);
	    Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_کرښه,
			  عدم);
	}
	وإلا
# endif
	    Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_پلار(toolBar),
			  عدم);
	إذا (XtIsManaged(menuBar))
	    Xt_Va_سېټ_ارزښتونه(Xt_پلار(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    عدم);
	وإلا
	    Xt_Va_سېټ_ارزښتونه(Xt_پلار(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    عدم);
    }
    وإلا
    {
	gui.toolbar_لوړوالی = 0;
	إذا (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
	وإلا
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_کرښه,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}

	Xt_Unmanage_ماشوم(Xt_پلار(toolBar));
    }
    gui_سېټ_shellsize(FALSE, FALSE, RESIZE_VERT);
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
gui_mch_compute_toolbar_لوړوالی(فراغ)
{
    Dimension	borders;
    Dimension	لوړوالی;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	عدد_Children;	    // how many children toolBar has
    صحيح		i;

    borders = 0;
    لوړوالی = 0;
    إذا (toolBar != (Widget)0 && tool_Bar_چوکاټ != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_ارزښتونه(tool_Bar_چوکاټ,
		XmNshadowThickness, &fst,
		Xm_Nmargin_لوړوالی, &fmh,
		عدم);
	borders += fst + fmh;
	Xt_Va_Get_ارزښتونه(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_لوړوالی, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &عدد_Children, عدم);
	borders += tst + tmh;
	لكل (i = 0; i < (صحيح)عدد_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_ارزښتونه(children[i], XmNheight, &whgt, عدم);
	    إذا (لوړوالی < whgt)
		لوړوالی = whgt;
	}
    }
# ifdef LESSTIF_نسخه
    // Hack: When starting up we get wrong dimensions.
    إذا (لوړوالی < 10)
	لوړوالی = 24;
# endif

    أرجع (صحيح)(لوړوالی + (borders << 1));
}

    فراغ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_ارزښتونه(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    عدم);
}
#د_که_پای

#إذا defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (tab_کرښه == (Widget)0)
	أرجع;

    إذا (!showit != !showing_tabline)
    {
	إذا (showit)
	{
	    Xt_Manage_ماشوم(tab_کرښه);
	    Xt_Unmanage_ماشوم(Xt_Name_منزل_Widget(tab_کرښه, "PageScroller"));
	    Xt_Unmanage_ماشوم(Xt_Name_منزل_Widget(tab_کرښه, "MinorTabScrollerNext"));
	    Xt_Unmanage_ماشوم(Xt_Name_منزل_Widget(tab_کرښه,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_پلار(toolBar)))
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(tab_کرښه,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	    Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_کرښه,
			  عدم);
	}
	وإلا
	{
	    Xt_Unmanage_ماشوم(tab_کرښه);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_پلار(toolBar)))
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_پلار(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سېټ_ارزښتونه(متن_Area_Form,
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
    أرجع tab_کرښه != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline(فراغ)
{
    tabpage_T		*tp;
    صحيح			nr = 1, n;
    Arg			دلیلونه[10];
    صحيح			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_پاڼه_Info	پاڼه_info;
    Xm_Notebook_پاڼه_حالت پاڼه_حالت;
    صحيح			وروستی_پاڼه, tab_شمېر;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    إذا (tab_کرښه == (Widget)0)
	أرجع;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = لومړی_tabpage; tp != عدم; tp = tp->tp_بل, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	پاڼه_حالت = Xm_Notebook_Get_پاڼه_Info(tab_کرښه, nr, &پاڼه_info);
	إذا (پاڼه_حالت == Xm_پاڼه_INVALID
		|| پاڼه_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_سېټ_Arg(دلیلونه[n], Xm_Nnotebook_ماشوم_Type, XmMAJOR_TAB); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNtraversalOn, False); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNhighlightThickness, 1); n++;
	    Xt_سېټ_Arg(دلیلونه[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ټېل_Button(tab_کرښه, "-Empty-", دلیلونه, n);
	    Xt_Manage_ماشوم(tab);
	    beval = gui_mch_create_beval_area(tab, عدم, tabline_balloon_cb,
									عدم);
	    Xt_Va_سېټ_ارزښتونه(tab, Xm_Nuser_معلومات, beval, عدم);
	}
	وإلا
	    tab = پاڼه_info.major_tab_widget;

	Xt_Va_سېټ_ارزښتونه(tab, Xm_Npage_شمېره, nr, عدم);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_ارزښتونه(tab, XmNlabelString, &label_str, عدم);
	إذا (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    إذا (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_سېټ_ارزښتونه(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, عدم);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_ماشوم(tab);
		Xt_Manage_ماشوم(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_شمېر = nr - 1;

    Xt_Va_Get_ارزښتونه(tab_کرښه, Xm_Nlast_پاڼه_شمېره, &وروستی_پاڼه, عدم);

    // Remove any old labels.
    طالما (nr <= وروستی_پاڼه)
    {
	إذا (Xm_Notebook_Get_پاڼه_Info(tab_کرښه, nr, &پاڼه_info) != Xm_پاڼه_INVALID
	    && پاڼه_info.پاڼه_شمېره == nr
	    && پاڼه_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_ارزښتونه(پاڼه_info.major_tab_widget, Xm_Nuser_معلومات, &beval, عدم);
	    إذا (beval != عدم)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_ماشوم(پاڼه_info.major_tab_widget);
	    XtDestroyWidget(پاڼه_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_سېټ_ارزښتونه(tab_کرښه, Xm_Nlast_پاڼه_شمېره, tab_شمېر, عدم);

    Xt_Va_Get_ارزښتونه(tab_کرښه, Xm_Ncurrent_پاڼه_شمېره, &currentpage, عدم);
    إذا (currentpage != curtabidx)
	Xt_Va_سېټ_ارزښتونه(tab_کرښه, Xm_Ncurrent_پاڼه_شمېره, curtabidx, عدم);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سېټ_curtab(صحيح nr)
{
    صحيح		currentpage;

    إذا (tab_کرښه == (Widget)0)
	أرجع;

    Xt_Va_Get_ارزښتونه(tab_کرښه, Xm_Ncurrent_پاڼه_شمېره, &currentpage, عدم);
    إذا (currentpage != nr)
	Xt_Va_سېټ_ارزښتونه(tab_کرښه, Xm_Ncurrent_پاڼه_شمېره, nr, عدم);
}
#د_که_پای

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
	Xt_Va_سېټ_ارزښتونه(id, XmNbackground, gui.menu_bg_pixel, عدم);
#د_که_پای
    إذا (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_سېټ_ارزښتونه(id, XmNforeground, gui.menu_fg_pixel, عدم);
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
	Xt_Va_سېټ_ارزښتونه(id, XmNbackground, gui.scroll_bg_pixel, عدم);
#د_که_پای
    إذا (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_سېټ_ارزښتونه(id, XmNforeground, gui.scroll_fg_pixel, عدم);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    فراغ
gui_motif_menu_fontlist(Widget id ناکارول_شوی)
{
#که_تعریف_شوی FEAT_MENU
# ifdef FONTSET_ALWAYS
    إذا (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_سېټ *)&gui.menu_fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_ماشوم(id);
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ماشوم(id);
	    }
	    وإلا
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_ماشوم(id);
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ماشوم(id);
	    }
	    وإلا
		Xt_Va_سېټ_ارزښتونه(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
#د_که_پای
}


/*
 * We don't create it twice for the sake of speed.
 */

تعريفنوع بنية Shared_پیدا_کړه_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget پورته;		// search direction 'Up' radio button
    Widget ښکته;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget پیدا_کړه;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_پیدا_کړه_Replace_2;

ساكن Shared_پیدا_کړه_Replace_2 پیدا_کړه_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن Shared_پیدا_کړه_Replace_2 repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن فراغ
پیدا_کړه_replace_destroy_callback(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    Shared_پیدا_کړه_Replace_2 *cd = (Shared_پیدا_کړه_Replace_2 *)client_معلومات_2;

    إذا (cd != عدم)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساكن فراغ
پیدا_کړه_replace_dismiss_callback(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    Shared_پیدا_کړه_Replace_2 *cd = (Shared_پیدا_کړه_Replace_2 *)client_معلومات_2;

    إذا (cd != عدم)
	Xt_Unmanage_ماشوم(cd->dialog);
}

    ساكن فراغ
ثبت_activate_callback(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    XmProcessTraversal((Widget)client_معلومات_2, XmTRAVERSE_CURRENT);
}

    ساكن فراغ
پیدا_کړه_replace_callback(
    Widget	w ناکارول_شوی,
    Xt_اشاره_ګر	client_معلومات_2,
    Xt_اشاره_ګر	call_معلومات ناکارول_شوی)
{
    long_u	flags = (long_u)client_معلومات_2;
    حرف	*پیدا_کړه_متن, *repl_متن;
    Boolean	direction_ښکته = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_پیدا_کړه_Replace_2 *sfr;

    إذا (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = تشه_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	أرجع;
    }

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &پیدا_کړه_widgets;
    }
    وإلا
    {
	repl_متن = Xm_متن_ډګر_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    پیدا_کړه_متن = Xm_متن_ډګر_Get_String(sfr->what);
    Xt_Va_Get_ارزښتونه(sfr->ښکته, XmNset, &direction_ښکته, عدم);
    Xt_Va_Get_ارزښتونه(sfr->wword, XmNset, &wword, عدم);
    Xt_Va_Get_ارزښتونه(sfr->mcase, XmNset, &mcase, عدم);
    إذا (wword)
	flags |= FRD_ټولیز_کلمه;
    إذا (mcase)
	flags |= FRD_سمون_CASE;

    (فراغ)gui_do_findrepl((صحيح)flags, (char_u *)پیدا_کړه_متن, (char_u *)repl_متن,
							      direction_ښکته);

    إذا (پیدا_کړه_متن != عدم)
	XtFree(پیدا_کړه_متن);
    إذا (repl_متن != عدم)
	XtFree(repl_متن);
}

    ساكن فراغ
پیدا_کړه_replace_keypress(
    Widget		w ناکارول_شوی,
    Shared_پیدا_کړه_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b ناکارول_شوی)
{
    کلی_Sym keysym;

    إذا (frdp == عدم)
	أرجع;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    إذا (keysym == XK_Escape)
	Xt_Unmanage_ماشوم(frdp->dialog);
}

    ساكن فراغ
سېټ_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *بل;
    کلی_Sym	mnemonic = NUL;

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)label);
    إذا (p == عدم)
	أرجع;
    لكل (بل = p; *بل; ++بل)
    {
	إذا (*بل == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(بل);

	    إذا (len > 0)
	    {
		mch_memmove(بل, بل + 1, len);
		mnemonic = بل[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    إذا (str)
    {
	Xt_Va_سېټ_ارزښتونه(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
پیدا_کړه_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_پیدا_کړه_Replace_2	*frdp;
    Widget		separator;
    Widget		ننوت_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		چوکاټ;
    XmString		str;
    صحيح			n;
    Arg			دلیلونه[6];
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    Dimension		پلنوالی;
    Dimension		widest;
    char_u		*ثبت_متن;

    frdp = do_replace ? &repl_widgets : &پیدا_کړه_widgets;

    // Get the search string to use.
    ثبت_متن = get_پیدا_کړه_dialog_متن(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	إذا (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_کړکۍ(Xt_پلار(frdp->dialog)));
	وإلا
	    Xt_Manage_ماشوم(frdp->dialog);
	XtPopup(Xt_پلار(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	إذا (ثبت_متن != عدم)
	    Xm_متن_ډګر_سېټ_String(frdp->what, (حرف *)ثبت_متن);
	vim_free(ثبت_متن);

	Xt_Va_سېټ_ارزښتونه(frdp->wword, XmNset, wword, عدم);
	أرجع;
    }

    // Create a fresh new dialog window
    إذا (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وإلا
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNautoUnmanage, False); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNnoResize, True); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", دلیلونه, n);
    XmStringFree(str);
    Xt_جمع_کړه_Callback(frdp->dialog, XmNdestroyCallback,
	    پیدا_کړه_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_دننه_واټن, 4,
	    عدم);

    frdp->پیدا_کړه = XtVaCreateManagedWidget("findButton",
	    xm_ټېل_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);
    سېټ_label(frdp->پیدا_کړه, _("Find &Next"));

    Xt_جمع_کړه_Callback(frdp->پیدا_کړه, XmNactivateCallback,
	    پیدا_کړه_replace_callback,
	    (do_replace ? (Xt_اشاره_ګر)FRD_R_FINDNEXT : (Xt_اشاره_ګر)FRD_FINDNEXT));

    إذا (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ټېل_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->پیدا_کړه,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سېټ_label(frdp->replace, _("&Replace"));
	Xt_جمع_کړه_Callback(frdp->replace, XmNactivateCallback,
		پیدا_کړه_replace_callback, (Xt_اشاره_ګر)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ټېل_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سېټ_label(frdp->all, _("Replace &All"));
	Xt_جمع_کړه_Callback(frdp->all, XmNactivateCallback,
		پیدا_کړه_replace_callback, (Xt_اشاره_ګر)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ټېل_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سېټ_label(frdp->undo, _("&Undo"));
	Xt_جمع_کړه_Callback(frdp->undo, XmNactivateCallback,
		پیدا_کړه_replace_callback, (Xt_اشاره_ګر)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ټېل_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    عدم);
    سېټ_label(frdp->cancel, _("&Cancel"));
    Xt_جمع_کړه_Callback(frdp->cancel, XmNactivateCallback,
	    پیدا_کړه_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_ماشوم(button_form);

    n = 0;
    Xt_سېټ_Arg(دلیلونه[n], XmNorientation, XmVERTICAL); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNrightWidget, button_form); n++;
    Xt_سېټ_Arg(دلیلونه[n], Xm_Nright_دننه_واټن, 4); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سېټ_Arg(دلیلونه[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", دلیلونه, n);
    Xt_Manage_ماشوم(separator);

    ننوت_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 4,
	    عدم);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ننوت_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_دننه_واټن, 4,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_متن_ډګر_Widget_Class, ننوت_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		عدم);

	إذا (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_متن_ډګر_Widget_Class,	ننوت_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_دننه_واټن, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	    Xt_جمع_کړه_Callback(frdp->with, XmNactivateCallback,
		    پیدا_کړه_replace_callback, (Xt_اشاره_ګر) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ننوت_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_دننه_واټن, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_جمع_کړه_Callback(frdp->what, XmNactivateCallback,
		    ثبت_activate_callback, frdp->with);
	    Xt_جمع_کړه_Event_Handler(frdp->with, کلی_Press_ماسک, False,
			    (XtEventHandler)پیدا_کړه_replace_keypress,
			    (Xt_اشاره_ګر) frdp);

	}
	وإلا
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_جمع_کړه_Callback(frdp->what, XmNactivateCallback,
		    پیدا_کړه_replace_callback, (Xt_اشاره_ګر)FRD_FINDNEXT);
	}
	Xt_جمع_کړه_Event_Handler(frdp->what, کلی_Press_ماسک, False,
			    (XtEventHandler)پیدا_کړه_replace_keypress,
			    (Xt_اشاره_ګر)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_سېټ_Arg(دلیلونه[n], XmNwidth, &پلنوالی); n++;
	Xt_Get_ارزښتونه(label_what, دلیلونه, n);
	widest = پلنوالی;
	إذا (do_replace)
	{
	    Xt_Get_ارزښتونه(label_with, دلیلونه, n);
	    إذا (پلنوالی > widest)
		widest = پلنوالی;
	}

	Xt_Va_سېټ_ارزښتونه(frdp->what, Xm_Nleft_دننه_واټن, widest, عدم);
	إذا (do_replace)
	    Xt_Va_سېټ_ارزښتونه(frdp->with, Xm_Nleft_دننه_واټن, widest, عدم);

    }

    Xt_Manage_ماشوم(ننوت_form);

    {
	Widget radio_box;
	Widget w;

	چوکاټ = XtVaCreateWidget("directionFrame",
		xm_چوکاټ_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ننوت_form,
		Xm_Ntop_دننه_واټن, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_دننه_واټن, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ننوت_form,
		عدم);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, چوکاټ,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_چوکاټ_TITLE_ماشوم,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(چوکاټ, "radioBox",
		(ArgList)عدم, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->پورته = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->پورته);

	str = XmStringCreateSimple(_("Down"));
	frdp->ښکته = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->ښکته);

	Xt_Manage_ماشوم(radio_box);
	Xt_Manage_ماشوم(چوکاټ);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, چوکاټ,
	    Xm_Nright_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ننوت_form,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_دننه_واټن, 4,
	    عدم);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNset, wword,
	    عدم);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_دننه_واټن, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_دننه_واټن, 4,
	    XmNset, mcase,
	    عدم);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_ماشوم(toggle_form);

    إذا (ثبت_متن != عدم)
	Xm_متن_ډګر_سېټ_String(frdp->what, (حرف *)ثبت_متن);
    vim_free(ثبت_متن);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   فراغ
gui_mch_پیدا_کړه_dialog(exarg_T *eap)
{
    إذا (!gui.دننه_use)
	أرجع;

    پیدا_کړه_replace_dialog_create(eap->arg, FALSE);
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (!gui.دننه_use)
	أرجع;

    پیدا_کړه_replace_dialog_create(eap->arg, TRUE);
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
    Shared_پیدا_کړه_Replace_2 *frdp;
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
	frdp = (do_replace) ? (&repl_widgets) : (&پیدا_کړه_widgets);
	إذا (frdp->dialog)
	{
	    Xt_Va_سېټ_ارزښتونه(frdp->what, XmNfontList, font_list, عدم);
	    إذا (do_replace)
		Xt_Va_سېټ_ارزښتونه(frdp->with, XmNfontList, font_list, عدم);
	}
    }

    XmFontListFree(font_list);
}
