/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଅନ୍ତର୍ଭାଵ <Xm/Form.h>
#ଅନ୍ତର୍ଭାଵ <Xm/RowColumn.h>
#ଅନ୍ତର୍ଭାଵ <Xm/PushB.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Text.h>
#ଅନ୍ତର୍ଭାଵ <Xm/TextF.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Separator.h>
#ଅନ୍ତର୍ଭାଵ <Xm/Label.h>
#ଅନ୍ତର୍ଭାଵ <Xm/CascadeB.h>
#ଅନ୍ତର୍ଭାଵ <Xm/ScrollBar.h>
#ଅନ୍ତର୍ଭାଵ <Xm/MenuShell.h>
#ଅନ୍ତର୍ଭାଵ <Xm/DrawingA.h>
#ଯଦି (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#ଅନ୍ତର୍ଭାଵ <Xm/Frame.h>
#ଅନ୍ତର୍ଭାଵ <Xm/LabelG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/ToggleBG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/SeparatoG.h>
#ଅନ୍ତର୍ଭାଵ <Xm/XmP.h>

#ଅନ୍ତର୍ଭାଵ <X11/keysym.h>
#ଅନ୍ତର୍ଭାଵ <X11/Xatom.h>
#ଅନ୍ତର୍ଭାଵ <X11/StringDefs.h>
#ଅନ୍ତର୍ଭାଵ <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# ଯଦି defined(VMS)
#  include <xpm.h>
# ଅନ୍ୟଥା
#  include <X11/xpm.h>
# endif
#ଅନ୍ୟଥା
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#ଅନ୍ତର୍ଭାଵ "gui_xmebw.h"	// for our Enhanced Button Widget

#ଯଦି defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

ବାହ୍ୟ Widget vimShell;

ସ୍ଥିର Widget vimForm;
ସ୍ଥିର Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
ସ୍ଥିର Widget toolBarFrame;
ସ୍ଥିର Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
ସ୍ଥିର Widget	tab_ଧାଡ଼ି;
ସ୍ଥିର Widget	tab_ଧାଡ଼ି_menu = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# ଯଦି (XmVersion >= 1002)
// remember the last set value for the tearoff item
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tearoff_val = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED;
# endif
ସ୍ଥିର Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
ସ୍ଥିର ଶୂନ୍ୟ reset_focus(ଶୂନ୍ୟ);
#endif

ସ୍ଥିର ଶୂନ୍ୟ gui_motif_menu_colors(Widget id);
ସ୍ଥିର ଶୂନ୍ୟ gui_motif_scroll_colors(Widget id);

#ଯଦି (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#ଅନ୍ୟଥା
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    ସ୍ଥିର ଶୂନ୍ୟ
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	ମୂଲ୍ୟ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dragging;

    sb = gui_find_scrollbar((long)client_data);

    ମୂଲ୍ୟ = ((XmScrollBarCallbackStruct *)call_data)->ମୂଲ୍ୟ;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmCR_DRAG);
    gui_drag_scrollbar(sb, ମୂଲ୍ୟ, dragging);
}

