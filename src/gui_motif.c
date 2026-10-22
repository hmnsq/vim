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
#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
# include <Xm/RepType.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <Xm/Frame.h>
#ಅನ್ತರ್ಭಾವ <Xm/LabelG.h>
#ಅನ್ತರ್ಭಾವ <Xm/ToggleBG.h>
#ಅನ್ತರ್ಭಾವ <Xm/SeparatoG.h>
#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>

#ಅನ್ತರ್ಭಾವ <X11/keysym.h>
#ಅನ್ತರ್ಭಾವ <X11/Xatom.h>
#ಅನ್ತರ್ಭಾವ <X11/StringDefs.h>
#ಅನ್ತರ್ಭಾವ <X11/Intrinsic.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_X11_XPM_H
# ಆದರೆ defined(VMS)
#  include <xpm.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <X11/xpm.h>
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_XM_XPMP_H
#  include <Xm/XpmP.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_XM_NOTEBOOK_H
# include <Xm/Notebook.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "gui_xmebw.h"	// for our Enhanced Button Widget

#ಆದರೆ defined(FEAT_GUI_DIALOG) && defined(HAVE_XPM)
# include "../pixmaps/alert.xpm"
# include "../pixmaps/error.xpm"
# include "../pixmaps/generic.xpm"
# include "../pixmaps/info.xpm"
# include "../pixmaps/quest.xpm"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು MOTIF_POPUP

ಬಾಹ್ಯ Widget vimShell;

ಸ್ಥಿರ Widget vimForm;
ಸ್ಥಿರ Widget ಪಠ್ಯ_Area_Form;
Widget ಪಠ್ಯ_Area;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
ಸ್ಥಿರ Widget tool_Bar_ಚೌಕಟ್ಟು;
ಸ್ಥಿರ Widget toolBar;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
ಸ್ಥಿರ Widget	tab_ಸಾಲು;
ಸ್ಥಿರ Widget	tab_ಸಾಲು_menu = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	showing_tabline = 0;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
// remember the last set value for the tearoff item
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ tearoff_val = (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED;
# endif
ಸ್ಥಿರ Widget menuBar;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
ಸ್ಥಿರ ಶೂನ್ಯ reset_focus(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_menu_colors(Widget id);
ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_scroll_colors(Widget id);

#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
# define STRING_ಟ್ಯಾಗ್  Xm_FONTLIST_DEFAULT_ಟ್ಯಾಗ್
#ಇಲ್ಲದಿದ್ದರೆ
# define STRING_ಟ್ಯಾಗ್  XmSTRING_DEFAULT_CHARSET
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call-back routines.
 */

    ಸ್ಥಿರ ಶೂನ್ಯ
scroll_cb(Widget w ಬಳಸದ, Xt_ಸೂಚಕ client_ದತ್ತಾಂಶ_2, Xt_ಸೂಚಕ call_ದತ್ತಾಂಶ)
{
    scrollbar_T *sb;
    ದೀರ್ಘ	ಮೌಲ್ಯ;
    ಪೂರ್ಣಾಂಕ		dragging;

    sb = gui_ಕಂಡುಹಿಡಿ_scrollbar((ದೀರ್ಘ)client_ದತ್ತಾಂಶ_2);

    ಮೌಲ್ಯ = ((XmScrollBarCallbackStruct *)call_ದತ್ತಾಂಶ)->ಮೌಲ್ಯ;
    dragging = (((XmScrollBarCallbackStruct *)call_ದತ್ತಾಂಶ)->reason ==
							      (ಪೂರ್ಣಾಂಕ)XmCR_DRAG);
    gui_drag_scrollbar(sb, ಮೌಲ್ಯ, dragging);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_cb(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ)
{
    XmNotebookCallbackStruct *nptr;

    nptr = (XmNotebookCallbackStruct *)call_ದತ್ತಾಂಶ;
    ಆದರೆ (nptr->reason != (ಪೂರ್ಣಾಂಕ)XmCR_NONE)
	send_tabline_event(nptr->ಪುಟ_ಸಂಖ್ಯೆ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_button_cb(
    Widget	w,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    Xt_ಸೂಚಕ	cmd, tab_idx;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, Xm_Nuser_ದತ್ತಾಂಶ, &cmd, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, Xm_Nuser_ದತ್ತಾಂಶ, &tab_idx, ಖಾಲಿ);

    send_tabline_menu_event((ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)tab_idx, (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)cmd);
}

/*
 * Tabline single mouse click timeout handler
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
motif_tabline_timer_cb (
    Xt_ಸೂಚಕ		timed_ಹೊರಹೋಗು,
    XtIntervalId	*interval_id ಬಳಸದ)
{
    *((ಪೂರ್ಣಾಂಕ *)timed_ಹೊರಹೋಗು) = TRUE;
}

/*
 * check if the tabline tab scroller is clicked
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tabline_scroller_clicked(
    ಅಕ್ಷರ		*scroller_name,
    XButtonPressedEvent *event)
{
    Widget	tab_scroll_w;
    Position	pos_x, pos_y;
    Dimension	ಅಗಲ, ಎತ್ತರ;

    tab_scroll_w = Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು, scroller_name);
    ಆದರೆ (tab_scroll_w != (Widget)0)
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_scroll_w, XmNx, &pos_x, XmNy, &pos_y, XmNwidth,
		      &ಅಗಲ, XmNheight, &ಎತ್ತರ, ಖಾಲಿ);
	ಆದರೆ (pos_x >= 0)
	{
	    // Tab scroller (next) is visible
	    ಆದರೆ (event->x >= pos_x && event->x <= pos_x + ಅಗಲ
		    && event->y >= pos_y && event->y <= pos_y + ಎತ್ತರ)
		// Clicked on the scroller
		ಹಿಂತಿರುಗಿ TRUE;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_cb(
    Widget	w,
    Xt_ಸೂಚಕ	ಆವರಣ ಬಳಸದ,
    XEvent	*e,
    Boolean	*continue_dispatch ಬಳಸದ)
{
    Widget			tab_w;
    XButtonPressedEvent		*event;
    ಪೂರ್ಣಾಂಕ				tab_idx = 0;
    WidgetList			children;
    Cardinal			ಅಂಕೆ_Children;
    ಸ್ಥಿರ XtIntervalId		timer = (XtIntervalId)0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ			timed_ಹೊರಹೋಗು = TRUE;

    event = (XButtonPressedEvent *)e;

    ಆದರೆ (event->button == Button1)
    {
	ಆದರೆ (tabline_scroller_clicked("MajorTabScrollerNext", event)
	    || tabline_scroller_clicked("MajorTabScrollerPrevious", event))
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (!timed_ಹೊರಹೋಗು)
	{
	    Xt_Remove_ಸಮಯ_ಹೊರಹೋಗು(timer);
	    timed_ಹೊರಹೋಗು = TRUE;

	    /*
	     * Double click on the tabline gutter, add a new tab
	     */
	    send_tabline_menu_event(0, TABLINE_MENU_NEW);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Single click on the tabline gutter, start a timer to check
	     * for double clicks
	     */
	    timer = Xt_App_ಕೂಡಿಸು_ಸಮಯ_ಹೊರಹೋಗು(app_context, (long_u)p_mouset,
				    motif_tabline_timer_cb, &timed_ಹೊರಹೋಗು);
	    timed_ಹೊರಹೋಗು = FALSE;
	}
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (event->button == Button2)
    {
	// Middle mouse click on tabpage label closes that tab.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, Xm_Nuser_ದತ್ತಾಂಶ, &tab_idx, ಖಾಲಿ);
	send_tabline_menu_event(tab_idx, (ಪೂರ್ಣಾಂಕ)TABLINE_MENU_CLOSE);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (event->button != Button3)
	ಹಿಂತಿರುಗಿ;

    // When ignoring events don't show the menu.
    ಆದರೆ (hold_gui_events || cmdwin_type != 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (event->subwindow != None)
    {
	tab_w = Xt_ಕಿಟಕಿ_ಗಮ್ಯ_Widget(XtDisplay(w), event->subwindow);
	// LINTED: avoid warning: dubious operation on enum
	ಆದರೆ (tab_w != (Widget)0 && Xm_Is_ತಳ್ಳು_Button(tab_w))
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_w, Xm_Npage_ಸಂಖ್ಯೆ, &tab_idx, ಖಾಲಿ);
    }

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, Xm_Nuser_ದತ್ತಾಂಶ, (Xt_ಸೂಚಕ)(ದೀರ್ಘ)tab_idx, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು_menu, XmNchildren, &children, XmNnumChildren,
		  &ಅಂಕೆ_Children, ಖಾಲಿ);
    XtManageChildren(children, ಅಂಕೆ_Children);
    XmMenuPosition(tab_ಸಾಲು_menu, (XButtonPressedEvent *)e) ;
    Xt_Manage_ಮಗು(tab_ಸಾಲು_menu);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_balloon_cb(BalloonEval *beval, ಪೂರ್ಣಾಂಕ state ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		nr;
    tabpage_T	*tp;

    ಆದರೆ (beval->ಗುರಿ == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(beval->ಗುರಿ, Xm_Npage_ಸಂಖ್ಯೆ, &nr, ಖಾಲಿ);
    tp = ಕಂಡುಹಿಡಿ_tabpage(nr);
    ಆದರೆ (tp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    get_tabline_label(tp, TRUE);
    gui_mch_post_balloon(beval, NameBuff);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * End of call-back routines
 */

/*
 * Implement three dimensional shading of insensitive labels.
 * By Marcin Dalecki.
 */

#ಅನ್ತರ್ಭಾವ <Xm/XmP.h>
#ಅನ್ತರ್ಭಾವ <Xm/LabelP.h>

ಸ್ಥಿರ XtExposeProc old_label_expose = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
label_expose(Widget _w, XEvent *_event, Region _region)
{
    GC		    insensitiveGC;
    XmLabelWidget   lw = (XmLabelWidget)_w;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   label_type = (ಪೂರ್ಣಾಂಕ)XmSTRING;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(_w, XmNlabelType, &label_type, (Xt_ಸೂಚಕ)0);

    ಆದರೆ (XtIsSensitive(_w) || label_type != (ಪೂರ್ಣಾಂಕ)XmSTRING)
	(*old_label_expose)(_w, _event, _region);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	XGCValues   ಮೌಲ್ಯಗಳು;
	XtGCMask    ಮುಖವಾಡ;
	XtGCMask    dynamic;
	XFontStruct *fs;

	_XmFontListGetDefaultFont(lw->label.font, &fs);

	// FIXME: we should be doing the whole drawing ourself here.
	insensitiveGC = lw->label.insensitive_GC;

	ಮುಖವಾಡ = GCForeground | GCBackground | GCGraphicsExposures;
	dynamic = GCClip_ಮುಖವಾಡ | GCClipXOrigin | GCClipYOrigin;
	ಮೌಲ್ಯಗಳು.graphics_exposures = False;

	ಆದರೆ (fs != 0)
	{
	    ಮುಖವಾಡ |= GCFont;
	    ಮೌಲ್ಯಗಳು.font = fs->fid;
	}

	ಆದರೆ (lw->primitive.ತುದಿ_shadow_pixmap != None
		&& lw->primitive.ತುದಿ_shadow_pixmap != XmUNSPECIFIED_PIXMAP)
	{
	    ಮುಖವಾಡ |= GCFillStyle | GCTile;
	    ಮೌಲ್ಯಗಳು.fill_style = FillTiled;
	    ಮೌಲ್ಯಗಳು.tile = lw->primitive.ತುದಿ_shadow_pixmap;
	}

	lw->label.ಪಠ್ಯ_Rect.x += 1;
	lw->label.ಪಠ್ಯ_Rect.y += 1;
	ಆದರೆ (lw->label.acc_ಪಠ್ಯ != 0)
	{
	    lw->label.acc_ಪಠ್ಯ_Rect.x += 1;
	    lw->label.acc_ಪಠ್ಯ_Rect.y += 1;
	}

	ಮೌಲ್ಯಗಳು.foreground = lw->primitive.ತುದಿ_shadow_color;
	ಮೌಲ್ಯಗಳು.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget)lw, 0, ಮುಖವಾಡ,
					       &ಮೌಲ್ಯಗಳು, dynamic, (XtGCMask)0);
	(*old_label_expose)(_w, _event, _region);
	XtReleaseGC(_w, lw->label.insensitive_GC);

	lw->label.ಪಠ್ಯ_Rect.x -= 1;
	lw->label.ಪಠ್ಯ_Rect.y -= 1;
	ಆದರೆ (lw->label.acc_ಪಠ್ಯ != 0)
	{
	    lw->label.acc_ಪಠ್ಯ_Rect.x -= 1;
	    lw->label.acc_ಪಠ್ಯ_Rect.y -= 1;
	}

	ಮೌಲ್ಯಗಳು.foreground = lw->primitive.bottom_shadow_color;
	ಮೌಲ್ಯಗಳು.background = lw->core.background_pixel;

	lw->label.insensitive_GC = XtAllocateGC((Widget) lw, 0, ಮುಖವಾಡ,
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    Widget	button, scroller;
    Arg		ವಾದಗಳು[10];
    ಪೂರ್ಣಾಂಕ		n;
    XmString	xms;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Install the 3D shade effect drawing routines.
     */
    ಆದರೆ (old_label_expose == ಖಾಲಿ)
    {
	old_label_expose = xmLabelWidgetClass->core_class.expose;
	xmLabelWidgetClass->core_class.expose = label_expose;
    }

    /*
     * Start out by adding the configured border width into the border offset
     */
    gui.border_ಸ್ಥಾನಾಂತರ = gui.border_ಅಗಲ;

    /*
     * Install the tearOffModel resource converter.
     */
#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
    XmRepTypeInstallTearOffModelConverter();
#ಆದರೆ_ಅಂತ್ಯ

    // Make sure the "Quit" menu entry of the window manager is ignored
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(vimShell, XmNdeleteResponse, XmDO_NOTHING, ಖಾಲಿ);

    vimForm = XtVaCreateManagedWidget("vimForm",
	xmFormWidgetClass, vimShell,
	Xm_Nborder_ಅಗಲ, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	Xm_Nmargin_ಅಗಲ, 0,
	Xm_Nmargin_ಎತ್ತರ, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ಖಾಲಿ);
    gui_motif_menu_colors(vimForm);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    {
	Arg al[7]; // Make sure there is enough room for arguments!
	ಪೂರ್ಣಾಂಕ ac = 0;

# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	Xt_ಗಣ_Arg(al[ac], XmNtearOffModel, tearoff_val); ac++;
# endif
	Xt_ಗಣ_Arg(al[ac], XmNleftAttachment,  XmATTACH_FORM); ac++;
	Xt_ಗಣ_Arg(al[ac], XmNtopAttachment,   XmATTACH_FORM); ac++;
	Xt_ಗಣ_Arg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
# ifndef FEAT_TOOLBAR
	// Always stick to right hand side.
	Xt_ಗಣ_Arg(al[ac], Xm_Nright_ಸ್ಥಾನಾಂತರ, 0); ac++;
# endif
	Xt_ಗಣ_Arg(al[ac], Xm_Nmargin_ಎತ್ತರ, 0); ac++;
	menuBar = XmCreateMenuBar(vimForm, "menuBar", al, ac);
	Xt_Manage_ಮಗು(menuBar);

	gui_motif_menu_colors(menuBar);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    /*
     * Create an empty ToolBar. We should get buttons defined from menu.vim.
     */
    tool_Bar_ಚೌಕಟ್ಟು = XtVaCreateWidget("toolBarFrame",
	xm_ಚೌಕಟ್ಟು_Widget_Class, vimForm,
	XmNshadowThickness, 0,
	Xm_Nmargin_ಎತ್ತರ, 0,
	Xm_Nmargin_ಅಗಲ, 0,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	ಖಾಲಿ);
    gui_motif_menu_colors(tool_Bar_ಚೌಕಟ್ಟು);

    toolBar = XtVaCreateManagedWidget("toolBar",
	xm_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ_Widget_Class, tool_Bar_ಚೌಕಟ್ಟು,
	XmNchildType, Xm_ಚೌಕಟ್ಟು_WORKAREA_ಮಗು,
	Xm_Nrow_ಸ್ತಂಭ_Type, XmWORK_AREA,
	XmNorientation, XmHORIZONTAL,
	XmNtraversalOn, False,
	XmNisHomogeneous, False,
	XmNpacking, XmPACK_TIGHT,
	XmNspacing, 0,
	XmNshadowThickness, 0,
	XmNhighlightThickness, 0,
	Xm_Nmargin_ಎತ್ತರ, 0,
	Xm_Nmargin_ಅಗಲ, 0,
	Xm_Nadjust_ಕೊನೆಯ, True,
	ಖಾಲಿ);
    gui_motif_menu_colors(toolBar);

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    // Create the Vim GUI tabline
    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbindingType, XmNONE); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation, XmVERTICAL); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nback_ಪುಟ_ಗಾತ್ರ, XmNONE); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nback_ಪುಟ_ಸಂಖ್ಯೆ, 0); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nback_ಪುಟ_Placement, Xm_ತುದಿ_ಬಲ); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNmajorTabSpacing, 0); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNshadowThickness, 0); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_FORM); n++;
    tab_ಸಾಲು = XmCreateNotebook(vimForm, "Vim tabline", ವಾದಗಳು, n);

    Xt_ಕೂಡಿಸು_Callback(tab_ಸಾಲು, Xm_Npage_ಬದಲಾಯಿತು_Callback, (XtCallbackProc)tabline_cb,
			ಖಾಲಿ);
    Xt_ಕೂಡಿಸು_Event_Handler(tab_ಸಾಲು, Button_Press_ಮುಖವಾಡ, False,
			(XtEventHandler)tabline_menu_cb, ಖಾಲಿ);

    gui.tabline_ಎತ್ತರ = TABLINE_ಎತ್ತರ;

    /*
     * Set the size of the minor next/prev scrollers to zero, so
     * that they are not displayed. Due to a bug in OpenMotif 2.3,
     * even if these children widget are unmanaged, they are again
     * managed by the Notebook widget and the notebook widget geometry
     * is adjusted to account for the minor scroller widgets.
     */
    scroller = Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು, "MinorTabScrollerNext");
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ಖಾಲಿ);
    scroller = Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು, "MinorTabScrollerPrevious");
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(scroller, XmNwidth, 0, XmNresizable, False,
		  XmNtraversalOn, False, ಖಾಲಿ);

    // Create the tabline popup menu
    tab_ಸಾಲು_menu = XmCreatePopupMenu(tab_ಸಾಲು, "tabline popup", ಖಾಲಿ, 0);

    // Add the buttons to the tabline popup menu
    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nuser_ದತ್ತಾಂಶ, (Xt_ಸೂಚಕ)TABLINE_MENU_CLOSE); n++;
    xms = XmStringCreate((ಅಕ್ಷರ *)"Close tab", STRING_ಟ್ಯಾಗ್);
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "Close", ವಾದಗಳು, n);
    Xt_ಕೂಡಿಸು_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ಖಾಲಿ);
    XmStringFree(xms);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nuser_ದತ್ತಾಂಶ, (Xt_ಸೂಚಕ)TABLINE_MENU_NEW); n++;
    xms = XmStringCreate((ಅಕ್ಷರ *)"New Tab", STRING_ಟ್ಯಾಗ್);
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "New Tab", ವಾದಗಳು, n);
    Xt_ಕೂಡಿಸು_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ಖಾಲಿ);
    XmStringFree(xms);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nuser_ದತ್ತಾಂಶ, (Xt_ಸೂಚಕ)TABLINE_MENU_OPEN); n++;
    xms = XmStringCreate((ಅಕ್ಷರ *)"Open tab...", STRING_ಟ್ಯಾಗ್);
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, xms); n++;
    button = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು_menu, "Open tab...", ವಾದಗಳು, n);
    Xt_ಕೂಡಿಸು_Callback(button, XmNactivateCallback,
		  (XtCallbackProc)tabline_button_cb, ಖಾಲಿ);
    XmStringFree(xms);
