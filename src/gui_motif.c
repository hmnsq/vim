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
#إذا (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#ضمن <Xm/Frame.h>
#ضمن <Xm/LabelG.h>
#ضمن <Xm/ToggleBG.h>
#ضمن <Xm/SeparatoG.h>
#ضمن <Xm/XmP.h>

#ضمن <X11/keysym.h>
#ضمن <X11/Xatom.h>
#ضمن <X11/StringDefs.h>
#ضمن <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# إذا defined(VMS)
#  include <xpm.h>
# وإلا
#  include <X11/xpm.h>
# endif
#وإلا
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#ضمن "gui_xmebw.h"	// for our Enhanced Button Widget

#إذا defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

خارجي Widget vimShell;

ساكن Widget vimForm;
ساكن Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
ساكن Widget toolBarFrame;
ساكن Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
ساكن Widget	tab_سطر;
ساكن Widget	tab_سطر_menu = 0;
ساكن صحيح	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# إذا (XmVersion >= 1002)
// remember the last set value for the tearoff item
ساكن صحيح tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
# endif
ساكن Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
ساكن فراغ reset_focus(فراغ);
#endif

ساكن فراغ gui_motif_menu_colors(Widget id);
ساكن فراغ gui_motif_scroll_colors(Widget id);

#إذا (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#وإلا
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    ساكن فراغ
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	قيمة;
    صحيح		dragging;

    sb = gui_find_scrollbar((long)client_data);

    قيمة = ((XmScrollBarCallbackStruct *)call_data)->قيمة;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (صحيح)XmCR_DRAG);
    gui_drag_scrollbar(sb, قيمة, dragging);
}