#ifdef FEAT_GUI_TABLINE
    ସ୍ଥିର ଶୂନ୍ୟ
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    ଯଦି (nptr->reason != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmCR_NONE)
	send_tabline_event(nptr->page_ସଂଖ୍ୟା);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNuserData, &cmd, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି_menu, XmNuserData, &tab_idx, ଖାଲି);

    send_tabline_menu_event((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)tab_idx, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ସ୍ଥିର ଶୂନ୍ୟ
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tabline_scroller_clicked(
    ଅକ୍ଷର		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_ଧାଡ଼ି, scroller_name);
    ଯଦି (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, ଖାଲି);
	ଯଦି (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    ଯଦି (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		ଫେରନ୍ତୁ TRUE;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା				tab_idx = 0;
    WidgetList			children;
    Cardinal			ଅଙ୍କ_Children;
    ସ୍ଥିର XtIntervalId		timer = (XtIntervalId)0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    ଯଦି (event->button == Button1)
    {
	ଯଦି (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    ଫେରନ୍ତୁ;

	ଯଦି (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	ଫେରନ୍ତୁ;
    }

    ଯଦି (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି_menu, XmNuserData, &tab_idx, ଖାଲି);
	send_tabline_menu_event(tab_idx, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)TABLINE_MENU_CLOSE);
	ଫେରନ୍ତୁ;
    }

    ଯଦି (event->button != Button3)
	ଫେରନ୍ତୁ;

    // When ignoring events don't show the menu.
    ଯଦି (hold_gui_events || cmdwin_type != 0)
	ଫେରନ୍ତୁ;

    ଯଦି (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	ଯଦି (tab_w != (Widget)0 && Xm_Is_ଠେଲ_Button(tab_w))
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_w, Xm_Npage_ସଂଖ୍ୟା, &tab_idx, ଖାଲି);
    }

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି_menu, XmNuserData, (XtPointer)(long)tab_idx, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି_menu, XmNchildren, &children, XmNnumChildren,
		  &ଅଙ୍କ_Children, ଖାଲି);
    XtManageChildren(children, ଅଙ୍କ_Children);
    XmMenuPosition(tab_ଧାଡ଼ି_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_ଧାଡ଼ି_menu);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tabline_balloon_cb(BalloonEval *beval, ପୂର୍ଣ୍ଣସଂଖ୍ୟା state UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr;
    tabpage_T	*tp;

    ଯଦି (beval->target == (Widget)0)
	ଫେରନ୍ତୁ;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(beval->target, Xm_Npage_ସଂଖ୍ୟା, &nr, ଖାଲି);
    tp = find_tabpage(nr);
    ଯଦି (tp == ଖାଲି)
	ଫେରନ୍ତୁ;

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

#ଅନ୍ତର୍ଭାଵ <Xm/XmP.h>
#ଅନ୍ତର୍ଭାଵ <Xm/LabelP.h>

ସ୍ଥିର XtExposeProc old_label_expose = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned ଅକ୍ଷର   label_type = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmSTRING;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(_w, XmNlabelType, &label_type, (XtPointer)0);

    ଯଦି (XtIsSensitive(_w) || label_type != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    ଅନ୍ୟଥା
    {
	XGCValues   ମୂଲ୍ୟଗୁଡ଼ିକ;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	ମୂଲ୍ୟଗୁଡ଼ିକ.graphics_exposures = False;

	ଯଦି (fs != 0)
	{
	    mask |= GCFont;
	    ମୂଲ୍ୟଗୁଡ଼ିକ.font = fs->fid;
	}

	ଯଦି (lw->primitive.ଶୀର୍ଷ_shadow_pixmap != None
		&& lw->primitive.ଶୀର୍ଷ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    ମୂଲ୍ୟଗୁଡ଼ିକ.fill_style = FillTiled;
	    ମୂଲ୍ୟଗୁଡ଼ିକ.tile = lw->primitive.ଶୀର୍ଷ_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	ଯଦି (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	ମୂଲ୍ୟଗୁଡ଼ିକ.foreground = lw->primitive.ଶୀର୍ଷ_shadow_color;
	ମୂଲ୍ୟଗୁଡ଼ିକ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &ମୂଲ୍ୟଗୁଡ଼ିକ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	ଯଦି (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	ମୂଲ୍ୟଗୁଡ଼ିକ.foreground = lw->primitive.bottom_shadow_color;
	ମୂଲ୍ୟଗୁଡ଼ିକ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &ମୂଲ୍ୟଗୁଡ଼ିକ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    ଶୂନ୍ୟ
gui_x11_create_widgets(ଶୂନ୍ୟ)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    ଯଦି (old_label_expose == ଖାଲି)
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
#ଯଦି (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XmNdeleteResponse, XmDO_NOTHING, ଖାଲି);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ଖାଲି);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ac = 0;

# ଯଦି (XmVersion >= 1002)
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
	ଖାଲି);
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
	ଖାଲି);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_ସଂଖ୍ୟା, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_ଶୀର୍ଷ_ଡାହାଣ); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_ଧାଡ଼ି = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_ଧାଡ଼ି, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			ଖାଲି);
    XtAddEventHandler(tab_ଧାଡ଼ି, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, ଖାଲି);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_ଧାଡ଼ି, "MinorTabScrollerNext");
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ଖାଲି);
    scroller = XtNameToWidget(tab_ଧାଡ଼ି, "MinorTabScrollerPrevious");
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ଖାଲି);

    // Create the tabline popup menu
    tab_ଧାଡ଼ି_menu = XmCreatePopupMenu(tab_ଧାଡ଼ି, "tabline popup", ଖାଲି, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((ଅକ୍ଷର *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ଠେଲ_Button(tab_ଧାଡ଼ି_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ଖାଲି);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((ଅକ୍ଷର *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ଠେଲ_Button(tab_ଧାଡ଼ି_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ଖାଲି);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((ଅକ୍ଷର *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_ଠେଲ_Button(tab_ଧାଡ଼ି_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ଖାଲି);
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
	ଖାଲି);
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
	ଖାଲି);

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
    ଶୂନ୍ୟ
gui_x11_destroy_widgets(ଶୂନ୍ୟ)
{
    textArea = ଖାଲି;
#ifdef FEAT_MENU
    menuBar = ଖାଲି;
#endif
}

    ଶୂନ୍ୟ
gui_mch_set_text_area_pos(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    y UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    w UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    ଶୂନ୍ୟ
gui_x11_set_back_color(ଶୂନ୍ୟ)
{
    ଯଦି (textArea != ଖାଲି)
#ଯଦି (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#ଅନ୍ୟଥା
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textArea,
		  XmNbackground, gui.back_pixel,
		  ଖାଲି);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    ଶୂନ୍ୟ
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window ମୂଳ, child;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, height, border_width, depth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNmappedWhenManaged, &mappedWhenManaged, ଖାଲି);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNmappedWhenManaged, False, ଖାଲି);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &ମୂଳ, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &ମୂଳ, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    ଯଦି (x < 0)
	x = 0;
    ଯଦି (x > maxX)
	x = maxX;
    ଯଦି (y < 0)
	y = 0;
    ଯଦି (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNx, x, XmNy, y, ଖାଲି);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(shell, XmNmappedWhenManaged, mappedWhenManaged, ଖାଲି);
}

#ଯଦି defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# ଯଦି (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# ଅନ୍ୟଥା
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(ଖାଲି, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    ଫେରନ୍ତୁ font_list;
}

# ଯଦି ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(ଖାଲି, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    ଫେରନ୍ତୁ font_list;
}
# endif

#endif

#ଯଦି defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ସ୍ଥିର ଶୂନ୍ୟ gui_motif_add_actext(vimmenu_T *menu);
# ଯଦି (XmVersion >= 1002)
ସ୍ଥିର ଶୂନ୍ୟ toggle_tearoff(Widget wid);
ସ୍ଥିର ଶୂନ୍ୟ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ସ୍ଥିର ଶୂନ୍ୟ submenu_change(vimmenu_T *mp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା colors);

ସ୍ଥିର ଶୂନ୍ୟ do_set_mnemonics(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା menu_enabled = TRUE;

    ଶୂନ୍ୟ
gui_mch_enable_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ଯଦି (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	ଯଦି (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ଖାଲି);
#  ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
#  endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ଖାଲି);
	}
	ଅନ୍ୟଥା
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ଖାଲି);
	}
    }
    ଅନ୍ୟଥା
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	ଯଦି (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ଖାଲି);
#  ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
#  endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ଖାଲି);
	}
	ଅନ୍ୟଥା
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_FORM,
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ଖାଲି);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    ଶୂନ୍ୟ
gui_motif_set_mnemonics(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    ଯଦି (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    ସ୍ଥିର ଶୂନ୍ୟ
do_set_mnemonics(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	ଯଦି (menu->id != (Widget)0)
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ଖାଲି);
}

    ଶୂନ୍ୟ
gui_mch_add_menu(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    ଯଦି (menu_is_popup(menu->name))
    {
	Arg arg[2];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  ଯଦି (XmVersion <= 1002)
	ଯଦି (mouse_model_popup())
#  endif
	{
	    ଯଦି (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    ଯଦି (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	ଫେରନ୍ତୁ;
    }
# endif

    ଯଦି (!menu_is_menubar(menu->name)
	    || (parent != ଖାଲି && parent->submenu_id == (Widget)0))
	ଫେରନ୍ତୁ;

    label = XmStringCreate((ଅକ୍ଷର *)menu->dname, STRING_TAG);
    ଯଦି (label == ଖାଲି)
	ଫେରନ୍ତୁ;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == ଖାଲି) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# ଯଦି (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != ଖାଲି
			   && tearoff_val == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ଖାଲି);
    XmStringFree(label);

    ଯଦି (menu->id == (Widget)0)		// failed
	ଫେରନ୍ତୁ;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    ଯଦି (parent == ଖାଲି && menu->priority >= 9999)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menuBar,
		XmNmenuHelpWidget, menu->id,
		ଖାଲି);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ଖାଲି);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	ଖାଲି);
    gui_motif_menu_colors(menu->submenu_id);

    ଯଦି (menu->submenu_id == (Widget)0)		// failed
	ଫେରନ୍ତୁ;