#ಆದರೆ_ಅಂತ್ಯ

    ಪಠ್ಯ_Area_Form = XtVaCreateManagedWidget("textAreaForm",
	xmFormWidgetClass, vimForm,
	XmNleftAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	Xm_Nmargin_ಅಗಲ, 0,
	Xm_Nmargin_ಎತ್ತರ, 0,
	XmNresizePolicy, XmRESIZE_ANY,
	ಖಾಲಿ);
    gui_motif_scroll_colors(ಪಠ್ಯ_Area_Form);

    ಪಠ್ಯ_Area = XtVaCreateManagedWidget("textArea",
	xmDrawingAreaWidgetClass, ಪಠ್ಯ_Area_Form,
	XmNforeground, gui.norm_pixel,
	XmNbackground, gui.ಹಿಂಭಾಗ_pixel,
	XmNleftAttachment, XmATTACH_FORM,
	XmNtopAttachment, XmATTACH_FORM,
	XmNrightAttachment, XmATTACH_FORM,
	XmNbottomAttachment, XmATTACH_FORM,

	/*
	 * These take some control away from the user, but avoids making them
	 * add resources to get a decent looking setup.
	 */
	Xm_Nborder_ಅಗಲ, 0,
	XmNhighlightThickness, 0,
	XmNshadowThickness, 0,
	ಖಾಲಿ);

    /*
     * Install the callbacks.
     */
    gui_x11_callbacks(ಪಠ್ಯ_Area, vimForm);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.ಒಳ_focus = false;
}

/*
 * Called when the GUI is not going to start after all.
 */
    ಶೂನ್ಯ
