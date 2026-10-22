/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#অন্তর্ভাৱ "vim.h"

#অন্তর্ভাৱ <Xm/Form.h>
#অন্তর্ভাৱ <Xm/RowColumn.h>
#অন্তর্ভাৱ <Xm/PushB.h>
#অন্তর্ভাৱ <Xm/Text.h>
#অন্তর্ভাৱ <Xm/TextF.h>
#অন্তর্ভাৱ <Xm/Separator.h>
#অন্তর্ভাৱ <Xm/Label.h>
#অন্তর্ভাৱ <Xm/CascadeB.h>
#অন্তর্ভাৱ <Xm/ScrollBar.h>
#অন্তর্ভাৱ <Xm/MenuShell.h>
#অন্তর্ভাৱ <Xm/DrawingA.h>
#যদি (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#অন্তর্ভাৱ <Xm/Frame.h>
#অন্তর্ভাৱ <Xm/LabelG.h>
#অন্তর্ভাৱ <Xm/ToggleBG.h>
#অন্তর্ভাৱ <Xm/SeparatoG.h>
#অন্তর্ভাৱ <Xm/XmP.h>

#অন্তর্ভাৱ <X11/keysym.h>
#অন্তর্ভাৱ <X11/Xatom.h>
#অন্তর্ভাৱ <X11/StringDefs.h>
#অন্তর্ভাৱ <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# যদি defined(VMS)
#  include <xpm.h>
# অন্যথা
#  include <X11/xpm.h>
# endif
#অন্যথা
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#অন্তর্ভাৱ "gui_xmebw.h"	// for our Enhanced Button Widget

#যদি defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

বাহ্য Widget vimShell;

স্থির Widget vimForm;
স্থির Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
স্থির Widget toolBarFrame;
স্থির Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
স্থির Widget	tab_সাৰি;
স্থির Widget	tab_সাৰি_menu = 0;
স্থির পূর্ণাঙ্ক	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# যদি (XmVersion >= 1002)
// remember the last set value for the tearoff item
স্থির পূর্ণাঙ্ক tearoff_val = (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED;
# endif
স্থির Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
স্থির শূন্য reset_focus(শূন্য);
#endif

স্থির শূন্য gui_motif_menu_colors(Widget id);
স্থির শূন্য gui_motif_scroll_colors(Widget id);

#যদি (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#অন্যথা
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    স্থির শূন্য
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	মান;
    পূর্ণাঙ্ক		dragging;

    sb = gui_find_scrollbar((long)client_data);

    মান = ((XmScrollBarCallbackStruct *)call_data)->মান;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (পূর্ণাঙ্ক)XmCR_DRAG);
    gui_drag_scrollbar(sb, মান, dragging);
}

