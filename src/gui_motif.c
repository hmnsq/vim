/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#അന്തര്ഭാവ "vim.h"

#അന്തര്ഭാവ <Xm/Form.h>
#അന്തര്ഭാവ <Xm/RowColumn.h>
#അന്തര്ഭാവ <Xm/PushB.h>
#അന്തര്ഭാവ <Xm/Text.h>
#അന്തര്ഭാവ <Xm/TextF.h>
#അന്തര്ഭാവ <Xm/Separator.h>
#അന്തര്ഭാവ <Xm/Label.h>
#അന്തര്ഭാവ <Xm/CascadeB.h>
#അന്തര്ഭാവ <Xm/ScrollBar.h>
#അന്തര്ഭാവ <Xm/MenuShell.h>
#അന്തര്ഭാവ <Xm/DrawingA.h>
#എങ്കിൽ (XmVersion >= 1002)
# include <Xm/RepType.h>
#endif
#അന്തര്ഭാവ <Xm/Frame.h>
#അന്തര്ഭാവ <Xm/LabelG.h>
#അന്തര്ഭാവ <Xm/ToggleBG.h>
#അന്തര്ഭാവ <Xm/SeparatoG.h>
#അന്തര്ഭാവ <Xm/XmP.h>

#അന്തര്ഭാവ <X11/keysym.h>
#അന്തര്ഭാവ <X11/Xatom.h>
#അന്തര്ഭാവ <X11/StringDefs.h>
#അന്തര്ഭാവ <X11/Intrinsic.h>
#ifdef HAVE_X11_XPM_H
# എങ്കിൽ defined(VMS)
#  include <xpm.h>
# അല്ലെങ്കിൽ
#  include <X11/xpm.h>
# endif
#അല്ലെങ്കിൽ
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#endif
#ifdef HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#endif

#അന്തര്ഭാവ "gui_xmebw.h"	// for our Enhanced Button Widget

#എങ്കിൽ defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#endif

#define MOTIF_POPUP

ബാഹ്യ Widget vimShell;

സ്ഥിതം Widget vimForm;
സ്ഥിതം Widget textAreaForm;
Widget textArea;
#ifdef FEAT_TOOLBAR
സ്ഥിതം Widget toolBarFrame;
സ്ഥിതം Widget toolBar;
#endif
#ifdef FEAT_GUI_TABLINE
സ്ഥിതം Widget	tab_വരി;
സ്ഥിതം Widget	tab_വരി_menu = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ	showing_tabline = 0;
#endif
#ifdef FEAT_MENU
# എങ്കിൽ (XmVersion >= 1002)
// remember the last set value for the tearoff item
സ്ഥിതം പൂർണ്ണസംഖ്യ tearoff_val = (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED;
# endif
സ്ഥിതം Widget menuBar;
#endif

#ifdef FEAT_TOOLBAR
സ്ഥിതം ശൂന്യം reset_focus(ശൂന്യം);
#endif

സ്ഥിതം ശൂന്യം gui_motif_menu_colors(Widget id);
സ്ഥിതം ശൂന്യം gui_motif_scroll_colors(Widget id);

#എങ്കിൽ (XmVersion >= 1002)
# define STRING_TAG  XmFONTLIST_DEFAULT_TAG
#അല്ലെങ്കിൽ
# define STRING_TAG  XmSTRING_DEFAULT_CHARSET
#endif

/*
 * Call-back routines.
 */

    സ്ഥിതം ശൂന്യം
scroll_cb(Widget w UNUSED, XtPointer client_data, XtPointer call_data)
{
    scrollbar_T *sb;
    long	മൂല്യം;
    പൂർണ്ണസംഖ്യ		dragging;

    sb = gui_find_scrollbar((long)client_data);

    മൂല്യം = ((XmScrollBarCallbackStruct *)call_data)->മൂല്യം;
    dragging = (((XmScrollBarCallbackStruct *)call_data)->reason ==
							      (പൂർണ്ണസംഖ്യ)XmCR_DRAG);
    gui_drag_scrollbar(sb, മൂല്യം, dragging);
}

#ifdef FEAT_GUI_TABLINE
    സ്ഥിതം ശൂന്യം
tabline_cb(
    Widget	w UNUSED,
    XtPointer	client_data UNUSED,
    XtPointer	call_data)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_data;
    എങ്കിൽ (nptr->reason != (പൂർണ്ണസംഖ്യ)XmCR_NONE)
	send_tabline_event(nptr->page_സംഖ്യ);
}

    സ്ഥിതം ശൂന്യം
tabline_button_cb(
    Widget	w,
    XtPointer	client_data UNUSED,
    XtPointer	call_data UNUSED)
{
    XtPointer	cmd, tab_idx;

    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNuserData, &cmd, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി_menu, XmNuserData, &tab_idx, ശൂന്യ);

    send_tabline_menu_event((പൂർണ്ണസംഖ്യ)(long)tab_idx, (പൂർണ്ണസംഖ്യ)(long)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    സ്ഥിതം ശൂന്യം
motif_tabline_timer_cb (
    XtPointer		timed_out,
    XtIntervalId	*interval_id UNUSED)
{
    *((പൂർണ്ണസംഖ്യ *)timed_out) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
tabline_scroller_clicked(
    അക്ഷരം		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	width, height;

    tab_scroll_w = XtNameToWidget(tab_വരി, scroller_name);
    എങ്കിൽ (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_മൂല്യങ്ങൾ(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &width, XmNheight, &height, ശൂന്യ);
	എങ്കിൽ (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    എങ്കിൽ (event->x >= pos_x && event->x <= pos_x + width
		    && event->y >= pos_y && event->y <= pos_y + height)
		// Clicked on the scroller
		തിരികെ TRUE;
	}
    }
    തിരികെ FALSE;
}

    സ്ഥിതം ശൂന്യം
tabline_menu_cb(
    Widget	w,
    XtPointer	closure UNUSED,
    XEvent	*e,
    Boolean	*continue_dispatch UNUSED)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    പൂർണ്ണസംഖ്യ				tab_idx = 0;
    WidgetList			children;
    Cardinal			അക്കം_Children;
    സ്ഥിതം XtIntervalId		timer = (XtIntervalId)0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ			timed_out = TRUE;

    event = (XButtonPressedEvent *)e;

    എങ്കിൽ (event->button == Button1)
    {
	എങ്കിൽ (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    തിരികെ;

	എങ്കിൽ (!timed_out)
	{
	    XtRemoveTimeOut(timer);
	    timed_out = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = XtAppAddTimeOut(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_out);
	    timed_out = FALSE;
	}
	തിരികെ;
    }

    എങ്കിൽ (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി_menu, XmNuserData, &tab_idx, ശൂന്യ);
	send_tabline_menu_event(tab_idx, (പൂർണ്ണസംഖ്യ)TABLINE_MENU_CLOSE);
	തിരികെ;
    }

    എങ്കിൽ (event->button != Button3)
	തിരികെ;

    // When ignoring events don't show the menu.
    എങ്കിൽ (hold_gui_events || cmdwin_type != 0)
	തിരികെ;

    എങ്കിൽ (event->subwindow != None)
    {
	tab_w = XtWindowToWidget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	എങ്കിൽ (tab_w != (Widget)0 && Xm_Is_തള്ളുക_Button(tab_w))
	    Xt_Va_Get_മൂല്യങ്ങൾ(tab_w, Xm_Npage_സംഖ്യ, &tab_idx, ശൂന്യ);
    }

    Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി_menu, XmNuserData, (XtPointer)(long)tab_idx, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി_menu, XmNchildren, &children, XmNnumChildren,
		  &അക്കം_Children, ശൂന്യ);
    XtManageChildren(children, അക്കം_Children);
    XmMenuPosition(tab_വരി_menu, (XButtonPressedEvent *)e) ;
    XtManageChild(tab_വരി_menu);
}

    സ്ഥിതം ശൂന്യം