gui_x11_destroy_widgets(ಶೂನ್ಯ)
{
    ಪಠ್ಯ_Area = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    menuBar = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_ಪಠ್ಯ_area_pos(
    ಪೂರ್ಣಾಂಕ	    x ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    y ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    w ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ	    h ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    // Give keyboard focus to the textArea instead of the toolbar.
    reset_focus();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gui_x11_ಗಣ_ಹಿಂಭಾಗ_color(ಶೂನ್ಯ)
{
    ಆದರೆ (ಪಠ್ಯ_Area != ಖಾಲಿ)
#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	XmChangeColor(ಪಠ್ಯ_Area, gui.ಹಿಂಭಾಗ_pixel);
#ಇಲ್ಲದಿದ್ದರೆ
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area,
		  XmNbackground, gui.ಹಿಂಭಾಗ_pixel,
		  ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Manage dialog centered on pointer.
 */
    ಶೂನ್ಯ
manage_centered(Widget dialog_ಮಗು)
{
    Widget shell = Xt_ಪೋಷಕ(dialog_ಮಗು);
    ಕಿಟಕಿ ಬೇರು, ಮಗು;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಅಗಲ, ಎತ್ತರ, border_ಅಗಲ, ಆಳ;
    ಪೂರ್ಣಾಂಕ x, y, win_x, win_y, maxX, maxY;
    Boolean mappedWhenManaged;

    // Temporarily set value of XmNmappedWhenManaged
    // to stop the dialog from popping up right away
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, &mappedWhenManaged, ಖಾಲಿ);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, False, ಖಾಲಿ);

    Xt_Manage_ಮಗು(dialog_ಮಗು);

    // Get the pointer position (x, y)
    XQuery_ಸೂಚಕ(XtDisplay(shell), Xt_ಕಿಟಕಿ(shell), &ಬೇರು, &ಮಗು,
		  &x, &y, &win_x, &win_y, &ಮುಖವಾಡ);

    // Translate the pointer position (x, y) into a position for the new
    // window that will place the pointer at its center
    XGetGeometry(XtDisplay(shell), Xt_ಕಿಟಕಿ(shell), &ಬೇರು, &win_x, &win_y,
		 &ಅಗಲ, &ಎತ್ತರ, &border_ಅಗಲ, &ಆಳ);
    ಅಗಲ += 2 * border_ಅಗಲ;
    ಎತ್ತರ += 2 * border_ಅಗಲ;
    x -= ಅಗಲ / 2;
    y -= ಎತ್ತರ / 2;

    // Ensure that the dialog remains on screen
    maxX = XtScreen(shell)->ಅಗಲ - ಅಗಲ;
    maxY = XtScreen(shell)->ಎತ್ತರ - ಎತ್ತರ;
    ಆದರೆ (x < 0)
	x = 0;
    ಆದರೆ (x > maxX)
	x = maxX;
    ಆದರೆ (y < 0)
	y = 0;
    ಆದರೆ (y > maxY)
	y = maxY;

    // Set desired window position in the DialogShell
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(shell, XmNx, x, XmNy, y, ಖಾಲಿ);

    // Map the widget
    XtMapWidget(shell);

    // Restore the value of XmNmappedWhenManaged
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(shell, XmNmappedWhenManaged, mappedWhenManaged, ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_MENU) || defined(FEAT_GUI_DIALOG)

/*
 * Encapsulate the way an XmFontList is created.
 */
    XmFontList
gui_motif_create_fontlist(XFontStruct *font)
{
    XmFontList font_list;

# ಆದರೆ (Xm_ಆವೃತ್ತಿ <= 1001)
    // Motif 1.1 method
    font_list = XmFontListCreate(font, STRING_ಟ್ಯಾಗ್);
# ಇಲ್ಲದಿದ್ದರೆ
    // Motif 1.2 method
    Xm_Font_List_ನಮೂದು font_list_ನಮೂದು;

    font_list_ನಮೂದು = Xm_Font_List_ನಮೂದು_Create(STRING_ಟ್ಯಾಗ್, XmFONT_IS_FONT,
					    (Xt_ಸೂಚಕ)font);
    font_list = Xm_Font_List_Append_ನಮೂದು(ಖಾಲಿ, font_list_ನಮೂದು);
    Xm_Font_List_ನಮೂದು_Free(&font_list_ನಮೂದು);
# endif
    ಹಿಂತಿರುಗಿ font_list;
}

# ಆದರೆ ((Xm_ಆವೃತ್ತಿ > 1001) && defined(FEAT_XFONTSET))
    XmFontList
gui_motif_fontset2fontlist(XFont_ಗಣ *fontset)
{
    XmFontList font_list;

    // Motif 1.2 method
    Xm_Font_List_ನಮೂದು font_list_ನಮೂದು;

    font_list_ನಮೂದು = Xm_Font_List_ನಮೂದು_Create(STRING_ಟ್ಯಾಗ್,
					    XmFONT_IS_FONTSET,
					    (Xt_ಸೂಚಕ)*fontset);
    font_list = Xm_Font_List_Append_ನಮೂದು(ಖಾಲಿ, font_list_ನಮೂದು);
    Xm_Font_List_ನಮೂದು_Free(&font_list_ನಮೂದು);
    ಹಿಂತಿರುಗಿ font_list;
}
# endif

#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MENU)
/*
 * Menu stuff.
 */

ಸ್ಥಿರ ಶೂನ್ಯ gui_motif_ಕೂಡಿಸು_actext(vimmenu_T *menu);
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
ಸ್ಥಿರ ಶೂನ್ಯ toggle_tearoff(Widget wid);
ಸ್ಥಿರ ಶೂನ್ಯ gui_mch_recurse_tearoffs(vimmenu_T *menu);
# endif
ಸ್ಥಿರ ಶೂನ್ಯ submenu_change(vimmenu_T *mp, ಪೂರ್ಣಾಂಕ colors);

ಸ್ಥಿರ ಶೂನ್ಯ do_ಗಣ_mnemonics(ಪೂರ್ಣಾಂಕ enable);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ menu_enabled = TRUE;

    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (flag)
    {
	Xt_Manage_ಮಗು(menuBar);
# ifdef FEAT_TOOLBAR
	ಆದರೆ (XtIsManaged(Xt_ಪೋಷಕ(toolBar)))
	{
	    // toolBar is attached to top form
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(Xt_ಪೋಷಕ(toolBar),
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, menuBar,
		ಖಾಲಿ);
#  ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			      ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ಖಾಲಿ);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Xt_Unmanage_ಮಗು(menuBar);
# ifdef FEAT_TOOLBAR
	ಆದರೆ (XtIsManaged(Xt_ಪೋಷಕ(toolBar)))
	{
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(Xt_ಪೋಷಕ(toolBar),
		XmNtopAttachment, XmATTACH_FORM,
		ಖಾಲಿ);
#  ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			      ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
# ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM,
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ಖಾಲಿ);
	}
    }

}

/*
 * Enable or disable mnemonics for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_motif_ಗಣ_mnemonics(ಪೂರ್ಣಾಂಕ enable)
{
    /*
     * Don't enable menu mnemonics when the menu bar is disabled, LessTif
     * crashes when using a mnemonic then.
     */
    ಆದರೆ (!menu_enabled)
	enable = FALSE;
    do_ಗಣ_mnemonics(enable);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
do_ಗಣ_mnemonics(ಪೂರ್ಣಾಂಕ enable)
{
    vimmenu_T	*menu;

    FOR_ALL_MENUS(menu)
	ಆದರೆ (menu->id != (Widget)0)
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(menu->id,
		    XmNmnemonic, enable ? menu->mnemonic : NUL,
		    ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    XmString	label;
    Widget	shell;
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

# ifdef MOTIF_POPUP
    ಆದರೆ (menu_is_popup(menu->name))
    {
	Arg arg[2];
	ಪೂರ್ಣಾಂಕ n = 0;

	// Only create the popup menu when it's actually used, otherwise there
	// is a delay when using the right mouse button.
#  ಆದರೆ (Xm_ಆವೃತ್ತಿ <= 1002)
	ಆದರೆ (mouse_model_popup())
#  endif
	{
	    ಆದರೆ (gui.menu_bg_pixel != INVALCOLOR)
	    {
		Xt_ಗಣ_Arg(arg[0], XmNbackground, gui.menu_bg_pixel); n++;
	    }
	    ಆದರೆ (gui.menu_fg_pixel != INVALCOLOR)
	    {
		Xt_ಗಣ_Arg(arg[1], XmNforeground, gui.menu_fg_pixel); n++;
	    }
	    menu->submenu_id = XmCreatePopupMenu(ಪಠ್ಯ_Area, "contextMenu",
								      arg, n);
	    menu->id = (Widget)0;
	}
	ಹಿಂತಿರುಗಿ;
    }
# endif

    ಆದರೆ (!menu_is_menubar(menu->name)
	    || (ಪೋಷಕ != ಖಾಲಿ && ಪೋಷಕ->submenu_id == (Widget)0))
	ಹಿಂತಿರುಗಿ;

    label = XmStringCreate((ಅಕ್ಷರ *)menu->dname, STRING_ಟ್ಯಾಗ್);
    ಆದರೆ (label == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    menu->id = XtVaCreateWidget("subMenu",
	    xmCascadeButtonWidgetClass,
	    (ಪೋಷಕ == ಖಾಲಿ) ? menuBar : ಪೋಷಕ->submenu_id,
	    XmNlabelString, label,
	    XmNmnemonic, p_wak[0] == 'n' ? NUL : menu->mnemonic,
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	    // submenu: count the tearoff item (needed for LessTif)
	    Xm_Nposition_ಸೂಚ್ಯಂಕ, idx + (ಪೋಷಕ != ಖಾಲಿ
			   && tearoff_val == (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED ? 1 : 0),
# endif
	    ಖಾಲಿ);
    XmStringFree(label);

    ಆದರೆ (menu->id == (Widget)0)		// failed
	ಹಿಂತಿರುಗಿ;

    // The "Help" menu is a special case, and should be placed at the far
    // right hand side of the menu-bar.  It's recognized by its high priority.
    ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ && menu->ಆದ್ಯತೆ >= 9999)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(menuBar,
		XmNmenuHelpWidget, menu->id,
		ಖಾಲಿ);

    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);

    // add accelerator text
    gui_motif_ಕೂಡಿಸು_actext(menu);

    shell = XtVaCreateWidget("subMenuShell",
	xmMenuShellWidgetClass, menu->id,
	XmNwidth, 1,
	XmNheight, 1,
	ಖಾಲಿ);
    gui_motif_menu_colors(shell);
    menu->submenu_id = XtVaCreateWidget("rowColumnMenu",
	xm_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ_Widget_Class, shell,
	Xm_Nrow_ಸ್ತಂಭ_Type, XmMENU_PULLDOWN,
	ಖಾಲಿ);
    gui_motif_menu_colors(menu->submenu_id);

    ಆದರೆ (menu->submenu_id == (Widget)0)		// failed
	ಹಿಂತಿರುಗಿ;

# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
    // Set the colors for the tear off widget
    toggle_tearoff(menu->submenu_id);
# endif

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(menu->id,
	XmNsubMenuId, menu->submenu_id,
	ಖಾಲಿ);

    // When we add a top-level item to the menu bar, we can figure out how
    // high the menu bar should be.
    ಆದರೆ (ಪೋಷಕ == ಖಾಲಿ)
	gui_mch_compute_menu_ಎತ್ತರ(menu->id);
}


/*
 * Add mnemonic and accelerator text to a menu button.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_ಕೂಡಿಸು_actext(vimmenu_T *menu)
{
    XmString	label;

    // Add accelerator text, if there is one
    ಆದರೆ (menu->actext == ಖಾಲಿ || menu->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    label = XmStringCreate((ಅಕ್ಷರ *)menu->actext, STRING_ಟ್ಯಾಗ್);
    ಆದರೆ (label == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(menu->id, Xm_Naccelerator_ಪಠ್ಯ, label, ಖಾಲಿ);
    XmStringFree(label);
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಂಕ enable)
{
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
    ಆದರೆ (enable)
	tearoff_val = (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED;
    ಇಲ್ಲದಿದ್ದರೆ
	tearoff_val = (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_DISABLED;
    toggle_tearoff(menuBar);
    gui_mch_recurse_tearoffs(ಬೇರು_menu);
# endif
}

# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
/*
 * Set the tearoff for one menu widget on or off, and set the color of the
 * tearoff widget.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
toggle_tearoff(Widget wid)
{
    Widget	w;

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(wid, XmNtearOffModel, tearoff_val, ಖಾಲಿ);
    ಆದರೆ (tearoff_val == (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED
	    && (w = XmGetTearOffControl(wid)) != (Widget)0)
	gui_motif_menu_colors(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
gui_mch_recurse_tearoffs(vimmenu_T *menu)
{
    ಆಗ (menu != ಖಾಲಿ)
    {
	ಆದರೆ (!menu_is_popup(menu->name))
	{
	    ಆದರೆ (menu->submenu_id != (Widget)0)
		toggle_tearoff(menu->submenu_id);
	    gui_mch_recurse_tearoffs(menu->children);
	}
	menu = menu->ಮುಂದಿನ;
    }
}
# endif

    ಪೂರ್ಣಾಂಕ
gui_mch_ಪಠ್ಯ_area_extra_ಎತ್ತರ(ಶೂನ್ಯ)
{
    Dimension	shadow_ಎತ್ತರ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form, XmNshadowThickness, &shadow_ಎತ್ತರ, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ shadow_ಎತ್ತರ;
}

/*
 * Compute the height of the menu bar.
 * We need to check all the items for their position and height, for the case
 * there are several rows, and/or some characters extend higher or lower.
 */
    ಶೂನ್ಯ
gui_mch_compute_menu_ಎತ್ತರ(
    Widget	id)		    // can be NULL when deleting menu
{
    Dimension	y, maxy;
    Dimension	margin, shadow;
    vimmenu_T	*mp;
    ಸ್ಥಿರ Dimension	ಎತ್ತರ = 21;	// normal height of a menu item

    /*
     * Get the height of the new item, before managing it, because it will
     * still reflect the font size.  After managing it depends on the menu
     * height, which is what we just wanted to get!.
     */
    ಆದರೆ (id != (Widget)0)
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(id, XmNheight, &ಎತ್ತರ, ಖಾಲಿ);

    // Find any menu Widget, to be able to call XtManageChild()
    ಇಲ್ಲದಿದ್ದರೆ
	FOR_ALL_MENUS(mp)
	    ಆದರೆ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	    {
		id = mp->id;
		ನಿಲ್ಲಿಸು;
	    }

    /*
     * Now manage the menu item, to make them all be positioned (makes an
     * extra row when needed, removes it when not needed).
     */
    ಆದರೆ (id != (Widget)0)
	Xt_Manage_ಮಗು(id);

    /*
     * Now find the menu item that is the furthest down, and get its position.
     */
    maxy = 0;
    FOR_ALL_MENUS(mp)
    {
	ಆದರೆ (mp->id != (Widget)0 && menu_is_menubar(mp->name))
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(mp->id, XmNy, &y, ಖಾಲಿ);
	    ಆದರೆ (y > maxy)
		maxy = y;
	}
    }

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(menuBar,
	Xm_Nmargin_ಎತ್ತರ, &margin,
	XmNshadowThickness, &shadow,
	ಖಾಲಿ);

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
    gui.menu_ಎತ್ತರ = maxy + ಎತ್ತರ - 2 * shadow + 2 * margin + 4;

    // Somehow the menu bar doesn't resize automatically.  Set it here,
    // even though this is a catch 22.  Don't do this when starting up,
    // somehow the menu gets very high then.
    ಆದರೆ (gui.shell_created)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(menuBar, XmNheight, gui.menu_ಎತ್ತರ, ಖಾಲಿ);
}

# ifdef FEAT_TOOLBAR

/*
 * Icons used by the toolbar code.
 */
#  include "gui_x11_pm.h"

ಸ್ಥಿರ ಅಕ್ಷರ **get_toolbar_pixmap(vimmenu_T *menu, ಅಕ್ಷರ **fname);

/*
 * Read an Xpm file.  Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_xpm(char_u *ಮಾರ್ಗ)
{
    Xpm_ಗುಣಲಕ್ಷಣಗಳು attrs;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
    Pixmap	ಮುಖವಾಡ;
    Pixmap	map;

    attrs.valuemask = 0;

    // Create the "sensitive" pixmap
    ಸ್ಥಿತಿ_2 = Xpm_Read_File_ಗಮ್ಯ_Pixmap(gui.dpy,
	    ಬೇರು_ಕಿಟಕಿ_2(gui.dpy, DefaultScreen(gui.dpy)),
	    (ಅಕ್ಷರ *)ಮಾರ್ಗ, &map, &ಮುಖವಾಡ, &attrs);
    Xpm_Free_ಗುಣಲಕ್ಷಣಗಳು(&attrs);

    ಆದರೆ (ಸ್ಥಿತಿ_2 == XpmSuccess)
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}


/*
 * Allocated a pixmap for toolbar menu "menu".
 * When it's to be read from a file, "fname" is set to the file name
 * (in allocated memory).
 * Return a blank pixmap if it fails.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ **
get_toolbar_pixmap(vimmenu_T *menu, ಅಕ್ಷರ **fname)
{
    char_u	buf[MAXPATHL];		// buffer storing expanded pathname
    ಅಕ್ಷರ	**xpm = ಖಾಲಿ;		// xpm array
    ಪೂರ್ಣಾಂಕ		res;

    *fname = ಖಾಲಿ;
    buf[0] = NUL;			// start with NULL path

    ಆದರೆ (menu->iconfile != ಖಾಲಿ)
    {
	// Use the "icon="  argument.
	gui_ಕಂಡುಹಿಡಿ_iconfile(menu->iconfile, buf, "xpm");
	res = check_xpm(buf);

	// If it failed, try using the menu name.
	ಆದರೆ (res == FAIL && gui_ಕಂಡುಹಿಡಿ_bitmap(menu->name, buf, "xpm") == OK)
	    res = check_xpm(buf);
	ಆದರೆ (res == OK)
	{
	    *fname = (ಅಕ್ಷರ *)vim_strsave(buf);
	    ಹಿಂತಿರುಗಿ tb_blank_xpm;
	}
    }

    ಆದರೆ (menu->icon_builtin || gui_ಕಂಡುಹಿಡಿ_bitmap(menu->name, buf, "xpm") == FAIL)
    {
	ಆದರೆ (menu->iconidx >= 0 && menu->iconidx
	       < (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(built_ಒಳ_pixmaps) / ಗಾತ್ರ(built_ಒಳ_pixmaps[0])))
	    xpm = built_ಒಳ_pixmaps[menu->iconidx];
	ಇಲ್ಲದಿದ್ದರೆ
	    xpm = tb_blank_xpm;
    }

    ಹಿಂತಿರುಗಿ xpm;
}

/*
 * Add arguments for the toolbar pixmap to a menu item.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_pixmap_ವಾದಗಳು(vimmenu_T *menu, Arg *ವಾದಗಳು, ಪೂರ್ಣಾಂಕ n)
{
    vim_free(menu->xpm_fname);
    menu->xpm = get_toolbar_pixmap(menu, &menu->xpm_fname);
    ಆದರೆ (menu->xpm == ಖಾಲಿ)
    {
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelType, XmSTRING); n++;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (menu->xpm_fname != ಖಾಲಿ)
	{
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNpixmapFile, menu->xpm_fname); n++;
	}
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Npixmap_ದತ್ತಾಂಶ, menu->xpm); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelLocation, XmBOTTOM); n++;
    }
    ಹಿಂತಿರುಗಿ n;
}
# endif // FEAT_TOOLBAR

    ಶೂನ್ಯ
gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ idx)
{
    XmString	label;
    vimmenu_T	*ಪೋಷಕ = menu->ಪೋಷಕ;

# ಆದರೆ (Xm_ಆವೃತ್ತಿ <= 1002)
    // Don't add Popup menu items when the popup menu isn't used.
    ಆದರೆ (menu_is_ಮಗು_ದ_popup(menu) && !mouse_model_popup())
	ಹಿಂತಿರುಗಿ;
# endif

# ifdef FEAT_TOOLBAR
    ಆದರೆ (menu_is_toolbar(ಪೋಷಕ->name))
    {
	WidgetClass	type;
	XmString	xms = ಖಾಲಿ;    // fallback label if pixmap not found
	ಪೂರ್ಣಾಂಕ		n;
	Arg		ವಾದಗಳು[18];

	n = 0;
	ಆದರೆ (menu_is_separator(menu->name))
	{
	    ಅಕ್ಷರ	*cp;
	    Dimension	wid;

	    /*
	     * A separator has the format "-sep%d[:%d]-". The optional :%d is
	     * a width specifier. If no width is specified then we choose one.
	     */
	    cp = (ಅಕ್ಷರ *)vim_strchr(menu->name, ':');
	    ಆದರೆ (cp != ಖಾಲಿ)
		wid = (Dimension)atoi(++cp);
	    ಇಲ್ಲದಿದ್ದರೆ
		wid = 4;

	    type = xmSeparatorWidgetClass;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNwidth, wid); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nmin_ಅಗಲ, wid); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation, XmVERTICAL); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNseparatorType, Xm_SHADOW_ETCHED_ಒಳ); n++;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Without shadows one can't sense whatever the button has been
	    // pressed or not! However we want to save a bit of space...
	    // Need the highlightThickness to see the focus.
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNhighlightThickness, 1); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNhighlightOnEnter, True); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nmargin_ಅಗಲ, 0); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nmargin_ಎತ್ತರ, 0); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtraversalOn, False); n++;
	    // Set the label here, so that we can switch between icons/text
	    // by changing the XmNlabelType resource.
	    xms = XmStringCreate((ಅಕ್ಷರ *)menu->dname, STRING_ಟ್ಯಾಗ್);
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelString, xms); n++;

	    n = ಕೂಡಿಸು_pixmap_ವಾದಗಳು(menu, ವಾದಗಳು, n);

	    type = xmEnhancedButtonWidgetClass;
	}

	Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nposition_ಸೂಚ್ಯಂಕ, idx); n++;
	ಆದರೆ (menu->id == ಖಾಲಿ)
	{
	    menu->id = XtCreateManagedWidget((ಅಕ್ಷರ *)menu->dname,
			type, toolBar, ವಾದಗಳು, n);
	    ಆದರೆ (menu->id != ಖಾಲಿ && type == xmEnhancedButtonWidgetClass)
	    {
		Xt_ಕೂಡಿಸು_Callback(menu->id,
			XmNactivateCallback, gui_x11_menu_cb, menu);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(menu->id, ವಾದಗಳು, n);
	ಆದರೆ (xms != ಖಾಲಿ)
	    XmStringFree(xms);

#  ifdef FEAT_BEVAL_GUI
	gui_mch_menu_ಗಣ_tip(menu);
#  endif

	menu->ಪೋಷಕ = ಪೋಷಕ;
	menu->submenu_id = ಖಾಲಿ;
	// When adding first item to toolbar it might have to be enabled .
	ಆದರೆ (!XtIsManaged(Xt_ಪೋಷಕ(toolBar))
		    && vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ)
	    gui_mch_show_toolbar(TRUE);
	gui.toolbar_ಎತ್ತರ = gui_mch_compute_toolbar_ಎತ್ತರ();
	ಹಿಂತಿರುಗಿ;
    } // toolbar menu item
# endif

    // No parent, must be a non-menubar menu
    ಆದರೆ (ಪೋಷಕ->submenu_id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    menu->submenu_id = (Widget)0;

    // Add menu separator
    ಆದರೆ (menu_is_separator(menu->name))
    {
	menu->id = XtVaCreateWidget("subMenu",
		xmSeparatorGadgetClass, ಪೋಷಕ->submenu_id,
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
		// count the tearoff item (needed for LessTif)
		Xm_Nposition_ಸೂಚ್ಯಂಕ, idx + (tearoff_val == (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
		ಖಾಲಿ);
	gui_motif_menu_colors(menu->id);
	ಹಿಂತಿರುಗಿ;
    }

    label = XmStringCreate((ಅಕ್ಷರ *)menu->dname, STRING_ಟ್ಯಾಗ್);
    ಆದರೆ (label == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    menu->id = XtVaCreateWidget("subMenu",
	xm_ತಳ್ಳು_Button_Widget_Class, ಪೋಷಕ->submenu_id,
	XmNlabelString, label,
	XmNmnemonic, menu->mnemonic,
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	// count the tearoff item (needed for LessTif)
	Xm_Nposition_ಸೂಚ್ಯಂಕ, idx + (tearoff_val == (ಪೂರ್ಣಾಂಕ)XmTEAR_OFF_ENABLED
								     ? 1 : 0),
# endif
	ಖಾಲಿ);
    gui_motif_menu_colors(menu->id);
    gui_motif_menu_fontlist(menu->id);
    XmStringFree(label);

    ಆದರೆ (menu->id != (Widget)0)
    {
	Xt_ಕೂಡಿಸು_Callback(menu->id, XmNactivateCallback, gui_x11_menu_cb,
		(Xt_ಸೂಚಕ)menu);
	// add accelerator text
	gui_motif_ಕೂಡಿಸು_actext(menu);
    }
}

# ಆದರೆ (Xm_ಆವೃತ್ತಿ <= 1002)
/*
 * This function will destroy/create the popup menus dynamically,
 * according to the value of 'mousemodel'.
 * This will fix the "right mouse button freeze" that occurs when
 * there exists a popup menu but it isn't managed.
 */
    ಶೂನ್ಯ
gui_motif_update_mousemodel(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಂಕ		idx = 0;

    // When GUI hasn't started the menus have not been created.
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    ಆಗ (menu)
    {
      ಆದರೆ (menu->children != ಖಾಲಿ)
      {
	  ಆದರೆ (menu_is_popup(menu->name))
	  {
	      ಆದರೆ (mouse_model_popup())
	      {
		  // Popup menu will be used.  Create the popup menus.
		  gui_mch_ಕೂಡಿಸು_menu(menu, idx);
		  gui_motif_update_mousemodel(menu->children);
	      }
	      ಇಲ್ಲದಿದ್ದರೆ
	      {
		  // Popup menu will not be used.  Destroy the popup menus.
		  gui_motif_update_mousemodel(menu->children);
		  gui_mch_destroy_menu(menu);
	      }
	  }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (menu_is_ಮಗು_ದ_popup(menu))
      {
	  ಆದರೆ (mouse_model_popup())
	      gui_mch_ಕೂಡಿಸು_menu_ಅಂಶ(menu, idx);
	  ಇಲ್ಲದಿದ್ದರೆ
	      gui_mch_destroy_menu(menu);
      }
      menu = menu->ಮುಂದಿನ;
      ++idx;
    }
}
# endif

    ಶೂನ್ಯ
gui_mch_new_menu_colors(ಶೂನ್ಯ)
{
    ಆದರೆ (menuBar == (Widget)0)
	ಹಿಂತಿರುಗಿ;
    gui_motif_menu_colors(menuBar);
# ifdef FEAT_TOOLBAR
    gui_motif_menu_colors(tool_Bar_ಚೌಕಟ್ಟು);
    gui_motif_menu_colors(toolBar);
# endif

    submenu_change(ಬೇರು_menu, TRUE);
}

    ಶೂನ್ಯ
gui_mch_new_menu_font(ಶೂನ್ಯ)
{
    ಆದರೆ (menuBar == (Widget)0)
	ಹಿಂತಿರುಗಿ;
    submenu_change(ಬೇರು_menu, FALSE);
    {
	Dimension   ಎತ್ತರ;
	Position w, h;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(menuBar, XmNheight, &ಎತ್ತರ, ಖಾಲಿ);
	gui.menu_ಎತ್ತರ = ಎತ್ತರ;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNwidth, &w, XtNheight, &h, ಖಾಲಿ);
	gui_resize_shell(w, h
# ifdef FEAT_XIM
		- xim_get_ಸ್ಥಿತಿ_area_ಎತ್ತರ()
# endif
		     );
    }
    gui_ಗಣ_shellsize(FALSE, TRUE, RESIZE_VERT);
    ui_new_shellsize();
}

# ಆದರೆ defined(FEAT_BEVAL_GUI)
    ಶೂನ್ಯ
gui_mch_new_tooltip_font(ಶೂನ್ಯ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *menu;

    ಆದರೆ (toolBar == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    menu = gui_ಕಂಡುಹಿಡಿ_menu((char_u *)"ToolBar");
    ಆದರೆ (menu != ಖಾಲಿ)
	submenu_change(menu, FALSE);
#  endif
}

    ಶೂನ್ಯ
gui_mch_new_tooltip_colors(ಶೂನ್ಯ)
{
#  ifdef FEAT_TOOLBAR
    vimmenu_T   *toolbar;

    ಆದರೆ (toolBar == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    toolbar = gui_ಕಂಡುಹಿಡಿ_menu((char_u *)"ToolBar");
    ಆದರೆ (toolbar != ಖಾಲಿ)
	submenu_change(toolbar, TRUE);
#  endif
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
submenu_change(
    vimmenu_T	*menu,
    ಪೂರ್ಣಾಂಕ		colors)		// TRUE for colors, FALSE for font
{
    vimmenu_T	*mp;

    ಗಾಗಿ (mp = menu; mp != ಖಾಲಿ; mp = mp->ಮುಂದಿನ)
    {
	ಆದರೆ (mp->id != (Widget)0)
	{
	    ಆದರೆ (colors)
	    {
		gui_motif_menu_colors(mp->id);
# ifdef FEAT_TOOLBAR
		// For a toolbar item: Free the pixmap and allocate a new one,
		// so that the background color is right.
		ಆದರೆ (mp->xpm != ಖಾಲಿ)
		{
		    ಪೂರ್ಣಾಂಕ		n = 0;
		    Arg		ವಾದಗಳು[18];

		    n = ಕೂಡಿಸು_pixmap_ವಾದಗಳು(mp, ವಾದಗಳು, n);
		    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(mp->id, ವಾದಗಳು, n);
		}
#  ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ಆದರೆ (mp->tip != ಖಾಲಿ)
		{
		    Arg ವಾದಗಳು[2];

		    ವಾದಗಳು[0].name = XmNbackground;
		    ವಾದಗಳು[0].ಮೌಲ್ಯ = gui.tooltip_bg_pixel;
		    ವಾದಗಳು[1].name = XmNforeground;
		    ವಾದಗಳು[1].ಮೌಲ್ಯ = gui.tooltip_fg_pixel;
		    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(mp->tip->balloonLabel, &ವಾದಗಳು[0], Xt_ಸಂಖ್ಯೆ(ವಾದಗಳು));
		}
#  endif
# endif
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		gui_motif_menu_fontlist(mp->id);
# ifdef FEAT_BEVAL_GUI
		// If we have a tooltip, then we need to change its font
		ಆದರೆ (mp->tip != ಖಾಲಿ)
		{
		    Arg ವಾದಗಳು[1];

		    ವಾದಗಳು[0].name = XmNfontList;
		    ವಾದಗಳು[0].ಮೌಲ್ಯ = (XtArgVal)gui_motif_fontset2fontlist(
						    &gui.tooltip_fontset);
		    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(mp->tip->balloonLabel, &ವಾದಗಳು[0], Xt_ಸಂಖ್ಯೆ(ವಾದಗಳು));
		}
# endif
	    }
	}

	ಆದರೆ (mp->children != ಖಾಲಿ)
	{
# ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	    // Set the colors/font for the tear off widget
	    ಆದರೆ (mp->submenu_id != (Widget)0)
	    {
		ಆದರೆ (colors)
		    gui_motif_menu_colors(mp->submenu_id);
		ಇಲ್ಲದಿದ್ದರೆ
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
    ಆದರೆ (menu->submenu_id != (Widget)0)
    {
	XtDestroyWidget(menu->submenu_id);
	menu->submenu_id = (Widget)0;
    }

    ಆದರೆ (menu->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    Widget	    ಪೋಷಕ;

    ಪೋಷಕ = Xt_ಪೋಷಕ(menu->id);
# ಆದರೆ defined(FEAT_TOOLBAR) && defined(FEAT_BEVAL_GUI)
    ಆದರೆ (ಪೋಷಕ == toolBar && menu->tip != ಖಾಲಿ)
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
	menu->tip = ಖಾಲಿ;
    }
# endif
    XtDestroyWidget(menu->id);
    menu->id = (Widget)0;
    ಆದರೆ (ಪೋಷಕ == menuBar)
	gui_mch_compute_menu_ಎತ್ತರ((Widget)0);
# ifdef FEAT_TOOLBAR
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪೋಷಕ == toolBar)
    {
	Cardinal    ಅಂಕೆ_children;

	// When removing last toolbar item, don't display the toolbar.
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNnumChildren, &ಅಂಕೆ_children, ಖಾಲಿ);
	ಆದರೆ (ಅಂಕೆ_children == 0)
	    gui_mch_show_toolbar(FALSE);
	ಇಲ್ಲದಿದ್ದರೆ
	    gui.toolbar_ಎತ್ತರ = gui_mch_compute_toolbar_ಎತ್ತರ();
    }
# endif
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu ಬಳಸದ)
{
# ifdef MOTIF_POPUP
    XmMenuPosition(menu->submenu_id, gui_x11_get_ಕೊನೆಯ_mouse_event());
    Xt_Manage_ಮಗು(menu->submenu_id);
# endif
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

/*
 * Set the menu and scrollbar colors to their default values.
 */
    ಶೂನ್ಯ
gui_mch_def_colors(ಶೂನ್ಯ)
{
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    gui.menu_fg_pixel = gui_get_color((char_u *)gui.rsrc_menu_fg_name);
    gui.menu_bg_pixel = gui_get_color((char_u *)gui.rsrc_menu_bg_name);
    gui.scroll_fg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_fg_name);
    gui.scroll_bg_pixel = gui_get_color((char_u *)gui.rsrc_scroll_bg_name);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_GUI
    gui.tooltip_fg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_fg_name);
    gui.tooltip_bg_pixel =
	gui_get_color((char_u *)gui.rsrc_tooltip_bg_name);
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * Scrollbar stuff.
 */

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_thumb(
    scrollbar_T *sb,
    ದೀರ್ಘ	val,
    ದೀರ್ಘ	ಗಾತ್ರ_2,
    ದೀರ್ಘ	max)
{
    ಆದರೆ (sb->id != (Widget)0)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(sb->id,
		  XmNvalue, val,
		  Xm_Nslider_ಗಾತ್ರ, ಗಾತ್ರ_2,
		  XmNpageIncrement, (ಗಾತ್ರ_2 > 2 ? ಗಾತ್ರ_2 - 2 : 1),
		  XmNmaximum, max + 1,	    // Motif has max one past the end
		  ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_pos(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಂಕ		x,
    ಪೂರ್ಣಾಂಕ		y,
    ಪೂರ್ಣಾಂಕ		w,
    ಪೂರ್ಣಾಂಕ		h)
{
    ಆದರೆ (sb->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (sb->type == SBAR_ಎಡ || sb->type == SBAR_ಬಲ)
    {
	ಆದರೆ (y == 0)
	    h -= gui.border_ಸ್ಥಾನಾಂತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    y -= gui.border_ಸ್ಥಾನಾಂತರ;
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(sb->id,
		Xm_Ntop_ಸ್ಥಾನಾಂತರ, y,
		Xm_Nbottom_ಸ್ಥಾನಾಂತರ, -y - h,
		XmNwidth, w,
		ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(sb->id,
		Xm_Ntop_ಸ್ಥಾನಾಂತರ, y,
		Xm_Nleft_ಸ್ಥಾನಾಂತರ, x,
		Xm_Nright_ಸ್ಥಾನಾಂತರ, gui.which_scrollbars[SBAR_ಬಲ]
						     ? gui.scrollbar_ಅಗಲ : 0,
		XmNheight, h,
		ಖಾಲಿ);
    Xt_Manage_ಮಗು(sb->id);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ xpad;
    Dimension tw, ww;
    Position  tx;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area, XtNwidth, &tw, XtNx, &tx, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNwidth, &ww, ಖಾಲಿ);
    xpad = ww - tw - tx - gui.scrollbar_ಅಗಲ;
    ಹಿಂತಿರುಗಿ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ypad;
    Dimension th, wh;
    Position  ty;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area, XtNheight, &th, XtNy, &ty, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(vimShell, XtNheight, &wh, ಖಾಲಿ);
    ypad = wh - th - ty - gui.scrollbar_ಎತ್ತರ;
    ಹಿಂತಿರುಗಿ (ypad < 0) ? 0 : ypad;
}

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ flag)
{
    Arg		ವಾದಗಳು[16];
    ಪೂರ್ಣಾಂಕ		n;

    ಆದರೆ (sb->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    n = 0;
    ಆದರೆ (flag)
    {
	ಆಯ್ಕೆ (sb->type)
	{
	    ಸ್ಥಿತಿ SBAR_ಎಡ:
		Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nleft_ಸ್ಥಾನಾಂತರ, gui.scrollbar_ಅಗಲ); n++;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ SBAR_ಬಲ:
		Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nright_ಸ್ಥಾನಾಂತರ, gui.scrollbar_ಅಗಲ); n++;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ SBAR_BOTTOM:
		Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nbottom_ಸ್ಥಾನಾಂತರ, gui.scrollbar_ಎತ್ತರ);n++;
		ನಿಲ್ಲಿಸು;
	}
	Xt_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area, ವಾದಗಳು, n);
	Xt_Manage_ಮಗು(sb->id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!gui.which_scrollbars[sb->type])
	{
	    // The scrollbars of this type are all disabled, adjust the
	    // textArea attachment offset.
	    ಆಯ್ಕೆ (sb->type)
	    {
		ಸ್ಥಿತಿ SBAR_ಎಡ:
		    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nleft_ಸ್ಥಾನಾಂತರ, 0); n++;
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ SBAR_ಬಲ:
		    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nright_ಸ್ಥಾನಾಂತರ, 0); n++;
		    ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ SBAR_BOTTOM:
		    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 0);n++;
		    ನಿಲ್ಲಿಸು;
	    }
	    Xt_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area, ವಾದಗಳು, n);
	}
	Xt_Unmanage_ಮಗು(sb->id);
    }
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(
    scrollbar_T *sb,
    ಪೂರ್ಣಾಂಕ		orient)	// SBAR_VERT or SBAR_HORIZ
{
    Arg		ವಾದಗಳು[16];
    ಪೂರ್ಣಾಂಕ		n = 0;

    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNminimum, 0); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation,
	    (orient == SBAR_VERT) ? XmVERTICAL : XmHORIZONTAL); n++;

    ಆಯ್ಕೆ (sb->type)
    {
	ಸ್ಥಿತಿ SBAR_ಎಡ:
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNwidth, gui.scrollbar_ಅಗಲ); n++;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SBAR_ಬಲ:
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtopAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_OPPOSITE_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNwidth, gui.scrollbar_ಅಗಲ); n++;
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SBAR_BOTTOM:
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_FORM); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNheight, gui.scrollbar_ಎತ್ತರ); n++;
	    ನಿಲ್ಲಿಸು;
    }

    sb->id = XtCreateWidget("scrollBar",
	    xmScrollBarWidgetClass, ಪಠ್ಯ_Area_Form, ವಾದಗಳು, n);
    ಆದರೆ (sb->id == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    gui_mch_ಗಣ_scrollbar_colors(sb);
    Xt_ಕೂಡಿಸು_Callback(sb->id, Xm_Nvalue_ಬದಲಾಯಿತು_Callback,
	    scroll_cb, (Xt_ಸೂಚಕ)sb->ident);
    Xt_ಕೂಡಿಸು_Callback(sb->id, XmNdragCallback,
	    scroll_cb, (Xt_ಸೂಚಕ)sb->ident);
    Xt_ಕೂಡಿಸು_Event_Handler(sb->id, ಕೀಲಿ_Press_ಮುಖವಾಡ, FALSE, gui_x11_ಕೀಲಿ_ದೊರಕಿತು_cb,
	    (Xt_ಸೂಚಕ)0);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಆದರೆ (sb->id != (Widget)0)
	XtDestroyWidget(sb->id);
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_scrollbar_colors(scrollbar_T *sb)
{
    ಆದರೆ (sb->id != (Widget)0)
    {
	ಆದರೆ (gui.scroll_bg_pixel != INVALCOLOR)
	{
#ಆದರೆ (Xm_ಆವೃತ್ತಿ>=1002)
	    // This should not only set the through color but also adjust
	    // related colors, such as shadows.
	    XmChangeColor(sb->id, gui.scroll_bg_pixel);
#ಆದರೆ_ಅಂತ್ಯ

	    // Set the through color directly, in case XmChangeColor() decided
	    // to change it.
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(sb->id,
		    XmNtroughColor, gui.scroll_bg_pixel,
		    ಖಾಲಿ);
	}

	ಆದರೆ (gui.scroll_fg_pixel != INVALCOLOR)
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(sb->id,
		    XmNforeground, gui.scroll_fg_pixel,
		    XmNbackground, gui.scroll_fg_pixel,
		    ಖಾಲಿ);
    }

    // This is needed for the rectangle below the vertical scrollbars.
    ಆದರೆ (sb == &gui.bottom_sbar && ಪಠ್ಯ_Area_Form != (Widget)0)
	gui_motif_scroll_colors(ಪಠ್ಯ_Area_Form);
}

/*
 * Miscellaneous stuff:
 */

    ಕಿಟಕಿ
gui_x11_get_wid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ(Xt_ಕಿಟಕಿ(ಪಠ್ಯ_Area));
}

