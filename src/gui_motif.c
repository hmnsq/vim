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
#إذا (Xm_نسخو >= 1002)
# include <Xm/RepType.h>
#جيڪڏهن_پورو
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/XmP.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#جيڪڏهن_وصف_ٿيل HAVE_X11_XPM_H
# إذا defined(VMS)
#  include <xpm.h>
# وإلا
#  include <X11/xpm.h>
# endif
#وإلا
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#جيڪڏهن_پورو

#ضمن "gui_xmebw.h"	// for our Enhanced Button Widget

#إذا defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#جيڪڏهن_پورو

#وصف_ڪريو MOTIF_POPUP

خارجي Widget vimShell;

ساكن Widget vimForm;
ساكن Widget متن_Area_Form;
Widget متن_Area;
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
ساكن Widget tool_Bar_چوکٺو;
ساكن Widget toolBar;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
ساكن Widget	tab_سٽ;
ساكن Widget	tab_سٽ_menu = 0;
ساكن صحيح	showing_tabline = 0;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MENU
# إذا (Xm_نسخو >= 1002)
// remember the last set value for the tearoff item
ساكن صحيح tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
# endif
ساكن Widget menuBar;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
ساكن فراغ reset_focus(فراغ);
#جيڪڏهن_پورو

ساكن فراغ gui_motif_menu_colors(Widget id);
ساكن فراغ gui_motif_scroll_colors(Widget id);

#إذا (Xm_نسخو >= 1002)
# define STRING_ٽيگ  Xm_FONTLIST_DEFAULT_ٽيگ
#وإلا
# define STRING_ٽيگ  XmSTRING_DEFAULT_CHARSET
#جيڪڏهن_پورو

/*
 * Call-back routines.
 */

    ساكن فراغ
scroll_cb(Widget w اڻ_استعمال_ٿيل, Xt_اشارو client_معلومات_2, Xt_اشارو call_معلومات)
{
    scrollbar_T *sb;
    ڊگهو	قدر;
    صحيح		dragging;

    sb = gui_ڳوليو_scrollbar((ڊگهو)client_معلومات_2);

    قدر = ((XmScrollBarCallbackStruct *)call_معلومات)->قدر;
    dragging = (((XmScrollBarCallbackStruct *)call_معلومات)->reason ==
							      (صحيح)XmCR_DRAG);
    gui_drag_scrollbar(sb, قدر, dragging);
}

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    ساكن فراغ
tabline_cb(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل,
    Xt_اشارو	call_معلومات)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_معلومات;
    إذا (nptr->reason != (صحيح)XmCR_NONE)
	send_tabline_event(nptr->صفحو_عدد);
}

    ساكن فراغ
tabline_button_cb(
    Widget	w,
    Xt_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    Xt_اشارو	cmd, tab_idx;

    Xt_Va_Get_قدرون(w, Xm_Nuser_معلومات, &cmd, عدم);
    Xt_Va_Get_قدرون(tab_سٽ_menu, Xm_Nuser_معلومات, &tab_idx, عدم);

    send_tabline_menu_event((صحيح)(ڊگهو)tab_idx, (صحيح)(ڊگهو)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساكن فراغ
motif_tabline_timer_cb (
    Xt_اشارو		timed_ٻاهريون,
    XtIntervalId	*interval_id اڻ_استعمال_ٿيل)
{
    *((صحيح *)timed_ٻاهريون) = TRUE;
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
    Dimension	ويڪر, اوچائي;

    tab_scroll_w = Xt_Name_منزل_Widget(tab_سٽ, scroller_name);
    إذا (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_قدرون(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &ويڪر, XmNheight, &اوچائي, عدم);
	إذا (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    إذا (event->x >= pos_x && event->x <= pos_x + ويڪر
		    && event->y >= pos_y && event->y <= pos_y + اوچائي)
		// Clicked on the scroller
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
tabline_menu_cb(
    Widget	w,
    Xt_اشارو	گهيرو اڻ_استعمال_ٿيل,
    XEvent	*e,
    Boolean	*continue_dispatch اڻ_استعمال_ٿيل)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحيح				tab_idx = 0;
    WidgetList			children;
    Cardinal			انگ_Children;
    ساكن XtIntervalId		timer = (XtIntervalId)0;
    ساكن صحيح			timed_ٻاهريون = TRUE;

    event = (XButtonPressedEvent *)e;

    إذا (event->button == Button1)
    {
	إذا (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    أرجع;

	إذا (!timed_ٻاهريون)
	{
	    Xt_Remove_وقت_ٻاهريون(timer);
	    timed_ٻاهريون = TRUE;

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
	    timer = Xt_App_جوڙيو_وقت_ٻاهريون(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_ٻاهريون);
	    timed_ٻاهريون = FALSE;
	}
	أرجع;
    }

    إذا (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_قدرون(tab_سٽ_menu, Xm_Nuser_معلومات, &tab_idx, عدم);
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
	tab_w = Xt_دري_منزل_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	إذا (tab_w != (Widget)0 && Xm_Is_ڌڪو_Button(tab_w))
	    Xt_Va_Get_قدرون(tab_w, Xm_Npage_عدد, &tab_idx, عدم);
    }

    Xt_Va_سيٽ_قدرون(tab_سٽ_menu, Xm_Nuser_معلومات, (Xt_اشارو)(ڊگهو)tab_idx, عدم);
    Xt_Va_Get_قدرون(tab_سٽ_menu, XmNchildren, &children, XmNnumChildren,
		  &انگ_Children, عدم);
    XtManageChildren(children, انگ_Children);
    XmMenuPosition(tab_سٽ_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_ٻار(tab_سٽ_menu);
}

    ساكن فراغ
tabline_balloon_cb(BalloonEval *beval, صحيح state اڻ_استعمال_ٿيل)
{
    صحيح		nr;
    tabpage_T	*tp;

    إذا (beval->هدف == (Widget)0)
	أرجع;

    Xt_Va_Get_قدرون(beval->هدف, Xm_Npage_عدد, &nr, عدم);
    tp = ڳوليو_tabpage(nr);
    إذا (tp == عدم)
	أرجع;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#جيڪڏهن_پورو

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
    بنانشان حرف   label_type = (صحيح)XmSTRING;

    Xt_Va_Get_قدرون(_w, XmNlabelType, &label_type, (Xt_اشارو)0);

    إذا (XtIsSensitive(_w) || label_type != (صحيح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وإلا
    {
	XGCValues   قدرون;
	XtGCMask    نقاب;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	نقاب = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_نقاب | GCClipXOrigin | GCClipYOrigin;
	قدرون.graphics_exposures = False;

	إذا (fs != 0)
	{
	    نقاب |= GCFont;
	    قدرون.font = fs->fid;
	}

	إذا (lw->primitive.چوٽي_shadow_pixmap != None
		&& lw->primitive.چوٽي_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    نقاب |= GCFillStyle | GCTile;
	    قدرون.fill_style = FillTiled;
	    قدرون.tile = lw->primitive.چوٽي_shadow_pixmap;
	}

	lw->label.متن_Rect.x += 1;
	lw->label.متن_Rect.y += 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x += 1;
	    lw->label.acc_متن_Rect.y += 1;
	}

	قدرون.foreground = lw->primitive.چوٽي_shadow_color;
	قدرون.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, نقاب,
					       &قدرون, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.متن_Rect.x -= 1;
	lw->label.متن_Rect.y -= 1;
	إذا (lw->label.acc_متن != 0)
	{
	    lw->label.acc_متن_Rect.x -= 1;
	    lw->label.acc_متن_Rect.y -= 1;
	}

	قدرون.foreground = lw->primitive.bottom_shadow_color;
	قدرون.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, نقاب,
					       &قدرون, dynamic, (XtGCMask)0);
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
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		دليلون[10];
    صحيح		n;
    XmString	xms;
#جيڪڏهن_پورو

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
    gui.border_وٿي = gui.border_ويڪر;

    /*
     * Install the tearOffModel resource converter.
     */
#إذا (Xm_نسخو >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#جيڪڏهن_پورو

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_سيٽ_قدرون(vimShell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_ويڪر, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_ويڪر, 0,
	Xm_Nmargin_اوچائي, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_menu_colors(vimForm);

#جيڪڏهن_وصف_ٿيل FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحيح ac = 0;

# إذا (Xm_نسخو >= 1002)
	Xt_سيٽ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_سيٽ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_سيٽ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_سيٽ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_سيٽ_Arg(al[ac], Xm_Nright_وٿي, 0); ac++;
# endif
	Xt_سيٽ_Arg(al[ac], Xm_Nmargin_اوچائي, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_ٻار(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_چوکٺو = XtVaCreateWidget("toolBarFrame",
	xm_چوکٺو_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_اوچائي, 0,
	Xm_Nmargin_ويڪر, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	عدم);
    gui_motif_menu_colors(tool_Bar_چوکٺو);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_صف_ڪالم_Widget_Class, tool_Bar_چوکٺو,
	XmNchildType, Xm_چوکٺو_WORKAREA_ٻار,
	Xm_Nrow_ڪالم_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_اوچائي, 0,
	Xm_Nmargin_ويڪر, 0,
	Xm_Nadjust_آخري, True,
	عدم);
    gui_motif_menu_colors(toolBar);

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNbindingType, XmNONE); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNorientation, XmVERTICAL); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nback_صفحو_سائيز, XmNONE); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nback_صفحو_عدد, 0); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nback_صفحو_Placement, Xm_چوٽي_ساڄو); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNmajorTabSpacing, 0); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNshadowThickness, 0); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سٽ = XmCreateNotebook(vimForm, "Vim tabline", دليلون, n);

    Xt_جوڙيو_Callback(tab_سٽ, Xm_Npage_بدليو_Callback, (XtCallbackProc)tabline_cb,
			عدم);
    Xt_جوڙيو_Event_Handler(tab_سٽ, Button_Press_نقاب, False,
			(XtEventHandler)tabline_menu_cb, عدم);

    gui.tabline_اوچائي = TABLINE_اوچائي;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_منزل_Widget(tab_سٽ, "MinorTabScrollerNext");
    Xt_Va_سيٽ_قدرون(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);
    scroller = Xt_Name_منزل_Widget(tab_سٽ, "MinorTabScrollerPrevious");
    Xt_Va_سيٽ_قدرون(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);

    // Create the tabline popup menu
    tab_سٽ_menu = XmCreatePopupMenu(tab_سٽ, "tabline popup", عدم, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nuser_معلومات, (Xt_اشارو)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_ٽيگ);
    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڌڪو_Button(tab_سٽ_menu, "Close", دليلون, n);
    Xt_جوڙيو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nuser_معلومات, (Xt_اشارو)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_ٽيگ);
    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڌڪو_Button(tab_سٽ_menu, "New Tab", دليلون, n);
    Xt_جوڙيو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nuser_معلومات, (Xt_اشارو)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_ٽيگ);
    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, xms); n++;
    button = Xm_Create_ڌڪو_Button(tab_سٽ_menu, "Open tab...", دليلون, n);
    Xt_جوڙيو_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);
