/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಅನ್ತರ್ಭಾವ <Xm/Form.h>
#ಅನ್ತರ್ಭಾವ <Xm/RowColumn.h>
#ಅನ್ತರ್ಭಾವ <Xm/PushB.h>
#ಅನ್ತರ್ಭಾವ <Xm/Text.h>
#ಅನ್ತರ್ಭಾವ <Xm/TextF.h>
#ಅನ್ತರ್ಭಾವ <Xm/Separator.h>
#ಅನ್ತರ್ಭಾವ <Xm/Label.h>
#ಅನ್ತರ್ಭಾವ <Xm/CascadeB.h>
#ಅನ್ತರ್ಭಾವ <Xm/ScrollBar.h>
#ಅನ್ತರ್ಭಾವ <Xm/MenuShell.h>
#ಅನ್ತರ್ಭಾವ <Xm/DrawingA.h>
#ಯದಿ (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#ಅನ್ತರ್ಭಾವ <Xm/Frame.h>
#ಅನ್ತರ್ಭಾವ <Xm/LabelG.h>
#ಅನ್ತರ್ಭಾವ <Xm/ToggleBG.h>
#ಅನ್ತರ್ಭಾವ <Xm/SeparatoG.h>
#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>

#ಅನ್ತರ್ಭಾವ <X11/keysym.h>
#ಅನ್ತರ್ಭಾವ <X11/Xatom.h>
#ಅನ್ತರ್ಭಾವ <X11/StringDefs.h>
#ಅನ್ತರ್ಭಾವ <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# ಯದಿ defined(VMS)
#  include <xpm.h>
# ಅನ್ಯಥಾ
#  include <X11/xpm.h>
# endif
#ಅನ್ಯಥಾ
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#ಅನ್ತರ್ಭಾವ "gui_xmebw.h"	// for our Enhanced Button Widget

#ಯದಿ defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

ಬಾಹ್ಯ Widget vimShell;

ಸ್ಥಿರ Widget vimForm;
ಸ್ಥಿರ Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
ಸ್ಥಿರ Widget toolBarFrame;
ಸ್ಥಿರ Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
ಸ್ಥಿರ Widget	tab_ಸಾಲು;
ಸ್ಥಿರ Widget	tab_ಸಾಲು_menu = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# ಯದಿ (XmVersion >= 1002)
// remember the last set value for the tearoff item
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tearoff_val = (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED;
# endif
ಸ್ಥಿರ Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
ಸ್ಥಿರ ಶೂನ್ಯ reset_focus(ಶೂನ್ಯ);
#endif

ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_menu_colors(Widget id);
ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_scroll_colors(Widget id);

#ಯದಿ (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#ಅನ್ಯಥಾ
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಙ್ಕ		dragging;

    sb = gui_find_scrollbar((long)client_data);

    ಮೌಲ್ಯ = ((XmScrollBarCallbackStruct *)call_data)->ಮೌಲ್ಯ;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (ಪೂರ್ಣಾಙ್ಕ)XmCR_DRAG);
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
}