#ifdef FEAT_GUI_TABLINE
    ساكن فراغ
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    إذا (nptr->reason != (صحيح)XmCR_NONE)
	send_tabline_event(nptr->page_رقم);
}

    ساكن فراغ
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_قيم(w, XmNuserData, &cmd, عدم);
    Xt_Va_Get_قيم(tab_سطر_menu, XmNuserData, &tab_idx, عدم);

    send_tabline_menu_event((صحيح)(long)tab_idx, (صحيح)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ساكن فراغ
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((صحيح *)timed_out) = TRUE;
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
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_سطر, scroller_name);
    إذا (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_قيم(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, عدم);
	إذا (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    إذا (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		أرجع TRUE;
	}
    }
    أرجع FALSE;
}

    ساكن فراغ
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    صحيح				tab_idx = 0;
    WidgetList			children;
    Cardinal			رقم_Children;
    ساكن XtIntervalId		timer = (XtIntervalId)0;
    ساكن صحيح			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    إذا (event->button == Button1)
    {
	إذا (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    أرجع;

	إذا (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

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
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	أرجع;
    }

    إذا (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_قيم(tab_سطر_menu, XmNuserData, &tab_idx, عدم);
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
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	إذا (tab_w != (Widget)0 && Xm_Is_ادفع_Button(tab_w))
	    Xt_Va_Get_قيم(tab_w, Xm_Npage_رقم, &tab_idx, عدم);
    }

    Xt_Va_Set_قيم(tab_سطر_menu, XmNuserData, (XtPointer)(long)tab_idx, عدم);
    Xt_Va_Get_قيم(tab_سطر_menu, XmNchildren, &children, XmNnumChildren,
		  &رقم_Children, عدم);
    XtManageChildren(children, رقم_Children);
    XmMenuPosition(tab_سطر_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_سطر_menu);
}

    ساكن فراغ
tabline_balloon_cb(BalloonEval *beval, صحيح state UNUSED)
{
    صحيح		nr;
    tabpage_T	*tp;

    إذا (beval->target == (Widget)0)
	أرجع;

    Xt_Va_Get_قيم(beval->target, Xm_Npage_رقم, &nr, عدم);
    tp = find_tabpage(nr);
    إذا (tp == عدم)
	أرجع;

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

#ضمن <Xm/XmP.h>
#ضمن <Xm/LabelP.h>

ساكن XtExposeProc old_label_expose = عدم;

    ساكن فراغ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned حرف   label_type = (صحيح)XmSTRING;

    Xt_Va_Get_قيم(_w, XmNlabelType, &label_type, (XtPointer)0);

    إذا (XtIsSensitive(_w) || label_type != (صحيح)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    وإلا
    {
	XGCValues   قيم;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	قيم.graphics_exposures = False;

	إذا (fs != 0)
	{
	    mask |= GCFont;
	    قيم.font = fs->fid;
	}

	إذا (lw->primitive.قمة_shadow_pixmap != None
		&& lw->primitive.قمة_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    قيم.fill_style = FillTiled;
	    قيم.tile = lw->primitive.قمة_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	إذا (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	قيم.foreground = lw->primitive.قمة_shadow_color;
	قيم.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &قيم, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	إذا (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	قيم.foreground = lw->primitive.bottom_shadow_color;
	قيم.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &قيم, dynamic, (XtGCMask)0);
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
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		وسائط[10];
    صحيح		n;
    XmString	xms;
#endif

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
    gui.border_offset = gui.border_width;

    /*
     * Install the tearOffModel resource converter.
     */
#إذا (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_قيم(vimShell, XmNdeleteResponse, XmDO_NOTHING, عدم);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	عدم);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	صحيح ac = 0;

# إذا (XmVersion >= 1002)
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
	عدم);
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
	عدم);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(وسائط[n], XmNbindingType, XmNONE); n++;
    XtSetArg(وسائط[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(وسائط[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(وسائط[n], Xm_Nback_Page_رقم, 0); n++;
    XtSetArg(وسائط[n], XmNbackPagePlacement, Xm_قمة_يمين); n++;
    XtSetArg(وسائط[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(وسائط[n], XmNshadowThickness, 0); n++;
    XtSetArg(وسائط[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(وسائط[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_سطر = XmCreateNotebook(vimForm, "Vim tabline", وسائط, n);

    XtAddCallback(tab_سطر, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			عدم);
    XtAddEventHandler(tab_سطر, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, عدم);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_سطر, "MinorTabScrollerNext");
    Xt_Va_Set_قيم(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);
    scroller = XtNameToWidget(tab_سطر, "MinorTabScrollerPrevious");
    Xt_Va_Set_قيم(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, عدم);

    // Create the tabline popup menu
    tab_سطر_menu = XmCreatePopupMenu(tab_سطر, "tabline popup", عدم, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(وسائط[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((حرف *)"Close tab", STRING_TAG);
    XtSetArg(وسائط[n], XmNlabelString, xms); n++;
    button = Xm_Create_ادفع_Button(tab_سطر_menu, "Close", وسائط, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    XtSetArg(وسائط[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((حرف *)"New Tab", STRING_TAG);
    XtSetArg(وسائط[n], XmNlabelString, xms); n++;
    button = Xm_Create_ادفع_Button(tab_سطر_menu, "New Tab", وسائط, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
    XmStringFree(xms);

    n = 0;
    XtSetArg(وسائط[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((حرف *)"Open tab...", STRING_TAG);
    XtSetArg(وسائط[n], XmNlabelString, xms); n++;
    button = Xm_Create_ادفع_Button(tab_سطر_menu, "Open tab...", وسائط, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, عدم);
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
	عدم);
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
	عدم);

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
    فراغ
gui_x11_destroy_widgets(فراغ)
{
    textArea = عدم;
#ifdef FEAT_MENU
    menuBar = عدم;
#endif
}

    فراغ
gui_mch_set_text_area_pos(
    صحيح	    x UNUSED,
    صحيح	    y UNUSED,
    صحيح	    w UNUSED,
    صحيح	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    فراغ
gui_x11_set_back_color(فراغ)
{
    إذا (textArea != عدم)
#إذا (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#وإلا
	Xt_Va_Set_قيم(textArea,
		  XmNbackground, gui.back_pixel,
		  عدم);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    فراغ
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window جذر, child;
    unsigned صحيح mask;
    unsigned صحيح width, height, border_width, depth;
    صحيح x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_قيم(shell, XmNmappedWhenManaged, &mappedWhenManaged, عدم);
    Xt_Va_Set_قيم(shell, XmNmappedWhenManaged, False, عدم);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &جذر, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &جذر, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    إذا (x < 0)
	x = 0;
    إذا (x > maxX)
	x = maxX;
    إذا (y < 0)
	y = 0;
    إذا (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_قيم(shell, XmNx, x, XmNy, y, عدم);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_قيم(shell, XmNmappedWhenManaged, mappedWhenManaged, عدم);
}

#إذا defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# إذا (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# وإلا
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(عدم, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    أرجع font_list;
}

# إذا ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(عدم, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    أرجع font_list;
}
# endif

#endif

#إذا defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ساكن فراغ gui_motif_add_actext(vimmenu_T *menu);
# إذا (XmVersion >= 1002)
ساكن فراغ toggle_tearoff(Widget wid);
ساكن فراغ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ساكن فراغ submenu_change(vimmenu_T *mp, صحيح colors);

ساكن فراغ do_set_mnemonics(صحيح enable);
ساكن صحيح menu_enabled = TRUE;

    فراغ
gui_mch_enable_menu(صحيح flag)
{
    إذا (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_قيم(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
    }
    وإلا
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	إذا (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_قيم(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		عدم);
#  ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
#  endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      عدم);
	}
	وإلا
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    فراغ
gui_motif_set_mnemonics(صحيح enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    إذا (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    ساكن فراغ
do_set_mnemonics(صحيح enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	إذا (menu->id != (Widget)0)
	    Xt_Va_Set_قيم(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    عدم);
}

    فراغ
gui_mch_add_menu(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    إذا (menu_is_popup(menu->name))
    {
	Arg arg[2];
	صحيح n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  إذا (XmVersion <= 1002)
	إذا (mouse_model_popup())
#  endif
	{
	    إذا (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    إذا (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	أرجع;
    }
# endif

    إذا (!menu_is_menubar(menu->name)
	    || (parent != عدم && parent->submenu_id == (Widget)0))
	أرجع;

    label = XmStringCreate((حرف *)menu->dname, STRING_TAG);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == عدم) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# إذا (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != عدم
			   && tearoff_val == (صحيح)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    عدم);
    XmStringFree(label);

    إذا (menu->id == (Widget)0)		// failed
	أرجع;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    إذا (parent == عدم && menu->priority >= 9999)
	Xt_Va_Set_قيم(menuBar,
		XmNmenuHelpWidget, menu->id,
		عدم);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	عدم);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	عدم);
    gui_motif_menu_colors(menu->submenu_id);

    إذا (menu->submenu_id == (Widget)0)		// failed
	أرجع;

# إذا (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_قيم(menu->id,
	XmNsubMenuId, menu->submenu_id,
	عدم);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    إذا (parent == عدم)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ساكن فراغ
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    إذا (menu->actext == عدم || menu->id == (Widget)0)
	أرجع;

    label = XmStringCreate((حرف *)menu->actext, STRING_TAG);
    إذا (label == عدم)
	أرجع;
    Xt_Va_Set_قيم(menu->id, XmNacceleratorText, label, عدم);
    XmStringFree(label);
}

    فراغ
gui_mch_toggle_tearoffs(صحيح enable)
{
# إذا (XmVersion >= 1002)
    إذا (enable)
	tearoff_val = (صحيح)XmTEAR_OFF_ENABLED;
    وإلا
	tearoff_val = (صحيح)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(جذر_menu);
# endif
}

# إذا (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ساكن فراغ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_قيم(wid, XmNtearOffModel, tearoff_val, عدم);
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
	menu = menu->next;
    }
}
# endif

    صحيح
gui_mch_text_area_extra_height(فراغ)
{
    Dimension	shadowHeight;

    Xt_Va_Get_قيم(textAreaForm, XmNshadowThickness, &shadowHeight, عدم);
    أرجع shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    فراغ
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ساكن Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    إذا (id != (Widget)0)
	Xt_Va_Get_قيم(id, XmNheight, &height, عدم);

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
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	إذا (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_قيم(mp->id, XmNy, &y, عدم);
	    إذا (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_قيم(menuBar,
	XmNmarginHeight, &margin,
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
    gui.menu_height = maxy + height - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    إذا (gui.shell_created)
	Xt_Va_Set_قيم(menuBar, XmNheight, gui.menu_height, عدم);
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
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    صحيح		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    جذر_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (حرف *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    إذا (status == XpmSuccess)
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
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	إذا (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	إذا (res == OK)
	{
	    *fname = (حرف *)vim_strsave(buf);
	    أرجع tb_blank_xpm;
	}
    }

    إذا (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	إذا (menu->iconidx >= 0 && menu->iconidx
	       < (صحيح)(حجم(built_in_pixmaps) / حجم(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	وإلا
	    xpm = tb_blank_xpm;
    }

    أرجع xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ساكن صحيح
add_pixmap_وسائط(vimmenu_T *menu, Arg *وسائط, صحيح n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    إذا (menu->xpm == عدم)
    {
	XtSetArg(وسائط[n], XmNlabelType, XmSTRING); n++;
    }
    وإلا
    {
	إذا (menu->xpm_fname != عدم)
	{
	    XtSetArg(وسائط[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(وسائط[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(وسائط[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    أرجع n;
}
# endif // FEAT_TOOLBAR

    فراغ
gui_mch_add_menu_item(vimmenu_T *menu, صحيح idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# إذا (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    إذا (menu_is_child_of_popup(menu) && !mouse_model_popup())
	أرجع;
# endif

# ifdef FEAT_TOOLBAR
    إذا (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = عدم;    // fallback label if pixmap not found
	صحيح		n;
	Arg		وسائط[18];

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
	    XtSetArg(وسائط[n], XmNwidth, wid); n++;
	    XtSetArg(وسائط[n], XmNminWidth, wid); n++;
	    XtSetArg(وسائط[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(وسائط[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	وإلا
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    XtSetArg(وسائط[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(وسائط[n], XmNhighlightOnEnter, True); n++;
	    XtSetArg(وسائط[n], XmNmarginWidth, 0); n++;
	    XtSetArg(وسائط[n], XmNmarginHeight, 0); n++;
	    XtSetArg(وسائط[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((حرف *)menu->dname, STRING_TAG);
	    XtSetArg(وسائط[n], XmNlabelString, xms); n++;

	    n = add_pixmap_وسائط(menu, وسائط, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(وسائط[n], XmNpositionIndex, idx); n++;
	إذا (menu->id == عدم)
	{
	    menu->id = XtCreateManagedWidget((حرف *)menu->dname,
			type, toolBar, وسائط, n);
	    إذا (menu->id != عدم && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	وإلا
	    Xt_Set_قيم(menu->id, وسائط, n);
	إذا (xms != عدم)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = عدم;
	// When adding first item to toolbar it might have to be enabled .
	إذا (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != عدم)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	أرجع;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    إذا (parent->submenu_id == (Widget)0)
	أرجع;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    إذا (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# إذا (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		عدم);
	gui_motif_menu_colors(menu->id);
	أرجع;
    }

    label = XmStringCreate((حرف *)menu->dname, STRING_TAG);
    إذا (label == عدم)
	أرجع;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ادفع_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# إذا (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (صحيح)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	عدم);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    إذا (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# إذا (XmVersion <= 1002)
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
    إذا (!gui.in_use)
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
		  gui_mch_add_menu(menu, idx);
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
      وإلا إذا (menu_is_child_of_popup(menu))
      {
	  إذا (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  وإلا
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
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
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(جذر_menu, TRUE);
}

    فراغ
gui_mch_new_menu_font(فراغ)
{
    إذا (menuBar == (Widget)0)
	أرجع;
    submenu_change(جذر_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_قيم(menuBar, XmNheight, &height, عدم);
	gui.menu_height = height;

	Xt_Va_Get_قيم(vimShell, XtNwidth, &w, XtNheight, &h, عدم);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
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

    menu = gui_find_menu((char_u *)"ToolBar");
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

    toolbar = gui_find_menu((char_u *)"ToolBar");
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

    لكل (mp = menu; mp != عدم; mp = mp->next)
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
		    Arg		وسائط[18];

		    n = add_pixmap_وسائط(mp, وسائط, n);
		    Xt_Set_قيم(mp->id, وسائط, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		إذا (mp->tip != عدم)
		{
		    Arg وسائط[2];

		    وسائط[0].name = XmNbackground;
		    وسائط[0].قيمة = gui.tooltip_bg_pixel;
		    وسائط[1].name = XmNforeground;
		    وسائط[1].قيمة = gui.tooltip_fg_pixel;
		    Xt_Set_قيم(mp->tip->balloonLabel, &وسائط[0], Xt_رقم(وسائط));
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
		    Arg وسائط[1];

		    وسائط[0].name = XmNfontList;
		    وسائط[0].قيمة = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_قيم(mp->tip->balloonLabel, &وسائط[0], Xt_رقم(وسائط));
		}
# endif
	    }
	}

	إذا (mp->children != عدم)
	{
# إذا (XmVersion >= 1002)
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

    Widget	    parent;

    parent = XtParent(menu->id);
# إذا defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    إذا (parent == toolBar && menu->tip != عدم)
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
    إذا (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    وإلا إذا (parent == toolBar)
    {
	Cardinal    رقم_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_قيم(toolBar, XmNnumChildren, &رقم_children, عدم);
	إذا (رقم_children == 0)
	    gui_mch_show_toolbar(FALSE);
	وإلا
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    فراغ
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
    فراغ
gui_mch_def_colors(فراغ)
{
    إذا (!gui.in_use)
	أرجع;

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

    فراغ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    إذا (sb->id != (Widget)0)
	Xt_Va_Set_قيم(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  عدم);
}

    فراغ
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    صحيح		x,
    صحيح		y,
    صحيح		w,
    صحيح		h)
{
    إذا (sb->id == (Widget)0)
	أرجع;

    إذا (sb->type == SBAR_يسار || sb->type == SBAR_يمين)
    {
	إذا (y == 0)
	    h -= gui.border_offset;
	وإلا
	    y -= gui.border_offset;
	Xt_Va_Set_قيم(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		عدم);
    }
    وإلا
	Xt_Va_Set_قيم(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_يمين]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		عدم);
    XtManageChild(sb->id);
}

    صحيح
gui_mch_get_scrollbar_xpadding(فراغ)
{
    صحيح xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_قيم(textArea, XtNwidth, &tw, XtNx, &tx, عدم);
    Xt_Va_Get_قيم(vimShell, XtNwidth, &ww, عدم);
    xpad = ww - tw - tx - gui.scrollbar_width;
    أرجع (xpad < 0) ? 0 : xpad;
}

    صحيح
gui_mch_get_scrollbar_ypadding(فراغ)
{
    صحيح ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_قيم(textArea, XtNheight, &th, XtNy, &ty, عدم);
    Xt_Va_Get_قيم(vimShell, XtNheight, &wh, عدم);
    ypad = wh - th - ty - gui.scrollbar_height;
    أرجع (ypad < 0) ? 0 : ypad;
}

    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    Arg		وسائط[16];
    صحيح		n;

    إذا (sb->id == (Widget)0)
	أرجع;

    n = 0;
    إذا (flag)
    {
	بدل (sb->type)
	{
	    حالة SBAR_يسار:
		XtSetArg(وسائط[n], XmNleftOffset, gui.scrollbar_width); n++;
		اكسر;

	    حالة SBAR_يمين:
		XtSetArg(وسائط[n], XmNrightOffset, gui.scrollbar_width); n++;
		اكسر;

	    حالة SBAR_BOTTOM:
		XtSetArg(وسائط[n], XmNbottomOffset, gui.scrollbar_height);n++;
		اكسر;
	}
	Xt_Set_قيم(textArea, وسائط, n);
	XtManageChild(sb->id);
    }
    وإلا
    {
	إذا (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    بدل (sb->type)
	    {
		حالة SBAR_يسار:
		    XtSetArg(وسائط[n], XmNleftOffset, 0); n++;
		    اكسر;

		حالة SBAR_يمين:
		    XtSetArg(وسائط[n], XmNrightOffset, 0); n++;
		    اكسر;

		حالة SBAR_BOTTOM:
		    XtSetArg(وسائط[n], XmNbottomOffset, 0);n++;
		    اكسر;
	    }
	    Xt_Set_قيم(textArea, وسائط, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    فراغ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    صحيح		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		وسائط[16];
    صحيح		n = 0;

    XtSetArg(وسائط[n], XmNminimum, 0); n++;
    XtSetArg(وسائط[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    بدل (sb->type)
    {
	حالة SBAR_يسار:
	    XtSetArg(وسائط[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(وسائط[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNwidth, gui.scrollbar_width); n++;
	    اكسر;

	حالة SBAR_يمين:
	    XtSetArg(وسائط[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(وسائط[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNwidth, gui.scrollbar_width); n++;
	    اكسر;

	حالة SBAR_BOTTOM:
	    XtSetArg(وسائط[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(وسائط[n], XmNheight, gui.scrollbar_height); n++;
	    اكسر;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, وسائط, n);
    إذا (sb->id == (Widget)0)
	أرجع;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, مفتاح_Press_Mask, FALSE, gui_x11_مفتاح_hit_cb,
	    (XtPointer)0);
}

    فراغ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    فراغ
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    إذا (sb->id != (Widget)0)
    {
	إذا (gui.scroll_bg_pixel != INVALCOLOR)
	{
#إذا (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_قيم(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    عدم);
	}

	إذا (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_قيم(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    عدم);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    إذا (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(فراغ)
{
    أرجع(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ساكن فراغ
do_mnemonic(Widget w, unsigned صحيح keycode)
{
    WidgetList	    children;
    صحيح		    رقم_Children, i;
    Boolean	    isMenu;
    مفتاح_Sym	    mnemonic = '\0';
    حرف	    mneString[2];
    Widget	    userData;
    unsigned حرف   rowColType;

    إذا (XtIsComposite(w))
    {
	إذا (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_قيم(w, XmNrowColumnType, &rowColType, عدم);
	    isMenu = (rowColType != (unsigned حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_قيم(w, XmNchildren, &children, XmNnumChildren,
			  &رقم_Children, عدم);
	    لكل (i = 0; i < رقم_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    وإلا
    {
	Xt_Va_Get_قيم(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    إذا (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		إذا (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_قيم(w, XmNuserData, &userData, عدم);
		    إذا (userData != عدم && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		وإلا
		{
		    XKeyPressedEvent مفتاح_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(مفتاح_Event);
		    مفتاح_Event.type = مفتاح_Press;
		    مفتاح_Event.serial = 1;
		    مفتاح_Event.send_event = True;
		    مفتاح_Event.display = XtDisplay(w);
		    مفتاح_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & مفتاح_Event,
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
    ساكن فراغ
add_mnemonic_grabs(Widget dialog, Widget w)
{
    حرف	    mneString[2];
    WidgetList	    children;
    صحيح		    رقم_Children, i;
    Boolean	    isMenu;
    مفتاح_Sym	    mnemonic = '\0';
    unsigned حرف   rowColType;

    إذا (XtIsComposite(w))
    {
	إذا (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_قيم(w, XmNrowColumnType, &rowColType, عدم);
	    isMenu = (rowColType != (unsigned حرف)XmWORK_AREA);
	}
	وإلا
	    isMenu = False;
	إذا (!isMenu)
	{
	    Xt_Va_Get_قيم(w, XmNchildren, &children, XmNnumChildren,
							  &رقم_Children, عدم);
	    لكل (i = 0; i < رقم_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    وإلا
    {
	Xt_Va_Get_قيم(w, XmNmnemonic, &mnemonic, عدم);
	إذا (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_مفتاح(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    ساكن فراغ
activate_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    XtAddEventHandler(dialog, مفتاح_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) عدم);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ساكن فراغ
suppress_dialog_mnemonics(Widget dialog)
{
    إذا (!dialog)
	أرجع;

    Xt_Ungrab_مفتاح(dialog, Any_مفتاح, Mod1Mask);
    XtRemoveEventHandler(dialog, مفتاح_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) عدم);
}

#إذا defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ساكن فراغ
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    إذا (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وإلا
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
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
		XtUnmanageChild(id);
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وإلا
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#إذا defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

تعريفنوع بنية dialog_callback_arg
{
    حرف *  وسائط;   // not used right now
    صحيح	    id;
} dcbarg_T;

ساكن Widget dialog_wgt;
ساكن حرف *browse_fname = عدم;
ساكن XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ساكن فراغ DialogCancelCB(Widget, XtPointer, XtPointer);
ساكن فراغ DialogAcceptCB(Widget, XtPointer, XtPointer);

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
set_predefined_label(Widget parent, String name, حرف *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    مفتاح_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)new_label);
    إذا (p == عدم)
	أرجع;
    لكل (next = p; *next; ++next)
    {
	إذا (*next == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(next);

	    إذا (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((حرف *)p, STRING_TAG);
    vim_free(p);

    إذا (str != عدم)
    {
	Xt_Va_Set_قيم(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    إذا (!w)
	أرجع;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    صحيح		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
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
	add_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    إذا (filter == عدم)
	pattern = (char_u *)"";
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
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	إذا (pattern == عدم)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_قيم(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (حرف *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (حرف *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (حرف *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (حرف *)title, STRLEN(title) + 1,
	عدم);

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
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_قيم(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, عدم);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned حرف)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    افعل
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_مدخل_Mask)XtIMAll);
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
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ساكن فراغ
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    إذا (browse_fname != عدم)
    {
	XtFree(browse_fname);
	browse_fname = عدم;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->قيمة, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#إذا defined(FEAT_GUI_DIALOG)

ساكن صحيح	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ساكن فراغ
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    حرف	buf[2];
    مفتاح_Sym	مفتاح_sym;

    إذا (XLookupString(&(event->xkey), buf, 2, &مفتاح_sym, عدم) == 1)
    {
	إذا (*buf == CAR)
	    dialogStatus = 1;
	وإلا إذا (*buf == ESC)
	    dialogStatus = 2;
    }
    إذا ((مفتاح_sym == XK_يسار || مفتاح_sym == XK_يمين)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    ساكن فراغ
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (صحيح)(long)client_data + 1;
}

# ifdef HAVE_XPM

    ساكن Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    حرف	**data,
    ArgList	وسائط,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    صحيح			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_رمز	color[5] =
    {
	{"none", عدم, 0},
	{"iconColor1", عدم, 0},
	{"bottomShadowColor", عدم, 0},
	{"topShadowColor", عدم, 0},
	{"selectColor", عدم, 0}
    };

    label = XmCreateLabelGadget(parent, name, وسائط, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_قيم(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  عدم);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, جذر_Window_Of_Screen(scr),
		    data, &pixmap, عدم, &attr);

    Xt_Va_Get_قيم(label, XmNrecomputeSize, &rs, عدم);
    Xt_Va_Set_قيم(label, XmNrecomputeSize, True, عدم);
    Xt_Va_Set_قيم(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    عدم);
    Xt_Va_Set_قيم(label, XmNrecomputeSize, rs, عدم);

    أرجع label;
}
# endif

    صحيح
gui_mch_dialog(
    صحيح		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    صحيح		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    صحيح		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
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
    Arg			وسائط[6];
# ifdef HAVE_XPM
    حرف		**icon_data = عدم;
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
    Xt_Va_Set_قيم(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    عدم);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
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
	مفتاح_Sym mnemonic = NUL;

	لكل (next = p; *next; ++next)
	{
	    إذا (*next == DLG_HOTKEY_CHAR)
	    {
		صحيح len = STRLEN(next);

		إذا (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    إذا (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		اكسر;
	    }
	}
	label = XmStringCreate(_((حرف *)p), STRING_TAG);
	إذا (label == عدم)
	    اكسر;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ادفع_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		عدم);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	إذا (butcount > 0)
	{
	    إذا (vertical)
		Xt_Va_Set_قيم(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			عدم);
	    وإلا
	    {
		إذا (*next == NUL)
		{
		    Xt_Va_Set_قيم(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    عدم);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    عدم);
		    XtManageChild(sep_form);
		}
		وإلا
		{
		    Xt_Va_Set_قيم(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    عدم);
		}
	    }
	}
	وإلا إذا (!vertical)
	{
	    إذا (*next == NUL)
	    {
		Xt_Va_Set_قيم(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			عدم);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			عدم);
		XtManageChild(sep_form);
	    }
	    وإلا
		Xt_Va_Set_قيم(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			عدم);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    إذا (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	XtSetArg(وسائط[n], XmNorientation, XmHORIZONTAL); n++;
	XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	XtSetArg(وسائط[n], XmNbottomWidget, buttons[0]); n++;
	XtSetArg(وسائط[n], XmNbottomOffset, 4); n++;
	XtSetArg(وسائط[n], XmNleftAttachment, XmATTACH_FORM); n++;
	XtSetArg(وسائط[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", وسائط, n);
	XtManageChild(separator);
    }

    إذا (textfield != عدم)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	إذا (butcount > 0)
	    Xt_Va_Set_قيم(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_Set_قيم(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (حرف *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, مفتاح_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)عدم);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    عدم);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    بدل (type)
    {
	حالة VIM_GENERIC:
	    icon_data = generic_xpm;
	    اكسر;
	حالة VIM_ERROR:
	    icon_data = error_xpm;
	    اكسر;
	حالة VIM_WARNING:
	    icon_data = alert_xpm;
	    اكسر;
	حالة VIM_INFO:
	    icon_data = info_xpm;
	    اكسر;
	حالة VIM_QUESTION:
	    icon_data = quest_xpm;
	    اكسر;
	افتراضي:
	    icon_data = generic_xpm;
    }

    n = 0;
    XtSetArg(وسائط[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(وسائط[n], XmNtopOffset, 8); n++;
    XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    XtSetArg(وسائط[n], XmNbottomOffset, 8); n++;
    XtSetArg(وسائط[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(وسائط[n], XmNleftOffset, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_data, وسائط, n);
    XtManageChild(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((حرف *)message, STRING_TAG);
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
				XmNtopOffset, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# وإلا
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				عدم);
    XmStringFree(label);
    set_fontlist(w);

    إذا (textfield != عدم)
    {
	Xt_Va_Set_قيم(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		عدم);
    }
    وإلا
    {
	إذا (butcount > 0)
	    Xt_Va_Set_قيم(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    عدم);
	وإلا
	    Xt_Va_Set_قيم(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
    }

    إذا (dfltbutton < 1)
	dfltbutton = 1;
    إذا (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_قيم(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], عدم);
    إذا (textfield != عدم)
	Xt_Va_Set_قيم(dialogform, XmNinitialFocus, dialogtextfield, عدم);
    وإلا
	Xt_Va_Set_قيم(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									عدم);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    إذا (textfield != عدم && *textfield != NUL)
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
    لكل (;;)
    {
	XtAppProcessEvent(app, (Xt_مدخل_Mask)XtIMAll);
	إذا (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    اكسر;
    }

    vim_free(buttons);

    إذا (textfield != عدم)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	إذا (p == عدم || dialogStatus < 0)
	    *textfield = NUL;
	وإلا
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((حرف *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    أرجع dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    Cardinal	رقم_Children;	    // how many children toolBar has

    إذا (toolBar == (Widget)0)
	أرجع;
    Xt_Va_Get_قيم(toolBar, XmNnumChildren, &رقم_Children, عدم);
    إذا (showit && رقم_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_قيم(toolBar, XmNchildren, &children, عدم);
	{
	    فراغ    (*action)(BalloonEval *);
	    صحيح	    text = 0;

	    إذا (strstr((ثابت حرف *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    وإلا
		action = &gui_mch_disable_beval_area;
	    إذا (strstr((ثابت حرف *)p_toolbar, "text"))
		text = 1;
	    وإلا إذا (strstr((ثابت حرف *)p_toolbar, "icons"))
		text = -1;
	    إذا (text != 0)
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
		    لكل (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    وسائط[1];
			صحيح	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			إذا (cur->tip != عدم)
			    (*action)(cur->tip);
			إذا (!menu_is_separator(cur->name))
			{
			    إذا (text == 1 || cur->xpm == عدم)
			    {
				XtSetArg(وسائط[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    إذا (cur->id != عدم)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_قيم(cur->id, وسائط, n);
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
	إذا (showing_tabline)
	{
	    Xt_Va_Set_قيم(tab_سطر,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  عدم);
	    Xt_Va_Set_قيم(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
# endif
	    Xt_Va_Set_قيم(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  عدم);
	إذا (XtIsManaged(menuBar))
	    Xt_Va_Set_قيم(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    عدم);
	وإلا
	    Xt_Va_Set_قيم(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    عدم);
    }
    وإلا
    {
	gui.toolbar_height = 0;
	إذا (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      عدم);
	}
	وإلا
	{
# ifdef FEAT_GUI_TABLINE
	    إذا (showing_tabline)
	    {
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_سطر,
			      عدم);
	    }
	    وإلا
# endif
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      عدم);
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
    ساكن فراغ
reset_focus(فراغ)
{
    إذا (textArea != عدم)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    صحيح
gui_mch_compute_toolbar_height(فراغ)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	رقم_Children;	    // how many children toolBar has
    صحيح		i;

    borders = 0;
    height = 0;
    إذا (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_قيم(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		عدم);
	borders += fst + fmh;
	Xt_Va_Get_قيم(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &رقم_Children, عدم);
	borders += tst + tmh;
	لكل (i = 0; i < (صحيح)رقم_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_قيم(children[i], XmNheight, &whgt, عدم);
	    إذا (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    إذا (height < 10)
	height = 24;
# endif

    أرجع (صحيح)(height + (borders << 1));
}

    فراغ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_قيم(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    عدم);
}
#endif

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
	    XtManageChild(tab_سطر);
	    XtUnmanageChild(XtNameToWidget(tab_سطر, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_سطر, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_سطر,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_Set_قيم(tab_سطر,
			      XmNtopAttachment, XmATTACH_FORM, عدم);
	    Xt_Va_Set_قيم(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_سطر,
			  عدم);
	}
	وإلا
	{
	    XtUnmanageChild(tab_سطر);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    إذا (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), عدم);
	    وإلا
#  endif
		إذا (XtIsManaged(menuBar))
		Xt_Va_Set_قيم(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, عدم);
	    وإلا
# endif
		Xt_Va_Set_قيم(textAreaForm,
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
    Arg			وسائط[10];
    صحيح			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    صحيح			last_page, tab_عدد;
    XmString		label_str;
    حرف		*label_cstr;
    BalloonEval		*beval;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = first_tabpage; tp != عدم; tp = tp->tp_next, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_سطر, nr, &page_info);
	إذا (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(وسائط[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(وسائط[n], XmNtraversalOn, False); n++;
	    XtSetArg(وسائط[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(وسائط[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(وسائط[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ادفع_Button(tab_سطر, "-Empty-", وسائط, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, عدم, tabline_balloon_cb,
									عدم);
	    Xt_Va_Set_قيم(tab, XmNuserData, beval, عدم);
	}
	وإلا
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_قيم(tab, Xm_Npage_رقم, nr, عدم);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_قيم(tab, XmNlabelString, &label_str, عدم);
	إذا (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    إذا (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_قيم(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, عدم);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_عدد = nr - 1;

    Xt_Va_Get_قيم(tab_سطر, Xm_Nlast_Page_رقم, &last_page, عدم);

    // Remove any old labels.
    طالما (nr <= last_page)
    {
	إذا (XmNotebookGetPageInfo(tab_سطر, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_رقم == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_قيم(page_info.major_tab_widget, XmNuserData, &beval, عدم);
	    إذا (beval != عدم)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_قيم(tab_سطر, Xm_Nlast_Page_رقم, tab_عدد, عدم);

    Xt_Va_Get_قيم(tab_سطر, Xm_Ncurrent_Page_رقم, &currentpage, عدم);
    إذا (currentpage != curtabidx)
	Xt_Va_Set_قيم(tab_سطر, Xm_Ncurrent_Page_رقم, curtabidx, عدم);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_set_curtab(صحيح nr)
{
    صحيح		currentpage;

    إذا (tab_سطر == (Widget)0)
	أرجع;

    Xt_Va_Get_قيم(tab_سطر, Xm_Ncurrent_Page_رقم, &currentpage, عدم);
    إذا (currentpage != nr)
	Xt_Va_Set_قيم(tab_سطر, Xm_Ncurrent_Page_رقم, nr, عدم);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ساكن فراغ
gui_motif_menu_colors(Widget id)
{
    إذا (gui.menu_bg_pixel != INVALCOLOR)
#إذا (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#وإلا
	Xt_Va_Set_قيم(id, XmNbackground, gui.menu_bg_pixel, عدم);
#endif
    إذا (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_قيم(id, XmNforeground, gui.menu_fg_pixel, عدم);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ساكن فراغ
gui_motif_scroll_colors(Widget id)
{
    إذا (gui.scroll_bg_pixel != INVALCOLOR)
#إذا (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#وإلا
	Xt_Va_Set_قيم(id, XmNbackground, gui.scroll_bg_pixel, عدم);
#endif
    إذا (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_قيم(id, XmNforeground, gui.scroll_fg_pixel, عدم);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    فراغ
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    إذا (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	إذا (fl != عدم)
	{
	    إذا (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وإلا
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
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
		XtUnmanageChild(id);
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    وإلا
		Xt_Va_Set_قيم(id, XmNfontList, fl, عدم);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

تعريفنوع بنية _SharedFindReplace
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

ساكن SharedFindReplace find_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};
ساكن SharedFindReplace repl_widgets = {عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم, عدم};

    ساكن فراغ
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    إذا (cd != عدم)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ساكن فراغ
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    إذا (cd != عدم)
	XtUnmanageChild(cd->dialog);
}

    ساكن فراغ
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    ساكن فراغ
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    حرف	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    إذا (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	أرجع;
    }

    // Get the search/replace strings from the dialog
    إذا (flags == FRD_FINDNEXT)
    {
	repl_text = عدم;
	sfr = &find_widgets;
    }
    وإلا
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_قيم(sfr->down, XmNset, &direction_down, عدم);
    Xt_Va_Get_قيم(sfr->wword, XmNset, &wword, عدم);
    Xt_Va_Get_قيم(sfr->mcase, XmNset, &mcase, عدم);
    إذا (wword)
	flags |= FRD_WHOLE_WORD;
    إذا (mcase)
	flags |= FRD_MATCH_CASE;

    (فراغ)gui_do_findrepl((صحيح)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    إذا (find_text != عدم)
	XtFree(find_text);
    إذا (repl_text != عدم)
	XtFree(repl_text);
}

    ساكن فراغ
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    مفتاح_Sym keysym;

    إذا (frdp == عدم)
	أرجع;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    إذا (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    ساكن فراغ
set_label(Widget w, حرف *label)
{
    XmString	str;
    char_u	*p, *next;
    مفتاح_Sym	mnemonic = NUL;

    إذا (!w)
	أرجع;

    p = vim_strsave((char_u *)label);
    إذا (p == عدم)
	أرجع;
    لكل (next = p; *next; ++next)
    {
	إذا (*next == DLG_HOTKEY_CHAR)
	{
	    صحيح len = STRLEN(next);

	    إذا (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((حرف *)p);
    vim_free(p);
    إذا (str)
    {
	Xt_Va_Set_قيم(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		عدم);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ساكن فراغ
find_replace_dialog_create(char_u *arg, صحيح do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		مدخل_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    صحيح			n;
    Arg			وسائط[6];
    صحيح			wword = FALSE;
    صحيح			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    إذا (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	إذا (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	وإلا
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	إذا (entry_text != عدم)
	    XmTextFieldSetString(frdp->what, (حرف *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_قيم(frdp->wword, XmNset, wword, عدم);
	أرجع;
    }

    // Create a fresh new dialog window
    إذا (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    وإلا
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    XtSetArg(وسائط[n], XmNautoUnmanage, False); n++;
    XtSetArg(وسائط[n], XmNnoResize, True); n++;
    XtSetArg(وسائط[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", وسائط, n);
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
	    عدم);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_ادفع_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    عدم);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    إذا (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ادفع_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ادفع_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ادفع_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		عدم);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ادفع_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    عدم);
    set_label(frdp->cancel, _("&Cancel"));
    XtAddCallback(frdp->cancel, XmNactivateCallback,
	    find_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    XtManageChild(button_form);

    n = 0;
    XtSetArg(وسائط[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(وسائط[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    XtSetArg(وسائط[n], XmNrightWidget, button_form); n++;
    XtSetArg(وسائط[n], XmNrightOffset, 4); n++;
    XtSetArg(وسائط[n], XmNtopAttachment, XmATTACH_FORM); n++;
    XtSetArg(وسائط[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", وسائط, n);
    XtManageChild(separator);

    مدخل_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    عدم);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, مدخل_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, مدخل_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		عدم);

	إذا (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	مدخل_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, مدخل_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    عدم);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, مفتاح_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	وإلا
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, مفتاح_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(وسائط[n], XmNwidth, &width); n++;
	Xt_Get_قيم(label_what, وسائط, n);
	widest = width;
	إذا (do_replace)
	{
	    Xt_Get_قيم(label_with, وسائط, n);
	    إذا (width > widest)
		widest = width;
	}

	Xt_Va_Set_قيم(frdp->what, XmNleftOffset, widest, عدم);
	إذا (do_replace)
	    Xt_Va_Set_قيم(frdp->with, XmNleftOffset, widest, عدم);

    }

    XtManageChild(مدخل_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, مدخل_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, مدخل_form,
		عدم);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)عدم, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		عدم);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		عدم);
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
	    XmNtopWidget, مدخل_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    عدم);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    عدم);
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
	    عدم);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    إذا (entry_text != عدم)
	XmTextFieldSetString(frdp->what, (حرف *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   فراغ
gui_mch_find_dialog(exarg_T *eap)
{
    إذا (!gui.in_use)
	أرجع;

    find_replace_dialog_create(eap->arg, FALSE);
}


    فراغ
gui_mch_replace_dialog(exarg_T *eap)
{
    إذا (!gui.in_use)
	أرجع;

    find_replace_dialog_create(eap->arg, TRUE);
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
    SharedFindReplace *frdp;
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
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	إذا (frdp->dialog)
	{
	    Xt_Va_Set_قيم(frdp->what, XmNfontList, font_list, عدم);
	    إذا (do_replace)
		Xt_Va_Set_قيم(frdp->with, XmNfontList, font_list, عدم);
	}
    }

    XmFontListFree(font_list);
}
