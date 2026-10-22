/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *				GUI/Motif support by Robert Webb
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Form.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/RowColumn.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/PushB.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Text.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/TextF.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Separator.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Label.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/CascadeB.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/ScrollBar.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/MenuShell.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/DrawingA.h>
#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
# include <Xm/RepType.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/Frame.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/LabelG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/ToggleBG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/SeparatoG.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/XmP.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <X11/keysym.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Xatom.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/StringDefs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <X11/Intrinsic.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_X11_XPM_H
# ਜੇ defined(VMS)
#  include <xpm.h>
# ਨਹੀਂਤਾਂ
#  include <X11/xpm.h>
# endif
#ਨਹੀਂਤਾਂ
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "gui_xmebw.h"	// for our Enhanced Button Widget

#ਜੇ defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOTIF_POPUP

ਬਾਹਰੀ Widget vimShell;

ਸਥਿਰ Widget vimForm;
ਸਥਿਰ Widget ਪਾਠ_Area_Form;
Widget ਪਾਠ_Area;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
ਸਥਿਰ Widget tool_Bar_ਚੌਖਟਾ;
ਸਥਿਰ Widget toolBar;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
ਸਥਿਰ Widget	tab_ਸਤਰ;
ਸਥਿਰ Widget	tab_ਸਤਰ_menu = 0;
ਸਥਿਰ ਪੂਰਨ	showing_tabline = 0;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
// remember the last set value for the tearoff item
ਸਥਿਰ ਪੂਰਨ tearoff_val = (ਪੂਰਨ)XmTEAR_OFF_ENABLED;
# endif
ਸਥਿਰ Widget menuBar;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
ਸਥਿਰ ਖਾਲੀ reset_focus(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ gui_motif_menu_colors(Widget id);
ਸਥਿਰ ਖਾਲੀ gui_motif_scroll_colors(Widget id);

#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
# define STRING_ਟੈਗ  Xm_FONTLIST_DEFAULT_ਟੈਗ
#ਨਹੀਂਤਾਂ
# define STRING_ਟੈਗ  XmSTRING_DEFAULT_CHARSET
#ਜੇ_ਅੰਤ

/*
 * Call-back routines.
 */

    ਸਥਿਰ ਖਾਲੀ
scroll_cb(Widget w ਅਣਵਰਤਿਆ, Xt_ਸੂਚਕ client_ਅੰਕੜੇ_2, Xt_ਸੂਚਕ call_ਅੰਕੜੇ)
{
    scrollbar_T *sb;
    ਲੰਮਾ	ਮੁੱਲ;
    ਪੂਰਨ		dragging;

    sb = gui_ਲੱਭੋ_scrollbar((ਲੰਮਾ)client_ਅੰਕੜੇ_2);

    ਮੁੱਲ = ((XmScrollBarCallbackStruct *)call_ਅੰਕੜੇ)->ਮੁੱਲ;
    dragging = (((XmScrollBarCallbackStruct *)call_ਅੰਕੜੇ)->reason ==
							      (ਪੂਰਨ)XmCR_DRAG);
    gui_drag_scrollbar(sb, ਮੁੱਲ, dragging);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    ਸਥਿਰ ਖਾਲੀ
tabline_cb(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_ਅੰਕੜੇ;
    ਜੇ (nptr->reason != (ਪੂਰਨ)XmCR_NONE)
	send_tabline_event(nptr->ਪੰਨਾ_ਸੰਖਿਆ);
}

    ਸਥਿਰ ਖਾਲੀ
tabline_button_cb(
    Widget	w,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    Xt_ਸੂਚਕ	cmd, tab_idx;

    Xt_Va_Get_ਮੁੱਲਾਂ(w, Xm_Nuser_ਅੰਕੜੇ, &cmd, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ_menu, Xm_Nuser_ਅੰਕੜੇ, &tab_idx, ਨਿਰਾ);

    send_tabline_menu_event((ਪੂਰਨ)(ਲੰਮਾ)tab_idx, (ਪੂਰਨ)(ਲੰਮਾ)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ਸਥਿਰ ਖਾਲੀ
motif_tabline_timer_cb (
    Xt_ਸੂਚਕ		timed_ਬਾਹਰਲਾ,
    XtIntervalId	*interval_id ਅਣਵਰਤਿਆ)
{
    *((ਪੂਰਨ *)timed_ਬਾਹਰਲਾ) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ਸਥਿਰ ਪੂਰਨ
tabline_scroller_clicked(
    ਅੱਖਰ		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	ਚੌੜਾਈ, ਉਚਾਈ;

    tab_scroll_w = Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ, scroller_name);
    ਜੇ (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_ਮੁੱਲਾਂ(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &ਚੌੜਾਈ, XmNheight, &ਉਚਾਈ, ਨਿਰਾ);
	ਜੇ (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    ਜੇ (event->x >= pos_x && event->x <= pos_x + ਚੌੜਾਈ
		    && event->y >= pos_y && event->y <= pos_y + ਉਚਾਈ)
		// Clicked on the scroller
		ਵਾਪਸ TRUE;
	}
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
tabline_menu_cb(
    Widget	w,
    Xt_ਸੂਚਕ	ਘੇਰਾ ਅਣਵਰਤਿਆ,
    XEvent	*e,
    Boolean	*continue_dispatch ਅਣਵਰਤਿਆ)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    ਪੂਰਨ				tab_idx = 0;
    WidgetList			children;
    Cardinal			ਅੰਕ_Children;
    ਸਥਿਰ XtIntervalId		timer = (XtIntervalId)0;
    ਸਥਿਰ ਪੂਰਨ			timed_ਬਾਹਰਲਾ = TRUE;

    event = (XButtonPressedEvent *)e;

    ਜੇ (event->button == Button1)
    {
	ਜੇ (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    ਵਾਪਸ;

	ਜੇ (!timed_ਬਾਹਰਲਾ)
	{
	    Xt_Remove_ਸਮਾਂ_ਬਾਹਰਲਾ(timer);
	    timed_ਬਾਹਰਲਾ = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = Xt_App_ਜੋੜੋ_ਸਮਾਂ_ਬਾਹਰਲਾ(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_ਬਾਹਰਲਾ);
	    timed_ਬਾਹਰਲਾ = FALSE;
	}
	ਵਾਪਸ;
    }

    ਜੇ (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ_menu, Xm_Nuser_ਅੰਕੜੇ, &tab_idx, ਨਿਰਾ);
	send_tabline_menu_event(tab_idx, (ਪੂਰਨ)TABLINE_MENU_CLOSE);
	ਵਾਪਸ;
    }

    ਜੇ (event->button != Button3)
	ਵਾਪਸ;

    // When ignoring events don't show the menu.
    ਜੇ (hold_gui_events || cmdwin_type != 0)
	ਵਾਪਸ;

    ਜੇ (event->subwindow != None)
    {
	tab_w = Xt_ਖਿੜਕੀ_ਮੰਜ਼ਿਲ_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	ਜੇ (tab_w != (Widget)0 && Xm_Is_ਧੱਕੋ_Button(tab_w))
	    Xt_Va_Get_ਮੁੱਲਾਂ(tab_w, Xm_Npage_ਸੰਖਿਆ, &tab_idx, ਨਿਰਾ);
    }

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ_menu, Xm_Nuser_ਅੰਕੜੇ, (Xt_ਸੂਚਕ)(ਲੰਮਾ)tab_idx, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ_menu, XmNchildren, &children, XmNnumChildren,
		  &ਅੰਕ_Children, ਨਿਰਾ);
    XtManageChildren(children, ਅੰਕ_Children);
    XmMenuPosition(tab_ਸਤਰ_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_ਬੱਚਾ(tab_ਸਤਰ_menu);
}

    ਸਥਿਰ ਖਾਲੀ
tabline_balloon_cb(BalloonEval *beval, ਪੂਰਨ state ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		nr;
    tabpage_T	*tp;

    ਜੇ (beval->ਨਿਸ਼ਾਨਾ == (Widget)0)
	ਵਾਪਸ;

    Xt_Va_Get_ਮੁੱਲਾਂ(beval->ਨਿਸ਼ਾਨਾ, Xm_Npage_ਸੰਖਿਆ, &nr, ਨਿਰਾ);
    tp = ਲੱਭੋ_tabpage(nr);
    ਜੇ (tp == ਨਿਰਾ)
	ਵਾਪਸ;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#ਜੇ_ਅੰਤ

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/XmP.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <Xm/LabelP.h>

ਸਥਿਰ XtExposeProc old_label_expose = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   label_type = (ਪੂਰਨ)XmSTRING;

    Xt_Va_Get_ਮੁੱਲਾਂ(_w, XmNlabelType, &label_type, (Xt_ਸੂਚਕ)0);

    ਜੇ (XtIsSensitive(_w) || label_type != (ਪੂਰਨ)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    ਨਹੀਂਤਾਂ
    {
	XGCValues   ਮੁੱਲਾਂ;
	XtGCMask    ਨਕਾਬ;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	ਨਕਾਬ = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_ਨਕਾਬ | GCClipXOrigin | GCClipYOrigin;
	ਮੁੱਲਾਂ.graphics_exposures = False;

	ਜੇ (fs != 0)
	{
	    ਨਕਾਬ |= GCFont;
	    ਮੁੱਲਾਂ.font = fs->fid;
	}

	ਜੇ (lw->primitive.ਸਿਖਰ_shadow_pixmap != None
		&& lw->primitive.ਸਿਖਰ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    ਨਕਾਬ |= GCFillStyle | GCTile;
	    ਮੁੱਲਾਂ.fill_style = FillTiled;
	    ਮੁੱਲਾਂ.tile = lw->primitive.ਸਿਖਰ_shadow_pixmap;
	}

	lw->label.ਪਾਠ_Rect.x += 1;
	lw->label.ਪਾਠ_Rect.y += 1;
	ਜੇ (lw->label.acc_ਪਾਠ != 0)
	{
	    lw->label.acc_ਪਾਠ_Rect.x += 1;
	    lw->label.acc_ਪਾਠ_Rect.y += 1;
	}

	ਮੁੱਲਾਂ.foreground = lw->primitive.ਸਿਖਰ_shadow_color;
	ਮੁੱਲਾਂ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, ਨਕਾਬ,
					       &ਮੁੱਲਾਂ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.ਪਾਠ_Rect.x -= 1;
	lw->label.ਪਾਠ_Rect.y -= 1;
	ਜੇ (lw->label.acc_ਪਾਠ != 0)
	{
	    lw->label.acc_ਪਾਠ_Rect.x -= 1;
	    lw->label.acc_ਪਾਠ_Rect.y -= 1;
	}

	ਮੁੱਲਾਂ.foreground = lw->primitive.bottom_shadow_color;
	ਮੁੱਲਾਂ.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, ਨਕਾਬ,
					       &ਮੁੱਲਾਂ, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.insensitive_GC = insensitiveGC;
    }
}

/*
 * Create all the motif widgets necessary.
 */
    ਖਾਲੀ
gui_x11_create_widgets(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		ਦਲੀਲਾਂ[10];
    ਪੂਰਨ		n;
    XmString	xms;
#ਜੇ_ਅੰਤ

    /*
     * Install the 3D shade effect drawing routines.
     */
    ਜੇ (old_label_expose == ਨਿਰਾ)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_ਵਿੱਥ = gui.border_ਚੌੜਾਈ;

    /*
     * Install the tearOffModel resource converter.
     */
#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#ਜੇ_ਅੰਤ

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(vimShell, XmNdeleteResponse, XmDO_NOTHING, ਨਿਰਾ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_ਚੌੜਾਈ, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_ਚੌੜਾਈ, 0,
	Xm_Nmargin_ਉਚਾਈ, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ਨਿਰਾ);
    gui_motif_menu_colors(vimForm);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	ਪੂਰਨ ac = 0;

# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	Xt_ਸੈੱਟ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_ਸੈੱਟ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_ਸੈੱਟ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_ਸੈੱਟ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_ਸੈੱਟ_Arg(al[ac], Xm_Nright_ਵਿੱਥ, 0); ac++;
# endif
	Xt_ਸੈੱਟ_Arg(al[ac], Xm_Nmargin_ਉਚਾਈ, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_ਬੱਚਾ(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_ਚੌਖਟਾ = XtVaCreateWidget("toolBarFrame",
	xm_ਚੌਖਟਾ_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_ਉਚਾਈ, 0,
	Xm_Nmargin_ਚੌੜਾਈ, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	ਨਿਰਾ);
    gui_motif_menu_colors(tool_Bar_ਚੌਖਟਾ);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_ਪੰਕਤੀ_ਕਾਲਮ_Widget_Class, tool_Bar_ਚੌਖਟਾ,
	XmNchildType, Xm_ਚੌਖਟਾ_WORKAREA_ਬੱਚਾ,
	Xm_Nrow_ਕਾਲਮ_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_ਉਚਾਈ, 0,
	Xm_Nmargin_ਚੌੜਾਈ, 0,
	Xm_Nadjust_ਆਖਰੀ, True,
	ਨਿਰਾ);
    gui_motif_menu_colors(toolBar);

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbindingType, XmNONE); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation, XmVERTICAL); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nback_ਪੰਨਾ_ਮਾਪ, XmNONE); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nback_ਪੰਨਾ_ਸੰਖਿਆ, 0); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nback_ਪੰਨਾ_Placement, Xm_ਸਿਖਰ_ਸੱਜਾ); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNmajorTabSpacing, 0); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNshadowThickness, 0); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_ਸਤਰ = XmCreateNotebook(vimForm, "Vim tabline", ਦਲੀਲਾਂ, n);

    Xt_ਜੋੜੋ_Callback(tab_ਸਤਰ, Xm_Npage_ਬਦਲਿਆ_Callback, (XtCallbackProc)tabline_cb,
			ਨਿਰਾ);
    Xt_ਜੋੜੋ_Event_Handler(tab_ਸਤਰ, Button_Press_ਨਕਾਬ, False,
			(XtEventHandler)tabline_menu_cb, ਨਿਰਾ);

    gui.tabline_ਉਚਾਈ = TABLINE_ਉਚਾਈ;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ, "MinorTabScrollerNext");
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ਨਿਰਾ);
    scroller = Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ, "MinorTabScrollerPrevious");
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ਨਿਰਾ);

    // Create the tabline popup menu
    tab_ਸਤਰ_menu = XmCreatePopupMenu(tab_ਸਤਰ, "tabline popup", ਨਿਰਾ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nuser_ਅੰਕੜੇ, (Xt_ਸੂਚਕ)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((ਅੱਖਰ *)"Close tab", STRING_ਟੈਗ);
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, xms); n++;
    button = Xm_Create_ਧੱਕੋ_Button(tab_ਸਤਰ_menu, "Close", ਦਲੀਲਾਂ, n);
    Xt_ਜੋੜੋ_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ਨਿਰਾ);
    XmStringFree(xms);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nuser_ਅੰਕੜੇ, (Xt_ਸੂਚਕ)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((ਅੱਖਰ *)"New Tab", STRING_ਟੈਗ);
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, xms); n++;
    button = Xm_Create_ਧੱਕੋ_Button(tab_ਸਤਰ_menu, "New Tab", ਦਲੀਲਾਂ, n);
    Xt_ਜੋੜੋ_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ਨਿਰਾ);
    XmStringFree(xms);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nuser_ਅੰਕੜੇ, (Xt_ਸੂਚਕ)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((ਅੱਖਰ *)"Open tab...", STRING_ਟੈਗ);
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, xms); n++;
    button = Xm_Create_ਧੱਕੋ_Button(tab_ਸਤਰ_menu, "Open tab...", ਦਲੀਲਾਂ, n);
    Xt_ਜੋੜੋ_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ਨਿਰਾ);
    XmStringFree(xms);