/*
 * Look for a widget in the widget tree w, with a mnemonic matching keycode.
 * When one is found, simulate a button press on that widget and give it the
 * keyboard focus.  If the mnemonic is on a label, look in the userData field
 * of the label to see if it points to another widget, and give that the focus.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
do_mnemonic(Widget w, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keycode)
{
    WidgetList	    children;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ_Children, i;
    Boolean	    isMenu;
    ಕೀಲಿ_Sym	    mnemonic = '\0';
    ಅಕ್ಷರ	    mneString[2];
    Widget	    user_ದತ್ತಾಂಶ_2;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type;

    ಆದರೆ (Xt_Is_ಸಂಯುಕ್ತ(w))
    {
	ಆದರೆ (XtClass(w) == xm_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ_Widget_Class)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, Xm_Nrow_ಸ್ತಂಭ_Type, &ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type, ಖಾಲಿ);
	    isMenu = (ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type != (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)XmWORK_AREA);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    isMenu = False;
	ಆದರೆ (!isMenu)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNchildren, &children, XmNnumChildren,
			  &ಅಂಕೆ_Children, ಖಾಲಿ);
	    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_Children; i++)
		do_mnemonic(children[i], keycode);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNmnemonic, &mnemonic, ಖಾಲಿ);
	ಆದರೆ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    ಆದರೆ (XKeysym_ಗಮ್ಯ_Keycode(XtDisplay(Xt_ಪೋಷಕ(w)),
				       XString_ಗಮ್ಯ_Keysym(mneString)) == keycode)
	    {
		ಆದರೆ (XtClass(w) == xmLabelWidgetClass
			|| XtClass(w) == xmLabelGadgetClass)
		{
		    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, Xm_Nuser_ದತ್ತಾಂಶ, &user_ದತ್ತಾಂಶ_2, ಖಾಲಿ);
		    ಆದರೆ (user_ದತ್ತಾಂಶ_2 != ಖಾಲಿ && XtIsWidget(user_ದತ್ತಾಂಶ_2))
			XmProcessTraversal(user_ದತ್ತಾಂಶ_2, XmTRAVERSE_CURRENT);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    XKeyPressedEvent ಕೀಲಿ_Event;

		    XmProcessTraversal(w, XmTRAVERSE_CURRENT);

		    CLEAR_ಕ್ಷೇತ್ರ(ಕೀಲಿ_Event);
		    ಕೀಲಿ_Event.type = ಕೀಲಿ_Press;
		    ಕೀಲಿ_Event.serial = 1;
		    ಕೀಲಿ_Event.send_event = True;
		    ಕೀಲಿ_Event.display = XtDisplay(w);
		    ಕೀಲಿ_Event.ಕಿಟಕಿ_2 = Xt_ಕಿಟಕಿ(w);
		    XtCallActionProc(w, "Activate", (XEvent *) & ಕೀಲಿ_Event,
								     ಖಾಲಿ, 0);
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
	Xt_ಸೂಚಕ   call_ದತ್ತಾಂಶ ಬಳಸದ,
	XKeyEvent   *event,
	Boolean	    *b ಬಳಸದ)
{
    do_mnemonic(w, event->keycode);
}


/*
 * Search the widget tree under w for widgets with mnemonics.  When found, add
 * a passive grab to the dialog widget for the mnemonic character, thus
 * directing mnemonic events to the dialog widget.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_mnemonic_grabs(Widget dialog, Widget w)
{
    ಅಕ್ಷರ	    mneString[2];
    WidgetList	    children;
    ಪೂರ್ಣಾಂಕ		    ಅಂಕೆ_Children, i;
    Boolean	    isMenu;
    ಕೀಲಿ_Sym	    mnemonic = '\0';
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ   ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type;

    ಆದರೆ (Xt_Is_ಸಂಯುಕ್ತ(w))
    {
	ಆದರೆ (XtClass(w) == xm_ಅಡ್ಡಸಾಲು_ಸ್ತಂಭ_Widget_Class)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, Xm_Nrow_ಸ್ತಂಭ_Type, &ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type, ಖಾಲಿ);
	    isMenu = (ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_Type != (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)XmWORK_AREA);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    isMenu = False;
	ಆದರೆ (!isMenu)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNchildren, &children, XmNnumChildren,
							  &ಅಂಕೆ_Children, ಖಾಲಿ);
	    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_Children; i++)
		ಕೂಡಿಸು_mnemonic_grabs(dialog, children[i]);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(w, XmNmnemonic, &mnemonic, ಖಾಲಿ);
	ಆದರೆ (mnemonic != '\0')
	{
	    mneString[0] = mnemonic;
	    mneString[1] = '\0';
	    Xt_Grab_ಕೀಲಿ(dialog, XKeysym_ಗಮ್ಯ_Keycode(XtDisplay(dialog),
						  XString_ಗಮ್ಯ_Keysym(mneString)),
		    Mod1_ಮುಖವಾಡ, True, Grab_ಬಹುಲಕ_Async, Grab_ಬಹುಲಕ_Async);
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
    ಆದರೆ (!dialog)
	ಹಿಂತಿರುಗಿ;

    Xt_ಕೂಡಿಸು_Event_Handler(dialog, ಕೀಲಿ_Press_ಮುಖವಾಡ, False,
			   (XtEventHandler) mnemonic_event, (Xt_ಸೂಚಕ) ಖಾಲಿ);
    ಕೂಡಿಸು_mnemonic_grabs(dialog, dialog);
}

/*
 * Removes the event handler and key-grabs for dialog mnemonic handling.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
suppress_dialog_mnemonics(Widget dialog)
{
    ಆದರೆ (!dialog)
	ಹಿಂತಿರುಗಿ;

    Xt_Ungrab_ಕೀಲಿ(dialog, Any_ಕೀಲಿ, Mod1_ಮುಖವಾಡ);
    XtRemoveEventHandler(dialog, ಕೀಲಿ_Press_ಮುಖವಾಡ, False,
			   (XtEventHandler) mnemonic_event, (Xt_ಸೂಚಕ) ಖಾಲಿ);
}

#ಆದರೆ defined(FEAT_BROWSE) || defined(FEAT_GUI_DIALOG)
/*
 * Use the 'guifont' or 'guifontset' as a fontlist for a dialog widget.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_fontlist(Widget id)
{
    XmFontList fl;

# ifdef FONTSET_ALWAYS
    ಆದರೆ (gui.fontset != NOFONTSET)
    {
	fl = gui_motif_fontset2fontlist((XFont_ಗಣ *)&gui.fontset);
	ಆದರೆ (fl != ಖಾಲಿ)
	{
	    ಆದರೆ (XtIsManaged(id))
	    {
		Xt_Unmanage_ಮಗು(id);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ಮಗು(id);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
	    XmFontListFree(fl);
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (gui.norm_font != NOFONT)
    {
	fl = gui_motif_create_fontlist((XFontStruct *)gui.norm_font);
	ಆದರೆ (fl != ಖಾಲಿ)
	{
	    ಆದರೆ (XtIsManaged(id))
	    {
		Xt_Unmanage_ಮಗು(id);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ಮಗು(id);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
	    XmFontListFree(fl);
	}
    }
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BROWSE)

/*
 * file selector related stuff
 */

