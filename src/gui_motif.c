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
#اگر (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#شامل <Xm/Frame.h>
#شامل <Xm/LabelG.h>
#شامل <Xm/ToggleBG.h>
#شامل <Xm/SeparatoG.h>
#شامل <Xm/XmP.h>

#شامل <X11/keysym.h>
#شامل <X11/Xatom.h>
#شامل <X11/StringDefs.h>
#شامل <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# اگر defined(VMS)
#  include <xpm.h>
# وگرنه
#  include <X11/xpm.h>
# endif
#وگرنه
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#شامل "gui_xmebw.h"	// for our Enhanced Button Widget

#اگر defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

بیرونی Widget vimShell;

ایستا Widget vimForm;
ایستا Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
ایستا Widget toolBarFrame;
ایستا Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
ایستا Widget	tab_سطر;
ایستا Widget	tab_سطر_menu = 0;
ایستا صحیح	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# اگر (XmVersion >= 1002)
// remember the last set value for the tearoff item
ایستا صحیح tearoff_val = (صحیح)XmTEAR_OFF_ENABLED;
# endif
ایستا Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
ایستا تهی reset_focus(تهی);
#endif

ایستا تهی gui_motif_menu_colors(Widget id);
ایستا تهی gui_motif_scroll_colors(Widget id);

#اگر (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#وگرنه
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    ایستا تهی
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	مقدار;
    صحیح		dragging;

    sb = gui_find_scrollbar((long)client_data);

    مقدار = ((XmScrollBarCallbackStruct *)call_data)->مقدار;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (صحیح)XmCR_DRAG);
    gui_drag_scrollbar(sb, مقدار, dragging);
}

