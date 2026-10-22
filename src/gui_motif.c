/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#அந்தர்பாவ "vim.h"

#அந்தர்பாவ <Xm/Form.h>
#அந்தர்பாவ <Xm/RowColumn.h>
#அந்தர்பாவ <Xm/PushB.h>
#அந்தர்பாவ <Xm/Text.h>
#அந்தர்பாவ <Xm/TextF.h>
#அந்தர்பாவ <Xm/Separator.h>
#அந்தர்பாவ <Xm/Label.h>
#அந்தர்பாவ <Xm/CascadeB.h>
#அந்தர்பாவ <Xm/ScrollBar.h>
#அந்தர்பாவ <Xm/MenuShell.h>
#அந்தர்பாவ <Xm/DrawingA.h>
#எனில் (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#அந்தர்பாவ <Xm/Frame.h>
#அந்தர்பாவ <Xm/LabelG.h>
#அந்தர்பாவ <Xm/ToggleBG.h>
#அந்தர்பாவ <Xm/SeparatoG.h>
#அந்தர்பாவ <Xm/XmP.h>

#அந்தர்பாவ <X11/keysym.h>
#அந்தர்பாவ <X11/Xatom.h>
#அந்தர்பாவ <X11/StringDefs.h>
#அந்தர்பாவ <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# எனில் defined(VMS)
#  include <xpm.h>
# இல்லையெனில்
#  include <X11/xpm.h>
# endif
#இல்லையெனில்
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#அந்தர்பாவ "gui_xmebw.h"	// for our Enhanced Button Widget

#எனில் defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

வெளி Widget vimShell;

நிலையான Widget vimForm;
நிலையான Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
நிலையான Widget toolBarFrame;
நிலையான Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
நிலையான Widget	tab_வரி;
நிலையான Widget	tab_வரி_menu = 0;
நிலையான முழு	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# எனில் (XmVersion >= 1002)
// remember the last set value for the tearoff item
நிலையான முழு tearoff_val = (முழு)XmTEAR_OFF_ENABLED;
# endif
நிலையான Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
நிலையான வெறுமை reset_focus(வெறுமை);
#endif

நிலையான வெறுமை gui_motif_menu_colors(Widget id);
நிலையான வெறுமை gui_motif_scroll_colors(Widget id);

#எனில் (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#இல்லையெனில்
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    நிலையான வெறுமை
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	மதிப்பு;
    முழு		dragging;

    sb = gui_find_scrollbar((long)client_data);

    மதிப்பு = ((XmScrollBarCallbackStruct *)call_data)->மதிப்பு;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (முழு)XmCR_DRAG);
    gui_drag_scrollbar(sb, மதிப்பு, dragging);
}