tabline_balloon_cb(BalloonEval *beval, പൂർണ്ണസംഖ്യ state UNUSED)
{
    പൂർണ്ണസംഖ്യ		nr;
    tabpage_T	*tp;

    എങ്കിൽ (beval->target == (Widget)0)
	തിരികെ;

    Xt_Va_Get_മൂല്യങ്ങൾ(beval->target, Xm_Npage_സംഖ്യ, &nr, ശൂന്യ);
    tp = find_tabpage(nr);
    എങ്കിൽ (tp == ശൂന്യ)
	തിരികെ;

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

#അന്തര്ഭാവ <Xm/XmP.h>
#അന്തര്ഭാവ <Xm/LabelP.h>

സ്ഥിതം XtExposeProc old_label_expose = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    unsigned അക്ഷരം   label_type = (പൂർണ്ണസംഖ്യ)XmSTRING;

    Xt_Va_Get_മൂല്യങ്ങൾ(_w, XmNlabelType, &label_type, (XtPointer)0);

    എങ്കിൽ (XtIsSensitive(_w) || label_type != (പൂർണ്ണസംഖ്യ)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    അല്ലെങ്കിൽ
    {
	XGCValues   മൂല്യങ്ങൾ;
	XtGCMask    mask;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	mask = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClipMask | GCClipXOrigin | GCClipYOrigin;
	മൂല്യങ്ങൾ.graphics_exposures = False;

	എങ്കിൽ (fs != 0)
	{
	    mask |= GCFont;
	    മൂല്യങ്ങൾ.font = fs->fid;
	}

	എങ്കിൽ (lw->primitive.മുകൾ_shadow_pixmap != None
		&& lw->primitive.മുകൾ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    mask |= GCFillStyle | GCTile;
	    മൂല്യങ്ങൾ.fill_style = FillTiled;
	    മൂല്യങ്ങൾ.tile = lw->primitive.മുകൾ_shadow_pixmap;
	}

	lw->label.TextRect.x += 1;
	lw->label.TextRect.y += 1;
	എങ്കിൽ (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x += 1;
	    lw->label.acc_TextRect.y += 1;
	}

	മൂല്യങ്ങൾ.foreground = lw->primitive.മുകൾ_shadow_color;
	മൂല്യങ്ങൾ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, mask,
					       &മൂല്യങ്ങൾ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.TextRect.x -= 1;
	lw->label.TextRect.y -= 1;
	എങ്കിൽ (lw->label._acc_text != 0)
	{
	    lw->label.acc_TextRect.x -= 1;
	    lw->label.acc_TextRect.y -= 1;
	}

	മൂല്യങ്ങൾ.foreground = lw->primitive.bottom_shadow_color;
	മൂല്യങ്ങൾ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, mask,
					       &മൂല്യങ്ങൾ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    ശൂന്യം
gui_x11_create_widgets(ശൂന്യം)
{
#ifdef FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		args[10];
    പൂർണ്ണസംഖ്യ		n;
    XmString	xms;
#endif

    /*
     * Install the 3D shade effect drawing routines.
     */
    എങ്കിൽ (old_label_expose == ശൂന്യ)
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
#എങ്കിൽ (XmVersion >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#endif

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_Set_മൂല്യങ്ങൾ(vimShell, XmNdeleteResponse, XmDO_NOTHING, ശൂന്യ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	XmNborderWidth, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	XmNmarginWidth, 0,
	XmNmarginHeight, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ശൂന്യ);
    gui_motif_menu_colors(vimForm);

#ifdef FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	പൂർണ്ണസംഖ്യ ac = 0;

# എങ്കിൽ (XmVersion >= 1002)
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
	ശൂന്യ);
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
	ശൂന്യ);
    gui_motif_menu_colors(toolBar);

#endif

#ifdef FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    XtSetArg(args[n], XmNbindingType, XmNONE); n++;
    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
    XtSetArg(args[n], XmNbackPageSize, XmNONE); n++;
    XtSetArg(args[n], Xm_Nback_Page_സംഖ്യ, 0); n++;
    XtSetArg(args[n], XmNbackPagePlacement, Xm_മുകൾ_വലത്); n++;
    XtSetArg(args[n], XmNmajorTabSpacing, 0); n++;
    XtSetArg(args[n], XmNshadowThickness, 0); n++;
    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_വരി = XmCreateNotebook(vimForm, "Vim tabline", args, n);

    XtAddCallback(tab_വരി, XmNpageChangedCallback, (XtCallbackProc)tabline_cb,
			ശൂന്യ);
    XtAddEventHandler(tab_വരി, ButtonPressMask, False,
			(XtEventHandler)tabline_menu_cb, ശൂന്യ);

    gui.tabline_height = TABLINE_HEIGHT;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = XtNameToWidget(tab_വരി, "MinorTabScrollerNext");
    Xt_Va_Set_മൂല്യങ്ങൾ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ശൂന്യ);
    scroller = XtNameToWidget(tab_വരി, "MinorTabScrollerPrevious");
    Xt_Va_Set_മൂല്യങ്ങൾ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ശൂന്യ);

    // Create the tabline popup menu
    tab_വരി_menu = XmCreatePopupMenu(tab_വരി, "tabline popup", ശൂന്യ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((അക്ഷരം *)"Close tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_തള്ളുക_Button(tab_വരി_menu, "Close", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ശൂന്യ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((അക്ഷരം *)"New Tab", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_തള്ളുക_Button(tab_വരി_menu, "New Tab", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ശൂന്യ);
    XmStringFree(xms);

    n = 0;
    XtSetArg(args[n], XmNuserData, (XtPointer)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((അക്ഷരം *)"Open tab...", STRING_TAG);
    XtSetArg(args[n], XmNlabelString, xms); n++;
    button = Xm_Create_തള്ളുക_Button(tab_വരി_menu, "Open tab...", args, n);
    XtAddCallback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ശൂന്യ);
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
	ശൂന്യ);
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
	ശൂന്യ);

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
    ശൂന്യം
gui_x11_destroy_widgets(ശൂന്യം)
{
    textArea = ശൂന്യ;
#ifdef FEAT_MENU
    menuBar = ശൂന്യ;
#endif
}

    ശൂന്യം
gui_mch_set_text_area_pos(
    പൂർണ്ണസംഖ്യ	    x UNUSED,
    പൂർണ്ണസംഖ്യ	    y UNUSED,
    പൂർണ്ണസംഖ്യ	    w UNUSED,
    പൂർണ്ണസംഖ്യ	    h UNUSED)
{
#ifdef FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#endif
}

    ശൂന്യം
gui_x11_set_back_color(ശൂന്യം)
{
    എങ്കിൽ (textArea != ശൂന്യ)
#എങ്കിൽ (XmVersion >= 1002)
	XmChangeColor(textArea, gui.back_pixel);
#അല്ലെങ്കിൽ
	Xt_Va_Set_മൂല്യങ്ങൾ(textArea,
		  XmNbackground, gui.back_pixel,
		  ശൂന്യ);
#endif
}

/*
 * Manage dialog centered on pointer.
 */
    ശൂന്യം
manage_centered(Widget dialog_child)
{
    Widget shell = XtParent(dialog_child);
    Window വേര്, child;
    unsigned പൂർണ്ണസംഖ്യ mask;
    unsigned പൂർണ്ണസംഖ്യ width, height, border_width, depth;
    പൂർണ്ണസംഖ്യ x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_മൂല്യങ്ങൾ(shell, XmNmappedWhenManaged, &mappedWhenManaged, ശൂന്യ);
    Xt_Va_Set_മൂല്യങ്ങൾ(shell, XmNmappedWhenManaged, False, ശൂന്യ);

    XtManageChild(dialog_child);

    // Get the pointer position (x, y)
    XQueryPointer(XtDisplay(shell), XtWindow(shell), &വേര്, &child,
		  &x, &y, &win_x, &win_y, &mask);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), XtWindow(shell), &വേര്, &win_x, &win_y,
		 &width, &height, &border_width, &depth);
    width += 2 * border_width;
    height += 2 * border_width;
    x -= width / 2;
    y -= height / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->width - width;
    maxY = XtScreen(shell)->height - height;
    എങ്കിൽ (x < 0)
	x = 0;
    എങ്കിൽ (x > maxX)
	x = maxX;
    എങ്കിൽ (y < 0)
	y = 0;
    എങ്കിൽ (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_Set_മൂല്യങ്ങൾ(shell, XmNx, x, XmNy, y, ശൂന്യ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_Set_മൂല്യങ്ങൾ(shell, XmNmappedWhenManaged, mappedWhenManaged, ശൂന്യ);
}

#എങ്കിൽ defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# എങ്കിൽ (XmVersion <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_TAG);
# അല്ലെങ്കിൽ
    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG, XmFONT_IS_FONT,
					    (XtPointer)font);
    font_list = XmFontListAppendEntry(ശൂന്യ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
# endif
    തിരികെ font_list;
}

# എങ്കിൽ ((XmVersion > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFontSet *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    XmFontListEntry font_list_entry;

    font_list_entry = XmFontListEntryCreate(STRING_TAG,
					    XmFONT_IS_FONTSET,
					    (XtPointer)*fontset);
    font_list = XmFontListAppendEntry(ശൂന്യ, font_list_entry);
    XmFontListEntryFree(&font_list_entry);
    തിരികെ font_list;
}
# endif

#endif

#എങ്കിൽ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

സ്ഥിതം ശൂന്യം gui_motif_add_actext(vimmenu_T *menu);
# എങ്കിൽ (XmVersion >= 1002)
സ്ഥിതം ശൂന്യം toggle_tearoff(Widget wid);
സ്ഥിതം ശൂന്യം gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
സ്ഥിതം ശൂന്യം submenu_change(vimmenu_T *mp, പൂർണ്ണസംഖ്യ colors);

സ്ഥിതം ശൂന്യം do_set_mnemonics(പൂർണ്ണസംഖ്യ enable);
സ്ഥിതം പൂർണ്ണസംഖ്യ menu_enabled = TRUE;

    ശൂന്യം