#ifdef FEAT_GUI_TABLINE
    ایستا تهی
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    اگر (nptr->reason != (صحیح)XmCR_NONE)
	send_tabline_event(nptr->page_عدد);
}

    ایستا تهی
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_مقادیر(w, XmNuserData, &cmd, پوچ);
    Xt_Va_Get_مقادیر(tab_سطر_menu, XmNuserData, &tab_idx, پوچ);

    send_tabline_menu_event((صحیح)(long)tab_idx, (صحیح)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ایستا تهی
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((صحیح *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ایستا صحیح
tabline_scroller_clicked(
    نویسه		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_سطر, scroller_name);
    اگر (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_مقادیر(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, پوچ);
	اگر (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    اگر (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		بازگشت TRUE;
	}
    }
    بازگشت FALSE;
}

    ایستا تهی
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحیح				tab_idx = 0;
    WidgetList			children;
    Cardinal			رقم_Children;
    ایستا XtIntervalId		timer = (XtIntervalId)0;
    ایستا صحیح			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    اگر (event->button == Button1)
    {
	اگر (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    بازگشت;

	اگر (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	وگرنه
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	بازگشت;
    }

    اگر (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_مقادیر(tab_سطر_menu, XmNuserData, &tab_idx, پوچ);
	send_tabline_menu_event(tab_idx, (صحیح)TABLINE_MENU_CLOSE);
	بازگشت;
    }

    اگر (event->button != Button3)
	بازگشت;

    // When ignoring events don't show the menu.
    اگر (hold_gui_events || cmdwin_type != 0)
	بازگشت;

    اگر (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	اگر (tab_w != (Widget)0 && Xm_Is_افزودن_Button(tab_w))
	    Xt_Va_Get_مقادیر(tab_w, Xm_Npage_عدد, &tab_idx, پوچ);
    }

    Xt_Va_Set_مقادیر(tab_سطر_menu, XmNuserData, (XtPointer)(long)tab_idx, پوچ);
    Xt_Va_Get_مقادیر(tab_سطر_menu, XmNchildren, &children, XmNnumChildren,
		  &رقم_Children, پوچ);
    XtManageChildren(children, رقم_Children);
    XmMenuPosition(tab_سطر_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_سطر_menu);
}

    ایستا تهی
tabline_balloon_cb(BalloonEval *beval, صحیح state UNUSED)
{
    صحیح		nr;
    tabpage_T	*tp;

    اگر (beval->target == (Widget)0)
	بازگشت;

    Xt_Va_Get_مقادیر(beval->target, Xm_Npage_عدد, &nr, پوچ);
    tp = find_tabpage(nr);
    اگر (tp == پوچ)
	بازگشت;

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

#شامل <Xm/XmP.h>
#شامل <Xm/LabelP.h>

ایستا XtExposeProc old_label_expose = پوچ;

    ایستا تهی
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned نویسه   label_type = (صحیح)XmSTRING;

    Xt_Va_Get_مقادیر(_w, XmNlabelType, &label_type, (XtPointer)0);

    اگر (XtIsSensitive(_w) || label_type != (صحیح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وگرنه
    {
	XGCValues   مقادیر;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	مقادیر.graphics_exposures = False;

	اگر (fs != 0)
	{
	    mask |= GCFont;
	    مقادیر.font = fs->fid;
	}

	اگر (lw->primitive.بالاترین_shadow_pixmap != None
		&& lw->primitive.بالاترین_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    مقادیر.fill_style = FillTiled;
	    مقادیر.tile = lw->primitive.بالاترین_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	اگر (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	مقادیر.foreground = lw->primitive.بالاترین_shadow_color;
	مقادیر.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &مقادیر, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	اگر (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	مقادیر.foreground = lw->primitive.bottom_shadow_color;
	مقادیر.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &مقادیر, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    تهی
gui_x11_create_widgets(تهی)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    صحیح		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    اگر (old_label_expose == پوچ)
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
#اگر (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_مقادیر(vimShell, XmNdeleteResponse, XmDO_NOTHING, پوچ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	پوچ);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحیح ac = 0;

# اگر (XmVersion >= 1002)
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
	پوچ);
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
	پوچ);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_عدد, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_بالاترین_راست); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سطر = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_سطر, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			پوچ);
    XtAddEventHandler(tab_سطر, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, پوچ);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_سطر, "MinorTabScrollerNext");
    Xt_Va_Set_مقادیر(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, پوچ);
    scroller = XtNameToWidget(tab_سطر, "MinorTabScrollerPrevious");
    Xt_Va_Set_مقادیر(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, پوچ);

    // Create the tabline popup menu
    tab_سطر_menu = XmCreatePopupMenu(tab_سطر, "tabline popup", پوچ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((نویسه *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, پوچ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((نویسه *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, پوچ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((نویسه *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_افزودن_Button(tab_سطر_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, پوچ);
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
	پوچ);
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
	پوچ);

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
    تهی
gui_x11_destroy_widgets(تهی)
{
    textArea = پوچ;
#ifdef FEAT_MENU
    menuBar = پوچ;
#endif
}

    تهی
gui_mch_set_text_area_pos(
    صحیح	    x UNUSED,
    صحیح	    y UNUSED,
    صحیح	    w UNUSED,
    صحیح	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    تهی
gui_x11_set_back_color(تهی)
{
    اگر (textArea != پوچ)
#اگر (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#وگرنه
	Xt_Va_Set_مقادیر(textArea,
		  XmNbackground, gui.back_pixel,
		  پوچ);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    تهی
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window ریشه, child;
    unsigned صحیح mask;
    unsigned صحیح width, height, border_width, depth;
    صحیح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_مقادیر(shell, XmNmappedWhenManaged, &mappedWhenManaged, پوچ);
    Xt_Va_Set_مقادیر(shell, XmNmappedWhenManaged, False, پوچ);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &ریشه, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &ریشه, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    اگر (x < 0)
	x = 0;
    اگر (x > maxX)
	x = maxX;
    اگر (y < 0)
	y = 0;
    اگر (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_مقادیر(shell, XmNx, x, XmNy, y, پوچ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_مقادیر(shell, XmNmappedWhenManaged, mappedWhenManaged, پوچ);
}

#اگر defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# اگر (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# وگرنه
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(پوچ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    بازگشت font_list;
}

# اگر ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(پوچ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    بازگشت font_list;
}
# endif

#endif

#اگر defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ایستا تهی gui_motif_add_actext(vimmenu_T *menu);
# اگر (XmVersion >= 1002)
ایستا تهی toggle_tearoff(Widget wid);
ایستا تهی gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ایستا تهی submenu_change(vimmenu_T *mp, صحیح colors);

ایستا تهی do_set_mnemonics(صحیح enable);
ایستا صحیح menu_enabled = TRUE;

    تهی
gui_mch_enable_menu(صحیح flag)
{
    اگر (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	اگر (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_مقادیر(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		پوچ);
#  ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
#  endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      پوچ);
	}
	وگرنه
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      پوچ);
	}
    }
    وگرنه
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	اگر (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_مقادیر(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		پوچ);
#  ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
#  endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      پوچ);
	}
	وگرنه
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      پوچ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    تهی
gui_motif_set_mnemonics(صحیح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    اگر (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    ایستا تهی
do_set_mnemonics(صحیح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	اگر (menu->id != (Widget)0)
	    Xt_Va_Set_مقادیر(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    پوچ);
}

    تهی
gui_mch_add_menu(vimmenu_T *menu, صحیح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    اگر (menu_is_popup(menu->name))
    {
	Arg arg[2];
	صحیح n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  اگر (XmVersion <= 1002)
	اگر (mouse_model_popup())
#  endif
	{
	    اگر (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    اگر (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	بازگشت;
    }
# endif

    اگر (!menu_is_menubar(menu->name)
	    || (parent != پوچ && parent->submenu_id == (Widget)0))
	بازگشت;

    label = XmStringCreate((نویسه *)menu->dname, STRING_TAG);
    اگر (label == پوچ)
	بازگشت;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == پوچ) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# اگر (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != پوچ
			   && tearoff_val == (صحیح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    پوچ);
    XmStringFree(label);

    اگر (menu->id == (Widget)0)		// failed
	بازگشت;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    اگر (parent == پوچ && menu->priority >= 9999)
	Xt_Va_Set_مقادیر(menuBar,
		XmNmenuHelpWidget, menu->id,
		پوچ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	پوچ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	پوچ);
    gui_motif_menu_colors(menu->submenu_id);

    اگر (menu->submenu_id == (Widget)0)		// failed
	بازگشت;

# اگر (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_مقادیر(menu->id,
	XmNsubMenuId, menu->submenu_id,
	پوچ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    اگر (parent == پوچ)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ایستا تهی
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    اگر (menu->actext == پوچ || menu->id == (Widget)0)
	بازگشت;

    label = XmStringCreate((نویسه *)menu->actext, STRING_TAG);
    اگر (label == پوچ)
	بازگشت;
    Xt_Va_Set_مقادیر(menu->id, XmNacceleratorText, label, پوچ);
    XmStringFree(label);
}

    تهی
gui_mch_toggle_tearoffs(صحیح enable)
{
# اگر (XmVersion >= 1002)
    اگر (enable)
	tearoff_val = (صحیح)XmTEAR_OFF_ENABLED;
    وگرنه
	tearoff_val = (صحیح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ریشه_menu);
# endif
}

# اگر (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ایستا تهی
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_مقادیر(wid, XmNtearOffModel, tearoff_val, پوچ);
    اگر (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ایستا تهی
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    وقتی (menu != پوچ)
    {
	اگر (!menu_is_popup(menu->name))
	{
	    اگر (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    صحیح
gui_mch_text_area_extra_height(تهی)
{
    Dimension	shadowHeight;

    Xt_Va_Get_مقادیر(textAreaForm, XmNshadowThickness, &shadowHeight, پوچ);
    بازگشت shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    تهی
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ایستا Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    اگر (id != (Widget)0)
	Xt_Va_Get_مقادیر(id, XmNheight, &height, پوچ);

    // Find any menu Widget, to be able to call XtManageChild()
    وگرنه
	FOR_ALL_MENUS(mp)
	    اگر (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		بشکن;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    اگر (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	اگر (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_مقادیر(mp->id, XmNy, &y, پوچ);
	    اگر (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_مقادیر(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	پوچ);

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
    اگر (gui.shell_created)
	Xt_Va_Set_مقادیر(menuBar, XmNheight, gui.menu_height, پوچ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ایستا نویسه **get_toolbar_pixmap(vimmenu_T *menu, نویسه **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ایستا صحیح
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    صحیح		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    ریشه_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (نویسه *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    اگر (status == XpmSuccess)
	بازگشت OK;
    بازگشت FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ایستا نویسه **
get_toolbar_pixmap(vimmenu_T *menu, نویسه **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    نویسه	**xpm = پوچ;		// xpm array
    صحیح		res;

    *fname = پوچ;
    buf[0] = NUL;			// start with NULL path

    اگر (menu->iconfile != پوچ)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	اگر (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	اگر (res == OK)
	{
	    *fname = (نویسه *)vim_strsave(buf);
	    بازگشت tb_blank_xpm;
	}
    }

    اگر (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	اگر (menu->iconidx >= 0 && menu->iconidx
	       < (صحیح)(اندازه(built_in_pixmaps) / اندازه(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	وگرنه
	    xpm = tb_blank_xpm;
    }

    بازگشت xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ایستا صحیح
add_pixmap_args(vimmenu_T *menu, Arg *args, صحیح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    اگر (menu->xpm == پوچ)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    وگرنه
    {
	اگر (menu->xpm_fname != پوچ)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    بازگشت n;
}
# endif // FEAT_TOOLBAR

    تهی
gui_mch_add_menu_item(vimmenu_T *menu, صحیح idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# اگر (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    اگر (menu_is_child_of_popup(menu) && !mouse_model_popup())
	بازگشت;
# endif

# ifdef FEAT_TOOLBAR
    اگر (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = پوچ;    // fallback label if pixmap not found
	صحیح		n;
	Arg		args[18];

	n = 0;
	اگر (menu_is_separator(menu->name))
	{
	    نویسه	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (نویسه *)vim_strchr(menu->name, ':');
	    اگر (cp != پوچ)
		wid = (Dimension)atoi(++cp);
	    وگرنه
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	وگرنه
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
	    xms = XmStringCreate((نویسه *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	اگر (menu->id == پوچ)
	{
	    menu->id = XtCreateManagedWidget((نویسه *)menu->dname,
			type, toolBar, args, n);
	    اگر (menu->id != پوچ && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وگرنه
	    Xt_Set_مقادیر(menu->id, args, n);
	اگر (xms != پوچ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = پوچ;
	// When adding first item to toolbar it might have to be enabled .
	اگر (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != پوچ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	بازگشت;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    اگر (parent->submenu_id == (Widget)0)
	بازگشت;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    اگر (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# اگر (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		پوچ);
	gui_motif_menu_colors(menu->id);
	بازگشت;
    }

    label = XmStringCreate((نویسه *)menu->dname, STRING_TAG);
    اگر (label == پوچ)
	بازگشت;
    menu->id = XtVaCreateWidget("subMenu",
	xm_افزودن_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# اگر (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (صحیح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	پوچ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    اگر (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# اگر (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    تهی
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    صحیح		idx = 0;

    // When GUI hasn't started the menus have not been created.
    اگر (!gui.in_use)
	بازگشت;

    وقتی (menu)
    {
      اگر (menu->children != پوچ)
      {
	  اگر (menu_is_popup(menu->name))
	  {
	      اگر (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      وگرنه
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      وگرنه اگر (menu_is_child_of_popup(menu))
      {
	  اگر (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  وگرنه
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    تهی
gui_mch_new_menu_colors(تهی)
{
    اگر (menuBar == (Widget)0)
	بازگشت;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ریشه_menu, TRUE);
}

    تهی
gui_mch_new_menu_font(تهی)
{
    اگر (menuBar == (Widget)0)
	بازگشت;
    submenu_change(ریشه_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_مقادیر(menuBar, XmNheight, &height, پوچ);
	gui.menu_height = height;

	Xt_Va_Get_مقادیر(vimShell, XtNwidth, &w, XtNheight, &h, پوچ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# اگر defined(FEAT_BEVAL_GUI)
    تهی
gui_mch_new_tooltip_font(تهی)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    اگر (toolBar == (Widget)0)
	بازگشت;

    menu = gui_find_menu((char_u *)"ToolBar");
    اگر (menu != پوچ)
	submenu_change(menu, FALSE);
#  endif
}

    تهی
gui_mch_new_tooltip_colors(تهی)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    اگر (toolBar == (Widget)0)
	بازگشت;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    اگر (toolbar != پوچ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ایستا تهی
submenu_change(
    vimmenu_T	*menu,
    صحیح		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    برای (mp = menu; mp != پوچ; mp = mp->next)
    {
	اگر (mp->id != (Widget)0)
	{
	    اگر (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		اگر (mp->xpm != پوچ)
		{
		    صحیح		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_مقادیر(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		اگر (mp->tip != پوچ)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].مقدار = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].مقدار = gui.tooltip_fg_pixel;
		    Xt_Set_مقادیر(mp->tip->balloonLabel, &args[0], Xt_عدد(args));
		}
#  endif
# endif
	    }
	    وگرنه
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		اگر (mp->tip != پوچ)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].مقدار = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_مقادیر(mp->tip->balloonLabel, &args[0], Xt_عدد(args));
		}
# endif
	    }
	}

	اگر (mp->children != پوچ)
	{
# اگر (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    اگر (mp->submenu_id != (Widget)0)
	    {
		اگر (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		وگرنه
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
    تهی
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
	بازگشت;

    Widget	    parent;

    parent = XtParent(menu->id);
# اگر defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    اگر (parent == toolBar && menu->tip != پوچ)
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
	menu->tip = پوچ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    اگر (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    وگرنه اگر (parent == toolBar)
    {
	Cardinal    رقم_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_مقادیر(toolBar, XmNnumChildren, &رقم_children, پوچ);
	اگر (رقم_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وگرنه
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    تهی
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
    تهی
gui_mch_def_colors(تهی)
{
    اگر (!gui.in_use)
	بازگشت;

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

    تهی
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    اگر (sb->id != (Widget)0)
	Xt_Va_Set_مقادیر(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  پوچ);
}

    تهی
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    صحیح		x,
    صحیح		y,
    صحیح		w,
    صحیح		h)
{
    اگر (sb->id == (Widget)0)
	بازگشت;

    اگر (sb->type == SBAR_چپ || sb->type == SBAR_راست)
    {
	اگر (y == 0)
	    h -= gui.border_offset;
	وگرنه
	    y -= gui.border_offset;
	Xt_Va_Set_مقادیر(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		پوچ);
    }
    وگرنه
	Xt_Va_Set_مقادیر(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_راست]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		پوچ);
    XtManageChild(sb->id);
}

    صحیح
gui_mch_get_scrollbar_xpadding(تهی)
{
    صحیح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_مقادیر(textArea, XtNwidth, &tw, XtNx, &tx, پوچ);
    Xt_Va_Get_مقادیر(vimShell, XtNwidth, &ww, پوچ);
    xpad = ww - tw - tx - gui.scrollbar_width;
    بازگشت (xpad < 0) ? 0 : xpad;
}

    صحیح
gui_mch_get_scrollbar_ypadding(تهی)
{
    صحیح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_مقادیر(textArea, XtNheight, &th, XtNy, &ty, پوچ);
    Xt_Va_Get_مقادیر(vimShell, XtNheight, &wh, پوچ);
    ypad = wh - th - ty - gui.scrollbar_height;
    بازگشت (ypad < 0) ? 0 : ypad;
}

    تهی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    Arg		args[16];
    صحیح		n;

    اگر (sb->id == (Widget)0)
	بازگشت;

    n = 0;
    اگر (flag)
    {
	انتخاب (sb->type)
	{
	    حالت SBAR_چپ:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		بشکن;

	    حالت SBAR_راست:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		بشکن;

	    حالت SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		بشکن;
	}
	Xt_Set_مقادیر(textArea, args, n);
	XtManageChild(sb->id);
    }
    وگرنه
    {
	اگر (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    انتخاب (sb->type)
	    {
		حالت SBAR_چپ:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    بشکن;

		حالت SBAR_راست:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    بشکن;

		حالت SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    بشکن;
	    }
	    Xt_Set_مقادیر(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    تهی
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحیح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    صحیح		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    انتخاب (sb->type)
    {
	حالت SBAR_چپ:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    بشکن;

	حالت SBAR_راست:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    بشکن;

	حالت SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    بشکن;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    اگر (sb->id == (Widget)0)
	بازگشت;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, کلید_Press_Mask, FALSE, gui_x11_کلید_hit_cb,
	    (XtPointer)0);
}

    تهی
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    اگر (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    تهی
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    اگر (sb->id != (Widget)0)
    {
	اگر (gui.scroll_bg_pixel != INVALCOLOR)
	{
#اگر (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_مقادیر(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    پوچ);
	}

	اگر (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_مقادیر(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    پوچ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    اگر (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(تهی)
{
    بازگشت(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ایستا تهی
do_mnemonic(Widget w, unsigned صحیح keycode)
{
    WidgetList	    children;
    صحیح		    رقم_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    نویسه	    mneString[2];
    Widget	    userData;
    unsigned نویسه   rowColType;

    اگر (XtIsComposite(w))
    {
	اگر (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_مقادیر(w, XmNrowColumnType, &rowColType, پوچ);
	    isMenu = (rowColType != (unsigned نویسه)XmWORK_AREA);
	}
	وگرنه
	    isMenu = False;
	اگر (!isMenu)
	{
	    Xt_Va_Get_مقادیر(w, XmNchildren, &children, XmNnumChildren,
			  &رقم_Children, پوچ);
	    برای (i = 0; i < رقم_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وگرنه
    {
	Xt_Va_Get_مقادیر(w, XmNmnemonic, &mnemonic, پوچ);
	اگر (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    اگر (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		اگر (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_مقادیر(w, XmNuserData, &userData, پوچ);
		    اگر (userData != پوچ && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		وگرنه
		{
		    XKeyPressedEvent کلید_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(کلید_Event);
		    کلید_Event.type = کلید_Press;
		    کلید_Event.serial = 1;
		    کلید_Event.send_event = True;
		    کلید_Event.display = XtDisplay(w);
		    کلید_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & کلید_Event,
								     پوچ, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ایستا تهی
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
    ایستا تهی
add_mnemonic_grabs(Widget dialog, Widget w)
{
    نویسه	    mneString[2];
    WidgetList	    children;
    صحیح		    رقم_Children, i;
    Boolean	    isMenu;
    کلید_Sym	    mnemonic = '\0';
    unsigned نویسه   rowColType;

    اگر (XtIsComposite(w))
    {
	اگر (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_مقادیر(w, XmNrowColumnType, &rowColType, پوچ);
	    isMenu = (rowColType != (unsigned نویسه)XmWORK_AREA);
	}
	وگرنه
	    isMenu = False;
	اگر (!isMenu)
	{
	    Xt_Va_Get_مقادیر(w, XmNchildren, &children, XmNnumChildren,
							  &رقم_Children, پوچ);
	    برای (i = 0; i < رقم_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    وگرنه
    {
	Xt_Va_Get_مقادیر(w, XmNmnemonic, &mnemonic, پوچ);
	اگر (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_کلید(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    ایستا تهی
activate_dialog_mnemonics(Widget dialog)
{
    اگر (!dialog)
	بازگشت;

    XtAddEventHandler(dialog, کلید_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) پوچ);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ایستا تهی
suppress_dialog_mnemonics(Widget dialog)
{
    اگر (!dialog)
	بازگشت;

    Xt_Ungrab_کلید(dialog, Any_کلید, Mod1Mask);
    XtRemoveEventHandler(dialog, کلید_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) پوچ);
}

#اگر defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ایستا تهی
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    اگر (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	اگر (fl != پوچ)
	{
	    اگر (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وگرنه
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
	    XmFontListFree(fl);
	}
    }
# وگرنه
    اگر (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	اگر (fl != پوچ)
	{
	    اگر (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وگرنه
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#اگر defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعریفنوع ساختار dialog_callback_arg
{
    نویسه *  args;   // not used right now
    صحیح	    id;
} dcbarg_T;

ایستا Widget dialog_wgt;
ایستا نویسه *browse_fname = پوچ;
ایستا XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ایستا تهی DialogCancelCB(Widget, XtPointer, XtPointer);
ایستا تهی DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ایستا تهی
set_predefined_label(Widget parent, String name, نویسه *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    کلید_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    اگر (!w)
	بازگشت;

    p = vim_strsave((char_u *)new_label);
    اگر (p == پوچ)
	بازگشت;
    برای (next = p; *next; ++next)
    {
	اگر (*next == DLG_HOTKEY_CHAR)
	{
	    صحیح len = STRLEN(next);

	    اگر (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((نویسه *)p, STRING_TAG);
    vim_free(p);

    اگر (str != پوچ)
    {
	Xt_Va_Set_مقادیر(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		پوچ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ایستا تهی
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    اگر (!w)
	بازگشت;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحیح		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = پوچ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", پوچ, 0);

    اگر (initdir == پوچ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    اگر (dflt == پوچ)
	dflt = (char_u *)"";
    وگرنه اگر (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    اگر (filter == پوچ)
	pattern = (char_u *)"";
    وگرنه
    {
	char_u	*s, *p;

	s = filter;
	برای (p = filter; *p != NUL; ++p)
	{
	    اگر (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    اگر (*p == ';' || *p == '\n')	// end of (first) pattern
		بشکن;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	اگر (pattern == پوچ)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_مقادیر(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (نویسه *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (نویسه *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (نویسه *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (نویسه *)title, STRLEN(title) + 1,
	پوچ);

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
    اگر (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_مقادیر(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, پوچ);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned نویسه)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    انجامبده
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_درون_داد_Mask)XtIMAll);
    } وقتی (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    اگر (browse_fname == پوچ)
	بازگشت پوچ;
    بازگشت vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ایستا تهی
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    اگر (browse_fname != پوچ)
    {
	XtFree(browse_fname);
	browse_fname = پوچ;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ایستا تهی
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    اگر (browse_fname != پوچ)
    {
	XtFree(browse_fname);
	browse_fname = پوچ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->مقدار, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#اگر defined(FEAT_GUI_DIALOG)

ایستا صحیح	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ایستا تهی
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    نویسه	buf[2];
    کلید_Sym	کلید_sym;

    اگر (XLookupString(&(event->xkey), buf, 2, &کلید_sym, پوچ) == 1)
    {
	اگر (*buf == CAR)
	    dialogStatus = 1;
	وگرنه اگر (*buf == ESC)
	    dialogStatus = 2;
    }
    اگر ((کلید_sym == XK_چپ || کلید_sym == XK_راست)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    ایستا تهی
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (صحیح)(long)client_data + 1;
}

# ifdef HAVE_XPM

    ایستا Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    نویسه	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحیح			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_نماد	color[5] =
    {
	{"none", پوچ, 0},
	{"iconColor1", پوچ, 0},
	{"bottomShadowColor", پوچ, 0},
	{"topShadowColor", پوچ, 0},
	{"selectColor", پوچ, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_مقادیر(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  پوچ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, ریشه_Window_Of_Screen(scr),
		    data, &pixmap, پوچ, &attr);

    Xt_Va_Get_مقادیر(label, XmNrecomputeSize, &rs, پوچ);
    Xt_Va_Set_مقادیر(label, XmNrecomputeSize, True, پوچ);
    Xt_Va_Set_مقادیر(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    پوچ);
    Xt_Va_Set_مقادیر(label, XmNrecomputeSize, rs, پوچ);

    بازگشت label;
}
# endif

    صحیح
gui_mch_dialog(
    صحیح		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    صحیح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحیح		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    صحیح			butcount;
    Widget		w;
    Widget		dialogform = پوچ;
    Widget		form = پوچ;
    Widget		dialogtextfield = پوچ;
    Widget		*buttons;
    Widget		sep_form = پوچ;
    Boolean		vertical;
    Widget		separator = پوچ;
    صحیح			n;
    Arg			args[6];
# ifdef HAVE_XPM
    نویسه		**icon_data = پوچ;
    Widget		dialogpixmap = پوچ;
# endif

    اگر (title == پوچ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (نویسه *)"dialog", پوچ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != پوچ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((نویسه *)title);
    اگر (label == پوچ)
	بازگشت -1;
    Xt_Va_Set_مقادیر(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    پوچ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    اگر (buts == پوچ)
	بازگشت -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    برای (p = buts; *p; ++p)
	اگر (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    اگر (buttons == پوچ)
    {
	vim_free(buts);
	بازگشت -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    برای (butcount = 0; *p; ++butcount)
    {
	کلید_Sym mnemonic = NUL;

	برای (next = p; *next; ++next)
	{
	    اگر (*next == DLG_HOTKEY_CHAR)
	    {
		صحیح len = STRLEN(next);

		اگر (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    اگر (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		بشکن;
	    }
	}
	label = XmStringCreate(_((نویسه *)p), STRING_TAG);
	اگر (label == پوچ)
	    بشکن;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_افزودن_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		پوچ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	اگر (butcount > 0)
	{
	    اگر (vertical)
		Xt_Va_Set_مقادیر(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			پوچ);
	    وگرنه
	    {
		اگر (*next == NUL)
		{
		    Xt_Va_Set_مقادیر(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    پوچ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    پوچ);
		    XtManageChild(sep_form);
		}
		وگرنه
		{
		    Xt_Va_Set_مقادیر(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    پوچ);
		}
	    }
	}
	وگرنه اگر (!vertical)
	{
	    اگر (*next == NUL)
	    {
		Xt_Va_Set_مقادیر(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			پوچ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			پوچ);
		XtManageChild(sep_form);
	    }
	    وگرنه
		Xt_Va_Set_مقادیر(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			پوچ);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    اگر (butcount > 0)
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

    اگر (textfield != پوچ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		پوچ);
	اگر (butcount > 0)
	    Xt_Va_Set_مقادیر(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    پوچ);
	وگرنه
	    Xt_Va_Set_مقادیر(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    پوچ);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (نویسه *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, کلید_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)پوچ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    پوچ);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    انتخاب (type)
    {
	حالت VIM_GENERIC:
	    icon_data = generic_xpm;
	    بشکن;
	حالت VIM_ERROR:
	    icon_data = error_xpm;
	    بشکن;
	حالت VIM_WARNING:
	    icon_data = alert_xpm;
	    بشکن;
	حالت VIM_INFO:
	    icon_data = info_xpm;
	    بشکن;
	حالت VIM_QUESTION:
	    icon_data = quest_xpm;
	    بشکن;
	پیشفرض:
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
    label = XmStringCreateLtoR((نویسه *)message, STRING_TAG);
    اگر (label == پوچ)
    {
	vim_free(buttons);
	بازگشت -1;
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
# وگرنه
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				پوچ);
    XmStringFree(label);
    set_fontlist(w);

    اگر (textfield != پوچ)
    {
	Xt_Va_Set_مقادیر(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		پوچ);
    }
    وگرنه
    {
	اگر (butcount > 0)
	    Xt_Va_Set_مقادیر(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    پوچ);
	وگرنه
	    Xt_Va_Set_مقادیر(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    پوچ);
    }

    اگر (dfltbutton < 1)
	dfltbutton = 1;
    اگر (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_مقادیر(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], پوچ);
    اگر (textfield != پوچ)
	Xt_Va_Set_مقادیر(dialogform, XmNinitialFocus, dialogtextfield, پوچ);
    وگرنه
	Xt_Va_Set_مقادیر(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									پوچ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    اگر (textfield != پوچ && *textfield != NUL)
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
    برای (;;)
    {
	XtAppProcessEvent(app, (Xt_درون_داد_Mask)XtIMAll);
	اگر (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    بشکن;
    }

    vim_free(buttons);

    اگر (textfield != پوچ)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	اگر (p == پوچ || dialogStatus < 0)
	    *textfield = NUL;
	وگرنه
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((نویسه *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    بازگشت dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#اگر defined(FEAT_TOOLBAR)
    تهی
gui_mch_show_toolbar(صحیح showit)
{
    Cardinal	رقم_Children;	    // how many children toolBar has

    اگر (toolBar == (Widget)0)
	بازگشت;
    Xt_Va_Get_مقادیر(toolBar, XmNnumChildren, &رقم_Children, پوچ);
    اگر (showit && رقم_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_مقادیر(toolBar, XmNchildren, &children, پوچ);
	{
	    تهی    (*action)(BalloonEval *);
	    صحیح	    text = 0;

	    اگر (strstr((ثابت نویسه *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وگرنه
		action = &gui_mch_disable_beval_area;
	    اگر (strstr((ثابت نویسه *)p_toolbar, "text"))
		text = 1;
	    وگرنه اگر (strstr((ثابت نویسه *)p_toolbar, "icons"))
		text = -1;
	    اگر (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    اگر (menu_is_toolbar(toolbar->dname))
			بشکن;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		اگر (toolbar)
		{
		    برای (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			صحیح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			اگر (cur->tip != پوچ)
			    (*action)(cur->tip);
			اگر (!menu_is_separator(cur->name))
			{
			    اگر (text == 1 || cur->xpm == پوچ)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    اگر (cur->id != پوچ)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_مقادیر(cur->id, args, n);
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
	اگر (showing_tabline)
	{
	    Xt_Va_Set_مقادیر(tab_سطر,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  پوچ);
	    Xt_Va_Set_مقادیر(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  پوچ);
	}
	وگرنه
# endif
	    Xt_Va_Set_مقادیر(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  پوچ);
	اگر (XtIsManaged(menuBar))
	    Xt_Va_Set_مقادیر(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    پوچ);
	وگرنه
	    Xt_Va_Set_مقادیر(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    پوچ);
    }
    وگرنه
    {
	gui.toolbar_height = 0;
	اگر (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      پوچ);
	}
	وگرنه
	{
# ifdef FEAT_GUI_TABLINE
	    اگر (showing_tabline)
	    {
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      پوچ);
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      پوچ);
	    }
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      پوچ);
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
    ایستا تهی
reset_focus(تهی)
{
    اگر (textArea != پوچ)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    صحیح
gui_mch_compute_toolbar_height(تهی)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	رقم_Children;	    // how many children toolBar has
    صحیح		i;

    borders = 0;
    height = 0;
    اگر (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_مقادیر(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		پوچ);
	borders += fst + fmh;
	Xt_Va_Get_مقادیر(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &رقم_Children, پوچ);
	borders += tst + tmh;
	برای (i = 0; i < (صحیح)رقم_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_مقادیر(children[i], XmNheight, &whgt, پوچ);
	    اگر (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    اگر (height < 10)
	height = 24;
# endif

    بازگشت (صحیح)(height + (borders << 1));
}

    تهی
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
	    پوچ);
}
#endif

#اگر defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    تهی
gui_mch_show_tabline(صحیح showit)
{
    اگر (tab_سطر == (Widget)0)
	بازگشت;

    اگر (!showit != !showing_tabline)
    {
	اگر (showit)
	{
	    XtManageChild(tab_سطر);
	    XtUnmanageChild(XtNameToWidget(tab_سطر, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_سطر, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_سطر,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    اگر (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), پوچ);
	    وگرنه
#  endif
		اگر (XtIsManaged(menuBar))
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, پوچ);
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM, پوچ);
	    Xt_Va_Set_مقادیر(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  پوچ);
	}
	وگرنه
	{
	    XtUnmanageChild(tab_سطر);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    اگر (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), پوچ);
	    وگرنه
#  endif
		اگر (XtIsManaged(menuBar))
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, پوچ);
	    وگرنه
# endif
		Xt_Va_Set_مقادیر(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, پوچ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(تهی)
{
    بازگشت tab_سطر != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    تهی
gui_mch_update_tabline(تهی)
{
    tabpage_T		*tp;
    صحیح			nr = 1, n;
    Arg			args[10];
    صحیح			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    صحیح			last_page, tab_شمار;
    XmString		label_str;
    نویسه		*label_cstr;
    BalloonEval		*beval;

    اگر (tab_سطر == (Widget)0)
	بازگشت;

    // Add a label for each tab page.  They all contain the same text area.
    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_سطر, nr, &page_info);
	اگر (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_افزودن_Button(tab_سطر, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, پوچ, tabline_balloon_cb,
									پوچ);
	    Xt_Va_Set_مقادیر(tab, XmNuserData, beval, پوچ);
	}
	وگرنه
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_مقادیر(tab, Xm_Npage_عدد, nr, پوچ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_مقادیر(tab, XmNlabelString, &label_str, پوچ);
	اگر (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    اگر (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_مقادیر(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, پوچ);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_شمار = nr - 1;

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Nlast_Page_عدد, &last_page, پوچ);

    // Remove any old labels.
    وقتی (nr <= last_page)
    {
	اگر (XmNotebookGetPageInfo(tab_سطر, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_عدد == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_مقادیر(page_info.major_tab_widget, XmNuserData, &beval, پوچ);
	    اگر (beval != پوچ)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_مقادیر(tab_سطر, Xm_Nlast_Page_عدد, tab_شمار, پوچ);

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Ncurrent_Page_عدد, &currentpage, پوچ);
    اگر (currentpage != curtabidx)
	Xt_Va_Set_مقادیر(tab_سطر, Xm_Ncurrent_Page_عدد, curtabidx, پوچ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    تهی
gui_mch_set_curtab(صحیح nr)
{
    صحیح		currentpage;

    اگر (tab_سطر == (Widget)0)
	بازگشت;

    Xt_Va_Get_مقادیر(tab_سطر, Xm_Ncurrent_Page_عدد, &currentpage, پوچ);
    اگر (currentpage != nr)
	Xt_Va_Set_مقادیر(tab_سطر, Xm_Ncurrent_Page_عدد, nr, پوچ);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ایستا تهی
gui_motif_menu_colors(Widget id)
{
    اگر (gui.menu_bg_pixel != INVALCOLOR)
#اگر (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#وگرنه
	Xt_Va_Set_مقادیر(id, XmNbackground, gui.menu_bg_pixel, پوچ);
#endif
    اگر (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_مقادیر(id, XmNforeground, gui.menu_fg_pixel, پوچ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ایستا تهی
gui_motif_scroll_colors(Widget id)
{
    اگر (gui.scroll_bg_pixel != INVALCOLOR)
#اگر (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#وگرنه
	Xt_Va_Set_مقادیر(id, XmNbackground, gui.scroll_bg_pixel, پوچ);
#endif
    اگر (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_مقادیر(id, XmNforeground, gui.scroll_fg_pixel, پوچ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    تهی
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    اگر (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	اگر (fl != پوچ)
	{
	    اگر (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وگرنه
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
	    XmFontListFree(fl);
	}
    }
# وگرنه
    اگر (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	اگر (fl != پوچ)
	{
	    اگر (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وگرنه
		Xt_Va_Set_مقادیر(id, XmNfontList, fl, پوچ);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

تعریفنوع ساختار _SharedFindReplace
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

ایستا SharedFindReplace find_widgets = {پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ};
ایستا SharedFindReplace repl_widgets = {پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ, پوچ};

    ایستا تهی
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    اگر (cd != پوچ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ایستا تهی
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    اگر (cd != پوچ)
	XtUnmanageChild(cd->dialog);
}

    ایستا تهی
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    ایستا تهی
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    نویسه	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    اگر (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	بازگشت;
    }

    // Get the search/replace strings from the dialog
    اگر (flags == FRD_FINDNEXT)
    {
	repl_text = پوچ;
	sfr = &find_widgets;
    }
    وگرنه
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_مقادیر(sfr->down, XmNset, &direction_down, پوچ);
    Xt_Va_Get_مقادیر(sfr->wword, XmNset, &wword, پوچ);
    Xt_Va_Get_مقادیر(sfr->mcase, XmNset, &mcase, پوچ);
    اگر (wword)
	flags |= FRD_WHOLE_WORD;
    اگر (mcase)
	flags |= FRD_MATCH_CASE;

    (تهی)gui_do_findrepl((صحیح)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    اگر (find_text != پوچ)
	XtFree(find_text);
    اگر (repl_text != پوچ)
	XtFree(repl_text);
}

    ایستا تهی
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    کلید_Sym keysym;

    اگر (frdp == پوچ)
	بازگشت;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    اگر (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    ایستا تهی
set_label(Widget w, نویسه *label)
{
    XmString	str;
    char_u	*p, *next;
    کلید_Sym	mnemonic = NUL;

    اگر (!w)
	بازگشت;

    p = vim_strsave((char_u *)label);
    اگر (p == پوچ)
	بازگشت;
    برای (next = p; *next; ++next)
    {
	اگر (*next == DLG_HOTKEY_CHAR)
	{
	    صحیح len = STRLEN(next);

	    اگر (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((نویسه *)p);
    vim_free(p);
    اگر (str)
    {
	Xt_Va_Set_مقادیر(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		پوچ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ایستا تهی
find_replace_dialog_create(char_u *arg, صحیح do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		درون_داد_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    صحیح			n;
    Arg			args[6];
    صحیح			wword = FALSE;
    صحیح			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    اگر (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	اگر (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	وگرنه
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	اگر (entry_text != پوچ)
	    XmTextFieldSetString(frdp->what, (نویسه *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_مقادیر(frdp->wword, XmNset, wword, پوچ);
	بازگشت;
    }

    // Create a fresh new dialog window
    اگر (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وگرنه
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
	    پوچ);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_افزودن_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    پوچ);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    اگر (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		پوچ);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		پوچ);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_افزودن_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		پوچ);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_افزودن_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    پوچ);
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

    درون_داد_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    پوچ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, درون_داد_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		پوچ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, درون_داد_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		پوچ);

	اگر (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	درون_داد_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    پوچ);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, درون_داد_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    پوچ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, کلید_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	وگرنه
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, کلید_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_مقادیر(label_what, args, n);
	widest = width;
	اگر (do_replace)
	{
	    Xt_Get_مقادیر(label_with, args, n);
	    اگر (width > widest)
		widest = width;
	}

	Xt_Va_Set_مقادیر(frdp->what, XmNleftOffset, widest, پوچ);
	اگر (do_replace)
	    Xt_Va_Set_مقادیر(frdp->with, XmNleftOffset, widest, پوچ);

    }

    XtManageChild(درون_داد_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, درون_داد_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, درون_داد_form,
		پوچ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		پوچ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)پوچ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		پوچ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		پوچ);
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
	    XmNtopWidget, درون_داد_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    پوچ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    پوچ);
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
	    پوچ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    اگر (entry_text != پوچ)
	XmTextFieldSetString(frdp->what, (نویسه *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   تهی
gui_mch_find_dialog(exarg_T *eap)
{
    اگر (!gui.in_use)
	بازگشت;

    find_replace_dialog_create(eap->arg, FALSE);
}


    تهی
gui_mch_replace_dialog(exarg_T *eap)
{
    اگر (!gui.in_use)
	بازگشت;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    تهی
gui_motif_synch_fonts(تهی)
{
    SharedFindReplace *frdp;
    صحیح		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    اگر (font == پوچ)
	بازگشت;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    برای (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	اگر (frdp->dialog)
	{
	    Xt_Va_Set_مقادیر(frdp->what, XmNfontList, font_list, پوچ);
	    اگر (do_replace)
		Xt_Va_Set_مقادیر(frdp->with, XmNfontList, font_list, پوچ);
	}
    }

    XmFontListFree(font_list);
}