# include <Xm/FileSB.h>
# include <Xm/XmStrDefs.h>

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ dialog_callback_arg
{
    ಅಕ್ಷರ *  ವಾದಗಳು;   // not used right now
    ಪೂರ್ಣಾಂಕ	    id;
} dcbarg_T;

ಸ್ಥಿರ Widget dialog_wgt;
ಸ್ಥಿರ ಅಕ್ಷರ *browse_fname = ಖಾಲಿ;
ಸ್ಥಿರ Xm_String_Char_ಗಣ charset = (Xm_String_Char_ಗಣ) XmSTRING_DEFAULT_CHARSET;
				// used to set up XmStrings

ಸ್ಥಿರ ಶೂನ್ಯ DialogCancelCB(Widget, Xt_ಸೂಚಕ, Xt_ಸೂಚಕ);
ಸ್ಥಿರ ಶೂನ್ಯ DialogAcceptCB(Widget, Xt_ಸೂಚಕ, Xt_ಸೂಚಕ);

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
ಗಣ_predefined_label(Widget ಪೋಷಕ, String name, ಅಕ್ಷರ *new_label)
{
    XmString	str;
    Widget	w;
    char_u	*p, *ಮುಂದಿನ;
    ಕೀಲಿ_Sym	mnemonic = NUL;

    w = Xt_Name_ಗಮ್ಯ_Widget(ಪೋಷಕ, name);

    ಆದರೆ (!w)
	ಹಿಂತಿರುಗಿ;

    p = vim_strsave((char_u *)new_label);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (ಮುಂದಿನ = p; *ಮುಂದಿನ; ++ಮುಂದಿನ)
    {
	ಆದರೆ (*ಮುಂದಿನ == DLG_HOTKEY_CHAR)
	{
	    ಪೂರ್ಣಾಂಕ len = STRLEN(ಮುಂದಿನ);

	    ಆದರೆ (len > 0)
	    {
		mch_memmove(ಮುಂದಿನ, ಮುಂದಿನ + 1, len);
		mnemonic = ಮುಂದಿನ[0];
	    }
	}
    }

    str = XmStringCreate((ಅಕ್ಷರ *)p, STRING_ಟ್ಯಾಗ್);
    vim_free(p);

    ಆದರೆ (str != ಖಾಲಿ)
    {
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ಖಾಲಿ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_predefined_fontlist(Widget ಪೋಷಕ, String name)
{
    Widget w;
    w = Xt_Name_ಗಮ್ಯ_Widget(ಪೋಷಕ, name);

    ಆದರೆ (!w)
	ಹಿಂತಿರುಗಿ;

    ಗಣ_fontlist(w);
}

/*
 * Put up a file requester.
 * Returns the selected name in allocated memory, or NULL for Cancel.
 */
    char_u *
gui_mch_browse(
    ಪೂರ್ಣಾಂಕ		saving ಬಳಸದ,	// select file to write
    char_u	*title,		// title for the window
    char_u	*dflt,		// default name
    char_u	*ext ಬಳಸದ,	// not used (extension added)
    char_u	*initdir,	// initial directory, NULL for current dir
    char_u	*filter)	// file name filter
{
    char_u	dirbuf[MAXPATHL];
    char_u	dfltbuf[MAXPATHL];
    char_u	*ನಮೂನೆ;
    char_u	*tofree = ಖಾಲಿ;

    // There a difference between the resource name and value, Therefore, we
    // avoid to (ab-)use the (maybe internationalized!) dialog title as a
    // dialog name.

    dialog_wgt = XmCreateFileSelectionDialog(vimShell, "browseDialog", ಖಾಲಿ, 0);

    ಆದರೆ (initdir == ಖಾಲಿ || *initdir == NUL)
    {
	mch_dirname(dirbuf, MAXPATHL);
	initdir = dirbuf;
    }

    ಆದರೆ (dflt == ಖಾಲಿ)
	dflt = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRLEN(initdir) + STRLEN(dflt) + 2 < MAXPATHL)
    {
	// The default selection should be the full path, "dflt" is only the
	// file name.
	STRCPY(dfltbuf, initdir);
	ಕೂಡಿಸು_pathsep(dfltbuf);
	STRCAT(dfltbuf, dflt);
	dflt = dfltbuf;
    }

    // Can only use one pattern for a file name.  Get the first pattern out of
    // the filter.  An empty pattern means everything matches.
    ಆದರೆ (filter == ಖಾಲಿ)
	ನಮೂನೆ = (char_u *)"";
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u	*s, *p;

	s = filter;
	ಗಾಗಿ (p = filter; *p != NUL; ++p)
	{
	    ಆದರೆ (*p == '\t')	// end of description, start of pattern
		s = p + 1;
	    ಆದರೆ (*p == ';' || *p == '\n')	// end of (first) pattern
		ನಿಲ್ಲಿಸು;
	}
	ನಮೂನೆ = vim_strnsave(s, p - s);
	tofree = ನಮೂನೆ;
	ಆದರೆ (ನಮೂನೆ == ಖಾಲಿ)
	    ನಮೂನೆ = (char_u *)"";
    }

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialog_wgt,
	XtVaTypedArg,
	    XmNdirectory, XmRString, (ಅಕ್ಷರ *)initdir, STRLEN(initdir) + 1,
	XtVaTypedArg,
	    XmNdirSpec,	XmRString, (ಅಕ್ಷರ *)dflt, STRLEN(dflt) + 1,
	XtVaTypedArg,
	    XmNpattern,	XmRString, (ಅಕ್ಷರ *)ನಮೂನೆ, STRLEN(ನಮೂನೆ) + 1,
	XtVaTypedArg,
	    XmNdialogTitle, XmRString, (ಅಕ್ಷರ *)title, STRLEN(title) + 1,
	ಖಾಲಿ);

    ಗಣ_predefined_label(dialog_wgt, "Apply", _("&Filter"));
    ಗಣ_predefined_label(dialog_wgt, "Cancel", _("&Cancel"));
    ಗಣ_predefined_label(dialog_wgt, "Dir", _("Directories"));
    ಗಣ_predefined_label(dialog_wgt, "FilterLabel", _("Filter"));
    ಗಣ_predefined_label(dialog_wgt, "Help", _("&Help"));
    ಗಣ_predefined_label(dialog_wgt, "Items", _("Files"));
    ಗಣ_predefined_label(dialog_wgt, "OK", _("&OK"));
    ಗಣ_predefined_label(dialog_wgt, "Selection", _("Selection"));

    // This is to save us from silly external settings using not fixed with
    // fonts for file selection.
    ಗಣ_predefined_fontlist(dialog_wgt, "DirListSW.DirList");
    ಗಣ_predefined_fontlist(dialog_wgt, "ItemsListSW.ItemsList");

    gui_motif_menu_colors(dialog_wgt);
    ಆದರೆ (gui.scroll_bg_pixel != INVALCOLOR)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialog_wgt, XmNtroughColor, gui.scroll_bg_pixel, ಖಾಲಿ);

    Xt_ಕೂಡಿಸು_Callback(dialog_wgt, XmNokCallback, DialogAcceptCB, (Xt_ಸೂಚಕ)0);
    Xt_ಕೂಡಿಸು_Callback(dialog_wgt, XmNcancelCallback, DialogCancelCB, (Xt_ಸೂಚಕ)0);
    // We have no help in this window, so hide help button
    Xt_Unmanage_ಮಗು(Xm_File_Selection_Box_Get_ಮಗು(dialog_wgt,
					(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)XmDIALOG_HELP_BUTTON));

    manage_centered(dialog_wgt);
    activate_dialog_mnemonics(dialog_wgt);

    // sit in a loop until the dialog box has gone away
    ಮಾಡು
    {
	XtAppProcessEvent(Xt_Widget_ಗಮ್ಯ_Application_Context(dialog_wgt),
	    (Xt_ಒಳಹರಿವು_ಮುಖವಾಡ)XtIMAll);
    } ಆಗ (XtIsManaged(dialog_wgt));

    suppress_dialog_mnemonics(dialog_wgt);
    XtDestroyWidget(dialog_wgt);
    vim_free(tofree);

    ಆದರೆ (browse_fname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strsave((char_u *)browse_fname);
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
    Widget	w ಬಳಸದ,		//  widget id
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,	//  data from application
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)	//  data from widget class
{
    ಆದರೆ (browse_fname != ಖಾಲಿ)
    {
	XtFree(browse_fname);
	browse_fname = ಖಾಲಿ;
    }
    Xt_Unmanage_ಮಗು(dialog_wgt);
}

/*
 * Process callback from Dialog actions.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
DialogAcceptCB(
    Widget	w ಬಳಸದ,		//  widget id
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,	//  data from application
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ)		//  data from widget class
{
    XmFileSelectionBoxCallbackStruct *fcb;

    ಆದರೆ (browse_fname != ಖಾಲಿ)
    {
	XtFree(browse_fname);
	browse_fname = ಖಾಲಿ;
    }
    fcb = (XmFileSelectionBoxCallbackStruct *)call_ದತ್ತಾಂಶ;

    // get the filename from the file selection box
    XmStringGetLtoR(fcb->ಮೌಲ್ಯ, charset, &browse_fname);

    // popdown the file selection box
    Xt_Unmanage_ಮಗು(dialog_wgt);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_BROWSE

#ಆದರೆ defined(FEAT_GUI_DIALOG)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	dialog_ಸ್ಥಿತಿ;

/*
 * Callback function for the textfield.  When CR is hit this works like
 * hitting the "OK" button, ESC like "Cancel".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
keyhit_callback(
    Widget		w,
    Xt_ಸೂಚಕ		client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    XEvent		*event,
    Boolean		*cont ಬಳಸದ)
{
    ಅಕ್ಷರ	buf[2];
    ಕೀಲಿ_Sym	ಕೀಲಿ_sym;

    ಆದರೆ (XLookupString(&(event->xkey), buf, 2, &ಕೀಲಿ_sym, ಖಾಲಿ) == 1)
    {
	ಆದರೆ (*buf == CAR)
	    dialog_ಸ್ಥಿತಿ = 1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*buf == ESC)
	    dialog_ಸ್ಥಿತಿ = 2;
    }
    ಆದರೆ ((ಕೀಲಿ_sym == XK_ಎಡ || ಕೀಲಿ_sym == XK_ಬಲ)
	    && !(event->xkey.state & ಸರಿಸು_ಮುಖವಾಡ))
	Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Clear_Selection(w, Xt_ಕೊನೆಯ_Timestamp_Processed(gui.dpy));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
butproc(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    dialog_ಸ್ಥಿತಿ = (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)client_ದತ್ತಾಂಶ_2 + 1;
}

# ifdef HAVE_XPM

    ಸ್ಥಿರ Widget
create_pixmap_label(
    Widget	ಪೋಷಕ,
    String	name,
    ಅಕ್ಷರ	**ದತ್ತಾಂಶ,
    ArgList	ವಾದಗಳು,
    Cardinal	arg)
{
    Widget		label;
    Display		*dsp;
    Screen		*scr;
    ಪೂರ್ಣಾಂಕ			ಆಳ;
    Pixmap		pixmap = 0;
    Xpm_ಗುಣಲಕ್ಷಣಗಳು	attr;
    Boolean		rs;
    Xpm_Color_ಚಿಹ್ನೆ	color[5] =
    {
	{"none", ಖಾಲಿ, 0},
	{"iconColor1", ಖಾಲಿ, 0},
	{"bottomShadowColor", ಖಾಲಿ, 0},
	{"topShadowColor", ಖಾಲಿ, 0},
	{"selectColor", ಖಾಲಿ, 0}
    };

    label = XmCreateLabelGadget(ಪೋಷಕ, name, ವಾದಗಳು, arg);

    /*
     * We need to be careful here, since in case of gadgets, there is
     * no way to get the background color directly from the widget itself.
     * In such cases we get it from The Core part of his parent instead.
     */
    dsp = Xt_Display_ದ_Object(label);
    scr = Xt_Screen_ದ_Object(label);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(XtIsSubclass(label, coreWidgetClass)
	    ?  label : Xt_ಪೋಷಕ(label),
		  XmNdepth, &ಆಳ,
		  XmNbackground, &color[0].pixel,
		  XmNforeground, &color[1].pixel,
		  XmNbottomShadowColor, &color[2].pixel,
		  XmNtopShadowColor, &color[3].pixel,
		  XmNhighlight, &color[4].pixel,
		  ಖಾಲಿ);

    attr.valuemask = XpmColorSymbols | XpmCloseness | Xpm_ಆಳ;
    attr.colorsymbols = color;
    attr.numsymbols = 5;
    attr.closeness = 65535;
    attr.ಆಳ = ಆಳ;
    Xpm_Create_Pixmap_From_ದತ್ತಾಂಶ(dsp, ಬೇರು_ಕಿಟಕಿ_ದ_Screen(scr),
		    ದತ್ತಾಂಶ, &pixmap, ಖಾಲಿ, &attr);

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(label, Xm_Nrecompute_ಗಾತ್ರ, &rs, ಖಾಲಿ);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(label, Xm_Nrecompute_ಗಾತ್ರ, True, ಖಾಲಿ);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(label,
	    XmNlabelType, XmPIXMAP,
	    XmNlabelPixmap, pixmap,
	    ಖಾಲಿ);
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(label, Xm_Nrecompute_ಗಾತ್ರ, rs, ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ label;
}
# endif

    ಪೂರ್ಣಾಂಕ