gui_mch_enable_menu(പൂർണ്ണസംഖ്യ flag)
{
    എങ്കിൽ (flag)
    {
	XtManageChild(menuBar);
# ifdef FEAT_TOOLBAR
	എങ്കിൽ (XtIsManaged(XtParent(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_Set_മൂല്യങ്ങൾ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ശൂന്യ);
#  ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
#  endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ശൂന്യ);
	}
	അല്ലെങ്കിൽ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ശൂന്യ);
	}
    }
    അല്ലെങ്കിൽ
    {
	XtUnmanageChild(menuBar);
# ifdef FEAT_TOOLBAR
	എങ്കിൽ (XtIsManaged(XtParent(toolBar)))
	{
	    Xt_Va_Set_മൂല്യങ്ങൾ(XtParent(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ശൂന്യ);
#  ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
#  endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar),
			      ശൂന്യ);
	}
	അല്ലെങ്കിൽ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_FORM,
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ശൂന്യ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    ശൂന്യം
gui_motif_set_mnemonics(പൂർണ്ണസംഖ്യ enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    എങ്കിൽ (!menu_enabled)
	enable = FALSE;
    do_set_mnemonics(enable);
}

    സ്ഥിതം ശൂന്യം
do_set_mnemonics(പൂർണ്ണസംഖ്യ enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	എങ്കിൽ (menu->id != (Widget)0)
	    Xt_Va_Set_മൂല്യങ്ങൾ(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ശൂന്യ);
}

    ശൂന്യം
gui_mch_add_menu(vimmenu_T *menu, പൂർണ്ണസംഖ്യ idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*parent = menu->parent;

# ifdef MOTIF_POPUP
    എങ്കിൽ (menu_is_popup(menu->name))
    {
	Arg arg[2];
	പൂർണ്ണസംഖ്യ n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  എങ്കിൽ (XmVersion <= 1002)
	എങ്കിൽ (mouse_model_popup())
#  endif
	{
	    എങ്കിൽ (gui.menu_bg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    എങ്കിൽ (gui.menu_fg_pixel != INVALCOLOR)
	    {
		XtSetArg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(textArea, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	തിരികെ;
    }
# endif

    എങ്കിൽ (!menu_is_menubar(menu->name)
	    || (parent != ശൂന്യ && parent->submenu_id == (Widget)0))
	തിരികെ;

    label = XmStringCreate((അക്ഷരം *)menu->dname, STRING_TAG);
    എങ്കിൽ (label == ശൂന്യ)
	തിരികെ;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (parent == ശൂന്യ) ? menuBar : parent->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# എങ്കിൽ (XmVersion >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    XmNpositionIndex, idx + (parent != ശൂന്യ
			   && tearoff_val == (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ശൂന്യ);
    XmStringFree(label);

    എങ്കിൽ (menu->id == (Widget)0)		// failed
	തിരികെ;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    എങ്കിൽ (parent == ശൂന്യ && menu->priority >= 9999)
	Xt_Va_Set_മൂല്യങ്ങൾ(menuBar,
		XmNmenuHelpWidget, menu->id,
		ശൂന്യ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_add_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ശൂന്യ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xmRowColumnWidgetClass, shell,
	XmNrowColumnType, XmMENU_PULLDOWN,
	ശൂന്യ);
    gui_motif_menu_colors(menu->submenu_id);

    എങ്കിൽ (menu->submenu_id == (Widget)0)		// failed
	തിരികെ;

# എങ്കിൽ (XmVersion >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_Set_മൂല്യങ്ങൾ(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ശൂന്യ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    എങ്കിൽ (parent == ശൂന്യ)
	gui_mch_compute_menu_height(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    സ്ഥിതം ശൂന്യം
gui_motif_add_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    എങ്കിൽ (menu->actext == ശൂന്യ || menu->id == (Widget)0)
	തിരികെ;

    label = XmStringCreate((അക്ഷരം *)menu->actext, STRING_TAG);
    എങ്കിൽ (label == ശൂന്യ)
	തിരികെ;
    Xt_Va_Set_മൂല്യങ്ങൾ(menu->id, XmNacceleratorText, label, ശൂന്യ);
    XmStringFree(label);
}

    ശൂന്യം
gui_mch_toggle_tearoffs(പൂർണ്ണസംഖ്യ enable)
{
# എങ്കിൽ (XmVersion >= 1002)
    എങ്കിൽ (enable)
	tearoff_val = (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED;
    അല്ലെങ്കിൽ
	tearoff_val = (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(വേര്_menu);
# endif
}

# എങ്കിൽ (XmVersion >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    സ്ഥിതം ശൂന്യം
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_Set_മൂല്യങ്ങൾ(wid, XmNtearOffModel, tearoff_val, ശൂന്യ);
    എങ്കിൽ (tearoff_val == (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    സ്ഥിതം ശൂന്യം
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    ആകുമ്പോൾ (menu != ശൂന്യ)
    {
	എങ്കിൽ (!menu_is_popup(menu->name))
	{
	    എങ്കിൽ (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->next;
    }
}
# endif

    പൂർണ്ണസംഖ്യ
gui_mch_text_area_extra_height(ശൂന്യം)
{
    Dimension	shadowHeight;

    Xt_Va_Get_മൂല്യങ്ങൾ(textAreaForm, XmNshadowThickness, &shadowHeight, ശൂന്യ);
    തിരികെ shadowHeight;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    ശൂന്യം
gui_mch_compute_menu_height(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    സ്ഥിതം Dimension	height = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    എങ്കിൽ (id != (Widget)0)
	Xt_Va_Get_മൂല്യങ്ങൾ(id, XmNheight, &height, ശൂന്യ);

    // Find any menu Widget, to be able to call XtManageChild()
    അല്ലെങ്കിൽ
	FOR_ALL_MENUS(mp)
	    എങ്കിൽ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		നിർത്തുക;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    എങ്കിൽ (id != (Widget)0)
	XtManageChild(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	എങ്കിൽ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(mp->id, XmNy, &y, ശൂന്യ);
	    എങ്കിൽ (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_മൂല്യങ്ങൾ(menuBar,
	XmNmarginHeight, &margin,
	XmNshadowThickness, &shadow,
	ശൂന്യ);

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
    എങ്കിൽ (gui.shell_created)
	Xt_Va_Set_മൂല്യങ്ങൾ(menuBar, XmNheight, gui.menu_height, ശൂന്യ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

സ്ഥിതം അക്ഷരം **get_toolbar_pixmap(vimmenu_T *menu, അക്ഷരം **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_xpm(char_u *path)
{
    XpmAttributes attrs;
    പൂർണ്ണസംഖ്യ		status;
    Pixmap	mask;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    status = XpmReadFileToPixmap(gui.dpy,
	    വേര്_Window(gui.dpy, DefaultScreen(gui.dpy)),
	    (അക്ഷരം *)path, &map, &mask, &attrs);
    XpmFreeAttributes(&attrs);

    എങ്കിൽ (status == XpmSuccess)
	തിരികെ OK;
    തിരികെ FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    സ്ഥിതം അക്ഷരം **
get_toolbar_pixmap(vimmenu_T *menu, അക്ഷരം **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    അക്ഷരം	**xpm = ശൂന്യ;		// xpm array
    പൂർണ്ണസംഖ്യ		res;

    *fname = ശൂന്യ;
    buf[0] = NUL;			// start with NULL path

    എങ്കിൽ (menu->iconfile != ശൂന്യ)
    {
	// Use the "icon="  argument.
	gui_find_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	എങ്കിൽ (res == FAIL && gui_find_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	എങ്കിൽ (res == OK)
	{
	    *fname = (അക്ഷരം *)vim_strsave(buf);
	    തിരികെ tb_blank_xpm;
	}
    }

    എങ്കിൽ (menu->icon_builtin || gui_find_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	എങ്കിൽ (menu->iconidx >= 0 && menu->iconidx
	       < (പൂർണ്ണസംഖ്യ)(വലുപ്പം(built_in_pixmaps) / വലുപ്പം(built_in_pixmaps[0])))
	    xpm = built_in_pixmaps[menu->iconidx];
	അല്ലെങ്കിൽ
	    xpm = tb_blank_xpm;
    }

    തിരികെ xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
add_pixmap_args(vimmenu_T *menu, Arg *args, പൂർണ്ണസംഖ്യ n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    എങ്കിൽ (menu->xpm == ശൂന്യ)
    {
	XtSetArg(args[n], XmNlabelType, XmSTRING); n++;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (menu->xpm_fname != ശൂന്യ)
	{
	    XtSetArg(args[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	XtSetArg(args[n], XmNpixmapData, menu->xpm); n++;
	XtSetArg(args[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    തിരികെ n;
}
# endif // FEAT_TOOLBAR

    ശൂന്യം
gui_mch_add_menu_item(vimmenu_T *menu, പൂർണ്ണസംഖ്യ idx)
{
    XmString	label;
    vimmenu_T	*parent = menu->parent;

# എങ്കിൽ (XmVersion <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    എങ്കിൽ (menu_is_child_of_popup(menu) && !mouse_model_popup())
	തിരികെ;
# endif

# ifdef FEAT_TOOLBAR
    എങ്കിൽ (menu_is_toolbar(parent->name))
    {
	WidgetClass	type;
	XmString	xms = ശൂന്യ;    // fallback label if pixmap not found
	പൂർണ്ണസംഖ്യ		n;
	Arg		args[18];

	n = 0;
	എങ്കിൽ (menu_is_separator(menu->name))
	{
	    അക്ഷരം	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (അക്ഷരം *)vim_strchr(menu->name, ':');
	    എങ്കിൽ (cp != ശൂന്യ)
		wid = (Dimension)atoi(++cp);
	    അല്ലെങ്കിൽ
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    XtSetArg(args[n], XmNwidth, wid); n++;
	    XtSetArg(args[n], XmNminWidth, wid); n++;
	    XtSetArg(args[n], XmNorientation, XmVERTICAL); n++;
	    XtSetArg(args[n], XmNseparatorType, XmSHADOW_ETCHED_IN); n++;
	}
	അല്ലെങ്കിൽ
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
	    xms = XmStringCreate((അക്ഷരം *)menu->dname, STRING_TAG);
	    XtSetArg(args[n], XmNlabelString, xms); n++;

	    n = add_pixmap_args(menu, args, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	XtSetArg(args[n], XmNpositionIndex, idx); n++;
	എങ്കിൽ (menu->id == ശൂന്യ)
	{
	    menu->id = XtCreateManagedWidget((അക്ഷരം *)menu->dname,
			type, toolBar, args, n);
	    എങ്കിൽ (menu->id != ശൂന്യ && type == xmEnhancedButtonWidgetClass)
	    {
		XtAddCallback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	അല്ലെങ്കിൽ
	    Xt_Set_മൂല്യങ്ങൾ(menu->id, args, n);
	എങ്കിൽ (xms != ശൂന്യ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_set_tip(menu);
#  endif

	menu->parent = parent;
	menu->submenu_id = ശൂന്യ;
	// When adding first item to toolbar it might have to be enabled .
	എങ്കിൽ (!XtIsManaged(XtParent(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_height = gui_mch_compute_toolbar_height();
	തിരികെ;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    എങ്കിൽ (parent->submenu_id == (Widget)0)
	തിരികെ;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    എങ്കിൽ (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, parent->submenu_id,
# എങ്കിൽ (XmVersion >= 1002)
		// count the tearoff item (needed for LessTif)
		XmNpositionIndex, idx + (tearoff_val == (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ശൂന്യ);
	gui_motif_menu_colors(menu->id);
	തിരികെ;
    }

    label = XmStringCreate((അക്ഷരം *)menu->dname, STRING_TAG);
    എങ്കിൽ (label == ശൂന്യ)
	തിരികെ;
    menu->id = XtVaCreateWidget("subMenu",
	xm_തള്ളുക_Button_Widget_Class, parent->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# എങ്കിൽ (XmVersion >= 1002)
	// count the tearoff item (needed for LessTif)
	XmNpositionIndex, idx + (tearoff_val == (പൂർണ്ണസംഖ്യ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ശൂന്യ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    എങ്കിൽ (menu->id != (Widget)0)
    {
	XtAddCallback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(XtPointer)menu);
	// add accelerator text
	gui_motif_add_actext(menu);
    }
}

# എങ്കിൽ (XmVersion <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    ശൂന്യം
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    പൂർണ്ണസംഖ്യ		idx = 0;

    // When GUI hasn't started the menus have not been created.
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    ആകുമ്പോൾ (menu)
    {
      എങ്കിൽ (menu->children != ശൂന്യ)
      {
	  എങ്കിൽ (menu_is_popup(menu->name))
	  {
	      എങ്കിൽ (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_add_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      അല്ലെങ്കിൽ
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      അല്ലെങ്കിൽ എങ്കിൽ (menu_is_child_of_popup(menu))
      {
	  എങ്കിൽ (mouse_model_popup())
	      gui_mch_add_menu_item(menu, idx);
	  അല്ലെങ്കിൽ
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->next;
      ++idx;
    }
}
# endif

    ശൂന്യം
gui_mch_new_menu_colors(ശൂന്യം)
{
    എങ്കിൽ (menuBar == (Widget)0)
	തിരികെ;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(toolBarFrame);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(വേര്_menu, TRUE);
}

    ശൂന്യം
gui_mch_new_menu_font(ശൂന്യം)
{
    എങ്കിൽ (menuBar == (Widget)0)
	തിരികെ;
    submenu_change(വേര്_menu, FALSE);
    {
	Dimension   height;
	Position w, h;

	Xt_Va_Get_മൂല്യങ്ങൾ(menuBar, XmNheight, &height, ശൂന്യ);
	gui.menu_height = height;

	Xt_Va_Get_മൂല്യങ്ങൾ(vimShell, XtNwidth, &w, XtNheight, &h, ശൂന്യ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_status_area_height()
# endif
		     );
    }
    gui_set_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# എങ്കിൽ defined(FEAT_BEVAL_GUI)
    ശൂന്യം
gui_mch_new_tooltip_font(ശൂന്യം)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    എങ്കിൽ (toolBar == (Widget)0)
	തിരികെ;

    menu = gui_find_menu((char_u *)"ToolBar");
    എങ്കിൽ (menu != ശൂന്യ)
	submenu_change(menu, FALSE);
#  endif
}

    ശൂന്യം
gui_mch_new_tooltip_colors(ശൂന്യം)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    എങ്കിൽ (toolBar == (Widget)0)
	തിരികെ;

    toolbar = gui_find_menu((char_u *)"ToolBar");
    എങ്കിൽ (toolbar != ശൂന്യ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    സ്ഥിതം ശൂന്യം
submenu_change(
    vimmenu_T	*menu,
    പൂർണ്ണസംഖ്യ		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    വേണ്ടി (mp = menu; mp != ശൂന്യ; mp = mp->next)
    {
	എങ്കിൽ (mp->id != (Widget)0)
	{
	    എങ്കിൽ (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		എങ്കിൽ (mp->xpm != ശൂന്യ)
		{
		    പൂർണ്ണസംഖ്യ		n = 0;
		    Arg		args[18];

		    n = add_pixmap_args(mp, args, n);
		    Xt_Set_മൂല്യങ്ങൾ(mp->id, args, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		എങ്കിൽ (mp->tip != ശൂന്യ)
		{
		    Arg args[2];

		    args[0].name = XmNbackground;
		    args[0].മൂല്യം = gui.tooltip_bg_pixel;
		    args[1].name = XmNforeground;
		    args[1].മൂല്യം = gui.tooltip_fg_pixel;
		    Xt_Set_മൂല്യങ്ങൾ(mp->tip->balloonLabel, &args[0], Xt_സംഖ്യ(args));
		}
#  endif
# endif
	    }
	    അല്ലെങ്കിൽ
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		എങ്കിൽ (mp->tip != ശൂന്യ)
		{
		    Arg args[1];

		    args[0].name = XmNfontList;
		    args[0].മൂല്യം = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_Set_മൂല്യങ്ങൾ(mp->tip->balloonLabel, &args[0], Xt_സംഖ്യ(args));
		}
# endif
	    }
	}

	എങ്കിൽ (mp->children != ശൂന്യ)
	{
# എങ്കിൽ (XmVersion >= 1002)
	    // Set the colors/font for the tear off widget
	    എങ്കിൽ (mp->submenu_id != (Widget)0)
	    {
		എങ്കിൽ (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		അല്ലെങ്കിൽ
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
    ശൂന്യം
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    എങ്കിൽ (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    എങ്കിൽ (menu->id == (Widget)0)
	തിരികെ;

    Widget	    parent;

    parent = XtParent(menu->id);
# എങ്കിൽ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    എങ്കിൽ (parent == toolBar && menu->tip != ശൂന്യ)
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
	menu->tip = ശൂന്യ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    എങ്കിൽ (parent == menuBar)
	gui_mch_compute_menu_height((Widget)0);
# ifdef FEAT_TOOLBAR
    അല്ലെങ്കിൽ എങ്കിൽ (parent == toolBar)
    {
	Cardinal    അക്കം_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_മൂല്യങ്ങൾ(toolBar, XmNnumChildren, &അക്കം_children, ശൂന്യ);
	എങ്കിൽ (അക്കം_children == 0)
	    gui_mch_show_toolbar(FALSE);
	അല്ലെങ്കിൽ
	    gui.toolbar_height = gui_mch_compute_toolbar_height();
    }
# endif
}

    ശൂന്യം
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
    ശൂന്യം
gui_mch_def_colors(ശൂന്യം)
{
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

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

    ശൂന്യം
gui_mch_set_scrollbar_thumb(
    scrollbar_T *sb,
    long	val,
    long	size,
    long	max)
{
    എങ്കിൽ (sb->id != (Widget)0)
	Xt_Va_Set_മൂല്യങ്ങൾ(sb->id,
		  XmNvalue, val,
		  XmNsliderSize, size,
		  XmNpageIncrement, (size > 2 ? size - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ശൂന്യ);
}

    ശൂന്യം
gui_mch_set_scrollbar_pos(
    scrollbar_T *sb,
    പൂർണ്ണസംഖ്യ		x,
    പൂർണ്ണസംഖ്യ		y,
    പൂർണ്ണസംഖ്യ		w,
    പൂർണ്ണസംഖ്യ		h)
{
    എങ്കിൽ (sb->id == (Widget)0)
	തിരികെ;

    എങ്കിൽ (sb->type == SBAR_ഇടത് || sb->type == SBAR_വലത്)
    {
	എങ്കിൽ (y == 0)
	    h -= gui.border_offset;
	അല്ലെങ്കിൽ
	    y -= gui.border_offset;
	Xt_Va_Set_മൂല്യങ്ങൾ(sb->id,
		XmNtopOffset, y,
		XmNbottomOffset, -y - h,
		XmNwidth, w,
		ശൂന്യ);
    }
    അല്ലെങ്കിൽ
	Xt_Va_Set_മൂല്യങ്ങൾ(sb->id,
		XmNtopOffset, y,
		XmNleftOffset, x,
		XmNrightOffset, gui.which_scrollbars[SBAR_വലത്]
						     ? gui.scrollbar_width : 0,
		XmNheight, h,
		ശൂന്യ);
    XtManageChild(sb->id);
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_xpadding(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_മൂല്യങ്ങൾ(textArea, XtNwidth, &tw, XtNx, &tx, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(vimShell, XtNwidth, &ww, ശൂന്യ);
    xpad = ww - tw - tx - gui.scrollbar_width;
    തിരികെ (xpad < 0) ? 0 : xpad;
}

    പൂർണ്ണസംഖ്യ
gui_mch_get_scrollbar_ypadding(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_മൂല്യങ്ങൾ(textArea, XtNheight, &th, XtNy, &ty, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(vimShell, XtNheight, &wh, ശൂന്യ);
    ypad = wh - th - ty - gui.scrollbar_height;
    തിരികെ (ypad < 0) ? 0 : ypad;
}

    ശൂന്യം
gui_mch_enable_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ flag)
{
    Arg		args[16];
    പൂർണ്ണസംഖ്യ		n;

    എങ്കിൽ (sb->id == (Widget)0)
	തിരികെ;

    n = 0;
    എങ്കിൽ (flag)
    {
	തിരഞ്ഞെടുപ്പ് (sb->type)
	{
	    അവസ്ഥ SBAR_ഇടത്:
		XtSetArg(args[n], XmNleftOffset, gui.scrollbar_width); n++;
		നിർത്തുക;

	    അവസ്ഥ SBAR_വലത്:
		XtSetArg(args[n], XmNrightOffset, gui.scrollbar_width); n++;
		നിർത്തുക;

	    അവസ്ഥ SBAR_BOTTOM:
		XtSetArg(args[n], XmNbottomOffset, gui.scrollbar_height);n++;
		നിർത്തുക;
	}
	Xt_Set_മൂല്യങ്ങൾ(textArea, args, n);
	XtManageChild(sb->id);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    തിരഞ്ഞെടുപ്പ് (sb->type)
	    {
		അവസ്ഥ SBAR_ഇടത്:
		    XtSetArg(args[n], XmNleftOffset, 0); n++;
		    നിർത്തുക;

		അവസ്ഥ SBAR_വലത്:
		    XtSetArg(args[n], XmNrightOffset, 0); n++;
		    നിർത്തുക;

		അവസ്ഥ SBAR_BOTTOM:
		    XtSetArg(args[n], XmNbottomOffset, 0);n++;
		    നിർത്തുക;
	    }
	    Xt_Set_മൂല്യങ്ങൾ(textArea, args, n);
	}
	XtUnmanageChild(sb->id);
    }
}

    ശൂന്യം
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    പൂർണ്ണസംഖ്യ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		args[16];
    പൂർണ്ണസംഖ്യ		n = 0;

    XtSetArg(args[n], XmNminimum, 0); n++;
    XtSetArg(args[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    തിരഞ്ഞെടുപ്പ് (sb->type)
    {
	അവസ്ഥ SBAR_ഇടത്:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    നിർത്തുക;

	അവസ്ഥ SBAR_വലത്:
	    XtSetArg(args[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNwidth, gui.scrollbar_width); n++;
	    നിർത്തുക;

	അവസ്ഥ SBAR_BOTTOM:
	    XtSetArg(args[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    XtSetArg(args[n], XmNheight, gui.scrollbar_height); n++;
	    നിർത്തുക;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, textAreaForm, args, n);
    എങ്കിൽ (sb->id == (Widget)0)
	തിരികെ;

    gui_mch_set_scrollbar_colors(sb);
    XtAddCallback(sb->id, XmNvalueChangedCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddCallback(sb->id, XmNdragCallback,
	    scroll_cb, (XtPointer)sb->ident);
    XtAddEventHandler(sb->id, താക്കോൽ_Press_Mask, FALSE, gui_x11_താക്കോൽ_hit_cb,
	    (XtPointer)0);
}

    ശൂന്യം
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    എങ്കിൽ (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    ശൂന്യം
gui_mch_set_scrollbar_colors(scrollbar_T *sb)
{
    എങ്കിൽ (sb->id != (Widget)0)
    {
	എങ്കിൽ (gui.scroll_bg_pixel != INVALCOLOR)
	{
#എങ്കിൽ (XmVersion>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#endif

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_Set_മൂല്യങ്ങൾ(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ശൂന്യ);
	}

	എങ്കിൽ (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_Set_മൂല്യങ്ങൾ(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ശൂന്യ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    എങ്കിൽ (sb == &gui.bottom_sbar && textAreaForm != (Widget)0)
	gui_motif_scroll_colors(textAreaForm);
}

/*
 * Miscellaneous stuff:
 */

    Window
gui_x11_get_wid(ശൂന്യം)
{
    തിരികെ(XtWindow(textArea));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    സ്ഥിതം ശൂന്യം
do_mnemonic(Widget w, unsigned പൂർണ്ണസംഖ്യ keycode)
{
    WidgetList	    children;
    പൂർണ്ണസംഖ്യ		    അക്കം_Children, i;
    Boolean	    isMenu;
    താക്കോൽ_Sym	    mnemonic = '\0';
    അക്ഷരം	    mneString[2];
    Widget	    userData;
    unsigned അക്ഷരം   rowColType;

    എങ്കിൽ (XtIsComposite(w))
    {
	എങ്കിൽ (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNrowColumnType, &rowColType, ശൂന്യ);
	    isMenu = (rowColType != (unsigned അക്ഷരം)XmWORK_AREA);
	}
	അല്ലെങ്കിൽ
	    isMenu = False;
	എങ്കിൽ (!isMenu)
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNchildren, &children, XmNnumChildren,
			  &അക്കം_Children, ശൂന്യ);
	    വേണ്ടി (i = 0; i < അക്കം_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    അല്ലെങ്കിൽ
    {
	Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNmnemonic, &mnemonic, ശൂന്യ);
	എങ്കിൽ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    എങ്കിൽ (XKeysymToKeycode(XtDisplay(XtParent(w)),
				       XStringToKeysym(mneString)) == keycode)
	    {
		എങ്കിൽ (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNuserData, &userData, ശൂന്യ);
		    എങ്കിൽ (userData != ശൂന്യ && XtIsWidget(userData))
			XmProcessTraversal(userData, XmTRAVERSE_CURRENT);
		}
		അല്ലെങ്കിൽ
		{
		    XKeyPressedEvent താക്കോൽ_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_FIELD(താക്കോൽ_Event);
		    താക്കോൽ_Event.type = താക്കോൽ_Press;
		    താക്കോൽ_Event.serial = 1;
		    താക്കോൽ_Event.send_event = True;
		    താക്കോൽ_Event.display = XtDisplay(w);
		    താക്കോൽ_Event.window = XtWindow(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & താക്കോൽ_Event,
								     ശൂന്യ, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
add_mnemonic_grabs(Widget dialog, Widget w)
{
    അക്ഷരം	    mneString[2];
    WidgetList	    children;
    പൂർണ്ണസംഖ്യ		    അക്കം_Children, i;
    Boolean	    isMenu;
    താക്കോൽ_Sym	    mnemonic = '\0';
    unsigned അക്ഷരം   rowColType;

    എങ്കിൽ (XtIsComposite(w))
    {
	എങ്കിൽ (XtClass(w) == xmRowColumnWidgetClass)
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNrowColumnType, &rowColType, ശൂന്യ);
	    isMenu = (rowColType != (unsigned അക്ഷരം)XmWORK_AREA);
	}
	അല്ലെങ്കിൽ
	    isMenu = False;
	എങ്കിൽ (!isMenu)
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNchildren, &children, XmNnumChildren,
							  &അക്കം_Children, ശൂന്യ);
	    വേണ്ടി (i = 0; i < അക്കം_Children; i++)
		add_mnemonic_grabs(dialog, children[i]);
	}
    }
    അല്ലെങ്കിൽ
    {
	Xt_Va_Get_മൂല്യങ്ങൾ(w, XmNmnemonic, &mnemonic, ശൂന്യ);
	എങ്കിൽ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_താക്കോൽ(dialog, XKeysymToKeycode(XtDisplay(dialog),
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
    സ്ഥിതം ശൂന്യം
activate_dialog_mnemonics(Widget dialog)
{
    എങ്കിൽ (!dialog)
	തിരികെ;

    XtAddEventHandler(dialog, താക്കോൽ_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ശൂന്യ);
    add_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    സ്ഥിതം ശൂന്യം
suppress_dialog_mnemonics(Widget dialog)
{
    എങ്കിൽ (!dialog)
	തിരികെ;

    Xt_Ungrab_താക്കോൽ(dialog, Any_താക്കോൽ, Mod1Mask);
    XtRemoveEventHandler(dialog, താക്കോൽ_Press_Mask, False,
			   (XtEventHandler) mnemonic_event, (XtPointer) ശൂന്യ);
}

#എങ്കിൽ defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    സ്ഥിതം ശൂന്യം
set_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    എങ്കിൽ (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.fontset);
	എങ്കിൽ (fl != ശൂന്യ)
	{
	    എങ്കിൽ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    അല്ലെങ്കിൽ
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
	    XmFontListFree(fl);
	}
    }
# അല്ലെങ്കിൽ
    എങ്കിൽ (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	എങ്കിൽ (fl != ശൂന്യ)
	{
	    എങ്കിൽ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    അല്ലെങ്കിൽ
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#endif

#എങ്കിൽ defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

തരനിർവചനം ഘടന dialog_callback_arg
{
    അക്ഷരം *  args;   // not used right now
    പൂർണ്ണസംഖ്യ	    id;
} dcbarg_T;

സ്ഥിതം Widget dialog_wgt;
സ്ഥിതം അക്ഷരം *browse_fname = ശൂന്യ;
സ്ഥിതം XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

സ്ഥിതം ശൂന്യം DialogCancelCB(Widget, XtPointer, XtPointer);
സ്ഥിതം ശൂന്യം DialogAcceptCB(Widget, XtPointer, XtPointer);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    സ്ഥിതം ശൂന്യം
set_predefined_label(Widget parent, String name, അക്ഷരം *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *next;
    താക്കോൽ_Sym	mnemonic = NUL;

    w = XtNameToWidget(parent, name);

    എങ്കിൽ (!w)
	തിരികെ;

    p = vim_strsave((char_u *)new_label);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (next = p; *next; ++next)
    {
	എങ്കിൽ (*next == DLG_HOTKEY_CHAR)
	{
	    പൂർണ്ണസംഖ്യ len = STRLEN(next);

	    എങ്കിൽ (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreate((അക്ഷരം *)p, STRING_TAG);
    vim_free(p);

    എങ്കിൽ (str != ശൂന്യ)
    {
	Xt_Va_Set_മൂല്യങ്ങൾ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ശൂന്യ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    സ്ഥിതം ശൂന്യം
set_predefined_fontlist(Widget parent, String name)
{
    Widget w;
    w = XtNameToWidget(parent, name);

    എങ്കിൽ (!w)
	തിരികെ;

    set_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    പൂർണ്ണസംഖ്യ		saving UNUSED,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext UNUSED,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*pattern;
    char_u	*tofree = ശൂന്യ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ശൂന്യ, 0);

    എങ്കിൽ (initdir == ശൂന്യ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    എങ്കിൽ (dflt == ശൂന്യ)
	dflt = (char_u *)"";
    അല്ലെങ്കിൽ എങ്കിൽ (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
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
    എങ്കിൽ (filter == ശൂന്യ)
	pattern = (char_u *)"";
    അല്ലെങ്കിൽ
    {
	char_u	*s, *p;

	s = filter;
	വേണ്ടി (p = filter; *p != NUL; ++p)
	{
	    എങ്കിൽ (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    എങ്കിൽ (*p == ';' || *p == '\n')	// end of (first) pattern
		നിർത്തുക;
	}
	pattern = vim_strnsave(s, p - s);
	tofree = pattern;
	എങ്കിൽ (pattern == ശൂന്യ)
	    pattern = (char_u *)"";
    }

    Xt_Va_Set_മൂല്യങ്ങൾ(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (അക്ഷരം *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (അക്ഷരം *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (അക്ഷരം *)pattern, STRLEN(pattern) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (അക്ഷരം *)title, STRLEN(title) + 1,
	ശൂന്യ);

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
    എങ്കിൽ (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_Set_മൂല്യങ്ങൾ(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ശൂന്യ);

    XtAddCallback(dialog_wgt, XmNokCallback, DialogAcceptCB, (XtPointer)0);
    XtAddCallback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (XtPointer)0);
    // We have no help in this window, so hide help button
    XtUnmanageChild(XmFileSelectionBoxGetChild(dialog_wgt,
					(unsigned അക്ഷരം)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    ചെയ്യുക
    {
	XtAppProcessEvent(XtWidgetToApplicationContext(dialog_wgt),
	    (Xt_നിവേശം_Mask)XtIMAll);
    } ആകുമ്പോൾ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    എങ്കിൽ (browse_fname == ശൂന്യ)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    സ്ഥിതം ശൂന്യം
DialogCancelCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data UNUSED)	//  data from widget class
{
    എങ്കിൽ (browse_fname != ശൂന്യ)
    {
	XtFree(browse_fname);
	browse_fname = ശൂന്യ;
    }
    XtUnmanageChild(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    സ്ഥിതം ശൂന്യം
DialogAcceptCB(
    Widget	w UNUSED,		//  widget id
    XtPointer	client_data UNUSED,	//  data from application
    XtPointer	call_data)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    എങ്കിൽ (browse_fname != ശൂന്യ)
    {
	XtFree(browse_fname);
	browse_fname = ശൂന്യ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_data;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->മൂല്യം, charset, &browse_fname);

    // popdown the file selection box
    XtUnmanageChild(dialog_wgt);
}

#endif // FEAT_BROWSE

#എങ്കിൽ defined(FEAT_GUI_DIALOG)

സ്ഥിതം പൂർണ്ണസംഖ്യ	dialogStatus;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    സ്ഥിതം ശൂന്യം
keyhit_callback(
    Widget		w,
    XtPointer		client_data UNUSED,
    XEvent		*event,
    Boolean		*cont UNUSED)
{
    അക്ഷരം	buf[2];
    താക്കോൽ_Sym	താക്കോൽ_sym;

    എങ്കിൽ (XLookupString(&(event->xkey), buf, 2, &താക്കോൽ_sym, ശൂന്യ) == 1)
    {
	എങ്കിൽ (*buf == CAR)
	    dialogStatus = 1;
	അല്ലെങ്കിൽ എങ്കിൽ (*buf == ESC)
	    dialogStatus = 2;
    }
    എങ്കിൽ ((താക്കോൽ_sym == XK_ഇടത് || താക്കോൽ_sym == XK_വലത്)
	    && !(event->xkey.state & ShiftMask))
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(gui.dpy));
}

    സ്ഥിതം ശൂന്യം
butproc(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    dialogStatus = (പൂർണ്ണസംഖ്യ)(long)client_data + 1;
}

# ifdef HAVE_XPM

    സ്ഥിതം Widget
create_pixmap_label(
    Widget	parent,
    String	name,
    അക്ഷരം	**data,
    ArgList	args,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    പൂർണ്ണസംഖ്യ			depth;
    Pixmap		pixmap = 0;
    XpmAttributes	attr;
    Boolean		rs;
    Xpm_Color_ചിഹ്നം	color[5] =
    {
	{"none", ശൂന്യ, 0},
	{"iconColor1", ശൂന്യ, 0},
	{"bottomShadowColor", ശൂന്യ, 0},
	{"topShadowColor", ശൂന്യ, 0},
	{"selectColor", ശൂന്യ, 0}
    };

    label = XmCreateLabelGadget(parent, name, args, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = XtDisplayOfObject(label);
    scr = XtScreenOfObject(label);
    Xt_Va_Get_മൂല്യങ്ങൾ(XtIsSubclass(label, coreWidgetClass)
	    ?  label : XtParent(label),
		  XmNdepth, &depth,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ശൂന്യ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | XpmDepth;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.depth = depth;
    XpmCreatePixmapFromData(dsp, വേര്_Window_Of_Screen(scr),
		    data, &pixmap, ശൂന്യ, &attr);

    Xt_Va_Get_മൂല്യങ്ങൾ(label, XmNrecomputeSize, &rs, ശൂന്യ);
    Xt_Va_Set_മൂല്യങ്ങൾ(label, XmNrecomputeSize, True, ശൂന്യ);
    Xt_Va_Set_മൂല്യങ്ങൾ(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ശൂന്യ);
    Xt_Va_Set_മൂല്യങ്ങൾ(label, XmNrecomputeSize, rs, ശൂന്യ);

    തിരികെ label;
}
# endif

    പൂർണ്ണസംഖ്യ
gui_mch_dialog(
    പൂർണ്ണസംഖ്യ		type UNUSED,
    char_u	*title,
    char_u	*message,
    char_u	*button_names,
    പൂർണ്ണസംഖ്യ		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    പൂർണ്ണസംഖ്യ		ex_cmd UNUSED)
{
    char_u		*buts;
    char_u		*p, *next;
    XtAppContext	app;
    XmString		label;
    പൂർണ്ണസംഖ്യ			butcount;
    Widget		w;
    Widget		dialogform = ശൂന്യ;
    Widget		form = ശൂന്യ;
    Widget		dialogtextfield = ശൂന്യ;
    Widget		*buttons;
    Widget		sep_form = ശൂന്യ;
    Boolean		vertical;
    Widget		separator = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			n;
    Arg			args[6];
# ifdef HAVE_XPM
    അക്ഷരം		**icon_data = ശൂന്യ;
    Widget		dialogpixmap = ശൂന്യ;
# endif

    എങ്കിൽ (title == ശൂന്യ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (അക്ഷരം *)"dialog", ശൂന്യ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ശൂന്യ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((അക്ഷരം *)title);
    എങ്കിൽ (label == ശൂന്യ)
	തിരികെ -1;
    Xt_Va_Set_മൂല്യങ്ങൾ(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ശൂന്യ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_names);
    എങ്കിൽ (buts == ശൂന്യ)
	തിരികെ -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    വേണ്ടി (p = buts; *p; ++p)
	എങ്കിൽ (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    എങ്കിൽ (buttons == ശൂന്യ)
    {
	vim_free(buts);
	തിരികെ -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    വേണ്ടി (butcount = 0; *p; ++butcount)
    {
	താക്കോൽ_Sym mnemonic = NUL;

	വേണ്ടി (next = p; *next; ++next)
	{
	    എങ്കിൽ (*next == DLG_HOTKEY_CHAR)
	    {
		പൂർണ്ണസംഖ്യ len = STRLEN(next);

		എങ്കിൽ (len > 0)
		{
		    mch_memmove(next, next + 1, len);
		    mnemonic = next[0];
		}
	    }
	    എങ്കിൽ (*next == DLG_BUTTON_SEP)
	    {
		*next++ = NUL;
		നിർത്തുക;
	    }
	}
	label = XmStringCreate(_((അക്ഷരം *)p), STRING_TAG);
	എങ്കിൽ (label == ശൂന്യ)
	    നിർത്തുക;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_തള്ളുക_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ശൂന്യ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	എങ്കിൽ (butcount > 0)
	{
	    എങ്കിൽ (vertical)
		Xt_Va_Set_മൂല്യങ്ങൾ(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ശൂന്യ);
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (*next == NUL)
		{
		    Xt_Va_Set_മൂല്യങ്ങൾ(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    XmNrightOffset, 4,
			    ശൂന്യ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    XmNbottomOffset, 4,
			    ശൂന്യ);
		    XtManageChild(sep_form);
		}
		അല്ലെങ്കിൽ
		{
		    Xt_Va_Set_മൂല്യങ്ങൾ(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ശൂന്യ);
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!vertical)
	{
	    എങ്കിൽ (*next == NUL)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			XmNrightOffset, 4,
			ശൂന്യ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			XmNbottomOffset, 4,
			ശൂന്യ);
		XtManageChild(sep_form);
	    }
	    അല്ലെങ്കിൽ
		Xt_Va_Set_മൂല്യങ്ങൾ(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			XmNleftOffset, 4,
			ശൂന്യ);
	}

	XtAddCallback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (XtPointer)(long)butcount);
	p = next;
    }
    vim_free(buts);

    separator = (Widget) 0;
    എങ്കിൽ (butcount > 0)
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

    എങ്കിൽ (textfield != ശൂന്യ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xmTextFieldWidgetClass, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ശൂന്യ);
	എങ്കിൽ (butcount > 0)
	    Xt_Va_Set_മൂല്യങ്ങൾ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ശൂന്യ);
	അല്ലെങ്കിൽ
	    Xt_Va_Set_മൂല്യങ്ങൾ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ശൂന്യ);

	set_fontlist(dialogtextfield);
	XmTextFieldSetString(dialogtextfield, (അക്ഷരം *)textfield);
	XtManageChild(dialogtextfield);
	XtAddEventHandler(dialogtextfield, താക്കോൽ_Press_Mask, False,
			    (XtEventHandler)keyhit_callback, (XtPointer)ശൂന്യ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ശൂന്യ);
    XtManageChild(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    തിരഞ്ഞെടുപ്പ് (type)
    {
	അവസ്ഥ VIM_GENERIC:
	    icon_data = generic_xpm;
	    നിർത്തുക;
	അവസ്ഥ VIM_ERROR:
	    icon_data = error_xpm;
	    നിർത്തുക;
	അവസ്ഥ VIM_WARNING:
	    icon_data = alert_xpm;
	    നിർത്തുക;
	അവസ്ഥ VIM_INFO:
	    icon_data = info_xpm;
	    നിർത്തുക;
	അവസ്ഥ VIM_QUESTION:
	    icon_data = quest_xpm;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
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
    label = XmStringCreateLtoR((അക്ഷരം *)message, STRING_TAG);
    എങ്കിൽ (label == ശൂന്യ)
    {
	vim_free(buttons);
	തിരികെ -1;
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
# അല്ലെങ്കിൽ
				XmNleftAttachment, XmATTACH_FORM,
# endif
				XmNleftOffset, 8,
				XmNrightAttachment, XmATTACH_FORM,
				XmNrightOffset, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				XmNbottomOffset, 8,
				ശൂന്യ);
    XmStringFree(label);
    set_fontlist(w);

    എങ്കിൽ (textfield != ശൂന്യ)
    {
	Xt_Va_Set_മൂല്യങ്ങൾ(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (butcount > 0)
	    Xt_Va_Set_മൂല്യങ്ങൾ(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ശൂന്യ);
	അല്ലെങ്കിൽ
	    Xt_Va_Set_മൂല്യങ്ങൾ(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ശൂന്യ);
    }

    എങ്കിൽ (dfltbutton < 1)
	dfltbutton = 1;
    എങ്കിൽ (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_Set_മൂല്യങ്ങൾ(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ശൂന്യ);
    എങ്കിൽ (textfield != ശൂന്യ)
	Xt_Va_Set_മൂല്യങ്ങൾ(dialogform, XmNinitialFocus, dialogtextfield, ശൂന്യ);
    അല്ലെങ്കിൽ
	Xt_Va_Set_മൂല്യങ്ങൾ(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ശൂന്യ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    എങ്കിൽ (textfield != ശൂന്യ && *textfield != NUL)
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
    വേണ്ടി (;;)
    {
	XtAppProcessEvent(app, (Xt_നിവേശം_Mask)XtIMAll);
	എങ്കിൽ (dialogStatus >= 0 || !XtIsManaged(dialogform))
	    നിർത്തുക;
    }

    vim_free(buttons);

    എങ്കിൽ (textfield != ശൂന്യ)
    {
	p = (char_u *)XmTextGetString(dialogtextfield);
	എങ്കിൽ (p == ശൂന്യ || dialogStatus < 0)
	    *textfield = NUL;
	അല്ലെങ്കിൽ
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((അക്ഷരം *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    തിരികെ dialogStatus;
}
#endif // FEAT_GUI_DIALOG

#എങ്കിൽ defined(FEAT_TOOLBAR)
    ശൂന്യം
gui_mch_show_toolbar(പൂർണ്ണസംഖ്യ showit)
{
    Cardinal	അക്കം_Children;	    // how many children toolBar has

    എങ്കിൽ (toolBar == (Widget)0)
	തിരികെ;
    Xt_Va_Get_മൂല്യങ്ങൾ(toolBar, XmNnumChildren, &അക്കം_Children, ശൂന്യ);
    എങ്കിൽ (showit && അക്കം_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_മൂല്യങ്ങൾ(toolBar, XmNchildren, &children, ശൂന്യ);
	{
	    ശൂന്യം    (*action)(BalloonEval *);
	    പൂർണ്ണസംഖ്യ	    text = 0;

	    എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    അല്ലെങ്കിൽ
		action = &gui_mch_disable_beval_area;
	    എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *)p_toolbar, "text"))
		text = 1;
	    അല്ലെങ്കിൽ എങ്കിൽ (strstr((സ്ഥിരം അക്ഷരം *)p_toolbar, "icons"))
		text = -1;
	    എങ്കിൽ (text != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    എങ്കിൽ (menu_is_toolbar(toolbar->dname))
			നിർത്തുക;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		എങ്കിൽ (toolbar)
		{
		    വേണ്ടി (cur = toolbar->children; cur; cur = cur->next)
		    {
			Arg	    args[1];
			പൂർണ്ണസംഖ്യ	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			എങ്കിൽ (cur->tip != ശൂന്യ)
			    (*action)(cur->tip);
			എങ്കിൽ (!menu_is_separator(cur->name))
			{
			    എങ്കിൽ (text == 1 || cur->xpm == ശൂന്യ)
			    {
				XtSetArg(args[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    എങ്കിൽ (cur->id != ശൂന്യ)
			    {
				XtUnmanageChild(cur->id);
				Xt_Set_മൂല്യങ്ങൾ(cur->id, args, n);
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
	എങ്കിൽ (showing_tabline)
	{
	    Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ശൂന്യ);
	    Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_വരി,
			  ശൂന്യ);
	}
	അല്ലെങ്കിൽ
# endif
	    Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, XtParent(toolBar),
			  ശൂന്യ);
	എങ്കിൽ (XtIsManaged(menuBar))
	    Xt_Va_Set_മൂല്യങ്ങൾ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ശൂന്യ);
	അല്ലെങ്കിൽ
	    Xt_Va_Set_മൂല്യങ്ങൾ(XtParent(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	gui.toolbar_height = 0;
	എങ്കിൽ (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
# ifdef FEAT_GUI_TABLINE
	    എങ്കിൽ (showing_tabline)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_FORM,
			      ശൂന്യ);
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_വരി,
			      ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM,
			      ശൂന്യ);
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
    സ്ഥിതം ശൂന്യം
reset_focus(ശൂന്യം)
{
    എങ്കിൽ (textArea != ശൂന്യ)
	XmProcessTraversal(textArea, XmTRAVERSE_CURRENT);
}

    പൂർണ്ണസംഖ്യ
gui_mch_compute_toolbar_height(ശൂന്യം)
{
    Dimension	borders;
    Dimension	height;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	അക്കം_Children;	    // how many children toolBar has
    പൂർണ്ണസംഖ്യ		i;

    borders = 0;
    height = 0;
    എങ്കിൽ (toolBar != (Widget)0 && toolBarFrame != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_മൂല്യങ്ങൾ(toolBarFrame,
		XmNshadowThickness, &fst,
		XmNmarginHeight, &fmh,
		ശൂന്യ);
	borders += fst + fmh;
	Xt_Va_Get_മൂല്യങ്ങൾ(toolBar,
		XmNshadowThickness, &tst,
		XmNmarginHeight, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &അക്കം_Children, ശൂന്യ);
	borders += tst + tmh;
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)അക്കം_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_മൂല്യങ്ങൾ(children[i], XmNheight, &whgt, ശൂന്യ);
	    എങ്കിൽ (height < whgt)
		height = whgt;
	}
    }
# ifdef LESSTIF_VERSION
    // Hack: When starting up we get wrong dimensions.
    എങ്കിൽ (height < 10)
	height = 24;
# endif

    തിരികെ (പൂർണ്ണസംഖ്യ)(height + (borders << 1));
}

    ശൂന്യം
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_മൂല്യങ്ങൾ(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    ശൂന്യ);
}
#endif

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    ശൂന്യം
gui_mch_show_tabline(പൂർണ്ണസംഖ്യ showit)
{
    എങ്കിൽ (tab_വരി == (Widget)0)
	തിരികെ;

    എങ്കിൽ (!showit != !showing_tabline)
    {
	എങ്കിൽ (showit)
	{
	    XtManageChild(tab_വരി);
	    XtUnmanageChild(XtNameToWidget(tab_വരി, "PageScroller"));
	    XtUnmanageChild(XtNameToWidget(tab_വരി, "MinorTabScrollerNext"));
	    XtUnmanageChild(XtNameToWidget(tab_വരി,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    എങ്കിൽ (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ശൂന്യ);
	    അല്ലെങ്കിൽ
#  endif
		എങ്കിൽ (XtIsManaged(menuBar))
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ശൂന്യ);
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി,
			      XmNtopAttachment, XmATTACH_FORM, ശൂന്യ);
	    Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_വരി,
			  ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	{
	    XtUnmanageChild(tab_വരി);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    എങ്കിൽ (XtIsManaged(XtParent(toolBar)))
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, XtParent(toolBar), ശൂന്യ);
	    അല്ലെങ്കിൽ
#  endif
		എങ്കിൽ (XtIsManaged(menuBar))
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ശൂന്യ);
	    അല്ലെങ്കിൽ
# endif
		Xt_Va_Set_മൂല്യങ്ങൾ(textAreaForm,
			      XmNtopAttachment, XmATTACH_FORM, ശൂന്യ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_showing_tabline(ശൂന്യം)
{
    തിരികെ tab_വരി != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ശൂന്യം
gui_mch_update_tabline(ശൂന്യം)
{
    tabpage_T		*tp;
    പൂർണ്ണസംഖ്യ			nr = 1, n;
    Arg			args[10];
    പൂർണ്ണസംഖ്യ			curtabidx = 0, currentpage;
    Widget		tab;
    XmNotebookPageInfo	page_info;
    XmNotebookPageStatus page_status;
    പൂർണ്ണസംഖ്യ			last_page, tab_എണ്ണം;
    XmString		label_str;
    അക്ഷരം		*label_cstr;
    BalloonEval		*beval;

    എങ്കിൽ (tab_വരി == (Widget)0)
	തിരികെ;

    // Add a label for each tab page.  They all contain the same text area.
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = tp->tp_next, ++nr)
    {
	എങ്കിൽ (tp == curtab)
	    curtabidx = nr;

	page_status = XmNotebookGetPageInfo(tab_വരി, nr, &page_info);
	എങ്കിൽ (page_status == XmPAGE_INVALID
		|| page_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    XtSetArg(args[n], XmNnotebookChildType, XmMAJOR_TAB); n++;
	    XtSetArg(args[n], XmNtraversalOn, False); n++;
	    XtSetArg(args[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    XtSetArg(args[n], XmNhighlightThickness, 1); n++;
	    XtSetArg(args[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_തള്ളുക_Button(tab_വരി, "-Empty-", args, n);
	    XtManageChild(tab);
	    beval = gui_mch_create_beval_area(tab, ശൂന്യ, tabline_balloon_cb,
									ശൂന്യ);
	    Xt_Va_Set_മൂല്യങ്ങൾ(tab, XmNuserData, beval, ശൂന്യ);
	}
	അല്ലെങ്കിൽ
	    tab = page_info.major_tab_widget;

	Xt_Va_Set_മൂല്യങ്ങൾ(tab, Xm_Npage_സംഖ്യ, nr, ശൂന്യ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_മൂല്യങ്ങൾ(tab, XmNlabelString, &label_str, ശൂന്യ);
	എങ്കിൽ (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    എങ്കിൽ (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_Set_മൂല്യങ്ങൾ(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ശൂന്യ);
		/*
		 * Force a resize of the tab label button
		 */
		XtUnmanageChild(tab);
		XtManageChild(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_എണ്ണം = nr - 1;

    Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി, Xm_Nlast_Page_സംഖ്യ, &last_page, ശൂന്യ);

    // Remove any old labels.
    ആകുമ്പോൾ (nr <= last_page)
    {
	എങ്കിൽ (XmNotebookGetPageInfo(tab_വരി, nr, &page_info) != XmPAGE_INVALID
	    && page_info.page_സംഖ്യ == nr
	    && page_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_മൂല്യങ്ങൾ(page_info.major_tab_widget, XmNuserData, &beval, ശൂന്യ);
	    എങ്കിൽ (beval != ശൂന്യ)
		gui_mch_destroy_beval_area(beval);
	    XtUnmanageChild(page_info.major_tab_widget);
	    XtDestroyWidget(page_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി, Xm_Nlast_Page_സംഖ്യ, tab_എണ്ണം, ശൂന്യ);

    Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി, Xm_Ncurrent_Page_സംഖ്യ, &currentpage, ശൂന്യ);
    എങ്കിൽ (currentpage != curtabidx)
	Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി, Xm_Ncurrent_Page_സംഖ്യ, curtabidx, ശൂന്യ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ശൂന്യം
gui_mch_set_curtab(പൂർണ്ണസംഖ്യ nr)
{
    പൂർണ്ണസംഖ്യ		currentpage;

    എങ്കിൽ (tab_വരി == (Widget)0)
	തിരികെ;

    Xt_Va_Get_മൂല്യങ്ങൾ(tab_വരി, Xm_Ncurrent_Page_സംഖ്യ, &currentpage, ശൂന്യ);
    എങ്കിൽ (currentpage != nr)
	Xt_Va_Set_മൂല്യങ്ങൾ(tab_വരി, Xm_Ncurrent_Page_സംഖ്യ, nr, ശൂന്യ);
}
#endif

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    സ്ഥിതം ശൂന്യം
gui_motif_menu_colors(Widget id)
{
    എങ്കിൽ (gui.menu_bg_pixel != INVALCOLOR)
#എങ്കിൽ (XmVersion >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#അല്ലെങ്കിൽ
	Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNbackground, gui.menu_bg_pixel, ശൂന്യ);
#endif
    എങ്കിൽ (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNforeground, gui.menu_fg_pixel, ശൂന്യ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    സ്ഥിതം ശൂന്യം
gui_motif_scroll_colors(Widget id)
{
    എങ്കിൽ (gui.scroll_bg_pixel != INVALCOLOR)
#എങ്കിൽ (XmVersion >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#അല്ലെങ്കിൽ
	Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNbackground, gui.scroll_bg_pixel, ശൂന്യ);
#endif
    എങ്കിൽ (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNforeground, gui.scroll_fg_pixel, ശൂന്യ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    ശൂന്യം
gui_motif_menu_fontlist(Widget id UNUSED)
{
#ifdef FEAT_MENU
# ifdef FONTSET_ALWAYS
    എങ്കിൽ (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFontSet *)&gui.menu_fontset);
	എങ്കിൽ (fl != ശൂന്യ)
	{
	    എങ്കിൽ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    അല്ലെങ്കിൽ
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
	    XmFontListFree(fl);
	}
    }
# അല്ലെങ്കിൽ
    എങ്കിൽ (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	എങ്കിൽ (fl != ശൂന്യ)
	{
	    എങ്കിൽ (XtIsManaged(id))
	    {
		XtUnmanageChild(id);
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
		// We should force the widget to recalculate its
		// geometry now.
		XtManageChild(id);
	    }
	    അല്ലെങ്കിൽ
		Xt_Va_Set_മൂല്യങ്ങൾ(id, XmNfontList, fl, ശൂന്യ);
	    XmFontListFree(fl);
	}
    }
# endif
#endif
}


/*
 * We don't create it twice for the sake of speed.
 */

തരനിർവചനം ഘടന _SharedFindReplace
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

സ്ഥിതം SharedFindReplace find_widgets = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};
സ്ഥിതം SharedFindReplace repl_widgets = {ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ};

    സ്ഥിതം ശൂന്യം
find_replace_destroy_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    എങ്കിൽ (cd != ശൂന്യ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    സ്ഥിതം ശൂന്യം
find_replace_dismiss_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    SharedFindReplace *cd = (SharedFindReplace *)client_data;

    എങ്കിൽ (cd != ശൂന്യ)
	XtUnmanageChild(cd->dialog);
}

    സ്ഥിതം ശൂന്യം
entry_activate_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    XmProcessTraversal((Widget)client_data, XmTRAVERSE_CURRENT);
}

    സ്ഥിതം ശൂന്യം
find_replace_callback(
    Widget	w UNUSED,
    XtPointer	client_data,
    XtPointer	call_data UNUSED)
{
    long_u	flags = (long_u)client_data;
    അക്ഷരം	*find_text, *repl_text;
    Boolean	direction_down = TRUE;
    Boolean	wword;
    Boolean	mcase;
    SharedFindReplace *sfr;

    എങ്കിൽ (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = empty_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	തിരികെ;
    }

    // Get the search/replace strings from the dialog
    എങ്കിൽ (flags == FRD_FINDNEXT)
    {
	repl_text = ശൂന്യ;
	sfr = &find_widgets;
    }
    അല്ലെങ്കിൽ
    {
	repl_text = XmTextFieldGetString(repl_widgets.with);
	sfr = &repl_widgets;
    }
    find_text = XmTextFieldGetString(sfr->what);
    Xt_Va_Get_മൂല്യങ്ങൾ(sfr->down, XmNset, &direction_down, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(sfr->wword, XmNset, &wword, ശൂന്യ);
    Xt_Va_Get_മൂല്യങ്ങൾ(sfr->mcase, XmNset, &mcase, ശൂന്യ);
    എങ്കിൽ (wword)
	flags |= FRD_WHOLE_WORD;
    എങ്കിൽ (mcase)
	flags |= FRD_MATCH_CASE;

    (ശൂന്യം)gui_do_findrepl((പൂർണ്ണസംഖ്യ)flags, (char_u *)find_text, (char_u *)repl_text,
							      direction_down);

    എങ്കിൽ (find_text != ശൂന്യ)
	XtFree(find_text);
    എങ്കിൽ (repl_text != ശൂന്യ)
	XtFree(repl_text);
}

    സ്ഥിതം ശൂന്യം
find_replace_keypress(
    Widget		w UNUSED,
    SharedFindReplace	*frdp,
    XKeyEvent		*event,
    Boolean		*b UNUSED)
{
    താക്കോൽ_Sym keysym;

    എങ്കിൽ (frdp == ശൂന്യ)
	തിരികെ;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    എങ്കിൽ (keysym == XK_Escape)
	XtUnmanageChild(frdp->dialog);
}

    സ്ഥിതം ശൂന്യം
set_label(Widget w, അക്ഷരം *label)
{
    XmString	str;
    char_u	*p, *next;
    താക്കോൽ_Sym	mnemonic = NUL;

    എങ്കിൽ (!w)
	തിരികെ;

    p = vim_strsave((char_u *)label);
    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ;
    വേണ്ടി (next = p; *next; ++next)
    {
	എങ്കിൽ (*next == DLG_HOTKEY_CHAR)
	{
	    പൂർണ്ണസംഖ്യ len = STRLEN(next);

	    എങ്കിൽ (len > 0)
	    {
		mch_memmove(next, next + 1, len);
		mnemonic = next[0];
	    }
	}
    }

    str = XmStringCreateSimple((അക്ഷരം *)p);
    vim_free(p);
    എങ്കിൽ (str)
    {
	Xt_Va_Set_മൂല്യങ്ങൾ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ശൂന്യ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    സ്ഥിതം ശൂന്യം
find_replace_dialog_create(char_u *arg, പൂർണ്ണസംഖ്യ do_replace)
{
    SharedFindReplace	*frdp;
    Widget		separator;
    Widget		നിവേശം_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		frame;
    XmString		str;
    പൂർണ്ണസംഖ്യ			n;
    Arg			args[6];
    പൂർണ്ണസംഖ്യ			wword = FALSE;
    പൂർണ്ണസംഖ്യ			mcase = !p_ic;
    Dimension		width;
    Dimension		widest;
    char_u		*entry_text;

    frdp = do_replace ? &repl_widgets : &find_widgets;

    // Get the search string to use.
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    എങ്കിൽ (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	എങ്കിൽ (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    XtWindow(XtParent(frdp->dialog)));
	അല്ലെങ്കിൽ
	    XtManageChild(frdp->dialog);
	XtPopup(XtParent(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	എങ്കിൽ (entry_text != ശൂന്യ)
	    XmTextFieldSetString(frdp->what, (അക്ഷരം *)entry_text);
	vim_free(entry_text);

	Xt_Va_Set_മൂല്യങ്ങൾ(frdp->wword, XmNset, wword, ശൂന്യ);
	തിരികെ;
    }

    // Create a fresh new dialog window
    എങ്കിൽ (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    അല്ലെങ്കിൽ
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
	    ശൂന്യ);

    frdp->find = XtVaCreateManagedWidget("findButton",
	    xm_തള്ളുക_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ശൂന്യ);
    set_label(frdp->find, _("Find &Next"));

    XtAddCallback(frdp->find, XmNactivateCallback,
	    find_replace_callback,
	    (do_replace ? (XtPointer)FRD_R_FINDNEXT : (XtPointer)FRD_FINDNEXT));

    എങ്കിൽ (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_തള്ളുക_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->find,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ശൂന്യ);
	set_label(frdp->replace, _("&Replace"));
	XtAddCallback(frdp->replace, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_തള്ളുക_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ശൂന്യ);
	set_label(frdp->all, _("Replace &All"));
	XtAddCallback(frdp->all, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_തള്ളുക_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ശൂന്യ);
	set_label(frdp->undo, _("&Undo"));
	XtAddCallback(frdp->undo, XmNactivateCallback,
		find_replace_callback, (XtPointer)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_തള്ളുക_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ശൂന്യ);
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

    നിവേശം_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    XmNrightOffset, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    ശൂന്യ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, നിവേശം_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		XmNtopOffset, 4,
		ശൂന്യ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xmTextFieldWidgetClass, നിവേശം_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ശൂന്യ);

	എങ്കിൽ (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xmTextFieldWidgetClass,	നിവേശം_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ശൂന്യ);

	    XtAddCallback(frdp->with, XmNactivateCallback,
		    find_replace_callback, (XtPointer) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, നിവേശം_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    XmNtopOffset, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ശൂന്യ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    entry_activate_callback, frdp->with);
	    XtAddEventHandler(frdp->with, താക്കോൽ_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer) frdp);

	}
	അല്ലെങ്കിൽ
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    XtAddCallback(frdp->what, XmNactivateCallback,
		    find_replace_callback, (XtPointer)FRD_FINDNEXT);
	}
	XtAddEventHandler(frdp->what, താക്കോൽ_Press_Mask, False,
			    (XtEventHandler)find_replace_keypress,
			    (XtPointer)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	XtSetArg(args[n], XmNwidth, &width); n++;
	Xt_Get_മൂല്യങ്ങൾ(label_what, args, n);
	widest = width;
	എങ്കിൽ (do_replace)
	{
	    Xt_Get_മൂല്യങ്ങൾ(label_with, args, n);
	    എങ്കിൽ (width > widest)
		widest = width;
	}

	Xt_Va_Set_മൂല്യങ്ങൾ(frdp->what, XmNleftOffset, widest, ശൂന്യ);
	എങ്കിൽ (do_replace)
	    Xt_Va_Set_മൂല്യങ്ങൾ(frdp->with, XmNleftOffset, widest, ശൂന്യ);

    }

    XtManageChild(നിവേശം_form);

    {
	Widget radio_box;
	Widget w;

	frame = XtVaCreateWidget("directionFrame",
		xmFrameWidgetClass, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, നിവേശം_form,
		XmNtopOffset, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		XmNbottomOffset, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, നിവേശം_form,
		ശൂന്യ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, frame,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, XmFRAME_TITLE_CHILD,
		ശൂന്യ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(frame, "radioBox",
		(ArgList)ശൂന്യ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->up = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ശൂന്യ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->up);

	str = XmStringCreateSimple(_("Down"));
	frdp->down = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ശൂന്യ);
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
	    XmNtopWidget, നിവേശം_form,
	    XmNtopOffset, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    XmNbottomOffset, 4,
	    ശൂന്യ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNtopOffset, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNleftOffset, 4,
	    XmNset, wword,
	    ശൂന്യ);
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
	    ശൂന്യ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    XtManageChild(toggle_form);

    എങ്കിൽ (entry_text != ശൂന്യ)
	XmTextFieldSetString(frdp->what, (അക്ഷരം *)entry_text);
    vim_free(entry_text);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   ശൂന്യം
gui_mch_find_dialog(exarg_T *eap)
{
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    find_replace_dialog_create(eap->arg, FALSE);
}


    ശൂന്യം
gui_mch_replace_dialog(exarg_T *eap)
{
    എങ്കിൽ (!gui.in_use)
	തിരികെ;

    find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    ശൂന്യം
gui_motif_synch_fonts(ശൂന്യം)
{
    SharedFindReplace *frdp;
    പൂർണ്ണസംഖ്യ		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    എങ്കിൽ (font == ശൂന്യ)
	തിരികെ;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    വേണ്ടി (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&find_widgets);
	എങ്കിൽ (frdp->dialog)
	{
	    Xt_Va_Set_മൂല്യങ്ങൾ(frdp->what, XmNfontList, font_list, ശൂന്യ);
	    എങ്കിൽ (do_replace)
		Xt_Va_Set_മൂല്യങ്ങൾ(frdp->with, XmNfontList, font_list, ശൂന്യ);
	}
    }

    XmFontListFree(font_list);
}
