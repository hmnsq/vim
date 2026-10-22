/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#అన్తర్భావ "vim.h"

#అన్తర్భావ <Xm/Form.h>
#అన్తర్భావ <Xm/RowColumn.h>
#అన్తర్భావ <Xm/PushB.h>
#అన్తర్భావ <Xm/Text.h>
#అన్తర్భావ <Xm/TextF.h>
#అన్తర్భావ <Xm/Separator.h>
#అన్తర్భావ <Xm/Label.h>
#అన్తర్భావ <Xm/CascadeB.h>
#అన్తర్భావ <Xm/ScrollBar.h>
#అన్తర్భావ <Xm/MenuShell.h>
#అన్తర్భావ <Xm/DrawingA.h>
#ఒకవేళ (Xm_సంస్కరణ >= 1002)
# include <Xm/RepType.h>
#అయితే_ముగింపు
#అన్తర్భావ <Xm/Frame.h>
#అన్తర్భావ <Xm/LabelG.h>
#అన్తర్భావ <Xm/ToggleBG.h>
#అన్తర్భావ <Xm/SeparatoG.h>
#అన్తర్భావ <Xm/XmP.h>

#అన్తర్భావ <X11/keysym.h>
#అన్తర్భావ <X11/Xatom.h>
#అన్తర్భావ <X11/StringDefs.h>
#అన్తర్భావ <X11/Intrinsic.h>
#నిర్వచితమైతే HAVE_X11_XPM_H
# ఒకవేళ defined(VMS)
#  include <xpm.h>
# లేకపోతే
#  include <X11/xpm.h>
# endif
#లేకపోతే
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#అయితే_ముగింపు

#అన్తర్భావ "gui_xmebw.h"	// for our Enhanced Button Widget

#ఒకవేళ defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#అయితే_ముగింపు

#నిర్వచించు MOTIF_POPUP

బాహ్య Widget vimShell;

స్థిర Widget vimForm;
స్థిర Widget పాఠ్యం_Area_Form;
Widget పాఠ్యం_Area;
#నిర్వచితమైతే FEAT_TOOLBAR
స్థిర Widget tool_Bar_చట్రం;
స్థిర Widget toolBar;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
స్థిర Widget	tab_పంక్తి;
స్థిర Widget	tab_పంక్తి_menu = 0;
స్థిర పూర్ణాంకం	showing_tabline = 0;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
// remember the last set value for the tearoff item
స్థిర పూర్ణాంకం tearoff_val = (పూర్ణాంకం)XmTEAR_OFF_ENABLED;
# endif
స్థిర Widget menuBar;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
స్థిర శూన్య reset_focus(శూన్య);
#అయితే_ముగింపు

స్థిర శూన్య gui_motif_menu_colors(Widget id);
స్థిర శూన్య gui_motif_scroll_colors(Widget id);

#ఒకవేళ (Xm_సంస్కరణ >= 1002)
# define STRING_ట్యాగ్  Xm_FONTLIST_DEFAULT_ట్యాగ్
#లేకపోతే
# define STRING_ట్యాగ్  XmSTRING_DEFAULT_CHARSET
#అయితే_ముగింపు

/*
 * Call-back routines.
 */

    స్థిర శూన్య
scroll_cb(Widget w వాడని, Xt_సూచిక client_దత్తాంశం_2, Xt_సూచిక call_దత్తాంశం)
{
    scrollbar_T *sb;
    దీర్ఘ	విలువ;
    పూర్ణాంకం		dragging;

    sb = gui_కనుగొను_scrollbar((దీర్ఘ)client_దత్తాంశం_2);

    విలువ = ((XmScrollBarCallbackStruct *)call_దత్తాంశం)->విలువ;
    dragging = (((XmScrollBarCallbackStruct *)call_దత్తాంశం)->reason ==
							      (పూర్ణాంకం)XmCR_DRAG);
    gui_drag_scrollbar(sb, విలువ, dragging);
}

#నిర్వచితమైతే FEAT_GUI_TABLINE
    స్థిర శూన్య
tabline_cb(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,
    Xt_సూచిక	call_దత్తాంశం)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_దత్తాంశం;
    ఒకవేళ (nptr->reason != (పూర్ణాంకం)XmCR_NONE)
	send_tabline_event(nptr->పుట_సంఖ్య);
}

    స్థిర శూన్య
