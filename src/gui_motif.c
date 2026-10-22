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
#إذا (Xm_نسخہ >= 1002)
# include <Xm/RepType.h>
#جے_انت
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/XmP.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#جے_تعریف_شدہ HAVE_X11_XPM_H
# إذا defined(VMS)
#  include <xpm.h>
# وإلا
#  include <X11/xpm.h>
# endif
#وإلا
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#جے_انت
#جے_تعریف_شدہ HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#جے_انت

#ضمن "gui_xmebw.h"	// for our Enhanced Button Widget

#إذا defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#جے_انت

#تعریف_کرو MOTIF_POPUP

خارجي Widget vimShell;

ساكن Widget vimForm;
ساكن Widget متن_Area_Form;
Widget متن_Area;
#جے_تعریف_شدہ FEAT_TOOLBAR
ساكن Widget tool_Bar_چوکھٹا;
ساكن Widget toolBar;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
ساكن Widget	tab_سطر;
ساكن Widget	tab_سطر_menu = 0;
ساكن صحيح	showing_tabline = 0;
#جے_انت
#جے_تعریف_شدہ FEAT_MENU
# إذا (Xm_نسخہ >= 1002)
// remember the last set value for the tearoff item
ساكن صحيح tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
# endif
ساكن Widget menuBar;
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
ساكن فراغ reset_focus(فراغ);
#جے_انت

ساكن فراغ gui_motif_menu_colors(Widget id);
ساكن فراغ gui_motif_scroll_colors(Widget id);

#إذا (Xm_نسخہ >= 1002)
# define STRING_ٹیگ  Xm_FONTLIST_DEFAULT_ٹیگ
#وإلا
# define STRING_ٹیگ  XmSTRING_DEFAULT_CHARSET
#جے_انت

/*
 * Call-back routines.
 */

    ساكن فراغ
scroll_cb(Widget w بن_ورتیا, Xt_اشارہ_گر client_انکڑے_2, Xt_اشارہ_گر call_انکڑے)
{
    scrollbar_T *sb;
    لما	قدر;
    صحيح		dragging;

    sb = gui_لبھو_scrollbar((لما)client_انکڑے_2);

    قدر = ((XmScrollBarCallbackStruct *)call_انکڑے)->قدر;
    dragging = (((XmScrollBarCallbackStruct *)call_انکڑے)->reason ==
							      (صحيح)XmCR_DRAG);
    gui_drag_scrollbar(sb, قدر, dragging);
}

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    ساكن فراغ
tabline_cb(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2 بن_ورتیا,
    Xt_اشارہ_گر	call_انکڑے)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_انکڑے;
    إذا (nptr->reason != (صحيح)XmCR_NONE)
	send_tabline_event(nptr->ورقہ_نمبر);
}

    ساكن فراغ