gui_mch_dialog(
    ಪೂರ್ಣಾಂಕ		type ಬಳಸದ,
    char_u	*title,
    char_u	*ಸಂದೇಶ_2,
    char_u	*button_ಹೆಸರುಗಳು,
    ಪೂರ್ಣಾಂಕ		dfltbutton,
    char_u	*textfield,		// buffer of size IOSIZE
    ಪೂರ್ಣಾಂಕ		ex_cmd ಬಳಸದ)
{
    char_u		*buts;
    char_u		*p, *ಮುಂದಿನ;
    XtAppContext	app;
    XmString		label;
    ಪೂರ್ಣಾಂಕ			butcount;
    Widget		w;
    Widget		dialogform = ಖಾಲಿ;
    Widget		form = ಖಾಲಿ;
    Widget		dialogtextfield = ಖಾಲಿ;
    Widget		*buttons;
    Widget		sep_form = ಖಾಲಿ;
    Boolean		vertical;
    Widget		separator = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			n;
    Arg			ವಾದಗಳು[6];
# ifdef HAVE_XPM
    ಅಕ್ಷರ		**icon_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;
    Widget		dialogpixmap = ಖಾಲಿ;
# endif

    ಆದರೆ (title == ಖಾಲಿ)
	title = (char_u *)_("Vim dialog");

    // if our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    dialogform = XmCreateFormDialog(vimShell, (ಅಕ್ಷರ *)"dialog", ಖಾಲಿ, 0);

    // Check 'v' flag in 'guioptions': vertical button placement.
    vertical = (vim_strchr(p_go, GO_VERTICAL) != ಖಾಲಿ);

    // Set the title of the Dialog window
    label = XmStringCreateSimple((ಅಕ್ಷರ *)title);
    ಆದರೆ (label == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogform,
	    XmNdialogTitle, label,
	    XmNhorizontalSpacing, 4,
	    XmNverticalSpacing, vertical ? 0 : 4,
	    ಖಾಲಿ);
    XmStringFree(label);

    // make a copy, so that we can insert NULs
    buts = vim_strsave(button_ಹೆಸರುಗಳು);
    ಆದರೆ (buts == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    // Count the number of buttons and allocate buttons[].
    butcount = 1;
    ಗಾಗಿ (p = buts; *p; ++p)
	ಆದರೆ (*p == DLG_BUTTON_SEP)
	    ++butcount;
    buttons = ALLOC_MULT(Widget, butcount);
    ಆದರೆ (buttons == ಖಾಲಿ)
    {
	vim_free(buts);
	ಹಿಂತಿರುಗಿ -1;
    }

    /*
     * Create the buttons.
     */
    sep_form = (Widget) 0;
    p = buts;
    ಗಾಗಿ (butcount = 0; *p; ++butcount)
    {
	ಕೀಲಿ_Sym mnemonic = NUL;

	ಗಾಗಿ (ಮುಂದಿನ = p; *ಮುಂದಿನ; ++ಮುಂದಿನ)
	{
	    ಆದರೆ (*ಮುಂದಿನ == DLG_HOTKEY_CHAR)
	    {
		ಪೂರ್ಣಾಂಕ len = STRLEN(ಮುಂದಿನ);

		ಆದರೆ (len > 0)
		{
		    mch_memmove(ಮುಂದಿನ, ಮುಂದಿನ + 1, len);
		    mnemonic = ಮುಂದಿನ[0];
		}
	    }
	    ಆದರೆ (*ಮುಂದಿನ == DLG_BUTTON_SEP)
	    {
		*ಮುಂದಿನ++ = NUL;
		ನಿಲ್ಲಿಸು;
	    }
	}
	label = XmStringCreate(_((ಅಕ್ಷರ *)p), STRING_ಟ್ಯಾಗ್);
	ಆದರೆ (label == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	buttons[butcount] = XtVaCreateManagedWidget("button",
		xm_ತಳ್ಳು_Button_Widget_Class, dialogform,
		XmNlabelString, label,
		XmNmnemonic, mnemonic,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
		XmNshowAsDefault, butcount == dfltbutton - 1,
		XmNdefaultButtonShadowThickness, 1,
		ಖಾಲಿ);
	XmStringFree(label);
	gui_motif_menu_fontlist(buttons[butcount]);

	// Layout properly.

	ಆದರೆ (butcount > 0)
	{
	    ಆದರೆ (vertical)
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(buttons[butcount],
			XmNtopWidget, buttons[butcount - 1],
			ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (*ಮುಂದಿನ == NUL)
		{
		    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(buttons[butcount],
			    XmNrightAttachment, XmATTACH_FORM,
			    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
			    ಖಾಲಿ);

		    // fill in a form as invisible separator
		    sep_form = XtVaCreateWidget("separatorForm",
			    xmFormWidgetClass,	dialogform,
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    XmNrightAttachment, XmATTACH_WIDGET,
			    XmNrightWidget, buttons[butcount],
			    XmNbottomAttachment, XmATTACH_FORM,
			    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
			    ಖಾಲಿ);
		    Xt_Manage_ಮಗು(sep_form);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(buttons[butcount],
			    XmNleftAttachment, XmATTACH_WIDGET,
			    XmNleftWidget, buttons[butcount - 1],
			    ಖಾಲಿ);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!vertical)
	{
	    ಆದರೆ (*ಮುಂದಿನ == NUL)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(buttons[0],
			XmNrightAttachment, XmATTACH_FORM,
			Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
			ಖಾಲಿ);

		// fill in a form as invisible separator
		sep_form = XtVaCreateWidget("separatorForm",
			xmFormWidgetClass, dialogform,
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
			XmNrightAttachment, XmATTACH_WIDGET,
			XmNrightWidget, buttons[0],
			XmNbottomAttachment, XmATTACH_FORM,
			Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
			ಖಾಲಿ);
		Xt_Manage_ಮಗು(sep_form);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(buttons[0],
			XmNleftAttachment, XmATTACH_FORM,
			Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
			ಖಾಲಿ);
	}

	Xt_ಕೂಡಿಸು_Callback(buttons[butcount], XmNactivateCallback,
			  (XtCallbackProc)butproc, (Xt_ಸೂಚಕ)(ದೀರ್ಘ)butcount);
	p = ಮುಂದಿನ;
    }
    vim_free(buts);

    separator = (Widget) 0;
    ಆದರೆ (butcount > 0)
    {
	// Create the separator for beauty.
	n = 0;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation, XmHORIZONTAL); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_WIDGET); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomWidget, buttons[0]); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); n++;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_FORM); n++;
	separator = XmCreateSeparatorGadget(dialogform, "separator", ವಾದಗಳು, n);
	Xt_Manage_ಮಗು(separator);
    }

    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	dialogtextfield = XtVaCreateWidget("textField",
		xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Widget_Class, dialogform,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ಖಾಲಿ);
	ಆದರೆ (butcount > 0)
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogtextfield,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ಖಾಲಿ);

	ಗಣ_fontlist(dialogtextfield);
	Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_String(dialogtextfield, (ಅಕ್ಷರ *)textfield);
	Xt_Manage_ಮಗು(dialogtextfield);
	Xt_ಕೂಡಿಸು_Event_Handler(dialogtextfield, ಕೀಲಿ_Press_ಮುಖವಾಡ, False,
			    (XtEventHandler)keyhit_callback, (Xt_ಸೂಚಕ)ಖಾಲಿ);
    }

    // Form holding both message and pixmap labels
    form = XtVaCreateWidget("separatorForm",
	    xmFormWidgetClass, dialogform,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNtopAttachment, XmATTACH_FORM,
	    ಖಾಲಿ);
    Xt_Manage_ಮಗು(form);

# ifdef HAVE_XPM
    // Add a pixmap, left of the message.
    ಆಯ್ಕೆ (type)
    {
	ಸ್ಥಿತಿ VIM_GENERIC:
	    icon_ದತ್ತಾಂಶ_2 = generic_xpm;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_ERROR:
	    icon_ದತ್ತಾಂಶ_2 = error_xpm;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_WARNING:
	    icon_ದತ್ತಾಂಶ_2 = alert_xpm;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_INFO:
	    icon_ದತ್ತಾಂಶ_2 = info_xpm;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VIM_QUESTION:
	    icon_ದತ್ತಾಂಶ_2 = quest_xpm;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    icon_ದತ್ತಾಂಶ_2 = generic_xpm;
    }

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Ntop_ಸ್ಥಾನಾಂತರ, 8); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 8); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNleftAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nleft_ಸ್ಥಾನಾಂತರ, 8); n++;

    dialogpixmap = create_pixmap_label(form, "dialogPixmap",
	    icon_ದತ್ತಾಂಶ_2, ವಾದಗಳು, n);
    Xt_Manage_ಮಗು(dialogpixmap);
# endif

    // Create the dialog message.
    // Since LessTif is apparently having problems with the creation of
    // properly localized string, we use LtoR here. The symptom is that the
    // string is not shown properly in multiple lines as it does in native
    // Motif.
    label = XmStringCreateLtoR((ಅಕ್ಷರ *)ಸಂದೇಶ_2, STRING_ಟ್ಯಾಗ್);
    ಆದರೆ (label == ಖಾಲಿ)
    {
	vim_free(buttons);
	ಹಿಂತಿರುಗಿ -1;
    }
    w = XtVaCreateManagedWidget("dialogMessage",
				xmLabelGadgetClass, form,
				XmNlabelString, label,
				XmNalignment, XmALIGNMENT_BEGINNING,
				XmNtopAttachment, XmATTACH_FORM,
				Xm_Ntop_ಸ್ಥಾನಾಂತರ, 8,
# ifdef HAVE_XPM
				XmNleftAttachment, XmATTACH_WIDGET,
				XmNleftWidget, dialogpixmap,
# ಇಲ್ಲದಿದ್ದರೆ
				XmNleftAttachment, XmATTACH_FORM,
# endif
				Xm_Nleft_ಸ್ಥಾನಾಂತರ, 8,
				XmNrightAttachment, XmATTACH_FORM,
				Xm_Nright_ಸ್ಥಾನಾಂತರ, 8,
				XmNbottomAttachment, XmATTACH_FORM,
				Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 8,
				ಖಾಲಿ);
    XmStringFree(label);
    ಗಣ_fontlist(w);

    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(form,
		XmNbottomAttachment, XmATTACH_WIDGET,
		XmNbottomWidget, dialogtextfield,
		ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (butcount > 0)
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(form,
		    XmNbottomAttachment, XmATTACH_WIDGET,
		    XmNbottomWidget, separator,
		    ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(form,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ಖಾಲಿ);
    }

    ಆದರೆ (dfltbutton < 1)
	dfltbutton = 1;
    ಆದರೆ (dfltbutton > butcount)
	dfltbutton = butcount;
    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogform,
	    XmNdefaultButton, buttons[dfltbutton - 1], ಖಾಲಿ);
    ಆದರೆ (textfield != ಖಾಲಿ)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogform, XmNinitialFocus, dialogtextfield, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(dialogform, XmNinitialFocus, buttons[dfltbutton - 1],
									ಖಾಲಿ);

    manage_centered(dialogform);
    activate_dialog_mnemonics(dialogform);

    ಆದರೆ (textfield != ಖಾಲಿ && *textfield != NUL)
    {
	// This only works after the textfield has been realised.
	Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_Selection(dialogtextfield,
			 (Xm_ಪಠ್ಯ_Position)0, (Xm_ಪಠ್ಯ_Position)STRLEN(textfield),
					   Xt_ಕೊನೆಯ_Timestamp_Processed(gui.dpy));
	Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_Cursor_Position(dialogtextfield,
					   (Xm_ಪಠ್ಯ_Position)STRLEN(textfield));
    }

    app = Xt_Widget_ಗಮ್ಯ_Application_Context(dialogform);

    // Loop until a button is pressed or the dialog is killed somehow.
    dialog_ಸ್ಥಿತಿ = -1;
    ಗಾಗಿ (;;)
    {
	XtAppProcessEvent(app, (Xt_ಒಳಹರಿವು_ಮುಖವಾಡ)XtIMAll);
	ಆದರೆ (dialog_ಸ್ಥಿತಿ >= 0 || !XtIsManaged(dialogform))
	    ನಿಲ್ಲಿಸು;
    }

    vim_free(buttons);

    ಆದರೆ (textfield != ಖಾಲಿ)
    {
	p = (char_u *)Xm_ಪಠ್ಯ_Get_String(dialogtextfield);
	ಆದರೆ (p == ಖಾಲಿ || dialog_ಸ್ಥಿತಿ < 0)
	    *textfield = NUL;
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_strncpy(textfield, p, IOSIZE - 1);
	XtFree((ಅಕ್ಷರ *)p);
    }

    suppress_dialog_mnemonics(dialogform);
    XtDestroyWidget(dialogform);

    ಹಿಂತಿರುಗಿ dialog_ಸ್ಥಿತಿ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DIALOG

#ಆದರೆ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    Cardinal	ಅಂಕೆ_Children;	    // how many children toolBar has

    ಆದರೆ (toolBar == (Widget)0)
	ಹಿಂತಿರುಗಿ;
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNnumChildren, &ಅಂಕೆ_Children, ಖಾಲಿ);
    ಆದರೆ (showit && ಅಂಕೆ_Children > 0)
    {
	// Assume that we want to show the toolbar if p_toolbar contains
	// valid option settings, therefore p_toolbar must not be NULL.
	WidgetList  children;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar, XmNchildren, &children, ಖಾಲಿ);
	{
	    ಶೂನ್ಯ    (*action)(BalloonEval *);
	    ಪೂರ್ಣಾಂಕ	    ಪಠ್ಯ = 0;

	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)p_toolbar, "tooltips"))
		action = &gui_mch_enable_beval_area;
	    ಇಲ್ಲದಿದ್ದರೆ
		action = &gui_mch_disable_beval_area;
	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)p_toolbar, "text"))
		ಪಠ್ಯ = 1;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)p_toolbar, "icons"))
		ಪಠ್ಯ = -1;
	    ಆದರೆ (ಪಠ್ಯ != 0)
	    {
		vimmenu_T   *toolbar;
		vimmenu_T   *cur;

		FOR_ALL_MENUS(toolbar)
		    ಆದರೆ (menu_is_toolbar(toolbar->dname))
			ನಿಲ್ಲಿಸು;
		// Assumption: toolbar is NULL if there is no toolbar,
		//	       otherwise it contains the toolbar menu structure.
		//
		// Assumption: "numChildren" == the number of items in the list
		//	       of items beginning with toolbar->children.
		ಆದರೆ (toolbar)
		{
		    ಗಾಗಿ (cur = toolbar->children; cur; cur = cur->ಮುಂದಿನ)
		    {
			Arg	    ವಾದಗಳು[1];
			ಪೂರ್ಣಾಂಕ	    n = 0;

			// Enable/Disable tooltip (OK to enable while
			// currently enabled).
			ಆದರೆ (cur->tip != ಖಾಲಿ)
			    (*action)(cur->tip);
			ಆದರೆ (!menu_is_separator(cur->name))
			{
			    ಆದರೆ (ಪಠ್ಯ == 1 || cur->xpm == ಖಾಲಿ)
			    {
				Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNlabelType, XmSTRING);
				++n;
			    }
			    ಆದರೆ (cur->id != ಖಾಲಿ)
			    {
				Xt_Unmanage_ಮಗು(cur->id);
				Xt_ಗಣ_ಮೌಲ್ಯಗಳು(cur->id, ವಾದಗಳು, n);
				Xt_Manage_ಮಗು(cur->id);
			    }
			}
		    }
		}
	    }
	}
	gui.toolbar_ಎತ್ತರ = gui_mch_compute_toolbar_ಎತ್ತರ();
	Xt_Manage_ಮಗು(Xt_ಪೋಷಕ(toolBar));
# ifdef FEAT_GUI_TABLINE
	ಆದರೆ (showing_tabline)
	{
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			  ಖಾಲಿ);
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ಸಾಲು,
			  ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, Xt_ಪೋಷಕ(toolBar),
			  ಖಾಲಿ);
	ಆದರೆ (XtIsManaged(menuBar))
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(Xt_ಪೋಷಕ(toolBar),
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, menuBar,
		    ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(Xt_ಪೋಷಕ(toolBar),
		    XmNtopAttachment, XmATTACH_FORM,
		    ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui.toolbar_ಎತ್ತರ = 0;
	ಆದರೆ (XtIsManaged(menuBar))
	{
# ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar,
			      ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
# ifdef FEAT_GUI_TABLINE
	    ಆದರೆ (showing_tabline)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM,
			      ಖಾಲಿ);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, tab_ಸಾಲು,
			      ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM,
			      ಖಾಲಿ);
	}

	Xt_Unmanage_ಮಗು(Xt_ಪೋಷಕ(toolBar));
    }
    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_VERT);
}