#جيڪڏهن_پورو

    متن_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_ويڪر, 0,
	Xm_Nmargin_اوچائي, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_scroll_colors(متن_Area_Form);

    متن_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, متن_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.پٺ_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_ويڪر, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	عدم);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(متن_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.اندريون_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    فراغ
gui_x11_destroy_widgets(فراغ)
{
    متن_Area = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_MENU
    menuBar = عدم;
#جيڪڏهن_پورو
}

    فراغ
gui_mch_سيٽ_متن_area_pos(
    صحيح	    x اڻ_استعمال_ٿيل,
    صحيح	    y اڻ_استعمال_ٿيل,
    صحيح	    w اڻ_استعمال_ٿيل,
    صحيح	    h اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#جيڪڏهن_پورو
}

    فراغ
gui_x11_سيٽ_پٺ_color(فراغ)
{
    إذا (متن_Area != عدم)
#إذا (Xm_نسخو >= 1002)
	XmChangeColor(متن_Area, gui.پٺ_pixel);
#وإلا
	Xt_Va_سيٽ_قدرون(متن_Area,
		  XmNbackground, gui.پٺ_pixel,
		  عدم);
#جيڪڏهن_پورو
}

/*
 * Manage dialog centered on pointer.
 */
    فراغ
manage_centered(Widget dialog_ٻار)
{
    Widget shell = Xt_والد(dialog_ٻار);
    دري پاڙ, ٻار;
    بنانشان صحيح نقاب;
    بنانشان صحيح ويڪر, اوچائي, border_ويڪر, اونهائي;
    صحيح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_قدرون(shell, XmNmappedWhenManaged, &mappedWhenManaged, عدم);
    Xt_Va_سيٽ_قدرون(shell, XmNmappedWhenManaged, False, عدم);

    Xt_Manage_ٻار(dialog_ٻار);

    // Get the pointer position (x, y)
    XQuery_اشارو(XtDisplay(shell), Xt_دري(shell), &پاڙ, &ٻار,
		  &x, &y, &win_x, &win_y, &نقاب);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_دري(shell), &پاڙ, &win_x, &win_y,
		 &ويڪر, &اوچائي, &border_ويڪر, &اونهائي);
    ويڪر += 2 * border_ويڪر;
    اوچائي += 2 * border_ويڪر;
    x -= ويڪر / 2;
    y -= اوچائي / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->ويڪر - ويڪر;
    maxY = XtScreen(shell)->اوچائي - اوچائي;
    إذا (x < 0)
	x = 0;
    إذا (x > maxX)
	x = maxX;
    إذا (y < 0)
	y = 0;
    إذا (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_سيٽ_قدرون(shell, XmNx, x, XmNy, y, عدم);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_سيٽ_قدرون(shell, XmNmappedWhenManaged, mappedWhenManaged, عدم);
}

#إذا defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# إذا (Xm_نسخو <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_ٽيگ);
# وإلا
    // Motif 1.2 method
    Xm_Font_List_داخلا font_list_داخلا;

    font_list_داخلا = Xm_Font_List_داخلا_Create(STRING_ٽيگ, XmFONT_IS_FONT,
					    (Xt_اشارو)font);
    font_list = Xm_Font_List_Append_داخلا(عدم, font_list_داخلا);
    Xm_Font_List_داخلا_Free(&font_list_داخلا);
# endif
    أرجع font_list;
}

# إذا ((Xm_نسخو > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_سيٽ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_داخلا font_list_داخلا;

    font_list_داخلا = Xm_Font_List_داخلا_Create(STRING_ٽيگ,
					    XmFONT_IS_FONTSET,
					    (Xt_اشارو)*fontset);
    font_list = Xm_Font_List_Append_داخلا(عدم, font_list_داخلا);
    Xm_Font_List_داخلا_Free(&font_list_داخلا);
    أرجع font_list;
}
# endif

#جيڪڏهن_پورو

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساكن فراغ gui_motif_جوڙيو_actext(vimmenu_T *menu);
# إذا (Xm_نسخو >= 1002)
ساكن فراغ toggle_tearoff(Widget wid);
ساكن فراغ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساكن فراغ submenu_change(vimmenu_T *mp, صحيح colors);