tabline_button_cb(
    Widget	w,
    Xt_اشارہ_گر	client_انکڑے_2 بن_ورتیا,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    Xt_اشارہ_گر	cmd, tab_idx;

    Xt_Va_Get_اقدار(w, Xm_Nuser_انکڑے, &cmd, عدم);
    Xt_Va_Get_اقدار(tab_سطر_menu, Xm_Nuser_انکڑے, &tab_idx, عدم);

    send_tabline_menu_event((صحيح)(لما)tab_idx, (صحيح)(لما)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساكن فراغ
motif_tabline_timer_cb (
    Xt_اشارہ_گر		timed_باہرلا,
    XtIntervalId	*interval_id بن_ورتیا)
{
    *((صحيح *)timed_باہرلا) = TRUE;
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
    Dimension	چوڑائی, اچائی;

    tab_scroll_w = Xt_Name_منزل_Widget(tab_سطر, scroller_name);
    إذا (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_اقدار(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &چوڑائی, XmNheight, &اچائی, عدم);
	إذا (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    إذا (event->x >= pos_x && event->x <= pos_x + چوڑائی
		    && event->y >= pos_y && event->y <= pos_y + اچائی)
		// Clicked on the scroller
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
tabline_menu_cb(
    Widget	w,
    Xt_اشارہ_گر	گھیرا بن_ورتیا,
    XEvent	*e,
    Boolean	*continue_dispatch بن_ورتیا)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحيح				tab_idx = 0;
    WidgetList			children;
    Cardinal			عدد_Children;
    ساكن XtIntervalId		timer = (XtIntervalId)0;
    ساكن صحيح			timed_باہرلا = TRUE;

    event = (XButtonPressedEvent *)e;

    إذا (event->button == Button1)
    {
	إذا (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    أرجع;

	إذا (!timed_باہرلا)
	{
	    Xt_Remove_ویلا_باہرلا(timer);
	    timed_باہرلا = TRUE;

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
	    timer = Xt_App_جوڑو_ویلا_باہرلا(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_باہرلا);
	    timed_باہرلا = FALSE;
	}
	أرجع;
    }

    إذا (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_اقدار(tab_سطر_menu, Xm_Nuser_انکڑے, &tab_idx, عدم);
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
	tab_w = Xt_کھڑکی_منزل_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	إذا (tab_w != (Widget)0 && Xm_Is_ڈالو_Button(tab_w))
	    Xt_Va_Get_اقدار(tab_w, Xm_Npage_نمبر, &tab_idx, عدم);
    }

    Xt_Va_سیٹ_اقدار(tab_سطر_menu, Xm_Nuser_انکڑے, (Xt_اشارہ_گر)(لما)tab_idx, عدم);
    Xt_Va_Get_اقدار(tab_سطر_menu, XmNchildren, &children, XmNnumChildren,
		  &عدد_Children, عدم);
    XtManageChildren(children, عدد_Children);
    XmMenuPosition(tab_سطر_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_بچہ(tab_سطر_menu);
}

    ساكن فراغ
tabline_balloon_cb(BalloonEval *beval, صحيح state بن_ورتیا)
{
    صحيح		nr;
    tabpage_T	*tp;

    إذا (beval->نشانہ == (Widget)0)
	أرجع;

    Xt_Va_Get_اقدار(beval->نشانہ, Xm_Npage_نمبر, &nr, عدم);
    tp = لبھو_tabpage(nr);
    إذا (tp == عدم)
	أرجع;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#جے_انت

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
    بنانشانی حرف   label_type = (صحيح)XmSTRING;

    Xt_Va_Get_اقدار(_w, XmNlabelType, &label_type, (Xt_اشارہ_گر)0);

    إذا (XtIsSensitive(_w) || label_type != (صحيح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وإلا
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

	إذا (fs != 0)
	{
	    نقاب |= GCFont;
	    اقدار.font = fs->fid;
	}

	إذا (lw->primitive.چوٹی_shadow_pixmap != None
		&& lw->primitive.چوٹی_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    نقاب |= GCFillStyle | GCTile;
	    اقدار.fill_style = FillTiled;
	    اقدار.tile = lw->primitive.چوٹی_shadow_pixmap;
	}

	lw->label.متن_Rect.x += 1;
	lw->label.متن_Rect.y += 1;
	إذا (lw->label.acc_متن != 0)
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
	إذا (lw->label.acc_متن != 0)
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
    فراغ
gui_x11_create_widgets(فراغ)
{
#جے_تعریف_شدہ FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		دلیلاں[10];
    صحيح		n;
    XmString	xms;
#جے_انت

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
    gui.border_وتھ = gui.border_چوڑائی;

    /*
     * Install the tearOffModel resource converter.
     */
#إذا (Xm_نسخہ >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#جے_انت

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_سیٹ_اقدار(vimShell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_چوڑائی, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nmargin_اچائی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_menu_colors(vimForm);

#جے_تعریف_شدہ FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحيح ac = 0;

# إذا (Xm_نسخہ >= 1002)
	Xt_سیٹ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_سیٹ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_سیٹ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_سیٹ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_سیٹ_Arg(al[ac], Xm_Nright_وتھ, 0); ac++;
# endif
	Xt_سیٹ_Arg(al[ac], Xm_Nmargin_اچائی, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_بچہ(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#جے_انت

#جے_تعریف_شدہ FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_چوکھٹا = XtVaCreateWidget("toolBarFrame",
	xm_چوکھٹا_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_اچائی, 0,
	Xm_Nmargin_چوڑائی, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	عدم);
    gui_motif_menu_colors(tool_Bar_چوکھٹا);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_صف_کالم_Widget_Class, tool_Bar_چوکھٹا,
	XmNchildType, Xm_چوکھٹا_WORKAREA_بچہ,
	Xm_Nrow_کالم_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_اچائی, 0,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nadjust_آخری, True,
	عدم);
    gui_motif_menu_colors(toolBar);

#جے_انت

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbindingType, XmNONE); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nback_ورقہ_ناپ, XmNONE); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nback_ورقہ_نمبر, 0); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nback_ورقہ_Placement, Xm_چوٹی_دایاں); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNmajorTabSpacing, 0); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNshadowThickness, 0); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سطر = XmCreateNotebook(vimForm, "Vim tabline", دلیلاں, n);

    Xt_جوڑو_Callback(tab_سطر, Xm_Npage_بدلیا_Callback, (XtCallbackProc)tabline_cb,
			عدم);
    Xt_جوڑو_Event_Handler(tab_سطر, Button_Press_نقاب, False,
			(XtEventHandler)tabline_menu_cb, عدم);

    gui.tabline_اچائی = TABLINE_اچائی;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_منزل_Widget(tab_سطر, "MinorTabScrollerNext");
    Xt_Va_سیٹ_اقدار(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);
    scroller = Xt_Name_منزل_Widget(tab_سطر, "MinorTabScrollerPrevious");
    Xt_Va_سیٹ_اقدار(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);

    // Create the tabline popup menu
    tab_سطر_menu = XmCreatePopupMenu(tab_سطر, "tabline popup", عدم, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nuser_انکڑے, (Xt_اشارہ_گر)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_ٹیگ);
    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "Close", دلیلاں, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nuser_انکڑے, (Xt_اشارہ_گر)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_ٹیگ);
    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "New Tab", دلیلاں, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nuser_انکڑے, (Xt_اشارہ_گر)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_ٹیگ);
    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڈالو_Button(tab_سطر_menu, "Open tab...", دلیلاں, n);
    Xt_جوڑو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);
#جے_انت

    متن_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_چوڑائی, 0,
	Xm_Nmargin_اچائی, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_scroll_colors(متن_Area_Form);

    متن_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, متن_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.پٹھ_pixel,
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
	عدم);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(متن_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.وچ_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    فراغ
gui_x11_destroy_widgets(فراغ)
{
    متن_Area = عدم;
#جے_تعریف_شدہ FEAT_MENU
    menuBar = عدم;
#جے_انت
}

    فراغ
gui_mch_سیٹ_متن_area_pos(
    صحيح	    x بن_ورتیا,
    صحيح	    y بن_ورتیا,
    صحيح	    w بن_ورتیا,
    صحيح	    h بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#جے_انت
}

    فراغ
gui_x11_سیٹ_پٹھ_color(فراغ)
{
    إذا (متن_Area != عدم)
#إذا (Xm_نسخہ >= 1002)
	XmChangeColor(متن_Area, gui.پٹھ_pixel);
#وإلا
	Xt_Va_سیٹ_اقدار(متن_Area,
		  XmNbackground, gui.پٹھ_pixel,
		  عدم);
#جے_انت
}

/*
 * Manage dialog centered on pointer.
 */
    فراغ
manage_centered(Widget dialog_بچہ)
{
    Widget shell = Xt_ماپا(dialog_بچہ);
    کھڑکی جڑ, بچہ;
    بنانشانی صحيح نقاب;
    بنانشانی صحيح چوڑائی, اچائی, border_چوڑائی, ڈونگھائی;
    صحيح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_اقدار(shell, XmNmappedWhenManaged, &mappedWhenManaged, عدم);
    Xt_Va_سیٹ_اقدار(shell, XmNmappedWhenManaged, False, عدم);

    Xt_Manage_بچہ(dialog_بچہ);

    // Get the pointer position (x, y)
    XQuery_اشارہ_گر(XtDisplay(shell), Xt_کھڑکی(shell), &جڑ, &بچہ,
		  &x, &y, &win_x, &win_y, &نقاب);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_کھڑکی(shell), &جڑ, &win_x, &win_y,
		 &چوڑائی, &اچائی, &border_چوڑائی, &ڈونگھائی);
    چوڑائی += 2 * border_چوڑائی;
    اچائی += 2 * border_چوڑائی;
    x -= چوڑائی / 2;
    y -= اچائی / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->چوڑائی - چوڑائی;
    maxY = XtScreen(shell)->اچائی - اچائی;
    إذا (x < 0)
	x = 0;
    إذا (x > maxX)
	x = maxX;
    إذا (y < 0)
	y = 0;
    إذا (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_سیٹ_اقدار(shell, XmNx, x, XmNy, y, عدم);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_سیٹ_اقدار(shell, XmNmappedWhenManaged, mappedWhenManaged, عدم);
}

#إذا defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# إذا (Xm_نسخہ <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_ٹیگ);
# وإلا
    // Motif 1.2 method
    Xm_Font_List_اندراج font_list_اندراج;

    font_list_اندراج = Xm_Font_List_اندراج_Create(STRING_ٹیگ, XmFONT_IS_FONT,
					    (Xt_اشارہ_گر)font);
    font_list = Xm_Font_List_Append_اندراج(عدم, font_list_اندراج);
    Xm_Font_List_اندراج_Free(&font_list_اندراج);
# endif
    أرجع font_list;
}

# إذا ((Xm_نسخہ > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_سیٹ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_اندراج font_list_اندراج;

    font_list_اندراج = Xm_Font_List_اندراج_Create(STRING_ٹیگ,
					    XmFONT_IS_FONTSET,
					    (Xt_اشارہ_گر)*fontset);
    font_list = Xm_Font_List_Append_اندراج(عدم, font_list_اندراج);
    Xm_Font_List_اندراج_Free(&font_list_اندراج);
    أرجع font_list;
}
# endif

#جے_انت

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساكن فراغ gui_motif_جوڑو_actext(vimmenu_T *menu);
# إذا (Xm_نسخہ >= 1002)
ساكن فراغ toggle_tearoff(Widget wid);
ساكن فراغ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساكن فراغ submenu_change(vimmenu_T *mp, صحيح colors);