/*
 * A toolbar button has been pushed; now reset the input focus
 * such that the user can type page up/down etc. and have the
 * input go to the editor window, not the button
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
reset_focus(ಶೂನ್ಯ)
{
    ಆದರೆ (ಪಠ್ಯ_Area != ಖಾಲಿ)
	XmProcessTraversal(ಪಠ್ಯ_Area, XmTRAVERSE_CURRENT);
}

    ಪೂರ್ಣಾಂಕ
gui_mch_compute_toolbar_ಎತ್ತರ(ಶೂನ್ಯ)
{
    Dimension	borders;
    Dimension	ಎತ್ತರ;		    // total Toolbar height
    Dimension	whgt;		    // height of each widget
    WidgetList	children;	    // list of toolBar's children
    Cardinal	ಅಂಕೆ_Children;	    // how many children toolBar has
    ಪೂರ್ಣಾಂಕ		i;

    borders = 0;
    ಎತ್ತರ = 0;
    ಆದರೆ (toolBar != (Widget)0 && tool_Bar_ಚೌಕಟ್ಟು != (Widget)0)
    {				    // get height of XmFrame parent
	Dimension	fst;
	Dimension	fmh;
	Dimension	tst;
	Dimension	tmh;

	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tool_Bar_ಚೌಕಟ್ಟು,
		XmNshadowThickness, &fst,
		Xm_Nmargin_ಎತ್ತರ, &fmh,
		ಖಾಲಿ);
	borders += fst + fmh;
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(toolBar,
		XmNshadowThickness, &tst,
		Xm_Nmargin_ಎತ್ತರ, &tmh,
		XmNchildren, &children,
		XmNnumChildren, &ಅಂಕೆ_Children, ಖಾಲಿ);
	borders += tst + tmh;
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)ಅಂಕೆ_Children; i++)
	{
	    whgt = 0;
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(children[i], XmNheight, &whgt, ಖಾಲಿ);
	    ಆದರೆ (ಎತ್ತರ < whgt)
		ಎತ್ತರ = whgt;
	}
    }
# ifdef LESSTIF_ಆವೃತ್ತಿ
    // Hack: When starting up we get wrong dimensions.
    ಆದರೆ (ಎತ್ತರ < 10)
	ಎತ್ತರ = 24;
# endif

    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(ಎತ್ತರ + (borders << 1));
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
	    ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_TABLINE)
/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (tab_ಸಾಲು == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!showit != !showing_tabline)
    {
	ಆದರೆ (showit)
	{
	    Xt_Manage_ಮಗು(tab_ಸಾಲು);
	    Xt_Unmanage_ಮಗು(Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು, "PageScroller"));
	    Xt_Unmanage_ಮಗು(Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು, "MinorTabScrollerNext"));
	    Xt_Unmanage_ಮಗು(Xt_Name_ಗಮ್ಯ_Widget(tab_ಸಾಲು,
					   "MinorTabScrollerPrevious"));
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ಆದರೆ (XtIsManaged(Xt_ಪೋಷಕ(toolBar)))
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar), ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		ಆದರೆ (XtIsManaged(menuBar))
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು,
			      XmNtopAttachment, XmATTACH_FORM, ಖಾಲಿ);
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			  XmNtopAttachment, XmATTACH_WIDGET,
			  XmNtopWidget, tab_ಸಾಲು,
			  ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    Xt_Unmanage_ಮಗು(tab_ಸಾಲು);
# ifdef FEAT_MENU
#  ifdef FEAT_TOOLBAR
	    ಆದರೆ (XtIsManaged(Xt_ಪೋಷಕ(toolBar)))
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, Xt_ಪೋಷಕ(toolBar), ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
		ಆದರೆ (XtIsManaged(menuBar))
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_WIDGET,
			      XmNtopWidget, menuBar, ಖಾಲಿ);
	    ಇಲ್ಲದಿದ್ದರೆ
# endif
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(ಪಠ್ಯ_Area_Form,
			      XmNtopAttachment, XmATTACH_FORM, ಖಾಲಿ);
	}
	showing_tabline = showit;
    }
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ tab_ಸಾಲು != (Widget)0 && showing_tabline;
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    tabpage_T		*tp;
    ಪೂರ್ಣಾಂಕ			nr = 1, n;
    Arg			ವಾದಗಳು[10];
    ಪೂರ್ಣಾಂಕ			curtabidx = 0, currentpage;
    Widget		tab;
    Xm_Notebook_ಪುಟ_Info	ಪುಟ_info;
    Xm_Notebook_ಪುಟ_ಸ್ಥಿತಿ ಪುಟ_ಸ್ಥಿತಿ;
    ಪೂರ್ಣಾಂಕ			ಕೊನೆಯ_ಪುಟ, tab_ಎಣಿಕೆ;
    XmString		label_str;
    ಅಕ್ಷರ		*label_cstr;
    BalloonEval		*beval;

    ಆದರೆ (tab_ಸಾಲು == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    // Add a label for each tab page.  They all contain the same text area.
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, ++nr)
    {
	ಆದರೆ (tp == curtab)
	    curtabidx = nr;

	ಪುಟ_ಸ್ಥಿತಿ = Xm_Notebook_Get_ಪುಟ_Info(tab_ಸಾಲು, nr, &ಪುಟ_info);
	ಆದರೆ (ಪುಟ_ಸ್ಥಿತಿ == Xm_ಪುಟ_INVALID
		|| ಪುಟ_info.major_tab_widget == (Widget)0)
	{
	    // Add the tab
	    n = 0;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nnotebook_ಮಗು_Type, XmMAJOR_TAB); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtraversalOn, False); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNalignment, XmALIGNMENT_BEGINNING); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNhighlightThickness, 1); n++;
	    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNshadowThickness , 1); n++;
	    tab = Xm_Create_ತಳ್ಳು_Button(tab_ಸಾಲು, "-Empty-", ವಾದಗಳು, n);
	    Xt_Manage_ಮಗು(tab);
	    beval = gui_mch_create_beval_area(tab, ಖಾಲಿ, tabline_balloon_cb,
									ಖಾಲಿ);
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab, Xm_Nuser_ದತ್ತಾಂಶ, beval, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    tab = ಪುಟ_info.major_tab_widget;

	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab, Xm_Npage_ಸಂಖ್ಯೆ, nr, ಖಾಲಿ);

	/*
	 * Change the label text only if it is different
	 */
	Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab, XmNlabelString, &label_str, ಖಾಲಿ);
	ಆದರೆ (XmStringGetLtoR(label_str, XmSTRING_DEFAULT_CHARSET, &label_cstr))
	{
	    get_tabline_label(tp, FALSE);
	    ಆದರೆ (STRCMP(label_cstr, NameBuff) != 0)
	    {
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab, XtVaTypedArg, XmNlabelString, XmRString,
			      NameBuff, STRLEN(NameBuff) + 1, ಖಾಲಿ);
		/*
		 * Force a resize of the tab label button
		 */
		Xt_Unmanage_ಮಗು(tab);
		Xt_Manage_ಮಗು(tab);
	    }
	    XtFree(label_cstr);
	}
    }

    tab_ಎಣಿಕೆ = nr - 1;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Nlast_ಪುಟ_ಸಂಖ್ಯೆ, &ಕೊನೆಯ_ಪುಟ, ಖಾಲಿ);

    // Remove any old labels.
    ಆಗ (nr <= ಕೊನೆಯ_ಪುಟ)
    {
	ಆದರೆ (Xm_Notebook_Get_ಪುಟ_Info(tab_ಸಾಲು, nr, &ಪುಟ_info) != Xm_ಪುಟ_INVALID
	    && ಪುಟ_info.ಪುಟ_ಸಂಖ್ಯೆ == nr
	    && ಪುಟ_info.major_tab_widget != (Widget)0)
	{
	    Xt_Va_Get_ಮೌಲ್ಯಗಳು(ಪುಟ_info.major_tab_widget, Xm_Nuser_ದತ್ತಾಂಶ, &beval, ಖಾಲಿ);
	    ಆದರೆ (beval != ಖಾಲಿ)
		gui_mch_destroy_beval_area(beval);
	    Xt_Unmanage_ಮಗು(ಪುಟ_info.major_tab_widget);
	    XtDestroyWidget(ಪುಟ_info.major_tab_widget);
	}
	nr++;
    }

    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Nlast_ಪುಟ_ಸಂಖ್ಯೆ, tab_ಎಣಿಕೆ, ಖಾಲಿ);

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_ಪುಟ_ಸಂಖ್ಯೆ, &currentpage, ಖಾಲಿ);
    ಆದರೆ (currentpage != curtabidx)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_ಪುಟ_ಸಂಖ್ಯೆ, curtabidx, ಖಾಲಿ);
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_curtab(ಪೂರ್ಣಾಂಕ nr)
{
    ಪೂರ್ಣಾಂಕ		currentpage;

    ಆದರೆ (tab_ಸಾಲು == (Widget)0)
	ಹಿಂತಿರುಗಿ;

    Xt_Va_Get_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_ಪುಟ_ಸಂಖ್ಯೆ, &currentpage, ಖಾಲಿ);
    ಆದರೆ (currentpage != nr)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(tab_ಸಾಲು, Xm_Ncurrent_ಪುಟ_ಸಂಖ್ಯೆ, nr, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the colors of Widget "id" to the menu colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_menu_colors(Widget id)
{
    ಆದರೆ (gui.menu_bg_pixel != INVALCOLOR)
#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	XmChangeColor(id, gui.menu_bg_pixel);
#ಇಲ್ಲದಿದ್ದರೆ
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNbackground, gui.menu_bg_pixel, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.menu_fg_pixel != INVALCOLOR)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNforeground, gui.menu_fg_pixel, ಖಾಲಿ);
}

/*
 * Set the colors of Widget "id" to the scrollbar colors.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_motif_scroll_colors(Widget id)
{
    ಆದರೆ (gui.scroll_bg_pixel != INVALCOLOR)
#ಆದರೆ (Xm_ಆವೃತ್ತಿ >= 1002)
	XmChangeColor(id, gui.scroll_bg_pixel);
#ಇಲ್ಲದಿದ್ದರೆ
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNbackground, gui.scroll_bg_pixel, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.scroll_fg_pixel != INVALCOLOR)
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNforeground, gui.scroll_fg_pixel, ಖಾಲಿ);
}

/*
 * Set the fontlist for Widget "id" to use gui.menu_fontset or gui.menu_font.
 */
    ಶೂನ್ಯ
