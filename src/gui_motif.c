/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#अन्तर्भाव "vim.h"

#अन्तर्भाव <Xm/Form.h>
#अन्तर्भाव <Xm/RowColumn.h>
#अन्तर्भाव <Xm/PushB.h>
#अन्तर्भाव <Xm/Text.h>
#अन्तर्भाव <Xm/TextF.h>
#अन्तर्भाव <Xm/Separator.h>
#अन्तर्भाव <Xm/Label.h>
#अन्तर्भाव <Xm/CascadeB.h>
#अन्तर्भाव <Xm/ScrollBar.h>
#अन्तर्भाव <Xm/MenuShell.h>
#अन्तर्भाव <Xm/DrawingA.h>
#यदि (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#अन्तर्भाव <Xm/Frame.h>
#अन्तर्भाव <Xm/LabelG.h>
#अन्तर्भाव <Xm/ToggleBG.h>
#अन्तर्भाव <Xm/SeparatoG.h>
#अन्तर्भाव <Xm/XmP.h>

#अन्तर्भाव <X11/keysym.h>
#अन्तर्भाव <X11/Xatom.h>
#अन्तर्भाव <X11/StringDefs.h>
#अन्तर्भाव <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# यदि defined(VMS)
#  include <xpm.h>
# अन्यथा
#  include <X11/xpm.h>
# endif
#अन्यथा
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#अन्तर्भाव "gui_xmebw.h"	// for our Enhanced Button Widget

#यदि defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

बाह्य Widget vimShell;

स्थिर Widget vimForm;
स्थिर Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
स्थिर Widget toolBarFrame;
स्थिर Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
स्थिर Widget	tab_ओळ;
स्थिर Widget	tab_ओळ_menu = 0;
स्थिर पूर्णाङ्क	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# यदि (XmVersion >= 1002)
// remember the last set value for the tearoff item
स्थिर पूर्णाङ्क tearoff_val = (पूर्णाङ्क)XmTEAR_OFF_ENABLED;
# endif
स्थिर Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
स्थिर शून्य reset_focus(शून्य);
#endif

स्थिर शून्य gui_motif_menu_colors(Widget id);
स्थिर शून्य gui_motif_scroll_colors(Widget id);

#यदि (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#अन्यथा
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    स्थिर शून्य
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	मूल्य;
    पूर्णाङ्क		dragging;

    sb = gui_find_scrollbar((long)client_data);

    मूल्य = ((XmScrollBarCallbackStruct *)call_data)->मूल्य;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (पूर्णाङ्क)XmCR_DRAG);
    gui_drag_scrollbar(sb, मूल्य, dragging);
}