# ଯଦି (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ଖାଲି);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    ଯଦି (parent == ଖାଲି)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    ଯଦି (menu->actext == ଖାଲି || menu->id == (Widget)0)
	ଫେରନ୍ତୁ;

    label = XmStringCreate((ଅକ୍ଷର *)menu->actext, STRING_TAG);
    ଯଦି (label == ଖାଲି)
	ଫେରନ୍ତୁ;
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menu->id, XmNacceleratorText, label, ଖାଲି);
    XmStringFree(label);
}

    ଶୂନ୍ୟ
gui_mch_toggle_tearoffs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
# ଯଦି (XmVersion >= 1002)
    ଯଦି (enable)
	tearoff_val = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED;
    ଅନ୍ୟଥା
	tearoff_val = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ମୂଳ_menu);
# endif
}

# ଯଦି (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(wid, XmNtearOffModel, tearoff_val, ଖାଲି);
    ଯଦି (tearoff_val == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ସ୍ଥିର ଶୂନ୍ୟ
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    ଯେତେବେଳେ (menu != ଖାଲି)
    {
	ଯଦି (!menu_is_popup(menu->name))
	{
	    ଯଦି (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_text_area_extra_height(ଶୂନ୍ୟ)
{
    Dimension	shadowHeight;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm, XmNshadowThickness, &shadowHeight, ଖାଲି);
    ଫେରନ୍ତୁ shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    ଶୂନ୍ୟ
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ସ୍ଥିର Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    ଯଦି (id != (Widget)0)
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNheight, &height, ଖାଲି);

    // Find any menu Widget, to be able to call XtManageChild()
    ଅନ୍ୟଥା
	FOR_ALL_MENUS(mp)
	    ଯଦି (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ଭାଙ୍ଗ;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    ଯଦି (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	ଯଦି (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(mp->id, XmNy, &y, ଖାଲି);
	    ଯଦି (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	ଖାଲି);

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
    ଯଦି (gui.shell_created)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menuBar, XmNheight, gui.menu_height, ଖାଲି);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ସ୍ଥିର ଅକ୍ଷର **get_toolbar_pixmap(vimmenu_T *menu, ଅକ୍ଷର **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    ମୂଳ_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (ଅକ୍ଷର *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    ଯଦି (status == XpmSuccess)
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ସ୍ଥିର ଅକ୍ଷର **
get_toolbar_pixmap(vimmenu_T *menu, ଅକ୍ଷର **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    ଅକ୍ଷର	**xpm = ଖାଲି;		// xpm array
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    *fname = ଖାଲି;
    buf[0] = NUL;			// start with NULL path

    ଯଦି (menu->iconfile != ଖାଲି)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	ଯଦି (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	ଯଦି (res == OK)
	{
	    *fname = (ଅକ୍ଷର *)vim_strsave(buf);
	    ଫେରନ୍ତୁ tb_blank_xpm;
	}
    }

    ଯଦି (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	ଯଦି (menu->iconidx >= 0 && menu->iconidx
	       < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଆକାର(built_in_pixmaps) / ଆକାର(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	ଅନ୍ୟଥା
	    xpm = tb_blank_xpm;
    }

    ଫେରନ୍ତୁ xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_pixmap_args(vimmenu_T *menu, Arg *args, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    ଯଦି (menu->xpm == ଖାଲି)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (menu->xpm_fname != ଖାଲି)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    ଫେରନ୍ତୁ n;
}
# endif // FEAT_TOOLBAR

    ଶୂନ୍ୟ
gui_mch_add_menu_item(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# ଯଦି (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    ଯଦି (menu_is_child_of_popup(menu) && !mouse_model_popup())
	ଫେରନ୍ତୁ;
# endif

# ifdef FEAT_TOOLBAR
    ଯଦି (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = ଖାଲି;    // fallback label if pixmap not found
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;
	Arg		args[18];

	n = 0;
	ଯଦି (menu_is_separator(menu->name))
	{
	    ଅକ୍ଷର	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (ଅକ୍ଷର *)vim_strchr(menu->name, ':');
	    ଯଦି (cp != ଖାଲି)
		wid = (Dimension)atoi(++cp);
	    ଅନ୍ୟଥା
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	ଅନ୍ୟଥା
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
	    xms = XmStringCreate((ଅକ୍ଷର *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	ଯଦି (menu->id == ଖାଲି)
	{
	    menu->id = XtCreateManagedWidget((ଅକ୍ଷର *)menu->dname,
			type, toolBar, args, n);
	    ଯଦି (menu->id != ଖାଲି && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	ଅନ୍ୟଥା
	    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(menu->id, args, n);
	ଯଦି (xms != ଖାଲି)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = ଖାଲି;
	// When adding first item to toolbar it might have to be enabled .
	ଯଦି (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ଖାଲି)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	ଫେରନ୍ତୁ;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    ଯଦି (parent->submenu_id == (Widget)0)
	ଫେରନ୍ତୁ;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    ଯଦି (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# ଯଦି (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ଖାଲି);
	gui_motif_menu_colors(menu->id);
	ଫେରନ୍ତୁ;
    }

    label = XmStringCreate((ଅକ୍ଷର *)menu->dname, STRING_TAG);
    ଯଦି (label == ଖାଲି)
	ଫେରନ୍ତୁ;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ଠେଲ_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# ଯଦି (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ଖାଲି);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    ଯଦି (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# ଯଦି (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    ଶୂନ୍ୟ
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;

    // When GUI hasn't started the menus have not been created.
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

    ଯେତେବେଳେ (menu)
    {
      ଯଦି (menu->children != ଖାଲି)
      {
	  ଯଦି (menu_is_popup(menu->name))
	  {
	      ଯଦି (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      ଅନ୍ୟଥା
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      ଅନ୍ୟଥା ଯଦି (menu_is_child_of_popup(menu))
      {
	  ଯଦି (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  ଅନ୍ୟଥା
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    ଶୂନ୍ୟ
gui_mch_new_menu_colors(ଶୂନ୍ୟ)
{
    ଯଦି (menuBar == (Widget)0)
	ଫେରନ୍ତୁ;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ମୂଳ_menu, TRUE);
}

    ଶୂନ୍ୟ
gui_mch_new_menu_font(ଶୂନ୍ୟ)
{
    ଯଦି (menuBar == (Widget)0)
	ଫେରନ୍ତୁ;
    submenu_change(ମୂଳ_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(menuBar, XmNheight, &height, ଖାଲି);
	gui.menu_height = height;

	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XtNwidth, &w, XtNheight, &h, ଖାଲି);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# ଯଦି defined(FEAT_BEVAL_GUI)
    ଶୂନ୍ୟ
gui_mch_new_tooltip_font(ଶୂନ୍ୟ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    ଯଦି (toolBar == (Widget)0)
	ଫେରନ୍ତୁ;

    menu = gui_find_menu((char_u *)"ToolBar");
    ଯଦି (menu != ଖାଲି)
	submenu_change(menu, FALSE);
#  endif
}

    ଶୂନ୍ୟ
gui_mch_new_tooltip_colors(ଶୂନ୍ୟ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    ଯଦି (toolBar == (Widget)0)
	ଫେରନ୍ତୁ;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    ଯଦି (toolbar != ଖାଲି)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
submenu_change(
    vimmenu_T	*menu,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    ପାଇଁ (mp = menu; mp != ଖାଲି; mp = mp->next)
    {
	ଯଦି (mp->id != (Widget)0)
	{
	    ଯଦି (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		ଯଦି (mp->xpm != ଖାଲି)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ଯଦି (mp->tip != ଖାଲି)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].ମୂଲ୍ୟ = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].ମୂଲ୍ୟ = gui.tooltip_fg_pixel;
		    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(mp->tip->balloonLabel, &args[0], Xt_ସଂଖ୍ୟା(args));
		}
#  endif
# endif
	    }
	    ଅନ୍ୟଥା
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ଯଦି (mp->tip != ଖାଲି)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].ମୂଲ୍ୟ = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(mp->tip->balloonLabel, &args[0], Xt_ସଂଖ୍ୟା(args));
		}
# endif
	    }
	}

	ଯଦି (mp->children != ଖାଲି)
	{
# ଯଦି (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    ଯଦି (mp->submenu_id != (Widget)0)
	    {
		ଯଦି (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    ଯଦି (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    ଯଦି (menu->id == (Widget)0)
	ଫେରନ୍ତୁ;

    Widget	    parent;

    parent = XtParent(menu->id);
# ଯଦି defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    ଯଦି (parent == toolBar && menu->tip != ଖାଲି)
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
	menu->tip = ଖାଲି;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    ଯଦି (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    ଅନ୍ୟଥା ଯଦି (parent == toolBar)
    {
	Cardinal    ଅଙ୍କ_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBar, XmNnumChildren, &ଅଙ୍କ_children, ଖାଲି);
	ଯଦି (ଅଙ୍କ_children == 0)
	    gui_mch_show_toolbar(FALSE);
	ଅନ୍ୟଥା
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    ଶୂନ୍ୟ
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
    ଶୂନ୍ୟ
gui_mch_def_colors(ଶୂନ୍ୟ)
{
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

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

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    ଯଦି (sb->id != (Widget)0)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ଖାଲି);
}

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h)
{
    ଯଦି (sb->id == (Widget)0)
	ଫେରନ୍ତୁ;

    ଯଦି (sb->type == SBAR_ବାମ || sb->type == SBAR_ଡାହାଣ)
    {
	ଯଦି (y == 0)
	    h -= gui.border_offset;
	ଅନ୍ୟଥା
	    y -= gui.border_offset;
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		ଖାଲି);
    }
    ଅନ୍ୟଥା
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_ଡାହାଣ]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		ଖାଲି);
    XtManageChild(sb->id);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_xpadding(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(textArea, XtNwidth, &tw, XtNx, &tx, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XtNwidth, &ww, ଖାଲି);
    xpad = ww - tw - tx - gui.scrollbar_width;
    ଫେରନ୍ତୁ (xpad < 0) ? 0 : xpad;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_ypadding(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(textArea, XtNheight, &th, XtNy, &ty, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(vimShell, XtNheight, &wh, ଖାଲି);
    ypad = wh - th - ty - gui.scrollbar_height;
    ଫେରନ୍ତୁ (ypad < 0) ? 0 : ypad;
}

    ଶୂନ୍ୟ
gui_mch_enable_scrollbar(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    Arg		args[16];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n;

    ଯଦି (sb->id == (Widget)0)
	ଫେରନ୍ତୁ;

    n = 0;
    ଯଦି (flag)
    {
	ବାଛ (sb->type)
	{
	    ସ୍ଥିତି SBAR_ବାମ:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SBAR_ଡାହାଣ:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		ଭାଙ୍ଗ;
	}
	Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textArea, args, n);
	XtManageChild(sb->id);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ବାଛ (sb->type)
	    {
		ସ୍ଥିତି SBAR_ବାମ:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି SBAR_ଡାହାଣ:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    ଭାଙ୍ଗ;

		ସ୍ଥିତି SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    ଭାଙ୍ଗ;
	    }
	    Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    ଶୂନ୍ୟ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ବାଛ (sb->type)
    {
	ସ୍ଥିତି SBAR_ବାମ:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SBAR_ଡାହାଣ:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    ଭାଙ୍ଗ;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    ଯଦି (sb->id == (Widget)0)
	ଫେରନ୍ତୁ;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, ଚାବି_Press_Mask, FALSE, gui_x11_ଚାବି_hit_cb,
	    (XtPointer)0);
}

    ଶୂନ୍ୟ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ଯଦି (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    ଯଦି (sb->id != (Widget)0)
    {
	ଯଦି (gui.scroll_bg_pixel != INVALCOLOR)
	{
#ଯଦି (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ଖାଲି);
	}

	ଯଦି (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ଖାଲି);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    ଯଦି (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
do_mnemonic(Widget w, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keycode)
{
    WidgetList	    children;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ_Children, i;
    Boolean	    isMenu;
    ଚାବି_Sym	    mnemonic = '\0';
    ଅକ୍ଷର	    mneString[2];
    Widget	    userData;
    unsigned ଅକ୍ଷର   rowColType;

    ଯଦି (XtIsComposite(w))
    {
	ଯଦି (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNrowColumnType, &rowColType, ଖାଲି);
	    isMenu = (rowColType != (unsigned ଅକ୍ଷର)XmWORK_AREA);
	}
	ଅନ୍ୟଥା
	    isMenu = False;
	ଯଦି (!isMenu)
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNchildren, &children, XmNnumChildren,
			  &ଅଙ୍କ_Children, ଖାଲି);
	    ପାଇଁ (i = 0; i < ଅଙ୍କ_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    ଅନ୍ୟଥା
    {
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNmnemonic, &mnemonic, ଖାଲି);
	ଯଦି (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    ଯଦି (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		ଯଦି (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNuserData, &userData, ଖାଲି);
		    ଯଦି (userData != ଖାଲି && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		ଅନ୍ୟଥା
		{
		    XKeyPressedEvent ଚାବି_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(ଚାବି_Event);
		    ଚାବି_Event.type = ଚାବି_Press;
		    ଚାବି_Event.serial = 1;
		    ଚାବି_Event.send_event = True;
		    ଚାବି_Event.display = XtDisplay(w);
		    ଚାବି_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & ଚାବି_Event,
								     ଖାଲି, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
add_mnemonic_grabs(Widget dialog, Widget w)
{
    ଅକ୍ଷର	    mneString[2];
    WidgetList	    children;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ଅଙ୍କ_Children, i;
    Boolean	    isMenu;
    ଚାବି_Sym	    mnemonic = '\0';
    unsigned ଅକ୍ଷର   rowColType;

    ଯଦି (XtIsComposite(w))
    {
	ଯଦି (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNrowColumnType, &rowColType, ଖାଲି);
	    isMenu = (rowColType != (unsigned ଅକ୍ଷର)XmWORK_AREA);
	}
	ଅନ୍ୟଥା
	    isMenu = False;
	ଯଦି (!isMenu)
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNchildren, &children, XmNnumChildren,
							  &ଅଙ୍କ_Children, ଖାଲି);
	    ପାଇଁ (i = 0; i < ଅଙ୍କ_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    ଅନ୍ୟଥା
    {
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(w, XmNmnemonic, &mnemonic, ଖାଲି);
	ଯଦି (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_ଚାବି(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    ସ୍ଥିର ଶୂନ୍ୟ
activate_dialog_mnemonics(Widget dialog)
{
    ଯଦି (!dialog)
	ଫେରନ୍ତୁ;

    XtAddEventHandler(dialog, ଚାବି_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ଖାଲି);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
suppress_dialog_mnemonics(Widget dialog)
{
    ଯଦି (!dialog)
	ଫେରନ୍ତୁ;

    Xt_Ungrab_ଚାବି(dialog, Any_ଚାବି, Mod1Mask);
    XtRemoveEventHandler(dialog, ଚାବି_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ଖାଲି);
}

#ଯଦି defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    ଯଦି (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	ଯଦି (fl != ଖାଲି)
	{
	    ଯଦି (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ଅନ୍ୟଥା
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
	    XmFontListFree(fl);
	}
    }
# ଅନ୍ୟଥା
    ଯଦି (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	ଯଦି (fl != ଖାଲି)
	{
	    ଯଦି (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ଅନ୍ୟଥା
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#ଯଦି defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା dialog_callback_arg
{
    ଅକ୍ଷର *  args;   // not used right now
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id;
} dcbarg_T;

ସ୍ଥିର Widget dialog_wgt;
ସ୍ଥିର ଅକ୍ଷର *browse_fname = ଖାଲି;
ସ୍ଥିର XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ସ୍ଥିର ଶୂନ୍ୟ DialogCancelCB(Widget, XtPointer, XtPointer);
ସ୍ଥିର ଶୂନ୍ୟ DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_predefined_label(Widget parent, String name, ଅକ୍ଷର *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    ଚାବି_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    ଯଦି (!w)
	ଫେରନ୍ତୁ;

    p = vim_strsave((char_u *)new_label);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (next = p; *next; ++next)
    {
	ଯଦି (*next == DLG_HOTKEY_CHAR)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = STRLEN(next);

	    ଯଦି (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((ଅକ୍ଷର *)p, STRING_TAG);
    vim_free(p);

    ଯଦି (str != ଖାଲି)
    {
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ଖାଲି);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    ଯଦି (!w)
	ଫେରନ୍ତୁ;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = ଖାଲି;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ଖାଲି, 0);

    ଯଦି (initdir == ଖାଲି || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    ଯଦି (dflt == ଖାଲି)
	dflt = (char_u *)"";
    ଅନ୍ୟଥା ଯଦି (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    ଯଦି (filter == ଖାଲି)
	pattern = (char_u *)"";
    ଅନ୍ୟଥା
    {
	char_u	*s, *p;

	s = filter;
	ପାଇଁ (p = filter; *p != NUL; ++p)
	{
	    ଯଦି (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    ଯଦି (*p == ';' || *p == '\n')	// end of (first) pattern
		ଭାଙ୍ଗ;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	ଯଦି (pattern == ଖାଲି)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (ଅକ୍ଷର *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (ଅକ୍ଷର *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (ଅକ୍ଷର *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (ଅକ୍ଷର *)title, STRLEN(title) + 1,
	ଖାଲି);

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
    ଯଦି (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ଖାଲି);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned ଅକ୍ଷର)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    କର
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_ନିବେଶ_Mask)XtIMAll);
    } ଯେତେବେଳେ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    ଯଦି (browse_fname == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    ଯଦି (browse_fname != ଖାଲି)
    {
	XtFree(browse_fname);
	browse_fname = ଖାଲି;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    ଯଦି (browse_fname != ଖାଲି)
    {
	XtFree(browse_fname);
	browse_fname = ଖାଲି;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->ମୂଲ୍ୟ, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#ଯଦି defined(FEAT_GUI_DIALOG)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    ଅକ୍ଷର	buf[2];
    ଚାବି_Sym	ଚାବି_sym;

    ଯଦି (XLookupString(&(event->xkey), buf, 2, &ଚାବି_sym, ଖାଲି) == 1)
    {
	ଯଦି (*buf == CAR)
	    dialogStatus = 1;
	ଅନ୍ୟଥା ଯଦି (*buf == ESC)
	    dialogStatus = 2;
    }
    ଯଦି ((ଚାବି_sym == XK_ବାମ || ଚାବି_sym == XK_ଡାହାଣ)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    ସ୍ଥିର ଶୂନ୍ୟ
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)client_data + 1;
}

# ifdef HAVE_XPM

    ସ୍ଥିର Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    ଅକ୍ଷର	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_ପ୍ରତୀକ	color[5] =
    {
	{"none", ଖାଲି, 0},
	{"iconColor1", ଖାଲି, 0},
	{"bottomShadowColor", ଖାଲି, 0},
	{"topShadowColor", ଖାଲି, 0},
	{"selectColor", ଖାଲି, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ଖାଲି);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, ମୂଳ_Window_Of_Screen(scr),
		    data, &pixmap, ଖାଲି, &attr);

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(label, XmNrecomputeSize, &rs, ଖାଲି);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(label, XmNrecomputeSize, True, ଖାଲି);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ଖାଲି);
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(label, XmNrecomputeSize, rs, ଖାଲି);

    ଫେରନ୍ତୁ label;
}
# endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_dialog(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			butcount;
    Widget		w;
    Widget		dialogform = ଖାଲି;
    Widget		form = ଖାଲି;
    Widget		dialogtextfield = ଖାଲି;
    Widget		*buttons;
    Widget		sep_form = ଖାଲି;
    Boolean		vertical;
    Widget		separator = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    Arg			args[6];
# ifdef HAVE_XPM
    ଅକ୍ଷର		**icon_data = ଖାଲି;
    Widget		dialogpixmap = ଖାଲି;
# endif

    ଯଦି (title == ଖାଲି)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (ଅକ୍ଷର *)"dialog", ଖାଲି, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ଖାଲି);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((ଅକ୍ଷର *)title);
    ଯଦି (label == ଖାଲି)
	ଫେରନ୍ତୁ -1;
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ଖାଲି);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    ଯଦି (buts == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    ପାଇଁ (p = buts; *p; ++p)
	ଯଦି (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    ଯଦି (buttons == ଖାଲି)
    {
	vim_free(buts);
	ଫେରନ୍ତୁ -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    ପାଇଁ (butcount = 0; *p; ++butcount)
    {
	ଚାବି_Sym mnemonic = NUL;

	ପାଇଁ (next = p; *next; ++next)
	{
	    ଯଦି (*next == DLG_HOTKEY_CHAR)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = STRLEN(next);

		ଯଦି (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    ଯଦି (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		ଭାଙ୍ଗ;
	    }
	}
	label = XmStringCreate(_((ଅକ୍ଷର *)p), STRING_TAG);
	ଯଦି (label == ଖାଲି)
	    ଭାଙ୍ଗ;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ଠେଲ_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ଖାଲି);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	ଯଦି (butcount > 0)
	{
	    ଯଦି (vertical)
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ଖାଲି);
	    ଅନ୍ୟଥା
	    {
		ଯଦି (*next == NUL)
		{
		    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    ଖାଲି);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    ଖାଲି);
		    XtManageChild(sep_form);
		}
		ଅନ୍ୟଥା
		{
		    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ଖାଲି);
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (!vertical)
	{
	    ଯଦି (*next == NUL)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			ଖାଲି);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			ଖାଲି);
		XtManageChild(sep_form);
	    }
	    ଅନ୍ୟଥା
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			ଖାଲି);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    ଯଦି (butcount > 0)
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

    ଯଦି (textfield != ଖାଲି)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ଖାଲି);
	ଯଦି (butcount > 0)
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ଖାଲି);
	ଅନ୍ୟଥା
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ଖାଲି);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (ଅକ୍ଷର *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, ଚାବି_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)ଖାଲି);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ଖାଲି);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ବାଛ (type)
    {
	ସ୍ଥିତି VIM_GENERIC:
	    icon_data = generic_xpm;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_ERROR:
	    icon_data = error_xpm;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_WARNING:
	    icon_data = alert_xpm;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_INFO:
	    icon_data = info_xpm;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VIM_QUESTION:
	    icon_data = quest_xpm;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
    label = XmStringCreateLtoR((ଅକ୍ଷର *)message, STRING_TAG);
    ଯଦି (label == ଖାଲି)
    {
	vim_free(buttons);
	ଫେରନ୍ତୁ -1;
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
# ଅନ୍ୟଥା
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				ଖାଲି);
    XmStringFree(label);
    set_fontlist(w);

    ଯଦି (textfield != ଖାଲି)
    {
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (butcount > 0)
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ଖାଲି);
	ଅନ୍ୟଥା
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ଖାଲି);
    }

    ଯଦି (dfltbutton < 1)
	dfltbutton = 1;
    ଯଦି (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ଖାଲି);
    ଯଦି (textfield != ଖାଲି)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogform, XmNinitialFocus, dialogtextfield, ଖାଲି);
    ଅନ୍ୟଥା
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ଖାଲି);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    ଯଦି (textfield != ଖାଲି && *textfield != NUL)
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
    ପାଇଁ (;;)
    {
	XtAppProcessEvent(app, (Xt_ନିବେଶ_Mask)XtIMAll);
	ଯଦି (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    ଭାଙ୍ଗ;
    }

    vim_free(buttons);

    ଯଦି (textfield != ଖାଲି)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	ଯଦି (p == ଖାଲି || dialogStatus < 0)
	    *textfield = NUL;
	ଅନ୍ୟଥା
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((ଅକ୍ଷର *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    ଫେରନ୍ତୁ dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#ଯଦି defined(FEAT_TOOLBAR)
    ଶୂନ୍ୟ
gui_mch_show_toolbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    Cardinal	ଅଙ୍କ_Children;	    // how many children toolBar has

    ଯଦି (toolBar == (Widget)0)
	ଫେରନ୍ତୁ;
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBar, XmNnumChildren, &ଅଙ୍କ_Children, ଖାଲି);
    ଯଦି (showit && ଅଙ୍କ_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBar, XmNchildren, &children, ଖାଲି);
	{
	    ଶୂନ୍ୟ    (*action)(BalloonEval *);
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    text = 0;

	    ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    ଅନ୍ୟଥା
		action = &gui_mch_disable_beval_area;
	    ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *)p_toolbar, "text"))
		text = 1;
	    ଅନ୍ୟଥା ଯଦି (strstr((ଅଚଳ ଅକ୍ଷର *)p_toolbar, "icons"))
		text = -1;
	    ଯଦି (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    ଯଦି (menu_is_toolbar(toolbar->dname))
			ଭାଙ୍ଗ;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		ଯଦି (toolbar)
		{
		    ପାଇଁ (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			ଯଦି (cur->tip != ଖାଲି)
			    (*action)(cur->tip);
			ଯଦି (!menu_is_separator(cur->name))
			{
			    ଯଦି (text == 1 || cur->xpm == ଖାଲି)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    ଯଦି (cur->id != ଖାଲି)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(cur->id, args, n);
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
	ଯଦି (showing_tabline)
	{
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ଖାଲି);
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ଧାଡ଼ି,
			  ଖାଲି);
	}
	ଅନ୍ୟଥା
# endif
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ଖାଲି);
	ଯଦି (XtIsManaged(menuBar))
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ଖାଲି);
	ଅନ୍ୟଥା
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	gui.toolbar_height = 0;
	ଯଦି (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ଖାଲି);
	}
	ଅନ୍ୟଥା
	{
# ifdef FEAT_GUI_TABLINE
	    ଯଦି (showing_tabline)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_FORM,
			      ଖାଲି);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ଧାଡ଼ି,
			      ଖାଲି);
	    }
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ଖାଲି);
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
    ସ୍ଥିର ଶୂନ୍ୟ
reset_focus(ଶୂନ୍ୟ)
{
    ଯଦି (textArea != ଖାଲି)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_compute_toolbar_height(ଶୂନ୍ୟ)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	ଅଙ୍କ_Children;	    // how many children toolBar has
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    borders = 0;
    height = 0;
    ଯଦି (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		ଖାଲି);
	borders += fst + fmh;
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &ଅଙ୍କ_Children, ଖାଲି);
	borders += tst + tmh;
	ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଅଙ୍କ_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(children[i], XmNheight, &whgt, ଖାଲି);
	    ଯଦି (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    ଯଦି (height < 10)
	height = 24;
# endif

    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(height + (borders << 1));
}

    ଶୂନ୍ୟ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    ଖାଲି);
}
#endif

#ଯଦି defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_show_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (tab_ଧାଡ଼ି == (Widget)0)
	ଫେରନ୍ତୁ;

    ଯଦି (!showit != !showing_tabline)
    {
	ଯଦି (showit)
	{
	    XtManageChild(tab_ଧାଡ଼ି);
	    XtUnmanageChild(XtNameToWidget(tab_ଧାଡ଼ି, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_ଧାଡ଼ି, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_ଧାଡ଼ି,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ଯଦି (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ଖାଲି);
	    ଅନ୍ୟଥା
#  endif
		ଯଦି (XtIsManaged(menuBar))
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ଖାଲି);
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି,
			      XmNtopAttachment, XmATTACH_FORM, ଖାଲି);
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ଧାଡ଼ି,
			  ଖାଲି);
	}
	ଅନ୍ୟଥା
	{
	    XtUnmanageChild(tab_ଧାଡ଼ି);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ଯଦି (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ଖାଲି);
	    ଅନ୍ୟଥା
#  endif
		ଯଦି (XtIsManaged(menuBar))
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ଖାଲି);
	    ଅନ୍ୟଥା
# endif
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, ଖାଲି);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_showing_tabline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ tab_ଧାଡ଼ି != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ଶୂନ୍ୟ
gui_mch_update_tabline(ଶୂନ୍ୟ)
{
    tabpage_T		*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			nr = 1, n;
    Arg			args[10];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			last_page, tab_ଗଣନା;
    XmString		label_str;
    ଅକ୍ଷର		*label_cstr;
    BalloonEval		*beval;

    ଯଦି (tab_ଧାଡ଼ି == (Widget)0)
	ଫେରନ୍ତୁ;

    // Add a label for each tab page.  They all contain the same text area.
    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = tp->tp_next, ++nr)
    {
	ଯଦି (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_ଧାଡ଼ି, nr, &page_info);
	ଯଦି (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ଠେଲ_Button(tab_ଧାଡ଼ି, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, ଖାଲି, tabline_balloon_cb,
									ଖାଲି);
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab, XmNuserData, beval, ଖାଲି);
	}
	ଅନ୍ୟଥା
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab, Xm_Npage_ସଂଖ୍ୟା, nr, ଖାଲି);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab, XmNlabelString, &label_str, ଖାଲି);
	ଯଦି (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    ଯଦି (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ଖାଲି);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_ଗଣନା = nr - 1;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Nlast_Page_ସଂଖ୍ୟା, &last_page, ଖାଲି);

    // Remove any old labels.
    ଯେତେବେଳେ (nr <= last_page)
    {
	ଯଦି (XmNotebookGetPageInfo(tab_ଧାଡ଼ି, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_ସଂଖ୍ୟା == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(page_info.major_tab_widget, XmNuserData, &beval, ଖାଲି);
	    ଯଦି (beval != ଖାଲି)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Nlast_Page_ସଂଖ୍ୟା, tab_ଗଣନା, ଖାଲି);

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Ncurrent_Page_ସଂଖ୍ୟା, &currentpage, ଖାଲି);
    ଯଦି (currentpage != curtabidx)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Ncurrent_Page_ସଂଖ୍ୟା, curtabidx, ଖାଲି);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ଶୂନ୍ୟ
gui_mch_set_curtab(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		currentpage;

    ଯଦି (tab_ଧାଡ଼ି == (Widget)0)
	ଫେରନ୍ତୁ;

    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Ncurrent_Page_ସଂଖ୍ୟା, &currentpage, ଖାଲି);
    ଯଦି (currentpage != nr)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(tab_ଧାଡ଼ି, Xm_Ncurrent_Page_ସଂଖ୍ୟା, nr, ଖାଲି);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_motif_menu_colors(Widget id)
{
    ଯଦି (gui.menu_bg_pixel != INVALCOLOR)
#ଯଦି (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#ଅନ୍ୟଥା
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNbackground, gui.menu_bg_pixel, ଖାଲି);
#endif
    ଯଦି (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNforeground, gui.menu_fg_pixel, ଖାଲି);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_motif_scroll_colors(Widget id)
{
    ଯଦି (gui.scroll_bg_pixel != INVALCOLOR)
#ଯଦି (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#ଅନ୍ୟଥା
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNbackground, gui.scroll_bg_pixel, ଖାଲି);
#endif
    ଯଦି (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNforeground, gui.scroll_fg_pixel, ଖାଲି);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    ଶୂନ୍ୟ
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    ଯଦି (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	ଯଦି (fl != ଖାଲି)
	{
	    ଯଦି (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ଅନ୍ୟଥା
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
	    XmFontListFree(fl);
	}
    }
# ଅନ୍ୟଥା
    ଯଦି (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	ଯଦି (fl != ଖାଲି)
	{
	    ଯଦି (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    ଅନ୍ୟଥା
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(id, XmNfontList, fl, ଖାଲି);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା _SharedFindReplace
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

ସ୍ଥିର SharedFindReplace find_widgets = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};
ସ୍ଥିର SharedFindReplace repl_widgets = {ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି};

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    ଯଦି (cd != ଖାଲି)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    ଯଦି (cd != ଖାଲି)
	XtUnmanageChild(cd->dialog);
}

    ସ୍ଥିର ଶୂନ୍ୟ
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    ଅକ୍ଷର	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    ଯଦି (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	ଫେରନ୍ତୁ;
    }

    // Get the search/replace strings from the dialog
    ଯଦି (flags == FRD_FINDNEXT)
    {
	repl_text = ଖାଲି;
	sfr = &find_widgets;
    }
    ଅନ୍ୟଥା
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(sfr->down, XmNset, &direction_down, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(sfr->wword, XmNset, &wword, ଖାଲି);
    Xt_Va_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(sfr->mcase, XmNset, &mcase, ଖାଲି);
    ଯଦି (wword)
	flags |= FRD_WHOLE_WORD;
    ଯଦି (mcase)
	flags |= FRD_MATCH_CASE;

    (ଶୂନ୍ୟ)gui_do_findrepl((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    ଯଦି (find_text != ଖାଲି)
	XtFree(find_text);
    ଯଦି (repl_text != ଖାଲି)
	XtFree(repl_text);
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    ଚାବି_Sym keysym;

    ଯଦି (frdp == ଖାଲି)
	ଫେରନ୍ତୁ;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    ଯଦି (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_label(Widget w, ଅକ୍ଷର *label)
{
    XmString	str;
    char_u	*p, *next;
    ଚାବି_Sym	mnemonic = NUL;

    ଯଦି (!w)
	ଫେରନ୍ତୁ;

    p = vim_strsave((char_u *)label);
    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ;
    ପାଇଁ (next = p; *next; ++next)
    {
	ଯଦି (*next == DLG_HOTKEY_CHAR)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = STRLEN(next);

	    ଯଦି (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((ଅକ୍ଷର *)p);
    vim_free(p);
    ଯଦି (str)
    {
	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ଖାଲି);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_dialog_create(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		ନିବେଶ_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n;
    Arg			args[6];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			wword = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    ଯଦି (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	ଯଦି (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	ଅନ୍ୟଥା
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	ଯଦି (entry_text != ଖାଲି)
	    XmTextFieldSetString(frdp->what, (ଅକ୍ଷର *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(frdp->wword, XmNset, wword, ଖାଲି);
	ଫେରନ୍ତୁ;
    }

    // Create a fresh new dialog window
    ଯଦି (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    ଅନ୍ୟଥା
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
	    ଖାଲି);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_ଠେଲ_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ଖାଲି);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    ଯଦି (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ଠେଲ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ଖାଲି);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ଠେଲ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ଖାଲି);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ଠେଲ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ଖାଲି);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ଠେଲ_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ଖାଲି);
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

    ନିବେଶ_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ଖାଲି);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ନିବେଶ_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		ଖାଲି);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, ନିବେଶ_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ଖାଲି);

	ଯଦି (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	ନିବେଶ_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ଖାଲି);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ନିବେଶ_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ଖାଲି);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, ଚାବି_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	ଅନ୍ୟଥା
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, ଚାବି_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(label_what, args, n);
	widest = width;
	ଯଦି (do_replace)
	{
	    Xt_Get_ମୂଲ୍ୟଗୁଡ଼ିକ(label_with, args, n);
	    ଯଦି (width > widest)
		widest = width;
	}

	Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(frdp->what, XmNleftOffset, widest, ଖାଲି);
	ଯଦି (do_replace)
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(frdp->with, XmNleftOffset, widest, ଖାଲି);

    }

    XtManageChild(ନିବେଶ_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ନିବେଶ_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ନିବେଶ_form,
		ଖାଲି);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		ଖାଲି);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)ଖାଲି, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ଖାଲି);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ଖାଲି);
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
	    XmNtopWidget, ନିବେଶ_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    ଖାଲି);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    ଖାଲି);
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
	    ଖାଲି);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    ଯଦି (entry_text != ଖାଲି)
	XmTextFieldSetString(frdp->what, (ଅକ୍ଷର *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   ଶୂନ୍ୟ
gui_mch_find_dialog(exarg_T *eap)
{
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

    find_replace_dialog_create(eap->arg, FALSE);
}


    ଶୂନ୍ୟ
gui_mch_replace_dialog(exarg_T *eap)
{
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    ଶୂନ୍ୟ
gui_motif_synch_fonts(ଶୂନ୍ୟ)
{
    SharedFindReplace *frdp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    ଯଦି (font == ଖାଲି)
	ଫେରନ୍ତୁ;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    ପାଇଁ (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	ଯଦି (frdp->dialog)
	{
	    Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(frdp->what, XmNfontList, font_list, ଖାଲି);
	    ଯଦି (do_replace)
		Xt_Va_Set_ମୂଲ୍ୟଗୁଡ଼ିକ(frdp->with, XmNfontList, font_list, ଖାଲି);
	}
    }

    XmFontListFree(font_list);
}