gui_motif_menu_fontlist(Widget id ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
# ifdef FONTSET_ALWAYS
    ಆದರೆ (gui.menu_fontset != NOFONTSET)
    {
	XmFontList fl;

	fl = gui_motif_fontset2fontlist((XFont_ಗಣ *)&gui.menu_fontset);
	ಆದರೆ (fl != ಖಾಲಿ)
	{
	    ಆದರೆ (XtIsManaged(id))
	    {
		Xt_Unmanage_ಮಗು(id);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ಮಗು(id);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
	    XmFontListFree(fl);
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (gui.menu_font != NOFONT)
    {
	XmFontList fl;

	fl = gui_motif_create_fontlist((XFontStruct *)gui.menu_font);
	ಆದರೆ (fl != ಖಾಲಿ)
	{
	    ಆದರೆ (XtIsManaged(id))
	    {
		Xt_Unmanage_ಮಗು(id);
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
		// We should force the widget to recalculate its
		// geometry now.
		Xt_Manage_ಮಗು(id);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(id, XmNfontList, fl, ಖಾಲಿ);
	    XmFontListFree(fl);
	}
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ
}


/*
 * We don't create it twice for the sake of speed.
 */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Shared_ಕಂಡುಹಿಡಿ_Replace
{
    Widget dialog;	// the main dialog widget
    Widget wword;	// 'Exact match' check button
    Widget mcase;	// 'match case' check button
    Widget ಮೇಲೆ;		// search direction 'Up' radio button
    Widget ಕೆಳಕ್ಕೆ;	// search direction 'Down' radio button
    Widget what;	// 'Find what' entry text widget
    Widget with;	// 'Replace with' entry text widget
    Widget ಕಂಡುಹಿಡಿ;	// 'Find Next' action button
    Widget replace;	// 'Replace With' action button
    Widget all;		// 'Replace All' action button
    Widget undo;	// 'Undo' action button

    Widget cancel;
} Shared_ಕಂಡುಹಿಡಿ_Replace_2;

ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 ಕಂಡುಹಿಡಿ_widgets = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};
ಸ್ಥಿರ Shared_ಕಂಡುಹಿಡಿ_Replace_2 repl_widgets = {ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ};

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_destroy_callback(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    Shared_ಕಂಡುಹಿಡಿ_Replace_2 *cd = (Shared_ಕಂಡುಹಿಡಿ_Replace_2 *)client_ದತ್ತಾಂಶ_2;

    ಆದರೆ (cd != ಖಾಲಿ)
       // suppress_dialog_mnemonics(cd->dialog);
	cd->dialog = (Widget)0;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_dismiss_callback(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    Shared_ಕಂಡುಹಿಡಿ_Replace_2 *cd = (Shared_ಕಂಡುಹಿಡಿ_Replace_2 *)client_ದತ್ತಾಂಶ_2;

    ಆದರೆ (cd != ಖಾಲಿ)
	Xt_Unmanage_ಮಗು(cd->dialog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ನಮೂದು_activate_callback(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    XmProcessTraversal((Widget)client_ದತ್ತಾಂಶ_2, XmTRAVERSE_CURRENT);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_callback(
    Widget	w ಬಳಸದ,
    Xt_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2,
    Xt_ಸೂಚಕ	call_ದತ್ತಾಂಶ ಬಳಸದ)
{
    long_u	flags = (long_u)client_ದತ್ತಾಂಶ_2;
    ಅಕ್ಷರ	*ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, *repl_ಪಠ್ಯ;
    Boolean	direction_ಕೆಳಕ್ಕೆ = TRUE;
    Boolean	wword;
    Boolean	mcase;
    Shared_ಕಂಡುಹಿಡಿ_Replace_2 *sfr;

    ಆದರೆ (flags == FRD_UNDO)
    {
	char_u	*save_cpo = p_cpo;

	// No need to be Vi compatible here.
	p_cpo = ಖಾಲಿಗಳು_option;
	u_undo(1);
	p_cpo = save_cpo;
	gui_update_screen();
	ಹಿಂತಿರುಗಿ;
    }

    // Get the search/replace strings from the dialog
    ಆದರೆ (flags == FRD_FINDNEXT)
    {
	repl_ಪಠ್ಯ = ಖಾಲಿ;
	sfr = &ಕಂಡುಹಿಡಿ_widgets;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	repl_ಪಠ್ಯ = Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_String(repl_widgets.with);
	sfr = &repl_widgets;
    }
    ಕಂಡುಹಿಡಿ_ಪಠ್ಯ = Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Get_String(sfr->what);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->ಕೆಳಕ್ಕೆ, XmNset, &direction_ಕೆಳಕ್ಕೆ, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->wword, XmNset, &wword, ಖಾಲಿ);
    Xt_Va_Get_ಮೌಲ್ಯಗಳು(sfr->mcase, XmNset, &mcase, ಖಾಲಿ);
    ಆದರೆ (wword)
	flags |= FRD_ಸಂಪೂರ್ಣ_ಶಬ್ದ;
    ಆದರೆ (mcase)
	flags |= FRD_ಹೊಂದಿಕೆ_CASE;

    (ಶೂನ್ಯ)gui_do_findrepl((ಪೂರ್ಣಾಂಕ)flags, (char_u *)ಕಂಡುಹಿಡಿ_ಪಠ್ಯ, (char_u *)repl_ಪಠ್ಯ,
							      direction_ಕೆಳಕ್ಕೆ);

    ಆದರೆ (ಕಂಡುಹಿಡಿ_ಪಠ್ಯ != ಖಾಲಿ)
	XtFree(ಕಂಡುಹಿಡಿ_ಪಠ್ಯ);
    ಆದರೆ (repl_ಪಠ್ಯ != ಖಾಲಿ)
	XtFree(repl_ಪಠ್ಯ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_keypress(
    Widget		w ಬಳಸದ,
    Shared_ಕಂಡುಹಿಡಿ_Replace_2	*frdp,
    XKeyEvent		*event,
    Boolean		*b ಬಳಸದ)
{
    ಕೀಲಿ_Sym keysym;

    ಆದರೆ (frdp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    keysym = XLookupKeysym(event, 0);

    // the scape key pops the whole dialog down
    ಆದರೆ (keysym == XK_Escape)
	Xt_Unmanage_ಮಗು(frdp->dialog);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_label(Widget w, ಅಕ್ಷರ *label)
{
    XmString	str;
    char_u	*p, *ಮುಂದಿನ;
    ಕೀಲಿ_Sym	mnemonic = NUL;

    ಆದರೆ (!w)
	ಹಿಂತಿರುಗಿ;

    p = vim_strsave((char_u *)label);
    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾಗಿ (ಮುಂದಿನ = p; *ಮುಂದಿನ; ++ಮುಂದಿನ)
    {
	ಆದರೆ (*ಮುಂದಿನ == DLG_HOTKEY_CHAR)
	{
	    ಪೂರ್ಣಾಂಕ len = STRLEN(ಮುಂದಿನ);

	    ಆದರೆ (len > 0)
	    {
		mch_memmove(ಮುಂದಿನ, ಮುಂದಿನ + 1, len);
		mnemonic = ಮುಂದಿನ[0];
	    }
	}
    }

    str = XmStringCreateSimple((ಅಕ್ಷರ *)p);
    vim_free(p);
    ಆದರೆ (str)
    {
	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(w,
		XmNlabelString, str,
		XmNmnemonic, mnemonic,
		ಖಾಲಿ);
	XmStringFree(str);
    }
    gui_motif_menu_fontlist(w);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕಂಡುಹಿಡಿ_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಂಕ do_replace)
{
    Shared_ಕಂಡುಹಿಡಿ_Replace_2	*frdp;
    Widget		separator;
    Widget		ಒಳಹರಿವು_form;
    Widget		button_form;
    Widget		toggle_form;
    Widget		ಚೌಕಟ್ಟು;
    XmString		str;
    ಪೂರ್ಣಾಂಕ			n;
    Arg			ವಾದಗಳು[6];
    ಪೂರ್ಣಾಂಕ			wword = FALSE;
    ಪೂರ್ಣಾಂಕ			mcase = !p_ic;
    Dimension		ಅಗಲ;
    Dimension		widest;
    char_u		*ನಮೂದು_ಪಠ್ಯ;

    frdp = do_replace ? &repl_widgets : &ಕಂಡುಹಿಡಿ_widgets;

    // Get the search string to use.
    ನಮೂದು_ಪಠ್ಯ = get_ಕಂಡುಹಿಡಿ_dialog_ಪಠ್ಯ(arg, &wword, &mcase);

    // If the dialog already exists, just raise it.
    ಆದರೆ (frdp->dialog)
    {
	gui_motif_synch_fonts();

	// If the window is already up, just pop it to the top
	ಆದರೆ (XtIsManaged(frdp->dialog))
	    XMapRaised(XtDisplay(frdp->dialog),
					    Xt_ಕಿಟಕಿ(Xt_ಪೋಷಕ(frdp->dialog)));
	ಇಲ್ಲದಿದ್ದರೆ
	    Xt_Manage_ಮಗು(frdp->dialog);
	XtPopup(Xt_ಪೋಷಕ(frdp->dialog), XtGrabNone);
	XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);

	ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	    Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_String(frdp->what, (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
	vim_free(ನಮೂದು_ಪಠ್ಯ);

	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(frdp->wword, XmNset, wword, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ;
    }

    // Create a fresh new dialog window
    ಆದರೆ (do_replace)
	 str = XmStringCreateSimple(_("VIM - Search and Replace..."));
    ಇಲ್ಲದಿದ್ದರೆ
	 str = XmStringCreateSimple(_("VIM - Search..."));

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNautoUnmanage, False); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNnoResize, True); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNdialogTitle, str); n++;

    frdp->dialog = XmCreateFormDialog(vimShell, "findReplaceDialog", ವಾದಗಳು, n);
    XmStringFree(str);
    Xt_ಕೂಡಿಸು_Callback(frdp->dialog, XmNdestroyCallback,
	    ಕಂಡುಹಿಡಿ_replace_destroy_callback, frdp);

    button_form = XtVaCreateWidget("buttonForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNrightAttachment, XmATTACH_FORM,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    ಖಾಲಿ);

    frdp->ಕಂಡುಹಿಡಿ = XtVaCreateManagedWidget("findButton",
	    xm_ತಳ್ಳು_Button_Widget_Class, button_form,
	    XmNsensitive, True,
	    XmNtopAttachment, XmATTACH_FORM,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    ಖಾಲಿ);
    ಗಣ_label(frdp->ಕಂಡುಹಿಡಿ, _("Find &Next"));

    Xt_ಕೂಡಿಸು_Callback(frdp->ಕಂಡುಹಿಡಿ, XmNactivateCallback,
	    ಕಂಡುಹಿಡಿ_replace_callback,
	    (do_replace ? (Xt_ಸೂಚಕ)FRD_R_FINDNEXT : (Xt_ಸೂಚಕ)FRD_FINDNEXT));

    ಆದರೆ (do_replace)
    {
	frdp->replace = XtVaCreateManagedWidget("replaceButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->ಕಂಡುಹಿಡಿ,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ಖಾಲಿ);
	ಗಣ_label(frdp->replace, _("&Replace"));
	Xt_ಕೂಡಿಸು_Callback(frdp->replace, XmNactivateCallback,
		ಕಂಡುಹಿಡಿ_replace_callback, (Xt_ಸೂಚಕ)FRD_REPLACE);

	frdp->all = XtVaCreateManagedWidget("replaceAllButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->replace,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ಖಾಲಿ);
	ಗಣ_label(frdp->all, _("Replace &All"));
	Xt_ಕೂಡಿಸು_Callback(frdp->all, XmNactivateCallback,
		ಕಂಡುಹಿಡಿ_replace_callback, (Xt_ಸೂಚಕ)FRD_REPLACEALL);

	frdp->undo = XtVaCreateManagedWidget("undoButton",
		xm_ತಳ್ಳು_Button_Widget_Class, button_form,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, frdp->all,
		XmNleftAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		ಖಾಲಿ);
	ಗಣ_label(frdp->undo, _("&Undo"));
	Xt_ಕೂಡಿಸು_Callback(frdp->undo, XmNactivateCallback,
		ಕಂಡುಹಿಡಿ_replace_callback, (Xt_ಸೂಚಕ)FRD_UNDO);
    }

    frdp->cancel = XtVaCreateManagedWidget("closeButton",
	    xm_ತಳ್ಳು_Button_Widget_Class, button_form,
	    XmNleftAttachment, XmATTACH_FORM,
	    XmNrightAttachment, XmATTACH_FORM,
	    XmNbottomAttachment, XmATTACH_FORM,
	    ಖಾಲಿ);
    ಗಣ_label(frdp->cancel, _("&Cancel"));
    Xt_ಕೂಡಿಸು_Callback(frdp->cancel, XmNactivateCallback,
	    ಕಂಡುಹಿಡಿ_replace_dismiss_callback, frdp);
    gui_motif_menu_fontlist(frdp->cancel);

    Xt_Manage_ಮಗು(button_form);

    n = 0;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNorientation, XmVERTICAL); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightAttachment, XmATTACH_WIDGET); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNrightWidget, button_form); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], Xm_Nright_ಸ್ಥಾನಾಂತರ, 4); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNtopAttachment, XmATTACH_FORM); n++;
    Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNbottomAttachment, XmATTACH_FORM); n++;
    separator = XmCreateSeparatorGadget(frdp->dialog, "separator", ವಾದಗಳು, n);
    Xt_Manage_ಮಗು(separator);

    ಒಳಹರಿವು_form = XtVaCreateWidget("inputForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, separator,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    ಖಾಲಿ);

    {
	Widget label_what;
	Widget label_with = (Widget)0;

	str = XmStringCreateSimple(_("Find what:"));
	label_what = XtVaCreateManagedWidget("whatLabel",
		xmLabelGadgetClass, ಒಳಹರಿವು_form,
		XmNlabelString, str,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNtopAttachment, XmATTACH_FORM,
		Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
		ಖಾಲಿ);
	XmStringFree(str);
	gui_motif_menu_fontlist(label_what);

	frdp->what = XtVaCreateManagedWidget("whatText",
		xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Widget_Class, ಒಳಹರಿವು_form,
		XmNtopAttachment, XmATTACH_FORM,
		XmNrightAttachment, XmATTACH_FORM,
		XmNleftAttachment, XmATTACH_FORM,
		ಖಾಲಿ);

	ಆದರೆ (do_replace)
	{
	    frdp->with = XtVaCreateManagedWidget("withText",
		    xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_Widget_Class,	ಒಳಹರಿವು_form,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNrightAttachment, XmATTACH_FORM,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ಖಾಲಿ);

	    Xt_ಕೂಡಿಸು_Callback(frdp->with, XmNactivateCallback,
		    ಕಂಡುಹಿಡಿ_replace_callback, (Xt_ಸೂಚಕ) FRD_R_FINDNEXT);

	    str = XmStringCreateSimple(_("Replace with:"));
	    label_with = XtVaCreateManagedWidget("withLabel",
		    xmLabelGadgetClass, ಒಳಹರಿವು_form,
		    XmNlabelString, str,
		    XmNleftAttachment, XmATTACH_FORM,
		    XmNtopAttachment, XmATTACH_WIDGET,
		    XmNtopWidget, frdp->what,
		    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
		    XmNbottomAttachment, XmATTACH_FORM,
		    ಖಾಲಿ);
	    XmStringFree(str);
	    gui_motif_menu_fontlist(label_with);

	    /*
	     * Make the entry activation only change the input focus onto the
	     * with item.
	     */
	    Xt_ಕೂಡಿಸು_Callback(frdp->what, XmNactivateCallback,
		    ನಮೂದು_activate_callback, frdp->with);
	    Xt_ಕೂಡಿಸು_Event_Handler(frdp->with, ಕೀಲಿ_Press_ಮುಖವಾಡ, False,
			    (XtEventHandler)ಕಂಡುಹಿಡಿ_replace_keypress,
			    (Xt_ಸೂಚಕ) frdp);

	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Make the entry activation do the search.
	     */
	    Xt_ಕೂಡಿಸು_Callback(frdp->what, XmNactivateCallback,
		    ಕಂಡುಹಿಡಿ_replace_callback, (Xt_ಸೂಚಕ)FRD_FINDNEXT);
	}
	Xt_ಕೂಡಿಸು_Event_Handler(frdp->what, ಕೀಲಿ_Press_ಮುಖವಾಡ, False,
			    (XtEventHandler)ಕಂಡುಹಿಡಿ_replace_keypress,
			    (Xt_ಸೂಚಕ)frdp);

	// Get the maximum width between the label widgets and line them up.
	n = 0;
	Xt_ಗಣ_Arg(ವಾದಗಳು[n], XmNwidth, &ಅಗಲ); n++;
	Xt_Get_ಮೌಲ್ಯಗಳು(label_what, ವಾದಗಳು, n);
	widest = ಅಗಲ;
	ಆದರೆ (do_replace)
	{
	    Xt_Get_ಮೌಲ್ಯಗಳು(label_with, ವಾದಗಳು, n);
	    ಆದರೆ (ಅಗಲ > widest)
		widest = ಅಗಲ;
	}

	Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(frdp->what, Xm_Nleft_ಸ್ಥಾನಾಂತರ, widest, ಖಾಲಿ);
	ಆದರೆ (do_replace)
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(frdp->with, Xm_Nleft_ಸ್ಥಾನಾಂತರ, widest, ಖಾಲಿ);

    }

    Xt_Manage_ಮಗು(ಒಳಹರಿವು_form);

    {
	Widget radio_box;
	Widget w;

	ಚೌಕಟ್ಟು = XtVaCreateWidget("directionFrame",
		xm_ಚೌಕಟ್ಟು_Widget_Class, frdp->dialog,
		XmNtopAttachment, XmATTACH_WIDGET,
		XmNtopWidget, ಒಳಹರಿವು_form,
		Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
		XmNbottomAttachment, XmATTACH_FORM,
		Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
		XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET,
		XmNrightWidget, ಒಳಹರಿವು_form,
		ಖಾಲಿ);

	str = XmStringCreateSimple(_("Direction"));
	w = XtVaCreateManagedWidget("directionFrameLabel",
		xmLabelGadgetClass, ಚೌಕಟ್ಟು,
		XmNlabelString, str,
		XmNchildHorizontalAlignment, XmALIGNMENT_BEGINNING,
		XmNchildType, Xm_ಚೌಕಟ್ಟು_TITLE_ಮಗು,
		ಖಾಲಿ);
	XmStringFree(str);
	gui_motif_menu_fontlist(w);

	radio_box = XmCreateRadioBox(ಚೌಕಟ್ಟು, "radioBox",
		(ArgList)ಖಾಲಿ, 0);

	str = XmStringCreateSimple( _("Up"));
	frdp->ಮೇಲೆ = XtVaCreateManagedWidget("upRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, False,
		ಖಾಲಿ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->ಮೇಲೆ);

	str = XmStringCreateSimple(_("Down"));
	frdp->ಕೆಳಕ್ಕೆ = XtVaCreateManagedWidget("downRadioButton",
		xmToggleButtonGadgetClass, radio_box,
		XmNlabelString, str,
		XmNset, True,
		ಖಾಲಿ);
	XmStringFree(str);
	gui_motif_menu_fontlist(frdp->ಕೆಳಕ್ಕೆ);

	Xt_Manage_ಮಗು(radio_box);
	Xt_Manage_ಮಗು(ಚೌಕಟ್ಟು);
    }

    toggle_form = XtVaCreateWidget("toggleForm",
	    xmFormWidgetClass,	frdp->dialog,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNrightAttachment, XmATTACH_WIDGET,
	    XmNrightWidget, ಚೌಕಟ್ಟು,
	    Xm_Nright_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, ಒಳಹರಿವು_form,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNbottomAttachment, XmATTACH_FORM,
	    Xm_Nbottom_ಸ್ಥಾನಾಂತರ, 4,
	    ಖಾಲಿ);

    str = XmStringCreateSimple(_("Match whole word only"));
    frdp->wword = XtVaCreateManagedWidget("wordToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNtopAttachment, XmATTACH_FORM,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNset, wword,
	    ಖಾಲಿ);
    XmStringFree(str);

    str = XmStringCreateSimple(_("Match case"));
    frdp->mcase = XtVaCreateManagedWidget("caseToggle",
	    xmToggleButtonGadgetClass, toggle_form,
	    XmNlabelString, str,
	    XmNleftAttachment, XmATTACH_FORM,
	    Xm_Nleft_ಸ್ಥಾನಾಂತರ, 4,
	    XmNtopAttachment, XmATTACH_WIDGET,
	    XmNtopWidget, frdp->wword,
	    Xm_Ntop_ಸ್ಥಾನಾಂತರ, 4,
	    XmNset, mcase,
	    ಖಾಲಿ);
    XmStringFree(str);
    gui_motif_menu_fontlist(frdp->wword);
    gui_motif_menu_fontlist(frdp->mcase);

    Xt_Manage_ಮಗು(toggle_form);

    ಆದರೆ (ನಮೂದು_ಪಠ್ಯ != ಖಾಲಿ)
	Xm_ಪಠ್ಯ_ಕ್ಷೇತ್ರ_ಗಣ_String(frdp->what, (ಅಕ್ಷರ *)ನಮೂದು_ಪಠ್ಯ);
    vim_free(ನಮೂದು_ಪಠ್ಯ);

    gui_motif_synch_fonts();

    manage_centered(frdp->dialog);
    activate_dialog_mnemonics(frdp->dialog);
    XmProcessTraversal(frdp->what, XmTRAVERSE_CURRENT);
}

   ಶೂನ್ಯ
gui_mch_ಕಂಡುಹಿಡಿ_dialog(exarg_T *eap)
{
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, FALSE);
}


    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಆದರೆ (!gui.ಒಳ_use)
	ಹಿಂತಿರುಗಿ;

    ಕಂಡುಹಿಡಿ_replace_dialog_create(eap->arg, TRUE);
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
    Shared_ಕಂಡುಹಿಡಿ_Replace_2 *frdp;
    ಪೂರ್ಣಾಂಕ		    do_replace;
    XFontStruct	    *font;
    XmFontList	    font_list;

    // FIXME: Unless we find out how to create a XmFontList from a XFontSet,
    // we just give up here on font synchronization.
    font = (XFontStruct *)gui.norm_font;
    ಆದರೆ (font == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    font_list = gui_motif_create_fontlist(font);

    // OK this loop is a bit tricky...
    ಗಾಗಿ (do_replace = 0; do_replace <= 1; ++do_replace)
    {
	frdp = (do_replace) ? (&repl_widgets) : (&ಕಂಡುಹಿಡಿ_widgets);
	ಆದರೆ (frdp->dialog)
	{
	    Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(frdp->what, XmNfontList, font_list, ಖಾಲಿ);
	    ಆದರೆ (do_replace)
		Xt_Va_ಗಣ_ಮೌಲ್ಯಗಳು(frdp->with, XmNfontList, font_list, ಖಾಲಿ);
	}
    }

    XmFontListFree(font_list);
}