#ifdef FEAT_GUI_TABLINE
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    ಯದಿ (nptr->reason != (ಪೂರ್ಣಾಙ್ಕ)XmCR_NONE)
	send_tabline_event(nptr->page_ಸಂಖ್ಯೆ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNuserData, &cmd, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, XmNuserData, &tab_idx, ರಿಕ್ತ);

    send_tabline_menu_event((ಪೂರ್ಣಾಙ್ಕ)(long)tab_idx, (ಪೂರ್ಣಾಙ್ಕ)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((ಪೂರ್ಣಾಙ್ಕ *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tabline_scroller_clicked(
    ವರ್ಣ		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_ಸಾಲು, scroller_name);
    ಯದಿ (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, ರಿಕ್ತ);
	ಯದಿ (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    ಯದಿ (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    ಪೂರ್ಣಾಙ್ಕ				tab_idx = 0;
    WidgetList			children;
    Cardinal			ಅಂಕೆ_Children;
    ಸ್ಥಿರ XtIntervalId		timer = (XtIntervalId)0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    ಯದಿ (event->button == Button1)
    {
	ಯದಿ (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, XmNuserData, &tab_idx, ರಿಕ್ತ);
	send_tabline_menu_event(tab_idx, (ಪೂರ್ಣಾಙ್ಕ)TABLINE_MENU_CLOSE);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (event->button != Button3)
	ಪ್ರತ್ಯಾಗಮ;

    // When ignoring events don't show the menu.
    ಯದಿ (hold_gui_events || cmdwin_type != 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	ಯದಿ (tab_w != (Widget)0 && Xm_Is_ತಳ್ಳು_Button(tab_w))
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_w, Xm_Npage_ಸಂಖ್ಯೆ, &tab_idx, ರಿಕ್ತ);
    }

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, XmNuserData, (XtPointer)(long)tab_idx, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, XmNchildren, &children, XmNnumChildren,
		  &ಅಂಕೆ_Children, ರಿಕ್ತ);
    XtManageChildren(children, ಅಂಕೆ_Children);
    XmMenuPosition(tab_ಸಾಲು_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_ಸಾಲು_menu);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_balloon_cb(BalloonEval *beval, ಪೂರ್ಣಾಙ್ಕ state UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		nr;
    tabpage_T	*tp;

    ಯದಿ (beval->target == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(beval->target, Xm_Npage_ಸಂಖ್ಯೆ, &nr, ರಿಕ್ತ);
    tp = find_tabpage(nr);
    ಯದಿ (tp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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

#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>
#ಅನ್ತರ್ಭಾವ <Xm/LabelP.h>

ಸ್ಥಿರ XtExposeProc old_label_expose = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned ವರ್ಣ   label_type = (ಪೂರ್ಣಾಙ್ಕ)XmSTRING;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(_w, XmNlabelType, &label_type, (XtPointer)0);

    ಯದಿ (XtIsSensitive(_w) || label_type != (ಪೂರ್ಣಾಙ್ಕ)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    ಅನ್ಯಥಾ
    {
	XGCValues   ಮೌಲ್ಯಗಳು;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	ಮೌಲ್ಯಗಳು.graphics_exposures = False;

	ಯದಿ (fs != 0)
	{
	    mask |= GCFont;
	    ಮೌಲ್ಯಗಳು.font = fs->fid;
	}

	ಯದಿ (lw->primitive.ತುದಿ_shadow_pixmap != None
		&& lw->primitive.ತುದಿ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    ಮೌಲ್ಯಗಳು.fill_style = FillTiled;
	    ಮೌಲ್ಯಗಳು.tile = lw->primitive.ತುದಿ_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	ಯದಿ (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	ಮೌಲ್ಯಗಳು.foreground = lw->primitive.ತುದಿ_shadow_color;
	ಮೌಲ್ಯಗಳು.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &ಮೌಲ್ಯಗಳು, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	ಯದಿ (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	ಮೌಲ್ಯಗಳು.foreground = lw->primitive.bottom_shadow_color;
	ಮೌಲ್ಯಗಳು.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &ಮೌಲ್ಯಗಳು, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    ಶೂನ್ಯ
gui_x11_create_widgets(ಶೂನ್ಯ)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    ಪೂರ್ಣಾಙ್ಕ		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    ಯದಿ (old_label_expose == ರಿಕ್ತ)
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
#ಯದಿ (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(vimShell, XmNdeleteResponse, XmDO_NOTHING, ರಿಕ್ತ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ರಿಕ್ತ);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	ಪೂರ್ಣಾಙ್ಕ ac = 0;

# ಯದಿ (XmVersion >= 1002)
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
	ರಿಕ್ತ);
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
	ರಿಕ್ತ);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_ಸಂಖ್ಯೆ, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_ತುದಿ_ಬಲ); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_ಸಾಲು = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_ಸಾಲು, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			ರಿಕ್ತ);
    XtAddEventHandler(tab_ಸಾಲು, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, ರಿಕ್ತ);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_ಸಾಲು, "MinorTabScrollerNext");
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ರಿಕ್ತ);
    scroller = XtNameToWidget(tab_ಸಾಲು, "MinorTabScrollerPrevious");
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ರಿಕ್ತ);

    // Create the tabline popup menu
    tab_ಸಾಲು_menu = XmCreatePopupMenu(tab_ಸಾಲು, "tabline popup", ರಿಕ್ತ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((ವರ್ಣ *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ರಿಕ್ತ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((ವರ್ಣ *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ರಿಕ್ತ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((ವರ್ಣ *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ರಿಕ್ತ);
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
	ರಿಕ್ತ);
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
	ರಿಕ್ತ);

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
    ಶೂನ್ಯ
gui_x11_destroy_widgets(ಶೂನ್ಯ)
{
    textArea = ರಿಕ್ತ;
#ifdef FEAT_MENU
    menuBar = ರಿಕ್ತ;
#endif
}

    ಶೂನ್ಯ
gui_mch_set_text_area_pos(
    ಪೂರ್ಣಾಙ್ಕ	    x UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    y UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    w UNUSED,
    ಪೂರ್ಣಾಙ್ಕ	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    ಶೂನ್ಯ
gui_x11_set_back_color(ಶೂನ್ಯ)
{
    ಯದಿ (textArea != ರಿಕ್ತ)
#ಯದಿ (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#ಅನ್ಯಥಾ
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(textArea,
		  XmNbackground, gui.back_pixel,
		  ರಿಕ್ತ);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    ಶೂನ್ಯ
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window ಬೇರು, child;
    unsigned ಪೂರ್ಣಾಙ್ಕ mask;
    unsigned ಪೂರ್ಣಾಙ್ಕ width, height, border_width, depth;
    ಪೂರ್ಣಾಙ್ಕ x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, &mappedWhenManaged, ರಿಕ್ತ);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, False, ರಿಕ್ತ);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &ಬೇರು, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &ಬೇರು, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    ಯದಿ (x < 0)
	x = 0;
    ಯದಿ (x > maxX)
	x = maxX;
    ಯದಿ (y < 0)
	y = 0;
    ಯದಿ (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(shell, XmNx, x, XmNy, y, ರಿಕ್ತ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, mappedWhenManaged, ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# ಯದಿ (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# ಅನ್ಯಥಾ
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(ರಿಕ್ತ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    ಪ್ರತ್ಯಾಗಮ font_list;
}

# ಯದಿ ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(ರಿಕ್ತ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    ಪ್ರತ್ಯಾಗಮ font_list;
}
# endif

#endif

#ಯದಿ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_add_actext(vimmenu_T *menu);
# ಯದಿ (XmVersion >= 1002)
ಸ್ಥಿರ ಶೂನ್ಯ toggle_tearoff(Widget wid);
ಸ್ಥಿರ ಶೂನ್ಯ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ submenu_change(vimmenu_T *mp, ಪೂರ್ಣಾಙ್ಕ colors);

ಸ್ಥಿರ ಶೂನ್ಯ do_set_mnemonics(ಪೂರ್ಣಾಙ್ಕ enable);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ menu_enabled = TRUE;

    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	ಯದಿ (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ರಿಕ್ತ);
#  ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
#  endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ರಿಕ್ತ);
	}
    }
    ಅನ್ಯಥಾ
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	ಯದಿ (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);
#  ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
#  endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM,
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ರಿಕ್ತ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_motif_set_mnemonics(ಪೂರ್ಣಾಙ್ಕ enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    ಯದಿ (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_set_mnemonics(ಪೂರ್ಣಾಙ್ಕ enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	ಯದಿ (menu->id != (Widget)0)
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ರಿಕ್ತ);
}

    ಶೂನ್ಯ
gui_mch_add_menu(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    ಯದಿ (menu_is_popup(menu->name))
    {
	Arg arg[2];
	ಪೂರ್ಣಾಙ್ಕ n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  ಯದಿ (XmVersion <= 1002)
	ಯದಿ (mouse_model_popup())
#  endif
	{
	    ಯದಿ (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    ಯದಿ (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

    ಯದಿ (!menu_is_menubar(menu->name)
	    || (parent != ರಿಕ್ತ && parent->submenu_id == (Widget)0))
	ಪ್ರತ್ಯಾಗಮ;

    label = XmStringCreate((ವರ್ಣ *)menu->dname, STRING_TAG);
    ಯದಿ (label == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == ರಿಕ್ತ) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# ಯದಿ (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != ರಿಕ್ತ
			   && tearoff_val == (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ರಿಕ್ತ);
    XmStringFree(label);

    ಯದಿ (menu->id == (Widget)0)		// failed
	ಪ್ರತ್ಯಾಗಮ;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    ಯದಿ (parent == ರಿಕ್ತ && menu->priority >= 9999)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(menuBar,
		XmNmenuHelpWidget, menu->id,
		ರಿಕ್ತ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ರಿಕ್ತ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	ರಿಕ್ತ);
    gui_motif_menu_colors(menu->submenu_id);

    ಯದಿ (menu->submenu_id == (Widget)0)		// failed
	ಪ್ರತ್ಯಾಗಮ;

# ಯದಿ (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ರಿಕ್ತ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    ಯದಿ (parent == ರಿಕ್ತ)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    ಯದಿ (menu->actext == ರಿಕ್ತ || menu->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    label = XmStringCreate((ವರ್ಣ *)menu->actext, STRING_TAG);
    ಯದಿ (label == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(menu->id, XmNacceleratorText, label, ರಿಕ್ತ);
    XmStringFree(label);
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable)
{
# ಯದಿ (XmVersion >= 1002)
    ಯದಿ (enable)
	tearoff_val = (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED;
    ಅನ್ಯಥಾ
	tearoff_val = (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ಬೇರು_menu);
# endif
}

# ಯದಿ (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(wid, XmNtearOffModel, tearoff_val, ರಿಕ್ತ);
    ಯದಿ (tearoff_val == (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    ಯಾವತ್ (menu != ರಿಕ್ತ)
    {
	ಯದಿ (!menu_is_popup(menu->name))
	{
	    ಯದಿ (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_text_area_extra_height(ಶೂನ್ಯ)
{
    Dimension	shadowHeight;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(textAreaForm, XmNshadowThickness, &shadowHeight, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    ಶೂನ್ಯ
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ಸ್ಥಿರ Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    ಯದಿ (id != (Widget)0)
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(id, XmNheight, &height, ರಿಕ್ತ);

    // Find any menu Widget, to be able to call XtManageChild()
    ಅನ್ಯಥಾ
	FOR_ALL_MENUS(mp)
	    ಯದಿ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ಭಙ್ಗ;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    ಯದಿ (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	ಯದಿ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(mp->id, XmNy, &y, ರಿಕ್ತ);
	    ಯದಿ (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	ರಿಕ್ತ);

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
    ಯದಿ (gui.shell_created)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(menuBar, XmNheight, gui.menu_height, ರಿಕ್ತ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ಸ್ಥಿರ ವರ್ಣ **get_toolbar_pixmap(vimmenu_T *menu, ವರ್ಣ **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    ಪೂರ್ಣಾಙ್ಕ		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    ಬೇರು_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (ವರ್ಣ *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    ಯದಿ (status == XpmSuccess)
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ಸ್ಥಿರ ವರ್ಣ **
get_toolbar_pixmap(vimmenu_T *menu, ವರ್ಣ **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    ವರ್ಣ	**xpm = ರಿಕ್ತ;		// xpm array
    ಪೂರ್ಣಾಙ್ಕ		res;

    *fname = ರಿಕ್ತ;
    buf[0] = NUL;			// start with NULL path

    ಯದಿ (menu->iconfile != ರಿಕ್ತ)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	ಯದಿ (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	ಯದಿ (res == OK)
	{
	    *fname = (ವರ್ಣ *)vim_strsave(buf);
	    ಪ್ರತ್ಯಾಗಮ tb_blank_xpm;
	}
    }

    ಯದಿ (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	ಯದಿ (menu->iconidx >= 0 && menu->iconidx
	       < (ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(built_in_pixmaps) / ಪರಿಮಾಣ(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	ಅನ್ಯಥಾ
	    xpm = tb_blank_xpm;
    }

    ಪ್ರತ್ಯಾಗಮ xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
add_pixmap_args(vimmenu_T *menu, Arg *args, ಪೂರ್ಣಾಙ್ಕ n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    ಯದಿ (menu->xpm == ರಿಕ್ತ)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (menu->xpm_fname != ರಿಕ್ತ)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    ಪ್ರತ್ಯಾಗಮ n;
}
# endif // FEAT_TOOLBAR

    ಶೂನ್ಯ
gui_mch_add_menu_item(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# ಯದಿ (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    ಯದಿ (menu_is_child_of_popup(menu) && !mouse_model_popup())
	ಪ್ರತ್ಯಾಗಮ;
# endif

# ifdef FEAT_TOOLBAR
    ಯದಿ (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = ರಿಕ್ತ;    // fallback label if pixmap not found
	ಪೂರ್ಣಾಙ್ಕ		n;
	Arg		args[18];

	n = 0;
	ಯದಿ (menu_is_separator(menu->name))
	{
	    ವರ್ಣ	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (ವರ್ಣ *)vim_strchr(menu->name, ':');
	    ಯದಿ (cp != ರಿಕ್ತ)
		wid = (Dimension)atoi(++cp);
	    ಅನ್ಯಥಾ
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	ಅನ್ಯಥಾ
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
	    xms = XmStringCreate((ವರ್ಣ *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	ಯದಿ (menu->id == ರಿಕ್ತ)
	{
	    menu->id = XtCreateManagedWidget((ವರ್ಣ *)menu->dname,
			type, toolBar, args, n);
	    ಯದಿ (menu->id != ರಿಕ್ತ && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	ಅನ್ಯಥಾ
	    Xt_Set_ಮೌಲ್ಯಗಳು(menu->id, args, n);
	ಯದಿ (xms != ರಿಕ್ತ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = ರಿಕ್ತ;
	// When adding first item to toolbar it might have to be enabled .
	ಯದಿ (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ರಿಕ್ತ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	ಪ್ರತ್ಯಾಗಮ;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    ಯದಿ (parent->submenu_id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    ಯದಿ (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# ಯದಿ (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ರಿಕ್ತ);
	gui_motif_menu_colors(menu->id);
	ಪ್ರತ್ಯಾಗಮ;
    }

    label = XmStringCreate((ವರ್ಣ *)menu->dname, STRING_TAG);
    ಯದಿ (label == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ತಳ್ಳು_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# ಯದಿ (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (ಪೂರ್ಣಾಙ್ಕ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ರಿಕ್ತ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    ಯದಿ (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# ಯದಿ (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    ಶೂನ್ಯ
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;

    // When GUI hasn't started the menus have not been created.
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

    ಯಾವತ್ (menu)
    {
      ಯದಿ (menu->children != ರಿಕ್ತ)
      {
	  ಯದಿ (menu_is_popup(menu->name))
	  {
	      ಯದಿ (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      ಅನ್ಯಥಾ
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      ಅನ್ಯಥಾ ಯದಿ (menu_is_child_of_popup(menu))
      {
	  ಯದಿ (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  ಅನ್ಯಥಾ
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    ಶೂನ್ಯ
gui_mch_new_menu_colors(ಶೂನ್ಯ)
{
    ಯದಿ (menuBar == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ಬೇರು_menu, TRUE);
}

    ಶೂನ್ಯ
gui_mch_new_menu_font(ಶೂನ್ಯ)
{
    ಯದಿ (menuBar == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;
    submenu_change(ಬೇರು_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(menuBar, XmNheight, &height, ರಿಕ್ತ);
	gui.menu_height = height;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNwidth, &w, XtNheight, &h, ರಿಕ್ತ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# ಯದಿ defined(FEAT_BEVAL_GUI)
    ಶೂನ್ಯ
gui_mch_new_tooltip_font(ಶೂನ್ಯ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    ಯದಿ (toolBar == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    menu = gui_find_menu((char_u *)"ToolBar");
    ಯದಿ (menu != ರಿಕ್ತ)
	submenu_change(menu, FALSE);
#  endif
}

    ಶೂನ್ಯ
gui_mch_new_tooltip_colors(ಶೂನ್ಯ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    ಯದಿ (toolBar == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    ಯದಿ (toolbar != ರಿಕ್ತ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
submenu_change(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಙ್ಕ		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    ಪ್ರತಿ (mp = menu; mp != ರಿಕ್ತ; mp = mp->next)
    {
	ಯದಿ (mp->id != (Widget)0)
	{
	    ಯದಿ (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		ಯದಿ (mp->xpm != ರಿಕ್ತ)
		{
		    ಪೂರ್ಣಾಙ್ಕ		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_ಮೌಲ್ಯಗಳು(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ಯದಿ (mp->tip != ರಿಕ್ತ)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].ಮೌಲ್ಯ = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].ಮೌಲ್ಯ = gui.tooltip_fg_pixel;
		    Xt_Set_ಮೌಲ್ಯಗಳು(mp->tip->balloonLabel, &args[0], Xt_ಸಂಖ್ಯೆ(args));
		}
#  endif
# endif
	    }
	    ಅನ್ಯಥಾ
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ಯದಿ (mp->tip != ರಿಕ್ತ)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].ಮೌಲ್ಯ = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_ಮೌಲ್ಯಗಳು(mp->tip->balloonLabel, &args[0], Xt_ಸಂಖ್ಯೆ(args));
		}
# endif
	    }
	}

	ಯದಿ (mp->children != ರಿಕ್ತ)
	{
# ಯದಿ (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    ಯದಿ (mp->submenu_id != (Widget)0)
	    {
		ಯದಿ (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    ಯದಿ (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    ಯದಿ (menu->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    Widget	    parent;

    parent = XtParent(menu->id);
# ಯದಿ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    ಯದಿ (parent == toolBar && menu->tip != ರಿಕ್ತ)
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
	menu->tip = ರಿಕ್ತ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    ಯದಿ (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    ಅನ್ಯಥಾ ಯದಿ (parent == toolBar)
    {
	Cardinal    ಅಂಕೆ_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNnumChildren, &ಅಂಕೆ_children, ರಿಕ್ತ);
	ಯದಿ (ಅಂಕೆ_children == 0)
	    gui_mch_show_toolbar(FALSE);
	ಅನ್ಯಥಾ
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    ಶೂನ್ಯ
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
    ಶೂನ್ಯ
gui_mch_def_colors(ಶೂನ್ಯ)
{
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    ಯದಿ (sb->id != (Widget)0)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ರಿಕ್ತ);
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಙ್ಕ		x,
    ಪೂರ್ಣಾಙ್ಕ		y,
    ಪೂರ್ಣಾಙ್ಕ		w,
    ಪೂರ್ಣಾಙ್ಕ		h)
{
    ಯದಿ (sb->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (sb->type == SBAR_ಎಡ || sb->type == SBAR_ಬಲ)
    {
	ಯದಿ (y == 0)
	    h -= gui.border_offset;
	ಅನ್ಯಥಾ
	    y -= gui.border_offset;
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_ಬಲ]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		ರಿಕ್ತ);
    XtManageChild(sb->id);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(textArea, XtNwidth, &tw, XtNx, &tx, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNwidth, &ww, ರಿಕ್ತ);
    xpad = ww - tw - tx - gui.scrollbar_width;
    ಪ್ರತ್ಯಾಗಮ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(textArea, XtNheight, &th, XtNy, &ty, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNheight, &wh, ರಿಕ್ತ);
    ypad = wh - th - ty - gui.scrollbar_height;
    ಪ್ರತ್ಯಾಗಮ (ypad < 0) ? 0 : ypad;
}

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ flag)
{
    Arg		args[16];
    ಪೂರ್ಣಾಙ್ಕ		n;

    ಯದಿ (sb->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    n = 0;
    ಯದಿ (flag)
    {
	ವಿಕಲ್ಪ (sb->type)
	{
	    ಪಕ್ಷ SBAR_ಎಡ:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SBAR_ಬಲ:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		ಭಙ್ಗ;

	    ಪಕ್ಷ SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		ಭಙ್ಗ;
	}
	Xt_Set_ಮೌಲ್ಯಗಳು(textArea, args, n);
	XtManageChild(sb->id);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ವಿಕಲ್ಪ (sb->type)
	    {
		ಪಕ್ಷ SBAR_ಎಡ:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    ಭಙ್ಗ;

		ಪಕ್ಷ SBAR_ಬಲ:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    ಭಙ್ಗ;

		ಪಕ್ಷ SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    ಭಙ್ಗ;
	    }
	    Xt_Set_ಮೌಲ್ಯಗಳು(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಙ್ಕ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    ಪೂರ್ಣಾಙ್ಕ		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ವಿಕಲ್ಪ (sb->type)
    {
	ಪಕ್ಷ SBAR_ಎಡ:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ಭಙ್ಗ;

	ಪಕ್ಷ SBAR_ಬಲ:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ಭಙ್ಗ;

	ಪಕ್ಷ SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    ಭಙ್ಗ;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    ಯದಿ (sb->id == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, ಕೀಲಿ_Press_Mask, FALSE, gui_x11_ಕೀಲಿ_hit_cb,
	    (XtPointer)0);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಯದಿ (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    ಯದಿ (sb->id != (Widget)0)
    {
	ಯದಿ (gui.scroll_bg_pixel != INVALCOLOR)
	{
#ಯದಿ (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ರಿಕ್ತ);
	}

	ಯದಿ (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ರಿಕ್ತ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    ಯದಿ (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_mnemonic(Widget w, unsigned ಪೂರ್ಣಾಙ್ಕ keycode)
{
    WidgetList	    children;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ_Children, i;
    Boolean	    isMenu;
    ಕೀಲಿ_Sym	    mnemonic = '\0';
    ವರ್ಣ	    mneString[2];
    Widget	    userData;
    unsigned ವರ್ಣ   rowColType;

    ಯದಿ (XtIsComposite(w))
    {
	ಯದಿ (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNrowColumnType, &rowColType, ರಿಕ್ತ);
	    isMenu = (rowColType != (unsigned ವರ್ಣ)XmWORK_AREA);
	}
	ಅನ್ಯಥಾ
	    isMenu = False;
	ಯದಿ (!isMenu)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNchildren, &children, XmNnumChildren,
			  &ಅಂಕೆ_Children, ರಿಕ್ತ);
	    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    ಅನ್ಯಥಾ
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNmnemonic, &mnemonic, ರಿಕ್ತ);
	ಯದಿ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    ಯದಿ (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		ಯದಿ (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNuserData, &userData, ರಿಕ್ತ);
		    ಯದಿ (userData != ರಿಕ್ತ && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		ಅನ್ಯಥಾ
		{
		    XKeyPressedEvent ಕೀಲಿ_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(ಕೀಲಿ_Event);
		    ಕೀಲಿ_Event.type = ಕೀಲಿ_Press;
		    ಕೀಲಿ_Event.serial = 1;
		    ಕೀಲಿ_Event.send_event = True;
		    ಕೀಲಿ_Event.display = XtDisplay(w);
		    ಕೀಲಿ_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & ಕೀಲಿ_Event,
								     ರಿಕ್ತ, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
add_mnemonic_grabs(Widget dialog, Widget w)
{
    ವರ್ಣ	    mneString[2];
    WidgetList	    children;
    ಪೂರ್ಣಾಙ್ಕ		    ಅಂಕೆ_Children, i;
    Boolean	    isMenu;
    ಕೀಲಿ_Sym	    mnemonic = '\0';
    unsigned ವರ್ಣ   rowColType;

    ಯದಿ (XtIsComposite(w))
    {
	ಯದಿ (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNrowColumnType, &rowColType, ರಿಕ್ತ);
	    isMenu = (rowColType != (unsigned ವರ್ಣ)XmWORK_AREA);
	}
	ಅನ್ಯಥಾ
	    isMenu = False;
	ಯದಿ (!isMenu)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNchildren, &children, XmNnumChildren,
							  &ಅಂಕೆ_Children, ರಿಕ್ತ);
	    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    ಅನ್ಯಥಾ
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNmnemonic, &mnemonic, ರಿಕ್ತ);
	ಯದಿ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_ಕೀಲಿ(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    ಸ್ಥಿರ ಶೂನ್ಯ
activate_dialog_mnemonics(Widget dialog)
{
    ಯದಿ (!dialog)
	ಪ್ರತ್ಯಾಗಮ;

    XtAddEventHandler(dialog, ಕೀಲಿ_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ರಿಕ್ತ);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suppress_dialog_mnemonics(Widget dialog)
{
    ಯದಿ (!dialog)
	ಪ್ರತ್ಯಾಗಮ;

    Xt_Ungrab_ಕೀಲಿ(dialog, Any_ಕೀಲಿ, Mod1Mask);
    XtRemoveEventHandler(dialog, ಕೀಲಿ_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ರಿಕ್ತ);
}

#ಯದಿ defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    ಯದಿ (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	ಯದಿ (fl != ರಿಕ್ತ)
	{
	    ಯದಿ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ಅನ್ಯಥಾ
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
	    XmFontListFree(fl);
	}
    }
# ಅನ್ಯಥಾ
    ಯದಿ (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	ಯದಿ (fl != ರಿಕ್ತ)
	{
	    ಯದಿ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ಅನ್ಯಥಾ
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#ಯದಿ defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ dialog_callback_arg
{
    ವರ್ಣ *  args;   // not used right now
    ಪೂರ್ಣಾಙ್ಕ	    id;
} dcbarg_T;

ಸ್ಥಿರ Widget dialog_wgt;
ಸ್ಥಿರ ವರ್ಣ *browse_fname = ರಿಕ್ತ;
ಸ್ಥಿರ XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ಸ್ಥಿರ ಶೂನ್ಯ DialogCancelCB(Widget, XtPointer, XtPointer);
ಸ್ಥಿರ ಶೂನ್ಯ DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_predefined_label(Widget parent, String name, ವರ್ಣ *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    ಕೀಲಿ_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    ಯದಿ (!w)
	ಪ್ರತ್ಯಾಗಮ;

    p = vim_strsave((char_u *)new_label);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (next = p; *next; ++next)
    {
	ಯದಿ (*next == DLG_HOTKEY_CHAR)
	{
	    ಪೂರ್ಣಾಙ್ಕ len = STRLEN(next);

	    ಯದಿ (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((ವರ್ಣ *)p, STRING_TAG);
    vim_free(p);

    ಯದಿ (str != ರಿಕ್ತ)
    {
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ರಿಕ್ತ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    ಯದಿ (!w)
	ಪ್ರತ್ಯಾಗಮ;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    ಪೂರ್ಣಾಙ್ಕ		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = ರಿಕ್ತ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ರಿಕ್ತ, 0);

    ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    ಯದಿ (dflt == ರಿಕ್ತ)
	dflt = (char_u *)"";
    ಅನ್ಯಥಾ ಯದಿ (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    ಯದಿ (filter == ರಿಕ್ತ)
	pattern = (char_u *)"";
    ಅನ್ಯಥಾ
    {
	char_u	*s, *p;

	s = filter;
	ಪ್ರತಿ (p = filter; *p != NUL; ++p)
	{
	    ಯದಿ (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    ಯದಿ (*p == ';' || *p == '\n')	// end of (first) pattern
		ಭಙ್ಗ;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	ಯದಿ (pattern == ರಿಕ್ತ)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (ವರ್ಣ *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (ವರ್ಣ *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (ವರ್ಣ *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (ವರ್ಣ *)title, STRLEN(title) + 1,
	ರಿಕ್ತ);

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
    ಯದಿ (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ರಿಕ್ತ);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned ವರ್ಣ)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    ಕುರು
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_ಒಳಹರಿವು_Mask)XtIMAll);
    } ಯಾವತ್ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    ಯದಿ (browse_fname == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    ಯದಿ (browse_fname != ರಿಕ್ತ)
    {
	XtFree(browse_fname);
	browse_fname = ರಿಕ್ತ;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    ಯದಿ (browse_fname != ರಿಕ್ತ)
    {
	XtFree(browse_fname);
	browse_fname = ರಿಕ್ತ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->ಮೌಲ್ಯ, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#ಯದಿ defined(FEAT_GUI_DIALOG)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    ವರ್ಣ	buf[2];
    ಕೀಲಿ_Sym	ಕೀಲಿ_sym;

    ಯದಿ (XLookupString(&(event->xkey), buf, 2, &ಕೀಲಿ_sym, ರಿಕ್ತ) == 1)
    {
	ಯದಿ (*buf == CAR)
	    dialogStatus = 1;
	ಅನ್ಯಥಾ ಯದಿ (*buf == ESC)
	    dialogStatus = 2;
    }
    ಯದಿ ((ಕೀಲಿ_sym == XK_ಎಡ || ಕೀಲಿ_sym == XK_ಬಲ)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (ಪೂರ್ಣಾಙ್ಕ)(long)client_data + 1;
}

# ifdef HAVE_XPM

    ಸ್ಥಿರ Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    ವರ್ಣ	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    ಪೂರ್ಣಾಙ್ಕ			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_ಚಿಹ್ನೆ	color[5] =
    {
	{"none", ರಿಕ್ತ, 0},
	{"iconColor1", ರಿಕ್ತ, 0},
	{"bottomShadowColor", ರಿಕ್ತ, 0},
	{"topShadowColor", ರಿಕ್ತ, 0},
	{"selectColor", ರಿಕ್ತ, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ರಿಕ್ತ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, ಬೇರು_Window_Of_Screen(scr),
		    data, &pixmap, ರಿಕ್ತ, &attr);

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(label, XmNrecomputeSize, &rs, ರಿಕ್ತ);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(label, XmNrecomputeSize, True, ರಿಕ್ತ);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ರಿಕ್ತ);
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(label, XmNrecomputeSize, rs, ರಿಕ್ತ);

    ಪ್ರತ್ಯಾಗಮ label;
}
# endif

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(
    ಪೂರ್ಣಾಙ್ಕ		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    ಪೂರ್ಣಾಙ್ಕ		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    ಪೂರ್ಣಾಙ್ಕ		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    ಪೂರ್ಣಾಙ್ಕ			butcount;
    Widget		w;
    Widget		dialogform = ರಿಕ್ತ;
    Widget		form = ರಿಕ್ತ;
    Widget		dialogtextfield = ರಿಕ್ತ;
    Widget		*buttons;
    Widget		sep_form = ರಿಕ್ತ;
    Boolean		vertical;
    Widget		separator = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			n;
    Arg			args[6];
# ifdef HAVE_XPM
    ವರ್ಣ		**icon_data = ರಿಕ್ತ;
    Widget		dialogpixmap = ರಿಕ್ತ;
# endif

    ಯದಿ (title == ರಿಕ್ತ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (ವರ್ಣ *)"dialog", ರಿಕ್ತ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ರಿಕ್ತ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((ವರ್ಣ *)title);
    ಯದಿ (label == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ರಿಕ್ತ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    ಯದಿ (buts == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    ಪ್ರತಿ (p = buts; *p; ++p)
	ಯದಿ (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    ಯದಿ (buttons == ರಿಕ್ತ)
    {
	vim_free(buts);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    ಪ್ರತಿ (butcount = 0; *p; ++butcount)
    {
	ಕೀಲಿ_Sym mnemonic = NUL;

	ಪ್ರತಿ (next = p; *next; ++next)
	{
	    ಯದಿ (*next == DLG_HOTKEY_CHAR)
	    {
		ಪೂರ್ಣಾಙ್ಕ len = STRLEN(next);

		ಯದಿ (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    ಯದಿ (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		ಭಙ್ಗ;
	    }
	}
	label = XmStringCreate(_((ವರ್ಣ *)p), STRING_TAG);
	ಯದಿ (label == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ತಳ್ಳು_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ರಿಕ್ತ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	ಯದಿ (butcount > 0)
	{
	    ಯದಿ (vertical)
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (*next == NUL)
		{
		    Xt_Va_Set_ಮೌಲ್ಯಗಳು(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    ರಿಕ್ತ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    ರಿಕ್ತ);
		    XtManageChild(sep_form);
		}
		ಅನ್ಯಥಾ
		{
		    Xt_Va_Set_ಮೌಲ್ಯಗಳು(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ರಿಕ್ತ);
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (!vertical)
	{
	    ಯದಿ (*next == NUL)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			ರಿಕ್ತ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			ರಿಕ್ತ);
		XtManageChild(sep_form);
	    }
	    ಅನ್ಯಥಾ
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			ರಿಕ್ತ);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    ಯದಿ (butcount > 0)
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

    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);
	ಯದಿ (butcount > 0)
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ರಿಕ್ತ);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (ವರ್ಣ *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, ಕೀಲಿ_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)ರಿಕ್ತ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ರಿಕ್ತ);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ VIM_GENERIC:
	    icon_data = generic_xpm;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_ERROR:
	    icon_data = error_xpm;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_WARNING:
	    icon_data = alert_xpm;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_INFO:
	    icon_data = info_xpm;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VIM_QUESTION:
	    icon_data = quest_xpm;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
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
    label = XmStringCreateLtoR((ವರ್ಣ *)message, STRING_TAG);
    ಯದಿ (label == ರಿಕ್ತ)
    {
	vim_free(buttons);
	ಪ್ರತ್ಯಾಗಮ -1;
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
# ಅನ್ಯಥಾ
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				ರಿಕ್ತ);
    XmStringFree(label);
    set_fontlist(w);

    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (butcount > 0)
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ರಿಕ್ತ);
    }

    ಯದಿ (dfltbutton < 1)
	dfltbutton = 1;
    ಯದಿ (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ರಿಕ್ತ);
    ಯದಿ (textfield != ರಿಕ್ತ)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogform, XmNinitialFocus, dialogtextfield, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ರಿಕ್ತ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    ಯದಿ (textfield != ರಿಕ್ತ && *textfield != NUL)
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
    ಪ್ರತಿ (;;)
    {
	XtAppProcessEvent(app, (Xt_ಒಳಹರಿವು_Mask)XtIMAll);
	ಯದಿ (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    ಭಙ್ಗ;
    }

    vim_free(buttons);

    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	ಯದಿ (p == ರಿಕ್ತ || dialogStatus < 0)
	    *textfield = NUL;
	ಅನ್ಯಥಾ
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((ವರ್ಣ *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    ಪ್ರತ್ಯಾಗಮ dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#ಯದಿ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಙ್ಕ showit)
{
    Cardinal	ಅಂಕೆ_Children;	    // how many children toolBar has

    ಯದಿ (toolBar == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNnumChildren, &ಅಂಕೆ_Children, ರಿಕ್ತ);
    ಯದಿ (showit && ಅಂಕೆ_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNchildren, &children, ರಿಕ್ತ);
	{
	    ಶೂನ್ಯ    (*action)(BalloonEval *);
	    ಪೂರ್ಣಾಙ್ಕ	    text = 0;

	    ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    ಅನ್ಯಥಾ
		action = &gui_mch_disable_beval_area;
	    ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *)p_toolbar, "text"))
		text = 1;
	    ಅನ್ಯಥಾ ಯದಿ (strstr((ಧ್ರುವ ವರ್ಣ *)p_toolbar, "icons"))
		text = -1;
	    ಯದಿ (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    ಯದಿ (menu_is_toolbar(toolbar->dname))
			ಭಙ್ಗ;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		ಯದಿ (toolbar)
		{
		    ಪ್ರತಿ (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			ಪೂರ್ಣಾಙ್ಕ	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			ಯದಿ (cur->tip != ರಿಕ್ತ)
			    (*action)(cur->tip);
			ಯದಿ (!menu_is_separator(cur->name))
			{
			    ಯದಿ (text == 1 || cur->xpm == ರಿಕ್ತ)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    ಯದಿ (cur->id != ರಿಕ್ತ)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_ಮೌಲ್ಯಗಳು(cur->id, args, n);
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
	ಯದಿ (showing_tabline)
	{
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ರಿಕ್ತ);
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ಸಾಲು,
			  ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
# endif
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ರಿಕ್ತ);
	ಯದಿ (XtIsManaged(menuBar))
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	gui.toolbar_height = 0;
	ಯದಿ (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	{
# ifdef FEAT_GUI_TABLINE
	    ಯದಿ (showing_tabline)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM,
			      ರಿಕ್ತ);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ರಿಕ್ತ);
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
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_focus(ಶೂನ್ಯ)
{
    ಯದಿ (textArea != ರಿಕ್ತ)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_compute_toolbar_height(ಶೂನ್ಯ)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	ಅಂಕೆ_Children;	    // how many children toolBar has
    ಪೂರ್ಣಾಙ್ಕ		i;

    borders = 0;
    height = 0;
    ಯದಿ (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		ರಿಕ್ತ);
	borders += fst + fmh;
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &ಅಂಕೆ_Children, ರಿಕ್ತ);
	borders += tst + tmh;
	ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ಅಂಕೆ_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(children[i], XmNheight, &whgt, ರಿಕ್ತ);
	    ಯದಿ (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    ಯದಿ (height < 10)
	height = 24;
# endif

    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(height + (borders << 1));
}

    ಶೂನ್ಯ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    ರಿಕ್ತ);
}
#endif

#ಯದಿ defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (tab_ಸಾಲು == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (!showit != !showing_tabline)
    {
	ಯದಿ (showit)
	{
	    XtManageChild(tab_ಸಾಲು);
	    XtUnmanageChild(XtNameToWidget(tab_ಸಾಲು, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_ಸಾಲು, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_ಸಾಲು,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ಯದಿ (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
#  endif
		ಯದಿ (XtIsManaged(menuBar))
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM, ರಿಕ್ತ);
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ಸಾಲು,
			  ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	{
	    XtUnmanageChild(tab_ಸಾಲು);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ಯದಿ (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
#  endif
		ಯದಿ (XtIsManaged(menuBar))
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ರಿಕ್ತ);
	    ಅನ್ಯಥಾ
# endif
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, ರಿಕ್ತ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ tab_ಸಾಲು != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    tabpage_T		*tp;
    ಪೂರ್ಣಾಙ್ಕ			nr = 1, n;
    Arg			args[10];
    ಪೂರ್ಣಾಙ್ಕ			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    ಪೂರ್ಣಾಙ್ಕ			last_page, tab_ಎಣಿಕೆ;
    XmString		label_str;
    ವರ್ಣ		*label_cstr;
    BalloonEval		*beval;

    ಯದಿ (tab_ಸಾಲು == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    // Add a label for each tab page.  They all contain the same text area.
    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = tp->tp_next, ++nr)
    {
	ಯದಿ (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_ಸಾಲು, nr, &page_info);
	ಯದಿ (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, ರಿಕ್ತ, tabline_balloon_cb,
									ರಿಕ್ತ);
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab, XmNuserData, beval, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab, Xm_Npage_ಸಂಖ್ಯೆ, nr, ರಿಕ್ತ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab, XmNlabelString, &label_str, ರಿಕ್ತ);
	ಯದಿ (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    ಯದಿ (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ರಿಕ್ತ);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_ಎಣಿಕೆ = nr - 1;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Nlast_Page_ಸಂಖ್ಯೆ, &last_page, ರಿಕ್ತ);

    // Remove any old labels.
    ಯಾವತ್ (nr <= last_page)
    {
	ಯದಿ (XmNotebookGetPageInfo(tab_ಸಾಲು, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_ಸಂಖ್ಯೆ == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(page_info.major_tab_widget, XmNuserData, &beval, ರಿಕ್ತ);
	    ಯದಿ (beval != ರಿಕ್ತ)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Nlast_Page_ಸಂಖ್ಯೆ, tab_ಎಣಿಕೆ, ರಿಕ್ತ);

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_Page_ಸಂಖ್ಯೆ, &currentpage, ರಿಕ್ತ);
    ಯದಿ (currentpage != curtabidx)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_Page_ಸಂಖ್ಯೆ, curtabidx, ರಿಕ್ತ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_set_curtab(ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಪೂರ್ಣಾಙ್ಕ		currentpage;

    ಯದಿ (tab_ಸಾಲು == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_Page_ಸಂಖ್ಯೆ, &currentpage, ರಿಕ್ತ);
    ಯದಿ (currentpage != nr)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_Page_ಸಂಖ್ಯೆ, nr, ರಿಕ್ತ);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_menu_colors(Widget id)
{
    ಯದಿ (gui.menu_bg_pixel != INVALCOLOR)
#ಯದಿ (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#ಅನ್ಯಥಾ
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNbackground, gui.menu_bg_pixel, ರಿಕ್ತ);
#endif
    ಯದಿ (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNforeground, gui.menu_fg_pixel, ರಿಕ್ತ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_scroll_colors(Widget id)
{
    ಯದಿ (gui.scroll_bg_pixel != INVALCOLOR)
#ಯದಿ (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#ಅನ್ಯಥಾ
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNbackground, gui.scroll_bg_pixel, ರಿಕ್ತ);
#endif
    ಯದಿ (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNforeground, gui.scroll_fg_pixel, ರಿಕ್ತ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    ಶೂನ್ಯ
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    ಯದಿ (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	ಯದಿ (fl != ರಿಕ್ತ)
	{
	    ಯದಿ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ಅನ್ಯಥಾ
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
	    XmFontListFree(fl);
	}
    }
# ಅನ್ಯಥಾ
    ಯದಿ (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	ಯದಿ (fl != ರಿಕ್ತ)
	{
	    ಯದಿ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ಅನ್ಯಥಾ
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ರಿಕ್ತ);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ _SharedFindReplace
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

ಸ್ಥಿರ SharedFindReplace find_widgets = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};
ಸ್ಥಿರ SharedFindReplace repl_widgets = {ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ};

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    ಯದಿ (cd != ರಿಕ್ತ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    ಯದಿ (cd != ರಿಕ್ತ)
	XtUnmanageChild(cd->dialog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    ವರ್ಣ	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    ಯದಿ (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Get the search/replace strings from the dialog
    ಯದಿ (flags == FRD_FINDNEXT)
    {
	repl_text = ರಿಕ್ತ;
	sfr = &find_widgets;
    }
    ಅನ್ಯಥಾ
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->down, XmNset, &direction_down, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->wword, XmNset, &wword, ರಿಕ್ತ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->mcase, XmNset, &mcase, ರಿಕ್ತ);
    ಯದಿ (wword)
	flags |= FRD_WHOLE_WORD;
    ಯದಿ (mcase)
	flags |= FRD_MATCH_CASE;

    (ಶೂನ್ಯ)gui_do_findrepl((ಪೂರ್ಣಾಙ್ಕ)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    ಯದಿ (find_text != ರಿಕ್ತ)
	XtFree(find_text);
    ಯದಿ (repl_text != ರಿಕ್ತ)
	XtFree(repl_text);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    ಕೀಲಿ_Sym keysym;

    ಯದಿ (frdp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    ಯದಿ (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_label(Widget w, ವರ್ಣ *label)
{
    XmString	str;
    char_u	*p, *next;
    ಕೀಲಿ_Sym	mnemonic = NUL;

    ಯದಿ (!w)
	ಪ್ರತ್ಯಾಗಮ;

    p = vim_strsave((char_u *)label);
    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಪ್ರತಿ (next = p; *next; ++next)
    {
	ಯದಿ (*next == DLG_HOTKEY_CHAR)
	{
	    ಪೂರ್ಣಾಙ್ಕ len = STRLEN(next);

	    ಯದಿ (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((ವರ್ಣ *)p);
    vim_free(p);
    ಯದಿ (str)
    {
	Xt_Va_Set_ಮೌಲ್ಯಗಳು(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ರಿಕ್ತ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಙ್ಕ do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		ಒಳಹರಿವು_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    ಪೂರ್ಣಾಙ್ಕ			n;
    Arg			args[6];
    ಪೂರ್ಣಾಙ್ಕ			wword = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    ಯದಿ (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	ಯದಿ (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	ಅನ್ಯಥಾ
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	ಯದಿ (entry_text != ರಿಕ್ತ)
	    XmTextFieldSetString(frdp->what, (ವರ್ಣ *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_ಮೌಲ್ಯಗಳು(frdp->wword, XmNset, wword, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Create a fresh new dialog window
    ಯದಿ (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    ಅನ್ಯಥಾ
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
	    ರಿಕ್ತ);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_ತಳ್ಳು_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ರಿಕ್ತ);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    ಯದಿ (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ತಳ್ಳು_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ರಿಕ್ತ);
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

    ಒಳಹರಿವು_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ರಿಕ್ತ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ಒಳಹರಿವು_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		ರಿಕ್ತ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, ಒಳಹರಿವು_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ರಿಕ್ತ);

	ಯದಿ (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	ಒಳಹರಿವು_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ರಿಕ್ತ);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ಒಳಹರಿವು_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ರಿಕ್ತ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, ಕೀಲಿ_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	ಅನ್ಯಥಾ
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, ಕೀಲಿ_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_ಮೌಲ್ಯಗಳು(label_what, args, n);
	widest = width;
	ಯದಿ (do_replace)
	{
	    Xt_Get_ಮೌಲ್ಯಗಳು(label_with, args, n);
	    ಯದಿ (width > widest)
		widest = width;
	}

	Xt_Va_Set_ಮೌಲ್ಯಗಳು(frdp->what, XmNleftOffset, widest, ರಿಕ್ತ);
	ಯದಿ (do_replace)
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(frdp->with, XmNleftOffset, widest, ರಿಕ್ತ);

    }

    XtManageChild(ಒಳಹರಿವು_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ಒಳಹರಿವು_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ಒಳಹರಿವು_form,
		ರಿಕ್ತ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		ರಿಕ್ತ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)ರಿಕ್ತ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ರಿಕ್ತ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ರಿಕ್ತ);
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
	    XmNtopWidget, ಒಳಹರಿವು_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    ರಿಕ್ತ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    ರಿಕ್ತ);
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
	    ರಿಕ್ತ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    ಯದಿ (entry_text != ರಿಕ್ತ)
	XmTextFieldSetString(frdp->what, (ವರ್ಣ *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   ಶೂನ್ಯ
gui_mch_find_dialog(exarg_T *eap)
{
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

    find_replace_dialog_create(eap->arg, FALSE);
}


    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    ಶೂನ್ಯ
gui_motif_synch_fonts(ಶೂನ್ಯ)
{
    SharedFindReplace *frdp;
    ಪೂರ್ಣಾಙ್ಕ		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    ಯದಿ (font == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    ಪ್ರತಿ (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	ಯದಿ (frdp->dialog)
	{
	    Xt_Va_Set_ಮೌಲ್ಯಗಳು(frdp->what, XmNfontList, font_list, ರಿಕ್ತ);
	    ಯದಿ (do_replace)
		Xt_Va_Set_ಮೌಲ್ಯಗಳು(frdp->with, XmNfontList, font_list, ರಿಕ್ತ);
	}
    }

    XmFontListFree(font_list);
}