#ifdef FEAT_GUI_TABLINE
    স্থির শূন্য
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    যদি (nptr->reason != (পূর্ণাঙ্ক)XmCR_NONE)
	send_tabline_event(nptr->page_সংখ্যা);
}

    স্থির শূন্য
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_মানসমূহ(w, XmNuserData, &cmd, রিক্ত);
    Xt_Va_Get_মানসমূহ(tab_সাৰি_menu, XmNuserData, &tab_idx, রিক্ত);

    send_tabline_menu_event((পূর্ণাঙ্ক)(long)tab_idx, (পূর্ণাঙ্ক)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    স্থির শূন্য
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((পূর্ণাঙ্ক *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    স্থির পূর্ণাঙ্ক
tabline_scroller_clicked(
    ৱর্ণ		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_সাৰি, scroller_name);
    যদি (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_মানসমূহ(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, রিক্ত);
	যদি (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    যদি (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		ঘূৰাই TRUE;
	}
    }
    ঘূৰাই FALSE;
}

    স্থির শূন্য
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    পূর্ণাঙ্ক				tab_idx = 0;
    WidgetList			children;
    Cardinal			অঙ্ক_Children;
    স্থির XtIntervalId		timer = (XtIntervalId)0;
    স্থির পূর্ণাঙ্ক			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    যদি (event->button == Button1)
    {
	যদি (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    ঘূৰাই;

	যদি (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	অন্যথা
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	ঘূৰাই;
    }

    যদি (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_মানসমূহ(tab_সাৰি_menu, XmNuserData, &tab_idx, রিক্ত);
	send_tabline_menu_event(tab_idx, (পূর্ণাঙ্ক)TABLINE_MENU_CLOSE);
	ঘূৰাই;
    }

    যদি (event->button != Button3)
	ঘূৰাই;

    // When ignoring events don't show the menu.
    যদি (hold_gui_events || cmdwin_type != 0)
	ঘূৰাই;

    যদি (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	যদি (tab_w != (Widget)0 && Xm_Is_ঠেলো_Button(tab_w))
	    Xt_Va_Get_মানসমূহ(tab_w, Xm_Npage_সংখ্যা, &tab_idx, রিক্ত);
    }

    Xt_Va_Set_মানসমূহ(tab_সাৰি_menu, XmNuserData, (XtPointer)(long)tab_idx, রিক্ত);
    Xt_Va_Get_মানসমূহ(tab_সাৰি_menu, XmNchildren, &children, XmNnumChildren,
		  &অঙ্ক_Children, রিক্ত);
    XtManageChildren(children, অঙ্ক_Children);
    XmMenuPosition(tab_সাৰি_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_সাৰি_menu);
}

    স্থির শূন্য
tabline_balloon_cb(BalloonEval *beval, পূর্ণাঙ্ক state UNUSED)
{
    পূর্ণাঙ্ক		nr;
    tabpage_T	*tp;

    যদি (beval->target == (Widget)0)
	ঘূৰাই;

    Xt_Va_Get_মানসমূহ(beval->target, Xm_Npage_সংখ্যা, &nr, রিক্ত);
    tp = find_tabpage(nr);
    যদি (tp == রিক্ত)
	ঘূৰাই;

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

#অন্তর্ভাৱ <Xm/XmP.h>
#অন্তর্ভাৱ <Xm/LabelP.h>

স্থির XtExposeProc old_label_expose = রিক্ত;

    স্থির শূন্য
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned ৱর্ণ   label_type = (পূর্ণাঙ্ক)XmSTRING;

    Xt_Va_Get_মানসমূহ(_w, XmNlabelType, &label_type, (XtPointer)0);

    যদি (XtIsSensitive(_w) || label_type != (পূর্ণাঙ্ক)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    অন্যথা
    {
	XGCValues   মানসমূহ;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	মানসমূহ.graphics_exposures = False;

	যদি (fs != 0)
	{
	    mask |= GCFont;
	    মানসমূহ.font = fs->fid;
	}

	যদি (lw->primitive.শীৰ্ষ_shadow_pixmap != None
		&& lw->primitive.শীৰ্ষ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    মানসমূহ.fill_style = FillTiled;
	    মানসমূহ.tile = lw->primitive.শীৰ্ষ_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	যদি (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	মানসমূহ.foreground = lw->primitive.শীৰ্ষ_shadow_color;
	মানসমূহ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &মানসমূহ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	যদি (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	মানসমূহ.foreground = lw->primitive.bottom_shadow_color;
	মানসমূহ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &মানসমূহ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    শূন্য
gui_x11_create_widgets(শূন্য)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    পূর্ণাঙ্ক		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    যদি (old_label_expose == রিক্ত)
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
#যদি (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_মানসমূহ(vimShell, XmNdeleteResponse, XmDO_NOTHING, রিক্ত);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	রিক্ত);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	পূর্ণাঙ্ক ac = 0;

# যদি (XmVersion >= 1002)
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
	রিক্ত);
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
	রিক্ত);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_সংখ্যা, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_শীৰ্ষ_ডান); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_সাৰি = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_সাৰি, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			রিক্ত);
    XtAddEventHandler(tab_সাৰি, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, রিক্ত);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_সাৰি, "MinorTabScrollerNext");
    Xt_Va_Set_মানসমূহ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, রিক্ত);
    scroller = XtNameToWidget(tab_সাৰি, "MinorTabScrollerPrevious");
    Xt_Va_Set_মানসমূহ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, রিক্ত);

    // Create the tabline popup menu
    tab_সাৰি_menu = XmCreatePopupMenu(tab_সাৰি, "tabline popup", রিক্ত, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((ৱর্ণ *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ঠেলো_Button(tab_সাৰি_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, রিক্ত);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((ৱর্ণ *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ঠেলো_Button(tab_সাৰি_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, রিক্ত);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((ৱর্ণ *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ঠেলো_Button(tab_সাৰি_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, রিক্ত);
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
	রিক্ত);
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
	রিক্ত);

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
    শূন্য
gui_x11_destroy_widgets(শূন্য)
{
    textArea = রিক্ত;
#ifdef FEAT_MENU
    menuBar = রিক্ত;
#endif
}

    শূন্য
gui_mch_set_text_area_pos(
    পূর্ণাঙ্ক	    x UNUSED,
    পূর্ণাঙ্ক	    y UNUSED,
    পূর্ণাঙ্ক	    w UNUSED,
    পূর্ণাঙ্ক	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    শূন্য
gui_x11_set_back_color(শূন্য)
{
    যদি (textArea != রিক্ত)
#যদি (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#অন্যথা
	Xt_Va_Set_মানসমূহ(textArea,
		  XmNbackground, gui.back_pixel,
		  রিক্ত);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    শূন্য
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window মূল, child;
    unsigned পূর্ণাঙ্ক mask;
    unsigned পূর্ণাঙ্ক width, height, border_width, depth;
    পূর্ণাঙ্ক x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_মানসমূহ(shell, XmNmappedWhenManaged, &mappedWhenManaged, রিক্ত);
    Xt_Va_Set_মানসমূহ(shell, XmNmappedWhenManaged, False, রিক্ত);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &মূল, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &মূল, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    যদি (x < 0)
	x = 0;
    যদি (x > maxX)
	x = maxX;
    যদি (y < 0)
	y = 0;
    যদি (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_মানসমূহ(shell, XmNx, x, XmNy, y, রিক্ত);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_মানসমূহ(shell, XmNmappedWhenManaged, mappedWhenManaged, রিক্ত);
}

#যদি defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# যদি (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# অন্যথা
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(রিক্ত, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    ঘূৰাই font_list;
}

# যদি ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(রিক্ত, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    ঘূৰাই font_list;
}
# endif

#endif

#যদি defined(FEAT_MENU)
/*
 * Menu stuff.
 */

স্থির শূন্য gui_motif_add_actext(vimmenu_T *menu);
# যদি (XmVersion >= 1002)
স্থির শূন্য toggle_tearoff(Widget wid);
স্থির শূন্য gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
স্থির শূন্য submenu_change(vimmenu_T *mp, পূর্ণাঙ্ক colors);

স্থির শূন্য do_set_mnemonics(পূর্ণাঙ্ক enable);
স্থির পূর্ণাঙ্ক menu_enabled = TRUE;

    শূন্য
gui_mch_enable_menu(পূর্ণাঙ্ক flag)
{
    যদি (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	যদি (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_মানসমূহ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		রিক্ত);
#  ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
#  endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      রিক্ত);
	}
	অন্যথা
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      রিক্ত);
	}
    }
    অন্যথা
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	যদি (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_মানসমূহ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		রিক্ত);
#  ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
#  endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      রিক্ত);
	}
	অন্যথা
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_FORM,
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      রিক্ত);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    শূন্য
gui_motif_set_mnemonics(পূর্ণাঙ্ক enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    যদি (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    স্থির শূন্য
do_set_mnemonics(পূর্ণাঙ্ক enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	যদি (menu->id != (Widget)0)
	    Xt_Va_Set_মানসমূহ(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    রিক্ত);
}

    শূন্য
gui_mch_add_menu(vimmenu_T *menu, পূর্ণাঙ্ক idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    যদি (menu_is_popup(menu->name))
    {
	Arg arg[2];
	পূর্ণাঙ্ক n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  যদি (XmVersion <= 1002)
	যদি (mouse_model_popup())
#  endif
	{
	    যদি (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    যদি (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	ঘূৰাই;
    }
# endif

    যদি (!menu_is_menubar(menu->name)
	    || (parent != রিক্ত && parent->submenu_id == (Widget)0))
	ঘূৰাই;

    label = XmStringCreate((ৱর্ণ *)menu->dname, STRING_TAG);
    যদি (label == রিক্ত)
	ঘূৰাই;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == রিক্ত) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# যদি (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != রিক্ত
			   && tearoff_val == (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    রিক্ত);
    XmStringFree(label);

    যদি (menu->id == (Widget)0)		// failed
	ঘূৰাই;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    যদি (parent == রিক্ত && menu->priority >= 9999)
	Xt_Va_Set_মানসমূহ(menuBar,
		XmNmenuHelpWidget, menu->id,
		রিক্ত);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	রিক্ত);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	রিক্ত);
    gui_motif_menu_colors(menu->submenu_id);

    যদি (menu->submenu_id == (Widget)0)		// failed
	ঘূৰাই;

# যদি (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_মানসমূহ(menu->id,
	XmNsubMenuId, menu->submenu_id,
	রিক্ত);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    যদি (parent == রিক্ত)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    স্থির শূন্য
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    যদি (menu->actext == রিক্ত || menu->id == (Widget)0)
	ঘূৰাই;

    label = XmStringCreate((ৱর্ণ *)menu->actext, STRING_TAG);
    যদি (label == রিক্ত)
	ঘূৰাই;
    Xt_Va_Set_মানসমূহ(menu->id, XmNacceleratorText, label, রিক্ত);
    XmStringFree(label);
}

    শূন্য
gui_mch_toggle_tearoffs(পূর্ণাঙ্ক enable)
{
# যদি (XmVersion >= 1002)
    যদি (enable)
	tearoff_val = (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED;
    অন্যথা
	tearoff_val = (পূর্ণাঙ্ক)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(মূল_menu);
# endif
}

# যদি (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    স্থির শূন্য
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_মানসমূহ(wid, XmNtearOffModel, tearoff_val, রিক্ত);
    যদি (tearoff_val == (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    স্থির শূন্য
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    যেতিয়ালৈ (menu != রিক্ত)
    {
	যদি (!menu_is_popup(menu->name))
	{
	    যদি (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    পূর্ণাঙ্ক
gui_mch_text_area_extra_height(শূন্য)
{
    Dimension	shadowHeight;

    Xt_Va_Get_মানসমূহ(textAreaForm, XmNshadowThickness, &shadowHeight, রিক্ত);
    ঘূৰাই shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    শূন্য
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    স্থির Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    যদি (id != (Widget)0)
	Xt_Va_Get_মানসমূহ(id, XmNheight, &height, রিক্ত);

    // Find any menu Widget, to be able to call XtManageChild()
    অন্যথা
	FOR_ALL_MENUS(mp)
	    যদি (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ভঙ্গ;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    যদি (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	যদি (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_মানসমূহ(mp->id, XmNy, &y, রিক্ত);
	    যদি (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_মানসমূহ(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	রিক্ত);

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
    যদি (gui.shell_created)
	Xt_Va_Set_মানসমূহ(menuBar, XmNheight, gui.menu_height, রিক্ত);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

স্থির ৱর্ণ **get_toolbar_pixmap(vimmenu_T *menu, ৱর্ণ **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    পূর্ণাঙ্ক		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    মূল_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (ৱর্ণ *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    যদি (status == XpmSuccess)
	ঘূৰাই OK;
    ঘূৰাই FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    স্থির ৱর্ণ **
get_toolbar_pixmap(vimmenu_T *menu, ৱর্ণ **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    ৱর্ণ	**xpm = রিক্ত;		// xpm array
    পূর্ণাঙ্ক		res;

    *fname = রিক্ত;
    buf[0] = NUL;			// start with NULL path

    যদি (menu->iconfile != রিক্ত)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	যদি (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	যদি (res == OK)
	{
	    *fname = (ৱর্ণ *)vim_strsave(buf);
	    ঘূৰাই tb_blank_xpm;
	}
    }

    যদি (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	যদি (menu->iconidx >= 0 && menu->iconidx
	       < (পূর্ণাঙ্ক)(পরিমাণ(built_in_pixmaps) / পরিমাণ(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	অন্যথা
	    xpm = tb_blank_xpm;
    }

    ঘূৰাই xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    স্থির পূর্ণাঙ্ক
add_pixmap_args(vimmenu_T *menu, Arg *args, পূর্ণাঙ্ক n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    যদি (menu->xpm == রিক্ত)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    অন্যথা
    {
	যদি (menu->xpm_fname != রিক্ত)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    ঘূৰাই n;
}
# endif // FEAT_TOOLBAR

    শূন্য
gui_mch_add_menu_item(vimmenu_T *menu, পূর্ণাঙ্ক idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# যদি (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    যদি (menu_is_child_of_popup(menu) && !mouse_model_popup())
	ঘূৰাই;
# endif

# ifdef FEAT_TOOLBAR
    যদি (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = রিক্ত;    // fallback label if pixmap not found
	পূর্ণাঙ্ক		n;
	Arg		args[18];

	n = 0;
	যদি (menu_is_separator(menu->name))
	{
	    ৱর্ণ	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (ৱর্ণ *)vim_strchr(menu->name, ':');
	    যদি (cp != রিক্ত)
		wid = (Dimension)atoi(++cp);
	    অন্যথা
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	অন্যথা
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
	    xms = XmStringCreate((ৱর্ণ *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	যদি (menu->id == রিক্ত)
	{
	    menu->id = XtCreateManagedWidget((ৱর্ণ *)menu->dname,
			type, toolBar, args, n);
	    যদি (menu->id != রিক্ত && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	অন্যথা
	    Xt_Set_মানসমূহ(menu->id, args, n);
	যদি (xms != রিক্ত)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = রিক্ত;
	// When adding first item to toolbar it might have to be enabled .
	যদি (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != রিক্ত)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	ঘূৰাই;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    যদি (parent->submenu_id == (Widget)0)
	ঘূৰাই;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    যদি (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# যদি (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		রিক্ত);
	gui_motif_menu_colors(menu->id);
	ঘূৰাই;
    }

    label = XmStringCreate((ৱর্ণ *)menu->dname, STRING_TAG);
    যদি (label == রিক্ত)
	ঘূৰাই;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ঠেলো_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# যদি (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (পূর্ণাঙ্ক)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	রিক্ত);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    যদি (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# যদি (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    শূন্য
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    পূর্ণাঙ্ক		idx = 0;

    // When GUI hasn't started the menus have not been created.
    যদি (!gui.in_use)
	ঘূৰাই;

    যেতিয়ালৈ (menu)
    {
      যদি (menu->children != রিক্ত)
      {
	  যদি (menu_is_popup(menu->name))
	  {
	      যদি (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      অন্যথা
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      অন্যথা যদি (menu_is_child_of_popup(menu))
      {
	  যদি (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  অন্যথা
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    শূন্য
gui_mch_new_menu_colors(শূন্য)
{
    যদি (menuBar == (Widget)0)
	ঘূৰাই;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(মূল_menu, TRUE);
}

    শূন্য
gui_mch_new_menu_font(শূন্য)
{
    যদি (menuBar == (Widget)0)
	ঘূৰাই;
    submenu_change(মূল_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_মানসমূহ(menuBar, XmNheight, &height, রিক্ত);
	gui.menu_height = height;

	Xt_Va_Get_মানসমূহ(vimShell, XtNwidth, &w, XtNheight, &h, রিক্ত);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# যদি defined(FEAT_BEVAL_GUI)
    শূন্য
gui_mch_new_tooltip_font(শূন্য)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    যদি (toolBar == (Widget)0)
	ঘূৰাই;

    menu = gui_find_menu((char_u *)"ToolBar");
    যদি (menu != রিক্ত)
	submenu_change(menu, FALSE);
#  endif
}

    শূন্য
gui_mch_new_tooltip_colors(শূন্য)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    যদি (toolBar == (Widget)0)
	ঘূৰাই;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    যদি (toolbar != রিক্ত)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    স্থির শূন্য
submenu_change(
    vimmenu_T	*menu,
    পূর্ণাঙ্ক		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    বাবে (mp = menu; mp != রিক্ত; mp = mp->next)
    {
	যদি (mp->id != (Widget)0)
	{
	    যদি (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		যদি (mp->xpm != রিক্ত)
		{
		    পূর্ণাঙ্ক		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_মানসমূহ(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		যদি (mp->tip != রিক্ত)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].মান = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].মান = gui.tooltip_fg_pixel;
		    Xt_Set_মানসমূহ(mp->tip->balloonLabel, &args[0], Xt_সংখ্যা(args));
		}
#  endif
# endif
	    }
	    অন্যথা
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		যদি (mp->tip != রিক্ত)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].মান = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_মানসমূহ(mp->tip->balloonLabel, &args[0], Xt_সংখ্যা(args));
		}
# endif
	    }
	}

	যদি (mp->children != রিক্ত)
	{
# যদি (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    যদি (mp->submenu_id != (Widget)0)
	    {
		যদি (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		অন্যথা
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
    শূন্য
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    যদি (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    যদি (menu->id == (Widget)0)
	ঘূৰাই;

    Widget	    parent;

    parent = XtParent(menu->id);
# যদি defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    যদি (parent == toolBar && menu->tip != রিক্ত)
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
	menu->tip = রিক্ত;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    যদি (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    অন্যথা যদি (parent == toolBar)
    {
	Cardinal    অঙ্ক_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_মানসমূহ(toolBar, XmNnumChildren, &অঙ্ক_children, রিক্ত);
	যদি (অঙ্ক_children == 0)
	    gui_mch_show_toolbar(FALSE);
	অন্যথা
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    শূন্য
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
    শূন্য
gui_mch_def_colors(শূন্য)
{
    যদি (!gui.in_use)
	ঘূৰাই;

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

    শূন্য
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    যদি (sb->id != (Widget)0)
	Xt_Va_Set_মানসমূহ(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  রিক্ত);
}

    শূন্য
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    পূর্ণাঙ্ক		x,
    পূর্ণাঙ্ক		y,
    পূর্ণাঙ্ক		w,
    পূর্ণাঙ্ক		h)
{
    যদি (sb->id == (Widget)0)
	ঘূৰাই;

    যদি (sb->type == SBAR_বাম || sb->type == SBAR_ডান)
    {
	যদি (y == 0)
	    h -= gui.border_offset;
	অন্যথা
	    y -= gui.border_offset;
	Xt_Va_Set_মানসমূহ(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		রিক্ত);
    }
    অন্যথা
	Xt_Va_Set_মানসমূহ(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_ডান]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		রিক্ত);
    XtManageChild(sb->id);
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_xpadding(শূন্য)
{
    পূর্ণাঙ্ক xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_মানসমূহ(textArea, XtNwidth, &tw, XtNx, &tx, রিক্ত);
    Xt_Va_Get_মানসমূহ(vimShell, XtNwidth, &ww, রিক্ত);
    xpad = ww - tw - tx - gui.scrollbar_width;
    ঘূৰাই (xpad < 0) ? 0 : xpad;
}

    পূর্ণাঙ্ক
gui_mch_get_scrollbar_ypadding(শূন্য)
{
    পূর্ণাঙ্ক ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_মানসমূহ(textArea, XtNheight, &th, XtNy, &ty, রিক্ত);
    Xt_Va_Get_মানসমূহ(vimShell, XtNheight, &wh, রিক্ত);
    ypad = wh - th - ty - gui.scrollbar_height;
    ঘূৰাই (ypad < 0) ? 0 : ypad;
}

    শূন্য
gui_mch_enable_scrollbar(scrollbar_T *sb, পূর্ণাঙ্ক flag)
{
    Arg		args[16];
    পূর্ণাঙ্ক		n;

    যদি (sb->id == (Widget)0)
	ঘূৰাই;

    n = 0;
    যদি (flag)
    {
	ৱিকল্প (sb->type)
	{
	    পক্ষ SBAR_বাম:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		ভঙ্গ;

	    পক্ষ SBAR_ডান:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		ভঙ্গ;

	    পক্ষ SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		ভঙ্গ;
	}
	Xt_Set_মানসমূহ(textArea, args, n);
	XtManageChild(sb->id);
    }
    অন্যথা
    {
	যদি (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ৱিকল্প (sb->type)
	    {
		পক্ষ SBAR_বাম:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    ভঙ্গ;

		পক্ষ SBAR_ডান:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    ভঙ্গ;

		পক্ষ SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    ভঙ্গ;
	    }
	    Xt_Set_মানসমূহ(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    শূন্য
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    পূর্ণাঙ্ক		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    পূর্ণাঙ্ক		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ৱিকল্প (sb->type)
    {
	পক্ষ SBAR_বাম:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ভঙ্গ;

	পক্ষ SBAR_ডান:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ভঙ্গ;

	পক্ষ SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    ভঙ্গ;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    যদি (sb->id == (Widget)0)
	ঘূৰাই;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, চাবি_Press_Mask, FALSE, gui_x11_চাবি_hit_cb,
	    (XtPointer)0);
}

    শূন্য
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    যদি (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    শূন্য
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    যদি (sb->id != (Widget)0)
    {
	যদি (gui.scroll_bg_pixel != INVALCOLOR)
	{
#যদি (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_মানসমূহ(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    রিক্ত);
	}

	যদি (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_মানসমূহ(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    রিক্ত);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    যদি (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(শূন্য)
{
    ঘূৰাই(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    স্থির শূন্য
do_mnemonic(Widget w, unsigned পূর্ণাঙ্ক keycode)
{
    WidgetList	    children;
    পূর্ণাঙ্ক		    অঙ্ক_Children, i;
    Boolean	    isMenu;
    চাবি_Sym	    mnemonic = '\0';
    ৱর্ণ	    mneString[2];
    Widget	    userData;
    unsigned ৱর্ণ   rowColType;

    যদি (XtIsComposite(w))
    {
	যদি (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_মানসমূহ(w, XmNrowColumnType, &rowColType, রিক্ত);
	    isMenu = (rowColType != (unsigned ৱর্ণ)XmWORK_AREA);
	}
	অন্যথা
	    isMenu = False;
	যদি (!isMenu)
	{
	    Xt_Va_Get_মানসমূহ(w, XmNchildren, &children, XmNnumChildren,
			  &অঙ্ক_Children, রিক্ত);
	    বাবে (i = 0; i < অঙ্ক_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    অন্যথা
    {
	Xt_Va_Get_মানসমূহ(w, XmNmnemonic, &mnemonic, রিক্ত);
	যদি (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    যদি (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		যদি (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_মানসমূহ(w, XmNuserData, &userData, রিক্ত);
		    যদি (userData != রিক্ত && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		অন্যথা
		{
		    XKeyPressedEvent চাবি_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(চাবি_Event);
		    চাবি_Event.type = চাবি_Press;
		    চাবি_Event.serial = 1;
		    চাবি_Event.send_event = True;
		    চাবি_Event.display = XtDisplay(w);
		    চাবি_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & চাবি_Event,
								     রিক্ত, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    স্থির শূন্য
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
    স্থির শূন্য
add_mnemonic_grabs(Widget dialog, Widget w)
{
    ৱর্ণ	    mneString[2];
    WidgetList	    children;
    পূর্ণাঙ্ক		    অঙ্ক_Children, i;
    Boolean	    isMenu;
    চাবি_Sym	    mnemonic = '\0';
    unsigned ৱর্ণ   rowColType;

    যদি (XtIsComposite(w))
    {
	যদি (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_মানসমূহ(w, XmNrowColumnType, &rowColType, রিক্ত);
	    isMenu = (rowColType != (unsigned ৱর্ণ)XmWORK_AREA);
	}
	অন্যথা
	    isMenu = False;
	যদি (!isMenu)
	{
	    Xt_Va_Get_মানসমূহ(w, XmNchildren, &children, XmNnumChildren,
							  &অঙ্ক_Children, রিক্ত);
	    বাবে (i = 0; i < অঙ্ক_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    অন্যথা
    {
	Xt_Va_Get_মানসমূহ(w, XmNmnemonic, &mnemonic, রিক্ত);
	যদি (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_চাবি(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    স্থির শূন্য
activate_dialog_mnemonics(Widget dialog)
{
    যদি (!dialog)
	ঘূৰাই;

    XtAddEventHandler(dialog, চাবি_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) রিক্ত);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    স্থির শূন্য
suppress_dialog_mnemonics(Widget dialog)
{
    যদি (!dialog)
	ঘূৰাই;

    Xt_Ungrab_চাবি(dialog, Any_চাবি, Mod1Mask);
    XtRemoveEventHandler(dialog, চাবি_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) রিক্ত);
}

#যদি defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    স্থির শূন্য
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    যদি (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	যদি (fl != রিক্ত)
	{
	    যদি (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    অন্যথা
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
	    XmFontListFree(fl);
	}
    }
# অন্যথা
    যদি (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	যদি (fl != রিক্ত)
	{
	    যদি (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    অন্যথা
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#যদি defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

প্রকারনির্দেশ সংরচনা dialog_callback_arg
{
    ৱর্ণ *  args;   // not used right now
    পূর্ণাঙ্ক	    id;
} dcbarg_T;

স্থির Widget dialog_wgt;
স্থির ৱর্ণ *browse_fname = রিক্ত;
স্থির XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

স্থির শূন্য DialogCancelCB(Widget, XtPointer, XtPointer);
স্থির শূন্য DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    স্থির শূন্য
set_predefined_label(Widget parent, String name, ৱর্ণ *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    চাবি_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    যদি (!w)
	ঘূৰাই;

    p = vim_strsave((char_u *)new_label);
    যদি (p == রিক্ত)
	ঘূৰাই;
    বাবে (next = p; *next; ++next)
    {
	যদি (*next == DLG_HOTKEY_CHAR)
	{
	    পূর্ণাঙ্ক len = STRLEN(next);

	    যদি (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((ৱর্ণ *)p, STRING_TAG);
    vim_free(p);

    যদি (str != রিক্ত)
    {
	Xt_Va_Set_মানসমূহ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		রিক্ত);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    স্থির শূন্য
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    যদি (!w)
	ঘূৰাই;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    পূর্ণাঙ্ক		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = রিক্ত;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", রিক্ত, 0);

    যদি (initdir == রিক্ত || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    যদি (dflt == রিক্ত)
	dflt = (char_u *)"";
    অন্যথা যদি (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    যদি (filter == রিক্ত)
	pattern = (char_u *)"";
    অন্যথা
    {
	char_u	*s, *p;

	s = filter;
	বাবে (p = filter; *p != NUL; ++p)
	{
	    যদি (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    যদি (*p == ';' || *p == '\n')	// end of (first) pattern
		ভঙ্গ;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	যদি (pattern == রিক্ত)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_মানসমূহ(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (ৱর্ণ *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (ৱর্ণ *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (ৱর্ণ *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (ৱর্ণ *)title, STRLEN(title) + 1,
	রিক্ত);

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
    যদি (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_মানসমূহ(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, রিক্ত);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned ৱর্ণ)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    কৰা
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_প্ৰবেশ_Mask)XtIMAll);
    } যেতিয়ালৈ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    যদি (browse_fname == রিক্ত)
	ঘূৰাই রিক্ত;
    ঘূৰাই vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    স্থির শূন্য
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    যদি (browse_fname != রিক্ত)
    {
	XtFree(browse_fname);
	browse_fname = রিক্ত;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    স্থির শূন্য
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    যদি (browse_fname != রিক্ত)
    {
	XtFree(browse_fname);
	browse_fname = রিক্ত;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->মান, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#যদি defined(FEAT_GUI_DIALOG)

স্থির পূর্ণাঙ্ক	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    স্থির শূন্য
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    ৱর্ণ	buf[2];
    চাবি_Sym	চাবি_sym;

    যদি (XLookupString(&(event->xkey), buf, 2, &চাবি_sym, রিক্ত) == 1)
    {
	যদি (*buf == CAR)
	    dialogStatus = 1;
	অন্যথা যদি (*buf == ESC)
	    dialogStatus = 2;
    }
    যদি ((চাবি_sym == XK_বাম || চাবি_sym == XK_ডান)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    স্থির শূন্য
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (পূর্ণাঙ্ক)(long)client_data + 1;
}

# ifdef HAVE_XPM

    স্থির Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    ৱর্ণ	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    পূর্ণাঙ্ক			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_প্ৰতীক	color[5] =
    {
	{"none", রিক্ত, 0},
	{"iconColor1", রিক্ত, 0},
	{"bottomShadowColor", রিক্ত, 0},
	{"topShadowColor", রিক্ত, 0},
	{"selectColor", রিক্ত, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_মানসমূহ(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  রিক্ত);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, মূল_Window_Of_Screen(scr),
		    data, &pixmap, রিক্ত, &attr);

    Xt_Va_Get_মানসমূহ(label, XmNrecomputeSize, &rs, রিক্ত);
    Xt_Va_Set_মানসমূহ(label, XmNrecomputeSize, True, রিক্ত);
    Xt_Va_Set_মানসমূহ(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    রিক্ত);
    Xt_Va_Set_মানসমূহ(label, XmNrecomputeSize, rs, রিক্ত);

    ঘূৰাই label;
}
# endif

    পূর্ণাঙ্ক
gui_mch_dialog(
    পূর্ণাঙ্ক		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    পূর্ণাঙ্ক		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    পূর্ণাঙ্ক		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    পূর্ণাঙ্ক			butcount;
    Widget		w;
    Widget		dialogform = রিক্ত;
    Widget		form = রিক্ত;
    Widget		dialogtextfield = রিক্ত;
    Widget		*buttons;
    Widget		sep_form = রিক্ত;
    Boolean		vertical;
    Widget		separator = রিক্ত;
    পূর্ণাঙ্ক			n;
    Arg			args[6];
# ifdef HAVE_XPM
    ৱর্ণ		**icon_data = রিক্ত;
    Widget		dialogpixmap = রিক্ত;
# endif

    যদি (title == রিক্ত)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (ৱর্ণ *)"dialog", রিক্ত, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != রিক্ত);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((ৱর্ণ *)title);
    যদি (label == রিক্ত)
	ঘূৰাই -1;
    Xt_Va_Set_মানসমূহ(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    রিক্ত);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    যদি (buts == রিক্ত)
	ঘূৰাই -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    বাবে (p = buts; *p; ++p)
	যদি (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    যদি (buttons == রিক্ত)
    {
	vim_free(buts);
	ঘূৰাই -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    বাবে (butcount = 0; *p; ++butcount)
    {
	চাবি_Sym mnemonic = NUL;

	বাবে (next = p; *next; ++next)
	{
	    যদি (*next == DLG_HOTKEY_CHAR)
	    {
		পূর্ণাঙ্ক len = STRLEN(next);

		যদি (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    যদি (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		ভঙ্গ;
	    }
	}
	label = XmStringCreate(_((ৱর্ণ *)p), STRING_TAG);
	যদি (label == রিক্ত)
	    ভঙ্গ;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ঠেলো_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		রিক্ত);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	যদি (butcount > 0)
	{
	    যদি (vertical)
		Xt_Va_Set_মানসমূহ(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			রিক্ত);
	    অন্যথা
	    {
		যদি (*next == NUL)
		{
		    Xt_Va_Set_মানসমূহ(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    রিক্ত);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    রিক্ত);
		    XtManageChild(sep_form);
		}
		অন্যথা
		{
		    Xt_Va_Set_মানসমূহ(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    রিক্ত);
		}
	    }
	}
	অন্যথা যদি (!vertical)
	{
	    যদি (*next == NUL)
	    {
		Xt_Va_Set_মানসমূহ(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			রিক্ত);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			রিক্ত);
		XtManageChild(sep_form);
	    }
	    অন্যথা
		Xt_Va_Set_মানসমূহ(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			রিক্ত);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    যদি (butcount > 0)
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

    যদি (textfield != রিক্ত)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		রিক্ত);
	যদি (butcount > 0)
	    Xt_Va_Set_মানসমূহ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    রিক্ত);
	অন্যথা
	    Xt_Va_Set_মানসমূহ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    রিক্ত);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (ৱর্ণ *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, চাবি_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)রিক্ত);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    রিক্ত);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ৱিকল্প (type)
    {
	পক্ষ VIM_GENERIC:
	    icon_data = generic_xpm;
	    ভঙ্গ;
	পক্ষ VIM_ERROR:
	    icon_data = error_xpm;
	    ভঙ্গ;
	পক্ষ VIM_WARNING:
	    icon_data = alert_xpm;
	    ভঙ্গ;
	পক্ষ VIM_INFO:
	    icon_data = info_xpm;
	    ভঙ্গ;
	পক্ষ VIM_QUESTION:
	    icon_data = quest_xpm;
	    ভঙ্গ;
	স্ৱভাৱ:
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
    label = XmStringCreateLtoR((ৱর্ণ *)message, STRING_TAG);
    যদি (label == রিক্ত)
    {
	vim_free(buttons);
	ঘূৰাই -1;
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
# অন্যথা
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				রিক্ত);
    XmStringFree(label);
    set_fontlist(w);

    যদি (textfield != রিক্ত)
    {
	Xt_Va_Set_মানসমূহ(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		রিক্ত);
    }
    অন্যথা
    {
	যদি (butcount > 0)
	    Xt_Va_Set_মানসমূহ(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    রিক্ত);
	অন্যথা
	    Xt_Va_Set_মানসমূহ(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    রিক্ত);
    }

    যদি (dfltbutton < 1)
	dfltbutton = 1;
    যদি (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_মানসমূহ(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], রিক্ত);
    যদি (textfield != রিক্ত)
	Xt_Va_Set_মানসমূহ(dialogform, XmNinitialFocus, dialogtextfield, রিক্ত);
    অন্যথা
	Xt_Va_Set_মানসমূহ(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									রিক্ত);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    যদি (textfield != রিক্ত && *textfield != NUL)
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
    বাবে (;;)
    {
	XtAppProcessEvent(app, (Xt_প্ৰবেশ_Mask)XtIMAll);
	যদি (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    ভঙ্গ;
    }

    vim_free(buttons);

    যদি (textfield != রিক্ত)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	যদি (p == রিক্ত || dialogStatus < 0)
	    *textfield = NUL;
	অন্যথা
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((ৱর্ণ *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    ঘূৰাই dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#যদি defined(FEAT_TOOLBAR)
    শূন্য
gui_mch_show_toolbar(পূর্ণাঙ্ক showit)
{
    Cardinal	অঙ্ক_Children;	    // how many children toolBar has

    যদি (toolBar == (Widget)0)
	ঘূৰাই;
    Xt_Va_Get_মানসমূহ(toolBar, XmNnumChildren, &অঙ্ক_Children, রিক্ত);
    যদি (showit && অঙ্ক_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_মানসমূহ(toolBar, XmNchildren, &children, রিক্ত);
	{
	    শূন্য    (*action)(BalloonEval *);
	    পূর্ণাঙ্ক	    text = 0;

	    যদি (strstr((ধ্রুৱ ৱর্ণ *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    অন্যথা
		action = &gui_mch_disable_beval_area;
	    যদি (strstr((ধ্রুৱ ৱর্ণ *)p_toolbar, "text"))
		text = 1;
	    অন্যথা যদি (strstr((ধ্রুৱ ৱর্ণ *)p_toolbar, "icons"))
		text = -1;
	    যদি (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    যদি (menu_is_toolbar(toolbar->dname))
			ভঙ্গ;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		যদি (toolbar)
		{
		    বাবে (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			পূর্ণাঙ্ক	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			যদি (cur->tip != রিক্ত)
			    (*action)(cur->tip);
			যদি (!menu_is_separator(cur->name))
			{
			    যদি (text == 1 || cur->xpm == রিক্ত)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    যদি (cur->id != রিক্ত)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_মানসমূহ(cur->id, args, n);
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
	যদি (showing_tabline)
	{
	    Xt_Va_Set_মানসমূহ(tab_সাৰি,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  রিক্ত);
	    Xt_Va_Set_মানসমূহ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_সাৰি,
			  রিক্ত);
	}
	অন্যথা
# endif
	    Xt_Va_Set_মানসমূহ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  রিক্ত);
	যদি (XtIsManaged(menuBar))
	    Xt_Va_Set_মানসমূহ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    রিক্ত);
	অন্যথা
	    Xt_Va_Set_মানসমূহ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    রিক্ত);
    }
    অন্যথা
    {
	gui.toolbar_height = 0;
	যদি (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      রিক্ত);
	}
	অন্যথা
	{
# ifdef FEAT_GUI_TABLINE
	    যদি (showing_tabline)
	    {
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_FORM,
			      রিক্ত);
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_সাৰি,
			      রিক্ত);
	    }
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      রিক্ত);
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
    স্থির শূন্য
reset_focus(শূন্য)
{
    যদি (textArea != রিক্ত)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    পূর্ণাঙ্ক
gui_mch_compute_toolbar_height(শূন্য)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	অঙ্ক_Children;	    // how many children toolBar has
    পূর্ণাঙ্ক		i;

    borders = 0;
    height = 0;
    যদি (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_মানসমূহ(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		রিক্ত);
	borders += fst + fmh;
	Xt_Va_Get_মানসমূহ(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &অঙ্ক_Children, রিক্ত);
	borders += tst + tmh;
	বাবে (i = 0; i < (পূর্ণাঙ্ক)অঙ্ক_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_মানসমূহ(children[i], XmNheight, &whgt, রিক্ত);
	    যদি (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    যদি (height < 10)
	height = 24;
# endif

    ঘূৰাই (পূর্ণাঙ্ক)(height + (borders << 1));
}

    শূন্য
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_মানসমূহ(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    রিক্ত);
}
#endif

#যদি defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    শূন্য
gui_mch_show_tabline(পূর্ণাঙ্ক showit)
{
    যদি (tab_সাৰি == (Widget)0)
	ঘূৰাই;

    যদি (!showit != !showing_tabline)
    {
	যদি (showit)
	{
	    XtManageChild(tab_সাৰি);
	    XtUnmanageChild(XtNameToWidget(tab_সাৰি, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_সাৰি, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_সাৰি,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    যদি (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), রিক্ত);
	    অন্যথা
#  endif
		যদি (XtIsManaged(menuBar))
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, রিক্ত);
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(tab_সাৰি,
			      XmNtopAttachment, XmATTACH_FORM, রিক্ত);
	    Xt_Va_Set_মানসমূহ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_সাৰি,
			  রিক্ত);
	}
	অন্যথা
	{
	    XtUnmanageChild(tab_সাৰি);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    যদি (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), রিক্ত);
	    অন্যথা
#  endif
		যদি (XtIsManaged(menuBar))
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, রিক্ত);
	    অন্যথা
# endif
		Xt_Va_Set_মানসমূহ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, রিক্ত);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    পূর্ণাঙ্ক
gui_mch_showing_tabline(শূন্য)
{
    ঘূৰাই tab_সাৰি != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    শূন্য
gui_mch_update_tabline(শূন্য)
{
    tabpage_T		*tp;
    পূর্ণাঙ্ক			nr = 1, n;
    Arg			args[10];
    পূর্ণাঙ্ক			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    পূর্ণাঙ্ক			last_page, tab_গুনতি;
    XmString		label_str;
    ৱর্ণ		*label_cstr;
    BalloonEval		*beval;

    যদি (tab_সাৰি == (Widget)0)
	ঘূৰাই;

    // Add a label for each tab page.  They all contain the same text area.
    বাবে (tp = first_tabpage; tp != রিক্ত; tp = tp->tp_next, ++nr)
    {
	যদি (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_সাৰি, nr, &page_info);
	যদি (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ঠেলো_Button(tab_সাৰি, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, রিক্ত, tabline_balloon_cb,
									রিক্ত);
	    Xt_Va_Set_মানসমূহ(tab, XmNuserData, beval, রিক্ত);
	}
	অন্যথা
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_মানসমূহ(tab, Xm_Npage_সংখ্যা, nr, রিক্ত);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_মানসমূহ(tab, XmNlabelString, &label_str, রিক্ত);
	যদি (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    যদি (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_মানসমূহ(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, রিক্ত);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_গুনতি = nr - 1;

    Xt_Va_Get_মানসমূহ(tab_সাৰি, Xm_Nlast_Page_সংখ্যা, &last_page, রিক্ত);

    // Remove any old labels.
    যেতিয়ালৈ (nr <= last_page)
    {
	যদি (XmNotebookGetPageInfo(tab_সাৰি, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_সংখ্যা == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_মানসমূহ(page_info.major_tab_widget, XmNuserData, &beval, রিক্ত);
	    যদি (beval != রিক্ত)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_মানসমূহ(tab_সাৰি, Xm_Nlast_Page_সংখ্যা, tab_গুনতি, রিক্ত);

    Xt_Va_Get_মানসমূহ(tab_সাৰি, Xm_Ncurrent_Page_সংখ্যা, &currentpage, রিক্ত);
    যদি (currentpage != curtabidx)
	Xt_Va_Set_মানসমূহ(tab_সাৰি, Xm_Ncurrent_Page_সংখ্যা, curtabidx, রিক্ত);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    শূন্য
gui_mch_set_curtab(পূর্ণাঙ্ক nr)
{
    পূর্ণাঙ্ক		currentpage;

    যদি (tab_সাৰি == (Widget)0)
	ঘূৰাই;

    Xt_Va_Get_মানসমূহ(tab_সাৰি, Xm_Ncurrent_Page_সংখ্যা, &currentpage, রিক্ত);
    যদি (currentpage != nr)
	Xt_Va_Set_মানসমূহ(tab_সাৰি, Xm_Ncurrent_Page_সংখ্যা, nr, রিক্ত);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    স্থির শূন্য
gui_motif_menu_colors(Widget id)
{
    যদি (gui.menu_bg_pixel != INVALCOLOR)
#যদি (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#অন্যথা
	Xt_Va_Set_মানসমূহ(id, XmNbackground, gui.menu_bg_pixel, রিক্ত);
#endif
    যদি (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_মানসমূহ(id, XmNforeground, gui.menu_fg_pixel, রিক্ত);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    স্থির শূন্য
gui_motif_scroll_colors(Widget id)
{
    যদি (gui.scroll_bg_pixel != INVALCOLOR)
#যদি (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#অন্যথা
	Xt_Va_Set_মানসমূহ(id, XmNbackground, gui.scroll_bg_pixel, রিক্ত);
#endif
    যদি (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_মানসমূহ(id, XmNforeground, gui.scroll_fg_pixel, রিক্ত);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    শূন্য
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    যদি (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	যদি (fl != রিক্ত)
	{
	    যদি (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    অন্যথা
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
	    XmFontListFree(fl);
	}
    }
# অন্যথা
    যদি (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	যদি (fl != রিক্ত)
	{
	    যদি (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    অন্যথা
		Xt_Va_Set_মানসমূহ(id, XmNfontList, fl, রিক্ত);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

প্রকারনির্দেশ সংরচনা _SharedFindReplace
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

স্থির SharedFindReplace find_widgets = {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};
স্থির SharedFindReplace repl_widgets = {রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত, রিক্ত};

    স্থির শূন্য
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    যদি (cd != রিক্ত)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    স্থির শূন্য
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    যদি (cd != রিক্ত)
	XtUnmanageChild(cd->dialog);
}

    স্থির শূন্য
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    স্থির শূন্য
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    ৱর্ণ	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    যদি (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	ঘূৰাই;
    }

    // Get the search/replace strings from the dialog
    যদি (flags == FRD_FINDNEXT)
    {
	repl_text = রিক্ত;
	sfr = &find_widgets;
    }
    অন্যথা
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_মানসমূহ(sfr->down, XmNset, &direction_down, রিক্ত);
    Xt_Va_Get_মানসমূহ(sfr->wword, XmNset, &wword, রিক্ত);
    Xt_Va_Get_মানসমূহ(sfr->mcase, XmNset, &mcase, রিক্ত);
    যদি (wword)
	flags |= FRD_WHOLE_WORD;
    যদি (mcase)
	flags |= FRD_MATCH_CASE;

    (শূন্য)gui_do_findrepl((পূর্ণাঙ্ক)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    যদি (find_text != রিক্ত)
	XtFree(find_text);
    যদি (repl_text != রিক্ত)
	XtFree(repl_text);
}

    স্থির শূন্য
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    চাবি_Sym keysym;

    যদি (frdp == রিক্ত)
	ঘূৰাই;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    যদি (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    স্থির শূন্য
set_label(Widget w, ৱর্ণ *label)
{
    XmString	str;
    char_u	*p, *next;
    চাবি_Sym	mnemonic = NUL;

    যদি (!w)
	ঘূৰাই;

    p = vim_strsave((char_u *)label);
    যদি (p == রিক্ত)
	ঘূৰাই;
    বাবে (next = p; *next; ++next)
    {
	যদি (*next == DLG_HOTKEY_CHAR)
	{
	    পূর্ণাঙ্ক len = STRLEN(next);

	    যদি (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((ৱর্ণ *)p);
    vim_free(p);
    যদি (str)
    {
	Xt_Va_Set_মানসমূহ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		রিক্ত);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    স্থির শূন্য
find_replace_dialog_create(char_u *arg, পূর্ণাঙ্ক do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		প্ৰবেশ_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    পূর্ণাঙ্ক			n;
    Arg			args[6];
    পূর্ণাঙ্ক			wword = FALSE;
    পূর্ণাঙ্ক			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    যদি (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	যদি (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	অন্যথা
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	যদি (entry_text != রিক্ত)
	    XmTextFieldSetString(frdp->what, (ৱর্ণ *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_মানসমূহ(frdp->wword, XmNset, wword, রিক্ত);
	ঘূৰাই;
    }

    // Create a fresh new dialog window
    যদি (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    অন্যথা
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
	    রিক্ত);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_ঠেলো_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    রিক্ত);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    যদি (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ঠেলো_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		রিক্ত);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ঠেলো_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		রিক্ত);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ঠেলো_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		রিক্ত);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ঠেলো_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    রিক্ত);
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

    প্ৰবেশ_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    রিক্ত);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, প্ৰবেশ_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		রিক্ত);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, প্ৰবেশ_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		রিক্ত);

	যদি (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	প্ৰবেশ_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    রিক্ত);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, প্ৰবেশ_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    রিক্ত);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, চাবি_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	অন্যথা
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, চাবি_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_মানসমূহ(label_what, args, n);
	widest = width;
	যদি (do_replace)
	{
	    Xt_Get_মানসমূহ(label_with, args, n);
	    যদি (width > widest)
		widest = width;
	}

	Xt_Va_Set_মানসমূহ(frdp->what, XmNleftOffset, widest, রিক্ত);
	যদি (do_replace)
	    Xt_Va_Set_মানসমূহ(frdp->with, XmNleftOffset, widest, রিক্ত);

    }

    XtManageChild(প্ৰবেশ_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, প্ৰবেশ_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, প্ৰবেশ_form,
		রিক্ত);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		রিক্ত);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)রিক্ত, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		রিক্ত);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		রিক্ত);
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
	    XmNtopWidget, প্ৰবেশ_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    রিক্ত);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    রিক্ত);
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
	    রিক্ত);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    যদি (entry_text != রিক্ত)
	XmTextFieldSetString(frdp->what, (ৱর্ণ *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   শূন্য
gui_mch_find_dialog(exarg_T *eap)
{
    যদি (!gui.in_use)
	ঘূৰাই;

    find_replace_dialog_create(eap->arg, FALSE);
}


    শূন্য
gui_mch_replace_dialog(exarg_T *eap)
{
    যদি (!gui.in_use)
	ঘূৰাই;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    শূন্য
gui_motif_synch_fonts(শূন্য)
{
    SharedFindReplace *frdp;
    পূর্ণাঙ্ক		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    যদি (font == রিক্ত)
	ঘূৰাই;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    বাবে (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	যদি (frdp->dialog)
	{
	    Xt_Va_Set_মানসমূহ(frdp->what, XmNfontList, font_list, রিক্ত);
	    যদি (do_replace)
		Xt_Va_Set_মানসমূহ(frdp->with, XmNfontList, font_list, রিক্ত);
	}
    }

    XmFontListFree(font_list);
}