tabline_button_cb(
    Widget	w,
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    Xt_సూచిక	cmd, tab_idx;

    Xt_Va_Get_విలువలు(w, Xm_Nuser_దత్తాంశం, &cmd, ఖాళీ);
    Xt_Va_Get_విలువలు(tab_పంక్తి_menu, Xm_Nuser_దత్తాంశం, &tab_idx, ఖాళీ);

    send_tabline_menu_event((పూర్ణాంకం)(దీర్ఘ)tab_idx, (పూర్ణాంకం)(దీర్ఘ)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    స్థిర శూన్య
motif_tabline_timer_cb (
    Xt_సూచిక		timed_బహిర్గతం,
    XtIntervalId	*interval_id వాడని)
{
    *((పూర్ణాంకం *)timed_బహిర్గతం) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    స్థిర పూర్ణాంకం
tabline_scroller_clicked(
    అక్షరం		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	వెడల్పు, ఎత్తు;

    tab_scroll_w = Xt_Name_గమ్యం_Widget(tab_పంక్తి, scroller_name);
    ఒకవేళ (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_విలువలు(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &వెడల్పు, XmNheight, &ఎత్తు, ఖాళీ);
	ఒకవేళ (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    ఒకవేళ (event->x >= pos_x && event->x <= pos_x + వెడల్పు
		    && event->y >= pos_y && event->y <= pos_y + ఎత్తు)
		// Clicked on the scroller
		తిరిగి TRUE;
	}
    }
    తిరిగి FALSE;
}

    స్థిర శూన్య
tabline_menu_cb(
    Widget	w,
    Xt_సూచిక	సంవృతం వాడని,
    XEvent	*e,
    Boolean	*continue_dispatch వాడని)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    పూర్ణాంకం				tab_idx = 0;
    WidgetList			children;
    Cardinal			అంకె_Children;
    స్థిర XtIntervalId		timer = (XtIntervalId)0;
    స్థిర పూర్ణాంకం			timed_బహిర్గతం = TRUE;

    event = (XButtonPressedEvent *)e;

    ఒకవేళ (event->button == Button1)
    {
	ఒకవేళ (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    తిరిగి;

	ఒకవేళ (!timed_బహిర్గతం)
	{
	    Xt_Remove_సమయం_బహిర్గతం(timer);
	    timed_బహిర్గతం = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	లేకపోతే
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = Xt_App_కూడు_సమయం_బహిర్గతం(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_బహిర్గతం);
	    timed_బహిర్గతం = FALSE;
	}
	తిరిగి;
    }

    ఒకవేళ (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_విలువలు(tab_పంక్తి_menu, Xm_Nuser_దత్తాంశం, &tab_idx, ఖాళీ);
	send_tabline_menu_event(tab_idx, (పూర్ణాంకం)TABLINE_MENU_CLOSE);
	తిరిగి;
    }

    ఒకవేళ (event->button != Button3)
	తిరిగి;

    // When ignoring events don't show the menu.
    ఒకవేళ (hold_gui_events || cmdwin_type != 0)
	తిరిగి;

    ఒకవేళ (event->subwindow != None)
    {
	tab_w = Xt_కిటికీ_గమ్యం_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	ఒకవేళ (tab_w != (Widget)0 && Xm_Is_నెట్టు_Button(tab_w))
	    Xt_Va_Get_విలువలు(tab_w, Xm_Npage_సంఖ్య, &tab_idx, ఖాళీ);
    }

    Xt_Va_సమితి_విలువలు(tab_పంక్తి_menu, Xm_Nuser_దత్తాంశం, (Xt_సూచిక)(దీర్ఘ)tab_idx, ఖాళీ);
    Xt_Va_Get_విలువలు(tab_పంక్తి_menu, XmNchildren, &children, XmNnumChildren,
		  &అంకె_Children, ఖాళీ);
    XtManageChildren(children, అంకె_Children);
    XmMenuPosition(tab_పంక్తి_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_బిడ్డ(tab_పంక్తి_menu);
}

    స్థిర శూన్య
tabline_balloon_cb(BalloonEval *beval, పూర్ణాంకం state వాడని)
{
    పూర్ణాంకం		nr;
    tabpage_T	*tp;

    ఒకవేళ (beval->లక్ష్యం == (Widget)0)
	తిరిగి;

    Xt_Va_Get_విలువలు(beval->లక్ష్యం, Xm_Npage_సంఖ్య, &nr, ఖాళీ);
    tp = కనుగొను_tabpage(nr);
    ఒకవేళ (tp == ఖాళీ)
	తిరిగి;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#అయితే_ముగింపు

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#అన్తర్భావ <Xm/XmP.h>
#అన్తర్భావ <Xm/LabelP.h>

స్థిర XtExposeProc old_label_expose = ఖాళీ;

    స్థిర శూన్య
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    గుర్తులేని అక్షరం   label_type = (పూర్ణాంకం)XmSTRING;

    Xt_Va_Get_విలువలు(_w, XmNlabelType, &label_type, (Xt_సూచిక)0);

    ఒకవేళ (XtIsSensitive(_w) || label_type != (పూర్ణాంకం)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    లేకపోతే
    {
	XGCValues   విలువలు;
	XtGCMask    ముసుగు;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	ముసుగు = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_ముసుగు | GCClipXOrigin | GCClipYOrigin;
	విలువలు.graphics_exposures = False;

	ఒకవేళ (fs != 0)
	{
	    ముసుగు |= GCFont;
	    విలువలు.font = fs->fid;
	}

	ఒకవేళ (lw->primitive.శిఖరం_shadow_pixmap != None
		&& lw->primitive.శిఖరం_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    ముసుగు |= GCFillStyle | GCTile;
	    విలువలు.fill_style = FillTiled;
	    విలువలు.tile = lw->primitive.శిఖరం_shadow_pixmap;
	}

	lw->label.పాఠ్యం_Rect.x += 1;
	lw->label.పాఠ్యం_Rect.y += 1;
	ఒకవేళ (lw->label.acc_పాఠ్యం != 0)
	{
	    lw->label.acc_పాఠ్యం_Rect.x += 1;
	    lw->label.acc_పాఠ్యం_Rect.y += 1;
	}

	విలువలు.foreground = lw->primitive.శిఖరం_shadow_color;
	విలువలు.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, ముసుగు,
					       &విలువలు, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.పాఠ్యం_Rect.x -= 1;
	lw->label.పాఠ్యం_Rect.y -= 1;
	ఒకవేళ (lw->label.acc_పాఠ్యం != 0)
	{
	    lw->label.acc_పాఠ్యం_Rect.x -= 1;
	    lw->label.acc_పాఠ్యం_Rect.y -= 1;
	}

	విలువలు.foreground = lw->primitive.bottom_shadow_color;
	విలువలు.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, ముసుగు,
					       &విలువలు, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    శూన్య
gui_x11_create_widgets(శూన్య)
{
#నిర్వచితమైతే FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		వాదనలు[10];
    పూర్ణాంకం		n;
    XmString	xms;
#అయితే_ముగింపు

    /*
     * Install the 3D shade effect drawing routines.
     */
    ఒకవేళ (old_label_expose == ఖాళీ)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_స్థానభ్రంశం = gui.border_వెడల్పు;

    /*
     * Install the tearOffModel resource converter.
     */
#ఒకవేళ (Xm_సంస్కరణ >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#అయితే_ముగింపు

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_సమితి_విలువలు(vimShell, XmNdeleteResponse, XmDO_NOTHING, ఖాళీ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_వెడల్పు, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_వెడల్పు, 0,
	Xm_Nmargin_ఎత్తు, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ఖాళీ);
    gui_motif_menu_colors(vimForm);

#నిర్వచితమైతే FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	పూర్ణాంకం ac = 0;

# ఒకవేళ (Xm_సంస్కరణ >= 1002)
	Xt_సమితి_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_సమితి_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_సమితి_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_సమితి_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_సమితి_Arg(al[ac], Xm_Nright_స్థానభ్రంశం, 0); ac++;
# endif
	Xt_సమితి_Arg(al[ac], Xm_Nmargin_ఎత్తు, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_బిడ్డ(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_చట్రం = XtVaCreateWidget("toolBarFrame",
	xm_చట్రం_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_ఎత్తు, 0,
	Xm_Nmargin_వెడల్పు, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	ఖాళీ);
    gui_motif_menu_colors(tool_Bar_చట్రం);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_అడ్డువరుస_స్తంభం_Widget_Class, tool_Bar_చట్రం,
	XmNchildType, Xm_చట్రం_WORKAREA_బిడ్డ,
	Xm_Nrow_స్తంభం_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_ఎత్తు, 0,
	Xm_Nmargin_వెడల్పు, 0,
	Xm_Nadjust_చివరి, True,
	ఖాళీ);
    gui_motif_menu_colors(toolBar);

#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNbindingType, XmNONE); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNorientation, XmVERTICAL); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nback_పుట_కొలత, XmNONE); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nback_పుట_సంఖ్య, 0); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nback_పుట_Placement, Xm_శిఖరం_కుడి); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNmajorTabSpacing, 0); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNshadowThickness, 0); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_పంక్తి = XmCreateNotebook(vimForm, "Vim tabline", వాదనలు, n);

    Xt_కూడు_Callback(tab_పంక్తి, Xm_Npage_మారింది_Callback, (XtCallbackProc)tabline_cb,
			ఖాళీ);
    Xt_కూడు_Event_Handler(tab_పంక్తి, Button_Press_ముసుగు, False,
			(XtEventHandler)tabline_menu_cb, ఖాళీ);

    gui.tabline_ఎత్తు = TABLINE_ఎత్తు;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_గమ్యం_Widget(tab_పంక్తి, "MinorTabScrollerNext");
    Xt_Va_సమితి_విలువలు(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ఖాళీ);
    scroller = Xt_Name_గమ్యం_Widget(tab_పంక్తి, "MinorTabScrollerPrevious");
    Xt_Va_సమితి_విలువలు(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ఖాళీ);

    // Create the tabline popup menu
    tab_పంక్తి_menu = XmCreatePopupMenu(tab_పంక్తి, "tabline popup", ఖాళీ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nuser_దత్తాంశం, (Xt_సూచిక)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((అక్షరం *)"Close tab", STRING_ట్యాగ్);
    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, xms); n++;
    button = Xm_Create_నెట్టు_Button(tab_పంక్తి_menu, "Close", వాదనలు, n);
    Xt_కూడు_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ఖాళీ);
    XmStringFree(xms);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nuser_దత్తాంశం, (Xt_సూచిక)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((అక్షరం *)"New Tab", STRING_ట్యాగ్);
    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, xms); n++;
    button = Xm_Create_నెట్టు_Button(tab_పంక్తి_menu, "New Tab", వాదనలు, n);
    Xt_కూడు_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ఖాళీ);
    XmStringFree(xms);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nuser_దత్తాంశం, (Xt_సూచిక)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((అక్షరం *)"Open tab...", STRING_ట్యాగ్);
    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, xms); n++;
    button = Xm_Create_నెట్టు_Button(tab_పంక్తి_menu, "Open tab...", వాదనలు, n);
    Xt_కూడు_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ఖాళీ);
    XmStringFree(xms);
#అయితే_ముగింపు

    పాఠ్యం_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_వెడల్పు, 0,
	Xm_Nmargin_ఎత్తు, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ఖాళీ);
    gui_motif_scroll_colors(పాఠ్యం_Area_Form);

    పాఠ్యం_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, పాఠ్యం_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.వెనుక_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_వెడల్పు, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	ఖాళీ);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(పాఠ్యం_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.లోపల_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    శూన్య
gui_x11_destroy_widgets(శూన్య)
{
    పాఠ్యం_Area = ఖాళీ;
#నిర్వచితమైతే FEAT_MENU
    menuBar = ఖాళీ;
#అయితే_ముగింపు
}

    శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(
    పూర్ణాంకం	    x వాడని,
    పూర్ణాంకం	    y వాడని,
    పూర్ణాంకం	    w వాడని,
    పూర్ణాంకం	    h వాడని)
{
#నిర్వచితమైతే FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#అయితే_ముగింపు
}

    శూన్య
gui_x11_సమితి_వెనుక_color(శూన్య)
{
    ఒకవేళ (పాఠ్యం_Area != ఖాళీ)
#ఒకవేళ (Xm_సంస్కరణ >= 1002)
	XmChangeColor(పాఠ్యం_Area, gui.వెనుక_pixel);
#లేకపోతే
	Xt_Va_సమితి_విలువలు(పాఠ్యం_Area,
		  XmNbackground, gui.వెనుక_pixel,
		  ఖాళీ);
#అయితే_ముగింపు
}

/*
 * Manage dialog centered on pointer.
 */
    శూన్య
manage_centered(Widget dialog_బిడ్డ)
{
    Widget shell = Xt_మాతృ(dialog_బిడ్డ);
    కిటికీ మూలం, బిడ్డ;
    గుర్తులేని పూర్ణాంకం ముసుగు;
    గుర్తులేని పూర్ణాంకం వెడల్పు, ఎత్తు, border_వెడల్పు, లోతు;
    పూర్ణాంకం x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_విలువలు(shell, XmNmappedWhenManaged, &mappedWhenManaged, ఖాళీ);
    Xt_Va_సమితి_విలువలు(shell, XmNmappedWhenManaged, False, ఖాళీ);

    Xt_Manage_బిడ్డ(dialog_బిడ్డ);

    // Get the pointer position (x, y)
    XQuery_సూచిక(XtDisplay(shell), Xt_కిటికీ(shell), &మూలం, &బిడ్డ,
		  &x, &y, &win_x, &win_y, &ముసుగు);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_కిటికీ(shell), &మూలం, &win_x, &win_y,
		 &వెడల్పు, &ఎత్తు, &border_వెడల్పు, &లోతు);
    వెడల్పు += 2 * border_వెడల్పు;
    ఎత్తు += 2 * border_వెడల్పు;
    x -= వెడల్పు / 2;
    y -= ఎత్తు / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->వెడల్పు - వెడల్పు;
    maxY = XtScreen(shell)->ఎత్తు - ఎత్తు;
    ఒకవేళ (x < 0)
	x = 0;
    ఒకవేళ (x > maxX)
	x = maxX;
    ఒకవేళ (y < 0)
	y = 0;
    ఒకవేళ (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_సమితి_విలువలు(shell, XmNx, x, XmNy, y, ఖాళీ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_సమితి_విలువలు(shell, XmNmappedWhenManaged, mappedWhenManaged, ఖాళీ);
}

#ఒకవేళ defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# ఒకవేళ (Xm_సంస్కరణ <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_ట్యాగ్);
# లేకపోతే
    // Motif 1.2 method
    Xm_Font_List_నమోదు font_list_నమోదు;

    font_list_నమోదు = Xm_Font_List_నమోదు_Create(STRING_ట్యాగ్, XmFONT_IS_FONT,
					    (Xt_సూచిక)font);
    font_list = Xm_Font_List_Append_నమోదు(ఖాళీ, font_list_నమోదు);
    Xm_Font_List_నమోదు_Free(&font_list_నమోదు);
# endif
    తిరిగి font_list;
}

# ఒకవేళ ((Xm_సంస్కరణ > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_సమితి *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_నమోదు font_list_నమోదు;

    font_list_నమోదు = Xm_Font_List_నమోదు_Create(STRING_ట్యాగ్,
					    XmFONT_IS_FONTSET,
					    (Xt_సూచిక)*fontset);
    font_list = Xm_Font_List_Append_నమోదు(ఖాళీ, font_list_నమోదు);
    Xm_Font_List_నమోదు_Free(&font_list_నమోదు);
    తిరిగి font_list;
}
# endif

#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

స్థిర శూన్య gui_motif_కూడు_actext(vimmenu_T *menu);
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
స్థిర శూన్య toggle_tearoff(Widget wid);
స్థిర శూన్య gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
స్థిర శూన్య submenu_change(vimmenu_T *mp, పూర్ణాంకం colors);

స్థిర శూన్య do_సమితి_mnemonics(పూర్ణాంకం enable);
స్థిర పూర్ణాంకం menu_enabled = TRUE;

    శూన్య
gui_mch_enable_menu(పూర్ణాంకం flag)
{
    ఒకవేళ (flag)
    {
	Xt_Manage_బిడ్డ(menuBar);
# ifdef FEAT_TOOLBAR
	ఒకవేళ (XtIsManaged(Xt_మాతృ(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_సమితి_విలువలు(Xt_మాతృ(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ఖాళీ);
#  ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar),
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
#  endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar),
			      ఖాళీ);
	}
	లేకపోతే
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ఖాళీ);
	}
    }
    లేకపోతే
    {
	Xt_Unmanage_బిడ్డ(menuBar);
# ifdef FEAT_TOOLBAR
	ఒకవేళ (XtIsManaged(Xt_మాతృ(toolBar)))
	{
	    Xt_Va_సమితి_విలువలు(Xt_మాతృ(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ఖాళీ);
#  ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar),
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
#  endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar),
			      ఖాళీ);
	}
	లేకపోతే
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_FORM,
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ఖాళీ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    శూన్య
gui_motif_సమితి_mnemonics(పూర్ణాంకం enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    ఒకవేళ (!menu_enabled)
	enable = FALSE;
    do_సమితి_mnemonics(enable);
}

    స్థిర శూన్య
do_సమితి_mnemonics(పూర్ణాంకం enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	ఒకవేళ (menu->id != (Widget)0)
	    Xt_Va_సమితి_విలువలు(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ఖాళీ);
}

    శూన్య
gui_mch_కూడు_menu(vimmenu_T *menu, పూర్ణాంకం idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*మాతృ = menu->మాతృ;

# ifdef MOTIF_POPUP
    ఒకవేళ (menu_is_popup(menu->name))
    {
	Arg arg[2];
	పూర్ణాంకం n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  ఒకవేళ (Xm_సంస్కరణ <= 1002)
	ఒకవేళ (mouse_model_popup())
#  endif
	{
	    ఒకవేళ (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_సమితి_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    ఒకవేళ (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_సమితి_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(పాఠ్యం_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	తిరిగి;
    }
# endif

    ఒకవేళ (!menu_is_menubar(menu->name)
	    || (మాతృ != ఖాళీ && మాతృ->submenu_id == (Widget)0))
	తిరిగి;

    label = XmStringCreate((అక్షరం *)menu->dname, STRING_ట్యాగ్);
    ఒకవేళ (label == ఖాళీ)
	తిరిగి;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (మాతృ == ఖాళీ) ? menuBar : మాతృ->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_సూచీ, idx + (మాతృ != ఖాళీ
			   && tearoff_val == (పూర్ణాంకం)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ఖాళీ);
    XmStringFree(label);

    ఒకవేళ (menu->id == (Widget)0)		// failed
	తిరిగి;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    ఒకవేళ (మాతృ == ఖాళీ && menu->ప్రాధాన్యత >= 9999)
	Xt_Va_సమితి_విలువలు(menuBar,
		XmNmenuHelpWidget, menu->id,
		ఖాళీ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_కూడు_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ఖాళీ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_అడ్డువరుస_స్తంభం_Widget_Class, shell,
	Xm_Nrow_స్తంభం_Type, XmMENU_PULLDOWN,
	ఖాళీ);
    gui_motif_menu_colors(menu->submenu_id);

    ఒకవేళ (menu->submenu_id == (Widget)0)		// failed
	తిరిగి;

# ఒకవేళ (Xm_సంస్కరణ >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_సమితి_విలువలు(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ఖాళీ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    ఒకవేళ (మాతృ == ఖాళీ)
	gui_mch_compute_menu_ఎత్తు(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    స్థిర శూన్య
gui_motif_కూడు_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    ఒకవేళ (menu->actext == ఖాళీ || menu->id == (Widget)0)
	తిరిగి;

    label = XmStringCreate((అక్షరం *)menu->actext, STRING_ట్యాగ్);
    ఒకవేళ (label == ఖాళీ)
	తిరిగి;
    Xt_Va_సమితి_విలువలు(menu->id, Xm_Naccelerator_పాఠ్యం, label, ఖాళీ);
    XmStringFree(label);
}

    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable)
{
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
    ఒకవేళ (enable)
	tearoff_val = (పూర్ణాంకం)XmTEAR_OFF_ENABLED;
    లేకపోతే
	tearoff_val = (పూర్ణాంకం)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(మూలం_menu);
# endif
}

# ఒకవేళ (Xm_సంస్కరణ >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    స్థిర శూన్య
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_సమితి_విలువలు(wid, XmNtearOffModel, tearoff_val, ఖాళీ);
    ఒకవేళ (tearoff_val == (పూర్ణాంకం)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    స్థిర శూన్య
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    అయితే (menu != ఖాళీ)
    {
	ఒకవేళ (!menu_is_popup(menu->name))
	{
	    ఒకవేళ (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->తదుపరి;
    }
}
# endif

    పూర్ణాంకం
gui_mch_పాఠ్యం_area_extra_ఎత్తు(శూన్య)
{
    Dimension	shadow_ఎత్తు;

    Xt_Va_Get_విలువలు(పాఠ్యం_Area_Form, XmNshadowThickness, &shadow_ఎత్తు, ఖాళీ);
    తిరిగి shadow_ఎత్తు;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    శూన్య
gui_mch_compute_menu_ఎత్తు(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    స్థిర Dimension	ఎత్తు = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    ఒకవేళ (id != (Widget)0)
	Xt_Va_Get_విలువలు(id, XmNheight, &ఎత్తు, ఖాళీ);

    // Find any menu Widget, to be able to call XtManageChild()
    లేకపోతే
	FOR_ALL_MENUS(mp)
	    ఒకవేళ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ఆపు;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    ఒకవేళ (id != (Widget)0)
	Xt_Manage_బిడ్డ(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	ఒకవేళ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_విలువలు(mp->id, XmNy, &y, ఖాళీ);
	    ఒకవేళ (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_విలువలు(menuBar,
	Xm_Nmargin_ఎత్తు, &margin,
	XmNshadowThickness, &shadow,
	ఖాళీ);

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
    gui.menu_ఎత్తు = maxy + ఎత్తు - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    ఒకవేళ (gui.shell_created)
	Xt_Va_సమితి_విలువలు(menuBar, XmNheight, gui.menu_ఎత్తు, ఖాళీ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

స్థిర అక్షరం **get_toolbar_pixmap(vimmenu_T *menu, అక్షరం **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
check_xpm(char_u *మార్గం)
{
    Xpm_లక్షణాలు attrs;
    పూర్ణాంకం		స్థితి_2;
    Pixmap	ముసుగు;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    స్థితి_2 = Xpm_Read_File_గమ్యం_Pixmap(gui.dpy,
	    మూలం_కిటికీ_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (అక్షరం *)మార్గం, &map, &ముసుగు, &attrs);
    Xpm_Free_లక్షణాలు(&attrs);

    ఒకవేళ (స్థితి_2 == XpmSuccess)
	తిరిగి OK;
    తిరిగి FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    స్థిర అక్షరం **
get_toolbar_pixmap(vimmenu_T *menu, అక్షరం **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    అక్షరం	**xpm = ఖాళీ;		// xpm array
    పూర్ణాంకం		res;

    *fname = ఖాళీ;
    buf[0] = NUL;			// start with NULL path

    ఒకవేళ (menu->iconfile != ఖాళీ)
    {
	// Use the "icon="  argument.
	gui_కనుగొను_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	ఒకవేళ (res == FAIL && gui_కనుగొను_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	ఒకవేళ (res == OK)
	{
	    *fname = (అక్షరం *)vim_strsave(buf);
	    తిరిగి tb_blank_xpm;
	}
    }

    ఒకవేళ (menu->icon_builtin || gui_కనుగొను_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	ఒకవేళ (menu->iconidx >= 0 && menu->iconidx
	       < (పూర్ణాంకం)(పరిమాణం(built_లోపల_pixmaps) / పరిమాణం(built_లోపల_pixmaps[0])))
	    xpm = built_లోపల_pixmaps[menu->iconidx];
	లేకపోతే
	    xpm = tb_blank_xpm;
    }

    తిరిగి xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    స్థిర పూర్ణాంకం
కూడు_pixmap_వాదనలు(vimmenu_T *menu, Arg *వాదనలు, పూర్ణాంకం n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    ఒకవేళ (menu->xpm == ఖాళీ)
    {
	Xt_సమితి_Arg(వాదనలు[n], XmNlabelType, XmSTRING); n++;
    }
    లేకపోతే
    {
	ఒకవేళ (menu->xpm_fname != ఖాళీ)
	{
	    Xt_సమితి_Arg(వాదనలు[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_సమితి_Arg(వాదనలు[n], Xm_Npixmap_దత్తాంశం, menu->xpm); n++;
	Xt_సమితి_Arg(వాదనలు[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    తిరిగి n;
}
# endif // FEAT_TOOLBAR

    శూన్య
gui_mch_కూడు_menu_అంశం(vimmenu_T *menu, పూర్ణాంకం idx)
{
    XmString	label;
    vimmenu_T	*మాతృ = menu->మాతృ;

# ఒకవేళ (Xm_సంస్కరణ <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    ఒకవేళ (menu_is_బిడ్డ_యొక్క_popup(menu) && !mouse_model_popup())
	తిరిగి;
# endif

# ifdef FEAT_TOOLBAR
    ఒకవేళ (menu_is_toolbar(మాతృ->name))
    {
	WidgetClass	type;
	XmString	xms = ఖాళీ;    // fallback label if pixmap not found
	పూర్ణాంకం		n;
	Arg		వాదనలు[18];

	n = 0;
	ఒకవేళ (menu_is_separator(menu->name))
	{
	    అక్షరం	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (అక్షరం *)vim_strchr(menu->name, ':');
	    ఒకవేళ (cp != ఖాళీ)
		wid = (Dimension)atoi(++cp);
	    లేకపోతే
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    Xt_సమితి_Arg(వాదనలు[n], XmNwidth, wid); n++;
	    Xt_సమితి_Arg(వాదనలు[n], Xm_Nmin_వెడల్పు, wid); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNorientation, XmVERTICAL); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNseparatorType, Xm_SHADOW_ETCHED_లోపల); n++;
	}
	లేకపోతే
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_సమితి_Arg(వాదనలు[n], XmNhighlightThickness, 1); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNhighlightOnEnter, True); n++;
	    Xt_సమితి_Arg(వాదనలు[n], Xm_Nmargin_వెడల్పు, 0); n++;
	    Xt_సమితి_Arg(వాదనలు[n], Xm_Nmargin_ఎత్తు, 0); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((అక్షరం *)menu->dname, STRING_ట్యాగ్);
	    Xt_సమితి_Arg(వాదనలు[n], XmNlabelString, xms); n++;

	    n = కూడు_pixmap_వాదనలు(menu, వాదనలు, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_సమితి_Arg(వాదనలు[n], Xm_Nposition_సూచీ, idx); n++;
	ఒకవేళ (menu->id == ఖాళీ)
	{
	    menu->id = XtCreateManagedWidget((అక్షరం *)menu->dname,
			type, toolBar, వాదనలు, n);
	    ఒకవేళ (menu->id != ఖాళీ && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_కూడు_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	లేకపోతే
	    Xt_సమితి_విలువలు(menu->id, వాదనలు, n);
	ఒకవేళ (xms != ఖాళీ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_సమితి_tip(menu);
#  endif

	menu->మాతృ = మాతృ;
	menu->submenu_id = ఖాళీ;
	// When adding first item to toolbar it might have to be enabled .
	ఒకవేళ (!XtIsManaged(Xt_మాతృ(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_ఎత్తు = gui_mch_compute_toolbar_ఎత్తు();
	తిరిగి;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    ఒకవేళ (మాతృ->submenu_id == (Widget)0)
	తిరిగి;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    ఒకవేళ (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, మాతృ->submenu_id,
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_సూచీ, idx + (tearoff_val == (పూర్ణాంకం)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ఖాళీ);
	gui_motif_menu_colors(menu->id);
	తిరిగి;
    }

    label = XmStringCreate((అక్షరం *)menu->dname, STRING_ట్యాగ్);
    ఒకవేళ (label == ఖాళీ)
	తిరిగి;
    menu->id = XtVaCreateWidget("subMenu",
	xm_నెట్టు_Button_Widget_Class, మాతృ->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_సూచీ, idx + (tearoff_val == (పూర్ణాంకం)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ఖాళీ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    ఒకవేళ (menu->id != (Widget)0)
    {
	Xt_కూడు_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_సూచిక)menu);
	// add accelerator text
	gui_motif_కూడు_actext(menu);
    }
}

# ఒకవేళ (Xm_సంస్కరణ <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    శూన్య
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    పూర్ణాంకం		idx = 0;

    // When GUI hasn't started the menus have not been created.
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    అయితే (menu)
    {
      ఒకవేళ (menu->children != ఖాళీ)
      {
	  ఒకవేళ (menu_is_popup(menu->name))
	  {
	      ఒకవేళ (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_కూడు_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      లేకపోతే
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      లేకపోతే ఒకవేళ (menu_is_బిడ్డ_యొక్క_popup(menu))
      {
	  ఒకవేళ (mouse_model_popup())
	      gui_mch_కూడు_menu_అంశం(menu, idx);
	  లేకపోతే
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->తదుపరి;
      ++idx;
    }
}
# endif

    శూన్య
gui_mch_new_menu_colors(శూన్య)
{
    ఒకవేళ (menuBar == (Widget)0)
	తిరిగి;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(tool_Bar_చట్రం);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(మూలం_menu, TRUE);
}

    శూన్య
gui_mch_new_menu_font(శూన్య)
{
    ఒకవేళ (menuBar == (Widget)0)
	తిరిగి;
    submenu_change(మూలం_menu, FALSE);
    {
	Dimension   ఎత్తు;
	Position w, h;

	Xt_Va_Get_విలువలు(menuBar, XmNheight, &ఎత్తు, ఖాళీ);
	gui.menu_ఎత్తు = ఎత్తు;

	Xt_Va_Get_విలువలు(vimShell, XtNwidth, &w, XtNheight, &h, ఖాళీ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_స్థితి_area_ఎత్తు()
# endif
		     );
    }
    gui_సమితి_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# ఒకవేళ defined(FEAT_BEVAL_GUI)
    శూన్య
gui_mch_new_tooltip_font(శూన్య)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    ఒకవేళ (toolBar == (Widget)0)
	తిరిగి;

    menu = gui_కనుగొను_menu((char_u *)"ToolBar");
    ఒకవేళ (menu != ఖాళీ)
	submenu_change(menu, FALSE);
#  endif
}

    శూన్య
gui_mch_new_tooltip_colors(శూన్య)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    ఒకవేళ (toolBar == (Widget)0)
	తిరిగి;

    toolbar = gui_కనుగొను_menu((char_u *)"ToolBar");
    ఒకవేళ (toolbar != ఖాళీ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    స్థిర శూన్య
submenu_change(
    vimmenu_T	*menu,
    పూర్ణాంకం		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    కొరకు (mp = menu; mp != ఖాళీ; mp = mp->తదుపరి)
    {
	ఒకవేళ (mp->id != (Widget)0)
	{
	    ఒకవేళ (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		ఒకవేళ (mp->xpm != ఖాళీ)
		{
		    పూర్ణాంకం		n = 0;
		    Arg		వాదనలు[18];

		    n = కూడు_pixmap_వాదనలు(mp, వాదనలు, n);
		    Xt_సమితి_విలువలు(mp->id, వాదనలు, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ఒకవేళ (mp->tip != ఖాళీ)
		{
		    Arg వాదనలు[2];

		    వాదనలు[0].name = XmNbackground;
		    వాదనలు[0].విలువ = gui.tooltip_bg_pixel;
		    వాదనలు[1].name = XmNforeground;
		    వాదనలు[1].విలువ = gui.tooltip_fg_pixel;
		    Xt_సమితి_విలువలు(mp->tip->balloonLabel, &వాదనలు[0], Xt_సంఖ్య(వాదనలు));
		}
#  endif
# endif
	    }
	    లేకపోతే
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ఒకవేళ (mp->tip != ఖాళీ)
		{
		    Arg వాదనలు[1];

		    వాదనలు[0].name = XmNfontList;
		    వాదనలు[0].విలువ = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_సమితి_విలువలు(mp->tip->balloonLabel, &వాదనలు[0], Xt_సంఖ్య(వాదనలు));
		}
# endif
	    }
	}

	ఒకవేళ (mp->children != ఖాళీ)
	{
# ఒకవేళ (Xm_సంస్కరణ >= 1002)
	    // Set the colors/font for the tear off widget
	    ఒకవేళ (mp->submenu_id != (Widget)0)
	    {
		ఒకవేళ (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		లేకపోతే
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
    శూన్య
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    ఒకవేళ (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    ఒకవేళ (menu->id == (Widget)0)
	తిరిగి;

    Widget	    మాతృ;

    మాతృ = Xt_మాతృ(menu->id);
# ఒకవేళ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    ఒకవేళ (మాతృ == toolBar && menu->tip != ఖాళీ)
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
	menu->tip = ఖాళీ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    ఒకవేళ (మాతృ == menuBar)
	gui_mch_compute_menu_ఎత్తు((Widget)0);
# ifdef FEAT_TOOLBAR
    లేకపోతే ఒకవేళ (మాతృ == toolBar)
    {
	Cardinal    అంకె_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_విలువలు(toolBar, XmNnumChildren, &అంకె_children, ఖాళీ);
	ఒకవేళ (అంకె_children == 0)
	    gui_mch_show_toolbar(FALSE);
	లేకపోతే
	    gui.toolbar_ఎత్తు = gui_mch_compute_toolbar_ఎత్తు();
    }
# endif
}

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu వాడని)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_చివరి_mouse_event());
    Xt_Manage_బిడ్డ(menu->submenu_id);
# endif
}

#అయితే_ముగింపు // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    శూన్య
gui_mch_def_colors(శూన్య)
{
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#నిర్వచితమైతే FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#అయితే_ముగింపు
}


/*
 * Scrollbar stuff.
 */

    శూన్య
gui_mch_సమితి_scrollbar_thumb(
    scrollbar_T *sb,
    దీర్ఘ	val,
    దీర్ఘ	కొలత,
    దీర్ఘ	max)
{
    ఒకవేళ (sb->id != (Widget)0)
	Xt_Va_సమితి_విలువలు(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_కొలత, కొలత,
		  XmNpageIncrement, (కొలత > 2 ? కొలత - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ఖాళీ);
}

    శూన్య
gui_mch_సమితి_scrollbar_pos(
    scrollbar_T *sb,
    పూర్ణాంకం		x,
    పూర్ణాంకం		y,
    పూర్ణాంకం		w,
    పూర్ణాంకం		h)
{
    ఒకవేళ (sb->id == (Widget)0)
	తిరిగి;

    ఒకవేళ (sb->type == SBAR_ఎడమ || sb->type == SBAR_కుడి)
    {
	ఒకవేళ (y == 0)
	    h -= gui.border_స్థానభ్రంశం;
	లేకపోతే
	    y -= gui.border_స్థానభ్రంశం;
	Xt_Va_సమితి_విలువలు(sb->id,
		Xm_Ntop_స్థానభ్రంశం, y,
		Xm_Nbottom_స్థానభ్రంశం, -y - h,
		XmNwidth, w,
		ఖాళీ);
    }
    లేకపోతే
	Xt_Va_సమితి_విలువలు(sb->id,
		Xm_Ntop_స్థానభ్రంశం, y,
		Xm_Nleft_స్థానభ్రంశం, x,
		Xm_Nright_స్థానభ్రంశం, gui.which_scrollbars[SBAR_కుడి]
						     ? gui.scrollbar_వెడల్పు : 0,
		XmNheight, h,
		ఖాళీ);
    Xt_Manage_బిడ్డ(sb->id);
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    పూర్ణాంకం xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_విలువలు(పాఠ్యం_Area, XtNwidth, &tw, XtNx, &tx, ఖాళీ);
    Xt_Va_Get_విలువలు(vimShell, XtNwidth, &ww, ఖాళీ);
    xpad = ww - tw - tx - gui.scrollbar_వెడల్పు;
    తిరిగి (xpad < 0) ? 0 : xpad;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    పూర్ణాంకం ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_విలువలు(పాఠ్యం_Area, XtNheight, &th, XtNy, &ty, ఖాళీ);
    Xt_Va_Get_విలువలు(vimShell, XtNheight, &wh, ఖాళీ);
    ypad = wh - th - ty - gui.scrollbar_ఎత్తు;
    తిరిగి (ypad < 0) ? 0 : ypad;
}

    శూన్య
gui_mch_enable_scrollbar(scrollbar_T *sb, పూర్ణాంకం flag)
{
    Arg		వాదనలు[16];
    పూర్ణాంకం		n;

    ఒకవేళ (sb->id == (Widget)0)
	తిరిగి;

    n = 0;
    ఒకవేళ (flag)
    {
	ఎంపిక (sb->type)
	{
	    స్థితి SBAR_ఎడమ:
		Xt_సమితి_Arg(వాదనలు[n], Xm_Nleft_స్థానభ్రంశం, gui.scrollbar_వెడల్పు); n++;
		ఆపు;

	    స్థితి SBAR_కుడి:
		Xt_సమితి_Arg(వాదనలు[n], Xm_Nright_స్థానభ్రంశం, gui.scrollbar_వెడల్పు); n++;
		ఆపు;

	    స్థితి SBAR_BOTTOM:
		Xt_సమితి_Arg(వాదనలు[n], Xm_Nbottom_స్థానభ్రంశం, gui.scrollbar_ఎత్తు);n++;
		ఆపు;
	}
	Xt_సమితి_విలువలు(పాఠ్యం_Area, వాదనలు, n);
	Xt_Manage_బిడ్డ(sb->id);
    }
    లేకపోతే
    {
	ఒకవేళ (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ఎంపిక (sb->type)
	    {
		స్థితి SBAR_ఎడమ:
		    Xt_సమితి_Arg(వాదనలు[n], Xm_Nleft_స్థానభ్రంశం, 0); n++;
		    ఆపు;

		స్థితి SBAR_కుడి:
		    Xt_సమితి_Arg(వాదనలు[n], Xm_Nright_స్థానభ్రంశం, 0); n++;
		    ఆపు;

		స్థితి SBAR_BOTTOM:
		    Xt_సమితి_Arg(వాదనలు[n], Xm_Nbottom_స్థానభ్రంశం, 0);n++;
		    ఆపు;
	    }
	    Xt_సమితి_విలువలు(పాఠ్యం_Area, వాదనలు, n);
	}
	Xt_Unmanage_బిడ్డ(sb->id);
    }
}

    శూన్య
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    పూర్ణాంకం		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		వాదనలు[16];
    పూర్ణాంకం		n = 0;

    Xt_సమితి_Arg(వాదనలు[n], XmNminimum, 0); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ఎంపిక (sb->type)
    {
	స్థితి SBAR_ఎడమ:
	    Xt_సమితి_Arg(వాదనలు[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNwidth, gui.scrollbar_వెడల్పు); n++;
	    ఆపు;

	స్థితి SBAR_కుడి:
	    Xt_సమితి_Arg(వాదనలు[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNwidth, gui.scrollbar_వెడల్పు); n++;
	    ఆపు;

	స్థితి SBAR_BOTTOM:
	    Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNheight, gui.scrollbar_ఎత్తు); n++;
	    ఆపు;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, పాఠ్యం_Area_Form, వాదనలు, n);
    ఒకవేళ (sb->id == (Widget)0)
	తిరిగి;

    gui_mch_సమితి_scrollbar_colors(sb);
    Xt_కూడు_Callback(sb->id, Xm_Nvalue_మారింది_Callback,
	    scroll_cb, (Xt_సూచిక)sb->ident);
    Xt_కూడు_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_సూచిక)sb->ident);
    Xt_కూడు_Event_Handler(sb->id, కీలకం_Press_ముసుగు, FALSE, gui_x11_కీలకం_తగిలింది_cb,
	    (Xt_సూచిక)0);
}

    శూన్య
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ఒకవేళ (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    శూన్య
gui_mch_సమితి_scrollbar_colors(scrollbar_T *sb)
{
    ఒకవేళ (sb->id != (Widget)0)
    {
	ఒకవేళ (gui.scroll_bg_pixel != INVALCOLOR)
	{
#ఒకవేళ (Xm_సంస్కరణ>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#అయితే_ముగింపు

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_సమితి_విలువలు(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ఖాళీ);
	}

	ఒకవేళ (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_సమితి_విలువలు(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ఖాళీ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    ఒకవేళ (sb == &gui.bottom_sbar && పాఠ్యం_Area_Form != (Widget)0)
	gui_motif_scroll_colors(పాఠ్యం_Area_Form);
}

/*
 * Miscellaneous stuff:
 */

    కిటికీ
gui_x11_get_wid(శూన్య)
{
    తిరిగి(Xt_కిటికీ(పాఠ్యం_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    స్థిర శూన్య
do_mnemonic(Widget w, గుర్తులేని పూర్ణాంకం keycode)
{
    WidgetList	    children;
    పూర్ణాంకం		    అంకె_Children, i;
    Boolean	    isMenu;
    కీలకం_Sym	    mnemonic = '\0';
    అక్షరం	    mneString[2];
    Widget	    user_దత్తాంశం_2;
    గుర్తులేని అక్షరం   అడ్డువరుస_నిలువు_Type;

    ఒకవేళ (Xt_Is_సంయుక్త(w))
    {
	ఒకవేళ (XtClass(w) == xm_అడ్డువరుస_స్తంభం_Widget_Class)
	{
	    Xt_Va_Get_విలువలు(w, Xm_Nrow_స్తంభం_Type, &అడ్డువరుస_నిలువు_Type, ఖాళీ);
	    isMenu = (అడ్డువరుస_నిలువు_Type != (గుర్తులేని అక్షరం)XmWORK_AREA);
	}
	లేకపోతే
	    isMenu = False;
	ఒకవేళ (!isMenu)
	{
	    Xt_Va_Get_విలువలు(w, XmNchildren, &children, XmNnumChildren,
			  &అంకె_Children, ఖాళీ);
	    కొరకు (i = 0; i < అంకె_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    లేకపోతే
    {
	Xt_Va_Get_విలువలు(w, XmNmnemonic, &mnemonic, ఖాళీ);
	ఒకవేళ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    ఒకవేళ (XKeysym_గమ్యం_Keycode(XtDisplay(Xt_మాతృ(w)),
				       XString_గమ్యం_Keysym(mneString)) == keycode)
	    {
		ఒకవేళ (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_విలువలు(w, Xm_Nuser_దత్తాంశం, &user_దత్తాంశం_2, ఖాళీ);
		    ఒకవేళ (user_దత్తాంశం_2 != ఖాళీ && XtIsWidget(user_దత్తాంశం_2))
			XmProcessTraversal(user_దత్తాంశం_2, XmTRAVERSE_CURRENT);
		}
		లేకపోతే
		{
		    XKeyPressedEvent కీలకం_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_క్షేత్రం(కీలకం_Event);
		    కీలకం_Event.type = కీలకం_Press;
		    కీలకం_Event.serial = 1;
		    కీలకం_Event.send_event = True;
		    కీలకం_Event.display = XtDisplay(w);
		    కీలకం_Event.కిటికీ_2 = Xt_కిటికీ(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & కీలకం_Event,
								     ఖాళీ, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    స్థిర శూన్య
mnemonic_event(
	Widget	    w,
	Xt_సూచిక   call_దత్తాంశం వాడని,
	XKeyEvent   *event,
	Boolean	    *b వాడని)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    స్థిర శూన్య
కూడు_mnemonic_grabs(Widget dialog, Widget w)
{
    అక్షరం	    mneString[2];
    WidgetList	    children;
    పూర్ణాంకం		    అంకె_Children, i;
    Boolean	    isMenu;
    కీలకం_Sym	    mnemonic = '\0';
    గుర్తులేని అక్షరం   అడ్డువరుస_నిలువు_Type;

    ఒకవేళ (Xt_Is_సంయుక్త(w))
    {
	ఒకవేళ (XtClass(w) == xm_అడ్డువరుస_స్తంభం_Widget_Class)
	{
	    Xt_Va_Get_విలువలు(w, Xm_Nrow_స్తంభం_Type, &అడ్డువరుస_నిలువు_Type, ఖాళీ);
	    isMenu = (అడ్డువరుస_నిలువు_Type != (గుర్తులేని అక్షరం)XmWORK_AREA);
	}
	లేకపోతే
	    isMenu = False;
	ఒకవేళ (!isMenu)
	{
	    Xt_Va_Get_విలువలు(w, XmNchildren, &children, XmNnumChildren,
							  &అంకె_Children, ఖాళీ);
	    కొరకు (i = 0; i < అంకె_Children; i++)
		కూడు_mnemonic_grabs(dialog, children[i]);
	}
    }
    లేకపోతే
    {
	Xt_Va_Get_విలువలు(w, XmNmnemonic, &mnemonic, ఖాళీ);
	ఒకవేళ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_కీలకం(dialog, XKeysym_గమ్యం_Keycode(XtDisplay(dialog),
						  XString_గమ్యం_Keysym(mneString)),
		    Mod1_ముసుగు, True, Grab_బహుళకం_Async, Grab_బహుళకం_Async);
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
    స్థిర శూన్య
activate_dialog_mnemonics(Widget dialog)
{
    ఒకవేళ (!dialog)
	తిరిగి;

    Xt_కూడు_Event_Handler(dialog, కీలకం_Press_ముసుగు, False,
			   (XtEventHandler) mnemonic_event, (Xt_సూచిక) ఖాళీ);
    కూడు_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    స్థిర శూన్య
suppress_dialog_mnemonics(Widget dialog)
{
    ఒకవేళ (!dialog)
	తిరిగి;

    Xt_Ungrab_కీలకం(dialog, Any_కీలకం, Mod1_ముసుగు);
    XtRemoveEventHandler(dialog, కీలకం_Press_ముసుగు, False,
			   (XtEventHandler) mnemonic_event, (Xt_సూచిక) ఖాళీ);
}

#ఒకవేళ defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    స్థిర శూన్య
సమితి_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    ఒకవేళ (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_సమితి *)&gui.fontset);
	ఒకవేళ (fl != ఖాళీ)
	{
	    ఒకవేళ (XtIsManaged(id))
	    {
		Xt_Unmanage_బిడ్డ(id);
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_బిడ్డ(id);
	    }
	    లేకపోతే
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
	    XmFontListFree(fl);
	}
    }
# లేకపోతే
    ఒకవేళ (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	ఒకవేళ (fl != ఖాళీ)
	{
	    ఒకవేళ (XtIsManaged(id))
	    {
		Xt_Unmanage_బిడ్డ(id);
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_బిడ్డ(id);
	    }
	    లేకపోతే
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

రకనిర్వచనం నిర్మాణం dialog_callback_arg
{
    అక్షరం *  వాదనలు;   // not used right now
    పూర్ణాంకం	    id;
} dcbarg_T;

స్థిర Widget dialog_wgt;
స్థిర అక్షరం *browse_fname = ఖాళీ;
స్థిర Xm_String_Char_సమితి charset = (Xm_String_Char_సమితి) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

స్థిర శూన్య DialogCancelCB(Widget, Xt_సూచిక, Xt_సూచిక);
స్థిర శూన్య DialogAcceptCB(Widget, Xt_సూచిక, Xt_సూచిక);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    స్థిర శూన్య
సమితి_predefined_label(Widget మాతృ, String name, అక్షరం *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *తదుపరి;
    కీలకం_Sym	mnemonic = NUL;

    w = Xt_Name_గమ్యం_Widget(మాతృ, name);

    ఒకవేళ (!w)
	తిరిగి;

    p = vim_strsave((char_u *)new_label);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;
    కొరకు (తదుపరి = p; *తదుపరి; ++తదుపరి)
    {
	ఒకవేళ (*తదుపరి == DLG_HOTKEY_CHAR)
	{
	    పూర్ణాంకం len = STRLEN(తదుపరి);

	    ఒకవేళ (len > 0)
	    {
		mch_memmove(తదుపరి, తదుపరి + 1, len);
		mnemonic = తదుపరి[0];
	    }
	}
    }

    str = XmStringCreate((అక్షరం *)p, STRING_ట్యాగ్);
    vim_free(p);

    ఒకవేళ (str != ఖాళీ)
    {
	Xt_Va_సమితి_విలువలు(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ఖాళీ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    స్థిర శూన్య
సమితి_predefined_fontlist(Widget మాతృ, String name)
{
    Widget w;
    w = Xt_Name_గమ్యం_Widget(మాతృ, name);

    ఒకవేళ (!w)
	తిరిగి;

    సమితి_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    పూర్ణాంకం		saving వాడని,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext వాడని,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*నమూనా;
    char_u	*tofree = ఖాళీ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ఖాళీ, 0);

    ఒకవేళ (initdir == ఖాళీ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    ఒకవేళ (dflt == ఖాళీ)
	dflt = (char_u *)"";
    లేకపోతే ఒకవేళ (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	కూడు_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    ఒకవేళ (filter == ఖాళీ)
	నమూనా = (char_u *)"";
    లేకపోతే
    {
	char_u	*s, *p;

	s = filter;
	కొరకు (p = filter; *p != NUL; ++p)
	{
	    ఒకవేళ (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    ఒకవేళ (*p == ';' || *p == '\n')	// end of (first) pattern
		ఆపు;
	}
	నమూనా = vim_strnsave(s, p - s);
	tofree = నమూనా;
	ఒకవేళ (నమూనా == ఖాళీ)
	    నమూనా = (char_u *)"";
    }

    Xt_Va_సమితి_విలువలు(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (అక్షరం *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (అక్షరం *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (అక్షరం *)నమూనా, STRLEN(నమూనా) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (అక్షరం *)title, STRLEN(title) + 1,
	ఖాళీ);

    సమితి_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    సమితి_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    సమితి_predefined_label(dialog_wgt, "Dir", _("Directories"));
    సమితి_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    సమితి_predefined_label(dialog_wgt, "Help", _("&Help"));
    సమితి_predefined_label(dialog_wgt, "Items", _("Files"));
    సమితి_predefined_label(dialog_wgt, "OK", _("&OK"));
    సమితి_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    సమితి_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    సమితి_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    ఒకవేళ (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_సమితి_విలువలు(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ఖాళీ);

    Xt_కూడు_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_సూచిక)0);
    Xt_కూడు_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_సూచిక)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_బిడ్డ(Xm_File_Selection_Box_Get_బిడ్డ(dialog_wgt,
					(గుర్తులేని అక్షరం)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    చేయి
    {
	XtAppProcessEvent(Xt_Widget_గమ్యం_Application_Context(dialog_wgt),
	    (Xt_ఆదానం_ముసుగు)XtIMAll);
    } అయితే (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    ఒకవేళ (browse_fname == ఖాళీ)
	తిరిగి ఖాళీ;
    తిరిగి vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    స్థిర శూన్య
DialogCancelCB(
    Widget	w వాడని,		//  widget id
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,	//  data from application
    Xt_సూచిక	call_దత్తాంశం వాడని)	//  data from widget class
{
    ఒకవేళ (browse_fname != ఖాళీ)
    {
	XtFree(browse_fname);
	browse_fname = ఖాళీ;
    }
    Xt_Unmanage_బిడ్డ(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    స్థిర శూన్య
DialogAcceptCB(
    Widget	w వాడని,		//  widget id
    Xt_సూచిక	client_దత్తాంశం_2 వాడని,	//  data from application
    Xt_సూచిక	call_దత్తాంశం)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    ఒకవేళ (browse_fname != ఖాళీ)
    {
	XtFree(browse_fname);
	browse_fname = ఖాళీ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_దత్తాంశం;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->విలువ, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_బిడ్డ(dialog_wgt);
}

#అయితే_ముగింపు // FEAT_BROWSE

#ఒకవేళ defined(FEAT_GUI_DIALOG)

స్థిర పూర్ణాంకం	dialog_స్థితి;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    స్థిర శూన్య
keyhit_callback(
    Widget		w,
    Xt_సూచిక		client_దత్తాంశం_2 వాడని,
    XEvent		*event,
    Boolean		*cont వాడని)
{
    అక్షరం	buf[2];
    కీలకం_Sym	కీలకం_sym;

    ఒకవేళ (XLookupString(&(event->xkey), buf, 2, &కీలకం_sym, ఖాళీ) == 1)
    {
	ఒకవేళ (*buf == CAR)
	    dialog_స్థితి = 1;
	లేకపోతే ఒకవేళ (*buf == ESC)
	    dialog_స్థితి = 2;
    }
    ఒకవేళ ((కీలకం_sym == XK_ఎడమ || కీలకం_sym == XK_కుడి)
	    && !(event->xkey.state & జరుపు_ముసుగు))
	Xm_పాఠ్యం_క్షేత్రం_Clear_Selection(w, Xt_చివరి_Timestamp_Processed(gui.dpy));
}

    స్థిర శూన్య
butproc(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    dialog_స్థితి = (పూర్ణాంకం)(దీర్ఘ)client_దత్తాంశం_2 + 1;
}

# ifdef HAVE_XPM

    స్థిర Widget
create_pixmap_label(
    Widget	మాతృ,
    String	name,
    అక్షరం	**దత్తాంశం,
    ArgList	వాదనలు,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    పూర్ణాంకం			లోతు;
    Pixmap		pixmap = 0;
    Xpm_లక్షణాలు	attr;
    Boolean		rs;
    Xpm_Color_చిహ్నం	color[5] =
    {
	{"none", ఖాళీ, 0},
	{"iconColor1", ఖాళీ, 0},
	{"bottomShadowColor", ఖాళీ, 0},
	{"topShadowColor", ఖాళీ, 0},
	{"selectColor", ఖాళీ, 0}
    };

    label = XmCreateLabelGadget(మాతృ, name, వాదనలు, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_యొక్క_Object(label);
    scr = Xt_Screen_యొక్క_Object(label);
    Xt_Va_Get_విలువలు(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_మాతృ(label),
		  XmNdepth, &లోతు,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ఖాళీ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_లోతు;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.లోతు = లోతు;
    Xpm_Create_Pixmap_From_దత్తాంశం(dsp, మూలం_కిటికీ_యొక్క_Screen(scr),
		    దత్తాంశం, &pixmap, ఖాళీ, &attr);

    Xt_Va_Get_విలువలు(label, Xm_Nrecompute_కొలత, &rs, ఖాళీ);
    Xt_Va_సమితి_విలువలు(label, Xm_Nrecompute_కొలత, True, ఖాళీ);
    Xt_Va_సమితి_విలువలు(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ఖాళీ);
    Xt_Va_సమితి_విలువలు(label, Xm_Nrecompute_కొలత, rs, ఖాళీ);

    తిరిగి label;
}
# endif

    పూర్ణాంకం
gui_mch_dialog(
    పూర్ణాంకం		type వాడని,
    char_u	*title,
    char_u	*సందేశం_2,
    char_u	*button_పేర్లు,
    పూర్ణాంకం		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    పూర్ణాంకం		ex_cmd వాడని)
{
    char_u		*buts;
    char_u		*p, *తదుపరి;
    XtAppContext	app;
    XmString		label;
    పూర్ణాంకం			butcount;
    Widget		w;
    Widget		dialogform = ఖాళీ;
    Widget		form = ఖాళీ;
    Widget		dialogtextfield = ఖాళీ;
    Widget		*buttons;
    Widget		sep_form = ఖాళీ;
    Boolean		vertical;
    Widget		separator = ఖాళీ;
    పూర్ణాంకం			n;
    Arg			వాదనలు[6];
# ifdef HAVE_XPM
    అక్షరం		**icon_దత్తాంశం_2 = ఖాళీ;
    Widget		dialogpixmap = ఖాళీ;
# endif

    ఒకవేళ (title == ఖాళీ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (అక్షరం *)"dialog", ఖాళీ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ఖాళీ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((అక్షరం *)title);
    ఒకవేళ (label == ఖాళీ)
	తిరిగి -1;
    Xt_Va_సమితి_విలువలు(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ఖాళీ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_పేర్లు);
    ఒకవేళ (buts == ఖాళీ)
	తిరిగి -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    కొరకు (p = buts; *p; ++p)
	ఒకవేళ (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    ఒకవేళ (buttons == ఖాళీ)
    {
	vim_free(buts);
	తిరిగి -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    కొరకు (butcount = 0; *p; ++butcount)
    {
	కీలకం_Sym mnemonic = NUL;

	కొరకు (తదుపరి = p; *తదుపరి; ++తదుపరి)
	{
	    ఒకవేళ (*తదుపరి == DLG_HOTKEY_CHAR)
	    {
		పూర్ణాంకం len = STRLEN(తదుపరి);

		ఒకవేళ (len > 0)
		{
		    mch_memmove(తదుపరి, తదుపరి + 1, len);
		    mnemonic = తదుపరి[0];
		}
	    }
	    ఒకవేళ (*తదుపరి == DLG_BUTTON_SEP)
	    {
		*తదుపరి++ = NUL;
		ఆపు;
	    }
	}
	label = XmStringCreate(_((అక్షరం *)p), STRING_ట్యాగ్);
	ఒకవేళ (label == ఖాళీ)
	    ఆపు;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_నెట్టు_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_స్థానభ్రంశం, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ఖాళీ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	ఒకవేళ (butcount > 0)
	{
	    ఒకవేళ (vertical)
		Xt_Va_సమితి_విలువలు(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ఖాళీ);
	    లేకపోతే
	    {
		ఒకవేళ (*తదుపరి == NUL)
		{
		    Xt_Va_సమితి_విలువలు(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_స్థానభ్రంశం, 4,
			    ఖాళీ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_స్థానభ్రంశం, 4,
			    ఖాళీ);
		    Xt_Manage_బిడ్డ(sep_form);
		}
		లేకపోతే
		{
		    Xt_Va_సమితి_విలువలు(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ఖాళీ);
		}
	    }
	}
	లేకపోతే ఒకవేళ (!vertical)
	{
	    ఒకవేళ (*తదుపరి == NUL)
	    {
		Xt_Va_సమితి_విలువలు(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_స్థానభ్రంశం, 4,
			ఖాళీ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_స్థానభ్రంశం, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_స్థానభ్రంశం, 4,
			ఖాళీ);
		Xt_Manage_బిడ్డ(sep_form);
	    }
	    లేకపోతే
		Xt_Va_సమితి_విలువలు(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_స్థానభ్రంశం, 4,
			ఖాళీ);
	}

	Xt_కూడు_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_సూచిక)(దీర్ఘ)butcount);
	p = తదుపరి;
    }
    vim_free(buts);

    separator = (Widget) 0;
    ఒకవేళ (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_సమితి_Arg(వాదనలు[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_సమితి_Arg(వాదనలు[n], XmNbottomWidget, buttons[0]); n++;
	Xt_సమితి_Arg(వాదనలు[n], Xm_Nbottom_స్థానభ్రంశం, 4); n++;
	Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", వాదనలు, n);
	Xt_Manage_బిడ్డ(separator);
    }

    ఒకవేళ (textfield != ఖాళీ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_పాఠ్యం_క్షేత్రం_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ఖాళీ);
	ఒకవేళ (butcount > 0)
	    Xt_Va_సమితి_విలువలు(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ఖాళీ);
	లేకపోతే
	    Xt_Va_సమితి_విలువలు(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ఖాళీ);

	సమితి_fontlist(dialogtextfield);
	Xm_పాఠ్యం_క్షేత్రం_సమితి_String(dialogtextfield, (అక్షరం *)textfield);
	Xt_Manage_బిడ్డ(dialogtextfield);
	Xt_కూడు_Event_Handler(dialogtextfield, కీలకం_Press_ముసుగు, False,
			    (XtEventHandler)keyhit_callback, (Xt_సూచిక)ఖాళీ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ఖాళీ);
    Xt_Manage_బిడ్డ(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ఎంపిక (type)
    {
	స్థితి VIM_GENERIC:
	    icon_దత్తాంశం_2 = generic_xpm;
	    ఆపు;
	స్థితి VIM_ERROR:
	    icon_దత్తాంశం_2 = error_xpm;
	    ఆపు;
	స్థితి VIM_WARNING:
	    icon_దత్తాంశం_2 = alert_xpm;
	    ఆపు;
	స్థితి VIM_INFO:
	    icon_దత్తాంశం_2 = info_xpm;
	    ఆపు;
	స్థితి VIM_QUESTION:
	    icon_దత్తాంశం_2 = quest_xpm;
	    ఆపు;
	అప్రమేయ:
	    icon_దత్తాంశం_2 = generic_xpm;
    }

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Ntop_స్థానభ్రంశం, 8); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nbottom_స్థానభ్రంశం, 8); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nleft_స్థానభ్రంశం, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_దత్తాంశం_2, వాదనలు, n);
    Xt_Manage_బిడ్డ(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((అక్షరం *)సందేశం_2, STRING_ట్యాగ్);
    ఒకవేళ (label == ఖాళీ)
    {
	vim_free(buttons);
	తిరిగి -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				Xm_Ntop_స్థానభ్రంశం, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# లేకపోతే
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_స్థానభ్రంశం, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_స్థానభ్రంశం, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_స్థానభ్రంశం, 8,
				ఖాళీ);
    XmStringFree(label);
    సమితి_fontlist(w);

    ఒకవేళ (textfield != ఖాళీ)
    {
	Xt_Va_సమితి_విలువలు(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ఖాళీ);
    }
    లేకపోతే
    {
	ఒకవేళ (butcount > 0)
	    Xt_Va_సమితి_విలువలు(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ఖాళీ);
	లేకపోతే
	    Xt_Va_సమితి_విలువలు(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ఖాళీ);
    }

    ఒకవేళ (dfltbutton < 1)
	dfltbutton = 1;
    ఒకవేళ (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_సమితి_విలువలు(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ఖాళీ);
    ఒకవేళ (textfield != ఖాళీ)
	Xt_Va_సమితి_విలువలు(dialogform, XmNinitialFocus, dialogtextfield, ఖాళీ);
    లేకపోతే
	Xt_Va_సమితి_విలువలు(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ఖాళీ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    ఒకవేళ (textfield != ఖాళీ && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_పాఠ్యం_క్షేత్రం_సమితి_Selection(dialogtextfield,
			 (Xm_పాఠ్యం_Position)0, (Xm_పాఠ్యం_Position)STRLEN(textfield),
					   Xt_చివరి_Timestamp_Processed(gui.dpy));
	Xm_పాఠ్యం_క్షేత్రం_సమితి_Cursor_Position(dialogtextfield,
					   (Xm_పాఠ్యం_Position)STRLEN(textfield));
    }

    app = Xt_Widget_గమ్యం_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_స్థితి = -1;
    కొరకు (;;)
    {
	XtAppProcessEvent(app, (Xt_ఆదానం_ముసుగు)XtIMAll);
	ఒకవేళ (dialog_స్థితి >= 0 || !XtIsManaged(dialogform))
	    ఆపు;
    }

    vim_free(buttons);

    ఒకవేళ (textfield != ఖాళీ)
    {
	p = (char_u *)Xm_పాఠ్యం_Get_String(dialogtextfield);
	ఒకవేళ (p == ఖాళీ || dialog_స్థితి < 0)
	    *textfield = NUL;
	లేకపోతే
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((అక్షరం *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    తిరిగి dialog_స్థితి;
}
#అయితే_ముగింపు // FEAT_GUI_DIALOG

#ఒకవేళ defined(FEAT_TOOLBAR)
    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    Cardinal	అంకె_Children;	    // how many children toolBar has

    ఒకవేళ (toolBar == (Widget)0)
	తిరిగి;
    Xt_Va_Get_విలువలు(toolBar, XmNnumChildren, &అంకె_Children, ఖాళీ);
    ఒకవేళ (showit && అంకె_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_విలువలు(toolBar, XmNchildren, &children, ఖాళీ);
	{
	    శూన్య    (*action)(BalloonEval *);
	    పూర్ణాంకం	    పాఠ్యం = 0;

	    ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    లేకపోతే
		action = &gui_mch_disable_beval_area;
	    ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *)p_toolbar, "text"))
		పాఠ్యం = 1;
	    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన((స్థిరరాశి అక్షరం *)p_toolbar, "icons"))
		పాఠ్యం = -1;
	    ఒకవేళ (పాఠ్యం != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    ఒకవేళ (menu_is_toolbar(toolbar->dname))
			ఆపు;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		ఒకవేళ (toolbar)
		{
		    కొరకు (cur = toolbar->children; cur; cur = cur->తదుపరి)
		    {
			Arg	    వాదనలు[1];
			పూర్ణాంకం	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			ఒకవేళ (cur->tip != ఖాళీ)
			    (*action)(cur->tip);
			ఒకవేళ (!menu_is_separator(cur->name))
			{
			    ఒకవేళ (పాఠ్యం == 1 || cur->xpm == ఖాళీ)
			    {
				Xt_సమితి_Arg(వాదనలు[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    ఒకవేళ (cur->id != ఖాళీ)
			    {
				Xt_Unmanage_బిడ్డ(cur->id);
				Xt_సమితి_విలువలు(cur->id, వాదనలు, n);
				Xt_Manage_బిడ్డ(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_ఎత్తు = gui_mch_compute_toolbar_ఎత్తు();
	Xt_Manage_బిడ్డ(Xt_మాతృ(toolBar));
# ifdef FEAT_GUI_TABLINE
	ఒకవేళ (showing_tabline)
	{
	    Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_మాతృ(toolBar),
			  ఖాళీ);
	    Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_పంక్తి,
			  ఖాళీ);
	}
	లేకపోతే
# endif
	    Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_మాతృ(toolBar),
			  ఖాళీ);
	ఒకవేళ (XtIsManaged(menuBar))
	    Xt_Va_సమితి_విలువలు(Xt_మాతృ(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ఖాళీ);
	లేకపోతే
	    Xt_Va_సమితి_విలువలు(Xt_మాతృ(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ఖాళీ);
    }
    లేకపోతే
    {
	gui.toolbar_ఎత్తు = 0;
	ఒకవేళ (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ఖాళీ);
	}
	లేకపోతే
	{
# ifdef FEAT_GUI_TABLINE
	    ఒకవేళ (showing_tabline)
	    {
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_FORM,
			      ఖాళీ);
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_పంక్తి,
			      ఖాళీ);
	    }
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ఖాళీ);
	}

	Xt_Unmanage_బిడ్డ(Xt_మాతృ(toolBar));
    }
    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    స్థిర శూన్య
reset_focus(శూన్య)
{
    ఒకవేళ (పాఠ్యం_Area != ఖాళీ)
	XmProcessTraversal(పాఠ్యం_Area, XmTRAVERSE_CURRENT);
}

    పూర్ణాంకం
gui_mch_compute_toolbar_ఎత్తు(శూన్య)
{
    Dimension	borders;
    Dimension	ఎత్తు;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	అంకె_Children;	    // how many children toolBar has
    పూర్ణాంకం		i;

    borders = 0;
    ఎత్తు = 0;
    ఒకవేళ (toolBar != (Widget)0 && tool_Bar_చట్రం != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_విలువలు(tool_Bar_చట్రం,
		XmNshadowThickness, &fst,
		Xm_Nmargin_ఎత్తు, &fmh,
		ఖాళీ);
	borders += fst + fmh;
	Xt_Va_Get_విలువలు(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_ఎత్తు, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &అంకె_Children, ఖాళీ);
	borders += tst + tmh;
	కొరకు (i = 0; i < (పూర్ణాంకం)అంకె_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_విలువలు(children[i], XmNheight, &whgt, ఖాళీ);
	    ఒకవేళ (ఎత్తు < whgt)
		ఎత్తు = whgt;
	}
    }
# ifdef LESSTIF_సంస్కరణ
    // Hack: When starting up we get wrong dimensions.
    ఒకవేళ (ఎత్తు < 10)
	ఎత్తు = 24;
# endif

    తిరిగి (పూర్ణాంకం)(ఎత్తు + (borders << 1));
}

    శూన్య
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_విలువలు(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    ఖాళీ);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    శూన్య
gui_mch_show_tabline(పూర్ణాంకం showit)
{
    ఒకవేళ (tab_పంక్తి == (Widget)0)
	తిరిగి;

    ఒకవేళ (!showit != !showing_tabline)
    {
	ఒకవేళ (showit)
	{
	    Xt_Manage_బిడ్డ(tab_పంక్తి);
	    Xt_Unmanage_బిడ్డ(Xt_Name_గమ్యం_Widget(tab_పంక్తి, "PageScroller"));
	    Xt_Unmanage_బిడ్డ(Xt_Name_గమ్యం_Widget(tab_పంక్తి, "MinorTabScrollerNext"));
	    Xt_Unmanage_బిడ్డ(Xt_Name_గమ్యం_Widget(tab_పంక్తి,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ఒకవేళ (XtIsManaged(Xt_మాతృ(toolBar)))
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar), ఖాళీ);
	    లేకపోతే
#  endif
		ఒకవేళ (XtIsManaged(menuBar))
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ఖాళీ);
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(tab_పంక్తి,
			      XmNtopAttachment, XmATTACH_FORM, ఖాళీ);
	    Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_పంక్తి,
			  ఖాళీ);
	}
	లేకపోతే
	{
	    Xt_Unmanage_బిడ్డ(tab_పంక్తి);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ఒకవేళ (XtIsManaged(Xt_మాతృ(toolBar)))
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_మాతృ(toolBar), ఖాళీ);
	    లేకపోతే
#  endif
		ఒకవేళ (XtIsManaged(menuBar))
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ఖాళీ);
	    లేకపోతే
# endif
		Xt_Va_సమితి_విలువలు(పాఠ్యం_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM, ఖాళీ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    పూర్ణాంకం
gui_mch_showing_tabline(శూన్య)
{
    తిరిగి tab_పంక్తి != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    శూన్య
gui_mch_update_tabline(శూన్య)
{
    tabpage_T		*tp;
    పూర్ణాంకం			nr = 1, n;
    Arg			వాదనలు[10];
    పూర్ణాంకం			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_పుట_Info	పుట_info;
    Xm_Notebook_పుట_స్థితి పుట_స్థితి;
    పూర్ణాంకం			చివరి_పుట, tab_లెక్క;
    XmString		label_str;
    అక్షరం		*label_cstr;
    BalloonEval		*beval;

    ఒకవేళ (tab_పంక్తి == (Widget)0)
	తిరిగి;

    // Add a label for each tab page.  They all contain the same text area.
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, ++nr)
    {
	ఒకవేళ (tp == curtab)
	    curtabidx = nr;

	పుట_స్థితి = Xm_Notebook_Get_పుట_Info(tab_పంక్తి, nr, &పుట_info);
	ఒకవేళ (పుట_స్థితి == Xm_పుట_INVALID
		|| పుట_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_సమితి_Arg(వాదనలు[n], Xm_Nnotebook_బిడ్డ_Type, XmMAJOR_TAB); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNtraversalOn, False); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNhighlightThickness, 1); n++;
	    Xt_సమితి_Arg(వాదనలు[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_నెట్టు_Button(tab_పంక్తి, "-Empty-", వాదనలు, n);
	    Xt_Manage_బిడ్డ(tab);
	    beval = gui_mch_create_beval_area(tab, ఖాళీ, tabline_balloon_cb,
									ఖాళీ);
	    Xt_Va_సమితి_విలువలు(tab, Xm_Nuser_దత్తాంశం, beval, ఖాళీ);
	}
	లేకపోతే
	    tab = పుట_info.major_tab_widget;

	Xt_Va_సమితి_విలువలు(tab, Xm_Npage_సంఖ్య, nr, ఖాళీ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_విలువలు(tab, XmNlabelString, &label_str, ఖాళీ);
	ఒకవేళ (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    ఒకవేళ (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_సమితి_విలువలు(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ఖాళీ);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_బిడ్డ(tab);
		Xt_Manage_బిడ్డ(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_లెక్క = nr - 1;

    Xt_Va_Get_విలువలు(tab_పంక్తి, Xm_Nlast_పుట_సంఖ్య, &చివరి_పుట, ఖాళీ);

    // Remove any old labels.
    అయితే (nr <= చివరి_పుట)
    {
	ఒకవేళ (Xm_Notebook_Get_పుట_Info(tab_పంక్తి, nr, &పుట_info) != Xm_పుట_INVALID
	    && పుట_info.పుట_సంఖ్య == nr
	    && పుట_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_విలువలు(పుట_info.major_tab_widget, Xm_Nuser_దత్తాంశం, &beval, ఖాళీ);
	    ఒకవేళ (beval != ఖాళీ)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_బిడ్డ(పుట_info.major_tab_widget);
	    XtDestroyWidget(పుట_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_సమితి_విలువలు(tab_పంక్తి, Xm_Nlast_పుట_సంఖ్య, tab_లెక్క, ఖాళీ);

    Xt_Va_Get_విలువలు(tab_పంక్తి, Xm_Ncurrent_పుట_సంఖ్య, &currentpage, ఖాళీ);
    ఒకవేళ (currentpage != curtabidx)
	Xt_Va_సమితి_విలువలు(tab_పంక్తి, Xm_Ncurrent_పుట_సంఖ్య, curtabidx, ఖాళీ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    శూన్య
gui_mch_సమితి_curtab(పూర్ణాంకం nr)
{
    పూర్ణాంకం		currentpage;

    ఒకవేళ (tab_పంక్తి == (Widget)0)
	తిరిగి;

    Xt_Va_Get_విలువలు(tab_పంక్తి, Xm_Ncurrent_పుట_సంఖ్య, &currentpage, ఖాళీ);
    ఒకవేళ (currentpage != nr)
	Xt_Va_సమితి_విలువలు(tab_పంక్తి, Xm_Ncurrent_పుట_సంఖ్య, nr, ఖాళీ);
}
#అయితే_ముగింపు

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    స్థిర శూన్య
gui_motif_menu_colors(Widget id)
{
    ఒకవేళ (gui.menu_bg_pixel != INVALCOLOR)
#ఒకవేళ (Xm_సంస్కరణ >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#లేకపోతే
	Xt_Va_సమితి_విలువలు(id, XmNbackground, gui.menu_bg_pixel, ఖాళీ);
#అయితే_ముగింపు
    ఒకవేళ (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_సమితి_విలువలు(id, XmNforeground, gui.menu_fg_pixel, ఖాళీ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    స్థిర శూన్య
gui_motif_scroll_colors(Widget id)
{
    ఒకవేళ (gui.scroll_bg_pixel != INVALCOLOR)
#ఒకవేళ (Xm_సంస్కరణ >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#లేకపోతే
	Xt_Va_సమితి_విలువలు(id, XmNbackground, gui.scroll_bg_pixel, ఖాళీ);
#అయితే_ముగింపు
    ఒకవేళ (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_సమితి_విలువలు(id, XmNforeground, gui.scroll_fg_pixel, ఖాళీ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    శూన్య
gui_motif_menu_fontlist(Widget id వాడని)
{
#నిర్వచితమైతే FEAT_MENU
# ifdef FONTSET_ALWAYS
    ఒకవేళ (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_సమితి *)&gui.menu_fontset);
	ఒకవేళ (fl != ఖాళీ)
	{
	    ఒకవేళ (XtIsManaged(id))
	    {
		Xt_Unmanage_బిడ్డ(id);
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_బిడ్డ(id);
	    }
	    లేకపోతే
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
	    XmFontListFree(fl);
	}
    }
# లేకపోతే
    ఒకవేళ (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	ఒకవేళ (fl != ఖాళీ)
	{
	    ఒకవేళ (XtIsManaged(id))
	    {
		Xt_Unmanage_బిడ్డ(id);
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_బిడ్డ(id);
	    }
	    లేకపోతే
		Xt_Va_సమితి_విలువలు(id, XmNfontList, fl, ఖాళీ);
	    XmFontListFree(fl);
	}
    }
# endif
#అయితే_ముగింపు
}


/*
 * We don't create it twice for the sake of speed.
 */

రకనిర్వచనం నిర్మాణం Shared_కనుగొను_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget పైకి;		// search direction 'Up' radio button
    Widget కిందికి;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget కనుగొను;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_కనుగొను_Replace_2;

స్థిర Shared_కనుగొను_Replace_2 కనుగొను_widgets = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};
స్థిర Shared_కనుగొను_Replace_2 repl_widgets = {ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ};

    స్థిర శూన్య
కనుగొను_replace_destroy_callback(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    Shared_కనుగొను_Replace_2 *cd = (Shared_కనుగొను_Replace_2 *)client_దత్తాంశం_2;

    ఒకవేళ (cd != ఖాళీ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    స్థిర శూన్య
కనుగొను_replace_dismiss_callback(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    Shared_కనుగొను_Replace_2 *cd = (Shared_కనుగొను_Replace_2 *)client_దత్తాంశం_2;

    ఒకవేళ (cd != ఖాళీ)
	Xt_Unmanage_బిడ్డ(cd->dialog);
}

    స్థిర శూన్య
నమోదు_activate_callback(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    XmProcessTraversal((Widget)client_దత్తాంశం_2, XmTRAVERSE_CURRENT);
}

    స్థిర శూన్య
కనుగొను_replace_callback(
    Widget	w వాడని,
    Xt_సూచిక	client_దత్తాంశం_2,
    Xt_సూచిక	call_దత్తాంశం వాడని)
{
    long_u	flags = (long_u)client_దత్తాంశం_2;
    అక్షరం	*కనుగొను_పాఠ్యం, *repl_పాఠ్యం;
    Boolean	direction_కిందికి = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_కనుగొను_Replace_2 *sfr;

    ఒకవేళ (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = ఖాళీలు_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	తిరిగి;
    }

    // Get the search/replace strings from the dialog
    ఒకవేళ (flags == FRD_FINDNEXT)
    {
	repl_పాఠ్యం = ఖాళీ;
	sfr = &కనుగొను_widgets;
    }
    లేకపోతే
    {
	repl_పాఠ్యం = Xm_పాఠ్యం_క్షేత్రం_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    కనుగొను_పాఠ్యం = Xm_పాఠ్యం_క్షేత్రం_Get_String(sfr->what);
    Xt_Va_Get_విలువలు(sfr->కిందికి, XmNset, &direction_కిందికి, ఖాళీ);
    Xt_Va_Get_విలువలు(sfr->wword, XmNset, &wword, ఖాళీ);
    Xt_Va_Get_విలువలు(sfr->mcase, XmNset, &mcase, ఖాళీ);
    ఒకవేళ (wword)
	flags |= FRD_సంపూర్ణ_మాట;
    ఒకవేళ (mcase)
	flags |= FRD_సరిపోలిక_CASE;

    (శూన్య)gui_do_findrepl((పూర్ణాంకం)flags, (char_u *)కనుగొను_పాఠ్యం, (char_u *)repl_పాఠ్యం,
							      direction_కిందికి);

    ఒకవేళ (కనుగొను_పాఠ్యం != ఖాళీ)
	XtFree(కనుగొను_పాఠ్యం);
    ఒకవేళ (repl_పాఠ్యం != ఖాళీ)
	XtFree(repl_పాఠ్యం);
}

    స్థిర శూన్య
కనుగొను_replace_keypress(
    Widget		w వాడని,
    Shared_కనుగొను_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b వాడని)
{
    కీలకం_Sym keysym;

    ఒకవేళ (frdp == ఖాళీ)
	తిరిగి;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    ఒకవేళ (keysym == XK_Escape)
	Xt_Unmanage_బిడ్డ(frdp->dialog);
}

    స్థిర శూన్య
సమితి_label(Widget w, అక్షరం *label)
{
    XmString	str;
    char_u	*p, *తదుపరి;
    కీలకం_Sym	mnemonic = NUL;

    ఒకవేళ (!w)
	తిరిగి;

    p = vim_strsave((char_u *)label);
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;
    కొరకు (తదుపరి = p; *తదుపరి; ++తదుపరి)
    {
	ఒకవేళ (*తదుపరి == DLG_HOTKEY_CHAR)
	{
	    పూర్ణాంకం len = STRLEN(తదుపరి);

	    ఒకవేళ (len > 0)
	    {
		mch_memmove(తదుపరి, తదుపరి + 1, len);
		mnemonic = తదుపరి[0];
	    }
	}
    }

    str = XmStringCreateSimple((అక్షరం *)p);
    vim_free(p);
    ఒకవేళ (str)
    {
	Xt_Va_సమితి_విలువలు(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ఖాళీ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    స్థిర శూన్య
కనుగొను_replace_dialog_create(char_u *arg, పూర్ణాంకం do_replace)
{
    Shared_కనుగొను_Replace_2	*frdp;
    Widget		separator;
    Widget		ఆదానం_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		చట్రం;
    XmString		str;
    పూర్ణాంకం			n;
    Arg			వాదనలు[6];
    పూర్ణాంకం			wword = FALSE;
    పూర్ణాంకం			mcase = !p_ic;
    Dimension		వెడల్పు;
    Dimension		widest;
    char_u		*నమోదు_పాఠ్యం;

    frdp = do_replace ? &repl_widgets : &కనుగొను_widgets;

    // Get the search string to use.
    నమోదు_పాఠ్యం = get_కనుగొను_dialog_పాఠ్యం(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    ఒకవేళ (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	ఒకవేళ (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_కిటికీ(Xt_మాతృ(frdp->dialog)));
	లేకపోతే
	    Xt_Manage_బిడ్డ(frdp->dialog);
	XtPopup(Xt_మాతృ(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	    Xm_పాఠ్యం_క్షేత్రం_సమితి_String(frdp->what, (అక్షరం *)నమోదు_పాఠ్యం);
	vim_free(నమోదు_పాఠ్యం);

	Xt_Va_సమితి_విలువలు(frdp->wword, XmNset, wword, ఖాళీ);
	తిరిగి;
    }

    // Create a fresh new dialog window
    ఒకవేళ (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    లేకపోతే
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNautoUnmanage, False); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNnoResize, True); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", వాదనలు, n);
    XmStringFree(str);
    Xt_కూడు_Callback(frdp->dialog, XmNdestroyCallback,
	    కనుగొను_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    ఖాళీ);

    frdp->కనుగొను = XtVaCreateManagedWidget("findButton",
	    xm_నెట్టు_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ఖాళీ);
    సమితి_label(frdp->కనుగొను, _("Find &Next"));

    Xt_కూడు_Callback(frdp->కనుగొను, XmNactivateCallback,
	    కనుగొను_replace_callback,
	    (do_replace ? (Xt_సూచిక)FRD_R_FINDNEXT : (Xt_సూచిక)FRD_FINDNEXT));

    ఒకవేళ (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_నెట్టు_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->కనుగొను,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ఖాళీ);
	సమితి_label(frdp->replace, _("&Replace"));
	Xt_కూడు_Callback(frdp->replace, XmNactivateCallback,
		కనుగొను_replace_callback, (Xt_సూచిక)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_నెట్టు_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ఖాళీ);
	సమితి_label(frdp->all, _("Replace &All"));
	Xt_కూడు_Callback(frdp->all, XmNactivateCallback,
		కనుగొను_replace_callback, (Xt_సూచిక)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_నెట్టు_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ఖాళీ);
	సమితి_label(frdp->undo, _("&Undo"));
	Xt_కూడు_Callback(frdp->undo, XmNactivateCallback,
		కనుగొను_replace_callback, (Xt_సూచిక)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_నెట్టు_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ఖాళీ);
    సమితి_label(frdp->cancel, _("&Cancel"));
    Xt_కూడు_Callback(frdp->cancel, XmNactivateCallback,
	    కనుగొను_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_బిడ్డ(button_form);

    n = 0;
    Xt_సమితి_Arg(వాదనలు[n], XmNorientation, XmVERTICAL); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNrightWidget, button_form); n++;
    Xt_సమితి_Arg(వాదనలు[n], Xm_Nright_స్థానభ్రంశం, 4); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_సమితి_Arg(వాదనలు[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", వాదనలు, n);
    Xt_Manage_బిడ్డ(separator);

    ఆదానం_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    ఖాళీ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ఆదానం_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_స్థానభ్రంశం, 4,
		ఖాళీ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_పాఠ్యం_క్షేత్రం_Widget_Class, ఆదానం_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ఖాళీ);

	ఒకవేళ (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_పాఠ్యం_క్షేత్రం_Widget_Class,	ఆదానం_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_స్థానభ్రంశం, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ఖాళీ);

	    Xt_కూడు_Callback(frdp->with, XmNactivateCallback,
		    కనుగొను_replace_callback, (Xt_సూచిక) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ఆదానం_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_స్థానభ్రంశం, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ఖాళీ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_కూడు_Callback(frdp->what, XmNactivateCallback,
		    నమోదు_activate_callback, frdp->with);
	    Xt_కూడు_Event_Handler(frdp->with, కీలకం_Press_ముసుగు, False,
			    (XtEventHandler)కనుగొను_replace_keypress,
			    (Xt_సూచిక) frdp);

	}
	లేకపోతే
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_కూడు_Callback(frdp->what, XmNactivateCallback,
		    కనుగొను_replace_callback, (Xt_సూచిక)FRD_FINDNEXT);
	}
	Xt_కూడు_Event_Handler(frdp->what, కీలకం_Press_ముసుగు, False,
			    (XtEventHandler)కనుగొను_replace_keypress,
			    (Xt_సూచిక)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_సమితి_Arg(వాదనలు[n], XmNwidth, &వెడల్పు); n++;
	Xt_Get_విలువలు(label_what, వాదనలు, n);
	widest = వెడల్పు;
	ఒకవేళ (do_replace)
	{
	    Xt_Get_విలువలు(label_with, వాదనలు, n);
	    ఒకవేళ (వెడల్పు > widest)
		widest = వెడల్పు;
	}

	Xt_Va_సమితి_విలువలు(frdp->what, Xm_Nleft_స్థానభ్రంశం, widest, ఖాళీ);
	ఒకవేళ (do_replace)
	    Xt_Va_సమితి_విలువలు(frdp->with, Xm_Nleft_స్థానభ్రంశం, widest, ఖాళీ);

    }

    Xt_Manage_బిడ్డ(ఆదానం_form);

    {
	Widget radio_box;
	Widget w;

	చట్రం = XtVaCreateWidget("directionFrame",
		xm_చట్రం_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ఆదానం_form,
		Xm_Ntop_స్థానభ్రంశం, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_స్థానభ్రంశం, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ఆదానం_form,
		ఖాళీ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, చట్రం,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_చట్రం_TITLE_బిడ్డ,
		ఖాళీ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(చట్రం, "radioBox",
		(ArgList)ఖాళీ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->పైకి = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ఖాళీ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->పైకి);

	str = XmStringCreateSimple(_("Down"));
	frdp->కిందికి = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ఖాళీ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->కిందికి);

	Xt_Manage_బిడ్డ(radio_box);
	Xt_Manage_బిడ్డ(చట్రం);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, చట్రం,
	    Xm_Nright_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ఆదానం_form,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_స్థానభ్రంశం, 4,
	    ఖాళీ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNset, wword,
	    ఖాళీ);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_స్థానభ్రంశం, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_స్థానభ్రంశం, 4,
	    XmNset, mcase,
	    ఖాళీ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_బిడ్డ(toggle_form);

    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	Xm_పాఠ్యం_క్షేత్రం_సమితి_String(frdp->what, (అక్షరం *)నమోదు_పాఠ్యం);
    vim_free(నమోదు_పాఠ్యం);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   శూన్య
gui_mch_కనుగొను_dialog(exarg_T *eap)
{
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    కనుగొను_replace_dialog_create(eap->arg, FALSE);
}


    శూన్య
gui_mch_replace_dialog(exarg_T *eap)
{
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి;

    కనుగొను_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    శూన్య
gui_motif_synch_fonts(శూన్య)
{
    Shared_కనుగొను_Replace_2 *frdp;
    పూర్ణాంకం		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    ఒకవేళ (font == ఖాళీ)
	తిరిగి;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    కొరకు (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&కనుగొను_widgets);
	ఒకవేళ (frdp->dialog)
	{
	    Xt_Va_సమితి_విలువలు(frdp->what, XmNfontList, font_list, ఖాళీ);
	    ఒకవేళ (do_replace)
		Xt_Va_సమితి_విలువలు(frdp->with, XmNfontList, font_list, ఖాళీ);
	}
    }

    XmFontListFree(font_list);
}