#ifdef FEAT_GUI_TABLINE
    நிலையான வெறுமை
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    எனில் (nptr->reason != (முழு)XmCR_NONE)
	send_tabline_event(nptr->page_எண்);
}

    நிலையான வெறுமை
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_மதிப்புகள்(w, XmNuserData, &cmd, வெற்று);
    Xt_Va_Get_மதிப்புகள்(tab_வரி_menu, XmNuserData, &tab_idx, வெற்று);

    send_tabline_menu_event((முழு)(long)tab_idx, (முழு)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    நிலையான வெறுமை
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((முழு *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    நிலையான முழு
tabline_scroller_clicked(
    எழுத்து		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_வரி, scroller_name);
    எனில் (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_மதிப்புகள்(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, வெற்று);
	எனில் (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    எனில் (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		திரும்பு TRUE;
	}
    }
    திரும்பு FALSE;
}

    நிலையான வெறுமை
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    முழு				tab_idx = 0;
    WidgetList			children;
    Cardinal			இலக்கம்_Children;
    நிலையான XtIntervalId		timer = (XtIntervalId)0;
    நிலையான முழு			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    எனில் (event->button == Button1)
    {
	எனில் (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    திரும்பு;

	எனில் (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	இல்லையெனில்
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	திரும்பு;
    }

    எனில் (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_மதிப்புகள்(tab_வரி_menu, XmNuserData, &tab_idx, வெற்று);
	send_tabline_menu_event(tab_idx, (முழு)TABLINE_MENU_CLOSE);
	திரும்பு;
    }

    எனில் (event->button != Button3)
	திரும்பு;

    // When ignoring events don't show the menu.
    எனில் (hold_gui_events || cmdwin_type != 0)
	திரும்பு;

    எனில் (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	எனில் (tab_w != (Widget)0 && Xm_Is_தள்ளு_Button(tab_w))
	    Xt_Va_Get_மதிப்புகள்(tab_w, Xm_Npage_எண், &tab_idx, வெற்று);
    }

    Xt_Va_Set_மதிப்புகள்(tab_வரி_menu, XmNuserData, (XtPointer)(long)tab_idx, வெற்று);
    Xt_Va_Get_மதிப்புகள்(tab_வரி_menu, XmNchildren, &children, XmNnumChildren,
		  &இலக்கம்_Children, வெற்று);
    XtManageChildren(children, இலக்கம்_Children);
    XmMenuPosition(tab_வரி_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_வரி_menu);
}

    நிலையான வெறுமை
tabline_balloon_cb(BalloonEval *beval, முழு state UNUSED)
{
    முழு		nr;
    tabpage_T	*tp;

    எனில் (beval->target == (Widget)0)
	திரும்பு;

    Xt_Va_Get_மதிப்புகள்(beval->target, Xm_Npage_எண், &nr, வெற்று);
    tp = find_tabpage(nr);
    எனில் (tp == வெற்று)
	திரும்பு;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#endif

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#அந்தர்பாவ <Xm/XmP.h>
#அந்தர்பாவ <Xm/LabelP.h>

நிலையான XtExposeProc old_label_expose = வெற்று;

    நிலையான வெறுமை
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned எழுத்து   label_type = (முழு)XmSTRING;

    Xt_Va_Get_மதிப்புகள்(_w, XmNlabelType, &label_type, (XtPointer)0);

    எனில் (XtIsSensitive(_w) || label_type != (முழு)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    இல்லையெனில்
    {
	XGCValues   மதிப்புகள்;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	மதிப்புகள்.graphics_exposures = False;

	எனில் (fs != 0)
	{
	    mask |= GCFont;
	    மதிப்புகள்.font = fs->fid;
	}

	எனில் (lw->primitive.உச்சி_shadow_pixmap != None
		&& lw->primitive.உச்சி_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    மதிப்புகள்.fill_style = FillTiled;
	    மதிப்புகள்.tile = lw->primitive.உச்சி_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	எனில் (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	மதிப்புகள்.foreground = lw->primitive.உச்சி_shadow_color;
	மதிப்புகள்.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &மதிப்புகள், dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	எனில் (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	மதிப்புகள்.foreground = lw->primitive.bottom_shadow_color;
	மதிப்புகள்.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &மதிப்புகள், dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    வெறுமை
gui_x11_create_widgets(வெறுமை)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    முழு		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    எனில் (old_label_expose == வெற்று)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_offset = gui.border_width;

    /*
     * Install the tearOffModel resource converter.
     */
#எனில் (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_மதிப்புகள்(vimShell, XmNdeleteResponse, XmDO_NOTHING, வெற்று);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	வெற்று);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	முழு ac = 0;

# எனில் (XmVersion >= 1002)
	XtSetArg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	XtSetArg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	XtSetArg(al[ac], XmNrightOffset, 0); ac++;
# endif
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	XtManageChild(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#endif

#ifdef FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    toolBarFrame = XtVaCreateWidget("toolBarFrame",
	xmFrameWidgetClass, vimForm,
	XmNshadowThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginWidth, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	வெற்று);
    gui_motif_menu_colors(toolBarFrame);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xmRowColumnWidgetClass, toolBarFrame,
	XmNchildType, XmFRAME_WORKAREA_CHILD,
	XmNrowColumnType, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	XmNmarginHeight, 0,
	XmNmarginWidth, 0,
	XmNadjustLast, True,
	வெற்று);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_எண், 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_உச்சி_வலது); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_வரி = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_வரி, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			வெற்று);
    XtAddEventHandler(tab_வரி, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, வெற்று);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_வரி, "MinorTabScrollerNext");
    Xt_Va_Set_மதிப்புகள்(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, வெற்று);
    scroller = XtNameToWidget(tab_வரி, "MinorTabScrollerPrevious");
    Xt_Va_Set_மதிப்புகள்(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, வெற்று);

    // Create the tabline popup menu
    tab_வரி_menu = XmCreatePopupMenu(tab_வரி, "tabline popup", வெற்று, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((எழுத்து *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_தள்ளு_Button(tab_வரி_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, வெற்று);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((எழுத்து *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_தள்ளு_Button(tab_வரி_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, வெற்று);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((எழுத்து *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_தள்ளு_Button(tab_வரி_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, வெற்று);
    XmStringFree(xms);
#endif

    textAreaForm = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	வெற்று);
    gui_motif_scroll_colors(textAreaForm);

    textArea = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, textAreaForm,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.back_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	வெற்று);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(textArea, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.in_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    வெறுமை
gui_x11_destroy_widgets(வெறுமை)
{
    textArea = வெற்று;
#ifdef FEAT_MENU
    menuBar = வெற்று;
#endif
}

    வெறுமை
gui_mch_set_text_area_pos(
    முழு	    x UNUSED,
    முழு	    y UNUSED,
    முழு	    w UNUSED,
    முழு	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    வெறுமை
gui_x11_set_back_color(வெறுமை)
{
    எனில் (textArea != வெற்று)
#எனில் (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#இல்லையெனில்
	Xt_Va_Set_மதிப்புகள்(textArea,
		  XmNbackground, gui.back_pixel,
		  வெற்று);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    வெறுமை
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window வேர், child;
    unsigned முழு mask;
    unsigned முழு width, height, border_width, depth;
    முழு x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_மதிப்புகள்(shell, XmNmappedWhenManaged, &mappedWhenManaged, வெற்று);
    Xt_Va_Set_மதிப்புகள்(shell, XmNmappedWhenManaged, False, வெற்று);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &வேர், &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &வேர், &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    எனில் (x < 0)
	x = 0;
    எனில் (x > maxX)
	x = maxX;
    எனில் (y < 0)
	y = 0;
    எனில் (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_மதிப்புகள்(shell, XmNx, x, XmNy, y, வெற்று);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_மதிப்புகள்(shell, XmNmappedWhenManaged, mappedWhenManaged, வெற்று);
}

#எனில் defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# எனில் (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# இல்லையெனில்
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(வெற்று, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    திரும்பு font_list;
}

# எனில் ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(வெற்று, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    திரும்பு font_list;
}
# endif

#endif

#எனில் defined(FEAT_MENU)
/*
 * Menu stuff.
 */

நிலையான வெறுமை gui_motif_add_actext(vimmenu_T *menu);
# எனில் (XmVersion >= 1002)
நிலையான வெறுமை toggle_tearoff(Widget wid);
நிலையான வெறுமை gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
நிலையான வெறுமை submenu_change(vimmenu_T *mp, முழு colors);

நிலையான வெறுமை do_set_mnemonics(முழு enable);
நிலையான முழு menu_enabled = TRUE;

    வெறுமை
gui_mch_enable_menu(முழு flag)
{
    எனில் (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	எனில் (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_மதிப்புகள்(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		வெற்று);
#  ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
#  endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      வெற்று);
	}
	இல்லையெனில்
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      வெற்று);
	}
    }
    இல்லையெனில்
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	எனில் (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_மதிப்புகள்(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		வெற்று);
#  ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
#  endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      வெற்று);
	}
	இல்லையெனில்
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_FORM,
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      வெற்று);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    வெறுமை
gui_motif_set_mnemonics(முழு enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    எனில் (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    நிலையான வெறுமை
do_set_mnemonics(முழு enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	எனில் (menu->id != (Widget)0)
	    Xt_Va_Set_மதிப்புகள்(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    வெற்று);
}

    வெறுமை
gui_mch_add_menu(vimmenu_T *menu, முழு idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    எனில் (menu_is_popup(menu->name))
    {
	Arg arg[2];
	முழு n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  எனில் (XmVersion <= 1002)
	எனில் (mouse_model_popup())
#  endif
	{
	    எனில் (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    எனில் (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	திரும்பு;
    }
# endif

    எனில் (!menu_is_menubar(menu->name)
	    || (parent != வெற்று && parent->submenu_id == (Widget)0))
	திரும்பு;

    label = XmStringCreate((எழுத்து *)menu->dname, STRING_TAG);
    எனில் (label == வெற்று)
	திரும்பு;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == வெற்று) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# எனில் (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != வெற்று
			   && tearoff_val == (முழு)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    வெற்று);
    XmStringFree(label);

    எனில் (menu->id == (Widget)0)		// failed
	திரும்பு;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    எனில் (parent == வெற்று && menu->priority >= 9999)
	Xt_Va_Set_மதிப்புகள்(menuBar,
		XmNmenuHelpWidget, menu->id,
		வெற்று);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	வெற்று);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	வெற்று);
    gui_motif_menu_colors(menu->submenu_id);

    எனில் (menu->submenu_id == (Widget)0)		// failed
	திரும்பு;

# எனில் (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_மதிப்புகள்(menu->id,
	XmNsubMenuId, menu->submenu_id,
	வெற்று);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    எனில் (parent == வெற்று)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    நிலையான வெறுமை
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    எனில் (menu->actext == வெற்று || menu->id == (Widget)0)
	திரும்பு;

    label = XmStringCreate((எழுத்து *)menu->actext, STRING_TAG);
    எனில் (label == வெற்று)
	திரும்பு;
    Xt_Va_Set_மதிப்புகள்(menu->id, XmNacceleratorText, label, வெற்று);
    XmStringFree(label);
}

    வெறுமை
gui_mch_toggle_tearoffs(முழு enable)
{
# எனில் (XmVersion >= 1002)
    எனில் (enable)
	tearoff_val = (முழு)XmTEAR_OFF_ENABLED;
    இல்லையெனில்
	tearoff_val = (முழு)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(வேர்_menu);
# endif
}

# எனில் (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    நிலையான வெறுமை
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_மதிப்புகள்(wid, XmNtearOffModel, tearoff_val, வெற்று);
    எனில் (tearoff_val == (முழு)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    நிலையான வெறுமை
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    போது (menu != வெற்று)
    {
	எனில் (!menu_is_popup(menu->name))
	{
	    எனில் (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    முழு
gui_mch_text_area_extra_height(வெறுமை)
{
    Dimension	shadowHeight;

    Xt_Va_Get_மதிப்புகள்(textAreaForm, XmNshadowThickness, &shadowHeight, வெற்று);
    திரும்பு shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    வெறுமை
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    நிலையான Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    எனில் (id != (Widget)0)
	Xt_Va_Get_மதிப்புகள்(id, XmNheight, &height, வெற்று);

    // Find any menu Widget, to be able to call XtManageChild()
    இல்லையெனில்
	FOR_ALL_MENUS(mp)
	    எனில் (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		நிறுத்து;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    எனில் (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	எனில் (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_மதிப்புகள்(mp->id, XmNy, &y, வெற்று);
	    எனில் (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_மதிப்புகள்(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	வெற்று);

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
    gui.menu_height = maxy + height - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    எனில் (gui.shell_created)
	Xt_Va_Set_மதிப்புகள்(menuBar, XmNheight, gui.menu_height, வெற்று);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

நிலையான எழுத்து **get_toolbar_pixmap(vimmenu_T *menu, எழுத்து **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    நிலையான முழு
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    முழு		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    வேர்_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (எழுத்து *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    எனில் (status == XpmSuccess)
	திரும்பு OK;
    திரும்பு FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    நிலையான எழுத்து **
get_toolbar_pixmap(vimmenu_T *menu, எழுத்து **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    எழுத்து	**xpm = வெற்று;		// xpm array
    முழு		res;

    *fname = வெற்று;
    buf[0] = NUL;			// start with NULL path

    எனில் (menu->iconfile != வெற்று)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	எனில் (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	எனில் (res == OK)
	{
	    *fname = (எழுத்து *)vim_strsave(buf);
	    திரும்பு tb_blank_xpm;
	}
    }

    எனில் (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	எனில் (menu->iconidx >= 0 && menu->iconidx
	       < (முழு)(அளவு(built_in_pixmaps) / அளவு(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	இல்லையெனில்
	    xpm = tb_blank_xpm;
    }

    திரும்பு xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    நிலையான முழு
add_pixmap_args(vimmenu_T *menu, Arg *args, முழு n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    எனில் (menu->xpm == வெற்று)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    இல்லையெனில்
    {
	எனில் (menu->xpm_fname != வெற்று)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    திரும்பு n;
}
# endif // FEAT_TOOLBAR

    வெறுமை
gui_mch_add_menu_item(vimmenu_T *menu, முழு idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# எனில் (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    எனில் (menu_is_child_of_popup(menu) && !mouse_model_popup())
	திரும்பு;
# endif

# ifdef FEAT_TOOLBAR
    எனில் (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = வெற்று;    // fallback label if pixmap not found
	முழு		n;
	Arg		args[18];

	n = 0;
	எனில் (menu_is_separator(menu->name))
	{
	    எழுத்து	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (எழுத்து *)vim_strchr(menu->name, ':');
	    எனில் (cp != வெற்று)
		wid = (Dimension)atoi(++cp);
	    இல்லையெனில்
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	இல்லையெனில்
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNhighlightOnEnter, True); n++;
	    XtSetArg(args[n], XmNmarginWidth, 0); n++;
	    XtSetArg(args[n], XmNmarginHeight, 0); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((எழுத்து *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	எனில் (menu->id == வெற்று)
	{
	    menu->id = XtCreateManagedWidget((எழுத்து *)menu->dname,
			type, toolBar, args, n);
	    எனில் (menu->id != வெற்று && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	இல்லையெனில்
	    Xt_Set_மதிப்புகள்(menu->id, args, n);
	எனில் (xms != வெற்று)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = வெற்று;
	// When adding first item to toolbar it might have to be enabled .
	எனில் (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != வெற்று)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	திரும்பு;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    எனில் (parent->submenu_id == (Widget)0)
	திரும்பு;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    எனில் (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# எனில் (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (முழு)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		வெற்று);
	gui_motif_menu_colors(menu->id);
	திரும்பு;
    }

    label = XmStringCreate((எழுத்து *)menu->dname, STRING_TAG);
    எனில் (label == வெற்று)
	திரும்பு;
    menu->id = XtVaCreateWidget("subMenu",
	xm_தள்ளு_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# எனில் (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (முழு)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	வெற்று);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    எனில் (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# எனில் (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    வெறுமை
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    முழு		idx = 0;

    // When GUI hasn't started the menus have not been created.
    எனில் (!gui.in_use)
	திரும்பு;

    போது (menu)
    {
      எனில் (menu->children != வெற்று)
      {
	  எனில் (menu_is_popup(menu->name))
	  {
	      எனில் (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      இல்லையெனில்
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      இல்லையெனில் எனில் (menu_is_child_of_popup(menu))
      {
	  எனில் (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  இல்லையெனில்
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    வெறுமை
gui_mch_new_menu_colors(வெறுமை)
{
    எனில் (menuBar == (Widget)0)
	திரும்பு;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(வேர்_menu, TRUE);
}

    வெறுமை
gui_mch_new_menu_font(வெறுமை)
{
    எனில் (menuBar == (Widget)0)
	திரும்பு;
    submenu_change(வேர்_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_மதிப்புகள்(menuBar, XmNheight, &height, வெற்று);
	gui.menu_height = height;

	Xt_Va_Get_மதிப்புகள்(vimShell, XtNwidth, &w, XtNheight, &h, வெற்று);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# எனில் defined(FEAT_BEVAL_GUI)
    வெறுமை
gui_mch_new_tooltip_font(வெறுமை)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    எனில் (toolBar == (Widget)0)
	திரும்பு;

    menu = gui_find_menu((char_u *)"ToolBar");
    எனில் (menu != வெற்று)
	submenu_change(menu, FALSE);
#  endif
}

    வெறுமை
gui_mch_new_tooltip_colors(வெறுமை)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    எனில் (toolBar == (Widget)0)
	திரும்பு;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    எனில் (toolbar != வெற்று)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    நிலையான வெறுமை
submenu_change(
    vimmenu_T	*menu,
    முழு		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    க்கு (mp = menu; mp != வெற்று; mp = mp->next)
    {
	எனில் (mp->id != (Widget)0)
	{
	    எனில் (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		எனில் (mp->xpm != வெற்று)
		{
		    முழு		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_மதிப்புகள்(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		எனில் (mp->tip != வெற்று)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].மதிப்பு = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].மதிப்பு = gui.tooltip_fg_pixel;
		    Xt_Set_மதிப்புகள்(mp->tip->balloonLabel, &args[0], Xt_எண்(args));
		}
#  endif
# endif
	    }
	    இல்லையெனில்
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		எனில் (mp->tip != வெற்று)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].மதிப்பு = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_மதிப்புகள்(mp->tip->balloonLabel, &args[0], Xt_எண்(args));
		}
# endif
	    }
	}

	எனில் (mp->children != வெற்று)
	{
# எனில் (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    எனில் (mp->submenu_id != (Widget)0)
	    {
		எனில் (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		இல்லையெனில்
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
    வெறுமை
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    எனில் (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    எனில் (menu->id == (Widget)0)
	திரும்பு;

    Widget	    parent;

    parent = XtParent(menu->id);
# எனில் defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    எனில் (parent == toolBar && menu->tip != வெற்று)
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
	menu->tip = வெற்று;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    எனில் (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    இல்லையெனில் எனில் (parent == toolBar)
    {
	Cardinal    இலக்கம்_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_மதிப்புகள்(toolBar, XmNnumChildren, &இலக்கம்_children, வெற்று);
	எனில் (இலக்கம்_children == 0)
	    gui_mch_show_toolbar(FALSE);
	இல்லையெனில்
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    வெறுமை
gui_mch_show_popupmenu(vimmenu_T *menu UNUSED)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_last_mouse_event());
    XtManageChild(menu->submenu_id);
# endif
}

#endif // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    வெறுமை
gui_mch_def_colors(வெறுமை)
{
    எனில் (!gui.in_use)
	திரும்பு;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ifdef FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#endif
}


/*
 * Scrollbar stuff.
 */

    வெறுமை
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    எனில் (sb->id != (Widget)0)
	Xt_Va_Set_மதிப்புகள்(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  வெற்று);
}

    வெறுமை
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    முழு		x,
    முழு		y,
    முழு		w,
    முழு		h)
{
    எனில் (sb->id == (Widget)0)
	திரும்பு;

    எனில் (sb->type == SBAR_இடது || sb->type == SBAR_வலது)
    {
	எனில் (y == 0)
	    h -= gui.border_offset;
	இல்லையெனில்
	    y -= gui.border_offset;
	Xt_Va_Set_மதிப்புகள்(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		வெற்று);
    }
    இல்லையெனில்
	Xt_Va_Set_மதிப்புகள்(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_வலது]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		வெற்று);
    XtManageChild(sb->id);
}

    முழு
gui_mch_get_scrollbar_xpadding(வெறுமை)
{
    முழு xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_மதிப்புகள்(textArea, XtNwidth, &tw, XtNx, &tx, வெற்று);
    Xt_Va_Get_மதிப்புகள்(vimShell, XtNwidth, &ww, வெற்று);
    xpad = ww - tw - tx - gui.scrollbar_width;
    திரும்பு (xpad < 0) ? 0 : xpad;
}

    முழு
gui_mch_get_scrollbar_ypadding(வெறுமை)
{
    முழு ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_மதிப்புகள்(textArea, XtNheight, &th, XtNy, &ty, வெற்று);
    Xt_Va_Get_மதிப்புகள்(vimShell, XtNheight, &wh, வெற்று);
    ypad = wh - th - ty - gui.scrollbar_height;
    திரும்பு (ypad < 0) ? 0 : ypad;
}

    வெறுமை
gui_mch_enable_scrollbar(scrollbar_T *sb, முழு flag)
{
    Arg		args[16];
    முழு		n;

    எனில் (sb->id == (Widget)0)
	திரும்பு;

    n = 0;
    எனில் (flag)
    {
	தேர்வு (sb->type)
	{
	    நிலை SBAR_இடது:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		நிறுத்து;

	    நிலை SBAR_வலது:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		நிறுத்து;

	    நிலை SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		நிறுத்து;
	}
	Xt_Set_மதிப்புகள்(textArea, args, n);
	XtManageChild(sb->id);
    }
    இல்லையெனில்
    {
	எனில் (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    தேர்வு (sb->type)
	    {
		நிலை SBAR_இடது:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    நிறுத்து;

		நிலை SBAR_வலது:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    நிறுத்து;

		நிலை SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    நிறுத்து;
	    }
	    Xt_Set_மதிப்புகள்(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    வெறுமை
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    முழு		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    முழு		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    தேர்வு (sb->type)
    {
	நிலை SBAR_இடது:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    நிறுத்து;

	நிலை SBAR_வலது:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    நிறுத்து;

	நிலை SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    நிறுத்து;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    எனில் (sb->id == (Widget)0)
	திரும்பு;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, சாவி_Press_Mask, FALSE, gui_x11_சாவி_hit_cb,
	    (XtPointer)0);
}

    வெறுமை
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    எனில் (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    வெறுமை
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    எனில் (sb->id != (Widget)0)
    {
	எனில் (gui.scroll_bg_pixel != INVALCOLOR)
	{
#எனில் (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_மதிப்புகள்(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    வெற்று);
	}

	எனில் (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_மதிப்புகள்(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    வெற்று);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    எனில் (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(வெறுமை)
{
    திரும்பு(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    நிலையான வெறுமை
do_mnemonic(Widget w, unsigned முழு keycode)
{
    WidgetList	    children;
    முழு		    இலக்கம்_Children, i;
    Boolean	    isMenu;
    சாவி_Sym	    mnemonic = '\0';
    எழுத்து	    mneString[2];
    Widget	    userData;
    unsigned எழுத்து   rowColType;

    எனில் (XtIsComposite(w))
    {
	எனில் (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_மதிப்புகள்(w, XmNrowColumnType, &rowColType, வெற்று);
	    isMenu = (rowColType != (unsigned எழுத்து)XmWORK_AREA);
	}
	இல்லையெனில்
	    isMenu = False;
	எனில் (!isMenu)
	{
	    Xt_Va_Get_மதிப்புகள்(w, XmNchildren, &children, XmNnumChildren,
			  &இலக்கம்_Children, வெற்று);
	    க்கு (i = 0; i < இலக்கம்_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    இல்லையெனில்
    {
	Xt_Va_Get_மதிப்புகள்(w, XmNmnemonic, &mnemonic, வெற்று);
	எனில் (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    எனில் (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		எனில் (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_மதிப்புகள்(w, XmNuserData, &userData, வெற்று);
		    எனில் (userData != வெற்று && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		இல்லையெனில்
		{
		    XKeyPressedEvent சாவி_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(சாவி_Event);
		    சாவி_Event.type = சாவி_Press;
		    சாவி_Event.serial = 1;
		    சாவி_Event.send_event = True;
		    சாவி_Event.display = XtDisplay(w);
		    சாவி_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & சாவி_Event,
								     வெற்று, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    நிலையான வெறுமை
mnemonic_event(
	Widget	    w,
	XtPointer   call_data UNUSED,
	XKeyEvent   *event,
	Boolean	    *b UNUSED)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    நிலையான வெறுமை
add_mnemonic_grabs(Widget dialog, Widget w)
{
    எழுத்து	    mneString[2];
    WidgetList	    children;
    முழு		    இலக்கம்_Children, i;
    Boolean	    isMenu;
    சாவி_Sym	    mnemonic = '\0';
    unsigned எழுத்து   rowColType;

    எனில் (XtIsComposite(w))
    {
	எனில் (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_மதிப்புகள்(w, XmNrowColumnType, &rowColType, வெற்று);
	    isMenu = (rowColType != (unsigned எழுத்து)XmWORK_AREA);
	}
	இல்லையெனில்
	    isMenu = False;
	எனில் (!isMenu)
	{
	    Xt_Va_Get_மதிப்புகள்(w, XmNchildren, &children, XmNnumChildren,
							  &இலக்கம்_Children, வெற்று);
	    க்கு (i = 0; i < இலக்கம்_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    இல்லையெனில்
    {
	Xt_Va_Get_மதிப்புகள்(w, XmNmnemonic, &mnemonic, வெற்று);
	எனில் (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_சாவி(dialog, XKeysymToKeycode(XtDisplay(dialog),
						  XStringToKeysym(mneString)),
		    Mod1Mask, True, GrabModeAsync, GrabModeAsync);
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
    நிலையான வெறுமை
activate_dialog_mnemonics(Widget dialog)
{
    எனில் (!dialog)
	திரும்பு;

    XtAddEventHandler(dialog, சாவி_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) வெற்று);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    நிலையான வெறுமை
suppress_dialog_mnemonics(Widget dialog)
{
    எனில் (!dialog)
	திரும்பு;

    Xt_Ungrab_சாவி(dialog, Any_சாவி, Mod1Mask);
    XtRemoveEventHandler(dialog, சாவி_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) வெற்று);
}

#எனில் defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    நிலையான வெறுமை
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    எனில் (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	எனில் (fl != வெற்று)
	{
	    எனில் (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    இல்லையெனில்
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
	    XmFontListFree(fl);
	}
    }
# இல்லையெனில்
    எனில் (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	எனில் (fl != வெற்று)
	{
	    எனில் (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    இல்லையெனில்
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#எனில் defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

வகைவரை கட்டமைப்பு dialog_callback_arg
{
    எழுத்து *  args;   // not used right now
    முழு	    id;
} dcbarg_T;

நிலையான Widget dialog_wgt;
நிலையான எழுத்து *browse_fname = வெற்று;
நிலையான XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

நிலையான வெறுமை DialogCancelCB(Widget, XtPointer, XtPointer);
நிலையான வெறுமை DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    நிலையான வெறுமை
set_predefined_label(Widget parent, String name, எழுத்து *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    சாவி_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    எனில் (!w)
	திரும்பு;

    p = vim_strsave((char_u *)new_label);
    எனில் (p == வெற்று)
	திரும்பு;
    க்கு (next = p; *next; ++next)
    {
	எனில் (*next == DLG_HOTKEY_CHAR)
	{
	    முழு len = STRLEN(next);

	    எனில் (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((எழுத்து *)p, STRING_TAG);
    vim_free(p);

    எனில் (str != வெற்று)
    {
	Xt_Va_Set_மதிப்புகள்(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		வெற்று);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    நிலையான வெறுமை
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    எனில் (!w)
	திரும்பு;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    முழு		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = வெற்று;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", வெற்று, 0);

    எனில் (initdir == வெற்று || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    எனில் (dflt == வெற்று)
	dflt = (char_u *)"";
    இல்லையெனில் எனில் (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	add_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    எனில் (filter == வெற்று)
	pattern = (char_u *)"";
    இல்லையெனில்
    {
	char_u	*s, *p;

	s = filter;
	க்கு (p = filter; *p != NUL; ++p)
	{
	    எனில் (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    எனில் (*p == ';' || *p == '\n')	// end of (first) pattern
		நிறுத்து;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	எனில் (pattern == வெற்று)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_மதிப்புகள்(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (எழுத்து *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (எழுத்து *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (எழுத்து *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (எழுத்து *)title, STRLEN(title) + 1,
	வெற்று);

    set_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    set_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    set_predefined_label(dialog_wgt, "Dir", _("Directories"));
    set_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    set_predefined_label(dialog_wgt, "Help", _("&Help"));
    set_predefined_label(dialog_wgt, "Items", _("Files"));
    set_predefined_label(dialog_wgt, "OK", _("&OK"));
    set_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    set_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    set_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    எனில் (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_மதிப்புகள்(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, வெற்று);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned எழுத்து)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    செய்
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_உள்ளீடு_Mask)XtIMAll);
    } போது (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    எனில் (browse_fname == வெற்று)
	திரும்பு வெற்று;
    திரும்பு vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    நிலையான வெறுமை
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    எனில் (browse_fname != வெற்று)
    {
	XtFree(browse_fname);
	browse_fname = வெற்று;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    நிலையான வெறுமை
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    எனில் (browse_fname != வெற்று)
    {
	XtFree(browse_fname);
	browse_fname = வெற்று;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->மதிப்பு, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#எனில் defined(FEAT_GUI_DIALOG)

நிலையான முழு	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    நிலையான வெறுமை
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    எழுத்து	buf[2];
    சாவி_Sym	சாவி_sym;

    எனில் (XLookupString(&(event->xkey), buf, 2, &சாவி_sym, வெற்று) == 1)
    {
	எனில் (*buf == CAR)
	    dialogStatus = 1;
	இல்லையெனில் எனில் (*buf == ESC)
	    dialogStatus = 2;
    }
    எனில் ((சாவி_sym == XK_இடது || சாவி_sym == XK_வலது)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    நிலையான வெறுமை
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (முழு)(long)client_data + 1;
}

# ifdef HAVE_XPM

    நிலையான Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    எழுத்து	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    முழு			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_குறியீடு	color[5] =
    {
	{"none", வெற்று, 0},
	{"iconColor1", வெற்று, 0},
	{"bottomShadowColor", வெற்று, 0},
	{"topShadowColor", வெற்று, 0},
	{"selectColor", வெற்று, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_மதிப்புகள்(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  வெற்று);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, வேர்_Window_Of_Screen(scr),
		    data, &pixmap, வெற்று, &attr);

    Xt_Va_Get_மதிப்புகள்(label, XmNrecomputeSize, &rs, வெற்று);
    Xt_Va_Set_மதிப்புகள்(label, XmNrecomputeSize, True, வெற்று);
    Xt_Va_Set_மதிப்புகள்(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    வெற்று);
    Xt_Va_Set_மதிப்புகள்(label, XmNrecomputeSize, rs, வெற்று);

    திரும்பு label;
}
# endif

    முழு
gui_mch_dialog(
    முழு		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    முழு		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    முழு		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    முழு			butcount;
    Widget		w;
    Widget		dialogform = வெற்று;
    Widget		form = வெற்று;
    Widget		dialogtextfield = வெற்று;
    Widget		*buttons;
    Widget		sep_form = வெற்று;
    Boolean		vertical;
    Widget		separator = வெற்று;
    முழு			n;
    Arg			args[6];
# ifdef HAVE_XPM
    எழுத்து		**icon_data = வெற்று;
    Widget		dialogpixmap = வெற்று;
# endif

    எனில் (title == வெற்று)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (எழுத்து *)"dialog", வெற்று, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != வெற்று);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((எழுத்து *)title);
    எனில் (label == வெற்று)
	திரும்பு -1;
    Xt_Va_Set_மதிப்புகள்(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    வெற்று);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    எனில் (buts == வெற்று)
	திரும்பு -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    க்கு (p = buts; *p; ++p)
	எனில் (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    எனில் (buttons == வெற்று)
    {
	vim_free(buts);
	திரும்பு -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    க்கு (butcount = 0; *p; ++butcount)
    {
	சாவி_Sym mnemonic = NUL;

	க்கு (next = p; *next; ++next)
	{
	    எனில் (*next == DLG_HOTKEY_CHAR)
	    {
		முழு len = STRLEN(next);

		எனில் (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    எனில் (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		நிறுத்து;
	    }
	}
	label = XmStringCreate(_((எழுத்து *)p), STRING_TAG);
	எனில் (label == வெற்று)
	    நிறுத்து;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_தள்ளு_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		வெற்று);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	எனில் (butcount > 0)
	{
	    எனில் (vertical)
		Xt_Va_Set_மதிப்புகள்(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			வெற்று);
	    இல்லையெனில்
	    {
		எனில் (*next == NUL)
		{
		    Xt_Va_Set_மதிப்புகள்(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    வெற்று);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    வெற்று);
		    XtManageChild(sep_form);
		}
		இல்லையெனில்
		{
		    Xt_Va_Set_மதிப்புகள்(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    வெற்று);
		}
	    }
	}
	இல்லையெனில் எனில் (!vertical)
	{
	    எனில் (*next == NUL)
	    {
		Xt_Va_Set_மதிப்புகள்(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			வெற்று);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			வெற்று);
		XtManageChild(sep_form);
	    }
	    இல்லையெனில்
		Xt_Va_Set_மதிப்புகள்(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			வெற்று);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    எனில் (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	XtSetArg(args[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(args[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(args[n], XmNbottomWidget, buttons[0]); n++;
	XtSetArg(args[n], XmNbottomOffset, 4); n++;
	XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", args, n);
	XtManageChild(separator);
    }

    எனில் (textfield != வெற்று)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		வெற்று);
	எனில் (butcount > 0)
	    Xt_Va_Set_மதிப்புகள்(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    வெற்று);
	இல்லையெனில்
	    Xt_Va_Set_மதிப்புகள்(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    வெற்று);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (எழுத்து *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, சாவி_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)வெற்று);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    வெற்று);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    தேர்வு (type)
    {
	நிலை VIM_GENERIC:
	    icon_data = generic_xpm;
	    நிறுத்து;
	நிலை VIM_ERROR:
	    icon_data = error_xpm;
	    நிறுத்து;
	நிலை VIM_WARNING:
	    icon_data = alert_xpm;
	    நிறுத்து;
	நிலை VIM_INFO:
	    icon_data = info_xpm;
	    நிறுத்து;
	நிலை VIM_QUESTION:
	    icon_data = quest_xpm;
	    நிறுத்து;
	இயல்பு:
	    icon_data = generic_xpm;
    }

    n = 0;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNtopOffset, 8); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomOffset, 8); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNleftOffset, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_data, args, n);
    XtManageChild(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((எழுத்து *)message, STRING_TAG);
    எனில் (label == வெற்று)
    {
	vim_free(buttons);
	திரும்பு -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				XmNtopOffset, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# இல்லையெனில்
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				வெற்று);
    XmStringFree(label);
    set_fontlist(w);

    எனில் (textfield != வெற்று)
    {
	Xt_Va_Set_மதிப்புகள்(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		வெற்று);
    }
    இல்லையெனில்
    {
	எனில் (butcount > 0)
	    Xt_Va_Set_மதிப்புகள்(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    வெற்று);
	இல்லையெனில்
	    Xt_Va_Set_மதிப்புகள்(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    வெற்று);
    }

    எனில் (dfltbutton < 1)
	dfltbutton = 1;
    எனில் (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_மதிப்புகள்(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], வெற்று);
    எனில் (textfield != வெற்று)
	Xt_Va_Set_மதிப்புகள்(dialogform, XmNinitialFocus, dialogtextfield, வெற்று);
    இல்லையெனில்
	Xt_Va_Set_மதிப்புகள்(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									வெற்று);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    எனில் (textfield != வெற்று && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	XmTextFieldSetSelection(dialogtextfield,
			 (XmTextPosition)0, (XmTextPosition)STRLEN(textfield),
					   XtLastTimestampProcessed(gui.dpy));
	XmTextFieldSetCursorPosition(dialogtextfield,
					   (XmTextPosition)STRLEN(textfield));
    }

    app = XtWidgetToApplicationContext(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialogStatus = -1;
    க்கு (;;)
    {
	XtAppProcessEvent(app, (Xt_உள்ளீடு_Mask)XtIMAll);
	எனில் (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    நிறுத்து;
    }

    vim_free(buttons);

    எனில் (textfield != வெற்று)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	எனில் (p == வெற்று || dialogStatus < 0)
	    *textfield = NUL;
	இல்லையெனில்
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((எழுத்து *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    திரும்பு dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#எனில் defined(FEAT_TOOLBAR)
    வெறுமை
gui_mch_show_toolbar(முழு showit)
{
    Cardinal	இலக்கம்_Children;	    // how many children toolBar has

    எனில் (toolBar == (Widget)0)
	திரும்பு;
    Xt_Va_Get_மதிப்புகள்(toolBar, XmNnumChildren, &இலக்கம்_Children, வெற்று);
    எனில் (showit && இலக்கம்_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_மதிப்புகள்(toolBar, XmNchildren, &children, வெற்று);
	{
	    வெறுமை    (*action)(BalloonEval *);
	    முழு	    text = 0;

	    எனில் (strstr((மாறிலி எழுத்து *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    இல்லையெனில்
		action = &gui_mch_disable_beval_area;
	    எனில் (strstr((மாறிலி எழுத்து *)p_toolbar, "text"))
		text = 1;
	    இல்லையெனில் எனில் (strstr((மாறிலி எழுத்து *)p_toolbar, "icons"))
		text = -1;
	    எனில் (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    எனில் (menu_is_toolbar(toolbar->dname))
			நிறுத்து;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		எனில் (toolbar)
		{
		    க்கு (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			முழு	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			எனில் (cur->tip != வெற்று)
			    (*action)(cur->tip);
			எனில் (!menu_is_separator(cur->name))
			{
			    எனில் (text == 1 || cur->xpm == வெற்று)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    எனில் (cur->id != வெற்று)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_மதிப்புகள்(cur->id, args, n);
				XtManageChild(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	XtManageChild(XtParent(toolBar));
# ifdef FEAT_GUI_TABLINE
	எனில் (showing_tabline)
	{
	    Xt_Va_Set_மதிப்புகள்(tab_வரி,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  வெற்று);
	    Xt_Va_Set_மதிப்புகள்(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_வரி,
			  வெற்று);
	}
	இல்லையெனில்
# endif
	    Xt_Va_Set_மதிப்புகள்(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  வெற்று);
	எனில் (XtIsManaged(menuBar))
	    Xt_Va_Set_மதிப்புகள்(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    வெற்று);
	இல்லையெனில்
	    Xt_Va_Set_மதிப்புகள்(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    வெற்று);
    }
    இல்லையெனில்
    {
	gui.toolbar_height = 0;
	எனில் (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      வெற்று);
	}
	இல்லையெனில்
	{
# ifdef FEAT_GUI_TABLINE
	    எனில் (showing_tabline)
	    {
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_FORM,
			      வெற்று);
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_வரி,
			      வெற்று);
	    }
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      வெற்று);
	}

	XtUnmanageChild(XtParent(toolBar));
    }
    gui_set_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    நிலையான வெறுமை
reset_focus(வெறுமை)
{
    எனில் (textArea != வெற்று)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    முழு
gui_mch_compute_toolbar_height(வெறுமை)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	இலக்கம்_Children;	    // how many children toolBar has
    முழு		i;

    borders = 0;
    height = 0;
    எனில் (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_மதிப்புகள்(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		வெற்று);
	borders += fst + fmh;
	Xt_Va_Get_மதிப்புகள்(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &இலக்கம்_Children, வெற்று);
	borders += tst + tmh;
	க்கு (i = 0; i < (முழு)இலக்கம்_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_மதிப்புகள்(children[i], XmNheight, &whgt, வெற்று);
	    எனில் (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    எனில் (height < 10)
	height = 24;
# endif

    திரும்பு (முழு)(height + (borders << 1));
}

    வெறுமை
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_மதிப்புகள்(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    வெற்று);
}
#endif

#எனில் defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    வெறுமை
gui_mch_show_tabline(முழு showit)
{
    எனில் (tab_வரி == (Widget)0)
	திரும்பு;

    எனில் (!showit != !showing_tabline)
    {
	எனில் (showit)
	{
	    XtManageChild(tab_வரி);
	    XtUnmanageChild(XtNameToWidget(tab_வரி, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_வரி, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_வரி,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    எனில் (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), வெற்று);
	    இல்லையெனில்
#  endif
		எனில் (XtIsManaged(menuBar))
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, வெற்று);
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(tab_வரி,
			      XmNtopAttachment, XmATTACH_FORM, வெற்று);
	    Xt_Va_Set_மதிப்புகள்(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_வரி,
			  வெற்று);
	}
	இல்லையெனில்
	{
	    XtUnmanageChild(tab_வரி);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    எனில் (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), வெற்று);
	    இல்லையெனில்
#  endif
		எனில் (XtIsManaged(menuBar))
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, வெற்று);
	    இல்லையெனில்
# endif
		Xt_Va_Set_மதிப்புகள்(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, வெற்று);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    முழு
gui_mch_showing_tabline(வெறுமை)
{
    திரும்பு tab_வரி != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    வெறுமை
gui_mch_update_tabline(வெறுமை)
{
    tabpage_T		*tp;
    முழு			nr = 1, n;
    Arg			args[10];
    முழு			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    முழு			last_page, tab_மொத்தம்;
    XmString		label_str;
    எழுத்து		*label_cstr;
    BalloonEval		*beval;

    எனில் (tab_வரி == (Widget)0)
	திரும்பு;

    // Add a label for each tab page.  They all contain the same text area.
    க்கு (tp = first_tabpage; tp != வெற்று; tp = tp->tp_next, ++nr)
    {
	எனில் (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_வரி, nr, &page_info);
	எனில் (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_தள்ளு_Button(tab_வரி, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, வெற்று, tabline_balloon_cb,
									வெற்று);
	    Xt_Va_Set_மதிப்புகள்(tab, XmNuserData, beval, வெற்று);
	}
	இல்லையெனில்
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_மதிப்புகள்(tab, Xm_Npage_எண், nr, வெற்று);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_மதிப்புகள்(tab, XmNlabelString, &label_str, வெற்று);
	எனில் (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    எனில் (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_மதிப்புகள்(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, வெற்று);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_மொத்தம் = nr - 1;

    Xt_Va_Get_மதிப்புகள்(tab_வரி, Xm_Nlast_Page_எண், &last_page, வெற்று);

    // Remove any old labels.
    போது (nr <= last_page)
    {
	எனில் (XmNotebookGetPageInfo(tab_வரி, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_எண் == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_மதிப்புகள்(page_info.major_tab_widget, XmNuserData, &beval, வெற்று);
	    எனில் (beval != வெற்று)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_மதிப்புகள்(tab_வரி, Xm_Nlast_Page_எண், tab_மொத்தம், வெற்று);

    Xt_Va_Get_மதிப்புகள்(tab_வரி, Xm_Ncurrent_Page_எண், &currentpage, வெற்று);
    எனில் (currentpage != curtabidx)
	Xt_Va_Set_மதிப்புகள்(tab_வரி, Xm_Ncurrent_Page_எண், curtabidx, வெற்று);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    வெறுமை
gui_mch_set_curtab(முழு nr)
{
    முழு		currentpage;

    எனில் (tab_வரி == (Widget)0)
	திரும்பு;

    Xt_Va_Get_மதிப்புகள்(tab_வரி, Xm_Ncurrent_Page_எண், &currentpage, வெற்று);
    எனில் (currentpage != nr)
	Xt_Va_Set_மதிப்புகள்(tab_வரி, Xm_Ncurrent_Page_எண், nr, வெற்று);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    நிலையான வெறுமை
gui_motif_menu_colors(Widget id)
{
    எனில் (gui.menu_bg_pixel != INVALCOLOR)
#எனில் (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#இல்லையெனில்
	Xt_Va_Set_மதிப்புகள்(id, XmNbackground, gui.menu_bg_pixel, வெற்று);
#endif
    எனில் (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_மதிப்புகள்(id, XmNforeground, gui.menu_fg_pixel, வெற்று);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    நிலையான வெறுமை
gui_motif_scroll_colors(Widget id)
{
    எனில் (gui.scroll_bg_pixel != INVALCOLOR)
#எனில் (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#இல்லையெனில்
	Xt_Va_Set_மதிப்புகள்(id, XmNbackground, gui.scroll_bg_pixel, வெற்று);
#endif
    எனில் (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_மதிப்புகள்(id, XmNforeground, gui.scroll_fg_pixel, வெற்று);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    வெறுமை
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    எனில் (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	எனில் (fl != வெற்று)
	{
	    எனில் (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    இல்லையெனில்
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
	    XmFontListFree(fl);
	}
    }
# இல்லையெனில்
    எனில் (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	எனில் (fl != வெற்று)
	{
	    எனில் (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    இல்லையெனில்
		Xt_Va_Set_மதிப்புகள்(id, XmNfontList, fl, வெற்று);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

வகைவரை கட்டமைப்பு _SharedFindReplace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget up;		// search direction 'Up' radio button
    Widget down;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget find;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} SharedFindReplace;

நிலையான SharedFindReplace find_widgets = {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};
நிலையான SharedFindReplace repl_widgets = {வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று, வெற்று};

    நிலையான வெறுமை
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    எனில் (cd != வெற்று)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    நிலையான வெறுமை
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    எனில் (cd != வெற்று)
	XtUnmanageChild(cd->dialog);
}

    நிலையான வெறுமை
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    நிலையான வெறுமை
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    எழுத்து	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    எனில் (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	திரும்பு;
    }

    // Get the search/replace strings from the dialog
    எனில் (flags == FRD_FINDNEXT)
    {
	repl_text = வெற்று;
	sfr = &find_widgets;
    }
    இல்லையெனில்
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_மதிப்புகள்(sfr->down, XmNset, &direction_down, வெற்று);
    Xt_Va_Get_மதிப்புகள்(sfr->wword, XmNset, &wword, வெற்று);
    Xt_Va_Get_மதிப்புகள்(sfr->mcase, XmNset, &mcase, வெற்று);
    எனில் (wword)
	flags |= FRD_WHOLE_WORD;
    எனில் (mcase)
	flags |= FRD_MATCH_CASE;

    (வெறுமை)gui_do_findrepl((முழு)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    எனில் (find_text != வெற்று)
	XtFree(find_text);
    எனில் (repl_text != வெற்று)
	XtFree(repl_text);
}

    நிலையான வெறுமை
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    சாவி_Sym keysym;

    எனில் (frdp == வெற்று)
	திரும்பு;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    எனில் (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    நிலையான வெறுமை
set_label(Widget w, எழுத்து *label)
{
    XmString	str;
    char_u	*p, *next;
    சாவி_Sym	mnemonic = NUL;

    எனில் (!w)
	திரும்பு;

    p = vim_strsave((char_u *)label);
    எனில் (p == வெற்று)
	திரும்பு;
    க்கு (next = p; *next; ++next)
    {
	எனில் (*next == DLG_HOTKEY_CHAR)
	{
	    முழு len = STRLEN(next);

	    எனில் (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((எழுத்து *)p);
    vim_free(p);
    எனில் (str)
    {
	Xt_Va_Set_மதிப்புகள்(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		வெற்று);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    நிலையான வெறுமை
find_replace_dialog_create(char_u *arg, முழு do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		உள்ளீடு_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    முழு			n;
    Arg			args[6];
    முழு			wword = FALSE;
    முழு			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    எனில் (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	எனில் (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	இல்லையெனில்
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	எனில் (entry_text != வெற்று)
	    XmTextFieldSetString(frdp->what, (எழுத்து *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_மதிப்புகள்(frdp->wword, XmNset, wword, வெற்று);
	திரும்பு;
    }

    // Create a fresh new dialog window
    எனில் (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    இல்லையெனில்
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    XtSetArg(args[n], XmNautoUnmanage, False); n++;
    XtSetArg(args[n], XmNnoResize, True); n++;
    XtSetArg(args[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", args, n);
    XmStringFree(str);
    XtAddCallback(frdp->dialog, XmNdestroyCallback,
	    find_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    வெற்று);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_தள்ளு_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    வெற்று);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    எனில் (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_தள்ளு_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		வெற்று);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_தள்ளு_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		வெற்று);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_தள்ளு_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		வெற்று);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_தள்ளு_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    வெற்று);
    set_label(frdp->cancel, _("&Cancel"));
    XtAddCallback(frdp->cancel, XmNactivateCallback,
	    find_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    XtManageChild(button_form);

    n = 0;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(args[n], XmNrightWidget, button_form); n++;
    XtSetArg(args[n], XmNrightOffset, 4); n++;
    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", args, n);
    XtManageChild(separator);

    உள்ளீடு_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    வெற்று);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, உள்ளீடு_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		வெற்று);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, உள்ளீடு_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		வெற்று);

	எனில் (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	உள்ளீடு_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    வெற்று);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, உள்ளீடு_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    வெற்று);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, சாவி_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	இல்லையெனில்
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, சாவி_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_மதிப்புகள்(label_what, args, n);
	widest = width;
	எனில் (do_replace)
	{
	    Xt_Get_மதிப்புகள்(label_with, args, n);
	    எனில் (width > widest)
		widest = width;
	}

	Xt_Va_Set_மதிப்புகள்(frdp->what, XmNleftOffset, widest, வெற்று);
	எனில் (do_replace)
	    Xt_Va_Set_மதிப்புகள்(frdp->with, XmNleftOffset, widest, வெற்று);

    }

    XtManageChild(உள்ளீடு_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, உள்ளீடு_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, உள்ளீடு_form,
		வெற்று);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		வெற்று);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)வெற்று, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		வெற்று);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		வெற்று);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->down);

	XtManageChild(radio_box);
	XtManageChild(frame);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, frame,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, உள்ளீடு_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    வெற்று);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    வெற்று);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    XmNtopOffset, 4,
	    XmNset, mcase,
	    வெற்று);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    எனில் (entry_text != வெற்று)
	XmTextFieldSetString(frdp->what, (எழுத்து *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   வெறுமை
gui_mch_find_dialog(exarg_T *eap)
{
    எனில் (!gui.in_use)
	திரும்பு;

    find_replace_dialog_create(eap->arg, FALSE);
}


    வெறுமை
gui_mch_replace_dialog(exarg_T *eap)
{
    எனில் (!gui.in_use)
	திரும்பு;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    வெறுமை
gui_motif_synch_fonts(வெறுமை)
{
    SharedFindReplace *frdp;
    முழு		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    எனில் (font == வெற்று)
	திரும்பு;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    க்கு (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	எனில் (frdp->dialog)
	{
	    Xt_Va_Set_மதிப்புகள்(frdp->what, XmNfontList, font_list, வெற்று);
	    எனில் (do_replace)
		Xt_Va_Set_மதிப்புகள்(frdp->with, XmNfontList, font_list, வெற்று);
	}
    }

    XmFontListFree(font_list);
}