#ifdef FEAT_GUI_TABLINE
    स्थिर शून्य
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    यदि (nptr->reason != (पूर्णाङ्क)XmCR_NONE)
	send_tabline_event(nptr->page_संख्या);
}

    स्थिर शून्य
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_मूल्ये(w, XmNuserData, &cmd, रिक्त);
    Xt_Va_Get_मूल्ये(tab_ओळ_menu, XmNuserData, &tab_idx, रिक्त);

    send_tabline_menu_event((पूर्णाङ्क)(long)tab_idx, (पूर्णाङ्क)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    स्थिर शून्य
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((पूर्णाङ्क *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    स्थिर पूर्णाङ्क
tabline_scroller_clicked(
    वर्ण		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_ओळ, scroller_name);
    यदि (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_मूल्ये(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, रिक्त);
	यदि (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    यदि (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		प्रत्यागम TRUE;
	}
    }
    प्रत्यागम FALSE;
}

    स्थिर शून्य
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    पूर्णाङ्क				tab_idx = 0;
    WidgetList			children;
    Cardinal			अंक_Children;
    स्थिर XtIntervalId		timer = (XtIntervalId)0;
    स्थिर पूर्णाङ्क			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    यदि (event->button == Button1)
    {
	यदि (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    प्रत्यागम;

	यदि (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	अन्यथा
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	प्रत्यागम;
    }

    यदि (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_मूल्ये(tab_ओळ_menu, XmNuserData, &tab_idx, रिक्त);
	send_tabline_menu_event(tab_idx, (पूर्णाङ्क)TABLINE_MENU_CLOSE);
	प्रत्यागम;
    }

    यदि (event->button != Button3)
	प्रत्यागम;

    // When ignoring events don't show the menu.
    यदि (hold_gui_events || cmdwin_type != 0)
	प्रत्यागम;

    यदि (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	यदि (tab_w != (Widget)0 && Xm_Is_ढकला_Button(tab_w))
	    Xt_Va_Get_मूल्ये(tab_w, Xm_Npage_संख्या, &tab_idx, रिक्त);
    }

    Xt_Va_Set_मूल्ये(tab_ओळ_menu, XmNuserData, (XtPointer)(long)tab_idx, रिक्त);
    Xt_Va_Get_मूल्ये(tab_ओळ_menu, XmNchildren, &children, XmNnumChildren,
		  &अंक_Children, रिक्त);
    XtManageChildren(children, अंक_Children);
    XmMenuPosition(tab_ओळ_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_ओळ_menu);
}

    स्थिर शून्य
tabline_balloon_cb(BalloonEval *beval, पूर्णाङ्क state UNUSED)
{
    पूर्णाङ्क		nr;
    tabpage_T	*tp;

    यदि (beval->target == (Widget)0)
	प्रत्यागम;

    Xt_Va_Get_मूल्ये(beval->target, Xm_Npage_संख्या, &nr, रिक्त);
    tp = find_tabpage(nr);
    यदि (tp == रिक्त)
	प्रत्यागम;

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

#अन्तर्भाव <Xm/XmP.h>
#अन्तर्भाव <Xm/LabelP.h>

स्थिर XtExposeProc old_label_expose = रिक्त;

    स्थिर शून्य
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned वर्ण   label_type = (पूर्णाङ्क)XmSTRING;

    Xt_Va_Get_मूल्ये(_w, XmNlabelType, &label_type, (XtPointer)0);

    यदि (XtIsSensitive(_w) || label_type != (पूर्णाङ्क)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    अन्यथा
    {
	XGCValues   मूल्ये;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	मूल्ये.graphics_exposures = False;

	यदि (fs != 0)
	{
	    mask |= GCFont;
	    मूल्ये.font = fs->fid;
	}

	यदि (lw->primitive.शीर्ष_shadow_pixmap != None
		&& lw->primitive.शीर्ष_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    मूल्ये.fill_style = FillTiled;
	    मूल्ये.tile = lw->primitive.शीर्ष_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	यदि (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	मूल्ये.foreground = lw->primitive.शीर्ष_shadow_color;
	मूल्ये.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &मूल्ये, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	यदि (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	मूल्ये.foreground = lw->primitive.bottom_shadow_color;
	मूल्ये.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &मूल्ये, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    शून्य
gui_x11_create_widgets(शून्य)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    पूर्णाङ्क		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    यदि (old_label_expose == रिक्त)
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
#यदि (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_मूल्ये(vimShell, XmNdeleteResponse, XmDO_NOTHING, रिक्त);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	रिक्त);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	पूर्णाङ्क ac = 0;

# यदि (XmVersion >= 1002)
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
	रिक्त);
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
	रिक्त);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_संख्या, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_शीर्ष_उजवा); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_ओळ = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_ओळ, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			रिक्त);
    XtAddEventHandler(tab_ओळ, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, रिक्त);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_ओळ, "MinorTabScrollerNext");
    Xt_Va_Set_मूल्ये(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, रिक्त);
    scroller = XtNameToWidget(tab_ओळ, "MinorTabScrollerPrevious");
    Xt_Va_Set_मूल्ये(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, रिक्त);

    // Create the tabline popup menu
    tab_ओळ_menu = XmCreatePopupMenu(tab_ओळ, "tabline popup", रिक्त, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((वर्ण *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ढकला_Button(tab_ओळ_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, रिक्त);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((वर्ण *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ढकला_Button(tab_ओळ_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, रिक्त);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((वर्ण *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ढकला_Button(tab_ओळ_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, रिक्त);
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
	रिक्त);
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
	रिक्त);

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
    शून्य
gui_x11_destroy_widgets(शून्य)
{
    textArea = रिक्त;
#ifdef FEAT_MENU
    menuBar = रिक्त;
#endif
}

    शून्य
gui_mch_set_text_area_pos(
    पूर्णाङ्क	    x UNUSED,
    पूर्णाङ्क	    y UNUSED,
    पूर्णाङ्क	    w UNUSED,
    पूर्णाङ्क	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    शून्य
gui_x11_set_back_color(शून्य)
{
    यदि (textArea != रिक्त)
#यदि (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#अन्यथा
	Xt_Va_Set_मूल्ये(textArea,
		  XmNbackground, gui.back_pixel,
		  रिक्त);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    शून्य
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window मूळ, child;
    unsigned पूर्णाङ्क mask;
    unsigned पूर्णाङ्क width, height, border_width, depth;
    पूर्णाङ्क x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_मूल्ये(shell, XmNmappedWhenManaged, &mappedWhenManaged, रिक्त);
    Xt_Va_Set_मूल्ये(shell, XmNmappedWhenManaged, False, रिक्त);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &मूळ, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &मूळ, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    यदि (x < 0)
	x = 0;
    यदि (x > maxX)
	x = maxX;
    यदि (y < 0)
	y = 0;
    यदि (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_मूल्ये(shell, XmNx, x, XmNy, y, रिक्त);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_मूल्ये(shell, XmNmappedWhenManaged, mappedWhenManaged, रिक्त);
}

#यदि defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# यदि (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# अन्यथा
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(रिक्त, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    प्रत्यागम font_list;
}

# यदि ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(रिक्त, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    प्रत्यागम font_list;
}
# endif

#endif

#यदि defined(FEAT_MENU)
/*
 * Menu stuff.
 */

स्थिर शून्य gui_motif_add_actext(vimmenu_T *menu);
# यदि (XmVersion >= 1002)
स्थिर शून्य toggle_tearoff(Widget wid);
स्थिर शून्य gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
स्थिर शून्य submenu_change(vimmenu_T *mp, पूर्णाङ्क colors);

स्थिर शून्य do_set_mnemonics(पूर्णाङ्क enable);
स्थिर पूर्णाङ्क menu_enabled = TRUE;

    शून्य
gui_mch_enable_menu(पूर्णाङ्क flag)
{
    यदि (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	यदि (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_मूल्ये(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		रिक्त);
#  ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
#  endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      रिक्त);
	}
	अन्यथा
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      रिक्त);
	}
    }
    अन्यथा
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	यदि (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_मूल्ये(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		रिक्त);
#  ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
#  endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      रिक्त);
	}
	अन्यथा
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_FORM,
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      रिक्त);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    शून्य
gui_motif_set_mnemonics(पूर्णाङ्क enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    यदि (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    स्थिर शून्य
do_set_mnemonics(पूर्णाङ्क enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	यदि (menu->id != (Widget)0)
	    Xt_Va_Set_मूल्ये(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    रिक्त);
}

    शून्य
gui_mch_add_menu(vimmenu_T *menu, पूर्णाङ्क idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    यदि (menu_is_popup(menu->name))
    {
	Arg arg[2];
	पूर्णाङ्क n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  यदि (XmVersion <= 1002)
	यदि (mouse_model_popup())
#  endif
	{
	    यदि (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    यदि (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	प्रत्यागम;
    }
# endif

    यदि (!menu_is_menubar(menu->name)
	    || (parent != रिक्त && parent->submenu_id == (Widget)0))
	प्रत्यागम;

    label = XmStringCreate((वर्ण *)menu->dname, STRING_TAG);
    यदि (label == रिक्त)
	प्रत्यागम;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == रिक्त) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# यदि (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != रिक्त
			   && tearoff_val == (पूर्णाङ्क)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    रिक्त);
    XmStringFree(label);

    यदि (menu->id == (Widget)0)		// failed
	प्रत्यागम;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    यदि (parent == रिक्त && menu->priority >= 9999)
	Xt_Va_Set_मूल्ये(menuBar,
		XmNmenuHelpWidget, menu->id,
		रिक्त);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	रिक्त);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	रिक्त);
    gui_motif_menu_colors(menu->submenu_id);

    यदि (menu->submenu_id == (Widget)0)		// failed
	प्रत्यागम;

# यदि (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_मूल्ये(menu->id,
	XmNsubMenuId, menu->submenu_id,
	रिक्त);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    यदि (parent == रिक्त)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    स्थिर शून्य
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    यदि (menu->actext == रिक्त || menu->id == (Widget)0)
	प्रत्यागम;

    label = XmStringCreate((वर्ण *)menu->actext, STRING_TAG);
    यदि (label == रिक्त)
	प्रत्यागम;
    Xt_Va_Set_मूल्ये(menu->id, XmNacceleratorText, label, रिक्त);
    XmStringFree(label);
}

    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable)
{
# यदि (XmVersion >= 1002)
    यदि (enable)
	tearoff_val = (पूर्णाङ्क)XmTEAR_OFF_ENABLED;
    अन्यथा
	tearoff_val = (पूर्णाङ्क)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(मूळ_menu);
# endif
}

# यदि (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    स्थिर शून्य
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_मूल्ये(wid, XmNtearOffModel, tearoff_val, रिक्त);
    यदि (tearoff_val == (पूर्णाङ्क)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    स्थिर शून्य
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    यावत् (menu != रिक्त)
    {
	यदि (!menu_is_popup(menu->name))
	{
	    यदि (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    पूर्णाङ्क
gui_mch_text_area_extra_height(शून्य)
{
    Dimension	shadowHeight;

    Xt_Va_Get_मूल्ये(textAreaForm, XmNshadowThickness, &shadowHeight, रिक्त);
    प्रत्यागम shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    शून्य
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    स्थिर Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    यदि (id != (Widget)0)
	Xt_Va_Get_मूल्ये(id, XmNheight, &height, रिक्त);

    // Find any menu Widget, to be able to call XtManageChild()
    अन्यथा
	FOR_ALL_MENUS(mp)
	    यदि (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		भङ्ग;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    यदि (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	यदि (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_मूल्ये(mp->id, XmNy, &y, रिक्त);
	    यदि (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_मूल्ये(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	रिक्त);

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
    यदि (gui.shell_created)
	Xt_Va_Set_मूल्ये(menuBar, XmNheight, gui.menu_height, रिक्त);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

स्थिर वर्ण **get_toolbar_pixmap(vimmenu_T *menu, वर्ण **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    पूर्णाङ्क		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    मूळ_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (वर्ण *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    यदि (status == XpmSuccess)
	प्रत्यागम OK;
    प्रत्यागम FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    स्थिर वर्ण **
get_toolbar_pixmap(vimmenu_T *menu, वर्ण **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    वर्ण	**xpm = रिक्त;		// xpm array
    पूर्णाङ्क		res;

    *fname = रिक्त;
    buf[0] = NUL;			// start with NULL path

    यदि (menu->iconfile != रिक्त)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	यदि (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	यदि (res == OK)
	{
	    *fname = (वर्ण *)vim_strsave(buf);
	    प्रत्यागम tb_blank_xpm;
	}
    }

    यदि (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	यदि (menu->iconidx >= 0 && menu->iconidx
	       < (पूर्णाङ्क)(परिमाण(built_in_pixmaps) / परिमाण(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	अन्यथा
	    xpm = tb_blank_xpm;
    }

    प्रत्यागम xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    स्थिर पूर्णाङ्क
add_pixmap_args(vimmenu_T *menu, Arg *args, पूर्णाङ्क n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    यदि (menu->xpm == रिक्त)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    अन्यथा
    {
	यदि (menu->xpm_fname != रिक्त)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    प्रत्यागम n;
}
# endif // FEAT_TOOLBAR

    शून्य
gui_mch_add_menu_item(vimmenu_T *menu, पूर्णाङ्क idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# यदि (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    यदि (menu_is_child_of_popup(menu) && !mouse_model_popup())
	प्रत्यागम;
# endif

# ifdef FEAT_TOOLBAR
    यदि (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = रिक्त;    // fallback label if pixmap not found
	पूर्णाङ्क		n;
	Arg		args[18];

	n = 0;
	यदि (menu_is_separator(menu->name))
	{
	    वर्ण	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (वर्ण *)vim_strchr(menu->name, ':');
	    यदि (cp != रिक्त)
		wid = (Dimension)atoi(++cp);
	    अन्यथा
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	अन्यथा
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
	    xms = XmStringCreate((वर्ण *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	यदि (menu->id == रिक्त)
	{
	    menu->id = XtCreateManagedWidget((वर्ण *)menu->dname,
			type, toolBar, args, n);
	    यदि (menu->id != रिक्त && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	अन्यथा
	    Xt_Set_मूल्ये(menu->id, args, n);
	यदि (xms != रिक्त)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = रिक्त;
	// When adding first item to toolbar it might have to be enabled .
	यदि (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != रिक्त)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	प्रत्यागम;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    यदि (parent->submenu_id == (Widget)0)
	प्रत्यागम;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    यदि (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# यदि (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (पूर्णाङ्क)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		रिक्त);
	gui_motif_menu_colors(menu->id);
	प्रत्यागम;
    }

    label = XmStringCreate((वर्ण *)menu->dname, STRING_TAG);
    यदि (label == रिक्त)
	प्रत्यागम;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ढकला_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# यदि (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (पूर्णाङ्क)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	रिक्त);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    यदि (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# यदि (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    शून्य
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    पूर्णाङ्क		idx = 0;

    // When GUI hasn't started the menus have not been created.
    यदि (!gui.in_use)
	प्रत्यागम;

    यावत् (menu)
    {
      यदि (menu->children != रिक्त)
      {
	  यदि (menu_is_popup(menu->name))
	  {
	      यदि (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      अन्यथा
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      अन्यथा यदि (menu_is_child_of_popup(menu))
      {
	  यदि (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  अन्यथा
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    शून्य
gui_mch_new_menu_colors(शून्य)
{
    यदि (menuBar == (Widget)0)
	प्रत्यागम;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(मूळ_menu, TRUE);
}

    शून्य
gui_mch_new_menu_font(शून्य)
{
    यदि (menuBar == (Widget)0)
	प्रत्यागम;
    submenu_change(मूळ_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_मूल्ये(menuBar, XmNheight, &height, रिक्त);
	gui.menu_height = height;

	Xt_Va_Get_मूल्ये(vimShell, XtNwidth, &w, XtNheight, &h, रिक्त);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# यदि defined(FEAT_BEVAL_GUI)
    शून्य
gui_mch_new_tooltip_font(शून्य)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    यदि (toolBar == (Widget)0)
	प्रत्यागम;

    menu = gui_find_menu((char_u *)"ToolBar");
    यदि (menu != रिक्त)
	submenu_change(menu, FALSE);
#  endif
}

    शून्य
gui_mch_new_tooltip_colors(शून्य)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    यदि (toolBar == (Widget)0)
	प्रत्यागम;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    यदि (toolbar != रिक्त)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    स्थिर शून्य
submenu_change(
    vimmenu_T	*menu,
    पूर्णाङ्क		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    प्रति (mp = menu; mp != रिक्त; mp = mp->next)
    {
	यदि (mp->id != (Widget)0)
	{
	    यदि (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		यदि (mp->xpm != रिक्त)
		{
		    पूर्णाङ्क		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_मूल्ये(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		यदि (mp->tip != रिक्त)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].मूल्य = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].मूल्य = gui.tooltip_fg_pixel;
		    Xt_Set_मूल्ये(mp->tip->balloonLabel, &args[0], Xt_संख्या(args));
		}
#  endif
# endif
	    }
	    अन्यथा
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		यदि (mp->tip != रिक्त)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].मूल्य = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_मूल्ये(mp->tip->balloonLabel, &args[0], Xt_संख्या(args));
		}
# endif
	    }
	}

	यदि (mp->children != रिक्त)
	{
# यदि (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    यदि (mp->submenu_id != (Widget)0)
	    {
		यदि (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		अन्यथा
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
    शून्य
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    यदि (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    यदि (menu->id == (Widget)0)
	प्रत्यागम;

    Widget	    parent;

    parent = XtParent(menu->id);
# यदि defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    यदि (parent == toolBar && menu->tip != रिक्त)
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
	menu->tip = रिक्त;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    यदि (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    अन्यथा यदि (parent == toolBar)
    {
	Cardinal    अंक_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_मूल्ये(toolBar, XmNnumChildren, &अंक_children, रिक्त);
	यदि (अंक_children == 0)
	    gui_mch_show_toolbar(FALSE);
	अन्यथा
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    शून्य
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
    शून्य
gui_mch_def_colors(शून्य)
{
    यदि (!gui.in_use)
	प्रत्यागम;

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

    शून्य
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    यदि (sb->id != (Widget)0)
	Xt_Va_Set_मूल्ये(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  रिक्त);
}

    शून्य
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    पूर्णाङ्क		x,
    पूर्णाङ्क		y,
    पूर्णाङ्क		w,
    पूर्णाङ्क		h)
{
    यदि (sb->id == (Widget)0)
	प्रत्यागम;

    यदि (sb->type == SBAR_डावा || sb->type == SBAR_उजवा)
    {
	यदि (y == 0)
	    h -= gui.border_offset;
	अन्यथा
	    y -= gui.border_offset;
	Xt_Va_Set_मूल्ये(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		रिक्त);
    }
    अन्यथा
	Xt_Va_Set_मूल्ये(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_उजवा]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		रिक्त);
    XtManageChild(sb->id);
}

    पूर्णाङ्क
gui_mch_get_scrollbar_xpadding(शून्य)
{
    पूर्णाङ्क xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_मूल्ये(textArea, XtNwidth, &tw, XtNx, &tx, रिक्त);
    Xt_Va_Get_मूल्ये(vimShell, XtNwidth, &ww, रिक्त);
    xpad = ww - tw - tx - gui.scrollbar_width;
    प्रत्यागम (xpad < 0) ? 0 : xpad;
}

    पूर्णाङ्क
gui_mch_get_scrollbar_ypadding(शून्य)
{
    पूर्णाङ्क ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_मूल्ये(textArea, XtNheight, &th, XtNy, &ty, रिक्त);
    Xt_Va_Get_मूल्ये(vimShell, XtNheight, &wh, रिक्त);
    ypad = wh - th - ty - gui.scrollbar_height;
    प्रत्यागम (ypad < 0) ? 0 : ypad;
}

    शून्य
gui_mch_enable_scrollbar(scrollbar_T *sb, पूर्णाङ्क flag)
{
    Arg		args[16];
    पूर्णाङ्क		n;

    यदि (sb->id == (Widget)0)
	प्रत्यागम;

    n = 0;
    यदि (flag)
    {
	विकल्प (sb->type)
	{
	    पक्ष SBAR_डावा:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		भङ्ग;

	    पक्ष SBAR_उजवा:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		भङ्ग;

	    पक्ष SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		भङ्ग;
	}
	Xt_Set_मूल्ये(textArea, args, n);
	XtManageChild(sb->id);
    }
    अन्यथा
    {
	यदि (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    विकल्प (sb->type)
	    {
		पक्ष SBAR_डावा:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    भङ्ग;

		पक्ष SBAR_उजवा:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    भङ्ग;

		पक्ष SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    भङ्ग;
	    }
	    Xt_Set_मूल्ये(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    शून्य
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    पूर्णाङ्क		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    पूर्णाङ्क		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    विकल्प (sb->type)
    {
	पक्ष SBAR_डावा:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    भङ्ग;

	पक्ष SBAR_उजवा:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    भङ्ग;

	पक्ष SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    भङ्ग;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    यदि (sb->id == (Widget)0)
	प्रत्यागम;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, किल्ली_Press_Mask, FALSE, gui_x11_किल्ली_hit_cb,
	    (XtPointer)0);
}

    शून्य
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    यदि (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    शून्य
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    यदि (sb->id != (Widget)0)
    {
	यदि (gui.scroll_bg_pixel != INVALCOLOR)
	{
#यदि (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_मूल्ये(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    रिक्त);
	}

	यदि (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_मूल्ये(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    रिक्त);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    यदि (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(शून्य)
{
    प्रत्यागम(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    स्थिर शून्य
do_mnemonic(Widget w, unsigned पूर्णाङ्क keycode)
{
    WidgetList	    children;
    पूर्णाङ्क		    अंक_Children, i;
    Boolean	    isMenu;
    किल्ली_Sym	    mnemonic = '\0';
    वर्ण	    mneString[2];
    Widget	    userData;
    unsigned वर्ण   rowColType;

    यदि (XtIsComposite(w))
    {
	यदि (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_मूल्ये(w, XmNrowColumnType, &rowColType, रिक्त);
	    isMenu = (rowColType != (unsigned वर्ण)XmWORK_AREA);
	}
	अन्यथा
	    isMenu = False;
	यदि (!isMenu)
	{
	    Xt_Va_Get_मूल्ये(w, XmNchildren, &children, XmNnumChildren,
			  &अंक_Children, रिक्त);
	    प्रति (i = 0; i < अंक_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    अन्यथा
    {
	Xt_Va_Get_मूल्ये(w, XmNmnemonic, &mnemonic, रिक्त);
	यदि (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    यदि (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		यदि (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_मूल्ये(w, XmNuserData, &userData, रिक्त);
		    यदि (userData != रिक्त && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		अन्यथा
		{
		    XKeyPressedEvent किल्ली_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(किल्ली_Event);
		    किल्ली_Event.type = किल्ली_Press;
		    किल्ली_Event.serial = 1;
		    किल्ली_Event.send_event = True;
		    किल्ली_Event.display = XtDisplay(w);
		    किल्ली_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & किल्ली_Event,
								     रिक्त, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    स्थिर शून्य
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
    स्थिर शून्य
add_mnemonic_grabs(Widget dialog, Widget w)
{
    वर्ण	    mneString[2];
    WidgetList	    children;
    पूर्णाङ्क		    अंक_Children, i;
    Boolean	    isMenu;
    किल्ली_Sym	    mnemonic = '\0';
    unsigned वर्ण   rowColType;

    यदि (XtIsComposite(w))
    {
	यदि (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_मूल्ये(w, XmNrowColumnType, &rowColType, रिक्त);
	    isMenu = (rowColType != (unsigned वर्ण)XmWORK_AREA);
	}
	अन्यथा
	    isMenu = False;
	यदि (!isMenu)
	{
	    Xt_Va_Get_मूल्ये(w, XmNchildren, &children, XmNnumChildren,
							  &अंक_Children, रिक्त);
	    प्रति (i = 0; i < अंक_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    अन्यथा
    {
	Xt_Va_Get_मूल्ये(w, XmNmnemonic, &mnemonic, रिक्त);
	यदि (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_किल्ली(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    स्थिर शून्य
activate_dialog_mnemonics(Widget dialog)
{
    यदि (!dialog)
	प्रत्यागम;

    XtAddEventHandler(dialog, किल्ली_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) रिक्त);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    स्थिर शून्य
suppress_dialog_mnemonics(Widget dialog)
{
    यदि (!dialog)
	प्रत्यागम;

    Xt_Ungrab_किल्ली(dialog, Any_किल्ली, Mod1Mask);
    XtRemoveEventHandler(dialog, किल्ली_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) रिक्त);
}

#यदि defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    स्थिर शून्य
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    यदि (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	यदि (fl != रिक्त)
	{
	    यदि (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    अन्यथा
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
	    XmFontListFree(fl);
	}
    }
# अन्यथा
    यदि (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	यदि (fl != रिक्त)
	{
	    यदि (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    अन्यथा
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#यदि defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

प्रकारनिर्देश संरचना dialog_callback_arg
{
    वर्ण *  args;   // not used right now
    पूर्णाङ्क	    id;
} dcbarg_T;

स्थिर Widget dialog_wgt;
स्थिर वर्ण *browse_fname = रिक्त;
स्थिर XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

स्थिर शून्य DialogCancelCB(Widget, XtPointer, XtPointer);
स्थिर शून्य DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    स्थिर शून्य
set_predefined_label(Widget parent, String name, वर्ण *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    किल्ली_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    यदि (!w)
	प्रत्यागम;

    p = vim_strsave((char_u *)new_label);
    यदि (p == रिक्त)
	प्रत्यागम;
    प्रति (next = p; *next; ++next)
    {
	यदि (*next == DLG_HOTKEY_CHAR)
	{
	    पूर्णाङ्क len = STRLEN(next);

	    यदि (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((वर्ण *)p, STRING_TAG);
    vim_free(p);

    यदि (str != रिक्त)
    {
	Xt_Va_Set_मूल्ये(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		रिक्त);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    स्थिर शून्य
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    यदि (!w)
	प्रत्यागम;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    पूर्णाङ्क		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = रिक्त;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", रिक्त, 0);

    यदि (initdir == रिक्त || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    यदि (dflt == रिक्त)
	dflt = (char_u *)"";
    अन्यथा यदि (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    यदि (filter == रिक्त)
	pattern = (char_u *)"";
    अन्यथा
    {
	char_u	*s, *p;

	s = filter;
	प्रति (p = filter; *p != NUL; ++p)
	{
	    यदि (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    यदि (*p == ';' || *p == '\n')	// end of (first) pattern
		भङ्ग;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	यदि (pattern == रिक्त)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_मूल्ये(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (वर्ण *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (वर्ण *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (वर्ण *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (वर्ण *)title, STRLEN(title) + 1,
	रिक्त);

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
    यदि (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_मूल्ये(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, रिक्त);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned वर्ण)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    कुरु
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_आदान_Mask)XtIMAll);
    } यावत् (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    यदि (browse_fname == रिक्त)
	प्रत्यागम रिक्त;
    प्रत्यागम vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    स्थिर शून्य
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    यदि (browse_fname != रिक्त)
    {
	XtFree(browse_fname);
	browse_fname = रिक्त;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    स्थिर शून्य
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    यदि (browse_fname != रिक्त)
    {
	XtFree(browse_fname);
	browse_fname = रिक्त;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->मूल्य, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#यदि defined(FEAT_GUI_DIALOG)

स्थिर पूर्णाङ्क	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    स्थिर शून्य
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    वर्ण	buf[2];
    किल्ली_Sym	किल्ली_sym;

    यदि (XLookupString(&(event->xkey), buf, 2, &किल्ली_sym, रिक्त) == 1)
    {
	यदि (*buf == CAR)
	    dialogStatus = 1;
	अन्यथा यदि (*buf == ESC)
	    dialogStatus = 2;
    }
    यदि ((किल्ली_sym == XK_डावा || किल्ली_sym == XK_उजवा)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    स्थिर शून्य
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (पूर्णाङ्क)(long)client_data + 1;
}

# ifdef HAVE_XPM

    स्थिर Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    वर्ण	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    पूर्णाङ्क			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_चिन्ह	color[5] =
    {
	{"none", रिक्त, 0},
	{"iconColor1", रिक्त, 0},
	{"bottomShadowColor", रिक्त, 0},
	{"topShadowColor", रिक्त, 0},
	{"selectColor", रिक्त, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_मूल्ये(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  रिक्त);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, मूळ_Window_Of_Screen(scr),
		    data, &pixmap, रिक्त, &attr);

    Xt_Va_Get_मूल्ये(label, XmNrecomputeSize, &rs, रिक्त);
    Xt_Va_Set_मूल्ये(label, XmNrecomputeSize, True, रिक्त);
    Xt_Va_Set_मूल्ये(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    रिक्त);
    Xt_Va_Set_मूल्ये(label, XmNrecomputeSize, rs, रिक्त);

    प्रत्यागम label;
}
# endif

    पूर्णाङ्क
gui_mch_dialog(
    पूर्णाङ्क		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    पूर्णाङ्क		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    पूर्णाङ्क		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    पूर्णाङ्क			butcount;
    Widget		w;
    Widget		dialogform = रिक्त;
    Widget		form = रिक्त;
    Widget		dialogtextfield = रिक्त;
    Widget		*buttons;
    Widget		sep_form = रिक्त;
    Boolean		vertical;
    Widget		separator = रिक्त;
    पूर्णाङ्क			n;
    Arg			args[6];
# ifdef HAVE_XPM
    वर्ण		**icon_data = रिक्त;
    Widget		dialogpixmap = रिक्त;
# endif

    यदि (title == रिक्त)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (वर्ण *)"dialog", रिक्त, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != रिक्त);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((वर्ण *)title);
    यदि (label == रिक्त)
	प्रत्यागम -1;
    Xt_Va_Set_मूल्ये(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    रिक्त);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    यदि (buts == रिक्त)
	प्रत्यागम -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    प्रति (p = buts; *p; ++p)
	यदि (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    यदि (buttons == रिक्त)
    {
	vim_free(buts);
	प्रत्यागम -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    प्रति (butcount = 0; *p; ++butcount)
    {
	किल्ली_Sym mnemonic = NUL;

	प्रति (next = p; *next; ++next)
	{
	    यदि (*next == DLG_HOTKEY_CHAR)
	    {
		पूर्णाङ्क len = STRLEN(next);

		यदि (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    यदि (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		भङ्ग;
	    }
	}
	label = XmStringCreate(_((वर्ण *)p), STRING_TAG);
	यदि (label == रिक्त)
	    भङ्ग;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ढकला_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		रिक्त);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	यदि (butcount > 0)
	{
	    यदि (vertical)
		Xt_Va_Set_मूल्ये(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			रिक्त);
	    अन्यथा
	    {
		यदि (*next == NUL)
		{
		    Xt_Va_Set_मूल्ये(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    रिक्त);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    रिक्त);
		    XtManageChild(sep_form);
		}
		अन्यथा
		{
		    Xt_Va_Set_मूल्ये(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    रिक्त);
		}
	    }
	}
	अन्यथा यदि (!vertical)
	{
	    यदि (*next == NUL)
	    {
		Xt_Va_Set_मूल्ये(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			रिक्त);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			रिक्त);
		XtManageChild(sep_form);
	    }
	    अन्यथा
		Xt_Va_Set_मूल्ये(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			रिक्त);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    यदि (butcount > 0)
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

    यदि (textfield != रिक्त)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		रिक्त);
	यदि (butcount > 0)
	    Xt_Va_Set_मूल्ये(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    रिक्त);
	अन्यथा
	    Xt_Va_Set_मूल्ये(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    रिक्त);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (वर्ण *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, किल्ली_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)रिक्त);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    रिक्त);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    विकल्प (type)
    {
	पक्ष VIM_GENERIC:
	    icon_data = generic_xpm;
	    भङ्ग;
	पक्ष VIM_ERROR:
	    icon_data = error_xpm;
	    भङ्ग;
	पक्ष VIM_WARNING:
	    icon_data = alert_xpm;
	    भङ्ग;
	पक्ष VIM_INFO:
	    icon_data = info_xpm;
	    भङ्ग;
	पक्ष VIM_QUESTION:
	    icon_data = quest_xpm;
	    भङ्ग;
	स्वभाव:
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
    label = XmStringCreateLtoR((वर्ण *)message, STRING_TAG);
    यदि (label == रिक्त)
    {
	vim_free(buttons);
	प्रत्यागम -1;
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
# अन्यथा
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				रिक्त);
    XmStringFree(label);
    set_fontlist(w);

    यदि (textfield != रिक्त)
    {
	Xt_Va_Set_मूल्ये(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		रिक्त);
    }
    अन्यथा
    {
	यदि (butcount > 0)
	    Xt_Va_Set_मूल्ये(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    रिक्त);
	अन्यथा
	    Xt_Va_Set_मूल्ये(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    रिक्त);
    }

    यदि (dfltbutton < 1)
	dfltbutton = 1;
    यदि (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_मूल्ये(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], रिक्त);
    यदि (textfield != रिक्त)
	Xt_Va_Set_मूल्ये(dialogform, XmNinitialFocus, dialogtextfield, रिक्त);
    अन्यथा
	Xt_Va_Set_मूल्ये(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									रिक्त);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    यदि (textfield != रिक्त && *textfield != NUL)
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
    प्रति (;;)
    {
	XtAppProcessEvent(app, (Xt_आदान_Mask)XtIMAll);
	यदि (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    भङ्ग;
    }

    vim_free(buttons);

    यदि (textfield != रिक्त)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	यदि (p == रिक्त || dialogStatus < 0)
	    *textfield = NUL;
	अन्यथा
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((वर्ण *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    प्रत्यागम dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#यदि defined(FEAT_TOOLBAR)
    शून्य
gui_mch_show_toolbar(पूर्णाङ्क showit)
{
    Cardinal	अंक_Children;	    // how many children toolBar has

    यदि (toolBar == (Widget)0)
	प्रत्यागम;
    Xt_Va_Get_मूल्ये(toolBar, XmNnumChildren, &अंक_Children, रिक्त);
    यदि (showit && अंक_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_मूल्ये(toolBar, XmNchildren, &children, रिक्त);
	{
	    शून्य    (*action)(BalloonEval *);
	    पूर्णाङ्क	    text = 0;

	    यदि (strstr((ध्रुव वर्ण *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    अन्यथा
		action = &gui_mch_disable_beval_area;
	    यदि (strstr((ध्रुव वर्ण *)p_toolbar, "text"))
		text = 1;
	    अन्यथा यदि (strstr((ध्रुव वर्ण *)p_toolbar, "icons"))
		text = -1;
	    यदि (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    यदि (menu_is_toolbar(toolbar->dname))
			भङ्ग;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		यदि (toolbar)
		{
		    प्रति (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			पूर्णाङ्क	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			यदि (cur->tip != रिक्त)
			    (*action)(cur->tip);
			यदि (!menu_is_separator(cur->name))
			{
			    यदि (text == 1 || cur->xpm == रिक्त)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    यदि (cur->id != रिक्त)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_मूल्ये(cur->id, args, n);
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
	यदि (showing_tabline)
	{
	    Xt_Va_Set_मूल्ये(tab_ओळ,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  रिक्त);
	    Xt_Va_Set_मूल्ये(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ओळ,
			  रिक्त);
	}
	अन्यथा
# endif
	    Xt_Va_Set_मूल्ये(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  रिक्त);
	यदि (XtIsManaged(menuBar))
	    Xt_Va_Set_मूल्ये(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    रिक्त);
	अन्यथा
	    Xt_Va_Set_मूल्ये(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    रिक्त);
    }
    अन्यथा
    {
	gui.toolbar_height = 0;
	यदि (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      रिक्त);
	}
	अन्यथा
	{
# ifdef FEAT_GUI_TABLINE
	    यदि (showing_tabline)
	    {
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_FORM,
			      रिक्त);
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ओळ,
			      रिक्त);
	    }
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      रिक्त);
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
    स्थिर शून्य
reset_focus(शून्य)
{
    यदि (textArea != रिक्त)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    पूर्णाङ्क
gui_mch_compute_toolbar_height(शून्य)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	अंक_Children;	    // how many children toolBar has
    पूर्णाङ्क		i;

    borders = 0;
    height = 0;
    यदि (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_मूल्ये(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		रिक्त);
	borders += fst + fmh;
	Xt_Va_Get_मूल्ये(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &अंक_Children, रिक्त);
	borders += tst + tmh;
	प्रति (i = 0; i < (पूर्णाङ्क)अंक_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_मूल्ये(children[i], XmNheight, &whgt, रिक्त);
	    यदि (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    यदि (height < 10)
	height = 24;
# endif

    प्रत्यागम (पूर्णाङ्क)(height + (borders << 1));
}

    शून्य
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_मूल्ये(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    रिक्त);
}
#endif

#यदि defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    शून्य
gui_mch_show_tabline(पूर्णाङ्क showit)
{
    यदि (tab_ओळ == (Widget)0)
	प्रत्यागम;

    यदि (!showit != !showing_tabline)
    {
	यदि (showit)
	{
	    XtManageChild(tab_ओळ);
	    XtUnmanageChild(XtNameToWidget(tab_ओळ, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_ओळ, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_ओळ,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    यदि (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), रिक्त);
	    अन्यथा
#  endif
		यदि (XtIsManaged(menuBar))
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, रिक्त);
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(tab_ओळ,
			      XmNtopAttachment, XmATTACH_FORM, रिक्त);
	    Xt_Va_Set_मूल्ये(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ओळ,
			  रिक्त);
	}
	अन्यथा
	{
	    XtUnmanageChild(tab_ओळ);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    यदि (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), रिक्त);
	    अन्यथा
#  endif
		यदि (XtIsManaged(menuBar))
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, रिक्त);
	    अन्यथा
# endif
		Xt_Va_Set_मूल्ये(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, रिक्त);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    पूर्णाङ्क
gui_mch_showing_tabline(शून्य)
{
    प्रत्यागम tab_ओळ != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    शून्य
gui_mch_update_tabline(शून्य)
{
    tabpage_T		*tp;
    पूर्णाङ्क			nr = 1, n;
    Arg			args[10];
    पूर्णाङ्क			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    पूर्णाङ्क			last_page, tab_मोजणी;
    XmString		label_str;
    वर्ण		*label_cstr;
    BalloonEval		*beval;

    यदि (tab_ओळ == (Widget)0)
	प्रत्यागम;

    // Add a label for each tab page.  They all contain the same text area.
    प्रति (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, ++nr)
    {
	यदि (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_ओळ, nr, &page_info);
	यदि (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ढकला_Button(tab_ओळ, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, रिक्त, tabline_balloon_cb,
									रिक्त);
	    Xt_Va_Set_मूल्ये(tab, XmNuserData, beval, रिक्त);
	}
	अन्यथा
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_मूल्ये(tab, Xm_Npage_संख्या, nr, रिक्त);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_मूल्ये(tab, XmNlabelString, &label_str, रिक्त);
	यदि (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    यदि (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_मूल्ये(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, रिक्त);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_मोजणी = nr - 1;

    Xt_Va_Get_मूल्ये(tab_ओळ, Xm_Nlast_Page_संख्या, &last_page, रिक्त);

    // Remove any old labels.
    यावत् (nr <= last_page)
    {
	यदि (XmNotebookGetPageInfo(tab_ओळ, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_संख्या == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_मूल्ये(page_info.major_tab_widget, XmNuserData, &beval, रिक्त);
	    यदि (beval != रिक्त)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_मूल्ये(tab_ओळ, Xm_Nlast_Page_संख्या, tab_मोजणी, रिक्त);

    Xt_Va_Get_मूल्ये(tab_ओळ, Xm_Ncurrent_Page_संख्या, &currentpage, रिक्त);
    यदि (currentpage != curtabidx)
	Xt_Va_Set_मूल्ये(tab_ओळ, Xm_Ncurrent_Page_संख्या, curtabidx, रिक्त);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    शून्य
gui_mch_set_curtab(पूर्णाङ्क nr)
{
    पूर्णाङ्क		currentpage;

    यदि (tab_ओळ == (Widget)0)
	प्रत्यागम;

    Xt_Va_Get_मूल्ये(tab_ओळ, Xm_Ncurrent_Page_संख्या, &currentpage, रिक्त);
    यदि (currentpage != nr)
	Xt_Va_Set_मूल्ये(tab_ओळ, Xm_Ncurrent_Page_संख्या, nr, रिक्त);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    स्थिर शून्य
gui_motif_menu_colors(Widget id)
{
    यदि (gui.menu_bg_pixel != INVALCOLOR)
#यदि (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#अन्यथा
	Xt_Va_Set_मूल्ये(id, XmNbackground, gui.menu_bg_pixel, रिक्त);
#endif
    यदि (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_मूल्ये(id, XmNforeground, gui.menu_fg_pixel, रिक्त);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    स्थिर शून्य
gui_motif_scroll_colors(Widget id)
{
    यदि (gui.scroll_bg_pixel != INVALCOLOR)
#यदि (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#अन्यथा
	Xt_Va_Set_मूल्ये(id, XmNbackground, gui.scroll_bg_pixel, रिक्त);
#endif
    यदि (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_मूल्ये(id, XmNforeground, gui.scroll_fg_pixel, रिक्त);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    शून्य
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    यदि (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	यदि (fl != रिक्त)
	{
	    यदि (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    अन्यथा
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
	    XmFontListFree(fl);
	}
    }
# अन्यथा
    यदि (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	यदि (fl != रिक्त)
	{
	    यदि (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    अन्यथा
		Xt_Va_Set_मूल्ये(id, XmNfontList, fl, रिक्त);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

प्रकारनिर्देश संरचना _SharedFindReplace
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

स्थिर SharedFindReplace find_widgets = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};
स्थिर SharedFindReplace repl_widgets = {रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त, रिक्त};

    स्थिर शून्य
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    यदि (cd != रिक्त)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    स्थिर शून्य
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    यदि (cd != रिक्त)
	XtUnmanageChild(cd->dialog);
}

    स्थिर शून्य
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    स्थिर शून्य
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    वर्ण	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    यदि (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	प्रत्यागम;
    }

    // Get the search/replace strings from the dialog
    यदि (flags == FRD_FINDNEXT)
    {
	repl_text = रिक्त;
	sfr = &find_widgets;
    }
    अन्यथा
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_मूल्ये(sfr->down, XmNset, &direction_down, रिक्त);
    Xt_Va_Get_मूल्ये(sfr->wword, XmNset, &wword, रिक्त);
    Xt_Va_Get_मूल्ये(sfr->mcase, XmNset, &mcase, रिक्त);
    यदि (wword)
	flags |= FRD_WHOLE_WORD;
    यदि (mcase)
	flags |= FRD_MATCH_CASE;

    (शून्य)gui_do_findrepl((पूर्णाङ्क)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    यदि (find_text != रिक्त)
	XtFree(find_text);
    यदि (repl_text != रिक्त)
	XtFree(repl_text);
}

    स्थिर शून्य
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    किल्ली_Sym keysym;

    यदि (frdp == रिक्त)
	प्रत्यागम;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    यदि (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    स्थिर शून्य
set_label(Widget w, वर्ण *label)
{
    XmString	str;
    char_u	*p, *next;
    किल्ली_Sym	mnemonic = NUL;

    यदि (!w)
	प्रत्यागम;

    p = vim_strsave((char_u *)label);
    यदि (p == रिक्त)
	प्रत्यागम;
    प्रति (next = p; *next; ++next)
    {
	यदि (*next == DLG_HOTKEY_CHAR)
	{
	    पूर्णाङ्क len = STRLEN(next);

	    यदि (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((वर्ण *)p);
    vim_free(p);
    यदि (str)
    {
	Xt_Va_Set_मूल्ये(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		रिक्त);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    स्थिर शून्य
find_replace_dialog_create(char_u *arg, पूर्णाङ्क do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		आदान_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    पूर्णाङ्क			n;
    Arg			args[6];
    पूर्णाङ्क			wword = FALSE;
    पूर्णाङ्क			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    यदि (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	यदि (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	अन्यथा
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	यदि (entry_text != रिक्त)
	    XmTextFieldSetString(frdp->what, (वर्ण *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_मूल्ये(frdp->wword, XmNset, wword, रिक्त);
	प्रत्यागम;
    }

    // Create a fresh new dialog window
    यदि (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    अन्यथा
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
	    रिक्त);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_ढकला_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    रिक्त);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    यदि (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ढकला_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		रिक्त);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ढकला_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		रिक्त);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ढकला_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		रिक्त);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ढकला_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    रिक्त);
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

    आदान_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    रिक्त);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, आदान_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		रिक्त);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, आदान_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		रिक्त);

	यदि (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	आदान_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    रिक्त);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, आदान_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    रिक्त);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, किल्ली_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	अन्यथा
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, किल्ली_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_मूल्ये(label_what, args, n);
	widest = width;
	यदि (do_replace)
	{
	    Xt_Get_मूल्ये(label_with, args, n);
	    यदि (width > widest)
		widest = width;
	}

	Xt_Va_Set_मूल्ये(frdp->what, XmNleftOffset, widest, रिक्त);
	यदि (do_replace)
	    Xt_Va_Set_मूल्ये(frdp->with, XmNleftOffset, widest, रिक्त);

    }

    XtManageChild(आदान_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, आदान_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, आदान_form,
		रिक्त);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		रिक्त);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)रिक्त, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		रिक्त);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		रिक्त);
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
	    XmNtopWidget, आदान_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    रिक्त);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    रिक्त);
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
	    रिक्त);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    यदि (entry_text != रिक्त)
	XmTextFieldSetString(frdp->what, (वर्ण *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   शून्य
gui_mch_find_dialog(exarg_T *eap)
{
    यदि (!gui.in_use)
	प्रत्यागम;

    find_replace_dialog_create(eap->arg, FALSE);
}


    शून्य
gui_mch_replace_dialog(exarg_T *eap)
{
    यदि (!gui.in_use)
	प्रत्यागम;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    शून्य
gui_motif_synch_fonts(शून्य)
{
    SharedFindReplace *frdp;
    पूर्णाङ्क		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    यदि (font == रिक्त)
	प्रत्यागम;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    प्रति (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	यदि (frdp->dialog)
	{
	    Xt_Va_Set_मूल्ये(frdp->what, XmNfontList, font_list, रिक्त);
	    यदि (do_replace)
		Xt_Va_Set_मूल्ये(frdp->with, XmNfontList, font_list, रिक्त);
	}
    }

    XmFontListFree(font_list);
}