ساكن فراغ do_سيٽ_mnemonics(صحيح enable);
ساكن صحيح menu_enabled = TRUE;

    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag)
    {
	Xt_Manage_ٻار(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_والد(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_سيٽ_قدرون(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
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
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
    }
    وإلا
    {
	Xt_Unmanage_ٻار(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(Xt_والد(toolBar)))
	{
	    Xt_Va_سيٽ_قدرون(Xt_والد(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar),
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
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
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    فراغ
gui_motif_سيٽ_mnemonics(صحيح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    إذا (!menu_enabled)
	enable = FALSE;
    do_سيٽ_mnemonics(enable);
}

    ساكن فراغ
do_سيٽ_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	إذا (menu->id != (Widget)0)
	    Xt_Va_سيٽ_قدرون(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    عدم);
}

    فراغ
gui_mch_جوڙيو_menu(vimmenu_T *menu, صحيح idx)
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
#  إذا (Xm_نسخو <= 1002)
	إذا (mouse_model_popup())
#  endif
	{
	    إذا (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_سيٽ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    إذا (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_سيٽ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
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

    label = XmStringCreate((حرف *)menu->dname, STRING_ٽيگ);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (والد == عدم) ? menuBar : والد->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# إذا (Xm_نسخو >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_اشاريو, idx + (والد != عدم
			   && tearoff_val == (صحيح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    عدم);
    XmStringFree(label);

    إذا (menu->id == (Widget)0)		// failed
	أرجع;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    إذا (والد == عدم && menu->ترجيح >= 9999)
	Xt_Va_سيٽ_قدرون(menuBar,
		XmNmenuHelpWidget, menu->id,
		عدم);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_جوڙيو_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	عدم);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_صف_ڪالم_Widget_Class, shell,
	Xm_Nrow_ڪالم_Type, XmMENU_PULLDOWN,
	عدم);
    gui_motif_menu_colors(menu->submenu_id);

    إذا (menu->submenu_id == (Widget)0)		// failed
	أرجع;

# إذا (Xm_نسخو >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_سيٽ_قدرون(menu->id,
	XmNsubMenuId, menu->submenu_id,
	عدم);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    إذا (والد == عدم)
	gui_mch_compute_menu_اوچائي(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساكن فراغ
gui_motif_جوڙيو_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    إذا (menu->actext == عدم || menu->id == (Widget)0)
	أرجع;

    label = XmStringCreate((حرف *)menu->actext, STRING_ٽيگ);
    إذا (label == عدم)
	أرجع;
    Xt_Va_سيٽ_قدرون(menu->id, Xm_Naccelerator_متن, label, عدم);
    XmStringFree(label);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
# إذا (Xm_نسخو >= 1002)
    إذا (enable)
	tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
    وإلا
	tearoff_val = (صحيح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(پاڙ_menu);
# endif
}

# إذا (Xm_نسخو >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ساكن فراغ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_سيٽ_قدرون(wid, XmNtearOffModel, tearoff_val, عدم);
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
	menu = menu->ايندڙ;
    }
}
# endif

    صحيح
gui_mch_متن_area_extra_اوچائي(فراغ)
{
    Dimension	shadow_اوچائي;

    Xt_Va_Get_قدرون(متن_Area_Form, XmNshadowThickness, &shadow_اوچائي, عدم);
    أرجع shadow_اوچائي;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    فراغ
gui_mch_compute_menu_اوچائي(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساكن Dimension	اوچائي = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    إذا (id != (Widget)0)
	Xt_Va_Get_قدرون(id, XmNheight, &اوچائي, عدم);

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
	Xt_Manage_ٻار(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_قدرون(mp->id, XmNy, &y, عدم);
	    إذا (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_قدرون(menuBar,
	Xm_Nmargin_اوچائي, &margin,
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
    gui.menu_اوچائي = maxy + اوچائي - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    إذا (gui.shell_created)
	Xt_Va_سيٽ_قدرون(menuBar, XmNheight, gui.menu_اوچائي, عدم);
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
check_xpm(char_u *رستو)
{
    Xpm_خاصيتون attrs;
    صحيح		حالت;
    Pixmap	نقاب;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    حالت = Xpm_Read_File_منزل_Pixmap(gui.dpy,
	    پاڙ_دري_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)رستو, &map, &نقاب, &attrs);
    Xpm_Free_خاصيتون(&attrs);

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
	gui_ڳوليو_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	إذا (res == FAIL && gui_ڳوليو_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	إذا (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    أرجع tb_blank_xpm;
	}
    }

    إذا (menu->icon_builtin || gui_ڳوليو_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	إذا (menu->iconidx >= 0 && menu->iconidx
	       < (صحيح)(حجم(built_اندريون_pixmaps) / حجم(built_اندريون_pixmaps[0])))
	    xpm = built_اندريون_pixmaps[menu->iconidx];
	وإلا
	    xpm = tb_blank_xpm;
    }

    أرجع xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساكن صحيح
جوڙيو_pixmap_دليلون(vimmenu_T *menu, Arg *دليلون, صحيح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    إذا (menu->xpm == عدم)
    {
	Xt_سيٽ_Arg(دليلون[n], XmNlabelType, XmSTRING); n++;
    }
    وإلا
    {
	إذا (menu->xpm_fname != عدم)
	{
	    Xt_سيٽ_Arg(دليلون[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_سيٽ_Arg(دليلون[n], Xm_Npixmap_معلومات, menu->xpm); n++;
	Xt_سيٽ_Arg(دليلون[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    أرجع n;
}
# endif // FEAT_TOOLBAR

    فراغ
gui_mch_جوڙيو_menu_شي(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    vimmenu_T	*والد = menu->والد;

# إذا (Xm_نسخو <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    إذا (menu_is_ٻار_جو_popup(menu) && !mouse_model_popup())
	أرجع;
# endif

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(والد->name))
    {
	WidgetClass	type;
	XmString	xms = عدم;    // fallback label if pixmap not found
	صحيح		n;
	Arg		دليلون[18];

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
	    Xt_سيٽ_Arg(دليلون[n], XmNwidth, wid); n++;
	    Xt_سيٽ_Arg(دليلون[n], Xm_Nmin_ويڪر, wid); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNorientation, XmVERTICAL); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNseparatorType, Xm_SHADOW_ETCHED_اندريون); n++;
	}
	وإلا
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_سيٽ_Arg(دليلون[n], XmNhighlightThickness, 1); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNhighlightOnEnter, True); n++;
	    Xt_سيٽ_Arg(دليلون[n], Xm_Nmargin_ويڪر, 0); n++;
	    Xt_سيٽ_Arg(دليلون[n], Xm_Nmargin_اوچائي, 0); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_ٽيگ);
	    Xt_سيٽ_Arg(دليلون[n], XmNlabelString, xms); n++;

	    n = جوڙيو_pixmap_دليلون(menu, دليلون, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_سيٽ_Arg(دليلون[n], Xm_Nposition_اشاريو, idx); n++;
	إذا (menu->id == عدم)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, دليلون, n);
	    إذا (menu->id != عدم && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_جوڙيو_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وإلا
	    Xt_سيٽ_قدرون(menu->id, دليلون, n);
	إذا (xms != عدم)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_سيٽ_tip(menu);
#  endif

	menu->والد = والد;
	menu->submenu_id = عدم;
	// When adding first item to toolbar it might have to be enabled .
	إذا (!XtIsManaged(Xt_والد(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_اوچائي = gui_mch_compute_toolbar_اوچائي();
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
# إذا (Xm_نسخو >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_اشاريو, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		عدم);
	gui_motif_menu_colors(menu->id);
	أرجع;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_ٽيگ);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ڌڪو_Button_Widget_Class, والد->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# إذا (Xm_نسخو >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_اشاريو, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	عدم);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    إذا (menu->id != (Widget)0)
    {
	Xt_جوڙيو_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_اشارو)menu);
	// add accelerator text
	gui_motif_جوڙيو_actext(menu);
    }
}

# إذا (Xm_نسخو <= 1002)
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
    إذا (!gui.اندريون_use)
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
		  gui_mch_جوڙيو_menu(menu, idx);
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
      وإلا إذا (menu_is_ٻار_جو_popup(menu))
      {
	  إذا (mouse_model_popup())
	      gui_mch_جوڙيو_menu_شي(menu, idx);
	  وإلا
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->ايندڙ;
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
    gui_motif_menu_colors(tool_Bar_چوکٺو);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(پاڙ_menu, TRUE);
}

    فراغ
gui_mch_new_menu_font(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    submenu_change(پاڙ_menu, FALSE);
    {
	Dimension   اوچائي;
	Position w, h;

	Xt_Va_Get_قدرون(menuBar, XmNheight, &اوچائي, عدم);
	gui.menu_اوچائي = اوچائي;

	Xt_Va_Get_قدرون(vimShell, XtNwidth, &w, XtNheight, &h, عدم);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_حالت_area_اوچائي()
# endif
		     );
    }
    gui_سيٽ_shellsize(FALSE, TRUE, RESIZE_VERT);
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

    menu = gui_ڳوليو_menu((char_u *)"ToolBar");
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

    toolbar = gui_ڳوليو_menu((char_u *)"ToolBar");
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

    لكل (mp = menu; mp != عدم; mp = mp->ايندڙ)
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
		    Arg		دليلون[18];

		    n = جوڙيو_pixmap_دليلون(mp, دليلون, n);
		    Xt_سيٽ_قدرون(mp->id, دليلون, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg دليلون[2];

		    دليلون[0].name = XmNbackground;
		    دليلون[0].قدر = gui.tooltip_bg_pixel;
		    دليلون[1].name = XmNforeground;
		    دليلون[1].قدر = gui.tooltip_fg_pixel;
		    Xt_سيٽ_قدرون(mp->tip->balloonLabel, &دليلون[0], Xt_عدد(دليلون));
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
		    Arg دليلون[1];

		    دليلون[0].name = XmNfontList;
		    دليلون[0].قدر = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_سيٽ_قدرون(mp->tip->balloonLabel, &دليلون[0], Xt_عدد(دليلون));
		}
# endif
	    }
	}

	إذا (mp->children != عدم)
	{
# إذا (Xm_نسخو >= 1002)
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
	gui_mch_compute_menu_اوچائي((Widget)0);
# ifdef FEAT_TOOLBAR
    وإلا إذا (والد == toolBar)
    {
	Cardinal    انگ_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_قدرون(toolBar, XmNnumChildren, &انگ_children, عدم);
	إذا (انگ_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وإلا
	    gui.toolbar_اوچائي = gui_mch_compute_toolbar_اوچائي();
    }
# endif
}

    فراغ
gui_mch_show_popupmenu(vimmenu_T *menu اڻ_استعمال_ٿيل)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_آخري_mouse_event());
    Xt_Manage_ٻار(menu->submenu_id);
# endif
}

#جيڪڏهن_پورو // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    فراغ
gui_mch_def_colors(فراغ)
{
    إذا (!gui.اندريون_use)
	أرجع;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#جيڪڏهن_پورو
}


/*
 * Scrollbar stuff.
 */

    فراغ
gui_mch_سيٽ_scrollbar_thumb(
    scrollbar_T *sb,
    ڊگهو	val,
    ڊگهو	سائيز,
    ڊگهو	max)
{
    إذا (sb->id != (Widget)0)
	Xt_Va_سيٽ_قدرون(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_سائيز, سائيز,
		  XmNpageIncrement, (سائيز > 2 ? سائيز - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  عدم);
}

    فراغ
gui_mch_سيٽ_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    إذا (sb->id == (Widget)0)
	أرجع;

    إذا (sb->type == SBAR_کاٻو || sb->type == SBAR_ساڄو)
    {
	إذا (y == 0)
	    h -= gui.border_وٿي;
	وإلا
	    y -= gui.border_وٿي;
	Xt_Va_سيٽ_قدرون(sb->id,
		Xm_Ntop_وٿي, y,
		Xm_Nbottom_وٿي, -y - h,
		XmNwidth, w,
		عدم);
    }
    وإلا
	Xt_Va_سيٽ_قدرون(sb->id,
		Xm_Ntop_وٿي, y,
		Xm_Nleft_وٿي, x,
		Xm_Nright_وٿي, gui.which_scrollbars[SBAR_ساڄو]
						     ? gui.scrollbar_ويڪر : 0,
		XmNheight, h,
		عدم);
    Xt_Manage_ٻار(sb->id);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_قدرون(متن_Area, XtNwidth, &tw, XtNx, &tx, عدم);
    Xt_Va_Get_قدرون(vimShell, XtNwidth, &ww, عدم);
    xpad = ww - tw - tx - gui.scrollbar_ويڪر;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_قدرون(متن_Area, XtNheight, &th, XtNy, &ty, عدم);
    Xt_Va_Get_قدرون(vimShell, XtNheight, &wh, عدم);
    ypad = wh - th - ty - gui.scrollbar_اوچائي;
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    Arg		دليلون[16];
    صحيح		n;

    إذا (sb->id == (Widget)0)
	أرجع;

    n = 0;
    إذا (flag)
    {
	بدل (sb->type)
	{
	    حالة SBAR_کاٻو:
		Xt_سيٽ_Arg(دليلون[n], Xm_Nleft_وٿي, gui.scrollbar_ويڪر); n++;
		اكسر;

	    حالة SBAR_ساڄو:
		Xt_سيٽ_Arg(دليلون[n], Xm_Nright_وٿي, gui.scrollbar_ويڪر); n++;
		اكسر;

	    حالة SBAR_BOTTOM:
		Xt_سيٽ_Arg(دليلون[n], Xm_Nbottom_وٿي, gui.scrollbar_اوچائي);n++;
		اكسر;
	}
	Xt_سيٽ_قدرون(متن_Area, دليلون, n);
	Xt_Manage_ٻار(sb->id);
    }
    وإلا
    {
	إذا (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    بدل (sb->type)
	    {
		حالة SBAR_کاٻو:
		    Xt_سيٽ_Arg(دليلون[n], Xm_Nleft_وٿي, 0); n++;
		    اكسر;

		حالة SBAR_ساڄو:
		    Xt_سيٽ_Arg(دليلون[n], Xm_Nright_وٿي, 0); n++;
		    اكسر;

		حالة SBAR_BOTTOM:
		    Xt_سيٽ_Arg(دليلون[n], Xm_Nbottom_وٿي, 0);n++;
		    اكسر;
	    }
	    Xt_سيٽ_قدرون(متن_Area, دليلون, n);
	}
	Xt_Unmanage_ٻار(sb->id);
    }
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		دليلون[16];
    صحيح		n = 0;

    Xt_سيٽ_Arg(دليلون[n], XmNminimum, 0); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    بدل (sb->type)
    {
	حالة SBAR_کاٻو:
	    Xt_سيٽ_Arg(دليلون[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNwidth, gui.scrollbar_ويڪر); n++;
	    اكسر;

	حالة SBAR_ساڄو:
	    Xt_سيٽ_Arg(دليلون[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNwidth, gui.scrollbar_ويڪر); n++;
	    اكسر;

	حالة SBAR_BOTTOM:
	    Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNheight, gui.scrollbar_اوچائي); n++;
	    اكسر;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, متن_Area_Form, دليلون, n);
    إذا (sb->id == (Widget)0)
	أرجع;

    gui_mch_سيٽ_scrollbar_colors(sb);
    Xt_جوڙيو_Callback(sb->id, Xm_Nvalue_بدليو_Callback,
	    scroll_cb, (Xt_اشارو)sb->ident);
    Xt_جوڙيو_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_اشارو)sb->ident);
    Xt_جوڙيو_Event_Handler(sb->id, ڪنجي_Press_نقاب, FALSE, gui_x11_ڪنجي_ڪاميابي_cb,
	    (Xt_اشارو)0);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    فراغ
gui_mch_سيٽ_scrollbar_colors(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
    {
	إذا (gui.scroll_bg_pixel != INVALCOLOR)
	{
#إذا (Xm_نسخو>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#جيڪڏهن_پورو

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_سيٽ_قدرون(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    عدم);
	}

	إذا (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_سيٽ_قدرون(sb->id,
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

    دري
gui_x11_get_wid(فراغ)
{
    أرجع(Xt_دري(متن_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساكن فراغ
do_mnemonic(Widget w, بنانشان صحيح keycode)
{
    WidgetList	    children;
    صحيح		    انگ_Children, i;
    Boolean	    isMenu;
    ڪنجي_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    user_معلومات_2;
    بنانشان حرف   صف_کانو_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_صف_ڪالم_Widget_Class)
	{
	    Xt_Va_Get_قدرون(w, Xm_Nrow_ڪالم_Type, &صف_کانو_Type, عدم);
	    isMenu = (صف_کانو_Type != (بنانشان حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_قدرون(w, XmNchildren, &children, XmNnumChildren,
			  &انگ_Children, عدم);
	    لكل (i = 0; i < انگ_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وإلا
    {
	Xt_Va_Get_قدرون(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    إذا (XKeysym_منزل_Keycode(XtDisplay(Xt_والد(w)),
				       XString_منزل_Keysym(mneString)) == keycode)
	    {
		إذا (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_قدرون(w, Xm_Nuser_معلومات, &user_معلومات_2, عدم);
		    إذا (user_معلومات_2 != عدم && XtIsWidget(user_معلومات_2))
			XmProcessTraversal(user_معلومات_2, XmTRAVERSE_CURRENT);
		}
		وإلا
		{
		    XKeyPressedEvent ڪنجي_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_ميدان(ڪنجي_Event);
		    ڪنجي_Event.type = ڪنجي_Press;
		    ڪنجي_Event.serial = 1;
		    ڪنجي_Event.send_event = True;
		    ڪنجي_Event.display = XtDisplay(w);
		    ڪنجي_Event.دري_2 = Xt_دري(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & ڪنجي_Event,
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
	Xt_اشارو   call_معلومات اڻ_استعمال_ٿيل,
	XKeyEvent   *event,
	Boolean	    *b اڻ_استعمال_ٿيل)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ساكن فراغ
جوڙيو_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحيح		    انگ_Children, i;
    Boolean	    isMenu;
    ڪنجي_Sym	    mnemonic = '\0';
    بنانشان حرف   صف_کانو_Type;

    إذا (Xt_Is_مرکب(w))
    {
	إذا (XtClass(w) == xm_صف_ڪالم_Widget_Class)
	{
	    Xt_Va_Get_قدرون(w, Xm_Nrow_ڪالم_Type, &صف_کانو_Type, عدم);
	    isMenu = (صف_کانو_Type != (بنانشان حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_قدرون(w, XmNchildren, &children, XmNnumChildren,
							  &انگ_Children, عدم);
	    لكل (i = 0; i < انگ_Children; i++)
		جوڙيو_mnemonic_grabs(dialog, children[i]);
	}
    }
    وإلا
    {
	Xt_Va_Get_قدرون(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_ڪنجي(dialog, XKeysym_منزل_Keycode(XtDisplay(dialog),
						  XString_منزل_Keysym(mneString)),
		    Mod1_نقاب, True, Grab_گهڻائي_Async, Grab_گهڻائي_Async);
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

    Xt_جوڙيو_Event_Handler(dialog, ڪنجي_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارو) عدم);
    جوڙيو_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساكن فراغ
suppress_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_Ungrab_ڪنجي(dialog, Any_ڪنجي, Mod1_نقاب);
    XtRemoveEventHandler(dialog, ڪنجي_Press_نقاب, False,
			   (XtEventHandler) mnemonic_event, (Xt_اشارو) عدم);
}

#إذا defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساكن فراغ
سيٽ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    إذا (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_سيٽ *)&gui.fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_ٻار(id);
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ٻار(id);
	    }
	    وإلا
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_ٻار(id);
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ٻار(id);
	    }
	    وإلا
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
}
#جيڪڏهن_پورو

#إذا defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعريفنوع بنية dialog_callback_arg
{
    حرف *  دليلون;   // not used right now
    صحيح	    id;
} dcbarg_T;

ساكن Widget dialog_wgt;
ساكن حرف *browse_fname = عدم;
ساكن Xm_String_Char_سيٽ charset = (Xm_String_Char_سيٽ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساكن فراغ DialogCancelCB(Widget, Xt_اشارو, Xt_اشارو);
ساكن فراغ DialogAcceptCB(Widget, Xt_اشارو, Xt_اشارو);

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
سيٽ_predefined_label(Widget والد, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *ايندڙ;
    ڪنجي_Sym	mnemonic = NUL;

    w = Xt_Name_منزل_Widget(والد, name);

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)new_label);
    إذا (p == عدم)
	أرجع;
    لكل (ايندڙ = p; *ايندڙ; ++ايندڙ)
    {
	إذا (*ايندڙ == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(ايندڙ);

	    إذا (len > 0)
	    {
		mch_memmove(ايندڙ, ايندڙ + 1, len);
		mnemonic = ايندڙ[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_ٽيگ);
    vim_free(p);

    إذا (str != عدم)
    {
	Xt_Va_سيٽ_قدرون(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
سيٽ_predefined_fontlist(Widget والد, String name)
{
    Widget w;
    w = Xt_Name_منزل_Widget(والد, name);

    إذا (!w)
	أرجع;

    سيٽ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحيح		saving اڻ_استعمال_ٿيل,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext اڻ_استعمال_ٿيل,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*نمونو;
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
	جوڙيو_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    إذا (filter == عدم)
	نمونو = (char_u *)"";
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
	نمونو = vim_strnsave(s, p - s);
	tofree = نمونو;
	إذا (نمونو == عدم)
	    نمونو = (char_u *)"";
    }

    Xt_Va_سيٽ_قدرون(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (حرف *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (حرف *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (حرف *)نمونو, STRLEN(نمونو) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (حرف *)title, STRLEN(title) + 1,
	عدم);

    سيٽ_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    سيٽ_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    سيٽ_predefined_label(dialog_wgt, "Dir", _("Directories"));
    سيٽ_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    سيٽ_predefined_label(dialog_wgt, "Help", _("&Help"));
    سيٽ_predefined_label(dialog_wgt, "Items", _("Files"));
    سيٽ_predefined_label(dialog_wgt, "OK", _("&OK"));
    سيٽ_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    سيٽ_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    سيٽ_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_سيٽ_قدرون(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, عدم);

    Xt_جوڙيو_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_اشارو)0);
    Xt_جوڙيو_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_اشارو)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_ٻار(Xm_File_Selection_Box_Get_ٻار(dialog_wgt,
					(بنانشان حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    افعل
    {
	XtAppProcessEvent(Xt_Widget_منزل_Application_Context(dialog_wgt),
	    (Xt_داخلا_نقاب)XtIMAll);
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
    Widget	w اڻ_استعمال_ٿيل,		//  widget id
    Xt_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل,	//  data from application
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)	//  data from widget class
{
    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    Xt_Unmanage_ٻار(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساكن فراغ
DialogAcceptCB(
    Widget	w اڻ_استعمال_ٿيل,		//  widget id
    Xt_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل,	//  data from application
    Xt_اشارو	call_معلومات)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_معلومات;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->قدر, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_ٻار(dialog_wgt);
}

#جيڪڏهن_پورو // FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

ساكن صحيح	dialog_حالت;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساكن فراغ
keyhit_callback(
    Widget		w,
    Xt_اشارو		client_معلومات_2 اڻ_استعمال_ٿيل,
    XEvent		*event,
    Boolean		*cont اڻ_استعمال_ٿيل)
{
    حرف	buf[2];
    ڪنجي_Sym	ڪنجي_sym;

    إذا (XLookupString(&(event->xkey), buf, 2, &ڪنجي_sym, عدم) == 1)
    {
	إذا (*buf == CAR)
	    dialog_حالت = 1;
	وإلا إذا (*buf == ESC)
	    dialog_حالت = 2;
    }
    إذا ((ڪنجي_sym == XK_کاٻو || ڪنجي_sym == XK_ساڄو)
	    && !(event->xkey.state & سرڪايو_نقاب))
	Xm_متن_ميدان_Clear_Selection(w, Xt_آخري_Timestamp_Processed(gui.dpy));
}

    ساكن فراغ
butproc(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    dialog_حالت = (صحيح)(ڊگهو)client_معلومات_2 + 1;
}

# ifdef HAVE_XPM

    ساكن Widget
create_pixmap_label(
    Widget	والد,
    String	name,
    حرف	**معلومات,
    ArgList	دليلون,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحيح			اونهائي;
    Pixmap		pixmap = 0;
    Xpm_خاصيتون	attr;
    Boolean		rs;
    Xpm_Color_نشان	color[5] =
    {
	{"none", عدم, 0},
	{"iconColor1", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"selectColor", عدم, 0}
    };

    label = XmCreateLabelGadget(والد, name, دليلون, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_جو_Object(label);
    scr = Xt_Screen_جو_Object(label);
    Xt_Va_Get_قدرون(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_والد(label),
		  XmNdepth, &اونهائي,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  عدم);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_اونهائي;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.اونهائي = اونهائي;
    Xpm_Create_Pixmap_From_معلومات(dsp, پاڙ_دري_جو_Screen(scr),
		    معلومات, &pixmap, عدم, &attr);

    Xt_Va_Get_قدرون(label, Xm_Nrecompute_سائيز, &rs, عدم);
    Xt_Va_سيٽ_قدرون(label, Xm_Nrecompute_سائيز, True, عدم);
    Xt_Va_سيٽ_قدرون(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    عدم);
    Xt_Va_سيٽ_قدرون(label, Xm_Nrecompute_سائيز, rs, عدم);

    أرجع label;
}
# endif

    صحيح
gui_mch_dialog(
    صحيح		type اڻ_استعمال_ٿيل,
    char_u	*title,
    char_u	*پيغام,
    char_u	*button_نالا,
    صحيح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحيح		ex_cmd اڻ_استعمال_ٿيل)
{
    char_u		*buts;
    char_u		*p, *ايندڙ;
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
    Arg			دليلون[6];
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
    Xt_Va_سيٽ_قدرون(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    عدم);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_نالا);
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
	ڪنجي_Sym mnemonic = NUL;

	لكل (ايندڙ = p; *ايندڙ; ++ايندڙ)
	{
	    إذا (*ايندڙ == DLG_HOTKEY_CHAR)
	    {
		صحيح len = STRLEN(ايندڙ);

		إذا (len > 0)
		{
		    mch_memmove(ايندڙ, ايندڙ + 1, len);
		    mnemonic = ايندڙ[0];
		}
	    }
	    إذا (*ايندڙ == DLG_BUTTON_SEP)
	    {
		*ايندڙ++ = NUL;
		اكسر;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_ٽيگ);
	إذا (label == عدم)
	    اكسر;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ڌڪو_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_وٿي, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		عدم);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	إذا (butcount > 0)
	{
	    إذا (vertical)
		Xt_Va_سيٽ_قدرون(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			عدم);
	    وإلا
	    {
		إذا (*ايندڙ == NUL)
		{
		    Xt_Va_سيٽ_قدرون(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_وٿي, 4,
			    عدم);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_وٿي, 4,
			    عدم);
		    Xt_Manage_ٻار(sep_form);
		}
		وإلا
		{
		    Xt_Va_سيٽ_قدرون(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    عدم);
		}
	    }
	}
	وإلا إذا (!vertical)
	{
	    إذا (*ايندڙ == NUL)
	    {
		Xt_Va_سيٽ_قدرون(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_وٿي, 4,
			عدم);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_وٿي, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_وٿي, 4,
			عدم);
		Xt_Manage_ٻار(sep_form);
	    }
	    وإلا
		Xt_Va_سيٽ_قدرون(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_وٿي, 4,
			عدم);
	}

	Xt_جوڙيو_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_اشارو)(ڊگهو)butcount);
	p = ايندڙ;
    }
    vim_free(buts);

    separator = (Widget) 0;
    إذا (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_سيٽ_Arg(دليلون[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_سيٽ_Arg(دليلون[n], XmNbottomWidget, buttons[0]); n++;
	Xt_سيٽ_Arg(دليلون[n], Xm_Nbottom_وٿي, 4); n++;
	Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", دليلون, n);
	Xt_Manage_ٻار(separator);
    }

    إذا (textfield != عدم)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_متن_ميدان_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	إذا (butcount > 0)
	    Xt_Va_سيٽ_قدرون(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سيٽ_قدرون(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	سيٽ_fontlist(dialogtextfield);
	Xm_متن_ميدان_سيٽ_String(dialogtextfield, (حرف *)textfield);
	Xt_Manage_ٻار(dialogtextfield);
	Xt_جوڙيو_Event_Handler(dialogtextfield, ڪنجي_Press_نقاب, False,
			    (XtEventHandler)keyhit_callback, (Xt_اشارو)عدم);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    عدم);
    Xt_Manage_ٻار(form);

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
    Xt_سيٽ_Arg(دليلون[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Ntop_وٿي, 8); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nbottom_وٿي, 8); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nleft_وٿي, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_معلومات_2, دليلون, n);
    Xt_Manage_ٻار(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)پيغام, STRING_ٽيگ);
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
				Xm_Ntop_وٿي, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# وإلا
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_وٿي, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_وٿي, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_وٿي, 8,
				عدم);
    XmStringFree(label);
    سيٽ_fontlist(w);

    إذا (textfield != عدم)
    {
	Xt_Va_سيٽ_قدرون(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		عدم);
    }
    وإلا
    {
	إذا (butcount > 0)
	    Xt_Va_سيٽ_قدرون(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_سيٽ_قدرون(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
    }

    إذا (dfltbutton < 1)
	dfltbutton = 1;
    إذا (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_سيٽ_قدرون(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], عدم);
    إذا (textfield != عدم)
	Xt_Va_سيٽ_قدرون(dialogform, XmNinitialFocus, dialogtextfield, عدم);
    وإلا
	Xt_Va_سيٽ_قدرون(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									عدم);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    إذا (textfield != عدم && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_متن_ميدان_سيٽ_Selection(dialogtextfield,
			 (Xm_متن_Position)0, (Xm_متن_Position)STRLEN(textfield),
					   Xt_آخري_Timestamp_Processed(gui.dpy));
	Xm_متن_ميدان_سيٽ_Cursor_Position(dialogtextfield,
					   (Xm_متن_Position)STRLEN(textfield));
    }

    app = Xt_Widget_منزل_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_حالت = -1;
    لكل (;;)
    {
	XtAppProcessEvent(app, (Xt_داخلا_نقاب)XtIMAll);
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
#جيڪڏهن_پورو // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    Cardinal	انگ_Children;	    // how many children toolBar has

    إذا (toolBar == (Widget)0)
	أرجع;
    Xt_Va_Get_قدرون(toolBar, XmNnumChildren, &انگ_Children, عدم);
    إذا (showit && انگ_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_قدرون(toolBar, XmNchildren, &children, عدم);
	{
	    فراغ    (*action)(BalloonEval *);
	    صحيح	    متن = 0;

	    إذا (لڙي_ڳولا((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وإلا
		action = &gui_mch_disable_beval_area;
	    إذا (لڙي_ڳولا((ثابت حرف *)p_toolbar, "text"))
		متن = 1;
	    وإلا إذا (لڙي_ڳولا((ثابت حرف *)p_toolbar, "icons"))
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
		    لكل (cur = toolbar->children; cur; cur = cur->ايندڙ)
		    {
			Arg	    دليلون[1];
			صحيح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			إذا (cur->tip != عدم)
			    (*action)(cur->tip);
			إذا (!menu_is_separator(cur->name))
			{
			    إذا (متن == 1 || cur->xpm == عدم)
			    {
				Xt_سيٽ_Arg(دليلون[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    إذا (cur->id != عدم)
			    {
				Xt_Unmanage_ٻار(cur->id);
				Xt_سيٽ_قدرون(cur->id, دليلون, n);
				Xt_Manage_ٻار(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_اوچائي = gui_mch_compute_toolbar_اوچائي();
	Xt_Manage_ٻار(Xt_والد(toolBar));
# ifdef FEAT_GUI_TABLINE
	إذا (showing_tabline)
	{
	    Xt_Va_سيٽ_قدرون(tab_سٽ,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  عدم);
	    Xt_Va_سيٽ_قدرون(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سٽ,
			  عدم);
	}
	وإلا
# endif
	    Xt_Va_سيٽ_قدرون(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_والد(toolBar),
			  عدم);
	إذا (XtIsManaged(menuBar))
	    Xt_Va_سيٽ_قدرون(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    عدم);
	وإلا
	    Xt_Va_سيٽ_قدرون(Xt_والد(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    عدم);
    }
    وإلا
    {
	gui.toolbar_اوچائي = 0;
	إذا (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
	وإلا
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سٽ,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}

	Xt_Unmanage_ٻار(Xt_والد(toolBar));
    }
    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_VERT);
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
gui_mch_compute_toolbar_اوچائي(فراغ)
{
    Dimension	borders;
    Dimension	اوچائي;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	انگ_Children;	    // how many children toolBar has
    صحيح		i;

    borders = 0;
    اوچائي = 0;
    إذا (toolBar != (Widget)0 && tool_Bar_چوکٺو != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_قدرون(tool_Bar_چوکٺو,
		XmNshadowThickness, &fst,
		Xm_Nmargin_اوچائي, &fmh,
		عدم);
	borders += fst + fmh;
	Xt_Va_Get_قدرون(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_اوچائي, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &انگ_Children, عدم);
	borders += tst + tmh;
	لكل (i = 0; i < (صحيح)انگ_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_قدرون(children[i], XmNheight, &whgt, عدم);
	    إذا (اوچائي < whgt)
		اوچائي = whgt;
	}
    }
# ifdef LESSTIF_نسخو
    // Hack: When starting up we get wrong dimensions.
    إذا (اوچائي < 10)
	اوچائي = 24;
# endif

    أرجع (صحيح)(اوچائي + (borders << 1));
}

    فراغ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_قدرون(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    عدم);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (tab_سٽ == (Widget)0)
	أرجع;

    إذا (!showit != !showing_tabline)
    {
	إذا (showit)
	{
	    Xt_Manage_ٻار(tab_سٽ);
	    Xt_Unmanage_ٻار(Xt_Name_منزل_Widget(tab_سٽ, "PageScroller"));
	    Xt_Unmanage_ٻار(Xt_Name_منزل_Widget(tab_سٽ, "MinorTabScrollerNext"));
	    Xt_Unmanage_ٻار(Xt_Name_منزل_Widget(tab_سٽ,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(tab_سٽ,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	    Xt_Va_سيٽ_قدرون(متن_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سٽ,
			  عدم);
	}
	وإلا
	{
	    Xt_Unmanage_ٻار(tab_سٽ);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(Xt_والد(toolBar)))
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_والد(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_سيٽ_قدرون(متن_Area_Form,
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
    أرجع tab_سٽ != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline(فراغ)
{
    tabpage_T		*tp;
    صحيح			nr = 1, n;
    Arg			دليلون[10];
    صحيح			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_صفحو_Info	صفحو_info;
    Xm_Notebook_صفحو_حالت صفحو_حالت;
    صحيح			آخري_صفحو, tab_ڳڻپ;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    إذا (tab_سٽ == (Widget)0)
	أرجع;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = پهريون_tabpage; tp != عدم; tp = tp->tp_ايندڙ, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	صفحو_حالت = Xm_Notebook_Get_صفحو_Info(tab_سٽ, nr, &صفحو_info);
	إذا (صفحو_حالت == Xm_صفحو_INVALID
		|| صفحو_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_سيٽ_Arg(دليلون[n], Xm_Nnotebook_ٻار_Type, XmMAJOR_TAB); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNtraversalOn, False); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNhighlightThickness, 1); n++;
	    Xt_سيٽ_Arg(دليلون[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ڌڪو_Button(tab_سٽ, "-Empty-", دليلون, n);
	    Xt_Manage_ٻار(tab);
	    beval = gui_mch_create_beval_area(tab, عدم, tabline_balloon_cb,
									عدم);
	    Xt_Va_سيٽ_قدرون(tab, Xm_Nuser_معلومات, beval, عدم);
	}
	وإلا
	    tab = صفحو_info.major_tab_widget;

	Xt_Va_سيٽ_قدرون(tab, Xm_Npage_عدد, nr, عدم);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_قدرون(tab, XmNlabelString, &label_str, عدم);
	إذا (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    إذا (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_سيٽ_قدرون(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, عدم);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_ٻار(tab);
		Xt_Manage_ٻار(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_ڳڻپ = nr - 1;

    Xt_Va_Get_قدرون(tab_سٽ, Xm_Nlast_صفحو_عدد, &آخري_صفحو, عدم);

    // Remove any old labels.
    طالما (nr <= آخري_صفحو)
    {
	إذا (Xm_Notebook_Get_صفحو_Info(tab_سٽ, nr, &صفحو_info) != Xm_صفحو_INVALID
	    && صفحو_info.صفحو_عدد == nr
	    && صفحو_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_قدرون(صفحو_info.major_tab_widget, Xm_Nuser_معلومات, &beval, عدم);
	    إذا (beval != عدم)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_ٻار(صفحو_info.major_tab_widget);
	    XtDestroyWidget(صفحو_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_سيٽ_قدرون(tab_سٽ, Xm_Nlast_صفحو_عدد, tab_ڳڻپ, عدم);

    Xt_Va_Get_قدرون(tab_سٽ, Xm_Ncurrent_صفحو_عدد, &currentpage, عدم);
    إذا (currentpage != curtabidx)
	Xt_Va_سيٽ_قدرون(tab_سٽ, Xm_Ncurrent_صفحو_عدد, curtabidx, عدم);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سيٽ_curtab(صحيح nr)
{
    صحيح		currentpage;

    إذا (tab_سٽ == (Widget)0)
	أرجع;

    Xt_Va_Get_قدرون(tab_سٽ, Xm_Ncurrent_صفحو_عدد, &currentpage, عدم);
    إذا (currentpage != nr)
	Xt_Va_سيٽ_قدرون(tab_سٽ, Xm_Ncurrent_صفحو_عدد, nr, عدم);
}
#جيڪڏهن_پورو

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ساكن فراغ
gui_motif_menu_colors(Widget id)
{
    إذا (gui.menu_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخو >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#وإلا
	Xt_Va_سيٽ_قدرون(id, XmNbackground, gui.menu_bg_pixel, عدم);
#جيڪڏهن_پورو
    إذا (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_سيٽ_قدرون(id, XmNforeground, gui.menu_fg_pixel, عدم);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ساكن فراغ
gui_motif_scroll_colors(Widget id)
{
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
#إذا (Xm_نسخو >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#وإلا
	Xt_Va_سيٽ_قدرون(id, XmNbackground, gui.scroll_bg_pixel, عدم);
#جيڪڏهن_پورو
    إذا (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_سيٽ_قدرون(id, XmNforeground, gui.scroll_fg_pixel, عدم);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    فراغ
gui_motif_menu_fontlist(Widget id اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_MENU
# ifdef FONTSET_ALWAYS
    إذا (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_سيٽ *)&gui.menu_fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		Xt_Unmanage_ٻار(id);
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ٻار(id);
	    }
	    وإلا
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
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
		Xt_Unmanage_ٻار(id);
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ٻار(id);
	    }
	    وإلا
		Xt_Va_سيٽ_قدرون(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
#جيڪڏهن_پورو
}


/*
 * We don't create it twice for the sake of speed.
 */

تعريفنوع بنية Shared_ڳوليو_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget مٿي;		// search direction 'Up' radio button
    Widget هيٺ_ڏانهن;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget ڳوليو;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_ڳوليو_Replace_2;

ساكن Shared_ڳوليو_Replace_2 ڳوليو_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن Shared_ڳوليو_Replace_2 repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن فراغ
ڳوليو_replace_destroy_callback(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    Shared_ڳوليو_Replace_2 *cd = (Shared_ڳوليو_Replace_2 *)client_معلومات_2;

    إذا (cd != عدم)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساكن فراغ
ڳوليو_replace_dismiss_callback(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    Shared_ڳوليو_Replace_2 *cd = (Shared_ڳوليو_Replace_2 *)client_معلومات_2;

    إذا (cd != عدم)
	Xt_Unmanage_ٻار(cd->dialog);
}

    ساكن فراغ
داخلا_activate_callback(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    XmProcessTraversal((Widget)client_معلومات_2, XmTRAVERSE_CURRENT);
}

    ساكن فراغ
ڳوليو_replace_callback(
    Widget	w اڻ_استعمال_ٿيل,
    Xt_اشارو	client_معلومات_2,
    Xt_اشارو	call_معلومات اڻ_استعمال_ٿيل)
{
    long_u	flags = (long_u)client_معلومات_2;
    حرف	*ڳوليو_متن, *repl_متن;
    Boolean	direction_هيٺ_ڏانهن = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_ڳوليو_Replace_2 *sfr;

    إذا (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = خالي_جايون_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	أرجع;
    }

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_متن = عدم;
	sfr = &ڳوليو_widgets;
    }
    وإلا
    {
	repl_متن = Xm_متن_ميدان_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    ڳوليو_متن = Xm_متن_ميدان_Get_String(sfr->what);
    Xt_Va_Get_قدرون(sfr->هيٺ_ڏانهن, XmNset, &direction_هيٺ_ڏانهن, عدم);
    Xt_Va_Get_قدرون(sfr->wword, XmNset, &wword, عدم);
    Xt_Va_Get_قدرون(sfr->mcase, XmNset, &mcase, عدم);
    إذا (wword)
	flags |= FRD_سڄو_لفظ;
    إذا (mcase)
	flags |= FRD_ميلاپ_CASE;

    (فراغ)gui_do_findrepl((صحيح)flags, (char_u *)ڳوليو_متن, (char_u *)repl_متن,
							      direction_هيٺ_ڏانهن);

    إذا (ڳوليو_متن != عدم)
	XtFree(ڳوليو_متن);
    إذا (repl_متن != عدم)
	XtFree(repl_متن);
}

    ساكن فراغ
ڳوليو_replace_keypress(
    Widget		w اڻ_استعمال_ٿيل,
    Shared_ڳوليو_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b اڻ_استعمال_ٿيل)
{
    ڪنجي_Sym keysym;

    إذا (frdp == عدم)
	أرجع;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    إذا (keysym == XK_Escape)
	Xt_Unmanage_ٻار(frdp->dialog);
}

    ساكن فراغ
سيٽ_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *ايندڙ;
    ڪنجي_Sym	mnemonic = NUL;

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)label);
    إذا (p == عدم)
	أرجع;
    لكل (ايندڙ = p; *ايندڙ; ++ايندڙ)
    {
	إذا (*ايندڙ == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(ايندڙ);

	    إذا (len > 0)
	    {
		mch_memmove(ايندڙ, ايندڙ + 1, len);
		mnemonic = ايندڙ[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    إذا (str)
    {
	Xt_Va_سيٽ_قدرون(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
ڳوليو_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    Shared_ڳوليو_Replace_2	*frdp;
    Widget		separator;
    Widget		داخلا_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		چوکٺو;
    XmString		str;
    صحيح			n;
    Arg			دليلون[6];
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    Dimension		ويڪر;
    Dimension		widest;
    char_u		*داخلا_متن;

    frdp = do_replace ? &repl_widgets : &ڳوليو_widgets;

    // Get the search string to use.
    داخلا_متن = get_ڳوليو_dialog_متن(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	إذا (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_دري(Xt_والد(frdp->dialog)));
	وإلا
	    Xt_Manage_ٻار(frdp->dialog);
	XtPopup(Xt_والد(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	إذا (داخلا_متن != عدم)
	    Xm_متن_ميدان_سيٽ_String(frdp->what, (حرف *)داخلا_متن);
	vim_free(داخلا_متن);

	Xt_Va_سيٽ_قدرون(frdp->wword, XmNset, wword, عدم);
	أرجع;
    }

    // Create a fresh new dialog window
    إذا (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وإلا
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNautoUnmanage, False); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNnoResize, True); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", دليلون, n);
    XmStringFree(str);
    Xt_جوڙيو_Callback(frdp->dialog, XmNdestroyCallback,
	    ڳوليو_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_وٿي, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وٿي, 4,
	    عدم);

    frdp->ڳوليو = XtVaCreateManagedWidget("findButton",
	    xm_ڌڪو_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);
    سيٽ_label(frdp->ڳوليو, _("Find &Next"));

    Xt_جوڙيو_Callback(frdp->ڳوليو, XmNactivateCallback,
	    ڳوليو_replace_callback,
	    (do_replace ? (Xt_اشارو)FRD_R_FINDNEXT : (Xt_اشارو)FRD_FINDNEXT));

    إذا (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ڌڪو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->ڳوليو,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سيٽ_label(frdp->replace, _("&Replace"));
	Xt_جوڙيو_Callback(frdp->replace, XmNactivateCallback,
		ڳوليو_replace_callback, (Xt_اشارو)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ڌڪو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سيٽ_label(frdp->all, _("Replace &All"));
	Xt_جوڙيو_Callback(frdp->all, XmNactivateCallback,
		ڳوليو_replace_callback, (Xt_اشارو)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ڌڪو_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	سيٽ_label(frdp->undo, _("&Undo"));
	Xt_جوڙيو_Callback(frdp->undo, XmNactivateCallback,
		ڳوليو_replace_callback, (Xt_اشارو)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ڌڪو_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    عدم);
    سيٽ_label(frdp->cancel, _("&Cancel"));
    Xt_جوڙيو_Callback(frdp->cancel, XmNactivateCallback,
	    ڳوليو_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_ٻار(button_form);

    n = 0;
    Xt_سيٽ_Arg(دليلون[n], XmNorientation, XmVERTICAL); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNrightWidget, button_form); n++;
    Xt_سيٽ_Arg(دليلون[n], Xm_Nright_وٿي, 4); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_سيٽ_Arg(دليلون[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", دليلون, n);
    Xt_Manage_ٻار(separator);

    داخلا_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_وٿي, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 4,
	    عدم);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, داخلا_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_وٿي, 4,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_متن_ميدان_Widget_Class, داخلا_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		عدم);

	إذا (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_متن_ميدان_Widget_Class,	داخلا_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_وٿي, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	    Xt_جوڙيو_Callback(frdp->with, XmNactivateCallback,
		    ڳوليو_replace_callback, (Xt_اشارو) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, داخلا_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_وٿي, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_جوڙيو_Callback(frdp->what, XmNactivateCallback,
		    داخلا_activate_callback, frdp->with);
	    Xt_جوڙيو_Event_Handler(frdp->with, ڪنجي_Press_نقاب, False,
			    (XtEventHandler)ڳوليو_replace_keypress,
			    (Xt_اشارو) frdp);

	}
	وإلا
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_جوڙيو_Callback(frdp->what, XmNactivateCallback,
		    ڳوليو_replace_callback, (Xt_اشارو)FRD_FINDNEXT);
	}
	Xt_جوڙيو_Event_Handler(frdp->what, ڪنجي_Press_نقاب, False,
			    (XtEventHandler)ڳوليو_replace_keypress,
			    (Xt_اشارو)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_سيٽ_Arg(دليلون[n], XmNwidth, &ويڪر); n++;
	Xt_Get_قدرون(label_what, دليلون, n);
	widest = ويڪر;
	إذا (do_replace)
	{
	    Xt_Get_قدرون(label_with, دليلون, n);
	    إذا (ويڪر > widest)
		widest = ويڪر;
	}

	Xt_Va_سيٽ_قدرون(frdp->what, Xm_Nleft_وٿي, widest, عدم);
	إذا (do_replace)
	    Xt_Va_سيٽ_قدرون(frdp->with, Xm_Nleft_وٿي, widest, عدم);

    }

    Xt_Manage_ٻار(داخلا_form);

    {
	Widget radio_box;
	Widget w;

	چوکٺو = XtVaCreateWidget("directionFrame",
		xm_چوکٺو_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, داخلا_form,
		Xm_Ntop_وٿي, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_وٿي, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, داخلا_form,
		عدم);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, چوکٺو,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_چوکٺو_TITLE_ٻار,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(چوکٺو, "radioBox",
		(ArgList)عدم, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->مٿي = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->مٿي);

	str = XmStringCreateSimple(_("Down"));
	frdp->هيٺ_ڏانهن = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->هيٺ_ڏانهن);

	Xt_Manage_ٻار(radio_box);
	Xt_Manage_ٻار(چوکٺو);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, چوکٺو,
	    Xm_Nright_وٿي, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, داخلا_form,
	    Xm_Ntop_وٿي, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_وٿي, 4,
	    عدم);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_وٿي, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNset, wword,
	    عدم);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_وٿي, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_وٿي, 4,
	    XmNset, mcase,
	    عدم);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_ٻار(toggle_form);

    إذا (داخلا_متن != عدم)
	Xm_متن_ميدان_سيٽ_String(frdp->what, (حرف *)داخلا_متن);
    vim_free(داخلا_متن);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   فراغ
gui_mch_ڳوليو_dialog(exarg_T *eap)
{
    إذا (!gui.اندريون_use)
	أرجع;

    ڳوليو_replace_dialog_create(eap->arg, FALSE);
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (!gui.اندريون_use)
	أرجع;

    ڳوليو_replace_dialog_create(eap->arg, TRUE);
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
    Shared_ڳوليو_Replace_2 *frdp;
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
	frdp = (do_replace) ? (&repl_widgets) : (&ڳوليو_widgets);
	إذا (frdp->dialog)
	{
	    Xt_Va_سيٽ_قدرون(frdp->what, XmNfontList, font_list, عدم);
	    إذا (do_replace)
		Xt_Va_سيٽ_قدرون(frdp->with, XmNfontList, font_list, عدم);
	}
    }

    XmFontListFree(font_list);
}