ساكن فراغ do_سیٹ_mnemonics(صحيح enable);
ساكن صحيح menu_enabled = TRUE;

    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag)
    {
	Xt_Manage_بچہ(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_ماپا(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_سیٹ_اقدار(Xt_ماپا(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar),
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
    }
    وإلا
    {
	Xt_Unmanage_بچہ(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_ماپا(toolBar)))
	{
	    Xt_Va_سیٹ_اقدار(Xt_ماپا(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar),
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    فراغ
gui_motif_سیٹ_mnemonics(صحيح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    إذا (!menu_enabled)
	enable = FALSE;
    do_سیٹ_mnemonics(enable);
}

    ساكن فراغ
do_سیٹ_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	إذا (menu->id != (Widget)0)
	    Xt_Va_سیٹ_اقدار(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    عدم);
}

    فراغ
gui_mch_جوڑو_menu(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*ماپا = menu->ماپا;

# ifdef MOTIF_POPUP
    إذا (menu_is_popup(menu->name))
    {
	Arg arg[2];
	صحيح n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  إذا (Xm_نسخہ <= 1002)
	إذا (mouse_model_popup())
#  endif
	{
	    إذا (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_سیٹ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    إذا (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_سیٹ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(متن_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	أرجع;
    }
# endif

    إذا (!menu_is_menubar(menu->name)
	    || (ماپا != عدم && ماپا->submenu_id == (Widget)0))
	أرجع;

    label = XmStringCreate((حرف *)menu->dname, STRING_ٹیگ);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (ماپا == عدم) ? menuBar : ماپا->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# إذا (Xm_نسخہ >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_اشاریہ, idx + (ماپا != عدم
			   && tearoff_val == (صحيح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    عدم);
    XmStringFree(label);

    إذا (menu->id == (Widget)0)		// failed
	أرجع;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    إذا (ماپا == عدم && menu->ترجیح >= 9999)
	Xt_Va_سیٹ_اقدار(menuBar,
		XmNmenuHelpWidget, menu->id,
		عدم);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_جوڑو_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	عدم);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_صف_کالم_Widget_Class, shell,
	Xm_Nrow_کالم_Type, XmMENU_PULLDOWN,
	عدم);
    gui_motif_menu_colors(menu->submenu_id);

    إذا (menu->submenu_id == (Widget)0)		// failed
	أرجع;

# إذا (Xm_نسخہ >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_سیٹ_اقدار(menu->id,
	XmNsubMenuId, menu->submenu_id,
	عدم);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    إذا (ماپا == عدم)
	gui_mch_compute_menu_اچائی(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساكن فراغ
gui_motif_جوڑو_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    إذا (menu->actext == عدم || menu->id == (Widget)0)
	أرجع;

    label = XmStringCreate((حرف *)menu->actext, STRING_ٹیگ);
    إذا (label == عدم)
	أرجع;
    Xt_Va_سیٹ_اقدار(menu->id, Xm_Naccelerator_متن, label, عدم);
    XmStringFree(label);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
# إذا (Xm_نسخہ >= 1002)
    إذا (enable)
	tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
    وإلا
	tearoff_val = (صحيح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(جڑ_menu);
# endif
}

# إذا (Xm_نسخہ >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ساكن فراغ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_سیٹ_اقدار(wid, XmNtearOffModel, tearoff_val, عدم);
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
	menu = menu->اگلا;
    }
}
# endif

    صحيح
gui_mch_متن_area_extra_اچائی(فراغ)
{
    Dimension	shadow_اچائی;

    Xt_Va_Get_اقدار(متن_Area_Form, XmNshadowThickness, &shadow_اچائی, عدم);
    أرجع shadow_اچائی;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    فراغ
gui_mch_compute_menu_اچائی(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساكن Dimension	اچائی = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    إذا (id != (Widget)0)
	Xt_Va_Get_اقدار(id, XmNheight, &اچائی, عدم);

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
	Xt_Manage_بچہ(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_اقدار(mp->id, XmNy, &y, عدم);
	    إذا (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_اقدار(menuBar,
	Xm_Nmargin_اچائی, &margin,
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
    gui.menu_اچائی = maxy + اچائی - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    إذا (gui.shell_created)
	Xt_Va_سیٹ_اقدار(menuBar, XmNheight, gui.menu_اچائی, عدم);
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
check_xpm(char_u *رستہ)
{
    Xpm_گن attrs;
    صحيح		حالت;
    Pixmap	نقاب;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    حالت = Xpm_Read_File_منزل_Pixmap(gui.dpy,
	    جڑ_کھڑکی_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)رستہ, &map, &نقاب, &attrs);
    Xpm_Free_گن(&attrs);

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
	gui_لبھو_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	إذا (res == FAIL && gui_لبھو_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	إذا (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    أرجع tb_blank_xpm;
	}
    }

    إذا (menu->icon_builtin || gui_لبھو_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	إذا (menu->iconidx >= 0 && menu->iconidx
	       < (صحيح)(حجم(built_وچ_pixmaps) / حجم(built_وچ_pixmaps[0])))
	    xpm = built_وچ_pixmaps[menu->iconidx];
	وإلا
	    xpm = tb_blank_xpm;
    }

    أرجع xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساكن صحيح
جوڑو_pixmap_دلیلاں(vimmenu_T *menu, Arg *دلیلاں, صحيح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    إذا (menu->xpm == عدم)
    {
	Xt_سیٹ_Arg(دلیلاں[n], XmNlabelType, XmSTRING); n++;
    }
    وإلا
    {
	إذا (menu->xpm_fname != عدم)
	{
	    Xt_سیٹ_Arg(دلیلاں[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_سیٹ_Arg(دلیلاں[n], Xm_Npixmap_انکڑے, menu->xpm); n++;
	Xt_سیٹ_Arg(دلیلاں[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    أرجع n;
}
# endif // FEAT_TOOLBAR

    فراغ
gui_mch_جوڑو_menu_شے(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    vimmenu_T	*ماپا = menu->ماپا;

# إذا (Xm_نسخہ <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    إذا (menu_is_بچہ_دا_popup(menu) && !mouse_model_popup())
	أرجع;
# endif

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(ماپا->name))
    {
	WidgetClass	type;
	XmString	xms = عدم;    // fallback label if pixmap not found
	صحيح		n;
	Arg		دلیلاں[18];

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
	    Xt_سیٹ_Arg(دلیلاں[n], XmNwidth, wid); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nmin_چوڑائی, wid); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNorientation, XmVERTICAL); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNseparatorType, Xm_SHADOW_ETCHED_وچ); n++;
	}
	وإلا
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_سیٹ_Arg(دلیلاں[n], XmNhighlightThickness, 1); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNhighlightOnEnter, True); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nmargin_چوڑائی, 0); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nmargin_اچائی, 0); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_ٹیگ);
	    Xt_سیٹ_Arg(دلیلاں[n], XmNlabelString, xms); n++;

	    n = جوڑو_pixmap_دلیلاں(menu, دلیلاں, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_سیٹ_Arg(دلیلاں[n], Xm_Nposition_اشاریہ, idx); n++;
	إذا (menu->id == عدم)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, دلیلاں, n);
	    إذا (menu->id != عدم && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_جوڑو_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وإلا
	    Xt_سیٹ_اقدار(menu->id, دلیلاں, n);
	إذا (xms != عدم)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_سیٹ_tip(menu);
#  endif

	menu->ماپا = ماپا;
	menu->submenu_id = عدم;
	// When adding first item to toolbar it might have to be enabled .
	إذا (!XtIsManaged(Xt_ماپا(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_اچائی = gui_mch_compute_toolbar_اچائی();
	أرجع;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    إذا (ماپا->submenu_id == (Widget)0)
	أرجع;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    إذا (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, ماپا->submenu_id,
# إذا (Xm_نسخہ >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_اشاریہ, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		عدم);
	gui_motif_menu_colors(menu->id);
	أرجع;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_ٹیگ);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ڈالو_Button_Widget_Class, ماپا->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# إذا (Xm_نسخہ >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_اشاریہ, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	عدم);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    إذا (menu->id != (Widget)0)
    {
	Xt_جوڑو_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_اشارہ_گر)menu);
	// add accelerator text
	gui_motif_جوڑو_actext(menu);
    }
}

# إذا (Xm_نسخہ <= 1002)
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
    إذا (!gui.وچ_use)
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
		  gui_mch_جوڑو_menu(menu, idx);
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
      وإلا إذا (menu_is_بچہ_دا_popup(menu))
      {
	  إذا (mouse_model_popup())
	      gui_mch_جوڑو_menu_شے(menu, idx);
	  وإلا
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->اگلا;
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
    gui_motif_menu_colors(tool_Bar_چوکھٹا);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(جڑ_menu, TRUE);
}

    فراغ
gui_mch_new_menu_font(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    submenu_change(جڑ_menu, FALSE);
    {
	Dimension   اچائی;
	Position w, h;

	Xt_Va_Get_اقدار(menuBar, XmNheight, &اچائی, عدم);
	gui.menu_اچائی = اچائی;

	Xt_Va_Get_اقدار(vimShell, XtNwidth, &w, XtNheight, &h, عدم);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_حالت_area_اچائی()
# endif
		     );
    }
    gui_سیٹ_shellsize(FALSE, TRUE, RESIZE_VERT);
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

    menu = gui_لبھو_menu((char_u *)"ToolBar");
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

    toolbar = gui_لبھو_menu((char_u *)"ToolBar");
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

    لكل (mp = menu; mp != عدم; mp = mp->اگلا)
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
		    Arg		دلیلاں[18];

		    n = جوڑو_pixmap_دلیلاں(mp, دلیلاں, n);
		    Xt_سیٹ_اقدار(mp->id, دلیلاں, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg دلیلاں[2];

		    دلیلاں[0].name = XmNbackground;
		    دلیلاں[0].قدر = gui.tooltip_bg_pixel;
		    دلیلاں[1].name = XmNforeground;
		    دلیلاں[1].قدر = gui.tooltip_fg_pixel;
		    Xt_سیٹ_اقدار(mp->tip->balloonLabel, &دلیلاں[0], Xt_نمبر(دلیلاں));
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
		    Arg دلیلاں[1];

		    دلیلاں[0].name = XmNfontList;
		    دلیلاں[0].قدر = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_سیٹ_اقدار(mp->tip->balloonLabel, &دلیلاں[0], Xt_نمبر(دلیلاں));
		}
# endif
	    }
	}

	إذا (mp->children != عدم)
	{
# إذا (Xm_نسخہ >= 1002)
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

    Widget	    ماپا;

    ماپا = Xt_ماپا(menu->id);
# إذا defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    إذا (ماپا == toolBar && menu->tip != عدم)
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
    إذا (ماپا == menuBar)
	gui_mch_compute_menu_اچائی((Widget)0);
# ifdef FEAT_TOOLBAR
    وإلا إذا (ماپا == toolBar)
    {
	Cardinal    عدد_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_اقدار(toolBar, XmNnumChildren, &عدد_children, عدم);
	إذا (عدد_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وإلا
	    gui.toolbar_اچائی = gui_mch_compute_toolbar_اچائی();
    }
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu بن_ورتیا)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_آخری_mouse_event());
    Xt_Manage_بچہ(menu->submenu_id);
# endif
}

#جے_انت // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    إذا (!gui.وچ_use)
	أرجع;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#جے_تعریف_شدہ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#جے_انت
}


/*
 * Scrollbar stuff.
 */

    فراغ
gui_mch_سیٹ_scrollbar_thumb(
    scrollbar_T *sb,
    لما	val,
    لما	ناپ,
    لما	max)
{
    إذا (sb->id != (Widget)0)
	Xt_Va_سیٹ_اقدار(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_ناپ, ناپ,
		  XmNpageIncrement, (ناپ > 2 ? ناپ - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  عدم);
}

    فراغ
gui_mch_سیٹ_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    إذا (sb->id == (Widget)0)
	أرجع;

    إذا (sb->type == SBAR_بایاں || sb->type == SBAR_دایاں)
    {
	إذا (y == 0)
	    h -= gui.border_وتھ;
	وإلا
	    y -= gui.border_وتھ;
	Xt_Va_سیٹ_اقدار(sb->id,
		Xm_Ntop_وتھ, y,
		Xm_Nbottom_وتھ, -y - h,
		XmNwidth, w,
		عدم);
    }
    وإلا
	Xt_Va_سیٹ_اقدار(sb->id,
		Xm_Ntop_وتھ, y,
		Xm_Nleft_وتھ, x,
		Xm_Nright_وتھ, gui.which_scrollbars[SBAR_دایاں]
						     ? gui.scrollbar_چوڑائی : 0,
		XmNheight, h,
		عدم);
    Xt_Manage_بچہ(sb->id);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_اقدار(متن_Area, XtNwidth, &tw, XtNx, &tx, عدم);
    Xt_Va_Get_اقدار(vimShell, XtNwidth, &ww, عدم);
    xpad = ww - tw - tx - gui.scrollbar_چوڑائی;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_اقدار(متن_Area, XtNheight, &th, XtNy, &ty, عدم);
    Xt_Va_Get_اقدار(vimShell, XtNheight, &wh, عدم);
    ypad = wh - th - ty - gui.scrollbar_اچائی;
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    Arg		دلیلاں[16];
    صحيح		n;

    إذا (sb->id == (Widget)0)
	أرجع;

    n = 0;
    إذا (flag)
    {
	بدل (sb->type)
	{
	    حالة SBAR_بایاں:
		Xt_سیٹ_Arg(دلیلاں[n], Xm_Nleft_وتھ, gui.scrollbar_چوڑائی); n++;
		اكسر;

	    حالة SBAR_دایاں:
		Xt_سیٹ_Arg(دلیلاں[n], Xm_Nright_وتھ, gui.scrollbar_چوڑائی); n++;
		اكسر;

	    حالة SBAR_BOTTOM:
		Xt_سیٹ_Arg(دلیلاں[n], Xm_Nbottom_وتھ, gui.scrollbar_اچائی);n++;
		اكسر;
	}
	Xt_سیٹ_اقدار(متن_Area, دلیلاں, n);
	Xt_Manage_بچہ(sb->id);
    }
    وإلا
    {
	إذا (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    بدل (sb->type)
	    {
		حالة SBAR_بایاں:
		    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nleft_وتھ, 0); n++;
		    اكسر;

		حالة SBAR_دایاں:
		    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nright_وتھ, 0); n++;
		    اكسر;

		حالة SBAR_BOTTOM:
		    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nbottom_وتھ, 0);n++;
		    اكسر;
	    }
	    Xt_سیٹ_اقدار(متن_Area, دلیلاں, n);
	}
	Xt_Unmanage_بچہ(sb->id);
    }
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		دلیلاں[16];
    صحيح		n = 0;

    Xt_سیٹ_Arg(دلیلاں[n], XmNminimum, 0); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    بدل (sb->type)
    {
	حالة SBAR_بایاں:
	    Xt_سیٹ_Arg(دلیلاں[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNwidth, gui.scrollbar_چوڑائی); n++;
	    اكسر;

	حالة SBAR_دایاں:
	    Xt_سیٹ_Arg(دلیلاں[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNwidth, gui.scrollbar_چوڑائی); n++;
	    اكسر;

	حالة SBAR_BOTTOM:
	    Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNheight, gui.scrollbar_اچائی); n++;
	    اكسر;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, متن_Area_Form, دلیلاں, n);
    إذا (sb->id == (Widget)0)
	أرجع;

    gui_mch_سیٹ_scrollbar_colors(sb);
    Xt_جوڑو_Callback(sb->id, Xm_Nvalue_بدلیا_Callback,
	    scroll_cb, (Xt_اشارہ_گر)sb->ident);
    Xt_جوڑو_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_اشارہ_گر)sb->ident);
    Xt_جوڑو_Event_Handler(sb->id, کلید_Press_نقاب, FALSE, gui_x11_کلید_کامیابی_cb,
	    (Xt_اشارہ_گر)0);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    فراغ
gui_mch_سیٹ_scrollbar_colors(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
    {
	إذا (gui.scroll_bg_pixel != INVALCOLOR)
	{
#إذا (Xm_نسخہ>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#جے_انت

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_سیٹ_اقدار(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    عدم);
	}

	إذا (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_سیٹ_اقدار(sb->id,
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

    کھڑکی
gui_x11_get_wid(فراغ)
{
    أرجع(Xt_کھڑکی(متن_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساكن فراغ
do_mnemonic(Widget w, بنانشانی صحيح keycode)
{
    WidgetList	    children;
    صحيح		    عدد_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    user_انکڑے_2;
    بنانشانی حرف   صف_خانہ_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_صف_کالم_Widget_Class)
	{
	    Xt_Va_Get_اقدار(w, Xm_Nrow_کالم_Type, &صف_خانہ_Type, عدم);
	    isMenu = (صف_خانہ_Type != (بنانشانی حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_اقدار(w, XmNchildren, &children, XmNnumChildren,
			  &عدد_Children, عدم);
	    لكل (i = 0; i < عدد_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وإلا
    {
	Xt_Va_Get_اقدار(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    إذا (XKeysym_منزل_Keycode(XtDisplay(Xt_ماپا(w)),
				       XString_منزل_Keysym(mneString)) == keycode)
	    {
		إذا (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_اقدار(w, Xm_Nuser_انکڑے, &user_انکڑے_2, عدم);
		    إذا (user_انکڑے_2 != عدم && XtIsWidget(user_انکڑے_2))
			XmProcessTraversal(user_انکڑے_2, XmTRAVERSE_CURRENT);
		}
		وإلا
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
	Xt_اشارہ_گر   call_انکڑے بن_ورتیا,
	XKeyEvent   *event,
	Boolean	    *b بن_ورتیا)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ساكن فراغ
جوڑو_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحيح		    عدد_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    بنانشانی حرف   صف_خانہ_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_صف_کالم_Widget_Class)
	{
	    Xt_Va_Get_اقدار(w, Xm_Nrow_کالم_Type, &صف_خانہ_Type, عدم);
	    isMenu = (صف_خانہ_Type != (بنانشانی حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_اقدار(w, XmNchildren, &children, XmNnumChildren,
							  &عدد_Children, عدم);
	    لكل (i = 0; i < عدد_Children; i++)
		جوڑو_mnemonic_grabs(dialog, children[i]);
	}
    }
    وإلا
    {
	Xt_Va_Get_اقدار(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_کلید(dialog, XKeysym_منزل_Keycode(XtDisplay(dialog),
						  XString_منزل_Keysym(mneString)),
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
    ساكن فراغ
activate_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_جوڑو_Event_Handler(dialog, کلید_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارہ_گر) عدم);
    جوڑو_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساكن فراغ
suppress_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_Ungrab_کلید(dialog, Any_کلید, Mod1_نقاب);
    XtRemoveEventHandler(dialog, کلید_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارہ_گر) عدم);
}

#إذا defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساكن فراغ
سیٹ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    إذا (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_سیٹ *)&gui.fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    وإلا
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    وإلا
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
}
#جے_انت

#إذا defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعريفنوع بنية dialog_callback_arg
{
    حرف *  دلیلاں;   // not used right now
    صحيح	    id;
} dcbarg_T;

ساكن Widget dialog_wgt;
ساكن حرف *browse_fname = عدم;
ساكن Xm_String_Char_سیٹ charset = (Xm_String_Char_سیٹ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساكن فراغ DialogCancelCB(Widget, Xt_اشارہ_گر, Xt_اشارہ_گر);
ساكن فراغ DialogAcceptCB(Widget, Xt_اشارہ_گر, Xt_اشارہ_گر);

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
سیٹ_predefined_label(Widget ماپا, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *اگلا;
    کلید_Sym	mnemonic = NUL;

    w = Xt_Name_منزل_Widget(ماپا, name);

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)new_label);
    إذا (p == عدم)
	أرجع;
    لكل (اگلا = p; *اگلا; ++اگلا)
    {
	إذا (*اگلا == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(اگلا);

	    إذا (len > 0)
	    {
		mch_memmove(اگلا, اگلا + 1, len);
		mnemonic = اگلا[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_ٹیگ);
    vim_free(p);

    إذا (str != عدم)
    {
	Xt_Va_سیٹ_اقدار(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
سیٹ_predefined_fontlist(Widget ماپا, String name)
{
    Widget w;
    w = Xt_Name_منزل_Widget(ماپا, name);

    إذا (!w)
	أرجع;

    سیٹ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحيح		saving بن_ورتیا,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext بن_ورتیا,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*نمونہ;
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
	جوڑو_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    إذا (filter == عدم)
	نمونہ = (char_u *)"";
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
	نمونہ = vim_strnsave(s, p - s);
	tofree = نمونہ;
	إذا (نمونہ == عدم)
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
	عدم);

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
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, عدم);

    Xt_جوڑو_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_اشارہ_گر)0);
    Xt_جوڑو_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_اشارہ_گر)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_بچہ(Xm_File_Selection_Box_Get_بچہ(dialog_wgt,
					(بنانشانی حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    افعل
    {
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(dialog_wgt),
	    (Xt_داخلہ_نقاب)XtIMAll);
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
    Widget	w بن_ورتیا,		//  widget id
    Xt_اشارہ_گر	client_انکڑے_2 بن_ورتیا,	//  data from application
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)	//  data from widget class
{
    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    Xt_Unmanage_بچہ(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساكن فراغ
DialogAcceptCB(
    Widget	w بن_ورتیا,		//  widget id
    Xt_اشارہ_گر	client_انکڑے_2 بن_ورتیا,	//  data from application
    Xt_اشارہ_گر	call_انکڑے)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_انکڑے;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->قدر, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_بچہ(dialog_wgt);
}

#جے_انت // FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

ساكن صحيح	dialog_حالت;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساكن فراغ
keyhit_callback(
    Widget		w,
    Xt_اشارہ_گر		client_انکڑے_2 بن_ورتیا,
    XEvent		*event,
    Boolean		*cont بن_ورتیا)
{
    حرف	buf[2];
    کلید_Sym	کلید_sym;

    إذا (XLookupString(&(event->xkey), buf, 2, &کلید_sym, عدم) == 1)
    {
	إذا (*buf == CAR)
	    dialog_حالت = 1;
	وإلا إذا (*buf == ESC)
	    dialog_حالت = 2;
    }
    إذا ((کلید_sym == XK_بایاں || کلید_sym == XK_دایاں)
	    && !(event->xkey.state & کھسکاؤ_نقاب))
	Xm_متن_میدان_Clear_Selection(w, Xt_آخری_Timestamp_Processed(gui.dpy));
}

    ساكن فراغ
butproc(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    dialog_حالت = (صحيح)(لما)client_انکڑے_2 + 1;
}

# ifdef HAVE_XPM

    ساكن Widget
create_pixmap_label(
    Widget	ماپا,
    String	name,
    حرف	**انکڑے,
    ArgList	دلیلاں,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحيح			ڈونگھائی;
    Pixmap		pixmap = 0;
    Xpm_گن	attr;
    Boolean		rs;
    Xpm_Color_علامت	color[5] =
    {
	{"none", عدم, 0},
	{"iconColor1", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"selectColor", عدم, 0}
    };

    label = XmCreateLabelGadget(ماپا, name, دلیلاں, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_دا_Object(label);
    scr = Xt_Screen_دا_Object(label);
    Xt_Va_Get_اقدار(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_ماپا(label),
		  XmNdepth, &ڈونگھائی,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  عدم);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_ڈونگھائی;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.ڈونگھائی = ڈونگھائی;
    Xpm_Create_Pixmap_From_انکڑے(dsp, جڑ_کھڑکی_دا_Screen(scr),
		    انکڑے, &pixmap, عدم, &attr);

    Xt_Va_Get_اقدار(label, Xm_Nrecompute_ناپ, &rs, عدم);
    Xt_Va_سیٹ_اقدار(label, Xm_Nrecompute_ناپ, True, عدم);
    Xt_Va_سیٹ_اقدار(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    عدم);
    Xt_Va_سیٹ_اقدار(label, Xm_Nrecompute_ناپ, rs, عدم);

    أرجع label;
}
# endif

    صحيح
gui_mch_dialog(
    صحيح		type بن_ورتیا,
    char_u	*title,
    char_u	*سنیہا,
    char_u	*button_ناں,
    صحيح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحيح		ex_cmd بن_ورتیا)
{
    char_u		*buts;
    char_u		*p, *اگلا;
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
    Arg			دلیلاں[6];
# ifdef HAVE_XPM
    حرف		**icon_انکڑے_2 = عدم;
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
    Xt_Va_سیٹ_اقدار(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    عدم);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_ناں);
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

	لكل (اگلا = p; *اگلا; ++اگلا)
	{
	    إذا (*اگلا == DLG_HOTKEY_CHAR)
	    {
		صحيح len = STRLEN(اگلا);

		إذا (len > 0)
		{
		    mch_memmove(اگلا, اگلا + 1, len);
		    mnemonic = اگلا[0];
		}
	    }
	    إذا (*اگلا == DLG_BUTTON_SEP)
	    {
		*اگلا++ = NUL;
		اكسر;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_ٹیگ);
	إذا (label == عدم)
	    اكسر;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ڈالو_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_وتھ, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		عدم);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	إذا (butcount > 0)
	{
	    إذا (vertical)
		Xt_Va_سیٹ_اقدار(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			عدم);
	    وإلا
	    {
		إذا (*اگلا == NUL)
		{
		    Xt_Va_سیٹ_اقدار(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_وتھ, 4,
			    عدم);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_وتھ, 4,
			    عدم);
		    Xt_Manage_بچہ(sep_form);
		}
		وإلا
		{
		    Xt_Va_سیٹ_اقدار(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    عدم);
		}
	    }
	}
	وإلا إذا (!vertical)
	{
	    إذا (*اگلا == NUL)
	    {
		Xt_Va_سیٹ_اقدار(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_وتھ, 4,
			عدم);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_وتھ, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_وتھ, 4,
			عدم);
		Xt_Manage_بچہ(sep_form);
	    }
	    وإلا
		Xt_Va_سیٹ_اقدار(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_وتھ, 4,
			عدم);
	}

	Xt_جوڑو_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_اشارہ_گر)(لما)butcount);
	p = اگلا;
    }
    vim_free(buts);

    separator = (Widget) 0;
    إذا (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_سیٹ_Arg(دلیلاں[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_سیٹ_Arg(دلیلاں[n], XmNbottomWidget, buttons[0]); n++;
	Xt_سیٹ_Arg(دلیلاں[n], Xm_Nbottom_وتھ, 4); n++;
	Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", دلیلاں, n);
	Xt_Manage_بچہ(separator);
    }

    إذا (textfield != عدم)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_متن_میدان_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	إذا (butcount > 0)
	    Xt_Va_سیٹ_اقدار(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سیٹ_اقدار(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	سیٹ_fontlist(dialogtextfield);
	Xm_متن_میدان_سیٹ_String(dialogtextfield, (حرف *)textfield);
	Xt_Manage_بچہ(dialogtextfield);
	Xt_جوڑو_Event_Handler(dialogtextfield, کلید_Press_نقاب, False,
			    (XtEventHandler)keyhit_callback, (Xt_اشارہ_گر)عدم);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    عدم);
    Xt_Manage_بچہ(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    بدل (type)
    {
	حالة VIM_GENERIC:
	    icon_انکڑے_2 = generic_xpm;
	    اكسر;
	حالة VIM_ERROR:
	    icon_انکڑے_2 = error_xpm;
	    اكسر;
	حالة VIM_WARNING:
	    icon_انکڑے_2 = alert_xpm;
	    اكسر;
	حالة VIM_INFO:
	    icon_انکڑے_2 = info_xpm;
	    اكسر;
	حالة VIM_QUESTION:
	    icon_انکڑے_2 = quest_xpm;
	    اكسر;
	افتراضي:
	    icon_انکڑے_2 = generic_xpm;
    }

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Ntop_وتھ, 8); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nbottom_وتھ, 8); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nleft_وتھ, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_انکڑے_2, دلیلاں, n);
    Xt_Manage_بچہ(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)سنیہا, STRING_ٹیگ);
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
				Xm_Ntop_وتھ, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# وإلا
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_وتھ, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_وتھ, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_وتھ, 8,
				عدم);
    XmStringFree(label);
    سیٹ_fontlist(w);

    إذا (textfield != عدم)
    {
	Xt_Va_سیٹ_اقدار(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		عدم);
    }
    وإلا
    {
	إذا (butcount > 0)
	    Xt_Va_سیٹ_اقدار(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سیٹ_اقدار(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
    }

    إذا (dfltbutton < 1)
	dfltbutton = 1;
    إذا (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_سیٹ_اقدار(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], عدم);
    إذا (textfield != عدم)
	Xt_Va_سیٹ_اقدار(dialogform, XmNinitialFocus, dialogtextfield, عدم);
    وإلا
	Xt_Va_سیٹ_اقدار(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									عدم);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    إذا (textfield != عدم && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_متن_میدان_سیٹ_Selection(dialogtextfield,
			 (Xm_متن_Position)0, (Xm_متن_Position)STRLEN(textfield),
					   Xt_آخری_Timestamp_Processed(gui.dpy));
	Xm_متن_میدان_سیٹ_Cursor_Position(dialogtextfield,
					   (Xm_متن_Position)STRLEN(textfield));
    }

    app = Xt_Widget_منزل_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_حالت = -1;
    لكل (;;)
    {
	XtAppProcessEvent(app, (Xt_داخلہ_نقاب)XtIMAll);
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
#جے_انت // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    Cardinal	عدد_Children;	    // how many children toolBar has

    إذا (toolBar == (Widget)0)
	أرجع;
    Xt_Va_Get_اقدار(toolBar, XmNnumChildren, &عدد_Children, عدم);
    إذا (showit && عدد_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_اقدار(toolBar, XmNchildren, &children, عدم);
	{
	    فراغ    (*action)(BalloonEval *);
	    صحيح	    متن = 0;

	    إذا (لڑی_کھوج((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وإلا
		action = &gui_mch_disable_beval_area;
	    إذا (لڑی_کھوج((ثابت حرف *)p_toolbar, "text"))
		متن = 1;
	    وإلا إذا (لڑی_کھوج((ثابت حرف *)p_toolbar, "icons"))
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
		    لكل (cur = toolbar->children; cur; cur = cur->اگلا)
		    {
			Arg	    دلیلاں[1];
			صحيح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			إذا (cur->tip != عدم)
			    (*action)(cur->tip);
			إذا (!menu_is_separator(cur->name))
			{
			    إذا (متن == 1 || cur->xpm == عدم)
			    {
				Xt_سیٹ_Arg(دلیلاں[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    إذا (cur->id != عدم)
			    {
				Xt_Unmanage_بچہ(cur->id);
				Xt_سیٹ_اقدار(cur->id, دلیلاں, n);
				Xt_Manage_بچہ(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_اچائی = gui_mch_compute_toolbar_اچائی();
	Xt_Manage_بچہ(Xt_ماپا(toolBar));
# ifdef FEAT_GUI_TABLINE
	إذا (showing_tabline)
	{
	    Xt_Va_سیٹ_اقدار(tab_سطر,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ماپا(toolBar),
			  عدم);
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
# endif
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ماپا(toolBar),
			  عدم);
	إذا (XtIsManaged(menuBar))
	    Xt_Va_سیٹ_اقدار(Xt_ماپا(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    عدم);
	وإلا
	    Xt_Va_سیٹ_اقدار(Xt_ماپا(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    عدم);
    }
    وإلا
    {
	gui.toolbar_اچائی = 0;
	إذا (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
	وإلا
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}

	Xt_Unmanage_بچہ(Xt_ماپا(toolBar));
    }
    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_VERT);
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
gui_mch_compute_toolbar_اچائی(فراغ)
{
    Dimension	borders;
    Dimension	اچائی;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	عدد_Children;	    // how many children toolBar has
    صحيح		i;

    borders = 0;
    اچائی = 0;
    إذا (toolBar != (Widget)0 && tool_Bar_چوکھٹا != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_اقدار(tool_Bar_چوکھٹا,
		XmNshadowThickness, &fst,
		Xm_Nmargin_اچائی, &fmh,
		عدم);
	borders += fst + fmh;
	Xt_Va_Get_اقدار(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_اچائی, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &عدد_Children, عدم);
	borders += tst + tmh;
	لكل (i = 0; i < (صحيح)عدد_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_اقدار(children[i], XmNheight, &whgt, عدم);
	    إذا (اچائی < whgt)
		اچائی = whgt;
	}
    }
# ifdef LESSTIF_نسخہ
    // Hack: When starting up we get wrong dimensions.
    إذا (اچائی < 10)
	اچائی = 24;
# endif

    أرجع (صحيح)(اچائی + (borders << 1));
}

    فراغ
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
	    عدم);
}
#جے_انت

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
	    Xt_Manage_بچہ(tab_سطر);
	    Xt_Unmanage_بچہ(Xt_Name_منزل_Widget(tab_سطر, "PageScroller"));
	    Xt_Unmanage_بچہ(Xt_Name_منزل_Widget(tab_سطر, "MinorTabScrollerNext"));
	    Xt_Unmanage_بچہ(Xt_Name_منزل_Widget(tab_سطر,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_ماپا(toolBar)))
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	    Xt_Va_سیٹ_اقدار(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
	{
	    Xt_Unmanage_بچہ(tab_سطر);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_ماپا(toolBar)))
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ماپا(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سیٹ_اقدار(متن_Area_Form,
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
    Arg			دلیلاں[10];
    صحيح			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_ورقہ_Info	ورقہ_info;
    Xm_Notebook_ورقہ_حالت ورقہ_حالت;
    صحيح			آخری_ورقہ, tab_گنتی;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = پہلا_tabpage; tp != عدم; tp = tp->tp_اگلا, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	ورقہ_حالت = Xm_Notebook_Get_ورقہ_Info(tab_سطر, nr, &ورقہ_info);
	إذا (ورقہ_حالت == Xm_ورقہ_INVALID
		|| ورقہ_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nnotebook_بچہ_Type, XmMAJOR_TAB); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNtraversalOn, False); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNhighlightThickness, 1); n++;
	    Xt_سیٹ_Arg(دلیلاں[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ڈالو_Button(tab_سطر, "-Empty-", دلیلاں, n);
	    Xt_Manage_بچہ(tab);
	    beval = gui_mch_create_beval_area(tab, عدم, tabline_balloon_cb,
									عدم);
	    Xt_Va_سیٹ_اقدار(tab, Xm_Nuser_انکڑے, beval, عدم);
	}
	وإلا
	    tab = ورقہ_info.major_tab_widget;

	Xt_Va_سیٹ_اقدار(tab, Xm_Npage_نمبر, nr, عدم);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_اقدار(tab, XmNlabelString, &label_str, عدم);
	إذا (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    إذا (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_سیٹ_اقدار(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, عدم);
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

    Xt_Va_Get_اقدار(tab_سطر, Xm_Nlast_ورقہ_نمبر, &آخری_ورقہ, عدم);

    // Remove any old labels.
    طالما (nr <= آخری_ورقہ)
    {
	إذا (Xm_Notebook_Get_ورقہ_Info(tab_سطر, nr, &ورقہ_info) != Xm_ورقہ_INVALID
	    && ورقہ_info.ورقہ_نمبر == nr
	    && ورقہ_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_اقدار(ورقہ_info.major_tab_widget, Xm_Nuser_انکڑے, &beval, عدم);
	    إذا (beval != عدم)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_بچہ(ورقہ_info.major_tab_widget);
	    XtDestroyWidget(ورقہ_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Nlast_ورقہ_نمبر, tab_گنتی, عدم);

    Xt_Va_Get_اقدار(tab_سطر, Xm_Ncurrent_ورقہ_نمبر, &currentpage, عدم);
    إذا (currentpage != curtabidx)
	Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Ncurrent_ورقہ_نمبر, curtabidx, عدم);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سیٹ_curtab(صحيح nr)
{
    صحيح		currentpage;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    Xt_Va_Get_اقدار(tab_سطر, Xm_Ncurrent_ورقہ_نمبر, &currentpage, عدم);
    إذا (currentpage != nr)
	Xt_Va_سیٹ_اقدار(tab_سطر, Xm_Ncurrent_ورقہ_نمبر, nr, عدم);
}
#جے_انت

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ساكن فراغ
gui_motif_menu_colors(Widget id)
{
    إذا (gui.menu_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخہ >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#وإلا
	Xt_Va_سیٹ_اقدار(id, XmNbackground, gui.menu_bg_pixel, عدم);
#جے_انت
    إذا (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(id, XmNforeground, gui.menu_fg_pixel, عدم);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ساكن فراغ
gui_motif_scroll_colors(Widget id)
{
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخہ >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#وإلا
	Xt_Va_سیٹ_اقدار(id, XmNbackground, gui.scroll_bg_pixel, عدم);
#جے_انت
    إذا (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_سیٹ_اقدار(id, XmNforeground, gui.scroll_fg_pixel, عدم);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    فراغ
gui_motif_menu_fontlist(Widget id بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_MENU
# ifdef FONTSET_ALWAYS
    إذا (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_سیٹ *)&gui.menu_fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    وإلا
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_بچہ(id);
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_بچہ(id);
	    }
	    وإلا
		Xt_Va_سیٹ_اقدار(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
#جے_انت
}


/*
 * We don't create it twice for the sake of speed.
 */

تعريفنوع بنية Shared_لبھو_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget اتے;		// search direction 'Up' radio button
    Widget تھلے;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget لبھو;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_لبھو_Replace_2;

ساكن Shared_لبھو_Replace_2 لبھو_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن Shared_لبھو_Replace_2 repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن فراغ
لبھو_replace_destroy_callback(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    Shared_لبھو_Replace_2 *cd = (Shared_لبھو_Replace_2 *)client_انکڑے_2;

    إذا (cd != عدم)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساكن فراغ
لبھو_replace_dismiss_callback(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    Shared_لبھو_Replace_2 *cd = (Shared_لبھو_Replace_2 *)client_انکڑے_2;

    إذا (cd != عدم)
	Xt_Unmanage_بچہ(cd->dialog);
}

    ساكن فراغ
اندراج_activate_callback(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    XmProcessTraversal((Widget)client_انکڑے_2, XmTRAVERSE_CURRENT);
}

    ساكن فراغ
لبھو_replace_callback(
    Widget	w بن_ورتیا,
    Xt_اشارہ_گر	client_انکڑے_2,
    Xt_اشارہ_گر	call_انکڑے بن_ورتیا)
{
    long_u	flags = (long_u)client_انکڑے_2;
    حرف	*لبھو_متن, *repl_متن;
    Boolean	direction_تھلے = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_لبھو_Replace_2 *sfr;

    إذا (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = خالی_تھاواں_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	أرجع;
    }

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &لبھو_widgets;
    }
    وإلا
    {
	repl_متن = Xm_متن_میدان_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    لبھو_متن = Xm_متن_میدان_Get_String(sfr->what);
    Xt_Va_Get_اقدار(sfr->تھلے, XmNset, &direction_تھلے, عدم);
    Xt_Va_Get_اقدار(sfr->wword, XmNset, &wword, عدم);
    Xt_Va_Get_اقدار(sfr->mcase, XmNset, &mcase, عدم);
    إذا (wword)
	flags |= FRD_سارا_لفظ;
    إذا (mcase)
	flags |= FRD_میل_CASE;

    (فراغ)gui_do_findrepl((صحيح)flags, (char_u *)لبھو_متن, (char_u *)repl_متن,
							      direction_تھلے);

    إذا (لبھو_متن != عدم)
	XtFree(لبھو_متن);
    إذا (repl_متن != عدم)
	XtFree(repl_متن);
}

    ساكن فراغ
لبھو_replace_keypress(
    Widget		w بن_ورتیا,
    Shared_لبھو_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b بن_ورتیا)
{
    کلید_Sym keysym;

    إذا (frdp == عدم)
	أرجع;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    إذا (keysym == XK_Escape)
	Xt_Unmanage_بچہ(frdp->dialog);
}

    ساكن فراغ
سیٹ_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *اگلا;
    کلید_Sym	mnemonic = NUL;

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)label);
    إذا (p == عدم)
	أرجع;
    لكل (اگلا = p; *اگلا; ++اگلا)
    {
	إذا (*اگلا == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(اگلا);

	    إذا (len > 0)
	    {
		mch_memmove(اگلا, اگلا + 1, len);
		mnemonic = اگلا[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    إذا (str)
    {
	Xt_Va_سیٹ_اقدار(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
لبھو_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_لبھو_Replace_2	*frdp;
    Widget		separator;
    Widget		داخلہ_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		چوکھٹا;
    XmString		str;
    صحيح			n;
    Arg			دلیلاں[6];
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    Dimension		چوڑائی;
    Dimension		widest;
    char_u		*اندراج_متن;

    frdp = do_replace ? &repl_widgets : &لبھو_widgets;

    // Get the search string to use.
    اندراج_متن = get_لبھو_dialog_متن(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	إذا (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_کھڑکی(Xt_ماپا(frdp->dialog)));
	وإلا
	    Xt_Manage_بچہ(frdp->dialog);
	XtPopup(Xt_ماپا(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	إذا (اندراج_متن != عدم)
	    Xm_متن_میدان_سیٹ_String(frdp->what, (حرف *)اندراج_متن);
	vim_free(اندراج_متن);

	Xt_Va_سیٹ_اقدار(frdp->wword, XmNset, wword, عدم);
	أرجع;
    }

    // Create a fresh new dialog window
    إذا (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وإلا
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNautoUnmanage, False); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNnoResize, True); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", دلیلاں, n);
    XmStringFree(str);
    Xt_جوڑو_Callback(frdp->dialog, XmNdestroyCallback,
	    لبھو_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_وتھ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وتھ, 4,
	    عدم);

    frdp->لبھو = XtVaCreateManagedWidget("findButton",
	    xm_ڈالو_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);
    سیٹ_label(frdp->لبھو, _("Find &Next"));

    Xt_جوڑو_Callback(frdp->لبھو, XmNactivateCallback,
	    لبھو_replace_callback,
	    (do_replace ? (Xt_اشارہ_گر)FRD_R_FINDNEXT : (Xt_اشارہ_گر)FRD_FINDNEXT));

    إذا (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->لبھو,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سیٹ_label(frdp->replace, _("&Replace"));
	Xt_جوڑو_Callback(frdp->replace, XmNactivateCallback,
		لبھو_replace_callback, (Xt_اشارہ_گر)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سیٹ_label(frdp->all, _("Replace &All"));
	Xt_جوڑو_Callback(frdp->all, XmNactivateCallback,
		لبھو_replace_callback, (Xt_اشارہ_گر)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ڈالو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سیٹ_label(frdp->undo, _("&Undo"));
	Xt_جوڑو_Callback(frdp->undo, XmNactivateCallback,
		لبھو_replace_callback, (Xt_اشارہ_گر)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ڈالو_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    عدم);
    سیٹ_label(frdp->cancel, _("&Cancel"));
    Xt_جوڑو_Callback(frdp->cancel, XmNactivateCallback,
	    لبھو_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_بچہ(button_form);

    n = 0;
    Xt_سیٹ_Arg(دلیلاں[n], XmNorientation, XmVERTICAL); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNrightWidget, button_form); n++;
    Xt_سیٹ_Arg(دلیلاں[n], Xm_Nright_وتھ, 4); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سیٹ_Arg(دلیلاں[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", دلیلاں, n);
    Xt_Manage_بچہ(separator);

    داخلہ_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وتھ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 4,
	    عدم);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, داخلہ_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_وتھ, 4,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_متن_میدان_Widget_Class, داخلہ_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		عدم);

	إذا (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_متن_میدان_Widget_Class,	داخلہ_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_وتھ, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	    Xt_جوڑو_Callback(frdp->with, XmNactivateCallback,
		    لبھو_replace_callback, (Xt_اشارہ_گر) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, داخلہ_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_وتھ, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_جوڑو_Callback(frdp->what, XmNactivateCallback,
		    اندراج_activate_callback, frdp->with);
	    Xt_جوڑو_Event_Handler(frdp->with, کلید_Press_نقاب, False,
			    (XtEventHandler)لبھو_replace_keypress,
			    (Xt_اشارہ_گر) frdp);

	}
	وإلا
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_جوڑو_Callback(frdp->what, XmNactivateCallback,
		    لبھو_replace_callback, (Xt_اشارہ_گر)FRD_FINDNEXT);
	}
	Xt_جوڑو_Event_Handler(frdp->what, کلید_Press_نقاب, False,
			    (XtEventHandler)لبھو_replace_keypress,
			    (Xt_اشارہ_گر)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_سیٹ_Arg(دلیلاں[n], XmNwidth, &چوڑائی); n++;
	Xt_Get_اقدار(label_what, دلیلاں, n);
	widest = چوڑائی;
	إذا (do_replace)
	{
	    Xt_Get_اقدار(label_with, دلیلاں, n);
	    إذا (چوڑائی > widest)
		widest = چوڑائی;
	}

	Xt_Va_سیٹ_اقدار(frdp->what, Xm_Nleft_وتھ, widest, عدم);
	إذا (do_replace)
	    Xt_Va_سیٹ_اقدار(frdp->with, Xm_Nleft_وتھ, widest, عدم);

    }

    Xt_Manage_بچہ(داخلہ_form);

    {
	Widget radio_box;
	Widget w;

	چوکھٹا = XtVaCreateWidget("directionFrame",
		xm_چوکھٹا_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, داخلہ_form,
		Xm_Ntop_وتھ, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_وتھ, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, داخلہ_form,
		عدم);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, چوکھٹا,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_چوکھٹا_TITLE_بچہ,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(چوکھٹا, "radioBox",
		(ArgList)عدم, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->اتے = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->اتے);

	str = XmStringCreateSimple(_("Down"));
	frdp->تھلے = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->تھلے);

	Xt_Manage_بچہ(radio_box);
	Xt_Manage_بچہ(چوکھٹا);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, چوکھٹا,
	    Xm_Nright_وتھ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, داخلہ_form,
	    Xm_Ntop_وتھ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وتھ, 4,
	    عدم);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وتھ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNset, wword,
	    عدم);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وتھ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_وتھ, 4,
	    XmNset, mcase,
	    عدم);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_بچہ(toggle_form);

    إذا (اندراج_متن != عدم)
	Xm_متن_میدان_سیٹ_String(frdp->what, (حرف *)اندراج_متن);
    vim_free(اندراج_متن);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   فراغ
gui_mch_لبھو_dialog(exarg_T *eap)
{
    إذا (!gui.وچ_use)
	أرجع;

    لبھو_replace_dialog_create(eap->arg, FALSE);
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (!gui.وچ_use)
	أرجع;

    لبھو_replace_dialog_create(eap->arg, TRUE);
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
    Shared_لبھو_Replace_2 *frdp;
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
	frdp = (do_replace) ? (&repl_widgets) : (&لبھو_widgets);
	إذا (frdp->dialog)
	{
	    Xt_Va_سیٹ_اقدار(frdp->what, XmNfontList, font_list, عدم);
	    إذا (do_replace)
		Xt_Va_سیٹ_اقدار(frdp->with, XmNfontList, font_list, عدم);
	}
    }

    XmFontListFree(font_list);
}