#ਜੇ_ਅੰਤ

    ਪਾਠ_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_ਚੌੜਾਈ, 0,
	Xm_Nmargin_ਉਚਾਈ, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ਨਿਰਾ);
    gui_motif_scroll_colors(ਪਾਠ_Area_Form);

    ਪਾਠ_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, ਪਾਠ_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.ਪਿੱਠ_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_ਚੌੜਾਈ, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	ਨਿਰਾ);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(ਪਾਠ_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.ਵਿੱਚ_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    ਖਾਲੀ
gui_x11_destroy_widgets(ਖਾਲੀ)
{
    ਪਾਠ_Area = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    menuBar = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_ਪਾਠ_area_pos(
    ਪੂਰਨ	    x ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    y ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    w ਅਣਵਰਤਿਆ,
    ਪੂਰਨ	    h ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gui_x11_ਸੈੱਟ_ਪਿੱਠ_color(ਖਾਲੀ)
{
    ਜੇ (ਪਾਠ_Area != ਨਿਰਾ)
#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	XmChangeColor(ਪਾਠ_Area, gui.ਪਿੱਠ_pixel);
#ਨਹੀਂਤਾਂ
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area,
		  XmNbackground, gui.ਪਿੱਠ_pixel,
		  ਨਿਰਾ);
#ਜੇ_ਅੰਤ
}

/*
 * Manage dialog centered on pointer.
 */
    ਖਾਲੀ
manage_centered(Widget dialog_ਬੱਚਾ)
{
    Widget shell = Xt_ਮਾਪਾ(dialog_ਬੱਚਾ);
    ਖਿੜਕੀ ਜੜ੍ਹ, ਬੱਚਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਚੌੜਾਈ, ਉਚਾਈ, border_ਚੌੜਾਈ, ਡੂੰਘਾਈ;
    ਪੂਰਨ x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_ਮੁੱਲਾਂ(shell, XmNmappedWhenManaged, &mappedWhenManaged, ਨਿਰਾ);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(shell, XmNmappedWhenManaged, False, ਨਿਰਾ);

    Xt_Manage_ਬੱਚਾ(dialog_ਬੱਚਾ);

    // Get the pointer position (x, y)
    XQuery_ਸੂਚਕ(XtDisplay(shell), Xt_ਖਿੜਕੀ(shell), &ਜੜ੍ਹ, &ਬੱਚਾ,
		  &x, &y, &win_x, &win_y, &ਨਕਾਬ);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_ਖਿੜਕੀ(shell), &ਜੜ੍ਹ, &win_x, &win_y,
		 &ਚੌੜਾਈ, &ਉਚਾਈ, &border_ਚੌੜਾਈ, &ਡੂੰਘਾਈ);
    ਚੌੜਾਈ += 2 * border_ਚੌੜਾਈ;
    ਉਚਾਈ += 2 * border_ਚੌੜਾਈ;
    x -= ਚੌੜਾਈ / 2;
    y -= ਉਚਾਈ / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->ਚੌੜਾਈ - ਚੌੜਾਈ;
    maxY = XtScreen(shell)->ਉਚਾਈ - ਉਚਾਈ;
    ਜੇ (x < 0)
	x = 0;
    ਜੇ (x > maxX)
	x = maxX;
    ਜੇ (y < 0)
	y = 0;
    ਜੇ (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(shell, XmNx, x, XmNy, y, ਨਿਰਾ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(shell, XmNmappedWhenManaged, mappedWhenManaged, ਨਿਰਾ);
}

#ਜੇ defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# ਜੇ (Xm_ਸੰਸਕਰਣ <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_ਟੈਗ);
# ਨਹੀਂਤਾਂ
    // Motif 1.2 method
    Xm_Font_List_ਇੰਦਰਾਜ font_list_ਇੰਦਰਾਜ;

    font_list_ਇੰਦਰਾਜ = Xm_Font_List_ਇੰਦਰਾਜ_Create(STRING_ਟੈਗ, XmFONT_IS_FONT,
					    (Xt_ਸੂਚਕ)font);
    font_list = Xm_Font_List_Append_ਇੰਦਰਾਜ(ਨਿਰਾ, font_list_ਇੰਦਰਾਜ);
    Xm_Font_List_ਇੰਦਰਾਜ_Free(&font_list_ਇੰਦਰਾਜ);
# endif
    ਵਾਪਸ font_list;
}

# ਜੇ ((Xm_ਸੰਸਕਰਣ > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_ਸੈੱਟ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_ਇੰਦਰਾਜ font_list_ਇੰਦਰਾਜ;

    font_list_ਇੰਦਰਾਜ = Xm_Font_List_ਇੰਦਰਾਜ_Create(STRING_ਟੈਗ,
					    XmFONT_IS_FONTSET,
					    (Xt_ਸੂਚਕ)*fontset);
    font_list = Xm_Font_List_Append_ਇੰਦਰਾਜ(ਨਿਰਾ, font_list_ਇੰਦਰਾਜ);
    Xm_Font_List_ਇੰਦਰਾਜ_Free(&font_list_ਇੰਦਰਾਜ);
    ਵਾਪਸ font_list;
}
# endif

#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ਸਥਿਰ ਖਾਲੀ gui_motif_ਜੋੜੋ_actext(vimmenu_T *menu);
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
ਸਥਿਰ ਖਾਲੀ toggle_tearoff(Widget wid);
ਸਥਿਰ ਖਾਲੀ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ਸਥਿਰ ਖਾਲੀ submenu_change(vimmenu_T *mp, ਪੂਰਨ colors);

ਸਥਿਰ ਖਾਲੀ do_ਸੈੱਟ_mnemonics(ਪੂਰਨ enable);
ਸਥਿਰ ਪੂਰਨ menu_enabled = TRUE;

    ਖਾਲੀ
gui_mch_enable_menu(ਪੂਰਨ flag)
{
    ਜੇ (flag)
    {
	Xt_Manage_ਬੱਚਾ(menuBar);
# ifdef FEAT_TOOLBAR
	ਜੇ (XtIsManaged(Xt_ਮਾਪਾ(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(Xt_ਮਾਪਾ(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ਨਿਰਾ);
#  ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
#  endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			      ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ਨਿਰਾ);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	Xt_Unmanage_ਬੱਚਾ(menuBar);
# ifdef FEAT_TOOLBAR
	ਜੇ (XtIsManaged(Xt_ਮਾਪਾ(toolBar)))
	{
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(Xt_ਮਾਪਾ(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ਨਿਰਾ);
#  ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
#  endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			      ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_FORM,
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ਨਿਰਾ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    ਖਾਲੀ
gui_motif_ਸੈੱਟ_mnemonics(ਪੂਰਨ enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    ਜੇ (!menu_enabled)
	enable = FALSE;
    do_ਸੈੱਟ_mnemonics(enable);
}

    ਸਥਿਰ ਖਾਲੀ
do_ਸੈੱਟ_mnemonics(ਪੂਰਨ enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	ਜੇ (menu->id != (Widget)0)
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu(vimmenu_T *menu, ਪੂਰਨ idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

# ifdef MOTIF_POPUP
    ਜੇ (menu_is_popup(menu->name))
    {
	Arg arg[2];
	ਪੂਰਨ n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  ਜੇ (Xm_ਸੰਸਕਰਣ <= 1002)
	ਜੇ (mouse_model_popup())
#  endif
	{
	    ਜੇ (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_ਸੈੱਟ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    ਜੇ (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_ਸੈੱਟ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(ਪਾਠ_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	ਵਾਪਸ;
    }
# endif

    ਜੇ (!menu_is_menubar(menu->name)
	    || (ਮਾਪਾ != ਨਿਰਾ && ਮਾਪਾ->submenu_id == (Widget)0))
	ਵਾਪਸ;

    label = XmStringCreate((ਅੱਖਰ *)menu->dname, STRING_ਟੈਗ);
    ਜੇ (label == ਨਿਰਾ)
	ਵਾਪਸ;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (ਮਾਪਾ == ਨਿਰਾ) ? menuBar : ਮਾਪਾ->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_ਸੂਚਕਾਂਕ, idx + (ਮਾਪਾ != ਨਿਰਾ
			   && tearoff_val == (ਪੂਰਨ)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ਨਿਰਾ);
    XmStringFree(label);

    ਜੇ (menu->id == (Widget)0)		// failed
	ਵਾਪਸ;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    ਜੇ (ਮਾਪਾ == ਨਿਰਾ && menu->ਤਰਜੀਹ >= 9999)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(menuBar,
		XmNmenuHelpWidget, menu->id,
		ਨਿਰਾ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_ਜੋੜੋ_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ਨਿਰਾ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_ਪੰਕਤੀ_ਕਾਲਮ_Widget_Class, shell,
	Xm_Nrow_ਕਾਲਮ_Type, XmMENU_PULLDOWN,
	ਨਿਰਾ);
    gui_motif_menu_colors(menu->submenu_id);

    ਜੇ (menu->submenu_id == (Widget)0)		// failed
	ਵਾਪਸ;

# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ਨਿਰਾ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    ਜੇ (ਮਾਪਾ == ਨਿਰਾ)
	gui_mch_compute_menu_ਉਚਾਈ(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_motif_ਜੋੜੋ_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    ਜੇ (menu->actext == ਨਿਰਾ || menu->id == (Widget)0)
	ਵਾਪਸ;

    label = XmStringCreate((ਅੱਖਰ *)menu->actext, STRING_ਟੈਗ);
    ਜੇ (label == ਨਿਰਾ)
	ਵਾਪਸ;
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(menu->id, Xm_Naccelerator_ਪਾਠ, label, ਨਿਰਾ);
    XmStringFree(label);
}

    ਖਾਲੀ
gui_mch_toggle_tearoffs(ਪੂਰਨ enable)
{
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
    ਜੇ (enable)
	tearoff_val = (ਪੂਰਨ)XmTEAR_OFF_ENABLED;
    ਨਹੀਂਤਾਂ
	tearoff_val = (ਪੂਰਨ)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ਜੜ੍ਹ_menu);
# endif
}

# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ਸਥਿਰ ਖਾਲੀ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(wid, XmNtearOffModel, tearoff_val, ਨਿਰਾ);
    ਜੇ (tearoff_val == (ਪੂਰਨ)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ਸਥਿਰ ਖਾਲੀ
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    ਜਦੋਂ (menu != ਨਿਰਾ)
    {
	ਜੇ (!menu_is_popup(menu->name))
	{
	    ਜੇ (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->ਅਗਲਾ;
    }
}
# endif

    ਪੂਰਨ
gui_mch_ਪਾਠ_area_extra_ਉਚਾਈ(ਖਾਲੀ)
{
    Dimension	shadow_ਉਚਾਈ;

    Xt_Va_Get_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form, XmNshadowThickness, &shadow_ਉਚਾਈ, ਨਿਰਾ);
    ਵਾਪਸ shadow_ਉਚਾਈ;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    ਖਾਲੀ
gui_mch_compute_menu_ਉਚਾਈ(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ਸਥਿਰ Dimension	ਉਚਾਈ = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    ਜੇ (id != (Widget)0)
	Xt_Va_Get_ਮੁੱਲਾਂ(id, XmNheight, &ਉਚਾਈ, ਨਿਰਾ);

    // Find any menu Widget, to be able to call XtManageChild()
    ਨਹੀਂਤਾਂ
	FOR_ALL_MENUS(mp)
	    ਜੇ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ਰੋਕੋ;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    ਜੇ (id != (Widget)0)
	Xt_Manage_ਬੱਚਾ(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	ਜੇ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(mp->id, XmNy, &y, ਨਿਰਾ);
	    ਜੇ (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_ਮੁੱਲਾਂ(menuBar,
	Xm_Nmargin_ਉਚਾਈ, &margin,
	XmNshadowThickness, &shadow,
	ਨਿਰਾ);

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
    gui.menu_ਉਚਾਈ = maxy + ਉਚਾਈ - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    ਜੇ (gui.shell_created)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(menuBar, XmNheight, gui.menu_ਉਚਾਈ, ਨਿਰਾ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ਸਥਿਰ ਅੱਖਰ **get_toolbar_pixmap(vimmenu_T *menu, ਅੱਖਰ **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
check_xpm(char_u *ਰਸਤਾ)
{
    Xpm_ਗੁਣ attrs;
    ਪੂਰਨ		ਸਥਿਤੀ;
    Pixmap	ਨਕਾਬ;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    ਸਥਿਤੀ = Xpm_Read_File_ਮੰਜ਼ਿਲ_Pixmap(gui.dpy,
	    ਜੜ੍ਹ_ਖਿੜਕੀ_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (ਅੱਖਰ *)ਰਸਤਾ, &map, &ਨਕਾਬ, &attrs);
    Xpm_Free_ਗੁਣ(&attrs);

    ਜੇ (ਸਥਿਤੀ == XpmSuccess)
	ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ਸਥਿਰ ਅੱਖਰ **
get_toolbar_pixmap(vimmenu_T *menu, ਅੱਖਰ **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    ਅੱਖਰ	**xpm = ਨਿਰਾ;		// xpm array
    ਪੂਰਨ		res;

    *fname = ਨਿਰਾ;
    buf[0] = NUL;			// start with NULL path

    ਜੇ (menu->iconfile != ਨਿਰਾ)
    {
	// Use the "icon="  argument.
	gui_ਲੱਭੋ_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	ਜੇ (res == FAIL && gui_ਲੱਭੋ_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	ਜੇ (res == OK)
	{
	    *fname = (ਅੱਖਰ *)vim_strsave(buf);
	    ਵਾਪਸ tb_blank_xpm;
	}
    }

    ਜੇ (menu->icon_builtin || gui_ਲੱਭੋ_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	ਜੇ (menu->iconidx >= 0 && menu->iconidx
	       < (ਪੂਰਨ)(ਆਕਾਰ(built_ਵਿੱਚ_pixmaps) / ਆਕਾਰ(built_ਵਿੱਚ_pixmaps[0])))
	    xpm = built_ਵਿੱਚ_pixmaps[menu->iconidx];
	ਨਹੀਂਤਾਂ
	    xpm = tb_blank_xpm;
    }

    ਵਾਪਸ xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ਸਥਿਰ ਪੂਰਨ
ਜੋੜੋ_pixmap_ਦਲੀਲਾਂ(vimmenu_T *menu, Arg *ਦਲੀਲਾਂ, ਪੂਰਨ n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    ਜੇ (menu->xpm == ਨਿਰਾ)
    {
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelType, XmSTRING); n++;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (menu->xpm_fname != ਨਿਰਾ)
	{
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Npixmap_ਅੰਕੜੇ, menu->xpm); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    ਵਾਪਸ n;
}
# endif // FEAT_TOOLBAR

    ਖਾਲੀ
gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(vimmenu_T *menu, ਪੂਰਨ idx)
{
    XmString	label;
    vimmenu_T	*ਮਾਪਾ = menu->ਮਾਪਾ;

# ਜੇ (Xm_ਸੰਸਕਰਣ <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    ਜੇ (menu_is_ਬੱਚਾ_ਦਾ_popup(menu) && !mouse_model_popup())
	ਵਾਪਸ;
# endif

# ifdef FEAT_TOOLBAR
    ਜੇ (menu_is_toolbar(ਮਾਪਾ->name))
    {
	WidgetClass	type;
	XmString	xms = ਨਿਰਾ;    // fallback label if pixmap not found
	ਪੂਰਨ		n;
	Arg		ਦਲੀਲਾਂ[18];

	n = 0;
	ਜੇ (menu_is_separator(menu->name))
	{
	    ਅੱਖਰ	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (ਅੱਖਰ *)vim_strchr(menu->name, ':');
	    ਜੇ (cp != ਨਿਰਾ)
		wid = (Dimension)atoi(++cp);
	    ਨਹੀਂਤਾਂ
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNwidth, wid); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nmin_ਚੌੜਾਈ, wid); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation, XmVERTICAL); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNseparatorType, Xm_SHADOW_ETCHED_ਵਿੱਚ); n++;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNhighlightThickness, 1); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNhighlightOnEnter, True); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nmargin_ਚੌੜਾਈ, 0); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nmargin_ਉਚਾਈ, 0); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((ਅੱਖਰ *)menu->dname, STRING_ਟੈਗ);
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelString, xms); n++;

	    n = ਜੋੜੋ_pixmap_ਦਲੀਲਾਂ(menu, ਦਲੀਲਾਂ, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nposition_ਸੂਚਕਾਂਕ, idx); n++;
	ਜੇ (menu->id == ਨਿਰਾ)
	{
	    menu->id = XtCreateManagedWidget((ਅੱਖਰ *)menu->dname,
			type, toolBar, ਦਲੀਲਾਂ, n);
	    ਜੇ (menu->id != ਨਿਰਾ && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_ਜੋੜੋ_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	ਨਹੀਂਤਾਂ
	    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(menu->id, ਦਲੀਲਾਂ, n);
	ਜੇ (xms != ਨਿਰਾ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_ਸੈੱਟ_tip(menu);
#  endif

	menu->ਮਾਪਾ = ਮਾਪਾ;
	menu->submenu_id = ਨਿਰਾ;
	// When adding first item to toolbar it might have to be enabled .
	ਜੇ (!XtIsManaged(Xt_ਮਾਪਾ(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_ਉਚਾਈ = gui_mch_compute_toolbar_ਉਚਾਈ();
	ਵਾਪਸ;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    ਜੇ (ਮਾਪਾ->submenu_id == (Widget)0)
	ਵਾਪਸ;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    ਜੇ (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, ਮਾਪਾ->submenu_id,
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_ਸੂਚਕਾਂਕ, idx + (tearoff_val == (ਪੂਰਨ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ਨਿਰਾ);
	gui_motif_menu_colors(menu->id);
	ਵਾਪਸ;
    }

    label = XmStringCreate((ਅੱਖਰ *)menu->dname, STRING_ਟੈਗ);
    ਜੇ (label == ਨਿਰਾ)
	ਵਾਪਸ;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ਧੱਕੋ_Button_Widget_Class, ਮਾਪਾ->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_ਸੂਚਕਾਂਕ, idx + (tearoff_val == (ਪੂਰਨ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ਨਿਰਾ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    ਜੇ (menu->id != (Widget)0)
    {
	Xt_ਜੋੜੋ_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_ਸੂਚਕ)menu);
	// add accelerator text
	gui_motif_ਜੋੜੋ_actext(menu);
    }
}

# ਜੇ (Xm_ਸੰਸਕਰਣ <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    ਖਾਲੀ
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    ਪੂਰਨ		idx = 0;

    // When GUI hasn't started the menus have not been created.
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    ਜਦੋਂ (menu)
    {
      ਜੇ (menu->children != ਨਿਰਾ)
      {
	  ਜੇ (menu_is_popup(menu->name))
	  {
	      ਜੇ (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_ਜੋੜੋ_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      ਨਹੀਂਤਾਂ
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      ਨਹੀਂਤਾਂ ਜੇ (menu_is_ਬੱਚਾ_ਦਾ_popup(menu))
      {
	  ਜੇ (mouse_model_popup())
	      gui_mch_ਜੋੜੋ_menu_ਵਸਤੂ(menu, idx);
	  ਨਹੀਂਤਾਂ
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->ਅਗਲਾ;
      ++idx;
    }
}
# endif

    ਖਾਲੀ
gui_mch_new_menu_colors(ਖਾਲੀ)
{
    ਜੇ (menuBar == (Widget)0)
	ਵਾਪਸ;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(tool_Bar_ਚੌਖਟਾ);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ਜੜ੍ਹ_menu, TRUE);
}

    ਖਾਲੀ
gui_mch_new_menu_font(ਖਾਲੀ)
{
    ਜੇ (menuBar == (Widget)0)
	ਵਾਪਸ;
    submenu_change(ਜੜ੍ਹ_menu, FALSE);
    {
	Dimension   ਉਚਾਈ;
	Position w, h;

	Xt_Va_Get_ਮੁੱਲਾਂ(menuBar, XmNheight, &ਉਚਾਈ, ਨਿਰਾ);
	gui.menu_ਉਚਾਈ = ਉਚਾਈ;

	Xt_Va_Get_ਮੁੱਲਾਂ(vimShell, XtNwidth, &w, XtNheight, &h, ਨਿਰਾ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_ਸਥਿਤੀ_area_ਉਚਾਈ()
# endif
		     );
    }
    gui_ਸੈੱਟ_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# ਜੇ defined(FEAT_BEVAL_GUI)
    ਖਾਲੀ
gui_mch_new_tooltip_font(ਖਾਲੀ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    ਜੇ (toolBar == (Widget)0)
	ਵਾਪਸ;

    menu = gui_ਲੱਭੋ_menu((char_u *)"ToolBar");
    ਜੇ (menu != ਨਿਰਾ)
	submenu_change(menu, FALSE);
#  endif
}

    ਖਾਲੀ
gui_mch_new_tooltip_colors(ਖਾਲੀ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    ਜੇ (toolBar == (Widget)0)
	ਵਾਪਸ;

    toolbar = gui_ਲੱਭੋ_menu((char_u *)"ToolBar");
    ਜੇ (toolbar != ਨਿਰਾ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ਸਥਿਰ ਖਾਲੀ
submenu_change(
    vimmenu_T	*menu,
    ਪੂਰਨ		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    ਲਈ (mp = menu; mp != ਨਿਰਾ; mp = mp->ਅਗਲਾ)
    {
	ਜੇ (mp->id != (Widget)0)
	{
	    ਜੇ (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		ਜੇ (mp->xpm != ਨਿਰਾ)
		{
		    ਪੂਰਨ		n = 0;
		    Arg		ਦਲੀਲਾਂ[18];

		    n = ਜੋੜੋ_pixmap_ਦਲੀਲਾਂ(mp, ਦਲੀਲਾਂ, n);
		    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(mp->id, ਦਲੀਲਾਂ, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ਜੇ (mp->tip != ਨਿਰਾ)
		{
		    Arg ਦਲੀਲਾਂ[2];

		    ਦਲੀਲਾਂ[0].name = XmNbackground;
		    ਦਲੀਲਾਂ[0].ਮੁੱਲ = gui.tooltip_bg_pixel;
		    ਦਲੀਲਾਂ[1].name = XmNforeground;
		    ਦਲੀਲਾਂ[1].ਮੁੱਲ = gui.tooltip_fg_pixel;
		    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(mp->tip->balloonLabel, &ਦਲੀਲਾਂ[0], Xt_ਸੰਖਿਆ(ਦਲੀਲਾਂ));
		}
#  endif
# endif
	    }
	    ਨਹੀਂਤਾਂ
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ਜੇ (mp->tip != ਨਿਰਾ)
		{
		    Arg ਦਲੀਲਾਂ[1];

		    ਦਲੀਲਾਂ[0].name = XmNfontList;
		    ਦਲੀਲਾਂ[0].ਮੁੱਲ = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(mp->tip->balloonLabel, &ਦਲੀਲਾਂ[0], Xt_ਸੰਖਿਆ(ਦਲੀਲਾਂ));
		}
# endif
	    }
	}

	ਜੇ (mp->children != ਨਿਰਾ)
	{
# ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	    // Set the colors/font for the tear off widget
	    ਜੇ (mp->submenu_id != (Widget)0)
	    {
		ਜੇ (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		ਨਹੀਂਤਾਂ
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
    ਖਾਲੀ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // Please be sure to destroy the parent widget first (i.e. menu->id).
    // On the other hand, problems have been reported that the submenu must be
    // deleted first...
    ਜੇ (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    ਜੇ (menu->id == (Widget)0)
	ਵਾਪਸ;

    Widget	    ਮਾਪਾ;

    ਮਾਪਾ = Xt_ਮਾਪਾ(menu->id);
# ਜੇ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    ਜੇ (ਮਾਪਾ == toolBar && menu->tip != ਨਿਰਾ)
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
	menu->tip = ਨਿਰਾ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    ਜੇ (ਮਾਪਾ == menuBar)
	gui_mch_compute_menu_ਉਚਾਈ((Widget)0);
# ifdef FEAT_TOOLBAR
    ਨਹੀਂਤਾਂ ਜੇ (ਮਾਪਾ == toolBar)
    {
	Cardinal    ਅੰਕ_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_ਮੁੱਲਾਂ(toolBar, XmNnumChildren, &ਅੰਕ_children, ਨਿਰਾ);
	ਜੇ (ਅੰਕ_children == 0)
	    gui_mch_show_toolbar(FALSE);
	ਨਹੀਂਤਾਂ
	    gui.toolbar_ਉਚਾਈ = gui_mch_compute_toolbar_ਉਚਾਈ();
    }
# endif
}

    ਖਾਲੀ
gui_mch_show_popupmenu(vimmenu_T *menu ਅਣਵਰਤਿਆ)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_ਆਖਰੀ_mouse_event());
    Xt_Manage_ਬੱਚਾ(menu->submenu_id);
# endif
}

#ਜੇ_ਅੰਤ // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    ਖਾਲੀ
gui_mch_def_colors(ਖਾਲੀ)
{
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#ਜੇ_ਅੰਤ
}


/*
 * Scrollbar stuff.
 */

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_thumb(
    scrollbar_T *sb,
    ਲੰਮਾ	val,
    ਲੰਮਾ	ਮਾਪ,
    ਲੰਮਾ	max)
{
    ਜੇ (sb->id != (Widget)0)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_ਮਾਪ, ਮਾਪ,
		  XmNpageIncrement, (ਮਾਪ > 2 ? ਮਾਪ - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_pos(
    scrollbar_T *sb,
    ਪੂਰਨ		x,
    ਪੂਰਨ		y,
    ਪੂਰਨ		w,
    ਪੂਰਨ		h)
{
    ਜੇ (sb->id == (Widget)0)
	ਵਾਪਸ;

    ਜੇ (sb->type == SBAR_ਖੱਬਾ || sb->type == SBAR_ਸੱਜਾ)
    {
	ਜੇ (y == 0)
	    h -= gui.border_ਵਿੱਥ;
	ਨਹੀਂਤਾਂ
	    y -= gui.border_ਵਿੱਥ;
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(sb->id,
		Xm_Ntop_ਵਿੱਥ, y,
		Xm_Nbottom_ਵਿੱਥ, -y - h,
		XmNwidth, w,
		ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(sb->id,
		Xm_Ntop_ਵਿੱਥ, y,
		Xm_Nleft_ਵਿੱਥ, x,
		Xm_Nright_ਵਿੱਥ, gui.which_scrollbars[SBAR_ਸੱਜਾ]
						     ? gui.scrollbar_ਚੌੜਾਈ : 0,
		XmNheight, h,
		ਨਿਰਾ);
    Xt_Manage_ਬੱਚਾ(sb->id);
}

    ਪੂਰਨ
gui_mch_get_scrollbar_xpadding(ਖਾਲੀ)
{
    ਪੂਰਨ xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_ਮੁੱਲਾਂ(ਪਾਠ_Area, XtNwidth, &tw, XtNx, &tx, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(vimShell, XtNwidth, &ww, ਨਿਰਾ);
    xpad = ww - tw - tx - gui.scrollbar_ਚੌੜਾਈ;
    ਵਾਪਸ (xpad < 0) ? 0 : xpad;
}

    ਪੂਰਨ
gui_mch_get_scrollbar_ypadding(ਖਾਲੀ)
{
    ਪੂਰਨ ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_ਮੁੱਲਾਂ(ਪਾਠ_Area, XtNheight, &th, XtNy, &ty, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(vimShell, XtNheight, &wh, ਨਿਰਾ);
    ypad = wh - th - ty - gui.scrollbar_ਉਚਾਈ;
    ਵਾਪਸ (ypad < 0) ? 0 : ypad;
}

    ਖਾਲੀ
gui_mch_enable_scrollbar(scrollbar_T *sb, ਪੂਰਨ flag)
{
    Arg		ਦਲੀਲਾਂ[16];
    ਪੂਰਨ		n;

    ਜੇ (sb->id == (Widget)0)
	ਵਾਪਸ;

    n = 0;
    ਜੇ (flag)
    {
	ਚੋਣ (sb->type)
	{
	    ਹਾਲਤ SBAR_ਖੱਬਾ:
		Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nleft_ਵਿੱਥ, gui.scrollbar_ਚੌੜਾਈ); n++;
		ਰੋਕੋ;

	    ਹਾਲਤ SBAR_ਸੱਜਾ:
		Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nright_ਵਿੱਥ, gui.scrollbar_ਚੌੜਾਈ); n++;
		ਰੋਕੋ;

	    ਹਾਲਤ SBAR_BOTTOM:
		Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nbottom_ਵਿੱਥ, gui.scrollbar_ਉਚਾਈ);n++;
		ਰੋਕੋ;
	}
	Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area, ਦਲੀਲਾਂ, n);
	Xt_Manage_ਬੱਚਾ(sb->id);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ਚੋਣ (sb->type)
	    {
		ਹਾਲਤ SBAR_ਖੱਬਾ:
		    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nleft_ਵਿੱਥ, 0); n++;
		    ਰੋਕੋ;

		ਹਾਲਤ SBAR_ਸੱਜਾ:
		    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nright_ਵਿੱਥ, 0); n++;
		    ਰੋਕੋ;

		ਹਾਲਤ SBAR_BOTTOM:
		    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nbottom_ਵਿੱਥ, 0);n++;
		    ਰੋਕੋ;
	    }
	    Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area, ਦਲੀਲਾਂ, n);
	}
	Xt_Unmanage_ਬੱਚਾ(sb->id);
    }
}

    ਖਾਲੀ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ਪੂਰਨ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		ਦਲੀਲਾਂ[16];
    ਪੂਰਨ		n = 0;

    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNminimum, 0); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ਚੋਣ (sb->type)
    {
	ਹਾਲਤ SBAR_ਖੱਬਾ:
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNwidth, gui.scrollbar_ਚੌੜਾਈ); n++;
	    ਰੋਕੋ;

	ਹਾਲਤ SBAR_ਸੱਜਾ:
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNwidth, gui.scrollbar_ਚੌੜਾਈ); n++;
	    ਰੋਕੋ;

	ਹਾਲਤ SBAR_BOTTOM:
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNheight, gui.scrollbar_ਉਚਾਈ); n++;
	    ਰੋਕੋ;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, ਪਾਠ_Area_Form, ਦਲੀਲਾਂ, n);
    ਜੇ (sb->id == (Widget)0)
	ਵਾਪਸ;

    gui_mch_ਸੈੱਟ_scrollbar_colors(sb);
    Xt_ਜੋੜੋ_Callback(sb->id, Xm_Nvalue_ਬਦਲਿਆ_Callback,
	    scroll_cb, (Xt_ਸੂਚਕ)sb->ident);
    Xt_ਜੋੜੋ_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_ਸੂਚਕ)sb->ident);
    Xt_ਜੋੜੋ_Event_Handler(sb->id, ਕੁੰਜੀ_Press_ਨਕਾਬ, FALSE, gui_x11_ਕੁੰਜੀ_ਪ੍ਰਾਪਤੀ_cb,
	    (Xt_ਸੂਚਕ)0);
}

    ਖਾਲੀ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ਜੇ (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_scrollbar_colors(scrollbar_T *sb)
{
    ਜੇ (sb->id != (Widget)0)
    {
	ਜੇ (gui.scroll_bg_pixel != INVALCOLOR)
	{
#ਜੇ (Xm_ਸੰਸਕਰਣ>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#ਜੇ_ਅੰਤ

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ਨਿਰਾ);
	}

	ਜੇ (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ਨਿਰਾ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    ਜੇ (sb == &gui.bottom_sbar && ਪਾਠ_Area_Form != (Widget)0)
	gui_motif_scroll_colors(ਪਾਠ_Area_Form);
}

/*
 * Miscellaneous stuff:
 */

    ਖਿੜਕੀ
gui_x11_get_wid(ਖਾਲੀ)
{
    ਵਾਪਸ(Xt_ਖਿੜਕੀ(ਪਾਠ_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ਸਥਿਰ ਖਾਲੀ
do_mnemonic(Widget w, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keycode)
{
    WidgetList	    children;
    ਪੂਰਨ		    ਅੰਕ_Children, i;
    Boolean	    isMenu;
    ਕੁੰਜੀ_Sym	    mnemonic = '\0';
    ਅੱਖਰ	    mneString[2];
    Widget	    user_ਅੰਕੜੇ_2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   ਪੰਕਤੀ_ਖਾਨਾ_Type;

    ਜੇ (Xt_Is_ਸੰਯੁਕਤ(w))
    {
	ਜੇ (XtClass(w) == xm_ਪੰਕਤੀ_ਕਾਲਮ_Widget_Class)
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(w, Xm_Nrow_ਕਾਲਮ_Type, &ਪੰਕਤੀ_ਖਾਨਾ_Type, ਨਿਰਾ);
	    isMenu = (ਪੰਕਤੀ_ਖਾਨਾ_Type != (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)XmWORK_AREA);
	}
	ਨਹੀਂਤਾਂ
	    isMenu = False;
	ਜੇ (!isMenu)
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(w, XmNchildren, &children, XmNnumChildren,
			  &ਅੰਕ_Children, ਨਿਰਾ);
	    ਲਈ (i = 0; i < ਅੰਕ_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	Xt_Va_Get_ਮੁੱਲਾਂ(w, XmNmnemonic, &mnemonic, ਨਿਰਾ);
	ਜੇ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    ਜੇ (XKeysym_ਮੰਜ਼ਿਲ_Keycode(XtDisplay(Xt_ਮਾਪਾ(w)),
				       XString_ਮੰਜ਼ਿਲ_Keysym(mneString)) == keycode)
	    {
		ਜੇ (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_ਮੁੱਲਾਂ(w, Xm_Nuser_ਅੰਕੜੇ, &user_ਅੰਕੜੇ_2, ਨਿਰਾ);
		    ਜੇ (user_ਅੰਕੜੇ_2 != ਨਿਰਾ && XtIsWidget(user_ਅੰਕੜੇ_2))
			XmProcessTraversal(user_ਅੰਕੜੇ_2, XmTRAVERSE_CURRENT);
		}
		ਨਹੀਂਤਾਂ
		{
		    XKeyPressedEvent ਕੁੰਜੀ_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_ਖੇਤਰ(ਕੁੰਜੀ_Event);
		    ਕੁੰਜੀ_Event.type = ਕੁੰਜੀ_Press;
		    ਕੁੰਜੀ_Event.serial = 1;
		    ਕੁੰਜੀ_Event.send_event = True;
		    ਕੁੰਜੀ_Event.display = XtDisplay(w);
		    ਕੁੰਜੀ_Event.ਖਿੜਕੀ_2 = Xt_ਖਿੜਕੀ(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & ਕੁੰਜੀ_Event,
								     ਨਿਰਾ, 0);
		}
	    }
	}
    }
}

/*
 * Callback routine for dialog mnemonic processing.
 */
    ਸਥਿਰ ਖਾਲੀ
mnemonic_event(
	Widget	    w,
	Xt_ਸੂਚਕ   call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
	XKeyEvent   *event,
	Boolean	    *b ਅਣਵਰਤਿਆ)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_mnemonic_grabs(Widget dialog, Widget w)
{
    ਅੱਖਰ	    mneString[2];
    WidgetList	    children;
    ਪੂਰਨ		    ਅੰਕ_Children, i;
    Boolean	    isMenu;
    ਕੁੰਜੀ_Sym	    mnemonic = '\0';
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ   ਪੰਕਤੀ_ਖਾਨਾ_Type;

    ਜੇ (Xt_Is_ਸੰਯੁਕਤ(w))
    {
	ਜੇ (XtClass(w) == xm_ਪੰਕਤੀ_ਕਾਲਮ_Widget_Class)
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(w, Xm_Nrow_ਕਾਲਮ_Type, &ਪੰਕਤੀ_ਖਾਨਾ_Type, ਨਿਰਾ);
	    isMenu = (ਪੰਕਤੀ_ਖਾਨਾ_Type != (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)XmWORK_AREA);
	}
	ਨਹੀਂਤਾਂ
	    isMenu = False;
	ਜੇ (!isMenu)
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(w, XmNchildren, &children, XmNnumChildren,
							  &ਅੰਕ_Children, ਨਿਰਾ);
	    ਲਈ (i = 0; i < ਅੰਕ_Children; i++)
		ਜੋੜੋ_mnemonic_grabs(dialog, children[i]);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	Xt_Va_Get_ਮੁੱਲਾਂ(w, XmNmnemonic, &mnemonic, ਨਿਰਾ);
	ਜੇ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_ਕੁੰਜੀ(dialog, XKeysym_ਮੰਜ਼ਿਲ_Keycode(XtDisplay(dialog),
						  XString_ਮੰਜ਼ਿਲ_Keysym(mneString)),
		    Mod1_ਨਕਾਬ, True, Grab_ਬਹੁਲਕ_Async, Grab_ਬਹੁਲਕ_Async);
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
    ਸਥਿਰ ਖਾਲੀ
activate_dialog_mnemonics(Widget dialog)
{
    ਜੇ (!dialog)
	ਵਾਪਸ;

    Xt_ਜੋੜੋ_Event_Handler(dialog, ਕੁੰਜੀ_Press_ਨਕਾਬ, False,
			   (XtEventHandler) mnemonic_event, (Xt_ਸੂਚਕ) ਨਿਰਾ);
    ਜੋੜੋ_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ਸਥਿਰ ਖਾਲੀ
suppress_dialog_mnemonics(Widget dialog)
{
    ਜੇ (!dialog)
	ਵਾਪਸ;

    Xt_Ungrab_ਕੁੰਜੀ(dialog, Any_ਕੁੰਜੀ, Mod1_ਨਕਾਬ);
    XtRemoveEventHandler(dialog, ਕੁੰਜੀ_Press_ਨਕਾਬ, False,
			   (XtEventHandler) mnemonic_event, (Xt_ਸੂਚਕ) ਨਿਰਾ);
}

#ਜੇ defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    ਜੇ (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_ਸੈੱਟ *)&gui.fontset);
	ਜੇ (fl != ਨਿਰਾ)
	{
	    ਜੇ (XtIsManaged(id))
	    {
		Xt_Unmanage_ਬੱਚਾ(id);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ਬੱਚਾ(id);
	    }
	    ਨਹੀਂਤਾਂ
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
	    XmFontListFree(fl);
	}
    }
# ਨਹੀਂਤਾਂ
    ਜੇ (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	ਜੇ (fl != ਨਿਰਾ)
	{
	    ਜੇ (XtIsManaged(id))
	    {
		Xt_Unmanage_ਬੱਚਾ(id);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ਬੱਚਾ(id);
	    }
	    ਨਹੀਂਤਾਂ
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ dialog_callback_arg
{
    ਅੱਖਰ *  ਦਲੀਲਾਂ;   // not used right now
    ਪੂਰਨ	    id;
} dcbarg_T;

ਸਥਿਰ Widget dialog_wgt;
ਸਥਿਰ ਅੱਖਰ *browse_fname = ਨਿਰਾ;
ਸਥਿਰ Xm_String_Char_ਸੈੱਟ charset = (Xm_String_Char_ਸੈੱਟ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ਸਥਿਰ ਖਾਲੀ DialogCancelCB(Widget, Xt_ਸੂਚਕ, Xt_ਸੂਚਕ);
ਸਥਿਰ ਖਾਲੀ DialogAcceptCB(Widget, Xt_ਸੂਚਕ, Xt_ਸੂਚਕ);

/*
 * This function is used to translate the predefined label text of the
 * precomposed dialogs. We do this explicitly to allow:
 *
 * - usage of gettext for translation, as in all the other places.
 *
 * - equalize the messages between different GUI implementations as far as
 * possible.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_predefined_label(Widget ਮਾਪਾ, String name, ਅੱਖਰ *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *ਅਗਲਾ;
    ਕੁੰਜੀ_Sym	mnemonic = NUL;

    w = Xt_Name_ਮੰਜ਼ਿਲ_Widget(ਮਾਪਾ, name);

    ਜੇ (!w)
	ਵਾਪਸ;

    p = vim_strsave((char_u *)new_label);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (ਅਗਲਾ = p; *ਅਗਲਾ; ++ਅਗਲਾ)
    {
	ਜੇ (*ਅਗਲਾ == DLG_HOTKEY_CHAR)
	{
	    ਪੂਰਨ len = STRLEN(ਅਗਲਾ);

	    ਜੇ (len > 0)
	    {
		mch_memmove(ਅਗਲਾ, ਅਗਲਾ + 1, len);
		mnemonic = ਅਗਲਾ[0];
	    }
	}
    }

    str = XmStringCreate((ਅੱਖਰ *)p, STRING_ਟੈਗ);
    vim_free(p);

    ਜੇ (str != ਨਿਰਾ)
    {
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ਨਿਰਾ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_predefined_fontlist(Widget ਮਾਪਾ, String name)
{
    Widget w;
    w = Xt_Name_ਮੰਜ਼ਿਲ_Widget(ਮਾਪਾ, name);

    ਜੇ (!w)
	ਵਾਪਸ;

    ਸੈੱਟ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    ਪੂਰਨ		saving ਅਣਵਰਤਿਆ,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext ਅਣਵਰਤਿਆ,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*ਨਮੂਨਾ;
    char_u	*tofree = ਨਿਰਾ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ਨਿਰਾ, 0);

    ਜੇ (initdir == ਨਿਰਾ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    ਜੇ (dflt == ਨਿਰਾ)
	dflt = (char_u *)"";
    ਨਹੀਂਤਾਂ ਜੇ (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	ਜੋੜੋ_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    ਜੇ (filter == ਨਿਰਾ)
	ਨਮੂਨਾ = (char_u *)"";
    ਨਹੀਂਤਾਂ
    {
	char_u	*s, *p;

	s = filter;
	ਲਈ (p = filter; *p != NUL; ++p)
	{
	    ਜੇ (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    ਜੇ (*p == ';' || *p == '\n')	// end of (first) pattern
		ਰੋਕੋ;
	}
	ਨਮੂਨਾ = vim_strnsave(s, p - s);
	tofree = ਨਮੂਨਾ;
	ਜੇ (ਨਮੂਨਾ == ਨਿਰਾ)
	    ਨਮੂਨਾ = (char_u *)"";
    }

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (ਅੱਖਰ *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (ਅੱਖਰ *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (ਅੱਖਰ *)ਨਮੂਨਾ, STRLEN(ਨਮੂਨਾ) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (ਅੱਖਰ *)title, STRLEN(title) + 1,
	ਨਿਰਾ);

    ਸੈੱਟ_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "Dir", _("Directories"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "Help", _("&Help"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "Items", _("Files"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "OK", _("&OK"));
    ਸੈੱਟ_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    ਸੈੱਟ_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    ਸੈੱਟ_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    ਜੇ (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ਨਿਰਾ);

    Xt_ਜੋੜੋ_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_ਸੂਚਕ)0);
    Xt_ਜੋੜੋ_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_ਸੂਚਕ)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_ਬੱਚਾ(Xm_File_Selection_Box_Get_ਬੱਚਾ(dialog_wgt,
					(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    ਕਰੋ
    {
	XtAppProcessEvent(Xt_Widget_ਮੰਜ਼ਿਲ_Application_Context(dialog_wgt),
	    (Xt_ਨਿਵੇਸ਼_ਨਕਾਬ)XtIMAll);
    } ਜਦੋਂ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    ਜੇ (browse_fname == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਵਾਪਸ vim_strsave((char_u *)browse_fname);
}

/*
 * The code below was originally taken from
 *	/usr/examples/motif/xmsamplers/xmeditor.c
 * on Digital Unix 4.0d, but heavily modified.
 */

/*
 * Process callback from Dialog cancel actions.
 */
    ਸਥਿਰ ਖਾਲੀ
DialogCancelCB(
    Widget	w ਅਣਵਰਤਿਆ,		//  widget id
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,	//  data from application
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)	//  data from widget class
{
    ਜੇ (browse_fname != ਨਿਰਾ)
    {
	XtFree(browse_fname);
	browse_fname = ਨਿਰਾ;
    }
    Xt_Unmanage_ਬੱਚਾ(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ਸਥਿਰ ਖਾਲੀ
DialogAcceptCB(
    Widget	w ਅਣਵਰਤਿਆ,		//  widget id
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,	//  data from application
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    ਜੇ (browse_fname != ਨਿਰਾ)
    {
	XtFree(browse_fname);
	browse_fname = ਨਿਰਾ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_ਅੰਕੜੇ;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->ਮੁੱਲ, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_ਬੱਚਾ(dialog_wgt);
}

#ਜੇ_ਅੰਤ // FEAT_BROWSE

#ਜੇ defined(FEAT_GUI_DIALOG)

ਸਥਿਰ ਪੂਰਨ	dialog_ਸਥਿਤੀ;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ਸਥਿਰ ਖਾਲੀ
keyhit_callback(
    Widget		w,
    Xt_ਸੂਚਕ		client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    XEvent		*event,
    Boolean		*cont ਅਣਵਰਤਿਆ)
{
    ਅੱਖਰ	buf[2];
    ਕੁੰਜੀ_Sym	ਕੁੰਜੀ_sym;

    ਜੇ (XLookupString(&(event->xkey), buf, 2, &ਕੁੰਜੀ_sym, ਨਿਰਾ) == 1)
    {
	ਜੇ (*buf == CAR)
	    dialog_ਸਥਿਤੀ = 1;
	ਨਹੀਂਤਾਂ ਜੇ (*buf == ESC)
	    dialog_ਸਥਿਤੀ = 2;
    }
    ਜੇ ((ਕੁੰਜੀ_sym == XK_ਖੱਬਾ || ਕੁੰਜੀ_sym == XK_ਸੱਜਾ)
	    && !(event->xkey.state & ਖਿਸਕਾਓ_ਨਕਾਬ))
	Xm_ਪਾਠ_ਖੇਤਰ_Clear_Selection(w, Xt_ਆਖਰੀ_Timestamp_Processed(gui.dpy));
}

    ਸਥਿਰ ਖਾਲੀ
butproc(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    dialog_ਸਥਿਤੀ = (ਪੂਰਨ)(ਲੰਮਾ)client_ਅੰਕੜੇ_2 + 1;
}

# ifdef HAVE_XPM

    ਸਥਿਰ Widget
create_pixmap_label(
    Widget	ਮਾਪਾ,
    String	name,
    ਅੱਖਰ	**ਅੰਕੜੇ,
    ArgList	ਦਲੀਲਾਂ,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    ਪੂਰਨ			ਡੂੰਘਾਈ;
    Pixmap		pixmap = 0;
    Xpm_ਗੁਣ	attr;
    Boolean		rs;
    Xpm_Color_ਚਿੰਨ੍ਹ	color[5] =
    {
	{"none", ਨਿਰਾ, 0},
	{"iconColor1", ਨਿਰਾ, 0},
	{"bottomShadowColor", ਨਿਰਾ, 0},
	{"topShadowColor", ਨਿਰਾ, 0},
	{"selectColor", ਨਿਰਾ, 0}
    };

    label = XmCreateLabelGadget(ਮਾਪਾ, name, ਦਲੀਲਾਂ, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_ਦਾ_Object(label);
    scr = Xt_Screen_ਦਾ_Object(label);
    Xt_Va_Get_ਮੁੱਲਾਂ(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_ਮਾਪਾ(label),
		  XmNdepth, &ਡੂੰਘਾਈ,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ਨਿਰਾ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_ਡੂੰਘਾਈ;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.ਡੂੰਘਾਈ = ਡੂੰਘਾਈ;
    Xpm_Create_Pixmap_From_ਅੰਕੜੇ(dsp, ਜੜ੍ਹ_ਖਿੜਕੀ_ਦਾ_Screen(scr),
		    ਅੰਕੜੇ, &pixmap, ਨਿਰਾ, &attr);

    Xt_Va_Get_ਮੁੱਲਾਂ(label, Xm_Nrecompute_ਮਾਪ, &rs, ਨਿਰਾ);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(label, Xm_Nrecompute_ਮਾਪ, True, ਨਿਰਾ);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ਨਿਰਾ);
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(label, Xm_Nrecompute_ਮਾਪ, rs, ਨਿਰਾ);

    ਵਾਪਸ label;
}
# endif

    ਪੂਰਨ
gui_mch_dialog(
    ਪੂਰਨ		type ਅਣਵਰਤਿਆ,
    char_u	*title,
    char_u	*ਸੁਨੇਹਾ_2,
    char_u	*button_ਨਾਂ,
    ਪੂਰਨ		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    ਪੂਰਨ		ex_cmd ਅਣਵਰਤਿਆ)
{
    char_u		*buts;
    char_u		*p, *ਅਗਲਾ;
    XtAppContext	app;
    XmString		label;
    ਪੂਰਨ			butcount;
    Widget		w;
    Widget		dialogform = ਨਿਰਾ;
    Widget		form = ਨਿਰਾ;
    Widget		dialogtextfield = ਨਿਰਾ;
    Widget		*buttons;
    Widget		sep_form = ਨਿਰਾ;
    Boolean		vertical;
    Widget		separator = ਨਿਰਾ;
    ਪੂਰਨ			n;
    Arg			ਦਲੀਲਾਂ[6];
# ifdef HAVE_XPM
    ਅੱਖਰ		**icon_ਅੰਕੜੇ_2 = ਨਿਰਾ;
    Widget		dialogpixmap = ਨਿਰਾ;
# endif

    ਜੇ (title == ਨਿਰਾ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (ਅੱਖਰ *)"dialog", ਨਿਰਾ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ਨਿਰਾ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((ਅੱਖਰ *)title);
    ਜੇ (label == ਨਿਰਾ)
	ਵਾਪਸ -1;
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ਨਿਰਾ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_ਨਾਂ);
    ਜੇ (buts == ਨਿਰਾ)
	ਵਾਪਸ -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    ਲਈ (p = buts; *p; ++p)
	ਜੇ (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    ਜੇ (buttons == ਨਿਰਾ)
    {
	vim_free(buts);
	ਵਾਪਸ -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    ਲਈ (butcount = 0; *p; ++butcount)
    {
	ਕੁੰਜੀ_Sym mnemonic = NUL;

	ਲਈ (ਅਗਲਾ = p; *ਅਗਲਾ; ++ਅਗਲਾ)
	{
	    ਜੇ (*ਅਗਲਾ == DLG_HOTKEY_CHAR)
	    {
		ਪੂਰਨ len = STRLEN(ਅਗਲਾ);

		ਜੇ (len > 0)
		{
		    mch_memmove(ਅਗਲਾ, ਅਗਲਾ + 1, len);
		    mnemonic = ਅਗਲਾ[0];
		}
	    }
	    ਜੇ (*ਅਗਲਾ == DLG_BUTTON_SEP)
	    {
		*ਅਗਲਾ++ = NUL;
		ਰੋਕੋ;
	    }
	}
	label = XmStringCreate(_((ਅੱਖਰ *)p), STRING_ਟੈਗ);
	ਜੇ (label == ਨਿਰਾ)
	    ਰੋਕੋ;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ਧੱਕੋ_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_ਵਿੱਥ, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ਨਿਰਾ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	ਜੇ (butcount > 0)
	{
	    ਜੇ (vertical)
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (*ਅਗਲਾ == NUL)
		{
		    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_ਵਿੱਥ, 4,
			    ਨਿਰਾ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_ਵਿੱਥ, 4,
			    ਨਿਰਾ);
		    Xt_Manage_ਬੱਚਾ(sep_form);
		}
		ਨਹੀਂਤਾਂ
		{
		    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ਨਿਰਾ);
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (!vertical)
	{
	    ਜੇ (*ਅਗਲਾ == NUL)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_ਵਿੱਥ, 4,
			ਨਿਰਾ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_ਵਿੱਥ, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_ਵਿੱਥ, 4,
			ਨਿਰਾ);
		Xt_Manage_ਬੱਚਾ(sep_form);
	    }
	    ਨਹੀਂਤਾਂ
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_ਵਿੱਥ, 4,
			ਨਿਰਾ);
	}

	Xt_ਜੋੜੋ_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_ਸੂਚਕ)(ਲੰਮਾ)butcount);
	p = ਅਗਲਾ;
    }
    vim_free(buts);

    separator = (Widget) 0;
    ਜੇ (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomWidget, buttons[0]); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nbottom_ਵਿੱਥ, 4); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", ਦਲੀਲਾਂ, n);
	Xt_Manage_ਬੱਚਾ(separator);
    }

    ਜੇ (textfield != ਨਿਰਾ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_ਪਾਠ_ਖੇਤਰ_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ਨਿਰਾ);
	ਜੇ (butcount > 0)
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ਨਿਰਾ);

	ਸੈੱਟ_fontlist(dialogtextfield);
	Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_String(dialogtextfield, (ਅੱਖਰ *)textfield);
	Xt_Manage_ਬੱਚਾ(dialogtextfield);
	Xt_ਜੋੜੋ_Event_Handler(dialogtextfield, ਕੁੰਜੀ_Press_ਨਕਾਬ, False,
			    (XtEventHandler)keyhit_callback, (Xt_ਸੂਚਕ)ਨਿਰਾ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ਨਿਰਾ);
    Xt_Manage_ਬੱਚਾ(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ਚੋਣ (type)
    {
	ਹਾਲਤ VIM_GENERIC:
	    icon_ਅੰਕੜੇ_2 = generic_xpm;
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_ERROR:
	    icon_ਅੰਕੜੇ_2 = error_xpm;
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_WARNING:
	    icon_ਅੰਕੜੇ_2 = alert_xpm;
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_INFO:
	    icon_ਅੰਕੜੇ_2 = info_xpm;
	    ਰੋਕੋ;
	ਹਾਲਤ VIM_QUESTION:
	    icon_ਅੰਕੜੇ_2 = quest_xpm;
	    ਰੋਕੋ;
	ਮੂਲ:
	    icon_ਅੰਕੜੇ_2 = generic_xpm;
    }

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Ntop_ਵਿੱਥ, 8); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nbottom_ਵਿੱਥ, 8); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nleft_ਵਿੱਥ, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_ਅੰਕੜੇ_2, ਦਲੀਲਾਂ, n);
    Xt_Manage_ਬੱਚਾ(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((ਅੱਖਰ *)ਸੁਨੇਹਾ_2, STRING_ਟੈਗ);
    ਜੇ (label == ਨਿਰਾ)
    {
	vim_free(buttons);
	ਵਾਪਸ -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				Xm_Ntop_ਵਿੱਥ, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# ਨਹੀਂਤਾਂ
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_ਵਿੱਥ, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_ਵਿੱਥ, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_ਵਿੱਥ, 8,
				ਨਿਰਾ);
    XmStringFree(label);
    ਸੈੱਟ_fontlist(w);

    ਜੇ (textfield != ਨਿਰਾ)
    {
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (butcount > 0)
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ਨਿਰਾ);
    }

    ਜੇ (dfltbutton < 1)
	dfltbutton = 1;
    ਜੇ (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ਨਿਰਾ);
    ਜੇ (textfield != ਨਿਰਾ)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogform, XmNinitialFocus, dialogtextfield, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ਨਿਰਾ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    ਜੇ (textfield != ਨਿਰਾ && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_Selection(dialogtextfield,
			 (Xm_ਪਾਠ_Position)0, (Xm_ਪਾਠ_Position)STRLEN(textfield),
					   Xt_ਆਖਰੀ_Timestamp_Processed(gui.dpy));
	Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_Cursor_Position(dialogtextfield,
					   (Xm_ਪਾਠ_Position)STRLEN(textfield));
    }

    app = Xt_Widget_ਮੰਜ਼ਿਲ_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_ਸਥਿਤੀ = -1;
    ਲਈ (;;)
    {
	XtAppProcessEvent(app, (Xt_ਨਿਵੇਸ਼_ਨਕਾਬ)XtIMAll);
	ਜੇ (dialog_ਸਥਿਤੀ >= 0 || !XtIsManaged(dialogform))
	    ਰੋਕੋ;
    }

    vim_free(buttons);

    ਜੇ (textfield != ਨਿਰਾ)
    {
	p = (char_u *)Xm_ਪਾਠ_Get_String(dialogtextfield);
	ਜੇ (p == ਨਿਰਾ || dialog_ਸਥਿਤੀ < 0)
	    *textfield = NUL;
	ਨਹੀਂਤਾਂ
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((ਅੱਖਰ *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    ਵਾਪਸ dialog_ਸਥਿਤੀ;
}
#ਜੇ_ਅੰਤ // FEAT_GUI_DIALOG

#ਜੇ defined(FEAT_TOOLBAR)
    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    Cardinal	ਅੰਕ_Children;	    // how many children toolBar has

    ਜੇ (toolBar == (Widget)0)
	ਵਾਪਸ;
    Xt_Va_Get_ਮੁੱਲਾਂ(toolBar, XmNnumChildren, &ਅੰਕ_Children, ਨਿਰਾ);
    ਜੇ (showit && ਅੰਕ_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_ਮੁੱਲਾਂ(toolBar, XmNchildren, &children, ਨਿਰਾ);
	{
	    ਖਾਲੀ    (*action)(BalloonEval *);
	    ਪੂਰਨ	    ਪਾਠ = 0;

	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    ਨਹੀਂਤਾਂ
		action = &gui_mch_disable_beval_area;
	    ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *)p_toolbar, "text"))
		ਪਾਠ = 1;
	    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ((ਅਚਲ ਅੱਖਰ *)p_toolbar, "icons"))
		ਪਾਠ = -1;
	    ਜੇ (ਪਾਠ != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    ਜੇ (menu_is_toolbar(toolbar->dname))
			ਰੋਕੋ;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		ਜੇ (toolbar)
		{
		    ਲਈ (cur = toolbar->children; cur; cur = cur->ਅਗਲਾ)
		    {
			Arg	    ਦਲੀਲਾਂ[1];
			ਪੂਰਨ	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			ਜੇ (cur->tip != ਨਿਰਾ)
			    (*action)(cur->tip);
			ਜੇ (!menu_is_separator(cur->name))
			{
			    ਜੇ (ਪਾਠ == 1 || cur->xpm == ਨਿਰਾ)
			    {
				Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    ਜੇ (cur->id != ਨਿਰਾ)
			    {
				Xt_Unmanage_ਬੱਚਾ(cur->id);
				Xt_ਸੈੱਟ_ਮੁੱਲਾਂ(cur->id, ਦਲੀਲਾਂ, n);
				Xt_Manage_ਬੱਚਾ(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_ਉਚਾਈ = gui_mch_compute_toolbar_ਉਚਾਈ();
	Xt_Manage_ਬੱਚਾ(Xt_ਮਾਪਾ(toolBar));
# ifdef FEAT_GUI_TABLINE
	ਜੇ (showing_tabline)
	{
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			  ਨਿਰਾ);
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ਸਤਰ,
			  ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
# endif
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ਮਾਪਾ(toolBar),
			  ਨਿਰਾ);
	ਜੇ (XtIsManaged(menuBar))
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(Xt_ਮਾਪਾ(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(Xt_ਮਾਪਾ(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	gui.toolbar_ਉਚਾਈ = 0;
	ਜੇ (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
# ifdef FEAT_GUI_TABLINE
	    ਜੇ (showing_tabline)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_FORM,
			      ਨਿਰਾ);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ਸਤਰ,
			      ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ਨਿਰਾ);
	}

	Xt_Unmanage_ਬੱਚਾ(Xt_ਮਾਪਾ(toolBar));
    }
    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    ਸਥਿਰ ਖਾਲੀ
reset_focus(ਖਾਲੀ)
{
    ਜੇ (ਪਾਠ_Area != ਨਿਰਾ)
	XmProcessTraversal(ਪਾਠ_Area, XmTRAVERSE_CURRENT);
}

    ਪੂਰਨ
gui_mch_compute_toolbar_ਉਚਾਈ(ਖਾਲੀ)
{
    Dimension	borders;
    Dimension	ਉਚਾਈ;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	ਅੰਕ_Children;	    // how many children toolBar has
    ਪੂਰਨ		i;

    borders = 0;
    ਉਚਾਈ = 0;
    ਜੇ (toolBar != (Widget)0 && tool_Bar_ਚੌਖਟਾ != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_ਮੁੱਲਾਂ(tool_Bar_ਚੌਖਟਾ,
		XmNshadowThickness, &fst,
		Xm_Nmargin_ਉਚਾਈ, &fmh,
		ਨਿਰਾ);
	borders += fst + fmh;
	Xt_Va_Get_ਮੁੱਲਾਂ(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_ਉਚਾਈ, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &ਅੰਕ_Children, ਨਿਰਾ);
	borders += tst + tmh;
	ਲਈ (i = 0; i < (ਪੂਰਨ)ਅੰਕ_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_ਮੁੱਲਾਂ(children[i], XmNheight, &whgt, ਨਿਰਾ);
	    ਜੇ (ਉਚਾਈ < whgt)
		ਉਚਾਈ = whgt;
	}
    }
# ifdef LESSTIF_ਸੰਸਕਰਣ
    // Hack: When starting up we get wrong dimensions.
    ਜੇ (ਉਚਾਈ < 10)
	ਉਚਾਈ = 24;
# endif

    ਵਾਪਸ (ਪੂਰਨ)(ਉਚਾਈ + (borders << 1));
}

    ਖਾਲੀ
motif_get_toolbar_colors(
    Pixel       *bgp,
    Pixel       *fgp,
    Pixel       *bsp,
    Pixel       *tsp,
    Pixel       *hsp)
{
    Xt_Va_Get_ਮੁੱਲਾਂ(toolBar,
	    XmNbackground, bgp,
	    XmNforeground, fgp,
	    XmNbottomShadowColor, bsp,
	    XmNtopShadowColor, tsp,
	    XmNhighlightColor, hsp,
	    ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    ਖਾਲੀ
gui_mch_show_tabline(ਪੂਰਨ showit)
{
    ਜੇ (tab_ਸਤਰ == (Widget)0)
	ਵਾਪਸ;

    ਜੇ (!showit != !showing_tabline)
    {
	ਜੇ (showit)
	{
	    Xt_Manage_ਬੱਚਾ(tab_ਸਤਰ);
	    Xt_Unmanage_ਬੱਚਾ(Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ, "PageScroller"));
	    Xt_Unmanage_ਬੱਚਾ(Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ, "MinorTabScrollerNext"));
	    Xt_Unmanage_ਬੱਚਾ(Xt_Name_ਮੰਜ਼ਿਲ_Widget(tab_ਸਤਰ,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ਜੇ (XtIsManaged(Xt_ਮਾਪਾ(toolBar)))
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar), ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
#  endif
		ਜੇ (XtIsManaged(menuBar))
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ,
			      XmNtopAttachment, XmATTACH_FORM, ਨਿਰਾ);
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ਸਤਰ,
			  ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	{
	    Xt_Unmanage_ਬੱਚਾ(tab_ਸਤਰ);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ਜੇ (XtIsManaged(Xt_ਮਾਪਾ(toolBar)))
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ਮਾਪਾ(toolBar), ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
#  endif
		ਜੇ (XtIsManaged(menuBar))
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ਨਿਰਾ);
	    ਨਹੀਂਤਾਂ
# endif
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(ਪਾਠ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM, ਨਿਰਾ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ਪੂਰਨ
gui_mch_showing_tabline(ਖਾਲੀ)
{
    ਵਾਪਸ tab_ਸਤਰ != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ਖਾਲੀ
gui_mch_update_tabline(ਖਾਲੀ)
{
    tabpage_T		*tp;
    ਪੂਰਨ			nr = 1, n;
    Arg			ਦਲੀਲਾਂ[10];
    ਪੂਰਨ			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_ਪੰਨਾ_Info	ਪੰਨਾ_info;
    Xm_Notebook_ਪੰਨਾ_ਸਥਿਤੀ ਪੰਨਾ_ਸਥਿਤੀ;
    ਪੂਰਨ			ਆਖਰੀ_ਪੰਨਾ, tab_ਗਿਣਤੀ;
    XmString		label_str;
    ਅੱਖਰ		*label_cstr;
    BalloonEval		*beval;

    ਜੇ (tab_ਸਤਰ == (Widget)0)
	ਵਾਪਸ;

    // Add a label for each tab page.  They all contain the same text area.
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, ++nr)
    {
	ਜੇ (tp == curtab)
	    curtabidx = nr;

	ਪੰਨਾ_ਸਥਿਤੀ = Xm_Notebook_Get_ਪੰਨਾ_Info(tab_ਸਤਰ, nr, &ਪੰਨਾ_info);
	ਜੇ (ਪੰਨਾ_ਸਥਿਤੀ == Xm_ਪੰਨਾ_INVALID
		|| ਪੰਨਾ_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nnotebook_ਬੱਚਾ_Type, XmMAJOR_TAB); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtraversalOn, False); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNhighlightThickness, 1); n++;
	    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ਧੱਕੋ_Button(tab_ਸਤਰ, "-Empty-", ਦਲੀਲਾਂ, n);
	    Xt_Manage_ਬੱਚਾ(tab);
	    beval = gui_mch_create_beval_area(tab, ਨਿਰਾ, tabline_balloon_cb,
									ਨਿਰਾ);
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab, Xm_Nuser_ਅੰਕੜੇ, beval, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ
	    tab = ਪੰਨਾ_info.major_tab_widget;

	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab, Xm_Npage_ਸੰਖਿਆ, nr, ਨਿਰਾ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_ਮੁੱਲਾਂ(tab, XmNlabelString, &label_str, ਨਿਰਾ);
	ਜੇ (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    ਜੇ (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ਨਿਰਾ);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_ਬੱਚਾ(tab);
		Xt_Manage_ਬੱਚਾ(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_ਗਿਣਤੀ = nr - 1;

    Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Nlast_ਪੰਨਾ_ਸੰਖਿਆ, &ਆਖਰੀ_ਪੰਨਾ, ਨਿਰਾ);

    // Remove any old labels.
    ਜਦੋਂ (nr <= ਆਖਰੀ_ਪੰਨਾ)
    {
	ਜੇ (Xm_Notebook_Get_ਪੰਨਾ_Info(tab_ਸਤਰ, nr, &ਪੰਨਾ_info) != Xm_ਪੰਨਾ_INVALID
	    && ਪੰਨਾ_info.ਪੰਨਾ_ਸੰਖਿਆ == nr
	    && ਪੰਨਾ_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_ਮੁੱਲਾਂ(ਪੰਨਾ_info.major_tab_widget, Xm_Nuser_ਅੰਕੜੇ, &beval, ਨਿਰਾ);
	    ਜੇ (beval != ਨਿਰਾ)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_ਬੱਚਾ(ਪੰਨਾ_info.major_tab_widget);
	    XtDestroyWidget(ਪੰਨਾ_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Nlast_ਪੰਨਾ_ਸੰਖਿਆ, tab_ਗਿਣਤੀ, ਨਿਰਾ);

    Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Ncurrent_ਪੰਨਾ_ਸੰਖਿਆ, &currentpage, ਨਿਰਾ);
    ਜੇ (currentpage != curtabidx)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Ncurrent_ਪੰਨਾ_ਸੰਖਿਆ, curtabidx, ਨਿਰਾ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_curtab(ਪੂਰਨ nr)
{
    ਪੂਰਨ		currentpage;

    ਜੇ (tab_ਸਤਰ == (Widget)0)
	ਵਾਪਸ;

    Xt_Va_Get_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Ncurrent_ਪੰਨਾ_ਸੰਖਿਆ, &currentpage, ਨਿਰਾ);
    ਜੇ (currentpage != nr)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(tab_ਸਤਰ, Xm_Ncurrent_ਪੰਨਾ_ਸੰਖਿਆ, nr, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_motif_menu_colors(Widget id)
{
    ਜੇ (gui.menu_bg_pixel != INVALCOLOR)
#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#ਨਹੀਂਤਾਂ
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNbackground, gui.menu_bg_pixel, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    ਜੇ (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNforeground, gui.menu_fg_pixel, ਨਿਰਾ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_motif_scroll_colors(Widget id)
{
    ਜੇ (gui.scroll_bg_pixel != INVALCOLOR)
#ਜੇ (Xm_ਸੰਸਕਰਣ >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#ਨਹੀਂਤਾਂ
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNbackground, gui.scroll_bg_pixel, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    ਜੇ (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNforeground, gui.scroll_fg_pixel, ਨਿਰਾ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    ਖਾਲੀ
gui_motif_menu_fontlist(Widget id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
# ifdef FONTSET_ALWAYS
    ਜੇ (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_ਸੈੱਟ *)&gui.menu_fontset);
	ਜੇ (fl != ਨਿਰਾ)
	{
	    ਜੇ (XtIsManaged(id))
	    {
		Xt_Unmanage_ਬੱਚਾ(id);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ਬੱਚਾ(id);
	    }
	    ਨਹੀਂਤਾਂ
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
	    XmFontListFree(fl);
	}
    }
# ਨਹੀਂਤਾਂ
    ਜੇ (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	ਜੇ (fl != ਨਿਰਾ)
	{
	    ਜੇ (XtIsManaged(id))
	    {
		Xt_Unmanage_ਬੱਚਾ(id);
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ਬੱਚਾ(id);
	    }
	    ਨਹੀਂਤਾਂ
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(id, XmNfontList, fl, ਨਿਰਾ);
	    XmFontListFree(fl);
	}
    }
# endif
#ਜੇ_ਅੰਤ
}


/*
 * We don't create it twice for the sake of speed.
 */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Shared_ਲੱਭੋ_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget ਉੱਪਰ;		// search direction 'Up' radio button
    Widget ਥੱਲੇ;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget ਲੱਭੋ;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_ਲੱਭੋ_Replace_2;

ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 ਲੱਭੋ_widgets = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};
ਸਥਿਰ Shared_ਲੱਭੋ_Replace_2 repl_widgets = {ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ};

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_destroy_callback(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    Shared_ਲੱਭੋ_Replace_2 *cd = (Shared_ਲੱਭੋ_Replace_2 *)client_ਅੰਕੜੇ_2;

    ਜੇ (cd != ਨਿਰਾ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_dismiss_callback(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    Shared_ਲੱਭੋ_Replace_2 *cd = (Shared_ਲੱਭੋ_Replace_2 *)client_ਅੰਕੜੇ_2;

    ਜੇ (cd != ਨਿਰਾ)
	Xt_Unmanage_ਬੱਚਾ(cd->dialog);
}

    ਸਥਿਰ ਖਾਲੀ
ਇੰਦਰਾਜ_activate_callback(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    XmProcessTraversal((Widget)client_ਅੰਕੜੇ_2, XmTRAVERSE_CURRENT);
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_callback(
    Widget	w ਅਣਵਰਤਿਆ,
    Xt_ਸੂਚਕ	client_ਅੰਕੜੇ_2,
    Xt_ਸੂਚਕ	call_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    long_u	flags = (long_u)client_ਅੰਕੜੇ_2;
    ਅੱਖਰ	*ਲੱਭੋ_ਪਾਠ, *repl_ਪਾਠ;
    Boolean	direction_ਥੱਲੇ = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_ਲੱਭੋ_Replace_2 *sfr;

    ਜੇ (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = ਖਾਲੀ_ਥਾਵਾਂ_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	ਵਾਪਸ;
    }

    // Get the search/replace strings from the dialog
    ਜੇ (flags == FRD_FINDNEXT)
    {
	repl_ਪਾਠ = ਨਿਰਾ;
	sfr = &ਲੱਭੋ_widgets;
    }
    ਨਹੀਂਤਾਂ
    {
	repl_ਪਾਠ = Xm_ਪਾਠ_ਖੇਤਰ_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    ਲੱਭੋ_ਪਾਠ = Xm_ਪਾਠ_ਖੇਤਰ_Get_String(sfr->what);
    Xt_Va_Get_ਮੁੱਲਾਂ(sfr->ਥੱਲੇ, XmNset, &direction_ਥੱਲੇ, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(sfr->wword, XmNset, &wword, ਨਿਰਾ);
    Xt_Va_Get_ਮੁੱਲਾਂ(sfr->mcase, XmNset, &mcase, ਨਿਰਾ);
    ਜੇ (wword)
	flags |= FRD_ਸਾਰਾ_ਸ਼ਬਦ;
    ਜੇ (mcase)
	flags |= FRD_ਮੇਲ_CASE;

    (ਖਾਲੀ)gui_do_findrepl((ਪੂਰਨ)flags, (char_u *)ਲੱਭੋ_ਪਾਠ, (char_u *)repl_ਪਾਠ,
							      direction_ਥੱਲੇ);

    ਜੇ (ਲੱਭੋ_ਪਾਠ != ਨਿਰਾ)
	XtFree(ਲੱਭੋ_ਪਾਠ);
    ਜੇ (repl_ਪਾਠ != ਨਿਰਾ)
	XtFree(repl_ਪਾਠ);
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_keypress(
    Widget		w ਅਣਵਰਤਿਆ,
    Shared_ਲੱਭੋ_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b ਅਣਵਰਤਿਆ)
{
    ਕੁੰਜੀ_Sym keysym;

    ਜੇ (frdp == ਨਿਰਾ)
	ਵਾਪਸ;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    ਜੇ (keysym == XK_Escape)
	Xt_Unmanage_ਬੱਚਾ(frdp->dialog);
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_label(Widget w, ਅੱਖਰ *label)
{
    XmString	str;
    char_u	*p, *ਅਗਲਾ;
    ਕੁੰਜੀ_Sym	mnemonic = NUL;

    ਜੇ (!w)
	ਵਾਪਸ;

    p = vim_strsave((char_u *)label);
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;
    ਲਈ (ਅਗਲਾ = p; *ਅਗਲਾ; ++ਅਗਲਾ)
    {
	ਜੇ (*ਅਗਲਾ == DLG_HOTKEY_CHAR)
	{
	    ਪੂਰਨ len = STRLEN(ਅਗਲਾ);

	    ਜੇ (len > 0)
	    {
		mch_memmove(ਅਗਲਾ, ਅਗਲਾ + 1, len);
		mnemonic = ਅਗਲਾ[0];
	    }
	}
    }

    str = XmStringCreateSimple((ਅੱਖਰ *)p);
    vim_free(p);
    ਜੇ (str)
    {
	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ਨਿਰਾ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ਸਥਿਰ ਖਾਲੀ
ਲੱਭੋ_replace_dialog_create(char_u *arg, ਪੂਰਨ do_replace)
{
    Shared_ਲੱਭੋ_Replace_2	*frdp;
    Widget		separator;
    Widget		ਨਿਵੇਸ਼_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		ਚੌਖਟਾ;
    XmString		str;
    ਪੂਰਨ			n;
    Arg			ਦਲੀਲਾਂ[6];
    ਪੂਰਨ			wword = FALSE;
    ਪੂਰਨ			mcase = !p_ic;
    Dimension		ਚੌੜਾਈ;
    Dimension		widest;
    char_u		*ਇੰਦਰਾਜ_ਪਾਠ;

    frdp = do_replace ? &repl_widgets : &ਲੱਭੋ_widgets;

    // Get the search string to use.
    ਇੰਦਰਾਜ_ਪਾਠ = get_ਲੱਭੋ_dialog_ਪਾਠ(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    ਜੇ (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	ਜੇ (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_ਖਿੜਕੀ(Xt_ਮਾਪਾ(frdp->dialog)));
	ਨਹੀਂਤਾਂ
	    Xt_Manage_ਬੱਚਾ(frdp->dialog);
	XtPopup(Xt_ਮਾਪਾ(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	    Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_String(frdp->what, (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
	vim_free(ਇੰਦਰਾਜ_ਪਾਠ);

	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(frdp->wword, XmNset, wword, ਨਿਰਾ);
	ਵਾਪਸ;
    }

    // Create a fresh new dialog window
    ਜੇ (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    ਨਹੀਂਤਾਂ
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNautoUnmanage, False); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNnoResize, True); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", ਦਲੀਲਾਂ, n);
    XmStringFree(str);
    Xt_ਜੋੜੋ_Callback(frdp->dialog, XmNdestroyCallback,
	    ਲੱਭੋ_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    ਨਿਰਾ);

    frdp->ਲੱਭੋ = XtVaCreateManagedWidget("findButton",
	    xm_ਧੱਕੋ_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ਨਿਰਾ);
    ਸੈੱਟ_label(frdp->ਲੱਭੋ, _("Find &Next"));

    Xt_ਜੋੜੋ_Callback(frdp->ਲੱਭੋ, XmNactivateCallback,
	    ਲੱਭੋ_replace_callback,
	    (do_replace ? (Xt_ਸੂਚਕ)FRD_R_FINDNEXT : (Xt_ਸੂਚਕ)FRD_FINDNEXT));

    ਜੇ (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ਧੱਕੋ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->ਲੱਭੋ,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ਨਿਰਾ);
	ਸੈੱਟ_label(frdp->replace, _("&Replace"));
	Xt_ਜੋੜੋ_Callback(frdp->replace, XmNactivateCallback,
		ਲੱਭੋ_replace_callback, (Xt_ਸੂਚਕ)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ਧੱਕੋ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ਨਿਰਾ);
	ਸੈੱਟ_label(frdp->all, _("Replace &All"));
	Xt_ਜੋੜੋ_Callback(frdp->all, XmNactivateCallback,
		ਲੱਭੋ_replace_callback, (Xt_ਸੂਚਕ)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ਧੱਕੋ_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ਨਿਰਾ);
	ਸੈੱਟ_label(frdp->undo, _("&Undo"));
	Xt_ਜੋੜੋ_Callback(frdp->undo, XmNactivateCallback,
		ਲੱਭੋ_replace_callback, (Xt_ਸੂਚਕ)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ਧੱਕੋ_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ਨਿਰਾ);
    ਸੈੱਟ_label(frdp->cancel, _("&Cancel"));
    Xt_ਜੋੜੋ_Callback(frdp->cancel, XmNactivateCallback,
	    ਲੱਭੋ_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_ਬੱਚਾ(button_form);

    n = 0;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNorientation, XmVERTICAL); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNrightWidget, button_form); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], Xm_Nright_ਵਿੱਥ, 4); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", ਦਲੀਲਾਂ, n);
    Xt_Manage_ਬੱਚਾ(separator);

    ਨਿਵੇਸ਼_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    ਨਿਰਾ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ਨਿਵੇਸ਼_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_ਵਿੱਥ, 4,
		ਨਿਰਾ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_ਪਾਠ_ਖੇਤਰ_Widget_Class, ਨਿਵੇਸ਼_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ਨਿਰਾ);

	ਜੇ (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_ਪਾਠ_ਖੇਤਰ_Widget_Class,	ਨਿਵੇਸ਼_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_ਵਿੱਥ, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ਨਿਰਾ);

	    Xt_ਜੋੜੋ_Callback(frdp->with, XmNactivateCallback,
		    ਲੱਭੋ_replace_callback, (Xt_ਸੂਚਕ) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ਨਿਵੇਸ਼_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_ਵਿੱਥ, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ਨਿਰਾ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_ਜੋੜੋ_Callback(frdp->what, XmNactivateCallback,
		    ਇੰਦਰਾਜ_activate_callback, frdp->with);
	    Xt_ਜੋੜੋ_Event_Handler(frdp->with, ਕੁੰਜੀ_Press_ਨਕਾਬ, False,
			    (XtEventHandler)ਲੱਭੋ_replace_keypress,
			    (Xt_ਸੂਚਕ) frdp);

	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_ਜੋੜੋ_Callback(frdp->what, XmNactivateCallback,
		    ਲੱਭੋ_replace_callback, (Xt_ਸੂਚਕ)FRD_FINDNEXT);
	}
	Xt_ਜੋੜੋ_Event_Handler(frdp->what, ਕੁੰਜੀ_Press_ਨਕਾਬ, False,
			    (XtEventHandler)ਲੱਭੋ_replace_keypress,
			    (Xt_ਸੂਚਕ)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_ਸੈੱਟ_Arg(ਦਲੀਲਾਂ[n], XmNwidth, &ਚੌੜਾਈ); n++;
	Xt_Get_ਮੁੱਲਾਂ(label_what, ਦਲੀਲਾਂ, n);
	widest = ਚੌੜਾਈ;
	ਜੇ (do_replace)
	{
	    Xt_Get_ਮੁੱਲਾਂ(label_with, ਦਲੀਲਾਂ, n);
	    ਜੇ (ਚੌੜਾਈ > widest)
		widest = ਚੌੜਾਈ;
	}

	Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(frdp->what, Xm_Nleft_ਵਿੱਥ, widest, ਨਿਰਾ);
	ਜੇ (do_replace)
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(frdp->with, Xm_Nleft_ਵਿੱਥ, widest, ਨਿਰਾ);

    }

    Xt_Manage_ਬੱਚਾ(ਨਿਵੇਸ਼_form);

    {
	Widget radio_box;
	Widget w;

	ਚੌਖਟਾ = XtVaCreateWidget("directionFrame",
		xm_ਚੌਖਟਾ_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ਨਿਵੇਸ਼_form,
		Xm_Ntop_ਵਿੱਥ, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_ਵਿੱਥ, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ਨਿਵੇਸ਼_form,
		ਨਿਰਾ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, ਚੌਖਟਾ,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_ਚੌਖਟਾ_TITLE_ਬੱਚਾ,
		ਨਿਰਾ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(ਚੌਖਟਾ, "radioBox",
		(ArgList)ਨਿਰਾ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->ਉੱਪਰ = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ਨਿਰਾ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->ਉੱਪਰ);

	str = XmStringCreateSimple(_("Down"));
	frdp->ਥੱਲੇ = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ਨਿਰਾ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->ਥੱਲੇ);

	Xt_Manage_ਬੱਚਾ(radio_box);
	Xt_Manage_ਬੱਚਾ(ਚੌਖਟਾ);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, ਚੌਖਟਾ,
	    Xm_Nright_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ਨਿਵੇਸ਼_form,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ਵਿੱਥ, 4,
	    ਨਿਰਾ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNset, wword,
	    ਨਿਰਾ);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ਵਿੱਥ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_ਵਿੱਥ, 4,
	    XmNset, mcase,
	    ਨਿਰਾ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_ਬੱਚਾ(toggle_form);

    ਜੇ (ਇੰਦਰਾਜ_ਪਾਠ != ਨਿਰਾ)
	Xm_ਪਾਠ_ਖੇਤਰ_ਸੈੱਟ_String(frdp->what, (ਅੱਖਰ *)ਇੰਦਰਾਜ_ਪਾਠ);
    vim_free(ਇੰਦਰਾਜ_ਪਾਠ);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   ਖਾਲੀ
gui_mch_ਲੱਭੋ_dialog(exarg_T *eap)
{
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    ਲੱਭੋ_replace_dialog_create(eap->arg, FALSE);
}


    ਖਾਲੀ
gui_mch_replace_dialog(exarg_T *eap)
{
    ਜੇ (!gui.ਵਿੱਚ_use)
	ਵਾਪਸ;

    ਲੱਭੋ_replace_dialog_create(eap->arg, TRUE);
}

/*
 * Synchronize all gui elements, which are dependent upon the
 * main text font used. Those are in esp. the find/replace dialogs.
 * If you don't understand why this should be needed, please try to
 * search for "pi\xea\xb6\xe6" in iso8859-2.
 */
    ਖਾਲੀ
gui_motif_synch_fonts(ਖਾਲੀ)
{
    Shared_ਲੱਭੋ_Replace_2 *frdp;
    ਪੂਰਨ		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    ਜੇ (font == ਨਿਰਾ)
	ਵਾਪਸ;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    ਲਈ (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&ਲੱਭੋ_widgets);
	ਜੇ (frdp->dialog)
	{
	    Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(frdp->what, XmNfontList, font_list, ਨਿਰਾ);
	    ਜੇ (do_replace)
		Xt_Va_ਸੈੱਟ_ਮੁੱਲਾਂ(frdp->with, XmNfontList, font_list, ਨਿਰਾ);
	}
    }

    XmFontListFree(font_list);
}
